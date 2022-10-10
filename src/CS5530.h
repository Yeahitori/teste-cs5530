#ifndef CS5530_H
#define CS5530_H
#include <SPI.h>

/*
=============================================================
                        Timing Table
=============================================================
                           Start-up

Oscillator Start-up Time XTAL = 4.9152 MHz
=============================================================
                     Serial Port Timing

Serial Clock Frequency SCLK               0  -  2 MHz
=============================================================
                        Serial Clock

Pulse Width High                          t1    250ns
Pulse Width Low                           t2    250ns
=============================================================
                     SDI Write Timing

CS Enable to Valid Latch Clock            t3    50 ns
Data Set-up Time prior to SCLK rising     t4    50 ns
Data Hold Time After SCLK Rising          t5    100 ns
SCLK Falling Prior to CS Disable          t6    100 ns
=============================================================
                     SDO Read Timing

CS to Data Valid                          t7    150 ns (MAX)
SCLK Falling to New Data Bit              t8    150 ns (MAX)
CS Rising to SDO Hi-Z                     t9    150 ns (MAX)
=============================================================

*Refer to DS742F3, page 9;

*/

//Espelhamento de dados

#define i32 long int
#define i16 int
#define i8  char
#define u32 unsigned long int
#define u16 unsigned int
#define u8  unsigned char

//===================================================================================================================================================//

#define RESET_TIME               1200000       // Tempo de Reset - 18 ciclos ao menos (verificar o tempo de cada ciclo em acordo com o clock setado);
#define WASTE_TIME               90000000      // Tempo para a conversão (40 ciclos de clock) ?

//===================================================================================================================================================//

#define SELF_OFFSET              1              // Manda 0000 1001
#define SELF_GAIN                2              // Manda 0000 1001
#define SYSTEM_OFFSET            3              // Manda 0000 1001
#define SYSTEM_GAIN              4              // Manda 0000 1001

#define SINGLE_CONVERSION        1              // Manda 0000 1001
#define CONTINUED_CONVERSION     2              // Manda 0000 1001

//===================================================================================================================================================//

//Espelho para resultados de operações

#define PASS                     0x00           // Manda 0000 0000
#define FAIL                     0xFF           // Manda 1111 1111
#define TRUE                     1              // Manda 1
#define FALSE                    0              // Manda 0

//===================================================================================================================================================//

#define NOTHING                  0              // Manda 0

#define CS5530_READ_DATA_TIMEOUT 20000          // Tempo Limite para Ler dados (20s)

#define TIMEOUTERR               0xFF           // Manda 1111 1111
 
#define SUCCESSFUL               0x00           // Manda 0000 0000

#define CMD_STOP_CONT_CONV       0xFF           // Manda 1111 1111

#define CAL_TIMEOUT_LIMIT        4000           // Tempo Limite para Calibrar (4s)

#define READ_CONV_RESULT         0x00           // Manda 0000 0000

//===================================================================================================================================================//

#define DATA_VALID               0              // Caso os dados sejam validos, responde o booleano 0;
 
#define ERR_AD_BUSY              1              // Conversor ocupado, retorna 1;
 
#define ERR_AD_OVER_FLOW         2              // Overflow (estoura de pilha), retorna 2;
 
#define ERR_FILTER_ONGOING       3              // Filtro em andamento retorna 3;

//===================================================================================================================================================//
// *2.3.10 page 19* Configuration Register Descriptions Where 1UL is an unsigned long int, with a value of 1 represented at the bit level as:
// 00000000000000000000000000000001. Is possible to move 0's by the same process, declarating 0UL;
//===================================================================================================================================================//

#define REG_CONFIG_PSS           1UL << 31     //Power Save Select         0 Standby Mode (Oscillator active, allows quick power-up). 1 Sleep Mode (Oscillator inactive).
#define REG_CONFIG_PDM           1UL << 30     //Power Down Mode           0 Normal Mode.       1 Activate the power save select mode.
#define REG_CONFIG_RS            1UL << 29     //Reset System              0 Normal Operation.  1 Activate a Reset cycle. See System Reset Sequence in the datasheet text.
#define REG_CONFIG_RV            1UL << 28     //Reset Valid               0 Normal Operation.  1 System was reset. This bit is read only. Bit is cleared to logic zero after the configuration register is read.
#define REG_CONFIG_IS            1UL << 27     //Input Short               0 Normal Input.      1 All signal input pairs for each channel are disconnected from the pins and shorted internally.
#define REG_CONFIG_VRS           0UL << 25     //Voltage Reference Select  0 2.5 V < VREF ≤ [(VA+) - (VA-)]. 1 1 V ≤ VREF ≤ 2.5V
#define REG_CONFIG_A1            1UL << 24     //Output Latch bit          00 A1 = 0, A0 = 0 // 10 A1 = 1, A0 = 0
#define REG_CONFIG_A0            1UL << 23     //Output Latch bit          01 A1 = 0, A0 = 1 // 11 A1 = 1, A0 = 1
#define REG_CONFIG_FRS           1UL << 19     //Filter Rate Select        0 Use the default output word rates. 1 Scale all output word rates and their corresponding filter characteristics by a factor of 5/6.
#define REG_CONFIG_OCD           1UL << 9      //Open Circuit Detect Bit   0 Normal mode. 1 Activate current source (Used for thermocouple).
#define REG_CONFIG_UNIPOLAR          1UL << 10     // Manda 1 e desloca 10 para a esquerda. 1 Select Unipolar mode.
#define REG_CONFIG_BIPOLAR           0UL << 10     // Manda 0 e desloca 10 para a esquerda. 0 Select Bipolar mode.
#define REG_DATA_OF              1UL << 3      // Manda 1 e desloca 03 posicoes para a esquerda

//===================================================================================================================================================//

//CS5532 Gain settings

#define GAINX1                   1UL  << 24    // Gain at 1x
#define GAINX2                   2UL  << 24    // Gain at 2x
#define GAINX4                   4UL  << 24    // Gain at 4x
#define GAINX8                   8UL  << 24    // Gain at 8x
#define GAINX16                  16UL << 24    // Gain at 16x
#define GAINX32                  32UL << 24    // Gain at 32x
#define GAINX64                  64UL << 24    // Gain at 64x

//===================================================================================================================================================//

//Word Rate when FSR=1

#define WORD_RATE_100SPS         0x0 << 11     // Manda 0000 e desloca 11 posicoes para a esquerda
#define WORD_RATE_50SPS          0x1 << 11     // Manda 0001 e desloca 11 posicoes para a esquerda
#define WORD_RATE_25SPS          0x2 << 11     // Manda 0010 e desloca 11 posicoes para a esquerda
#define WORD_RATE_12P5SPS        0x3 << 11     // Manda 0011 e desloca 11 posicoes para a esquerda
#define WORD_RATE_6P25SPS        0x4 << 11     // Manda 0100 e desloca 11 posicoes para a esquerda
#define WORD_RATE_3200SPS        0x8 << 11     // Manda 1000 e desloca 11 posicoes para a esquerda
#define WORD_RATE_1600SPS        0x9 << 11     // Manda 1001 e desloca 11 posicoes para a esquerda
#define WORD_RATE_800SPS         0xA << 11     // Manda 1010 e desloca 11 posicoes para a esquerda
#define WORD_RATE_400SPS         0xB << 11     // Manda 1011 e desloca 11 posicoes para a esquerda
#define WORD_RATE_200SPS         0xC << 11     // Manda 1100 e desloca 11 posicoes para a esquerda

//===================================================================================================================================================//
/*

The listed Word Rates are for continuous conversion mode using a 4.9152 MHz clock. All word rates will
scale linearly with the clock frequency used. The very first conversion using continuous conversion mode
will last longer, as will conversions done with the single conversion mode. See the section on Performing
Conversions and Tables 1 and 2 for more details.

Word Rate Table

Bit      WR(FRS = 0) WR(FRS = 1)

0000     120 Sps     100 Sps
0001     60 Sps      50 Sps
0010     30 Sps      25 Sps
0011     15 Sps      12.5 Sps
0100     7.5 Sps     6.25 Sps
1000     3840 Sps    3200 Sps
1001     1920 Sps    1600 Sps
1010     960 Sps     800 Sps
1011     480 Sps     400 Sps
1100     240 Sps     200 Sps
*/
//===================================================================================================================================================//

//Unipolar / Bipolar

//===================================================================================================================================================//

#define CMD_STOP_CONT_CONV       0xFF          // Manda 1111 1111

//===================================================================================================================================================//

#define CS5530_DEFAULT_SPI           SPI           // Define que o SPI é o SPI da biblioteca arduino SPI.
#define CS5530_DEFAULT_SPI_FREQUENCY 20000000      //(1E8480 == 2MHz) (Original 8E6 == 2278 (Hz?) )
#define CS5530_DEFAULT_SS_PIN        53            // Pino CS do arduino MEGA 2560.

/* 
No arduino MEGA 2560, os pinos SPI são definidos como:

    CS/SS == 53
    SCK   == 52
    SDO   == 51
    SDI   == 50f 
    
*/

//===================================================================================================================================================//

//Command Register (CS5530_COMM_REG)

#define CMD_OFFSET_READ         0x09 // Manda 0000 1001
#define CMD_OFFSET_WRITE        0x01 // Manda 0000 0001
#define CMD_GAIN_READ           0x0A // Manda 0000 1010
#define CMD_GAIN_WRITE          0x02 // Manda 0000 0010
#define CMD_CONFIG_READ         0x0B // Manda 0000 1011
#define CMD_CONFIG_WRITE        0x03 // Manda 0000 0011

#define CMD_CONVERSION_SINGLE    0x80 // Manda 1000 0000
#define CMD_CONVERSION_CONTINU  0xC0 // Manda 1100 0000
#define CMD_SYS_OFFSET_CALI     0x85 // Manda 1000 0101
#define CMD_SYS_GAIN_CALI       0x86 // Manda 1000 0110
#define CMD_SYNC1               0xFF // Manda 1111 1111   //Function: Part of the serial port re-initialization sequence.
#define CMD_SYNC0               0xFE // Manda 1111 1110   //Function: End of the serial port re-initialization sequence.
#define CMD_NULL                0x00 // Manda 0000 0000   //Function: This command is used to clear a port flag and keep the converter in the continuous conversion mode.

//===================================================================================================================================================//
//Sinalização de status do conversor;

    enum EAdStatus {
       E_AD_STATUS_BUSY,
       E_AD_STATUS_READY,
       E_AD_STATUS_OVERFLOW
    };

//===================================================================================================================================================//

constexpr char OVER_RANGE_FLAG  {0x04};
class CS5530 {
public:
    CS5530();
    int begin();                                 // Inicia o CS5530
    void setPin (int ss = CS5530_DEFAULT_SS_PIN); // Função SETPIN do Controlador, caso necessário usar para definir o Pino em outro Arduino
    void setSPI (SPIClass &spi);                  // Inicia o SPI do controlador
    void setSPIFrequency (u32);                   // Define a Frequencia do SPI (CS5530 suporta no maximo 2Mhz);
    void setOffset (u32 offset);
    void setGain (u32 gain);
    void setConfigurationRegister (u32 config_register);
    u32 twoComplement (u32);                      // Define como Complemento de 2 a conversão
    u8 readChar (void);                           // Inicia a Leitura de um vetor de caracteres (registrador) como char
    u32 readLong (void);                          // Inicia a Leitura de um vetor de inteiros (registrador) como long
    void writeLong (u32);                         // Define a escrita de uma cadeia de inteiros do tipo long, 32 bits
    void writeChar (u8);                          // Escreve uma cadeia de inteiros de 8 bits
    bool isReady (void);                          // Define que o conversor está pronto
    bool reset (void);                            // Função de RESET
    u32 singleConversion();
    u32 readAverage (int n_conversions = 1, u32 rate = WORD_RATE_100SPS);
    u32 gain (); // getter
    u32 offset (); // getter 
    u32 configurationRegister (); // getter
    //===================================================================================================================================================//
private:
    SPISettings _spiSettings;
    SPIClass *_spi;
    int _ss;
};

//===================================================================================================================================================//

#endif




