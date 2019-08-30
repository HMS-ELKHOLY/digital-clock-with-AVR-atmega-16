# digital-clock-with-AVR-atmega-16

Author :Hussien Mostafa EL-kholy

 - tools used :

   - eclipse
   - proteus
 
- brief description :
  - I used selective 7-segment and 7447 decoder with atmega16 for displaying time in form of(hours : minutes : seconds) .
  - time is set to zero when pressing the button on PD2:INT0 (on falling edge ) and counting resumed when pressing  again .
  - time stopped  when pressing switch on PD3:INT1 (on falling edge ) and counting resumed when the button  again .
  - I used timer1 in CTC mode with pre-scaler 64 to generate interrupt every 1sec (OCR1A=15625) to calculate time.
  
  ![example](/images/Untitled.png)
