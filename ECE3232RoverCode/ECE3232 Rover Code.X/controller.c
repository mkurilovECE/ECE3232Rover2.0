/*
 * File:   controller.c
 * Author: Declan
 *
 */

 /*
              (4)     (5)     (8)        (9)     (6)    (7)
               |       |       |          |       |      |
            ___|_______|_______|__________|_______|______|___
           /   |       |    .-""-.      .-""-.    |      |   \
           |   |       |   /      \    /      \   |      |   |
           |   |       |  |    /   |  |   \    |  |      |   |
           |  ___     ___  \  /   /    \   \  /  ___    ___  |
           | / O \   / O \  '-..-'      '-..-'  / O \  / O \ |
           | | | |   | | |                      | | |  | | | |
           | \___/   \___/        FS-16X        \___/  \___/ |
           |                                                 |
           |       .-----.                      .-----.      |
           |     .`   O   `.                  .`   O   `.    |
           |    /  ___|___  \                /  ___|___  \   |
      (3)-----  | |___|___| |                | |___|___| | -----(0)
           |    \           /                \           /   |
           |     '.       .'                  '.       .'    |
           |       '-----'                      '-----'      |
           |          |                            |         |
           |          |                            |         |
           \__________|____________________________|_________/
                      |                            |
                      |                            |
                     (2)                          (1)

   ------------------
   JOYSTICKS            
   ------------------
   (1):  1000-2000      
   (2):  1000-2000       
   (3):  1000-2000       
   (4):  1000-2000        
   ------------------ 
   SWITCHES              
   ------------------
   (5):  1000/2000      
   (6):  1000/2000      
   (7):  1000/1500/2000  
   (8):  1000/2000     
   ------------------
   POTENTIOMETERS       
   ------------------
   (9):  1000-2000     
   (10): 1000-2000   

  */


  /*
   Operating idea:
   *
   * - Pass controller channel data from main into controller function as 10 integer variables
   *      -Alternately, pass nothing in and have controller function be responsible
   *      for  fetching input data
   * - Based on controller channel data and operating parameters, assign a state number to each channel
   * - Concatenate state numbers into an integer/string and return to main program
   *
   * -Also need another function to convert controller states into actionable variable states
   *
   * - Write PWM/PPM send code on arduino/freenove
   * - Write single interrupt rising edge time detector on PIC, attempt to measure time between
   *   arduino rising edges accurately
   * - Write multiple rising edge interrupts to detect time between succesive rising edges
   *
   * -Wait in controller.c while(timer start = 0) loop for controller input
   * -On rising edge detected, trigger interrupt and go to ISR
   * -Rising edge detected, interrupt entered, cleared, check timer start (0), begin timer, enable timer interrupt, timer start = 1, enter while(1) loop to wait
   * -Timer rollover triggers interrupt, enter interrupt and clear timer rollover interrupt. Increment rollover counter for element X in array and wait in while(1)
   * -Timer rollover interrupts continue triggering, incrementing the rollover counter for element X.
   * -2nd rising edge detected, check timer start (1) and check if (X != Xarray.length - 1), then increment rollover counter element increment (X = X+1) , else disable timer interrupt and disable timer, reset timer to 0. Exit ISR and return to main program
   * -Should have full array Xarray of rollover counts.
   */
#include <xc.h>


void controller_rising_edge_interrupt(int* timer_status, int* channel, int* PPM_complete, int* PPM_channels) {

    if (*timer_status == 1) {
        if (*channel != *PPM_channels) {
            *channel += 1;
        }
        else {
            *PPM_complete = 1;
        }
    }
    else if (*timer_status == 0) {
        T0CON0 = 0b10000000;
        *timer_status = 1;
    }
    return;
}

void controller_main(int* PPM_complete, int* channel, int* timer_status) {

    while (*PPM_complete == 0) {

    }

    *PPM_complete = 0;
    *channel = 1;
    *timer_status = 0;
    T0CON0 = 0b00000000;            //disable TMR0
    TMR0H = 80;
    TMR0L = 0;
    return;

}

int controller_normalize(int PPM_rollover) {
    if (PPM_rollover > 200) {
        PPM_rollover = 200;
    }
    else if (PPM_rollover < 100) {
        PPM_rollover = 100;
    }
    return (PPM_rollover - 100);
}