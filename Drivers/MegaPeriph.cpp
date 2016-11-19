#include "MegaPeriph.h"

//===========================================================================
void SPI_PowerControl(unsigned char ucPowerLevel)
{
  if (ucPowerLevel == plNormalPower)
  {
    // MOSI, SCK, xSS как выходы
    SPI_DDR |= (1 << SPI_MOSI) | (1 << SPI_SCK) | (1 << SPI_SS);

    // MISO как вход
    SPI_DDR &= ~(1 << SPI_MISO);

    // Удвоенная скорость = Fosc/2
    SPSR |= (1 << SPI2X);
    // Мастер, Разрешить SPI
    SPCR = (1 << SPE) | (1 << MSTR);
  }
  else if (ucPowerLevel == plLowPower)
  {
    // Удвоенная скорость = Fosc/2 - Выкл
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1) | (1 << SPR0);
    SPSR &= ~(1 << SPI2X);
  }
  else
  {
    SPSR = 0x00;
    SPCR = 0x00;
    SPI_DDR &= ~((1 << SPI_MOSI) | (1 << SPI_SCK) | (1 << SPI_SS));
    SPI_PORT &= ~((1 << SPI_MOSI) | (1 << SPI_SCK) | (1 << SPI_SS));
  }
}

//===========================================================================
void SPI_WriteByte(unsigned char ucByte)
{
  SPDR = ucByte;
  while(!(SPSR & (1 << SPIF)));
}

//===========================================================================
unsigned char SPI_ReadByte()
{
  SPDR = 0xFF;
  while(!(SPSR & (1 << SPIF)));
  return SPDR;
}

//===========================================================================
void SPI_ByteIO(unsigned char* ucByte)
{
  SPDR = *ucByte;
  while(!(SPSR & (1 << SPIF)));
  *ucByte = SPDR;
}

//===========================================================================
void SPI_BufferIO(unsigned char* ucBuffer, unsigned int uiSize)
{
  for(unsigned int i = 0; i < uiSize; i++)
  {
    SPDR = ucBuffer[i];
    while(!(SPSR & (1 << SPIF)));
    ucBuffer[i] = SPDR;
  }
}

//===========================================================================
void SPI_WriteBuffer(const unsigned char* cucBuffer, unsigned int uiSize)
{
  for(unsigned int i = 0; i < uiSize; i++)
  {
    SPDR = cucBuffer[i];
    while(!(SPSR & (1 << SPIF)));
  }
}

//===========================================================================
void SPI_ReadBuffer(unsigned char* ucBuffer, unsigned int uiSize)
{
  for(unsigned int i = 0; i < uiSize; i++)
  {
    SPDR = 0xFF;
    while(!(SPSR & (1 << SPIF)));
    ucBuffer[i] = SPDR;
  }
}

//===========================================================================
void SPI_SendConst(const unsigned char cucNumber, unsigned int uiCount)
{
  for(unsigned int i = 0; i < uiCount; i++)
  {
    SPDR = cucNumber;
    while(!(SPSR & (1 << SPIF)));
  }
}
