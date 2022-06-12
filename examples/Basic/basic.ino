// I2C_Slave basic example
// creates an I2C slave device on the microcontroller hardware I2C pins
// stores a single analog pin value for retrieval over the I2C bus
// connection to ArduPilot autopilots requires extras/analog_read.lua

#include <Arduino.h>
#include <I2C_Slave.h>

void setup() { Slave.begin(); }

void loop() {
    Slave.writeRegisters(analogRead(A0));
    delay(100);  // update at 10Hz
}
