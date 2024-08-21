/*
  I2C_Slave DS18B20 example
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
// receives basic commands and sends temperature data as register values
// connection to ArduPilot autopilots requires extras/ds18b20.lua

#include <Arduino.h>
#include <DallasTemperature.h>
#include <I2C_Slave.h>
#include <OneWire.h>

#define I2C_ADDR 0x09  // unused address (https://i2cdevices.org/addresses)
#define SERIAL_BAUD 115200
#define ONE_WIRE_BUS 2  // pin where DS18B20s are connected
#define MAX_DEVICES 16  // up to 255
#define DEFAULT_RESOLUTION 12
#define CONVERSION_TIME_MS 750  // https://datasheets.maximintegrated.com/en/ds/DS18B20.pdf

// command (single byte) definitions
#define SET_RESOLUTION 0xAA
#define USE_CELSIUS 0xAC
#define USE_FAHRENHEIT 0xAF
#define GET_NUM_SENSORS 0xFE
#define SET_SENSOR_INDEX 0xFF

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature ds_bus(&oneWire);

uint8_t units = USE_FAHRENHEIT;
uint8_t num_sensors = 0;
uint32_t num_errors = 0;

struct {
    DeviceAddress addr;
    double temp;
} devices[MAX_DEVICES];

// callback to handle I2C commands upon receipt
void command_handler(uint8_t command, uint8_t value) {
    switch (command) {
        case SET_RESOLUTION:
            if (value > 8 && value < 13) {
                ds_bus.setResolution(value);
                Serial.print(F("Resolution set to "));
                Serial.print(value);
                Serial.println(F(" bits.\n"));
            } else {
                Serial.print(F("Invalid resolution request: "));
                Serial.print(value);
                Serial.println(F("\n"));
            }
            break;
        case USE_CELSIUS:
            units = USE_CELSIUS;
            Serial.println(F("Units set to Celsius.\n"));
            break;
        case USE_FAHRENHEIT:
            units = USE_FAHRENHEIT;
            Serial.println(F("Units set to Fahrenheit.\n"));
            break;
        case GET_NUM_SENSORS:
            Slave.writeRegisters(num_sensors);
            Serial.println(F("Device count requested.\n"));
            break;
        case SET_SENSOR_INDEX:
        // store the selected device's temperature in the I2C registers
            if (value < num_sensors) {
                Slave.writeRegisters(devices[value].temp);
            } else {
                Serial.print(F("Requested sensor index ("));
                Serial.print(value);
                Serial.println(F(") out of range.  Using index 0.\n"));
                Slave.writeRegisters(devices[0].temp);
            }
            break;
    }
}

void setup() {
    Slave.begin(I2C_ADDR);
    Slave.onCommand(command_handler);
    Serial.begin(SERIAL_BAUD);
    ds_bus.begin();
    num_sensors = ds_bus.getDeviceCount();
    ds_bus.setResolution(DEFAULT_RESOLUTION);

    // print some debug info
    Serial.print(F("\nSlave address      : 0x"));
    if (I2C_ADDR < 0x10) Serial.print('0');
    Serial.println(I2C_ADDR, HEX);
    Serial.print(F("Device count       : "));
    Serial.print(num_sensors);
    Serial.print(F("\nDefault resolution : "));
    Serial.print(DEFAULT_RESOLUTION);
    Serial.print(F(" bits\nDefault units      : "));
    size_t t = (units == USE_FAHRENHEIT) ? Serial.println(F("Fahrenheit"))
                                         : Serial.println(F("Celsius"));

    // store all device addresses for polling
    for (int i = 0; i < num_sensors; i++) {
        ds_bus.getAddress(devices[i].addr, i);
    }

#ifdef LED_BUILTIN
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
#endif

    Serial.println(F("\nInitialization complete.\n"));
}

void loop() {
    // collect all sensor temp data
    for (int i = 0; i < num_sensors; i++) {
        float val = (units == USE_FAHRENHEIT)
                        ? ds_bus.getTempF(devices[i].addr)
                        : ds_bus.getTempC(devices[i].addr);
        devices[i].temp = val;
    }

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

    // request temp conversion and allow time for it to occur
    ds_bus.requestTemperatures();
    delay(CONVERSION_TIME_MS);
}
