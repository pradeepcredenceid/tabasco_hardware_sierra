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
       $File name:  /msw_tools/FlashTool/libFlsTool/inc/IMC_bootcore_strings.h $
       $CC-Version: .../dhkristx_140314_v0/1 $
       $Date:       2014-04-29    11:20:36 UTC $
    -------------------------------------------------------------------------

    This file delivers error code enums and corresponding error texts. This
    alows host-side tools to interpret error codes sent from target code.

    IMC_bootcore_enum
    --> Contains definitions of error codes defined in files
    referenced in "psi_text_def_file_list.h"

    IMC_bootcore_string[]
    --> Contains all error codes, a string representing the code mnemonic as well
    as the actual error description string (see IMC_bootcore_string_t struct).
    This table is generated ONLY if USE_TEXT_CLASS is defined.

    get_psi_error_name() - translates an error code into the corresponding
    string representation of the ENUM name.
    Functions is generated ONLY if USE_TEXT_CLASS is defined.

    get_psi_error_description() - translates an error code into the corresponding
    error description string.
    Functions is generated ONLY if USE_TEXT_CLASS is defined.

    DO NOT add any additional definitions or types here; this file is meant
    to ONLY provide codes and corresponding texts.

    If you wish to add error codes from a new module, please see
    "psi_text_def_file_list.h".

    ------------------------------------------------------------------------- */

#ifndef BOOTCORE_ENUMS_DEFINED
  #undef TEXT_DEF
  #define TEXT_DEF(eNUM, sTR) eNUM,
  #define ERRCODE_SECTION_BASE(name, x) name=x,
  typedef enum
  {
    #include "psi_text_def_file_list.h"
    PSIERR_LAST_ERROR,
    BOOTCORE_enum_count
  } IMC_bootcore_enum;
  #define BOOTCORE_ENUMS_DEFINED
#endif


#ifdef USE_TEXT_CLASS
  #ifndef BOOTCORE_STRINGS_DEFINED
    typedef struct {
      IMC_bootcore_enum code;
      const char* enumStr;
      const char* formatStr;
    } IMC_bootcore_string_t;
    #undef TEXT_DEF
    #define TEXT_DEF(eNUM, sTR) {eNUM, # eNUM, sTR},
    #undef ERRCODE_SECTION_BASE
    #define ERRCODE_SECTION_BASE(name, x)
    IMC_bootcore_string_t IMC_bootcore_string[] =
    {
      #include "psi_text_def_file_list.h"
      {PSIERR_LAST_ERROR, "PSIERR_LAST_ERROR", "Last PSI error code"},
      {BOOTCORE_enum_count, NULL, NULL}
    };

    inline const char* get_psi_error_name(IMC_bootcore_enum code){
       int i=0;
       while(IMC_bootcore_string[i].enumStr!=NULL )
       {
         if(IMC_bootcore_string[i].code == code ) return IMC_bootcore_string[i].enumStr;
         i++;
       }
       return "Unknown error code";
    }

    inline const char* get_psi_error_description(IMC_bootcore_enum code){
       int i=0;
       while(IMC_bootcore_string[i].formatStr!=NULL )
       {
         if(IMC_bootcore_string[i].code == code ) return IMC_bootcore_string[i].formatStr;
         i++;
       }
       return "Unknown error code";
    }
    #define BOOTCORE_STRINGS_DEFINED
  #endif
#endif

#undef TEXT_DEF

