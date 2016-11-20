#include "data.h"

unsigned char BlockAddressmsg[] = {'b', 'l', 'k', 'a', 'd', 'd', 'r', ':', ' '};
unsigned char TotalBlocksmsg[] = {'t', 'o', 't', 'b', 'l', 'k', 's', ':', ' '};
unsigned char currentblkmsg[] = {'c', 'u', 'r', 'r', 'e', 'n', 't', 'b', 'l', 'k', ':', ' '};
unsigned char endpointfullmsg[] = {'e', 'n', 'd', 'p', 'o', 'i', 'n', 't', ' ', 'f', 'u', 'l', 'l', '\n', '\r'};
unsigned char buffermsg[] = {'b', 'u', 'f', 'f', 'e', 'r', ':', ' '};
unsigned char chunkmsg[] = {'c', 'h', 'u', 'n', 'k'};
void readData(const uint32_t BlockAddress, uint16_t TotalBlocks)
{
	serialWrite('c');
	serialWrite('a');
	serialWrite('l');
	serialWrite('l');
	serialWrite('\n');
	serialWrite('\r');
	serialWriteArray(BlockAddressmsg, 9);
	for(int i = 32; i >= 0; i--)
	{
		uint8_t bit = BlockAddress >> i;
		bit &= 1;
		if(bit == 1)
			serialWrite('1');
		else
			serialWrite('0');
	}
	serialWrite(' ');
	serialWriteArray(TotalBlocksmsg, 9);
	for(int i = 16; i >= 0; i--)
	{
		uint8_t bit = TotalBlocks >> i;
		bit &= 1;
		if(bit == 1)
			serialWrite('1');
		else
			serialWrite('0');
	}
	serialWrite('\n');
	serialWrite('\r');

	/* Wait until endpoint is ready before continuing */
	if (Endpoint_WaitUntilReady())
	  return;

	for(uint8_t currentBlock = 0; currentBlock < TotalBlocks; currentBlock++)
	{
		uint8_t currentblk = currentBlock;
		serialWriteArray(currentblkmsg, 12);
		for(int i = 8; i >= 0; i--)
		{
			uint8_t bit = currentblk >> i;
			bit &= 1;
			if(bit == 1)
				serialWrite('1');
			else
				serialWrite('0');
		}
		serialWrite(' ');
		serialWrite('o');
		serialWrite('f');
		serialWrite(' ');
		for(int i = 8; i >= 0; i--)
		{
			uint8_t bit = currentblk >> i;
			bit &= 1;
			if(bit == 1)
				serialWrite('1');
			else
				serialWrite('0');
		}
		for(int chunk = 0; chunk * 16 < VIRTUAL_MEMORY_BLOCK_SIZE; chunk++)
		{
			serialWriteArray(chunkmsg, 5);
			serialWrite(' ');
			serialWrite(chunk + 48);
			serialWrite('\n');
			serialWrite('\r');
			/* Check if the endpoint is currently full */
			if (!(Endpoint_IsReadWriteAllowed()))
			{
				serialWriteArray(endpointfullmsg, 15);
				/* Clear the endpoint bank to send its contents to the host */
				Endpoint_ClearIN();

				/* Wait until the endpoint is ready for more data */
				if (Endpoint_WaitUntilReady())
				  return;
			}

			for(int i = 0; i < 16; i++)
			{
				uint8_t buffer;
				sd_raw_read(currentBlock * VIRTUAL_MEMORY_BLOCK_SIZE + chunk * 16 + i, &buffer, 1);
				Endpoint_Write_8(buffer);
			}

			/* Check if the current command is being aborted by the host */
			if (IsMassStoreReset)
			  return;
		}
	}

	/* If the endpoint is full, send its contents to the host */
	if (!(Endpoint_IsReadWriteAllowed()))
	  Endpoint_ClearIN();
}