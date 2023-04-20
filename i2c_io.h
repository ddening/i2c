/*************************************************************************
* Title		: i2c_io.h
* Author	: Dimitri Dening
* Created	: 22.12.2022
* Software	: Microchip Studio V7
* Hardware	: Atmega2560
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
*	This file should only be included from <spi.h>, never directly.
*************************************************************************/

/**
@file i2c_io.h
@author Dimitri Dening
@date 22.12.2022
@copyright (C) 2022 Dimitri Dening, MIT License
@brief Port declaration for the ATmega1284P.

This file provides the required port delcarations to use the built-in I2C.
Extend this file according to the datasheet of your MCU.

@note This file should only be included from <i2c.h>, never directly.
@bug No known bugs.
*/
#ifndef I2C_IO_H_
#define I2C_IO_H_

#include <avr/io.h>

/* I2C Port Declaration */
#if defined(__AVR_ATmega1284P__) || defined(__AVR_ATmega16__)
#elif defined(__AVR_ATmega2560__)
#   define SDA PD1
#   define SCL PD0
#else
#  if !defined(__COMPILING_AVR_LIBC__)
#    warning "ATmega1284P or ATmega16 not found"
#  endif
#endif

#endif /* I2C_IO_H_ */