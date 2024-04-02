
/*
 * File:   setup.c
 * Author: Declan
 *
 */


#include <xc.h>
#include "setup.h"

void setup(void) {
    //RC7 rising edge detection
    TRISCbits.TRISC7 = 1;           //enable input on RC7  
    ANSELCbits.ANSC7 = 0;           //disable analog pin
    IOCCPbits.IOCCP7 = 1;           //IOCCFC7 bit and IOCIF flag set upon detecting a positive edge

    //TMR0
    T0CON1bits.T0CS = 0b010;        //F0sc/4 clock source, synchronised to Fosc/4, 1:2 prescaler
    T0CON1bits.T0ASYNC = 0;
    T0CON1bits.T0CKPS = 0b0000;
    TMR0H = 80;                     //TMR0 compare register = 56, 200 cycles before overflow = 50us
    TMR0L = 0;

    //USART
    TX1STAbits.TXEN = 1;            // enable EUSART transmitter
    TX1STAbits.SYNC = 0;            // make the EUSART asynchronous
    RC1STAbits.SPEN = 1;            // enable EUSART and configure TX/CK I/O pin as output
    RC1STAbits.CREN = 1;

    TX1STAbits.BRGH = 1;
    BAUD1CONbits.BRG16 = 0;
    SP1BRGL = 16;                   // baud rate set to 115.2K to match PCLS board
    ANSELCbits.ANSC5 = 0;
    TRISCbits.TRISC5 = 0;
    RC5PPS = 0x10;                  // set TX pin to RC5 I/O pin;
    ANSELCbits.ANSC6 = 0;
    TRISCbits.TRISC6 = 1;
    RXPPS = 0x16;                  // set RX pin to RC6 I/O pin;


    //Interrupts
    PIR0bits.IOCIF = 0;             //clear interrupt on change interrupt flag
    PIE0bits.IOCIE = 1;             //enable IOC interrupt on change

    PIR0bits.TMR0IF = 0;            //clear TMR0 interrupt flag
    PIE0bits.TMR0IE = 1;            //enable TMR0 rollover interrupt 

    PIE3bits.RCIE = 1;              //enable USART receive interrupt
    PIE3bits.TXIE = 1;              //enable USART transmit interrupt

    INTCONbits.GIE = 1;             //enable global interrupts
    INTCONbits.PEIE = 1;            //enable peripheral interrupts

    return;
}
