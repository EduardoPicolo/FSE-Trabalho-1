#ifndef _BME280_HANDLER_H_
#define _BME280_HANDLER_H_

#include "bme280_defs.h"

struct bme280_dev dev;
int8_t FD;

int8_t i2c_read(uint8_t reg_addr, uint8_t *data, uint32_t len, void *intf_ptr);

int8_t i2c_write(uint8_t reg_addr, const uint8_t *data, uint32_t len, void *intf_ptr);

void i2c_delay(uint32_t period, void *intf_ptr);

void init_bme();

void open_i2c_bus();

void configure_bme();

void close_bme();

void configure_bme_sensor();

struct bme280_data get_sensor_data();

#endif