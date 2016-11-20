/* SCSI processing for Mass Storage "Bulk-Only Transport" protocol */

#define  INCLUDE_FROM_SCSI_C
#include "SCSI.h"

/** Structure to hold the SCSI response data to a SCSI INQUIRY command. This gives information about the device's
 *  features and capabilities.
 */
static const SCSI_Inquiry_Response_t InquiryData =
	{
		.DeviceType          = DEVICE_TYPE_BLOCK,
		.PeripheralQualifier = 0,

		.Removable           = false,

		.Version             = 0,

		.ResponseDataFormat  = 2,
		.NormACA             = false,
		.TrmTsk              = false,
		.AERC                = false,

		.AdditionalLength    = 0x1F,

		.SoftReset           = false,
		.CmdQue              = false,
		.Linked              = false,
		.Sync                = false,
		.WideBus16Bit        = false,
		.WideBus32Bit        = false,
		.RelAddr             = false,

		.VendorID            = "Spur",
		.ProductID           = "Spur Multiboot",
		.RevisionID          = {'0','.','0','0'},
	};

unsigned char inquirymsg[] = {'i', 'n', 'q', 'u', 'i', 'r', 'y', ' ', 'c', 'o', 'm', 'm', 'a', 'n', 'd', '\n', '\r'};
unsigned char requestsensemsg[] = {'r', 'e', 'q', 'u', 'e', 's', 't', ' ', 's', 'e', 'n', 's', 'e', ' ', 'c', 'o', 'm', 'm', 'a', 'n', 'd', '\n', '\r'};
unsigned char readcapacitymsg[] = {'r', 'e', 'a', 'd', ' ', 'c', 'a', 'p', 'a', 'c', 'i', 't', 'y', ' ', 'c', 'o', 'm', 'm', 'a', 'n', 'd', '\n', '\r'};
unsigned char senddiagnosticmsg[] = {'s', 'e', 'n', 'd', ' ', 'd', 'i', 'a', 'g', 'n', 'o', 's', 't', 'i', 'c', ' ', 'c', 'o', 'm', 'm', 'a', 'n', 'd', '\n', '\r'};
unsigned char writemsg[] = {'w', 'r', 'i', 't', 'e', ' ', 'c', 'o', 'm', 'm', 'a', 'n', 'd', '\n', '\r'};
unsigned char readmsg[] = {'r', 'e', 'a', 'd', ' ', 'c', 'o', 'm', 'm', 'a', 'n', 'd', '\n', '\r'};
unsigned char modesensemsg[] = {'m', 'o', 'd', 'e', ' ', 's', 'e', 'n', 's', 'e', ' ', 'c', 'o', 'm', 'm', 'a', 'n', 'd', '\n', '\r'};
unsigned char startstopmsg[] = {'s', 't', 'a', 'r', 't', ' ', 's', 't', 'o', 'p', ' ', 'c', 'o', 'm', 'm', 'a', 'n', 'd', '\n', '\r'};
unsigned char testunitmsg[] = {'t', 'e', 's', 't', ' ', 'u', 'n', 'i', 't', ' ', 'c', 'o', 'm', 'm', 'a', 'n', 'd', '\n', '\r'};
unsigned char preventallowmediumremovalmsg[] = {'p', 'r', 'e', 'v', 'e', 'n', 't', ' ', 'a', 'l', 'l', 'o', 'w', ' ', 'r', 'e', 'm', 'o', 'v', 'a', 'l', ' ', 'c', 'o', 'm', 'm', 'a', 'n', 'd', '\n', '\r'};
unsigned char verifymsg[] = {'v', 'e', 'r', 'i', 'f', 'y', ' ', 'c', 'o', 'm', 'm', 'a', 'n', 'd', '\n', '\r'};
unsigned char invalidmsg[] = {'i', 'n', 'v', 'a', 'l', 'i', 'd', ' ', 'c', 'o', 'm', 'm', 'a', 'n', 'd', '\n', '\r'};

/** Structure to hold the sense data for the last issued SCSI command, which is returned to the host after a SCSI REQUEST SENSE
 *  command is issued. This gives information on exactly why the last command failed to complete.
 */
static SCSI_Request_Sense_Response_t SenseData =
	{
		.ResponseCode        = 0x70,
		.AdditionalLength    = 0x0A,
	};


/** Main routine to process the SCSI command located in the Command Block Wrapper read from the host. This dispatches
 *  to the appropriate SCSI command handling routine if the issued command is supported by the device, else it returns
 *  a command failure due to a ILLEGAL REQUEST.
 *
 *  \return Boolean \c true if the command completed successfully, \c false otherwise
 */
bool SCSI_DecodeSCSICommand(void)
{
	bool CommandSuccess = false;

	/* Run the appropriate SCSI command hander function based on the passed command */
	switch (CommandBlock.SCSICommandData[0])
	{
		case SCSI_CMD_INQUIRY:
			serialWriteArray(inquirymsg, 17);
			CommandSuccess = SCSI_Command_Inquiry();
			break;
		case SCSI_CMD_REQUEST_SENSE:
			serialWriteArray(requestsensemsg, 23);
			CommandSuccess = SCSI_Command_Request_Sense();
			break;
		case SCSI_CMD_READ_CAPACITY_10:
			serialWriteArray(readcapacitymsg, 23);
			CommandSuccess = SCSI_Command_Read_Capacity_10();
			break;
		case SCSI_CMD_SEND_DIAGNOSTIC:
			serialWriteArray(senddiagnosticmsg, 25);
			CommandSuccess = SCSI_Command_Send_Diagnostic();
			break;
		case SCSI_CMD_WRITE_10:
			serialWriteArray(writemsg, 15);
			CommandSuccess = SCSI_Command_ReadWrite_10(DATA_WRITE);
			break;
		case SCSI_CMD_READ_10:
			serialWriteArray(readmsg, 14);
			CommandSuccess = SCSI_Command_ReadWrite_10(DATA_READ);
			break;
		case SCSI_CMD_MODE_SENSE_6:
			serialWriteArray(modesensemsg, 20);
			CommandSuccess = SCSI_Command_ModeSense_6();
			break;
		case SCSI_CMD_START_STOP_UNIT:
			serialWriteArray(startstopmsg, 20);
			/* These commands should just succeed, no handling required */
			CommandSuccess = true;
			CommandBlock.DataTransferLength = 0;
			break;
		case SCSI_CMD_TEST_UNIT_READY:
			serialWriteArray(testunitmsg, 19);
			/* These commands should just succeed, no handling required */
			CommandSuccess = true;
			CommandBlock.DataTransferLength = 0;
			break;
		case SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL:
			serialWriteArray(preventallowmediumremovalmsg, 31);
			/* These commands should just succeed, no handling required */
			CommandSuccess = true;
			CommandBlock.DataTransferLength = 0;
			break;
		case SCSI_CMD_VERIFY_10:
			serialWriteArray(verifymsg, 16);
			/* These commands should just succeed, no handling required */
			CommandSuccess = true;
			CommandBlock.DataTransferLength = 0;
			break;
		default:
			/* Update the SENSE key to reflect the invalid command */
			SCSI_SET_SENSE(SCSI_SENSE_KEY_ILLEGAL_REQUEST,
		                   SCSI_ASENSE_INVALID_COMMAND,
		                   SCSI_ASENSEQ_NO_QUALIFIER);
			break;
	}

	/* Check if command was successfully processed */
	if (CommandSuccess)
	{
		SCSI_SET_SENSE(SCSI_SENSE_KEY_GOOD,
		               SCSI_ASENSE_NO_ADDITIONAL_INFORMATION,
		               SCSI_ASENSEQ_NO_QUALIFIER);

		return true;
	}

	return false;
}

/** Command processing for an issued SCSI INQUIRY command. This command returns information about the device's features
 *  and capabilities to the host.
 *
 *  \return Boolean \c true if the command completed successfully, \c false otherwise.
 */
static bool SCSI_Command_Inquiry(void)
{
	uint16_t AllocationLength  = SwapEndian_16(*(uint16_t*)&CommandBlock.SCSICommandData[3]);
	uint16_t BytesTransferred  = MIN(AllocationLength, sizeof(InquiryData));

	/* Only the standard INQUIRY data is supported, check if any optional INQUIRY bits set */
	if ((CommandBlock.SCSICommandData[1] & ((1 << 0) | (1 << 1))) ||
	     CommandBlock.SCSICommandData[2])
	{
		/* Optional but unsupported bits set - update the SENSE key and fail the request */
		SCSI_SET_SENSE(SCSI_SENSE_KEY_ILLEGAL_REQUEST,
		               SCSI_ASENSE_INVALID_FIELD_IN_CDB,
		               SCSI_ASENSEQ_NO_QUALIFIER);

		return false;
	}

	/* Write the INQUIRY data to the endpoint */
	Endpoint_Write_Stream_LE(&InquiryData, BytesTransferred, NULL);

	/* Pad out remaining bytes with 0x00 */
	Endpoint_Null_Stream((AllocationLength - BytesTransferred), NULL);

	/* Finalize the stream transfer to send the last packet */
	Endpoint_ClearIN();

	/* Succeed the command and update the bytes transferred counter */
	CommandBlock.DataTransferLength -= BytesTransferred;

	return true;
}

/** Command processing for an issued SCSI REQUEST SENSE command. This command returns information about the last issued command,
 *  including the error code and additional error information so that the host can determine why a command failed to complete.
 *
 *  \return Boolean \c true if the command completed successfully, \c false otherwise.
 */
static bool SCSI_Command_Request_Sense(void)
{
	uint8_t  AllocationLength = CommandBlock.SCSICommandData[4];
	uint8_t  BytesTransferred = MIN(AllocationLength, sizeof(SenseData));

	/* Send the SENSE data - this indicates to the host the status of the last command */
	Endpoint_Write_Stream_LE(&SenseData, BytesTransferred, NULL);

	/* Pad out remaining bytes with 0x00 */
	Endpoint_Null_Stream((AllocationLength - BytesTransferred), NULL);

	/* Finalize the stream transfer to send the last packet */
	Endpoint_ClearIN();

	/* Succeed the command and update the bytes transferred counter */
	CommandBlock.DataTransferLength -= BytesTransferred;

	return true;
}

/** Command processing for an issued SCSI READ CAPACITY (10) command. This command returns information about the device's capacity
 *  on the selected Logical Unit (drive), as a number of OS-sized blocks.
 *
 *  \return Boolean \c true if the command completed successfully, \c false otherwise.
 */
static bool SCSI_Command_Read_Capacity_10(void)
{
	/* Send the total number of logical blocks in the current LUN */
	//Endpoint_Write_32_BE(LUN_MEDIA_BLOCKS - 1);
	Endpoint_Write_32_BE(10);

	/* Send the logical block size of the device (must be 512 bytes) */
	//Endpoint_Write_32_BE(VIRTUAL_MEMORY_BLOCK_SIZE);
	Endpoint_Write_32_BE(1024);

	/* Check if the current command is being aborted by the host */
	if (IsMassStoreReset)
	  return false;

	/* Send the endpoint data packet to the host */
	Endpoint_ClearIN();

	/* Succeed the command and update the bytes transferred counter */
	CommandBlock.DataTransferLength -= 8;

	return true;
}

/** Command processing for an issued SCSI SEND DIAGNOSTIC command. This command performs a quick check of the Dataflash ICs on the
 *  board, and indicates if they are present and functioning correctly. Only the Self-Test portion of the diagnostic command is
 *  supported.
 *
 *  \return Boolean \c true if the command completed successfully, \c false otherwise.
 */
static bool SCSI_Command_Send_Diagnostic(void)
{
	/*
	/* Check to see if the SELF TEST bit is not set 
	if (!(CommandBlock.SCSICommandData[1] & (1 << 2)))
	{
		/* Only self-test supported - update SENSE key and fail the command 
		SCSI_SET_SENSE(SCSI_SENSE_KEY_ILLEGAL_REQUEST,
		               SCSI_ASENSE_INVALID_FIELD_IN_CDB,
		               SCSI_ASENSEQ_NO_QUALIFIER);

		return false;
	}

	/* Check to see if all attached Dataflash ICs are functional 
	if (!(DataflashManager_CheckDataflashOperation()))
	{
		/* Update SENSE key with a hardware error condition and return command fail 
		SCSI_SET_SENSE(SCSI_SENSE_KEY_HARDWARE_ERROR,
		               SCSI_ASENSE_NO_ADDITIONAL_INFORMATION,
		               SCSI_ASENSEQ_NO_QUALIFIER);

		return false;
	}
	*/

	/* Succeed the command and update the bytes transferred counter */
	CommandBlock.DataTransferLength = 0;

	return true;
}

/** Command processing for an issued SCSI READ (10) or WRITE (10) command. This command reads in the block start address
 *  and total number of blocks to process, then calls the appropriate low-level Dataflash routine to handle the actual
 *  reading and writing of the data.
 *
 *  \param[in] IsDataRead  Indicates if the command is a READ (10) command or WRITE (10) command (DATA_READ or DATA_WRITE)
 *
 *  \return Boolean \c true if the command completed successfully, \c false otherwise.
 */
static bool SCSI_Command_ReadWrite_10(const bool IsDataRead)
{
	uint32_t BlockAddress;
	uint16_t TotalBlocks;

	
	/* Check if the disk is write protected or not */
	if ((IsDataRead == DATA_WRITE) && DISK_READ_ONLY)
	{
		/* Block address is invalid, update SENSE key and return command fail */
		SCSI_SET_SENSE(SCSI_SENSE_KEY_DATA_PROTECT,
		               SCSI_ASENSE_WRITE_PROTECTED,
		               SCSI_ASENSEQ_NO_QUALIFIER);

		return false;
	}

	BlockAddress = SwapEndian_32(*(uint32_t*)&CommandBlock.SCSICommandData[2]);
	TotalBlocks  = SwapEndian_16(*(uint16_t*)&CommandBlock.SCSICommandData[7]);

	/* Check if the block address is outside the maximum allowable value for the LUN */
	if (BlockAddress >= LUN_MEDIA_BLOCKS)
	{
		/* Block address is invalid, update SENSE key and return command fail */
		SCSI_SET_SENSE(SCSI_SENSE_KEY_ILLEGAL_REQUEST,
		               SCSI_ASENSE_LOGICAL_BLOCK_ADDRESS_OUT_OF_RANGE,
		               SCSI_ASENSEQ_NO_QUALIFIER);

		return false;
	}

	for(int i = 32; i >= 0; i--)
	{
		uint8_t bit = BlockAddress >> i;
		bit &= 1;
		if(bit == 1)
			serialWrite('1');
		else
			serialWrite('0');
	}
	serialWrite('\n');
	serialWrite('\r');

	// read command
	readData(BlockAddress, TotalBlocks);

	/* Update the bytes transferred counter and succeed the command */
	CommandBlock.DataTransferLength -= ((uint32_t)TotalBlocks * VIRTUAL_MEMORY_BLOCK_SIZE);

	return true;
}

/** Command processing for an issued SCSI MODE SENSE (6) command. This command returns various informational pages about
 *  the SCSI device, as well as the device's Write Protect status.
 *
 *  \return Boolean \c true if the command completed successfully, \c false otherwise.
 */
static bool SCSI_Command_ModeSense_6(void)
{
	/* Send an empty header response with the Write Protect flag status */
	Endpoint_Write_8(0x00);
	Endpoint_Write_8(0x00);
	Endpoint_Write_8(DISK_READ_ONLY ? 0x80 : 0x00);
	Endpoint_Write_8(0x00);
	Endpoint_ClearIN();

	/* Update the bytes transferred counter and succeed the command */
	CommandBlock.DataTransferLength -= 4;

	return true;
}
