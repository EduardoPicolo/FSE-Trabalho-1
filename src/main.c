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
#include "reflow_handler.h"

int uart0;

void handle_sigint()
{
    set_system_state(0);
    power_off();
    close_bme();
    close_uart(&uart0);
    exit(0);
}

int main(int argc, const char *argv[])
{
    int mode;

    printf("Escolha o modo para a temperatura de referencia (TR):\n");
    printf("[0] Potenciometro\n");
    printf("[1] Curva Reflow\n");
    printf("[2] Terminal\n");
    printf("Escolha o modo para a temperatura de referencia: ");
    scanf("%d", &mode);

    if (mode == 2)
    {
        double tr;
        printf("Temperatura de referencia: ");
        scanf("%lf", &tr);
        printf("INPUTED REFERENCE: %f\n", tr);
        pid_set_reference(tr);
        set_mode(2);
    }
    else if (mode == 0)
    {
        set_mode(0);
    }
    else if (mode == 1)
    {
        loadReflowData();
        set_mode(1);
    }

    set_system_state(1);
    pid_configura_constantes(30.0, 0.2, 400.0);

    init_uart(&uart0);
    init_gpio();
    init_bme();

    signal(SIGINT, handle_sigint);

    dry_run();

    printf("\nStart Controller\n");
    controller_routine();

    close_uart(&uart0);
}
