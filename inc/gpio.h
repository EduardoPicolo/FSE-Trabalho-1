#ifndef GPIO_H_
#define GPIO_H_

#define RESISTOR_PIN 4
#define FAN_PIN 5
#define PWM_RANGE 100

// INIT GPIO
int init_gpio();

void activate_fan(int pwm);
void deactivate_fan();

void activate_resistor(int pwm);
void deactivate_resistor();

void power_off();

#endif /* GPIO_H_ */
