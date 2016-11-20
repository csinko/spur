#ifndef INCLUDE_SERIAL_H
#define INCLUDE_SERIAL_H

#include <avr/io.h>

void serialBegin(void);
void serialWrite( uint8_t data );
void serialWriteArray( unsigned char data[], unsigned int len);

#endif
