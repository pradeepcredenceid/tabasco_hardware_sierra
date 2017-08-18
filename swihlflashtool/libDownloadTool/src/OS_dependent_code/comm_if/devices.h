/*  -------------------------------------------------------------------------
    Copyright (C) 2012-2014 Intel Mobile Communications GmbH

         Sec Class: Intel Confidential (IC)

     ----------------------------------------------------------------------
     This document contains proprietary information belonging to IMC.
     Design A/S. Passing on and copying of this document, use and communication
     of its contents is not permitted without prior written authorisation.
     ----------------------------------------------------------------------
     Description:
     This file is used to identify the devices supported by libusb & WinUSB
     ----------------------------------------------------------------------
     Revision Information:
       $$File name:  /msw_tools/FlashTool/libDownloadTool/src/OS_dependent_code/comm_if/devices.h $
       $$CC-Version: .../dhkristx_140314_v0/3 $
       $$Date:       2014-05-02    13:10:17 UTC $
     ---------------------------------------------------------------------- */

#ifndef DEVICES_H_
#define DEVICES_H_

#ifdef _WIN32
  #include <windows.h>
  #include <winnt.h>
  #include <stdint.h>
#else
  #include <linux/stddef.h>
#endif

// Chip IDs in Fls files
#define E_GOLD_CHIPSET_V2     0
#define E_GOLD_LITE_CHIPSET   1
#define S_GOLD_CHIPSET_V1     2
#define S_GOLD_LITE_CHIPSET   3
#define E_GOLD_CHIPSET_V3     4
#define S_GOLD_CHIPSET_V2     5
#define S_GOLD_CHIPSET_V3     6
#define E_GOLD_RADIO_V1       7
#define E_GOLD_VOICE_V1       8
#define S_GOLD_RADIO_V1       9
#define X_GOLD_618            10
#define X_GOLD_102            11
#define X_GOLD_213            12
#define X_GOLD_110            13
#define X_GOLD_626            14
#define X_GOLD_223            15
#define X_GOLD_215            16
#define X_GOLD_706            17
#define X_GOLD_636            18
#define X_GOLD_225            19
#define X_GOLD_716            20
#define X_GOLD_631            21
#define X_GOLD_221            22
#define X_GOLD_726            23
#define X_GOLD_632            24
#define X_GOLD_222            25
#define X_GOLD_633            26

// Defining the SGold/XGold boot response codes
#define BOOT_ACK_NOSEC_SG1           0xB0
#define BOOT_ACK_NOSEC_SG2           0xC0
#define BOOT_ACK_NOSEC_SG3           0xD0
#define BOOT_ACK_NOSEC_SG3_BC_V1     0xD1
#define BOOT_ACK_NOSEC_SG3_BC_V2     0xD2
#define BOOT_ACK_NOSEC_SG3_BC_V3     0xD3
#define BOOT_ACK_NOSEC_SG3_BC_V4     0xD4
#define BOOT_ACK_NOSEC_SG3_BC_V5     0xD5
#define BOOT_ACK_NOSEC_SG3_BC_V6     0xD6
#define BOOT_ACK_NOSEC_SGR_BC_V1     0xE1
#define BOOT_ACK_NOSEC_SGR_BC_V2     0xE2
#define BOOT_ACK_NOSEC_SGR_BC_V3     0xE3
#define BOOT_ACK_NOSEC_SGR_BC_V4     0xE4
#define BOOT_ACK_NOSEC_SGR_BC_V5     0xE5
#define BOOT_ACK_NOSEC_SGR_BC_V6     0xE6

#define BOOT_ACK_NOSEC_XGOLD213_ES1  0xB1
#define BOOT_ACK_NOSEC_XGOLD213_ES2  0xB2
#define BOOT_ACK_NOSEC_XGOLD213_ES3  0xB3

#define BOOT_ACK_NOSEC_XG618_ES1     0xA1
#define BOOT_ACK_NOSEC_XG618_ES2     0x50
#define BOOT_ACK_NOSEC_XG626         0x51
#define BOOT_ACK_NOSEC_XG626_ES1     0x91
#define BOOT_ACK_NOSEC_XG626_ES2     0x92
#define BOOT_ACK_NOSEC_XG636         0x52
#define BOOT_ACK_NOSEC_XG706         0x53
#define BOOT_ACK_NOSEC_XG716         0x54
#define BOOT_ACK_NOSEC_XG631         0x55
#define BOOT_ACK_NOSEC_XG726         0x58
#define BOOT_ACK_NOSEC_XG632         0x59
#define BOOT_ACK_NOSEC_XG633         0x5A
#define BOOT_ACK_NOSEC_XG632_ES1     0x5C // XG642 (Sofia)


#define BOOT_ACK_NOSEC_XG223         0x16
#define BOOT_ACK_NOSEC_XG215         0x17
#define BOOT_ACK_NOSEC_XG225         0x18
#define BOOT_ACK_NOSEC_XG221         0x19
#define BOOT_ACK_NOSEC_XG222         0x25

#define NO_ID -1

#ifdef __cplusplus
extern "C" {
#endif


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
const char* DEVICE_get_imc_name(int vid, int pid, int cid, int cen);

#ifdef WIN32
  /* Just to avoid unused variable */
  const GUID* DEVICE_get_guid();

  /**
   * Locating FlashUSB devices via WinRegister
   *
   * @param dev        Saves the syspath here in found
   * @param max_size   Size of dev
   *
   * @return           True if found
   */
  uint8_t WinUSB_get_device_list(char* dev, size_t max_size);

  /**
   * Locates the symbolic name from a syspath
   *
   * @param syspath  A syspath (WinUSB_get_device_list)
   *
   * @return  The symbolic name (e.g. USB1)
   */
  char* WinUSB_get_device_name(char *syspath);

  /**
   * Extracts the VID & PID from a syspath
   *
   * @param syspath  A syspath (WinUSB_get_device_list)
   * @param vid      Pointer to the extracted VID
   * @param pid      Pointer to the extracted PID
   *
   * @return  True if successfully extracted
   */
  uint8_t WinUSB_get_vid_pid(const char *syspath, uint32_t *vid, uint32_t *pid);
#endif

#ifdef __cplusplus
}
#endif

#endif /* DEVICES_H_ */
