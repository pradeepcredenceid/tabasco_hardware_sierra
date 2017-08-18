/* -------------------------------------------------------------------------
 Copyright (C) 2013 Intel Mobile Communications GmbH

      Sec Class: Intel Confidential (IC)
 ----------------------------------------------------------------------
 Revision Information:
    $File name:  /msw_tools/FlashTool/libFlsTool/src/Exceptions.cpp $
    $CC-Version: .../oint_tools_flashtoole2_01/1 $
    $Date:       2013-10-02    15:00:42 UTC $
 ----------------------------------------------------------------------
 by CCCT (0.12e)
 ----------------------------------------------------------------------
 Description:
   Include to handle exceptions. This file is for supporting -fno-exceptions,
   which is the Android way.
 ---------------------------------------------------------------------- */

#include "Exceptions.h"

/* Android doesn't support exceptions */
#ifndef __EXCEPTIONS
  std::vector<myexception> chatched_exceptions;
#endif
