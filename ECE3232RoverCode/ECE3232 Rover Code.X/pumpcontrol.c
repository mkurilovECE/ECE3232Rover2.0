//Environmental Remediation Task Code
//(PWM motor control code)
//Author: Mattias Floyd 

#include <xc.h> //CONFIG
#include <stdbool.h>
#pragma config FEXTOSC = OFF    // External Oscillator mode selection bits (Oscillator not enabled)
#pragma config RSTOSC = HFINT32  // Power-up default value for COSC bits (HFINTOSC (1MHz))
#pragma config WDTE = OFF       // WDT operating mode (WDT Disabled, SWDTEN is ignored)
#define _XTAL_FREQ 1000000 //1,000,000 HZ internal clock (1MHz)
#define _OUTPIN_LAT LATAbits.LATA0 //uhhhhhh

int PWMVAL = 10; // for now ranges from 10-100 (will need to be fixed later)


//All this stuff below is garbage :) it uses interupts inside an external function from main
//had to do this because delay does not allow the use of a variable and
//I could not get the PWM library to work in time (revised version will need to use PWM library)
//void __interrupt() ISR(PWMVAL)
//{
//
//    if (PIR0bits.TMR0IF== 1) { //check the timer for inturrupt
//        PIR0bits.TMR0IF = 0; //clear the flag
//        _OUTPIN_LAT = 0;
//        LATCbits.LATC7 = 0;
//        if (PWMVAL == 10){
//            __delay_ms(0.21);
//            LATAbits.LATA0 = 1;
//            LATCbits.LATC7 = 1;
//        } else if(PWMVAL ==20){
//            __delay_ms(0.42);
//            LATAbits.LATA0 = 0;
//            LATCbits.LATC7 = 0; 
//        } else if(PWMVAL == 30){
//            __delay_ms(0.63);
//            LATAbits.LATA0 = 0;
//            LATCbits.LATC7 = 0; 
//        } else if(PWMVAL == 40){
//            __delay_ms(0.84);
//            LATAbits.LATA0 = 0;
//            LATCbits.LATC7 = 0; 
//        } else if(PWMVAL == 50){
//            __delay_ms(1.05);
//            LATAbits.LATA0 = 0;
//            LATCbits.LATC7 = 0; 
//        } else if(PWMVAL == 60){
//            __delay_ms(1.26);
//            LATAbits.LATA0 = 0;
//            LATCbits.LATC7 = 0; 
//        } else if(PWMVAL == 70){
//            __delay_ms(1.47);
//            LATAbits.LATA0 = 0;
//            LATCbits.LATC7 = 0; 
//        } else if(PWMVAL == 80){
//            __delay_ms(1.68);
//            LATAbits.LATA0 = 0;
//            LATCbits.LATC7 = 0; 
//        } else if(PWMVAL == 90){
//            __delay_ms(1.89);
//            LATAbits.LATA0 = 0;
//            LATCbits.LATC7 = 0; 
//        } else if(PWMVAL == 100){
//            __delay_ms(2.1);
//            LATAbits.LATA0 = 0;
//            LATCbits.LATC7 = 0; 
//        }
//    }
//return;
//}



void pumpcontol(bool state) { // 0 on 1 off
    
    //configure the direction of motor
    //temp values for forward
    //A1 is INT which is RC2
    //A2 which is RA1
    
    //Forward is A1 high A2 Low start with that
    
    
    TRISCbits.TRISC2 =!state; //Write A1 high
    ANSELCbits.ANSC2 =0;
    
    TRISAbits.TRISA1 =state; //Write A2 low
    ANSELAbits.ANSA1 =0;
    
    
    
    PIR0bits.TMR0IF = 0; //clear timer flag
    PIE0bits.TMR0IE =1; //enable timer interrupt
    INTCONbits.PEIE = 1; 
    INTCONbits.GIE = 1; //enables the global interrupt enable

    TRISAbits.TRISA0 =0; //configures the PWM output pin (LED MATCH)
    ANSELAbits.ANSA0 =0;
    
    TRISCbits.TRISC7 =0; //configures the PWM output pin
    ANSELCbits.ANSC7 =0;


 


    T0CON0 = 0b10000000;

    T0CON1bits.T0CS = 0b010; //sync clock source to F0sc
    T0CON1bits.T0ASYNC =0;
    T0CON1bits.T0CKPS=0b01; //10    
    
    
    TMR0L = 0; //Low value for the timer to start at
    TMR0H = 244; //Value to cause the interrupt at 
    
    
    T0CON0 = 0b10000000; //set the timer to 8-bit mode
   
    int x=0;
    while (1) {
      
        x=1;
        x=0;
        
    }
    return;
}
