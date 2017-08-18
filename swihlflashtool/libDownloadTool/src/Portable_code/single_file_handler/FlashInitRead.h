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
* Description: Interface for: Functions for reading INI files.
*
*
* Revision Information:
*  $File name:  /msw_tools/FlashTool/libDownloadTool/src/Portable_code/single_file_handler/FlashInitRead.h $
*  $CC-Version: .../oint_tools_flashtoole2_01/6 $
*  $Date:       2014-03-13    15:40:41 UTC $
*   Resonsible:
*   Comment:
*
*/

#ifndef FlashInitReadH
#define FlashInitReadH

#include "OS.h"
#include "IFWD_single_file_handler.h"

#ifdef __cplusplus
extern "C" {
#endif

extern uint8_t FlashInitRead(char *filename, FlashInfoStruct *cfi);
extern uint8_t FileFindText(FILE *in, char *find);

#ifdef __cplusplus
}
#endif


#endif
