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

#define _XTAL_FREQ 32000000

//PPM monitor variables
//int PPM_channels = PPM_CHANNEL_QUANTITY;
//int controller_states[PPM_CHANNEL_QUANTITY_PLUS_ONE];
//int timer_status = 0;
//int channel = 1;
//int PPM_rollovers[PPM_CHANNEL_QUANTITY_PLUS_ONE] = { 0 };
//int PPM_complete = 0;

//uart and pcls variables
//char rx_data[100] = { 0 };
//char rx_data_pointer = 0;


volatile char pcls_info_response[12] = { 0 };
volatile char user_data_response[26] = { 0 };
volatile char pcls_pointer = 0;
volatile char user_pointer = 0;

typedef enum { MODE_A, MODE_B, MODE_C } switchC_mode_type;
typedef enum { MODE_Disable, MODE_Repair, MODE_Ore, MODE_Conductivity, MODE_FFT } laser_mode_type;

switchC_mode_type switchC_mode = MODE_A;        //set switch C to ore type by default
laser_mode_type laser_mode = MODE_Disable;      // set laser to assault by default

// joystick controls
int RightX = 0;
int RightY = 0;
int LeftX = 0;
int LeftY = 0;

// switch controls
int switch_A = 0;
int switch_B = 0;
int switch_C = 0;
int switch_D = 0;
int prev_switchC = 0;

// potentiometer controls
int potA = 0;
int potB = 0;

// parameters for calculating wheel controls
int Powervec = 0;
int Steeringvec = 0;
int left;
int right;
int dir;

// task parameters
char ore_type = 0;
char conductivity_zone_number = 0;
int fft_frequency = 0;
char fft_frequency_lsb = 0;
char fft_frequency_msb = 0;
int adc_data_bus = 0;

char repair_code_flag = 0;
char shield_code_flag = 0;


// Command transmission parameters
volatile char timer_flag = 0;
char counter = 0;
volatile char pclsStart = 0;
volatile char pclsEnd = 0;
volatile char userStart = 0;
volatile char userEnd = 0;
bool valid_response;

void __interrupt() ISR() {
    if (PIR3bits.RCIF == 1 && PIE3bits.RCIE == 1) {

        if (counter == 0 && pclsStart == 1)
        {
            pcls_info_response[pcls_pointer] = RC1REG;
            pcls_pointer++;
            if (pcls_pointer == 12)
            {
                pclsStart = 0;
                pclsEnd = 1;
                pcls_pointer = 0;
                counter = !counter;
            }
        }
        else if (counter == 1 && userStart == 1)
        {
            user_data_response[user_pointer] = RC1REG;
            user_pointer++;
            if (user_pointer == 26)
            {
                userStart = 0;
                userEnd = 1;
                user_pointer = 0;
                counter = !counter;
            }
        }

        if (RC1STAbits.OERR == 1 || RC1STAbits.FERR == 1)   // if overflow or framing error occur 
        {
            RC1STAbits.SPEN = 0;            // clear the error by resetting receiver
            RC1STAbits.SPEN = 1;
        }
    }

    if (PIR0bits.TMR0IF == 1) {
        PIR0bits.TMR0IF = 0;        //clear interrupt flag

        timer_flag = 1;
    }
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

        while (timer_flag == 0)
        {
        }
        if (pclsStart == 0 && counter == 0 && timer_flag == 1)
        {
            get_pcls_info();
            timer_flag = 0;
            //counter = !counter;
            pclsStart = 1;
            pclsEnd = 0;
        }

        if (pclsEnd == 1 && expected_pcls_info_response(pcls_info_response))
        {
            shield_code_flag = pcls_info_response[10];
            repair_code_flag = pcls_info_response[11];
            pclsEnd = 0;

        }
        
        if (userStart == 0 && counter == 1 && timer_flag == 1)
        {
            get_user_data();
            timer_flag = 0;
            //counter = !counter;
            userStart = 1;
            userEnd = 0;
        }

        if (userEnd == 1 && expected_user_info_response(user_data_response))
        {
            
            // 1. copy the old controls
            // 2. get the new controls
            // 3. call wheel control
            // 4. call laser control
            // 5. check switch A to turn water pump on/off
            // 6. check switch B to do FFT
            // 7. detect the switch C mode and process the input based on that
            // 8. detect the laser mode and shoot the appropriate laser

            prev_switchC = switch_C;

            RightX = user_data_response[7] << 8 | user_data_response[6];
            LeftX = user_data_response[13] << 8 | user_data_response[12];
            RightY = user_data_response[9] << 8 | user_data_response[8];
            LeftY = user_data_response[11] << 8 | user_data_response[10];

            switch_A = user_data_response[15] << 8 | user_data_response[14];
            switch_B = user_data_response[17] << 8 | user_data_response[16];
            switch_C = user_data_response[19] << 8 | user_data_response[18];
            switch_D = user_data_response[21] << 8 | user_data_response[20];

            potA = user_data_response[23] << 8 | user_data_response[22];  // 2. get new controls
            potB = user_data_response[25] << 8 | user_data_response[24];


            // 3. call wheel control
            if (RightY >= 1600)
            {
                Powervec = (((RightY - 1600) / 8) + 50);
                dir = 1;
            }
            else if (RightY <= 1400)
            {
                Powervec = ((((2000 - RightY) - 600) / 8) + 50);
                dir = 2;
            }
            else
            {
                Powervec = 0;
                dir = 0;
            }

            Steeringvec = (RightX - 1000) / 10;

            right = motorvectorleft(Powervec, Steeringvec);
            left = motorvectorright(Powervec, Steeringvec);
            if (left > right) {
                left = left + right;
                right = 2 * right;
            }
            else if (right > left) {
                right = left + right;
                left = 2 * left;
            }
            while (timer_flag != 1)
            {
            }
            set_motor_settings(dir, (char)right, dir, (char)left);    //min 60% duty cycle to make the rover move independetly
            timer_flag = 0;
            // 4. call laser gimble
            
            while (timer_flag != 1)
            {
            }
            set_servo_pulse((char)(RightX/10),(char)(LeftX/10),0,0);
            timer_flag = 0;

            // 5. check switch A for water pump

            if (switch_A < 1500)    // if switch is up
            {

            }


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
            
            while(timer_flag != 1)
            {
            }
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
            timer_flag = 0;
            userEnd = 0;
        }

    }

    return;
}