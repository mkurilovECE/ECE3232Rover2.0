/*
 * File:   main.c
 * Author: Declan
 *
 * -Wait in controller.c while(timer start = 0) loop for controller input
 * -On rising edge detected, trigger interrupt and go to ISR
 * -Rising edge detected, interrupt entered, cleared, check timer start (0), begin timer, enable timer interrupt, timer start = 1, enter while(1) loop to wait
 * -Timer rollover triggers interrupt, enter interrupt and clear timer rollover interrupt. Increment rollover counter for element X in array and wait in while(1)
 * -Timer rollover interrupts continue triggering, incrementing the rollover counter for element X.
 * -2nd rising edge detected, check timer start (1) and check if (X != Xarray.length - 1), then increment rollover counter element increment (X = X+1) , else disable timer interrupt and disable timer, reset timer to 0. Exit ISR and return to main program
 * -Should have full array Xarray of rollover counts.
 *
 * Leave clock prescaler at 1:1,
 *
 */


#include <xc.h>
#include <stdbool.h>
#include "controller.h"
#include "UART.h"
#include "setup.h"
#include "PCLS.h"
#include "conductivity.h"

 // CONFIG1
#pragma config FEXTOSC = ECH    // External Oscillator mode selection bits (EC above 8MHz; PFM set to high power)
#pragma config RSTOSC = HFINT32 // Power-up default value for COSC bits (HFINTOSC with OSCFRQ= 32 MHz and CDIV = 1:1)
#pragma config CLKOUTEN = OFF   // Clock Out Enable bit (CLKOUT function is disabled; i/o or oscillator function on OSC2)
#pragma config CSWEN = ON       // Clock Switch Enable bit (Writing to NOSC and NDIV is allowed)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable bit (FSCM timer enabled)

// CONFIG2
#pragma config MCLRE = ON       // Master Clear Enable bit (MCLR pin is Master Clear function)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config LPBOREN = OFF    // Low-Power BOR enable bit (ULPBOR disabled)
#pragma config BOREN = ON       // Brown-out reset enable bits (Brown-out Reset Enabled, SBOREN bit is ignored)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (VBOR) set to 1.9V on LF, and 2.45V on F Devices)
#pragma config ZCD = OFF        // Zero-cross detect disable (Zero-cross detect circuit is disabled at POR.)
#pragma config PPS1WAY = ON     // Peripheral Pin Select one-way control (The PPSLOCK bit can be cleared and set only once in software)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable bit (Stack Overflow or Underflow will cause a reset)

// CONFIG3
#pragma config WDTCPS = WDTCPS_31// WDT Period Select bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = ON        // WDT operating mode (WDT enabled regardless of sleep; SWDTEN ignored)
#pragma config WDTCWS = WDTCWS_7// WDT Window Select bits (window always open (100%); software control; keyed access not required)
#pragma config WDTCCS = SC      // WDT input clock selector (Software Control)

// CONFIG4
#pragma config WRT = OFF        // UserNVM self-write protection bits (Write protection off)
#pragma config SCANE = available// Scanner Enable bit (Scanner module is available for use)
#pragma config LVP = ON         // Low Voltage Programming Enable bit (Low Voltage programming enabled. MCLR/Vpp pin function is MCLR.)

// CONFIG5
#pragma config CP = OFF         // UserNVM Program memory code protection bit (Program Memory code protection disabled)
#pragma config CPD = OFF        // DataNVM code protection bit (Data EEPROM code protection disabled)


#define PPM_CHANNEL_QUANTITY 10
#define PPM_CHANNEL_QUANTITY_PLUS_ONE 11
#define _XTAL_FREQ 32000000

    //PPM monitor variables
int PPM_channels = PPM_CHANNEL_QUANTITY;
int controller_states[PPM_CHANNEL_QUANTITY_PLUS_ONE];
int timer_status = 0;
int channel = 1;
int PPM_rollovers[PPM_CHANNEL_QUANTITY_PLUS_ONE] = { 0 };
int PPM_complete = 0;

//uart and pcls variables
char rx_data[100] = { 0 };
char rx_data_pointer = 0;
char pcls_info_response[12] = { 0 };
char user_data_response[26] = { 0 };

int adc_data_bus = 0;

bool valid_response;


void __interrupt() ISR() {
    if (PIR3bits.RCIF == 1 && PIE3bits.RCIE == 1) {

        if (RC1STAbits.OERR == 1 || RC1STAbits.FERR == 1)   // if overflow or framing error occur 
        {
            rx_data[rx_data_pointer] = RC1REG;
            rx_data_pointer++;
            rx_data[rx_data_pointer] = RC1REG;
            RC1STAbits.SPEN = 0;            // clear the error by resetting receiver
            RC1STAbits.SPEN = 1;
        }
        else
        {
            rx_data[rx_data_pointer] = RC1REG;
        }

        rx_data_pointer++;
        if (rx_data_pointer == 100)      // if the last character in the buffer was written
        {
            rx_data_pointer = 0;
        }
    }
    if (IOCCFbits.IOCCF7 == 1) {
        IOCCFbits.IOCCF7 = 0;       //clear interrupt flag

        controller_rising_edge_interrupt(&timer_status, &channel, &PPM_complete, &PPM_channels);
    }
    if (PIR0bits.TMR0IF == 1) {
        PIR0bits.TMR0IF = 0;        //clear interrupt flag

        PPM_rollovers[channel]++;   //increment channel interrupt counter
        PPM_rollovers[0]++;         //increment total interrupt counter
    }
    if (PIR3bits.TXIF == 1 && PIE3bits.TXIE == 1)
    {
        PIE3bits.TXIE = 0;          // stop transmission until the transmitter flag is up

    }
    if (PIR1bits.ADIF == 1)
    {
        adc_data_bus = 0;
        adc_data_bus = adc_data_bus | ADRESH;
        adc_data_bus = adc_data_bus << 8;
        adc_data_bus = adc_data_bus | adc_data_bus;

        PIR1bits.ADIF = 0;
        ADCON0bits.ADGO = 0;
    }

    return;
}


void main(void) {

    setup();

    while (1) {
        //controller_main(&PPM_complete, &channel, &timer_status);
        //for (int i = 0; i <= PPM_CHANNEL_QUANTITY_PLUS_ONE; i++){
        //    controller_states[i] = controller_normalize(PPM_rollovers[i]);
        //    PPM_rollovers[i] = 0;
        //}

        get_pcls_info();
        __delay_ms(3);

        for (int i = 11; i >= 0; i--) {
            pcls_info_response[i] = rx_data[i];
            rx_data[i] = 0;
            rx_data_pointer--;
        }
        if (expected_pcls_info_response(pcls_info_response))
        {
            shield_code_flag=pcls_info_response[10];
            repair_code_flag=pcls_info_response[11];
        }
        else if (unknown_message(pcls_info_response))
        {
            // UKNOWN COMMAND DETECTED
        }

        get_user_data();
        __delay_ms(3);
        for (int p = 25; p >= 0; p--) {
            user_data_response[p] = rx_data[p];
            rx_data[p] = 0;
            rx_data_pointer--;
        }
        if (expected_user_info_response(user_data_response))
            
        {
            int motor_pwm;
            int  motor_a_direction;  //both motors forward
            int  motor_b_direction;
            int right_y = user_data_response[9]<<8 | user_data_response[8];
                        
            if (right_y > 1500)
            {
                motor_pwm = 100.0*(right_y-1500.0)/500.0;
                motor_a_direction = 1;  //both motors forward
                motor_b_direction = 1;
            }
            else if (right_y < 1500)
            {
                motor_pwm = 100.0*(1500.0-right_y)/500.0;
                motor_a_direction = 2; //both motors backwards
                motor_b_direction = 2;
            }
            else
            {    
                motor_a_direction = 0; //break both motors
                motor_b_direction = 0;
            }

         int left_y = user_data_response[11]<<8 | user_data_response[10]; //pitch
                        
           int pitch=left_y/10; //convert 


       int left_x = user_data_response[13]<<8 | user_data_response[12]; //yaw
           int yaw=left_x/10; //convert to frequency for servo 
           
        
            

         
            
            set_motor_settings(motor_a_direction, (char)motor_pwm, motor_b_direction, (char)motor_pwm);
            set_servo_pulse(pitch, yaw, 0, 0);
        }
        else if (unknown_message(user_data_response))
        {
            // UKNOWN COMMAND DETECTED
        }

        conductivity_test();
        if (adc_data_bus > 0x0230)      // if voltage at RA0 > 1.805V, turn all LEDs on
        {
            LATAbits.LATA1 = 1;
            LATAbits.LATA2 = 1;
            LATAbits.LATA3 = 1;
        }
        else
        {
            LATAbits.LATA1 = 0;     // if less, turn all LEDs off
            LATAbits.LATA2 = 0;
            LATAbits.LATA3 = 0;
        }


    }


    return;
}
