--[[----------------------------------------------------------------------------
    analog_read.lua ArduPilot Lua script

    Polls an Arduino (or similar device) on the I2C bus for its analog pin values.

    Intended for use with the I2C_Slave Arduino library.

    CAUTION: This script is capable of engaging and disengaging autonomous control
    of a vehicle.  Use this script AT YOUR OWN RISK.

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
------------------------------------------------------------------------------]]

-- Cube and Matek autopilots typically make I2C bus 0 available
-- some Pixhawk (and other) autopilots only expose I2C bus 1
-- set the I2C_BUS variable as appropriate for your board
local I2C_BUS           =    0
local RUN_INTERVAL_MS   =  500
local SLAVE_ADDR        = 0x09
local ERROR_VALUE       = -255
local GET_NUM_PINS      = 0xFE
local SET_PIN_INDEX     = 0xFF
local MAV_SEVERITY_INFO =    6

local num_pins = 0

local arduino_i2c = i2c.get_device(I2C_BUS, SLAVE_ADDR)
arduino_i2c:set_retries(10)

local function unpack_uint(b)
    if type(b) ~= 'table' or #b == 0 then return ERROR_VALUE end
    local packed_string = string.char(table.unpack(b))
    local fmt = ('I%d'):format(#b)  -- unsigned integer of table size
    local val = string.unpack(fmt, packed_string)
    return val
end

local function read_register_data()
    local bytes = {}
    -- arduino i2c_slave library passes data size in register byte 0
    local size = arduino_i2c:read_registers(0)
    if not size then return nil end
    -- retrieve and store register data
    for idx = 1, size do
        bytes[idx] = arduino_i2c:read_registers(idx)
    end
    return bytes
end

local function get_num_pins()
    arduino_i2c:write_register(GET_NUM_PINS, 0)
    return arduino_i2c:read_registers(1)
end

function update()
    for idx = 0, num_pins - 1 do
        -- request to store analog pin value in I2C registers for given index
        arduino_i2c:write_register(SET_PIN_INDEX, idx)
        -- now read the register data and collect its value as an unsigned integer
        local data = read_register_data()
        gcs:send_named_float('A' .. idx, unpack_uint(data))
    end
    return update, RUN_INTERVAL_MS
end

function init()
    num_pins = get_num_pins()
    if num_pins and num_pins > 0 then
        gcs:send_text(MAV_SEVERITY_INFO, string.format('Analog Read: Monitoring %d pin(s)', num_pins))
        return update, RUN_INTERVAL_MS
    end
    return init, RUN_INTERVAL_MS
end

gcs:send_text(MAV_SEVERITY_INFO, 'Analog Read: Script active')

return init, RUN_INTERVAL_MS
