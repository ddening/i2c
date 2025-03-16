/*************************************************************************
* Title		: i2c_config.h
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
* NOTES:
*	This file should only be included from <i2c.h>, never directly.
*************************************************************************/
#ifndef I2C_CONFIG_H_
#define I2C_CONFIG_H_

#define I2C_STANDARD_MODE 100000  // 100 kHz
#define I2C_FAST_MODE     400000  // 400 kHz
#define I2C_HIGH_SPEED   3400000  // 3.4 MHz (not supported on all ATmega)

#define I2C_MASTER_MODE  1
#define I2C_SLAVE_MODE   0 // Not implemented yet.

#define I2C_DEFAULT_CONFIG { \
	.scl_target_frequency = I2C_STANDARD_MODE, \
	.internal_pullups = 1, \
	.mode = I2C_MASTER_MODE, \
}

typedef struct {
	uint32_t scl_target_frequency; // Target I2C frequency (e.g., 100000 for 100 kHz)
	uint8_t internal_pullups;         // Enable/Disable internal pull-ups
	uint8_t mode;                  // Master or Slave mode
} i2c_config_t;

#endif /* I2C_CONFIG_H_ */