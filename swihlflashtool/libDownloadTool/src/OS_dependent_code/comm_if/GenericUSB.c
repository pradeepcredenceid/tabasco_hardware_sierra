/*
 *       Copyright (C) 2011-2014 Intel Mobile Communications GmbH
 *
 *            Sec Class: Intel Confidential (IC)
 *
 *
 * This document contains proprietary information belonging to IMC.
 * Design A/S. Passing on and copying of this document, use and communication
 * of its contents is not permitted without prior written authorisation.
 *
 * Description:
 *   <hardcoded file description - the only part to be modified manually!>
 *
 * Revision Information:
 *  $File name:  /msw_tools/FlashTool/libDownloadTool/src/OS_dependent_code/comm_if/GenericUSB.c $
 *  $CC-Version: .../dhkristx_140314_v0/5 $
 *  $Date:       2014-05-02    13:10:06 UTC $
 */

#define THIS_MODULE MODULE_USB

//#define DISABLE_ANIMATION

#include "GenericUSB.h"
#include "DebugModule.h"

#include "INI.h"
#include "FileIO.h"
#include "devices.h"          //< DEVICE_get_imc_name()
#include "IFWD_global_func.h" //< GLOBAL_usb_device

#include <stdio.h>
#include <string.h>
#include <errno.h>            //< ETIMEDOUT

#include <dirent.h>           //< opendir()
#include <sys/param.h>        //< MAXPATHLEN

#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>

#ifndef WIN32
  #include "SysFs.h"
  #include <sys/inotify.h>
#else
  #include "WinRegistry.h"
#endif

//#define USB_IDS "/var/lib/usbutils/usb.ids"

#define ERROR_LIMIT      20
#define TIMEOUT          500

//< From IFWD_Comport.c
//#define OUT_BUFFER_SIZE                  (16384 * 2)
//#define IN_BUFFER_SIZE                   (16384 * 2)

//< From IFWD_global_func.h
#ifndef MAX_NUMBER_OF_CHANNELS
  #define MAX_NUMBER_OF_CHANNELS       32
#endif

/*****************************************************************************
*                               STRUCTURES                                  *
*****************************************************************************/

#ifndef DISABLE_LIBUSB
typedef enum
{
  USB_STATUS_CLOSED = 0,
  USB_STATUS_IDLE,
  USB_STATUS_READING,
  USB_STATUS_WRITING,
  USB_STATUS_ERROR
} libusb_status_t;


typedef struct
{
  // Public interface //
  comport_handler_t              port;

  // Private interface //
  int                       id;              //< Reference ID
  uint16_t                  busnum;          //< Current USB bus number
  uint16_t                  devnum;          //< Current device number
  char                      *sysname;        //< System USB path (Physically)
  char                      *devname;        //< Device USB path (Logically)
  int                       vendor;          //< For later validation
  int                       product;         //< For later validation
  uint8_t                   driver;         //< Re-attach driver after use (Linux Only)

  int                       interfaceNr;     //< Interface number
  uint32_t                  max_transfer;    //< Determining when to send Zero-length Packets (wMaxPacketSize)
  uint8_t                   zlp_enabled;     //< Disabled during PSI upload

  DevHandle                 dev;            //< Driver device (Linux Only)
  CommHandleType            usbHandle;       //< Open Device Handler
  CommHandleType            ctrlHandle;      //< Control Device Handler (Windows Only)

  int                       in;              //< Bulk Input endpoint
  int                       out;             //< Bulk Output endpoint
  int                       control;         //< Interrupt Control endpoint

  uint32_t                  bytesWritten;
  uint32_t                  bytesRead;

  libusb_status_t           status;          //< Current status
  comport_status_t          comport_status;  //< Used to inform above layer
  int                       last_error;
  int                       error;

  comm_error_report_t       error_report;    //< Error function
} libusb_dev_t;

/*****************************************************************************
*                             GenericUSB HANDLERS                                *
*****************************************************************************/

#ifndef WIN32
  static CtxHandle ctx = NULL;

  #ifdef DEBUG
    static int usb_debug = 3;
  #else
    static int usb_debug = 1;
  #endif

#else
  static GUID phone_guid = { 0 };
#endif

static int ptr = 0;
static int examined[100];   //< List of already examined devnum's
static libusb_dev_t *DeviceMap[MAX_NUMBER_OF_CHANNELS+1];

uint8_t libusb_initialized = false;

/*****************************************************************************
*                             LOCAL PROTOTYPES                              *
*****************************************************************************/

static libusb_dev_t* UsbMapper_new_device(int id, const char* sysname);
// Unused : static uint8_t UsbMapper_free_device(int id);

static int UsbMapper_get_sysname_id(const char* sysname);
// Unused : static void UsbMapper_dump_map();

static uint8_t GenericUSB_open_device(libusb_dev_t *device, const char* dev);

static uint8_t  GenericUSB_set_baudrate(uint8_t id, uint32_t baudrate);
static uint8_t  GenericUSB_get_modem_status(uint8_t id,
                                    comport_modem_status_line_t *modem_status_line);
//comport_handler_t *GenericUSB_open(uint8_t id, comport_cfg_t *comport_cfg,
//                                  comm_error_report_t error_report)
//static uint8_t  GenericUSB_close(uint8_t id);

static uint8_t  GenericUSB_set_buffer_size(uint8_t id, uint32_t rx_buf_size, uint32_t tx_buf_size);
static uint32_t GenericUSB_get_rx_used_buffer_size(uint8_t id);
static uint8_t  GenericUSB_set_rts(uint8_t id, uint8_t new_rts);
static uint8_t  GenericUSB_set_dtr(uint8_t id, uint8_t new_dtr);
static uint8_t  GenericUSB_flush_rx_buffer(uint8_t id);
static uint8_t  GenericUSB_flush_tx_buffer(uint8_t id);
static comport_error_t  GenericUSB_get_last_error(uint8_t id);
static comport_status_t GenericUSB_get_status(uint8_t id);
static void GenericUSB_process_communication(uint8_t id);
//static void GenericUSB_close_event_handler(libusb_dev_t *libusb_dev_temp);

#ifdef WIN32
  /* Declaring the WinUSB functions */
  DLL_DECLARE(WINAPI, uint8_t, WinUsb_Initialize, (HANDLE, PVOID));
  DLL_DECLARE(WINAPI, uint8_t, WinUsb_GetDescriptor, (HANDLE, UCHAR, UCHAR, USHORT, PUCHAR, ULONG, PULONG));
  DLL_DECLARE(WINAPI, uint8_t, WinUsb_QueryDeviceInformation, (HANDLE, ULONG, PULONG, PVOID));
  DLL_DECLARE(WINAPI, uint8_t, WinUsb_GetAssociatedInterface, (HANDLE, UCHAR, PVOID));
  DLL_DECLARE(WINAPI, uint8_t, WinUsb_QueryInterfaceSettings, (HANDLE, UCHAR, PUSB_INTERFACE_DESCRIPTOR));

  DLL_DECLARE(WINAPI, uint8_t, WinUsb_QueryPipe, (HANDLE, UCHAR, UCHAR, PVOID));
  DLL_DECLARE(WINAPI, uint8_t, WinUsb_GetPipePolicy, (HANDLE, UCHAR, ULONG, PULONG, PVOID));
  DLL_DECLARE(WINAPI, uint8_t, WinUsb_SetPipePolicy, (HANDLE, UCHAR, ULONG, ULONG, PVOID));

  DLL_DECLARE(WINAPI, uint8_t, WinUsb_ReadPipe, (HANDLE, UCHAR, PUCHAR, ULONG, PULONG, LPOVERLAPPED));
  DLL_DECLARE(WINAPI, uint8_t, WinUsb_WritePipe, (HANDLE, UCHAR, PUCHAR, ULONG, PULONG, LPOVERLAPPED));
  DLL_DECLARE(WINAPI, uint8_t, WinUsb_FlushPipe, (HANDLE, UCHAR));
  DLL_DECLARE(WINAPI, uint8_t, WinUsb_Free, (HANDLE));
#endif

/*****************************************************************************
*                            IMPLEMENTATION                                 *
*****************************************************************************/

static void process_animation()
{
#ifndef DISABLE_ANIMATION
  static size_t animation_state = 0;
  static char* animation = "/-\\|";
  printf("%c\r", animation[animation_state++]);
  fflush(stdout);
  if (animation_state > sizeof(animation)-1)
    animation_state = 0;
#endif
}


#ifdef WIN32

/*** Helper functions ***/
static char* guid_to_string(const GUID* guid)
{
  static char strGUID[40];   //< 38 + 1
  strGUID[0] = 0;
  sprintf(strGUID, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
          (uint32_t)guid->Data1, (uint32_t)guid->Data2, (uint32_t)guid->Data3,
          guid->Data4[0], guid->Data4[1], guid->Data4[2], guid->Data4[3],
          guid->Data4[4], guid->Data4[5], guid->Data4[6], guid->Data4[7]);
  return strGUID;
}

static uint8_t string_to_guid(const char* str, GUID* guid)
{
  if (!str)
    return false;

  uint32_t p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10 = 0;
  int res = sscanf(str, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
                   &p0, &p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10);
  if (res == 11)
  {
    guid->Data1 = p0;
    guid->Data2 = p1;
    guid->Data3 = p2;
    guid->Data4[0] = p3;
    guid->Data4[1] = p4;
    guid->Data4[2] = p5;
    guid->Data4[3] = p6;
    guid->Data4[4] = p7;
    guid->Data4[5] = p8;
    guid->Data4[6] = p9;
    guid->Data4[7] = p10;
    return true;
  }
  return false;
}

// TODO: Moved to OS.c
static const char* GetUsbErrorString(uint8_t error_code)
{
  static char strError[256];
  strError[0] = 0;
  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, GetLastError(),
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (char*)strError, 256, 0);
  if (error_code) {
    char* nl = strchr(strError, '\r');
    if (nl) nl = '\0';
    sprintf(strError, "%s (%d)", strError, (int)GetLastError());
  }
  return strError;
}

/*** WinUSB Helper functions ***/
uint8_t GenericUSB_set_GUID(const char* guid_str)
{
  return string_to_guid(guid_str, &phone_guid);
}

/**
 *  UsbMapper_get_sysname()
 *
 *  Obtain the Device System path/name
 *
 **/
uint8_t UsbMapper_get_sysname(const GUID *InterfaceGuid, char* DevicePath, size_t BufLen, int i)
{
  HDEVINFO deviceInfo;
  SP_DEVICE_INTERFACE_DATA interfaceData;
  PSP_DEVICE_INTERFACE_DETAIL_DATA detailData = NULL;
  ULONG length;
  ULONG requiredLength=0;

  static int issue=0;

  // [1] libusb.h does this:
  //   HDEVINFO *dev_info;
  //   *dev_info = pSetupDiGetClassDevsA(NULL, "USB", NULL, DIGCF_PRESENT|DIGCF_ALLCLASSES);
  deviceInfo = SetupDiGetClassDevs(InterfaceGuid,
                     NULL, NULL,
                     DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
  if (deviceInfo == INVALID_HANDLE_VALUE)
  {
    if (issue!=1)
      PrintError("Could not SetupDiGetClassDevs: %s\n"
                 "\t GUID: %s\n", GetUsbErrorString(true), guid_to_string(InterfaceGuid) );
    issue = 1;
    return false;
  }

// [2]
  interfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
  if (!SetupDiEnumDeviceInterfaces(deviceInfo,
                                   NULL,
                                   InterfaceGuid,
                                   i,
                                   &interfaceData))
  {
    if (GetLastError() == ERROR_NO_MORE_ITEMS)
    {
      SetupDiDestroyDeviceInfoList(deviceInfo);
      return false;
    }

    if (issue!=2)
      PrintError("Could not SetupDiEnumDeviceInterfaces: %s\n", GetUsbErrorString(true));
    issue=2;
    SetupDiDestroyDeviceInfoList(deviceInfo);
    return false;
  }

// [3]
  if (!SetupDiGetDeviceInterfaceDetail(deviceInfo,
                                  &interfaceData,
                                  NULL, 0,
                                  &requiredLength,
                                  NULL))
  {
    if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
    {
      if (issue!=3)
        PrintError("Could not SetupDiGetDeviceInterfaceDetail (Len: %lu): %s\n",
                   requiredLength, GetUsbErrorString(true));
      issue=3;
      SetupDiDestroyDeviceInfoList(deviceInfo);
      return false;
    }
  }

  detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(requiredLength);
  if(!detailData)
  {
    PrintError("Could not malloc detailData: %d\n", GetLastError());
    SetupDiDestroyDeviceInfoList(deviceInfo);
    return false;
  }
  detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
  length = requiredLength;

// [4]
  if (!SetupDiGetDeviceInterfaceDetail(deviceInfo,
                                           &interfaceData,
                                           detailData,
                                           length,
                                           &requiredLength,
                                           NULL))
  {
    if (issue!=5)
      PrintError("Could not SetupDiGetDeviceInterfaceDetail: %s\n", GetUsbErrorString(true));
    issue=5;
    SetupDiDestroyDeviceInfoList(deviceInfo);
    free(detailData);
    return false;
  }

  if (strncpy(DevicePath, detailData->DevicePath, BufLen) == NULL)
  {
    PrintError("Could not copy device path: %s\n", GetUsbErrorString(true));
    SetupDiDestroyDeviceInfoList(deviceInfo);
    free(detailData);
    return false;
  }

  SetupDiDestroyDeviceInfoList(deviceInfo);
  free(detailData);

  return true;
}

#else

static const char* GetUsbErrorString(int error_code)
{
  #ifndef LIBUSBHOST
    switch(error_code)
    {
    case LIBUSB_SUCCESS:             return "Success";
    case LIBUSB_ERROR_IO:            return "Input/output error";
    case LIBUSB_ERROR_INVALID_PARAM: return "Invalid parameter";
    case LIBUSB_ERROR_ACCESS:        return "Access denied";
    case LIBUSB_ERROR_NO_DEVICE:     return "Device disconnected";
    case LIBUSB_ERROR_NOT_FOUND:     return "Entity not found";
    case LIBUSB_ERROR_BUSY:          return "Resource busy";
    case LIBUSB_ERROR_TIMEOUT:       return "Timeout";
    case LIBUSB_ERROR_OVERFLOW:      return "Overflow";
    case LIBUSB_ERROR_PIPE:          return "Pipe error";
    case LIBUSB_ERROR_INTERRUPTED:   return "Interrupted";
    case LIBUSB_ERROR_NO_MEM:        return "Insufficient memory";
    case LIBUSB_ERROR_NOT_SUPPORTED:
      return "Operation not implemented on this platform";
    case LIBUSB_ERROR_OTHER:         break;
    default:                         break;
    }
  #endif

  return strerror(errno);
}

char *UsbMapper_get_sysname(int busnum, int devnum)
{
  DIR             *dir;
  FILE            *file;
  struct dirent   *entry;
  char            *syspath = NULL;
  char            spath[MAXPATHLEN] = "";

  if(!(dir = opendir(USB_SYS_DIR))) { perror("opendir"); return NULL; }
  for(entry = readdir(dir); entry; entry = readdir(dir))
  {
    if(entry->d_type == DT_LNK              //< Sysnames are symlinks
      && isdigit(entry->d_name[0])          //< Catching USB device symlinks
      && atoi(&entry->d_name[0]) == busnum  //< Catching the correct USB bus
      && !strchr(entry->d_name, ':'))       //< Catching all the 'usb_device' types
    {
      snprintf(spath, sizeof(spath), "%s/%s/devnum", USB_SYS_DIR, entry->d_name);

      //printf("Opening file %s\n", spath);
      file = fopen(spath, "r");
      if (file)
      {
        int num;
        fscanf(file, "%d", &num);
        fclose(file);

        //printf(" -> %s (%u == %u)\n", spath, num, devnum);
        if ((int)devnum == num)
        {
          syspath = strdup( entry->d_name );
          break;
        }
      }
    }
  }

  closedir(dir);
  return syspath;
}

#endif

static const char* GetDeviceClassName(int device_class)
{
  static char code[12];

  switch (device_class)
  {
    case 0x02: return "Comm";
    case 0x03: return "HID";
    case 0x05: return "Physical Endpoint";
    case 0x08: return "Mass Storage";
    case 0x09: return "Hub";
    case 0x0A: return "Data";
    case 0x0B: return "Smart Card";
    case 0x0D: return "Content Security";
    case 0xE0: return "Wireless";
    case 0xDC: return "Diagnostic Device";
    case 0xFE: return "Application";
    case 0xFF: return "Vendor Specific";
    default:
      sprintf(code, "0x%02X", device_class);
      break;
  }

  return (const char*)&code;
}


/**
 * GenericUSB_init()
 */
void GenericUSB_init()
{
  int i;
  if (libusb_initialized)
    return;

  #ifndef WIN32
    #ifdef LIBUSBHOST
      Debug2(" > Initializing GenericUSB (libusbhost)...\n");
      ctx = usb_host_init(); //< Note: Only needed for inotify
    #else
      Debug2(" > Initializing GenericUSB (libusb-1.0)...\n");
      i = libusb_init(&ctx);
      if (i < 0) {
        PrintError("libusb failed to initialize: %d\n", i);
        return;
      }
      /* usb_set_debug can be called before usb_init */
      if (usb_debug)
        libusb_set_debug(ctx, usb_debug);
    #endif
  #else
    Debug2(" > Initializing GenericUSB (WinUSB)...\n");
    if (!phone_guid.Data1)
      memcpy(&phone_guid, DEVICE_get_guid(), sizeof(GUID));
    Debug2(" > WinUSB GUID: %s\n", guid_to_string(&phone_guid));

    DLL_LOAD("winusb.dll", WinUsb_Initialize, true);
    DLL_LOAD("winusb.dll", WinUsb_GetDescriptor, true);
    DLL_LOAD("winusb.dll", WinUsb_GetAssociatedInterface, true);
    DLL_LOAD("winusb.dll", WinUsb_QueryInterfaceSettings, true);
    DLL_LOAD("winusb.dll", WinUsb_QueryDeviceInformation, true);
    DLL_LOAD("winusb.dll", WinUsb_QueryPipe, true);
    DLL_LOAD("winusb.dll", WinUsb_SetPipePolicy, true);
    DLL_LOAD("winusb.dll", WinUsb_GetPipePolicy, true);
    DLL_LOAD("winusb.dll", WinUsb_ReadPipe, true);
    DLL_LOAD("winusb.dll", WinUsb_WritePipe, true);
    DLL_LOAD("winusb.dll", WinUsb_FlushPipe, true);
    DLL_LOAD("winusb.dll", WinUsb_Free, true);
  #endif

  for(i=0; i<MAX_NUMBER_OF_CHANNELS; ++i)
    DeviceMap[i] = NULL;

  ptr = 0;
  memset(examined, 0, 100);
  libusb_initialized = true;
}

/**
 * GenericUSB_destroy()
 */
void GenericUSB_destroy()
{
  if (!libusb_initialized)
    return;
  #ifdef WIN32
    Debug2(" > De-Initializing GenericUSB (WinUSB)...\n");
  #else
    #ifdef LIBUSBHOST
      Debug2(" > De-Initializing GenericUSB (libusbhost)...\n");
      usb_host_cleanup(ctx);
    #else
      Debug2(" > De-Initializing GenericUSB (libusb-1.0)...\n");
      libusb_exit(ctx);
    #endif
  #endif

  libusb_initialized = false;
}

/**
 * GenericUSB_examined()
 */
#ifndef WIN32
static uint8_t GenericUSB_examined(int busnum, int devnum)
{
  int i;
  int unique_id = busnum*1000 + devnum;

  for(i=0; examined[i]; ++i)
    if (examined[i] == unique_id)
      return true;

  examined[ptr++] = unique_id;
  if (ptr==100)
    ptr = 0;

  return false;
}
#endif


#ifdef LIBUSBHOST
/**
 * usb_added()
 *
 * libusbhost Callback function for when devices are added.
 *
 * TODO: Use iNotify with libusb and make this the default way...
 */
static int GenericUSB_added_cb(const char *devname, void* client_data)
{
  const char* name;
  char *sysname;
  uint16_t vid = 0, pid = 0;
  int busnum, devnum;

  if (!devname || !fexists(devname))
    return 0;

  if (sscanf(devname, USB_DEV_DIR "/%03d/%03d", &busnum, &devnum) != 2)
  {
    PrintError("Invalid device name '%s'\n", devname);
    return 0;
  }

  Debug("Device Added (UsbPath: %s)\n", devname);
  if (GenericUSB_examined(busnum, devnum))
    return 0;

  //printf(" Getting sysname from %d...\n", devnum);
  sysname = UsbMapper_get_sysname(busnum, devnum);
  if (!sysname) {
    PrintError(" Could not find sysname for devnum %d on bus %d!\n", devnum, busnum);
    return 0;
  }

  SysFs_get_ids(sysname, &vid, &pid);

  Debug2(" Looking up %s (%04X:%04X)\n", devname, vid, pid);
  if (!(name = DEVICE_get_imc_name(vid, pid, NO_ID, NO_ID))) {
    // Sierra workaround to display reboot message for HL modem
    if(vid == 0x1519) {
      //Sierra HL modem found in normal mode   
      return 2;
    }
    return 0;
  }

  Debug(" Device %s (%04X:%04X) Online '%s' !\n", name, vid, pid, sysname);

  //< Save new device in Comport Device Table
  libusb_dev_t** devptr = (libusb_dev_t**)client_data;
  libusb_dev_t* device = UsbMapper_new_device(-1, sysname);

  device->busnum = busnum;
  device->devnum = devnum;

  Debug("Opening device (ID: %d)\n", device->id);
  if ( !GenericUSB_open_device(device, devname) )
  {
    free(sysname);
    PrintError(" Could not open device %d :'(\n", device->id);
    return 0;
  }

  Debug2(" %s open.\n", name);

  if (device->devname)
    free(device->devname);

  if (!device->sysname)
    device->sysname = strdup(sysname);

  device->devname = strdup(devname);
  device->status = USB_STATUS_IDLE; //< Ready for I/O

  //device->error_report = error_report;
  *devptr = device;

  free(sysname);
  return 1;
}

/**
 * usb_removed()
 **/
static int GenericUSB_removed_cb(const char *devname, void* client_data)
{
  if (!devname)
    return 0;
  Debug(" Device Removed (UsbPath: %s)\n", devname);
  return 0;
}

#endif

/**
 * GenericUSB_open()
 *
 * This function is being called until a device is detected and opened.
 *
 * TODO: New task for this function!
 *    Task: Make sure the DeviceWatcher is running
 *       - Mutex DeviceMapMutex
 *       - Condition DeviceDetected
 *       - Windows implementation via (RegisterDeviceNotification)
 *        (/media/tmp/jmb/eventPortDetection)
 */
comport_handler_t *GenericUSB_open(uint8_t id, comport_cfg_t *comport_cfg,
                                  comm_error_report_t error_report)
{
  const char* name = NULL;
  char devname[MAX_PATH+1];
  char sysname[MAX_PATH+1];
  int channel;
  libusb_dev_t  *device = NULL;
  uint32_t pid = 0, vid = 0;

  if (!comport_cfg || id != comport_cfg->id) {
    PrintError("Comport ID is Not equal to ID!?\n");
    return NULL;
  }

  if (!comport_cfg->comport_name                  //< No comport defined
      || (comport_cfg->id < 1)                    //< Comport ID out of range
      || (comport_cfg->id > MAX_NUMBER_OF_CHANNELS)) {
    PrintError("Comport ID is out of range!\n");
    return NULL;
  }

  GenericUSB_init();

  // Closing if it was previously opened
  if (DeviceMap[id])
  {
    device = DeviceMap[id];
    if (device->usbHandle)
    {
      if (device->last_error == EBUSY)
        PrintError(" USB Handle already open... Closing \n");

      device->last_error = EBUSY;
      GenericUSB_close(id);
    }
  }

  #ifdef LIBUSBHOST
    char *comport_name = (char*)comport_cfg->comport_name;

    if (strncmp(USB_DEV_DIR, comport_name, 12) == 0)
    {
      Debug(" -> %s is a devpath!\n", comport_name);
      if (!GenericUSB_added_cb(comport_name, &device))
        return NULL;
    }
    else
    if (!device || device->status == USB_STATUS_CLOSED)
    {
      Debug("Waiting for inotify to find the device...\n");
      usb_host_run(ctx, GenericUSB_added_cb, GenericUSB_removed_cb,
                   NULL, &device);
    }
    Debug("  I/O Pipes:\n\t In: 0x%02X, Out: 0x%02X, Control: 0x%02X\n",
          device->in, device->out, device->control);

    /* Export only the public interface */
    return (comport_handler_t *)device;
  #else

#ifdef WIN32
  if (!UsbMapper_get_sysname(&phone_guid, sysname, sizeof(char)*MAX_PATH, id-1))
    return NULL;

  Debug("  Detected system name %s!\n", sysname);
  if (WinUSB_get_vid_pid(sysname, &vid, &pid))
  {
    name = DEVICE_get_imc_name(vid, pid, NO_ID, NO_ID);
    if (!name)
      name = "No name";
  }

  channel = UsbMapper_get_sysname_id(sysname);
  if (channel == 0)
  {
    if (DeviceMap[id])
    {
      Debug("  Ignoring device.\n");
      return NULL;
    }
  }
  else if (channel != id)
  {
    Debug("  Identified as channel %d.. We are looking for %d\n", channel, id);
    return NULL;
  }

  strcpy(devname, sysname);
  goto found;

#else

  size_t count, i;
  int ret, busnum, devnum;
  DevHandle *dev_list = NULL;
  count = libusb_get_device_list(ctx, &dev_list);

  if (count <= 0) {
    PrintError("Error getting USB device list: %s\n", strerror(count));
    return NULL;
  }

  for (i=0; i < count; ++i)
  {
    DevHandle dev = dev_list[i];
    busnum = libusb_get_bus_number(dev);
    devnum = libusb_get_device_address(dev);

    // TODO: Figure out if this is a problem..

    //if (DeviceMap[id] && DeviceMap[id]->devnum == devnum)
    if (GenericUSB_examined(busnum, devnum))
      continue;

    UsbDeviceDescriptor descriptor;
    ret = libusb_get_device_descriptor(dev, &descriptor);
    if (ret < 0)  {
      PrintError("Error getting device descriptor %d: %s\n",
                 ret, GetUsbErrorString(ret));
      return NULL;
    }
    vid = descriptor.idVendor;
    pid = descriptor.idProduct;

    name = DEVICE_get_imc_name(vid, pid, NO_ID, NO_ID);
    if (!name) {
      Debug(" Device Detected %d (%04X:%04X). Not recognised as a target.\n", devnum, vid, pid);
      continue;
    }

    Debug("  Device '%s' (%04X:%04X) (%d)\n", name, vid, pid, devnum);

    char* tmp = UsbMapper_get_sysname(busnum, devnum);
    if (!tmp) {
      PrintError(" Could not find system name for devnum %d!\n", devnum);
      continue;
    }
    strncpy(sysname, tmp, MAX_PATH);
    sysname[MAX_PATH-1] = '\0';
    free(tmp);

    Debug3(" Detected system name %s!\n", sysname);
    channel = UsbMapper_get_sysname_id(sysname);
    if (channel == 0)
    {
      if (DeviceMap[id])
      {
        Debug2("  Ignoring device '%s' (%04X:%04X)\n", name, vid, pid);
        continue;
      }
    }
    else if (channel != id)
    {
      Debug2("  Ignoring channel %d.. We are looking for %d\n", channel, id);
      continue;
    }

    sprintf(devname, USB_DEV_DIR "/%03d/%03d", busnum, devnum);
    goto found;
  }
  libusb_free_device_list(dev_list, 1);
#endif

  //Debug3("No device found :'(\n");
  return NULL;

found:

  //< Save new device in Comport Device Table
  device = UsbMapper_new_device(id, sysname);
  Debug2(" Channel %d is now mapped (%s)!\n", id, sysname);

  // channel == id;
  device = DeviceMap[id];
  #ifndef WIN32
    device->busnum = busnum;
    device->devnum = devnum;
  #endif

  Debug(" Device %d Online '%s' (%04X:%04X) !\n", id, name, vid, pid);

  if ( GenericUSB_open_device(device, devname) )
  {
    Debug(" Device %d open.\n", device->id);
    device->error_report = error_report;
  }
  else
  {
    PrintError(" Could not open device %d :'(\n", id);
    #ifndef WIN32
    if (errno == 13)
      PrintError(" %s: You are missing a udev rule for device (%04X:%04X).\n",
                 strerror(errno), vid, pid);
    else
      PrintError(" Error %d: %s\n", errno, strerror(errno));
    #endif

    error_report(id, errno,__LINE__);
    device = NULL;
  }

  #ifndef WIN32
    libusb_free_device_list(dev_list, 1);
  #endif

  /* Export only the public interface */
  return (comport_handler_t *)device;
  #endif //< LIBUSBHOST
}

#ifdef LIBUSB_H
static libusb_device_handle *libusb_open_wrapper(uint8_t busnum, uint8_t devnum)
{
  int ret;
  size_t count, i;

  libusb_device *dev = NULL;
  libusb_device **dev_list = NULL;
  libusb_device_handle *handle = NULL;

  count = libusb_get_device_list(ctx, &dev_list);

  for (i=0; i < count; ++i)
  {
    dev = dev_list[i];
    if (busnum == libusb_get_bus_number(dev)
     && devnum == libusb_get_device_address(dev))
    {
      ret = libusb_open(dev, &handle);
      if (ret < 0)
        PrintError("Error opening device (%03d/%03d) %d: %s\n",
                    busnum, devnum, ret, GetUsbErrorString(ret));
      break;
    }
  }

  libusb_free_device_list(dev_list, 1);
  return handle;
}
#endif


/**
 * GenericUSB_open_device()
 *
 * This function will open a USB device.
 */
#ifndef WIN32
static uint8_t GenericUSB_open_device(libusb_dev_t *device, const char* devname)
{
  UsbHandle dev;
  int interface_nr = 0;
  int i, ret, max_transfer = 0, in = 0, out = 0, control = 0;

  const UsbInterfaceDescriptor *interface;
  const UsbEndpointDescriptor  *ep;

  Debug("GenericUSB_open_device(%s)\n", devname);

  //< Avoid race condition between inotify and permissions management
  int count = 0;
  do {
    FILE*fd = fopen(devname, "r");
    if (fd != NULL) {
      fclose(fd);
      break;
    }
    usleep(1000);
  } while (count++ < 100);

#ifdef LIBUSBHOST
  if (!(dev = usb_device_open(devname)))
#else
  if (!(dev = libusb_open_wrapper(device->busnum, device->devnum)))
#endif
  {
    PrintError(" Could not open %s (%d)!\n", devname, count);
    return false;
  }

#ifdef LIBUSBHOST
  struct usb_descriptor_header* header;
  struct usb_descriptor_iter iter;

  usb_descriptor_iter_init(dev, &iter);
  while ((header = usb_descriptor_iter_next(&iter)) != NULL)
  {
    if (header->bDescriptorType != USB_DT_ENDPOINT
     && header->bDescriptorType != USB_DT_INTERFACE)
    {
      continue;
    }

    /* An EndPoint descriptor */
    if (header->bDescriptorType == USB_DT_ENDPOINT)
    {
        ep = (struct usb_endpoint_descriptor *)header;

        int addr = ep->bEndpointAddress;
        max_transfer = ep->wMaxPacketSize;

        Debug2("   End-point Address: 0x%02X (Max: %d)\n", addr, max_transfer);
        if (addr & USB_DIR_IN)
          in = addr;
        else
          out = addr;
        continue;
    }

    interface = (struct usb_interface_descriptor *)header;
#else
  int s;
  uint8_t driver = false;
  UsbConfigDescriptor *config;
  UsbDeviceDescriptor descriptor;

  libusb_device* handle = libusb_get_device(dev);
  ret = libusb_get_device_descriptor(handle, &descriptor);

  if (descriptor.bNumConfigurations > 1)
  {
    PrintError(" There are too many USB Descriptor Configurations (%d)\n",
               descriptor.bNumConfigurations);
    return false;
  }

  ret = libusb_get_config_descriptor(handle, 0, &config);
  if (ret < 0) {
    PrintError("Could not get descriptor\n");
    return false;
  }

  Debug2("USB Descriptor: (Configs: %d, Config[0].Interfaces: %d)....\n",
         descriptor.bNumConfigurations, config->bNumInterfaces);
  for (i=0; i < config->bNumInterfaces; ++i)
  {
    const UsbInterface* iface = &config->interface[i];
    Debug2(" Available settings for Interface %d: %d\n", i, iface->num_altsetting);
    for (s=0; s < iface->num_altsetting; ++s)
    {
      interface = &iface->altsetting[s];
#endif

    /* Communication Control Class */
    if (interface->bInterfaceClass == USB_CLASS_COMM)
    {
      Debug2("Found Communication Control Class!\n");

      if (interface->bNumEndpoints != 1) {
        PrintError("Wrong amount of endpoints!!\n");
        return false;
      }
      #ifdef LIBUSBHOST
        ep = (struct usb_endpoint_descriptor *)usb_descriptor_iter_next(&iter);
      #else
        ep = &interface->endpoint[0];
      #endif
      if (!ep) {
        PrintError("Could not get Endpoint iter!!\n");
        return false;
      }

      int addr = ep->bEndpointAddress;
      max_transfer = ep->wMaxPacketSize;

      if (addr & USB_ENDPOINT_IN)
        control = addr;
    }
    /* Communication Data Class */
    else if (interface->bInterfaceClass == USB_CLASS_DATA
          || interface->bDescriptorType == USB_DT_INTERFACE)
    {
      Debug("Found Communication Data Class!\n");

      if (interface->bNumEndpoints < 2) {
        PrintError("Wrong amount of endpoints!!\n");
        return false;
      }

      interface_nr = interface->bInterfaceNumber;

      for (i=0; i<interface->bNumEndpoints; i++)
      {
        #ifdef LIBUSBHOST
          ep = (struct usb_endpoint_descriptor *)usb_descriptor_iter_next(&iter);
        #else
          ep = &interface->endpoint[i];
        #endif
        if (!ep) {
          PrintError("Could not get Endpoint iter!!\n");
          return false;
        }

        if (is_bulk_endpoint(ep->bmAttributes))
        {
          int addr = ep->bEndpointAddress;
          max_transfer = ep->wMaxPacketSize;
          Debug2("   End-point Address: 0x%02X (Max: %d, Intf: %d)\n", addr, max_transfer, interface->bInterfaceNumber);
          if (addr & USB_ENDPOINT_IN)
            in = addr;
          else
            out = addr;
        }
      }
    }
    /* An EndPoint descriptor (Hopefully never used) */
    else if (interface->bDescriptorType == USB_DT_ENDPOINT)
    {
      Debug("Found EndPoint Data Class!\n");

      #ifdef LIBUSBHOST
        ep = (struct usb_endpoint_descriptor *)header;
      #else
        ep = &interface->endpoint[0];
      #endif
        if (!ep) {
          PrintError("Could not get Endpoint iter!!\n");
          return false;
        }

      int addr = ep->bEndpointAddress;
      max_transfer = ep->wMaxPacketSize;

      Debug2("   End-point Address: 0x%02X (Max: %d)\n", addr, max_transfer);
      if (addr & USB_ENDPOINT_IN)
        in = addr;
      else
        out = addr;
    }
    else
    {
      printf("Ignoring Device class %s (0x%02X)\n",
            GetDeviceClassName(interface->bInterfaceClass),
            interface->bDescriptorType);

      //< Hack start - to get stupid USB descriptors working >//
      if (!in || !out)
      {
        for (i=0; i<interface->bNumEndpoints; i++)
        {
          #ifdef LIBUSBHOST
            ep = (struct usb_endpoint_descriptor *)header;
          #else
            ep = &interface->endpoint[i];
          #endif

          int addr = ep->bEndpointAddress;
          max_transfer = ep->wMaxPacketSize;

          Debug2("   USING End-point Address: 0x%02X (Max: %d)\n", addr, max_transfer);
          if (addr & USB_ENDPOINT_IN)
            in = addr;
          else
            out = addr;
        }
      }
      //< Hack end >//
    }
  }
#ifndef LIBUSBHOST
  }
#endif

  if (!control)
    Debug("Didn't find a Control End-point. Continuing without it...\n");

  if (!in || !out)
  {
    PrintError("Could not find all the USB I/O End-points\n");
    if (in)  PrintError("Found IN endpoint as 0x%02X\n", in);
    if (out) PrintError("Found OUT endpoint as 0x%02X\n", out);
    errno = ENOTCONN; // EPROTO or ECANCELED or ENODEV
    return false;
  }

  Debug2("  I/O Pipes:\n\t In: 0x%02X, Out: 0x%02X, Control: 0x%02X, Intf: %d\n",
         in,out,control, interface_nr);
  switch (max_transfer) {
    case 64:   Debug("  FullSpeed Device (12 Mbit/s or lower)\n");    break;
    case 512:  Debug("  HighSpeed Device (480 Mbit/s or higher)\n");  break;
    case 1024: Debug("  SuperSpeed Device (1.25 Gbit/s or higher)\n");   break;
    default:
      PrintError("  Cannot determine device speed: %d\n", max_transfer);
      return false;
  }

  //< Actually creating the device
  #ifdef LIBUSBHOST
    device->dev = devname;   // usb_device_get_name(dev);
    device->usbHandle = dev; // usb_device_open(device->dev);
  #else
    device->dev = handle;
    device->usbHandle = dev;
  #endif

  // Remove kernel driver and claim interface
  #ifdef LIBUSBHOST
  if ((ret = usb_device_claim_interface(dev, interface_nr)) < 0)
  {
    usb_device_connect_kernel_driver(dev, interface_nr, false);

    if ((ret = usb_device_claim_interface(dev, interface_nr)) < 0)
    {
      PrintError("error on usb_claim_interface: %s\n", strerror(errno));
      return false;
    }
  }
  #else
    ret = libusb_kernel_driver_active(dev, interface_nr);
    if (ret < 0)
    {
      PrintError("\n\nCould not get kernel driver %d: %s!\n", ret, strerror(ret));
      return false;
    }
    else if (ret == 1)
    {
      //Debug("Dumping kernel driver (%s)...\n", driver);
      ret = libusb_detach_kernel_driver(dev, interface_nr);
      if (ret < 0)
      {
        PrintError("\n\nCould not detach kernel driver %d: %s!\n", ret, strerror(ret));
        return false;
      }
      driver = true;
    }

    if (libusb_claim_interface(dev, interface_nr) < 0)
    {
      PrintError("error on usb_claim_interface: %s\n", strerror(ret));
      return false;
    }
  #endif

  //< Actually creating the device
  device->port.id = device->id;
  device->ctrlHandle = device->usbHandle;
  device->max_transfer = max_transfer;

  #ifdef LIBUSBHOST
    device->vendor  = usb_device_get_vendor_id(device->usbHandle);
    device->product = usb_device_get_product_id(device->usbHandle);
  #else
    device->driver  = driver;
    device->vendor  = descriptor.idVendor;
    device->product = descriptor.idProduct;
  #endif

  device->in = in;
  device->out = out;
  device->control = control;
  device->zlp_enabled = true;
  device->interfaceNr = interface_nr;
  device->status = USB_STATUS_IDLE;
  //device->comport_status = comport_status_no_event;

  device->port.close = GenericUSB_close;
  device->port.process_comm = GenericUSB_process_communication;
  device->port.tx = GenericUSB_write;
  device->port.rx = GenericUSB_read;
  device->port.flush_rx_buffer = GenericUSB_flush_rx_buffer;
  device->port.flush_tx_buffer = GenericUSB_flush_tx_buffer;
  device->port.get_modem_status = GenericUSB_get_modem_status;
  device->port.get_last_error = GenericUSB_get_last_error;
  device->port.get_rx_used_buffer_size = GenericUSB_get_rx_used_buffer_size;
  device->port.get_status = GenericUSB_get_status;
  device->port.set_baud = GenericUSB_set_baudrate;
  device->port.set_buffer_size = GenericUSB_set_buffer_size;
  device->port.set_dtr = GenericUSB_set_dtr;
  device->port.set_rts = GenericUSB_set_rts;

  return true;
}
#else
static uint8_t GenericUSB_open_device(libusb_dev_t *device, const char* sysname)
{
  int i, s, interface_nr;
  int in, out, control, max_transfer;
  UsbHandle usbHandle, ctrlHandle;

  int id = device->id;
  //libusb_dev_t *device = NULL;
  usbHandle = ctrlHandle = NULL;
  in = out = control = max_transfer = interface_nr = 0;

  UsbConfigDescriptor *config;

    Debug2("  Opening FilePath: %s...\n", sysname);
    HANDLE dev = CreateFile(sysname,
                     GENERIC_WRITE | GENERIC_READ,
                     FILE_SHARE_WRITE | FILE_SHARE_READ,
                     NULL,
                     OPEN_EXISTING,
                     FILE_FLAG_OVERLAPPED, //FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
                     NULL);
    if (dev == INVALID_HANDLE_VALUE)
    {
      PrintError("Could not CreateFile(%s): %s\n", sysname, GetUsbErrorString(true));
      goto fail;
    }

    Debug2("  Initializing WinUsb... \n");
    if (!WinUsb_Initialize(dev, &ctrlHandle)) {
      PrintError("Failed to WinUsb_Initialize: %s", GetUsbErrorString(true));
      goto fail;
    }

    //< USB_DEVICE_DESCRIPTOR_TYPE(0x01), US English (0x0409)
    UsbDeviceDescriptor DevHandler; //< USB device descriptor
    ULONG length = sizeof(UsbDeviceDescriptor);
    if (!WinUsb_GetDescriptor(ctrlHandle, 0x01, 0, 0x0409,
                              (PUCHAR)&DevHandler,
                              sizeof(DevHandler), &length))
    {
      PrintError("Failed to WinUsb_GetDescriptor: %s", GetUsbErrorString(true));
      goto fail;
    }
    Debug("  Vid: 0x%04X, Pid: 0x%04X\n", DevHandler.idVendor, DevHandler.idProduct);


    /* Creating the USB Config Descriptor Structure */
    WINUSB_PIPE_INFORMATION pipeInfo;  //< TODO: Add this to the structure.

    config = (UsbConfigDescriptor*)malloc(sizeof(UsbConfigDescriptor));
    config->bNumInterfaces = 0;
    config->interface = (struct usb_interface*)malloc(sizeof(struct usb_interface)*2);
    memset(config->interface, 0, sizeof(struct usb_interface)*2);

    config->bNumInterfaces++;
    config->interface[0].handle = ctrlHandle;
    config->interface[0].num_altsetting = 1;
    config->interface[0].altsetting = (UsbInterfaceDescriptor*)malloc(sizeof(UsbInterfaceDescriptor));
    if (!WinUsb_QueryInterfaceSettings(ctrlHandle, 0, config->interface[0].altsetting))
    {
      PrintError("Failed to WinUsb_QueryInterfaceSettings: %s", GetUsbErrorString(true));
      goto fail;
    }

    WinUsb_GetAssociatedInterface(ctrlHandle, 0, &usbHandle);
    int err = GetLastError();
    if (err != ERROR_NO_MORE_ITEMS)
    {
      config->bNumInterfaces++;
      config->interface[1].handle = usbHandle;
      config->interface[1].num_altsetting = 1;
      config->interface[1].altsetting = (UsbInterfaceDescriptor*)malloc(sizeof(UsbInterfaceDescriptor));

      if (!WinUsb_QueryInterfaceSettings(usbHandle, 0, config->interface[1].altsetting))
      {
        PrintError("Failed to WinUsb_QueryInterfaceSettings: %s", GetUsbErrorString(true));
        goto fail;
      }
    }

  Debug2("Available Interfaces: (%d)....\n", config->bNumInterfaces);
  for (i=0; i < config->bNumInterfaces; ++i)
  {
    const UsbInterface* interface = &config->interface[i];

    Debug2(" Available settings for Interface %d: %d\n", i, interface->num_altsetting);
    for (s=0; s < interface->num_altsetting; ++s)
    {
      const UsbInterfaceDescriptor *desc = &interface->altsetting[s];

      /* Communication Control Class */
      if (desc->bInterfaceClass == USB_CLASS_COMM)
      {
        Debug2("  Found Communication Control Class (0x02)!\n");

        if (desc->bNumEndpoints != 1) {
          Debug("Wrong amount of endpoints!!\n");
          goto fail;
        }
          ctrlHandle = interface->handle;
          if (!WinUsb_QueryPipe(ctrlHandle, i, 0, &pipeInfo)) {
            PrintError("Failed to WinUsb_QueryPipe()\n");
            goto fail;
          }
          int addr = pipeInfo.PipeId;
          max_transfer = pipeInfo.MaximumPacketSize;

        if (addr & USB_ENDPOINT_IN)
          control = addr;
      }
      /* Communication Data Class */
      else if (desc->bInterfaceClass == USB_CLASS_DATA)
      {
        Debug2("  Found Communication Data Class (0x0A)!\n");

        if (desc->bNumEndpoints != 2) {
          PrintError("Wrong amount of endpoints!!\n");
          goto fail;
        }

        interface_nr = i; //< XMM7160 uses interface 0 instead of 1
        int k;
        for (k = 0; k < desc->bNumEndpoints; ++k)
        {
            if (!WinUsb_QueryPipe(interface->handle, ((i>0) ? i-1 : i), k, &pipeInfo)) {
              PrintError("Failed to WinUsb_QueryPipe(%d,%d)\n", i, k);
              goto fail;
            }

            ULONG ms = TIMEOUT*6;
            int addr = pipeInfo.PipeId;
            max_transfer = pipeInfo.MaximumPacketSize;
            Debug2("   End-point Address: 0x%02X\n", addr);

            switch(pipeInfo.PipeType)
            {
            case UsbdPipeTypeBulk:
              usbHandle = interface->handle;
              if (!WinUsb_SetPipePolicy(interface->handle, pipeInfo.PipeId,
                  PIPE_TRANSFER_TIMEOUT, sizeof(ULONG), &ms)) {
                PrintError("Failed to set timeout for PipeID %d: %s\n", pipeInfo.PipeId,  GetUsbErrorString(true));
                goto fail;
              }

              if (addr & USB_ENDPOINT_IN)
              {
                UCHAR value = true;
                if (!WinUsb_SetPipePolicy(interface->handle, pipeInfo.PipeId,
                    WINUSB_AUTO_FLUSH, sizeof(UCHAR), &value)) {
                  PrintError("Failed to WinUsb_SetPipePolicy for PipeID %d: %s\n", pipeInfo.PipeId,  GetUsbErrorString(true));
                  goto fail;
                }

                in = addr;
              }
              else
              {
                UCHAR value = false;
                if (!WinUsb_SetPipePolicy(interface->handle, pipeInfo.PipeId,
                    SHORT_PACKET_TERMINATE, sizeof(UCHAR), &value)) {
                  PrintError("Failed to WinUsb_SetPipePolicy for PipeID %d: %s\n", pipeInfo.PipeId,  GetUsbErrorString(true));
                  goto fail;
                }

                out = addr;
              }
              break;

            case UsbdPipeTypeInterrupt:
            case UsbdPipeTypeIsochronous:
            case UsbdPipeTypeControl:
            default:
              PrintError(" FOUND AN unexpected USB PIPE of type 0x%02X!\n", pipeInfo.PipeType);
              goto fail;
            }
        }
      }
      else
      {
        PrintError("Ignoring Device class %s (0x%02X)\n",
               GetDeviceClassName(desc->bInterfaceClass),
               desc->bDescriptorType);
      }
    } //< foreach altsetting
  }   //< foreach bNumInterfaces

  if (!in || !out)
  {
    PrintError("Could not find all the USB I/O End-points\n");
    if (in)  PrintError("Found IN endpoint as 0x%02X\n", in);
    if (out) PrintError("Found OUT endpoint as 0x%02X\n", out);
    errno = ENOTCONN; // EPROTO or ECANCELED or ENODEV
    goto fail;
  }

  Debug("  Interfaces: In: 0x%02X, Out: 0x%02X, Control: 0x%02X\n", in, out, control);
  switch (max_transfer) {
    case 64:   Debug("  FullSpeed Device (12 Mbit/s or lower)\n");    break;
    case 512:  Debug("  HighSpeed Device (480 Mbit/s or higher)\n");  break;
    case 1024: Debug("  SuperSpeed Device (1.25 Gbit/s or higher)\n");   break;
    default:
      PrintError("  Cannot determine device speed: %d\n", max_transfer);
      goto fail;
  }

  //< Actually creating the device
  device->dev = dev;
  device->usbHandle = usbHandle;
  device->ctrlHandle = ctrlHandle;
  device->port.id = device->id = id;

  device->max_transfer = max_transfer;
  device->vendor  = DevHandler.idVendor;
  device->product = DevHandler.idProduct;

  device->in = in;
  device->out = out;
  device->control = control;
  device->zlp_enabled = true;
  device->interfaceNr = interface_nr;
  device->status = USB_STATUS_IDLE;
  //device->comport_status = comport_status_no_event;

  device->port.close = GenericUSB_close;
  device->port.process_comm = GenericUSB_process_communication;
  device->port.tx = GenericUSB_write;
  device->port.rx = GenericUSB_read;
  device->port.flush_rx_buffer = GenericUSB_flush_rx_buffer;
  device->port.flush_tx_buffer = GenericUSB_flush_tx_buffer;
  device->port.get_modem_status = GenericUSB_get_modem_status;
  device->port.get_last_error = GenericUSB_get_last_error;
  device->port.get_rx_used_buffer_size = GenericUSB_get_rx_used_buffer_size;
  device->port.get_status = GenericUSB_get_status;
  device->port.set_baud = GenericUSB_set_baudrate;
  device->port.set_buffer_size = GenericUSB_set_buffer_size;
  device->port.set_dtr = GenericUSB_set_dtr;
  device->port.set_rts = GenericUSB_set_rts;

  Debug("GenericUSB_open(%d)\n", id);
  goto done;

fail:
    if (dev) {
      Debug2("  Closing Dev Handler...\n");
      CloseHandle(dev);
    }

  return false;
done:
    if (config)
    {
      Debug3("  Closing Config...\n");
      if (config->interface && config->interface[0].num_altsetting)
      {
        for (s=0; s < config->interface[0].num_altsetting; ++s)
        {
          if (config->interface[s].altsetting)
          {
            Debug3("  Closing Interface %d...\n", s);
            free(config->interface[s].altsetting);
          }
        }
        Debug3("  Closing Interfaces rest...\n");
        free(config->interface);
      }
      Debug3("  Freeing config...\n");
      free(config);
      Debug3(" Done\n");
    }

  return true;
}
#endif // WIN32

/**
 * GenericUSB_close()
 */
uint8_t  GenericUSB_close(uint8_t id)
{
  int ret = true;
  libusb_dev_t *device = DeviceMap[id];

  if(!device)
    return false; //< EBADF

  Debug2("GenericUSB_close: Closing %d...\n", id);
  if (device->usbHandle)
  {
    #ifndef WIN32
      #ifdef LIBUSBHOST
        usb_device_release_interface(device->usbHandle, device->interfaceNr);
        usb_device_close(device->usbHandle);
      #else
        libusb_release_interface(device->usbHandle, device->interfaceNr);
        libusb_close(device->usbHandle);
      #endif
    #else
      if (device->dev) {
        Debug2("  Closing device handle...\n");
        CloseHandle(device->dev);
      }

      if (device->usbHandle) {
        Debug2("  Free USB handler\n");
        WinUsb_Free(device->usbHandle);
      }
    #endif
    device->usbHandle = 0;
    device->ctrlHandle = 0;
    device->error = 0;
    device->last_error = 0;
    device->status = USB_STATUS_CLOSED;
    device->comport_status = comport_status_no_event;
    Debug("GenericUSB_close: Closed %d.\n", id);
  }

  return ret;
}


/**
 * GenericUSB_read()
 *
 * Based on GenericUSB_rx_I()
 */
uint8_t GenericUSB_read(uint8_t id, uint8_t **block, uint32_t size,
                        uint32_t *bytes_read, comport_fx_rx_t fx)
{
  int res;
  uint8_t *data = *block;
  int error_limit = ERROR_LIMIT;

  libusb_dev_t *comport = DeviceMap[id];

  if(!comport || comport->status != USB_STATUS_IDLE)
    return 0;

  int timeout = MAX(TIMEOUT*size/1024, TIMEOUT);
  uint8_t ZeroLenthPacket = !(size % comport->max_transfer);

  comport->bytesRead = 0;
  comport->status = USB_STATUS_READING;
  comport->comport_status = comport_read_operation_ongoing;
  while (comport->bytesRead < size)
  {
    int nbytes = comport->bytesRead;

    Debug2("Reading %d bytes (ZLP: %d, Timeout: %d)...\n",
           size-nbytes, ZeroLenthPacket, timeout);

#ifndef WIN32
    #ifdef LIBUSBHOST
    errno = 0;
    int ret = usb_device_bulk_transfer(
                            comport->usbHandle,
                            comport->in,
                            data+nbytes, size-nbytes,
                            timeout);
    res = ret;
    if (errno)
      PrintError("Error %d: %s\n", errno, strerror(errno));
    #else
    int ret = libusb_bulk_transfer(
                  comport->usbHandle, comport->in,
                  data+nbytes, size-nbytes, &res,
                  timeout);
    if (ret < 0)
    {
      res = ret;
      //errno = ret;
    }
    #endif

#else
    if (!WinUsb_ReadPipe(comport->usbHandle,
                          comport->in,
                          (UCHAR*)(data+nbytes),
                          size-nbytes,
                          (PULONG)&res,
                          NULL))
    {
      res = -GetLastError();
      //PrintError("ReadToDevice(%d) Failed: %s\n", size, GetUsbErrorString(true));
    }
#endif

    if (res > 0)
    {
      nbytes = comport->bytesRead += res;
      *bytes_read = comport->bytesRead;

      Debug2("Read %d/%d bytes [ %s ]\n", nbytes, size,
              getHex(data, nbytes>25 ? 25 : nbytes));

      comport->error = (int)comport_error_no_error;
      if (comport->bytesRead == size) {
        comport->status = USB_STATUS_IDLE;
        comport->comport_status = comport_read_operation_finished;
      }

      // Notify COMM module via callback
      process_animation();
      //if (process_cb)
      //  process_cb(id, *bytes_read, size);
    }
    else // if (res < 0)
    {
      /* Read Error! */
      if (res == GENERIC_ERROR_TIMEOUT)
      {
        //error_limit++;
        if (GLOBAL_comm_timeout == 0)
          error_limit++;
        Debug("Timeout (%d): %d: %s\r", size, errno, GetUsbErrorString(res));
      }
      else if (res == ERROR_NODEV)
      {
        PrintError("Device disconnected.\n");
        error_limit = 0;
      }
      else if (comport->last_error != res)
      {
        comport->last_error = comport->error;
        comport->error = res;
        if (res != 0)
          PrintError("usb_bulk_read(%d) error %d: %s\n",
                     size, res, GetUsbErrorString(true));

        #ifndef WIN32
        if (errno == ERROR_NO_MEM)
        {
          printf("\n Your device might be connected through USB3.0 (xhci), which has issues.\n\n"
                 " Please move it to a USB2.0 port and try again.\n\n");
          error_limit = 0;
        }
        #endif
      }

      Sleep(10);
      comport->last_error = comport->error;
      comport->error = res;
      comport->status = USB_STATUS_ERROR;
      comport->comport_status = comport_read_operation_finished_with_error;

      if (error_limit-- == 0) {
        PrintError("Error Limit Reached!\n");
        return 2;
      }
    }
  }

  if (ZeroLenthPacket && comport->zlp_enabled)
  {
    int ret = 0, len = 0;
    error_limit = ERROR_LIMIT;
    Debug3("Reading Zero-Length Packet...\n");

    do {
    #ifndef WIN32
      #ifdef LIBUSBHOST
        errno = 0;
        ret = usb_device_bulk_transfer(comport->usbHandle, comport->in, (uint8_t*)"", 0, TIMEOUT);
      #else
        ret = libusb_bulk_transfer(comport->usbHandle, comport->in, (uint8_t*)"", 0, &len, TIMEOUT);
      #endif
    #else
      unsigned char ZLP = 0;
      if (!WinUsb_WritePipe(comport->usbHandle, comport->out, &ZLP, 0, (PULONG)&len, NULL))
        ret = -GetLastError();
    #endif
    } while (ret == GENERIC_ERROR_TIMEOUT && error_limit--);

    if (ret < 0)
    {
      comport->comport_status = comport_read_operation_finished_with_error;
      PrintError("Read ZeroLenthPacket failed: %s\n", GetUsbErrorString(ret));
      return 2;
    }
  }

  Debug("Finished reading %d/%d bytes [ %s ]\n",
        comport->bytesRead, size,
        getHex(data, size>25 ? 25 : size));

  return 0;
  // TODO: Make it look more like fread() by removing *bytes_read
  //return comport->bytesRead;
}


/**
 * GenericUSB_write()
 *
 * Based on GenericUSB_tx_I()
 *
 * USB 2.0 (60 MB/s)
 *  1 sec:    60*1024*8 = 491520 bytes
 *
 * BootRom (PSI ~ 6sec)
 *  We give it 8 sec!
 */
uint8_t  GenericUSB_write(uint8_t id, uint8_t *data, uint32_t size,
                          uint32_t *bytes_sent, comport_fx_tx_t notify_cb)
{
  int error_limit = ERROR_LIMIT;
  int timeout = MAX(TIMEOUT*size/1024, TIMEOUT);

  libusb_dev_t *comport = DeviceMap[id];
  if(!comport || comport->status != USB_STATUS_IDLE)
    return 1;

  uint8_t ZeroLenthPacket = !(size % comport->max_transfer);

  //*bytes_send = comport->bytesWritten;

  comport->bytesWritten = 0;
  comport->status = USB_STATUS_WRITING;
  comport->comport_status = comport_write_operation_ongoing;
  comport->last_error = comport->error;
  comport->error = (int)comport_error_no_error;

  Debug2("Writing %d bytes (Channel: %d, ZLP: %d, Timeout: %d)...\n",
         size, id, ZeroLenthPacket, timeout);

  while (comport->bytesWritten < size)
  {
    int res = 0;
    uint32_t written = comport->bytesWritten;

#ifndef WIN32
    int ret = 0;
    errno = 0;
    #ifdef LIBUSBHOST
      ret = usb_device_bulk_transfer(
                  comport->usbHandle, comport->out,
                  data+written, size-written,
                  timeout);
      res = ret;
      if (errno)
        PrintError("Error %d: %s\n", errno, strerror(errno));
    #else
      ret = libusb_bulk_transfer(
                  comport->usbHandle, comport->out,
                  data+written, size-written, &res,
                  timeout);
    #endif
    if (ret < 0)
    {
      res = ret;
    }
#else
    if (!WinUsb_WritePipe(comport->usbHandle,
                          comport->out,
                          (UCHAR*)(data+written),
                          size-written,
                          (PULONG)&res,
                          NULL))
    {
      res = -GetLastError();
      //PrintError("Failed to WriteToDevice(%d): %s\n", size, GetUsbErrorString(true));
    }
#endif

    if (res > 0)
    {
      error_limit = ERROR_LIMIT;
      written = comport->bytesWritten += res;
      *bytes_sent = comport->bytesWritten;

      if (written == size) {
        comport->status = USB_STATUS_IDLE;
        comport->comport_status = comport_write_operation_finished;
      }

      // Notify COMM module via callback
      process_animation();
      //if (process_cb)
      //  process_cb(id, *bytes_sent, size);

      Debug3("Wrote %d/%d bytes [ %s ]\n",
              written, size, getHex(data, written>25 ? 25 : written));
    }
    else // if (res <= 0)
    {
      /* Write Error! */
      if (res == GENERIC_ERROR_TIMEOUT)
      {
        /* Continous timeouts occur when setting breakpoints in the target */
        if (GLOBAL_comm_timeout == 0)
          error_limit++;
        Debug("Timeout (%d): %d: %s\r", size, errno, GetUsbErrorString(res));
      }
      else if (comport->last_error != res)
      {
        comport->last_error = comport->error;
        comport->error = res;
        PrintError("usb_bulk_write(%d) error %d: %s\n", size, res, GetUsbErrorString(true));

        #ifndef WIN32
        if (errno == ERROR_NO_MEM)
        {
          printf("\n Your device might be connected through USB3.0 (xhci), which has issues.\n\n"
                 " Please move it to a USB2.0 port and try again.\n\n");
          error_limit = 0;
        }
        #endif
      }

      //static error_limit = 5;
      if (error_limit-- == 0)
      {
        Debug("Error Limit Reached!\n");
        comport->status = USB_STATUS_ERROR;
        comport->comport_status = comport_write_operation_finished_with_error;
        return 2;
      }

      Sleep(10);
    }
  }

  if (ZeroLenthPacket && comport->zlp_enabled)
  {
    int ret = 0, len = 0;
    error_limit = ERROR_LIMIT;
    Debug3("Writing Zero-Length Packet...\n");

    do {
    #ifndef WIN32
      #ifdef LIBUSBHOST
        errno = 0;
        ret = usb_device_bulk_transfer(comport->usbHandle, comport->out, (uint8_t*)"", 0, TIMEOUT);
      #else
        ret = libusb_bulk_transfer(comport->usbHandle, comport->out, (uint8_t*)"", 0, &len, TIMEOUT);
      #endif
    #else
      unsigned char ZLP = 0;
      if (!WinUsb_WritePipe(comport->usbHandle, comport->out, &ZLP, 0, (PULONG)&len, NULL))
        ret = -GetLastError();
    #endif
    } while (ret == GENERIC_ERROR_TIMEOUT && error_limit--);

    if (ret < 0)
    {
      comport->comport_status = comport_write_operation_finished_with_error;
      PrintError("Write ZeroLenthPacket failed: %s\n", GetUsbErrorString(ret));
      return 2;
    }
  }

  Debug("Finished writing %d/%d bytes %s [ %s ]\n",
          comport->bytesWritten, size, ZeroLenthPacket ? "(ZLP)" : "",
          getHex(data, size>25 ? 25 : size));

  // Notify Above modules via callback
  if (notify_cb)
    notify_cb(id, *bytes_sent);

  return 0;
  //return comport->bytesWritten;
}


/**
 * GenericUSB_process_communication()
 */
void GenericUSB_process_communication(uint8_t id)
{
  Debug3("GenericUSB_process_communication\n");
  /*
  libusb_dev_t *device = DeviceMap[id];
  if (device
      && !device->done
      && device->status == comport_read_operation_ongoing)
  {
  }
*/
}


/**
 * GenericUSB_get_status()
 */
static comport_status_t GenericUSB_get_status(uint8_t id)
{
  Debug3("GenericUSB_get_status\n");
  libusb_dev_t *comport_dev = DeviceMap[id];
  if(!comport_dev)
    return comport_status_no_event;
  return comport_dev->comport_status;
}

/**
 * GenericUSB_get_last_error()
 */
static comport_error_t  GenericUSB_get_last_error(uint8_t id)
{
  libusb_dev_t *comport = DeviceMap[id];

  if(!comport)
    return comport_error_access_denied;

  Debug3("GenericUSB_get_last_error(%d)\n", comport->error);
  #ifndef WIN32
    return (comport_error_t)comport->error;
  #else
    comport_error_t error;
    switch(comport->error)
    {
       case EBADF:
         error = comport_error_invalid_handle;
         break;

       case EBUSY:
         error = comport_error_busy;
         break;

       case GENERIC_ERROR_TIMEOUT:
         error = comport_error_counter_timeout;
         break;

       case ENODEV:
         error = comport_error_no_more_devices;   //< TODO: Change to disconnected
         break;

       default:
         error = comport_error_unkown;
         break;
    }
    return error;
  #endif
}


/**
 * GenericUSB_set_rts()
 */
static uint8_t GenericUSB_set_rts(uint8_t id, uint8_t new_rts)
{
  Debug3("GenericUSB_set_rts\n");
/*
  int mdmbits;
  libusb_dev_t *comport_dev = DeviceMap[id];

  if (!comport_dev || comport_dev->h_comm == INVALID_HANDLE_VALUE)
    return 0;

  ioctl( comport_dev->h_comm, TIOCMGET, &mdmbits);
  if (new_rts)
  {
     mdmbits |= TIOCM_RTS;
  }
  else
  {
     mdmbits &= ~TIOCM_RTS;
  }
  if( ioctl( comport_dev->h_comm, TIOCMSET, &mdmbits ) < 0)
    return 0;
  else
*/
    return 0xFF;
}


/**
 * GenericUSB_set_dtr_I()
 */
static uint8_t GenericUSB_set_dtr(uint8_t id, uint8_t new_dtr)
{
  Debug3("GenericUSB_set_dtr\n");
  /*
  int mdmbits;
  libusb_dev_t *comport_dev = DeviceMap[id];

  if (!comport_dev || comport_dev->h_comm == INVALID_HANDLE_VALUE)
    return 0;

  ioctl( comport_dev->h_comm, TIOCMGET, &mdmbits );
  if (new_dtr)
  {
     mdmbits |= TIOCM_DTR;
  }
  else
  {
     mdmbits &= ~TIOCM_DTR;
  }
  if( ioctl( comport_dev->h_comm, TIOCMSET, &mdmbits ) < 0 )
    return 0;
  else
  */
    return 0xFF;
}

/**
 * GenericUSB_set_baudrate()
 */
static uint8_t GenericUSB_set_baudrate(uint8_t id, uint32_t baudrate)
{
  Debug3("GenericUSB_set_baudrate (Id: %d, BaudRate: %d)\n", id, baudrate);
  return 0xFF;
}

/**
 * GenericUSB_set_buffer_size()
 */
static uint8_t GenericUSB_set_buffer_size(uint8_t id, uint32_t rx_buf_size, uint32_t tx_buf_size)
{
  Debug3("GenericUSB_set_buffer_size (Id: %d, Rx: %d, Tx: %d)\n",
         id, rx_buf_size, tx_buf_size);
  return 0xFF;
}


/**
 * GenericUSB_get_rx_used_buffer_size()
 */
static uint32_t GenericUSB_get_rx_used_buffer_size(uint8_t id)
{
  Debug3("GenericUSB_get_rx_used_buffer_size\n");
  /*
  libusb_dev_t *comport_dev = DeviceMap[id];
  int bytes_available = 0;

  if( ioctl( comport_dev->h_comm, FIONREAD, &bytes_available ) < 0 )
  {
    Debug("Error> IOCTL failed during FIONREAD (errno: %s)\n", strerror(errno));
    return 0;
  }
  else
  {
    Debug("Bytes available: %d", bytes_available);
    return (uint32_t)bytes_available;
  }
  */
  return 0;
}

/**
 * GenericUSB_flush_rx_buffer()
 */
static uint8_t  GenericUSB_flush_rx_buffer(uint8_t id)
{
  Debug3("GenericUSB_flush_rx_buffer\n");
  libusb_dev_t *comport_dev = DeviceMap[id];

  if(!comport_dev)
    return 0;
/*
  if (GLOBAL_usb_device || GLOBAL_comm_technology[id])
    return 0xFF;

  if( tcflush(comport_dev->h_comm, TCIFLUSH) < 0 )
  {
  Debug("tcflush failed: %s\n", strerror(errno));
    comport_dev->last_reported_error = comport_dev->error;
    comport_dev->error = errno;
    comport_dev->error_report(comport_dev->id, comport_dev->error ,__LINE__);
    return 0;
  }
*/  return 0xFF;
}


/**
 * GenericUSB_flush_rx_buffer()
 */
static uint8_t GenericUSB_flush_tx_buffer(uint8_t id)
{
  Debug3("GenericUSB_flush_tx_buffer\n");
  return 0xFF;
}

/**
 * GenericUSB_get_modem_status()
 */
static uint8_t GenericUSB_get_modem_status(uint8_t id,
                comport_modem_status_line_t *modem_status_line)
{
  //Debug3("GenericUSB_get_modem_status\n");
  return 0xFF;
}



/********************************************************************/
/******************** The USB Map Handler ***************************/
/********************************************************************/

uint8_t GenericUSB_map(int id, const char* sysname)
{
  return (UsbMapper_new_device(id, sysname) != NULL);
}

/**
 * UsbMapper_new_device()
 *
 * Maps a sysname to a device ID
 *
 * @param id       The channel ID. If set to -1 an empty channel will be used.
 * @param sysname  The sysname to assign the channel.
 *
 * @return   A new device or an exsisting depending on the ID
 */
libusb_dev_t* UsbMapper_new_device(int id, const char* sysname)
{
  libusb_dev_t* device;

  if (id < 0)
  {
    while (id++ && id < MAX_NUMBER_OF_CHANNELS && DeviceMap[id]);
    if (id==0) id++;
  }

  if (DeviceMap[id])
  {
    device = DeviceMap[id];
  }
  else
  {
    device = (libusb_dev_t *) MEM_malloc (sizeof(libusb_dev_t));
    DeviceMap[id] = device;
    device->id = id;
    Debug2(" Device %d mapped as %s...\n", id, sysname);
  }

  if (device->sysname && strcmp(device->sysname, sysname) != 0)
  {
    Debug("Freeing old sysname...\n");
    free(device->sysname);
  }

  device->sysname = strdup(sysname);
  return device;
}

// TODO Make use of these functions
#ifdef UNUSED_FUNCTIONS
uint8_t UsbMapper_free_device(int id)
{
  libusb_dev_t* device = DeviceMap[id];

  if(!device)
     return false; //< EBADF

  Debug("UsbMapper_free_device(%d)\n", id);

  if (device->sysname)
  {
    Debug2("  Freeing sysname (%s)...\n", device->sysname);
    free(device->sysname);
  }

  Debug2("  Free Device structure\n");
  free(device);
  DeviceMap[id] = NULL;
  return true;
}


static void UsbMapper_dump_map()
{
  int id;
  for (id=1; DeviceMap[id]; ++id)
  {
    assert(DeviceMap[id]->sysname != NULL);
    Debug("ID %d sysname: '%s'\n", id, DeviceMap[id]->sysname);
  }
}
#endif

static int UsbMapper_get_sysname_id(const char* sysname)
{
  assert(sysname != NULL);

  int id;
  for (id=1; DeviceMap[id]; ++id)
  {
    assert(DeviceMap[id]->sysname != NULL);
    if (strcmp(sysname, DeviceMap[id]->sysname) == 0)
      return id;
  }

  return 0;
}


/********************************************************************/
/********************** The USB Mapper ******************************/
/********************************************************************/


/**
 * GenericUSB_load_map()
 */
static int map_handler(void*arg, const char* section,
                          const char* key, const char* value)
{
  static uint8_t usb_mapper_section = false;

  if (!key) {
    Debug3("[%s]\n", section);
    usb_mapper_section = (!strcasecmp(section, "USB Mapper"));
  }
  else if (usb_mapper_section)
  {
    Debug3("%s=%s\n", key, value);
    if (strncasecmp(key, "Channel", 7) == 0)
    {
      int channel = strtoul(key+7, NULL, 0);
      if (channel > 0 && channel < MAX_NUMBER_OF_CHANNELS)
      {
        Debug2("Mapping channel %d to sysname '%s'\n", channel, value);
        UsbMapper_new_device(channel, value);
      }
    }
  }

  return true;
}

int GenericUSB_load_map(const char* file)
{
  if (!file)
    return 0;
  GenericUSB_init();
  Debug(" Loading map %s...\n", file);
  return ParseINI(file, map_handler, NULL);
}

/**
 * GenericUSB_save_map()
 */
int GenericUSB_save_map(const char* file)
{
  size_t i, len;
  FILE* fp;
  static char buf[MAX_PATH];
  if (!file) return false;

  fp = FILE_open((char*)file, FILE_open_mode_write_create);
  if (!fp) return false;

  FILE_WriteLine(fp, ";---------------------------------;\n"
                     "; USB Device Map (Auto-generated) ;\n"
                     ";---------------------------------;\n"
                     "\n[USB Mapper]\n");

  for (i=1; DeviceMap[i]; ++i)
  {
    len = snprintf(buf, MAX_PATH, "Channel%d=\"%s\"\n", (int)i, DeviceMap[i]->sysname);
    if (!len) break;
    FILE_write(fp, (unsigned char*)buf, len);
  }

  if (FILE_close(fp) == FILE_status_ok)
  {
    Debug(" USB Device Map saved as %s.\n", file);
    return true;
  }
  return false;
}

/**
 * GenericUSB_mapper(int id)
 *
 * id - The ID to look for in the map. If ID is zero any
 */
char* GenericUSB_mapper(int id)
{
  uint32_t vid = 0, pid = 0;
  char *devname = NULL;
  char *sysname = NULL;
  const char* name = NULL;

  if (id <= 0 || id > MAX_NUMBER_OF_CHANNELS)
    return NULL;

  GenericUSB_init();

  if (DeviceMap[id])
    return DeviceMap[id]->sysname;

  Debug2(" Mapping USB device %d... \n", id);

  while (!sysname)
  {
    usleep(100);

    #ifdef WIN32
      static char tmp[MAX_PATH+1];
      tmp[0] = 0;

      UsbMapper_get_sysname(&phone_guid, tmp, sizeof(char)*MAX_PATH, id);

      if (!tmp[0])
        WinUSB_get_device_list(tmp, sizeof(tmp));

      if (!tmp[0])
        continue;

      if (WinUSB_get_vid_pid(tmp, &vid, &pid))
      {
        name = DEVICE_get_imc_name(vid, pid, NO_ID, NO_ID);
        if (!name)
          name = "No name";
      }

      sysname = strdup(tmp);
      devname = WinUSB_get_device_name(tmp);

      /* Common for Linux & Windows */
      int found = UsbMapper_get_sysname_id(sysname);
      if (found)
      {
        Debug(" Device %d already Mapped as %s\n", found, DeviceMap[found]->devname);
        if (sysname) free(sysname);
        sysname = NULL;
      }
      else
      {
        Debug("  Found %s %s (%s) (%04X:%04X)\n", devname, name, sysname, vid, pid);
        break;
      }
    #else

    #ifdef LIBUSBHOST
      Debug("Waiting for inotify to find the device...\n");

      libusb_dev_t* device;
      usb_host_run(ctx, GenericUSB_added_cb, GenericUSB_removed_cb,
                   NULL, &device);

      vid = device->vendor;
      pid = device->product;
      //name = device->name;

      int found = UsbMapper_get_sysname_id(device->sysname);
      if (found)
      {
        Debug(" Device %d already Mapped as %s\n", found, DeviceMap[found]->sysname);
        GenericUSB_close(id);
      }

      name = DEVICE_get_imc_name(vid, pid, NO_ID, NO_ID);
      sysname = strdup(device->sysname);

      Debug("  Found %s (%s) (%04X:%04X)\n", name, sysname, vid, pid);
      GenericUSB_close(id);
    #else

      int ret;
      size_t count, i;
      DevHandle dev;
      UsbDeviceDescriptor descriptor;

      DevHandle *dev_list = NULL;
      count = libusb_get_device_list(ctx, &dev_list);

      if (count <= 0)
        return NULL;

      for (i=0; i < count; ++i)
      {
        dev = dev_list[i];
        uint8_t busnum = libusb_get_bus_number(dev);
        uint8_t devnum = libusb_get_device_address(dev);

        if (GenericUSB_examined(busnum, devnum))
          continue;

        ret = libusb_get_device_descriptor(dev, &descriptor);
        if (ret < 0)  {
          PrintError("Error getting device descriptor\n");
          return NULL;
        }
        vid = descriptor.idVendor;
        pid = descriptor.idProduct;

        name = DEVICE_get_imc_name(vid, pid, NO_ID, NO_ID);
        if (!name)
          continue;

        sysname = UsbMapper_get_sysname(busnum, devnum);

        int found = UsbMapper_get_sysname_id(sysname);
        if (found)
        {
          Debug(" Device %d already Mapped as %s\n", found, DeviceMap[found]->sysname);
          if (sysname) free(sysname);
            sysname = NULL;
        }
        else
        {
          Debug("  Found %s (%s) (%04X:%04X)\n", name, sysname, vid, pid);
          break;
        }
      }

      libusb_free_device_list(dev_list, 1);

    #endif// LIBUSBHOST
    #endif // WIN32
  } // while( No sysname )

  if (sysname)
    Debug(" Mapping device %d as %s...\n", id, sysname);

  libusb_dev_t* device = UsbMapper_new_device(id, sysname);

  if (devname)
  {
    device->devname = strdup(devname);
    free(devname);
  }

  char* desc = NULL;
  if (sysname && name)
  {
    size_t len = strlen(sysname) + 64;
    desc = (char*)MEM_malloc(len+1);
    snprintf(desc, len, "%10s   %04X:%04X   %s", name, vid, pid, sysname);
  }
  if (sysname)
    free(sysname);
  return desc;
}

/*

#define USB2SER_IOCTL_INDEX 0x0800

// For GP0
#define GP0_OUTPUT_ENABLE CTL_CODE(FILE_DEVICE_UNKNOWN,\
                 USB2SER_IOCTL_INDEX+20,\
                 METHOD_BUFFERED, \
                 FILE_ANY_ACCESS)

#define GP0_SET_VALUE   CTL_CODE(FILE_DEVICE_UNKNOWN,\
             USB2SER_IOCTL_INDEX+22,\
                 METHOD_BUFFERED,\
                 FILE_ANY_ACCESS)

uint8_t GenericUSB_set_gp0(uint8_t channel, uint8_t value)
{
  return false;
}

int GenericUSB_prolific_open_port(const char *port_name, int *port_handle)
{
  return 1;
}

DWORD GenericUSB_prolific_close_port(int *port_handle)
{
  return 1;
}

int GenericUSB_prolific_set_gpio(int port_handle, int gpio_num, int gpio_level)
{
  return 1;
}

uint8_t GenericUSB_stop_prolific_board_reset(uint8_t channel)
{
  return false;
}

uint8_t GenericUSB_init_prolific_board_reset(uint8_t channel, char* reset_port_name, uint32_t gpio_mask)
{
  return false;
}

 void GenericUSB_prolific_port_reset_cb(uint8_t channel)
{
  //Debug("Linux Prolific GPIO Reset\n");
}
*/
#else

void GenericUSB_init() { return; }

void GenericUSB_destroy() { return; }

char* GenericUSB_mapper(int id) { return 0; }

int GenericUSB_load_map(const char* file) { return 0; }

int GenericUSB_save_map(const char* file) { return 0; }

comport_handler_t *GenericUSB_open(uint8_t id, comport_cfg_t *comport_cfg,
                                 comm_error_report_t error_report)
  { return 0; }

uint8_t GenericUSB_close(uint8_t id) { return 0; }

uint8_t GenericUSB_read(uint8_t id, uint8_t **block, uint32_t bytes_to_read,
                        uint32_t *bytes_read, comport_fx_rx_t fx)
  { return 0; }

uint8_t GenericUSB_write(uint8_t id, uint8_t *block, uint32_t bytes_to_send,
                         uint32_t *bytes_send, comport_fx_tx_t fx)
  { return 0; }

#endif //< DISABLE_LIBUSB
