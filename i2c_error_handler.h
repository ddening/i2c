/*************************************************************************
* Title		: C include file for I2C Error Handling
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
*************************************************************************/

/**
@file i2c_error_handler.h
@author Dimitri Dening
@date 22.12.2022
@copyright (C) 2022 Dimitri Dening, MIT License
@brief Error handler for I2C.
*/

#ifndef I2C_ERROR_HANDLER_H_
#define I2C_ERROR_HANDLER_H_

/* Describes possible error states */
typedef enum i2c_error_t {
    I2C_NO_ERROR,
} i2c_error_t;

/**
 * @brief   Prototype of an error handler.
 *
 * If an error occurs, a specific led sequence is shown on the STK600.
 *
 * @return  Returns an error code.
 */
i2c_error_t error_handler(i2c_error_t error);

#endif /* I2C_ERROR_HANDLER_H_ */