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
 *  $File name:  /msw_tools/FlashTool/libDownloadTool/src/OS_dependent_code/comm_if/GenericUSB.h $
 *  $CC-Version: .../dhkristx_140314_v0/2 $
 *  $Date:       2014-05-02    13:10:09 UTC $
 */
#ifndef LIBUSB_H_
#define LIBUSB_H_

#include "OS.h"
#include "devices.h"
#include "IFWD_comport.h"

#ifndef MAX
  #define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#define USB_CLASS_COMM             0x02
#define USB_CLASS_DATA             0x0A

//< This define can be used instead of testing for INTERFACE
// 		#define USB_CLASS_DIAG             0xDC
// Using this (interface->bInterfaceClass == USB_CLASS_DIAG)
// Instead of (interface->bDescriptorType == USB_DT_INTERFACE)

#define USB_ENDPOINT_IN            0x80
#define USB_ENDPOINT_OUT           0x00

#define USB_DEV_DIR "/dev/bus/usb"
#define USB_SYS_DIR "/sys/bus/usb/devices"

#ifndef _WIN32

  #define ERROR_NO_MEM  ENOMEM

  #ifndef LIBUSBHOST
    #include <libusb.h>

    typedef struct libusb_context*             CtxHandle;

    typedef struct libusb_device*              DevHandle;
    typedef struct libusb_device_handle*       UsbHandle;
    typedef struct libusb_device_handle*       CommHandleType;

    typedef struct libusb_interface            UsbInterface;
    typedef struct libusb_config_descriptor    UsbConfigDescriptor;
    typedef struct libusb_interface_descriptor UsbInterfaceDescriptor;
    typedef struct libusb_device_descriptor    UsbDeviceDescriptor;
    typedef struct libusb_endpoint_descriptor  UsbEndpointDescriptor;

    #define USB_DT_INTERFACE           LIBUSB_DT_INTERFACE
    #define USB_DT_ENDPOINT            LIBUSB_DT_ENDPOINT
    #define ERROR_NODEV                LIBUSB_TRANSFER_NO_DEVICE
    #define GENERIC_ERROR_TIMEOUT      LIBUSB_ERROR_TIMEOUT

    #define is_bulk_endpoint(attr) \
          ((attr & LIBUSB_TRANSFER_TYPE_MASK) == LIBUSB_TRANSFER_TYPE_BULK)

  #else
    #include <usbhost/usbhost.h>

    typedef struct usb_host_context*        CtxHandle;

    typedef const char*                     DevHandle;
    typedef struct usb_device*              UsbHandle;
    typedef struct usb_device*              CommHandleType;

    typedef struct usb_interface            UsbInterface;
    typedef struct usb_config_descriptor    UsbConfigDescriptor;
    typedef struct usb_interface_descriptor UsbInterfaceDescriptor;
    typedef struct usb_device_descriptor    UsbDeviceDescriptor;
    typedef struct usb_endpoint_descriptor  UsbEndpointDescriptor;

    #define GENERIC_ERROR_TIMEOUT ETIMEDOUT
    #define ERROR_NODEV   ENODEV

    #define GetErrorString(code)              strerror(errno)

    #define is_bulk_endpoint(attr) \
          ((attr & USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_BULK)
  #endif

#else //< WIN32
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <time.h>
  #include <unistd.h>
  #include <stdarg.h>
  #include <errno.h>        //< ETIMEDOUT

  #include <windows.h>
  #include <conio.h>
  #include <setupapi.h>

  #include <ctype.h>
  #include <fcntl.h>
  #include <process.h>
  #include <inttypes.h>
  #include <objbase.h>
  #include <initguid.h> //< GUID_DEVCLASS_USB

  typedef HANDLE CtxHandle;

  typedef char*   DevHandle;
  typedef HANDLE  UsbHandle;
  typedef HANDLE  CommHandleType;
  #undef interface      //< This is defined in basetyps.h

  // #define WSAETIMEDOUT 10060L
  #ifndef ENOTCONN
    #define ENOTCONN                WSAENOTCONN   //< errno.h equivalent
  #endif
  //#define ETIMEDOUT               ERROR_SEM_TIMEOUT
  // #define GENERIC_ERROR_TIMEOUT   ERROR_SEM_TIMEOUT
  #define GENERIC_ERROR_TIMEOUT   -ERROR_SEM_TIMEOUT
  //#define ERROR_NODEV             -6    // ERROR_INVALID_HANDLE
  #define ERROR_NODEV             -31
  #define ERROR_NO_MEM            ERROR_NOT_ENOUGH_MEMORY // 0x8

  #define DEVICE_SPEED            0x01
  #define MAX_DEVPATH_LENGTH      256

  //#include <ddk/usbdi.h>
  //#include <ddk/usb100.h>
  #define SHORT_PACKET_TERMINATE            0x01
  #define AUTO_CLEAR_STALL                  0x02
  #define PIPE_TRANSFER_TIMEOUT             0x03
  #define IGNORE_SHORT_PACKETS              0x04
  #define ALLOW_PARTIAL_READS               0x05 //< Default TRUE
  #define WINUSB_AUTO_FLUSH                 0x06 //< Zero-length Packets on read
  #define WINUSB_RAW_IO                     0x07
  #define MAXIMUM_TRANSFER_SIZE             0x08

  typedef enum _USBD_PIPE_TYPE {
    UsbdPipeTypeControl,
    UsbdPipeTypeIsochronous,
    UsbdPipeTypeBulk,
    UsbdPipeTypeInterrupt
  } USBD_PIPE_TYPE;

  typedef struct {
    USBD_PIPE_TYPE PipeType;
    UCHAR          PipeId;              //< bEndpointAddress
    USHORT         MaximumPacketSize;   //< wMaxPacketSize
    UCHAR          Interval;
  } WINUSB_PIPE_INFORMATION;

  typedef struct usb_device_descriptor {
    UCHAR  bLength;
    UCHAR  bDescriptorType;
    USHORT bcdUSB;
    UCHAR  bDeviceClass;
    UCHAR  bDeviceSubClass;
    UCHAR  bDeviceProtocol;
    UCHAR  bMaxPacketSize0;
    USHORT idVendor;
    USHORT idProduct;
    USHORT bcdDevice;
    UCHAR  iManufacturer;
    UCHAR  iProduct;
    UCHAR  iSerialNumber;
    UCHAR  bNumConfigurations;
  } UsbDeviceDescriptor, *PUSB_DEVICE_DESCRIPTOR;

  typedef struct usb_interface_descriptor {
    UCHAR  bLength;
    UCHAR  bDescriptorType;
    UCHAR  bInterfaceNumber;
    UCHAR  bAlternateSetting;
    UCHAR  bNumEndpoints;
    UCHAR  bInterfaceClass;
    UCHAR  bInterfaceSubClass;
    UCHAR  bInterfaceProtocol;
    UCHAR  iInterface;
  } UsbInterfaceDescriptor, *PUSB_INTERFACE_DESCRIPTOR;

  typedef struct usb_interface {
    UsbHandle handle;
    UsbInterfaceDescriptor *altsetting;
    int num_altsetting;
  } UsbInterface;

  typedef struct usb_config_descriptor {
    int bNumInterfaces;
    struct usb_interface *interface;
  } UsbConfigDescriptor;

  /*
   * API macros for Loading DLL functions
   */
  #define DLL_DECLARE_PREFIXNAME(api, ret, prefixname, name, args) \
    typedef ret (api * __dll_##name##_t)args;                     \
    static __dll_##name##_t prefixname = NULL

  #define DLL_DECLARE(api, ret, name, args) \
    DLL_DECLARE_PREFIXNAME(api, ret, name, name, args)

  #define DLL_LOAD_PREFIXNAME(dll, prefixname, name, ret_on_failure) \
    do {                                                           \
      HMODULE h = GetModuleHandleA(dll);                          \
    if (!h)                                                        \
      h = LoadLibraryA(dll);                                      \
    if (!h) {                                                      \
      if (ret_on_failure)                                          \
        PrintError("Could not load %s!\n", dll);                   \
      else                                                         \
        break;                                                     \
    }                                                              \
    prefixname = (__dll_##name##_t)GetProcAddress(h, #name);       \
    if (prefixname) break;                                         \
    prefixname = (__dll_##name##_t)GetProcAddress(h, #name "A");   \
    if (prefixname) break;                                         \
    prefixname = (__dll_##name##_t)GetProcAddress(h, #name "W");   \
    if (prefixname) break;                                         \
    if(ret_on_failure)                                             \
      PrintError("Error loading %s from %s", name, dll);            \
    } while(0)

    #define DLL_LOAD(dll, name, ret_on_failure) \
      DLL_LOAD_PREFIXNAME(dll, name, name, ret_on_failure)

#endif

#ifdef __cplusplus
  extern "C" {
#endif

void GenericUSB_init();
void GenericUSB_destroy();

#ifdef WIN32
/**
 * Set an alternative GUID when looking for WinUSB devices (Windows only)
 */
uint8_t GenericUSB_set_GUID(const char* guid_str);
#endif

/**
 * Mapping USB devices to Channel IDs
 */
char* GenericUSB_mapper(int id);

uint8_t GenericUSB_map(int id, const char* sysname);

int GenericUSB_load_map(const char* file);

int GenericUSB_save_map(const char* file);


/**
 * This function actually does not open anything.
 * Because of the current API this is the looping function checking for new
 * devices.
 *
 * If an IMC device is detected it is opened with LIBUSB_open_device.
 */
comport_handler_t *GenericUSB_open(uint8_t id, comport_cfg_t *comport_cfg,
                                   comm_error_report_t error_report);


/**
 * Does a couple of checks before opening a USB device.
 *
 * This function also removes any kernel drivers connected to the device.
 */
//libusb_dev_t *GenericUSB_open_device(DevHandle dev, int id);

/**
 * Close a device
 */
uint8_t GenericUSB_close(uint8_t id);

/**
 * Bulk read
 */
uint8_t GenericUSB_read(uint8_t id, uint8_t **block, uint32_t bytes_to_read,
                        uint32_t *bytes_read, comport_fx_rx_t fx);
/**
 * Bulk write
 */
uint8_t GenericUSB_write(uint8_t id, uint8_t *data, uint32_t bytes_to_send,
                         uint32_t *bytes_send, comport_fx_tx_t fx);

/**
 * Control Transfer

 int GenericUSB_ioctl_I(uint8_t id, unsigned int req, ...);
*/

#ifdef __cplusplus
  }
#endif

#endif /* LIBUSB_H_ */
