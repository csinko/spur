#ifndef DEBUG_H
#define DEBUG_H

#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include "serial.h"
#include "Config/AppConfig.h"

#ifdef DEBUG_MODE
	#define DEBUG(...) printDebug(__VA_ARGS__)
	#define DEBUG_TEXT(...)	printText(__VA_ARGS__)
	#define DEBUG_NEWLINE() serialWriteArray("\n\r", 2)
	#define DEBUG_INT(...) printInt(__VA_ARGS__)
	#define DEBUG_HEX(...) printHex(__VA_ARGS__)
	#define DEBUG_BIN(...) printBin(__VA_ARGS__)
#else
	#define DEBUG(...)
	#define DEBUG_TEXT(...)
	#define DEBUG_NEWLINE()
	#define DEBUG_INT(...)
	#define DEBUG_HEX(...)
	#define DEBUG_BIN(...)
#endif

#ifdef DEBUG_MODE
typedef enum
{
	MSG_SDCARD,
	MSG_USB,
	MSG_MASS,
	MSG_SWITCH
} msgcat_t;

typedef enum
{
	TYPE_INFO,
	TYPE_WARN,
	TYPE_ERR
} msgtype_t;

void printInt(uint32_t n, bool newline);
void printHex(uint32_t n, uint8_t digits, bool newline);
void printBin(uint32_t n, uint8_t digits, bool newline);

void printDebug(msgcat_t category, msgtype_t type, char msg[], uint8_t length, bool newline);
void printText(char text[], uint8_t length, bool newline);

#endif
#endif
