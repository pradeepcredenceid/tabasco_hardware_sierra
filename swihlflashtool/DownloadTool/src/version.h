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
       $File name:  /msw_tools/FlashTool/DownloadTool/src/version.h $
       $CC-Version: .../dhkristx_140314_v0/2 $
       $Date:       2014-04-10    14:57:21 UTC $
    -------------------------------------------------------------------------
    Description:
       This document describes the version information passed to the Windows DLL
    ------------------------------------------------------------------------- */

#ifndef _DOWNLOAD_TOOL_VERSION_H
#define _DOWNLOAD_TOOL_VERSION_H

#define VERSION          1,37,0,0
#define VERSION_STR      "1.37"

#if defined(PROJECT_Q) && defined(STATIC_DL_LINKING)
  #define VERSION_TEXT " - Q Edition"
#else
  #define VERSION_TEXT ""
#endif

#define VERSION_STRING  VERSION_STR VERSION_TEXT
#define FILENAME_STR    "DownloadTool.exe"
#define NAME            "Download Tool"
#define COMPANY         "Intel Mobile Communications GmbH"
#define COPYRIGHT       "Copyright (C) 2011 - 2014 Intel Mobile Communications GmbH"

// TODO: Remove targetver.h (if even needed)

#ifndef _WIN32_WINNT
  #pragma once
  #define _WIN32_WINNT 0x0501
#endif

#endif
