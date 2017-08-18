/*
 *       Copyright (C) 2011-2013 Intel Mobile Communications GmbH
 *
 *            Sec Class: Intel Confidential (IC)
 *
 *
 * This document contains proprietary information belonging to IMC.
 * Denmark A/S. Passing on and copying of this document, use and communication
 * of its contents is not permitted without prior written authorisation.
 *
 * Description:
 *   IFWD download DLL Text definitions
 *     Text definitions from all the used modules are connected by defintions in this file.
 *
 * Revision Information:
 *  $File name:  /msw_tools/FlashTool/libDownloadTool/src/Portable_code/text_func/IFWD_text_def.h $
 *  $CC-Version: .../oint_tools_flashtoole2_01/5 $
 *  $Date:       2013-10-02    15:01:02 UTC $
 *   Responsible: bentzen
 *   Comment:
 *     SMS00847521: "Need to remove Lint warnings..."
 */

#ifndef _INC_IFWD_TEXT
#define _INC_IFWD_TEXT

/*---------------------------------------------*/
/* Text declarations                           */
/*---------------------------------------------*/
#define MAX_TEXT_LENGTH 400
#define TEXT_ENUM(nAME, fORMATsTR) nAME,
#define TEXT_CONST(nAME, fORMATsTR) fORMATsTR,

/// Only include the enums
#define TEXT_DEF TEXT_ENUM

/// Including the Errors
#define TEXT_CLASS_IFWD_ERRORS
typedef enum
{
  #include "IFWD_text_def_file_list.h"
  IFWD_ERR_number_of_texts
} IFWD_ERR_enum;
#undef TEXT_CLASS_IFWD_ERRORS


/// Including the Info (same list)
#define TEXT_CLASS_IFWD_INFO
typedef enum
{
  #include "IFWD_text_def_file_list.h"
  IFWD_INFO_number_of_texts
} IFWD_INFO_Enum;
#undef TEXT_CLASS_IFWD_INFO

#undef TEXT_DEF

#endif
