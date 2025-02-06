/*
  I2C_Slave.cpp - Implement a basic I2C slave device
  Copyright (C) 2024 Yuri Rage

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "I2C_Slave.h"

// Initialize Class Variables //////////////////////////////////////////////////

volatile uint32_t _numErrors = 0;
void (*I2C_Slave::_user_onCommand)(uint8_t, uint8_t){};

// create a new wire object
TwoWire new_Wire = TwoWire();


// Constructors ////////////////////////////////////////////////////////////////

I2C_Slave::I2C_Slave() {}

// Public Methods //////////////////////////////////////////////////////////////

void I2C_Slave::begin() {
    new_Wire.begin(_addr);
    new_Wire.onRequest(_onRequest);
    new_Wire.onReceive(_onReceive);
}

void I2C_Slave::begin(uint8_t addr) {
    _addr = addr;
    this->begin();
}

// assign the pin if alternate I2C is used esle assign default pin
void I2C_Slave::begin(uint8_t addr, uint32_t sda = SDA, uint32_t scl = SCL) {
    new_Wire.setSDA(sda);
    new_Wire.setSCL(scl);
    _addr = addr;
    this->begin();
}

// return the number of I2C comm errors encountered
uint32_t I2C_Slave::numErrors() { return _numErrors; }

// return number of available registers
size_t I2C_Slave::numRegisters() { return BUFFER_LENGTH; }

// sets the function called on receipt of a command
void I2C_Slave::onCommand(void (*function)(uint8_t, uint8_t)) {
    _user_onCommand = function;
}

// explicitly handle writing char buffers to registers
// register 0 is set to the buffer size (in bytes)
size_t I2C_Slave::writeRegisters(char* buf) {
    size_t sz = strlen(buf);
    _registers[0] = sz;
    char* reg = (char*)&_registers[1];
    memcpy(reg, buf, sz);
    return sz;
}

// Private Methods
// //////////////////////////////////////////////////////////////

void I2C_Slave::_onRequest() {
    uint8_t idx = 0;
    if (new_Wire.available()) {
        idx = new_Wire.read();
        if (idx > BUFFER_LENGTH - 1) idx = 0;
    }
    new_Wire.write(_registers[idx]);
}

void I2C_Slave::_onReceive(int size) {
    if (size < 2) return;  // let the request handler process this

    if (size > 2) {
        // assume an error on more than 2 bytes - flush read buffer
        while (new_Wire.available()) new_Wire.read();
        _numErrors++;
        return;
    }

    // otherwise, two bytes indicate a command, pass to user function
    uint8_t b0, b1;
    b0 = new_Wire.read();
    b1 = new_Wire.read();
    _user_onCommand(b0, b1);
}

// Preinstantiate Objects //////////////////////////////////////////////////////

I2C_Slave Slave = I2C_Slave();
