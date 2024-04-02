/* Microchip Technology Inc. and its subsidiaries.  You may use this software
 * and any derivatives exclusively with Microchip products.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
 * TERMS.
 */

 /*
  * File:
  * Author:
  * Comments:
  * Revision history:
  */

  // This is a guard condition so that contents of this file are not included
  // more than once.  
#ifndef PCLS_H
#define	PCLS_H

#include <xc.h> // include processor files - each processor file is guarded.
#include <stdbool.h>

void sync(void);


void msg_id(char lsb, char msb);
void payload_size(char lsb, char msb);
void get_pcls_info(void);
void get_user_data(void);
void set_motor_settings(char motor_a_direction, char motor_a_PWM, char motor_b_direction, char motor_b_PWM);
void set_servo_pulse(char servo_1, char servo_2, char servo_3, char servo_4);
void set_laser_scope(char enable_flag);
void shoot_laser(char shot_type);
void transit_shield_code(char shield_code);
void request_repair_code(void);
void transmit_repair_code(char repair_code);
void surface_exploration(char task_id_lsb, char task_id_msb, char task_value_lsb, char task_value_msb);
void processing_plant_ore_type(char ore_type);
bool unknown_message(char* data_pointer);
bool expected_pcls_info_response(char* data_pointer);
bool expected_user_info_response(char* data_pointer);

#endif 


#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef  __cplusplus
}
#endif /* __cplusplus */