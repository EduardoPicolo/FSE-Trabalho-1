#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <signal.h>
#include <sys/signal.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <softPwm.h>

#include "controller.h"
#include "uart.h"
#include "dry_run.h"
#include "gpio.h"
#include "pid.h"
#include "display.h"
#include "bme280_handler.h"
#include "reflow_handler.h"

CONTROLLER controller = {NULL, 0, 0, 0, 0, 0, {0.0, 0.0, 0.0}};

void set_mode(int mode)
{
    controller.ref_mode = mode;
}

void set_system_state(int state)
{
    controller.on_off_state = state;
}

void set_uart_filestream(int *uart_filestream)
{
    controller.uart_filestream = uart_filestream;
}

float average_temperature(float current_temperature)
{
    controller.temperature_history[0] = controller.temperature_history[1];
    controller.temperature_history[1] = controller.temperature_history[2];
    controller.temperature_history[2] = current_temperature;
    return (controller.temperature_history[0] + controller.temperature_history[1] + controller.temperature_history[2]) / 3.0;
}

void handle_command(int command)
{
    switch (command)
    {
    case CMD_TURN_ON:
        printf("Turn on system\n");
        controller.on_off_state = 1;
        send_status();
        break;
    case CMD_TURN_OFF:
        printf("Turn off system\n");
        controller.on_off_state = 0;
        deactivate_fan();
        deactivate_resistor();
        send_status();
        break;
    case CMD_REF_POTENCIOMETRO:
        printf("Reference: Potenciômetro\n");
        controller.ref_mode = 0;
        send_mode();
        break;
    case CMD_REF_CURVE:
        printf("Reference: Curva\n");
        loadReflowData();
        controller.ref_mode = 1;
        controller.count_ref = 0;
        send_mode();
        break;

    default:
        break;
    }
}

void control()
{
    // controller.uart_filestream = uart_filestream;
    unsigned char temperature_code[7] = {SERVER_CODE, CMD_CODE, GET_TEMPERATURE, MATRICULA};
    unsigned char potenciometro_code[7] = {SERVER_CODE, CMD_CODE, GET_POTENCIOMETRO, MATRICULA};

    unsigned char control_signal[11] = {SERVER_CODE, SEND_COMMAND_CODE, SEND_CONTROL_SIGNAL, MATRICULA};
    unsigned char temperature_reference[11] = {SERVER_CODE, SEND_COMMAND_CODE, SEND_REF_SIGNAL, MATRICULA};

    struct bme280_data bme_data;
    float internal_temperature;
    float potenciometro_value;
    float ref_curve_value;
    float control_output;
    int INT_control_output = 0;

    write_uart(controller.uart_filestream, temperature_code, 7);
    read_data(controller.uart_filestream, temperature_code, &internal_temperature, 4);

    bme_data = get_sensor_data();

    // Potenciometro
    if (controller.ref_mode == 0)
    {
        write_uart(controller.uart_filestream, potenciometro_code, 7);
        read_data(controller.uart_filestream, potenciometro_code, &potenciometro_value, 4);
        pid_atualiza_referencia(potenciometro_value);
        display_temperatures(internal_temperature, bme_data.temperature, potenciometro_value, controller.ref_mode);
    }
    // Curva reflow
    else if (controller.ref_mode == 1)
    {
        ref_curve_value = get_curve_value(&controller.count_ref);
        pid_atualiza_referencia(ref_curve_value);
        memcpy(&temperature_reference[7], &ref_curve_value, 4);
        write_uart(controller.uart_filestream, temperature_reference, 11);
        display_temperatures(internal_temperature, bme_data.temperature, ref_curve_value, controller.ref_mode);
    }
    // Terminal
    else if (controller.ref_mode == 2)
    {
        float temp;
        temp = pid_get_reference();
        memcpy(&temperature_reference[7], &temp, 4);
        write_uart(controller.uart_filestream, temperature_reference, 11);
        display_temperatures(internal_temperature, bme_data.temperature, temp, controller.ref_mode);
    }

    control_output = pid_controle(average_temperature(internal_temperature));

    if (control_output > 0)
    {
        activate_resistor((int)control_output);
        deactivate_fan();
    }
    else
    {
        if (control_output < -40.0)
        {
            activate_fan((int)(-1.0 * control_output));
        }
        else
        {
            activate_fan(40.0);
        }
        deactivate_resistor();
    }

    INT_control_output = control_output;

    memcpy(&control_signal[7], &INT_control_output, 4);
    write_uart(controller.uart_filestream, control_signal, 11);
}

void get_command()
{
    unsigned char command[7] = {SERVER_CODE, CMD_CODE, GET_COMMAND, MATRICULA};

    write_uart(controller.uart_filestream, command, 7);
    int response;
    read_data(controller.uart_filestream, command, &response, 4);
    if (response)
    {
        printf("Command received: %d\n", response);
        handle_command(response);
    }
}

void send_status()
{
    unsigned char send_status[11] = {SERVER_CODE, SEND_COMMAND_CODE, SEND_SYSTEM_STATUS, MATRICULA};
    char byte = (char)controller.on_off_state;
    memcpy(&send_status[7], &byte, 1);
    write_uart(controller.uart_filestream, send_status, 8);
    int response;
    read_data(controller.uart_filestream, send_status, &response, 4);
    printf("Current system status: %d\n", response);
}

void send_mode()
{
    unsigned char ref_mode[11] = {SERVER_CODE, SEND_COMMAND_CODE, SEND_REF_MODE, MATRICULA};
    char byte;
    byte = (char)controller.ref_mode;
    memcpy(&ref_mode[7], &byte, 1);
    write_uart(controller.uart_filestream, ref_mode, 8);
    int response;
    read_data(controller.uart_filestream, ref_mode, &response, 4);
    printf("Mode: %d\n", response);
}

void controller_routine()
{
    // controller.on_off_state = 1;
    send_status();

    while (1)
    {
        if (controller.on_off_state)
        {
            control();
        }
        get_command();
        sleep(1);
    }
}