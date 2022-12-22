/*************************************************************************
* Title		: i2c.c
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

/* General libraries */
#include <avr/interrupt.h>

/* User defined libraries */
#include "i2c.h"

static queue_t q;

static queue_t* queue = NULL;

static payload_t* payload = NULL;

static device_t* device = NULL;

i2c_error_t i2c_init(i2c_config_t* config){
    
    queue = queue_init(&q);
    
    return I2C_NO_ERROR;
}

i2c_error_t i2c_read(void){   
    return I2C_NO_ERROR;
}

i2c_error_t i2c_write(payload_t* _payload){   
    return I2C_NO_ERROR;
}