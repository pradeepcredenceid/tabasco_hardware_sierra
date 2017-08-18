/*
 *    Copyright (C) 2011-2014 Intel Mobile Communications GmbH
 *
 *         Sec Class: Intel Confidential (IC)
 *
 *
 * This document contains proprietary information belonging to IMC.
 * Technologies Denmark A/S. Passing on and copying of this document, use
 * and communication of its contents is not permitted without prior written
 * authorisation.
 *
 * Description:
 *   <hardcoded file description - the only part to be modified manually!>
 *
 * Revision Information:
 *  $File name:  /msw_tools/FlashTool/libDownloadTool/src/Portable_code/protocol_handler/IFWD_target_error_handler.c $
 *  $CC-Version: .../dhkristx_140314_v0/1 $
 *  $Date:       2014-04-23    10:42:03 UTC $
 *   Responsible: bentzen
 *   Comment:
 *     Added bounds check on array access (SMS00882793)
 */

/*****************************************************************************
 *                           Include files                                   *
 *****************************************************************************/


#include "OS.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <memory.h>
#include "IFWD_mem.h"
#include "IFWD_target_error_handler.h"
#include "IFWD_global_func.h"

// Includes from ../bootcore/inc
#include "ebl2_error_def.h"
#include "ebl2_std_errors.h"
#include "ebl2_sec_errors.h"

#include "ebl2_files.h"


/******************************************************************************
 *                      CONSTANT DEFINITIONS                                  *
 ******************************************************************************/
#define PROT_NOF_SEC_ERROR_TEXTS ((int)(sizeof(ebl2_sec_errors) / sizeof(char*) - 1))
#define PROT_NOF_STD_ERROR_TEXTS ((int)(sizeof(ebl2_std_errors) / sizeof(char*) - 1))
#define PROT_NOF_EBL2_FILES      ((int)(sizeof(ebl2_files) / sizeof(char*) - 1))

#define PROT_NOF_FILE_ID_NAME    ((int)(sizeof(file_id_name) / sizeof(char*)))
#define PROT_NOF_CFI_FUNC_NAME   ((int)(sizeof(cfi_func_name) / sizeof(char*)))
#define PROT_NOF_FLASH_FUNC_NAME ((int)(sizeof(flash_func_name) / sizeof(char*)))
#define PROT_NOF_EBL_FUNC_NAME   ((int)(sizeof(ebl_func_name) / sizeof(char*)))

#define MAX_TEXT_LENGTH_IN_ERROR 500
static const char * const index_error = "(Index out of range)";

 /*****************************************************************************
 *                         ENUMERATIONS                                      *
 *****************************************************************************/

 /******************************************************************************
 *                   STRUCTURE  TYPE DEFINITIONS                              *
 ******************************************************************************/

 /*****************************************************************************
 *                      LOCAL PROTOTYPES                                     *
 *****************************************************************************/
static char *get_sec_string(int index);
static char *get_std_string(int index);
static char *get_ebl2_files(int index);
static char *limit_strlen(char *string);

 /*****************************************************************************
 *                     PUBLIC PROTOTYPES                                     *
 *****************************************************************************/

/*****************************************************************************
 *                      LOCAL Variables                                      *
 *****************************************************************************/
static const char * const file_id_name[] =
{
  "cfi.c",
  "flash.c",
  "prot.c",
  "ebl.c",
  "platform_cnf.c",
  "undefined_file"
};

static const char * const cfi_func_name[] =
{
  "cfi_get_flash_id",
  "cfi_calc_checsum",
  "cfi_read",
  "cfi_program_word",
  "cfi_program_flash",
  "cfi_program_buffered",
  "cfi_check_ready_sefa",
  "cfi_suspend_erase_sefa",
  "cfi_program_word_sefa",
  "cfi_program_flash_sefa",
  "cfi_reset",
  "cfi_erase_sector",
  "cfi_suspend_erase",
  "cfi_check_ready",
  "cfi_resume_erase",
  "cfi_unlock_sector",
  "cfi_lock_sector",
  "cfi_address_correction",
  "cfi_getWordBufferSize",
  "cfi_get_sector_info",
  "cfi_SetFuncPointers",
  "cfi_read_Esn_Intel",
  "cfi_read_Esn_Amd"
 };

static const char * const flash_func_name[] =
{
  "flash_Init",
  "flash_Init_RegionTable",
  "flash_Get_OTP_ID",
  "flash_get_cfi",
  "flash_process_handler",
  "flash_set_address",
  "flash_advance_address",
  "flash_clear_crc",
  "flash_get_erase_status",
  "flash_get_ID",
  "flash_erase",
  "flash_suspend_erase",
  "flash_resume_erase",
  "flash_program",
  "flash_read",
  "flash_config_cfi",
  "flash_Invalid_function",
  "flash_set_range",
  "flash_clear_range",
  "flash_is_first",
  "flash_addr_in_range",
  "flash_update_prg_addr",
  "flash_get_error_info",
  "flash_get_cfi_error_inf"
 };

static const char * const ebl_func_name[] =
{
  "ebl_error_handling",
  "ebl_main_Event_Process",
  "ebl_timer_timeout_proc",
  "ebl_ebl",
  "ebl_main"
};

 /******************************************************************************
 *                         IMPLEMENTATION                                     *
 ******************************************************************************/

 /************************** Public Functions ********************************/

/*******************************************************************************
 * Function: target_error_decode_pack(char *buffer, char *out_buffer, ushort size);
 *------------------------------------------------------------------------------
 * Purpose:
 *
 * Parameters:
 * IN         None
 *
 * IN/OUT     None
 *
 * Returns:   Nothing
 ******************************************************************************/
void target_error_decode_pack_ebl2(uint8_t *buffer, uint8_t channel)
{
  t_error *p_ebl2_error;
  char err_line[MAX_STRING_LENTH];
  char err_text[MAX_STRING_LENTH];
  uint32_t i;

  p_ebl2_error = (t_error*)buffer;

  for (i = 0; i < MAX_ERRORS; i++)
  {
    switch (p_ebl2_error->error[i].Error_class)
    {
      case ERR_CLASS_SEC:
      {
        if (p_ebl2_error->error[i].Error_code)
        {
          snprintf(err_line, MAX_STRING_LENTH, "Target error[%i]: '%s'  Line: %d  File: %s.c",
                                                                           i,get_sec_string(p_ebl2_error->error[i].Error_code),
                                                                           p_ebl2_error->error[i].Line_number,
                                                                           get_ebl2_files(p_ebl2_error->error[i].File_id));

          GLOBAL_callback(channel,IFWD_DL_ProcessOutlineUpdated, err_line);
        }
        break;
      }
      case ERR_CLASS_STD:
      {
        if (p_ebl2_error->error[i].Error_code)
        {
          snprintf(err_line, MAX_STRING_LENTH, "Target error[%i]: '%s'  Line: %d  File: %s.c",
                                                                           i,get_std_string(p_ebl2_error->error[i].Error_code),
                                                                           p_ebl2_error->error[i].Line_number,
                                                                           get_ebl2_files(p_ebl2_error->error[i].File_id));

          GLOBAL_callback(channel,IFWD_DL_ProcessOutlineUpdated, err_line);
        }
        break;
      }
      case ERR_CLASS_TXT:
      {
    	snprintf(err_text, MAX_STRING_LENTH, "%s",limit_strlen((char*)((char*)p_ebl2_error)+p_ebl2_error->error[i].Error_code));
    	if ((p_ebl2_error->error[i].File_id == 0x23) && (strstr(err_text,"Flash erased.")))
    	{
          snprintf(err_line, MAX_STRING_LENTH, "\n\nPLEASE NOTE: The OneNand chip is now erased.\nHowever the chip driver don't support programming after a full erase.\nNow please disable the erase option and start download again.\n\n");
          GLOBAL_callback(channel,IFWD_DL_ProcessOutlineUpdated, err_line);
    	  i = MAX_ERRORS;
    	  break;
    	}
    	else if (p_ebl2_error->error[i].Error_code)
        {
          snprintf(err_line, MAX_STRING_LENTH, "Target error[%i]: '%s'  Line: %d  File: %s.c",
                                                                           i,err_text,
                                                                           p_ebl2_error->error[i].Line_number,
                                                                           get_ebl2_files(p_ebl2_error->error[i].File_id));

          GLOBAL_callback(channel,IFWD_DL_ProcessOutlineUpdated, err_line);
        }
        break;
      }
    }
  }

}


/******************************************************************************
Local functions to return arrays *WITH* bounds check
******************************************************************************/

static char *get_sec_string(int i)
{
  if(i >= 0 && i < PROT_NOF_SEC_ERROR_TEXTS)
    return (char *)ebl2_sec_errors[i];
  return (char *)index_error;
}

static char *get_std_string(int i)
{
  if(i >= 0 && i < PROT_NOF_STD_ERROR_TEXTS)
    return (char *)ebl2_std_errors[i];
  return (char *)index_error;
}

static char *get_ebl2_files(int i)
{
  if(i >= 0 && i < PROT_NOF_EBL2_FILES)
    return (char *)ebl2_files[i];
  return (char *)index_error;
}


/******************************************************************************
Local functions to limit too long strings from over-running our buffer
******************************************************************************/
static char *limit_strlen(char *string)
{
  static char buf[MAX_TEXT_LENGTH_IN_ERROR + 10];
  if(string)
  {
    int len = 0;
    char *src = string, *p = buf;
    while(*src && len < MAX_TEXT_LENGTH_IN_ERROR)
    {
      *p++ = *src++;
      len++;
    }
    if(len < MAX_TEXT_LENGTH_IN_ERROR)
      return string;
    *p++ = '.';  // indicate string truncation
    *p++ = '.';
    *p++ = '.';
    *p = 0;
  }
  else
  {
    buf[0] = 0;
  }
  return (char *)buf;
}
/******************************************************************************/

