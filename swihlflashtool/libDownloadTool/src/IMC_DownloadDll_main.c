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
      $File name:  /msw_tools/FlashTool/libDownloadTool/src/IMC_DownloadDll_main.c $
      $CC-Version: .../dhkristx_140314_v0/3 $
      $Date:       2014-05-02    13:10:25 UTC $
    -------------------------------------------------------------------------
    Description:
       Main file for the IFWD download DLL
     ------------------------------------------------------------------------- */

#define THIS_MODULE MODULE_MAIN

#include "version.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#ifndef _WIN32
  #include <pthread.h>
  #include <unistd.h>
#else
  #include <windows.h>
  #include <process.h>
#endif

#include "IMC_DownloadDll.h"

#include "DebugModule.h"

#include "FileIO.h"
#include "IFWD_comm.h"
#include "IFWD_boot.h"
#include "IFWD_comport.h"
#include "IFWD_global_func.h"
#include "IFWD_multi_file_handler.h"
#include "IFWD_single_file_handler.h"
#include "IFWD_transport_handler.h"
#include "IFWD_protocol_handler.h"

#include "FlashLess.h"

//< The USB Mapper, libUSB & WinUSB
#ifndef DISABLE_LIBUSB
  #include "GenericUSB.h"
#endif

//#define TRACE_MAGIC 0x72547849 // trace without timestamp
#define TRACE_MAGIC 0x72545849   // trace with timestamp
//#define TRACE_MAGIC 0x72545850   // trace with timestamp, and info string first
#define DLL_TRACE_BUFFER_SIZE 512000 /* must be at least the larget size we send to target */


#ifdef __BORLANDC__
// We don't want a million warnings just because Borland thinks that thread code has no effect
#pragma warn -8019
#endif

#define WAIT_FOR_THREAD   { \
    WaitForMutex(loop_mutex); \
    SetEvent(idle_event); \
    GLOBAL_SFH_in_idle = false; \
    GLOBAL_MFH_in_idle = false; \
  }

#define FX_DONE           { ReleaseMutex(loop_mutex); }


/* ---------------------------------------------------------------------------
  Global variables
--------------------------------------------------------------------------- */
#ifdef _WIN32
  HANDLE ThreadHandle;
  DWORD ThreadId;
#else
  int ThreadHandle;
  pthread_t ThreadId;
#endif

extern IFWD_ExtFileHandleType *IFWD_GLOBAL_ExtFileHandle;
volatile unsigned int ThreadTerminate = 0;
//volatile unsigned int ThreadSemaphore = 0;
char ErrorText[MAX_TEXT_LENGTH];

char ini_file_name[400];
SFH_done_signal_enum done_mode = SFH_done_signal_semaphore;
static int open_port_count = 0;
char dll_file_path[MAX_PATH];
char dll_file[MAX_PATH];

/* ---------------------------------------------------------------------------
  Extern variables
--------------------------------------------------------------------------- */
extern HANDLE done_event;
extern HANDLE idle_event;

/* ---------------------------------------------------------------------------
  Local variables
--------------------------------------------------------------------------- */
static HANDLE loop_mutex;
static uint8_t lower_multichannel_CPU_load = false;
static uint8_t CallClose = false;
static uint8_t WasEntryPointCalled = false;
static char version_str[100] = "?.?,?.?"; // must be able to hold the version numbers AND InternalName field in project version settings

/* ---------------------------------------------------------------------------
  Local prototypes
--------------------------------------------------------------------------- */
static void WriteTraceHeader(FILE *out);


/* ---------------------------------------------------------------------------
  *nix Library Version Function...

  // TODO: Create a "config.h" with version specified.
  //       This is a standardised way of creating versions on software
--------------------------------------------------------------------------- */
#ifndef _WIN32
  static char *InternalGetVersion(void)
  {
    if(version_str[0] == (char)'?')
      snprintf(version_str, 100,"%s", VERSION_STRING);
    return version_str;
  }
#else
/* ---------------------------------------------------------------------------
  Windows Library Version Function...

  // This function is used internally to get the DLL version
  // (it is also used when getting the version externally, but that includes function trace).
--------------------------------------------------------------------------- */
  static char *InternalGetVersion(void)
  {
    if(version_str[0] == (char)'?')     /* version not set yet ? */
    {
      uint16_t Major = 0, Minor = 0, Release = 0, Build = 0;
      if(GLOBAL_GetFileVersion(dll_file, &Major, &Minor, &Release, &Build))
      {
        char SpecialName[100];
        sprintf_version_info(SpecialName, dll_file, "InternalName");
        if(SpecialName[0]) // format the string to upper case and replace . and ' ' to _
        {
          char *p = SpecialName;
          while(*p)
          {
            *p = toupper(*p);
            if(*p == (char)'.' || *p == (char)' ')
              *p = '_';
            p++;
          }
          sprintf(version_str,"%i.%i_%s,%i.%i",Major, Minor, SpecialName, Release, Build);
        }
        else
          sprintf(version_str,"%i.%i,%i.%i",Major, Minor, Release, Build);
        #ifdef VERSION_TEXT
          snprintf(version_str, 100,"%s %s", version_str, VERSION_TEXT);
        #endif
      }
    }
    return version_str;
  }
#endif


/* ---------------------------------------------------------------------------
  Cross-platform Export functions.
--------------------------------------------------------------------------- */
#ifdef USE_UNDERSCORE
  DECLSPEC char* _IFWD_DL_get_dll_version(void)
#else
  DECLSPEC char* IFWD_DL_get_dll_version(void)
#endif
{
  TR_BEGIN(TR_FunctionCall, TR_IFWD_DL_get_dll_version,0);
  TR_END;

  if(!WasEntryPointCalled)
  {
    ShowMessage("The DLL Entrypoint was not called before calling get version.\nThere is something wrong with your PC compiler setup.\n\nTry the following:\n\nEnsure that the first 8 characters in path are different from other views,\nor copy the whole project to a different folder away from other CCViews.");
    WasEntryPointCalled = true; /* don't show this warning again */
  }
  InternalGetVersion();

  TR_BEGIN(TR_FunctionCall_Result, TR_IFWD_DL_get_dll_version,0);
  TR_STR(version_str);
  TR_END;

  return version_str;
}

/*-------------------------------------------------------------------------------------*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_get_sw_version_data(uint8_t channel,char *fls_file_name,uint8_t *file_data,uint8_t *target_data, uint16_t data_size,char *status)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_get_sw_version_data(uint8_t channel,char *fls_file_name,uint8_t *file_data,uint8_t *target_data, uint16_t data_size,char *status)
#endif
{
  IFWD_DL_status_enum result = IFWD_DL_OK;
  SFH_status_enum sfh_result;
#if 0
  /* there is not enough information in this trace to make a module test simulation if this function is used */
  TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_get_sw_version_data,channel);
  TR_STR(fls_file_name);
  TR_16(data_size);
  TR_DATA(file_data,data_size);
  TR_16(data_size);
  TR_DATA(target_data,data_size);
  TR_END;
#endif

  if(status) // Remove any old status texts from previous function calls, e.g. "invalid channel"
    *status = 0;

  WAIT_FOR_THREAD;
  sfh_result = SFH_get_sw_version_data(channel, fls_file_name, file_data, target_data, data_size, done_mode, status);
  FX_DONE;

  if (sfh_result == SFH_status_wait_for_event)
  {
    WaitForSingleObject(done_event,INFINITE);
    /* Check the state */
  }
  else if (sfh_result == SFH_status_Error)
  {
    result = IFWD_DL_Error;
  }

  TR_BEGIN(TR_FunctionCall_Result,TR_IFWD_DL_get_sw_version_data,channel);
  TR_8(result);
  TR_END;

  return result;
}

/*-------------------------------------------------------------------------------------*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_force_target_reset(uint8_t channel, uint8_t mode, uint8_t p1, uint8_t p2, uint8_t p3, char *status)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_force_target_reset(uint8_t channel, uint8_t mode, uint8_t p1, uint8_t p2, uint8_t p3, char *status)
#endif
{
  IFWD_DL_status_enum result = IFWD_DL_OK;
  SFH_status_enum sfh_result;

  TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_force_target_reset,channel);
  TR_8(mode);
  TR_8(p1);
  TR_8(p2);
  TR_8(p3);
  TR_END;

  if(status) // Remove any old status texts from previous function calls, e.g. "invalid channel"
    *status = 0;

  WAIT_FOR_THREAD;
  sfh_result = SFH_force_target_reset(channel, mode, p1, p2, p3, done_mode, status);
  FX_DONE;

  if (sfh_result == SFH_status_wait_for_event)
  {
    WaitForSingleObject(done_event,INFINITE);
  }
  else if (sfh_result == SFH_status_Error)
  {
    result = IFWD_DL_Error;
  }

  TR_BEGIN(TR_FunctionCall_Result,TR_IFWD_DL_force_target_reset,channel);
  TR_8(result);
  TR_STR(status);
  TR_END;

  return result;
}

/*-------------------------------------------------------------------------------------*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_end_of_process(uint8_t channel, char *status)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_end_of_process(uint8_t channel, char *status)
#endif
{
  SFH_status_enum sfh_result;
  IFWD_DL_status_enum result = IFWD_DL_OK;

  TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_end_of_process,channel);
  TR_END;

  if(status) // Remove any old status texts from previous function calls, e.g. "invalid channel"
    *status = 0;

  WAIT_FOR_THREAD;
  sfh_result = SFH_end_of_process(channel, done_mode, status);
  FX_DONE;

  if (sfh_result == SFH_status_wait_for_event)
  {
    WaitForSingleObject(done_event,INFINITE);
  }
  else  if (sfh_result == SFH_status_Error)
  {
    result = IFWD_DL_Error;
  }

  TR_BEGIN(TR_FunctionCall_Result,TR_IFWD_DL_end_of_process,channel);
  TR_8(result);
  TR_STR(status);
  TR_END;

  return result;
}

/*-------------------------------------------------------------------------------------*/


#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_set_dll_parameter(/*IFWD_DL_dll_parameter_enum*/ uint8_t parameter_code, uintptr_t parameter_value, char* status)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_set_dll_parameter(/*IFWD_DL_dll_parameter_enum*/ uint8_t parameter_code, uintptr_t parameter_value, char* status)
#endif
{
  IFWD_DL_status_enum result = IFWD_DL_OK;
  uint32_t value = (uint32_t)parameter_value;
  uint32_t trace_id;

  switch(parameter_code)
  {
    case IFWD_DL_dll_parameter_set_trace_filename:
      trace_id = (uint32_t)TR_IFWD_DL_set_dll_parameter1;
      TR_BEGIN(TR_FunctionCall,trace_id,0);
      TR_8(parameter_code);
      TR_STR((char *)parameter_value);
      TR_END;
      if(status)
        *status = 0;
    break;
    case IFWD_DL_dll_parameter_set_nand_bitfield:
      trace_id = (uint32_t)TR_IFWD_DL_set_dll_parameter2;
      TR_BEGIN(TR_FunctionCall,trace_id,0);
      TR_8(parameter_code);
      TR_DATA((uint8_t*)parameter_value,sizeof(GLOBAL_nand_partition_config));
      TR_END;
      if(status)
        *status = 0;
    break;
    case IFWD_DL_dll_parameter_prolific_gp0:
      trace_id = (uint32_t)TR_IFWD_DL_set_dll_parameter3;
      TR_BEGIN(TR_FunctionCall,trace_id,0);
      TR_8(parameter_code);
      TR_32(status);
      TR_8(value);
      TR_END;
      /* Since "status" is used as input - *DO NOT* treat it as a output string and clear it or write to it!!! */
    break;

    case IFWD_DL_dll_parameter_set_winusb_guid:
      trace_id = (uint32_t)TR_IFWD_DL_set_dll_parameter1;
      TR_BEGIN(TR_FunctionCall,trace_id,0);
      TR_8(parameter_code);
      TR_STR((char *)parameter_value);
      TR_END;
      if(status)
        *status = 0;
      break;

    default:
      trace_id = (uint32_t)TR_IFWD_DL_set_dll_parameter;
      TR_BEGIN(TR_FunctionCall,trace_id,0);
      TR_8(parameter_code);
      TR_32(value);
      TR_END;
      if(status)
        *status = 0;
    break;
  }

  // okay now we can start the real work:
  WAIT_FOR_THREAD;
  switch(parameter_code)
  {
    case IFWD_DL_dll_parameter_prolific_gp0:
      if(!COMPORT_set_gp0((uintptr_t)status,value))
        result = IFWD_DL_Error;
    break;
    case IFWD_DL_dll_parameter_do_not_allow_EEP_mode_write:
      GLOBAL_ban_eep_mode_writes = (value != 0);
    break;

    case IFWD_DL_dll_parameter_stay_in_function:
      done_mode = (value == 0) ? SFH_done_signal_callback : SFH_done_signal_semaphore;
    break;

    case IFWD_DL_dll_parameter_boot_process_timeout:
      GLOBAL_boot_timeout = value;

      // TODO: Remove this timeout
      if (GLOBAL_comm_process_timeout < value/1000)
        GLOBAL_comm_process_timeout = value/1000;
    break;

    case IFWD_DL_dll_parameter_comm_timeout:
    GLOBAL_comm_timeout = value;

    // TODO: Remove this timeout
    if (GLOBAL_comm_process_timeout < value/1000)
      GLOBAL_comm_process_timeout = value/1000;
    break;

    case IFWD_DL_dll_parameter_force_full_emulation:
      GLOBAL_force_full_emulation = (value == 0) ? false : true;
    break;

    case IFWD_DL_dll_parameter_set_special_boot_sequence:
      if(value <= IFWD_DL_boot_seq_last_valid)
        GLOBAL_special_boot_seq = value;
      else
        result = IFWD_DL_Error;
    break;

    case IFWD_DL_dll_parameter_use_pre_erase:
      GLOBAL_use_pre_erase = (value == 0) ? false : true;
    break;

    case IFWD_DL_dll_parameter_cond_dyn_eep_erase:
      GLOBAL_cond_dyn_eep_erase = (value == 0) ? false : true;
    break;

    case IFWD_DL_dll_parameter_return_unformatted_text:
      GLOBAL_use_plain_text = (value == 0) ? true : false;
    break;

    case IFWD_DL_dll_parameter_skip_empty_blocks:
      dll_capabilities.compression = (value == 0) ? 0 : CAP_SKIP_CONST_FF_PACKS;
    break;

    case IFWD_DL_dll_parameter_faster_crc_method:
      dll_capabilities.crc_method = (value == 0) ? 0 : CAP_CRC_RETURNED_BY_SEC_END;
    break;
    case IFWD_DL_dll_parameter_lower_multichannel_CPU_load:
      lower_multichannel_CPU_load = (value == 0) ? false : true;
    break;

    case IFWD_DL_dll_parameter_erase_mode:
/* SWISTART */
#ifndef SIERRA
      if(value <= IFWD_DL_target_erase_last_valid)
        GLOBAL_erase_mode = value;
      else
        result = IFWD_DL_Error;
#endif/* SIERRA */
/* SWISTOP */
    break;
/*
  case IFWD_DL_dll_parameter_check_file_size:
    GLOBAL_file_size_check = (value == 0) ? false : true;
  break;

   case IFWD_DL_dll_parameter_erase_sector_verify:
    GLOBAL_erase_sector_verify = (value == 0) ? false : true;
    dll_capabilities.erase_sector_verify = (value == 0) ? 0 : ERASE_SECTOR_VERIFY;
  break;

  case IFWD_DL_dll_parameter_flash_debug:
    GLOBAL_flash_debug = (value == 0) ? false : true;
    dll_capabilities.flash_debug = (value == 0) ? 0 : FLASH_DEBUG_ON;
  break;
*/
    case IFWD_DL_dll_parameter_force_area_erase:
      GLOBAL_force_area_erase = (value == 0) ? false : true;
    break;
    case IFWD_DL_dll_parameter_allow_hw_channel_switch:
      GLOBAL_use_hw_ch_change = (value == 0) ? false : true;
    break;
    case IFWD_DL_dll_parameter_use_alt_boot_speed:
      GLOBAL_use_alt_boot_speed = (value == 0) ? false : true;
    break;
    case IFWD_DL_dll_parameter_use_sec_pack_erase_flag:
      GLOBAL_use_sec_pack_erase_flag = (value == 0) ? false : true;
    break;
    case IFWD_DL_dll_parameter_upload_mbn_data:
      GLOBAL_upload_mbn_data = (value == 0) ? false : true;
    break;
    case IFWD_DL_dll_parameter_set_trace_filename:
    {
      char *filename = (char *)parameter_value;
      if(filename && filename[1]) // trace file specified?
      {
        if(!debug_active)  /* make sure we do not start trace if the mutex has not been created */
        {
          if(status)
            strncpy(status, "DLL attach not called!", 24);
          result = IFWD_DL_Error;
        }
        else
        {
          if(trace_file) /* any old trace file open already? */
          {
            FILE_close(trace_file); /* then close if before we open the new one */
          }
          trace_file = FILE_open(filename, FILE_open_mode_write_create);
          if(trace_file) /* opening file okay ? */
          {
            int slen = strlen(filename);
            if(GLOBAL_trace_filename)
              (void)MEM_free((uint8_t *)GLOBAL_trace_filename);
            GLOBAL_trace_filename = (char *)MEM_malloc(slen+1);
            if(GLOBAL_trace_filename)
            {
              strncpy(GLOBAL_trace_filename, filename, slen+1); /* remember the file name */
              WriteTraceHeader(trace_file);
            }
            else
              result = IFWD_DL_Error;
          }
          else
          {
            result = IFWD_DL_Error;
          }
        }
      }
      else
        result = IFWD_DL_Error;
    }
    break;

    case IFWD_DL_dll_parameter_set_trace:
      if(!debug_active) /* make sure we do not start trace if the mutex has not been created */
      {
        if(status)
          strncpy(status, "DLL attach not called!", 24);
        result = IFWD_DL_Error;
      }
      else
      {
        if(value != 0) /* trace on? */
        {
          if(!GLOBAL_trace_filename) /* no trace file specified? */
          {
            GLOBAL_trace_filename = (char *)MEM_malloc(sizeof(char)*24);
            if(GLOBAL_trace_filename)
              strcpy(GLOBAL_trace_filename, "IFWD_DownloadDll.log");
          }
          if(!trace_file) /* not open already? */
          {
            trace_file = FILE_open(GLOBAL_trace_filename, FILE_open_mode_write_create);
            if(trace_file)
            {
              WriteTraceHeader(trace_file);
            }
            else
              result = IFWD_DL_Error;
          }
        }
        if(result != IFWD_DL_Error) /* no errors detected? */
        {
          GLOBAL_trace_verbosity = value;
        }
        if(!GLOBAL_trace_verbosity) /* have we turned trace off? */
        {
          if(trace_file)
          {
            FILE_close(trace_file);
            trace_file = 0;
          }
          if(GLOBAL_trace_buffer)
          {
            (void)MEM_free(GLOBAL_trace_buffer);
            GLOBAL_trace_buffer = 0;
          }
          /* don't free the trace filename now (in case trace is started again) */
        }
      }
    break;
    case IFWD_DL_dll_parameter_set_nand_bitfield:
      memcpy((void*)&GLOBAL_nand_partition_config, ((void*)parameter_value), sizeof(GLOBAL_nand_partition_config));
      break;

    case IFWD_DL_dll_parameter_force_plain_upload:
      GLOBAL_force_plain_upload = (value == 0) ? false : true;
    break;
    case IFWD_DL_dll_parameter_set_usb_auto_mode:
      GLOBAL_usb_auto_mode = (value == 0) ? false : true;
    break;
    case IFWD_DL_dll_parameter_skip_wr_prak_crc:
      GLOBAL_skip_wr_pack_crc = (value == 0) ? false : true;
    break;
    case IFWD_DL_dll_parameter_check_sign_hw_cfg_value:
      GLOBAL_check_sign_hw_cfg_value = (value == 0) ? false : true;
    break;
    case IFWD_DL_dll_parameter_emulation_tx:
    {
      int channel = SFH_get_used_channel();
      if(channel != 0) // channel that is not in idle?
      {
        if(status)
          xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_not_in_idle,channel) );
        result = IFWD_DL_Error;
      }
      else
      {
        GLOBAL_EmulateTx = (EmulateTx_t)parameter_value;
      }
    }
    break;
    case IFWD_DL_dll_parameter_emulation_rx:
    {
      int channel = SFH_get_used_channel();
      if(channel != 0) // channel that is not in idle?
      {
        if(status)
          xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_not_in_idle,channel) );
        result = IFWD_DL_Error;
      }
      else
      {
        GLOBAL_EmulateRx = (EmulateRx_t)parameter_value;
      }
    }
    break;
    case IFWD_DL_dll_parameter_emulation_rx_avail:
    {
      int channel = SFH_get_used_channel();
      if(channel != 0) // channel that is not in idle?
      {
        if(status)
          xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_not_in_idle,channel) );
        result = IFWD_DL_Error;
      }
      else
      {
        GLOBAL_EmulateRxAvail = (EmulateRxAvail_t)parameter_value;
      }
    }
    break;
    case IFWD_DL_dll_parameter_com_port_emulation:
    {
      int channel = SFH_get_used_channel();
      if(channel != 0) // channel that is not in idle?
      {
        if(status)
          xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_not_in_idle,channel) );
        result = IFWD_DL_Error;
      }
      else
      {
        if(value == 0) // turn emulation off?
        {
          GLOBAL_UseCOMPortEmulation = false;
        }
        else // turn emulation on:
        {
          if(GLOBAL_EmulateTx && GLOBAL_EmulateRx && GLOBAL_EmulateRxAvail) // all callbacks set?
            GLOBAL_UseCOMPortEmulation = true;
          else
          {
            result = IFWD_DL_Error;
            if(status)
              xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_emulation_cb_not_set) );
          }
        }
      }
    }
    break;

  case IFWD_DL_dll_parameter_USB_certification_test:
    GLOBAL_dut_test_request = (value == 0) ? GLOBAL_dut_test_unknown : GLOBAL_dut_test_usb_certification;
    break;

  case IFWD_DL_dll_parameter_USB_IN_token_test:
    GLOBAL_dut_test_request = (value == 0) ? GLOBAL_dut_test_unknown : GLOBAL_dut_test_usb_in_token;
    break;

  case IFWD_DL_dll_parameter_USB_OUT_token_test:
    GLOBAL_dut_test_request = (value == 0) ? GLOBAL_dut_test_unknown : GLOBAL_dut_test_usb_out_token;
    break;

  case IFWD_DL_dll_parameter_USB_loopback_token_test:
    GLOBAL_dut_test_request = (value == 0) ? GLOBAL_dut_test_unknown : GLOBAL_dut_test_usb_loopback;
    break;

  case IFWD_DL_dll_parameter_set_precheck_mode:
    GLOBAL_precheck_mode = (value == 0) ? false : true;
    break;

  case IFWD_DL_dll_parameter_skip_dl_if_sw_verified:
    GLOBAL_skip_sw_load_if_sys_val_ok = (value == 0) ? false : true;
    break;
#ifdef WIN32
  case IFWD_DL_dll_parameter_set_winusb_guid:
  {
    const char *guid_str = (const char *)parameter_value;
    if (!GenericUSB_set_GUID(guid_str))
    {
      if (status)
        sprintf(status, "String is not GUID: '%s'", guid_str);
      result = IFWD_DL_Error;
    }
  } break;
#endif
  default:
    result = IFWD_DL_Error;
    break;
  }
  FX_DONE;

  if (GLOBAL_trace_buffer)
  {
      TR_BEGIN(TR_FunctionCall_Result,trace_id,0);
      TR_8(result);
      TR_END;
  }
  return result;
}
/*-------------------------------------------------------------------------------------*/

#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_dll_get_parameter(/*IFWD_DL_dll_get_parameter_enum*/ uint8_t parameter_code, uint32_t *result_output, char *input_parameter)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_dll_get_parameter(/*IFWD_DL_dll_get_parameter_enum*/ uint8_t parameter_code, uint32_t *result_output, char *input_parameter)
#endif
{
  IFWD_DL_status_enum result = IFWD_DL_OK;
  uint32_t trace_id;

  if(result_output)    // make sure that our output is reset in case the output is not set (so our test do not fail due to random values)
    *result_output = 0;

  /* Lets determine input arguments format, so we can trace inputs correctly: */
  switch(parameter_code)
  {
    case IFWD_DL_dll_get_parameter_filechecksum:
    case IFWD_DL_dll_get_parameter_zip_get_size:
    case IFWD_DL_dll_get_parameter_zip_get_data:
      trace_id = (uint32_t)TR_IFWD_DL_dll_get_parameter;
      TR_BEGIN(TR_FunctionCall,trace_id,0);
      TR_8(parameter_code);
      TR_STR(input_parameter);
      TR_END;
    break;
    default:
      trace_id = (uint32_t)TR_IFWD_DL_dll_get_parameter1;
      TR_BEGIN(TR_FunctionCall,trace_id,0);
      TR_8(parameter_code);
      if(input_parameter)
      {
        TR_8(*input_parameter);
      }
      else
      {
        unsigned char temp = 0;
        TR_8(temp);
      }
      TR_END;
  }

  // Let the work begin
  WAIT_FOR_THREAD;

  switch(parameter_code)
  {
    case IFWD_DL_dll_get_parameter_filechecksum:
      if(result_output && input_parameter)
      {
        char *filename;
        int32_t TOC_index;
        filename = GLOBAL_TOC_filename(input_parameter, &TOC_index);
        if(TOC_index < 0)
          TOC_index = 0;
        if(MFH_TOC_get_checksum(filename, TOC_index, result_output) == MFH_status_Error)
          result = IFWD_DL_Error;
      }
      else
        result = IFWD_DL_Error;
    break;


    case IFWD_DL_dll_get_parameter_flash_id:
    {
      if(input_parameter == 0 || result_output == 0 || SFH_get_flash_id((uint8_t)*input_parameter, (uint32_t*)result_output) != SFH_status_OK)
        result = IFWD_DL_Error;
      break;
    }
    case IFWD_DL_dll_get_parameter_flash_voltage:
    {
      if(input_parameter == 0 || result_output == 0 || SFH_get_flash_voltage((uint8_t)*input_parameter, (uint32_t*)result_output) != SFH_status_OK)
        result = IFWD_DL_Error;
      break;
    }

    case IFWD_DL_dll_get_parameter_ebl_info_boot_mode:
    case IFWD_DL_dll_get_parameter_ebl_info_major_ver:
    case IFWD_DL_dll_get_parameter_ebl_info_minor_ver:
    case IFWD_DL_dll_get_parameter_ebl_info_name:
    case IFWD_DL_dll_get_parameter_ebl_info_cust_cfg:
    case IFWD_DL_dll_get_parameter_ebl_info_expected_mem_class:
    {
      if(input_parameter == 0 || result_output == 0 || SFH_get_ebl_info(parameter_code, (uint8_t)*input_parameter, (uint32_t*)result_output) != SFH_status_OK)
        result = IFWD_DL_Error;
      break;
    }
    case IFWD_DL_dll_get_parameter_zip_get_size:
    {
      if(result_output == 0 || MFH_ZIP_get_size((uint32_t*)result_output, input_parameter) != MFH_status_OK)
        result = IFWD_DL_Error;
      break;
    }
    case IFWD_DL_dll_get_parameter_zip_get_data:
    {
      if(result_output == 0 || MFH_ZIP_get_data((uint32_t*)result_output, input_parameter) != MFH_status_OK)
        result = IFWD_DL_Error;
      break;
    }

    default:
      result = IFWD_DL_Error;
      break;
  }

  FX_DONE;

  TR_BEGIN(TR_FunctionCall_Result,trace_id,0);
  TR_8(result);
  if(result_output)
  {
    TR_32(*result_output);
  }
  else
  {
    int temp = 0;
    TR_32(temp);
  }
  TR_END;

  return result;
}
/*-------------------------------------------------------------------------------------*/
#ifdef USE_UNDERSCORE
  uint32_t DECLSPEC _IFWD_DL_dll_get_nof_progressbars(uint8_t channel, char *filename, uint32_t *weights_output)
#else
  uint32_t DECLSPEC IFWD_DL_dll_get_nof_progressbars(uint8_t channel, char *filename, uint32_t *weights_output)
#endif
{
  uint32_t result;

  TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_dll_get_nof_progressbars,channel);
  TR_STR(filename);
  TR_8(weights_output != 0);
  TR_END;

  WAIT_FOR_THREAD;
  result = SFH_get_nof_progressbars(channel, filename, weights_output);
  FX_DONE;

  TR_BEGIN(TR_FunctionCall_Result,TR_IFWD_DL_dll_get_nof_progressbars,channel);
  TR_32(result);
  TR_END;

  return result;
}
/*-------------------------------------------------------------------------------------*/


#ifdef USE_UNDERSCORE
  uint32_t DECLSPEC _IFWD_DL_get_chip_response(uint8_t channel, uint8_t *response, uint32_t alloc_size)
#else
  uint32_t DECLSPEC IFWD_DL_get_chip_response(uint8_t channel, uint8_t *response, uint32_t alloc_size)
#endif
{
  boot_status_t boot_result;
  IFWD_DL_status_enum result = IFWD_DL_OK;

  TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_get_chip_response,channel);
  TR_32(alloc_size);
  TR_END;

  WAIT_FOR_THREAD;
  boot_result = BOOT_get_chip_response(channel, response, alloc_size);
  FX_DONE;

  TR_BEGIN(TR_FunctionCall_Result,TR_IFWD_DL_get_chip_response,channel);
  TR_32(boot_result);
  TR_DATA(response, alloc_size);
  TR_END;

  if (boot_result != boot_status_ok)
  {
    result = IFWD_DL_Error;
  }

  return result;
}
/*-------------------------------------------------------------------------------------*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_init_callback(void (*HostCallBack)(uint8_t channel, IFWD_DL_status_enum status, char *status_text), char *status)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_init_callback(void (*HostCallBack)(uint8_t channel, IFWD_DL_status_enum status, char *status_text), char *status)
#endif
{
  IFWD_DL_status_enum result = IFWD_DL_OK;

  TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_init_callback,0);
  TR_8((HostCallBack != NULL));
  TR_END;

  callback = HostCallBack;
  if(status)
    *status = 0;

  TR_BEGIN(TR_FunctionCall_Result,TR_IFWD_DL_init_callback,0);
  TR_8(result);
  TR_END;

  return result;
}
/*
void DECLSPEC IFWD_DL_init_callback_with_data(callback_func cb, void* data)
{
  TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_init_callback,0);
  TR_8((cb != NULL));
  TR_END;

  callback = cb;
  user_data = data;
}*/

/*-------------------------------------------------------------------------------------*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_setup_prolific_gpio_reset(uint8_t channel, char *reset_port_name, uint32_t gpio_mask, char *status)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_setup_prolific_gpio_reset(uint8_t channel, char *reset_port_name, uint32_t gpio_mask, char *status)
#endif
{
  SFH_status_enum sfh_result;
  IFWD_DL_status_enum result = IFWD_DL_OK;

  TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_setup_prolific_gpio_reset,channel);
  TR_STR(reset_port_name);
  TR_32(gpio_mask);
  TR_END;

  if(status) // Remove any old status texts from previous function calls, e.g. "invalid channel"
    *status = 0;

  Debug("call SFH ch=%d, port='%s', mask=%d",channel, reset_port_name, gpio_mask);
  sfh_result = SFH_prolific_reset_setup(channel, reset_port_name, gpio_mask, status);



  if (sfh_result == SFH_status_Error)
  {
    result = IFWD_DL_Error;
  }

  TR_BEGIN(TR_FunctionCall_Result,TR_IFWD_DL_setup_prolific_gpio_reset,channel);
  TR_8(result);
  TR_STR(status);
  TR_END;

  return result;
}

/*-------------------------------------------------------------------------------------*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_open_comm_port(uint8_t channel, char *comm_port_name, char *ctrl_port_name, uint32_t baud_rate, char *status)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_open_comm_port(uint8_t channel, char *comm_port_name, char *ctrl_port_name, uint32_t baud_rate, char *status)
#endif
{
  MFH_status_enum mfh_result;
  IFWD_DL_status_enum result = IFWD_DL_OK;

  TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_open_comm_port,channel);
  TR_STR(comm_port_name);
  TR_STR(ctrl_port_name);
  TR_32(baud_rate);
  TR_END;

  if(status) // Remove any old status texts from previous function calls, e.g. "invalid channel"
    *status = 0;

  open_port_count++;

  WAIT_FOR_THREAD;
  if(comm_port_name && ctrl_port_name)
    mfh_result = MFH_open_comm_port(channel, comm_port_name, ctrl_port_name, baud_rate, status);
  else
    mfh_result = MFH_status_Error;
  FX_DONE;

  if (mfh_result == MFH_status_wait_for_event)
  {
    WaitForSingleObject(done_event,INFINITE);
  }
  else if (mfh_result == MFH_status_Error)
  {
    result = IFWD_DL_Error;
  }

  TR_BEGIN(TR_FunctionCall_Result,TR_IFWD_DL_open_comm_port,channel);
  TR_8(result);
  TR_STR(status);
  TR_END;

//  IFWD_set_module_verbose(MODULE_ALL, 5);
  return result;
}
/*-------------------------------------------------------------------------------------*/

#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_close_comm_port(uint8_t channel, char *status)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_close_comm_port(uint8_t channel, char *status)
#endif
{
  MFH_status_enum mfh_result;
  IFWD_DL_status_enum result = IFWD_DL_OK;

  TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_close_comm_port,channel);
  TR_END;

  if(status) // Remove any old status texts from previous function calls, e.g. "invalid channel"
    *status = 0;

  open_port_count--;
  if (open_port_count < 0)
  {
    open_port_count = 0;
  }
  WAIT_FOR_THREAD;
  mfh_result = MFH_close_comm_port(channel, status);
  FX_DONE;

  if (mfh_result == MFH_status_wait_for_event)
  {
    WaitForSingleObject(done_event,INFINITE);
    /* Check the state */
    if(SFH_get_status(channel, status) == SFH_status_OK)
      mfh_result = MFH_status_OK;
    else
      mfh_result = MFH_status_Error;
  }

  if(mfh_result == MFH_status_Error)
  {
    result = IFWD_DL_Error;
  }

  TR_BEGIN(TR_FunctionCall_Result,TR_IFWD_DL_close_comm_port,channel);
  TR_8(result);
  TR_STR(status);
  TR_END;

  return result;
}

  /*-------------------------------------------------------------------------------------*/
  #ifdef USE_UNDERSCORE
    IFWD_DL_status_enum DECLSPEC _IFWD_DL_force_boot_components(uint8_t channel, char *psi_file_name, char *ebl_file_name)
  #else
    IFWD_DL_status_enum DECLSPEC IFWD_DL_force_boot_components(uint8_t channel, char *psi_file_name, char *ebl_file_name)
  #endif
  {
    IFWD_DL_status_enum result = IFWD_DL_OK;

    TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_force_boot_components,channel);
    TR_STR(psi_file_name);
    TR_STR(ebl_file_name);
    TR_END;


    WAIT_FOR_THREAD;
    if (!SFH_set_forced_boot_components(channel, psi_file_name, ebl_file_name))
    {
      result = IFWD_DL_Error;
    }
    FX_DONE;


    TR_BEGIN(TR_FunctionCall_Result,TR_IFWD_DL_force_boot_components,channel);
    TR_8(result);
    TR_END;

    return result;
  }
/*-------------------------------------------------------------------------------------*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_boot_target(uint8_t channel, char *ref_file, IFWD_DL_modem_control_signals_type *modem_control_signals, char *status)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_boot_target(uint8_t channel, char *ref_file, IFWD_DL_modem_control_signals_type *modem_control_signals, char *status)
#endif
{
  IFWD_DL_status_enum result = IFWD_DL_OK;
  SFH_status_enum sfh_result;

  TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_boot_target,channel);
  TR_STR(ref_file);
  TR_DATA(modem_control_signals,sizeof(IFWD_DL_modem_control_signals_type));
  TR_END;

  if(status) // Remove any old status texts from previous function calls, e.g. "invalid channel"
    *status = 0;

  WAIT_FOR_THREAD;
  sfh_result = SFH_boot_target(channel, ref_file, modem_control_signals, done_mode, status);
  FX_DONE;

  if (sfh_result == SFH_status_wait_for_event)
  {
    WaitForSingleObject(done_event,INFINITE);
    /* Check the state */
    sfh_result = SFH_get_status(channel, status);
  }
  if(sfh_result == SFH_status_Error)
  {
    result = IFWD_DL_Error;
  }

  TR_BEGIN(TR_FunctionCall_Result,TR_IFWD_DL_boot_target,channel);
  TR_8(result);
  TR_STR(status);
  TR_END;

  return result;
}

#ifdef USE_UNDERSCORE
    IFWD_DL_status_enum DECLSPEC _IFWD_DL_pre_boot_target(uint8_t channel, uint32_t platform_code, char *status)
#else
    IFWD_DL_status_enum DECLSPEC IFWD_DL_pre_boot_target(uint8_t channel, uint32_t platform_code, char *status)
#endif
{
   IFWD_DL_status_enum result = IFWD_DL_OK;
   SFH_status_enum sfh_result;

   TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_pre_boot_target,channel);
   TR_32(platform_code);
   TR_END;

   if(status) // Remove any old status texts from previous function calls, e.g. "invalid channel"
     *status = 0;

   WAIT_FOR_THREAD;
   sfh_result = SFH_pre_boot_target(channel, platform_code, done_mode, status);
   FX_DONE;

   if (sfh_result == SFH_status_wait_for_event)
   {
     WaitForSingleObject(done_event,INFINITE);
     /* Check the state */
      sfh_result = SFH_get_status(channel, status);
   }
   if(sfh_result == SFH_status_Error)
   {
     result = IFWD_DL_Error;
   }

   TR_BEGIN(TR_FunctionCall_Result,TR_IFWD_DL_pre_boot_target,channel);
   TR_8(result);
   TR_STR(status);
   TR_END;

   return result;
}

/*-------------------------------------------------------------------------------------*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_boot_bin_target(uint8_t channel, uint8_t *ref_file_header, IFWD_DL_modem_control_signals_type *modem_control_signals, char *status)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_boot_bin_target(uint8_t channel, uint8_t *ref_file_header, IFWD_DL_modem_control_signals_type *modem_control_signals, char *status)
#endif
{
  IFWD_DL_status_enum result = IFWD_DL_OK;
  SFH_status_enum sfh_result;

  TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_boot_bin_target,channel);
  TR_STR(ref_file_header);
  TR_DATA(modem_control_signals,sizeof(IFWD_DL_modem_control_signals_type));
  TR_END;

  if(status) // Remove any old status texts from previous function calls, e.g. "invalid channel"
    *status = 0;

  WAIT_FOR_THREAD;
  sfh_result = SFH_boot_bin_target(channel, ref_file_header, modem_control_signals, done_mode, status);
  FX_DONE;

  if (sfh_result == SFH_status_wait_for_event)
  {
    WaitForSingleObject(done_event,INFINITE);
    /* Check the state */
    sfh_result = SFH_get_status(channel, status);
  }
  if (sfh_result == SFH_status_Error)
  {
    result = IFWD_DL_Error;
  }

  TR_BEGIN(TR_FunctionCall_Result,TR_IFWD_DL_boot_bin_target,channel);
  TR_8(result);
  TR_STR(status);
  TR_END;

  return result;
}
/*-------------------------------------------------------------------------------------*/

#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_set_get_control_signal(uint8_t channel, IFWD_DL_modem_control_signals_type *modem_control_signals, char *status)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_set_get_control_signal(uint8_t channel, IFWD_DL_modem_control_signals_type *modem_control_signals, char *status)
#endif
{
  IFWD_DL_status_enum result = IFWD_DL_OK;
  SFH_status_enum sfh_result;

  if(!modem_control_signals)
    return IFWD_DL_Error;

  if(status) // Remove any old status texts from previous function calls, e.g. "invalid channel"
    *status = 0;

  TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_set_get_control_signal,channel);
  TR_8(modem_control_signals->DTR);
  TR_8(modem_control_signals->RTS);
  TR_END;

  WAIT_FOR_THREAD;
  sfh_result = SFH_control_modem_signals(channel, modem_control_signals, status);
  FX_DONE;

  if (sfh_result == SFH_status_wait_for_event)
  {
    WaitForSingleObject(done_event,INFINITE);
    /* Check the state */
    sfh_result = SFH_get_status(channel, status);
  }
  if (sfh_result == SFH_status_Error)
  {
    result = IFWD_DL_Error;
  }

  TR_BEGIN(TR_FunctionCall_Result,TR_IFWD_DL_set_get_control_signal,channel);
  TR_8(result);
  TR_8(modem_control_signals->CTS);
  TR_8(modem_control_signals->DSR);
  TR_8(modem_control_signals->RING);
  TR_8(modem_control_signals->RLSD);
  TR_STR(status);
  TR_END;

  return result;
}

/*-------------------------------------------------------------------------------------*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_download_fls_file(uint8_t channel, char *fls_file_name, char *status)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_download_fls_file(uint8_t channel, char *fls_file_name, char *status)
#endif
{
  IFWD_DL_status_enum result = IFWD_DL_OK;
  SFH_status_enum sfh_result;

  TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_download_fls_file,channel);
  TR_STR(fls_file_name);
  TR_END;

  if(status) // Remove any old status texts from previous function calls, e.g. "invalid channel"
    *status = 0;

  WAIT_FOR_THREAD;
  sfh_result = SFH_download_fls_file (channel, fls_file_name, done_mode, status);
  FX_DONE;

  if (sfh_result == SFH_status_wait_for_event)
  {
    WaitForSingleObject(done_event,INFINITE);
    /* Check the state */
    sfh_result = SFH_get_status(channel, status);
  }
  if (sfh_result == SFH_status_Error)
  {
    result = IFWD_DL_Error;
  }
  TR_BEGIN(TR_FunctionCall_Result,TR_IFWD_DL_download_fls_file,channel);
  TR_8(result);
  TR_STR(status);
  TR_END;
  return result;
}
/*-------------------------------------------------------------------------------------*/

#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_download_eep_file(uint8_t channel, char *eep_file_name, /*IFWD_DL_target_boot_mode_enum*/ uint8_t boot_mode, char *status)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_download_eep_file(uint8_t channel, char *eep_file_name, /*IFWD_DL_target_boot_mode_enum*/ uint8_t boot_mode, char *status)
#endif
{
  IFWD_DL_status_enum result = IFWD_DL_OK;
  SFH_status_enum sfh_result;

  TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_download_eep_file_file,channel);
  TR_STR(eep_file_name);
  TR_8(boot_mode);
  TR_END;

  if(status) // Remove any old status texts from previous function calls, e.g. "invalid channel"
    *status = 0;

  WAIT_FOR_THREAD;
  sfh_result = SFH_download_eep_file (channel, eep_file_name, (SFH_target_boot_mode_enum)boot_mode, done_mode, status);
  FX_DONE;

  if (sfh_result == SFH_status_wait_for_event)
  {
    WaitForSingleObject(done_event,INFINITE);
    /* Check the state */
    sfh_result = SFH_get_status(channel, status);
  }
  if(sfh_result == SFH_status_Error)
  {
    result = IFWD_DL_Error;
  }
  TR_BEGIN(TR_FunctionCall_Result,TR_IFWD_DL_download_eep_file_file,channel);
  TR_8(result);
  TR_STR(status);
  TR_END;
  return result;
}

/*-------------------------------------------------------------------------------------*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_download_dffs_file(uint8_t channel, char *dffs_file_name, /*IFWD_DL_dffs_load_sel_enum*/ uint8_t load_selection, char *status)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_download_dffs_file(uint8_t channel, char *dffs_file_name, /*IFWD_DL_dffs_load_sel_enum*/ uint8_t load_selection, char *status)
#endif
{
  IFWD_DL_status_enum result = IFWD_DL_OK;
  SFH_status_enum sfh_result;

  TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_download_dffs_file,channel);
  TR_STR(dffs_file_name);
  TR_8(load_selection);
  TR_END;

  if(status) // Remove any old status texts from previous function calls, e.g. "invalid channel"
    *status = 0;

  WAIT_FOR_THREAD;
  sfh_result = SFH_download_dffs_file (channel, dffs_file_name, (SFH_dffs_load_sel_enum)load_selection, done_mode, status);
  FX_DONE;

  if (sfh_result == SFH_status_wait_for_event)
  {
    WaitForSingleObject(done_event,INFINITE);
    /* Check the state */
    sfh_result = SFH_get_status(channel, status);
  }
  if (sfh_result == SFH_status_Error)
  {
    result = IFWD_DL_Error;
  }
  TR_BEGIN(TR_FunctionCall_Result,TR_IFWD_DL_download_dffs_file,channel);
  TR_8(result);
  TR_STR(status);
  TR_END;
  return result;
}

/*-------------------------------------------------------------------------------------*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_download_cust_file(uint8_t channel, char *cust_file_name, char *status)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_download_cust_file(uint8_t channel, char *cust_file_name, char *status)
#endif
{
  IFWD_DL_status_enum result = IFWD_DL_OK;
  SFH_status_enum sfh_result;

  TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_download_cust_file,channel);
  TR_STR(cust_file_name);
  TR_END;

  if(status) // Remove any old status texts from previous function calls, e.g. "invalid channel"
    *status = 0;

  WAIT_FOR_THREAD;
  sfh_result = SFH_download_cust_file (channel, cust_file_name, done_mode, status);
  FX_DONE;

  if (sfh_result == SFH_status_wait_for_event)
  {
    WaitForSingleObject(done_event,INFINITE);
    /* Check the state */
    sfh_result = SFH_get_status(channel, status);
  }
  if (sfh_result == SFH_status_Error)
  {
    result = IFWD_DL_Error;
  }
  TR_BEGIN(TR_FunctionCall_Result,TR_IFWD_DL_download_cust_file,channel);
  TR_8(result);
  TR_STR(status);
  TR_END;
  return result;
}

/*-------------------------------------------------------------------------------------*/
/* Not implemented! */
/* SWISTART */
#ifndef SIERRA
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_upload_eep_file(uint8_t channel, char *template_file_name, char *uploaded_eep_file_name, char *status)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_upload_eep_file(uint8_t channel, char *template_file_name, char *uploaded_eep_file_name, char *status)
#endif
{
  IFWD_DL_status_enum result = IFWD_DL_Error;
  if(status) // Remove any old status texts from previous function calls, e.g. "invalid channel"
    *status = 0;
  TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_upload_eep_file,channel);
  TR_STR(template_file_name);
  TR_STR(uploaded_eep_file_name);
  TR_END;

  GLOBAL_callback(channel,IFWD_DL_Error, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, MFH_ERR_not_yet_implemented,"IFWD_DL_upload_eep_file"));

  TR_BEGIN(TR_FunctionCall_Result,TR_IFWD_DL_upload_eep_file,channel);
  TR_8(result);
  TR_STR(status);
  TR_END;

  return result;
}

/*-------------------------------------------------------------------------------------*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_update_static_eeprom(uint8_t channel, char *dep_file_name, char *project_prefix_string, char *ref_eep_file_name, char *status)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_update_static_eeprom(uint8_t channel, char *dep_file_name, char *project_prefix_string, char *ref_eep_file_name, char *status)
#endif
{
  IFWD_DL_status_enum result = IFWD_DL_OK;
  SFH_status_enum sfh_result;

  TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_update_static_eeprom,channel);
  TR_STR(dep_file_name);
  TR_STR(project_prefix_string);
  TR_STR(ref_eep_file_name);
  TR_END;

  if(status) // Remove any old status texts from previous function calls, e.g. "invalid channel"
    *status = 0;

  WAIT_FOR_THREAD;
  sfh_result = SFH_update_static_eeprom (channel, dep_file_name, project_prefix_string, ref_eep_file_name, done_mode, status);
  FX_DONE;

  if (sfh_result == SFH_status_wait_for_event)
  {
    WaitForSingleObject(done_event,INFINITE);
    /* Check the state */
    sfh_result = SFH_get_status(channel, status);
  }
  if (sfh_result == SFH_status_Error)
  {
    result = IFWD_DL_Error;
  }
  TR_BEGIN(TR_FunctionCall_Result,TR_IFWD_DL_update_static_eeprom,channel);
  TR_8(result);
  TR_STR(status);
  TR_END;
  return result;
}

/*-------------------------------------------------------------------------------------*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_upload_bin_image(uint8_t channel, uint32_t image_start_address, uint32_t image_length, char *bin_file_name, char *ref_file_name, char *status)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_upload_bin_image(uint8_t channel, uint32_t image_start_address, uint32_t image_length, char *bin_file_name, char *ref_file_name, char *status)
#endif
{
  IFWD_DL_status_enum result = IFWD_DL_OK;
  SFH_status_enum sfh_result;

  TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_upload_bin_image,channel);
  TR_32(image_start_address);
  TR_32(image_length);
  TR_STR(bin_file_name);
  TR_STR(ref_file_name);
  TR_END;

  if(status) // Remove any old status texts from previous function calls, e.g. "invalid channel"
    *status = 0;

  WAIT_FOR_THREAD;
  sfh_result = SFH_upload_bin_image(channel, image_start_address, image_length, bin_file_name, ref_file_name, done_mode, status);
  FX_DONE;

  if (sfh_result == SFH_status_wait_for_event)
  {
    WaitForSingleObject(done_event,INFINITE);
    /* Check the state */
    sfh_result = SFH_get_status(channel, status);
  }
  if (sfh_result == SFH_status_Error)
  {
    result = IFWD_DL_Error;
  }
  TR_BEGIN(TR_FunctionCall_Result,TR_IFWD_DL_upload_bin_image,channel);
  TR_8(result);
  TR_STR(status);
  TR_END;
  return result;
}
/*-------------------------------------------------------------------------------------*/
#else /* SIERRA */
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_upload_eep_file(uint8_t channel, char *template_file_name, char *uploaded_eep_file_name, char *status)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_upload_eep_file(uint8_t channel, char *template_file_name, char *uploaded_eep_file_name, char *status)
#endif
{
  return IFWD_DL_Error;
}

/*-------------------------------------------------------------------------------------*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_update_static_eeprom(uint8_t channel, char *dep_file_name, char *project_prefix_string, char *ref_eep_file_name, char *status)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_update_static_eeprom(uint8_t channel, char *dep_file_name, char *project_prefix_string, char *ref_eep_file_name, char *status)
#endif
{
  return IFWD_DL_Error;
}

/*-------------------------------------------------------------------------------------*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_upload_bin_image(uint8_t channel, uint32_t image_start_address, uint32_t image_length, char *bin_file_name, char *ref_file_name, char *status)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_upload_bin_image(uint8_t channel, uint32_t image_start_address, uint32_t image_length, char *bin_file_name, char *ref_file_name, char *status)
#endif
{
  return IFWD_DL_Error;
}
/*-------------------------------------------------------------------------------------*/
#endif/* SIERRA */
/* SWISTOP */

#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_download_bin_image(uint8_t channel, uint32_t image_start_address, uint32_t image_length, uint32_t image_offset, char *bin_file_name, char *ref_file_name, char *status)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_download_bin_image(uint8_t channel, uint32_t image_start_address, uint32_t image_length, uint32_t image_offset, char *bin_file_name, char *ref_file_name, char *status)
#endif
{
  IFWD_DL_status_enum result = IFWD_DL_OK;
  SFH_status_enum sfh_result;

  TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_download_bin_image,channel);
  TR_32(image_start_address);
  TR_32(image_length);
  TR_32(image_offset);
  TR_STR(bin_file_name);
  TR_STR(ref_file_name);
  TR_END;

  if(status) // Remove any old status texts from previous function calls, e.g. "invalid channel"
    *status = 0;

  WAIT_FOR_THREAD;
  sfh_result = SFH_download_bin_image(channel, image_start_address, image_length, image_offset,bin_file_name, ref_file_name, done_mode, status);
  FX_DONE;

  if (sfh_result == SFH_status_wait_for_event)
  {
    WaitForSingleObject(done_event,INFINITE);
    /* Check the state */
    sfh_result = SFH_get_status(channel, status);
  }
  if (sfh_result == SFH_status_Error)
  {
    result = IFWD_DL_Error;
  }
  TR_BEGIN(TR_FunctionCall_Result,TR_IFWD_DL_download_bin_image,channel);
  TR_8(result);
  TR_STR(status);
  TR_END;
  return result;
}
/*-------------------------------------------------------------------------------------*/

/* SWISTART */
#ifndef SIERRA_NOERASE
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_erase_image(uint8_t channel, uint32_t image_start_address, uint32_t image_length, char *ref_file_name, char *status)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_erase_image(uint8_t channel, uint32_t image_start_address, uint32_t image_length, char *ref_file_name, char *status)
#endif
{
  IFWD_DL_status_enum result = IFWD_DL_OK;
  SFH_status_enum sfh_result;

  TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_erase_image,channel);
  TR_32(image_start_address);
  TR_32(image_length);
  TR_STR(ref_file_name);
  TR_END;

  if(status) // Remove any old status texts from previous function calls, e.g. "invalid channel"
    *status = 0;

  WAIT_FOR_THREAD;
  sfh_result = SFH_erase_image(channel, image_start_address, image_length, ref_file_name, done_mode, status);
  FX_DONE;

  if (sfh_result == SFH_status_wait_for_event)
  {
    WaitForSingleObject(done_event,INFINITE);
    /* Check the state */
    sfh_result = SFH_get_status(channel, status);
  }
  if (sfh_result == SFH_status_Error)
  {
    result = IFWD_DL_Error;
  }
  TR_BEGIN(TR_FunctionCall_Result,TR_IFWD_DL_erase_image,channel);
  TR_8(result);
  TR_STR(status);
  TR_END;
  return result;
}

/*-------------------------------------------------------------------------------------*/
#endif /* SIERRA_NOERASE */
/* SWISTOP */

#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_verify_flash_image(uint8_t channel, uint32_t image_start_address, uint32_t image_length, char *ref_file_name, char *status)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_verify_flash_image(uint8_t channel, uint32_t image_start_address, uint32_t image_length, char *ref_file_name, char *status)
#endif
{
  IFWD_DL_status_enum result = IFWD_DL_Error;

  TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_verify_flash_image,channel);
  TR_32(image_start_address);
  TR_32(image_length);
  TR_STR(ref_file_name);
  TR_END;

  if(status) // Remove any old status texts from previous function calls, e.g. "invalid channel"
    *status = 0;

  GLOBAL_callback(channel,IFWD_DL_Error, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, MFH_ERR_not_yet_implemented,"IFWD_DL_verify_flash_image"));

  TR_BEGIN(TR_FunctionCall_Result,TR_IFWD_DL_verify_flash_image,channel);
  TR_8(result);
  TR_STR(status);
  TR_END;
  return result;
}

/*-------------------------------------------------------------------------------------*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_abort_process(uint8_t channel, char *status)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_abort_process(uint8_t channel, char *status)
#endif
{
  IFWD_DL_status_enum result = IFWD_DL_OK;
  SFH_status_enum sfh_result;

  TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_abort_process,channel);
  TR_END;

  if(status) // Remove any old status texts from previous function calls, e.g. "invalid channel"
    *status = 0;

  WAIT_FOR_THREAD;
  sfh_result = SFH_abort_process(channel, status);
  FX_DONE;

  if (sfh_result == SFH_status_wait_for_event)
  {
    WaitForSingleObject(done_event,INFINITE);
    /* Check the state */
    sfh_result = SFH_get_status(channel, status);
  }
  if (sfh_result == SFH_status_Error)
  {
    result = IFWD_DL_Error;
  }

  TR_BEGIN(TR_FunctionCall_Result,TR_IFWD_DL_abort_process,channel);
  TR_8(result);
  TR_STR(status);
  TR_END;
  return result;
}

/*-------------------------------------------------------------------------------------*/

#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_start_AT_mode(uint8_t channel, char *status)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_start_AT_mode(uint8_t channel, char *status)
#endif
{
  IFWD_DL_status_enum result = IFWD_DL_OK;
  SFH_status_enum sfh_result;

  TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_start_AT_mode,channel);
  TR_END;

  if(status) // Remove any old status texts from previous function calls, e.g. "invalid channel"
    *status = 0;

  WAIT_FOR_THREAD;
  sfh_result = SFH_start_AT_mode(channel, status);
  FX_DONE;

  if(sfh_result == SFH_status_wait_for_event)
  {
    WaitForSingleObject(done_event,INFINITE);
  }
  else if(sfh_result == SFH_status_Error)
  {
    result = IFWD_DL_Error;
  }
  TR_BEGIN(TR_FunctionCall_Result,TR_IFWD_DL_start_AT_mode,channel);
  TR_8(result);
  TR_STR(status);
  TR_END;
  return result;
}
/*-------------------------------------------------------------------------------------*/

#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_stop_AT_mode(uint8_t channel, char *status)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_stop_AT_mode(uint8_t channel, char *status)
#endif
{
  IFWD_DL_status_enum result = IFWD_DL_OK;
  SFH_status_enum sfh_result;

  TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_stop_AT_mode,channel);
  TR_END;

  if(status) // Remove any old status texts from previous function calls, e.g. "invalid channel"
    *status = 0;

  WAIT_FOR_THREAD;
  sfh_result = SFH_stop_AT_mode(channel, status);
  FX_DONE;

  if(sfh_result == SFH_status_wait_for_event)
  {
    WaitForSingleObject(done_event,INFINITE);
  }
  else if(sfh_result == SFH_status_Error)
  {
    result = IFWD_DL_Error;
  }
  TR_BEGIN(TR_FunctionCall_Result,TR_IFWD_DL_stop_AT_mode,channel);
  TR_8(result);
  TR_STR(status);
  TR_END;
  return result;
}
/*-------------------------------------------------------------------------------------*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_AT_send(uint8_t channel, char *data, uint16_t length, char *status)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_AT_send(uint8_t channel, char *data, uint16_t length, char *status)
#endif
{
  IFWD_DL_status_enum result = IFWD_DL_OK;
  SFH_status_enum sfh_result;

  TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_AT_send,channel);
  TR_16(length);
  TR_DATA(data,length);
  TR_END;

  if(status) // Remove any old status texts from previous function calls, e.g. "invalid channel"
    *status = 0;

  WAIT_FOR_THREAD;
  sfh_result = SFH_AT_send(channel, data, length, status);
  FX_DONE;

  if(sfh_result == SFH_status_wait_for_event)
  {
    WaitForSingleObject(done_event,INFINITE);
  }
  else if(sfh_result == SFH_status_Error)
  {
    result = IFWD_DL_Error;
  }
  TR_BEGIN(TR_FunctionCall_Result,TR_IFWD_DL_AT_send,channel);
  TR_8(result);
  TR_STR(status);
  TR_END;
  return result;
}
/*-------------------------------------------------------------------------------------*/

/* TOC handling functions (toc index starts at 0) */
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_TOC_get_nof_items(char *filename, uint32_t *nof_output)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_TOC_get_nof_items(char *filename, uint32_t *nof_output)
#endif
{
  IFWD_DL_status_enum result = IFWD_DL_OK;

  TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_TOC_get_nof_items,0);
  TR_STR(filename);
  TR_8(nof_output != 0);
  TR_END;

  if(nof_output) // set default output in case the function does not set it (to give consistent test results)
    *nof_output = 0;

  WAIT_FOR_THREAD;
  if(MFH_TOC_get_nof_items(filename, nof_output) == MFH_status_Error)
    result = IFWD_DL_Error;
  FX_DONE;

  TR_BEGIN(TR_FunctionCall_Result,TR_IFWD_DL_TOC_get_nof_items,0);
  TR_8(result);
  if(nof_output)
  {
    TR_32(*nof_output);
  }
  else
  {
    int temp = 0;
    TR_32(temp);
  }
  TR_END;
  return result;
}
/*-------------------------------------------------------------------------------------*/

#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_TOC_get_memory_class(char *filename, uint32_t toc_index, uint32_t *mem_class_output)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_TOC_get_memory_class(char *filename, uint32_t toc_index, uint32_t *mem_class_output)
#endif
{
  IFWD_DL_status_enum result = IFWD_DL_OK;

  TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_TOC_get_memory_class,0);
  TR_STR(filename);
  TR_32(toc_index);
  TR_8(mem_class_output != 0);
  TR_END;

  if(mem_class_output) // set default output in case the function does not set it (to give consistent test results)
    *mem_class_output = 0;

  WAIT_FOR_THREAD;
  if(MFH_TOC_get_memory_class(filename, toc_index, mem_class_output) == MFH_status_Error)
    result = IFWD_DL_Error;
  FX_DONE;

  TR_BEGIN(TR_FunctionCall_Result,TR_IFWD_DL_TOC_get_memory_class,0);
  TR_8(result);
  if(mem_class_output)
  {
    TR_32(*mem_class_output);
  }
  else
  {
    int temp = 0;
    TR_32(temp);
  }
  TR_END;
  return result;
}
/*-------------------------------------------------------------------------------------*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_TOC_get_sw_option(char *filename,   uint32_t toc_index,  uint32_t option_code,   uint32_t *option_result)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_TOC_get_sw_option(char *filename,   uint32_t toc_index,  uint32_t option_code,   uint32_t *option_result)
#endif
{
  IFWD_DL_status_enum result = IFWD_DL_OK;

  TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_TOC_get_sw_option,0);
  TR_STR(filename);
  TR_32(toc_index);
  TR_32(option_code);
  TR_8(option_result != 0);
  TR_END;

  if(option_result) // set default output in case the function does not set it (to give consistent test results)
    *option_result = 0;

  WAIT_FOR_THREAD;
  if(MFH_TOC_get_sw_option(filename, toc_index, option_code, option_result) == MFH_status_Error)
    result = IFWD_DL_Error;
  FX_DONE;

  TR_BEGIN(TR_FunctionCall_Result,TR_IFWD_DL_TOC_get_sw_option,0);
  TR_8(result);
  if(option_result)
  {
    TR_32(*option_result);
  }
  else
  {
    int temp = 0;
    TR_32(temp);
  }
  TR_END;
  return result;
}
/*-------------------------------------------------------------------------------------*/

#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_get_sys_version(const char *filename, uint32_t option_code, char *option_result, uint32_t size)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_get_sys_version(const char *filename, uint32_t option_code, char *option_result, uint32_t size)
#endif
{
  int i, copied;
  SystemVersionLabelType sys_version;
  memset((char*)&sys_version, 0, sizeof(sys_version));
  IFWD_DL_status_enum result = IFWD_DL_OK;

  TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_get_sys_version,0);
  TR_STR(filename);
  TR_32(option_code);
  TR_8(option_result != 0);
  TR_END;

  if (size <= 0) {
    PrintError("No size defined!\n");
    return IFWD_DL_Error;
  }

  WAIT_FOR_THREAD;
  if(MFH_get_sys_version(filename, &sys_version) == MFH_status_Error)
    result = IFWD_DL_Error;
  FX_DONE;

  // Find option_code and set option_result
  if (result == IFWD_DL_OK)
  {
    switch((IFWD_DL_sys_version_code_enum)option_code)
    {
      case IFWD_DL_sys_version_short:
        copied = snprintf0(option_result, size, "%s\n", sys_version.version);
        break;

      case IFWD_DL_sys_version_all:
        copied = snprintf0(option_result, size, "%s\n Customer:\n", sys_version.version);
        for(i=0;i<3;++i)
        {
          if (sys_version.customer[i])
            copied += snprintf0(option_result+copied, size,
                                "    %d: %s\n", i+1, sys_version.customer[i]);
        }

        copied += snprintf0(option_result+copied, size, "\n Subsystem:\n");
        for(i=0;i<10;++i)
        {
          if (sys_version.subsystem[i])
          {
            copied += snprintf0(option_result+copied, size,
                                "    %d: %s\n", i+1, sys_version.subsystem[i]);
          }
        }
        break;

      case IFWD_DL_sys_version_customer:
        copied = snprintf0(option_result, size, " Customer:\n");
        for(i=0;i<3;++i)
        {
          if (sys_version.customer[i])
          {
            copied += snprintf0(option_result+copied, size,
                                "    %d: %s\n", i+1, sys_version.customer[i]);
          }
        }
        break;

      case IFWD_DL_sys_version_subsystem:
        copied = snprintf0(option_result, size, " Subsystem:\n");
        for(i=0;i<10;++i)
        {
          if (sys_version.subsystem[i])
          {
            copied += snprintf0(option_result+copied, size,
                                "    %d: %s\n", i+1, sys_version.subsystem[i]);
          }
        }
        break;

      default:
        PrintError("Option %d not supported!\n", option_code);
        *option_result = 0;
        result = IFWD_DL_Error;
        break;
    }
  }

  // Remember to free
  if (sys_version.filename)
    free(sys_version.filename);

  if (sys_version.version)
    free(sys_version.version);

  for(i=0;i<3;++i)
    if (sys_version.customer[i])
      free(sys_version.customer[i]);

  for(i=0;i<10;++i)
    if (sys_version.subsystem[i])
      free(sys_version.subsystem[i]);

  TR_BEGIN(TR_FunctionCall_Result,TR_IFWD_DL_get_sys_version,0);
  TR_8(result);
  TR_STR(option_result);
  TR_END;

  return result;
}
/*-------------------------------------------------------------------------------------*/

#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_TOC_get_filename(char *filename, uint32_t toc_index, char *filename_output)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_TOC_get_filename(char *filename, uint32_t toc_index, char *filename_output)
#endif
{
  IFWD_DL_status_enum result = IFWD_DL_OK;

  TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_TOC_get_filename,0);
  TR_STR(filename);
  TR_32(toc_index);
  TR_END;

  if(filename_output) // set default output in case the function does not set it (to give consistent test results)
    *filename_output = 0;

  WAIT_FOR_THREAD;
  if(MFH_TOC_get_filename(filename, toc_index, filename_output) == MFH_status_Error)
    result = IFWD_DL_Error;
  FX_DONE;

  TR_BEGIN(TR_FunctionCall_Result,TR_IFWD_DL_TOC_get_filename,0);
  TR_8(result);
  TR_STR(filename_output);
  TR_END;

  return result;
}
/*-------------------------------------------------------------------------------------*/

#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_TOC_get_checksum(char *filename, uint32_t toc_index, uint32_t *checksum_output)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_TOC_get_checksum(char *filename, uint32_t toc_index, uint32_t *checksum_output)
#endif
{
  IFWD_DL_status_enum result = IFWD_DL_OK;

  TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_TOC_get_checksum,0);
  TR_STR(filename);
  TR_32(toc_index);
  TR_8(checksum_output != 0);
  TR_END;

  if(checksum_output) // set default output in case the function does not set it (to give consistent test results)
    *checksum_output = 0;

  WAIT_FOR_THREAD;
  if(MFH_TOC_get_checksum(filename, toc_index, checksum_output) == MFH_status_Error)
    result = IFWD_DL_Error;
  FX_DONE;

  TR_BEGIN(TR_FunctionCall_Result,TR_IFWD_DL_TOC_get_checksum,0);
  TR_8(result);
  if(checksum_output)
  {
    TR_32(*checksum_output);
  }
  else
  {
    int temp = 0;
    TR_32(temp);
  }
  TR_END;

  return result;
}
/*-------------------------------------------------------------------------------------*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_TOC_get_filesize(char *filename, uint32_t toc_index, uint32_t *filesize_output)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_TOC_get_filesize(char *filename, uint32_t toc_index, uint32_t *filesize_output)
#endif
{
  IFWD_DL_status_enum result = IFWD_DL_OK;

  TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_TOC_get_filesize,0);
  TR_STR(filename);
  TR_32(toc_index);
  TR_END;

  if(filesize_output)  // set default output in case the function does not set it (to give consistent test results)
    *filesize_output = 0;

  WAIT_FOR_THREAD;
  if(MFH_TOC_get_filesize(filename, toc_index, filesize_output) == MFH_status_Error)
    result = IFWD_DL_Error;
  FX_DONE;

  TR_BEGIN(TR_FunctionCall_Result,TR_IFWD_DL_TOC_get_filesize,0);
  TR_8(result);
  if(filesize_output)
  {
    TR_32(*filesize_output);
  }
  else
  {
    int temp = 0;
    TR_32(temp);
  }
  TR_END;

  return result;
}
/*-------------------------------------------------------------------------------------*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_TOC_get_flash_technogy(char *filename, uint32_t *flash_technology_output)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_TOC_get_flash_technogy(char *filename, uint32_t *flash_technology_output)
#endif
{
  IFWD_DL_status_enum result = IFWD_DL_OK;

  TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_TOC_get_flash_technology,0);
  TR_STR(filename);
  TR_END;

  if(flash_technology_output)  // set default output in case the function does not set it (to give consistent test results)
    *flash_technology_output = 0;

  WAIT_FOR_THREAD;
  if(MFH_TOC_get_flash_technogy(filename, flash_technology_output) == MFH_status_Error)
    result = IFWD_DL_Error;
  FX_DONE;

  TR_BEGIN(TR_FunctionCall_Result,TR_IFWD_DL_TOC_get_flash_technology,0);
  TR_8(result);
  if(flash_technology_output)
  {
    TR_32(*flash_technology_output);
  }
  else
  {
    int temp = 0;
    TR_32(temp);
  }
  TR_END;

  return result;
}
/*-------------------------------------------------------------------------------------*/

#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_TOC_get_hw_platform(char *filename, uint32_t *hw_platform_output)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_TOC_get_hw_platform(char *filename, uint32_t *hw_platform_output)
#endif
{
  IFWD_DL_status_enum result = IFWD_DL_OK;

  TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_TOC_get_hw_platform,0);
  TR_STR(filename);
  TR_END;

  if(hw_platform_output)  // set default output in case the function does not set it (to give consistent test results)
    *hw_platform_output = 0;

  WAIT_FOR_THREAD;
  if(MFH_TOC_get_hw_platform(filename, hw_platform_output) == MFH_status_Error)
    result = IFWD_DL_Error;
  FX_DONE;

  TR_BEGIN(TR_FunctionCall_Result,TR_IFWD_DL_TOC_get_hw_platform,0);
  TR_8(result);
  if(hw_platform_output)
  {
    TR_32(*hw_platform_output);
  }
  else
  {
    int temp = 0;
    TR_32(temp);
  }
  TR_END;

  return result;
}
/*-------------------------------------------------------------------------------------*/

/* SWISTART */
#ifndef SIERRA
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum  _IFWD_DL_NVM_set_flashless_option (uint8_t *nvm_path, uint32_t nvm_options)
#else
  IFWD_DL_status_enum  IFWD_DL_NVM_set_flashless_option (uint8_t *nvm_path, uint32_t nvm_options)
#endif
{

  TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_NVM_set_flashless_option,0);
  TR_STR(nvm_path);
  TR_32(nvm_options);
  TR_END;

  WAIT_FOR_THREAD;

  if (GLOBAL_NVM_flashless_path)
  {
    MEM_free(GLOBAL_NVM_flashless_path);
    GLOBAL_NVM_flashless_path = NULL;
  }

  GLOBAL_NVM_flashless_path = (uint8_t*)strdup((const char*)nvm_path);
  GLOBAL_NVM_flashless_options = nvm_options;

  FX_DONE;
  return IFWD_DL_OK;
}
#else /* SIERRA */
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum  _IFWD_DL_NVM_set_flashless_option (uint8_t *nvm_path, uint32_t nvm_options)
#else
  IFWD_DL_status_enum  IFWD_DL_NVM_set_flashless_option (uint8_t *nvm_path, uint32_t nvm_options)
#endif
{
  return IFWD_DL_Error;
}
#endif /* SIERRA */
/* SWISTOP */



#ifdef USE_UNDERSCORE
  void DECLSPEC _IFWD_DL_set_external_file_handling(IFWD_ExtFileHandleType *ExtFileHandlers)
#else
  void DECLSPEC IFWD_DL_set_external_file_handling(IFWD_ExtFileHandleType *ExtFileHandlers)
#endif
{
  if (ExtFileHandlers)
  {
    TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_set_external_file_handling,0);
    TR_8(ExtFileHandlers != NULL);
    TR_END;
    WAIT_FOR_THREAD;
    IFWD_GLOBAL_ExtFileHandle = ExtFileHandlers;
    FX_DONE;
  }
}
/*-------------------------------------------------------------------------------------*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_NVM_get_nof_user_options(char *filename, uint32_t *nof_options)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_NVM_get_nof_user_options(char *filename, uint32_t *nof_options)
#endif
{
  IFWD_DL_status_enum result = IFWD_DL_OK;

  TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_NVM_get_nof_user_options,0);
  TR_STR(filename);
  TR_END;
  if (nof_options)
    *nof_options = 0;

  WAIT_FOR_THREAD;
  if(MFH_NVM_get_nof_user_options(filename, nof_options) == MFH_status_Error)
    result = IFWD_DL_Error;
  FX_DONE;

  TR_BEGIN(TR_FunctionCall_Result,TR_IFWD_DL_NVM_get_nof_user_options,0);
  TR_8(result);
  if(nof_options)
  {
    TR_32(*nof_options);
  }
  else
  {
    int temp = 0;
    TR_32(temp);
  }
  TR_END;

  return result;
}
/*-------------------------------------------------------------------------------------*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_NVM_get_user_option(char *filename, uint32_t i, uint32_t *nvm_tag)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_NVM_get_user_option(char *filename, uint32_t i, uint32_t *nvm_tag)
#endif
{
  IFWD_DL_status_enum result = IFWD_DL_OK;

  TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_NVM_get_user_option,0);
  TR_STR(filename);
  TR_32(i);
  TR_END;

  if(nvm_tag)
    *nvm_tag = 0;

  WAIT_FOR_THREAD;
  if(MFH_NVM_get_user_option(filename, i, nvm_tag) == MFH_status_Error)
    result = IFWD_DL_Error;
  FX_DONE;

  TR_BEGIN(TR_FunctionCall_Result,TR_IFWD_DL_NVM_get_user_option,0);
  TR_8(result);
  if(nvm_tag)
  {
    TR_32(*nvm_tag);
  }
  else
  {
    int temp = 0;
    TR_32(temp);
  }
  TR_END;

  return result;
}
/*-------------------------------------------------------------------------------------*/
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_NVM_set_user_option(char *filename,uint32_t i, uint8_t value)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_NVM_set_user_option(char *filename,uint32_t i, uint8_t value)
#endif
{
  IFWD_DL_status_enum result = IFWD_DL_OK;

  TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_NVM_set_user_option,0);
  TR_STR(filename);
  TR_32(i);
  TR_8(value);
  TR_END;

  WAIT_FOR_THREAD;
  if(MFH_NVM_set_user_option(filename, i, value) == MFH_status_Error)
    result = IFWD_DL_Error;
  FX_DONE;

  TR_BEGIN(TR_FunctionCall_Result,TR_IFWD_DL_NVM_set_user_option,0);
  TR_8(result);
  TR_END;

  return result;
}
/*-------------------------------------------------------------------------------------*/
/* SWISTART */
#ifndef SIERRA_CTF
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_out_of_session_comm(uint8_t  channel,
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_out_of_session_comm(uint8_t  channel,
#endif
                                                uint32_t action,
                                                uint32_t type,
                                                uint32_t length,
                                                uint32_t spare,
                                                uint8_t  *data)
{
  IFWD_DL_status_enum result = IFWD_DL_OK;
  SFH_status_enum sfh_result;

  TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_out_of_session_comm,channel);
  TR_32(action);
  TR_32(type);
  TR_32(length);
  TR_32(spare);
  TR_DATA(data,length);
  TR_END;


  WAIT_FOR_THREAD;
  sfh_result = SFH_out_of_session_comm(channel, action, type, length, spare, data, done_mode);
  FX_DONE;

  if(sfh_result == SFH_status_Error)
    result = IFWD_DL_Error;

  TR_BEGIN(TR_FunctionCall_Result,TR_IFWD_DL_out_of_session_comm,channel);
  TR_8(result);
  TR_END;

  if (sfh_result == SFH_status_wait_for_event)
  {
    WaitForSingleObject(done_event,INFINITE);

    /* Check the state */
    if(SFH_get_status(channel, NULL) == SFH_status_OK)
      result = IFWD_DL_OK;
    else
      result = IFWD_DL_Error;
  }

  return result;
}


#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_secure_channel_setup(uint8_t  channel, uint32_t options, sec_ch_callback_type callback_fx, char **cert_files, char *status)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_secure_channel_setup(uint8_t  channel, uint32_t options, sec_ch_callback_type callback_fx, char **cert_files, char *status)
#endif
{
  IFWD_DL_status_enum result = IFWD_DL_OK;

  TR_BEGIN(TR_FunctionCall,TR_IFWD_DL_secure_channel_setup,channel);
  TR_32(options);
  TR_32(callback_fx);
  TR_STR(cert_files);
  TR_END;

  WAIT_FOR_THREAD;
  if (SFH_secure_channel_setup(channel, options, callback_fx, cert_files, status) == SFH_status_Error)
    result = IFWD_DL_Error;
  FX_DONE;

  TR_BEGIN(TR_FunctionCall_Result,TR_IFWD_DL_secure_channel_setup,channel);
  TR_8(result);
  TR_END;

  return result;
}
#else /* SIERRA_CTF */
#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_out_of_session_comm(uint8_t  channel,
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_out_of_session_comm(uint8_t  channel,
#endif
                                                uint32_t action,
                                                uint32_t type,
                                                uint32_t length,
                                                uint32_t spare,
                                                uint8_t  *data)
{
  return IFWD_DL_Error;
}

#ifdef USE_UNDERSCORE
  IFWD_DL_status_enum DECLSPEC _IFWD_DL_secure_channel_setup(uint8_t  channel, uint32_t options, sec_ch_callback_type callback_fx, char **cert_files, char *status)
#else
  IFWD_DL_status_enum DECLSPEC IFWD_DL_secure_channel_setup(uint8_t  channel, uint32_t options, sec_ch_callback_type callback_fx, char **cert_files, char *status)
#endif
{
  return IFWD_DL_Error;
}
#endif /* SIERRA_CTF */
/* SWISTOP */

/*-------------------------------------------------------------------------------------*/

#ifdef ANDROID
  #define SLEEP_DELAY 1
#else
  #define SLEEP_DELAY 5
#endif

#ifdef LOWLEVEL_FUNCTION_TRACE
  unsigned long GLOBAL_clock_iteration = 0;
#endif

#if !defined(LOWLEVEL_FUNCTION_TRACE) && defined(ALLOW_DEBUG)
  #define DEBUG_DELAY 5
#endif

#ifndef _WIN32
  void *ThreadFunc(void* dummy)
#else
  DWORD WINAPI ThreadFunc(LPVOID dummy)
#endif
{
  #if !defined(LOWLEVEL_FUNCTION_TRACE) && defined(ALLOW_DEBUG)
    uint32_t counter = DEBUG_DELAY;
  #endif

  uint8_t idle_loop;

  /* Set-up this DLL's capabilities. */
  dll_capabilities.protocol_version    = CAP_PROT_VER_ENH;
  dll_capabilities.crc_method          = CAP_CRC_RETURNED_BY_SEC_END;
  dll_capabilities.crc_type            = CAP_CRC_NOT_USED;
  dll_capabilities.compression         = CAP_SKIP_CONST_FF_PACKS;
  dll_capabilities.protocol_crc        = CAP_PROT_NO_CRC;

  dll_capabilities.compression         = CAP_NOT_ENHANCED;
  dll_capabilities.erase_sector_verify = CAP_NOT_ENHANCED;

  dll_capabilities.flash_debug         = CAP_NOT_ENHANCED;
  dll_capabilities.skip_erase          = CAP_NOT_ENHANCED; /* May be set via API interface */
  dll_capabilities.skip_wr_pack_crc    = CAP_NOT_ENHANCED; /* May be set via API interface */
  dll_capabilities.skip_wr_pack_cnf    = CAP_SKIP_WR_PACK_CNF;

  //dll_capabilities.package_size        = CAP_PACK_SIZE_65536; /* TODO: WHY is this different from Win32?! */
  dll_capabilities.package_size        = CAP_PACK_SIZE_131072;
  dll_capabilities.flags               = CAP_FLAG_USE_EXT_CAP;
  ext_dll_capabilities.oos_config = EXT_CAP_OOS_CFG1_GET_HW_ID |
                                    EXT_CAP_OOS_CFG1_GET_1ST_16 |
                                    EXT_CAP_OOS_CFG1_GET_FLASH_ID |
                                    EXT_CAP_OOS_CFG1_ERASE_ALL |
                                    EXT_CAP_OOS_CFG1_WRITE_XML;

  ext_dll_capabilities.usb_handling_flags = EXT_CAP_USB_RAW_WR_MODE;
  ext_dll_capabilities.mipi_handling_flags = EXT_CAP_USB_RAW_WR_MODE;

  ext_dll_capabilities.comm_formats_1 = EXT_CAP_COMM_FORMAT_USIF |
                                        EXT_CAP_COMM_FORMAT_SPI |
                                        EXT_CAP_COMM_FORMAT_MIPI |
                                        EXT_CAP_COMM_FORMAT_USB;

  ext_dll_capabilities.verify_options = EXT_CAP_VERIFY_OPT_NO_PRG;

  ext_dll_capabilities.sec_handling_flags = EXT_CAP_SEC_OPEN_READ;

  ext_dll_capabilities.spare_07 = CAP_NOT_ENHANCED;
  ext_dll_capabilities.spare_08 = CAP_NOT_ENHANCED;
  ext_dll_capabilities.spare_09 = CAP_NOT_ENHANCED;
  ext_dll_capabilities.spare_10 = CAP_NOT_ENHANCED;
  ext_dll_capabilities.spare_11 = CAP_NOT_ENHANCED;
  ext_dll_capabilities.spare_12 = CAP_NOT_ENHANCED;

  idle_loop = true;            /* Enter idle loop until a DLL function is called */
  SetEvent( done_event );      /* Tell main thread that we are now ready */
  ResetEvent ( done_event );
  do
  {
    if(idle_loop)
    {
      WaitForSingleObject(idle_event,INFINITE);  /* Sleep until SFH goes out of idle or a DLL function is called */
      idle_loop = false;
      GLOBAL_SFH_in_idle = false;
      GLOBAL_MFH_in_idle = false;
    }
    WaitForMutex(loop_mutex); /* Enter critical region */
    /* Clock the handlers here */
    COMM_clock();
    BOOT_clock();
    PROT_clock();
    TRA_clock();
    SFH_clock();
    MFH_clock();
    /* provide a slow clock to avoid overloading by polling too fast */
    #ifndef LOWLEVEL_FUNCTION_TRACE
    #ifdef ALLOW_DEBUG
    if (counter-- == 0)
    {
      counter = DEBUG_DELAY;
      DEBUG_check_mail();
    }
    #endif
    #endif

    /**
     *  This changes the download speed from 1.5 sec to 5.5 sec on Android!
     */
    if(lower_multichannel_CPU_load || open_port_count <= 1)
    {
      if (!GLOBAL_is_in_time_critical_section)
      {
        Sleep(SLEEP_DELAY); /* Once in a while give more time OS. */
      }
    }

    #ifdef LOWLEVEL_FUNCTION_TRACE
    GLOBAL_clock_iteration++;
    #endif
    if(GLOBAL_SFH_in_idle && GLOBAL_MFH_in_idle) /* Both SFH *and* MFH in idle => we are doing nothing, so sleep */
    {
      idle_loop = true;
      ResetEvent(idle_event);
    }

    ReleaseMutex(loop_mutex);                 /* Leave critical region */
  } while (ThreadTerminate == 0);

  ThreadTerminate = 2; /* signal that we are done */
  #ifndef _WIN32
//    pthread_exit(0);
  #endif
  return 0;
}

/*---------------------------------------------------------------------------*/
void SetupUniqueLoopMutex(void)
{
  #ifndef _WIN32
    /* setup DLL and thread syncronication mutex's */
    CreateMutex( loop_mutex );
  #else
    char buf[25];
    int i =  0;
    uint8_t done = false;

    do
    {
      //snprintf(buf, 25, "libimc_loop%08X", (unsigned int)GetCurrentProcessId()&0xFFFFFFFF);
      snprintf(buf, 25,"IFWDLoop_%i",i);
      /* setup DLL and thread syncronication mutex's */
      loop_mutex = CreateMutex(0, false, buf);
      if(GetLastError() == ERROR_ALREADY_EXISTS) /* if the mutex has already been created by another instance */
      {
        (void)CloseHandle(loop_mutex);
        i++;                                     /* then find another one*/
      }
      else
      {
        done = true;
        if(GLOBAL_dll_instance_number == GLOBAL_INSTANCE_NOT_ASSIGNED) // no unique number found yet?
          GLOBAL_dll_instance_number = i;
      }
    } while( !done );
  #endif
}

/*---------------------------------------------------------------------------*/

/* ---------------------------------------------------------------------------
  DLL entry point.
--------------------------------------------------------------------------- */

/*
 * These function are automaticly called when the library is loaded
 * and unloaded.  When loaded initialization is done and a thread is
 * started, when unload the thread is stopped.
 */
#ifdef __GNUC__
/* SWISTART */
#ifndef _WIN32
  uint8_t WINAPI DllMain(HINSTANCE hinst, DWORD reason, LPVOID lpReserved);
#else
  BOOL WINAPI DllMain(HINSTANCE hinst, DWORD reason, LPVOID lpReserved);
#endif
/* SWISTOP */

  __attribute__((constructor)) void my_load()   { DllMain(0, DLL_PROCESS_ATTACH, NULL); }
  __attribute__((destructor))  void my_unload() { DllMain(0, DLL_PROCESS_DETACH, NULL); }
#endif

#if defined(__BORLANDC__) && !defined(LINT)
  #define DllMain DllEntryPoint
#endif

/* SWISTART */
#ifndef _WIN32
uint8_t WINAPI DllMain(HINSTANCE hinst, DWORD reason, LPVOID lpReserved)
#else
BOOL WINAPI DllMain(HINSTANCE hinst, DWORD reason, LPVOID lpReserved)
#endif
/* SWISTOP */
{
  #ifdef _WIN32
    PARAM_NOT_USED(lpReserved);
    uint16_t i;
    char event_name[50];
  #endif
  WasEntryPointCalled = true;   /* Sanity check  to ensure everything was initialized */

  switch (reason)
  {
    case DLL_PROCESS_ATTACH:
    {
      debug_active = true;
      DebugSetVerbosity(DEBUG_VERBOSITY_ERROR);

      #ifndef _WIN32
        CreateEvent( idle_event );
        CreateMutex( debug_mutex );
      #else

      /* Get this DLL's file path and name */
      memset(dll_file,0,MAX_PATH);
      (void)GetModuleFileName(hinst, dll_file, sizeof(dll_file));
      strcpy(dll_file_path, dll_file);

      /* Isolate the DLL path */
      i = strlen(dll_file_path);
      while ((dll_file_path[i] != (char )'\\') && i)
      {
        i--;
      }
      dll_file_path[i] = 0;
      /* Add the default INI file name to the path */
      sprintf(ini_file_name,"%s\\FlashCfi.ini", dll_file_path);

      sprintf(event_name,"libDownloadTool\\debug_file%i",GLOBAL_dll_instance_number);
      debug_mutex = CreateMutex(0, false, event_name);


      sprintf(event_name,"libDownloadTool\\idle%i",GLOBAL_dll_instance_number);
      idle_event = CreateEvent(NULL,    /* Use default security descriptor */
                               false,   /* Automatic reset to false after the waiting thread is released */
                               false,   /* Initial state */
                               event_name);   /* name of event */
      #endif // !WIN32

      done_mode = SFH_done_signal_semaphore;

      /* Init the CallBack to call the stub function */
      callback = GLOBAL_callback_stub;

      SetupUniqueLoopMutex();

      /* Set-up the thread */
      ThreadTerminate = 0;   /* The thread must terminate if != 0 */
      /* Init the OS IF's */
      COMPORT_init();
      // GenericUSB_init(); /* Note: Need to have winusb.dll installed */

      /* Init the handlers */
      BOOT_init(); /* Init the capabilities */
      MFH_init();
      SFH_init();
      TRA_init();
      PROT_init();
      COMM_init();
      CallClose = true; // remember that init() functions have been called

      #if 0
        CreateEvent( done_event );
        ThreadHandle = NewThread( ThreadId, ThreadFunc, NULL );
      #endif
      #ifndef _WIN32
        CreateEvent( done_event );
        ThreadHandle = pthread_create( &ThreadId, NULL, &ThreadFunc, NULL );
      #else
        /* This is the event that is used set when a function is completed by the SFH module */
        sprintf(event_name, "libDownloadTool%08X", (unsigned int)GetCurrentProcessId()&0xFFFFFFFF);
        done_event = CreateEvent(NULL,    /* Use default security descriptor */
                                 false,   /* Automatic reset to false after the waiting thread is released */
                                 false,   /* Initial state */
                                 event_name);   /* name of event */

        ThreadHandle = CreateThread(0,           /* Use default security descriptor */
                                    0,           /* Use default stack size */
                                    ThreadFunc,  /* The address of the thread function */
                                    0,           /* Parameter passed to the thread funtion */
                                    0,           /* The thread runs immediately after creation */
                                    &ThreadId);  /* Pointer to the variable to hold the thread ID */
      #endif // WIN32

      WaitForSingleObject( done_event, 55 ); /* Wait for thread to startup */

      //
      // TODO: Create libInjectionTool.a and statically link to use...
      //

      Debug("*** DLL MAIN (Attached) ***\n");
      open_port_count = 0;
      break;
    }

    case DLL_PROCESS_DETACH:
    {
      int n = 2;

      if(ThreadTerminate == 0)
        ThreadTerminate = 1;

      /* Close down the thread */
      GLOBAL_signal_not_in_idle(); /* make sure we bring the thread out of idle */
      while(ThreadTerminate == 1 && n > 0)
      {
        Sleep(55);
        n--;
      }

      if(CallClose)
      {
        COMM_close();
        BOOT_close();
        PROT_close();
        TRA_close();
        // SFH_close();  Removed from code as it was empty
        // MFH_close();  Removed from code as it was empty
        CallClose = false;
      }
      GLOBAL_trace_verbosity = 0;

      // TODO: Use the typedefs in OS.h
      #ifndef _WIN32
        //JoinThread( ThreadId );
        pthread_join( ThreadId, NULL );
      #else
        //JoinThread( ThreadHandle );
        CloseHandle(ThreadHandle);
      #endif
      CloseHandle(done_event);
      CloseMutex(loop_mutex);
      CloseHandle(idle_event);
      CloseMutex(debug_mutex);
      if(trace_file)
      {
        FILE_close(trace_file);
        trace_file = 0;
      }

      if(GLOBAL_trace_filename)
      {
        (void)MEM_free((uint8_t *)GLOBAL_trace_filename);
        GLOBAL_trace_filename = NULL;
      }
      if(GLOBAL_trace_buffer)
      {
        (void)MEM_free(GLOBAL_trace_buffer);
        GLOBAL_trace_buffer = NULL;
      }

      if (GLOBAL_NVM_flashless_path)
      {
        MEM_free(GLOBAL_NVM_flashless_path);
        GLOBAL_NVM_flashless_path = NULL;
      }

      GenericUSB_destroy();

      Debug("*** DLL MAIN (Detached) ***\n");
      break;
    }
  }
  return 1;
}
/*-------------------------------------------------------------------------------------*/
/*
Used to insert data into a DLL trace from an external source
*/
#ifdef USE_UNDERSCORE
  uint8_t DECLSPEC _IFWD_DL_trace_insert(uint8_t channel, uint8_t *data, uint32_t length, uint8_t tr_code)
#else
  uint8_t DECLSPEC IFWD_DL_trace_insert(uint8_t channel, uint8_t *data, uint32_t length, uint8_t tr_code)
#endif
{
  WAIT_FOR_THREAD;
  TR_BEGIN(TR_ExtInfo, tr_code,channel);
  if(data && length > 0)
  {
    TR_DATA(data,length);
  }
  TR_END;
  FX_DONE;
  return GLOBAL_trace_verbosity;
}
/*-------------------------------------------------------------------------------------*/

// local function to write header and PC info into a trace file
// the function *will* close the file after it is done, so you need to re-open it later
static void WriteTraceHeader(FILE *out)
{
  int data_length;
  uint32_t header = TRACE_MAGIC;
  char buffer[300];
  char OSver[65*2 + 1]; //    struct utsname name; has two members
  char prolific[50];
  char flashusb[50];
  char qflashusb[50];
  int oldtrace = GLOBAL_trace_verbosity;

  if ((void*)out == NULL)
    return;

  GetOSVersion(OSver);
#ifdef _WIN32
  GetSystem32FileVersion("drivers\\ser2pl.sys", prolific);
  GetSystem32FileVersion("drivers\\FlashUSB.sys", flashusb);
  GetSystem32FileVersion("drivers\\qFlashUSB.sys", qflashusb);
#else
  /* prolific, flashusb, qflashusb not present in Linux */
  snprintf(prolific, 50,"%s", "N/A");
  snprintf(flashusb, 50,"%s", "N/A");
  snprintf(qflashusb, 50,"%s", "N/A");
#endif

  // Write trace header identifier:
  (void)FILE_write(out, (uint8_t*)&header, sizeof(header));

  // Make sure we have allocated our trace buffer:
  if(!GLOBAL_trace_buffer)
    GLOBAL_trace_buffer = (uint8_t*)malloc(DLL_TRACE_BUFFER_SIZE);

  if(!GLOBAL_trace_buffer)
  {
    // Klocwork issue: Analyzed :)
    // This is the second try to make some kinda debug output with a minimal buffer.
    //  If this isn't allocated, then the problem is NOT in this app.
    GLOBAL_trace_buffer = (uint8_t*)malloc( sizeof(buffer) + 256 );
    if (GLOBAL_trace_buffer)
    {
      snprintf(buffer, 300, "\n Error: Not enough memory for the trace log!");
      data_length = strlen(buffer)+1;
      TR_BEGIN(TR_ExtInfo, TR_ExtInfo_Text,0);
      TR_DATA(buffer, data_length);
      TR_END;
    }
    oldtrace = false;
  }
  else
  {
    GLOBAL_trace_verbosity = DEBUG_VERBOSITY_NORMAL; // force trace on for now (so we can write the header now)
    snprintf(buffer, 300,"DLL:%s, OS:%s, Prolific:%s, FlashUSB:%s, qFlashUSB:%s",InternalGetVersion(),OSver,prolific,flashusb,qflashusb);
    data_length = strlen(buffer)+1;
    TR_BEGIN(TR_ExtInfo, TR_ExtInfo_Text,0);
    TR_DATA(buffer, data_length);
    TR_END;
  }

  GLOBAL_trace_verbosity = oldtrace; // restore settings
}
//---------------------------------------------------------------------------
#ifdef INCLUDE_UNUSED_FX
// TODO: Determine if this is actually needed in *nix...
void IFWD_switch_port(uint8_t channel, char *p_fw_download_port_name, uint32_t baud_rate)
{
  Debug("Switching Port to: %s (Baud rate %d)...", p_fw_download_port_name, baud_rate);

  if(COMM_close_physical_device_only(channel) == 0xFF) /* the boot port now is closed*/
    puts("Error in closing boot port");
  open_fw_download_port(channel, p_fw_download_port_name, baud_rate);
}
#endif

/**
 * Retreives the next free channel
 *
 * @return A channel number or zero for max channels used
 **/
#ifdef USE_UNDERSCORE
  uint8_t DECLSPEC _IFWD_get_next_free_channel()
#else
  uint8_t DECLSPEC IFWD_get_next_free_channel()
#endif
{
  return (open_port_count + 1);
}


/**
 * Retreives the next free channel
 *
 * @return A channel number or zero for max channels used
 **/
#ifdef USE_UNDERSCORE
  uint8_t DECLSPEC _IFWD_get_comm_handle(uint8_t id)
#else
  uint8_t DECLSPEC IFWD_get_comm_handle(uint8_t id)
#endif
{
  return (intptr_t)COMPORT_get_handle_I(id);
}


/**
 * Runs the USB Device Mapper
 *
 * @return An allocated path for the device id
 **/
#ifdef USE_UNDERSCORE
  char* DECLSPEC _IFWD_device_map(uint8_t id)
#else
  char* DECLSPEC IFWD_device_map(uint8_t id)
#endif
{
  return GenericUSB_mapper(id);
}

/**
 * Saves the current USB Device Map
 *
 * @return true on success
 **/
#ifdef USE_UNDERSCORE
  uint8_t DECLSPEC _IFWD_device_map_save(const char* map)
#else
  uint8_t DECLSPEC IFWD_device_map_save(const char* map)
#endif
{
  return GenericUSB_save_map(map);
}

/**
 * Loads a USB Device Map
 *
 * @return true on success
 **/
#ifdef USE_UNDERSCORE
  uint8_t DECLSPEC _IFWD_device_map_load(const char* map)
#else
  uint8_t DECLSPEC IFWD_device_map_load(const char* map)
#endif
{
  return GenericUSB_load_map(map);
}


#ifdef USE_UNDERSCORE
  void DECLSPEC _IFWD_set_module_verbose(MODULE_enum module, int verbose)
#else
  void DECLSPEC IFWD_set_module_verbose(MODULE_enum module, int verbose)
#endif
{
  if (module == MODULE_ALL)
    DebugSetVerbosity((DebugVerbosityLevel)verbose);
  else
    DebugSetModuleVerbosity(module, (DebugVerbosityLevel)verbose);
}

// This function is only used during testing of the DLL, and should return false to indicate this is the real DLL
#ifdef USE_UNDERSCORE
  uint8_t DECLSPEC _Test_Setup(void *data)
#else
  uint8_t DECLSPEC Test_Setup(void *data)
#endif
{
  PARAM_NOT_USED(data);
  GLOBAL_callback_stub(0, (IFWD_DL_status_enum)1, "void");
  WriteTraceHeader(NULL); // for coverage
  self_test();  // for coverage
#ifdef INCLUDE_UNUSED_FX
  formatBuffer(NULL,0);
#endif

#ifdef USE_UNDERSCORE
  _IFWD_get_next_free_channel();
  _IFWD_get_comm_handle(0);
  _IFWD_device_map(0);
  _IFWD_device_map_save(NULL);
  _IFWD_device_map_load(NULL);
#else
  IFWD_get_next_free_channel();
  IFWD_get_comm_handle(0);
  IFWD_device_map(0);
  IFWD_device_map_save(NULL);
  IFWD_device_map_load(NULL);
#endif

  return false;
}
//---------------------------------------------------------------------------
