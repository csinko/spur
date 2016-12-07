#include <avr/io.h>
#include <LUFA/Common/Common.h>
#include <LUFA/Drivers/USB/USB.h>
#include "debug.h"
#include "MassStorage.h"
#include "Lib/sd_raw.h"
#include "Lib/sd_raw_config.h"

void readData(const uint32_t BlockAddress, uint16_t TotalBlocks);