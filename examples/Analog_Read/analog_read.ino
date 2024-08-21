/*
  I2C_Slave analog read example
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
// sends analog pin data as register values
// connection to ArduPilot autopilots requires extras/analog_read.lua

#include <Arduino.h>
#include <I2C_Slave.h>

#define I2C_ADDR 0x09  // unused address (https://i2cdevices.org/addresses)
#define SERIAL_BAUD 115200

// command (single byte) definitions
#define GET_NUM_PINS 0xFE
#define SET_PIN_INDEX 0xFF

#ifdef ESP32  // board compatibility
static const uint8_t A1 = 34;
static const uint8_t A2 = 35;
#endif

uint32_t num_errors = 0;
const int err_value = -1;

// array of analog pins to monitor
const uint32_t analog_pins[] = { (uint32_t)A0, (uint32_t)A1, (uint32_t)A2, (uint32_t)A3 };
const uint8_t num_pins = sizeof(analog_pins) / sizeof(analog_pins[0]);

// callback to handle I2C commands upon receipt
void command_handler(uint8_t command, uint8_t value) {
    switch (command) {
        case GET_NUM_PINS:
            Slave.writeRegisters(num_pins);
            Serial.println(F("Pin count requested.\n"));
            break;
        case SET_PIN_INDEX:
            if (value < num_pins) {
                Slave.writeRegisters(analogRead(analog_pins[value]));
            } else {
                Slave.writeRegisters(err_value);
            }
            break;
    }
}

void setup() {
    Slave.begin(I2C_ADDR);
    Slave.onCommand(command_handler);
    Serial.begin(SERIAL_BAUD);

    // print some debug info
    Serial.print(F("\nSlave address    : 0x"));
    if (I2C_ADDR < 0x10) Serial.print('0');
    Serial.println(I2C_ADDR, HEX);
    Serial.print("Analog pin count : ");
    Serial.println(num_pins);

#ifdef LED_BUILTIN
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
#endif

    Serial.println(F("\nInitialization complete.\n"));
}

void loop() {
    // print some debug info
    uint32_t e = Slave.numErrors();
    if (num_errors != e) {
        num_errors = e;
        Serial.print(F("I2C communication error count: "));
        Serial.print(num_errors);
        Serial.println(F("\n"));
    }

#ifdef LED_BUILTIN
    // single line LED blink
    digitalWrite(LED_BUILTIN, digitalRead(LED_BUILTIN) ^ 1);
#endif

    delay(1000);
}
