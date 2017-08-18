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
       $$File name:  /msw_tools/FlashTool/libFlsTool/src/SysFs.h $
       $$CC-Version: .../dhkristx_140314_v0/1 $
       $$Date:       2014-04-16    12:30:37 UTC $
     ---------------------------------------------------------------------- */

#ifndef WIN32
#ifndef SYSFS_H_
#define SYSFS_H_

#include <stdint.h>         //< uint16_t

#ifdef __cplusplus
extern "C" {
#endif

  char* get_string_from_file(const char* path, const char* file);
  int get_int_from_file(const char* path, const char* file);
  int get_hex_from_file(const char* path, const char* file);

  char *SysFs_find_tty_device(const char* tty);
  char *SysFs_get_device_tty(const char* devpath);
  char *SysFs_find_device(uint16_t vid, uint16_t pid, int number);

  uint8_t SysFs_find_tty_ids(const char* tty, uint16_t* vid, uint16_t* pid);

  uint8_t SysFs_get_ids(const char* syspath, uint16_t* vid, uint16_t* pid);

  //char *SysFs_get_device_list();
  int SysFs_get_device_busnum(const char* syspath);
  int SysFs_get_device_devnum(const char* syspath);

  // void SysFs_dump_device_capabilities(const char*syspath, int indent);
#ifdef __cplusplus
}

/*
#include <string>

using namespace std;

namespace ipa
{
  namespace sysfs
  {
    //< Examples that would return e.g. "1-1.1.1"
    // ANSI C. char* sysfs = SysFs_find_tty_device("ttyUSB1");
    // C++ ex. string sysfs = ipa::sysfs::find_tty("ttyUSB1");

    inline string find_tty(string tty) { return SysFs_find_tty_device(tty.c_str()); }

    //< Examples that would return e.g. "ttyUSB1"
    // ANSI C. char* tty = SysFs_get_device_tty("1-1.1.1");
    // C++ ex. string tty = ipa::sysfs::get_tty("1-1.1.1");

    inline string get_tty(string devpath) { return SysFs_get_device_tty(path.c_str()); }

    ...
  }
}
*/

#endif

#endif /* SYSFS_H_ */
#endif /* WIN32 */
