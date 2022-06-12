# Arduino I2C_Slave Library
Creates an I2C slave device on the hardware I2C bus of an Arduino compatible microcontroller.

## Background
I2C_Slave is intended for use with the Lua scripting engine on [ArduPilot autopilots](https://ardupilot.org/) to allow communication with otherwise unsupported sensors.  However, it can easily be used wherever a simple means of I2C communication is desired.

## Usage
The library's `Slave` object is used much like the `Wire` object.  The library uses the hardware I2C bus and, in the background, the default `Wire` object, so any connected I2C peripherals should probably use a different bus, such as one created with the [SoftwareWire](https://www.arduino.cc/reference/en/libraries/softwarewire/) library.

To create an I2C slave interface, call the `begin()` method from within your sketch's `setup()` function.  The single byte address argument is optional and defaults to the unused address, `0x09`.
```
void setup() {}
    Slave.begin(); // or Slave.begin(0x0A);
}
```

The library creates virtual registers in which a data object may be stored.  The size (in bytes) of the data object is stored in the first byte (register 0).  The remainder of the registers contain the data object as a byte (char) array.  Store data using the `writeRegisters()` method.
```
float my_data = 3.14159265;
Slave.writeRegisters(my_data);
```

The library expects read requests to be preceded by a single byte transmission containing the requested register index.  Received transmissions of exactly two bytes are interpreted as commands.  Transmissions longer than two bytes will be discarded as errors.

Commands are handled by a callback function passed to the `onCommand()` method.  A switch statement makes for a simple command handler.
```
int first = 1;
int second = 2;

void command_handler(uint8_t command, uint8_t value) {
    switch (command) {
        case 0x00:
            Slave.writeRegisters(first);
            break;
        case 0x01:
            Slave.writeRegisters(second);
            break;
        case 0x02:
            first = value;
            break;
        case 0x03:
            second = value;
            break;
    }
}

Slave.onCommand(command_handler);
```

## Examples

The included DS18B20 example polls OneWire temperature sensors connected to pin 2, transmits their values upon request, and interprets a small set of configuration commands.  To use the example code with ArduPilot, a `ds18b20.lua` script is provided in the `extras` directory.

## License
This library is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later
version.

This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
details.

You should have received a copy of the GNU General Public License along with this library. If not, see <https://www.gnu.org/licenses/>.

<sub>&copy; Yuri, June 2022</sub>