/*
  I2C_Slave basic example
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

// creates an I2C slave device on the microcontroller hardware I2C pins
// stores a single analog pin value for retrieval over the I2C bus
// connection to ArduPilot autopilots requires extras/basic.lua

#include <Arduino.h>
#include <I2C_Slave.h>

void setup() { Slave.begin(); }

void loop() {
    Slave.writeRegisters(analogRead(A0));
    delay(100);  // update at 10Hz
}
