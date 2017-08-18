/*  -------------------------------------------------------------------------
    Copyright (C) 2014 Intel Mobile Communications GmbH

         Sec Class: Intel Confidential (IC)

    All rights reserved.
    -------------------------------------------------------------------------
    This document contains proprietary information belonging to IMC.
    Passing on and copying of this document, use and communication of its
    contents is not permitted without prior written authorization.
    -------------------------------------------------------------------------
    Revision Information:
       $File name:  /msw_tools/FlashTool/libFlsTool/inc/psi_text_def_file_list.h $
       $CC-Version: .../dhkristx_140314_v0/1 $
       $Date:       2014-04-29    11:20:47 UTC $
    ------------------------------------------------------------------------- 

    This file may contain ONLY includes of other files containing ONLY TEXTDEF type
    declarations. Do NOT add any additional statements in either this file or the files
    included (except for comments).

    If you wish to add error codes from a new module, please create
    the needed file inside the strings subdirectory; get inspiration from psi_strings.h.

    Error ENUM definitiones will be automatically available if you include
    IMC_bootcore_strings.h or psi_std_err.h.

    The ERRCODE_SECTION_BASE macro is used to define the base number for a series of
    error codes and serves the purpose of reserving space for additional
    definitions in the previous include. Please make sure to reserve sufficient
    error code space for the previous module when you add a new one (why not
    keep the initial idea of 0x1000 which is reserver for psi_strings.h).


    ------------------------------------------------------------------------- */

#include "strings/psi_strings.h"
ERRCODE_SECTION_BASE(MEMTEST_ERRCODE_BASE,0x1000)
#include "strings/memtest_strings.h"

