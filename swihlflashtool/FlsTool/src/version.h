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
       $File name:  /msw_tools/FlashTool/FlsTool/src/version.h $
       $CC-Version: .../dhkristx_140314_v0/1 $
       $Date:       2014-04-23    11:06:03 UTC $
    ------------------------------------------------------------------------- */

#ifndef _FLS_TOOL_VERSION_H
#define _FLS_TOOL_VERSION_H

#define VERSION          1,17,0,0
#define VERSION_STR     "1.17"
#define VERSION_TEXT    ""

#define VERSION_STRING  VERSION_STR VERSION_TEXT
#define FILENAME_STR    "FlsTool.exe"
#define NAME            "FlsTool"
#define COMPANY         "Intel Mobile Communications GmbH"
#define COPYRIGHT       "Copyright (C) 2011 - 2013 Intel Mobile Communications GmbH"

#if defined(_WIN32) && !defined(_WIN32_WINNT)
  #pragma once
  #define _WIN32_WINNT 0x0501
#endif

#endif
