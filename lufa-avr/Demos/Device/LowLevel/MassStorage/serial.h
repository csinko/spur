#ifndef INCLUDE_SERIAL_H
#define INCLUDE_SERIAL_H

#include <avr/io.h>
#include "Config/AppConfig.h"

#ifdef DEBUG_MODE
void serialBegin(void);
void serialWrite(uint8_t data);
void serialWriteArray(char data[], unsigned int len);
#endif
#endif
