/*
 *       Copyright (C) 2011-2014 Intel Mobile Communications GmbH
 *
 *            Sec Class: Intel Confidential (IC)
 *
 *
 * This document contains proprietary information belonging to IMC.
 * Denmark A/S. Passing on and copying of this document, use and communication
 * of its contents is not permitted without prior written authorisation.
 *
 * Description:
 *  Helper functions for mainly filesystem related tasks
 *
 * Revision Information:
 *  $File name:  /msw_tools/FlashTool/libFlsTool/src/Security.cpp $
 *  $CC-Version: .../oint_tools_flashtoole2_01/5 $
 *  $Date:       2014-03-13    15:37:18 UTC $
 *   Resonsible:
 *   Comment:
 *
 */

#ifndef __CC_ARM
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Security.h"
#include "FileIO.h"

#include <time.h>      // needed for "srand()"
#include <fcntl.h>
#include <sys/types.h> // getpid
#include <unistd.h>    // getpid

#include <string>

using namespace std;

#define MAXPATH   260

/*---------------------------------------------*/
/* extern variables                            */
/*---------------------------------------------*/
//extern char dll_file_path[];
#ifdef _WIN32
  #define DEFAULT_SEC_LIBRARY "IFWD_Coder.dll"
#endif

#ifdef __linux__
  #define DEFAULT_SEC_LIBRARY "libIFWD_Coder.so"
#endif

#ifdef __APPLE__
  #define DEFAULT_SEC_LIBRARY "dylibIFWD_Coder.dylib"
#endif


/*---------------------------------------------*/
/* local variables                             */
/*---------------------------------------------*/

/* char *(*IFWD_get_dll_version)(void) = NULL; */
//static char sec_dll_file[MAXPATH] = "";

/* error messages: */
static const char * const NoDongle      = "No dongle";
static const char * const BadSecureData = "Bad Secure Data";
static const char * const BadDongle     = "Wrong Dongle";
static const char * const BadDll        = "Error in DLL - missing functions";

/*--------------------------------------------------------------------------------------*/
uint8_t DecryptSecPack(SecurityStructType *Security, char **status_text)
{
  #if !defined(_WIN32) && !defined(LINT)
    return true;
  #endif

  uint8_t result = true;

  *status_text = NULL; /* mark as no error for now */
  if(Security->LoadMagic != LOAD_MAGIC) /* do we need to decrypt at all? */
  {
    HINSTANCE hInst;
    void (*IFWD_Decode)(uint8_t*,uint8_t*,uint32_t);

	#if 0 //def __BORLANDC__
	  std::string dllname = FileGetPath(Application->ExeName) + DEFAULT_SEC_LIBRARY;
    #else
      std::string dllname = DEFAULT_SEC_LIBRARY;
    #endif
    hInst = LoadLibrary(dllname.c_str());
    if(hInst)
    {
      /* IFWD_get_dll_version = (char *(*)(void))GetProcAddress(hInst, "_IFWD_get_dll_version"); */
      IFWD_Decode = (void (*)(uint8_t*,uint8_t*,uint32_t))GetProcAddress(hInst, "_IFWD_Decode");  /*lint !e611 */
      if(IFWD_Decode)
      {
        IFWD_Decode((uint8_t*)Security, (uint8_t*)Security, sizeof(SecurityStructType));
        if(Security->LoadMagic != LOAD_MAGIC)
        {
          *status_text = (char *)BadDongle;
          result = false;
        }
      }
      else
      {
        *status_text = (char *)BadDll;
        result = false;
      }
      if(hInst)
         FreeLibrary(hInst);
    }
    else
    {
      if(FileSystem_isfile(dllname))
        *status_text = (char *)NoDongle;
      else
        *status_text = (char *)BadSecureData;
      result = false;
    }
  }
  return result;
}
/*--------------------------------------------------------------------------------------*/

#endif
