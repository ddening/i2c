/*************************************************************************
* Title		: SPI Unit Test
* Author	: Dimitri Dening
* Created	: 13.02.2022 19:30:22
* Software	: Microchip Studio V7
* Hardware	: Atmega1284P
* Usage		: see Doxygen manual
*
* This is a modified version of Microchip Studios' SPI Unit Test.
* Following the license from Microchip used in the original file.
*
* Subject to your compliance with these terms, you may use Microchip
* software and any derivatives exclusively with Microchip products.
* It is your responsibility to comply with third party license terms applicable
* to your use of third party software (including open source software) that
* may accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
* INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
* AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
* LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
* LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
* SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
* POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
* ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
* RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*
*************************************************************************/

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "suite.h"
#include "i2c.h"
#include "uart.h"
#include "led_lib.h"

/* Define CPU frequency in Hz here if not defined in Makefile */
#ifndef F_CPU
#define F_CPU 8000000UL
#endif
	
static device_t* i2c_device;

static int run_i2c_memory_leak_test(const struct test_case* test) {
    
    return TEST_PASS;
}

    
int main(void) {
    
	cli();		
	
	led_init();
	
	uart_init();
	
	i2c_init(&i2c_config);
	
	sei();
	
	// i2c_device = i2c_create_device();
    	
	DEFINE_TEST_CASE(i2c_memory_leak_test, NULL, run_i2c_memory_leak_test, NULL, "I2C memory leak test");

	/* Put test case addresses in an array */
	DEFINE_TEST_ARRAY(i2c_tests) = {
        &i2c_memory_leak_test,
	};
    	
	/* Define the test suite */
	DEFINE_TEST_SUITE(i2c_suite, i2c_tests, "I2C driver test suite");
    
	/* Run all tests in the test suite */
	test_suite_run(&i2c_suite);
	
	while (1) { /* Busy-wait forever. */ }
}