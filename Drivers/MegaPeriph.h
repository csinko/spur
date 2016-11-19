#ifndef _MEGA_PERIPHERAL_H_
#define _MEGA_PERIPHERAL_H_

#include <ioavr.h>
#include <intrinsics.h>
#include "Driver.h"

//===========================================================================
//===========================================================================
// SPI module functions                                                    //
//===========================================================================
#define SPI_PORT      PORTB
#define SPI_DDR       DDRB
#define SPI_SS        DDB2
#define SPI_MOSI      DDB3
#define SPI_MISO      DDB4
#define SPI_SCK       DDB5

// Bytes ios
void SPI_WriteByte(unsigned char ucByte);
unsigned char SPI_ReadByte(void);
void SPI_ByteIO(unsigned char* ucByte);

// Buffers ios
void SPI_WriteBuffer(const unsigned char* cucBuffer, unsigned int uiSize);
void SPI_BufferIO(unsigned char* ucBuffer, unsigned int uiSize);
void SPI_ReadBuffer(unsigned char* ucBuffer, unsigned int uiSize);

//Consts ios
void SPI_SendConst(const unsigned char cucNumber, unsigned int uiCount);

// SPI power control
void SPI_PowerControl(unsigned char ucPowerLevel);

//===========================================================================
//===========================================================================
// TWI module functions                                                    //
//===========================================================================

//===========================================================================
//  Раскоментируйте TWI_USE_INTERNAL_PULLUP для использования внутренних
// подтягивающих резисторов.
#define TWI_USE_INTERNAL_PULLUP

#ifdef TWI_USE_INTERNAL_PULLUP
# define TWI_PORT     PORTD
# define TWI_DDR      DDRD
# define TWI_SCL_NUM  0
# define TWI_SDA_NUM  1
#endif  //TWI_USE_INTERNAL_PULLUP

//===========================================================================
//                                  I2C                                    //
//===========================================================================
// Master mode only now
// I2C clock rate in kHz
#define TWI_SCL_FREQUENCY   100
// I2C prescaler value
#define TWI_TWPS_VALUE   0
// Calculate TWBR registry value for current TWI_SCL_FREQUENCY
#define TWI_TWBR_VALUE   ((X_SYSTEM_CPU_FREQUENCY / TWI_SCL_FREQUENCY) - 16)/(2*(4^TWI_TWPS_VALUE))
// I2C functions
void TWI_PowerControl(unsigned char ucPowerLevel);
void TWI_ReadBuffer(unsigned char ucDevAddr, unsigned char* ucBuffer, unsigned char ucSize);
void TWI_WriteBuffer(unsigned char ucDevAddr, const unsigned char* ucBuffer, unsigned char ucSize);

//===========================================================================
//                              internal EEPROM                            //
//===========================================================================
// EEPROM functions
unsigned char EEPROM_ReadByte(unsigned int uiAddress);
void EEPROM_WriteByte(unsigned int uiAddress, unsigned char ucData);
void EEPROM_ReadBuffer(unsigned int uiAddress, unsigned char* ucBuffer, unsigned char ucSize);
void EEPROM_WriteBuffer(unsigned int  uiAddress, const unsigned char* ucBuffer, unsigned char ucSize);


//===========================================================================
//                                USART                                    //
//===========================================================================

// define this for increase USART baudrate with lower CPU clock
//#define USART_DOUBLE_SPEED_MODE

// Defines some most recent USART speed
#define BAUD_RATE_2400    2400
#define BAUD_RATE_4800    4800
#define BAUD_RATE_9600    9600
#define BAUD_RATE_14400   14400
#define BAUD_RATE_19200   19200
#define BAUD_RATE_28800   28800
#define BAUD_RATE_38400   38400
#define BAUD_RATE_57600   57600
#define BAUD_RATE_76800   76800
#define BAUD_RATE_115200  115200
#define BAUD_RATE_230400  230400
#define BAUD_RATE_250000  250000
#define BAUD_RATE_500000  500000
#define BAUD_RATE_1000000 1000000

// Calculate maximum USART baudrate for current X_SYSTEM_CPU_FREQUENCY
#ifdef USART_DOUBLE_SPEED_MODE
# define BAUD_RATE_MAX     ((unsigned long)X_SYSTEM_CPU_FREQUENCY * 1000)/8
#else
# define BAUD_RATE_MAX     ((unsigned long)X_SYSTEM_CPU_FREQUENCY * 1000)/16
#endif //#ifdef USART_DOUBLE_SPEED_MODE

// USART functions
void USART_Initialize(unsigned long ulBaudRate);
void USART_TransmitByte(unsigned char ucData);
void USART_TransmitBuffer(unsigned char* ucData, unsigned int uiSize);
unsigned char USART_ReceiveByte(void);
void USART_ReceiveBuffer(unsigned char* ucBuffer, unsigned char ucSize);
unsigned char USART_ReceiveByteByCondition(unsigned char ucCondition);
void USART_ReceiveBufferByCondition(unsigned char* ucBuffer, unsigned char ucSize, unsigned char ucCondition);
void USART_ShutDown(void);

//===========================================================================
//                                WATCHDOG                                 //
//===========================================================================
void WDT_PowerControl(unsigned char ucPowerLevel);
inline void WDT_Reset(void){__watchdog_reset();};

//===========================================================================
//                                Internal OSC                             //
//===========================================================================
void OSC_SetDefaultValue();

#endif //_MEGA_PERIPHERAL_H_
