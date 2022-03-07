#include "gpio.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>
#include <softPwm.h>

int init_gpio()
{
    if (wiringPiSetup() == -1)
    {
        printf("Error: Failed to start WiringPi\n");
        exit(1);
    }
    else
    {
        printf("WiringPi configured\n");
    }
    pinMode(RESISTOR_PIN, OUTPUT);
    pinMode(FAN_PIN, OUTPUT);

    if (softPwmCreate(RESISTOR_PIN, 0, PWM_RANGE))
    {
        printf("Error: Failed to configure RESISTOR PWN on channel: %d\n", RESISTOR_PIN);
    }
    if (softPwmCreate(FAN_PIN, 0, PWM_RANGE))
    {
        printf("Error: Failed to configure FAN PWN on channel: %d\n", FAN_PIN);
    }

    return 0;
}

void activate_fan(int pwm)
{
    softPwmWrite(FAN_PIN, pwm);
}

void deactivate_fan()
{

    softPwmWrite(FAN_PIN, 0);
}

void activate_resistor(int pwm)
{
    softPwmWrite(RESISTOR_PIN, pwm);
}

void deactivate_resistor()
{
    softPwmWrite(RESISTOR_PIN, 0);
}

void power_off()
{
    printf("\n\t Shutting down GPIO... ");
    softPwmStop(RESISTOR_PIN);
    softPwmStop(FAN_PIN);
    sleep(1);
    printf("Closed.\n");
}
