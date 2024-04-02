/*
 * File:   uart.c
 * Author: Declan
 *
 */


#include <xc.h>
#include "UART.h"
#define _XTAL_FREQ 32000000

void set_up_uart(void)
{
    // Set up UART transmitter
    TX1STAbits.TXEN = 1;        // enable EUSART transmitter
    TX1STAbits.SYNC = 0;        // make the EUSART asynchronous
    RC1STAbits.SPEN = 1;        // enable EUSART and configure TX/CK I/O pin as output

    BAUD1CONbits.BRG16 = 1;     // configure the baud rate to Fosc/(4(n+1)) formula
    TX1STAbits.BRGH = 1;

    // set Baud rate to 115200 but writing 68 to BRG register

    SP1BRGL = 0b01000100;          // write 01000100 to the 8 LSBs

    //Use built in Tx interrupts from EUSART

    INTCONbits.GIE = 1;         // set Global Interrupt Enable
    INTCONbits.PEIE = 1;        // set Peripheral Interrupt Enable
    PIE3bits.TXIE = 1;          // disable Tx Interrupts until data needs to be transmitted

    ANSELCbits.ANSC5 = 0;
    RC5PPS = 0x10;

    // Set up UART Receiver

    TRISCbits.TRISC6 = 1;       // configure RC6 as input
    ANSELCbits.ANSC6 = 0;       // configure RC6 as digital

    RXPPS = 0x16;               // set RC6 as the RX pin

    RC1STAbits.CREN = 1;
    PIE3bits.RCIE = 1;          // enable Receiver Interrupt

    return;
}


void uart_send_byte(char data) {

    while (PIR3bits.TXIF != 1)
    {
        __delay_us(15);         // wait until the transmitter is ready to send
    }
    TX1REG = data;
    PIE3bits.TXIE = 1;          // enable transmitter interrupt

}