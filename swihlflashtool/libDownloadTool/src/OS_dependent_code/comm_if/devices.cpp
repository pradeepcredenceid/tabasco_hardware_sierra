/*
 *       Copyright (C) 2013-2014 Intel Mobile Communications GmbH
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
 *  $File name:  /msw_tools/FlashTool/libDownloadTool/src/OS_dependent_code/comm_if/devices.cpp $
 *  $CC-Version: .../dhkristx_140314_v0/4 $
 *  $Date:       2014-05-02    13:10:13 UTC $
 *   Responsible: bentzen
 *   Comment:
 *     SMS00897775
 */

#ifndef __DEVICES_H__
#define __DEVICES_H__

#include "devices.h"

#ifdef _WIN32
  #include "WinRegistry.h"

  using namespace ipa;
#endif

#include <string>
#include <stdio.h>

using namespace std;

// #define THIS_MODULE MODULE_COMM
// #include "DebugModule.h"


#ifdef _WIN32
  //static GUID GUID_PHONE = {0x88BAE032, 0x5A81, 0x49F0, {0xBC, 0x3D, 0xA4, 0xFF, 0x13, 0x82, 0x16, 0xD6}};
  static GUID GUID_PHONE = {0x88BAE032, 0x5A81, 0x49F0, {0xBC, 0x3D, 0xA4, 0xFF, 0x13, 0x82, 0x16, 0xD6}};

  //static GUID GUID_FLASHUSB = {0x86E0D1E0, 0x8089, 0x11D0, {0x9C, 0xE4, 0x08, 0x00, 0x3E, 0x30, 0x1F, 0x73}};

  /* Just to avoid unused variable */
  const GUID* DEVICE_get_guid()
  {
    return &GUID_PHONE;
  }
#endif

static struct DeviceListType {
  int chip_enum;
  int chip_id;
  int vid;
  int pid;
  const char* name;
  const char* desc;
} device_list[] = {
    { NO_ID     , NO_ID                      , 0x058b, 0x0015, "S-Gold",       "" },
    { NO_ID     , NO_ID                      , 0x058b, 0x0041, "X-Gold",       "" },
    { X_GOLD_213, BOOT_ACK_NOSEC_XGOLD213_ES1, 0x058b, 0x0041, "XG213-ES1",    "" },
    { X_GOLD_213, BOOT_ACK_NOSEC_XGOLD213_ES2, 0x058b, 0x0041, "XG213-ES2",    "" },
    { X_GOLD_213, BOOT_ACK_NOSEC_XGOLD213_ES3, 0x058b, 0x0041, "XG213-ES3",    "" },
    { X_GOLD_618, BOOT_ACK_NOSEC_XG618_ES1   , 0x058b, 0x0041, "XG618-ES1",    "" },
    { X_GOLD_618, BOOT_ACK_NOSEC_XG618_ES2   , 0x058b, 0x0041, "XG618-ES2",    "" },
    { X_GOLD_626, BOOT_ACK_NOSEC_XG626       , 0x058b, 0x0041, "XG626",        "" },
    { X_GOLD_223, BOOT_ACK_NOSEC_XG223       , 0x058b, 0x0041, "XG223",        "" },
    { X_GOLD_215, BOOT_ACK_NOSEC_XG215       , 0x058b, 0x0041, "XG215",        "" },
    { X_GOLD_632, BOOT_ACK_NOSEC_XG632       , 0x058b, 0x0041, "XG632",        "" }, // TODO Does this one not have new USB ID?
    { X_GOLD_633, BOOT_ACK_NOSEC_XG633       , 0x058b, 0x0041, "XG633",        "" }, // TODO Does this one not have new USB ID?
    { NO_ID     , BOOT_ACK_NOSEC_XG632_ES1   , 0x058b, 0x0041, "XG632_ES1",    "" }, // TODO Is this Sofia3G ? have new USB ID?

    { X_GOLD_706, BOOT_ACK_NOSEC_XG706       , 0x8087, 0x0706, "XG706",        "" },

    { NO_ID     , NO_ID                      , 0x8087, 0x0716, "XG716/XG636",  "" },
    { X_GOLD_716, BOOT_ACK_NOSEC_XG716       , 0x8087, 0x0716, "XG716"      ,  "" },
    { X_GOLD_636, BOOT_ACK_NOSEC_XG636       , 0x8087, 0x0716, "XG636"      ,  "" },

    { NO_ID     , NO_ID                      , 0x8087, 0x0736, "Intel Mobile", "" },
    { NO_ID     , NO_ID                      , 0x8087, 0x07EC, "XG714",        "" },
    { NO_ID     , NO_ID                      , 0x8087, 0x07ED, "FlashPSI",     "" },
    { X_GOLD_631, BOOT_ACK_NOSEC_XG631       , 0x8087, 0x07EE, "XG631",        "" },
    { X_GOLD_726, BOOT_ACK_NOSEC_XG726       , 0x8087, 0x07EF, "XG726",        "" },

    { X_GOLD_225, BOOT_ACK_NOSEC_XG225       , 0x8087, 0x07F0, "XG225",        "" },
    { X_GOLD_221, BOOT_ACK_NOSEC_XG221       , 0x8087, 0x07F1, "XG221",        "" },
    { NO_ID     , NO_ID                      , 0x8087, 0x07F2, "XG631H",       "" },  // TODO what is this one XG631H
    { X_GOLD_222, BOOT_ACK_NOSEC_XG222       , 0x8087, 0x07F3, "XG222",        "" },
    { NO_ID     , NO_ID                      , 0x8087, 0x07F4, "SoLTE",        "" },  // TODO SoLTE shall be handled

    { NO_ID     , NO_ID                      , 0x8087, 0x07F5, "Intel Mobile", "" },
    { NO_ID     , NO_ID                      , 0x8087, 0x07F6, "Intel Mobile", "" },
    { NO_ID     , NO_ID                      , 0x8087, 0x07F7, "Intel Mobile", "" },
    { NO_ID     , NO_ID                      , 0x8087, 0x07F8, "Intel Mobile", "" },
    { NO_ID     , NO_ID                      , 0x8087, 0x07F9, "Intel Mobile", "" },
    { NO_ID     , NO_ID                      , 0x8087, 0x07FA, "Intel Mobile", "" },
    { NO_ID     , NO_ID                      , 0x8087, 0x07FB, "Intel Mobile", "" },
    { NO_ID     , NO_ID                      , 0x8087, 0x07FC, "Intel Mobile", "" },
    { NO_ID     , NO_ID                      , 0x8087, 0x07FD, "Intel Mobile", "" },
    { NO_ID     , NO_ID                      , 0x8087, 0x07FE, "Intel Mobile", "" },
    { NO_ID     , NO_ID                      , 0x8087, 0x07FF, "Intel Mobile", "" },

    { NO_ID     , NO_ID                      , 0x8087, 0x0800, "Intel Mobile", "" },
    { NO_ID     , NO_ID                      , 0x8087, 0x0801, "Intel Mobile", "" },
    { NO_ID     , NO_ID                      , 0x8087, 0x0802, "Intel Mobile", "" },
    { NO_ID     , NO_ID                      , 0x8087, 0x0803, "Intel Mobile", "" },
    { NO_ID     , NO_ID                      , 0x8087, 0x0804, "Intel Mobile", "" },
    { NO_ID     , NO_ID                      , 0x8087, 0x0805, "Intel Mobile", "" },
    { NO_ID     , NO_ID                      , 0x8087, 0x0806, "Intel Mobile", "" },
    { NO_ID     , NO_ID                      , 0x8087, 0x0807, "Intel Mobile", "" },
    { NO_ID     , NO_ID                      , 0x8087, 0x0808, "Intel Mobile", "" },
    { NO_ID     , NO_ID                      , 0x8087, 0x0809, "Intel Mobile", "" },
    { NO_ID     , NO_ID                      , 0x8087, 0x080A, "Intel Mobile", "" },
    { NO_ID     , NO_ID                      , 0x8087, 0x080B, "Intel Mobile", "" },
    { NO_ID     , NO_ID                      , 0x8087, 0x080C, "Intel Mobile", "" },
    { NO_ID     , NO_ID                      , 0x8087, 0x080D, "Intel Mobile", "" },
    { NO_ID     , NO_ID                      , 0x8087, 0x080E, "Intel Mobile", "" },
    { NO_ID     , NO_ID                      , 0x8087, 0x080F, "Intel Mobile", "" },
    { NO_ID     , NO_ID                      , NO_ID , NO_ID ,  NULL         , "" }
};


/**
 * Finds IMC/ProjectQ device names via PID, VID, ChipID and/or ChipEnum
 *
 * @param vid  Vendor ID to search for
 * @param pid  Product ID to search for
 * @param cid  Chip ID to search for
 * @param cen  Chip enum to search for
 *
 * Use the define NO_ID to exclude one or more inputs from the search.
 *
 * @return     The name of the device or NULL if not found
 **/
const char* DEVICE_get_imc_name(int vid, int pid, int cid, int cen)
{
  int i;
  for (i=0; device_list[i].pid != NO_ID; ++i)
  {
    if (((vid == NO_ID) || (vid == device_list[i].vid)) &&
        ((pid == NO_ID) || (pid == device_list[i].pid)) &&
        ((cid == NO_ID) || (cid == device_list[i].chip_id)) &&
        ((cen == NO_ID) || (cen == device_list[i].chip_enum)))
      return device_list[i].name;
  }
  //Debug(" NO device found.\n");
  return NULL;
}

#ifdef _WIN32
uint8_t WinUSB_get_device_list(char* dev, size_t max_size)
{
  for (size_t i=0; device_list[i].pid>0; ++i)
  {
    string dev_name = winreg::find_device(device_list[i].vid, device_list[i].pid);
    if (!dev_name.empty())
    {
      printf(" WinReg_find_device(%4X:%4X) %s\n",
             device_list[i].pid, device_list[i].vid, dev_name.c_str());
      snprintf(dev, max_size, "%s", dev_name.c_str());
      return true;
    }
  }
  return false;
}

char* WinUSB_get_device_name(char *syspath)
{
  string key = syspath;
  string name = winreg::get_key_value(key+"\\Device Parameters", "CreateFileName");

  return name.empty() ? NULL : strdup( name.c_str() );
}


uint8_t WinUSB_get_vid_pid(const char *syspath, uint32_t *vid, uint32_t *pid)
{
  char* ptr = strstr(syspath, "VID_");
  if (!ptr && !(ptr = strstr(syspath, "vid_")))
      return false;

  if (sscanf(ptr,  "vid_%04X&pid_%04X", vid, pid) == 2
   || sscanf(ptr,  "VID_%04X&PID_%04X", vid, pid) == 2)
  {
    return true;
  }
  return false;
}

#else

  //   uint8_t SysFs_find_tty_ids(argv[1], &vid, &pid);
  //   char   *SysFs_find_tty_device(const char* tty);

#endif

#endif
