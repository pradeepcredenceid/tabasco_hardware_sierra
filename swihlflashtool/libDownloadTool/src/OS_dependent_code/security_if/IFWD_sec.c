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
 *  $File name:  /msw_tools/FlashTool/libDownloadTool/src/OS_dependent_code/security_if/IFWD_sec.c $
 *  $CC-Version: .../oint_tools_flashtoole2_01/8 $
 *  $Date:       2014-03-13    15:42:25 UTC $
 *   Responsible: bentzen, Hilmar
 *   Comment:
 *     SMS00847521: "Need to remove Lint warnings..."
 */

#ifdef MSVC
/*#include "stdafx.h"*/
#endif

#include <string.h>    /* C standard lib: Containing strcpy memcpy etc. */
#include <stdio.h>     /* C standard lib: Containing printf scanf etc. */

#include <stdlib.h>
#include <string.h>

#include "OS.h"
#include "FileIO.h"
#include "PrgHandler.h"

#include "IFWD_sec.h"
#include "IFWD_mem.h"
#include "c_sha1.h"


#ifdef _WIN32
	#include <windows.h>

	typedef HINSTANCE LibHandle;
	#define SecLibrary "\\IFWD_Coder.dll"
#else
	#include <dlfcn.h>

	typedef void* LibHandle;
	#define SecLibrary "/libIFWD_Coder.so"

	#define LoadLibrary( file )     dlopen(file, RTLD_LAZY)
	#define FreeLibrary( handle )	dlclose( handle )
	#define GetProcAddress( handle, func )	dlsym( handle, func )
#endif

typedef void (*IFWD_DecodeFunc)(uint8_t*,uint8_t*,uint32_t);

LibHandle libhandle = NULL;

extern char dll_file_path[];

/*---------------------------------------------*/
/* local variables                             */
/*---------------------------------------------*/

/* char *(*IFWD_get_dll_version)(void) = NULL; */
static char sec_dll_file[MAX_PATH] = "";
static SHA1Context Sha1Ctx;

/* error messages: */
static const char * const NoDongle      = "No dongle";
static const char * const BadSecureData = "Bad Secure Data";
static const char * const BadDongle     = "Wrong Dongle";
static const char * const BadDll        = "Error in DLL - missing functions";
static const char * const BadHash       = "Data signature check failed";

/*--------------------------------------------------------------------------------------*/
uint8_t DecryptSecPack(SecurityStructType *Security, char **status_text, FILE *hash_file)
{
  /* the sec pack is a reserved Windows DLL which code is not available */
  #ifdef ANDROID
    return true;
  #endif

  uint8_t *p;
  uint8_t result = true;
  IFWD_DecodeFunc IFWD_Decode = NULL;

  *status_text = NULL; /* mark as no error for now */
  if(Security->LoadMagic == LOAD_MAGIC) /* do we need to decrypt at all? */
	return true;

	if(*sec_dll_file == 0)  /* Have we extracted the DLL file name? */
	  snprintf(sec_dll_file, MAX_PATH, "%s%s", dll_file_path, SecLibrary);

	// == Loading the Library ==
	if(FILE_exists(sec_dll_file))
	{
    #if defined(linux) || defined(ANDROID)
      printf("Loading Security Library (%s)...\n", sec_dll_file);
    #endif
    libhandle = LoadLibrary(sec_dll_file);

    if(!libhandle)
	  {
      if(FILE_exists(sec_dll_file))
        *status_text = (char *)NoDongle;
      else
        *status_text = (char *)BadSecureData;
      return false;
    }

    /*lint -e611 */
    IFWD_Decode = (IFWD_DecodeFunc)GetProcAddress(libhandle, "_IFWD_Decode");
    /*lint +e611 */
    if(!IFWD_Decode) {
      *status_text = (char *)BadDll;
      return false;
    }
  }

	// == Running the Decoding process ==
  if (IFWD_Decode) // LINT
  {
    IFWD_Decode((uint8_t*)Security, (uint8_t*)Security, sizeof(SecurityStructType));
  }
	if(Security->LoadMagic != LOAD_MAGIC)
	{
	  *status_text = (char *)BadDongle;
	  result = false;
	}
	else
	{
	  if (hash_file != NULL)
	  {
		p = MEM_malloc((uint32_t)Security->LoadMap[0].TotalLength);

		FILE_read(hash_file, p, Security->LoadMap[0].TotalLength);

		SHA1Reset(&Sha1Ctx);
		SHA1Input(&Sha1Ctx, p, Security->LoadMap[0].UsedLength);
		SHA1Result(&Sha1Ctx);
		MEM_free(p);

		if ((Security->FileHash[0] != Sha1Ctx.Message_Digest[0]) ||
			(Security->FileHash[1] != Sha1Ctx.Message_Digest[1]) ||
			(Security->FileHash[2] != Sha1Ctx.Message_Digest[2]) ||
			(Security->FileHash[3] != Sha1Ctx.Message_Digest[3]) ||
			(Security->FileHash[4] != Sha1Ctx.Message_Digest[4]) )
		{
		  *status_text = (char *)BadHash;
		  result = false;
		}
	   }
	 }

  if(libhandle)
	FreeLibrary(libhandle);

  return result;
}
/*--------------------------------------------------------------------------------------*/

