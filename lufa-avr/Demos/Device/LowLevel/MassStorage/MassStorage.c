#define  INCLUDE_FROM_MASSSTORAGE_C
#include "MassStorage.h"

/** Structure to hold the latest Command Block Wrapper issued by the host, containing a SCSI command to execute. */
MS_CommandBlockWrapper_t  CommandBlock;

/** Structure to hold the latest Command Status Wrapper to return to the host, containing the status of the last issued command. */
MS_CommandStatusWrapper_t CommandStatus = { .Signature = MS_CSW_SIGNATURE };

/** Flag to asynchronously abort any in-progress data transfers upon the reception of a mass storage reset command. */
volatile bool IsMassStoreReset = false;

unsigned char serialmsg[] = {'T', 'e', 's', 't'};
unsigned char hexLookup[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'F'};
unsigned char sderrormsg[] = {'s', 'd', ' ', 'c', 'a', 'r', 'd', ' ', 'i', 'n', 'i', 't', ' ', 'e', 'r', 'r', 'o', 'r', '\n', '\r'};
unsigned char fslabelmsg[] = "original label:\n\r";
unsigned char fslabelmsg2[] = "modified label:\n\r";
unsigned char pos1msg[] = "position 1\n\r";
unsigned char pos2msg[] = "position 2\n\r";

int main(void)
{
	serialBegin();
	
	if(!sd_raw_init())
	{
		LEDs_SetAllLEDs(LEDMASK_USB_ERROR);
		serialWriteArray(sderrormsg, 20);
		for (;;) ;
	}

	/*
		pin for switch positions:
		Pin 10 -> PB6 (PCINT6)
		Pin 11 -> PB7 (PCINT7)

		fs label offsets:
		address			sector		byte
		0x00100047		2048		71		fat32 boot sector
		0x00100c47		2054		71		backup boot sector
		0x00300200		6145		0		directory entry
	*/

	// set pins as input
	DDRB &= ~(1 << DDB7) & ~(1 << DDB6);

	// get switch position
	if(PINB & (1 << PB6))
	{
		// PB6 is high, position 1 selected
		// set fs label to SPUR_MODE01
		sd_raw_write(0x00100047, "SPUR-MODE01", 11);
		sd_raw_write(0x00100c47, "SPUR-MODE01", 11);
		sd_raw_write(0x00300200, "SPUR-MODE01", 11);
	}
	else if(PINB & (1 << PB7))
	{
		// PB7 is high, position 2 selected
		// set fs label to SPUR_MODE02
		sd_raw_write(0x00100047, "SPUR_MODE02", 11);
		sd_raw_write(0x00100c47, "SPUR_MODE02", 11);
		sd_raw_write(0x00300200, "SPUR_MODE02", 11);
	}

	SetupHardware();

	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
	GlobalInterruptEnable();

	for (;;)
	{
		MassStorage_Task();
		USB_USBTask();
	}
}

void SetupHardware(void)
{
#if (ARCH == ARCH_AVR8)
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);
#elif (ARCH == ARCH_XMEGA)
	/* Start the PLL to multiply the 2MHz RC oscillator to 32MHz and switch the CPU core to run from it */
	XMEGACLK_StartPLL(CLOCK_SRC_INT_RC2MHZ, 2000000, F_CPU);
	XMEGACLK_SetCPUClockSource(CLOCK_SRC_PLL);

	/* Start the 32MHz internal RC oscillator and start the DFLL to increase it to 48MHz using the USB SOF as a reference */
	XMEGACLK_StartInternalOscillator(CLOCK_SRC_INT_RC32MHZ);
	XMEGACLK_StartDFLL(CLOCK_SRC_INT_RC32MHZ, DFLL_REF_INT_USBSOF, F_USB);

	PMIC.CTRL = PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;
#endif

	/* Hardware Initialization */
	LEDs_Init();
	//Dataflash_Init();
	USB_Init();

	/* Check if the Dataflash is working, abort if not */
	/*if (!(DataflashManager_CheckDataflashOperation()))
	{
		LEDs_SetAllLEDs(LEDMASK_USB_ERROR);
		for(;;);
	}

	/* Clear Dataflash sector protections, if enabled 
	DataflashManager_ResetDataflashProtections();*/
}

/** Event handler for the USB_Connect event. This indicates that the device is enumerating via the status LEDs. */
void EVENT_USB_Device_Connect(void)
{
	/* Indicate USB enumerating */
	LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);

	/* Reset the MSReset flag upon connection */
	IsMassStoreReset = false;
}

/** Event handler for the USB_Disconnect event. This indicates that the device is no longer connected to a host via
 *  the status LEDs and stops the Mass Storage management task.
 */
void EVENT_USB_Device_Disconnect(void)
{
	/* Indicate USB not ready */
	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
}

/** Event handler for the USB_ConfigurationChanged event. This is fired when the host set the current configuration
 *  of the USB device after enumeration - the device endpoints are configured and the Mass Storage management task started.
 */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	//serialWriteArray(msg, 9);
	bool ConfigSuccess = true;

	/* Setup Mass Storage Data Endpoints */
	ConfigSuccess &= Endpoint_ConfigureEndpoint(MASS_STORAGE_IN_EPADDR,  EP_TYPE_BULK, MASS_STORAGE_IO_EPSIZE, 1);
	ConfigSuccess &= Endpoint_ConfigureEndpoint(MASS_STORAGE_OUT_EPADDR, EP_TYPE_BULK, MASS_STORAGE_IO_EPSIZE, 1);

	/* Indicate endpoint configuration success or failure */
	LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
}

/** Event handler for the USB_ControlRequest event. This is used to catch and process control requests sent to
 *  the device from the USB host before passing along unhandled control requests to the library for processing
 *  internally.
 */
void EVENT_USB_Device_ControlRequest(void)
{
	/*uint8_t type = USB_ControlRequest.bmRequestType;
	for(int i = 7; i >= 0; i--)
	{
		uint8_t bit = type >> i;
		bit &= 1;
		if(bit == 1)
			serialWrite('1');
		else
			serialWrite('0');
	}
	serialWrite(' ');

	uint8_t commandCode = USB_ControlRequest.bRequest;
	for(int i = 7; i >= 0; i--)
	{
		uint8_t bit = commandCode >> i;
		bit &= 1;
		if(bit == 1)
			serialWrite('1');
		else
			serialWrite('0');
	}
	serialWrite(' ');

	uint16_t wValue = USB_ControlRequest.wValue;
	for(int i = 15; i >= 0; i--)
	{
		uint16_t bit = wValue >> i;
		bit &= 1;
		if(bit == 1)
			serialWrite('1');
		else
			serialWrite('0');
	}

	serialWrite(' ');
	serialWrite('\n');
	serialWrite('\r');*/

	/* Process UFI specific control requests */
	switch (USB_ControlRequest.bRequest)
	{
		case MS_REQ_MassStorageReset:
			if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();
				Endpoint_ClearStatusStage();

				/* Indicate that the current transfer should be aborted */
				IsMassStoreReset = true;
			}

			break;
		case MS_REQ_GetMaxLUN:
			if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();

				/* Indicate to the host the number of supported LUNs (virtual disks) on the device */
				Endpoint_Write_8(TOTAL_LUNS - 1);

				Endpoint_ClearIN();
				Endpoint_ClearStatusStage();
			}

			break;
	}
}

/** Task to manage the Mass Storage interface, reading in Command Block Wrappers from the host, processing the SCSI commands they
 *  contain, and returning Command Status Wrappers back to the host to indicate the success or failure of the last issued command.
 */
void MassStorage_Task(void)
{
	/* Device must be connected and configured for the task to run */
	if (USB_DeviceState != DEVICE_STATE_Configured)
	  return;

	/* Process sent command block from the host if one has been sent */
	if (ReadInCommandBlock())
	{
		/* Indicate busy */
		LEDs_SetAllLEDs(LEDMASK_USB_BUSY);

		/* Check direction of command, select Data IN endpoint if data is from the device */
		if (CommandBlock.Flags & MS_COMMAND_DIR_DATA_IN)
		  Endpoint_SelectEndpoint(MASS_STORAGE_IN_EPADDR);

		/* Decode the received SCSI command, set returned status code */
		CommandStatus.Status = SCSI_DecodeSCSICommand() ? MS_SCSI_COMMAND_Pass : MS_SCSI_COMMAND_Fail;

		/* Load in the CBW tag into the CSW to link them together */
		CommandStatus.Tag = CommandBlock.Tag;

		/* Load in the data residue counter into the CSW */
		CommandStatus.DataTransferResidue = CommandBlock.DataTransferLength;

		/* Stall the selected data pipe if command failed (if data is still to be transferred) */
		if ((CommandStatus.Status == MS_SCSI_COMMAND_Fail) && (CommandStatus.DataTransferResidue))
		  Endpoint_StallTransaction();

		/* Return command status block to the host */
		ReturnCommandStatus();

		/* Indicate ready */
		LEDs_SetAllLEDs(LEDMASK_USB_READY);
	}

	/* Check if a Mass Storage Reset occurred */
	if (IsMassStoreReset)
	{
		/* Reset the data endpoint banks */
		Endpoint_ResetEndpoint(MASS_STORAGE_OUT_EPADDR);
		Endpoint_ResetEndpoint(MASS_STORAGE_IN_EPADDR);

		Endpoint_SelectEndpoint(MASS_STORAGE_OUT_EPADDR);
		Endpoint_ClearStall();
		Endpoint_ResetDataToggle();
		Endpoint_SelectEndpoint(MASS_STORAGE_IN_EPADDR);
		Endpoint_ClearStall();
		Endpoint_ResetDataToggle();

		/* Clear the abort transfer flag */
		IsMassStoreReset = false;
	}
}

/** Function to read in a command block from the host, via the bulk data OUT endpoint. This function reads in the next command block
 *  if one has been issued, and performs validation to ensure that the block command is valid.
 *
 *  \return Boolean \c true if a valid command block has been read in from the endpoint, \c false otherwise
 */
static bool ReadInCommandBlock(void)
{
	uint16_t BytesTransferred;

	/* Select the Data Out endpoint */
	Endpoint_SelectEndpoint(MASS_STORAGE_OUT_EPADDR);

	/* Abort if no command has been sent from the host */
	if (!(Endpoint_IsOUTReceived()))
	  return false;

	/* Read in command block header */
	BytesTransferred = 0;
	while (Endpoint_Read_Stream_LE(&CommandBlock, (sizeof(CommandBlock) - sizeof(CommandBlock.SCSICommandData)),
	                               &BytesTransferred) == ENDPOINT_RWSTREAM_IncompleteTransfer)
	{
		/* Check if the current command is being aborted by the host */
		if (IsMassStoreReset)
		  return false;
	}

	/* Verify the command block - abort if invalid */
	if ((CommandBlock.Signature         != MS_CBW_SIGNATURE) ||
	    (CommandBlock.LUN               >= TOTAL_LUNS)       ||
		(CommandBlock.Flags              & 0x1F)             ||
		(CommandBlock.SCSICommandLength == 0)                ||
		(CommandBlock.SCSICommandLength >  sizeof(CommandBlock.SCSICommandData)))
	{
		/* Stall both data pipes until reset by host */
		Endpoint_StallTransaction();
		Endpoint_SelectEndpoint(MASS_STORAGE_IN_EPADDR);
		Endpoint_StallTransaction();

		return false;
	}

	//serialWriteArray(msg, 21);

	/* Read in command block command data */
	BytesTransferred = 0;
	while (Endpoint_Read_Stream_LE(&CommandBlock.SCSICommandData, CommandBlock.SCSICommandLength,
	                               &BytesTransferred) == ENDPOINT_RWSTREAM_IncompleteTransfer)
	{
		/* Check if the current command is being aborted by the host */
		if (IsMassStoreReset)
		  return false;
	}

	/* Finalize the stream transfer to send the last packet */
	Endpoint_ClearOUT();

	return true;
}

/** Returns the filled Command Status Wrapper back to the host via the bulk data IN endpoint, waiting for the host to clear any
 *  stalled data endpoints as needed.
 */
static void ReturnCommandStatus(void)
{
	uint16_t BytesTransferred;

	/* Select the Data Out endpoint */
	Endpoint_SelectEndpoint(MASS_STORAGE_OUT_EPADDR);

	/* While data pipe is stalled, wait until the host issues a control request to clear the stall */
	while (Endpoint_IsStalled())
	{
		/* Check if the current command is being aborted by the host */
		if (IsMassStoreReset)
		  return;
	}

	/* Select the Data In endpoint */
	Endpoint_SelectEndpoint(MASS_STORAGE_IN_EPADDR);

	/* While data pipe is stalled, wait until the host issues a control request to clear the stall */
	while (Endpoint_IsStalled())
	{
		/* Check if the current command is being aborted by the host */
		if (IsMassStoreReset)
		  return;
	}

	/* Write the CSW to the endpoint */
	BytesTransferred = 0;
	while (Endpoint_Write_Stream_LE(&CommandStatus, sizeof(CommandStatus),
	                                &BytesTransferred) == ENDPOINT_RWSTREAM_IncompleteTransfer)
	{
		/* Check if the current command is being aborted by the host */
		if (IsMassStoreReset)
		  return;
	}

	/* Finalize the stream transfer to send the last packet */
	Endpoint_ClearIN();
}

