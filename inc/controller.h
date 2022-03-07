#ifndef CONTROLLER_H
#define CONTROLLER_H

typedef struct CONTROLLER
{
    int *uart_filestream;
    int on_off_state;
    int ref_mode;
    float ref_value;
    int count_ref;
    int count_alarm;
    int seconds;
    float temperature_history[3];
} CONTROLLER;

void set_mode(int mode);
void set_system_state(int state);
void set_uart_filestream(int *uart_filestream);
void controller_routine();
void control();
void get_command();
void send_status();

void send_mode();

void cooldown(int time);

#endif /* CONTROLLER_H */
