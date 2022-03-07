#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

#include "../inc/bme280.h"
#include "../inc/bme280_handler.h"

struct bme280_dev dev;
int8_t FD = -1;

int8_t i2c_read(uint8_t reg_addr, uint8_t *data, uint32_t len, void *intf_ptr)
{
    write(FD, &reg_addr, 1);
    read(FD, data, len);
    return BME280_OK;
}

int8_t i2c_write(uint8_t reg_addr, const uint8_t *data, uint32_t len, void *intf_ptr)
{
    uint8_t *buffer;

    buffer = malloc(len + 1);
    buffer[0] = reg_addr;

    memcpy(buffer + 1, data, len);
    if (write(FD, buffer, len + 1) < (uint16_t)len)
    {
        return BME280_E_COMM_FAIL;
    }

    free(buffer);
    return BME280_OK;
}

void i2c_delay(uint32_t period, void *intf_ptr)
{
    usleep(period);
}

void init_bme()
{
    open_i2c_bus();
    configure_bme();
    configure_bme_sensor();
}

void open_i2c_bus()
{
    printf("Opening i2c... ");

    FD = open("/dev/i2c-1", O_RDWR);
    if (FD == -1)
    {
        printf("Error: Failed to open the i2c bus.\n");
        printf("Shutting down\n");
        exit(1);
    }
    else
    {
        printf("✓\n");
    }
}

void configure_bme()
{
    printf("Configuring bme... ");
    if (FD < 0)
        return;

    int8_t result = BME280_OK;

    dev.intf = BME280_I2C_INTF;
    dev.read = i2c_read;
    dev.write = i2c_write;
    dev.delay_us = i2c_delay;

    if (ioctl(FD, I2C_SLAVE, BME280_I2C_ADDR_PRIM) < 0)
    {
        fprintf(stderr, "Error: Failed to acquire bus access and/or talk to slave.\n");
        exit(1);
    }

    result = bme280_init(&dev);
    if (result != BME280_OK)
    {
        fprintf(stderr, "Failed to initialize the device (code %+d).\n", result);
        exit(1);
    }
    printf("✓\n");
}

void configure_bme_sensor()
{
    printf("Configuring bme sensor... ");
    int8_t result;
    uint8_t settings_sel;

    dev.settings.osr_h = BME280_OVERSAMPLING_1X;
    dev.settings.osr_p = BME280_OVERSAMPLING_16X;
    dev.settings.osr_t = BME280_OVERSAMPLING_2X;
    dev.settings.filter = BME280_FILTER_COEFF_16;
    dev.settings.standby_time = BME280_STANDBY_TIME_62_5_MS;

    settings_sel = BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_STANDBY_SEL | BME280_FILTER_SEL;

    result = bme280_set_sensor_settings(settings_sel, &dev);

    if (result != BME280_OK)
    {
        printf("Failed to set sensor settings (code %+d).\n", result);
        exit(1);
    }

    result = bme280_set_sensor_mode(BME280_NORMAL_MODE, &dev);

    if (result != BME280_OK)
    {
        printf("Failed to set sensor mode (code %+d).\n", result);
        exit(1);
    }

    printf("✓\n");
}

struct bme280_data get_sensor_data()
{
    int8_t result;
    struct bme280_data data;

    dev.delay_us(70, dev.intf_ptr);
    result = bme280_get_sensor_data(BME280_ALL, &data, &dev);
    if (result != BME280_OK)
    {
        printf("Failed to get sensor data (code %+d).\n", result);
        exit(1);
    }

    // #ifdef BME280_FLOAT_ENABLE
    //     printf("%0.2f, %0.2f, %0.2f\r\n", data.temperature, data.pressure, data.humidity);
    // #else
    //     printf("%ld, %ld, %ld\r\n", data.temperature, data.pressure, data.humidity);
    // #endif

    return data;
}

void close_bme()
{
    if (FD > 0)
    {
        printf("Closing i2c... ");
        close(FD);
        printf("✓\n");
    }
}