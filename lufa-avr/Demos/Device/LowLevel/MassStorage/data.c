#include "data.h"

unsigned char readcmdmsg[] = {'r', 'e', 'a', 'd', ' '};
unsigned char readcmdmsg2[] = {' ', 'l', 'e', 'n', 'g', 't', 'h', ' '};
unsigned char digitLookup[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
uint32_t test = 1234567890;

void readData(const uint32_t BlockAddress, uint16_t TotalBlocks)
{
	uint32_t offset = BlockAddress * VIRTUAL_MEMORY_BLOCK_SIZE;
	uint32_t length = TotalBlocks * VIRTUAL_MEMORY_BLOCK_SIZE;

	serialWriteArray(readcmdmsg, 5);
	for(int i = 9; i >= 0; i--)
	{
		serialWrite(digitLookup[(offset / (uint32_t)pow(10, i)) % 10]);
	}
	serialWriteArray(readcmdmsg2, 8);
	for(int i = 9; i >= 0; i--)
	{
		serialWrite(digitLookup[(length / (uint32_t)pow(10, i)) % 10]);
	}
	serialWrite('\n');
	serialWrite('\r');

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
				/* Clear the endpoint bank to send its contents to the host */
				Endpoint_ClearIN();

				/* Wait until the endpoint is ready for more data */
				if (Endpoint_WaitUntilReady())
				  return;
			}

			for(int j = 0; j < 16; j++)
			{
				uint8_t buffer;
				sd_raw_read(offset, &buffer, 1);
				Endpoint_Write_8(buffer);
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
	}

	/* If the endpoint is full, send its contents to the host */
	if (!(Endpoint_IsReadWriteAllowed()))
	  Endpoint_ClearIN();
}