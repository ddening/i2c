/*************************************************************************
* Title		: i2c.h
* Author	: Dimitri Dening
* Created	: 22.12.2022
* Software	: Microchip Studio V7
* Hardware	: Atmega1284P
* License	: MIT License
* Usage		: see Doxygen manual
*
*       Copyright (C) 2022 Dimitri Dening
*
*       Permission is hereby granted, free of charge, to any person obtaining a copy
*       of this software and associated documentation files (the "Software"), to deal
*       in the Software without restriction, including without limitation the rights
*       to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*       copies of the Software, and to permit persons to whom the Software is
*       furnished to do so, subject to the following conditions:
*
*       The above copyright notice and this permission notice shall be included in all
*       copies or substantial portions of the Software.
*
*       THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*       IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*       FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*       AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*       LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*       OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*       SOFTWARE.
*
*************************************************************************/
#ifndef I2C_H_
#define I2C_H_

/* General libraries */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* User defined libraries */
#include "i2c_io.h"
#include "i2c_config.h"
#include "i2c_error_handler.h"
#include "ringbuffer.h"

/* Describes a i2c device */
typedef struct device_t {
    uint8_t address;
    uint8_t pin;
    uint8_t port;
    uint8_t ddr;
} device_t;

i2c_error_t i2c_init(i2c_config_t*);

i2c_error_t i2c_read(void);

i2c_error_t i2c_write(payload_t*);

extern payload_t* payload_create_i2c(priority_t priority, device_t* device, uint8_t* data, uint8_t number_of_bytes, callback_fn* callback);

#endif /* I2C_H_ */