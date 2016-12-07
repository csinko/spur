#include "switch.h"

volatile bool switchPosChanged = false;

/*
	pins for switch positions:
	Position 1: Pin 10 -> PB6 (PCINT6)
	Position 2: Pin 11 -> PB7 (PCINT7)

	fs label offsets:
		address			sector		byte
		0x00100047		2048		71		fat32 boot sector
		0x00100c47		2054		71		backup boot sector
		0x00300200		6145		0		directory entry
*/

ISR(PCINT0_vect)
{
	switchPosChanged = true;
	_delay_ms(SWITCH_DEBOUNCE_TIME);
}

void initSwitch()
{
	// set switch pins as input
	DDRB &= ~(1 << DDB7) & ~(1 << DDB6);

	// enable interrupt on PB6
	PCICR |= 1 << PCIE0;
	PCMSK0 = 1 << PCINT6;

	switchPosChanged = true;
	switchTask();
}

void switchTask()
{
	if(switchPosChanged)
	{
		if(PINB & (1 << PB6))
		{
			// PB6 is high, position 1 selected
			uint8_t buffer[11];
			sd_raw_read(0x00100047, buffer, 11);
			serialWriteArray((char *)buffer, 11);
			serialWriteArray(" -> ", 4);

			// set fs label to SPUR_MODE01
			sd_raw_write(0x00100047, (unsigned char*)"SPUR-MODE01", 11);
			sd_raw_write(0x00100c47, (unsigned char*)"SPUR-MODE01", 11);
			sd_raw_write(0x00300200, (unsigned char*)"SPUR-MODE01", 11);

			sd_raw_read(0x00100047, buffer, 11);
			serialWriteArray((char *)buffer, 11);
			serialWriteArray("\n\r", 2);
		}
		else if(PINB & (1 << PB7))
		{
			// PB7 is high, position 2 selected
			uint8_t buffer[11];
			sd_raw_read(0x00100047, buffer, 11);
			serialWriteArray((char *)buffer, 11);
			serialWriteArray(" -> ", 4);

			// set fs label to SPUR_MODE02
			sd_raw_write(0x00100047, (unsigned char*)"SPUR-MODE02", 11);
			sd_raw_write(0x00100c47, (unsigned char*)"SPUR-MODE02", 11);
			sd_raw_write(0x00300200, (unsigned char*)"SPUR-MODE02", 11);
			
			sd_raw_read(0x00100047, buffer, 11);
			serialWriteArray((char *)buffer, 11);
			serialWriteArray("\n\r", 2);
		}
		else
		{
			switchPosChanged = true;
			return;
		}

		switchPosChanged = false;
	}
}
