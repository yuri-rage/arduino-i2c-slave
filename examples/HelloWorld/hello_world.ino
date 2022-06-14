// I2C_Slave hello world example
// creates an I2C slave device on the microcontroller hardware I2C pins
// stores a character array for retrieval over the I2C bus
// connection to ArduPilot autopilots requires extras/hello_world.lua

#include <Arduino.h>
#include <I2C_Slave.h>

void setup() {
    Slave.begin();
    char buffer[] = "Hello world!";
    Slave.writeRegisters(buffer);
}

void loop() {}
