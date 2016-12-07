#ifndef SWITCH_H
#define SWITCH_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include "serial.h"
#include "Lib/sd_raw.h"
#include "Lib/sd_raw_config.h"

#define SWITCH_DEBOUNCE_TIME 8		// time it takes for switch to bounce plus a little longer (in ms)

void initSwitch(void);
void switchTask(void);

#endif
