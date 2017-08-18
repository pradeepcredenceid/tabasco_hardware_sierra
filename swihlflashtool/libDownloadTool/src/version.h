/*  -------------------------------------------------------------------------
    Copyright (C) 2011-2014 Intel Mobile Communications GmbH

         Sec Class: Intel Confidential (IC)

    All rights reserved.
    -------------------------------------------------------------------------
    This document contains proprietary information belonging to IMC.
    Passing on and copying of this document, use and communication of its
    contents is not permitted without prior written authorization.
    -------------------------------------------------------------------------
    Revision Information:
       $File name:  /msw_tools/FlashTool/libDownloadTool/src/version.h $
       $CC-Version: .../dhkristx_140314_v0/1 $
       $Date:       2014-04-23    10:40:05 UTC $
    -------------------------------------------------------------------------
    Description:
       This document describes the version information passed to the Windows DLL
    ------------------------------------------------------------------------- */

#ifndef _LIBRARY_VERSION_H
#define _LIBRARY_VERSION_H

#define VERSION          4,140,0,0
#define VERSION_STR     "4.140"

#ifdef PROJECT_Q
  #define VERSION_TEXT     " - Q Edition"
#else
  #define VERSION_TEXT     ""
#endif

#define VERSION_STRING  VERSION_STR VERSION_TEXT
#define COMPANY         "Intel Mobile Communications GmbH"
#define NAME            "FlashTool"
#define FILENAME_STR    "DownloadTool.dll"
#define COPYRIGHT       "Copyright (C) 2011 - 2014 Intel Mobile Communications GmbH"

#endif
