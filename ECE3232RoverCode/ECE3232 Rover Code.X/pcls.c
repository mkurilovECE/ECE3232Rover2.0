/*
 * File:   PCLS_commands.c
 * Author: Declan
 *
 */


#include <xc.h>
#include "UART.h"
#include "PCLS.h"
#include <stdbool.h>


 //TODO
 //
 //-add custom payload capability for custom laser command

void sync(void) {
    uart_send_byte(0xFE);
    uart_send_byte(0x19);
}

void msg_id(char lsb, char msb) {
    uart_send_byte(lsb);
    uart_send_byte(msb);
}

void payload_size(char lsb, char msb) {
    uart_send_byte(lsb);
    uart_send_byte(msb);
}

bool unknown_message(char* data_pointer)
{
    if (*data_pointer == 0xFE && *(data_pointer + 1) == 0x19)   // check if synchronized
    {
        if (*(data_pointer + 2) == 0 && *(data_pointer + 3) == 0x03) // invalid message ID message
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else return NULL;
}

bool expected_pcls_info_response(char* data_pointer)
{
    if (*data_pointer == 0xFE && *(data_pointer + 1) == 0x19)   // check if synchronized
    {
        if (*(data_pointer + 2) == 0x02 && *(data_pointer + 3) == 0x04) // PCLS Info Response message ID message
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else return NULL;
}

bool expected_user_info_response(char* data_pointer)
{
    if (*data_pointer == 0xFE && *(data_pointer + 1) == 0x19)   // check if synchronized
    {
        if (*(data_pointer + 2) == 0x02 && *(data_pointer + 3) == 0x05) // PCLS Info Response message ID message
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else return NULL;
}

void get_pcls_info(void) {

    sync();
    msg_id(0x01, 0x04);
    payload_size(0x00, 0x00);

    return;
}

void get_user_data(void) {

    sync();
    msg_id(0x01, 0x05);
    payload_size(0x00, 0x00);

    return;
}

void set_motor_settings(char motor_a_direction, char motor_a_PWM, char motor_b_direction, char motor_b_PWM) {
    sync();
    msg_id(0x01, 0x06);
    payload_size(0x04, 0x00);
    uart_send_byte(motor_a_direction);
    uart_send_byte(motor_a_PWM);
    uart_send_byte(motor_b_direction);
    uart_send_byte(motor_b_PWM);
    return;
}

void set_servo_pulse(char servo_1, char servo_2, char servo_3, char servo_4) {
    sync();
    msg_id(0x01, 0x07);
    payload_size(0x04, 0x00);
    uart_send_byte(servo_1);
    uart_send_byte(servo_2);
    uart_send_byte(servo_3);
    uart_send_byte(servo_4);
    return;
}

void set_laser_scope(char enable_flag) {
    sync();
    msg_id(0x01, 0x08);
    payload_size(0x01, 0x00);
    uart_send_byte(enable_flag);
    return;
}

void shoot_laser(char shot_type) {
    sync();
    msg_id(0x01, 0x09);
    payload_size(0x01, 0x00);
    uart_send_byte(shot_type);
    return;
}

void transit_shield_code() {
    sync();
    msg_id(0x02, 0x09);
    payload_size(0x00, 0x00);
    return;
}

void request_repair_code(void) {
    sync();
    msg_id(0x03, 0x09);
    payload_size(0x00, 0x00);
    return;
}

void transmit_repair_code() {
    sync();
    msg_id(0x04, 0x09);
    payload_size(0x00, 0x00);
    return;
}

void surface_exploration(char task_id_lsb, char task_id_msb, char task_value_lsb, char task_value_msb) {
    sync();
    msg_id(0x01, 0x0A);
    payload_size(0x04, 0x00);
    uart_send_byte(task_id_lsb);
    uart_send_byte(task_id_msb);
    uart_send_byte(task_value_lsb);
    uart_send_byte(task_value_msb);
    return;
}

void processing_plant_ore_type(char ore_type) {
    sync();
    msg_id(0x03, 0x0A);
    payload_size(0x01, 0x00);
    uart_send_byte(ore_type);
    return;
}

