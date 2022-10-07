/*
Connections:
Arduino NANO/Uno    vCS5530    Description
(ATMEGA 328P)      24bit ADC
D2 (GPIO4)      0 RST         Output from ESP to reset display
D1 (GPIO5)      1 CE          Output from ESP to chip select/enable display
D6 (GPIO12)     2 DC          Output from display data/command to ESP
D7 (GPIO13)     3 Din         Output from ESP SPI MOSI to display data input
D5 (GPIO14)     4 Clk         Output from ESP SPI clock
3V3             5 Vcc         3.3V from ESP to display
D0 (GPIO16)     6 BL          3.3V to turn backlight on, or PWM
G               7 Gnd         Ground
Dependencies:
https://github.com/yasir-shahzad/CS5530
1
*/


#include "Arduino.h"

#include <avr/pgmspace.h>
#include "SPI.h"
#include "CS5530.h"


CS5530 cell;

u32 startTime;
i32 value;

void setup() {
    Serial.begin(115200);

    if (cell.reset())
    	Serial.println("CS5530 Initialized Successfully");
    else
        Serial.println("Starting CS5530 failed");

    u32 tmp = cell.readRegister(CMD_CONFIG_READ);
    Serial.print("CONFIG Register:");
    Serial.println(tmp, BIN);

    cell.writeRegister(CMD_OFFSET_WRITE, 2 << 10); // evita overrage
}


void loop() {
    u32 conversao = cell.readAverage(10, WORD_RATE_100SPS);
    Serial.println(conversao);
}


