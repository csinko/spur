#include "data.h"

void readData(const uint32_t BlockAddress, uint16_t TotalBlocks)
{
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

			/* Read one 16-byte chunk of data from the Dataflash */
			/*Endpoint_Write_8(Dataflash_ReceiveByte());
			Endpoint_Write_8(Dataflash_ReceiveByte());
			Endpoint_Write_8(Dataflash_ReceiveByte());
			Endpoint_Write_8(Dataflash_ReceiveByte());
			Endpoint_Write_8(Dataflash_ReceiveByte());
			Endpoint_Write_8(Dataflash_ReceiveByte());
			Endpoint_Write_8(Dataflash_ReceiveByte());
			Endpoint_Write_8(Dataflash_ReceiveByte());
			Endpoint_Write_8(Dataflash_ReceiveByte());
			Endpoint_Write_8(Dataflash_ReceiveByte());
			Endpoint_Write_8(Dataflash_ReceiveByte());
			Endpoint_Write_8(Dataflash_ReceiveByte());
			Endpoint_Write_8(Dataflash_ReceiveByte());
			Endpoint_Write_8(Dataflash_ReceiveByte());
			Endpoint_Write_8(Dataflash_ReceiveByte());
			Endpoint_Write_8(Dataflash_ReceiveByte());*/

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