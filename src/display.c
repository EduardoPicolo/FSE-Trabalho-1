#include "i2clcd.h"
#include "display.h"

void display_temperatures(float it, float et, float rt, int mode)
{
    if (wiringPiSetup() == -1)
        exit(1);

    fd = wiringPiI2CSetup(I2C_ADDR);
    // printf("fd = %d ", fd);

    lcd_init();
    ClrLcd();
    lcdLoc(LINE1);
    if (mode == 0 || mode == 1)
        typeln("UART");
    else if (mode == 2)
        typeln("TERMINAL");

    typeln("TR:");
    typeFloat(rt);

    lcdLoc(LINE2);
    typeln("TI:");
    typeFloat(it);

    typeln("TE:");
    typeFloat(et);
}