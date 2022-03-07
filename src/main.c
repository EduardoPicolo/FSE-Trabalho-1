#include <stdio.h>
#include <unistd.h>  //Used for UART
#include <fcntl.h>   //Used for UART
#include <termios.h> //Used for UART
#include <string.h>
#include <signal.h>
#include <sys/signal.h>
#include <stdlib.h>

#include "uart.h"
#include "dry_run.h"
#include "gpio.h"
#include "pid.h"
#include "controller.h"
#include "i2clcd.h"
#include "bme280_handler.h"

#include <wiringPi.h>
#include <softPwm.h>

int uart0;

void handle_sigint()
{
    power_off();
    close_bme();
    close_uart(&uart0);
    exit(0);
}

int main(int argc, const char *argv[])
{
    init_uart(&uart0);
    init_gpio();
    init_bme();

    signal(SIGINT, handle_sigint);

    dry_run();

    printf("\nStart Controller\n");
    pid_configura_constantes(30.0, 0.2, 400.0);
    controller_routine(&uart0);

    close_uart(&uart0);
}
