#include <string.h>
#include <avr/pgmspace.h>
#include <SPI.h>
#include "CS5530.h"
#include "Arduino.h"
#include <stdio.h>
//#include <intrin.h> //Library from _nop operation. Refer to "https://docs.microsoft.com/pt-br/cpp/intrinsics/nop?view=msvc-170"

CS5530::CS5530():
_spiSettings(CS5530_DEFAULT_SPI_FREQUENCY, MSBFIRST, SPI_MODE0),
_spi(&CS5530_DEFAULT_SPI), _ss(CS5530_DEFAULT_SS_PIN)

{
  _spi->begin();
}

int CS5530::begin()
{
  // setup pins
  pinMode(_ss, OUTPUT);

   digitalWrite(_ss, LOW);
   
   
  // set SS high
  digitalWrite(_ss, HIGH);

  // start SPI
  _spi->begin();

  return 1;
}

void CS5530::setPin(int ss)
{
  _ss = ss;
}

void CS5530::setSPI(SPIClass& spi)
{
  _spi = &spi;
}

void CS5530::setSPIFrequency(u32 frequency)
{
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
    int i;
    u32 tmp;
    
    //Initilizing SPI port (Cont 15 clock cycles at leats), writing 1111 1111 (0XFF) at config command register
    for(i=0;i<20;i++) {
      writeChar(CMD_SYNC1);
    }
     
    // Then, write 1111 1111 (0xFE) at SYNC0 register;
    writeChar(CMD_SYNC0);
    // VALIDED BY ANALYSER
    
    // Reseting CS5530
    writeRegister(CMD_CONFIG_WRITE,  REG_CONFIG_RS);    
    // Write Logical Level 1 on bit adressed at 29th position on Configuration Register at CS5530;

    //A função __nop é equivalente à instrução do computador NOP
            void __nop();                     


    //delayMicroseconds(1);  
    /* //At these setup, Wait 1 milli seconds// (Calculate these Delay according to selected clock of SPI.) If 2Mhz == 1 cycle == 0,0005 ms == 0,5us == 500ns.
    According to Arduino docummentation, is better to use function "millis()", due it do not stall controller during operation, as delay do.
    Refer to "https://www.arduino.cc/reference/pt/language/functions/time/millis/" */

    writeRegister(CMD_CONFIG_WRITE, CMD_NULL);
    //delayMicroseconds(1);
    void __nop();
    
    tmp = readRegister(CMD_CONFIG_READ); 
    //delayMicroseconds(1);
    void __nop();

    if(tmp & REG_CONFIG_RV) {
     return true;
    }

  return false;
}
/*

What is expected to:
Completing the reset cycle, initializes the on-chip registers to the following states:

Configuration Register:     00000000(H) (0000 0000 0000 0000 0000 0000 0000 0000 BIN)
Offset Register:            00000000(H) (0000 0000 0000 0000 0000 0000 0000 0000 BIN)
Gain Register               01000000(H) (0001 0000 0000 0000 0000 0000 0000 0000 BIN)

Esta Operação Está validada.
*/

// Funções Operacionais:

void CS5530::writeRegister (u8 reg, u32 dat) {
 
    writeChar(reg);
    writeLong(dat);
}

void CS5530::setBit (u8 reg, u32 dat) {
    u32 tmp = 0;
    u8 cmd = 0;
    switch (reg)
    {
        case CMD_GAIN_WRITE:   cmd = CMD_GAIN_READ; break; 
        case CMD_OFFSET_WRITE: cmd = CMD_OFFSET_READ; break;		
        case CMD_CONFIG_WRITE: cmd = CMD_CONFIG_READ; break; 
    }

    tmp =  readRegister(cmd);
    tmp |= dat;
    writeChar(reg);
    writeLong(tmp);
}

void CS5530::resetBit (u8 reg, u32 dat) {
     u32 tmp = 0;
     u8 cmd = 0;
    switch (reg)
    {
        case CMD_GAIN_WRITE:   cmd = CMD_GAIN_READ; break; 
        case CMD_OFFSET_WRITE: cmd = CMD_OFFSET_READ; break;		
        case CMD_CONFIG_WRITE: cmd = CMD_CONFIG_READ; break; 
    }

    tmp =  readRegister(cmd);
    tmp &= ~dat;
    writeChar(reg);
    writeLong(tmp);
}

void CS5530::writeChar (u8 dat) {
 
    digitalWrite(_ss, LOW);
    _spi->beginTransaction(_spiSettings);
    SPI.transfer(dat & 0xFF); 
    _spi->endTransaction();
    digitalWrite(_ss, HIGH);
}

void CS5530::writeLong (u32 dat) {
    int i;
    u8 tmp;

    for(i=3; i>=0; i--) {
        tmp = (u8)( (dat >> (8*i)) & 0xff);
        writeChar(tmp);
    }
}
 
u32 CS5530::readRegister (u8 reg) {
    u32 dat;
    
    writeChar(reg);
    dat = readLong();
    
    return dat;
}

u32 CS5530::readLong (void)      {
    int i;
    u32 dat=0; 
    u8 currntByte = 0;
   
    for(i=0; i<4; i++) {
        dat    <<=    8;
        dat    |= readChar();
    }

    return dat;
}

u8 CS5530::readChar (void)     {
    u8 dat=0;
      
    digitalWrite(_ss, LOW);
    _spi->beginTransaction(_spiSettings);
    dat = SPI.transfer(CMD_NULL);
    _spi->endTransaction();
    digitalWrite(_ss, HIGH);
      
    return dat;
}

bool CS5530::isReady (void) {
    if (digitalRead(51) == 1) {
        return true;
    }

    return false;
}

// Funções De Execução:

u32 CS5530::readWeightsclae () { //Leitura do valor do ADC
    u32 rec_data = 0;
    
    EAdStatus status;

   if(isReady()==false)
   {
       status = E_AD_STATUS_BUSY;
       Serial.println("Ocupado!");
       return -2;
   }
   else
   {    
       rec_data = readRegister(CMD_NULL);
       if((rec_data &  REG_DATA_OF) == 0)
       {
           rec_data &= 0xFFFFFF00;
           
           rec_data = rec_data >> 8;

           status =  E_AD_STATUS_READY;
           
           //Serial.println("Pronto!");
           return rec_data;
       }
       else
       {
             status =  E_AD_STATUS_OVERFLOW;
             Serial.println("Overflow");
             return -1;
       }
   }

   return status;
}

u8 CS5530::calibrate (u8 calibrate_type, int cfg_reg, int setup_reg) { //função calibração
    u32 calibrate_result;
    int waste_time, i;
    cfg_reg = (int)((calibrate_type % 2 == 1) ? (cfg_reg|REG_CONFIG_IS):(cfg_reg));
    u8 cmd,read_reg;

    writeRegister(CMD_CONFIG_WRITE, cfg_reg);
    writeChar(cmd);

    for(waste_time = WASTE_TIME; waste_time > 0; waste_time--);

    calibrate_result = readRegister(read_reg);

    /*Serial.print("Resultado da Calibracao: ");
    Serial.printf("%x ", calibrate_result);
    Serial.print("\n");*/

    return calibrate_result;
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

u8 CS5530::convert(u8 convert_type, u8 setup_reg_no, u8 reg_no, int word_rate) { // serve para conversao caso complemento de dois
    Serial.print("Preparando para a conversao...\n");
    u32 final_result = 0;
    int waste_time, i;
    u32 cfg_reg =  (u32)REG_CONFIG_VRS;
    int setup_reg = ((setup_reg_no % 2 == 1) ? ((word_rate) << 16) : word_rate);
    u8 cmd;

    switch (convert_type)
    {
        case SINGLE_CONVERSION: cmd = CMD_CONVERSION_SINGLE; break;
        case CONTINUED_CONVERSION: cmd = CMD_CONVERSION_CONTINU; break; 
    }

    writeRegister(CMD_CONFIG_WRITE, cfg_reg);
    void __nop();;//fazer if com nop;
    writeChar(cmd);
    Serial.print("Conversao Iniciada...\n");

    void __nop();
    u8 test = 0;
    test = readChar();   // wastercycles
    final_result = readLong();


    Serial.print("Resultado Bruto:"); Serial.println(final_result);
    //Serial.print("Resultado Bruto:"); Serial.println(final_result);

    final_result = twoComplement(final_result);
    Serial.print("Resultado Final:"); Serial.println(final_result);
    //Serial.print("Resultado Final:"); Serial.println(final_result);

    final_result = final_result * 500 / 0x7fffff;
    Serial.print("Resultado Final:"); Serial.println(final_result);

    return 1;
}

u32 CS5530::singleConversion() {

    union {
        char buffer [4];
        u32 info;
     } conversao2;

    digitalWrite(_ss, LOW);

    _spi->beginTransaction(_spiSettings);
    _spi->transfer(CMD_CONVERSION_SINGLE);

    while (digitalRead(50) == 1) { // wait for conversion 
    }

    _spi->transfer(0x00); // clear sdo flag

    for (int i = 0; i < 4; i++) {
        u8 data_transf = _spi->transfer(0x00);
        conversao2.buffer[3 - i] = data_transf;
    }

    _spi->endTransaction();
    digitalWrite(_ss, HIGH);


    Serial.println(conversao2.info);

    return 0;
}


/*

Operação: 
1° Sincronizar o SPI
2° Resetar o registrador de comando
3° Definir os parametros de operação; Ex: Bipolar, continua, Complemento de dois, etc.
4° Calibrar;
5° verificar o resultado da calibração;
6° Iniciar a conversão;
7° ler o resultado da conversão.
8° plotar o resultado da conversão

*/