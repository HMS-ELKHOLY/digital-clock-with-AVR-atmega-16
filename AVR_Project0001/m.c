/*
 * Digital clock shows seconds , minutes and hours with ability to reset or stop the clock
 *
 * Date:28/8/2019
 *
 * 08:00:00 am
 *
 * Author:Hussien Mostafa
 *
 * */
#include<avr/io.h>
#include <util/delay.h>
#include<avr/interrupt.h>

#define MAKE_CONTROL_PORT_ZERO CONTROL_PORT = (CONTROL_REG & ~0x3f)
#define SET_BIT_MACRO(REG,BIT) ((REG)= REG|(1<<BIT))
#define CLEAR_BIT_MACRO(REG,BIT) ((REG)= REG&~(1<<BIT))


#define CONTROL_PORT PORTB
#define CONTROL_REG DDRB

#define OUTPUT_REG DDRC
#define OUTPUT_PORT PORTC
/**frequency is 1 MHZ**/
/*
 * CONTROL PORT : controls which 7seg will show digits
 * CONTROL REG	: configures control port pins as output
 * OUTPUT PORT  : digits that will be send to any of the six 7seg appear on that port
 *
 * */
volatile unsigned char time_val[3] = { 0 };
/**
 * time_val[0]>>>>seconds
 *
 * time_val[1]>>>>minutes
 *
 * time_val[2]>>>>hours
 *
 *
 * */

volatile unsigned char reset_flag = 0;
volatile unsigned char stop_flag = 0;

void seven_seg_init() {
	/* control pins are from 0 to 5*/
	CONTROL_REG = (CONTROL_REG | 0x3f);
	CONTROL_PORT = (CONTROL_REG & ~0x3f);
}

void timer1_init_ctc(unsigned short top) {
	cli();/**stop interrupt first**/
	TCNT1 = 0;//count from zero
	OCR1A = top;//use top value
	TCCR1A = 0;
	SET_BIT_MACRO(TCCR1A, FOC1A);/*non -pwm*/
	SET_BIT_MACRO(TCCR1B, WGM12);/*compare mode*/
	/*64 pre-scaler */
	SET_BIT_MACRO(TCCR1B, CS11);
	SET_BIT_MACRO(TCCR1B, CS10);
	/*enable interrupt on compare match*/
	SET_BIT_MACRO(TIMSK, OCIE1A);/**enable interrupt at last**/
	sei();
}
ISR (TIMER1_COMPA_vect) {
	cli();
	DDRA=0xff;
	PORTA=OCR1A;
	if (reset_flag == 1)
		time_val[0] = time_val[1] = time_val[2] = 0;
	else if (stop_flag != 1)
		time_val[0]++;//increment seconds first
	if (time_val[0] == 60) {
		time_val[1]++;//increment minutes every 60 second and make seconds=0
		time_val[0] = 0;
	}
	if (time_val[1] == 60) {
		time_val[2]++;//increment hours every 60 second and make minutes=0
		time_val[1] = 0;
	}
/*overflow case ( when exceeds 24 hours )*/
	if (time_val[2] == 24) {
		time_val[0] = 0;
		time_val[1] = time_val[2] = 0;
	}

	sei();
}
void INT0_INIT_PULL_UP() {
	CLEAR_BIT_MACRO(DDRD, PD2);
	SET_BIT_MACRO(PORTD, PD2);
	cli();
	MCUCR |= (1 << ISC01);
	GICR |= (1 << INT0);
	sei();
}
void INT1_INIT_PULL_UP() {
	CLEAR_BIT_MACRO(DDRD, PD3);
	SET_BIT_MACRO(PORTD, PD3);
	cli();
	MCUCR |= (1 << ISC11);
	GICR |= (1 << INT1);
	sei();
}
ISR(INT0_vect) {
	reset_flag ^= 1;/*if pressed reset occurs and won't recount a until pressed again*/
}
ISR(INT1_vect) {
	stop_flag ^= 1;/*if pressed stopping counting occurs and won't resume a until pressed again*/
}

int main() {
	CONTROL_REG = 0x3f;
	CONTROL_PORT = 0x00;
	DDRD = 0xff;
	OUTPUT_REG = 0x3f;
	OUTPUT_PORT = 0;
	unsigned char counter = 0;
	INT0_INIT_PULL_UP();
	INT1_INIT_PULL_UP();
	timer1_init_ctc(15625);
	while (1) {
		for (counter = 0; counter <= 4; counter += 2) {
			/*each time will show the number of seconds then minutes then hours
			 * example :
			 * 1         5        :      5      0       :      1       7
			 * 									             <tens     ones>  ____>shown in the first of loop : counter=0
			 * 				          <tens     ones>  ____>shown in the second iteration of loop : counter=2
			 * <tens     ones>  ____>shown in the second iteration of loop : counter=4
			 * */
			OUTPUT_PORT = time_val[counter / 2] % 10; /*ones place*/
			SET_BIT_MACRO(CONTROL_PORT, counter);
			_delay_ms(1);
			MAKE_CONTROL_PORT_ZERO;
			OUTPUT_PORT = time_val[counter / 2] / 10; /*tens place*/
			SET_BIT_MACRO(CONTROL_PORT, (counter + 1));
			_delay_ms(1);
			MAKE_CONTROL_PORT_ZERO;
		}
		/**
		 * counter incremented by 2 to perform shift operation correctly
		 *
		 * example :
		 * counter 0: element 0 in array shown on (7seg 0 and  7seg(0+1) )
		 * counter 2: element 1 in array shown on (7seg 2 and 3 seg(2+1) )
		 * counter 0: element 2 in array shown on (7seg 4 and  7seg(4+1) )
		 * */

	}
}
