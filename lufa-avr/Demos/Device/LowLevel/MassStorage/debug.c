#include "debug.h"

#ifdef DEBUG_MODE

void printInt(uint32_t n, bool newline)
{
	bool printedNonzero = false;
	for(int i = 9; i >= 0; i--)
	{
		uint8_t digit = (n / (uint32_t)pow(10, i)) % 10;
		
		if(!printedNonzero && i != 0)
		{
			if(digit == 0)
				continue;
			else
				printedNonzero = true;
		}

		serialWrite(48 + digit);
	}

	if(newline) serialWriteArray("\n\r", 2);
}

void printHex(uint32_t n, uint8_t digits, bool newline)
{
	serialWriteArray("0x", 2);

	for(int i = 7; i >= 0; i--)
	{
		if(i + 1 > digits)
			continue;

		uint8_t digit = (n >> i * 4) & 0x0F;
		
		if(digit > 9)
			serialWrite(55 + digit);
		else
			serialWrite(48 + digit);
	}

	if(newline) serialWriteArray("\n\r", 2);
}

void printBin(uint32_t n, uint8_t digits, bool newline)
{
	serialWriteArray("0b", 2);

	for(int i = 31; i >= 0; i--)
	{
		if(i + 1 > digits)
			continue;

		serialWrite(48 + ((n >> i) & 1));
	}

	if(newline) serialWriteArray("\n\r", 2);
}

void printDebug(msgcat_t category, msgtype_t type, char msg[], uint8_t length, bool newline)
{
	switch(category)
	{
		case MSG_SDCARD:
			serialWriteArray("sdcard: ", 8);
			break;

		case MSG_USB:
			serialWriteArray("usb: ", 5);
			break;

		case MSG_MASS:
			serialWriteArray("mass_storage: ", 14);
			break;

		case MSG_SWITCH:
			serialWriteArray("switch: ", 8);
			break;
	}

	switch(type)
	{
		case TYPE_INFO:
			serialWriteArray("[info] ", 7);
			break;

		case TYPE_WARN:
			serialWriteArray("[warn] ", 7);
			break;

		case TYPE_ERR:
			serialWriteArray("[error] ", 8);
			break;
	}

	serialWriteArray(msg, length);
	if(newline) serialWriteArray("\n\r", 2);
}

void printText(char text[], uint8_t length, bool newline)
{
	serialWriteArray(text, length);
	if(newline) serialWriteArray("\n\r", 2);
}

#endif
