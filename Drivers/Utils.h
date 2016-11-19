#ifndef _UTILS_H_
#define _UTILS_H_

//=================================================================================
// Convert unsigned integer number to string
unsigned char* IntToStr(unsigned short usValue);

//=================================================================================
// Convert HEX number ucValue to BCD:
// -- Return BCD represent of ucValue
unsigned char HexToBCD(unsigned char ucValue);

//=================================================================================
// Convert BCD number ucValue to HEX:
// -- Return HEX represent of ucValue
unsigned char BCDToHex(unsigned char ucValue);

//=================================================================================
// Contain string with converted number by function IntToStr
void DosToWin(unsigned char* pString);

//=================================================================================
// Initialize random number generator with some starting value
// -- ucStart can't be zero
void Randomize(unsigned char ucStart);

//=================================================================================
// Generate random number from interval[0..ucMax]
// -- Return random number not grater than ucMax
unsigned char Random(unsigned char ucMax);

//=================================================================================
// Compare elements from 2 arrays:
//  -- If all elements are equal, return 0.
//  -- Otherwise return 1.
unsigned char x_memcmp(const unsigned char* p1, const unsigned char* p2, unsigned char ucSize);

//=================================================================================
// Initialize memory stating from pDest size ucSize with ucSetTo
void x_memset(unsigned char* pDest, unsigned char ucSetTo, unsigned char ucSize);

//=================================================================================
// Copy memory block, with size ucCount, from pSrc to pDest
void x_memcpy(unsigned char* pDest, const unsigned char* pSrc, unsigned char ucCount);

//=================================================================================
// Calculate string lenght:
//  -- Return string length without null-character
unsigned char x_strlen(const unsigned char* p1);

//=================================================================================
// Copy string from pSrc to pDest
void x_strcpy(unsigned char* pDest, const unsigned char* pSrc);

//=================================================================================
// Copy string from pSrc to pDest, where pSrc in flash memory
void x_strcpyff(unsigned char* pucDest, const unsigned char __flash* pfucSrc);

//=================================================================================
// Copy string from pSrc to pDest, where pSrc in flash memory
void x_memcpyff(unsigned char* pucDest, const unsigned char __flash* pfucSrc, unsigned char ucSize);


#endif //_UTILS_H_
