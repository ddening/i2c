/*************************************************************************
* Title		: I2C Unit Test
* Author	: Dimitri Dening
* Created	: 13.02.2022 19:30:22
* Software	: Microchip Studio V7
* Hardware	: Atmega2560
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

/* Define CPU frequency in Hz here if not defined in Makefile */
#ifndef F_CPU
#define F_CPU 10000000UL
#endif

/* General libraries */
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/* User defined libraries */
#include "suite.h"
#include "i2c.h"
#include "uart.h"
#include "led_lib.h"
	
#define I2C_DEVICE_ADDR 0x27  

/* HD44780 */
#define LINE_COUNT  0x04
#define LINE_LENGTH 0x14
#define LINE1	0x00
#define LINE2	LINE1 + 0x40
#define LINE3	LINE1 + LINE_LENGTH
#define	LINE4 	LINE2 + LINE_LENGTH
#define DDRAM_ADDR 0x80

/* OPCODE */
#define RSRW00 0x00
#define RSRW01 0x01
#define RSRW10 0x02
#define RSRW11 0x03

#define ALL_ON          0x0F
#define DISPLAY_OFF     0x08
#define DISPLAY_ON      0x0C
#define CURSOR_ON       0x0A    // 0b00001010 -> sets display off
#define BLINK_ON        0x09    // 0b00001001 -> sets display off
#define CLEAR_DISPLAY   0x01
#define RETURN_HOME     0x02
#define ENTRY_MODE      0x07
#define FUNCTION_SET_EUROPEAN 0x3C
    
static device_t* i2c_device;

static uint8_t dummy_payload[] = { 1, 2, 3, 4, 5 };

static int run_i2c_payload_test(const struct test_case* test){
    
    i2c_device = i2c_create_device(I2C_DEVICE_ADDR);
    
    if (i2c_device == NULL) {
        return TEST_ERROR;
    }
    
    uint8_t* data = (uint8_t*)malloc(sizeof(uint8_t) * ARRAY_LEN(dummy_payload));
    
    if (data == NULL) {
        return TEST_ERROR;
    }
    
    payload_t* payload = payload_create_i2c(PRIORITY_NORMAL, i2c_device, data, ARRAY_LEN(dummy_payload), NULL);
    
    if (payload->priority != PRIORITY_NORMAL                                ||
        payload->protocol.i2c.device->address != I2C_DEVICE_ADDR            ||
        payload->protocol.i2c.number_of_bytes != ARRAY_LEN(dummy_payload)   ||
        payload->protocol.i2c.callback != NULL) {
            free(data);
            free(payload);
            free(i2c_device);
            return TEST_FAIL;
        }
        
    /* Check data content */
    for (uint8_t i = 0; i < ARRAY_LEN(dummy_payload); i++) {
        if (payload->protocol.i2c.data[i] != dummy_payload[i]){
            free(data);
            free(payload);
            free(i2c_device);
            return TEST_FAIL;
        }
    }
    
    free(data);
    free(payload);
    free(i2c_device);  
    return TEST_PASS;
}

static void _check_fake_busy ( void ) {
    _delay_ms(20); // Fake Busy Response
}

static void _send_10_bit( uint8_t opcode, uint8_t instruction ) {
    
    _check_fake_busy();
    
    i2c_error_t err;
    
    uint8_t* data = (uint8_t*) malloc( sizeof( uint8_t ) * 2 );
    
    if ( data == NULL ) {
        return;
    }
         
    data[0] = ( ( opcode << 6) | ( instruction >> 2 ) );
    data[1] = ( instruction << 6 );
    
    payload_t* payload = payload_create_i2c(PRIORITY_NORMAL, i2c_device, data, 2, NULL);
    
    if ( payload == NULL ) {
        return;
    }
    
    err = i2c_write(payload);
    
    free( data );
}

static int run_i2c_memory_leak_test(const struct test_case* test) {
       
    _send_10_bit( RSRW00, FUNCTION_SET_EUROPEAN ); // has to be sent first!
    _send_10_bit( RSRW00, DISPLAY_OFF );
    _send_10_bit( RSRW00, ENTRY_MODE );
    _send_10_bit( RSRW00, CLEAR_DISPLAY );
    _send_10_bit( RSRW00, RETURN_HOME );
    _send_10_bit( RSRW00, ALL_ON );
    
    return TEST_PASS;
}
    
int test_i2c(void) {
    
	cli();		
	
	led_init();
	
	uart_init();
	
	i2c_init(&i2c_config);
    
    i2c_device = i2c_create_device(I2C_DEVICE_ADDR);
	
	sei();
  	
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