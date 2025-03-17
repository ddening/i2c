# I2C Library for AVR Microcontrollers

*A lightweight and efficient I2C (TWI) driver for AVR microcontrollers.*

## Overview
This library provides an interrupt-driven I2C (TWI) communication interface for AVR microcontrollers.

## Features
- Interrupt-driven I2C communication
- Supports Master mode only
- Non-blocking operation
- FIFO-based buffer for queued transactions
- Compatible with multiple AVR devices

## Prerequisites

- **Hardware**: Microcontroller with I2C support (e.g., ATmega series).
- **Software**: AVR-GCC compiler, AVRDUDE for programming, and a suitable development environment.

## Dependencies
This library is self-contained but may require additional AVR utilities. Check out the related repository:
[libAVR - Required Dependencies](https://github.com/ddening/libAVR)

## Installation
Clone this repository and include the necessary files in your project:
```sh
$ git clone https://github.com/ddening/i2c.git
```
Include the header in your code:
```c
#include "i2c.h"
```

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

