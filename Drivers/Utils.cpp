#include "Utils.h"
#include "MegaPeriph.h"

//=================================================================================
// Contain string with converted number by function IntToStr
static unsigned char uc_temp_str[6];
static unsigned long RndNext = 1;

unsigned char* IntToStr(unsigned short usValue)
{
  uc_temp_str[0] = ('0'+(usValue % 100000 / 10000));
  uc_temp_str[1] = ('0'+(usValue % 10000 / 1000));
  uc_temp_str[2] = ('0'+(usValue % 1000 / 100));
  uc_temp_str[3] = ('0'+(usValue % 100 / 10));
  uc_temp_str[4] = ('0'+(usValue % 10));
  uc_temp_str[5] = '\0';
  return &uc_temp_str[0];
}

unsigned char HexToBCD(unsigned char ucValue)
{
  unsigned char uc_temp = ucValue;
  unsigned char i = 0;
  while (uc_temp > 10)
  {
    uc_temp-= 10;
    i++;
  }
  return (i << 4) | uc_temp;
}

unsigned char BCDToHex(unsigned char ucValue)
{
  return (ucValue >> 4)*10 + (ucValue & 0x0F);

}

void DosToWin(unsigned char* ucStr)
{
  while (*ucStr)
  {
    if (*ucStr >= 0x80)
    {
      *ucStr+= 0x40;
    }
    *ucStr++;
  }
}

void Randomize(unsigned char ucStart)
{
  RndNext = ucStart;
}

unsigned char Random(unsigned char ucMax)
{
  RndNext = RndNext * 1103515245 + 12345;
  return ((unsigned char)(RndNext / 256) % ((unsigned int)ucMax + 1));
}

unsigned char x_memcmp(const unsigned char* p1, const unsigned char* p2, unsigned char ucSize)
{
  for(unsigned char i = 0; i < ucSize; i++)
  {
    if (*p1++ != *p2++)
      return 1;
  }
  return 0;
}

void x_memset(unsigned char* pDest, unsigned char ucSetTo, unsigned char ucSize)
{
  for(unsigned char i = 0; i < ucSize; i++)
  {
    *pDest++ = ucSetTo;
  }
}

void x_memcpy(unsigned char* pDest, const unsigned char* pSrc, unsigned char ucCount)
{
  while(ucCount--)
  {
    *pDest++ = *pSrc++;
  }
}

unsigned char x_strlen(const unsigned char* p1)
{
  unsigned char ucLength = 0x00;
  while(*p1++)
  {
    ucLength++;
  }
  return ucLength;
}

void x_strcpyff(unsigned char* pucDest, const unsigned char __flash* pfucSrc)
{
  while(*pucDest++ = __load_program_memory(pfucSrc++));
}

void x_strcpy(unsigned char* pDest, const unsigned char* pSrc)
{
  while(*pDest++ = *pSrc++);
}

void x_memcpyff(unsigned char* pucDest, const unsigned char __flash* pfucSrc, unsigned char ucSize)
{
  for (unsigned char i = ucSize; i > 0; i--)
  {
    *pucDest++ = __load_program_memory(pfucSrc++);
  }
}

