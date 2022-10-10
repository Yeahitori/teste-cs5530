#include <string.h>
#include <avr/pgmspace.h>
#include <SPI.h>
#include "CS5530.h"
#include "Arduino.h"
#include <stdio.h>
#include <assert.h>
//#include <intrin.h> //Library from _nop operation. Refer to "https://docs.microsoft.com/pt-br/cpp/intrinsics/nop?view=msvc-170"

CS5530::CS5530():
_spiSettings(CS5530_DEFAULT_SPI_FREQUENCY, MSBFIRST, SPI_MODE0),
_spi(&CS5530_DEFAULT_SPI), _ss(CS5530_DEFAULT_SS_PIN)

{
  _spi->begin();
}

int CS5530::begin () {
    // setup pins
    pinMode(_ss, OUTPUT);

    digitalWrite(_ss, LOW);
   
   
    digitalWrite(_ss, HIGH);

    _spi->begin();

    return 1;
}

void CS5530::setPin (int ss) {
  _ss = ss;
}

void CS5530::setSPI (SPIClass& spi) {
  _spi = &spi;
}

void CS5530::setSPIFrequency (u32 frequency) {
  _spiSettings = SPISettings(frequency, MSBFIRST, SPI_MODE0);
}

/*

    System reset sequence as follows: 

    1 - Write a logic 1 into the RS bit of the configuration register. 
        This will reset the calibration registers and other logic (but not the serial port).

        NOTE THAT A valid reset will set the RV bit in the configuration register to a logic 1. 

    2 - After writing the RS bit to a logic 1, wait 8 master clock cycles(Controler, at these case, SPI Frequency). 
        Then write the RS bit back to logic 0.

        NOTE THAT the other bits in the configuration register cannot be written on this write cycle as they are being held in reset until RS is set back to logic 0. 
        While this involves writing an entire word into the configuration register to casue the RS bit to go to logic 0, the RV bit is a read only bit, 
        therefore a write to the configuration register will not overwrite the RV bit. 

    3 - After clearing the RS bit back to logic 0, 
        Read the Configuration Register to check the STATE of the RV bit as this indicates that a valid reset occurred.

        NOTE THAT Reading the configuration register, these action clears the RV bit back to logic 0.

*/
//Inicia o reset do CS5530 - Check OK
bool CS5530::reset(void) {
    digitalWrite(_ss, LOW);
    _spi->beginTransaction(_spiSettings);

    for(int i=0;i<15;i++) {
        _spi->transfer(CMD_SYNC1);
    }
    _spi->transfer(CMD_SYNC0);

    _spi->endTransaction();
    digitalWrite(_ss, HIGH);
    
    setConfigurationRegister(REG_CONFIG_RS);

    delay(1);

    if(configurationRegister() & REG_CONFIG_RV) {
     return true;
    }

  return false;
}



bool CS5530::isReady (void) {
    if (digitalRead(51) == 1) {
        return true;
    }

    return false;
}

u32 CS5530::twoComplement (u32 n) { //caso seja complemento de dois.
    u32 negative = (n & (1UL << 23)) != 0;
    u32 native_int;

    if (negative)
      native_int = n | ~((1UL << 24) - 1);
    else
      native_int = n;
    return native_int;
}

u32 CS5530::singleConversion() {
    union {
        char buffer [3];
        u32 info;
    } conversao;
    char flag_over_range;

    digitalWrite(_ss, LOW);

    _spi->beginTransaction(_spiSettings);
    _spi->transfer(CMD_CONVERSION_SINGLE);

    while (digitalRead(50) == 1) { // wait for conversion 
    }

    _spi->transfer(0x00); // clear sdo flag
    conversao.buffer[2] = _spi->transfer(0x00);
    conversao.buffer[1] = _spi->transfer(0x00);
    conversao.buffer[0] = _spi->transfer(0x00);
    flag_over_range = _spi->transfer(0x00);

    _spi->endTransaction();
    digitalWrite(_ss, HIGH);

    if (flag_over_range == OVER_RANGE_FLAG) {
        return 1;
    }
    else {
        return 0;
    }

    return conversao.info;
}

u32 CS5530::readAverage (int n_conversions, u32 rate) {
    assert(n_conversions > 0);
    union {
        u8 buff [3];
        u32 val;
    } conversao;
    u32 sum;

    u32 config_register = configurationRegister();
    config_register |= rate;

    setConfigurationRegister(config_register);


    digitalWrite(_ss, LOW);
    _spi->beginTransaction(_spiSettings);

    _spi->transfer(CMD_CONVERSION_CONTINU);

    for (int i = 0; i < n_conversions; i++) {
        u8 overrage_flag;
        while (digitalRead(50) == 1) {
        }

        _spi->transfer(0x00); // clear sdo flag
        conversao.buff[2] = _spi->transfer(0x00);
        conversao.buff[1] = _spi->transfer(0x00);
        conversao.buff[0] = _spi->transfer(0x00);
        overrage_flag = _spi->transfer(0x00);

        if (overrage_flag == 4) {
            Serial.print("OVER RANGE ");
            Serial.println(conversao.val);
        }
        else
            Serial.println(conversao.val);
    }

    while (digitalRead(50) == 1) {
    }
    
    // encerrando as conversoes
    _spi->transfer(0xFF);
    _spi->transfer16(0x0);
    _spi->transfer16(0x0);
    _spi->endTransaction();
    digitalWrite(_ss, HIGH);
    return 0;
}

void CS5530::setOffset (u32 offset) {
    union {
        char buffer [4];
        u32 valor;
    } temp;
    temp.valor = offset;

    digitalWrite(_ss, LOW);
    _spi->beginTransaction(_spiSettings);

    _spi->transfer(CMD_OFFSET_WRITE);
    _spi->transfer(temp.buffer[3]);
    _spi->transfer(temp.buffer[2]);
    _spi->transfer(temp.buffer[1]);
    _spi->transfer(temp.buffer[0]);

    _spi->endTransaction();
    digitalWrite(_ss, HIGH);
}

void CS5530::setGain (u32 gain) {
    union {
        char buffer [4];
        u32 val;
    } temp;
    temp.val = gain;

    digitalWrite(_ss, LOW);
    _spi->beginTransaction(_spiSettings);
     
    _spi->transfer(CMD_GAIN_WRITE);
    _spi->transfer(temp.buffer[3]);
    _spi->transfer(temp.buffer[2]);
    _spi->transfer(temp.buffer[1]);
    _spi->transfer(temp.buffer[0]);

    _spi->endTransaction();
    digitalWrite(_ss, HIGH);
}

void CS5530::setConfigurationRegister (u32 config_register) {
    union {
        char buffer [4];
        u32 val;
    } temp;
    temp.val = config_register;

    digitalWrite(_ss, LOW);
    _spi->beginTransaction(_spiSettings);
     
    _spi->transfer(CMD_CONFIG_WRITE);
    _spi->transfer(temp.buffer[3]);
    _spi->transfer(temp.buffer[2]);
    _spi->transfer(temp.buffer[1]);
    _spi->transfer(temp.buffer[0]);

    _spi->endTransaction();
    digitalWrite(_ss, HIGH);
}

u32 CS5530::gain () {
    union {
        char buffer [4];
        u32 val;
    } temp;

    digitalWrite(_ss, LOW);
    _spi->beginTransaction(_spiSettings);

    _spi->transfer(CMD_GAIN_READ);
    temp.buffer[3] = _spi->transfer(0x00);
    temp.buffer[2] = _spi->transfer(0x00);
    temp.buffer[1] = _spi->transfer(0x00);
    temp.buffer[0] = _spi->transfer(0x00);

    _spi->endTransaction();
    digitalWrite(_ss, HIGH);

    return temp.val;
}

u32 CS5530::offset () {
    union {
        char buffer [4];
        u32 val;
    } temp;

    digitalWrite(_ss, LOW);
    _spi->beginTransaction(_spiSettings);

    _spi->transfer(CMD_OFFSET_READ);
    temp.buffer[3] = _spi->transfer(0x00);
    temp.buffer[2] = _spi->transfer(0x00);
    temp.buffer[1] = _spi->transfer(0x00);
    temp.buffer[0] = _spi->transfer(0x00);

    _spi->endTransaction();
    digitalWrite(_ss, HIGH);

    return temp.val;
}

u32 CS5530::configurationRegister () {
    union {
        char buffer [4];
        u32 val;
    } temp;

    digitalWrite(_ss, LOW);
    _spi->beginTransaction(_spiSettings);

    _spi->transfer(CMD_CONFIG_READ);
    temp.buffer[3] = _spi->transfer(0x00);
    temp.buffer[2] = _spi->transfer(0x00);
    temp.buffer[1] = _spi->transfer(0x00);
    temp.buffer[0] = _spi->transfer(0x00);

    _spi->endTransaction();
    digitalWrite(_ss, HIGH);

    return temp.val;
}
