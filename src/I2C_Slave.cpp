/*
  I2C_Slave.cpp - Implement a basic I2C slave device

  This library is free software: you can redistribute it and/or modify it under
  the terms of the GNU General Public License as published by the Free Software
  Foundation, either version 3 of the License, or (at your option) any later
  version.

  This library is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
  details.

  You should have received a copy of the GNU General Public License along with
  this library. If not, see <https://www.gnu.org/licenses/>.

  -- Yuri - June 2022
*/

#include "I2C_Slave.h"

// Initialize Class Variables //////////////////////////////////////////////////

volatile uint32_t _numErrors = 0;
void (*I2C_Slave::_user_onCommand)(uint8_t, uint8_t){};

// Constructors ////////////////////////////////////////////////////////////////

I2C_Slave::I2C_Slave() {}

// Public Methods //////////////////////////////////////////////////////////////

void I2C_Slave::begin() {
    Wire.begin(_addr);
    Wire.onRequest(_onRequest);
    Wire.onReceive(_onReceive);
}

void I2C_Slave::begin(uint8_t addr) {
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
    if (Wire.available()) {
        idx = Wire.read();
        if (idx > BUFFER_LENGTH - 1) idx = 0;
    }
    Wire.write(_registers[idx]);
}

void I2C_Slave::_onReceive(int size) {
    if (size < 2) return;  // let the request handler process this

    if (size > 2) {
        // assume an error on more than 2 bytes - flush read buffer
        while (Wire.available()) Wire.read();
        _numErrors++;
        return;
    }

    // otherwise, two bytes indicate a command, pass to user function
    uint8_t b0, b1;
    b0 = Wire.read();
    b1 = Wire.read();
    _user_onCommand(b0, b1);
}

// Preinstantiate Objects //////////////////////////////////////////////////////

I2C_Slave Slave = I2C_Slave();
