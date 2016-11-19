#ifndef _MMC_H_
#define _MMC_H_

#include "MegaPeriph.h"

//=================================================================================
// MCU to MMC _CS pin
#define _MMC_CS_DDR   DDRC
#define _MMC_CS_PORT  PORTC
#define _MMC_CS_NUM   0

//=================================================================================
// Card status
typedef enum
{
  ecsOK, ecsERROR, ecsNOCARD
} CCardStatus;

//=================================================================================
// Card answers
typedef union
{
  unsigned char ucData;
  struct
  {
    unsigned char
      bitIdleState      : 1,
      bitEraseReset     : 1,
      bitIllegalCmd     : 1,
      bitComCRCError    : 1,
      bitEraseSeqError  : 1,
      bitAddressError   : 1,
      bitParameterError : 1,
      bitAlwaysZero     : 1;
  } bits;
} CCardR1;

typedef union
{
  unsigned int ucData;
  struct
  {
    unsigned int
      bitCardIsLocked   : 1,
      bitWPEraseSkip    : 1,     // Lock/unlock cmd failed
      bitError          : 1,
      bitCCError        : 1,
      bitCardECCFailed  : 1,
      bitWPViolation    : 1,
      bitEraseParameter : 1,
      bitOutOfRange     : 1,

      bitIdleState      : 1,
      bitEraseReset     : 1,
      bitIllegalCmd     : 1,
      bitComCRCError    : 1,
      bitEraseSeqError  : 1,
      bitAddressError   : 1,
      bitParameterError : 1,
      bitAlwaysZero     : 1;
  } bits;
} CCardR2;

//=================================================================================
// Public functions
void MMC_ReadBlock(unsigned long ul_Address, unsigned char* uc_Buffer);
void MMC_WriteBlock(unsigned long ul_Address, unsigned char* uc_Buffer);
void MMC_PowerControl(unsigned char ucPowerLevel);
CCardStatus MMC_GetStatus(void);
#endif
