#include "switch.h"

volatile bool switchPosChanged = false;

/*
	pins for switch positions:
	Position 1: Pin 10 -> PB6 (PCINT6)
	Position 2: Pin 11 -> PB7 (PCINT7)
*/

ISR(PCINT0_vect)
{
	DEBUG(MSG_SWITCH, TYPE_INFO, "Switch interrupt triggered", 26, true);

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
			DEBUG(MSG_SWITCH, TYPE_INFO, "Position 1", 10, true);

			#ifdef DEBUG_MODE
			uint8_t buffer[11];
			sd_raw_read(FS_LABEL_OFFSET_1, buffer, 11);
			DEBUG(MSG_SWITCH, TYPE_INFO, "FAT32 label: ", 13, false); DEBUG_TEXT((char *)buffer, 11, false); DEBUG_TEXT(" -> ", 4, false);
			#endif

			// set fs label to SPUR_MODE01
			sd_raw_write(FS_LABEL_OFFSET_1, (unsigned char*)"SPUR-MODE01", 11);
			sd_raw_write(FS_LABEL_OFFSET_2, (unsigned char*)"SPUR-MODE01", 11);
			sd_raw_write(FS_LABEL_OFFSET_3, (unsigned char*)"SPUR-MODE01", 11);

			#ifdef DEBUG_MODE
			sd_raw_read(FS_LABEL_OFFSET_1, buffer, 11);
			DEBUG_TEXT((char *)buffer, 11, true);
			#endif
		}
		else if(PINB & (1 << PB7))
		{
			// PB7 is high, position 2 selected
			DEBUG(MSG_SWITCH, TYPE_INFO, "Position 2", 10, true);

			#ifdef DEBUG_MODE
			uint8_t buffer[11];
			sd_raw_read(FS_LABEL_OFFSET_1, buffer, 11);
			DEBUG(MSG_SWITCH, TYPE_INFO, "FAT32 label: ", 13, false); DEBUG_TEXT((char *)buffer, 11, false); DEBUG_TEXT(" -> ", 4, false);
			#endif

			// set fs label to SPUR_MODE02
			sd_raw_write(FS_LABEL_OFFSET_1, (unsigned char*)"SPUR-MODE02", 11);
			sd_raw_write(FS_LABEL_OFFSET_2, (unsigned char*)"SPUR-MODE02", 11);
			sd_raw_write(FS_LABEL_OFFSET_3, (unsigned char*)"SPUR-MODE02", 11);
			
			#ifdef DEBUG_MODE
			sd_raw_read(FS_LABEL_OFFSET_1, buffer, 11);
			DEBUG_TEXT((char *)buffer, 11, true);
			#endif
		}
		else
		{
			switchPosChanged = true;
			return;
		}

		switchPosChanged = false;
	}
}
