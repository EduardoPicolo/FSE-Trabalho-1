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
#include "logger.h"

int uart0;

void handle_sigint()
{
    set_system_state(0);
    send_status();
    gpio_power_off();
    close_bme();
    close_uart(&uart0);
    exit(0);
}

int main(int argc, const char *argv[])
{
    openLogFile();

    int mode, constants;
    double ki, kp, kd;

    printf("Escolha as constantes:  \n");
    printf("[0] Rasp42\n");
    printf("[1] Rasp43\n");
    printf("[2] Customizado\n");
    scanf("%d", &constants);

    switch (constants)
    {
    case 0:
        pid_configura_constantes(30.0, 0.2, 400.0);
        break;
    case 1:
        pid_configura_constantes(20.0, 0.1, 100.0);
        break;
    case 2:
        printf("Digite o valor de Kp: ");
        scanf("%lf", &kp);
        printf("Digite o valor de Ki: ");
        scanf("%lf", &ki);
        printf("Digite o valor de Kd: ");
        scanf("%lf", &kd);
        pid_configura_constantes(kp, ki, kd);
        break;

    default:
        break;
    }

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

    init_uart(&uart0);
    init_gpio();
    init_bme();

    signal(SIGINT, handle_sigint);

    dry_run();

    printf("\nStart Controller\n");
    controller_routine();

    close_uart(&uart0);
}
