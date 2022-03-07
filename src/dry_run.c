#include <unistd.h>
#include <stdio.h>

#include "dry_run.h"
#include "gpio.h"

void dry_run(void)
{
    printf("Dry Run Starting...\n");
    printf("\tTesting RESISTOR... ");
    activate_fan(100);
    sleep(2);
    deactivate_resistor();
    printf("✓.\n");

    printf("\tTesting FAN... ");
    activate_fan(100);
    sleep(2);
    deactivate_fan();
    printf("✓.\n");

    sleep(2);
    printf("Dry Run Finished\n\n");
}