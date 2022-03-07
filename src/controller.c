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

struct CONTROLLER controller = {NULL, 0, 0, 0, 0, 0, 0, {0.0, 0.0, 0.0}};

float average_temperature(float current_temperature)
{
    controller.temperature_history[0] = controller.temperature_history[1];
    controller.temperature_history[1] = controller.temperature_history[2];
    controller.temperature_history[2] = current_temperature;
    return (controller.temperature_history[0] + controller.temperature_history[1] + controller.temperature_history[2]) / 3.0;
}

float get_curve_value()
{
    int cooldown_time = 120;
    int curva[10][2] = {
        {0, 25},
        {60, 38},
        {120, 46},
        {240, 54},
        {260, 57},
        {300, 61},
        {360, 63},
        {420, 54},
        {480, 33},
        {600, 25}};

    controller.count_ref++;

    if (controller.count_ref <= cooldown_time)
    {
        return 30.0;
    }
    else if (controller.count_ref > cooldown_time)
    {
        for (size_t i = 0; i < 10; i++)
        {
            if (curva[i][0] > (controller.count_ref - cooldown_time))
            {
                return (float)curva[i][1];
            }
        }
        return 30.0;
    }
    return 30.0;
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
        send_ref();
        break;
    case CMD_REF_CURVE:
        printf("Reference: Curva\n");
        controller.ref_mode = 1;
        controller.count_ref = 0;
        send_ref();
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

    unsigned char control_code[11] = {SERVER_CODE, SEND_COMMAND_CODE, SEND_CONTROL_SIGNAL, MATRICULA};
    unsigned char ref_code[11] = {SERVER_CODE, SEND_COMMAND_CODE, SEND_REF_SIGNAL, MATRICULA};

    struct bme280_data bme_data;
    float internal_temperature;
    float potenciometro_value; // temp referencia
    float ref_curve_value;
    float control_output;
    int INT_control_output = 0;

    write_uart(controller.uart_filestream, temperature_code, 7);
    usleep(WAIT_TIME);
    read_data(controller.uart_filestream, temperature_code, &internal_temperature, 4);

    bme_data = get_sensor_data();

    if (controller.ref_mode == 0)
    {
        write_uart(controller.uart_filestream, potenciometro_code, 7);
        usleep(WAIT_TIME);
        read_data(controller.uart_filestream, potenciometro_code, &potenciometro_value, 4);
        pid_atualiza_referencia(potenciometro_value);
        display_temperatures(internal_temperature, bme_data.temperature, potenciometro_value, controller.ref_mode);
    }
    else
    {
        ref_curve_value = get_curve_value();
        pid_atualiza_referencia(ref_curve_value);
        memcpy(&ref_code[7], &ref_curve_value, 4);
        write_uart(controller.uart_filestream, ref_code, 11);
        usleep(WAIT_TIME);
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

    memcpy(&control_code[7], &INT_control_output, 4);
    write_uart(controller.uart_filestream, control_code, 11);
    usleep(WAIT_TIME);
}

void get_command()
{
    unsigned char command[7] = {SERVER_CODE, CMD_CODE, GET_COMMAND, MATRICULA};

    write_uart(controller.uart_filestream, command, 7);
    usleep(WAIT_TIME);
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
    usleep(WAIT_TIME);
    int response;
    read_data(controller.uart_filestream, send_status, &response, 4);
    printf("Current status: %d\n", response);
}

void send_ref()
{
    unsigned char ref_mode[11] = {SERVER_CODE, SEND_COMMAND_CODE, SEND_REF_MODE, MATRICULA};

    char byte = (char)controller.ref_mode;
    memcpy(&ref_mode[7], &byte, 1);
    write_uart(controller.uart_filestream, ref_mode, 8);
    usleep(WAIT_TIME);
    int response;
    read_data(controller.uart_filestream, ref_mode, &response, 4);
    printf("Reference: %d\n", response);
}

void controller_routine(int *uart_filestream)
{
    controller.uart_filestream = uart_filestream;

    while (1)
    {
        if (controller.on_off_state)
        {
            control(uart_filestream);
        }
        get_command();
        sleep(1);
    }
}