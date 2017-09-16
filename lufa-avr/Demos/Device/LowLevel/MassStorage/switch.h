#ifndef SWITCH_H
#define SWITCH_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include "serial.h"
#include "debug.h"
#include "Lib/sd_raw.h"
#include "Lib/sd_raw_config.h"

#define SWITCH_DEBOUNCE_TIME 8		// time it takes for switch to bounce plus a little longer (in ms)

#define FS_LABEL_OFFSET_1 0x004f4400
#define FS_LABEL_OFFSET_2 0x01bca447
#define FS_LABEL_OFFSET_3 0x01bcb047

void initSwitch(void);
void switchTask(void);

#endif
