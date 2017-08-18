/*   ----------------------------------------------------------------------
     Copyright (C) 2011-2013 Intel Mobile Communications GmbH

          Sec Class: Intel Confidential (IC)

     ----------------------------------------------------------------------
     Revision Information:
       $$File name:  /msw_tools/FlashTool/DownloadTool/src/targetver.h $
       $$CC-Version: .../oint_tools_flashtoole2_01/6 $
       $$Date:       2013-10-02    15:03:33 UTC $
     ----------------------------------------------------------------------
     by CCCT (v0.4)
     ---------------------------------------------------------------------- */


#pragma once

// The following macros define the minimum required platform.  The minimum required platform
// is the earliest version of Windows, Internet Explorer etc. that has the necessary features to run 
// your application.  The macros work by enabling all features available on platform versions up to and 
// including the version specified.

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef _WIN32_WINNT            // Specifies that the minimum required platform is Windows Vista (0x0600).
  #define _WIN32_WINNT 0x0600
  //#define _WIN32_WINNT 0x0501
#endif

