/* ----------------------------------------------------------------------------

   Copyright (C) 2014 Intel Mobile Communications GmbH

        Sec Class: Intel Confidential (IC)

   Copyright (C) 2008-2008 Infineon Technologies Denmark A/S.
   All rights reserved.
   ----------------------------------------------------------------------------

   This document contains proprietary information belonging to Infineon
   Technologies Denmark A/S. Passing on and copying of this document, use
   and communication of its contents is not permitted without prior written
   authorisation.

   Revision Information:
  $File name:  /msw_tools/FlashTool/libFlsTool/inc/ebl2_error_def.h $
  $CC-Version: .../dhkristx_140314_v0/1 $
  $Date:       2014-04-29    11:20:39 UTC $
  ---------------------------------------------------------------------------*/
#ifndef __EBL2_ERROR_DEF_H__
#define __EBL2_ERROR_DEF_H__

/*!
*  \defgroup ERROR ErrorHandler
*  \ingroup ERROR
*  \{
*  Definition of interface functions provided by Error handling module.
*/

#define MAX_ERRORS          8
#define MAX_ERROR_PACK_SIZE 2048

#define ERR_CLASS_SEC  0
#define ERR_CLASS_STD  1
#define ERR_CLASS_TXT  2

typedef struct
{
   uint16_t File_id;
   uint16_t Line_number;
   uint16_t Error_class;
   uint16_t Error_code;         // Global enum structure
} t_one_error;

typedef struct
{
   t_one_error error[MAX_ERRORS];
   uint8_t spare[MAX_ERROR_PACK_SIZE - (sizeof(t_one_error)*MAX_ERRORS)];
} t_error;


#endif
