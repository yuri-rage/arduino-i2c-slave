/*
  I2C_Slave.h - Implement a basic I2C slave device

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

#ifndef I2C_SLAVE_H
#define I2C_SLAVE_H

#include <Arduino.h>
#include <Wire.h>

// unused address (https://i2cdevices.org/addresses)
#define DEFAULT_I2C_ADDR 0x09

#ifndef BUFFER_LENGTH
#define BUFFER_LENGTH I2C_BUFFER_LENGTH
#endif

class I2C_Slave {
   private:
    uint8_t _addr = DEFAULT_I2C_ADDR;
    static void _onRequest();
    static void _onReceive(int);
    static void (*_user_onCommand)(uint8_t, uint8_t);

   public:
    I2C_Slave();
    void begin();
    void begin(uint8_t);
    uint32_t numErrors();
    size_t numRegisters();
    void onCommand(void (*)(uint8_t, uint8_t));
    template <typename T>
    size_t writeRegisters(T);
    size_t writeRegisters(char*);
};

// global register variable for use in static methods
inline volatile char _registers[BUFFER_LENGTH];  // BUFFER_LENGTH from Wire.h

// store any reasonable data object as an array of bytes (char)
// register 0 is set to the object size (in bytes)
template <typename T>
size_t I2C_Slave::writeRegisters(T val) {
    // implementation must reside in header file
    size_t sz = sizeof(val);
    _registers[0] = sz;
    char* reg = (char*)&_registers[1];
    memcpy(reg, &val, sz);
    return sz;
}

// I2C slave object on the hardware I2C bus
extern I2C_Slave Slave;

#endif  // I2C_SLAVE_H
