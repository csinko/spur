#include "MassStorage.h"
#include "utils.h"
#include "mmc.h"
#include "board.h"

//==============================================================================================
__flash char usbDescriptorDevice[] = {    	            /* USB дескриптор устройства */
    0x12,         /* sizeof(usbDescriptorDevice): длина устройства в байтах */
    USBDESCR_DEVICE,        				    /* тип дескриптора */
    0x10, 0x01,             				    /* поддерживаема€ верси€ USB */
    0x00,
    0x00,
    0x00,                      				    /* протокол */
    0x08,                      				    /* max размер пакета */
    /* следующие два преобразовани€ типа (cast) вли€ют только на первый байт константы, но
     * это важно дл€ того, чтобы избежать предупреждени€ (warning) с величинами по умолчанию.
     */
    (unsigned char)USB_CFG_VENDOR_ID,			    /* 2 байта */
    (unsigned char)USB_CFG_DEVICE_ID,			    /* 2 байта */
    USB_CFG_DEVICE_VERSION, 				    /* 2 байта */
    USB_CFG_DESCR_PROPS_STRING_VENDOR != 0 ? 1 : 0,         /* индекс строки производител€ */
    USB_CFG_DESCR_PROPS_STRING_PRODUCT != 0 ? 2 : 0,        /* индекс строки продукта */
    USB_CFG_DESCR_PROPS_STRING_SERIAL_NUMBER != 0 ? 3 : 0,  /* индекс строки серийного номера */
    1,          																						/* количество конфигураций */
};

//==================================================================================
__flash char usbDescriptorConfiguration[] = {    /* USB дескриптор конфигурации */
    0x09,                         /* sizeof(usbDescriptorConfiguration): длина устройства в байтах    */
    USBDESCR_CONFIG,              /* тип дескриптора                                                  */
    0x20, 0x00,                   /* обща€ длина возвращаемых данных (включа€ встроенные дескрипторы) */
    0x01,                         /* количество интерфейсов в этой конфигурации                       */
    0x01,                         /* индекс этой конфигурации                                         */
    0x00,                         /* индекс строки имени конфигурации                                 */
    0x80,                         /* self/bus powered and remote wakeup attribute                     */
    USB_CFG_MAX_BUS_POWER/2,      /* max ток USB в единицах 2mA                                       */
/* дескриптор интерфейса следует встроенным (inline): */
    0x09,                         /* sizeof(usbDescrInterface): длина дескриптора в байтах            */
    USBDESCR_INTERFACE,           /* тип дескриптора                                                  */
    0x00,                         /* индекс этого интерфейса                                          */
    0x00,                         /* альтернативна€ установка этого интерфейса                        */
    0x02,                         /* конечные точки за исключением 0: 2                               */
    USB_CFG_INTERFACE_CLASS,
    USB_CFG_INTERFACE_SUBCLASS,
    USB_CFG_INTERFACE_PROTOCOL,
    0x00,                         /* индекс строки дл€ интерфейса                                     */
/* дескриптор конечной точки дл€ конечной точки 1 */
    0x07,                         /* sizeof(usbDescrEndpoint)                                         */
    USBDESCR_ENDPOINT,            /* тип дескриптора = конечна€ точка                                 */
    0x81,                         /* IN endpoint номер 1                                              */
    0x02,                         /* атрибут: конечна€ точка bulk                                     */
    0x08, 0x00,                   /* max размер пакета                                                */
    0x00,                         /* в ms                                                             */
/* дескриптор конечной точки дл€ конечной точки 3 */
    0x07,                         /* sizeof(usbDescrEndpoint)                                         */
    USBDESCR_ENDPOINT,            /* тип дескриптора = конечна€ точка                                 */
    0x02,                         /* IN endpoint номер 3                                              */
    0x02,                         /* атрибут: конечна€ точка bulk                                     */
    0x08, 0x00,                   /* max размер пакета                                                */
    0x00,                         /* в ms                                                             */
};

//==============================================================================================
__flash int usbDescriptorStringSerialNumber[] =
{
  (2*(USB_CFG_SERIAL_NUMBER_LEN)+2) | (3<<8),
  USB_CFG_SERIAL_NUMBER
};

//==============================================================================================
__flash int usbDescriptorStringVendor[] =
{
  (2*(USB_CFG_VENDOR_NAME_LEN)+2) | (3<<8),
  USB_CFG_VENDOR_NAME
};

//==============================================================================================
__flash int usbDescriptorStringDevice[] =
{
  (2*(USB_CFG_DEVICE_NAME_LEN)+2) | (3<<8),
  USB_CFG_DEVICE_NAME
};

//==============================================================================================
const unsigned char __flash usb_format_capacity[] =
{
  0x00, 0x00, 0x00, 0x08,
  0x00, 0xF2, 0x38, 0x00,
  0x02,
  0x00, 0x02, 0x00
};

//==============================================================================================
const unsigned char __flash usb_read_capacity[] = {0x00, 0x0f, 0x23, 0x7F, 0x00, 0x00, 0x02, 0x00};

//==============================================================================================
const unsigned char __flash usb_mode_sense[] = {0x03, 0x00, 0x00, 0x00};

//==============================================================================================
const unsigned char __flash usb_request_sense[] = {0x70, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00};

//==============================================================================================
const __flash CInquiryResponse inq_resp =
{
  0x00,                                                             // direct access block device, conne
  0x80,                                                             // device is removable
  0x00,                                                             // SPC-2 compliance
  0x01,                                                             // response data format
  0x1F,                                                             // response has 20h + 4 bytes
  0x00, 0x00, 0x00,                                                 // additional fields, none set
  'A','T','m','e','g','a',' ','8',                                  // 8 -byte T10-assigned Vendor ID
  'V','-','U','S','B',' ','S','D','-','A','d','a','p','t','e','r',  // 16-byte product identification
  '0','0','0','0'                                                   // 4-byte product revision level
};

//=================================================================================
void MSD_PareseCBW();

//=================================================================================
USB_MSD_CSW csw;
USB_MSD_CBW cbw;

unsigned char eTxStage = txsIdle;
unsigned char eRxStage = rxsCBW;

static unsigned int uiRxBytes = 0x00;

static unsigned int ucPos = 0x00;
static unsigned int ucSize = 0x00;

static unsigned long ulAddr = 0x00;
static unsigned int uiBlockNum = 0x00;
static unsigned char ucDataPBP[512];


void MSD_Prepare()
{
  csw.dCSWSignature = 0x53425355;
}

unsigned char MSD_GetRxStage()
{
  return eRxStage;
}

unsigned char MSD_GetTxStage()
{
  return eTxStage;
}

void MSD_RxCBW(unsigned char* ucData, unsigned char ucLen)
{
  x_memcpy((unsigned char*)&cbw + uiRxBytes, ucData, ucLen);
  uiRxBytes+= ucLen;

  if (uiRxBytes == sizeof(USB_MSD_CBW))
  {
    MSD_PareseCBW();
  }
}

void MSD_PareseCBW()
{
  // Set Command status wrap TAG namber as command block wrap TAG
  csw.dCSWTag = cbw.dCBWTag;

  //CSW status of input command OK (by default), clear rx bytes count
  csw.bCSWStatus = csw.dCSWDataResidue = uiRxBytes = 0x00;

  // Now CBW is recieved and we must transmit data
  eTxStage = txsData;

  switch(cbw.CBWCB[0])
  {
    case READ_10:
      ucSize = 512;

      uiBlockNum = ((unsigned int)cbw.CBWCB[7] << 8) + cbw.CBWCB[8] - 1;
      ulAddr = 512 * (((unsigned long)cbw.CBWCB[2] << 24) + ((unsigned long)cbw.CBWCB[3] << 16) + ((unsigned int)cbw.CBWCB[4] << 8) + cbw.CBWCB[5]);
      MMC_ReadBlock(ulAddr, (unsigned char*)&ucDataPBP);
      ledGreenToogle();
      break;
      
    case WRITE_10:
      eTxStage = txsIdle;
      eRxStage = rxsData;
      ucSize = 0x00;

      uiBlockNum = ((unsigned int)cbw.CBWCB[7] << 8) + cbw.CBWCB[8];
      ulAddr = 512 * (((unsigned long)cbw.CBWCB[2] << 24) + ((unsigned long)cbw.CBWCB[3] << 16) + ((unsigned int)cbw.CBWCB[4] << 8) + cbw.CBWCB[5]);
      break;
      
    case INQUIRY:
      x_memcpyff((unsigned char*)&ucDataPBP, (const unsigned char __flash*)&inq_resp, sizeof(inq_resp));
      ucSize = sizeof(inq_resp);
      break;

    case READ_CAPACITY:
      x_memcpyff((unsigned char*)&ucDataPBP, (const unsigned char __flash*)&usb_read_capacity, sizeof(usb_read_capacity));
      ucSize = sizeof(usb_read_capacity);
      break;

    case READ_FORMAT_CAPACITY:
      x_memcpyff((unsigned char*)&ucDataPBP, (const unsigned char __flash*)&usb_format_capacity, sizeof(usb_format_capacity));
      ucSize = sizeof(usb_format_capacity);
      break;

    case TEST_UNIT_READY:
      ledGreenToogle();
      ucSize = 0x00;
      break;

    case MODE_SENSE:
      x_memcpyff((unsigned char*)&ucDataPBP, (const unsigned char __flash*)&usb_mode_sense, sizeof(usb_mode_sense));
      ucSize = sizeof(usb_mode_sense);
      break;

    case REQUEST_SENSE:
      x_memcpyff((unsigned char*)&ucDataPBP, (const unsigned char __flash*)&usb_request_sense, sizeof(usb_request_sense));
      ucSize = sizeof(usb_request_sense);
      break;

    default:
      ucSize = 0x00;
      csw.bCSWStatus = 0x01;
      break;
  }
}

void MSD_RxData(unsigned char* ucData, unsigned char ucLen)
{
  x_memcpy((unsigned char*)&ucDataPBP + uiRxBytes, ucData, ucLen);
  uiRxBytes+=ucLen;

  if (512 == uiRxBytes)
  {

    uiRxBytes = 0;
    MMC_WriteBlock(ulAddr, ucDataPBP);
    ulAddr+=512;
    uiBlockNum--;
    if (uiBlockNum == 0x00)
    {
      eRxStage = rxsCBW;
      eTxStage = txsCSW;
    }
  }
}

void MSD_TxData()
{
  if (usbInterruptIsReady())
  {
    unsigned char ucBR = 8;
    unsigned char res = 0x00;
    unsigned int ucChunkSize = ucSize - ucPos;

    if (ucChunkSize < 8)
    {
      if (uiBlockNum)
      {
        ucPos = 0x00;
        ulAddr += 512;
        uiBlockNum--;
        MMC_ReadBlock(ulAddr, (unsigned char*)&ucDataPBP);
      }
      else
      {
        ucBR = ucChunkSize;
        res = 1;
      }
    }
    
    if (ucBR)
    {
      usbSetInterrupt((unsigned char*)&ucDataPBP[ucPos], ucBR);
    }

    if (res)
    {
      ucPos = ucSize = 0x00;
      eTxStage = txsCSW;
    }
    else
    {
      ucPos+= ucBR;
    }
  }
}

void MSD_TxCSW()
{
  unsigned char ucBR = 8;

  if (usbInterruptIsReady())
  {
    if (ucPos)
    {
      ucBR = 5;
    }

    usbSetInterrupt((unsigned char*)&csw + ucPos, ucBR);

    if (ucPos)
    {
      ucPos = 0x00;
      eTxStage = txsIdle;
    }
    else
    {
      ucPos = 8;
    }
  }
}
