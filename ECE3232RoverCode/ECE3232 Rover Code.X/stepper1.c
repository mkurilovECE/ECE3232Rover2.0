#include <xc.h> //CONFIG
#define _XTAL_FREQ 32000000 //32,000,000 HZ internal clock (32MHz)


//So this function takes in a direction (0 or 1) for left and right respectively
//This also takes in a number of rotations referred to as "rotations"
//This uses pins RB0,RB1,RB2 and RB3 to drive the stepper motor driver board pins
//In1, In2, In3 and In4 respectively. 
//To turn the stepper motor windings step through 8 states and that is 1/512 of a rotation. So it needs to 
//got through the 8 state process 512 times to make a full rotation.


void pump_stepper_function(void) {

    int state = 1;
    int direction = 0; //0 makes it turn left, 1 makes it turn right
    int rotations = 2; //number of full rotations
    int currstates = 0;
    int microrotations = 0.0;

    microrotations = (512 * rotations);
    if (currstates >= (microrotations)) {
        LATBbits.LATB0 = 0; //set pin 1 low
        LATBbits.LATB1 = 0; //set pin 2 low
        LATBbits.LATB2 = 0; //set pin 3 low
        LATBbits.LATB3 = 0; //set pin 4 low
    }
    TRISBbits.TRISB0 = 0; //configures pin 1 stepper output
    ANSELBbits.ANSB0 = 0;

    TRISBbits.TRISB1 = 0; //configures pin 2 stepper output
    ANSELBbits.ANSB1 = 0;

    TRISBbits.TRISB2 = 0; //configures pin 3 stepper output
    ANSELBbits.ANSB2 = 0;

    TRISBbits.TRISB3 = 0; //configures pin 4 stepper output
    ANSELBbits.ANSB3 = 0;


    while (currstates <= (microrotations)) {

        switch (state) {
        case 1:
            LATBbits.LATB0 = 1; //set pin 1 high
            LATBbits.LATB1 = 1; //set pin 2 high
            LATBbits.LATB2 = 1; //set pin 3 high
            LATBbits.LATB3 = 0; //set pin 4 low
            break;
        case 2:
            LATBbits.LATB0 = 1; //set pin 1 high
            LATBbits.LATB1 = 1; //set pin 2 high
            LATBbits.LATB2 = 0; //set pin 3 low
            LATBbits.LATB3 = 0; //set pin 4 low
            break;
        case 3:
            LATBbits.LATB0 = 1; //set pin 1 high
            LATBbits.LATB1 = 1; //set pin 2 high
            LATBbits.LATB2 = 0; //set pin 3 low
            LATBbits.LATB3 = 1; //set pin 4 high
            break;
        case 4:
            LATBbits.LATB0 = 1; //set pin 1 high
            LATBbits.LATB1 = 0; //set pin 2 low
            LATBbits.LATB2 = 0; //set pin 3 low
            LATBbits.LATB3 = 1; //set pin 4 high
            break;
        case 5:
            LATBbits.LATB0 = 1; //set pin 1 high
            LATBbits.LATB1 = 0; //set pin 2 low
            LATBbits.LATB2 = 1; //set pin 3 high
            LATBbits.LATB3 = 1; //set pin 4 high
            break;
        case 6:
            LATBbits.LATB0 = 0; //set pin 1 low
            LATBbits.LATB1 = 0; //set pin 2 low
            LATBbits.LATB2 = 1; //set pin 3 high
            LATBbits.LATB3 = 1; //set pin 4 high
            break;
        case 7:
            LATBbits.LATB0 = 0; //set pin 1 low
            LATBbits.LATB1 = 1; //set pin 2 high
            LATBbits.LATB2 = 1; //set pin 3 high
            LATBbits.LATB3 = 1; //set pin 4 high
            break;
        case 8:
            LATBbits.LATB0 = 0; //set pin 1 low
            LATBbits.LATB1 = 1; //set pin 2 high
            LATBbits.LATB2 = 1; //set pin 3 high
            LATBbits.LATB3 = 0; //set pin 4 low
            break;
        default:
            LATBbits.LATB0 = 0; //set pin 1 low
            LATBbits.LATB1 = 0; //set pin 2 low
            LATBbits.LATB2 = 0; //set pin 3 low
            LATBbits.LATB3 = 0; //set pin 4 low
            state = 1;
            break;
        }
        if (direction == 0) { //checks direction (0 for forward)
            if (state >= 8) {
                state = 1;
                currstates++;
            }
            else {
                state++;
            }
        }
        else {               //any other state is backwards
            if (state == 1) {
                state = 8;
                currstates++;
            }
            else {
                state--;
            }
        }
        if (currstates >= (microrotations)) {
            LATBbits.LATB0 = 0; //set pin 1 low
            LATBbits.LATB1 = 0; //set pin 2 low
            LATBbits.LATB2 = 0; //set pin 3 low
            LATBbits.LATB3 = 0; //set pin 4 low
            break;
        }

        __delay_ms(1); //state interval (speed) any more than 1 ms makes motor stall


    }
    __delay_ms(1000);
    return;
}
