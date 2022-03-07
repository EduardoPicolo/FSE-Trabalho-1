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

void controller_routine(int *uart_filestream);
void control();
void get_command();
void send_status();
void send_ref();
void cooldown(int time);

#endif /* CONTROLLER_H */
