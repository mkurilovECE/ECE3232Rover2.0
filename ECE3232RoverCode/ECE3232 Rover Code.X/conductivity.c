/*
 * File:   main_conductivity_2.c
 * Author: kuril
 *
 * Created on February 22, 2024, 5:16 PM
 */

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include "conductivity.h"

 /*
  *
  */



void conductivity_test(void) {

    TRISAbits.TRISA0 = 1;           //RA0 is input
    ANSELAbits.ANSA0 = 1;           //RA0 is analog

    ADCLKbits.ADCCS = 0b111111;     // set Fosc/128 ADC Clock Source

    ADREFbits.ADPREF = 0b00;        // set Vdd as positive voltage ref
    ADREFbits.ADNREF = 0;           // set AVss as positive voltage ref

    ADPCHbits.ADPCH = 0;        // set the positive ADC channel to RA0 pin

    ADCON0bits.ADON = 1;            // turn on ADC module

    PIR1bits.ADIF = 0;              // clear ADC Interrupt Flag
    PIE1bits.ADIE = 1;              // enable ADC Interrupt
    INTCONbits.GIE = 1;             // set Global Interrupt Enable
    INTCONbits.PEIE = 1;            // set Peripheral Interrupt Enable

    ADACQ = 1;                      // set ADACQ to 1 clock of the selected ADC clock
    ADCON0bits.ADGO = 1;            // start ADC

    ADCON0bits.ADFRM0 = 1;          // make the data storage right-justified

    TRISAbits.TRISA1 = 0;           // set LED3-5 to outputs
    TRISAbits.TRISA2 = 0;
    TRISAbits.TRISA3 = 0;

    return;

}