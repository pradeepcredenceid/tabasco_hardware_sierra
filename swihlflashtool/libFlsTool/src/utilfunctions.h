/* -------------------------------------------------------------------------
 Copyright (C) 2014 Intel Mobile Communications GmbH

      Sec Class: Intel Confidential (IC)
 ----------------------------------------------------------------------
 Revision Information:
    $File name:  /msw_tools/FlashTool/libFlsTool/src/utilfunctions.h $
    $CC-Version: .../dhkristx_140314_v0/1 $
    $Date:       2014-04-16    12:21:10 UTC $
 ----------------------------------------------------------------------
 by CCCT (0.12l)
 ---------------------------------------------------------------------- */

#ifndef __UTILFUNCTIONS_H__
#define __UTILFUNCTIONS_H__

#include "errorcodes.h"
#include "as_defs.h"

#define UNKNOWN_ERROR "UNKNOWN_ERROR"

void AS_GetErrorString(uint8_t *errorstring, uint8_t error_code);
void copy_bytes(uint8_t* dest, uint8_t *src, int len);
void printlong(char *name, uint32_t *var, int len);
void printbytes(char *name, uint8_t *var, int len);
void AS_printf(const char* format, ...);
as_ret_type compare_long(uint32_t *A, uint32_t *B, int len);

#endif

