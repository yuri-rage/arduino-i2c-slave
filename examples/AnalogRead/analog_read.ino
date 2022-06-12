// I2C_Slave analog read example
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

uint32_t num_errors = 0;
int err_value = -1;

// callback to handle I2C commands upon receipt
void command_handler(uint8_t command, uint8_t value) {
    switch (command) {
        case GET_NUM_PINS:
            Slave.writeRegisters((uint8_t)NUM_ANALOG_INPUTS);
            Serial.println(F("Pin count requested.\n"));
            break;
        case SET_PIN_INDEX:
            if (value < NUM_ANALOG_INPUTS) {
                Slave.writeRegisters(analogRead(value));
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
    Serial.println(NUM_ANALOG_INPUTS);

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
