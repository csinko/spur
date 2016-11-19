#include "mmc.h"
#include "Utils.h"

#ifndef NULL
#define NULL 0
#endif //#ifndef NULL

//=================================================================================
// MMC card commands
#define MMC_COMMANDS_BASE       (unsigned char) 0x40
#define MMC_GO_IDLE_STATE       MMC_COMMANDS_BASE + 0
#define MMC_SEND_OP_COND        MMC_COMMANDS_BASE + 1
#define MMC_SEND_CID            MMC_COMMANDS_BASE + 10
#define MMC_SET_BLOCKLEN        MMC_COMMANDS_BASE + 16
#define MMC_READ_SINGLE_BLOCK   MMC_COMMANDS_BASE + 17
#define MMC_WRITE_SINGLE_BLOCK   MMC_COMMANDS_BASE + 24

//=================================================================================
// MMC data tokens
#define MMC_START_TOKEN_SINGLE (unsigned char) 0xFE


#define MMC_ACTIVATE() (_MMC_CS_PORT &= ~(1 << _MMC_CS_NUM))
#define MMC_DEACTIVATE() (_MMC_CS_PORT |= 1 << _MMC_CS_NUM)

#define MMC_BLOCK_SIZE  (unsigned int) 512

CCardStatus m_CardStatus = ecsNOCARD;

//=================================================================================
// Forward declaration
void MMC_SendCommand(unsigned char uc_Command, unsigned long uc_Parametr);
void MMC_Finish();
CCardR1 MMC_GetR1();

//=================================================================================
void MMC_PowerUp()
{
  _MMC_CS_DDR |= 1 << _MMC_CS_NUM;
  _MMC_CS_PORT |= 1 << _MMC_CS_NUM;
//  Delay_us(5000);
}

//=================================================================================
void MMC_PowerDown()
{
  _MMC_CS_PORT |= 1 << _MMC_CS_NUM;
}

//=================================================================================
void MMC_PowerControl(unsigned char ucPowerLevel)
{
  CCardR1 tempR1;
  unsigned int uiMaxErrorsCMD = 0x00FF;

  switch (ucPowerLevel)
  {
  case plNormalPower:

    MMC_PowerUp();

    SPI_SendConst(0xFF, 10);

    m_CardStatus = ecsNOCARD;

    MMC_ACTIVATE();

    do
    {
      MMC_SendCommand(MMC_GO_IDLE_STATE, (unsigned long) 0x00);
      tempR1 = MMC_GetR1();
    }while((tempR1.ucData != 0x01)&&(uiMaxErrorsCMD--));

    MMC_DEACTIVATE();
    SPI_SendConst(0xFF, 10);

    MMC_ACTIVATE();

    if (tempR1.ucData == 0x01)
    {
      m_CardStatus = ecsERROR;
      uiMaxErrorsCMD = 0x7FF;
      do
      {
        MMC_ACTIVATE();
        MMC_SendCommand(MMC_SEND_OP_COND, (unsigned long) 0x00);
        tempR1 = MMC_GetR1();
        MMC_DEACTIVATE();
        SPI_SendConst(0xFF, 10);
      }while ((tempR1.ucData != 0x00)&&(uiMaxErrorsCMD--));

      if (tempR1.ucData == 0x00)
      {
        m_CardStatus = ecsOK;
      }
    }

    MMC_Finish();

    if (m_CardStatus != ecsOK)
    {
      MMC_PowerDown();
    }
    else
    {
      MMC_ACTIVATE();
      MMC_SendCommand(MMC_SET_BLOCKLEN, MMC_BLOCK_SIZE);
      MMC_GetR1();
      MMC_Finish();
    }
    break;

  case plLowPower:
  case plMinimalPower:
    MMC_PowerDown();
    break;
  }
}

//=================================================================================
void MMC_SendCommand(unsigned char ucCommand, unsigned long ulParam)
{
  SPI_WriteByte(ucCommand);
  SPI_WriteByte((unsigned char) (ulParam>>24));
  SPI_WriteByte((unsigned char) (ulParam>>16));
  SPI_WriteByte((unsigned char) (ulParam>>8));
  SPI_WriteByte((unsigned char) (ulParam));
  SPI_WriteByte(0x95);
}

//=================================================================================
CCardR1 MMC_GetR1()
{
  CCardR1 tempR1;
  unsigned char uc_MaxErrors = 0xFF;
  do
  {
    tempR1.ucData = SPI_ReadByte();
  }while((uc_MaxErrors--)&&(tempR1.bits.bitAlwaysZero == 1));

  return tempR1;
}

//=================================================================================
void MMC_Finish()
{
  SPI_WriteByte(0xFF);
  SPI_WriteByte(0xFF);
  MMC_DEACTIVATE();
  SPI_WriteByte(0xFF);
}

//=================================================================================
CCardStatus MMC_GetStatus()
{
  return m_CardStatus;
}

//=================================================================================
CCardStatus MMC_WaitDataToken()
{
  unsigned char uc_Answer;
  unsigned char uc_MaxErrors = 0xFF;

  do
  {
    uc_Answer = SPI_ReadByte();
  }while((uc_MaxErrors--)&&(uc_Answer != MMC_START_TOKEN_SINGLE));

  if (uc_Answer != MMC_START_TOKEN_SINGLE)
  {
    return ecsERROR;
  }
  else
  {
    return ecsOK;
  }
}

//=================================================================================
void MMC_ReadBlock(unsigned long ul_Address, unsigned char* uc_Buffer)
{
  if (m_CardStatus != ecsOK)
  {
    return;
  }

  MMC_ACTIVATE();
  MMC_SendCommand(MMC_READ_SINGLE_BLOCK, ul_Address);

  if (MMC_GetR1().ucData == 0x00)
  {
    MMC_WaitDataToken();
    SPI_ReadBuffer(uc_Buffer, MMC_BLOCK_SIZE);
  }
  else
  {
    m_CardStatus = ecsERROR;
  }

  MMC_Finish();
}

//=================================================================================
void MMC_WriteBlock(unsigned long ul_Address, unsigned char* uc_Buffer)
{
  if (m_CardStatus != ecsOK)
  {
    return;
  }

  MMC_ACTIVATE();
  MMC_SendCommand(MMC_WRITE_SINGLE_BLOCK, ul_Address);

  if (MMC_GetR1().ucData == 0x00)
  {
    SPI_WriteByte(0xFF);
    SPI_WriteByte(0xFF);
    SPI_WriteByte(MMC_START_TOKEN_SINGLE);
    
    SPI_WriteBuffer(uc_Buffer, MMC_BLOCK_SIZE);
    
    SPI_WriteByte(0xFF);
    SPI_WriteByte(0xFF);
    SPI_ReadByte();
    while(SPI_ReadByte() == 0x00);
  }
  else
  {
    m_CardStatus = ecsERROR;
  }

  MMC_Finish();
}
