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

/* I2C TWSR FLAGS */
#define I2C_START		0x08
#define I2C_REP_START	0x10

#define I2C_MASTER_TX_ADDR_ACK  0x18  // SLA+W transmitted and ACK received
#define I2C_MASTER_TX_ADDR_NACK 0x20  // SLA+W transmitted and NACK received
#define I2C_ARB_LOST	        0x38

#define I2C_MASTER_TX_DATA_ACK  0x28  // Data transmitted and ACK received
#define I2C_MASTER_TX_DATA_NACK 0x30  // Data transmitted and NACK received

#define I2C_MASTER_RX_ADDR_ACK  0x40  // SLA+R transmitted and ACK received
#define I2C_MASTER_RX_ADDR_NACK 0x48  // SLA+R transmitted and NACK received
#define I2C_MASTER_RX_DATA_ACK  0x50  // Data transmitted and ACK received
#define I2C_MASTER_RX_DATA_NACK 0x58  // Data transmitted and NACK received

#define I2C_NO_STATE  0xF8 
#define I2C_BUS_ERROR 0x00 

/* I2C Protocol Macros */
#define I2C_TWCR_INIT()     (TWCR = (1 << TWEN)|(0 << TWIE)|(0 << TWINT)|(0 << TWEA)|(0 << TWSTA)|(0 << TWSTO)|(0 << TWWC))
#define I2C_SEND_START()    (TWCR = (1 << TWINT)|(1 << TWSTA)|(0 << TWSTO)|(1 << TWEN)|(1 << TWIE))
#define I2C_SEND_STOP()     (TWCR = (1 << TWINT)|(0 << TWSTA)|(1 << TWSTO)|(1 << TWEN)|(0 << TWIE))
#define I2C_SEND_TRANSMIT() (TWCR = (1 << TWINT)|(0 << TWSTA)|(0 << TWSTO)|(1 << TWEN)|(1 << TWIE))
#define I2C_SEND_ACK()      (TWCR = (1 << TWINT)|(1 << TWEN)|(1 << TWIE)|(1 << TWEA))
#define I2C_SEND_NACK()     (TWCR = (1 << TWINT)|(1 << TWEN)|(1 << TWIE))

typedef enum {
    I2C_ACTIVE,
    I2C_INACTIVE
} I2C_STATE_T;

static I2C_STATE_T I2C_STATE;

static queue_t q;

static queue_t* queue = NULL;

static payload_t* payload = NULL;

static device_t* device = NULL;

i2c_error_t i2c_init(i2c_config_t* config){
    
    /* Activate Internal Pullups */
    SET_PIN_OUTPUT(PORTD, SDA);
    SET_PIN_OUTPUT(PORTD, SCL);
    
    /* Define Bit Genereator Unit */
    uint16_t scl_frequency = F_CPU / ( 16 + (2 * TWBR) + 4^(TWPS0) ); //TWPS := Prescaler
       
    TWBR = 0x5C; // Target Frequency := 50kHz -> TWBR = 92 = 0x5C, with F_CPU=10MHz
    TWSR |= (0 << TWPS1) | (0 << TWPS0); // Set Prescaler To 1 
    
    TWCR = (0 << TWINT) | (0 << TWEA) | (0 << TWSTA) | (0 << TWSTO) | (0 << TWWC) | (1 << TWEN) | (0 << TWIE);
    
    queue = queue_init(&q);
    
    return I2C_NO_ERROR;
}

i2c_error_t _i2c(){
    
    if (I2C_STATE == I2C_INACTIVE) {
        
        payload = queue_dequeue(queue);
        
        I2C_STATE = I2C_ACTIVE;
        
        I2C_SEND_START();      
    }
    
    return I2C_NO_ERROR;
}

i2c_error_t i2c_read(payload_t* _payload){   
    
    _payload->protocol.i2c.device->address = (_payload->protocol.i2c.device->address << 1) | 0x01; 
    
    return I2C_NO_ERROR;
}

i2c_error_t i2c_write(payload_t* _payload){   
    
    i2c_error_t err;
    
    _payload->protocol.i2c.device->address = (_payload->protocol.i2c.device->address << 1) | 0x00; 
    
    err = queue_enqueue(queue, _payload);
    
    err = _i2c();
    
    return I2C_NO_ERROR;
}

device_t* i2c_create_device(uint8_t address){
    
    device_t* device = malloc(sizeof(device_t));
    
    if (device == NULL) {
        return NULL;
    }
    
    device->address = address; // First 7 bits describe the device address. Last bit := Read/Write
    
    return device;
}

i2c_error_t i2c_free_device(device_t* device){
    
    free(device);  
    
    device = NULL;   
    
    return I2C_NO_ERROR;
}

ISR(TWI_vect){
    
    /* Mask the prescaler bits to zero */
    switch(TWSR & I2C_NO_STATE) {
        case I2C_START:
        case I2C_REP_START: {
            TWDR = payload->protocol.i2c.device->address; // Load SLA+W
            I2C_SEND_TRANSMIT();
            break;
        }
          
        /* Address received by device -> ACK -> OK */ 
        case I2C_MASTER_TX_ADDR_ACK: {        
            TWDR = *(payload->protocol.i2c.data); // Load Data
            I2C_SEND_TRANSMIT();
            break;
        }
             
        /* Address NOT received by device -> NACK -> NOT OK -> Cleanup + STOP (?)
           -> Start error routine and/or load next job from buffer and skip this job */
        case I2C_MASTER_TX_ADDR_NACK: {         
            I2C_STATE = I2C_INACTIVE;
            I2C_SEND_STOP();
            payload_free_i2c(payload);
            break;
        }
            
        /* Data received by device -> ACK -> OK -> STOP transmission */
        case I2C_MASTER_TX_DATA_ACK: {
                      
            payload->protocol.i2c.number_of_bytes--;
            
            (payload->protocol.i2c.data)++;
            
            if (payload->protocol.i2c.number_of_bytes != 0) {
                TWDR = *(payload->protocol.i2c.data);
                I2C_SEND_TRANSMIT();          
            } else {
                I2C_STATE = I2C_INACTIVE;
                I2C_SEND_STOP();
            }
            break;
        }
           
        /*  Data NOT received by device -> NACK -> NOT OK
            -> STOP transmission OR REPEAT START and try to send data again (?)
            -> Start error routine */
        case I2C_MASTER_TX_DATA_NACK: {   
            I2C_STATE = I2C_INACTIVE;
            I2C_SEND_STOP();
            payload_free_i2c(payload);
            break;
        }
              
        case I2C_BUS_ERROR: {
           I2C_STATE = I2C_INACTIVE;
           I2C_SEND_STOP();
           payload_free_i2c(payload);
           break; 
        }
        
        case I2C_NO_STATE: {
            break;
        }
        
        default: {
            break;
        }            
    }
}