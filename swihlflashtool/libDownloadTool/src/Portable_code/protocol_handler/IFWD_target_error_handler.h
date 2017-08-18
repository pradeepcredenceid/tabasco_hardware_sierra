/**
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
 *  $File name:  /msw_tools/FlashTool/libDownloadTool/src/Portable_code/protocol_handler/IFWD_target_error_handler.h $
 *  $CC-Version: .../oint_tools_flashtoole2_01/6 $
 *  $Date:       2014-03-13    15:40:54 UTC $
 *   Responsible: bentzen
 *   Comment:
 *     Added bounds check on array access (SMS00882793)
 */
#ifndef __TARGET_ERROR_HANDLE__
#define __TARGET_ERROR_HANDLE__


/***************************** Constants *********************************/
#define FILE_NAME_MAX_SIZE     256
//#define LINE_MAX_SIZE           10  /* 2^^32 bits */
#define FUNCTION_NAME_MAX_SIZE 128

/*************************** enumerations ********************************/
typedef enum file_id_s
{
  file_cfi_file = 0x00,
  file_flash_file,
  file_protocol_file,
  file_ebl_file,
  file_platform_file,
  file_undefined_file,
} file_id_t;

typedef enum cfi_func_enum_s
{
  cfi_get_flash_id = 0x0,
  cfi_calc_checsum,
  cfi_read,
  cfi_program_word,
  cfi_program_flash,
  cfi_program_buffered,
  cfi_check_ready_sefa,
  cfi_suspend_erase_sefa,
  cfi_program_word_sefa,
  cfi_program_flash_sefa,
  cfi_reset,
  cfi_erase_sector,
  cfi_suspend_erase,
  cfi_check_ready,
  cfi_resume_erase,
  cfi_unlock_sector,
  cfi_lock_sector,
  cfi_address_correction,
  cfi_getWordBufferSize,
  cfi_get_sector_info,
  cfi_SetFuncPointers,
  cfi_read_Esn_Intel,
  cfi_read_Esn_Amd,
  cfi_last_function,
  cfi_function_not_valid = 0xFFFF,
} cfi_func_enum_t;

typedef enum flash_func_enum_s
{
  flash_Init,
  flash_Init_RegionTable,
  flash_Get_OTP_ID,
  flash_get_cfi,
  flash_process_handler,
  flash_set_address,
  flash_advance_address,
  flash_clear_crc,
  flash_get_erase_status,
  flash_get_ID,
  flash_erase,
  flash_suspend_erase,
  flash_resume_erase,
  flash_program,
  flash_read,
  flash_config_cfi,
  flash_Invalid_function,
  flash_set_range,
  flash_clear_range,
  flash_is_first,
  flash_addr_in_range,
  flash_update_prg_addr,
  flash_get_error_info,
  flash_get_cfi_error_info,
  flash_erase_sector_verfiy,
  flash_last_function
} flash_func_enum_t;

typedef enum flash_state_s
{
  Flash_idle_state = 0,
  Flash_erase_state,
  Flash_program_state,
  Flash_error_state,
} flash_state_t;

typedef enum ebl_func_enum_s
{
  ebl_error_handling,
  ebl_main_Event_Process,
  ebl_timer_timeout_proc,
  ebl_ebl,
  ebl_main,
  ebl_last_function,
  ebl_function_not_valid = 0xFFFF
} ebl_func_enum_t;

typedef enum ebl_state_s
{
  ebl_state_running = 0,
  ebl_state_stop_ok,
  ebl_state_stop_bad_cs_cfg,
  ebl_state_stop_range_check_fail,
  ebl_state_flash_error,
  ebl_state_cfi_error,
  ebl_state_prot_conf_error,
  ebl_state_not_valid = 0xFFFF
} ebl_state_t;

/**************************** Structures *********************************/

typedef struct debug_file_info_s
{
  uint32_t     file_id;
  uint16_t     line;
  uint16_t     func;
} debug_file_info_t;

typedef struct cfi_DQ_history_s
{
  uint32_t last_DQ_status;
  uint32_t sec_last_DQ_status;
  uint32_t thr_last_DQ_status;
} cfi_DQ_history_t;

typedef struct cfi_addr_history_s
{
  uint32_t last_address;
  uint32_t sec_last_address;
  uint32_t thr_last_address;
} cfi_addr_history_t;

typedef struct cfi_debug_info_s
{
  cfi_DQ_history_t   DQ_history;
  cfi_addr_history_t addr_history;
  uint32_t                amd_addr_mask;
  uint16_t                last_data;
} cfi_debug_info_t;

typedef struct flash_addr_history_s
{
  uint32_t root_address;
  uint32_t start_address;
  uint32_t address;
  uint32_t corr_address;
  uint32_t erase_address;
  uint32_t erase_next_address;
  uint32_t erase_end_address;
  uint32_t erase_sec_address;
} flash_addr_history_t;

typedef struct flash_debug_info_s
{
  flash_addr_history_t addr_history;
  flash_state_t        flash_state;
} flash_debug_info_t;

typedef struct ebl_debug_info_s
{
  ebl_state_t       ebl_state;
} ebl_debug_info_t;


typedef struct target_error_s
{
   file_id_t          file_id;
   char           file_name[FILE_NAME_MAX_SIZE];
   char           function_name[FUNCTION_NAME_MAX_SIZE];
   uint32_t                line;
   cfi_debug_info_t   cfi_debug_info;
   flash_debug_info_t flash_debug_info;
   ebl_debug_info_t   ebl_debug_info;
} target_error_t;

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
extern void target_error_decode_pack(uint8_t *buffer, target_error_t *pack_error);
extern void target_error_decode_pack_ebl2(uint8_t *buffer, uint8_t channel);

 #endif
