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
#include<stdio.h>
#include "controller.h"
#include "UART.h"
#include "setup.h"
#include "PCLS.h"
#include "MotorVector.h"
#include "stepper1.h"

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

int receive_finished_flag = 0;

bool valid_response;

typedef enum { MODE_A, MODE_B, MODE_C } switchC_mode_type;
typedef enum { MODE_Disable, MODE_Repair, MODE_Ore, MODE_Conductivity, MODE_FFT } laser_mode_type;

switchC_mode_type switchC_mode = MODE_A;        //set switch C to ore type by default
laser_mode_type laser_mode = MODE_Disable;      // set laser to assault by default

int potA = 0;
int potB = 0;

int switch_C = 0;
int switch_D = 0;
int prev_switchC = 0;

int prev_LeftX = 0;
int prev_LeftY = 0;
int prev_RightX = 0;
int prev_RightY = 0;

int LeftX = 0;
int LeftY = 0;
int RightX = 0;
int RightY = 0;
int Powervec = 0;
int Steeringvec = 0;
int left;
int right;
int dir;
char ore_type = 0;
char conductivity_zone_number = 0;
int fft_frequency = 0;
char fft_frequency_lsb = 0;
char fft_frequency_msb = 0;

char repair_code_flag = 0;
char repair_code = 0;

char shield_code_flag = 0;
void __interrupt() ISR() {
    if (PIR3bits.RCIF == 1 && PIE3bits.RCIE == 1) {

        if (RC1STAbits.OERR == 1 || RC1STAbits.FERR == 1)   // if overflow or framing error occur 
        {
            rx_data[rx_data_pointer] = RC1REG;
            rx_data_pointer++;
            rx_data[rx_data_pointer] = RC1REG;
            RC1STAbits.CREN = 0;            // clear the error by resetting receiver
            RC1STAbits.CREN = 1;
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
    //if (IOCCFbits.IOCCF7 == 1){
    //    IOCCFbits.IOCCF7 = 0;       //clear interrupt flag

    //    controller_rising_edge_interrupt(&timer_status, &channel, &PPM_complete, &PPM_channels);
    //}
    //if (PIR0bits.TMR0IF == 1) {
    //    PIR0bits.TMR0IF = 0;        //clear interrupt flag

    //    PPM_rollovers[channel]++;   //increment channel interrupt counter
    //    PPM_rollovers[0]++;         //increment total interrupt counter
    //}
    if (PIR3bits.TXIF == 1 && PIE3bits.TXIE == 1)
    {
        PIE3bits.TXIE = 0;          // stop transmission until the transmitter flag is up

    }
    //if (PIR1bits.ADIF == 1)
    //{
    //    adc_data_bus = 0;
    //    adc_data_bus = adc_data_bus | ADRESH;
    //    adc_data_bus = adc_data_bus << 8;
    //    adc_data_bus = adc_data_bus | ADRESL;

    //    PIR1bits.ADIF = 0;
    //    ADCON0bits.ADGO = 1;

        //if (adc_data_bus > 0x0230)      // if voltage at RA0 > 1.805V, turn all LEDs on
        //{
        //    LATAbits.LATA1 = 1;
        //    LATAbits.LATA2 = 1;
        //    LATAbits.LATA3 = 1;
        //}
        //else
        //{
        //    LATAbits.LATA1 = 0;     // if less, turn all LEDs off
        //    LATAbits.LATA2 = 0;
        //    LATAbits.LATA3 = 0;

       /* }*/
    //}

    return;
}


void main(void) {

    setup();

    while (1) {
        //controller_main(&PPM_complete, &channel, &timer_status);
        //for (int k = 0; k <= PPM_CHANNEL_QUANTITY_PLUS_ONE; k++){
        //    controller_states[k] = controller_normalize(PPM_rollovers[k]);
        //    PPM_rollovers[k] = 0;
        //}
        //set_laser_scope(0x01);      // turn laser on constantly
        //__delay_us(1000);

        get_pcls_info();
        __delay_us(2000);

        for (int i = 11; i >= 0; i--) {
            pcls_info_response[i] = rx_data[i];
            rx_data[i] = 0;
            rx_data_pointer--;
        }
        if (expected_pcls_info_response(pcls_info_response))
        {
            shield_code_flag = pcls_info_response[10];
            repair_code_flag = pcls_info_response[11];
        }

        get_user_data();
        __delay_us(3000);
        for (int p = 25; p >= 0; p--) {
            user_data_response[p] = rx_data[p];
            rx_data[p] = 0;
            rx_data_pointer--;
        }
        if (expected_user_info_response(user_data_response))
        {

            // 1. copy the old controls
            // 2. get the new controls
            // 3. call wheel control
            // 4. call laser control
            // 5. detect the switch C mode and process the input based on that
            // 6. detect the laser mode and shoot the appropriate laser

            prev_switchC = switch_C;
            prev_LeftX = LeftX;
            prev_LeftY = LeftY;
            prev_RightX = RightX;
            prev_RightY = RightY;

            potA = user_data_response[23] << 8 | user_data_response[22];  // 2. get new controls
            potB = user_data_response[25] << 8 | user_data_response[24];
            switch_C = user_data_response[19] << 8 | user_data_response[18];
            switch_D = user_data_response[21] << 8 | user_data_response[20];
            RightX = user_data_response[7] << 8 | user_data_response[6];
            LeftX = user_data_response[13] << 8 | user_data_response[12];
            RightY = user_data_response[9] << 8 | user_data_response[8];
            LeftY = user_data_response[11] << 8 | user_data_response[10];
            // 3. call wheel control

            Powervec = (LeftY - 1000) / 10;
            Steeringvec = (LeftX - 1000) / 10;
            left = (char)motorvectorleft(Powervec, Steeringvec);
            right = (char)motorvectorright(Powervec, Steeringvec);
            dir = (char)direction(Powervec);

            set_motor_settings(dir, left, dir, right);
            __delay_us(350);

            // 4. call laser gimble

            set_servo_pulse((char)(RightX / 10), (char)(LeftX / 10), 0, 0);
            __delay_us(350);

            //6. detect the switch C mode and process the input based on that
            if (potA < 1300)
            {
                switchC_mode = MODE_A;
            }
            else if (potA >= 1300 && potA < 1700)
            {
                switchC_mode = MODE_B;
            }
            else
            {
                switchC_mode = MODE_C;
            }

            switch (switchC_mode)
            {
            case MODE_A:
                // color processing code
                // decide on the color of the ore
                if (switch_C < 1300)
                {
                    ore_type = 1;   //solar (red)
                }
                else if (switch_C >= 1300 && switch_C < 1700) {

                    ore_type = 2;  // aurorium (yellow)
                }
                else {
                    ore_type = 3;   //cobalite (blue)
                }

                // send the command with the selected ore type
                break;

            case MODE_B:
                // conductivity test
                //decide on the number sent into the zone
                if (prev_switchC != switch_C) // change the number if the switch is in a different position
                {
                    if (switch_C < 1300)
                    {
                        conductivity_zone_number++;   //solar (red)
                    }
                    else if (switch_C > 1700) {
                        conductivity_zone_number--;
                    }
                    // if the switch is in the middle position, do not change the zone number
                }
                break;
            case MODE_C:
                // moving the ore paddles up and down
                if (prev_switchC != switch_C) // change the number if the switch is in a different position
                {
                    if (switch_C < 1300)
                    {
                        //paddle_stepper_function(1);
                    }
                    else if (switch_C > 1700)
                    {
                        //paddle_stepper_function(0);
                    }
                    // if the switch is in the middle position, do not change the anything
                }
                break;

            default:
                break;
            }
            // 7. detect the laser mode and shoot the appropriate laser
            // we still need to decide how we are receiving shield and repair codes
            if (potB < 1200)
            {
                laser_mode = MODE_Disable;
            }
            else if (potB >= 1200 && potB < 1400)
            {
                laser_mode = MODE_Repair;
            }
            else if (potB >= 1400 && potB < 1600)
            {
                laser_mode = MODE_Ore;
            }
            else if (potB >= 1600 && potB < 1800)
            {
                laser_mode = MODE_Conductivity;
            }
            else
            {
                laser_mode = MODE_FFT;
            }

            //transmit an appropriate laser based on the selected laser type 
            switch (laser_mode)
            {
            case MODE_Disable:
                //shoot an assault laser
                if (switch_D > 1500)
                {
                    shoot_laser(0x02);      // shoot a high caliber
                }
                break;
            case MODE_Repair:
                if (switch_D > 1500)
                {
                    repair_code_flag = pcls_info_response[11];
                    if (repair_code_flag != 0)
                    {
                        // get the repair code 
                        //transmit the repair code
                        transmit_repair_code();
                    }
                    else
                    {
                        request_repair_code();
                    }
                }
                break;
            case MODE_Ore:
                if (switch_D > 1500)
                {
                    processing_plant_ore_type(ore_type);
                }
                break;
            case MODE_Conductivity:
                if (switch_D > 1500)
                {
                    surface_exploration(0x04, 0x00, conductivity_zone_number, 0x00);
                }
                break;
            case MODE_FFT:
                // this should be the frequency we get from the FFT task
                fft_frequency_lsb = fft_frequency & 0XFF;
                fft_frequency_msb = (fft_frequency & 0XFF00) >> 8;
                if (switch_D > 1500)
                {
                    surface_exploration(0x02, 0x00, fft_frequency_lsb, fft_frequency_msb);
                }
                break;
            default:
                break;


            }
        }
        __delay_us(350);


    }

    return;
}








