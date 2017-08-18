/*  -------------------------------------------------------------------------
    Copyright (C) 2011-2014 Intel Mobile Communications GmbH

         Sec Class: Intel Confidential (IC)

     ----------------------------------------------------------------------
     This document contains proprietary information belonging to IMC.
     Design A/S. Passing on and copying of this document, use and communication
     of its contents is not permitted without prior written authorisation.
     ----------------------------------------------------------------------
     Description:
     Uses the sysfs to find information about devices attached to the system.
     ----------------------------------------------------------------------
     Revision Information:
       $$File name:  /msw_tools/FlashTool/libFlsTool/src/WinRegistry.h $
       $$CC-Version: .../dhkristx_140314_v0/1 $
       $$Date:       2014-04-16    12:25:22 UTC $
     ---------------------------------------------------------------------- */

#ifndef WINREG_H_
#define WINREG_H_

#ifdef WIN32
  #include <windows.h>
  #include <stdint.h>

  #ifdef __cplusplus
    #include <string>

    using namespace std;

    namespace ipa {
      namespace winreg {
 
      /**
       * WinReg function for getting a specific parameter value
       */
      string get_key_value(string key, string param);

      /**
       * WinReg_find_device
       *
       * Locates the first detected device with VID and PID. If the number
       * argument is set, then it will return the [number] detected device.
       */
      string find_device(uint16_t vid, uint16_t pid, int number = 1);

      /**
       * WinReg_device_exist
       *
       * Returns true or false if the device exists.
       *
       * TODO: Support for \\.\USBx
       *
       */
      bool device_exist(string& port);

      } /* namespace winreg */
    } /* namespace ipa */

    extern "C" {
  #endif
    /** ANSI C */

    const char* WinReg_get_key_value(const char* key, const char* param);

    const char* WinReg_find_device(uint16_t vid, uint16_t pid, int number);

    uint8_t WinReg_device_exist(const char* port);

  #ifdef __cplusplus
    }
  #endif

#endif /* WIN32 */
#endif /* WINREG_H_ */
