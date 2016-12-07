#include "data.h"

void readData(const uint32_t BlockAddress, uint16_t TotalBlocks)
{
	uint32_t offset = BlockAddress * VIRTUAL_MEMORY_BLOCK_SIZE;

	DEBUG(MSG_MASS, TYPE_INFO, "Reading from offset ", 20, false); DEBUG_HEX(offset, 8, false);
	DEBUG_TEXT(" (block ", 8, false); DEBUG_INT(BlockAddress, false); DEBUG_TEXT(") with length ", 14, false);
	DEBUG_INT(TotalBlocks * VIRTUAL_MEMORY_BLOCK_SIZE, false); DEBUG_TEXT(" bytes (", 8, false); DEBUG_INT(TotalBlocks, false); DEBUG_TEXT(" blocks)", 8, true);

	/* Wait until endpoint is ready before continuing */
	if (Endpoint_WaitUntilReady())
	  return;

	while (TotalBlocks)
	{
		uint8_t BytesInBlockDiv16 = 0;

		/* Read an endpoint packet sized data block from the Dataflash */
		while (BytesInBlockDiv16 < (VIRTUAL_MEMORY_BLOCK_SIZE >> 4))
		{
			/* Check if the endpoint is currently full */
			if (!(Endpoint_IsReadWriteAllowed()))
			{
				DEBUG(MSG_MASS, TYPE_INFO, "Endpoint not allowing read/write", 32, true);

				/* Clear the endpoint bank to send its contents to the host */
				Endpoint_ClearIN();
				DEBUG(MSG_MASS, TYPE_INFO, "Cleared endpoint", 16, true);

				/* Wait until the endpoint is ready for more data */
				if (Endpoint_WaitUntilReady())
				{
					DEBUG(MSG_MASS, TYPE_INFO, "Waiting for endpoint to be ready", 32, true);
					return;
				}
			}

			for(int j = 0; j < 16; j++)
			{
				uint8_t buffer;
				sd_raw_read(offset, &buffer, 1);
				Endpoint_Write_8(buffer);
				DEBUG(MSG_SDCARD, TYPE_INFO, "Read 16 bytes from offset ", 26, false); DEBUG_HEX(offset, 8, true);
				offset++;
			}

			/* Increment the block 16 byte block counter */
			BytesInBlockDiv16++;

			/* Check if the current command is being aborted by the host */
			if (IsMassStoreReset)
			  return;
		}

		/* Decrement the blocks remaining counter */
		TotalBlocks--;
		DEBUG(MSG_MASS, TYPE_INFO, "Read block", 22, true);
	}

	/* If the endpoint is full, send its contents to the host */
	if (!(Endpoint_IsReadWriteAllowed()))
	{
		Endpoint_ClearIN();
		DEBUG(MSG_MASS, TYPE_INFO, "Cleared endpoint", 16, true);
	}
}
