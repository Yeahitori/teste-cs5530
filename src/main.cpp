// CS5530 driver
//
//======================================================================================//
// Inclusão bibliotecas

#include "Arduino.h"
#include <avr/pgmspace.h>
#include "CS5530.h"

//======================================================================================//
// Definição de Pinout do arduino.

#define CIPO 51
#define COPI 50
#define SCK 52
#define CS 53
/*
No arduino MEGA 2560, os pinos SPI são definidos como:

    CS/SS == 53
    SCK   == 52
    SDO   == 51
    SDI   == 50

*/
//======================================================================================//
CS5530 cell;

u32 startTime;
i32 value;

void setup () {
    Serial.begin(2000000);

    cell.setSPIFrequency(2000000); // changing to 4MHz
    cell.begin();

    if (cell.reset())
        Serial.println("CS5530 - RESET COMPLETADO COM SUCESSO!!!\n");
    else
        Serial.println("ERRO AO RESETAR CS5530\n");

    // Escreve no registro o codigo 0000 0011
    //   cell.CS5530_Write_Reg(CMD_GAIN_WRITE, 0x3);

    u32 ConfigReg = cell.readRegister(CMD_CONFIG_READ);
    Serial.print("CONFIG Register:\n");
    Serial.println(ConfigReg, BIN);

    // Função de teste, impressao de todos os registradores.
    // ===============================================================================================================================

    u32 ConfigReg_2 = cell.readRegister(CMD_CONFIG_READ);
    Serial.print("Antes Calibrar CONFIG Register:\n");
    Serial.println(ConfigReg_2, HEX);

    u32 offset = cell.readRegister(CMD_OFFSET_READ);
    Serial.print("Antes Calibrar OFFSET Register:\n");
    Serial.println(offset, HEX);

    u32 gain = cell.readRegister(CMD_GAIN_READ);
    Serial.print("Antes Calibrar GAIN Register:\n");
    Serial.println(gain, HEX);

    // TESTADO - OK 19/09/22

    // Para calibrar, enviar o comando com MSB = 1, e os bits de comando do WordRate para selecionar o tipo de calibração WORD_RATE_100SPS
    // Setar o Filter Rate (FSR Reg para 1) REG_CONFIG_FRS
    // Setar o sinal para Unipolar ou Bipolar. CS5530_UNIPOLAR // CS5530_BIPOLAR;
    //
    //
    cell.writeRegister(CMD_CONFIG_WRITE, 0x84800); // 00000000000010000100100000000000
    u32 Uni = cell.readRegister(CMD_CONFIG_READ);
    Serial.print("\n");
    Serial.print("Após Setar Unipolar, o 11 bit deve estar como 1: exemplo 0000 0000 0000 0000 0000 1000 0000 0000\n");
    Serial.println(Uni, BIN);
    Serial.print("\n");

    /* cell.writeRegister(CMD_CONFIG_WRITE, WORD_RATE_3200SPS);
     u32 WD = cell.readRegister(CMD_CONFIG_READ);
     Serial.print("\n");
     Serial.print("Após Setar WordRate, o bit (Manda 1000 e desloca 11 posicoes para a esquerda): \n");
     Serial.println(WD, BIN);Serial.print("\n");

     cell.calibrate(WORD_RATE_3200SPS, REG_CONFIG_IS, CMD_OFFSET_WRITE);
     u32 calibration = cell.readRegister(CMD_CONFIG_READ);
     Serial.print("\n");
     Serial.print("Resultado da Calibracao:\n");
     Serial.println(calibration, HEX);Serial.print("\n");


     u32 ConfigReg_3 = cell.readRegister(CMD_CONFIG_READ);
     Serial.print("\n");
     Serial.print("Apos Calibrar CONFIG Register:\n");
     Serial.println(ConfigReg_3, HEX);Serial.print("\n");

     u32 offset1 = cell.readRegister(CMD_OFFSET_READ);
     Serial.print("\n");
     Serial.print("Apos Calibrar OFFSET Register:\n");
     Serial.println(offset1, HEX);Serial.print("\n");

     u32 gain1 = cell.readRegister(CMD_GAIN_READ);
     Serial.print("\n");
     Serial.print("Apos Calibrar GAIN Register:\n");
     Serial.println(gain1, HEX);Serial.print("\n");

   */
    // TESTE CONVERSÃO:
    u32 complemento_de_dois = cell.twoComplement(0x7FFFFF);
    Serial.print("\n");
    Serial.print("complemento:");
    Serial.println(complemento_de_dois);
    Serial.print("\n");

    u8 conversao = cell.convert(CONTINUED_CONVERSION, 2, 1, (int)WORD_RATE_100SPS);
    Serial.print("\n");
    Serial.print("Resultado da Conversao:\n");
    Serial.println(conversao);
    Serial.print("\n");

    i32 leitura = cell.readWeightsclae();
    Serial.print("\n");
    Serial.println(String());
    Serial.println(leitura);
    Serial.print("\n");
    // ===============================================================================================================================
    /*
        //Calibração - calibrate(u8 calibrate_type, int cfg_reg, int setup_reg)

       u32 calibration = cell.calibrate(CMD_SYS_OFFSET_CALI, REG_CONFIG_VRS, CMD_OFFSET_WRITE);

        void __nop();
        void __nop();
        void __nop();

        Serial.print("calibrate_result:\n");
        Serial.println(calibration, HEX);



        //u32 tmpdata = REG_CONFIG_UNIPOLAR | REG

        cell.writeRegister(CMD_CONFIG_WRITE, CS5530_UNIPOLAR);


        cell.convert(CONTINUED_CONVERSION, 1, 1, (int)WORD_RATE_100SPS);

        u32 cmpl = cell.twoComplement(0xFFFFFFFF);


        cell.writeChar(CMD_CONVERSION_CONTINU);
        cell.writeRegister(CMD_OFFSET_WRITE, cmpl);

    */
}

void loop()
{


    // Função para imprimir tudo que há nos registradores.

    i32 recData = cell.readWeightsclae();
    Serial.println(recData);

    // Serial.println(String(value));

    if (recData > 0)
    {
        value = 0.97 * value + 0.03 * recData; // running average
        delay(1);
    }

    if (millis() > startTime)
    {
        Serial.println(String((value - 111683) / 18) + " grms");
        startTime = millis() + 200;
    }
}