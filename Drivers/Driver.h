#ifndef _XDRIVER_H_
#define _XDRIVER_H_

#include <ioavr.h>
#include <intrinsics.h>

//=================================================================================
// Device power consumption level
enum
{
  plNormalPower    = 0,
  plLowPower       = 1,
  plMinimalPower   = 2,
};

#endif // _XDRIVER_H_
