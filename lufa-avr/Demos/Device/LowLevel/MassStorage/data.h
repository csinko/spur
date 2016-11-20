#include <avr/io.h>
#include <LUFA/Common/Common.h>
#include <LUFA/Drivers/USB/USB.h>
#include "../MassStorage.h"

void readData(const uint32_t BlockAddress, uint16_t TotalBlocks);