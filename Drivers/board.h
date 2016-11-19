#ifndef _BOARD_H_
#define _BOARD_H_

#include "megaperiph.h"

#define LED_RED_PORT	PORTC
#define LED_BLUE_PORT	PORTC
#define LED_RED_DDR	DDRC
#define LED_BLUE_DDR	DDRC
#define LED_RED_PIN	PC3
#define LED_BLUE_PIN	PC2

//=================================================================================
inline void Board_Setup()
{
  // Initialize all i/o ports as HI-Z
  DDRB = DDRC = DDRD = 0x00;
  PORTB = PORTC = PORTD = 0x00;

  LED_RED_DDR |= 1 << LED_RED_PIN;
  LED_BLUE_DDR |= 1 << LED_BLUE_PIN;
  LED_RED_PORT |= 1 << LED_RED_PIN;
  LED_BLUE_PORT |= 1 << LED_BLUE_PIN;
}

//=================================================================================
#define ledRedOn()    LED_RED_PORT &= ~(1 << LED_RED_PIN)
#define ledRedOff()   LED_RED_PORT |= (1 << LED_RED_PIN)
#define ledRedToogle()   (LED_RED_PORT ^= (1 << LED_RED_PIN))

#define ledGreenOn()  LED_BLUE_PORT &= ~(1 << LED_BLUE_PIN)
#define ledGreenOff() LED_BLUE_PORT |= (1 << LED_BLUE_PIN)
#define ledGreenToogle() ((LED_BLUE_PORT) ^= (1 << (LED_BLUE_PIN)))


#endif //#ifndef _BOARD_H_
