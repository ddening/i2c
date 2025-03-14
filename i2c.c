/*************************************************************************
* Title		: i2c.c
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
*************************************************************************/

/* General libraries */
#include <avr/interrupt.h>

/* User defined libraries */
#include "i2c.h"
#include "utils.h"
#include "memory.h"

// I2C Status Codes Master TX/RX Mode
#define I2C_STATUS_START		0x08
#define I2C_STATUS_REPEAT_START	0x10
#define I2C_STATUS_ARB_LOST		0x38  // Arbitration lost in SLA+W or data bytes

// I2C Status Codes Master TX Mode
#define I2C_STATUS_TX_ADDR_ACK  0x18  // SLA+W transmitted and ACK received
#define I2C_STATUS_TX_ADDR_NACK 0x20  // SLA+W transmitted and NACK received
#define I2C_STATUS_TX_DATA_ACK  0x28  // Data transmitted and ACK received
#define I2C_STATUS_TX_DATA_NACK 0x30  // Data transmitted and NACK received

// I2C Status Codes Master RX Mode
#define I2C_STATUS_RX_ADDR_ACK  0x40  // SLA+R transmitted and ACK received
#define I2C_STATUS_RX_ADDR_NACK 0x48  // SLA+R transmitted and NACK received
#define I2C_STATUS_RX_DATA_ACK  0x50  // Data transmitted and ACK received
#define I2C_STATUS_RX_DATA_NACK 0x58  // Data transmitted and NACK received 

// I2C Protocol Macros
#define I2C_TWCR_INIT()			TWCR = (0 << TWINT) | (0 << TWEA) | (0 << TWSTA) | (0 << TWSTO) | (0 << TWWC) | (1 << TWEN) | (1 << TWIE)

// Master Transmitter Mode
#define I2C_TX_START()			TWCR = (1 << TWINT) | (1 << TWSTA) | (0 << TWSTO) | (1 << TWEN) | (1 << TWIE)
#define I2C_TX_REPEAT_START()	TWCR = (1 << TWINT) | (1 << TWSTA) | (0 << TWSTO) | (1 << TWEN) | (1 << TWIE)
#define I2C_TX_TRANSMIT()		TWCR = (1 << TWINT) | (0 << TWSTA) | (0 << TWSTO) | (1 << TWEN) | (1 << TWIE)
#define I2C_TX_STOP()			TWCR = (1 << TWINT) | (0 << TWSTA) | (1 << TWSTO) | (1 << TWEN) | (0 << TWIE)
#define I2C_TX_STOP_START()		TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWSTO) | (1 << TWEN) | (1 << TWIE)

// Master Receiver Mode
#define I2C_RX_START()			TWCR = (1 << TWINT) | (1 << TWSTA) | (0 << TWSTO) | (1 << TWEN) | (1 << TWIE)
#define I2C_RX_TRANSMIT()		TWCR = (1 << TWINT) | (0 << TWSTA) | (0 << TWSTO) | (1 << TWEN) | (1 << TWIE)
#define I2C_RX_STOP()			TWCR = (1 << TWINT) | (0 << TWSTA) | (1 << TWSTO) | (1 << TWEN) | (0 << TWIE)
#define I2C_RX_STOP_START()		TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWSTO) | (1 << TWEN) | (1 << TWIE)
#define I2C_RX_SEND_NACK()		TWCR = (1 << TWINT) | (0 << TWSTA) | (0 << TWSTO) | (0 << TWEA) | (1 << TWEN) | (1 << TWIE)
#define I2C_RX_SEND_ACK()		TWCR = (1 << TWINT) | (0 << TWSTA) | (0 << TWSTO) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE)

typedef enum {
    I2C_ACTIVE,
    I2C_INACTIVE
} i2c_state_t;

static queue_t q;
static queue_t* queue = NULL;
static payload_t* payload = NULL;
static i2c_state_t I2C_STATE;

// Define CPU frequency in Hz here if not defined in Makefile
#ifndef F_CPU
#define F_CPU 10000000UL // Hz
#endif

i2c_error_t i2c_init(void) {
    
	uint32_t prescaler;
	uint32_t scl_target_frequency;
	uint32_t scl_frequency;
	
    // Activate Internal Pullups
    SET_PIN_OUTPUT(PORTD, SDA);
    SET_PIN_OUTPUT(PORTD, SCL);
    
	TWSR |= (0 << TWPS1) | (0 << TWPS0); // Set prescaler value to 1  
	
	// Define Bit Generator Unit
	prescaler = TWSR & 0x03;
	scl_target_frequency = 50000;
	TWBR = (F_CPU - (16 * scl_target_frequency)) / (2 * (1 << prescaler) * (1 << prescaler) * scl_target_frequency); // 4^n = 2^n * 2^n = (1 << n) * (1 << n)
	scl_frequency = F_CPU / (16 + (2 * TWBR) + (1 << prescaler) * (1 << prescaler)); // TODO: 
	
	// TODO: Make sure scl_frequency holds correct value
	if (scl_frequency > (F_CPU / 10)) {
		
	}
	 
	I2C_TWCR_INIT();
	
    I2C_STATE = I2C_INACTIVE;
    
    queue = queue_init(&q);
	
    return I2C_NO_ERROR;
}

i2c_error_t _i2c() {

    if (I2C_STATE == I2C_INACTIVE) {
        
        payload = queue_dequeue(queue);
        
        I2C_STATE = I2C_ACTIVE;

        I2C_TX_START();      
    }
    
    return I2C_NO_ERROR;
}

i2c_error_t i2c_read(payload_t* _payload) {   
    
    i2c_error_t err;
	
    _payload->i2c.mode = READ;
    
    err = queue_enqueue(queue, _payload);
    
    err = _i2c();
    
    return I2C_NO_ERROR;
}

i2c_error_t i2c_write(payload_t* _payload) {   
    
    i2c_error_t err;

	_payload->i2c.mode = WRITE; 

    err = queue_enqueue(queue, _payload);

    err = _i2c();
    
    return I2C_NO_ERROR;
}

device_t* i2c_create_device(uint8_t address) {
    
    device_t* device = (device_t*)malloc(sizeof(device_t));
    
    if (device == NULL) {
        return NULL;
    }
    
    device->address = address; // First 7 bits describe the device address. Last bit := Read/Write
    
    return device;
}

i2c_error_t i2c_free_device(device_t* device) {
    
    free(device);  
    
    device = NULL;   
    
    return I2C_NO_ERROR;
}

static void _isr_i2c_free_payload(void) {
	
	if (payload != NULL) {
		payload_free_i2c(payload);
		payload = NULL;
	}
}

static void _isr_i2c_no_ack_response(void) {
	
	_isr_i2c_free_payload();
	
	if (!queue_empty(queue)) {
		payload = queue_dequeue(queue);
		I2C_TX_STOP_START();
	} else {
		I2C_STATE = I2C_INACTIVE;
		I2C_TX_STOP();	
	}
}

static void _isr_i2c_handle_tx_complete(void) {

	if (payload->i2c.callback != NULL) {
		payload->i2c.callback(NULL);
		payload->i2c.callback = NULL;
	}
	
	_isr_i2c_free_payload();
	
	if (!queue_empty(queue)) {
		payload = queue_dequeue(queue);
		I2C_TX_STOP_START();
	} else {		
		I2C_STATE = I2C_INACTIVE;
		I2C_TX_STOP();	
	}
}

static void _isr_i2c_handle_rx_complete(void) {

	_isr_i2c_free_payload();
	
	if (!queue_empty(queue)) {
		payload = queue_dequeue(queue);
		I2C_RX_STOP_START();
	} else {
		I2C_STATE = I2C_INACTIVE;
		I2C_RX_STOP();
	}
}

static void _isr_i2c_callback(void) {
	
	if (payload->i2c.callback != NULL) {
		payload->i2c.callback(NULL);
		payload->i2c.callback = NULL;
	}
}

ISR(TWI_vect) {

    // Mask the prescaler bits to zero
    switch(TWSR & 0xF8) {
		// Master Transmitter Mode
        case I2C_STATUS_START:
        case I2C_STATUS_REPEAT_START: {	
			
			if (payload->i2c.mode == WRITE) {
				TWDR = ((payload->i2c.device->address << 1) | 0x00);			
			} else {			
				TWDR = ((payload->i2c.device->address << 1) | 0x01);
			}
            
            I2C_TX_TRANSMIT();
			
            break;
        }
          
        case I2C_STATUS_TX_ADDR_ACK: {  
				
            TWDR = *(payload->i2c.data);	
				
            I2C_TX_TRANSMIT();
			
            break;
        }
             
        case I2C_STATUS_TX_ADDR_NACK: {   
			    		
			_isr_i2c_no_ack_response();	
			
            break;
        }
            
        case I2C_STATUS_TX_DATA_ACK: {

            payload->i2c.number_of_bytes--;
			
            (payload->i2c.data)++;
            
            if (payload->i2c.number_of_bytes != 0) {	
						
                TWDR = *(payload->i2c.data);	
							
                I2C_TX_TRANSMIT();      	    
            } else {			
				_isr_i2c_handle_tx_complete();
			}
					
            break;
        }

        case I2C_STATUS_TX_DATA_NACK: { 
				   		
			_isr_i2c_no_ack_response();
			
            break;
        }
                     
		// Master Receiver Mode   
		case I2C_STATUS_RX_ADDR_ACK: {
			
			I2C_RX_SEND_ACK();
			
			break;
		} 
		
		case I2C_STATUS_RX_ADDR_NACK: {
			
			_isr_i2c_no_ack_response();
			
			break;
		}
		
		case I2C_STATUS_RX_DATA_ACK: {	
			
			payload->i2c.number_of_bytes--;
			
			(payload->i2c.data)++;
			
			_isr_i2c_callback();
			
			if (payload->i2c.number_of_bytes != 0) {
				I2C_RX_SEND_ACK();
			} else {
				I2C_RX_SEND_NACK();
			}
			
			break;
		}
		
		case I2C_STATUS_RX_DATA_NACK: {
			
			_isr_i2c_callback();
			_isr_i2c_handle_rx_complete();

			break;
		}
		
		default: {
			
			_isr_i2c_free_payload();
			
			I2C_STATE = I2C_INACTIVE;
			
			I2C_TX_STOP();
			
			break;
		}
    }
}