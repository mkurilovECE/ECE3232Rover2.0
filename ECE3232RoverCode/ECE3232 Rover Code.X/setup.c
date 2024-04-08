/*
 * File:   setup.c
 * Author: Declan
 *
 */


#include <xc.h>
#include "setup.h"

void setup(void) {
    //RC7 rising edge detection
    //TRISCbits.TRISC7 = 1;           //enable input on RC7  
    //ANSELCbits.ANSC7 = 0;           //disable analog pin
    //IOCCPbits.IOCCP7 = 1;           //IOCCFC7 bit and IOCIF flag set upon detecting a positive edge

    //TMR0
    T0CON1bits.T0CS = 0b010;        //F0sc/4 clock source, synchronised to Fosc/4, 1:32768 prescaler
    T0CON1bits.T0ASYNC = 0;
    T0CON1bits.T0CKPS = 0b1111;     // divide the clock to 32 Mhz/4/32768 = 244.14 cycles/sec
    TMR0H = 13;                     //TMR0 compare register = 13 to make the timer trigger every 53 ms
    TMR0L = 0;
    T0CON0bits.T016BIT = 0;
    T0CON0bits.T0EN = 1;

    //USART
    TX1STAbits.TXEN = 1;            // enable EUSART transmitter
    TX1STAbits.SYNC = 0;            // make the EUSART asynchronous
    RC1STAbits.SPEN = 1;            // enable EUSART and configure TX/CK I/O pin as output
    RC1STAbits.CREN = 1;

    TX1STAbits.BRGH = 1;
    BAUD1CONbits.BRG16 = 1;
    SP1BRGL = 68;                   // baud rate set to 115.2K to match PCLS board
    ANSELCbits.ANSC5 = 0;
    TRISCbits.TRISC5 = 0;
    RC5PPS = 0x10;                  // set TX pin to RC5 I/O pin;
    ANSELCbits.ANSC6 = 0;
    TRISCbits.TRISC6 = 1;
    RXPPS = 0x16;                  // set RX pin to RC6 I/O pin;

    //ADC
    TRISAbits.TRISA0 = 1;           //RA0 is input
    ANSELAbits.ANSA0 = 1;           //RA0 is analog

    ADCLKbits.ADCCS = 0b111111;     // set Fosc/128 ADC Clock Source

    ADREFbits.ADPREF = 0b00;        // set Vdd as positive voltage ref
    ADREFbits.ADNREF = 0;           // set AVss as positive voltage ref

    ADPCHbits.ADPCH = 0;            // set the positive ADC channel to RA0 pin

    ADCON0bits.ADON = 1;            // turn on ADC module

    ADACQ = 1;                      // set ADACQ to 1 clock of the selected ADC clock
    ADCON0bits.ADGO = 1;            // start ADC

    ADCON0bits.ADFRM0 = 1;          // make the data storage right-justified

    TRISAbits.TRISA1 = 0;           // set LED3-5 to outputs
    TRISAbits.TRISA2 = 0;
    TRISAbits.TRISA3 = 0;

    //Interrupts
    //PIR0bits.IOCIF = 0;             //clear interrupt on change interrupt flag
    //PIE0bits.IOCIE = 1;             //enable IOC interrupt on change

    PIR0bits.TMR0IF = 0;            //clear TMR0 interrupt flag
    PIE0bits.TMR0IE = 1;            //enable TMR0 rollover interrupt 

    PIE3bits.RCIE = 1;              //enable USART receive interrupt
    PIE3bits.TXIE = 1;              //enable USART transmit interrupt

    //PIR1bits.ADIF = 0;              // clear ADC Interrupt Flag
    //PIE1bits.ADIE = 1;              // enable ADC Interrupt

    INTCONbits.GIE = 1;             //enable global interrupts
    INTCONbits.PEIE = 1;            //enable peripheral interrupts

    return;
}

