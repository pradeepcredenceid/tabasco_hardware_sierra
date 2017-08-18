/*
*       Copyright (C) 2011-2014 Intel Mobile Communications GmbH
*
*            Sec Class: Intel Confidential (IC)
*
*
* This document contains proprietary information belonging to IMC.
* Design A/S. Passing on and copying of this document, use and communication
* of its contents is not permitted without prior written authorisation.
*
* Description: Interface for: functions for doing Tracing of data end events in the other modules.
*
*
* Revision Information:
*  $File name:  /msw_tools/FlashTool/libDownloadTool/src/OS_dependent_code/debug_os_if/DebugModule.h $
*  $CC-Version: .../dhkristx_140314_v0/1 $
*  $Date:       2014-04-23    11:01:42 UTC $
*   Resonsible:
*   Comment:
*
*/

#ifndef  DebugModuleH
#define  DebugModuleH

#include "OS.h"
#include "FileIO.h"
#include "StringExt.h"

#include "IFWD_mem.h"
#include "IFWD_timer.h"
#include "IFWD_global_func.h" //< GLOBAL_trace_verbosity + MODULE_enum

#include <stdarg.h>
#include <string.h> /* For memcpy() */

#ifdef ANDROID
  #define LOG_NDEBUG 0            //< Default verbose behaviour

  #ifndef LOG_TAG
    #define LOG_TAG "lib_dl"        //< Default Android logcat name
  #endif

  #include <utils/Log.h>
  #include <cutils/properties.h>
#endif

/**
 * Defines a debug function (could be expanded for the trace functionality)
 */

typedef enum {
  DEBUG_VERBOSITY_ERROR = 0,    //< PrintError()
  DEBUG_VERBOSITY_NORMAL,       //< Info()  (Default verbosity)
  DEBUG_VERBOSITY_DEBUG,        //< Debug()
  DEBUG_VERBOSITY_DEBUG_MORE,   //< Debug2()
  DEBUG_VERBOSITY_DEBUG_ALOT,   //< Debug3()
  DEBUG_VERBOSITY_STATE_TRACE   //< Not used, but would be interesting.
} DebugVerbosityLevel;

#ifndef THIS_MODULE
  #warning "Before including DebugModule.h you have to define THIS_MODULE"
  #define THIS_MODULE MODULE_NONE
#endif

#define Info(format, arg...) \
  PrintLog(THIS_MODULE, DEBUG_VERBOSITY_NORMAL, format, ## arg);

#define Debug(format, arg...) \
  PrintLog(THIS_MODULE, DEBUG_VERBOSITY_DEBUG, "%s:%d: " format,__FUNCTION__,__LINE__, ## arg)

#define Debug2(format, arg...) \
  PrintLog(THIS_MODULE, DEBUG_VERBOSITY_DEBUG_MORE, format, ## arg)

#define Debug3(format, arg...) \
  PrintLog(THIS_MODULE, DEBUG_VERBOSITY_DEBUG_ALOT, format, ## arg)

#define PrintError(format, arg...) \
  do { \
    debug_active = 1; \
    PrintLog(THIS_MODULE, DEBUG_VERBOSITY_ERROR, "%s:%d: " format,__FUNCTION__,__LINE__, ## arg); \
  } while (0)

//#undef THIS_MODULE

/*---------------------------------------------------------------------------
Debug functions needed by macros:
----------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

char *getChar(unsigned char c);
char* formatBuffer(const char* buffer, size_t nbytes);
const char* getHex(const uint8_t* buffer, size_t nbytes);

void DebugSetVerbosity(DebugVerbosityLevel verbosity);
void DebugSetModuleVerbosity(MODULE_enum module, DebugVerbosityLevel verbosity);

void PrintLog(MODULE_enum module, int level, const char* format, ...);

uint32_t findFormatSize(const char* format, va_list args);
extern HANDLE debug_mutex;
extern uint8_t debug_active;
extern FILE *trace_file;
extern uint8_t self_test(void);

#ifdef __cplusplus
}
#endif

/* Function/IO Trace macros: */
/*---------------------------------------------------------------------------
  #     #
  ##   ##
  # # # #   ###    ###  # ##   ###    ###
  #  #  #      #  #     ##    #   #  #
  #     #   ####  #     #     #   #   ##
  #     #  #   #  #     #     #   #     #
  #     #   ####   ###  #      ###   ###
  ---------------------------------------------------------------------------*/

#define TR_BEGIN(GRP,TYPE,CHANNEL)                     \
    if(GLOBAL_trace_verbosity)                         \
    {                                                  \
      int32_t ___DGB_len;                                  \
      uint8_t *___DGB_p = GLOBAL_trace_buffer + 4;          \
      WaitForMutex(debug_mutex);                       \
      ___DGB_len = (int32_t)GLOBAL_clock();                \
      TR_32(___DGB_len);                               \
      TR_8(GRP);                                       \
      ___DGB_len = (int32_t)(TYPE);                        \
      TR_16(___DGB_len);                               \
      TR_8(CHANNEL)

/*---------------------------------------------------------------------------*/
/* Add a 8 bit value: */
#define TR_8(VALUE)  *___DGB_p++ = (uint8_t)(VALUE);
/*---------------------------------------------------------------------------*/
/* Add a 16 bit value: */
#define TR_16(VALUE)  memcpy(___DGB_p,(uint16_t*)&(VALUE),2); ___DGB_p = ___DGB_p + 2;
/*---------------------------------------------------------------------------*/
/* Add a 16 bit value pointer: */
#define TR_16P(VALUE_PTR)  memcpy((void*)___DGB_p,(const void *)((uint16_t*)(VALUE_PTR)),2); ___DGB_p = ___DGB_p + 2;
/*---------------------------------------------------------------------------*/
/* Add a 32 bit value: */
#define TR_32(VALUE)  memcpy(___DGB_p,(uint32_t*)&(VALUE),4); ___DGB_p = ___DGB_p + 4;
/*---------------------------------------------------------------------------*/
/* Add a 32 bit value pointer: */
#define TR_32P(VALUE_PTR)  memcpy((void*)___DGB_p,(const void*)(VALUE_PTR),4); ___DGB_p = ___DGB_p + 4;
/*---------------------------------------------------------------------------*/
/* Add Null-terminated ASCII string: */
#define TR_STR(DATA) ___DGB_p = (uint8_t *)stpcpy0((char *)___DGB_p, (char *)(DATA));
/*---------------------------------------------------------------------------*/
/* Add fixed length data block: */
#define TR_DATA(DATA,DATA_LEN) memcpy(___DGB_p,DATA,DATA_LEN); ___DGB_p = ___DGB_p + (uint32_t)(DATA_LEN);
/*---------------------------------------------------------------------------*/
/* Adding run-time variable length data block :*/
#define TR_DATA_DYNAMIC(DATA,DATA_LEN)  { uint32_t ___DGB_tmp_len = DATA_LEN;          \
                                          memcpy(___DGB_p,(void*)&___DGB_tmp_len,sizeof(___DGB_tmp_len));       \
                                          memcpy(___DGB_p + sizeof(___DGB_tmp_len),(const void *)((unsigned int)(DATA)),___DGB_tmp_len); \
                                          ___DGB_p = ___DGB_p + ___DGB_tmp_len + sizeof(___DGB_tmp_len); }
/*---------------------------------------------------------------------------*/
/* End debug macro (writes data): */
#define TR_END                                                        \
      ___DGB_len = (___DGB_p - GLOBAL_trace_buffer);                  \
      *((int32_t *)GLOBAL_trace_buffer) = ___DGB_len - 4;                 \
      (void)FILE_write(trace_file, GLOBAL_trace_buffer, ___DGB_len);  \
      (void)FILE_flush(trace_file);                                   \
      ReleaseMutex(debug_mutex);                                      \
   }



/*-------------------------------------------------------------------------------------*/

enum
{
  // Functions to trace
  TR_IFWD_DL_get_dll_version,
  TR_IFWD_DL_get_sw_version_data,
  TR_IFWD_DL_force_target_reset,
  TR_IFWD_DL_end_of_process,
  TR_IFWD_DL_set_dll_parameter,
  TR_IFWD_DL_set_dll_parameter1,
  TR_IFWD_DL_set_dll_parameter2,
  TR_IFWD_DL_set_dll_parameter3,
  TR_IFWD_DL_dll_get_parameter,
  TR_IFWD_DL_dll_get_parameter1,
  TR_IFWD_DL_init_callback,
  TR_IFWD_DL_open_comm_port,
  TR_IFWD_DL_close_comm_port,
  TR_IFWD_DL_boot_target,
  TR_IFWD_DL_boot_bin_target,
  TR_IFWD_DL_set_get_control_signal,
  TR_IFWD_DL_download_fls_file,
  TR_IFWD_DL_download_eep_file_file,
  TR_IFWD_DL_download_dffs_file,
  TR_IFWD_DL_download_cust_file,
  TR_IFWD_DL_upload_eep_file,
  TR_IFWD_DL_update_static_eeprom,
  TR_IFWD_DL_upload_bin_image,
  TR_IFWD_DL_download_bin_image,
  TR_IFWD_DL_erase_image,
  TR_IFWD_DL_verify_flash_image,
  TR_IFWD_DL_abort_process,
  TR_IFWD_DL_start_AT_mode,
  TR_IFWD_DL_stop_AT_mode,
  TR_IFWD_DL_AT_send,
  TR_IFWD_DL_TOC_get_nof_items,
  TR_IFWD_DL_TOC_get_memory_class,
  TR_IFWD_DL_TOC_get_filename,
  TR_IFWD_DL_TOC_get_checksum,
  TR_IFWD_DL_TOC_get_filesize,
  TR_IFWD_DL_set_external_file_handling,
  TR_IFWD_DL_dll_get_nof_progressbars,
  TR_IFWD_DL_NVM_get_nof_user_options,
  TR_IFWD_DL_NVM_get_user_option,
  TR_IFWD_DL_NVM_set_user_option,
  TR_IFWD_DL_out_of_session_comm,
  TR_IFWD_DL_TOC_get_flash_technology,
  TR_IFWD_DL_TOC_get_hw_platform,
  TR_IFWD_DL_NVM_set_flashless_option,
  TR_IFWD_DL_TOC_get_sw_option,
  TR_IFWD_DL_get_sys_version,
  TR_IFWD_DL_setup_prolific_gpio_reset,
  TR_IFWD_DL_get_chip_response,
  TR_IFWD_DL_pre_boot_target,
  TR_IFWD_DL_force_boot_components,
  TR_IFWD_DL_secure_channel_setup,


  TR_IFWD_DL_trace_insert     //< This is not interpreted by TraceViewer!
};
/*-------------------------------------------------------------------------------------*/

enum
{
  TR_FunctionCall,
  TR_Tx,
  TR_Rx,
  TR_Sfh,
  TR_Callback,
  TR_FunctionCall_Result,
  TR_ExtInfo
};
/*-------------------------------------------------------------------------------------*/
enum
{
  TR_ExtInfo_RawBinary,
  TR_ExtInfo_Text,
  TR_ExtDebug_Text
};

enum
{
  TR_boot_tx_load_length = 0x9000,
  TR_boot_tx_load_length2,
  TR_boot_tx_at,
  TR_boot_tx_ebl_loader_length,
  TR_boot_tx_ebl_payload_data,
  TR_boot_tx_ebl_checksum,
  TR_boot_tx_ebl_loader_checksum,
  TR_boot_rx_ack_byte,
  TR_boot_rx_ebl_loader_checksum_result,
  TR_boot_rx_ebl_version,
  TR_boot_rx_ebl_checksum_result,
  TR_boot_rx_payload_ind,
  TR_boot_rx_ebl_ready,
  TR_boot_tx_baud,
  TR_boot_tx_ebl_loader_payload_data,
  TR_boot_rx_ack_byte0,
  TR_boot_rx_ebl_loader_tlv1,
  TR_boot_rx_ebl_loader_tlv2,
  TR_boot_rpsi_tx_cmd,
  TR_boot_rpsi_rx_rsp,
  TR_boot_rx_psi_error,
  TR_boot_dut_usb_certification_test,
  TR_boot_ping_cmd,
  TR_boot_ping_rsp,
  TR_boot_rom_baud_cmd,
  TR_boot_rom_skip_psi_cmd,
  TR_boot_rom_baud_rsp,
  TR_boot_rom_skip_psi_rsp,
  TR_boot_rx_rpsi_ack
};
/*-------------------------------------------------------------------------------------*/
enum
{
  TR_sfh_state_change,
  TR_sfh_flags_change,
  TR_sfh_dummy
};
/*-------------------------------------------------------------------------------------*/
#endif //DebugModuleH

/*---------------------------------------------------------------------------*/
