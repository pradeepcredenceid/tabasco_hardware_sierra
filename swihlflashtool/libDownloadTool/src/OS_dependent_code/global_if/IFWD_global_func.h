
/*
 *       Copyright (C) 2011-2014 Intel Mobile Communications GmbH
 *
 *            Sec Class: Intel Confidential (IC)
 *
 *
 * This document contains proprietary information belonging to IMC.
 * Denmark A/S. Passing on and copying of this document, use and communication
 * of its contents is not permitted without prior written authorisation.
 *
 * Description:
 *   IFWD Download stack global functions - interface
 *
 * Revision Information:
 *  $File name:  /msw_tools/FlashTool/libDownloadTool/src/OS_dependent_code/global_if/IFWD_global_func.h $
 *  $CC-Version: .../dhkristx_140314_v0/1 $
 *  $Date:       2014-04-23    10:53:17 UTC $
 *   Comment:
 *     Added support for concatinating "Internal Name" in project version information to the DLL version string
 */
#ifndef _INC_GLOBAL
#define _INC_GLOBAL

#include "IMC_DownloadDll.h"
#include "IFWD_text_def.h"

#include "nand_spare_area_internal.h"

/*---------------------------------------------*/
/* Enumeration                                 */
/*---------------------------------------------*/

typedef enum
{
  GLOBAL_platform_group_unknown = 0,
  GLOBAL_platform_group_egold,
  GLOBAL_platform_group_sgold
} GLOBAL_platform_group_type;


typedef enum
{
  GLOBAL_dut_test_unknown = 0,
  GLOBAL_dut_test_usb_certification,
  GLOBAL_dut_test_usb_in_token,
  GLOBAL_dut_test_usb_out_token,
  GLOBAL_dut_test_usb_loopback
} GLOBAL_dut_test_group_type;

/*---------------------------------------------*/
/* External defines                            */
/*---------------------------------------------*/
#define MAX_TOC_LENGTH               256
#define MAX_STRING_LENTH             1024
#define MAX_BUFFER_LENGTH       1024*1024         // Max 1 MB

#define MAX_NUMBER_OF_CHANNELS       32
#define DEFAULT_COMM_PROCESS_TIMEOUT 120        //< Sec (per block comm timeout)
#define DEFAULT_GLOBAL_BOOT_TIMEOUT  10000      //< ms (Entire boot sequence)
#define DEFAULT_GLOBAL_COMM_TIMEOUT  6000       //< ms (per block prot timeout)
#define GLOBAL_INSTANCE_NOT_ASSIGNED 1000

#define COMM_TECH_ASYNC_COM             0x0000
#define COMM_TECH_MIPI_PRE_OPEN         0x0021   // use this state before the port is opened
#define COMM_TECH_MIPI_OPENED           0x0022   // the OPENED value must be +1 as the code expects this.
#define COMM_TECH_MIPI_EXT_BOX_PRE_OPEN 0x0031   // use this state before the port is opened
#define COMM_TECH_MIPI_EXT_BOX_OPENED   0x0032   // the OPENED value must be +1 as the code expects this.

#define COMM_TECH_IS_MIPI(cH)             ((GLOBAL_comm_technology[cH] == COMM_TECH_MIPI_EXT_BOX_OPENED) || (GLOBAL_comm_technology[cH] == COMM_TECH_MIPI_OPENED))
#define COMM_TECH_IS_MIPI_ADD_HEADER(cH)  (GLOBAL_comm_technology[cH] == COMM_TECH_MIPI_EXT_BOX_OPENED)

#define PRE_BOOT_NOT_USED 0x00000000
#define PRE_BOOT_ARMED    0x00000001
#define PRE_BOOT_DONE     0x00000002

#define Min(a,b) ((a)>(b) ? (b) : (a))
#define Max(a,b) ((a)<(b) ? (b) : (a))

#define xstrcpy(X,Y) \
  if((X)!=0) \
  {          \
    int max = 500-1; \
    strncpy(X, Y, max); \
    X[max] = '\0';      \
  }

/*---------------------------------------------*/
/* External types                              */
/*---------------------------------------------*/
typedef void (*EmulateTx_t)(uint32_t channel, uint8_t *data, uint32_t length, uint32_t *number_of_bytes_written);
typedef void (*EmulateRx_t)(uint32_t channel, uint8_t *dest, uint32_t number_of_bytes_to_read, uint32_t *number_of_bytes_read);
typedef uint32_t (*EmulateRxAvail_t)(uint32_t channel);

/*---------------------------------------------*/
/* External declarations                       */
/*---------------------------------------------*/
#ifdef __cplusplus
  extern "C" {
#endif

extern void (*callback)(uint8_t channel, IFWD_DL_status_enum status, char *status_text); /* Pointer to the CallBack function */

extern HANDLE done_event;

extern uint8_t  GLOBAL_use_plain_text;  /* Flag to determine if texts are send out as plain text or coded. */
extern uint32_t GLOBAL_boot_timeout;
extern uint32_t GLOBAL_comm_timeout;    /* Call "TRA_GetCommTimeOut()" to get the current timeout value for a given channel */
extern uint32_t GLOBAL_comm_process_timeout;

extern uint8_t GLOBAL_use_pre_erase;
extern uint8_t GLOBAL_cond_dyn_eep_erase;
extern uint8_t GLOBAL_force_area_erase;
extern uint8_t GLOBAL_use_sec_pack_erase_flag;
extern uint8_t GLOBAL_upload_mbn_data;
extern char    GLOBAL_text[MAX_TEXT_LENGTH];
extern uint8_t GLOBAL_SFH_in_idle;
extern uint8_t GLOBAL_MFH_in_idle;
extern int     GLOBAL_want_clock_now; /* used by clock() handlers and callback handlers to signal that they are about to do time
                                     critical stuff and want the clock handlers called again ASAP (don't give Windows breeth time) */
/* SWISTART */
#ifndef SIERRA
extern uint32_t GLOBAL_erase_mode;     /* tells how block should be erased on target before download */
#else /* SIERRA */
#define GLOBAL_erase_mode IFWD_DL_target_erase_before_write
#endif/* SIERRA */
/* SWISTOP */
extern uint32_t GLOBAL_dll_instance_number;   /* Unique instance number for this DLL */
extern uint8_t  GLOBAL_trace_verbosity;
extern char*    GLOBAL_trace_filename;
extern uint8_t* GLOBAL_trace_buffer;
extern uint8_t  GLOBAL_force_plain_upload;
extern T_MAC_PARTITION_CONFIG GLOBAL_nand_partition_config;
extern long (*GLOBAL_clock)(void); /* pointer to clock() function (encapsulation needed to make it possible to call it in OS independant files) */
extern uint8_t GLOBAL_usb_auto_mode;
extern uint8_t GLOBAL_skip_wr_pack_crc;
extern uint8_t GLOBAL_check_sign_hw_cfg_value;
extern uint8_t GLOBAL_ban_eep_mode_writes;
extern EmulateTx_t GLOBAL_EmulateTx;
extern EmulateRx_t GLOBAL_EmulateRx;
extern EmulateRxAvail_t GLOBAL_EmulateRxAvail;
extern uint8_t   GLOBAL_UseCOMPortEmulation;
extern uint32_t  GLOBAL_use_pack_size[MAX_NUMBER_OF_CHANNELS+1];
extern uint32_t  GLOBAL_comm_technology[MAX_NUMBER_OF_CHANNELS+1];
extern uint32_t  GLOBAL_comm_align_bytes[MAX_NUMBER_OF_CHANNELS+1];
extern uint8_t   GLOBAL_use_sec_ch_ext_cb[MAX_NUMBER_OF_CHANNELS+1];
extern uint32_t  GLOBAL_pre_boot[MAX_NUMBER_OF_CHANNELS+1];

extern char GLOBAL_comm_port_name[MAX_NUMBER_OF_CHANNELS+1][128];   /* The name of the main comm port                      */
extern char GLOBAL_ctrl_port_name[MAX_NUMBER_OF_CHANNELS+1][128];   /* The name of the comm port used for hardware control as the use of MIPI converter box */

extern uint8_t *GLOBAL_NVM_flashless_path;
extern uint32_t GLOBAL_NVM_flashless_options;
extern uint8_t GLOBAL_use_hw_ch_change;     //< TODO: Change name... This means more special treatment.
extern uint8_t GLOBAL_use_alt_boot_speed;
extern uint32_t GLOBAL_use_vp_tx_chunk_delay;  /* Default chunk delay time in ms */
extern uint32_t GLOBAL_use_vp_tx_chunk_size;   /* Default chunk size */
extern GLOBAL_platform_group_type GLOBAL_platform_group;
extern GLOBAL_dut_test_group_type GLOBAL_dut_test_request;
extern uint8_t GLOBAL_dut_in_test_mode;

extern sec_ch_callback_type secure_channel_cb_fx;
extern uint8_t GLOBAL_skip_sw_load_if_sys_val_ok;
extern uint8_t GLOBAL_is_in_time_critical_section;

extern uint8_t GLOBAL_force_full_emulation;
extern uint32_t  GLOBAL_special_boot_seq;
/**
  TODO: Let these globals be set in the COMPORT module

  GLOBAL_comm_technology[]

**/
extern uint8_t GLOBAL_usb_device;
extern uint8_t GLOBAL_use_GenericUSB;
extern uint8_t GLOBAL_hsi_link;

extern uint8_t GLOBAL_precheck_mode;

/*---------------------------------------------*/
/* External functions                          */
/*---------------------------------------------*/
void GLOBAL_callback_stub (uint8_t channel, IFWD_DL_status_enum status, char *status_text);
void GLOBAL_callback(uint8_t channel, IFWD_DL_status_enum status, char *status_text);
char *GLOBAL_format_text(IFWD_DL_text_class_enum text_class, uint32_t text_code, ...);
const char *GLOBAL_get_raw_text(IFWD_DL_text_class_enum text_class, uint32_t text_code);
void get_os_api_error_text(uint16_t error_code);
void GLOBAL_signal_not_in_idle(void);
void ShowMessage(char *text);
uint8_t GLOBAL_GetFileVersion(char *File, uint16_t *Major, uint16_t *Minor, uint16_t *Release, uint16_t *Build);
uint8_t GetSystem32FileVersion(char *driver_file, char *dest);
uint8_t sprintf_version_info(char *dest, const char *lpszModuleName, const char *desc);
void GetOSVersion(char *dest);


/*-------------------------------------------------------------------------------------
 *   char *GLOBAL_TOC_filename(char *filename, uint32_t *TOC_index_output)
 *
 *   Extracts the real filename and TOC number from a string
 *
 *   INPUT PARAMATERS:
 *     filename: the filename. e.g. "|0|c:\MPE_download.fls" or "c:\sw.fls"
 *     TOC_index_output: pointer to place the TOC number at. Extracted for "filename" (-1 if it is an ordinary file)
 *                       If this is not wanted a NULL pointer is passed
 *   OUTPUT:
 *     returns the input filename stripped of any TOC characters, e.g.
 *       "|0|c:\MPE_download.fls" => "c:\MPE_download.fls"
 *       "c:\MPE_download.fls" => "c:\MPE_download.fls"
 -------------------------------------------------------------------------------------*/
char *GLOBAL_TOC_filename(char *filename, int32_t *TOC_index_output);

#ifdef __cplusplus
  }
#endif

#endif
