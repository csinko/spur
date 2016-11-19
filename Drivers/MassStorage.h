#ifndef _MASS_STORAGE_H_
#define _MASS_STORAGE_H_

extern "C"
{
# include "usbdrv.h"
};

//==============================================================================================
//  Mandatory SCSI commands and common optional SCSI commands
// for USB mass-storage devices that comply with SBC-2 or SBC-3.
//==============================================================================================
#define INQUIRY                       (unsigned char) 0x12
#define READ_FORMAT_CAPACITY          (unsigned char) 0x23
#define READ_CAPACITY                 (unsigned char) 0x25
#define READ_10                       (unsigned char) 0x28
#define WRITE_10                      (unsigned char) 0x2a
#define REQUEST_SENSE                 (unsigned char) 0x03
#define MODE_SENSE                    (unsigned char) 0x1a
#define TEST_UNIT_READY               (unsigned char) 0x00
#define VERIFY                        (unsigned char) 0x2F
#define PREVENT_ALLOW_MEDIUM_REMOVAL  (unsigned char) 0x1e

//==============================================================================================
//The Command Block Wrapper
//==============================================================================================
typedef struct
{
  unsigned long dCBWSignature;
  unsigned long dCBWTag;
  unsigned long dCBWDataTransferLength;
  unsigned char bCBWFlags;
  unsigned char bCBWLUN;
  unsigned char bCBWCBLength;
  unsigned char CBWCB[16];
} USB_MSD_CBW;

//==============================================================================================
//The Command Status Wrapper
//==============================================================================================
typedef struct
{
  unsigned long dCSWSignature;
  unsigned long dCSWTag;
  unsigned long dCSWDataResidue;
  unsigned char bCSWStatus;
} USB_MSD_CSW;

//==============================================================================================
//The Inquiry Response
//==============================================================================================
typedef struct
{
    unsigned char Peripheral;
    unsigned char Removble;
    unsigned char Version;
    unsigned char Response_Data_Format;
    unsigned char AdditionalLength;
    unsigned char Sccstp;
    unsigned char bqueetc;
    unsigned char CmdQue;
    unsigned char vendorID[8];
    unsigned char productID[16];
    unsigned char productRev[4];
} CInquiryResponse;


//==============================================================================================
enum {txsIdle, txsData, txsCSW};
enum {rxsCBW, rxsData};

//==============================================================================================
void MSD_Prepare();

unsigned char MSD_GetRxStage();
void MSD_RxCBW(unsigned char* ucData, unsigned char ucLen);
void MSD_RxData(unsigned char* ucData, unsigned char ucLen);

unsigned char MSD_GetTxStage();
void MSD_TxData();
void MSD_TxCSW();

#endif //_MASS_STORAGE_H_
