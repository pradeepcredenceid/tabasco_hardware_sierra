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
* Description:
*   IFWD single file handler - code
*     The purpose of this handler is to do the processing and sequencing
*     needed to carry out single downloads or updates.
*
* Revision Information:
*  $File name:  /msw_tools/FlashTool/libDownloadTool/src/Portable_code/single_file_handler/IFWD_single_file_handler.c $
*  $CC-Version: .../oint_tools_flashtoole2_01/46 $
*  $Date:       2014-03-13    15:40:37 UTC $
*   Resonsible: njk, hilmar
*   Comment:
*     SMS01242479 Made support for odd length certificate.
*/

#define THIS_MODULE MODULE_SFH  //< Runtime verbosity evaluation

/*---------------------------------------------*/
/* Include files.                              */
/*---------------------------------------------*/
#include <string.h>    /* C standard lib: Containing strcpy memcpy etc. */
#include <stdio.h>     /* C standard lib: Containing printf scanf etc. */


/* Enable the Memory text array in header.h */
#define INCLUDE_MEM_CLASS_NAME_ARRAY
#include "OS.h"
#include "FileIO.h"
#include "header.h"

#include "IMC_DownloadDll.h"
#include "IFWD_global_func.h"

#include "IFWD_timer.h"
#include "IFWD_mem.h"
#include "IFWD_boot.h"
#include "DebugModule.h"

#include "FlashInitRead.h"
#include "EepDeltaMerge.h"

#include "IFWD_single_file_handler.h"
#include "IFWD_transport_handler.h"
#include "IFWD_protocol_handler.h"
#include "IFWD_comport.h"
#include "IFWD_comm.h"
#include "IFWD_sec.h"     //< TODO: Replace with #include "Security.h"
#include "PrgHandler.h"

#include "IFWD_sec_channel.h"


//#define THIS_DLL_VERSION      0x00030000
#define THIS_DLL_VERSION        0x00040000

//#define HANDLE_DFFS             0x00000001
#define HANDLE_EEP              0x00000002
#define HANDLE_USB              0x00000004
#define HANDLE_PREERASE         0x00000008

#define HANDLE_LOADMAP          0x00000010
#define HANDLE_EEP_TEST_MODE    0x00000020
#define HANDLE_DFFS_DYN         0x00000040
#define HANDLE_DFFS_STAT        0x00000080

#define HANDLE_USE_CALC_CRC     0x00000100
#define HANDLE_STOP_AFTER_ERASE 0x00000200
#define HANDLE_CHECK_SPLIT_ID   0x00000400
#define HANDLE_DYN_EEP_ERASE    0x00000800

#define HANDLE_FLS              0x00001000
#define HANDLE_UPLOAD_TO_MEM    0x00002000
#define HANDLE_BIN              0x00004000
#define HANDLE_DNLOAD_FROM_MEM  0x00008000

#define HANDLE_RMW_SEQ          0x00010000 /* Read Modify Write => Update sequence */
#define HANDLE_ERASE_ALL        0x00020000
#define HANDLE_ERASE_TOTAL_LEN  0x00040000
#define HANDLE_ERASE_ALL_BAD   (0x00080000 | HANDLE_ERASE_ALL)

#define HANDLE_BIN_SEND_MBN     0x00100000
#define HANDLE_UPLOAD_ADD_EBL   0x00200000
#define HANDLE_BIN_GET_MBN      0x00400000
#define HANDLE_ERASE_REGION     0x00800000  /* To be set if the flag in the load-map for erasing the full region is set AND the global flags enabeling this is set */

#define LOAD_MAP_ERASE_REGION_FLAG 0x80000000

#define SFH_PACK_SIZE           0x40000    /* 32 Kbytes */
#define SFH_CNF_PACK_SIZE       0x4000
/* { ab 03-06-2010: sms01519721 */
#define SFH_1_32_CNF_PACK_SIZE  0x20000
/* } */
#define DELAY_CHANGE_BAUD      20 // milli seconds
#define DELAY_CHANGE_PROTOCOL 200 // milli seconds
#define DELAY_ERASE_CHECK     200 // milli seconds
#define DELAY_TARGET_RESET      2 // milli seconde

//< Previous USB chunk size: 0x01000000 (Same as in IFWD_comm.c)
#define USB_CHUNK_SIZE    0x020000

//< Target MIPI DMA has a max of 256kB (including header) (Same as in IFWD_comm.c)
#define MIPI_CHUNK_SIZE   (256*1024)-sizeof(uint32_t)

const char* boot_verdict_string = "";
const char boot_verdict_txt[4][30] = {"Booting allowed.", "Booting not allowed.", "Booting restricted mode.", "Index out of range"};

/*---------------------------------------------*/
/* typedef                                     */
/*---------------------------------------------*/
typedef enum
{
  SFH_state_idle = 0,

  /* Stages to wait for callback */
  SFH_state_await_tra, //1
  SFH_state_await_boot, //2

  /* Boot stages */
  SFH_state_boot, //3
  SFH_state_boot_delayed_open, //4
  SFH_state_boot_check_capabilities, //5
  SFH_state_boot_set_high_baud, //6
  SFH_state_boot_get_id, //7
  SFH_state_boot_cfi_1, //8
  SFH_state_boot_cfi_2, //9
  SFH_state_send_nand_control, //10

  SFH_state_boot_ok, //11

  /* Load stages */
  SFH_state_load, //12
  SFH_state_load_loop_dffs_sections_start, //13
  SFH_state_read_split_id_1,      // OBSOLETE .... definition keept so trace file definitions are still valid.
  SFH_state_read_split_id_2,      // OBSOLETE .... definition keept so trace file definitions are still valid.
  SFH_state_read_split_id_rsp,    // OBSOLETE .... definition keept so trace file definitions are still valid.

  SFH_state_erase_dyn_eep, //17
  SFH_state_erase_dyn_eep_ver_sec, //18
  SFH_state_erase_dyn_eep_req_ver, //19
  SFH_state_erase_dyn_eep_ver_chk, //20
  SFH_state_erase_dyn_eep_ver_sec_end, //21
  SFH_state_erase_dyn_eep_sec_start, //22
  SFH_state_erase_dyn_eep_doit, //23
  SFH_state_erase_dyn_eep_chk, //24
  SFH_state_erase_dyn_eep_chk_cnf, //25
  SFH_state_erase_dyn_eep_sec_end, //26

  SFH_state_load_send_sec, //27
  SFH_state_load_send_sec_rsp, //28
  SFH_state_load_loop_loadmap_start, //29
  SFH_state_load_start_erase, //30
  SFH_state_load_pre_erase, //31
  SFH_state_load_pre_erase_cnf, //32
  SFH_state_load_pre_erase_delay, //33
  SFH_state_load_send_address, //34
  SFH_state_load_send_mbn, //35
  SFH_state_load_modify_usb_and_send_1st, //36
  SFH_state_load_modify_eep_and_send_1st, //37
  SFH_state_load_send_data, //38
  SFH_state_load_check_calc_checksum, //39
  SFH_state_erase_chk_cnf, //40
  SFH_state_erase_chk_2, //41
  SFH_state_erase_chk, //42
  SFH_state_load_loop_loadmap_end, //43
  SFH_state_load_check_normal_checksum, //44
  SFH_state_load_send_nvm, //45
  SFH_state_load_send_sec_end, //46
  SFH_state_load_send_sec_end_rsp, //47
  SFH_state_load_loop_dffs_sections_end, //48

  /* Upload stages */
  SFH_state_upload, //49
  SFH_state_upload_send_sec, //50
  SFH_state_upload_send_sec_rsp, //51
  SFH_state_upload_send_address, //52
  SFH_state_upload_get_mbn_data, //53
  SFH_state_upload_get_data, //54
  SFH_state_upload_add_ebl, //55
  SFH_state_upload_send_sec_end, //56
  SFH_state_upload_send_sec_end_rsp, //57

  /* Update stages */
  SFH_state_eeprom_update, //58

  /* AT_mode state */
  SFH_state_AT_mode, //59

  /* OutOfSession communication handling */
  SFH_state_oos_send_control, //60
  SFH_state_oos_send_data, //61
  SFH_state_oos_receive_data, //62
  SFH_state_oos_receive_data_length, //63

  /* Target reset handling */
  SFH_state_force_target_reset, //64
  SFH_state_force_target_reset_waiting, //65

  /* Send 'end of process' pack to the target */
  SFH_state_end_of_process, //66

  /* pass or fail ... ALWAYS end here !!!!! */
  SFH_state_done_error,//67
  SFH_state_done_ok, //68

  SFH_state_send_xml_oos_ctrl, //69
  SFH_state_send_xml_oos_data, //70

  SFH_state_erase_all_nand_sec_start, //71
  SFH_state_erase_all_nand, //72
  SFH_state_erase_all_nand_chk, //73
  SFH_state_erase_all_nand_chk_cnf, //74
  SFH_state_erase_all_nand_sec_end, //75
  SFH_state_load_send_data_raw1, //76
  SFH_state_load_send_data_raw2, //77
  SFH_state_load_send_data_raw_cnf1, //78
  SFH_state_load_send_data_raw_cnf2, //79
  SFH_state_load_send_data_raw_loop, //80
  SFH_state_await_comm, //81

  SFH_state_change_protocol, //82
  SFH_state_change_protocol_doit, //83
  SFH_state_change_protocol_wait, //84
  SFH_state_boot_set_high_baud_delay, //85
  SFH_state_sec_ch_req_nonce_and_ver,      // 86
  SFH_state_sec_ch_req_nonce_and_ver_rsp,  // 87
  SFH_state_sec_ch_req_verdict,            // 88
  SFH_state_sec_ch_req_verdict_rsp,        // 89
  SFH_state_force_target_reset_send,       // 90
  SFH_state_load_dummy
} SFH_state_type;



typedef struct {
  uint32_t PsiOffset;
  uint32_t PsiLength;
  uint32_t EblOffset;
  uint32_t EblLength;
  uint16_t PsiCrc;
  uint16_t EblCrc;

  HardwareStructType  Hardware;
  MemoryMapStructType MemoryMap;
  SecurityStructType  Security;
}NEWH_GenericHeaderStructType;


typedef struct
{
  /* Linked list management */
  void *prev_ctx;                   /* Pointer to the previous context in the linked list  */
  void *next_ctx;                   /* Pointer to the previous context in the linked list  */
  uint8_t channel;                  /* The logical channel assigned to this context        */

  /* For the state machine */
  SFH_state_type prev_state;        /* Used to detect state transitions                    */
  SFH_state_type state;             /* The current state of the SFH state machine          */
  SFH_state_type next_state;        /* The next state of the SFH state machine, (used for macro states)          */
  char status_text[500];            /* Used to hold status messages that can not be returned immidiately */
  SFH_status_enum status;           /* The status of the last opperation */
  SFH_done_signal_enum done_signal; /* Tell us how to report the error or dons state  */

  /* releated to the ports */
  char comm_port_name[MAX_COMPORT_NAME_LENGTH+1];   /* The name of the main comm port                      */
  char ctrl_port_name[MAX_COMPORT_NAME_LENGTH+1];   /* The name of the comm port used for hardware control as the use of MIPI converter box */
  uint32_t boot_baudrate;             /* The baudrate to be used while booting               */
  uint32_t high_baudrate;             /* The baudrate to be used while downloading           */
  uint32_t temp_timeout_storage;
  /* Boot stuff */
  char ref_file_name[500];   /* Name of the file used when booting                  */
  FILE *ref_file;            /* Handler of the reference file                       */
  NEWH_GenericHeaderStructType ref_header; /* Holds the generic header part of the FLS file header for reference */

  uint8_t *psi_data_ptr;              /* Pointer to the PSI data to be injected during boot  */
  uint8_t *ebl_data_ptr;              /* Pointer to the EBL data to be injected during boot  */
  boot_dev_cfg_t boot_dev_cfg;
  boot_ebl_version_t boot_ebl_version;
  uint32_t used_protocol;
  uint8_t got_boot_callback;
  uint8_t boot_status;
  IFWD_DL_modem_control_signals_type *modem_controls;
  char force_use_psi_file_name[MAX_PATH];
  char force_use_ebl_file_name[MAX_PATH];


  /* Handling commands */
  uint8_t cmd_buf[SFH_PACK_SIZE];
  uint8_t cnf_buf[SFH_CNF_PACK_SIZE];
  uint8_t got_tra_callback;
  TRA_status_enum tra_status;

  /* Download file handling */
  char load_file_name[500];   /* Name of the file used when booting                  */
  FILE *load_file;            /* Handler of the reference file                       */
  NEWH_GenericHeaderStructType load_header; /* Holds the generic header part of the FLS file header */
  DownloadDataStructType download_data_struct;
  uint32_t handling_flags;             /* Flags to trigger special handling in the state machine */
  uint32_t prev_handling_flags;        /* for tracing changes to the flags. */
  uint32_t load_address;               /* The absolute address in the target memory where to load */
  uint32_t load_length;                /* The length (bytes) of the area to load */
  uint32_t load_length_total;          /* The length (bytes) of the area to load */
  uint32_t erase_length;               /* The total size of the flash as indicated by the CFI information resurned during boot */
  uint32_t erase_address;              /* Start address for erase the flash */
  uint32_t load_file_offset;           /* The offset into the file where the data to load is read */
  uint32_t load_uid;
  uint32_t load_mem_class;
  uint8_t  loadmap_index;              /* Index into the loadmap structure */
  uint8_t  dffs_index;                 /* Index into the dffs section structure */
  uint16_t target_checksum;            /* The checksum received from the target for each load region */
  uint16_t calc_checksum;              /* The calculated checksum from transport layer */
  uint16_t exp_checksum;               /* The XOR sum of the expected checksum for each load region within a sec region. */
  uint16_t last_percent;               /* Used when reporting percent during erase */
  uint16_t temp_u16;                   /* Used for local counters etc. May not survive outside scope */
  uint16_t temp2_u16;                  /* Used for local counters etc. May not survive outside scope */
  uint32_t temp_u32;                   /* Used for local counters etc. May not survive outside scope */
  uint8_t erase_all_done;
  uint8_t target_sw_running_skip_dl;   /* Used to signal that boot handler have reported .. */
                                       /* .. that the target flashless sw has been validated and started .. no need for download */

  /* Upload stuff */
  uint8_t *load_data_ptr;              /* Used when uploading to memory */
  char dep_file_name[500];             /* Name of the file used for delta update                  */
  char project_prefix[32];             /* Name of the project prefix used to create the CFG file names for delta update */
  uint8_t upload_mbn_buffer[500];
  uint16_t cur_eeprom_ver;
  uint16_t new_eeprom_ver;

  /* OutOfSession communication stuff */
  uint32_t oos_action;
  uint32_t oos_type;
  uint32_t oos_length;
  uint32_t oos_spare;
  uint8_t *oos_data_ptr;

  /* Keep info that may be requested by the host program */
  uint32_t flash_id;
  uint32_t flash_voltage;

  /* XML handling stuff */
  uint8_t *xml_ptr;
  uint32_t xml_length;
  uint8_t got_comm_callback;

  /* Raw data buf */
  uint8_t       *raw_data_ptr;
  FILE *raw_data_file;
  uint32_t       raw_remaining_bytes;
  uint32_t       raw_chunk_size;
  uint32_t       raw_load_length;

  /* Debug and test stuff */
  timer_chronom_t test_timer;
  timer_chronom_t total_timer;
  timer_chronom_t bandwidth_meas_timer;
  uint8_t bw_meas_index;

  /* Timer used create delay between the states in the machine below */
  timer_event_timer_t state_delay_timer;

} SFH_ctx_type;

void fail_global_text(SFH_ctx_type *ctx_ptr, char* function, int text_code);
void SFH_COMM_callback(uint8_t channel);
/*---------------------------------------------*/
/* globale variabler                           */
/*---------------------------------------------*/

/*---------------------------------------------*/
/* Local defines.                              */
/*---------------------------------------------*/


/*---------------------------------------------*/
/* Local const                                 */
/*---------------------------------------------*/


/*---------------------------------------------*/
/* local variables                             */
/*---------------------------------------------*/
static SFH_ctx_type *ctx_base_ptr = NULL;
static SFH_ctx_type *active_ctx_ptr = NULL;
static uint32_t SFH_nof_active_instances = 0;
static uint32_t SFH_nof_idle = 0;
static uint32_t SFH_nvm_nof_options = 0;
static uint8_t *SFH_nvm_options = NULL;

/*---------------------------------------------*/
/* local functions.                            */
/*---------------------------------------------*/
static SFH_ctx_type *get_ctx(uint8_t channel)
{
  SFH_ctx_type *ctx_ptr;
  if (ctx_base_ptr == NULL)
  {
    return (NULL);
  }
  else
  {
    ctx_ptr = ctx_base_ptr;
    do
    {
      if (ctx_ptr->channel == channel)
      {
        return(ctx_ptr);
      }
      else
      {
        ctx_ptr = ctx_ptr->next_ctx;
      }
    } while(ctx_ptr);
  }
  /* could not find a match */
  return (NULL);
}

static void send_percent(uint8_t channel, int percent)
{
  SFH_ctx_type *ctx_ptr;
  static char percentStr[4];

  ctx_ptr = get_ctx(channel);
  if (ctx_ptr)
  {
    if(percent > 100) /* this should never happen, but in case of rounding errors */
      percent = 100;
    if(percent < 0)
      percent = 0;
    if(ctx_ptr->last_percent != percent)
    {
      ctx_ptr->last_percent = percent;
      snprintf(percentStr, 4,"%i",percent);
      GLOBAL_callback(channel, IFWD_DL_ProgressUpdated, percentStr);
    }
  }
}


static void show_capabilities(uint8_t channel, capabilities_t *cap)
{
  SFH_ctx_type *ctx_ptr;

  ctx_ptr = get_ctx(channel);
  if (ctx_ptr)
  {
     /* report CRC/compression in normal log for everyone to see: */
     snprintf(GLOBAL_text, MAX_TEXT_LENGTH, "%s: %s, %s %s %s",
       GLOBAL_get_raw_text(IFWD_DL_text_class_process_info,SFH_INFO_using),
       (cap->crc_method == 0)  ? GLOBAL_get_raw_text(IFWD_DL_text_class_process_info,SFH_INFO_normal_crc) : GLOBAL_get_raw_text(IFWD_DL_text_class_process_info,SFH_INFO_faster_crc),
       (cap->compression == 0) ? GLOBAL_get_raw_text(IFWD_DL_text_class_process_info,SFH_INFO_all_blocks) : GLOBAL_get_raw_text(IFWD_DL_text_class_process_info,SFH_INFO_skip_blocks),
       (cap->crc_type == 0) ? "" : GLOBAL_get_raw_text(IFWD_DL_text_class_process_info,SFH_INFO_no_data_crc),
       (cap->protocol_crc == 0) ? "" : GLOBAL_get_raw_text(IFWD_DL_text_class_process_info,SFH_INFO_no_prot_crc)
     );
     GLOBAL_callback(channel,IFWD_DL_ProcessOutlineUpdated,GLOBAL_text);

     /* report package size in detailed log: */
     switch(ctx_ptr->used_protocol)
     {
       case PROT_3_32:
       {
         GLOBAL_callback(channel, IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_package_len,
                         (cap->package_size == 0) ? DEFAULT_PACK_SIZE : cap->package_size*1024, "3.32"));
         break;
       }
       case PROT_1_16:
       {
         GLOBAL_callback(channel, IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_package_len,
                        (cap->package_size == 0) ? DEFAULT_PACK_SIZE : cap->package_size*1024, "1.16"));
         break;
       }
       case PROT_1_32:
       {
         GLOBAL_callback(channel, IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_package_len,
                         (cap->package_size*1024), "1.32"));
         break;
       }
     }
  }
}


static uint16_t calc_percent(uint32_t position, uint32_t total)
{
  if(total > 10000000)   /* Possible overflow ? */
  {
    total = total / 1000;
    position = position / 1000;
  }
  else if (total == 0)
  {
    return 0;
  }
  return (position * 100) / total;
}

static void ShowTime(uint8_t channel, uint32_t msec)
{
  if(msec < 3000)  /* Show short times in miliseconds */
  {
    GLOBAL_callback(channel,IFWD_DL_ProcessOutlineUpdated/*IFWD_DL_ProcessDetailUpdated*/, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_load_time_msec,msec));
  }
  else
  {
    int sec, min;

    sec = msec / 1000;
    min = sec / 60;
    sec -= min * 60; /* subtract the seconds we have shown in minutes */

    if(min == 0)
    {
      char time_buffer[8];
      int fraction = (msec - sec*1000) / 100; /*  fraction digit is measured in 10th of a second */
      snprintf(time_buffer, 8,"%i.%i",sec,fraction);
      GLOBAL_callback(channel,IFWD_DL_ProcessOutlineUpdated/*IFWD_DL_ProcessDetailUpdated*/, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_load_time_sec,time_buffer));
    }
    else
      GLOBAL_callback(channel,IFWD_DL_ProcessOutlineUpdated/*IFWD_DL_ProcessDetailUpdated*/, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_load_time,min,sec));
  }
}
static void ShowTotalTime(uint8_t channel, uint32_t msec)
{
  if(msec < 3000)  /* Show short times in miliseconds */
  {
    GLOBAL_callback(channel,IFWD_DL_ProcessOutlineUpdated/*IFWD_DL_ProcessDetailUpdated*/, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_tot_load_time_msec,msec));
  }
  else
  {
    int sec, min;

    sec = msec / 1000;
    min = sec / 60;
    sec -= min * 60; /* subtract the seconds we have shown in minutes */

    if(min == 0)
    {
      char time_buffer[8];
      int fraction = (msec - sec*1000) / 100; /*  fraction digit is measured in 10th of a second */
      snprintf(time_buffer, 8,"%i.%i",sec,fraction);
      GLOBAL_callback(channel,IFWD_DL_ProcessOutlineUpdated/*IFWD_DL_ProcessDetailUpdated*/, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_tot_load_time_sec,time_buffer));
    }
    else
      GLOBAL_callback(channel,IFWD_DL_ProcessOutlineUpdated/*IFWD_DL_ProcessDetailUpdated*/, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_tot_load_time,min,sec));
  }
}
uint32_t SFH_GetDebugHeaderOffset_file(FILE *file)
{
  uint8_t offset[4];
  uint32_t marker;
  FILE_seek(file,0);
  FILE_read(file,(uint8_t*)&marker,4);
  if (marker != 0xED1D6491)
  {
    return(0);
  }
  FILE_seek(file,0x26);
  FILE_read(file,offset,4);
  marker  = offset[0] << 24;
  marker |= offset[1] << 16;
  marker |= offset[2] << 8;
  marker |= offset[3];
  return (marker);
}

uint32_t SFH_GetDebugHeaderOffset(uint8_t *data_ptr)
{
  uint8_t offset[4];
  uint32_t marker;
  marker = *((uint32_t*)data_ptr);
  if (marker != 0xED1D6491)
  {
    return(0);
  }
  *((uint32_t*)offset) = *((uint32_t*)(data_ptr + 0x26));

  marker  = offset[0] << 24;
  marker |= offset[1] << 16;
  marker |= offset[2] << 8;
  marker |= offset[3];
  return (marker);
}

/*---------------------------------------------------------------------------*/


void init_ctx(uint8_t channel, SFH_ctx_type *this_ctx, char *status)
{
  /* init the context here */
  this_ctx->channel = channel;
  this_ctx->state = SFH_state_idle;
  this_ctx->prev_state = SFH_state_load_dummy; /* Set to un-used state so the first logged state transition will be to Idle */
  this_ctx->comm_port_name[0] = 0;
  this_ctx->ctrl_port_name[0] = 0;
  this_ctx->boot_baudrate = 0;

  this_ctx->ref_file_name[0] = 0;
  this_ctx->ref_file = NULL;
  this_ctx->psi_data_ptr = NULL;
  this_ctx->ebl_data_ptr = NULL;
  this_ctx->xml_ptr = NULL;
  this_ctx->xml_length = 0;
  this_ctx->force_use_psi_file_name[0] = 0;
  this_ctx->force_use_ebl_file_name[0] = 0;

  this_ctx->load_file_name[0] = 0;
  this_ctx->load_file = NULL;
  this_ctx->handling_flags      = 0x00000000;
  this_ctx->prev_handling_flags = 0x00000000;
  this_ctx->got_tra_callback = false;
  this_ctx->got_boot_callback = false;
  this_ctx->got_comm_callback = false;
  this_ctx->target_sw_running_skip_dl = false;
  this_ctx->status_text[0] = 0;

  if(status)
    status[0] = 0;

  // For coverage
  fail_global_text(NULL,NULL,0);
  SFH_COMM_callback(0);
  SFH_get_status(0,NULL);
}

void close_ctx(SFH_ctx_type *this_ctx)
{
  /* free all allocated memory for this context*/
  /* Free the PSI data if any */
  if (this_ctx->psi_data_ptr)
  {
    MEM_free( this_ctx->psi_data_ptr );
    this_ctx->psi_data_ptr = NULL;
  }

  /* Free the EBL data if any */
  if (this_ctx->ebl_data_ptr)
  {
    MEM_free( this_ctx->ebl_data_ptr );
    this_ctx->ebl_data_ptr = NULL;
  }

  /* Free the XML data if any */
  if (this_ctx->xml_ptr)
  {
    MEM_free( this_ctx->xml_ptr );
    this_ctx->xml_ptr = NULL;
  }

  if (this_ctx->load_file)
  {
    FILE_close(this_ctx->load_file);
    this_ctx->load_file = NULL;
  }

}

/**
 *  Different ways of failure :)
 * @param ctx_ptr = Context pointer
 * @param status_text = Text generated from the Global text definnitions
 */
void fail(SFH_ctx_type *ctx_ptr, char *status_text)
{
  if (ctx_ptr->status_text[0] == 0)
  {
    Debug("Fail Error: %s\n", status_text);
    xstrcpy(ctx_ptr->status_text, status_text);
  }

  ctx_ptr->status = SFH_status_Error;
  ctx_ptr->state = SFH_state_done_error;
}


void fail_global_text(SFH_ctx_type *ctx_ptr, char* function, int text_code)
{
  if (ctx_ptr && function)
  {
    char* status_text = GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, text_code);

    if (ctx_ptr->status_text[0] == 0)
    {
      Debug("Fail (%s): %s\n", function, status_text);
      xstrcpy(ctx_ptr->status_text, status_text);
    }

    ctx_ptr->status = SFH_status_Error;
    ctx_ptr->state = SFH_state_done_error;
  }
}



/**
 * Callbacks....
 * @param channel
 * @param status
 * @param status_text
 */
void SFH_TRA_callback( uint8_t channel, TRA_status_enum status, char *status_text)
{
  SFH_ctx_type *ctx_ptr;

  ctx_ptr = get_ctx(channel);
  if (ctx_ptr)
  {
    ctx_ptr->got_tra_callback = true;
    ctx_ptr->tra_status = status;
    if (status != TRA_status_ok)
    {
      SFH_set_error_string(channel, status_text);
    }
  }
}

void SFH_COMM_callback(uint8_t channel)
{
  SFH_ctx_type *ctx_ptr;

  if ((0 < channel) && (channel <= MAX_NUMBER_OF_CHANNELS))
  {
    ctx_ptr = get_ctx(channel);
    if (ctx_ptr)
    {
      ctx_ptr->got_comm_callback = true;
    }
  }
}

void SFH_BOOT_callback(uint8_t id, uint8_t ebl_run, boot_ebl_version_t *p_version)
{
  SFH_ctx_type *ctx_ptr;

  PARAM_NOT_USED(p_version);

  ctx_ptr = get_ctx(id);
  if (ctx_ptr)
  {
    if (ebl_run == BOOT_SYNC)
    {
      TIMER_start_chronom_sec(&ctx_ptr->total_timer);  // Synchronized ... so restart the time measurement to eliminate the user response time.

      if (ctx_ptr->modem_controls != NULL)
      {
        SFH_control_modem_signals(ctx_ptr->channel, ctx_ptr->modem_controls, "^"); /* '^' to signal the action to be inverted. */
      }
    }
    else if (ebl_run == BOOT_START)
    {
      /* Check if the power-on signals need to be set */
      if (ctx_ptr->modem_controls != NULL)
      {
        SFH_control_modem_signals(ctx_ptr->channel, ctx_ptr->modem_controls, "");
      }
    }
    else if (ebl_run == SYS_SW_RUNNING)
    {
      ctx_ptr->target_sw_running_skip_dl = true;
      ctx_ptr->boot_status = ebl_run;
      ctx_ptr->got_boot_callback = true;
    }
    else
    {
      ctx_ptr->boot_status = ebl_run;
      ctx_ptr->got_boot_callback = true;
    }
  }
}

static void clock_the_machine(SFH_ctx_type *act_ctx)
{
  boot_status_t boot_status;
  comm_status_t  comm_status;

  /* Log the state transitions */
    static SFH_state_type sfh_state;
    if (sfh_state != act_ctx->state)
    {
      Debug3("SFH State Change: %d -> %d\n", sfh_state, act_ctx->state);
      sfh_state = act_ctx->state;
    }

  if (GLOBAL_trace_verbosity)
  {
    if (act_ctx->prev_handling_flags != act_ctx->handling_flags)
    {
      act_ctx->prev_handling_flags = act_ctx->handling_flags;
      TR_BEGIN(TR_Sfh, TR_sfh_flags_change,act_ctx->channel);
      TR_32(act_ctx->handling_flags);   /* uint32_t: flags   (bits) */
      TR_END;
    }
    if (act_ctx->prev_state != act_ctx->state)
    {
      act_ctx->prev_state = act_ctx->state;
      TR_BEGIN(TR_Sfh, TR_sfh_state_change,act_ctx->channel);
      TR_8(act_ctx->state);             /* uint8_t:  state   (enum) */
      TR_END;
    }
  }

  switch (act_ctx->state)
  {
    /* ==================================================================== */
    case SFH_state_idle:
    case SFH_state_AT_mode:
    /* ==================================================================== */
    {
      break;
    }

    /* ==================================================================== */
    case SFH_state_await_tra:
    /* ==================================================================== */
    {
      if (act_ctx->got_tra_callback)
      {
        if (act_ctx->tra_status != TRA_status_ok)
        {
          fail (act_ctx,"SFH_state_await_tra");
        }
        else
        {
          act_ctx->state = act_ctx->next_state;
        }
      }
      break;
    }

    /* ==================================================================== */
    case SFH_state_await_comm:
    /* ==================================================================== */
    {
      if (act_ctx->got_comm_callback)
      {
        act_ctx->state = act_ctx->next_state;
      } else {
        // TODO: Process bar during Raw data transfers...
        // Maybe we need to save the "comm offset" in the "act_ctx->cnf_buf" before this call???
        //send_percent(act_ctx->channel,calc_percent((*((uint32_t*)&act_ctx->cnf_buf[1]) - act_ctx->load_address), act_ctx->load_length_total));
      }
      break;
    }

    /* ==================================================================== */
    case SFH_state_await_boot:
    /* ==================================================================== */
    {
      if (act_ctx->got_boot_callback)
      {
        if (!act_ctx->boot_status)
        {
          act_ctx->status = SFH_status_Error;
          act_ctx->state = SFH_state_done_error;
        }
        else
        {
          if (act_ctx->target_sw_running_skip_dl)
          {
            // Boot handler have reported that SW was validated
            // No need to proceed ... skip rest of process.
            act_ctx->state = SFH_state_done_ok;
          }
          else
          {
            act_ctx->state = act_ctx->next_state;
          }
        }
      }
      break;
    }

    /* ==================================================================== */
    case SFH_state_boot:
    /* ==================================================================== */
    {

      /* Call abort to all other modules to ensure a konwn state before boot */
#if 0
      BOOT_abort_process(act_ctx->channel);
      COMM_abort_process(act_ctx->channel);
      PROT_abort_process(act_ctx->channel);
      TRA_abort_process(act_ctx->channel);
#endif
      TIMER_start_chronom_sec(&act_ctx->test_timer);
      TIMER_start_chronom_sec(&act_ctx->total_timer);
      memset((void *)&target_capabilities[act_ctx->channel], 0x00, sizeof(capabilities_t));
      TRA_CapabilitiesChanged(act_ctx->channel);

      /* Reset the status */
      act_ctx->erase_all_done = false;
      act_ctx->status = SFH_status_OK;
      act_ctx->status_text[0] = 0;
      act_ctx->got_boot_callback = false;
      act_ctx->target_sw_running_skip_dl = false;

      COMPORT_prolific_port_reset_cb(act_ctx->channel);
#if !defined(ANDROID) && !defined(LIBUSBHOST)
      if (GLOBAL_usb_device) // && !GLOBAL_comm_technology[act_ctx->channel])
      {
        act_ctx->state = SFH_state_boot_delayed_open;
        TIMER_new_timer(&act_ctx->state_delay_timer, GLOBAL_boot_timeout);
      }
      else
#endif
      {
        if (GLOBAL_pre_boot[act_ctx->channel] == PRE_BOOT_ARMED)
        {
            // We do not have any info about the target or SW so go for default speed.
            COMM_set_baudrate(act_ctx->channel, 115200);
            BOOT_set_EBL_load_speed(act_ctx->channel, 115200);
        }
        else
        {
          if (act_ctx->ref_header.Hardware.Platform == X_GOLD_626) // Only for XMM626
            {
              //Debug("Special X_GOLD_626 Bootup Baud Rate...\n");
              if(GLOBAL_use_alt_boot_speed)
                COMM_set_baudrate(act_ctx->channel,460800);
              else
                COMM_set_baudrate(act_ctx->channel,115200);
            }
            else
            {
              //Debug("Bootup Baud Rate: %d\n", act_ctx->ref_header.Hardware.BootSpeed);
              COMM_set_baudrate(act_ctx->channel, act_ctx->ref_header.Hardware.BootSpeed);
            }

          //Debug("High Baud Rate: %d\n", act_ctx->high_baudrate);
            BOOT_set_EBL_load_speed(act_ctx->channel, act_ctx->high_baudrate);
        }
        boot_status = BOOT_start_boot(act_ctx->channel, &act_ctx->boot_dev_cfg, &act_ctx->boot_ebl_version, SFH_BOOT_callback);
        if (boot_status != boot_status_ok)
        {
          fail_global_text(act_ctx, "SFH_state_boot", SFH_ERR_boot_error); break;
        }

        act_ctx->state = SFH_state_await_boot;
        act_ctx->next_state = SFH_state_boot_check_capabilities;
      }
      break;
    }

    /* ==================================================================== */
    case SFH_state_boot_delayed_open:
    /* ==================================================================== */
    {
      comm_cfg_t comm_cfg;

      if (TIMER_timer_expired(&act_ctx->state_delay_timer))  // GLOBAL_boot_timeout
      {
        fail_global_text(act_ctx, "SFH_state_boot_delayed_open", SFH_ERR_boot_timeout); break;
      }
      strncpy(comm_cfg.comport_name, act_ctx->comm_port_name,MAX_COMPORT_NAME_LENGTH);
      comm_cfg.baudrate = act_ctx->high_baudrate;
      if (COMM_open_port(act_ctx->channel, &comm_cfg) == comm_status_ok)
      {
        //Debug("SFH_state_boot_delayed_open baud rate change to %d\n", act_ctx->ref_header.Hardware.BootSpeed);
        if (GLOBAL_pre_boot[act_ctx->channel] == PRE_BOOT_ARMED)
        {
          (void)COMM_set_baudrate(act_ctx->channel, 115200);
        }
        else
        {
          (void)COMM_set_baudrate(act_ctx->channel, act_ctx->ref_header.Hardware.BootSpeed);
        }
        boot_status = BOOT_start_boot(act_ctx->channel, &act_ctx->boot_dev_cfg, &act_ctx->boot_ebl_version, SFH_BOOT_callback);
        if (boot_status != boot_status_ok)
        {
          fail_global_text(act_ctx, "SFH_state_boot_delayed_open", SFH_ERR_boot_error); break;
        }

        act_ctx->state = SFH_state_await_boot;
        act_ctx->next_state = SFH_state_boot_check_capabilities;
      }
      else
      {
        if (GLOBAL_usb_auto_mode)
        {
          /* When using Usb auto mode USB device should already be found by MMI before getting to this switch case.*/
          fail_global_text(act_ctx, "SFH_state_boot_delayed_open", SFH_ERR_boot_error); break;
        }
      }

      break;
    }


    /* ==================================================================== */
    case SFH_state_boot_check_capabilities:
    /* ==================================================================== */
    {
      if (GLOBAL_pre_boot[act_ctx->channel] == PRE_BOOT_DONE)
      {
        act_ctx->state = SFH_state_done_ok;
      }
      else if (GLOBAL_dut_in_test_mode)
      {
        act_ctx->state = SFH_state_done_ok;
      }
      else
      {
        GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_bootloader_run));
        GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_loader_ver,act_ctx->boot_ebl_version.name
                       ,act_ctx->boot_ebl_version.major,act_ctx->boot_ebl_version.minor));

        GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_boot_mode,act_ctx->boot_ebl_version.boot_mode));
        /* Check capabilities here, modify the list if needed and return it to the target */

      /* Set capabilities form API interface */
      dll_capabilities.skip_erase          = (GLOBAL_erase_mode == IFWD_DL_target_erase_nothing) ? 1 : 0;
      dll_capabilities.skip_wr_pack_crc    = GLOBAL_skip_wr_pack_crc ? 1 : 0;

      //Debug("protocol_version=%d\n", act_ctx->boot_ebl_version.capabilities.protocol_version);
      /* Check if target supports enhanced protocol */
      if (act_ctx->boot_ebl_version.capabilities.protocol_version >= CAP_PROT_VER_ENH)
      {
        if ((act_ctx->boot_ebl_version.major == 0xFFFF) &&
            (act_ctx->boot_ebl_version.minor == 0xFFFF) &&
            ((act_ctx->boot_dev_cfg.ebl_size  == 0x0001AA10) ||
             (act_ctx->boot_dev_cfg.ebl_size  == 0x0001AB40)) &&
             (act_ctx->boot_ebl_version.capabilities.protocol_version == CAP_PROT_VER_ENH))
        {
          act_ctx->boot_ebl_version.capabilities.package_size = CAP_PACK_SIZE_131072;
          act_ctx->used_protocol = PROT_1_32;
          PROT_set_protocol(act_ctx->channel, PROT_1_32);
        }
        else
        if ((act_ctx->boot_ebl_version.major == 0xFFFF) &&
            (act_ctx->boot_ebl_version.minor == 0xFFFF) &&
            (act_ctx->boot_dev_cfg.ebl_size  != 0x0001AA10) &&
            (act_ctx->boot_dev_cfg.ebl_size  != 0x0001AB40) &&
            (act_ctx->boot_ebl_version.capabilities.protocol_version == CAP_PROT_VER_ENH))
        {
          act_ctx->boot_ebl_version.capabilities.package_size = CAP_PACK_SIZE_16384;
          act_ctx->used_protocol = PROT_1_16;
          PROT_set_protocol(act_ctx->channel, PROT_1_16);
        }
        else
        if(act_ctx->boot_ebl_version.capabilities.protocol_version == CAP_PROT_VER_ENH_32BIT)
        {
          act_ctx->used_protocol = PROT_3_32;
          PROT_set_protocol(act_ctx->channel, PROT_2_32);
        }
        else
        {
          act_ctx->used_protocol = PROT_1_16;
          PROT_set_protocol(act_ctx->channel, PROT_1_16);
        }


        /* go through each capability item and accept it only if there is a match between */
        /* what is supported in this code and on the target */
        if (act_ctx->boot_ebl_version.capabilities.package_size > dll_capabilities.package_size)
          act_ctx->boot_ebl_version.capabilities.package_size = dll_capabilities.package_size;

        if (act_ctx->boot_ebl_version.capabilities.skip_wr_pack_cnf &&
            dll_capabilities.skip_wr_pack_cnf &&
            GLOBAL_usb_device &&   // <==  Set if USB port.
            act_ctx->boot_ebl_version.capabilities.crc_method &&
            dll_capabilities.crc_method)
        {
          act_ctx->boot_ebl_version.capabilities.skip_wr_pack_cnf = CAP_SKIP_WR_PACK_CNF;
          act_ctx->boot_ebl_version.capabilities.crc_method = CAP_CRC_RETURNED_BY_SEC_END;
        }
        if (act_ctx->boot_ebl_version.capabilities.crc_type != dll_capabilities.crc_type)
          act_ctx->boot_ebl_version.capabilities.crc_type = CAP_NOT_ENHANCED;

        if (act_ctx->boot_ebl_version.capabilities.compression != dll_capabilities.compression)
          act_ctx->boot_ebl_version.capabilities.compression = CAP_NOT_ENHANCED;

        if (act_ctx->boot_ebl_version.capabilities.protocol_crc != dll_capabilities.protocol_crc)
          act_ctx->boot_ebl_version.capabilities.protocol_crc = CAP_NOT_ENHANCED;

        if (act_ctx->boot_ebl_version.capabilities.skip_erase != dll_capabilities.skip_erase)
          act_ctx->boot_ebl_version.capabilities.skip_erase = CAP_NOT_ENHANCED;

        if (act_ctx->boot_ebl_version.capabilities.skip_wr_pack_crc != dll_capabilities.skip_wr_pack_crc)
          act_ctx->boot_ebl_version.capabilities.skip_wr_pack_crc = CAP_NOT_ENHANCED;

        if (act_ctx->boot_ebl_version.capabilities.erase_sector_verify != dll_capabilities.erase_sector_verify)
          act_ctx->boot_ebl_version.capabilities.erase_sector_verify = CAP_NOT_ENHANCED;

        if ((act_ctx->boot_ebl_version.capabilities.flags & CAP_FLAG_USE_EXT_CAP) != (dll_capabilities.flags & CAP_FLAG_USE_EXT_CAP))
          act_ctx->boot_ebl_version.capabilities.flags &= ~((uint8_t)CAP_FLAG_USE_EXT_CAP);
        // Clear unused bits.
        act_ctx->boot_ebl_version.capabilities.flags &= CAP_FLAG_USE_EXT_CAP;


        act_ctx->boot_ebl_version.reserved_long = 0x00000000;           // Not used so far so clear it.

        if (act_ctx->boot_ebl_version.capabilities.flags & CAP_FLAG_USE_EXT_CAP)
        {
          act_ctx->boot_ebl_version.ext_capabilities_are_handled = 0x01;  // Set from now on at always to signal that unused bits in the ext_capability struct is cleared.


          // Out Of Session capabilities
          if ((act_ctx->boot_ebl_version.ext_capabilities.oos_config & EXT_CAP_OOS_CFG1_GET_HW_ID) != (ext_dll_capabilities.oos_config & EXT_CAP_OOS_CFG1_GET_HW_ID))
            act_ctx->boot_ebl_version.ext_capabilities.oos_config &= ~((uint8_t)EXT_CAP_OOS_CFG1_GET_HW_ID);

          if ((act_ctx->boot_ebl_version.ext_capabilities.oos_config & EXT_CAP_OOS_CFG1_GET_1ST_16) != (ext_dll_capabilities.oos_config & EXT_CAP_OOS_CFG1_GET_1ST_16))
            act_ctx->boot_ebl_version.ext_capabilities.oos_config &= ~((uint8_t)EXT_CAP_OOS_CFG1_GET_1ST_16);

          if ((act_ctx->boot_ebl_version.ext_capabilities.oos_config & EXT_CAP_OOS_CFG1_GET_FLASH_ID) != (ext_dll_capabilities.oos_config & EXT_CAP_OOS_CFG1_GET_FLASH_ID))
            act_ctx->boot_ebl_version.ext_capabilities.oos_config &= ~((uint8_t)EXT_CAP_OOS_CFG1_GET_FLASH_ID);

          if ((act_ctx->boot_ebl_version.ext_capabilities.oos_config & EXT_CAP_OOS_CFG1_ERASE_ALL) != (ext_dll_capabilities.oos_config & EXT_CAP_OOS_CFG1_ERASE_ALL))
            act_ctx->boot_ebl_version.ext_capabilities.oos_config &= ~((uint8_t)EXT_CAP_OOS_CFG1_ERASE_ALL);

          if ((act_ctx->boot_ebl_version.ext_capabilities.oos_config & EXT_CAP_OOS_CFG1_WRITE_XML) != (ext_dll_capabilities.oos_config & EXT_CAP_OOS_CFG1_WRITE_XML))
            act_ctx->boot_ebl_version.ext_capabilities.oos_config &= ~((uint8_t)EXT_CAP_OOS_CFG1_WRITE_XML);

          // Clear unused bits.
          act_ctx->boot_ebl_version.ext_capabilities.oos_config &= (EXT_CAP_OOS_CFG1_GET_HW_ID |
                                                                    EXT_CAP_OOS_CFG1_GET_1ST_16 |
                                                                    EXT_CAP_OOS_CFG1_GET_FLASH_ID |
                                                                    EXT_CAP_OOS_CFG1_ERASE_ALL |
                                                                    EXT_CAP_OOS_CFG1_WRITE_XML);

          // Special USB modes
          // Clear unused bits.
          ext_capabilities_t* ebl_ext_caps = &act_ctx->boot_ebl_version.ext_capabilities;

          if (!(ebl_ext_caps->usb_handling_flags & EXT_CAP_USB_RAW_WR_MODE)
              || !GLOBAL_usb_device)
            ebl_ext_caps->usb_handling_flags &= ~((uint8_t)EXT_CAP_USB_RAW_WR_MODE);

          // Clear unused bits.
          ebl_ext_caps->usb_handling_flags &= EXT_CAP_USB_RAW_WR_MODE;

          // MIPI RAW Mode flag
          //if (!(ebl_ext_caps->mipi_handling_flags & EXT_CAP_MIPI_RAW_1_16))
          //  ebl_ext_caps->mipi_handling_flags &= ~((uint8_t)EXT_CAP_MIPI_RAW_1_16);

          if (COMM_TECH_IS_MIPI(act_ctx->channel) && (ebl_ext_caps->mipi_handling_flags & EXT_CAP_MIPI_RAW_3_32))
          {
            //Debug("\n\t ---> Forcing 3.32 (PacketSize: 2048-> 16384) <--- \n")
            act_ctx->used_protocol = PROT_3_32;
            act_ctx->boot_ebl_version.capabilities.protocol_version = CAP_PROT_VER_ENH_32BIT;
            PROT_set_protocol(act_ctx->channel, PROT_2_32);
          }
          if (!(ebl_ext_caps->mipi_handling_flags & EXT_CAP_MIPI_RAW_3_32))
            ebl_ext_caps->mipi_handling_flags &= ~((uint8_t)EXT_CAP_MIPI_RAW_3_32);

          // Clear unused bits.
          ebl_ext_caps->mipi_handling_flags &= (EXT_CAP_MIPI_RAW_1_16 | EXT_CAP_MIPI_RAW_3_32);

          // Communication formats
          if ((act_ctx->boot_ebl_version.ext_capabilities.comm_formats_1 & EXT_CAP_COMM_FORMAT_USIF) != (ext_dll_capabilities.comm_formats_1 & EXT_CAP_COMM_FORMAT_USIF))
            act_ctx->boot_ebl_version.ext_capabilities.comm_formats_1 &= ~((uint8_t)EXT_CAP_COMM_FORMAT_USIF);

          if ((act_ctx->boot_ebl_version.ext_capabilities.comm_formats_1 & EXT_CAP_COMM_FORMAT_USB) != (ext_dll_capabilities.comm_formats_1 & EXT_CAP_COMM_FORMAT_USB))
            act_ctx->boot_ebl_version.ext_capabilities.comm_formats_1 &= ~((uint8_t)EXT_CAP_COMM_FORMAT_USB);

          if ((act_ctx->boot_ebl_version.ext_capabilities.comm_formats_1 & EXT_CAP_COMM_FORMAT_MIPI) != (ext_dll_capabilities.comm_formats_1 & EXT_CAP_COMM_FORMAT_MIPI))
            act_ctx->boot_ebl_version.ext_capabilities.comm_formats_1 &= ~((uint8_t)EXT_CAP_COMM_FORMAT_MIPI);

          if ((act_ctx->boot_ebl_version.ext_capabilities.comm_formats_1 & EXT_CAP_COMM_FORMAT_SPI) != (ext_dll_capabilities.comm_formats_1 & EXT_CAP_COMM_FORMAT_SPI))
            act_ctx->boot_ebl_version.ext_capabilities.comm_formats_1 &= ~((uint8_t)EXT_CAP_COMM_FORMAT_SPI);

          if ((act_ctx->boot_ebl_version.ext_capabilities.comm_formats_1 & EXT_CAP_COMM_FORMAT_HSIC) != (ext_dll_capabilities.comm_formats_1 & EXT_CAP_COMM_FORMAT_HSIC))
            act_ctx->boot_ebl_version.ext_capabilities.comm_formats_1 &= ~((uint8_t)EXT_CAP_COMM_FORMAT_HSIC);
          // Clear unused bits.
          act_ctx->boot_ebl_version.ext_capabilities.comm_formats_1 &= (EXT_CAP_COMM_FORMAT_USIF |
                                                                        EXT_CAP_COMM_FORMAT_USB |
                                                                        EXT_CAP_COMM_FORMAT_MIPI |
                                                                        EXT_CAP_COMM_FORMAT_SPI |
                                                                        EXT_CAP_COMM_FORMAT_HSIC);

          // Verify modes
          if (((act_ctx->boot_ebl_version.ext_capabilities.verify_options & EXT_CAP_VERIFY_OPT_NO_PRG) != (ext_dll_capabilities.verify_options & EXT_CAP_VERIFY_OPT_NO_PRG)) ||
              (!GLOBAL_precheck_mode)) /* Use option only if set by API */
              act_ctx->boot_ebl_version.ext_capabilities.verify_options &= ~((uint8_t)EXT_CAP_VERIFY_OPT_NO_PRG);
          // Clear unused bits.
          act_ctx->boot_ebl_version.ext_capabilities.verify_options &= EXT_CAP_VERIFY_OPT_NO_PRG;

          // secure modes
          if ((act_ctx->boot_ebl_version.ext_capabilities.sec_handling_flags & EXT_CAP_SEC_OPEN_READ) != (ext_dll_capabilities.sec_handling_flags & EXT_CAP_SEC_OPEN_READ))
              act_ctx->boot_ebl_version.ext_capabilities.sec_handling_flags &= ~((uint8_t)EXT_CAP_SEC_OPEN_READ);
          // Clear unused bits.
          act_ctx->boot_ebl_version.ext_capabilities.sec_handling_flags &= EXT_CAP_SEC_OPEN_READ;

          // Clear unused bytes;
          act_ctx->boot_ebl_version.ext_capabilities.spare_07 = 0x00;
          act_ctx->boot_ebl_version.ext_capabilities.spare_08 = 0x00;
          act_ctx->boot_ebl_version.ext_capabilities.spare_09 = 0x00;
          act_ctx->boot_ebl_version.ext_capabilities.spare_10 = 0x00;
          act_ctx->boot_ebl_version.ext_capabilities.spare_11 = 0x00;
          act_ctx->boot_ebl_version.ext_capabilities.spare_12 = 0x00;

        }
        //GlobalPackSize = DEFAULT_PACK_SIZE;
        GLOBAL_use_pack_size[act_ctx->channel] = DEFAULT_PACK_SIZE;

        // Check here if there are combinations that should cause errors.
        if (GLOBAL_precheck_mode)
        {
          if (act_ctx->boot_ebl_version.ext_capabilities.verify_options & EXT_CAP_VERIFY_OPT_NO_PRG)
          {
            // option is supported so notify that it is being used.
            GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessOutlineUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_use_special_option,"PreCheck"));
          }
          else
          {
            // If Precheck option is set and the EBL does not support this option, give error.
            fail(act_ctx, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_option_not_supported, "PreCheck"));   break;
          }
        }

        act_ctx->got_tra_callback = false;
        if (TRA_receive_mem_data(act_ctx->channel, ReqSetProtConf, act_ctx->cnf_buf, 2,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
        {
          fail_global_text(act_ctx, "SFH_state_boot_check_capabilities", SFH_ERR_setup_confirm); break;
        }

        if (TRA_send_mem_data(act_ctx->channel, ReqSetProtConf, (uint8_t*)&(act_ctx->boot_ebl_version.boot_mode), sizeof(act_ctx->boot_ebl_version)-4,TRA_progress_report_disabled, NULL) != TRA_status_ok)
        {
          fail_global_text(act_ctx, "SFH_state_boot_check_capabilities", SFH_ERR_sending_command); break;
        }

        /* report back what capabilities we are using: */
        show_capabilities(act_ctx->channel, &act_ctx->boot_ebl_version.capabilities);

        act_ctx->state = SFH_state_await_tra;
        act_ctx->next_state = SFH_state_boot_set_high_baud;
      }
      else
      {
        /* Not enhanced support so just go on. */
        act_ctx->state = SFH_state_boot_set_high_baud;
      }
    }
      break;

    }

    /* ==================================================================== */
    case SFH_state_boot_set_high_baud:
    /* ==================================================================== */
    {
      /*   */
      if(act_ctx->boot_ebl_version.capabilities.package_size == 0)
        GLOBAL_use_pack_size[act_ctx->channel] = DEFAULT_PACK_SIZE;
      else
        GLOBAL_use_pack_size[act_ctx->channel] = 1024*act_ctx->boot_ebl_version.capabilities.package_size;

      memcpy((void*)&target_capabilities[act_ctx->channel], (void*)&(act_ctx->boot_ebl_version.capabilities), sizeof(dll_capabilities));
      TRA_CapabilitiesChanged(act_ctx->channel);

      if (COMM_TECH_IS_MIPI(act_ctx->channel))
      {
          act_ctx->state = SFH_state_boot_get_id;
      }
      else
      {
        *((uint32_t*)act_ctx->cmd_buf) = act_ctx->high_baudrate;

        act_ctx->got_tra_callback = false;
        if (TRA_receive_mem_data(act_ctx->channel, CnfBaudChange, act_ctx->cnf_buf, 4,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
        {
          fail_global_text(act_ctx, "SFH_state_boot_set_high_baud", SFH_ERR_setup_confirm); break;
        }

        if (TRA_send_mem_data(act_ctx->channel, CnfBaudChange, act_ctx->cmd_buf, 4,TRA_progress_report_disabled, NULL) != TRA_status_ok)
        {
          fail_global_text(act_ctx, "SFH_state_boot_set_high_baud", SFH_ERR_sending_command); break;
        }

        act_ctx->state = SFH_state_await_tra;
        act_ctx->next_state = SFH_state_boot_set_high_baud_delay;
      }
      break;

    }
    /* ==================================================================== */
    case SFH_state_boot_set_high_baud_delay:
    /* ==================================================================== */
    {
      comm_status = COMM_set_baudrate(act_ctx->channel, act_ctx->high_baudrate);
      if (comm_status == comm_status_set_baudrate_ok)
      {
        GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_boot_high_speed, act_ctx->high_baudrate));

        // Start a timer to allow the target side to do the change also.
        TIMER_new_timer(&act_ctx->state_delay_timer, DELAY_CHANGE_BAUD);  // delay 20mS ... subject to change
        act_ctx->state = SFH_state_boot_get_id;
      }
      else
      {
        fail_global_text(act_ctx, "SFH_state_boot_set_high_baud_delay", SFH_ERR_baudrate_error); break;
      }
      break;
    }
    /* ==================================================================== */
    case SFH_state_boot_get_id:
    /* ==================================================================== */
    {

      if((TIMER_timer_expired(&act_ctx->state_delay_timer)) || (COMM_TECH_IS_MIPI(act_ctx->channel)))// DELAY_CHANGE_BAUD
      {
        GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_boot_get_id));
        /* Set-up the receive path for the command to be send */

        act_ctx->got_tra_callback = false;
        if (TRA_receive_mem_data(act_ctx->channel, ReqFlashId, act_ctx->cnf_buf, 20,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
        {
          fail_global_text(act_ctx, "SFH_state_boot_get_id", SFH_ERR_setup_confirm); break;
        }

        if (TRA_send_mem_data(act_ctx->channel, ReqFlashId, (uint8_t*)&act_ctx->ref_header.Hardware, sizeof(HardwareStructType) ,TRA_progress_report_disabled, NULL) != TRA_status_ok)
        {
          fail_global_text(act_ctx, "SFH_state_boot_get_id", SFH_ERR_sending_command); break;
        }

        act_ctx->state = SFH_state_await_tra;
        act_ctx->next_state = SFH_state_boot_cfi_1;
      }
      break;
    }

    /* ==================================================================== */
    case SFH_state_boot_cfi_1:
    /* ==================================================================== */
    {
      if (act_ctx->boot_dev_cfg.technology == NOR_FLASH)
      {
        GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_boot_cfi_1));
      }
      /* Set-up the receive path for the command to be send */

      *((uint16_t*)act_ctx->cmd_buf) = 0x0000;

      act_ctx->got_tra_callback = false;
      if (TRA_receive_mem_data(act_ctx->channel, ReqCfiInfo_1, act_ctx->cnf_buf, sizeof(flash_cfi_info_typ),TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
      {
        fail_global_text(act_ctx, "SFH_state_boot_cfi_1", SFH_ERR_setup_confirm); break;
      }

      if (TRA_send_mem_data(act_ctx->channel, ReqCfiInfo_1, act_ctx->cmd_buf, 2,TRA_progress_report_disabled, NULL) != TRA_status_ok)
      {
        fail_global_text(act_ctx, "SFH_state_boot_cfi_1", SFH_ERR_sending_command); break;
      }

      act_ctx->state = SFH_state_await_tra;
      act_ctx->next_state = SFH_state_boot_cfi_2;

      break;
    }

    /* ==================================================================== */
    case SFH_state_boot_cfi_2:
    /* ==================================================================== */
    {
      flash_cfi_info_typ *cfi_info_ptr;
      /* Send the CFI info right back to the target */
      memcpy(act_ctx->cmd_buf,act_ctx->cnf_buf,sizeof(flash_cfi_info_typ));

      /* REMEMBER must handle INI stuff here */
      cfi_info_ptr = (flash_cfi_info_typ*)&act_ctx->cmd_buf[0];
      act_ctx->flash_id = (cfi_info_ptr[0]->ID<<16)+cfi_info_ptr[0]->Manufacturer;
      act_ctx->flash_voltage = cfi_info_ptr[0]->Algorithm >> 16; // The voltage indacation is enbedded as the high word in the algorithm

      act_ctx->erase_length  = (*cfi_info_ptr)[0].Size;
      act_ctx->erase_length += (*cfi_info_ptr)[1].Size;
      act_ctx->erase_length += (*cfi_info_ptr)[2].Size;

      if((act_ctx->ref_header.Hardware.Platform != S_GOLD_CHIPSET_V3) &&
        (act_ctx->ref_header.Hardware.Platform != X_GOLD_213) &&
        (act_ctx->ref_header.Hardware.Platform != X_GOLD_626) &&
        (act_ctx->ref_header.Hardware.Platform != X_GOLD_706) &&
        (act_ctx->ref_header.Hardware.Platform != X_GOLD_716) &&
        (act_ctx->ref_header.Hardware.Platform != X_GOLD_726) &&
        (act_ctx->ref_header.Hardware.Platform != X_GOLD_631) &&
        (act_ctx->ref_header.Hardware.Platform != X_GOLD_632) &&
        (act_ctx->ref_header.Hardware.Platform != X_GOLD_633) &&
        (act_ctx->ref_header.Hardware.Platform != X_GOLD_636) &&
        (act_ctx->ref_header.Hardware.Platform != X_GOLD_221) &&
        (act_ctx->ref_header.Hardware.Platform != X_GOLD_222) &&
        (act_ctx->ref_header.Hardware.Platform != X_GOLD_223) &&
        (act_ctx->ref_header.Hardware.Platform != X_GOLD_225) &&
        (act_ctx->ref_header.Hardware.Platform != X_GOLD_215) &&
        (act_ctx->ref_header.Hardware.Platform != X_GOLD_618))
      {
        act_ctx->erase_length += (*cfi_info_ptr)[3].Size;
      }

      GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_flash_id,act_ctx->flash_id));
      if (!FILE_exists(ini_file_name))
      {
        /*    fail(act_ctx, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_not_found, ini_file_name));   break; */
      }
      else
      {
        FlashInitRead(ini_file_name, (FlashInfoStruct*)&act_ctx->cmd_buf[0] );
      }

      if (act_ctx->boot_dev_cfg.technology == NOR_FLASH)
      {
        GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_boot_cfi_2));
      }
      /* Set-up the receive path for the command to be send */

      act_ctx->got_tra_callback = false;
      if (TRA_receive_mem_data(act_ctx->channel, ReqCfiInfo_2, act_ctx->cnf_buf, 2,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
      {
        fail_global_text(act_ctx, "SFH_state_boot_cfi_2", SFH_ERR_setup_confirm); break;
      }

      if (TRA_send_mem_data(act_ctx->channel, ReqCfiInfo_2, act_ctx->cmd_buf, sizeof(flash_cfi_info_typ),TRA_progress_report_disabled, NULL) != TRA_status_ok)
      {
        fail_global_text(act_ctx, "SFH_state_boot_cfi_2", SFH_ERR_sending_command); break;
      }

      act_ctx->state = SFH_state_await_tra;
      act_ctx->next_state = SFH_state_send_nand_control;

      break;
    }
    /* ==================================================================== */
    case SFH_state_send_nand_control:
    /* ==================================================================== */
    {
      /* See if we must sent the NAND control bits to the target */
      if ((act_ctx->boot_dev_cfg.technology == NAND_FLASH) &&
        (GLOBAL_nand_partition_config.Specification != MAC_NOSPECIFICATION) )
      {
        act_ctx->state = SFH_state_await_tra;

        act_ctx->got_tra_callback = false;
        if (TRA_receive_mem_data(act_ctx->channel, ReqSetNandCtrlBitField, act_ctx->cnf_buf, 2,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
        {
          fail_global_text(act_ctx, "SFH_state_send_nand_control", SFH_ERR_setup_confirm); break;
        }

        if (TRA_send_mem_data(act_ctx->channel, ReqSetNandCtrlBitField, (uint8_t *)&GLOBAL_nand_partition_config, sizeof(GLOBAL_nand_partition_config),TRA_progress_report_disabled, NULL) != TRA_status_ok)
        {
          fail_global_text(act_ctx, "SFH_state_send_nand_control", SFH_ERR_sending_command); break;
        }

        act_ctx->state = SFH_state_await_tra;
        act_ctx->next_state = SFH_state_send_xml_oos_ctrl;
      }
      else
      {
        act_ctx->state = SFH_state_send_xml_oos_ctrl;
      }
      break;

    }
    /* ==================================================================== */
  case SFH_state_send_xml_oos_ctrl:
    /* ==================================================================== */
    {

      if ((act_ctx->boot_ebl_version.ext_capabilities.oos_config & EXT_CAP_OOS_CFG1_WRITE_XML) &&
          (act_ctx->xml_ptr && act_ctx->xml_length))
      {
         *(((uint32_t*)act_ctx->cmd_buf)+0) = IFWD_DL_oosc_action_write;
         *(((uint32_t*)act_ctx->cmd_buf)+1) = IFWD_DL_oosc_type_xml_data;
         *(((uint32_t*)act_ctx->cmd_buf)+2) = act_ctx->xml_length;
         *(((uint32_t*)act_ctx->cmd_buf)+3) = 0;

         act_ctx->got_tra_callback = false;

         if (TRA_receive_mem_data(act_ctx->channel, ReqOutOfSessionControl, act_ctx->cnf_buf, 4 ,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
         {
           fail_global_text(act_ctx, "SFH_state_send_xml_oos_ctrl", SFH_ERR_setup_confirm); break;
         }

         if (TRA_send_mem_data(act_ctx->channel, ReqOutOfSessionControl, act_ctx->cmd_buf, 16 ,TRA_progress_report_disabled, NULL) != TRA_status_ok)
         {
           fail_global_text(act_ctx, "SFH_state_send_xml_oos_ctrl", SFH_ERR_sending_command); break;
         }

         act_ctx->state = SFH_state_await_tra;
         act_ctx->next_state = SFH_state_send_xml_oos_data;
      }
      else
      {
         act_ctx->state = SFH_state_change_protocol;
      }

      break;
    }
    /* ==================================================================== */
  case SFH_state_send_xml_oos_data:
    /* ==================================================================== */
    {

      act_ctx->got_tra_callback = false;

      if (TRA_receive_mem_data(act_ctx->channel, ReqOutOfSessionDataWrite, act_ctx->cnf_buf, 4 ,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
      {
        fail_global_text(act_ctx, "SFH_state_send_xml_oos_data", SFH_ERR_setup_confirm); break;
      }

      if (TRA_send_mem_data(act_ctx->channel, ReqOutOfSessionDataWrite, act_ctx->xml_ptr, act_ctx->xml_length ,TRA_progress_report_disabled, NULL) != TRA_status_ok)
      {
        fail_global_text(act_ctx, "SFH_state_send_xml_oos_data", SFH_ERR_sending_command); break;
      }

      act_ctx->state = SFH_state_await_tra;
      act_ctx->next_state = SFH_state_change_protocol;
      break;
    }

    /* ==================================================================== */
    case SFH_state_change_protocol:
    /* ==================================================================== */
    {
      uint32_t prot_change = false;
      uint8_t format = act_ctx->boot_ebl_version.ext_capabilities.comm_formats_1;

      act_ctx->got_tra_callback = false;
      if (strcmp(act_ctx->comm_port_name,act_ctx->ctrl_port_name))   // Only if the two comm port names are different
      {
        if (format & EXT_CAP_COMM_FORMAT_SPI)
          prot_change = EXT_CAP_COMM_FORMAT_SPI;

        if (format & EXT_CAP_COMM_FORMAT_MIPI)
          prot_change = EXT_CAP_COMM_FORMAT_MIPI;

        // TODO: EXT_CAP_COMM_FORMAT_USB ?
        // TODO: EXT_CAP_COMM_FORMAT_HSIC ?
      }

      if (prot_change)
      {
        // Keep using existing Protocol but change comm to the new one
        Debug("Changing Communication protocol from %s to %s..\n", act_ctx->comm_port_name,act_ctx->ctrl_port_name);
        *((uint32_t*)act_ctx->cmd_buf) = 0x0000 | (((uint16_t)prot_change) << 16);

       if (TRA_receive_mem_data(act_ctx->channel, ReqProtocolChange, act_ctx->cnf_buf, 4,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
       {
         fail_global_text(act_ctx, "SFH_state_change_protocol", SFH_ERR_setup_confirm); break;
       }

       if (TRA_send_mem_data(act_ctx->channel, ReqProtocolChange, act_ctx->cmd_buf, 4,TRA_progress_report_disabled, NULL) != TRA_status_ok)
       {
         fail_global_text(act_ctx, "SFH_state_change_protocol", SFH_ERR_sending_command); break;
       }

        // Go wait for the response
        act_ctx->state = SFH_state_await_tra;
        act_ctx->next_state = SFH_state_change_protocol_doit;
      }
      else
      {
        // No protocol change so just go to last boot sequence state
        act_ctx->state = SFH_state_boot_ok;
      }

      break;
    }

    /* ==================================================================== */
    case SFH_state_change_protocol_doit:
    /* ==================================================================== */
    {
      comm_cfg_t port_param;

      // Got the response from target so do the actual change.
      // Close the comport
      COMM_close_comport(act_ctx->channel);
      COMM_delete_channel(act_ctx->channel);

      // Open the comport ... or another comm-driver
      snprintf(port_param.comport_name, sizeof(port_param.comport_name)-1, "%s", act_ctx->ctrl_port_name);  // the ctrl_port_name may be different from comm_port_name ... so use it instead

      COMM_open_port(act_ctx->channel, &port_param);
      COMM_add_channel(act_ctx->channel);

      GLOBAL_comm_technology[act_ctx->channel]++;  // Count up from PRE_OPEN to OPENED.

      if (COMM_TECH_IS_MIPI(act_ctx->channel))
        GLOBAL_comm_align_bytes[act_ctx->channel] = 4; // When using MIPI the protocol must ensure read/write is done with 4 bytes alignment.

      // Start a timer to allow the target side to do the change also.
      TIMER_new_timer(&act_ctx->state_delay_timer, DELAY_CHANGE_PROTOCOL);

      act_ctx->state = SFH_state_change_protocol_wait;
      break;
    }

    /* ==================================================================== */
    case SFH_state_change_protocol_wait:
    /* ==================================================================== */
    {
      if(TIMER_timer_expired(&act_ctx->state_delay_timer)) // DELAY_CHANGE_PROTOCOL
      {
        act_ctx->state = SFH_state_boot_ok;
      }
      break;
    }
    /* ==================================================================== */
    case SFH_state_boot_ok:
    /* ==================================================================== */
    {

      GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_boot_ok));

      /* Free the PSI data if any */
      if (act_ctx->psi_data_ptr)
      {
        MEM_free( act_ctx->psi_data_ptr );
        act_ctx->psi_data_ptr = NULL;
      }

      /* Free the EBL data if any */
      if (act_ctx->ebl_data_ptr)
      {
        MEM_free( act_ctx->ebl_data_ptr );
        act_ctx->ebl_data_ptr = NULL;
      }

      /* Free the XML data if any */
      if (act_ctx->xml_ptr)
      {
        MEM_free( act_ctx->xml_ptr = NULL );
        act_ctx->xml_ptr = NULL;
      }

      act_ctx->state = SFH_state_done_ok;

      break;
    }



    /* ==================================================================== */
    case SFH_state_load:
    /* ==================================================================== */
    {
      Debug("Entering time critical section.\n");
      GLOBAL_is_in_time_critical_section = true;

      /* Reset the status */
      act_ctx->status = SFH_status_OK;
      act_ctx->status_text[0] = 0;
      TIMER_start_chronom_sec(&act_ctx->test_timer);
      act_ctx->load_length_total = act_ctx->load_length;

      if (act_ctx->target_sw_running_skip_dl)
      {
        // Boot handler have reported that SW was validated
        // No need to proceed ... skip rest of process.
        act_ctx->state = SFH_state_done_ok;
        GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessDetailUpdated, "System SW is validated and running. Skipping download.");
      }
      else if (act_ctx->handling_flags & HANDLE_RMW_SEQ)
      {
        act_ctx->state = SFH_state_load_send_sec;  /* If ReadModifyWrite then skip all the dyn erased stuff */
      }
      else
      {
        if (act_ctx->boot_dev_cfg.technology == NOR_FLASH)   // No erase on NAND Systems
        {
          act_ctx->state = SFH_state_erase_dyn_eep;
        }
        else if (act_ctx->boot_dev_cfg.technology == NAND_FLASH)   // No erase on NAND Systems, except erase all.
        {
          act_ctx->state = SFH_state_erase_all_nand_sec_start;
        }
        else
        {
          act_ctx->state = SFH_state_load_send_sec;  /* Must be FlashLessso skip erase stuff */
        }
      }
      break;
    }

    /* ==================================================================== */
    case SFH_state_erase_dyn_eep:
    /* ==================================================================== */
    {
      if ((act_ctx->handling_flags & HANDLE_DYN_EEP_ERASE) && (GLOBAL_cond_dyn_eep_erase))
      {
        act_ctx->got_tra_callback = false;
        if (TRA_receive_mem_data(act_ctx->channel,  ReqSecStart, act_ctx->cnf_buf, 2,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
        {
          fail_global_text(act_ctx, "SFH_state_erase_dyn_eep", SFH_ERR_setup_confirm); break;
        }

        if (TRA_send_mem_data(act_ctx->channel, ReqSecStart, (uint8_t*)&(act_ctx->load_header.Security), sizeof(SecurityStructType),TRA_progress_report_disabled, NULL) != TRA_status_ok)
        {
          fail_global_text(act_ctx, "SFH_state_erase_dyn_eep", SFH_ERR_sending_command); break;
        }

        act_ctx->state = SFH_state_await_tra;
        act_ctx->next_state = SFH_state_erase_dyn_eep_ver_sec;
      }
      else
      {
        act_ctx->state = SFH_state_erase_dyn_eep_sec_start;
      }

      break;
    }


    /* ==================================================================== */
    case SFH_state_erase_dyn_eep_ver_sec:
    /* ==================================================================== */
    {
      uint32_t offset = 0;

#ifdef INCLUDE_UNUSED_FX
      if (act_ctx->boot_dev_cfg.technology == NAND_FLASH)
      {
        /* For NAND system there may be an offset to be added to EepRevAddrInSw as the Security structure
        is saved in flash before the code. Furthermore there may be a alignment. */

        PartitionStructType nand_partition;
        uint32_t file_offset;
        uint32_t alignment;
        uint32_t options = 0;
        uint32_t i;

        /* First get the partition table */
        PRGH_GetElementIndex_File(act_ctx->load_file,NANDPARTITION_ELEM_TYPE, GENERIC_UID, &file_offset);
        FILE_seek(act_ctx->load_file, file_offset+sizeof(ElementHeaderStructType));
        FILE_read(act_ctx->load_file, (uint8_t*)&(nand_partition), sizeof(PartitionStructType));
        for (i=0; i< MAX_PARTITIONS; i++)
        {
          if (nand_partition.Partition[i].ID == NAND_ID_CODE_IMAGE)
          {
            options = nand_partition.Partition[i].Options;
            break;
          }
        }
        /* Calculate the offset */
        alignment = 0;

        if (options & NAND_OPT_LOADMAP)
        {
          offset = sizeof(SecurityStructType);
        }

        if (options & NAND_ALIGNMENT_MASK)
        {
          alignment = 1 << ((options & NAND_ALIGNMENT_MASK) >> 28);
        }

        if (alignment > offset)
        {
          offset = alignment;
        }

      } /* If NAND */
#endif
      *((uint32_t*)act_ctx->cmd_buf) = act_ctx->load_header.MemoryMap.EepRevAddrInSw + offset;

      act_ctx->state = SFH_state_await_tra;

      act_ctx->got_tra_callback = false;
      if (TRA_receive_mem_data(act_ctx->channel, ReqFlashSetAddress, act_ctx->cnf_buf, 6,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
      {
        fail_global_text(act_ctx, "SFH_state_erase_dyn_eep_ver_sec", SFH_ERR_setup_confirm); break;
      }

      if (TRA_send_mem_data(act_ctx->channel, ReqFlashSetAddress, act_ctx->cmd_buf, 4,TRA_progress_report_disabled, NULL) != TRA_status_ok)
      {
        fail_global_text(act_ctx, "SFH_state_erase_dyn_eep_ver_sec", SFH_ERR_sending_command); break;
      }

      act_ctx->state = SFH_state_await_tra;
      act_ctx->next_state = SFH_state_erase_dyn_eep_req_ver;
      break;
    }
    /* ==================================================================== */
    case SFH_state_erase_dyn_eep_req_ver:
    /* ==================================================================== */
    {
      *((uint16_t*)act_ctx->cmd_buf) = 4;

      act_ctx->got_tra_callback = false;
      if (TRA_receive_mem_data(act_ctx->channel, ReqFlashReadBlock, act_ctx->cnf_buf, 4 ,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
      {
        fail_global_text(act_ctx, "SFH_state_erase_dyn_eep_req_ver", SFH_ERR_setup_confirm); break;
      }

      if (TRA_send_mem_data(act_ctx->channel, ReqFlashReadBlock, act_ctx->cmd_buf, 2 ,TRA_progress_report_disabled, NULL) != TRA_status_ok)
      {
        fail_global_text(act_ctx, "SFH_state_erase_dyn_eep_req_ver", SFH_ERR_sending_command); break;
      }

      act_ctx->state = SFH_state_await_tra;
      act_ctx->next_state = SFH_state_erase_dyn_eep_ver_sec_end;
      break;
    }
    /* ==================================================================== */
    case SFH_state_erase_dyn_eep_ver_sec_end:
    /* ==================================================================== */
    {
      uint8_t exp_cnf_len;

      /* Save target version while sending the sec-end package */
      act_ctx->temp2_u16 = *((uint16_t*)act_ctx->cnf_buf);

      *((uint16_t*)act_ctx->cmd_buf) = 0;

      act_ctx->got_tra_callback = false;
      if (target_capabilities[act_ctx->channel].crc_method == CAP_CRC_RETURNED_BY_SEC_END)
      {
        exp_cnf_len = 4;
      }
      else
      {
        exp_cnf_len = 2;
      }
      if (TRA_receive_mem_data(act_ctx->channel, ReqSecEnd, act_ctx->cnf_buf, exp_cnf_len,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
      {
        fail_global_text(act_ctx, "SFH_state_erase_dyn_eep_ver_sec_end", SFH_ERR_setup_confirm); break;
      }

      if (TRA_send_mem_data(act_ctx->channel, ReqSecEnd, act_ctx->cmd_buf, 2,TRA_progress_report_disabled, NULL) != TRA_status_ok)
      {
        fail_global_text(act_ctx, "SFH_state_erase_dyn_eep_ver_sec_end", SFH_ERR_sending_command); break;
      }

      act_ctx->state = SFH_state_await_tra;
      act_ctx->next_state = SFH_state_erase_dyn_eep_ver_chk;
      break;
    }
    /* ==================================================================== */
    case SFH_state_erase_dyn_eep_ver_chk:
    /* ==================================================================== */
    {
      uint16_t target_ver;

      target_ver = act_ctx->temp2_u16;
      /* Now get the eep version from the FLS file */
      if (act_ctx->new_eeprom_ver != target_ver)
      {
        act_ctx->state = SFH_state_erase_dyn_eep_sec_start;
      }
      else
      {
        send_percent(act_ctx->channel,calc_percent(0, 100));
        send_percent(act_ctx->channel,calc_percent(100, 100));

        act_ctx->state = SFH_state_load_send_sec;
      }
      break;
    }

    /* ==================================================================== */
    case SFH_state_erase_dyn_eep_sec_start:
    /* ==================================================================== */
    {
      SecurityStructType dyn_eep_sec_pack;

      memcpy((uint8_t*)&(dyn_eep_sec_pack),(uint8_t*)&(act_ctx->load_header.Security), sizeof(SecurityStructType));

      if ((act_ctx->boot_dev_cfg.technology == NOR_FLASH) && (act_ctx->handling_flags & HANDLE_ERASE_ALL))
      {
        // Force loadmap to cover the whole area to be erased.
        dyn_eep_sec_pack.LoadMap[0].StartAddr   = 0;
        dyn_eep_sec_pack.LoadMap[0].TotalLength = 0xFFFFFFFF;
        dyn_eep_sec_pack.LoadMap[0].UsedLength  = 0xFFFFFFFF;
        dyn_eep_sec_pack.LoadMap[0].ImageFlags  = 0;
      }
      else
      {
        uint32_t i;
        for (i=0; i<MAX_MAP_ENTRIES; i++)
        {
          if (act_ctx->load_header.MemoryMap.Entry[i].Class == DYN_EEP_CLASS)
          {
            dyn_eep_sec_pack.LoadMap[0].StartAddr   = act_ctx->load_header.MemoryMap.Entry[i].Start;
            dyn_eep_sec_pack.LoadMap[0].UsedLength  = act_ctx->load_header.MemoryMap.Entry[i].Start + act_ctx->load_header.MemoryMap.Entry[i].Length - 2;
            dyn_eep_sec_pack.LoadMap[0].TotalLength = act_ctx->load_header.MemoryMap.Entry[i].Start + act_ctx->load_header.MemoryMap.Entry[i].Length - 2;
            dyn_eep_sec_pack.LoadMap[0].ImageFlags  = 0;
            break;
          }
        }
      }

      if (((act_ctx->boot_dev_cfg.technology == NOR_FLASH)
          && !(act_ctx->handling_flags & HANDLE_ERASE_ALL)) ||
          ((act_ctx->boot_ebl_version.ext_capabilities.oos_config & EXT_CAP_OOS_CFG1_ERASE_ALL)
          && (act_ctx->handling_flags & HANDLE_ERASE_ALL)))
      {
        act_ctx->state = SFH_state_erase_dyn_eep_doit;
      }
      else
      {
        act_ctx->got_tra_callback = false;
        if (TRA_receive_mem_data(act_ctx->channel,  ReqSecStart, act_ctx->cnf_buf, 2,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
        {
          fail_global_text(act_ctx, "SFH_state_erase_dyn_eep_sec_start", SFH_ERR_setup_confirm); break;
        }

        if (TRA_send_mem_data(act_ctx->channel, ReqSecStart, (uint8_t*)&(dyn_eep_sec_pack), sizeof(SecurityStructType),TRA_progress_report_disabled, NULL) != TRA_status_ok)
        {
          fail_global_text(act_ctx, "SFH_state_erase_dyn_eep_sec_start", SFH_ERR_sending_command); break;
        }

        act_ctx->state = SFH_state_await_tra;
        act_ctx->next_state = SFH_state_erase_dyn_eep_doit;
      }
      break;
    }

    /* ==================================================================== */
    case SFH_state_erase_dyn_eep_doit:
    /* ==================================================================== */
    {
      /* ********************************************* */
      uint32_t i;

      if ((act_ctx->handling_flags & HANDLE_DYN_EEP_ERASE) || (act_ctx->handling_flags & HANDLE_ERASE_ALL))
      {
        if (act_ctx->handling_flags & HANDLE_DYN_EEP_ERASE)
        {
          GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_erase_dyn_eep));
          for (i=0; i<MAX_MAP_ENTRIES; i++)
          {
            if (act_ctx->load_header.MemoryMap.Entry[i].Class == DYN_EEP_CLASS)
            {
              act_ctx->erase_address = act_ctx->load_header.MemoryMap.Entry[i].Start;
              act_ctx->erase_length  = act_ctx->load_header.MemoryMap.Entry[i].Length;
              break;
            }
          }
        }
        else
        {
          if (act_ctx->boot_dev_cfg.technology == NOR_FLASH)
          {
            GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_erase_all));
            switch(act_ctx->load_header.Hardware.Platform)
            {
              case E_GOLD_CHIPSET_V2:
              case E_GOLD_LITE_CHIPSET:
              case E_GOLD_CHIPSET_V3:
              case E_GOLD_RADIO_V1:
              case E_GOLD_VOICE_V1:
              case X_GOLD_102:
              case X_GOLD_110:
              {
                act_ctx->erase_address = 0x00800000;
                break;
              }

              case S_GOLD_CHIPSET_V1:
              case S_GOLD_LITE_CHIPSET:
              case S_GOLD_CHIPSET_V2:
              case S_GOLD_RADIO_V1:
              {
                act_ctx->erase_address = 0xA0000000;
                break;
              }

              case S_GOLD_CHIPSET_V3:
              {
                act_ctx->erase_address = 0x20000000;
                break;
              }

              case X_GOLD_618:
              case X_GOLD_626:
              case X_GOLD_706:
              case X_GOLD_716:
              case X_GOLD_726:
              case X_GOLD_631:
              case X_GOLD_632:
              case X_GOLD_633:
              case X_GOLD_636:
              case X_GOLD_221:
              case X_GOLD_222:
              case X_GOLD_223:
              case X_GOLD_225:
              case X_GOLD_215:
              case X_GOLD_213:
              {
                act_ctx->erase_address = 0x40000000;
                break;
              }
            }

            /* Correct the length in case of odd system size .... must be Sibley flash then so add 4 * 8K EFA sectors */
            if (act_ctx->load_header.Hardware.SystemSize & 0x00FFFFFF)
            {
              /* act_ctx->erase_length  += (0x2000 * 4)  causes timeout :-(  */;
            }
          }
          else
          {
            act_ctx->erase_address = 0x00000000;
            if((act_ctx->handling_flags & HANDLE_ERASE_ALL_BAD) == HANDLE_ERASE_ALL_BAD)
            {
              GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_erase_all_bad));
              act_ctx->erase_length  = 0xFFFFFFFE;
            }
            else
            {
              GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_erase_all));
              act_ctx->erase_length  = 0xFFFFFFFF;
            }
          }
          act_ctx->erase_all_done = true;
        }

        *((uint32_t*)&act_ctx->cmd_buf[0]) = act_ctx->erase_address;
        if(act_ctx->erase_length >= 0xFFFFFFFE) /* don't know why there is -2, but that messes erase up for NAND, so skip it for lengths the EBL checks */
          *((uint32_t*)&act_ctx->cmd_buf[4]) = act_ctx->erase_length;
        else
          *((uint32_t*)&act_ctx->cmd_buf[4]) = act_ctx->erase_address + act_ctx->erase_length - 2;

        act_ctx->got_tra_callback = false;

        if((act_ctx->handling_flags & HANDLE_ERASE_ALL) && (act_ctx->boot_ebl_version.ext_capabilities.oos_config & EXT_CAP_OOS_CFG1_ERASE_ALL))
        {
           /* Erase All on specific project on XMM6180 platforms use the Out Of Session command to initiate the erase */
           *((uint32_t*)&act_ctx->cmd_buf[0])  = IFWD_DL_oosc_action_erase;
           *((uint32_t*)&act_ctx->cmd_buf[4])  = IFWD_DL_oosc_type_all_flash;
           *((uint32_t*)&act_ctx->cmd_buf[8])  = act_ctx->erase_length;
           *((uint32_t*)&act_ctx->cmd_buf[12]) = 0x00000000;
           if (TRA_receive_mem_data(act_ctx->channel,  ReqOutOfSessionControl, act_ctx->cnf_buf, 4,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
           {
             fail(act_ctx, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_setup_confirm) );   break;
           }

           if (TRA_send_mem_data(act_ctx->channel, ReqOutOfSessionControl, act_ctx->cmd_buf, 16,TRA_progress_report_disabled, NULL) != TRA_status_ok)
           {
             fail(act_ctx,  GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_sending_command) );  break;
           }
        }
        else
        {
          if (TRA_receive_mem_data(act_ctx->channel,  ReqFlashEraseStart, act_ctx->cnf_buf, 2,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
          {
            fail_global_text(act_ctx, "SFH_state_erase_dyn_eep_doit", SFH_ERR_setup_confirm); break;
          }

          if (TRA_send_mem_data(act_ctx->channel, ReqFlashEraseStart, act_ctx->cmd_buf, 8,TRA_progress_report_disabled, NULL) != TRA_status_ok)
          {
            fail_global_text(act_ctx, "SFH_state_erase_dyn_eep_doit", SFH_ERR_sending_command); break;
          }
        }
        send_percent(act_ctx->channel,calc_percent(0, act_ctx->erase_length));

        act_ctx->state = SFH_state_await_tra;
        act_ctx->next_state = SFH_state_erase_dyn_eep_chk;
      }
      else
      {
        act_ctx->state = SFH_state_erase_dyn_eep_sec_end;
      }
      break;
    }

    /* ==================================================================== */
  case SFH_state_erase_dyn_eep_chk:
    /* ==================================================================== */
    {
      *((uint16_t*)&act_ctx->cmd_buf[0]) = 0;

      act_ctx->got_tra_callback = false;
      if (TRA_receive_mem_data(act_ctx->channel,  ReqFlashEraseCheck, act_ctx->cnf_buf, 6,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
      {
        fail_global_text(act_ctx, "SFH_state_erase_dyn_eep_chk", SFH_ERR_setup_confirm); break;
      }

      if (TRA_send_mem_data(act_ctx->channel, ReqFlashEraseCheck, act_ctx->cmd_buf, 2,TRA_progress_report_disabled, NULL) != TRA_status_ok)
      {
        fail_global_text(act_ctx, "SFH_state_erase_dyn_eep_chk", SFH_ERR_sending_command); break;
      }

      act_ctx->state = SFH_state_await_tra;
      act_ctx->next_state = SFH_state_erase_dyn_eep_chk_cnf;
      break;
    }

    /* ==================================================================== */
  case SFH_state_erase_dyn_eep_chk_cnf:
    /* ==================================================================== */
    {
      if (act_ctx->cnf_buf[0] == 0)
      {
        send_percent(act_ctx->channel,calc_percent((*((uint32_t*)&act_ctx->cnf_buf[1]) - act_ctx->erase_address), act_ctx->erase_length));
        act_ctx->state = SFH_state_erase_dyn_eep_chk;
      }
      else
      {
        send_percent(act_ctx->channel,calc_percent(100, 100));
        if ((act_ctx->handling_flags & HANDLE_ERASE_ALL) && (act_ctx->handling_flags & HANDLE_STOP_AFTER_ERASE))
        {
          GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_abort_process));

          act_ctx->state = SFH_state_done_ok;

        }
        else
        {
          act_ctx->state = SFH_state_erase_dyn_eep_sec_end;
        }
      }
      break;
    }
    /* ==================================================================== */
  case SFH_state_erase_dyn_eep_sec_end:
    /* ==================================================================== */
    {
      if (((act_ctx->boot_dev_cfg.technology == NOR_FLASH)
          && !(act_ctx->handling_flags & HANDLE_ERASE_ALL)) ||
          ((act_ctx->boot_ebl_version.ext_capabilities.oos_config & EXT_CAP_OOS_CFG1_ERASE_ALL)
          && (act_ctx->handling_flags & HANDLE_ERASE_ALL)))
      {
        act_ctx->state = SFH_state_load_send_sec;
      }
      else
      {
        uint8_t exp_cnf_len;
        *((uint16_t*)act_ctx->cmd_buf) = 0;


        act_ctx->got_tra_callback = false;
        if (target_capabilities[act_ctx->channel].crc_method == CAP_CRC_RETURNED_BY_SEC_END)
        {
          exp_cnf_len = 4;
        }
        else
        {
          exp_cnf_len = 2;
        }
        if (TRA_receive_mem_data(act_ctx->channel, ReqSecEnd, act_ctx->cnf_buf, exp_cnf_len,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
        {
          fail_global_text(act_ctx, "SFH_state_erase_dyn_eep_sec_end", SFH_ERR_setup_confirm); break;
        }

        if (TRA_send_mem_data(act_ctx->channel, ReqSecEnd, act_ctx->cmd_buf, 2,TRA_progress_report_disabled, NULL) != TRA_status_ok)
        {
          fail_global_text(act_ctx, "SFH_state_erase_dyn_eep_sec_end", SFH_ERR_sending_command); break;
        }
        act_ctx->state = SFH_state_await_tra;
        act_ctx->next_state = SFH_state_load_send_sec;
      }
      break;
    }


    /* ********************************************* */
    /* ==================================================================== */
    case SFH_state_erase_all_nand_sec_start:
    /* ==================================================================== */
    {
      if (act_ctx->handling_flags & HANDLE_ERASE_ALL)
      {
        if (act_ctx->boot_ebl_version.ext_capabilities.oos_config & EXT_CAP_OOS_CFG1_ERASE_ALL)
        {
          act_ctx->state = SFH_state_erase_all_nand;
        }
        else
        {
          act_ctx->got_tra_callback = false;
          if (TRA_receive_mem_data(act_ctx->channel,  ReqSecStart, act_ctx->cnf_buf, 2,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
          {
            fail(act_ctx, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_setup_confirm) );   break;
          }

          if (TRA_send_mem_data(act_ctx->channel, ReqSecStart, (uint8_t*)&(act_ctx->load_header.Security), sizeof(SecurityStructType),TRA_progress_report_disabled, NULL) != TRA_status_ok)
          {
            fail(act_ctx,  GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_sending_command) );  break;
          }

          act_ctx->state = SFH_state_await_tra;
          act_ctx->next_state = SFH_state_erase_all_nand;
        }
      }
      else
      {
        act_ctx->state = SFH_state_load_send_sec;
      }
      break;
    }
    /* ==================================================================== */
    case SFH_state_erase_all_nand:
    /* ==================================================================== */
    {
      /* ********************************************* */

      GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_erase_all));
      {
        act_ctx->erase_address = 0x00000000;
        if((act_ctx->handling_flags & HANDLE_ERASE_ALL_BAD) == HANDLE_ERASE_ALL_BAD)
          act_ctx->erase_length  = 0xFFFFFFFE;
        else
          act_ctx->erase_length  = 0xFFFFFFFF;
      }
      act_ctx->erase_all_done = true;

      *((uint32_t*)&act_ctx->cmd_buf[0]) = act_ctx->erase_address;
      if(act_ctx->erase_length >= 0xFFFFFFFE) /* don't know why there is -2, but that messes erase up for NAND, so skip it for lengths the EBL checks */
        *((uint32_t*)&act_ctx->cmd_buf[4]) = act_ctx->erase_length;
      else
        *((uint32_t*)&act_ctx->cmd_buf[4]) = act_ctx->erase_address + act_ctx->erase_length - 2;

      act_ctx->got_tra_callback = false;

      if((act_ctx->handling_flags & HANDLE_ERASE_ALL) && (act_ctx->boot_ebl_version.ext_capabilities.oos_config & EXT_CAP_OOS_CFG1_ERASE_ALL))
      {
         /* Erase All on specific project on XMM6180 platforms use the Out Of Session command to initiate the erase */
         *((uint32_t*)&act_ctx->cmd_buf[0])  = IFWD_DL_oosc_action_erase;
         *((uint32_t*)&act_ctx->cmd_buf[4])  = IFWD_DL_oosc_type_all_flash;
         *((uint32_t*)&act_ctx->cmd_buf[8])  = act_ctx->erase_length;
         *((uint32_t*)&act_ctx->cmd_buf[12]) = 0x00000000;
         if (TRA_receive_mem_data(act_ctx->channel,  ReqOutOfSessionControl, act_ctx->cnf_buf, 4,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
         {
           fail(act_ctx, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_setup_confirm) );   break;
         }

         if (TRA_send_mem_data(act_ctx->channel, ReqOutOfSessionControl, act_ctx->cmd_buf, 16,TRA_progress_report_disabled, NULL) != TRA_status_ok)
         {
           fail(act_ctx,  GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_sending_command) );  break;
         }
      }
      else
      {

         if (TRA_receive_mem_data(act_ctx->channel,  ReqFlashEraseStart, act_ctx->cnf_buf, 2,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
         {
           fail(act_ctx, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_setup_confirm) );   break;
         }

         if (TRA_send_mem_data(act_ctx->channel, ReqFlashEraseStart, act_ctx->cmd_buf, 8,TRA_progress_report_disabled, NULL) != TRA_status_ok)
         {
           fail(act_ctx,  GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_sending_command) );  break;
         }
      }
      send_percent(act_ctx->channel,calc_percent(0, act_ctx->erase_length));

      act_ctx->state = SFH_state_await_tra;
      act_ctx->next_state = SFH_state_erase_all_nand_chk;
      break;
    }

    /* ==================================================================== */
  case SFH_state_erase_all_nand_chk:
    /* ==================================================================== */
    {
      *((uint16_t*)&act_ctx->cmd_buf[0]) = 0;

      act_ctx->got_tra_callback = false;
      if (TRA_receive_mem_data(act_ctx->channel,  ReqFlashEraseCheck, act_ctx->cnf_buf, 6,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
      {
        fail(act_ctx, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_setup_confirm) );   break;
      }

      if (TRA_send_mem_data(act_ctx->channel, ReqFlashEraseCheck, act_ctx->cmd_buf, 2,TRA_progress_report_disabled, NULL) != TRA_status_ok)
      {
        fail(act_ctx,  GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_sending_command) );  break;
      }

      act_ctx->state = SFH_state_await_tra;
      act_ctx->next_state = SFH_state_erase_all_nand_chk_cnf;
      break;
    }

    /* ==================================================================== */
  case SFH_state_erase_all_nand_chk_cnf:
    /* ==================================================================== */
    {
      if (act_ctx->cnf_buf[0] == 0)
      {
        send_percent(act_ctx->channel,calc_percent((*((uint32_t*)&act_ctx->cnf_buf[1]) - act_ctx->erase_address), act_ctx->erase_length));
        act_ctx->state = SFH_state_erase_all_nand_chk;
      }
      else
      {
        send_percent(act_ctx->channel,calc_percent(100, 100));
        if ((act_ctx->handling_flags & HANDLE_ERASE_ALL) && (act_ctx->handling_flags & HANDLE_STOP_AFTER_ERASE))
        {
          GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_abort_process));

          act_ctx->state = SFH_state_done_ok;

        }
        else
        {
          if (act_ctx->boot_ebl_version.ext_capabilities.oos_config & EXT_CAP_OOS_CFG1_ERASE_ALL)
          {
            act_ctx->state = SFH_state_load_send_sec;
          }
          else
          {
            act_ctx->state = SFH_state_erase_all_nand_sec_end;
          }
        }
      }
      break;
    }

    /* ==================================================================== */
    case SFH_state_erase_all_nand_sec_end:
    /* ==================================================================== */
    {
      uint8_t exp_cnf_len;
      *((uint16_t*)act_ctx->cmd_buf) = 0;


      act_ctx->got_tra_callback = false;
      if (target_capabilities[act_ctx->channel].crc_method == CAP_CRC_RETURNED_BY_SEC_END)
      {
        exp_cnf_len = 4;
      }
      else
      {
        exp_cnf_len = 2;
      }
      if (TRA_receive_mem_data(act_ctx->channel, ReqSecEnd, act_ctx->cnf_buf, exp_cnf_len,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
      {
        fail(act_ctx, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_setup_confirm) );   break;
      }

      if (TRA_send_mem_data(act_ctx->channel, ReqSecEnd, act_ctx->cmd_buf, 2,TRA_progress_report_disabled, NULL) != TRA_status_ok)
      {
        fail(act_ctx,  GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_sending_command) );  break;
      }
      act_ctx->state = SFH_state_await_tra;
      act_ctx->next_state = SFH_state_load_send_sec;
      break;
    }

    /* ********************************************* */

    /* ==================================================================== */
  case SFH_state_load_send_sec:
    /* ==================================================================== */
    {
      TRA_CalcChecksum(act_ctx->channel, (act_ctx->handling_flags & HANDLE_USE_CALC_CRC) != 0); /* Tell TRA if it should calc checksum */
      if (act_ctx->handling_flags & HANDLE_RMW_SEQ)
      {
        act_ctx->state = SFH_state_load_loop_loadmap_start;  /* Do not send the sec pack here when doing read-modify-write */
      }
      else
      {
        /* Send the SEC-Pack to the target */
        GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_load_send_sec));

        act_ctx->got_tra_callback = false;
        if (TRA_receive_mem_data(act_ctx->channel,  ReqSecStart, act_ctx->cnf_buf, 2,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
        {
          fail_global_text(act_ctx, "SFH_state_load_send_sec", SFH_ERR_setup_confirm); break;
        }

        if (TRA_send_mem_data(act_ctx->channel, ReqSecStart, (uint8_t*)&(act_ctx->load_header.Security), sizeof(SecurityStructType),TRA_progress_report_disabled, NULL) != TRA_status_ok)
        {
          fail_global_text(act_ctx, "SFH_state_load_send_sec", SFH_ERR_sending_command); break;
        }

        act_ctx->target_checksum = 0;
        act_ctx->calc_checksum = 0;
        act_ctx->state = SFH_state_await_tra;
        act_ctx->next_state = SFH_state_load_send_sec_rsp;
      }
      break;
    }

    /* ==================================================================== */
  case SFH_state_load_send_sec_rsp:
    /* ==================================================================== */
    {
      if (act_ctx->cnf_buf[0] != 1)
      {
        /* Secure pack has returned NAK */
      }
      act_ctx->state = SFH_state_load_loop_loadmap_start;
      break;
    }

    /* ==================================================================== */
  case SFH_state_load_loop_loadmap_start:
    /* ==================================================================== */
    {
      if (act_ctx->handling_flags & HANDLE_LOADMAP)
      {
        /* Loadmap is used */
        /* Setup the load address and length accordingly */
        act_ctx->handling_flags &= ~((uint32_t) HANDLE_ERASE_REGION);  /* Clear the flag .... if needed it is set further down in this state */
        if (act_ctx->load_header.Security.LoadMap[act_ctx->loadmap_index].UsedLength != 0)
        {


          if (act_ctx->handling_flags & HANDLE_RMW_SEQ)
          {
            GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_load_region, act_ctx->loadmap_index));
            /* In case of read-modify-write do not touch the parameters */
          }
          else if (act_ctx->handling_flags & HANDLE_BIN)
          {
#if 0
            GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_load_region, act_ctx->loadmap_index));
            act_ctx->load_address     = act_ctx->load_header.Security.LoadMap[act_ctx->loadmap_index].StartAddr;
            act_ctx->load_length      = act_ctx->load_header.Security.LoadMap[act_ctx->loadmap_index].UsedLength;
            act_ctx->load_length_total = act_ctx->load_header.Security.LoadMap[act_ctx->loadmap_index].TotalLength;
            if (act_ctx->loadmap_index)
            {
              act_ctx->load_file_offset = act_ctx->load_file_offset + (act_ctx->load_header.Security.LoadMap[act_ctx->loadmap_index].StartAddr - act_ctx->load_header.Security.LoadMap[act_ctx->loadmap_index-1].StartAddr);
            }
#endif
          }
          else /* Normal FLS, EEP and CUST  download */
          {

            uint32_t file_offset;
            uint32_t debug_offset;

            GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_load_region, act_ctx->loadmap_index));
            debug_offset = SFH_GetDebugHeaderOffset_file(act_ctx->load_file);
            PRGH_SetFileOffset(debug_offset);

            PRGH_GetElementIndex_File(act_ctx->load_file,DOWNLOADDATA_ELEM_TYPE, act_ctx->load_uid, &file_offset);
            FILE_seek(act_ctx->load_file, file_offset+sizeof(ElementHeaderStructType));
            FILE_read(act_ctx->load_file, (uint8_t*)&(act_ctx->download_data_struct), sizeof(DownloadDataStructType));

            while ((uint8_t)act_ctx->download_data_struct.LoadMapIndex != act_ctx->loadmap_index)
            {
              ElementHeaderStructType eh;
              PRGH_GetNextElementIndex_File(act_ctx->load_file, &file_offset);
              FILE_seek(act_ctx->load_file, file_offset);
              FILE_read(act_ctx->load_file, (uint8_t*)&(eh), sizeof(ElementHeaderStructType));
              if (eh.Type == DOWNLOADDATA_ELEM_TYPE && eh.UID == act_ctx->load_uid)
              {
                FILE_seek(act_ctx->load_file, file_offset+sizeof(ElementHeaderStructType));
                FILE_read(act_ctx->load_file, (uint8_t*)&(act_ctx->download_data_struct), sizeof(DownloadDataStructType));
              }
            }
            act_ctx->load_address     = act_ctx->load_header.Security.LoadMap[act_ctx->loadmap_index].StartAddr;
            act_ctx->load_length      = act_ctx->load_header.Security.LoadMap[act_ctx->loadmap_index].UsedLength;
            act_ctx->load_length_total = act_ctx->load_header.Security.LoadMap[act_ctx->loadmap_index].TotalLength;
            act_ctx->load_file_offset = act_ctx->download_data_struct.DataOffset+debug_offset;
            act_ctx->exp_checksum     ^= (uint16_t)act_ctx->download_data_struct.CRC;

            // Check NVM images (ONLY in FLASHLESS files)
            if (act_ctx->boot_dev_cfg.technology == FLASH_LESS &&
               (act_ctx->load_header.Security.LoadMap[act_ctx->loadmap_index].ImageFlags & 0x40000000))
            {
              // Handle check for NVM images UsedLength and TotalLength must be the same.
              if (act_ctx->load_length != act_ctx->load_length_total)
              {
                GLOBAL_callback(act_ctx->channel,IFWD_DL_Error, "File format rule violation - NVM Region total and used-size is not equal");
                act_ctx->status = SFH_status_Error;
                act_ctx->state = SFH_state_done_error;
                break;
              }
              // Handle check for NVM files modulo 4 byte image length
              if (act_ctx->load_length % 4)
              {
                GLOBAL_callback(act_ctx->channel,IFWD_DL_Error, "File format rule violation - NVM Region size is not modulo 4 bytes");
                act_ctx->status = SFH_status_Error;
                act_ctx->state = SFH_state_done_error;
                break;
              }
            }
          }

          act_ctx->state = SFH_state_load_start_erase;
        }
        else  // In case of used-length = 0, it may be that the region should be erased. (Flags in the loadmap)
        {
          if (GLOBAL_use_sec_pack_erase_flag &&
              act_ctx->load_header.Security.LoadMap[act_ctx->loadmap_index].ImageFlags & LOAD_MAP_ERASE_REGION_FLAG)
          {
            act_ctx->handling_flags |= HANDLE_ERASE_REGION;

            GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_load_region, act_ctx->loadmap_index));

            act_ctx->load_address     = act_ctx->load_header.Security.LoadMap[act_ctx->loadmap_index].StartAddr;
            act_ctx->load_length      = act_ctx->load_header.Security.LoadMap[act_ctx->loadmap_index].UsedLength;
            act_ctx->load_length_total = act_ctx->load_header.Security.LoadMap[act_ctx->loadmap_index].TotalLength;

            act_ctx->state = SFH_state_load_start_erase;
          }
          else // The flags for load region erase is not set so skip this region cmpletely.
          {
            act_ctx->state = SFH_state_load_loop_loadmap_end;
          }
        }
      }
      else
      {
        act_ctx->state = SFH_state_load_start_erase;
      }
      break;
    }

    /* ==================================================================== */
  case SFH_state_load_start_erase:
    /* ==================================================================== */
    {
      char erase_style_str[128];
      Debug("SFH_state_load_start_erase (EraseDone: %d, Tech: %X != %X)\n", act_ctx->erase_all_done, act_ctx->boot_dev_cfg.technology, FLASH_LESS);

      if ((!act_ctx->erase_all_done) && (act_ctx->boot_dev_cfg.technology != FLASH_LESS)) /* Only erase if the whole flash is not already erased or not flashless*/
      {
        /* When using the USB Raw transfer mode the flash must be erased first .. so force Pre-erase mode */
        if ((GLOBAL_usb_device && act_ctx->boot_ebl_version.ext_capabilities.usb_handling_flags & EXT_CAP_USB_RAW_WR_MODE)
        || (COMM_TECH_IS_MIPI(act_ctx->channel) && act_ctx->boot_ebl_version.ext_capabilities.mipi_handling_flags & EXT_CAP_MIPI_RAW_3_32))
        {
          act_ctx->handling_flags |= HANDLE_PREERASE;
        }

        if (act_ctx->handling_flags & HANDLE_ERASE_REGION)
        {
          snprintf(erase_style_str, 128,"Erase, Total length");
        }
        else if (act_ctx->handling_flags & HANDLE_ERASE_TOTAL_LEN)
        {
          if (act_ctx->handling_flags & HANDLE_PREERASE)
          {
            snprintf(erase_style_str, 128,"Pre-erase, Total-length");
          }
          else
          {
            snprintf(erase_style_str, 128,"Erase while writing, Total-length");
          }
        }
        else
        {
          if (act_ctx->handling_flags & HANDLE_PREERASE)
          {
            snprintf(erase_style_str,128,"Pre-erase, Used-length");
          }
          else
          {
            snprintf(erase_style_str,128,"Erase while writing, Used-length");
          }
        }

        GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_erase,erase_style_str));

        *((uint32_t*)&act_ctx->cmd_buf[0]) = act_ctx->load_address;
        if ((act_ctx->handling_flags & HANDLE_ERASE_REGION) ||
            (act_ctx->handling_flags & HANDLE_ERASE_TOTAL_LEN))
        {
          *((uint32_t*)&act_ctx->cmd_buf[4]) = act_ctx->load_address + act_ctx->load_length_total-2;
          if (((act_ctx->handling_flags & HANDLE_BIN) == 0) && (act_ctx->boot_dev_cfg.technology == NAND_FLASH))
          {
            send_percent(act_ctx->channel,calc_percent(0, act_ctx->load_length_total));
          }
        }
        else
        {
          *((uint32_t*)&act_ctx->cmd_buf[4]) = act_ctx->load_address + act_ctx->load_length-2;
          if (((act_ctx->handling_flags & HANDLE_BIN) == 0) && (act_ctx->boot_dev_cfg.technology == NAND_FLASH))
          {
            send_percent(act_ctx->channel,calc_percent(0, act_ctx->load_length));
          }
        }
        // NJK_DEBUG
#if 0
        {
          char debug[200];
          snprintf(debug, 200,"Start erase %08x - %08x",act_ctx->load_address,*((uint32_t*)&act_ctx->cmd_buf[4]));
          GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessDetailUpdated, debug);
        }
#endif
        act_ctx->got_tra_callback = false;
        if (TRA_receive_mem_data(act_ctx->channel,  ReqFlashEraseStart, act_ctx->cnf_buf, 2,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
        {
          fail_global_text(act_ctx, "SFH_state_load_start_erase", SFH_ERR_setup_confirm); break;
        }

        if (TRA_send_mem_data(act_ctx->channel, ReqFlashEraseStart, act_ctx->cmd_buf, 8,TRA_progress_report_disabled, NULL) != TRA_status_ok)
        {
          fail_global_text(act_ctx, "SFH_state_load_start_erase", SFH_ERR_sending_command); break;
        }

        act_ctx->state = SFH_state_await_tra;
        act_ctx->next_state = SFH_state_load_pre_erase;
      }
      else
      {
        act_ctx->state = SFH_state_load_send_address;
      }
      break;
    }

    /* ==================================================================== */
  case SFH_state_load_pre_erase:
    /* ==================================================================== */
    {
      if (act_ctx->handling_flags & HANDLE_PREERASE)
      {

        // NJK_DEBUG
#if 0
        {
          char debug[200];
          sprintf(debug,"Check erase ");
          GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessDetailUpdated, debug);
        }
#endif

        *((uint16_t*)&act_ctx->cmd_buf[0]) = 0;

        act_ctx->got_tra_callback = false;
        if (TRA_receive_mem_data(act_ctx->channel,  ReqFlashEraseCheck, act_ctx->cnf_buf, 6,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
        {
          fail_global_text(act_ctx, "SFH_state_load_pre_erase", SFH_ERR_setup_confirm); break;
        }

        if (TRA_send_mem_data(act_ctx->channel, ReqFlashEraseCheck, act_ctx->cmd_buf, 2,TRA_progress_report_disabled, NULL) != TRA_status_ok)
        {
          fail_global_text(act_ctx, "SFH_state_load_pre_erase", SFH_ERR_sending_command); break;
        }

        act_ctx->state = SFH_state_await_tra;
        act_ctx->next_state = SFH_state_load_pre_erase_cnf;
      }
      else
      {
        act_ctx->state = SFH_state_load_send_address;
      }
      break;
    }

    /* ==================================================================== */
  case SFH_state_load_pre_erase_cnf:
    /* ==================================================================== */
    {
      if (act_ctx->cnf_buf[0] == 0)
      {
        if (((act_ctx->handling_flags & HANDLE_BIN) == 0) && (act_ctx->boot_dev_cfg.technology == NAND_FLASH))
        {
          if ((act_ctx->handling_flags & HANDLE_ERASE_REGION) ||
              (act_ctx->handling_flags & HANDLE_ERASE_TOTAL_LEN))
          {
            send_percent(act_ctx->channel,calc_percent((*((uint32_t*)&act_ctx->cnf_buf[1]) - act_ctx->load_address), act_ctx->load_length_total));
          }
          else
          {
            send_percent(act_ctx->channel,calc_percent((*((uint32_t*)&act_ctx->cnf_buf[1]) - act_ctx->load_address), act_ctx->load_length));
          }
        }
        TIMER_new_timer(&act_ctx->state_delay_timer, DELAY_ERASE_CHECK);
        act_ctx->state = SFH_state_load_pre_erase_delay; /* wait before going to SFH_state_load_send_address */
      }
      else
      {
        if (((act_ctx->handling_flags & HANDLE_BIN) == 0) && (act_ctx->boot_dev_cfg.technology == NAND_FLASH))
        {
          send_percent(act_ctx->channel,calc_percent(act_ctx->load_length_total, act_ctx->load_length_total));
        }
        act_ctx->state = SFH_state_load_send_address;
      }
      break;
    }

    /* ==================================================================== */
  case SFH_state_load_pre_erase_delay:
    /* ==================================================================== */
    {
      if (TIMER_timer_expired(&act_ctx->state_delay_timer)) /* Time to poll again? (DELAY_ERASE_CHECK) */
        act_ctx->state = SFH_state_load_pre_erase;
      break;
    }

    /* ==================================================================== */
  case SFH_state_load_send_address:
    /* ==================================================================== */
    {
      if ((act_ctx->handling_flags & HANDLE_ERASE_REGION) ||
          (act_ctx->handling_flags & HANDLE_STOP_AFTER_ERASE))
      {
        act_ctx->state = SFH_state_erase_chk;
      }
      else
      {
        *((uint32_t*)act_ctx->cmd_buf) = act_ctx->load_address;
        // NJK_DEBUG
#if 0
        {
          char debug[200];
          sprintf(debug,"Send addr %08x ",act_ctx->load_address);
          GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessDetailUpdated, debug);
        }
#endif

        act_ctx->state = SFH_state_await_tra;

        act_ctx->got_tra_callback = false;
        if (TRA_receive_mem_data(act_ctx->channel, ReqFlashSetAddress, act_ctx->cnf_buf, 6,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
        {
          fail_global_text(act_ctx, "SFH_state_load_send_address", SFH_ERR_setup_confirm); break;
        }

        if (TRA_send_mem_data(act_ctx->channel, ReqFlashSetAddress, act_ctx->cmd_buf, 4,TRA_progress_report_disabled, NULL) != TRA_status_ok)
        {
          fail_global_text(act_ctx, "SFH_state_load_send_address", SFH_ERR_sending_command); break;
        }

        act_ctx->state = SFH_state_await_tra;
        act_ctx->next_state = SFH_state_load_send_mbn;
      }
      break;
    }
    /* ==================================================================== */
  case SFH_state_load_send_mbn:
    /* ==================================================================== */
    {
      if (act_ctx->load_file == NULL)
      {
        act_ctx->load_file = FILE_open(act_ctx->load_file_name, FILE_open_mode_read);
      }
      if (act_ctx->handling_flags & HANDLE_BIN_SEND_MBN)
      {
        uint32_t bin_length;
        uint32_t mbn_offset;
        uint32_t mbn_length;

        bin_length = FILE_size(act_ctx->load_file);

        FILE_seek(act_ctx->load_file, bin_length-4);
        FILE_read(act_ctx->load_file, (uint8_t*)&mbn_offset, 4);
        mbn_length = bin_length-8 - mbn_offset;

        if (mbn_length)
        {
          /* have mbn data */
          act_ctx->got_tra_callback = false;
          if (TRA_receive_mem_data(act_ctx->channel, ReqFlashWriteBlock, act_ctx->cnf_buf, 6,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
          {
            fail_global_text(act_ctx, "SFH_state_load_send_address", SFH_ERR_setup_confirm); break;
          }

          if (TRA_send_file_data(act_ctx->channel, ReqFlashWriteBlock, act_ctx->load_file_name, mbn_offset, mbn_length ,TRA_progress_report_disabled, NULL) != TRA_status_ok)
          {
            fail_global_text(act_ctx, "SFH_state_load_send_address", SFH_ERR_sending_command); break;
          }

          act_ctx->state = SFH_state_await_tra;
          act_ctx->next_state = SFH_state_load_modify_usb_and_send_1st;
        }
        else
        {
          /* mbn_ofset points to the location of the ebl_offset value ==> no mbn data */
          act_ctx->state = SFH_state_load_modify_usb_and_send_1st;
        }
      }
      else
      {
        act_ctx->state = SFH_state_load_modify_usb_and_send_1st;
      }

      break;
    }
    /* ==================================================================== */
  case SFH_state_load_modify_usb_and_send_1st:
    /* ==================================================================== */
    {
      if (act_ctx->handling_flags & HANDLE_USB)
      {
        /* Check if the value, holding the target start-mode in the static eeprom, is within the first protocol pack */
        act_ctx->temp_u16 = 0x000c;
        if ((uint32_t)act_ctx->temp_u16 < GLOBAL_use_pack_size[act_ctx->channel])
        {
          /* Open the file and read one block */
          act_ctx->load_file = FILE_open(act_ctx->load_file_name, FILE_open_mode_read);
          //##          FILE_seek(act_ctx->load_file, act_ctx->load_header.HeaderLength);
          FILE_read(act_ctx->load_file, act_ctx->cmd_buf, GLOBAL_use_pack_size[act_ctx->channel]);

          /* Modify the  the data as needed */
          //##          *((uint32_t*)&act_ctx->cmd_buf[0x000c]) = act_ctx->load_header.MemoryMap.AlternativeStartAddr;

          /* modify the checksum */
          //##          act_ctx->load_header.FlashCRC ^= (act_ctx->load_header.MemoryMap.AlternativeStartAddr) & 0xFFFF;
          //##          act_ctx->load_header.FlashCRC ^= (act_ctx->load_header.MemoryMap.AlternativeStartAddr) >> 16;

          /* Modify the address, length and offset for the rest of the download */
          act_ctx->load_length      -= GLOBAL_use_pack_size[act_ctx->channel];
          act_ctx->load_file_offset += GLOBAL_use_pack_size[act_ctx->channel];

          /* Send the block and move on to download of the rest */
          act_ctx->got_tra_callback = false;
          if (TRA_receive_mem_data(act_ctx->channel, ReqFlashWriteBlock, act_ctx->cnf_buf, 6,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
          {
            fail_global_text(act_ctx, "SFH_state_load_modify_usb_and_send_1st", SFH_ERR_setup_confirm); break;
          }
          if (TRA_send_mem_data(act_ctx->channel, ReqFlashWriteBlock, act_ctx->cmd_buf, GLOBAL_use_pack_size[act_ctx->channel],TRA_progress_report_disabled, NULL) != TRA_status_ok)
          {
            fail_global_text(act_ctx, "SFH_state_load_modify_usb_and_send_1st", SFH_ERR_sending_command); break;
          }

          act_ctx->state = SFH_state_await_tra;
          act_ctx->next_state = SFH_state_load_modify_eep_and_send_1st;

        }
      }
      else
      {
        act_ctx->state = SFH_state_load_modify_eep_and_send_1st;
      }
      break;
    }

    /* ==================================================================== */
  case SFH_state_load_modify_eep_and_send_1st:
    /* ==================================================================== */
    {
      if (act_ctx->handling_flags & HANDLE_EEP)
      {
        uint32_t i;
        uint32_t eep_start_mode_addr = 0;
        uint32_t eep_start_addr = 0;
        uint32_t eep_size = 0;
        uint32_t file_offset;
        uint32_t debug_offset;
        uint32_t eep_offset = 0;

        for (i=0; i<MAX_MAP_ENTRIES; i++)
        {
          if (act_ctx->load_header.MemoryMap.Entry[i].Class == STAT_EEP_CLASS)
          {
            eep_start_mode_addr = act_ctx->load_header.MemoryMap.EepStartModeAddr;
            eep_start_addr      = act_ctx->load_header.MemoryMap.Entry[i].Start;
            eep_size            = act_ctx->load_header.MemoryMap.Entry[i].Length;
            eep_offset = eep_start_mode_addr - eep_start_addr;
            break;
          }
        }
        debug_offset = SFH_GetDebugHeaderOffset_file(act_ctx->load_file);
        PRGH_SetFileOffset(debug_offset);

        PRGH_GetElementIndex_File(act_ctx->load_file, DOWNLOADDATA_ELEM_TYPE, act_ctx->load_uid, &file_offset);
        FILE_seek(act_ctx->load_file, file_offset+sizeof(ElementHeaderStructType));
        FILE_read(act_ctx->load_file, (uint8_t*)&(act_ctx->download_data_struct), sizeof(DownloadDataStructType));

        /* Check if the value, holding the target start-mode in the static eeprom, is within the first protocol pack */
        if (( eep_start_addr <= eep_start_mode_addr) &&
          ( eep_start_mode_addr <= eep_start_addr + eep_size))
          /* Check address for startup mode (Test/Normal mode) is ok. Should already be done by MakePrg. */
        {
          if (eep_size <= SFH_PACK_SIZE)
          {
            FILE_seek(act_ctx->load_file, act_ctx->download_data_struct.DataOffset+debug_offset);
            FILE_read(act_ctx->load_file, act_ctx->cmd_buf, eep_size);

            if(!GLOBAL_ban_eep_mode_writes)
            {
            /* Modify the  the data as needed */
            if (act_ctx->handling_flags & HANDLE_EEP_TEST_MODE)
            {
              /* Update the expected checksum */
              act_ctx->exp_checksum ^= *((uint16_t*)&act_ctx->cmd_buf[eep_offset]);
              act_ctx->exp_checksum ^= (uint16_t)act_ctx->load_header.MemoryMap.TestModeValue;
              *((uint16_t*)&act_ctx->cmd_buf[eep_offset]) = (uint16_t)act_ctx->load_header.MemoryMap.TestModeValue;
            }
            else
            {
              /* Update the expected checksum */
              act_ctx->exp_checksum ^= *((uint16_t*)&act_ctx->cmd_buf[eep_offset]);
              act_ctx->exp_checksum ^= (uint16_t)act_ctx->load_header.MemoryMap.NormalModeValue;
              *((uint16_t*)&act_ctx->cmd_buf[eep_offset]) = (uint16_t)act_ctx->load_header.MemoryMap.NormalModeValue;
              }
            }

            /* Modify the address, length and offset for the rest of the download */
            act_ctx->load_length      -= GLOBAL_use_pack_size[act_ctx->channel];
            act_ctx->load_file_offset += GLOBAL_use_pack_size[act_ctx->channel];

            /* Send the block and move on to download of the rest */
            act_ctx->got_tra_callback = false;
            if (TRA_receive_mem_data(act_ctx->channel, ReqFlashWriteBlock, act_ctx->cnf_buf, 6,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
            {
              fail_global_text(act_ctx, "SFH_state_load_modify_eep_and_send_1st", SFH_ERR_setup_confirm); break;
            }

            if (TRA_send_mem_data(act_ctx->channel, ReqFlashWriteBlock, act_ctx->cmd_buf, act_ctx->load_header.Security.LoadMap[0].UsedLength,TRA_progress_report_disabled, NULL) != TRA_status_ok)
            {
              fail_global_text(act_ctx, "SFH_state_load_modify_eep_and_send_1st", SFH_ERR_sending_command); break;
            }

            TIMER_start_chronom_millisec(&act_ctx->bandwidth_meas_timer);
            act_ctx->state = SFH_state_await_tra;
            act_ctx->next_state = SFH_state_load_loop_loadmap_end;
          }
          else
          { /* There is not memory enough to read eeprom file */
            fail_global_text(act_ctx, "SFH_state_load_modify_eep_and_send_1st", SFH_ERR_not_enough_mem_to_read_eep); break;
          }
        }
        else
        { /* address for startup mode (Test/Normal mode) outside eeprom memory address rande. */
          fail_global_text(act_ctx, "SFH_state_load_modify_eep_and_send_1st", SFH_ERR_startup_address_error); break;
        }
      }
      else
      {
        act_ctx->state = SFH_state_load_send_data;
      }
      break;
    }

    /* ==================================================================== */
  case SFH_state_load_send_data:
    /* ==================================================================== */
    {
      TIMER_start_chronom_millisec(&act_ctx->bandwidth_meas_timer);
      act_ctx->bw_meas_index = act_ctx->loadmap_index;

      /* Handle odd length */
      if (act_ctx->load_length & 0x1)
      {
        uint8_t temp;
        FILE_seek(act_ctx->load_file, act_ctx->load_file_offset+act_ctx->load_length);
        FILE_read(act_ctx->load_file, (uint8_t*)&(temp), 1);
        act_ctx->exp_checksum ^= ((uint16_t)temp<<8);
        act_ctx->load_length++;
      }

      if ((GLOBAL_usb_device && act_ctx->boot_ebl_version.ext_capabilities.usb_handling_flags & EXT_CAP_USB_RAW_WR_MODE)
       || (COMM_TECH_IS_MIPI(act_ctx->channel) && act_ctx->boot_ebl_version.ext_capabilities.mipi_handling_flags & EXT_CAP_MIPI_RAW_3_32))
      {
        char* txt = COMM_TECH_IS_MIPI(act_ctx->channel) ? "MIPI RAW mode" : "USB RAW mode";
        GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_load_send_data,txt));

        act_ctx->raw_remaining_bytes = act_ctx->load_length;
        act_ctx->raw_chunk_size = COMM_TECH_IS_MIPI(act_ctx->channel) ? MIPI_CHUNK_SIZE : USB_CHUNK_SIZE;

        Debug(" -> RAW Chunk Size: %d\n", act_ctx->raw_chunk_size);
        act_ctx->raw_data_ptr = MEM_malloc(act_ctx->raw_chunk_size + sizeof(uint32_t));
        if (!act_ctx->raw_data_ptr)
        {
          fail(act_ctx, "Could not allocate raw data memory" );
          break;
        }

        act_ctx->last_percent = 1; // To be sure that the '0' below is sent;
        send_percent(act_ctx->channel,0);

        act_ctx->state = SFH_state_load_send_data_raw1;
      }
      else
      {
        GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_load_send_data, "Normal mode"));
        // NJK_DEBUG
#if 1   /* Switch to disable data download  for debug purpose */
      act_ctx->got_tra_callback = false;
      if (TRA_receive_mem_data(act_ctx->channel, ReqFlashWriteBlock, act_ctx->cnf_buf, 6,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
      {
        fail_global_text(act_ctx, "SFH_state_load_send_data", SFH_ERR_setup_confirm); break;
      }

      if (act_ctx->handling_flags & HANDLE_DNLOAD_FROM_MEM)
      {
        if ((act_ctx->handling_flags & HANDLE_RMW_SEQ) != HANDLE_RMW_SEQ)
        {
          if (TRA_send_mem_data(act_ctx->channel, ReqFlashWriteBlock, act_ctx->load_data_ptr, act_ctx->load_length ,TRA_progress_report_enabled, NULL) != TRA_status_ok)
          {
            fail_global_text(act_ctx, "SFH_state_load_send_data", SFH_ERR_sending_command); break;
          }
          send_percent(act_ctx->channel,calc_percent(0, act_ctx->load_length));
        }
        else
        {
          if (TRA_send_mem_data(act_ctx->channel, ReqFlashWriteBlock, act_ctx->load_data_ptr, act_ctx->load_length ,TRA_progress_report_disabled, NULL) != TRA_status_ok)
          {
            fail_global_text(act_ctx, "SFH_state_load_send_data", SFH_ERR_sending_command); break;
          }

        }
      }
      else
      {
        if ((act_ctx->handling_flags & HANDLE_RMW_SEQ) != HANDLE_RMW_SEQ)
        {
          if (TRA_send_file_data(act_ctx->channel, ReqFlashWriteBlock, act_ctx->load_file_name, act_ctx->load_file_offset, act_ctx->load_length ,TRA_progress_report_enabled, NULL) != TRA_status_ok)
          {
            fail_global_text(act_ctx, "SFH_state_load_send_data", SFH_ERR_sending_command); break;
          }
          send_percent(act_ctx->channel,calc_percent(0, act_ctx->load_length));
        }
        else
        {
          if (TRA_send_file_data(act_ctx->channel, ReqFlashWriteBlock, act_ctx->load_file_name, act_ctx->load_file_offset, act_ctx->load_length ,TRA_progress_report_disabled, NULL) != TRA_status_ok)
          {
            fail_global_text(act_ctx, "SFH_state_load_send_data", SFH_ERR_sending_command); break;
          }
        }
      }
      act_ctx->state = SFH_state_await_tra;
      act_ctx->next_state = SFH_state_erase_chk;
#else
      act_ctx->state = SFH_state_erase_chk;
#endif
       }
      break;
    }
    /* ==================================================================== */
  case SFH_state_load_send_data_raw1:
    /* ==================================================================== */
    {
      Debug("SFH_state_load_send_data_raw1> Send RAW data 1 (ReqFlashWriteImageRaw)\n");
      //TODO: => GLOBAL_callback(act_ctx->channel, DEBUG_SfhDetail, "Send RAW data 1\n");

      if (act_ctx->raw_remaining_bytes > act_ctx->raw_chunk_size)
      {
        act_ctx->raw_load_length = act_ctx->raw_chunk_size;
      }
      else
      {
        act_ctx->raw_load_length = act_ctx->raw_remaining_bytes;
      }

      act_ctx->raw_remaining_bytes -= act_ctx->raw_load_length;

      /* Handle Raw transfer mode */
      *((uint32_t*)act_ctx->cmd_buf) = act_ctx->raw_load_length;
      act_ctx->got_tra_callback = false;

      // Do not set-up to receive here as the response will be delayed till after the full image has been transfered
      // Send the Raw data package .. with payload uint32_t length of image.
      if (TRA_send_mem_data(act_ctx->channel, ReqFlashWriteImageRaw, act_ctx->cmd_buf, 4,TRA_progress_report_disabled, NULL) != TRA_status_ok)
      {
        fail(act_ctx,  GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_sending_command) );  break;
      }

      /* MIPI adds a header to the raw data... */
      if (COMM_TECH_IS_MIPI(act_ctx->channel))
      {
        memcpy(act_ctx->raw_data_ptr, (char*)&act_ctx->raw_load_length, sizeof(uint32_t));
        act_ctx->raw_data_ptr += sizeof(uint32_t);
      }

      FILE_seek(act_ctx->load_file, act_ctx->load_file_offset);
      FILE_read(act_ctx->load_file, act_ctx->raw_data_ptr, act_ctx->raw_load_length);

      act_ctx->load_file_offset += act_ctx->raw_load_length;

      if (COMM_TECH_IS_MIPI(act_ctx->channel))
      {
        act_ctx->raw_load_length += sizeof(uint32_t);
        act_ctx->raw_data_ptr -= sizeof(uint32_t);
      }

      act_ctx->state = SFH_state_load_send_data_raw2;
      break;
    }
    /* ==================================================================== */
  case SFH_state_load_send_data_raw2:
    /* ==================================================================== */
    {
         comm_status_t temp_comm_status;
         char comm_status_text[128];
         uint32_t bytes_sent;

         if (GLOBAL_UseCOMPortEmulation && !GLOBAL_force_full_emulation)  // In case of Emulation mode ... do not send data here as they are not being traced and does not apear in the module test cases.
         {
           act_ctx->got_comm_callback = false;  // Wait for the comm callback first
           act_ctx->state = SFH_state_load_send_data_raw_cnf1;
         }
         else
         {
           // Wait until the comm handler have sent the command.
           // Function TRA_send_mem_data abowe call all the way down to the COMM_handler, which set the state, before returning.
           // So it should be safe to wait here
           temp_comm_status = COMM_get_status(act_ctx->channel, comm_status_text);
           if (temp_comm_status == comm_status_write_finished)
           {
             Debug("SFH_state_load_send_data_raw2> Send RAW data 2 (%d bytes)...\n", act_ctx->raw_load_length);

             COMM_send_buffer(act_ctx->channel,
                              (char*)act_ctx->raw_data_ptr,
                              act_ctx->raw_load_length,
                              &bytes_sent,
                              SFH_COMM_callback);

             act_ctx->got_comm_callback = false;  // Wait for the comm callback first
             act_ctx->state = SFH_state_await_comm;  /* Wait for teh comm layer to confirm the transmission of the raw data */
             act_ctx->next_state = SFH_state_load_send_data_raw_cnf1;
           }
         }
       break;
    }
    /* ==================================================================== */
  case SFH_state_load_send_data_raw_cnf1:
    /* ==================================================================== */
    {
      Debug("SFH_state_load_send_data_raw_cnf1> Send RAW data 3 (ReqFlashWriteImageRaw)\n");

      act_ctx->got_tra_callback = false;   // then wait for the transport handler callback.
      // Setup to expect the confirm package ... as we now know that the command is transmitted.
      if (TRA_receive_mem_data_no_tx(act_ctx->channel, ReqFlashWriteImageRaw, act_ctx->cnf_buf, 2,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
      {
        fail(act_ctx, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_setup_confirm) );   break;
      }

      /* Got the comm call back .. now wait for the tra callback, triggered by the reception of the confirm package. */
      act_ctx->state = SFH_state_await_tra;
      act_ctx->next_state = SFH_state_load_send_data_raw_cnf2;
      break;
    }
    /* ==================================================================== */
  case SFH_state_load_send_data_raw_cnf2:
    /* ==================================================================== */
    {
      Debug("SFH_state_load_send_data_raw_cnf2> Send RAW data 4\n");
      //GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessDetailUpdated, "Send RAW data 4");

      act_ctx->state = SFH_state_await_tra;
      act_ctx->next_state = SFH_state_load_send_data_raw_loop;
      break;
    }
    /* ==================================================================== */
  case SFH_state_load_send_data_raw_loop:
    /* ==================================================================== */
    {
      send_percent(act_ctx->channel,calc_percent(act_ctx->load_length - act_ctx->raw_remaining_bytes, act_ctx->load_length));

      if (act_ctx->raw_remaining_bytes)
      {
        act_ctx->state = SFH_state_load_send_data_raw1;
      }
      else
      {
        MEM_free(act_ctx->raw_data_ptr);
        act_ctx->raw_data_ptr = NULL;

        act_ctx->state = SFH_state_erase_chk;
      }

      break;
    }
    /* ==================================================================== */
  case SFH_state_erase_chk:
    /* ==================================================================== */
    {
      if ((act_ctx->handling_flags & HANDLE_RMW_SEQ) != HANDLE_RMW_SEQ)
      {
        send_percent(act_ctx->channel,calc_percent(100, 100));
      }
      act_ctx->temp2_u16 = *((uint16_t*)&act_ctx->cnf_buf[0]);
      if ((act_ctx->handling_flags & HANDLE_ERASE_REGION) ||
          (act_ctx->handling_flags & HANDLE_ERASE_TOTAL_LEN))
      {
        act_ctx->state = SFH_state_erase_chk_2;
      }
      else
      {
        act_ctx->state = SFH_state_load_loop_loadmap_end;
      }
      break;
    }
    /* ==================================================================== */
  case SFH_state_erase_chk_2:
    /* ==================================================================== */
    {
      *((uint16_t*)&act_ctx->cmd_buf[0]) = 0;

      act_ctx->got_tra_callback = false;
      if (TRA_receive_mem_data(act_ctx->channel,  ReqFlashEraseCheck, act_ctx->cnf_buf, 6,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
      {
        fail_global_text(act_ctx, "SFH_state_erase_chk_2", SFH_ERR_setup_confirm); break;
      }

      if (TRA_send_mem_data(act_ctx->channel, ReqFlashEraseCheck, act_ctx->cmd_buf, 2,TRA_progress_report_disabled, NULL) != TRA_status_ok)
      {
        fail_global_text(act_ctx, "SFH_state_erase_chk_2", SFH_ERR_sending_command); break;
      }
      act_ctx->state = SFH_state_await_tra;
      act_ctx->next_state = SFH_state_erase_chk_cnf;
      break;
    }

    /* ==================================================================== */
  case SFH_state_erase_chk_cnf:
    /* ==================================================================== */
    {
      if (act_ctx->cnf_buf[0] == 0)
      {

        if ((act_ctx->handling_flags & HANDLE_ERASE_REGION) ||
            (act_ctx->handling_flags & HANDLE_ERASE_TOTAL_LEN))
        {
          send_percent(act_ctx->channel,calc_percent((*((uint32_t*)&act_ctx->cnf_buf[1]) - act_ctx->load_address), act_ctx->load_length_total));
        }
        else
        {
          send_percent(act_ctx->channel,calc_percent((*((uint32_t*)&act_ctx->cnf_buf[1]) - act_ctx->load_address), act_ctx->load_length));
        }

        act_ctx->state = SFH_state_erase_chk_2;
      }
      else
      {
        send_percent(act_ctx->channel,calc_percent(100,100));
        *((uint16_t*)&act_ctx->cnf_buf[0]) = act_ctx->temp2_u16;
        act_ctx->state = SFH_state_load_loop_loadmap_end;
      }
      break;
    }

    /* ==================================================================== */
  case SFH_state_load_loop_loadmap_end:
    /* ==================================================================== */
    {
#if 0  /* Enable for bandwidth measurement */
      uint32_t eff_bandwidth;
      uint32_t time_ms;
      char bw_str[64];
      if (act_ctx->bw_meas_index == act_ctx->loadmap_index)
      {
        time_ms = TIMER_stop_chronom_millisec(&act_ctx->bandwidth_meas_timer);
        eff_bandwidth = (uint32_t)((float)(((float)act_ctx->load_length / 1024) * 1000 * 8) / time_ms);
        sprintf(bw_str,"L=%d Kbytes;  T=%d mS;  BWeff=%d Kbit/sec ",act_ctx->load_length/1024, time_ms, eff_bandwidth);
        GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessDetailUpdated, bw_str);
      }
#endif

      if (act_ctx->handling_flags & HANDLE_LOADMAP)
      {
        /* Get the checksum reported from the target from the last load */
        if (act_ctx->load_header.Security.LoadMap[act_ctx->loadmap_index].UsedLength != 0)
        {
          if (act_ctx->boot_dev_cfg.technology == NAND_FLASH)
          {
            act_ctx->target_checksum ^= *((uint16_t*)&act_ctx->cnf_buf[0]);  // PES: EBL zero checksum after each ReqSecStart and ReqFlashSetAddress
          }
          else
          {
            act_ctx->target_checksum ^= *((uint16_t*)&act_ctx->cnf_buf[0]);
          }
          if (act_ctx->handling_flags & HANDLE_USE_CALC_CRC)
          {
            act_ctx->calc_checksum ^= TRA_get_last_checksum(act_ctx->channel);
          }
        }

        if (act_ctx->handling_flags & HANDLE_BIN)
        {
          act_ctx->loadmap_index = 8; // In binary mode we do not use load maps so force loop exit.
        }
        else
        {
          act_ctx->loadmap_index++;
        }
        if (act_ctx->loadmap_index < 8)
        {
          act_ctx->state = SFH_state_load_loop_loadmap_start;
          break;
        }
      }
      if (act_ctx->handling_flags & HANDLE_STOP_AFTER_ERASE)
      {
        act_ctx->state = SFH_state_load_send_nvm;
      }
      else
      {
        act_ctx->state = SFH_state_load_check_calc_checksum;
      }
      break;
    }

    /* ==================================================================== */
  case SFH_state_load_check_calc_checksum:
    /* ==================================================================== */
    {
      act_ctx->state = SFH_state_load_send_nvm;
      break;
    }

    /* ==================================================================== */
  case SFH_state_load_send_nvm:
    /* ==================================================================== */
    {
    if ((act_ctx->boot_dev_cfg.platform == X_GOLD_618 ||
         act_ctx->boot_dev_cfg.platform == X_GOLD_706 ||
         act_ctx->boot_dev_cfg.platform == X_GOLD_716 ||
         act_ctx->boot_dev_cfg.platform == X_GOLD_726 ||
         act_ctx->boot_dev_cfg.platform == X_GOLD_631 ||
         act_ctx->boot_dev_cfg.platform == X_GOLD_632 ||
         act_ctx->boot_dev_cfg.platform == X_GOLD_633 ||
         act_ctx->boot_dev_cfg.platform == X_GOLD_636 ||
         act_ctx->boot_dev_cfg.platform == X_GOLD_221 ||
         act_ctx->boot_dev_cfg.platform == X_GOLD_222 ||
         act_ctx->boot_dev_cfg.platform == X_GOLD_223 ||
         act_ctx->boot_dev_cfg.platform == X_GOLD_225 ||
         act_ctx->boot_dev_cfg.platform == X_GOLD_215 ||
         act_ctx->boot_dev_cfg.platform == X_GOLD_213 ||
         act_ctx->boot_dev_cfg.platform == X_GOLD_626) &&
      (((act_ctx->boot_ebl_version.major == 10) && (act_ctx->boot_ebl_version.minor > 5)) || (act_ctx->boot_ebl_version.major > 10)) &&


          (act_ctx->load_mem_class == CODE_CLASS) &&
          (SFH_nvm_nof_options))
      {

        act_ctx->state = SFH_state_await_tra;

        act_ctx->got_tra_callback = false;
        if (TRA_receive_mem_data(act_ctx->channel, ReqSetNvmOptions, act_ctx->cnf_buf, 2,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
        {
          fail_global_text(act_ctx, "SFH_state_load_send_nvm", SFH_ERR_setup_confirm); break;
        }

        if (TRA_send_mem_data(act_ctx->channel, ReqSetNvmOptions, SFH_nvm_options, SFH_nvm_nof_options,TRA_progress_report_disabled, NULL) != TRA_status_ok)
        {
          fail_global_text(act_ctx, "SFH_state_load_send_nvm", SFH_ERR_sending_command); break;
        }
        act_ctx->state = SFH_state_await_tra;
        act_ctx->next_state = SFH_state_load_send_sec_end;

      }
      else
      {
        if ((act_ctx->load_mem_class == CODE_CLASS) &&
          (SFH_nvm_nof_options))
        {
          GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessOutlineUpdated, "WARNING: Target EBL does not support NVM handling");
        }
        act_ctx->state = SFH_state_load_send_sec_end;
      }
      break;
    }

    /* ==================================================================== */
  case SFH_state_load_send_sec_end:
    /* ==================================================================== */
    {
      uint8_t exp_cnf_len;
      *((uint16_t*)act_ctx->cmd_buf) = 0;
      act_ctx->temp_timeout_storage = GLOBAL_comm_timeout;
      GLOBAL_comm_timeout = 60000; /* To support the time needed if the full image must be verified on target */

      GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_load_send_end));

      act_ctx->got_tra_callback = false;
      if (target_capabilities[act_ctx->channel].crc_method == CAP_CRC_RETURNED_BY_SEC_END)
      {
        exp_cnf_len = 4;
      }
      else
      {
        exp_cnf_len = 2;
      }
      if (TRA_receive_mem_data(act_ctx->channel, ReqSecEnd, act_ctx->cnf_buf, exp_cnf_len,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
      {
        fail_global_text(act_ctx, "SFH_state_load_send_sec_end", SFH_ERR_setup_confirm); break;
      }

      if (TRA_send_mem_data(act_ctx->channel, ReqSecEnd, act_ctx->cmd_buf, 2,TRA_progress_report_disabled, NULL) != TRA_status_ok)
      {
        fail_global_text(act_ctx, "SFH_state_load_send_sec_end", SFH_ERR_sending_command); break;
      }

      act_ctx->state = SFH_state_await_tra;
      act_ctx->next_state = SFH_state_load_send_sec_end_rsp;
      break;
    }

    /* ==================================================================== */
  case SFH_state_load_send_sec_end_rsp:
    /* ==================================================================== */
    {
      GLOBAL_comm_timeout = act_ctx->temp_timeout_storage;
      if (act_ctx->cnf_buf[0] == 0x00)
      {
        TIMER_stop_chronom_sec(&act_ctx->test_timer);
        fail_global_text(act_ctx, "SFH_state_load_send_sec_end_rsp", SFH_ERR_verify); break;
      }
      else
      {
        if (target_capabilities[act_ctx->channel].crc_method == CAP_CRC_RETURNED_BY_SEC_END)
        {
          act_ctx->target_checksum = *((uint16_t*)& act_ctx->cnf_buf[2]);
        }

        /* Test if the checksum is OK */
        if (act_ctx->handling_flags & HANDLE_USE_CALC_CRC)
        {
          act_ctx->temp_u16 = act_ctx->calc_checksum;
        }
        else
        {
          act_ctx->temp_u16 = act_ctx->exp_checksum;
        }

        if (!(act_ctx->handling_flags & HANDLE_STOP_AFTER_ERASE))
        {
          if ((act_ctx->target_checksum != act_ctx->temp_u16) && (target_capabilities[act_ctx->channel].crc_type != CAP_CRC_NOT_USED))
          {
            //fail_global_text(act_ctx, "SFH_state_load_send_sec_end_rsp", SFH_ERR_checksum_error); break;
            fail(act_ctx, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_checksum_error, act_ctx->temp_u16, act_ctx->target_checksum ) );
            break;
          }
          else
          {
            //            GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_load_crc_ok));
            GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessOutlineUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_load_crc_ok,act_ctx->target_checksum));

          }
        }
        GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_verify));
        TIMER_stop_chronom_sec(&act_ctx->test_timer);
        ShowTime(act_ctx->channel, act_ctx->test_timer.time);
      }

      Debug("Exiting time critical section.\n");
      GLOBAL_is_in_time_critical_section = false;
      act_ctx->state = SFH_state_done_ok;
      break;
    }

    /* ==================================================================== */
  case SFH_state_upload:
    /* ==================================================================== */
    {
      TIMER_start_chronom_sec(&act_ctx->test_timer);
      act_ctx->state = SFH_state_upload_send_sec;

      break;
    }

    /* ==================================================================== */
  case SFH_state_upload_send_sec:
    /* ==================================================================== */
    {
      /* Send the SEC-Pack to the target */
      GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_load_send_sec));

      act_ctx->got_tra_callback = false;
      if (TRA_receive_mem_data(act_ctx->channel,  ReqSecStart, act_ctx->cnf_buf, 2,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
      {
        fail_global_text(act_ctx, "SFH_state_upload_send_sec", SFH_ERR_setup_confirm); break;
      }

      if (TRA_send_mem_data(act_ctx->channel, ReqSecStart, (uint8_t*)&(act_ctx->load_header.Security), sizeof(SecurityStructType),TRA_progress_report_disabled, NULL) != TRA_status_ok)
      {
        fail_global_text(act_ctx, "SFH_state_upload_send_sec", SFH_ERR_sending_command); break;
      }

      act_ctx->state = SFH_state_await_tra;
      act_ctx->next_state = SFH_state_upload_send_sec_rsp;
      break;
    }

    /* ==================================================================== */
  case SFH_state_upload_send_sec_rsp:
    /* ==================================================================== */
    {
      if (act_ctx->cnf_buf[0] != 1)
      {
        /* Secure pack has returned NAK */
      }
      act_ctx->state = SFH_state_upload_send_address;
      break;
    }

    /* ==================================================================== */
  case SFH_state_upload_send_address:
    /* ==================================================================== */
    {
      uint8_t UseOldReadAdr;
      const char* EBL_name = (char*)act_ctx->boot_ebl_version.name;

      *((uint32_t*)act_ctx->cmd_buf) = act_ctx->load_address;

      act_ctx->state = SFH_state_await_tra;

      act_ctx->got_tra_callback = false;

      UseOldReadAdr = false;

      /* In the following ... please note that EBL with version before 10.X is not to be considered. */
      if (act_ctx->boot_dev_cfg.platform < X_GOLD_618)
      {
        /* E_GOLD_CHIPSET_V2, E_GOLD_LITE_CHIPSET, S_GOLD_CHIPSET_V1, S_GOLD_LITE_CHIPSET, E_GOLD_CHIPSET_V3
           S_GOLD_CHIPSET_V2, S_GOLD_CHIPSET_V3,E_GOLD_RADIO_V1, E_GOLD_VOICE_V1, S_GOLD_RADIO_V1 */
        UseOldReadAdr = true;  // For all these older chipsets, if at all used any more, use old method
      }
      else if ((act_ctx->boot_dev_cfg.platform == X_GOLD_110) &&
               ((act_ctx->boot_ebl_version.major == 10) && (act_ctx->boot_ebl_version.minor < 7)))
      {
        UseOldReadAdr = true;  // For XG110 platform and EBL before version 10.7, use old method
      }
      else if ((act_ctx->boot_dev_cfg.platform == X_GOLD_618) &&
               (act_ctx->boot_dev_cfg.technology == NOR_FLASH) &&     /* All NAND anf FlashLess uses new method */
               (strstr(EBL_name, "6180_Bootloader")) &&   /* ICE3 uses new method */
               ((act_ctx->boot_ebl_version.major == 10) && (act_ctx->boot_ebl_version.minor < 11)))
      {
        UseOldReadAdr = true;  // For XG618 NOR platform with specific EBL name text and EBL before version 10.11, use old method
      }
      else if ((act_ctx->boot_dev_cfg.platform == X_GOLD_213) &&
               (strstr(EBL_name, "Quantum_Bootloader")) &&
               ((act_ctx->boot_ebl_version.major == 10) && (act_ctx->boot_ebl_version.minor < 17)))
      {
        UseOldReadAdr = true;  // For XG213 (Quantum) and EBL before version 10.17, use old method
      }
      else if ((act_ctx->boot_dev_cfg.platform == X_GOLD_213) &&
               (strstr(EBL_name, "2130_Bootloader")) &&
               ((act_ctx->boot_ebl_version.major == 10) && (act_ctx->boot_ebl_version.minor < 3)))
      {
        UseOldReadAdr = true;  // For XG213 (EHS2AGR) platform and EBL before version 10.3, use old method
      }
      else if ((act_ctx->boot_dev_cfg.platform == X_GOLD_213) &&
               (strstr(EBL_name, "2130_RAM_Bootloader")))
      {
        UseOldReadAdr = true;  // For XG213 (EHS2AGR) and embedded EBL
      }

      if (UseOldReadAdr)
      {
        if (TRA_receive_mem_data(act_ctx->channel, ReqFlashSetAddress, act_ctx->cnf_buf, 6,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
        {
          fail(act_ctx, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_setup_confirm) );   break;
        }

        if (TRA_send_mem_data(act_ctx->channel, ReqFlashSetAddress, act_ctx->cmd_buf, 4,TRA_progress_report_disabled, NULL) != TRA_status_ok)
        {
          fail(act_ctx,  GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_sending_command) );  break;
        }
      }
      else
      {
        if (TRA_receive_mem_data(act_ctx->channel, ReqFlashSetReadAddress, act_ctx->cnf_buf, 6,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
        {
          fail_global_text(act_ctx, "SFH_state_upload_send_address", SFH_ERR_setup_confirm); break;
        }

        if (TRA_send_mem_data(act_ctx->channel, ReqFlashSetReadAddress, act_ctx->cmd_buf, 4,TRA_progress_report_disabled, NULL) != TRA_status_ok)
        {
          fail_global_text(act_ctx, "SFH_state_upload_send_address", SFH_ERR_sending_command); break;
        }
      }

      act_ctx->state = SFH_state_await_tra;
      if (act_ctx->handling_flags & HANDLE_BIN_GET_MBN)
      {
        act_ctx->next_state = SFH_state_upload_get_mbn_data;
      }
      else
      {
        act_ctx->next_state = SFH_state_upload_get_data;
      }

      break;
    }

    /* ==================================================================== */
  case SFH_state_upload_get_mbn_data:
    /* ==================================================================== */
    {
      GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_upload_rx_data));

      *((uint16_t*)act_ctx->cmd_buf) = 256; // MBN data size is 256 bytes.



      act_ctx->got_tra_callback = false;

      if (TRA_receive_mem_data(act_ctx->channel, ReqFlashReadBlock, act_ctx->upload_mbn_buffer, 256 ,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
      {
        fail_global_text(act_ctx, "SFH_state_upload_get_mbn_data", SFH_ERR_setup_confirm); break;
      }

      if (TRA_send_mem_data(act_ctx->channel, ReqFlashReadBlock, act_ctx->cmd_buf, 2 ,TRA_progress_report_disabled, NULL) != TRA_status_ok)
      {
        fail_global_text(act_ctx, "SFH_state_upload_get_mbn_data", SFH_ERR_sending_command); break;
      }

      act_ctx->state = SFH_state_await_tra;
      act_ctx->next_state = SFH_state_upload_get_data;

      break;
    }


    /* ==================================================================== */
  case SFH_state_upload_get_data:
    /* ==================================================================== */
    {
      uint16_t pack_size;

      GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_upload_rx_data));
      pack_size = act_ctx->boot_ebl_version.capabilities.package_size;
      if (pack_size == 0)
      {
        pack_size = 2;
      }

      if ((GLOBAL_usb_device) || COMM_TECH_IS_MIPI(act_ctx->channel))
      {
        if ((uint32_t)(pack_size * 1024) <= act_ctx->load_length)
        {
          *((uint16_t*)act_ctx->cmd_buf) = pack_size * 1024;
        }
        else
        {
          *((uint16_t*)act_ctx->cmd_buf) = act_ctx->load_length;
        }
      }
      else  // Handle USIF special as the Prolific chip have 256 or 384 bytes of buffer. Set package length to 256 to avoid buffer overflow.
      {
        if ((uint32_t)(256) <= act_ctx->load_length)
        {
          *((uint16_t*)act_ctx->cmd_buf) = 256;
        }
        else
        {
          *((uint16_t*)act_ctx->cmd_buf) = act_ctx->load_length;
        }
      }

      act_ctx->got_tra_callback = false;

      if (act_ctx->handling_flags & HANDLE_UPLOAD_TO_MEM)
      {
        if (act_ctx->handling_flags & HANDLE_RMW_SEQ)
        {
          if (TRA_receive_mem_data(act_ctx->channel, ReqFlashReadBlock, act_ctx->load_data_ptr, act_ctx->load_length ,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
          {
            fail_global_text(act_ctx, "SFH_state_upload_get_data", SFH_ERR_setup_confirm); break;
          }
        }
        else
        {
          if (TRA_receive_mem_data(act_ctx->channel, ReqFlashReadBlock, act_ctx->load_data_ptr, act_ctx->load_length ,TRA_progress_report_enabled, SFH_TRA_callback) != TRA_status_ok)
          {
            fail_global_text(act_ctx, "SFH_state_upload_get_data", SFH_ERR_setup_confirm); break;
          }
        }
      }
      else
      {
        if (TRA_receive_file_data(act_ctx->channel, ReqFlashReadBlock, act_ctx->load_file_name, *((uint16_t*)act_ctx->cmd_buf), act_ctx->load_length ,TRA_progress_report_enabled, SFH_TRA_callback) != TRA_status_ok)
        {
          fail_global_text(act_ctx, "SFH_state_upload_get_data", SFH_ERR_setup_confirm); break;
        }
      }

      if (TRA_send_mem_data(act_ctx->channel, ReqFlashReadBlock, act_ctx->cmd_buf, 2 ,TRA_progress_report_disabled, NULL) != TRA_status_ok)
      {
        fail_global_text(act_ctx, "SFH_state_upload_get_data", SFH_ERR_sending_command); break;
      }

      act_ctx->state = SFH_state_await_tra;
      if (act_ctx->handling_flags & HANDLE_RMW_SEQ)
      {
        act_ctx->next_state = SFH_state_eeprom_update;
      }
      else
      {
        if (act_ctx->handling_flags & HANDLE_UPLOAD_ADD_EBL)
        {
          act_ctx->next_state = SFH_state_upload_add_ebl;
        }
        else
        {
          act_ctx->next_state = SFH_state_upload_send_sec_end;
        }
      }
      break;
    }
    /* ==================================================================== */
  case SFH_state_upload_add_ebl:
    /* ==================================================================== */
    {
      FILE *bin_file;
      uint8_t *ebl_data_ptr;
      uint32_t ebl_offset;
      uint32_t ebl_length;

      /* for NAND systems the EBL file must be appended to the BIN file. */
      act_ctx->ref_file = FILE_open(act_ctx->ref_file_name, FILE_open_mode_read);
      bin_file = FILE_open(act_ctx->load_file_name, FILE_open_mode_write_append);

      ebl_length = FILE_size(act_ctx->ref_file);
      ebl_offset = FILE_size(bin_file);
      ebl_data_ptr = MEM_malloc(ebl_length);
      if (ebl_data_ptr)
      {
        /* Append the EBL reference file to the bin upload image */
        FILE_read(act_ctx->ref_file, ebl_data_ptr, ebl_length);
        FILE_write(bin_file, ebl_data_ptr, ebl_length);

        /* Append the MBN data to the BIN file and creadte an MBN file if needed */
        if (act_ctx->handling_flags & HANDLE_BIN_GET_MBN)
        {
          FILE *mbn_file;
          char mbn_file_name[300];
          char *ext_ptr;

          /* Copy the binary file name */
          strncpy(mbn_file_name,act_ctx->load_file_name, sizeof(mbn_file_name)-1);
          mbn_file_name[sizeof(mbn_file_name)-1] = '\0';

          /* Change file extention to MBN */
          ext_ptr = FILE_ExtractFileExt(mbn_file_name);
          if (ext_ptr)
            sprintf(ext_ptr,".mbn");

          mbn_file = FILE_open(mbn_file_name, FILE_open_mode_write_create);
          FILE_write(mbn_file, act_ctx->upload_mbn_buffer, 256);
          FILE_close(mbn_file);

          /* Write the MBN data */
          FILE_write(bin_file, act_ctx->upload_mbn_buffer, 256);
        }

        /* Append the EBL data offset */
        FILE_write(bin_file, (uint8_t*)&ebl_offset, 4);

        /* Append the MBN data offset */
        ebl_offset += ebl_length; /* The MBN data (if any) is placed imideately after the EBL data */
        FILE_write(bin_file, (uint8_t*)&ebl_offset, 4);

        MEM_free(ebl_data_ptr);
      }

      FILE_close(bin_file);
      FILE_close(act_ctx->ref_file);

      act_ctx->state = SFH_state_upload_send_sec_end;
      break;
    }


    /* ==================================================================== */
  case SFH_state_eeprom_update:
    /* ==================================================================== */
    {
      int32_t i; /* a temporary loop variable that doesn't need to be more than a S16 */
      char path[300];
      char temp[300];
      char cur_cfg[300];
      char new_cfg[300];
      /* Do the update stuff here */
      EEP_Init();

      /* Get the current versions */
      act_ctx->cur_eeprom_ver = *((uint16_t*)&(act_ctx->load_data_ptr[ act_ctx->load_header.MemoryMap.EepRevAddrInEep - act_ctx->load_address ]));


      if (act_ctx->new_eeprom_ver == 0)
      {
        /* Get the version of the CFG file, to be used while writing, from the DEP file name. */
        char  *p, *p1,  verstr[10];
        p = strstr(act_ctx->dep_file_name,"delta");
        if(!p)
        {
          fail_global_text(act_ctx, "SFH_state_eeprom_update", SFH_ERR_dep_filename_error); break;
        }
        p1 = strchr(p,'.');
        if(!p1)
        {
          fail_global_text(act_ctx, "SFH_state_eeprom_update", SFH_ERR_dep_filename_error); break;
        }
        p += 5;  /* Skip the word 'delta' */
        verstr[0] = 0;

        strncpy(verstr,p,sizeof(verstr)-1);
        if(verstr[0] == 0)
        {
          fail_global_text(act_ctx, "SFH_state_eeprom_update", SFH_ERR_dep_filename_error); break;
        }
        act_ctx->new_eeprom_ver = (uint16_t)strtoul(verstr, &p, 10);
      }
      /* Get the path */
      strncpy(path,act_ctx->dep_file_name, sizeof(path)-1);
      path[sizeof(path)-1] = '\0';
      for (i=strlen(path)-1; i; i--)
      {
        if (path[i] == (char)'/' || path[i] == (char)'\\')
        {
          path[i] = 0;
          break;
        }
      }
      /* Get the template string from the header */
      for (i=0; i < (int32_t)strlen((char*)act_ctx->load_header.Hardware.CfgTemplate); i++)
      {
        if (act_ctx->load_header.Hardware.CfgTemplate[i] == '_')
        {
          break;
        }
      }
//WIN32      snprintf(temp,300, "%%s\\%%s%s",&(act_ctx->load_header.Hardware.CfgTemplate[i]));
      snprintf(temp,300, "%%s/%%s%s",&(act_ctx->load_header.Hardware.CfgTemplate[i]));
      snprintf(cur_cfg,300,temp,path,act_ctx->project_prefix,act_ctx->cur_eeprom_ver);
      snprintf(new_cfg,300,temp,path,act_ctx->project_prefix,act_ctx->new_eeprom_ver);
      if (!FILE_exists(cur_cfg))
      {
        EEP_FreeAllMem();
        //fail_global_text(act_ctx, "SFH_state_eeprom_update", SFH_ERR_file_not_found); break;
        fail(act_ctx, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_not_found,cur_cfg) );   break;
      }
      if (!FILE_exists(new_cfg))
      {
        EEP_FreeAllMem();
        //fail_global_text(act_ctx, "SFH_state_eeprom_update", SFH_ERR_file_not_found); break;
        fail(act_ctx, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_not_found,new_cfg) );   break;
      }

      GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_cur_eep_cfg,cur_cfg));
      GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_new_eep_cfg,new_cfg));
#if 0  /* Dump eepdata after upload */
      {
        FILE *eep_file_before;
        eep_file_before = FILE_open("d:\\sw\\before.bin", FILE_open_mode_write_create);
        FILE_write(eep_file_before, act_ctx->load_data_ptr, act_ctx->load_length);
        FILE_close(eep_file_before);
      }
#endif
#if 0  /* Read from dump file for debugging */
      {
        FILE *eep_file_before;
        eep_file_before = FILE_open("t:\\njk\\from_mfn\\before_failed.bin", FILE_open_mode_read);
        FILE_read(eep_file_before, act_ctx->load_data_ptr, act_ctx->load_length);
        FILE_close(eep_file_before);
      }
#endif

      /* Remove any segmentation */
      if ((act_ctx->load_header.Hardware.Platform == E_GOLD_VOICE_V1) ||
        (act_ctx->load_header.Hardware.Platform == X_GOLD_102) ||
        (act_ctx->load_header.Hardware.Platform == X_GOLD_110))
      {
        if (!EEP_linearize_bering((uint16_t*)act_ctx->load_data_ptr, act_ctx->load_header.Security.LoadMap[0].UsedLength, act_ctx->load_length))
        {
          EEP_FreeAllMem();
          fail_global_text(act_ctx, "SFH_state_eeprom_update", SFH_ERR_eep_linearize); break;
        }
      }
      else
      {
        if (!EEP_linearize((uint16_t*)act_ctx->load_data_ptr, act_ctx->load_header.Security.LoadMap[0].UsedLength, act_ctx->load_length))
        {
          EEP_FreeAllMem();
          fail_global_text(act_ctx, "SFH_state_eeprom_update", SFH_ERR_eep_linearize); break;
        }
      }
      memset((uint8_t*)&(act_ctx->load_data_ptr[act_ctx->load_header.Security.LoadMap[0].UsedLength]),0xFF,act_ctx->load_length - act_ctx->load_header.Security.LoadMap[0].UsedLength);


      /* Do the delta update */
      EEP_Load_CurrCFG_file(cur_cfg);
      EEP_Load_NewCFG_file(new_cfg);

      EEP_Load_DEP_file(act_ctx->dep_file_name);
      EEP_CnvFromBin(act_ctx->load_data_ptr, act_ctx->load_length);
      EEP_CnvToBin(act_ctx->load_data_ptr, act_ctx->load_length);
      EEP_FreeAllMem();

#if 0  /* Dump eepdata before download */
      {
        FILE *eep_file_after;
        eep_file_after = FILE_open("d:\\sw\\after.bin", FILE_open_mode_write_create);
        FILE_write(eep_file_after, act_ctx->load_data_ptr, act_ctx->load_length);
        FILE_close(eep_file_after);
      }
#endif

      /* Prepare the download phase */
      act_ctx->target_checksum = 0;
      act_ctx->calc_checksum = 0;
      act_ctx->state = SFH_state_load;
      break;
    }
    /* ==================================================================== */
  case SFH_state_upload_send_sec_end:
    /* ==================================================================== */
    {

      *((uint16_t*)act_ctx->cmd_buf) = 0;

      act_ctx->state = SFH_state_await_tra;

      act_ctx->got_tra_callback = false;
      if (TRA_receive_mem_data(act_ctx->channel, ReqSecEnd, act_ctx->cnf_buf, 2,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
      {
        fail_global_text(act_ctx, "SFH_state_upload_send_sec_end", SFH_ERR_setup_confirm); break;
      }

      if (TRA_send_mem_data(act_ctx->channel, ReqSecEnd, act_ctx->cmd_buf, 2,TRA_progress_report_disabled, NULL) != TRA_status_ok)
      {
        fail_global_text(act_ctx, "SFH_state_upload_send_sec_end", SFH_ERR_sending_command); break;
      }

      act_ctx->state = SFH_state_await_tra;
      act_ctx->next_state = SFH_state_upload_send_sec_end_rsp;
      break;
    }

    /* ==================================================================== */
  case SFH_state_upload_send_sec_end_rsp:
    /* ==================================================================== */
    {
      if (act_ctx->cnf_buf[0] == 0x00)
      {
        TIMER_stop_chronom_sec(&act_ctx->test_timer);
        fail_global_text(act_ctx, "SFH_state_upload_send_sec_end_rsp", SFH_ERR_verify); break;
      }
      else
      {
        GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_upload_ok));
        TIMER_stop_chronom_sec(&act_ctx->test_timer);
        ShowTime(act_ctx->channel, act_ctx->test_timer.time);
      }

      Debug("Exiting time critical section.\n");
      GLOBAL_is_in_time_critical_section = false;
      act_ctx->state = SFH_state_done_ok;
      break;
    }


    /* ==================================================================== */
  case SFH_state_oos_send_control:
    /* ==================================================================== */
    {
      uint8_t send_control = true;

      act_ctx->state = SFH_state_await_tra;

      if (act_ctx->oos_length & 1)
      {
        act_ctx->oos_length++;
      }

      if (act_ctx->oos_length)
      {
        if (act_ctx->oos_action == IFWD_DL_oosc_action_read )
        {
          send_control = false;
          act_ctx->next_state = SFH_state_oos_receive_data;
        }
        else
        {
          act_ctx->next_state = SFH_state_oos_send_data;
        }
      }
      else
      {
        if (act_ctx->oos_action == IFWD_DL_oosc_action_read )
        {
          act_ctx->next_state = SFH_state_oos_receive_data_length;
        }
        else
        {
          act_ctx->next_state = SFH_state_done_ok;
        }
      }

      if (send_control)
      {
        *(((uint32_t*)act_ctx->cmd_buf)+0) = act_ctx->oos_action;
        *(((uint32_t*)act_ctx->cmd_buf)+1) = act_ctx->oos_type;
        *(((uint32_t*)act_ctx->cmd_buf)+2) = act_ctx->oos_length;
        *(((uint32_t*)act_ctx->cmd_buf)+3) = act_ctx->oos_spare;

        act_ctx->got_tra_callback = false;

        if (TRA_receive_mem_data(act_ctx->channel, ReqOutOfSessionControl, act_ctx->cnf_buf, 4 ,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
        {
          fail_global_text(act_ctx, "SFH_state_oos_send_control", SFH_ERR_setup_confirm); break;
        }

        if (TRA_send_mem_data(act_ctx->channel, ReqOutOfSessionControl, act_ctx->cmd_buf, 16 ,TRA_progress_report_disabled, NULL) != TRA_status_ok)
        {
          fail_global_text(act_ctx, "SFH_state_oos_send_control", SFH_ERR_sending_command); break;
        }
      }
      break;
    }
    /* ==================================================================== */
  case SFH_state_oos_send_data:
    /* ==================================================================== */
    {

      act_ctx->got_tra_callback = false;

      if (TRA_receive_mem_data(act_ctx->channel, ReqOutOfSessionDataWrite, act_ctx->cnf_buf, 4 ,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
      {
        fail_global_text(act_ctx, "SFH_state_oos_send_data", SFH_ERR_setup_confirm); break;
      }

      if (TRA_send_mem_data(act_ctx->channel, ReqOutOfSessionDataWrite, act_ctx->oos_data_ptr, act_ctx->oos_length ,TRA_progress_report_disabled, NULL) != TRA_status_ok)
      {
        fail_global_text(act_ctx, "SFH_state_oos_send_data", SFH_ERR_sending_command); break;
      }

      act_ctx->state = SFH_state_await_tra;
      act_ctx->next_state = SFH_state_done_ok;
      break;
    }
    /* ==================================================================== */
  case SFH_state_oos_receive_data_length:
    /* ==================================================================== */
    {
       *((uint32_t*)act_ctx->oos_data_ptr) = *((uint32_t*)act_ctx->cnf_buf);
       act_ctx->state = SFH_state_done_ok;
       break;
    }
   /* ==================================================================== */

   /* ==================================================================== */

  /* ==================================================================== */

  case SFH_state_oos_receive_data:
    /* ==================================================================== */
    {
      if (act_ctx->oos_type == IFWD_DL_oosc_type_hw_details)
      {
        *((uint32_t*)act_ctx->cmd_buf) = act_ctx->oos_length;
      }
      else
      {
        uint16_t pack_size;

        pack_size = act_ctx->boot_ebl_version.capabilities.package_size;
        if (pack_size == 0)
        {
          pack_size = 2;
        }
        if ((uint32_t)(pack_size * 1024) <= act_ctx->oos_length)
        {
          *((uint32_t*)act_ctx->cmd_buf) = pack_size * 1024;
        }
        else
        {
          *((uint32_t*)act_ctx->cmd_buf) = act_ctx->oos_length;
        }
      }

      act_ctx->got_tra_callback = false;

      if (TRA_receive_mem_data(act_ctx->channel, ReqOutOfSessionDataRead, act_ctx->oos_data_ptr, act_ctx->oos_length ,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
      {
        fail_global_text(act_ctx, "SFH_state_oos_receive_data", SFH_ERR_setup_confirm); break;
      }

      if (TRA_send_mem_data(act_ctx->channel, ReqOutOfSessionDataRead, act_ctx->cmd_buf, 4 ,TRA_progress_report_disabled, NULL) != TRA_status_ok)
      {
        fail_global_text(act_ctx, "SFH_state_oos_receive_data", SFH_ERR_sending_command); break;
      }

      act_ctx->state = SFH_state_await_tra;
      act_ctx->next_state = SFH_state_done_ok;
      break;
    }


    /* ==================================================================== */
  case SFH_state_force_target_reset:
    /* ==================================================================== */
    {
      if (GLOBAL_use_sec_ch_ext_cb[act_ctx->channel]) // Handle secure channel if there was an external callback function defined.
      {
        act_ctx->state = SFH_state_sec_ch_req_nonce_and_ver;
      }
      else
      {
        act_ctx->state = SFH_state_force_target_reset_send;
      }
      break;
    }
    /* ==================================================================== */
  case SFH_state_force_target_reset_send:
    /* ==================================================================== */
    {
      /* As this call should reset the phone there will not be any response package to receive */
      TIMER_stop_chronom_sec(&act_ctx->total_timer);
      ShowTotalTime(act_ctx->channel, act_ctx->total_timer.time);
      if (TRA_send_mem_data(act_ctx->channel, ReqForceHwReset, act_ctx->cmd_buf, 4,TRA_progress_report_disabled, NULL) != TRA_status_ok)
      {
        fail_global_text(act_ctx, "SFH_state_force_target_reset", SFH_ERR_sending_command); break;
      }
      else
      {
        /* Wait for data tx, target startup + boot time (so we don't auto detect target as new phone, and start dl again) */
        TIMER_new_timer(&act_ctx->state_delay_timer, DELAY_TARGET_RESET);
        act_ctx->state = SFH_state_force_target_reset_waiting;
      }

      break;
    }

    /* ==================================================================== */
  case SFH_state_force_target_reset_waiting:
    /* ==================================================================== */
    {
      if(TIMER_timer_expired(&act_ctx->state_delay_timer))  // DELAY_TARGET_RESET
      {
        //COMM_close_comport(act_ctx->channel);
        COMM_abort_process(act_ctx->channel); /* abort all transactions */
        PROT_abort_process(act_ctx->channel);
        TRA_abort_process(act_ctx->channel);
        act_ctx->state = SFH_state_done_ok;
      }
      break;
    }
    /* ==================================================================== */
  case SFH_state_sec_ch_req_nonce_and_ver:
    /* ==================================================================== */
    {

      act_ctx->got_tra_callback = false;

      act_ctx->cmd_buf[0] = 0x00;
      act_ctx->cmd_buf[1] = 0x00;

      if (TRA_receive_mem_data(act_ctx->channel, ReqSecChNonceAndVersion, act_ctx->cnf_buf, 1024 ,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
      {
        fail_global_text(act_ctx, "ReqSecChNonceAndVersion", SFH_ERR_setup_confirm); break;
      }

      if (TRA_send_mem_data(act_ctx->channel, ReqSecChNonceAndVersion, act_ctx->cmd_buf, 2 ,TRA_progress_report_disabled, NULL) != TRA_status_ok)
      {
        fail_global_text(act_ctx, "ReqSecChNonceAndVersion", SFH_ERR_sending_command); break;
      }

      act_ctx->state      = SFH_state_await_tra;
      act_ctx->next_state = SFH_state_sec_ch_req_nonce_and_ver_rsp;
    break;
    }

    /* ==================================================================== */
  case SFH_state_sec_ch_req_nonce_and_ver_rsp:
    /* ==================================================================== */
    {
      if ((act_ctx->cnf_buf[0] == 0x00) && (act_ctx->cnf_buf[1] == 0x00))
      {
          // Get back to the normal state-flow
          act_ctx->state = SFH_state_force_target_reset_send;
      }
      else
      {
          act_ctx->state = SFH_state_sec_ch_req_verdict;
      }
    break;
    }

    /* ==================================================================== */
  case SFH_state_sec_ch_req_verdict:
    /* ==================================================================== */
    {
      // cnf_buf holds the returned TLV structure
      // Setup to do call-back
      uint32_t  cb_err_code;
      size_t cb_data_length = (((uint16_t*)act_ctx->cnf_buf)[1]) + 4;  // TLV structure: uint16_t type, uint16_t Length, ....    + 4 bytes for header.
      uint32_t  cb_data_type   = SECURE_CH_DATA_REQ;
      uint8_t  *cb_data_ptr    = act_ctx->cnf_buf;

      Debug("Got SecCh Nonce and Version. Length = %d First data 0x%08X", cb_data_length, *((uint32_t*) cb_data_ptr));

      cb_err_code = secure_channel_cb_fx(&cb_data_type, &cb_data_length, &cb_data_ptr );

      // Expect the verdict TLV structure to be returned via the parameters.
      if ((cb_err_code == SECURE_CH_RES_SUCCESS) && (cb_data_type == SECURE_CH_DATA_REPLY) && cb_data_length && (cb_data_ptr != NULL))
      {
          Debug("Got CallBack verdict. Length = %d First data 0x%08X", cb_data_length, *((uint32_t*) cb_data_ptr));

          act_ctx->got_tra_callback = false;
          memcpy((void*)act_ctx->cmd_buf,(void*)cb_data_ptr, cb_data_length );



          if (TRA_receive_mem_data(act_ctx->channel, ReqSecChValidateVerdict, act_ctx->cnf_buf, 1024 ,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
          {
            fail_global_text(act_ctx, "ReqSecChValidateVerdict", SFH_ERR_setup_confirm); break;
          }

          if (TRA_send_mem_data(act_ctx->channel, ReqSecChValidateVerdict, act_ctx->cmd_buf, cb_data_length ,TRA_progress_report_disabled, NULL) != TRA_status_ok)
          {
            fail_global_text(act_ctx, "ReqSecChValidateVerdict", SFH_ERR_sending_command); break;
          }

          // Call to free data
          cb_data_type   = SECURE_CH_DATA_FREE_RETURN_DATA;
          cb_data_length = 0;
          cb_data_ptr    = NULL;
          (void)secure_channel_cb_fx(&cb_data_type, &cb_data_length, &cb_data_ptr );

          act_ctx->state = SFH_state_await_tra;
          act_ctx->next_state = SFH_state_sec_ch_req_verdict_rsp;

      }
      else
      {
        // TODO make better error reporting.
        fail(act_ctx, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_secure_ch_cb_fail,cb_err_code) );   break;

      }
    break;
    }

    /* ==================================================================== */
  case SFH_state_sec_ch_req_verdict_rsp:
    /* ==================================================================== */
    {
      uint16_t boot_verdict = ((uint16_t*)act_ctx->cnf_buf)[0]; // to select one of the texts above.
      uint16_t sw_ver_valid = ((uint16_t*)act_ctx->cnf_buf)[1]; // boolean true if sw version is valid.

      Debug("SecCh Verdict %04X   SW valid %04X", boot_verdict, sw_ver_valid);

      if (boot_verdict > 2)
        boot_verdict = 3;

      boot_verdict_string = boot_verdict_txt[boot_verdict];

      // Validate the response and report status via call-back
      if (sw_ver_valid)
      {
        GLOBAL_callback(act_ctx->channel,IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info,SFH_INFO_secure_ch_ok, boot_verdict_txt[boot_verdict]));
      }
      else
      {
        fail(act_ctx, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_secure_ch_sign_fail, boot_verdict_txt[boot_verdict]) );   break;
      }


      // Get back to the normal state-flow
      act_ctx->state = SFH_state_force_target_reset_send;
      break;
    }


    /* ==================================================================== */
  case SFH_state_end_of_process:
    /* ==================================================================== */
    {
      /* As this call should reset the phone there will not be any responce package to receive */
      TIMER_stop_chronom_sec(&act_ctx->total_timer);
      ShowTotalTime(act_ctx->channel, act_ctx->total_timer.time);
      act_ctx->got_tra_callback = false;
      if (TRA_send_mem_data(act_ctx->channel, ReqSetLed, act_ctx->cmd_buf, 2,TRA_progress_report_disabled, SFH_TRA_callback) != TRA_status_ok)
      {
        fail_global_text(act_ctx, "SFH_state_end_of_process", SFH_ERR_sending_command); break;
      }
      else
      {
        act_ctx->state = SFH_state_await_tra;
        act_ctx->next_state = SFH_state_done_ok;
      }
      break;
    }

    /* ==================================================================== */
  case SFH_state_done_error:
    /* ==================================================================== */
    {
      TIMER_stop_chronom_sec(&act_ctx->test_timer);
      ShowTime(act_ctx->channel, act_ctx->test_timer.time);

      if ((act_ctx->handling_flags & HANDLE_RMW_SEQ) && act_ctx->load_data_ptr)
      {
        MEM_free((uint8_t*)act_ctx->load_data_ptr);
        act_ctx->load_data_ptr = NULL;
      }

      FILE_close(act_ctx->load_file);
      act_ctx->load_file = NULL;

      if (act_ctx->done_signal == SFH_done_signal_semaphore)
      {
        //GLOBAL_callback(act_ctx->channel,IFWD_DL_Error, act_ctx->status_text);
        SetEvent( done_event );
      }
      else if (act_ctx->done_signal == SFH_done_signal_callback)
      {
        GLOBAL_callback(act_ctx->channel,IFWD_DL_Error, act_ctx->status_text);
      }
      /* Make sure no one is stuck in an error state in this channel: */
      BOOT_abort_process(act_ctx->channel);
      COMM_abort_process(act_ctx->channel);
      PROT_abort_process(act_ctx->channel);
      TRA_abort_process(act_ctx->channel);

      Debug("Exiting time critical section.\n");
      GLOBAL_is_in_time_critical_section = false;
      act_ctx->state = SFH_state_idle;
      break;
    }

    /* ==================================================================== */
  case SFH_state_done_ok:
    /* ==================================================================== */
    {
      if ((act_ctx->handling_flags & HANDLE_RMW_SEQ) && act_ctx->load_data_ptr)
      {
        MEM_free((uint8_t*)act_ctx->load_data_ptr);
        act_ctx->load_data_ptr = NULL;
      }

      FILE_close(act_ctx->load_file);
      act_ctx->load_file = NULL;

      if (act_ctx->done_signal == SFH_done_signal_semaphore)
      {
        SetEvent( done_event );
      }
      else if (act_ctx->done_signal == SFH_done_signal_callback)
      {
        if (act_ctx->target_sw_running_skip_dl)
        {
          GLOBAL_callback(act_ctx->channel,IFWD_DL_OK, "Sys SW is running - skip downloads");
        }
        else
        {
          GLOBAL_callback(act_ctx->channel,IFWD_DL_OK, (char*)boot_verdict_string);
        }
      }
      act_ctx->state = SFH_state_idle;
      break;
    }

    /* ==================================================================== */
  default:
    /* do nothing */
    break;
    /* ==================================================================== */
  }
}


/****************************************************************************************
* Function:... SFH_init
* Parameters:. -
* Returns:.... -
* Description: This function initialize this handler
* Created:.... DD.MM.YY by (Full name / initials)
* Modified:... DD.MM.YY by (Full name / initials)
*                Modifications note.
****************************************************************************************/
void SFH_init(void)
{
  ctx_base_ptr = NULL;
  SFH_nof_active_instances = 0;
  SFH_nof_idle = 0;
  GLOBAL_SFH_in_idle = true;
  GLOBAL_nand_partition_config.Specification = MAC_NOSPECIFICATION;
  memset(GLOBAL_nand_partition_config.PartitionBit, 0x00, sizeof(GLOBAL_nand_partition_config.PartitionBit));
}


/****************************************************************************************
* Function:... SFH_clock
* Parameters:. -
* Returns:.... -
* Description: This function must be called repeatedly to clock the handler's internal state machine.
* Created:.... DD.MM.YY by (Full name / initials)
* Modified:... DD.MM.YY by (Full name / initials)
*                Modifications note.
****************************************************************************************/
void SFH_clock(void)
{
  if (active_ctx_ptr != NULL)
  {
    if(active_ctx_ptr->state == SFH_state_idle)
      SFH_nof_idle++;
    clock_the_machine(active_ctx_ptr);

    /* now find the next context to be active */
    active_ctx_ptr = active_ctx_ptr->next_ctx;
    if (active_ctx_ptr == NULL)
    {
      active_ctx_ptr = ctx_base_ptr;
      if(SFH_nof_idle != SFH_nof_active_instances && GLOBAL_SFH_in_idle) /* just changed from idle to active? */
        GLOBAL_signal_not_in_idle();  /* notify main loop that SFH is no longer in idle (needs OS dependent code to signal events to bring main loop out of idle)*/
      GLOBAL_SFH_in_idle = (SFH_nof_idle == SFH_nof_active_instances);
      SFH_nof_idle = 0;
    }
  }
  else
  {
    GLOBAL_SFH_in_idle = true;
    SFH_nof_idle = 0;
  }
}

/****************************************************************************************
* Function:... SFH_add_channel
* Parameters:. channel  In  Number of the logical channel (1..N)
* Returns:.... Status
* Description: This function is used to add a channel to the handler.
*              It must allocate the needed memory and initialize all variables.
* Created:.... DD.MM.YY by (Full name / initials)
* Modified:... DD.MM.YY by (Full name / initials)
*                Modifications note.
****************************************************************************************/
uint8_t SFH_add_channel(uint8_t channel, char *status)
{
  SFH_ctx_type *new_ctx_ptr;
  SFH_ctx_type *ctx_ptr;

  if (!status)
  {
    Debug("Error in %s: Status not defined", __func__);
    return false;
  }

  /* Check if the channel exists */
  if (get_ctx(channel))
  {
    /* The channel existed */
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_channel_already_opened, channel) );
    return (false);
  }

  /* OK - so add the channel */
  /* Get the memory needed for the new context */
  new_ctx_ptr = (SFH_ctx_type *)MEM_malloc(sizeof(SFH_ctx_type));
  if (!new_ctx_ptr)
  {
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_malloc) );
    return (false);
  }
  /* Got the memory so init the context */
  init_ctx(channel, new_ctx_ptr, status);
  TIMER_start_chronom_sec(&new_ctx_ptr->test_timer); /* make sure we don't report strange times if it fails before boot */

  SFH_nof_active_instances++; /* remember we have one more in the list */
  GLOBAL_signal_not_in_idle();  /* notify main loop that SFH is no longer in idle (needs OS dependent code to signal events to bring main loop out of idle)*/

  /* Insert the context in the linked list */
  if (ctx_base_ptr == NULL)
  {
    /* Its the first instance */
    new_ctx_ptr->prev_ctx = NULL;
    new_ctx_ptr->next_ctx = NULL;
    ctx_base_ptr = new_ctx_ptr;
  }
  else
  {
    /* insert the context as the first item in the list */
    ctx_ptr = ctx_base_ptr;

    /* Update the current 'first' context to see the new context as the previous */
    ctx_ptr->prev_ctx = new_ctx_ptr;

    /* Update the new context to see the current 'first' context as the next */
    new_ctx_ptr->next_ctx = ctx_ptr;
    new_ctx_ptr->prev_ctx = NULL;

    ctx_base_ptr = new_ctx_ptr;
  }

  /* Set the active context to whatever is pointed to by the base pointer */
  active_ctx_ptr = ctx_base_ptr;
  return (true);
}

/****************************************************************************************
* Function:... SFH_delete_channel
* Parameters:. channel  In  Number of the logical channel (1..N)
* Returns:.... Status
* Description: This function is used to delete a channel from the handler.
*              It must free the allocated memory.
* Created:.... DD.MM.YY by (Full name / initials)
* Modified:... DD.MM.YY by (Full name / initials)
*                Modifications note.
****************************************************************************************/
uint8_t SFH_delete_channel(uint8_t channel, char *status)
{
  SFH_ctx_type *ctx_ptr;

  PARAM_NOT_USED(status); // To remowe warning

  /* Check if the channel exists */
  ctx_ptr = get_ctx(channel);

  if (!ctx_ptr)
  {
    /* The channel does not exist */
    //    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_channel_does_not_exist, channel) );
    return (false);
  }

  /* Use this function to free memory used by the context */
  close_ctx(ctx_ptr);

  /* Remove the context from the list */
  if (ctx_ptr->prev_ctx == NULL)
  {
    if (ctx_ptr->next_ctx == NULL)
    {
      /* Context is both first and last */
      ctx_base_ptr = NULL;
    }
    else
    {
      /* context is first but not last */
      ctx_base_ptr = ctx_ptr->next_ctx;
      ((SFH_ctx_type*)ctx_ptr->next_ctx)->prev_ctx = NULL;
    }
  }
  else
  {
    if (ctx_ptr->next_ctx == NULL)
    {
      /* Context is not first but last */
      ((SFH_ctx_type*)ctx_ptr->prev_ctx)->next_ctx = NULL;
    }
    else
    {
      /* Context is somewhere in the middle. */
      ((SFH_ctx_type*)ctx_ptr->prev_ctx)->next_ctx = ctx_ptr->next_ctx;
      ((SFH_ctx_type*)ctx_ptr->next_ctx)->prev_ctx = ctx_ptr->prev_ctx;
    }
  }
  /* Free the context structure */
  MEM_free((uint8_t*)ctx_ptr);
  SFH_nof_active_instances--;  /* remember we have one less in the list */
  SFH_nof_idle = 0;            /* Make sure we do not go into idle this time around the clock (in case we deleted an idle channel with an active channel still present) */
  GLOBAL_signal_not_in_idle(); /* notify main loop that SFH is no longer in idle (needs OS dependent code to signal events to bring main loop out of idle)*/

  /* Set the active context to whatever is pointed to by the base pointer */
  active_ctx_ptr = ctx_base_ptr;
  return (true);
}

/****************************************************************************************
* Function:... SFH_get_status
* Parameters:. channel  In  Number of the logical channel (1..N)
*              status_text  Out  Status text
* Returns:.... Status
* Description: Detailed function description
* Created:.... DD.MM.YY by (Full name / initials)
* Modified:... DD.MM.YY by (Full name / initials)
*                Modifications note.
****************************************************************************************/
SFH_status_enum SFH_get_status(uint8_t channel, char *status_text)
{
  SFH_ctx_type *ctx_ptr;

  if ((0 < channel) && (channel <= MAX_NUMBER_OF_CHANNELS))
  {
    /* Check if the channel exists */
    ctx_ptr = get_ctx(channel);

    if (!ctx_ptr)
    {
      /* The channel does not exist */
      if(status_text)
        sprintf(status_text,"%s", GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_channel_does_not_exist, channel) );
      return (SFH_status_Error);
    }
    if(status_text)
    sprintf(status_text,"%s", ctx_ptr->status_text);

    return (ctx_ptr->status);
  }
  else
  {
    return (SFH_status_Error);
  }
}

/****************************************************************************************
* Function:... SFH_abort_process
* Parameters:. channel  In  Number of the logical channel (1..N)
* Returns:.... -
* Description: This function terminates the current process on the selected channel.
*              It will not return until the underlying handler has reported that it has
*              finished the abort process and is returned to its initial state.
* Created:.... DD.MM.YY by (Full name / initials)
* Modified:... DD.MM.YY by (Full name / initials)
*                Modifications note.
****************************************************************************************/
SFH_status_enum SFH_abort_process(uint8_t channel, char *status_text)
{
  SFH_ctx_type *ctx_ptr;

  /* Check if the channel exists */
  ctx_ptr = get_ctx(channel);
  if (!ctx_ptr)
  {
    /* The channel does not exist */
    //    xstrcpy(status_text, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_channel_does_not_exist, channel) );
    return (SFH_status_Error);
  }

  BOOT_abort_process(channel);
  COMM_abort_process(channel);
  PROT_abort_process(channel);
  TRA_abort_process(channel);

  close_ctx(ctx_ptr);
  init_ctx(channel, ctx_ptr, status_text);

  return (SFH_status_OK);
}
/****************************************************************************************
* Function:... SFH_download_fls_file
* Parameters:. channel  In  Number of the logical channel (1..N)
*              fls_file_name  In  Name of the FLS file.
*              use_done_signal  In  Defines how the final status is reported.
* Returns:.... -
* Description: This function is called to start the handlers processing of the FLS file download.
*              It will return immediately to allow other channels to start processing.
* Created:.... DD.MM.YY by (Full name / initials)
* Modified:... DD.MM.YY by (Full name / initials)
*                Modifications note.
****************************************************************************************/
SFH_status_enum SFH_download_fls_file (uint8_t channel,
                                       char *fls_file_name,
                                       SFH_done_signal_enum use_done_signal,
                                       char *status)
{
  SFH_ctx_type *ctx_ptr;
  FILE_status_enum file_status;
  char *status_text;
  uint32_t needed_version;
  uint32_t file_offset;
  uint32_t debug_offset;
  TocStructType toc_main;
  TocEntryStructType toc_entry;
  uint32_t i;
  uint8_t class_found;
  uint32_t TOC_index;

  if (!status)
  {
    Debug("Error in %s: Status not defined", __func__);
    return (SFH_status_Error);
  }

  /* Get the context pointer */
  ctx_ptr = get_ctx(channel);
  if (!ctx_ptr)
  {
    /* The channel does not exist */
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_channel_does_not_exist, channel) );
    goto __END__;
  }

  ctx_ptr->status_text[0] = 0 ;
  ctx_ptr->handling_flags = 0;
  if (ctx_ptr->state != SFH_state_idle)
  {
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_cant_boot_while_busy) );
    goto __END__;
  }

  i = sizeof(ctx_ptr->load_file_name);
  if (strnlen(fls_file_name, i) >= i)
    fls_file_name[i-1] = '\0';

  /* Extract the TOC index */
  if (fls_file_name[0] == (char)'|' )
  {
    //sscanf(fls_file_name+1,"%d%n",&TOC_index,&TOC_nof_chars);
    TOC_index = strtoul(fls_file_name+1, &fls_file_name, 10);
    fls_file_name++; /* Skip the "|nn|" part */

    if (TOC_index > MAX_TOC_LENGTH) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_toc_size_overflow, TOC_index ));  goto __END__;  }

    xstrcpy(ctx_ptr->load_file_name, fls_file_name);
  }
  else
  {
    TOC_index = 0;

    strncpy(ctx_ptr->load_file_name, fls_file_name, sizeof(ctx_ptr->load_file_name)-1);
  }
  ctx_ptr->load_file_name[i-1] = 0;

  if (strstr(ctx_ptr->boot_dev_cfg.boot_filename, ctx_ptr->load_file_name))
  {
    strncpy(ctx_ptr->load_file_name, ctx_ptr->boot_dev_cfg.boot_filename,sizeof(ctx_ptr->load_file_name)-1);
  }

  /* Save the boot information in the context */


  /* Check if the reference file exists */
  if (!FILE_exists(ctx_ptr->load_file_name))  { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_not_found, ctx_ptr->load_file_name)); goto __END__;  }

  /* Open the file in read mode */
  ctx_ptr->load_file = FILE_open(ctx_ptr->load_file_name, FILE_open_mode_read);
  if (ctx_ptr->load_file == NULL) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

  debug_offset = SFH_GetDebugHeaderOffset_file(ctx_ptr->load_file);
  PRGH_SetFileOffset(debug_offset);

  if (PRGH_FileCheck_File(ctx_ptr->load_file) == INVALID_FILE_TYPE)
  {
    //GLOBAL_callback(channel,IFWD_DL_ProcessOutlineUpdated, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_wrong_file_type));
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_wrong_file_type) );
    goto __END__;
  }
  needed_version = PRGH_VersionCheck_File(ctx_ptr->load_file,FLASHTOOL_TOOL_ELEM_TYPE, THIS_DLL_VERSION);
  if (needed_version)
  {
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_wrong_vesion, (needed_version >> 16), (needed_version & 0xFFFF)) );
    goto __END__;
  }

  /* Read the generic header info */
  PRGH_GetElementIndex_File(ctx_ptr->load_file,HARDWARE_ELEM_TYPE, GENERIC_UID, &file_offset);
  FILE_seek(ctx_ptr->load_file, file_offset+sizeof(ElementHeaderStructType));
  file_status = FILE_read(ctx_ptr->load_file, (uint8_t*)&(ctx_ptr->load_header.Hardware), sizeof(HardwareStructType));
  if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }


  PRGH_GetElementIndex_File(ctx_ptr->load_file,MEMORYMAP_ELEM_TYPE, GENERIC_UID, &file_offset);
  FILE_seek(ctx_ptr->load_file, file_offset+sizeof(ElementHeaderStructType));
  file_status = FILE_read(ctx_ptr->load_file, (uint8_t*)&(ctx_ptr->load_header.MemoryMap), sizeof(MemoryMapStructType));
  if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

  /* Check that the MemoryMap matches */
  if (memcmp((const void*)&ctx_ptr->load_header.MemoryMap, (const void*)&ctx_ptr->ref_header.MemoryMap, sizeof(MemoryMapStructType)))
  {
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_memory_map_mismatch, ctx_ptr->ref_file_name, ctx_ptr->load_file_name) );
    goto __END__;
  }

  /* ******************** */
  PRGH_GetElementIndex_File(ctx_ptr->load_file, TOC_ELEM_TYPE, GENERIC_UID, &file_offset);
  FILE_seek(ctx_ptr->load_file, file_offset+sizeof(ElementHeaderStructType));
  file_status = FILE_read(ctx_ptr->load_file, (uint8_t*)&(toc_main), sizeof(TocStructType));
  if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }
  FILE_seek(ctx_ptr->load_file, toc_main.DataOffset+debug_offset);

  class_found = false;
  /*  for (i=0; ((i<toc_main.NoOfEntries) && (!class_found)); i++)  */
  for (i=0; ((i<TOC_index+1) && (!class_found)); i++)
  {
    file_status = FILE_read(ctx_ptr->load_file, (uint8_t*)&(toc_entry), sizeof(TocEntryStructType));
    if (file_status != FILE_status_ok)
    {
      xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));
      goto __END__;
    }
  }
  if (toc_entry.MemoryClass >= NUM_MEM_CLASSES) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_unknown_mem_class, toc_entry.MemoryClass));  goto __END__;  }

  ctx_ptr->load_mem_class = toc_entry.MemoryClass;

  switch (ctx_ptr->load_mem_class)
  {
    case CODE_CLASS:
  {
      if (GLOBAL_erase_mode == IFWD_DL_target_erase_all ||
        GLOBAL_erase_mode == IFWD_DL_target_erase_all_incl_bad_blocks)
      {
        if (!ctx_ptr->erase_all_done)
        {
//          if (ctx_ptr->load_header.Hardware.FlashTechnology == NOR_FLASH)
//          {
            ctx_ptr->handling_flags |= HANDLE_ERASE_ALL;
            /* HACK to support erase all and no download for pathfinder */
            if (GLOBAL_boot_timeout == 9999)
            {
              ctx_ptr->handling_flags |= HANDLE_STOP_AFTER_ERASE;

            }
//          }
//          else
//          {
//            xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_erase_all_CODE) );
//            goto __END__;
//          }
        }
      }
      else
      {
        if (ctx_ptr->boot_ebl_version.boot_mode != 0xCC)
        {
          // Check if the Dyn EEP is in the memory map.
          for (i=0; i<MAX_MAP_ENTRIES; i++)
          {
            if (ctx_ptr->load_header.MemoryMap.Entry[i].Class == DYN_EEP_CLASS)
            {
              ctx_ptr->handling_flags |= HANDLE_DYN_EEP_ERASE;
              break;
            }
          }
        }
      }


      class_found = true;
      ctx_ptr->load_uid = toc_entry.UID;
      break;
    }
    case BOOT_CORE_PSI_CLASS:
    case BOOT_CORE_SLB_CLASS:
    case BOOT_CORE_EBL_CLASS:
    {
      if (GLOBAL_erase_mode == IFWD_DL_target_erase_all ||
        GLOBAL_erase_mode == IFWD_DL_target_erase_all_incl_bad_blocks)
      {
#if 0
        if (ctx_ptr->load_header.Hardware.FlashTechnology == NOR_FLASH)
        {
          xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_erase_all_NOR) ); goto __END__;
        }
        else
#endif
        {
          if (!ctx_ptr->erase_all_done)
          {
            if(GLOBAL_erase_mode == IFWD_DL_target_erase_all_incl_bad_blocks)
              ctx_ptr->handling_flags |= HANDLE_ERASE_ALL_BAD;
            else
              ctx_ptr->handling_flags |= HANDLE_ERASE_ALL;
          }
        }
      }
      class_found = true;
      ctx_ptr->load_uid = toc_entry.UID;
      break;
    }
  }

  if (!class_found)
  {
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_mem_class_not_found ));
    goto __END__;
  }

  PRGH_GetElementIndex_File(ctx_ptr->load_file,SECURITY_ELEM_TYPE, ctx_ptr->load_uid, &file_offset);
  FILE_seek(ctx_ptr->load_file, file_offset+sizeof(ElementHeaderStructType));
  file_status = FILE_read(ctx_ptr->load_file, (uint8_t*)&(ctx_ptr->load_header.Security), sizeof(SecurityStructType));
  if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

  // Get filepointer to the data element.
  PRGH_GetElementIndex_File(ctx_ptr->load_file,DOWNLOADDATA_ELEM_TYPE, ctx_ptr->load_uid, &file_offset);
  FILE_seek(ctx_ptr->load_file, file_offset+sizeof(ElementHeaderStructType));
  file_status = FILE_read(ctx_ptr->load_file, (uint8_t*)&(ctx_ptr->download_data_struct), sizeof(DownloadDataStructType));
  FILE_seek(ctx_ptr->load_file, ctx_ptr->download_data_struct.DataOffset);
  /* Decrypt sec pack if needed and possible */
  if(!DecryptSecPack(&ctx_ptr->load_header.Security, &status_text, NULL))
  {
    xstrcpy(status, status_text );
    goto __END__;
  }
  /* Check for special handling */
  /* Use LoadMap ? */
  if (ctx_ptr->load_header.Security.LoadMagic == LOAD_MAGIC)
  {
    ctx_ptr->handling_flags |= HANDLE_LOADMAP;
    ctx_ptr->loadmap_index = 0;
  }
  if (GLOBAL_use_pre_erase)
  {
    ctx_ptr->handling_flags |= HANDLE_PREERASE;
  }

  ctx_ptr->handling_flags |= HANDLE_FLS;
  if (GLOBAL_force_area_erase)
  {
    ctx_ptr->handling_flags |= HANDLE_ERASE_TOTAL_LEN;
  }

  GLOBAL_callback(channel,IFWD_DL_ProcessOutlineUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_load_start, mem_class_name[toc_entry.MemoryClass], toc_entry.FileName, ctx_ptr->load_file_name));

  /* Set-up the variables used by the load state-machine if not overruled by special handling */

  ctx_ptr->load_address     = ctx_ptr->load_header.Security.LoadMap[0].StartAddr;
  ctx_ptr->load_length      = ctx_ptr->load_header.Security.LoadMap[0].UsedLength;
  ctx_ptr->load_file_offset = ctx_ptr->download_data_struct.DataOffset;
  ctx_ptr->exp_checksum     = 0;

  /* Extract the eeprom version to be used if the eeprom is updated later. */
  FILE_seek(ctx_ptr->load_file, ctx_ptr->load_file_offset + (ctx_ptr->load_header.MemoryMap.EepRevAddrInSw - ctx_ptr->load_address));
  FILE_read(ctx_ptr->load_file, (uint8_t*)&(ctx_ptr->new_eeprom_ver), 2);


  /* Set the state of the machine to 'boot' */
  ctx_ptr->state = SFH_state_load;
  ctx_ptr->done_signal = use_done_signal;

  /* Closing down the file, and returns */
__END__:
  /**
   *  KlocWork NOTE: We cannot close the file, since it is read later on...
   *  TODO: Need to make a work around
   **/
  if (status && status[0])
  {
    if (ctx_ptr && ctx_ptr->load_file)
    {
      if (FILE_close(ctx_ptr->load_file) != FILE_status_ok)
        xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));
      ctx_ptr->load_file = NULL;
    }
    return (SFH_status_Error);
  }
  else if (use_done_signal != SFH_done_signal_callback)
  {
    return (SFH_status_wait_for_event);
  }
  else
  {
    return (SFH_status_OK);
  }
}
/****************************************************************************************
* Function:... SFH_download_eep_file
* Parameters:. channel  In  Number of the logical channel (1..N)
*              eep_file_name  In  Name of the EEP file.
*              boot_mode  In  Defines if the target starts in normal or test mode after download.
*              use_done_signal  In  Defines how the final status is reported.
* Returns:.... -
* Description: This function is called to start the handlers processing of the EEP file download.
*              It will return immediately to allow other channels to start processing.
* Created:.... DD.MM.YY by (Full name / initials)
* Modified:... DD.MM.YY by (Full name / initials)
*                Modifications note.
****************************************************************************************/
SFH_status_enum SFH_download_eep_file (uint8_t channel,
                                       char *eep_file_name,
                                       SFH_target_boot_mode_enum boot_mode,
                                       SFH_done_signal_enum use_done_signal,
                                       char *status)
{
  SFH_ctx_type *ctx_ptr;
  FILE_status_enum file_status;
  char *status_text;
  uint32_t needed_version;
  uint32_t file_offset;
  uint32_t debug_offset;
  TocStructType toc_main;
  TocEntryStructType toc_entry;
  uint32_t i;
  uint8_t class_found;
  uint32_t TOC_index;

  if (!status)
  {
    Debug("Error in %s: Status not defined", __func__);
    return (SFH_status_Error);
  }

  /* Get the context pointer */
  ctx_ptr = get_ctx(channel);
  if (!ctx_ptr)
  {
    /* The channel does not exist */
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_channel_does_not_exist, channel) );
    goto __END__;
  }

  ctx_ptr->status_text[0] = 0 ;
  ctx_ptr->handling_flags = 0;
  if (ctx_ptr->state != SFH_state_idle) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_cant_boot_while_busy) ); goto __END__;  }

  /* Extract the TOC index */
  if (eep_file_name[0] == (char)'|' )
  {
    //sscanf(eep_file_name+1,"%d%n",&TOC_index,&TOC_nof_chars);
    TOC_index = strtoul(eep_file_name+1, &eep_file_name, 10);

    if (TOC_index > MAX_TOC_LENGTH) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_toc_size_overflow, TOC_index ));  goto __END__;  }
    xstrcpy(ctx_ptr->load_file_name, eep_file_name+1); /* Skip the "|nn|" part */
  }
  else
  {
    TOC_index = 0;
    xstrcpy(ctx_ptr->load_file_name, eep_file_name);
  }

  /* Save the boot information in the context */


  /* Check if the reference file exists */
  if (!FILE_exists(ctx_ptr->load_file_name))  { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_not_found, ctx_ptr->load_file_name)); goto __END__;  }

  /* Open the file in read mode */
  ctx_ptr->load_file = FILE_open(ctx_ptr->load_file_name, FILE_open_mode_read);
  if (ctx_ptr->load_file == NULL) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

  debug_offset = SFH_GetDebugHeaderOffset_file(ctx_ptr->load_file);
  PRGH_SetFileOffset(debug_offset);

  if (PRGH_FileCheck_File(ctx_ptr->load_file) == INVALID_FILE_TYPE)
  {
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_wrong_file_type) );
    goto __END__;
  }
  needed_version = PRGH_VersionCheck_File(ctx_ptr->load_file,FLASHTOOL_TOOL_ELEM_TYPE, THIS_DLL_VERSION);
  if (needed_version)
  {
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_wrong_vesion, (needed_version >> 16), (needed_version & 0xFFFF)) );
    goto __END__;
  }

  /* Read the generic header info */
  PRGH_GetElementIndex_File(ctx_ptr->load_file,HARDWARE_ELEM_TYPE, GENERIC_UID, &file_offset);
  FILE_seek(ctx_ptr->load_file, file_offset+sizeof(ElementHeaderStructType));
  file_status = FILE_read(ctx_ptr->load_file, (uint8_t*)&(ctx_ptr->load_header.Hardware), sizeof(HardwareStructType));
  if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }


  PRGH_GetElementIndex_File(ctx_ptr->load_file,MEMORYMAP_ELEM_TYPE, GENERIC_UID, &file_offset);
  FILE_seek(ctx_ptr->load_file, file_offset+sizeof(ElementHeaderStructType));
  file_status = FILE_read(ctx_ptr->load_file, (uint8_t*)&(ctx_ptr->load_header.MemoryMap), sizeof(MemoryMapStructType));
  if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }


  PRGH_GetElementIndex_File(ctx_ptr->load_file, TOC_ELEM_TYPE, GENERIC_UID, &file_offset);
  FILE_seek(ctx_ptr->load_file, file_offset+sizeof(ElementHeaderStructType));
  file_status = FILE_read(ctx_ptr->load_file, (uint8_t*)&(toc_main), sizeof(TocStructType));
  if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }
  FILE_seek(ctx_ptr->load_file, toc_main.DataOffset+debug_offset);

  class_found = false;
  for (i=0; ((i<TOC_index+1) && (!class_found)); i++)
  {
    file_status = FILE_read(ctx_ptr->load_file, (uint8_t*)&(toc_entry), sizeof(TocEntryStructType));
  }

  {
    if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }
    if (toc_entry.MemoryClass >= NUM_MEM_CLASSES) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_unknown_mem_class, toc_entry.MemoryClass));  goto __END__;  }

    ctx_ptr->load_mem_class = toc_entry.MemoryClass;

    switch (ctx_ptr->load_mem_class)
    {
      case STAT_EEP_CLASS:
      {
        if (GLOBAL_erase_mode == IFWD_DL_target_erase_all ||
          GLOBAL_erase_mode == IFWD_DL_target_erase_all_incl_bad_blocks)
        {
          if (!ctx_ptr->erase_all_done)
          {
            xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_erase_all_CODE) ); goto __END__;
          }
        }
        class_found = true;
        ctx_ptr->load_uid = toc_entry.UID;
        break;
      }
    }

  }

  if (!class_found)
  {
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_mem_class_not_found ));
    goto __END__;
  }
  PRGH_GetElementIndex_File(ctx_ptr->load_file,SECURITY_ELEM_TYPE, ctx_ptr->load_uid, &file_offset);
  FILE_seek(ctx_ptr->load_file, file_offset+sizeof(ElementHeaderStructType));
  file_status = FILE_read(ctx_ptr->load_file, (uint8_t*)&(ctx_ptr->load_header.Security), sizeof(SecurityStructType));
  if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

  // Get filepointer to the data element.
  PRGH_GetElementIndex_File(ctx_ptr->load_file,DOWNLOADDATA_ELEM_TYPE, ctx_ptr->load_uid, &file_offset);
  FILE_seek(ctx_ptr->load_file, file_offset+sizeof(ElementHeaderStructType));
  file_status = FILE_read(ctx_ptr->load_file, (uint8_t*)&(ctx_ptr->download_data_struct), sizeof(DownloadDataStructType));
  FILE_seek(ctx_ptr->load_file, ctx_ptr->download_data_struct.DataOffset);

  /* Decrypt sec pack if needed and possible */
  if(!DecryptSecPack(&ctx_ptr->load_header.Security, &status_text, ctx_ptr->load_file))
  {
    xstrcpy(status, status_text );
    goto __END__;
  }

  /* Check for special handling */
  /* Use LoadMap ? */
  if (ctx_ptr->load_header.Security.LoadMagic == LOAD_MAGIC)
  {
    ctx_ptr->handling_flags |= HANDLE_LOADMAP;
    ctx_ptr->loadmap_index = 0;
  }
  if (GLOBAL_use_pre_erase)
  {
    ctx_ptr->handling_flags |= HANDLE_PREERASE;
  }

  if (GLOBAL_force_area_erase)
  {
    ctx_ptr->handling_flags |= HANDLE_ERASE_TOTAL_LEN;
  }

  ctx_ptr->handling_flags |= HANDLE_EEP;
  if (boot_mode == SFH_target_boot_mode_test)
  {
    ctx_ptr->handling_flags |= HANDLE_EEP_TEST_MODE;
  }

  GLOBAL_callback(channel,IFWD_DL_ProcessOutlineUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_load_start, mem_class_name[toc_entry.MemoryClass], toc_entry.FileName, ctx_ptr->load_file_name));


  ctx_ptr->load_address     = ctx_ptr->load_header.Security.LoadMap[0].StartAddr;
  ctx_ptr->load_length      = ctx_ptr->load_header.Security.LoadMap[0].UsedLength;
  ctx_ptr->load_file_offset = ctx_ptr->download_data_struct.DataOffset;
  ctx_ptr->exp_checksum     = 0;


  /* Set the state of the machine to 'boot' */
  ctx_ptr->state = SFH_state_load;
  ctx_ptr->done_signal = use_done_signal;

  /* Closing down the file, and returns */
__END__:
  /**
   *  KlocWork NOTE: We cannot close the file, since it is read later on...
   *  TODO: Need to make a work around
   **/
  if (status && status[0])
  {
    if (ctx_ptr && ctx_ptr->load_file)
    {
      if (FILE_close(ctx_ptr->load_file) != FILE_status_ok)
        xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));
      ctx_ptr->load_file = NULL;
    }
    return (SFH_status_Error);
  }
  else if (use_done_signal != SFH_done_signal_callback)
  {
    return (SFH_status_wait_for_event);
  }
  else
  {
    return (SFH_status_OK);
  }
}
/****************************************************************************************
* Function:... SFH_download_dffs_file
* Parameters:. channel  In  Number of the logical channel (1..N)
*              dffs_file_name  In  Name of the DFFS file.
*              load_selection  In  Defines if the static and/or the dynamic part of the DFFS file is downloaded.
*              use_done_signal  In  Defines how the final status is reported.
* Returns:.... -
* Description: This function is called to start the handlers processing of the DFFS file download.
*              It will return immediately to allow other channels to start processing.
* Created:.... DD.MM.YY by (Full name / initials)
* Modified:... DD.MM.YY by (Full name / initials)
*                Modifications note.
****************************************************************************************/
SFH_status_enum SFH_download_dffs_file (uint8_t channel,
                                        char *dffs_file_name,
                                        SFH_dffs_load_sel_enum load_selection,
                                        SFH_done_signal_enum use_done_signal,
                                        char *status)
{
  SFH_ctx_type *ctx_ptr;
  FILE_status_enum file_status;
  char *status_text;
  uint32_t needed_version;
  uint32_t file_offset;
  uint32_t debug_offset;
  TocStructType toc_main;
  TocEntryStructType toc_entry;
  uint32_t i;
  uint8_t class_found;
  uint32_t TOC_index;

  if (!status)
  {
    Debug("Error in %s: Status not defined", __func__);
    return (SFH_status_Error);
  }

  /* Get the context pointer */
  ctx_ptr = get_ctx(channel);
  if (!ctx_ptr)
  {
    /* The channel does not exist */
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_channel_does_not_exist, channel) );
    goto __END__;
  }

  ctx_ptr->status_text[0] = 0 ;
  ctx_ptr->handling_flags = 0;
  if (ctx_ptr->state != SFH_state_idle) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_cant_boot_while_busy) ); goto __END__;  }

  /* Extract the TOC index */
  if (dffs_file_name[0] == (char)'|' )
  {
    //sscanf(dffs_file_name+1,"%d%n",&TOC_index,&TOC_nof_chars);
    TOC_index = strtoul(dffs_file_name+1, &dffs_file_name, 10);
    if (TOC_index > MAX_TOC_LENGTH) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_toc_size_overflow, TOC_index ));  goto __END__;  }
    xstrcpy(ctx_ptr->load_file_name, dffs_file_name+1); /* Skip the "|nn|" part */
  }
  else
  {
    TOC_index = 0;
    xstrcpy(ctx_ptr->load_file_name, dffs_file_name);
  }

  /* Save the boot information in the context */

  /* Check if the reference file exists */
  if (!FILE_exists(ctx_ptr->load_file_name))  { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_not_found, ctx_ptr->load_file_name)); goto __END__;  }

  /* Open the file in read mode */
  ctx_ptr->load_file = FILE_open(ctx_ptr->load_file_name, FILE_open_mode_read);
  if (ctx_ptr->load_file == NULL) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

  debug_offset = SFH_GetDebugHeaderOffset_file(ctx_ptr->load_file);
  PRGH_SetFileOffset(debug_offset);

  if (PRGH_FileCheck_File(ctx_ptr->load_file) == INVALID_FILE_TYPE)
  {
    //GLOBAL_callback(channel,IFWD_DL_ProcessOutlineUpdated, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_wrong_file_type));
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_wrong_file_type) );
    goto __END__;
  }
  needed_version = PRGH_VersionCheck_File(ctx_ptr->load_file,FLASHTOOL_TOOL_ELEM_TYPE, THIS_DLL_VERSION);
  if (needed_version)
  {
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_wrong_vesion, (needed_version >> 16), (needed_version & 0xFFFF)) );
    goto __END__;
  }

  /* Read the generic header info */
  PRGH_GetElementIndex_File(ctx_ptr->load_file,HARDWARE_ELEM_TYPE, GENERIC_UID, &file_offset);
  FILE_seek(ctx_ptr->load_file, file_offset+sizeof(ElementHeaderStructType));
  file_status = FILE_read(ctx_ptr->load_file, (uint8_t*)&(ctx_ptr->load_header.Hardware), sizeof(HardwareStructType));
  if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }


  PRGH_GetElementIndex_File(ctx_ptr->load_file,MEMORYMAP_ELEM_TYPE, GENERIC_UID, &file_offset);
  FILE_seek(ctx_ptr->load_file, file_offset+sizeof(ElementHeaderStructType));
  file_status = FILE_read(ctx_ptr->load_file, (uint8_t*)&(ctx_ptr->load_header.MemoryMap), sizeof(MemoryMapStructType));
  if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }


  /* Check that the MemoryMap matches */
  if (memcmp((const void*)&ctx_ptr->load_header.MemoryMap, (const void*)&ctx_ptr->ref_header.MemoryMap, sizeof(MemoryMapStructType)))
  {
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_memory_map_mismatch, ctx_ptr->ref_file_name, ctx_ptr->load_file_name) ); goto __END__;
  }



  /* ******************** */
  PRGH_GetElementIndex_File(ctx_ptr->load_file, TOC_ELEM_TYPE, GENERIC_UID, &file_offset);
  FILE_seek(ctx_ptr->load_file, file_offset+sizeof(ElementHeaderStructType));
  file_status = FILE_read(ctx_ptr->load_file, (uint8_t*)&(toc_main), sizeof(TocStructType));
  if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }
  FILE_seek(ctx_ptr->load_file, toc_main.DataOffset+debug_offset);


  if (GLOBAL_erase_mode == IFWD_DL_target_erase_all ||
    GLOBAL_erase_mode == IFWD_DL_target_erase_all_incl_bad_blocks)
  {
    if (!ctx_ptr->erase_all_done)
    {
      xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_erase_all_CODE) ); goto __END__;
    }
  }


  class_found = false;
  class_found =   class_found;
  for (i=0; ((i<TOC_index+1) && (!class_found)); i++)
  {
    file_status = FILE_read(ctx_ptr->load_file, (uint8_t*)&(toc_entry), sizeof(TocEntryStructType));
  }
  {
    if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }
    if (toc_entry.MemoryClass >= NUM_MEM_CLASSES) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_unknown_mem_class, toc_entry.MemoryClass));  goto __END__;  }

    ctx_ptr->load_mem_class = toc_entry.MemoryClass;

    if ((load_selection == SFH_dffs_load_sel_static) && (ctx_ptr->load_mem_class == STAT_FFS_CLASS))
    {
      class_found = true;
      ctx_ptr->load_uid = toc_entry.UID;
    }
    else if ((load_selection == SFH_dffs_load_sel_dynamic) && (ctx_ptr->load_mem_class == DYN_FFS_CLASS))
    {
      class_found = true;
      ctx_ptr->load_uid = toc_entry.UID;
    }
    else
      if (load_selection == SFH_dffs_load_sel_nand_disk)
      {
        class_found = true;
        ctx_ptr->load_uid = toc_entry.UID;
      }

  }

  if (!class_found)
  {
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_mem_class_not_found ));
    goto __END__;
  }

  PRGH_GetElementIndex_File(ctx_ptr->load_file,SECURITY_ELEM_TYPE, ctx_ptr->load_uid, &file_offset);
  FILE_seek(ctx_ptr->load_file, file_offset+sizeof(ElementHeaderStructType));
  file_status = FILE_read(ctx_ptr->load_file, (uint8_t*)&(ctx_ptr->load_header.Security), sizeof(SecurityStructType));
  if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

  // Get filepointer to the data element.
  PRGH_GetElementIndex_File(ctx_ptr->load_file,DOWNLOADDATA_ELEM_TYPE, ctx_ptr->load_uid, &file_offset);
  FILE_seek(ctx_ptr->load_file, file_offset+sizeof(ElementHeaderStructType));
  file_status = FILE_read(ctx_ptr->load_file, (uint8_t*)&(ctx_ptr->download_data_struct), sizeof(DownloadDataStructType));
  FILE_seek(ctx_ptr->load_file, ctx_ptr->download_data_struct.DataOffset);

  /* Decrypt sec pack if needed and possible */
  if(!DecryptSecPack(&ctx_ptr->load_header.Security, &status_text, NULL))
  {
    xstrcpy(status, status_text );
    goto __END__;
  }
  /* Check for special handling */
  /* Use LoadMap ? */
  if (ctx_ptr->load_header.Security.LoadMagic == LOAD_MAGIC)
  {
    ctx_ptr->handling_flags |= HANDLE_LOADMAP;
    ctx_ptr->loadmap_index = 0;
  }
  if (GLOBAL_use_pre_erase)
  {
    ctx_ptr->handling_flags |= HANDLE_PREERASE;
  }

  if (GLOBAL_force_area_erase)
  {
    ctx_ptr->handling_flags |= HANDLE_ERASE_TOTAL_LEN;
  }

  GLOBAL_callback(channel,IFWD_DL_ProcessOutlineUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_load_start, mem_class_name[toc_entry.MemoryClass], toc_entry.FileName, ctx_ptr->load_file_name));

  /* Set-up the variables used by the load state-machine if not overruled by special handling */

  ctx_ptr->load_address     = ctx_ptr->load_header.Security.LoadMap[0].StartAddr;
  ctx_ptr->load_length      = ctx_ptr->load_header.Security.LoadMap[0].UsedLength;
  ctx_ptr->load_file_offset = ctx_ptr->download_data_struct.DataOffset;
  ctx_ptr->exp_checksum     = 0;


  /* Set the state of the machine to 'boot' */
  ctx_ptr->state = SFH_state_load;
  ctx_ptr->done_signal = use_done_signal;

  /* Closing down the file, and returns */
__END__:
  /**
   *  KlocWork NOTE: We cannot close the file, since it is read later on...
   *  TODO: Need to make a work around
   **/
  if (status && status[0])
  {
    if (ctx_ptr && ctx_ptr->load_file)
    {
      if (FILE_close(ctx_ptr->load_file) != FILE_status_ok)
        xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));
      ctx_ptr->load_file = NULL;
    }
    return (SFH_status_Error);
  }
  else if (use_done_signal != SFH_done_signal_callback)
  {
    return (SFH_status_wait_for_event);
  }
  else
  {
    return (SFH_status_OK);
  }
}
/****************************************************************************************
* Function:... SFH_download_cust_file
* Parameters:. channel  In  Number of the logical channel (1..N)
*              cust_file_name  In  Name of the CUST file.
*              use_done_signal  In  Defines how the final status is reported.
* Returns:.... -
* Description: This function is called to start the handlers processing of the CUST file download.
*              It will return immediately to allow other channels to start processing.
* Created:.... DD.MM.YY by (Full name / initials)
* Modified:... DD.MM.YY by (Full name / initials)
*                Modifications note.
****************************************************************************************/
SFH_status_enum SFH_download_cust_file (uint8_t channel,
                                        char *cust_file_name,
                                        SFH_done_signal_enum use_done_signal,
                                        char *status)
{
  SFH_ctx_type *ctx_ptr;
  FILE_status_enum file_status;
  char *status_text;
  uint32_t needed_version;
  uint32_t file_offset;
  uint32_t debug_offset;
  TocStructType toc_main;
  TocEntryStructType toc_entry;
  uint32_t i;
  uint8_t class_found;
  uint32_t TOC_index;

  if (!status)
  {
    Debug("Error in %s: Status not defined", __func__);
    return (SFH_status_Error);
  }

  /* Get the context pointer */
  ctx_ptr = get_ctx(channel);
  if (!ctx_ptr)
  {
    /* The channel does not exist */
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_channel_does_not_exist, channel) );
    goto __END__;
  }

  ctx_ptr->status_text[0] = 0 ;
  ctx_ptr->handling_flags = 0;
  if (ctx_ptr->state != SFH_state_idle) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_cant_boot_while_busy) ); goto __END__;  }

  /* Extract the TOC index */
  if (cust_file_name[0] == (char)'|' )
  {
    //sscanf(cust_file_name+1,"%d%n",&TOC_index,&TOC_nof_chars);
    TOC_index = strtoul(cust_file_name+1, &cust_file_name, 10);
    if (TOC_index > MAX_TOC_LENGTH) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_toc_size_overflow, TOC_index ));  goto __END__;  }
    xstrcpy(ctx_ptr->load_file_name, cust_file_name+1); /* Skip the "|nn|" part */
  }
  else
  {
    TOC_index = 0;
    xstrcpy(ctx_ptr->load_file_name, cust_file_name);
  }

  /* Save the boot information in the context */

  /* Check if the reference file exists */
  if (!FILE_exists(ctx_ptr->load_file_name))  { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_not_found, ctx_ptr->load_file_name)); goto __END__;  }

  /* Open the file in read mode */
  ctx_ptr->load_file = FILE_open(ctx_ptr->load_file_name, FILE_open_mode_read);
  if (ctx_ptr->load_file == NULL) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

  debug_offset = SFH_GetDebugHeaderOffset_file(ctx_ptr->load_file);
  PRGH_SetFileOffset(debug_offset);

  if (PRGH_FileCheck_File(ctx_ptr->load_file) == INVALID_FILE_TYPE)
  {
    //GLOBAL_callback(channel,IFWD_DL_ProcessOutlineUpdated, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_wrong_file_type));
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_wrong_file_type) );
    goto __END__;
  }
  needed_version = PRGH_VersionCheck_File(ctx_ptr->load_file,FLASHTOOL_TOOL_ELEM_TYPE, THIS_DLL_VERSION);
  if (needed_version)
  {
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_wrong_vesion, (needed_version >> 16), (needed_version & 0xFFFF)) );
    goto __END__;
  }

  /* Read the generic header info */
  PRGH_GetElementIndex_File(ctx_ptr->load_file,HARDWARE_ELEM_TYPE, GENERIC_UID, &file_offset);
  FILE_seek(ctx_ptr->load_file, file_offset+sizeof(ElementHeaderStructType));
  file_status = FILE_read(ctx_ptr->load_file, (uint8_t*)&(ctx_ptr->load_header.Hardware), sizeof(HardwareStructType));
  if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }


  PRGH_GetElementIndex_File(ctx_ptr->load_file,MEMORYMAP_ELEM_TYPE, GENERIC_UID, &file_offset);
  FILE_seek(ctx_ptr->load_file, file_offset+sizeof(ElementHeaderStructType));
  file_status = FILE_read(ctx_ptr->load_file, (uint8_t*)&(ctx_ptr->load_header.MemoryMap), sizeof(MemoryMapStructType));
  if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }


  /* Check that the MemoryMap matches */
  if (memcmp((const void*)&ctx_ptr->load_header.MemoryMap, (const void*)&ctx_ptr->ref_header.MemoryMap, sizeof(MemoryMapStructType)))
  {
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_memory_map_mismatch, ctx_ptr->ref_file_name, ctx_ptr->load_file_name) ); goto __END__;
  }


  /* ******************** */
  PRGH_GetElementIndex_File(ctx_ptr->load_file, TOC_ELEM_TYPE, GENERIC_UID, &file_offset);
  FILE_seek(ctx_ptr->load_file, file_offset+sizeof(ElementHeaderStructType));
  file_status = FILE_read(ctx_ptr->load_file, (uint8_t*)&(toc_main), sizeof(TocStructType));
  if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }
  FILE_seek(ctx_ptr->load_file, toc_main.DataOffset+debug_offset);

  class_found = false;
  for (i=0; ((i<TOC_index+1) && (!class_found)); i++)
  {
    file_status = FILE_read(ctx_ptr->load_file, (uint8_t*)&(toc_entry), sizeof(TocEntryStructType));
  }
  {
    if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }
    if (toc_entry.MemoryClass >= NUM_MEM_CLASSES) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_unknown_mem_class, toc_entry.MemoryClass));  goto __END__;  }

    ctx_ptr->load_mem_class = toc_entry.MemoryClass;

    switch (ctx_ptr->load_mem_class)
    {
      case CUST_CLASS:
      case DSP_SW_CLASS:
      {
        if (GLOBAL_erase_mode == IFWD_DL_target_erase_all ||
          GLOBAL_erase_mode == IFWD_DL_target_erase_all_incl_bad_blocks)
        {
          if (!ctx_ptr->erase_all_done)
          {
            xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_erase_all_CODE) ); goto __END__;
          }
        }
        class_found = true;
        ctx_ptr->load_uid = toc_entry.UID;
        break;
      }
    }
  }

  if (!class_found)
  {
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_mem_class_not_found ));
    goto __END__;
  }

  PRGH_GetElementIndex_File(ctx_ptr->load_file,SECURITY_ELEM_TYPE, ctx_ptr->load_uid, &file_offset);
  FILE_seek(ctx_ptr->load_file, file_offset+sizeof(ElementHeaderStructType));
  file_status = FILE_read(ctx_ptr->load_file, (uint8_t*)&(ctx_ptr->load_header.Security), sizeof(SecurityStructType));
  if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

  // Get filepointer to the data element.
  PRGH_GetElementIndex_File(ctx_ptr->load_file,DOWNLOADDATA_ELEM_TYPE, ctx_ptr->load_uid, &file_offset);
  FILE_seek(ctx_ptr->load_file, file_offset+sizeof(ElementHeaderStructType));
  file_status = FILE_read(ctx_ptr->load_file, (uint8_t*)&(ctx_ptr->download_data_struct), sizeof(DownloadDataStructType));
  FILE_seek(ctx_ptr->load_file, ctx_ptr->download_data_struct.DataOffset);

  /* Decrypt sec pack if needed and possible */
  if(!DecryptSecPack(&ctx_ptr->load_header.Security, &status_text, NULL))
  {
    xstrcpy(status, status_text );
    goto __END__;
  }
  /* Check for special handling */
  /* Use LoadMap ? */
  if (ctx_ptr->load_header.Security.LoadMagic == LOAD_MAGIC)
  {
    ctx_ptr->handling_flags |= HANDLE_LOADMAP;
    ctx_ptr->loadmap_index = 0;
  }
  if (GLOBAL_use_pre_erase)
  {
    ctx_ptr->handling_flags |= HANDLE_PREERASE;
  }

  if (GLOBAL_force_area_erase)
  {
    ctx_ptr->handling_flags |= HANDLE_ERASE_TOTAL_LEN;
  }


  GLOBAL_callback(channel,IFWD_DL_ProcessOutlineUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_load_start, mem_class_name[toc_entry.MemoryClass], toc_entry.FileName, ctx_ptr->load_file_name));

  /* Set-up the variables used by the load state-machine if not overruled by special handling */

  ctx_ptr->load_address     = ctx_ptr->load_header.Security.LoadMap[0].StartAddr;
  ctx_ptr->load_length      = ctx_ptr->load_header.Security.LoadMap[0].UsedLength;
  ctx_ptr->load_file_offset = ctx_ptr->download_data_struct.DataOffset;
  ctx_ptr->exp_checksum     = 0;


  /* Set the state of the machine to 'boot' */
  ctx_ptr->state = SFH_state_load;
  ctx_ptr->done_signal = use_done_signal;

  /* Closing down the file, and returns */
__END__:
  /**
   *  KlocWork NOTE: We cannot close the file, since it is read later on...
   *  TODO: Need to make a work around
   **/
  if (status && status[0])
  {
    if (ctx_ptr && ctx_ptr->load_file)
    {
      if (FILE_close(ctx_ptr->load_file) != FILE_status_ok)
        xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));
      ctx_ptr->load_file = NULL;
    }
    return (SFH_status_Error);
  }
  else if (use_done_signal != SFH_done_signal_callback)
  {
    return (SFH_status_wait_for_event);
  }
  else
  {
    return (SFH_status_OK);
  }
}

/****************************************************************************************
* Function:... SFH_update_static_eeprom
* Parameters:. channel  In  Number of the logical channel (1..N)
*              dep_file_name  In  Name of the DEP file.
*              project_prefix_string  In  Project prefix string.
*              ref_eep_file_name  In  The name of the EEP file to be used as reference.
*              use_done_signal  In  Defines how the final status is reported.
* Returns:.... -
* Description: This function is called to start the handlers processing of the static EEPROM update.
*              It will return immediately to allow other channels to start processing.
* Created:.... DD.MM.YY by (Full name / initials)
* Modified:... DD.MM.YY by (Full name / initials)
*                Modifications note.
****************************************************************************************/
SFH_status_enum SFH_update_static_eeprom (uint8_t channel,
                                          char *dep_file_name,
                                          char *project_prefix_string,
                                          char *ref_eep_file_name,
                                          SFH_done_signal_enum use_done_signal,
                                          char *status)
{
  /* Do the checks and propare the upload phase */
  SFH_ctx_type *ctx_ptr;
  FILE_status_enum file_status;
  char *status_text;
  uint32_t needed_version;
  uint32_t file_offset;
  uint32_t debug_offset;
  TocStructType toc_main;
  TocEntryStructType toc_entry;
  uint32_t i;
  uint8_t class_found;
  uint32_t TOC_index;

  if (!status)
  {
    Debug("Error in %s: Status not defined", __func__);
    return (SFH_status_Error);
  }

  /* Get the context pointer */
  ctx_ptr = get_ctx(channel);
  if (!ctx_ptr)
  {
    /* The channel does not exist */
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_channel_does_not_exist, channel) );
    goto __END__;
  }

  ctx_ptr->status_text[0] = 0 ;
  ctx_ptr->handling_flags = 0;

  if (ctx_ptr->state != SFH_state_idle) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_not_in_idle) ); goto __END__;  }

  /* Extract the TOC index */
  if (ref_eep_file_name[0] == (char)'|' )
  {
    //sscanf(ref_eep_file_name+1,"%d%n",&TOC_index,&TOC_nof_chars);
    TOC_index = strtoul(ref_eep_file_name+1, &ref_eep_file_name, 10);
    if (TOC_index > MAX_TOC_LENGTH) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_toc_size_overflow, TOC_index ));  goto __END__;  }
    xstrcpy(ctx_ptr->load_file_name, ref_eep_file_name+1); /* Skip the "|nn|" part */
  }
  else
  {
    TOC_index = 0;
    xstrcpy(ctx_ptr->load_file_name, ref_eep_file_name);
  }

  /* Save the boot information in the context */
  strncpy(ctx_ptr->dep_file_name, dep_file_name, sizeof(ctx_ptr->dep_file_name)-1);
  strncpy(ctx_ptr->project_prefix, project_prefix_string,sizeof(ctx_ptr->project_prefix)-1);

  GLOBAL_callback(channel,IFWD_DL_ProcessOutlineUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_update_eep,ctx_ptr->dep_file_name));



  /* Check if the reference file exists */
  if (!FILE_exists(ctx_ptr->load_file_name))  { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_not_found, ctx_ptr->load_file_name)); goto __END__;  }

  /* Open the file in read mode */
  ctx_ptr->load_file = FILE_open(ctx_ptr->load_file_name, FILE_open_mode_read);
  if (ctx_ptr->load_file == NULL) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

  debug_offset = SFH_GetDebugHeaderOffset_file(ctx_ptr->load_file);
  PRGH_SetFileOffset(debug_offset);

  if (PRGH_FileCheck_File(ctx_ptr->load_file) == INVALID_FILE_TYPE)
  {
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_wrong_file_type) );
    goto __END__;
  }
  needed_version = PRGH_VersionCheck_File(ctx_ptr->load_file,FLASHTOOL_TOOL_ELEM_TYPE, THIS_DLL_VERSION);
  if (needed_version)
  {
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_wrong_vesion, (needed_version >> 16), (needed_version & 0xFFFF)) );
    goto __END__;
  }

  /* Read the generic header info */
  PRGH_GetElementIndex_File(ctx_ptr->load_file,HARDWARE_ELEM_TYPE, GENERIC_UID, &file_offset);
  FILE_seek(ctx_ptr->load_file, file_offset+sizeof(ElementHeaderStructType));
  file_status = FILE_read(ctx_ptr->load_file, (uint8_t*)&(ctx_ptr->load_header.Hardware), sizeof(HardwareStructType));
  if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }


  PRGH_GetElementIndex_File(ctx_ptr->load_file,MEMORYMAP_ELEM_TYPE, GENERIC_UID, &file_offset);
  FILE_seek(ctx_ptr->load_file, file_offset+sizeof(ElementHeaderStructType));
  file_status = FILE_read(ctx_ptr->load_file, (uint8_t*)&(ctx_ptr->load_header.MemoryMap), sizeof(MemoryMapStructType));
  if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }


  PRGH_GetElementIndex_File(ctx_ptr->load_file, TOC_ELEM_TYPE, GENERIC_UID, &file_offset);
  FILE_seek(ctx_ptr->load_file, file_offset+sizeof(ElementHeaderStructType));
  file_status = FILE_read(ctx_ptr->load_file, (uint8_t*)&(toc_main), sizeof(TocStructType));
  if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }
  FILE_seek(ctx_ptr->load_file, toc_main.DataOffset+debug_offset);

  class_found = false;
  for (i=0; ((i<TOC_index+1) && (!class_found)); i++)
  {
    file_status = FILE_read(ctx_ptr->load_file, (uint8_t*)&(toc_entry), sizeof(TocEntryStructType));
  }
  {
    if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

    ctx_ptr->load_mem_class = toc_entry.MemoryClass;

    switch (ctx_ptr->load_mem_class)
    {
      case STAT_EEP_CLASS:
      {
        class_found = true;
        ctx_ptr->load_uid = toc_entry.UID;
        break;
      }
    }

  }

  if (!class_found)
  {
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_mem_class_not_found ));
    goto __END__;
  }

  PRGH_GetElementIndex_File(ctx_ptr->load_file,SECURITY_ELEM_TYPE, ctx_ptr->load_uid, &file_offset);
  FILE_seek(ctx_ptr->load_file, file_offset+sizeof(ElementHeaderStructType));
  file_status = FILE_read(ctx_ptr->load_file, (uint8_t*)&(ctx_ptr->load_header.Security), sizeof(SecurityStructType));
  if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

  // Get filepointer to the data element.
  PRGH_GetElementIndex_File(ctx_ptr->load_file,DOWNLOADDATA_ELEM_TYPE, ctx_ptr->load_uid, &file_offset);
  FILE_seek(ctx_ptr->load_file, file_offset+sizeof(ElementHeaderStructType));
  file_status = FILE_read(ctx_ptr->load_file, (uint8_t*)&(ctx_ptr->download_data_struct), sizeof(DownloadDataStructType));
  FILE_seek(ctx_ptr->load_file, ctx_ptr->download_data_struct.DataOffset);

  /* Decrypt sec pack if needed and possible */
  if(!DecryptSecPack(&ctx_ptr->load_header.Security, &status_text, ctx_ptr->load_file))
  {
    xstrcpy(status, status_text );
    goto __END__;
  }

  /* Check for special handling */
  /* Use LoadMap ? */
  if (ctx_ptr->load_header.Security.LoadMagic == LOAD_MAGIC)
  {
    ctx_ptr->handling_flags |= HANDLE_LOADMAP;
    ctx_ptr->loadmap_index = 0;
  }

  if (GLOBAL_use_pre_erase)
  {
    ctx_ptr->handling_flags |= HANDLE_PREERASE;
  }
  ctx_ptr->handling_flags |= HANDLE_UPLOAD_TO_MEM;
  ctx_ptr->handling_flags |= HANDLE_DNLOAD_FROM_MEM;
  ctx_ptr->handling_flags |= HANDLE_RMW_SEQ;
  ctx_ptr->handling_flags |= HANDLE_USE_CALC_CRC;

  /* Set-up the variables used by the load state-machine if not overruled by special handling */

  ctx_ptr->load_address     = ctx_ptr->load_header.Security.LoadMap[0].StartAddr;
  ctx_ptr->load_length      = ctx_ptr->load_header.Security.LoadMap[0].TotalLength;
  ctx_ptr->load_file_offset = ctx_ptr->download_data_struct.DataOffset;
  ctx_ptr->load_data_ptr    = (uint8_t*)MEM_malloc(ctx_ptr->load_length+20);
  ctx_ptr->exp_checksum     = 0;


  /* Adjust length to word alignment*/
  if (ctx_ptr->load_length & 1)
  {
    ctx_ptr->load_length++;
  }

  /* Set the state of the machine to 'upload' */
  ctx_ptr->state = SFH_state_upload;
  ctx_ptr->done_signal = use_done_signal;

  /* Closing down the file, and returns */
__END__:
  /**
   *  KlocWork NOTE: We cannot close the file, since it is read later on...
   *  TODO: Need to make a work around
   **/
  if (status && status[0])
  {
    if (ctx_ptr && ctx_ptr->load_file)
    {
      if (FILE_close(ctx_ptr->load_file) != FILE_status_ok)
        xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));
      ctx_ptr->load_file = NULL;
    }
    return (SFH_status_Error);
  }
  else if (use_done_signal != SFH_done_signal_callback)
  {
    return (SFH_status_wait_for_event);
  }
  else
  {
    return (SFH_status_OK);
  }
}
/****************************************************************************************
* Function:... SFH_upload_bin_image
* Parameters:. channel  In  Number of the logical channel (1..N)
*              image_start_address  In  The absolute address of the image to be read.
*              image_length  In  The length (in bytes) of the image to be read.
*              bin_file_name  In  The name of the BIN file in which the uploaded image is stored.
*              ref_file_name  In  Name of file to be used as reference.
*              use_done_signal  In  Defines how the final status is reported.
* Returns:.... -
* Description: This function is called to start the handlers processing of the binary image upload.
*              It will return immediately to allow other channels to start processing.
* Created:.... DD.MM.YY by (Full name / initials)
* Modified:... DD.MM.YY by (Full name / initials)
*                Modifications note.
****************************************************************************************/
SFH_status_enum SFH_upload_bin_image(uint8_t channel,
                                     uint32_t image_start_address,
                                     uint32_t image_length,
                                     char *bin_file_name,
                                     char *ref_file_name,
                                     SFH_done_signal_enum use_done_signal,
                                     char *status)
{
  SFH_ctx_type *ctx_ptr;
  FILE_status_enum file_status;
  char *status_text;
  uint32_t needed_version;
  uint32_t file_offset;
  uint32_t debug_offset;


  if (!status)
  {
    Debug("Error in %s: Status not defined", __func__);
    return (SFH_status_Error);
  }

  /* Get the context pointer */
  ctx_ptr = get_ctx(channel);
  if (!ctx_ptr)
  {
    /* The channel does not exist */
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_channel_does_not_exist, channel) );
    goto __END__;
  }

  ctx_ptr->status_text[0] = 0 ;

  ctx_ptr->handling_flags = 0;
  if (ctx_ptr->state != SFH_state_idle) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_cant_boot_while_busy) ); goto __END__;  }

  /* Save the boot information in the context */
  xstrcpy(ctx_ptr->load_file_name, bin_file_name);

  GLOBAL_callback(channel,IFWD_DL_ProcessOutlineUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_upload_start,ctx_ptr->load_file_name));

  /* If the ref file name is NULL use the header information form when the phone was booted */
  if (ref_file_name == NULL)
  {
    memcpy((uint8_t*)&(ctx_ptr->load_header),(uint8_t*)&(ctx_ptr->ref_header), sizeof(NEWH_GenericHeaderStructType));
  }
  else
  {
    xstrcpy(ctx_ptr->ref_file_name, ref_file_name);
    /* Check if the reference file exists */
    if (!FILE_exists(ctx_ptr->ref_file_name))  { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_not_found, ctx_ptr->ref_file_name)); goto __END__;  }

    /* Open the file in read mode */
    ctx_ptr->ref_file = FILE_open(ctx_ptr->ref_file_name, FILE_open_mode_read);
    if (ctx_ptr->ref_file == NULL) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

    debug_offset = SFH_GetDebugHeaderOffset_file(ctx_ptr->ref_file);
    PRGH_SetFileOffset(debug_offset);

    if (PRGH_FileCheck_File(ctx_ptr->ref_file) == INVALID_FILE_TYPE)
    {
      //GLOBAL_callback(channel,IFWD_DL_ProcessOutlineUpdated, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_wrong_file_type));
      xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_wrong_file_type) );
      goto __END__;
    }
    needed_version = PRGH_VersionCheck_File(ctx_ptr->ref_file,FLASHTOOL_TOOL_ELEM_TYPE, THIS_DLL_VERSION);
    if (needed_version)
    {
      xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_wrong_vesion, (needed_version >> 16), (needed_version & 0xFFFF)) );
      goto __END__;
    }

    /* Read from ref_file ... into load_header as it is what will be used later on. */
    /* Read the generic header info */
    file_status = FILE_read(ctx_ptr->ref_file, (uint8_t*)&(ctx_ptr->load_header), sizeof(NEWH_GenericHeaderStructType));
    if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

    /* Read the generic header info */
    PRGH_GetElementIndex_File(ctx_ptr->ref_file,HARDWARE_ELEM_TYPE, GENERIC_UID, &file_offset);
    FILE_seek(ctx_ptr->ref_file, file_offset+sizeof(ElementHeaderStructType));
    file_status = FILE_read(ctx_ptr->ref_file, (uint8_t*)&(ctx_ptr->load_header.Hardware), sizeof(HardwareStructType));
    if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

    PRGH_GetElementIndex_File(ctx_ptr->ref_file,SECURITY_ELEM_TYPE, GENERIC_UID, &file_offset);
    FILE_seek(ctx_ptr->ref_file, file_offset+sizeof(ElementHeaderStructType));
    file_status = FILE_read(ctx_ptr->ref_file, (uint8_t*)&(ctx_ptr->load_header.Security), sizeof(SecurityStructType));
    if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

    PRGH_GetElementIndex_File(ctx_ptr->ref_file,MEMORYMAP_ELEM_TYPE, GENERIC_UID, &file_offset);
    FILE_seek(ctx_ptr->ref_file, file_offset+sizeof(ElementHeaderStructType));
    file_status = FILE_read(ctx_ptr->ref_file, (uint8_t*)&(ctx_ptr->load_header.MemoryMap), sizeof(MemoryMapStructType));
    if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

    if (ctx_ptr->boot_dev_cfg.platform) // Test if booted ... if not hack to set protocol values.
    {
      ctx_ptr->boot_dev_cfg.platform   = ctx_ptr->ref_header.Hardware.Platform;
      ctx_ptr->boot_dev_cfg.technology = ctx_ptr->ref_header.Hardware.FlashTechnology;

      if (GLOBAL_usb_device || GLOBAL_use_GenericUSB) // USB .. so assume 2.32 protocol and 16Kbytes packs
      {
        PROT_set_protocol(ctx_ptr->channel, PROT_2_32);
        ctx_ptr->boot_ebl_version.capabilities.package_size = 16;
        GLOBAL_use_pack_size[ctx_ptr->channel] = 1024*ctx_ptr->boot_ebl_version.capabilities.package_size;
        TRA_CapabilitiesChanged(ctx_ptr->channel);
      }
      else if(GLOBAL_hsi_link) // HSI 2.32 and 16Kbytes packs
      {
        PROT_set_protocol(ctx_ptr->channel, PROT_2_32);
        ctx_ptr->boot_ebl_version.capabilities.package_size = 16;
        GLOBAL_use_pack_size[ctx_ptr->channel] = 1024*ctx_ptr->boot_ebl_version.capabilities.package_size;
        TRA_CapabilitiesChanged(ctx_ptr->channel);
      }
      else // USIF 1.16 and 2K pack
      {
        xstrcpy(status, "Binary upload via USIF and using --no-boot option is currently not supported.");  goto __END__;
      }
    }
    PRGH_SetFileOffset(0);
  }

  /* Decrypt sec pack if needed and possible */
  if(!DecryptSecPack(&ctx_ptr->load_header.Security, &status_text, NULL))
  {
    xstrcpy(status, status_text );
    goto __END__;
  }
  /* Check for special handling */
  /* Use LoadMap ? */
  if (ctx_ptr->load_header.Security.LoadMagic == LOAD_MAGIC)
  {
    ctx_ptr->handling_flags |= HANDLE_LOADMAP;
    ctx_ptr->loadmap_index = 0;
  }
  if (GLOBAL_use_pre_erase)
  {
    ctx_ptr->handling_flags |= HANDLE_PREERASE;
  }
  ctx_ptr->handling_flags |= HANDLE_USE_CALC_CRC;

  /* In case of NAND we must add the reference EBL file to the uploaded BIN file */

  if (ctx_ptr->boot_dev_cfg.technology == NAND_FLASH)
  {
    if (!GLOBAL_force_plain_upload) /* do we allow "extra" upload data? */
    {
      if (GLOBAL_upload_mbn_data)   /* Should the mbn be included? */
      {
        ctx_ptr->handling_flags |= HANDLE_BIN_GET_MBN;
      }
      ctx_ptr->handling_flags |= HANDLE_UPLOAD_ADD_EBL; /* Include EBL */
    }
  }

  ctx_ptr->load_address = image_start_address;
  ctx_ptr->load_length = image_length;
  ctx_ptr->load_file_offset = 0;
  ctx_ptr->exp_checksum     = 0;


  if ((ctx_ptr->boot_dev_cfg.technology == NAND_FLASH) &&
    !(ctx_ptr->boot_ebl_version.ext_capabilities.sec_handling_flags & EXT_CAP_SEC_OPEN_READ))
  {
    /* Must use 1 load regions */
    ctx_ptr->load_header.Security.LoadMap[0].StartAddr   = ctx_ptr->load_address;
    ctx_ptr->load_header.Security.LoadMap[0].TotalLength = ctx_ptr->load_length;
    ctx_ptr->load_header.Security.LoadMap[0].UsedLength  = ctx_ptr->load_length;
    ctx_ptr->load_header.Security.LoadMap[0].ImageFlags  = NAND_SKIP_PT_CHECK;
    ctx_ptr->load_header.Security.LoadMap[1].UsedLength  = 0;
    ctx_ptr->load_header.Security.LoadMap[2].UsedLength  = 0;
    ctx_ptr->load_header.Security.LoadMap[3].UsedLength  = 0;
    ctx_ptr->load_header.Security.LoadMap[4].UsedLength  = 0;
    ctx_ptr->load_header.Security.LoadMap[5].UsedLength  = 0;
    ctx_ptr->load_header.Security.LoadMap[6].UsedLength  = 0;
    ctx_ptr->load_header.Security.LoadMap[7].UsedLength  = 0;
  }

   /* Set the state of the machine to 'upload' */
  ctx_ptr->state = SFH_state_upload;
  ctx_ptr->done_signal = use_done_signal;

  /* Closing down the file, and returns */
__END__:
  /**
   *  KlocWork NOTE: We cannot close the file, since it is read later on...
   *  TODO: Need to make a work around
   **/
  if (status && status[0])
  {
    if (ctx_ptr && ctx_ptr->load_file)
    {
      if (FILE_close(ctx_ptr->load_file) != FILE_status_ok)
        xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));
      ctx_ptr->load_file = NULL;
    }
    return (SFH_status_Error);
  }
  else if (use_done_signal != SFH_done_signal_callback)
  {
    return (SFH_status_wait_for_event);
  }
  else
  {
    return (SFH_status_OK);
  }
}

SFH_status_enum SFH_download_bin_image(uint8_t channel,
                                       uint32_t image_start_address,
                                       uint32_t image_length,
                                       uint32_t image_offset,
                                       char *bin_file_name,
                                       char *ref_file_name,
                                       SFH_done_signal_enum use_done_signal,
                                       char *status)
{
  char *status_text;
  uint32_t needed_version;
  uint32_t file_offset;
  uint32_t debug_offset;
  SFH_ctx_type *ctx_ptr;
  FILE_status_enum file_status;

  if (!status)
  {
    Debug("Error in %s: Status not defined", __func__);
    return (SFH_status_Error);
  }

  /* Get the context pointer */
  ctx_ptr = get_ctx(channel);
  if (!ctx_ptr)
  {
    /* The channel does not exist */
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_channel_does_not_exist, channel) );
    goto __END__;
  }

  ctx_ptr->status_text[0] = 0 ;

  ctx_ptr->handling_flags = 0;
  if (ctx_ptr->state != SFH_state_idle) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_cant_boot_while_busy) ); goto __END__;  }

  /* Save the boot information in the context */
  xstrcpy(ctx_ptr->load_file_name, bin_file_name);

  /* If the ref file name is NULL use the header information form when the phone was booted */
  if (ref_file_name == NULL)
  {
    memcpy((uint8_t*)&(ctx_ptr->load_header),(uint8_t*)&(ctx_ptr->ref_header), sizeof(NEWH_GenericHeaderStructType));
  }
  else
  {
    GLOBAL_callback(channel,IFWD_DL_ProcessOutlineUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_load_start, "","", ctx_ptr->load_file_name));

    /* Check if the reference file exists */
    if (!FILE_exists(ref_file_name))  { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_not_found, ref_file_name)); goto __END__;  }

    /* Open the file in read mode */
    ctx_ptr->ref_file = FILE_open(ref_file_name, FILE_open_mode_read);
    if (ctx_ptr->ref_file == NULL) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

    debug_offset = SFH_GetDebugHeaderOffset_file(ctx_ptr->ref_file);
    PRGH_SetFileOffset(debug_offset);

    if (PRGH_FileCheck_File(ctx_ptr->ref_file) == INVALID_FILE_TYPE)
    {
      //GLOBAL_callback(channel,IFWD_DL_ProcessOutlineUpdated, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_wrong_file_type));
      xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_wrong_file_type) );
      goto __END__;
    }
    needed_version = PRGH_VersionCheck_File(ctx_ptr->ref_file,FLASHTOOL_TOOL_ELEM_TYPE, THIS_DLL_VERSION);
    if (needed_version)
    {
      xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_wrong_vesion, (needed_version >> 16), (needed_version & 0xFFFF)) );
      goto __END__;
    }

    /* Read from ref_file ... into load_header as it is what will be used later on. */
    /* Read the generic header info */
    file_status = FILE_read(ctx_ptr->ref_file, (uint8_t*)&(ctx_ptr->load_header), sizeof(NEWH_GenericHeaderStructType));
    if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

    /* Read the generic header info */
    PRGH_GetElementIndex_File(ctx_ptr->ref_file,HARDWARE_ELEM_TYPE, GENERIC_UID, &file_offset);
    FILE_seek(ctx_ptr->ref_file, file_offset+sizeof(ElementHeaderStructType));
    file_status = FILE_read(ctx_ptr->ref_file, (uint8_t*)&(ctx_ptr->load_header.Hardware), sizeof(HardwareStructType));
    if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

    PRGH_GetElementIndex_File(ctx_ptr->ref_file,SECURITY_ELEM_TYPE, GENERIC_UID, &file_offset);
    FILE_seek(ctx_ptr->ref_file, file_offset+sizeof(ElementHeaderStructType));
    file_status = FILE_read(ctx_ptr->ref_file, (uint8_t*)&(ctx_ptr->load_header.Security), sizeof(SecurityStructType));
    if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

    PRGH_GetElementIndex_File(ctx_ptr->ref_file,MEMORYMAP_ELEM_TYPE, GENERIC_UID, &file_offset);
    FILE_seek(ctx_ptr->ref_file, file_offset+sizeof(ElementHeaderStructType));
    file_status = FILE_read(ctx_ptr->ref_file, (uint8_t*)&(ctx_ptr->load_header.MemoryMap), sizeof(MemoryMapStructType));
    if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

    PRGH_SetFileOffset(0);

    /* Decrypt sec pack if needed and possible */
    if(!DecryptSecPack(&ctx_ptr->load_header.Security, &status_text, NULL))
    {
      xstrcpy(status, status_text );
      goto __END__;
    }

  }


  /* Check if the fls length must be set automatically. */
  ctx_ptr->load_address = image_start_address;
  ctx_ptr->load_file_offset = image_offset;
  if (image_length)
  {
    ctx_ptr->load_length = image_length;
  }
  else
  {
    ctx_ptr->load_length = 0;
    if (!FILE_exists(bin_file_name))  { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_not_found, bin_file_name)); goto __END__;  }

    ctx_ptr->load_length = fsize(bin_file_name) - image_offset;
  }

  ctx_ptr->handling_flags |= HANDLE_LOADMAP;
  ctx_ptr->loadmap_index = 0;
  ctx_ptr->handling_flags |= HANDLE_BIN;

  if (GLOBAL_use_pre_erase)
  {
    ctx_ptr->handling_flags |= HANDLE_PREERASE;
  }
  ctx_ptr->handling_flags |= HANDLE_USE_CALC_CRC;
  //ctx_ptr->handling_flags |= HANDLE_ERASE_ALL;

  /* In case of NAND we must send the MBN data before the actual binary data. */
  if (ctx_ptr->boot_dev_cfg.technology == NAND_FLASH)
  {
    if (GLOBAL_nand_partition_config.Specification == MAC_NOSPECIFICATION)
    {
      ctx_ptr->handling_flags |= HANDLE_BIN_SEND_MBN;
    }
    /* Must use 1 load regions */
    ctx_ptr->load_header.Security.LoadMap[0].StartAddr   = ctx_ptr->load_address;
    ctx_ptr->load_header.Security.LoadMap[0].TotalLength = ctx_ptr->load_length;
    ctx_ptr->load_header.Security.LoadMap[0].UsedLength  = ctx_ptr->load_length;
    ctx_ptr->load_header.Security.LoadMap[0].ImageFlags  = 0;
    ctx_ptr->load_header.Security.LoadMap[1].UsedLength  = 0;
    ctx_ptr->load_header.Security.LoadMap[2].UsedLength  = 0;
    ctx_ptr->load_header.Security.LoadMap[3].UsedLength  = 0;
    ctx_ptr->load_header.Security.LoadMap[4].UsedLength  = 0;
    ctx_ptr->load_header.Security.LoadMap[5].UsedLength  = 0;
    ctx_ptr->load_header.Security.LoadMap[6].UsedLength  = 0;
    ctx_ptr->load_header.Security.LoadMap[7].UsedLength  = 0;
  }



  ctx_ptr->exp_checksum     = 0;

  /* Set the state of the machine to 'upload' */
  ctx_ptr->state = SFH_state_load;
  ctx_ptr->done_signal = use_done_signal;

  /* Closing down the file, and returns */
__END__:
  if (ctx_ptr)
  {
    if (ctx_ptr->ref_file)
    {
      if (FILE_close(ctx_ptr->ref_file) != FILE_status_ok)
        xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));
      ctx_ptr->ref_file = NULL;
    }
    if (ctx_ptr->load_file)
    {
      if (FILE_close(ctx_ptr->load_file) != FILE_status_ok)
        xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));
      ctx_ptr->load_file = NULL;
    }
  }

  if (status && status[0])
  {
    return (SFH_status_Error);
  }
  else if (use_done_signal == SFH_done_signal_callback)
  {
    return (SFH_status_OK);
  }
  else
  {
    return (SFH_status_wait_for_event);
  }
}

SFH_status_enum SFH_erase_image(uint8_t channel,
                                uint32_t image_start_address,
                                uint32_t image_length,
                                char *ref_file_name,
                                SFH_done_signal_enum use_done_signal,
                                char *status)
{
  char *status_text;
  uint32_t needed_version;
  uint32_t file_offset;
  SFH_ctx_type *ctx_ptr;
  FILE_status_enum file_status;

  if (!status)
  {
    Debug("Error in %s: Status not defined", __func__);
    return (SFH_status_Error);
  }
/* SWISTART */
#ifdef SIERRA
  if (!(
      (image_start_address==0x63e80000 && image_length==0x100000) ||
      (image_start_address==0x61e80000 && image_length==0x100000)
      ))
  {
    Debug("Error in %s: not supported", __func__);
    return (SFH_status_Error);
  }
#endif /* SIERRA */
/* SWISTOP */

  /* Get the context pointer */
  ctx_ptr = get_ctx(channel);
  if (!ctx_ptr)
  {
    /* The channel does not exist */
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_channel_does_not_exist, channel) );
    goto __END__;
  }

  ctx_ptr->status_text[0] = 0 ;

  ctx_ptr->handling_flags = 0;
  if (ctx_ptr->state != SFH_state_idle) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_cant_boot_while_busy) ); goto __END__;  }


  /* If the ref file name is NULL use the header information form when the phone was booted */
  if (ref_file_name == NULL)
  {
    memcpy((uint8_t*)&(ctx_ptr->load_header),(uint8_t*)&(ctx_ptr->ref_header), sizeof(NEWH_GenericHeaderStructType));
  }
  else
  {
    GLOBAL_callback(channel,IFWD_DL_ProcessOutlineUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_load_start, "", "",ctx_ptr->load_file_name));

    /* Check if the reference file exists */
    if (!FILE_exists(ref_file_name))  { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_not_found, ref_file_name)); goto __END__;  }

    /* Open the file in read mode */
    ctx_ptr->ref_file = FILE_open(ref_file_name, FILE_open_mode_read);
    if (ctx_ptr->ref_file == NULL) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

    if (PRGH_FileCheck_File(ctx_ptr->ref_file) == INVALID_FILE_TYPE)
    {
      //GLOBAL_callback(channel,IFWD_DL_ProcessOutlineUpdated, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_wrong_file_type));
      xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_wrong_file_type) );
      goto __END__;
    }
    needed_version = PRGH_VersionCheck_File(ctx_ptr->ref_file,FLASHTOOL_TOOL_ELEM_TYPE, THIS_DLL_VERSION);
    if (needed_version)
    {
      xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_wrong_vesion, (needed_version >> 16), (needed_version & 0xFFFF)) );
      goto __END__;
    }

    /* Read from ref_file ... into load_header as it is what will be used later on. */
    /* Read the generic header info */
    file_status = FILE_read(ctx_ptr->ref_file, (uint8_t*)&(ctx_ptr->load_header), sizeof(NEWH_GenericHeaderStructType));
    if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

    /* Read the generic header info */
    PRGH_GetElementIndex_File(ctx_ptr->ref_file,HARDWARE_ELEM_TYPE, GENERIC_UID, &file_offset);
    FILE_seek(ctx_ptr->ref_file, file_offset+sizeof(ElementHeaderStructType));
    file_status = FILE_read(ctx_ptr->ref_file, (uint8_t*)&(ctx_ptr->load_header.Hardware), sizeof(HardwareStructType));
    if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

    PRGH_GetElementIndex_File(ctx_ptr->ref_file,SECURITY_ELEM_TYPE, GENERIC_UID, &file_offset);
    FILE_seek(ctx_ptr->ref_file, file_offset+sizeof(ElementHeaderStructType));
    file_status = FILE_read(ctx_ptr->ref_file, (uint8_t*)&(ctx_ptr->load_header.Security), sizeof(SecurityStructType));
    if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

    PRGH_GetElementIndex_File(ctx_ptr->ref_file,MEMORYMAP_ELEM_TYPE, GENERIC_UID, &file_offset);
    FILE_seek(ctx_ptr->ref_file, file_offset+sizeof(ElementHeaderStructType));
    file_status = FILE_read(ctx_ptr->ref_file, (uint8_t*)&(ctx_ptr->load_header.MemoryMap), sizeof(MemoryMapStructType));
    if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

    /* Decrypt sec pack if needed and possible */
    if(!DecryptSecPack(&ctx_ptr->load_header.Security, &status_text, NULL))
    {
      xstrcpy(status, status_text );
      goto __END__;
    }
  }

  ctx_ptr->handling_flags |= HANDLE_LOADMAP;
  ctx_ptr->loadmap_index = 0;
  ctx_ptr->handling_flags |= HANDLE_PREERASE;
  ctx_ptr->handling_flags |= HANDLE_STOP_AFTER_ERASE;
  ctx_ptr->handling_flags |= HANDLE_BIN;

  if (image_length == 0)
  {
    /* Chip erase */
    ctx_ptr->load_address = ctx_ptr->load_header.MemoryMap.FlashStartAddr;
    ctx_ptr->load_length = ctx_ptr->erase_length-2;
  }
  else
  {
    ctx_ptr->load_address = image_start_address;
    ctx_ptr->load_length = image_length;
  }

  ctx_ptr->exp_checksum     = 0;

  /* Set the state of the machine to 'upload' */
  ctx_ptr->state = SFH_state_load;
  ctx_ptr->done_signal = use_done_signal;

  /* Closing down the file, and returns */
__END__:
  /**
   *  KlocWork NOTE: We cannot close the file, since it is read later on...
   *  TODO: Need to make a work around
   **/
  if (status && status[0])
  {
    if (ctx_ptr && ctx_ptr->load_file)
    {
      if (FILE_close(ctx_ptr->load_file) != FILE_status_ok)
        xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));
      ctx_ptr->load_file = NULL;
    }
    return (SFH_status_Error);
  }
  else if (use_done_signal != SFH_done_signal_callback)
  {
    return (SFH_status_wait_for_event);
  }
  else
  {
    return (SFH_status_OK);
  }
}

/****************************************************************************************
* Function:... Function name
* Parameters:. -
* Returns:.... -
* Description: Detailed function description
* Created:.... DD.MM.YY by (Full name / initials)
* Modified:... DD.MM.YY by (Full name / initials)
*                Modifications note.
****************************************************************************************/
SFH_status_enum SFH_open_comm_port(uint8_t channel,
                                   char *comm_port_name,
                                   char *ctrl_port_name,
                                   uint32_t baud_rate,
                                   char *status)
{
  comm_cfg_t comm_cfg;
  comm_status_t comm_status;
  SFH_status_enum result = SFH_status_Error; /* assume error for now */

  /* Check the channel range */
  if (channel == 0 || channel > MAX_NUMBER_OF_CHANNELS)
  {
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_channel_is_outside_range,channel) );
    return(SFH_status_Error);
  }

  // Sanity Check
  if (!status)
  {
    PrintError("Error in %s: Status not defined", __func__);
    return (SFH_status_Error);
  }

  if (!comm_port_name || !ctrl_port_name)
  {
    PrintError("Need a comport and a control port\n");
    return SFH_status_Error;
  }

  // Copy the port names as they may be needed in other modules.
  strncpy(&GLOBAL_comm_port_name[channel][0], comm_port_name, MAX_COMPORT_NAME_LENGTH);
  strncpy(&GLOBAL_ctrl_port_name[channel][0], ctrl_port_name, MAX_COMPORT_NAME_LENGTH);

  strncpy(comm_cfg.comport_name, comm_port_name,MAX_COMPORT_NAME_LENGTH);
  comm_cfg.baudrate = baud_rate;

  Debug("Opening comm port %s (baud rate: %d)\n", comm_port_name, baud_rate);
  if (strcmp(comm_port_name, ctrl_port_name) != 0)
    Debug("Ctrl Port: %s\n", ctrl_port_name);

  comm_status = COMM_open_port(channel, &comm_cfg);

//  if(comm_status == comm_status_channel_port_open_ok)
//    comm_status = comm_status_ok;

  if ((comm_status != comm_status_ok)
      && GLOBAL_usb_device
      && !GLOBAL_usb_auto_mode)
  {
    comm_status = comm_status_ok;
  }

  if (comm_status != comm_status_ok)
    PrintError("Could not open communication port %s.\n");
  else
  {
    /* Commport opened OK */
    /* Add a channel to the SFH handler */
    if (SFH_add_channel(channel, status))
    {
      /* Channel added OK */
      if (TRA_add_channel(channel, status))
      {
        /* Channel added OK */
        if (PROT_add_channel(channel) == prot_status_ok)
        {
          if (COMM_add_channel(channel) == comm_status_channel_added)
          {
            if (BOOT_add_channel(channel) == boot_status_channel_added)
            {
              /* Save the comm port informations in the context */
              strncpy(active_ctx_ptr->comm_port_name, comm_port_name,MAX_COMPORT_NAME_LENGTH);

              if (ctrl_port_name)
              {
                strncpy(active_ctx_ptr->ctrl_port_name, ctrl_port_name,MAX_COMPORT_NAME_LENGTH);
              }
              else
              {
                strncpy(active_ctx_ptr->ctrl_port_name, comm_port_name,MAX_COMPORT_NAME_LENGTH);
              }

              if(GLOBAL_hsi_link)
                GLOBAL_comm_technology[channel]++;  // Count up from PRE_OPEN to OPENED.

              Debug("  Comm port %s open at %d baud (%d)\n",
                    comm_port_name, baud_rate, GLOBAL_comm_technology[channel]);
              active_ctx_ptr->high_baudrate = baud_rate;
              result = SFH_status_OK; /* everything went well */
            } else
              PrintError(" Error> BOOT_add_channel failed\n");
          } else
            PrintError(" Error> COMM_add_channel failed\n");
        } else
          PrintError(" Error> PROT_add_channel failed\n");
      } else
        PrintError(" Error> TRA_add_channel failed\n");
    } else
      PrintError(" Error> SFH_add_channel failed\n");
  }
  if(result != SFH_status_OK) /* did we encounter an error? */
  {
    SFH_delete_channel(channel, status); /* then clean up...*/
    TRA_delete_channel(channel, status);
    PROT_delete_channel(channel);
    BOOT_delete_channel(channel);
  }
  else
    sprintf(status,"%s",GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_channel_opened,channel));
  return result;
}

/****************************************************************************************
* Function:... Function name
* Parameters:. -
* Returns:.... -
* Description: Detailed function description
* Created:.... DD.MM.YY by (Full name / initials)
* Modified:... DD.MM.YY by (Full name / initials)
*                Modifications note.
****************************************************************************************/
SFH_status_enum SFH_close_comm_port(uint8_t channel, char *status)

{
  if (!status)
  {
    Debug("Error in %s: Status not defined", __func__);
    return (SFH_status_Error);
  }

  COMPORT_stop_prolific_board_reset(channel);

  SFH_abort_process(channel,status); /* stop any pending operations */

  SFH_delete_channel(channel, status);
  TRA_delete_channel(channel, status);
  PROT_delete_channel(channel);
  BOOT_delete_channel(channel);
  GLOBAL_hsi_link = false;

  if(COMM_close_comport(channel) == comm_status_channel_port_close_ok &&
    COMM_delete_channel(channel) == comm_status_channel_deleted)
  {
    sprintf(status,"%s", GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_channel_closed, channel));
    return SFH_status_OK;
  }
  return SFH_status_Error;
}

/****************************************************************************************
* Function:... Function name
* Parameters:. -
* Returns:.... -
* Description: Detailed function description
* Created:.... DD.MM.YY by (Full name / initials)
* Modified:... DD.MM.YY by (Full name / initials)
*                Modifications note.
****************************************************************************************/
SFH_status_enum SFH_boot_target(uint8_t channel,
                                char *ref_file,
                                IFWD_DL_modem_control_signals_type *modem_control_signals,
                                SFH_done_signal_enum use_done_signal,
                                char *status)
{
  SFH_ctx_type *ctx_ptr;
  FILE_status_enum file_status;
  uint32_t needed_version;
  uint32_t file_offset;
  uint32_t debug_offset;
  InjectionDataStructType InjectionData;

  if (!status)
  {
    Debug("Error in %s: Status not defined", __func__);
    return (SFH_status_Error);
  }

  /* Get the context pointer */
  ctx_ptr = get_ctx(channel);
  if (!ctx_ptr)
  {
    /* The channel does not exist */
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_channel_does_not_exist, channel) );
    goto __END__;
  }
  ctx_ptr->modem_controls = modem_control_signals;

  ctx_ptr->status_text[0] = 0;
  ctx_ptr->cur_eeprom_ver = 0;
  ctx_ptr->new_eeprom_ver = 0;

  if (ctx_ptr->state != SFH_state_idle) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_cant_boot_while_busy) ); goto __END__;  }

  //  GLOBAL_callback(channel,IFWD_DL_ProcessOutlineUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_boot_start));
  /* Save the boot information in the context */
  xstrcpy(ctx_ptr->ref_file_name, ref_file);

  /* Check if the reference file exists */
  if (!FILE_exists(ctx_ptr->ref_file_name))  { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_not_found, ctx_ptr->ref_file_name)); goto __END__;  }

  /* Open the file in read mode */
  ctx_ptr->ref_file = FILE_open(ctx_ptr->ref_file_name, FILE_open_mode_read);
  if (ctx_ptr->ref_file == NULL) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

  debug_offset = SFH_GetDebugHeaderOffset_file(ctx_ptr->ref_file);
  PRGH_SetFileOffset(debug_offset);

  if (PRGH_FileCheck_File(ctx_ptr->ref_file) == INVALID_FILE_TYPE)
  {
    FILE_close(ctx_ptr->ref_file);
    //GLOBAL_callback(channel,IFWD_DL_ProcessOutlineUpdated, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_wrong_file_type));
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_wrong_file_type) );
    goto __END__;
  }
  needed_version = PRGH_VersionCheck_File(ctx_ptr->ref_file,FLASHTOOL_TOOL_ELEM_TYPE,THIS_DLL_VERSION);
  if (needed_version)
  {
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_wrong_vesion, (needed_version >> 16), (needed_version & 0xFFFF)) );
    goto __END__;
  }

  /* Read the generic header info */
  PRGH_GetElementIndex_File(ctx_ptr->ref_file,HARDWARE_ELEM_TYPE, GENERIC_UID, &file_offset);
  FILE_seek(ctx_ptr->ref_file, file_offset+sizeof(ElementHeaderStructType));
  file_status = FILE_read(ctx_ptr->ref_file, (uint8_t*)&(ctx_ptr->ref_header.Hardware), sizeof(HardwareStructType));
  if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

  PRGH_GetElementIndex_File(ctx_ptr->ref_file,SECURITY_ELEM_TYPE, GENERIC_UID, &file_offset);
  FILE_seek(ctx_ptr->ref_file, file_offset+sizeof(ElementHeaderStructType));
  file_status = FILE_read(ctx_ptr->ref_file, (uint8_t*)&(ctx_ptr->ref_header.Security), sizeof(SecurityStructType/*SecurityElementStructType*/));
  if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

  PRGH_GetElementIndex_File(ctx_ptr->ref_file,MEMORYMAP_ELEM_TYPE, GENERIC_UID, &file_offset);
  FILE_seek(ctx_ptr->ref_file, file_offset+sizeof(ElementHeaderStructType));
  file_status = FILE_read(ctx_ptr->ref_file, (uint8_t*)&(ctx_ptr->ref_header.MemoryMap), sizeof(MemoryMapStructType /*MemoryMapElementStructType*/));
  if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

  ctx_ptr->boot_dev_cfg.platform   = ctx_ptr->ref_header.Hardware.Platform;
  ctx_ptr->boot_dev_cfg.technology = ctx_ptr->ref_header.Hardware.FlashTechnology;

  // ********** HANDLE XML ************
#if 1
  /* Check if there is XML data to be injected */
  if (PRGH_GetElementIndex_File(ctx_ptr->ref_file,INJECTED_PDF_ELEM_TYPE, GENERIC_UID, &file_offset))
  {
    FILE_seek(ctx_ptr->ref_file, file_offset+sizeof(ElementHeaderStructType));
    file_status = FILE_read(ctx_ptr->ref_file, (uint8_t*)&(InjectionData), sizeof(InjectionDataStructType));
    if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

    ctx_ptr->xml_length = InjectionData.DataLength;
    if (InjectionData.DataLength > MAX_BUFFER_LENGTH) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_data_size_overflow, "XML", InjectionData.DataLength ));  goto __END__;  }

    /* Get the memory needed to hold the XML data */
    ctx_ptr->xml_ptr = MEM_malloc(ctx_ptr->xml_length);
    if (!ctx_ptr->xml_ptr)
    {
      xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_malloc) );
      goto __END__;
    }

    /* Seek the file position for the XML data */
    file_status = FILE_seek(ctx_ptr->ref_file, InjectionData.DataOffset+debug_offset);
    if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__; }

    /* Read the XML data */
    file_status = FILE_read(ctx_ptr->ref_file, (uint8_t*)(ctx_ptr->xml_ptr), ctx_ptr->xml_length);
    if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() )); goto __END__; }
  }
  else
#endif
  {
    ctx_ptr->xml_length = 0;
    ctx_ptr->xml_ptr = NULL;
  }

  // ********** HANDLE PSI ************
  if (ctx_ptr->force_use_psi_file_name[0])  // See if there is a psi.fls to overrule the injected file.
  {
    FILE *psi_file;
    DownloadDataStructType download_data_struct;

    // Check if file exist and if so ... open it.
    if (!FILE_exists(ctx_ptr->force_use_psi_file_name))  { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_not_found, ctx_ptr->force_use_psi_file_name)); goto __END__;  }
    psi_file = FILE_open(ctx_ptr->force_use_psi_file_name,FILE_open_mode_read);
    if (psi_file == NULL) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }
    // Opened the file ... now get the psi data .... for the moment assuming there are just those data in the psi.FLS file.
    // .. so do not look-up in the TOC ... go directly for the data.
    PRGH_SetFileOffset(0); // Do not expect debug-header on psi-fls files.
    PRGH_GetElementIndex_File(psi_file,DOWNLOADDATA_ELEM_TYPE, 0, &file_offset); // UID = 0
    FILE_seek(psi_file, file_offset+sizeof(ElementHeaderStructType));
    file_status = FILE_read(psi_file, (uint8_t*)&(download_data_struct), sizeof(DownloadDataStructType));
    if (file_status != FILE_status_ok) { FILE_close(psi_file); xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error()) ); goto __END__;  }
    if (download_data_struct.DataLength > MAX_BUFFER_LENGTH) { FILE_close(psi_file); xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_malloc) ); goto __END__;  }
      FILE_seek(psi_file, download_data_struct.DataOffset);

    ctx_ptr->psi_data_ptr = MEM_malloc(download_data_struct.DataLength);
    if (!ctx_ptr->psi_data_ptr) { FILE_close(psi_file); xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_malloc) ); goto __END__;  }

    file_status = FILE_read(psi_file, (uint8_t*)(ctx_ptr->psi_data_ptr), download_data_struct.DataLength);
    if (file_status != FILE_status_ok) { FILE_close(psi_file); xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error()) ); goto __END__;  }

    ctx_ptr->boot_dev_cfg.psi_offset = ctx_ptr->psi_data_ptr;
    ctx_ptr->boot_dev_cfg.psi_size = download_data_struct.DataLength;

    FILE_close(psi_file);
  }
  else
  {
    /* Check if there is PSI data to be injected */
    if (PRGH_GetElementIndex_File(ctx_ptr->ref_file,INJECTED_PSI_ELEM_TYPE, GENERIC_UID, &file_offset))
    {
      FILE_seek(ctx_ptr->ref_file, file_offset+sizeof(ElementHeaderStructType));
      file_status = FILE_read(ctx_ptr->ref_file, (uint8_t*)&(InjectionData), sizeof(InjectionDataStructType));
      if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

      ctx_ptr->ref_header.PsiLength = InjectionData.DataLength;
      if (ctx_ptr->ref_header.PsiLength > MAX_BUFFER_LENGTH) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_data_size_overflow, "PSI", InjectionData.DataLength ));  goto __END__;  }

      /* Get the memory needed to hold the PSI data */
      ctx_ptr->psi_data_ptr = MEM_malloc(ctx_ptr->ref_header.PsiLength);
      if (!ctx_ptr->psi_data_ptr)
      {
        xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_malloc) );
        goto __END__;
      }

      /* Seek the file position for the PSI data */
      file_status = FILE_seek(ctx_ptr->ref_file, InjectionData.DataOffset+debug_offset);
      if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__; }

      /* Read the PSI data */
      file_status = FILE_read(ctx_ptr->ref_file, (uint8_t*)(ctx_ptr->psi_data_ptr), (ctx_ptr->ref_header).PsiLength);
      if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() )); goto __END__; }

      ctx_ptr->boot_dev_cfg.psi_offset = ctx_ptr->psi_data_ptr;
      ctx_ptr->boot_dev_cfg.psi_size = ctx_ptr->ref_header.PsiLength;

    }
    else
    {
      ctx_ptr->boot_dev_cfg.psi_offset = NULL;
      ctx_ptr->boot_dev_cfg.psi_size = 0;
      ctx_ptr->psi_data_ptr = NULL;
    }
  }

  // ********** HANDLE EBL ************
  if (ctx_ptr->force_use_ebl_file_name[0])  // See if there is a ebl.fls to overrule the injected file.
  {
    FILE *ebl_file;
    DownloadDataStructType download_data_struct;

    // Check if file exist and if so ... open it.
    if (!FILE_exists(ctx_ptr->force_use_ebl_file_name))  { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_not_found, ctx_ptr->force_use_ebl_file_name)); goto __END__;  }
    ebl_file = FILE_open(ctx_ptr->force_use_ebl_file_name,FILE_open_mode_read);
    if (ebl_file == NULL) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

    // Opened the file ... now get the psi data .... for the moment assuming there are just those data in the psi.FLS file.
    // .. so do not look-up in the TOC ... go directly for the data.
    PRGH_SetFileOffset(0); // Do not expect debug-header on psi-fls files.
    PRGH_GetElementIndex_File(ebl_file,DOWNLOADDATA_ELEM_TYPE, 0, &file_offset); // UID = 0
    FILE_seek(ebl_file, file_offset+sizeof(ElementHeaderStructType));
    file_status = FILE_read(ebl_file, (uint8_t*)&(download_data_struct), sizeof(DownloadDataStructType));
    if (file_status != FILE_status_ok) { FILE_close(ebl_file); xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error()) ); goto __END__;  }
    if (download_data_struct.DataLength > MAX_BUFFER_LENGTH) { FILE_close(ebl_file); xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_malloc) ); goto __END__;  }
    FILE_seek(ebl_file, download_data_struct.DataOffset);

    ctx_ptr->ebl_data_ptr = MEM_malloc(download_data_struct.DataLength);
    if (!ctx_ptr->ebl_data_ptr) {  xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_malloc) ); goto __END__;  }

    file_status = FILE_read(ebl_file, (uint8_t*)(ctx_ptr->ebl_data_ptr), download_data_struct.DataLength);

    ctx_ptr->boot_dev_cfg.ebl_offset = ctx_ptr->ebl_data_ptr;
    ctx_ptr->boot_dev_cfg.ebl_size = download_data_struct.DataLength;

    FILE_close(ebl_file);
  }
  else
  {
    if (PRGH_GetElementIndex_File(ctx_ptr->ref_file,INJECTED_EBL_ELEM_TYPE, GENERIC_UID, &file_offset))
    {
      FILE_seek(ctx_ptr->ref_file, file_offset+sizeof(ElementHeaderStructType));
      file_status = FILE_read(ctx_ptr->ref_file, (uint8_t*)&(InjectionData), sizeof(InjectionDataStructType));
      if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

      ctx_ptr->ref_header.EblLength = InjectionData.DataLength;
      if (ctx_ptr->ref_header.EblLength > MAX_BUFFER_LENGTH) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_data_size_overflow, "EBL", InjectionData.DataLength ));  goto __END__;  }

      /* Get the memory needed to hold the EBL data */
      ctx_ptr->ebl_data_ptr = MEM_malloc(ctx_ptr->ref_header.EblLength);
      if (!ctx_ptr->ebl_data_ptr)
      {
        xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_malloc) );
        goto __END__;
      }

      /* Seek the file position for the EBL data */
      file_status = FILE_seek(ctx_ptr->ref_file, InjectionData.DataOffset+debug_offset);
      if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

      /* Read the EBL data */
      file_status = FILE_read(ctx_ptr->ref_file, (uint8_t*)(ctx_ptr->ebl_data_ptr), (ctx_ptr->ref_header).EblLength);
      if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() )); goto __END__;  }

      ctx_ptr->boot_dev_cfg.ebl_offset = ctx_ptr->ebl_data_ptr;
      ctx_ptr->boot_dev_cfg.ebl_size = ctx_ptr->ref_header.EblLength;
    }
    else
    {
      /* in case of NAND there must be an injected EBL .. so .. error  */
      if (ctx_ptr->boot_dev_cfg.technology == NAND_FLASH)
      {
        xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_no_nand_ebl) );
        goto __END__;
      }

      ctx_ptr->boot_dev_cfg.ebl_offset = NULL;
      ctx_ptr->boot_dev_cfg.ebl_size = 0;
      ctx_ptr->ebl_data_ptr = NULL;
    }
  }

  // ********** HANDLE XML,PSI and EBL DONE ************
  ctx_ptr->boot_dev_cfg.boot_filename  = ctx_ptr->ref_file_name;

  /* Set the state of the machine to 'boot' */
  ctx_ptr->state = SFH_state_boot;
  ctx_ptr->done_signal = use_done_signal;

  /* Closing down the file, and returns */
__END__:
  if (ctx_ptr && ctx_ptr->ref_file)
  {
    if (FILE_close(ctx_ptr->ref_file) != FILE_status_ok)
      xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));
//  ctx_ptr->load_file = NULL; //< Was here before???
    ctx_ptr->ref_file = NULL;
  }


  if (status && status[0])
  {
    return (SFH_status_Error);
  }
  else if (use_done_signal == SFH_done_signal_callback)
  {
    return (SFH_status_OK);
  }
  else
  {
    return (SFH_status_wait_for_event);
  }
}

/*-------------------------------------------------------------------------------------*/
SFH_status_enum SFH_pre_boot_target(uint8_t channel,
                                uint32_t platform_code,
                                SFH_done_signal_enum use_done_signal,
                                char *status)
{
  SFH_ctx_type *ctx_ptr;

  if (!status)
  {
    Debug("Error in %s: Status not defined", __func__);
    return (SFH_status_Error);
  }

  /* Get the context pointer */
  ctx_ptr = get_ctx(channel);
  if (!ctx_ptr)
  {
    /* The channel does not exist */
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_channel_does_not_exist, channel) );
    goto __END__;
  }

  // Fake the needed info
  ctx_ptr->boot_dev_cfg.platform = platform_code;
  ctx_ptr->ref_header.Hardware.Platform = platform_code;
  ctx_ptr->boot_dev_cfg.platform   = ctx_ptr->ref_header.Hardware.Platform;
  ctx_ptr->boot_dev_cfg.technology = ctx_ptr->ref_header.Hardware.FlashTechnology;

  /* Set the state of the machine to 'boot' */
  GLOBAL_pre_boot[channel] = PRE_BOOT_ARMED;
  ctx_ptr->state = SFH_state_boot;
  ctx_ptr->done_signal = use_done_signal;

  /* Closing down the file, and returns */
__END__:
  if (ctx_ptr && ctx_ptr->ref_file)
  {
    if (FILE_close(ctx_ptr->ref_file) != FILE_status_ok)
      xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));
//  ctx_ptr->load_file = NULL; //< Was here before???
    ctx_ptr->ref_file = NULL;
  }


  if (status && status[0])
  {
    return (SFH_status_Error);
  }
  else if (use_done_signal == SFH_done_signal_callback)
  {
    return (SFH_status_OK);
  }
  else
  {
    return (SFH_status_wait_for_event);
  }
}

/*-------------------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------------------*/
SFH_status_enum SFH_boot_bin_target(uint8_t channel,
                                    uint8_t *ref_file_header,
                                    IFWD_DL_modem_control_signals_type *modem_control_signals,
                                    SFH_done_signal_enum use_done_signal,
                                    char *status)
{
  SFH_ctx_type *ctx_ptr;
  uint32_t needed_version;
  uint32_t file_offset;
  FILE_status_enum file_status;
  uint32_t element_index;
  InjectionDataStructType InjectionData;
  uint32_t debug_offset;

  if (!status)
  {
    Debug("Error in %s: Status not defined", __func__);
    return (SFH_status_Error);
  }

  /* Get the context pointer */
  ctx_ptr = get_ctx(channel);
  if (!ctx_ptr)
  {
    /* The channel does not exist */
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_channel_does_not_exist, channel) );
    goto __END__;
  }
  ctx_ptr->status_text[0] = 0 ;
  ctx_ptr->modem_controls = modem_control_signals;

  if (ctx_ptr->state != SFH_state_idle)
  {
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_cant_boot_while_busy) );
    goto __END__;
  }

  GLOBAL_callback(channel,IFWD_DL_ProcessOutlineUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_boot_start));

  /*  Check the input parameter to determine the type of data. It may be a pointer to a header structure on new or old format, or
  it may be a filename of a binary or a FLS style file. */


  debug_offset = SFH_GetDebugHeaderOffset(ref_file_header);

  //PRGH_SetFileOffset(debug_offset);
  ref_file_header += debug_offset;
  xstrcpy(ctx_ptr->ref_file_name, (const char*)ref_file_header);

  if (((*(uint32_t*)ref_file_header) & 0x0FFFFFFF) == 0x00000003)  /* E1 Toolchain */
  {
    /* E1 stuff is way to old  */
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_wrong_file_type) );
    goto __END__;
  }
  else if (*((uint32_t*)ref_file_header) == GENERIC_HEADER_ELEM_TYPE)  /* E2 Toolchain */
  {
    needed_version = PRGH_VersionCheck((char*)ref_file_header ,FLASHTOOL_TOOL_ELEM_TYPE, THIS_DLL_VERSION);
    if (needed_version)
    {
      xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_wrong_vesion, (needed_version >> 16), (needed_version & 0xFFFF)) );
      goto __END__;
    }

    /* Read the generic header info */
    PRGH_GetElementIndex((char*)ref_file_header, HARDWARE_ELEM_TYPE, GENERIC_UID, &element_index);
    memcpy((uint8_t*)&(ctx_ptr->ref_header.Hardware),ref_file_header+element_index+sizeof(ElementHeaderStructType) , sizeof(HardwareStructType));

    PRGH_GetElementIndex((char*)ref_file_header, SECURITY_ELEM_TYPE, GENERIC_UID, &element_index);
    memcpy((uint8_t*)&(ctx_ptr->ref_header.Security),ref_file_header+element_index+sizeof(ElementHeaderStructType) , sizeof(SecurityStructType));

    PRGH_GetElementIndex((char*)ref_file_header,MEMORYMAP_ELEM_TYPE, GENERIC_UID, &element_index);
    memcpy((uint8_t*)&(ctx_ptr->ref_header.MemoryMap),ref_file_header+element_index+sizeof(ElementHeaderStructType) , sizeof(MemoryMapStructType));



    /* Do not handle PSI and EBL in binary mode */
    ctx_ptr->boot_dev_cfg.platform   = ctx_ptr->ref_header.Hardware.Platform;
    ctx_ptr->boot_dev_cfg.technology = ctx_ptr->ref_header.Hardware.FlashTechnology;

#if 0
    ctx_ptr->psi_data_ptr = NULL;
    ctx_ptr->ebl_data_ptr = NULL;


    ctx_ptr->boot_dev_cfg.psi_offset = NULL;
    ctx_ptr->boot_dev_cfg.psi_size = 0;
    ctx_ptr->boot_dev_cfg.ebl_offset = NULL;
    ctx_ptr->boot_dev_cfg.ebl_size = 0;

#else
    /* Check if there is PSI data to be injected */
    if (PRGH_GetElementIndex((char*)ref_file_header,INJECTED_PSI_ELEM_TYPE, GENERIC_UID, &element_index))
    {
      memcpy((uint8_t*)&(InjectionData),ref_file_header+element_index+sizeof(ElementHeaderStructType), sizeof(InjectionDataStructType));

      ctx_ptr->ref_header.PsiLength = InjectionData.DataLength;
      if (InjectionData.DataLength > MAX_BUFFER_LENGTH) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_data_size_overflow, "PSI", InjectionData.DataLength ));  goto __END__;  }

      /* Get the memory needed to hold the PSI data */
      ctx_ptr->psi_data_ptr = MEM_malloc(ctx_ptr->ref_header.PsiLength);
      if (!ctx_ptr->psi_data_ptr)
      {
        xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_malloc) );
        goto __END__;
      }

      memcpy((uint8_t*)(ctx_ptr->psi_data_ptr), ref_file_header+InjectionData.DataOffset, (ctx_ptr->ref_header).PsiLength);

      ctx_ptr->boot_dev_cfg.psi_offset = ctx_ptr->psi_data_ptr;
      ctx_ptr->boot_dev_cfg.psi_size = ctx_ptr->ref_header.PsiLength;

    }
    else
    {
      ctx_ptr->boot_dev_cfg.psi_offset = NULL;
      ctx_ptr->boot_dev_cfg.psi_size = 0;
      ctx_ptr->psi_data_ptr = NULL;
    }

    if (PRGH_GetElementIndex((char*)ref_file_header,INJECTED_EBL_ELEM_TYPE, GENERIC_UID, &element_index))
    {
      memcpy((uint8_t*)&(InjectionData),ref_file_header+element_index+sizeof(ElementHeaderStructType), sizeof(InjectionDataStructType));

      ctx_ptr->ref_header.EblLength = InjectionData.DataLength;
      if (InjectionData.DataLength > MAX_BUFFER_LENGTH) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_data_size_overflow, "EBL", InjectionData.DataLength ));  goto __END__;  }

      /* Get the memory needed to hold the EBL data */
      ctx_ptr->ebl_data_ptr = MEM_malloc(ctx_ptr->ref_header.EblLength);
      if (!ctx_ptr->ebl_data_ptr)
      {
        xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_malloc) );
        goto __END__;
      }

      memcpy((uint8_t*)(ctx_ptr->ebl_data_ptr), ref_file_header+InjectionData.DataOffset, (ctx_ptr->ref_header).EblLength);

      ctx_ptr->boot_dev_cfg.ebl_offset = ctx_ptr->ebl_data_ptr;
      ctx_ptr->boot_dev_cfg.ebl_size = ctx_ptr->ref_header.EblLength;
    }
    else
    {
      ctx_ptr->boot_dev_cfg.ebl_offset = NULL;
      ctx_ptr->boot_dev_cfg.ebl_size = 0;
      ctx_ptr->ebl_data_ptr = NULL;
    }

#endif
  }
  else if (FILE_exists(ctx_ptr->ref_file_name)) /* If not E1 or E2 file header info it could be a filename */
  {
    uint32_t ebl_offset;
    DownloadDataStructType download_data;

    //  GLOBAL_callback(channel,IFWD_DL_ProcessOutlineUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_boot_start));
    /* Save the boot information in the context */

    /* Open the file in read mode */
    ctx_ptr->ref_file = FILE_open(ctx_ptr->ref_file_name, FILE_open_mode_read);
    if (ctx_ptr->ref_file == NULL) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

    debug_offset = SFH_GetDebugHeaderOffset_file(ctx_ptr->ref_file);
    PRGH_SetFileOffset(debug_offset);

    ebl_offset = 0;
    if (PRGH_FileCheck_File(ctx_ptr->ref_file) == INVALID_FILE_TYPE)
    {
      /* the file type is wrong ... in this case treat it as a binary file to be downloaded.  */
      ebl_offset = FILE_size(ctx_ptr->ref_file) - 8; /* This is the offset to the uint32_t holding the offset to the start of the EBL data */
      FILE_seek(ctx_ptr->ref_file, ebl_offset);
      FILE_read(ctx_ptr->ref_file, (uint8_t*)&ebl_offset, 4);
      FILE_seek(ctx_ptr->ref_file, ebl_offset);
      PRGH_SetFileOffset(ebl_offset);
    }
    else
    {
      /* At the moment the VersionCheck can not handle a file with start offset different from 0 */
      /* For the moment skip version check for NAND binary download */
      needed_version = PRGH_VersionCheck_File(ctx_ptr->ref_file,FLASHTOOL_TOOL_ELEM_TYPE,THIS_DLL_VERSION);
      if (needed_version)
      {
        xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_wrong_vesion, (needed_version >> 16), (needed_version & 0xFFFF)) );
        goto __END__;
      }
    }

    /* Read the generic header info */
    PRGH_GetElementIndex_File(ctx_ptr->ref_file,HARDWARE_ELEM_TYPE, GENERIC_UID, &file_offset);
    FILE_seek(ctx_ptr->ref_file, file_offset+sizeof(ElementHeaderStructType));

    file_status = FILE_read(ctx_ptr->ref_file, (uint8_t*)&(ctx_ptr->ref_header.Hardware), sizeof(HardwareStructType));
    if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

    PRGH_GetElementIndex_File(ctx_ptr->ref_file,SECURITY_ELEM_TYPE, GENERIC_UID, &file_offset);
    FILE_seek(ctx_ptr->ref_file, file_offset+sizeof(ElementHeaderStructType));
    file_status = FILE_read(ctx_ptr->ref_file, (uint8_t*)&(ctx_ptr->ref_header.Security), sizeof(SecurityStructType/*SecurityElementStructType*/));
    if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

    PRGH_GetElementIndex_File(ctx_ptr->ref_file,MEMORYMAP_ELEM_TYPE, GENERIC_UID, &file_offset);
    FILE_seek(ctx_ptr->ref_file, file_offset+sizeof(ElementHeaderStructType));
    file_status = FILE_read(ctx_ptr->ref_file, (uint8_t*)&(ctx_ptr->ref_header.MemoryMap), sizeof(MemoryMapStructType/*MemoryMapElementStructType*/));
    if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

    ctx_ptr->boot_dev_cfg.platform   = ctx_ptr->ref_header.Hardware.Platform;
    ctx_ptr->boot_dev_cfg.technology = ctx_ptr->ref_header.Hardware.FlashTechnology;


    /* Check if there is PSI data to be injected */
    if (PRGH_GetElementIndex_File(ctx_ptr->ref_file,INJECTED_PSI_ELEM_TYPE, GENERIC_UID, &file_offset))
    {
      FILE_seek(ctx_ptr->ref_file, file_offset+sizeof(ElementHeaderStructType));
      file_status = FILE_read(ctx_ptr->ref_file, (uint8_t*)&(InjectionData), sizeof(InjectionDataStructType));
      if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

      ctx_ptr->ref_header.PsiLength = InjectionData.DataLength;
      if (InjectionData.DataLength > MAX_BUFFER_LENGTH) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_data_size_overflow, "PSI", InjectionData.DataLength ));  goto __END__;  }

      /* Get the memory needed to hold the PSI data */
      ctx_ptr->psi_data_ptr = MEM_malloc(ctx_ptr->ref_header.PsiLength);
      if (!ctx_ptr->psi_data_ptr)
      {
        xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_malloc) );
        goto __END__;
      }

      /* Seek the file position for the PSI data */
      file_status = FILE_seek(ctx_ptr->ref_file, InjectionData.DataOffset+ebl_offset+debug_offset);
      if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__; }

      /* Read the PSI data */
      file_status = FILE_read(ctx_ptr->ref_file, (uint8_t*)(ctx_ptr->psi_data_ptr), (ctx_ptr->ref_header).PsiLength);
      if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() )); goto __END__; }

      ctx_ptr->boot_dev_cfg.psi_offset = ctx_ptr->psi_data_ptr;
      ctx_ptr->boot_dev_cfg.psi_size = ctx_ptr->ref_header.PsiLength;

    }
    else
    {
      ctx_ptr->boot_dev_cfg.psi_offset = NULL;
      ctx_ptr->boot_dev_cfg.psi_size = 0;
      ctx_ptr->psi_data_ptr = NULL;
    }

    /* Now we must make the boot-handler use the EBL code in this file */
    if (ctx_ptr->ref_header.Hardware.FlashTechnology != NAND_FLASH)
    {
      if (PRGH_GetElementIndex_File(ctx_ptr->ref_file,INJECTED_EBL_ELEM_TYPE, GENERIC_UID, &file_offset))
      {
        FILE_seek(ctx_ptr->ref_file, file_offset+sizeof(ElementHeaderStructType));
        file_status = FILE_read(ctx_ptr->ref_file, (uint8_t*)&(InjectionData), sizeof(InjectionDataStructType));
        if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

        ctx_ptr->ref_header.EblLength = InjectionData.DataLength;
        if (InjectionData.DataLength > MAX_BUFFER_LENGTH) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_data_size_overflow, "EBL", InjectionData.DataLength ));  goto __END__;  }

        /* Get the memory needed to hold the PSI data */
        ctx_ptr->ebl_data_ptr = MEM_malloc(ctx_ptr->ref_header.EblLength);
        if (!ctx_ptr->ebl_data_ptr)
        {
          xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_malloc) );
          goto __END__;
        }

        /* Seek the file position for the EBL data */
        file_status = FILE_seek(ctx_ptr->ref_file, InjectionData.DataOffset+ebl_offset+debug_offset);
        if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__; }

        /* Read the EBL data */
        file_status = FILE_read(ctx_ptr->ref_file, (uint8_t*)(ctx_ptr->ebl_data_ptr), (ctx_ptr->ref_header).EblLength);
        if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() )); goto __END__;  }

        ctx_ptr->boot_dev_cfg.ebl_offset = ctx_ptr->ebl_data_ptr;
        ctx_ptr->boot_dev_cfg.ebl_size = ctx_ptr->ref_header.EblLength;
      }

    }
    else
    {
      /* NAND case */
      if (PRGH_GetElementIndex_File(ctx_ptr->ref_file,DOWNLOADDATA_ELEM_TYPE, GENERIC_UID, &file_offset))
      {
        FILE_seek(ctx_ptr->ref_file, file_offset+sizeof(ElementHeaderStructType));
        file_status = FILE_read(ctx_ptr->ref_file, (uint8_t*)&(download_data), sizeof(DownloadDataStructType));
        if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

        //if (InjectionData.DataLength > MAX_BUFFER_LENGTH) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_data_size_overflow, "EBL", InjectionData.DataLength ));  goto __END__;  }
        if (download_data.DataLength > MAX_BUFFER_LENGTH) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_data_size_overflow, "EBL", download_data.DataLength ));  goto __END__;  }
        ctx_ptr->ref_header.EblLength = download_data.DataLength;

        /* Get the memory needed to hold the EBL data */
        ctx_ptr->ebl_data_ptr = MEM_malloc(ctx_ptr->ref_header.EblLength);
        if (!ctx_ptr->ebl_data_ptr)
        {
          xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_malloc) );
          goto __END__;
        }


        /* Seek the file position for the EBL data */
        file_status = FILE_seek(ctx_ptr->ref_file, download_data.DataOffset+ebl_offset);
        if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

        /* Read the EBL data */
        file_status = FILE_read(ctx_ptr->ref_file, (uint8_t*)(ctx_ptr->ebl_data_ptr), (ctx_ptr->ref_header).EblLength);
        if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() )); goto __END__;  }

        ctx_ptr->boot_dev_cfg.ebl_offset = ctx_ptr->ebl_data_ptr;
        ctx_ptr->boot_dev_cfg.ebl_size = ctx_ptr->ref_header.EblLength;
      }
    }

    ctx_ptr->boot_dev_cfg.boot_filename  = ctx_ptr->ref_file_name;
  }
  else
  {
    /* Can't figure out what the f*ck I got */
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_wrong_file_type) );
    goto __END__;
  }


  PRGH_SetFileOffset(0);


  /* Set the state of the machine to 'boot' */
  ctx_ptr->state = SFH_state_boot;
  ctx_ptr->done_signal = use_done_signal;

  /* Closing down the file, and returns */
__END__:
  if (ctx_ptr && ctx_ptr->ref_file)
  {
    if (FILE_close(ctx_ptr->ref_file) != FILE_status_ok)
      xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));
//  ctx_ptr->load_file = NULL; //< Was here before???
    ctx_ptr->ref_file = NULL;
  }

  if (status && status[0])
  {
    return (SFH_status_Error);
  }
  else if (use_done_signal == SFH_done_signal_callback)
  {
    return (SFH_status_OK);
  }
  return (SFH_status_wait_for_event);
}

/*-------------------------------------------------------------------------------------*/
void SFH_set_error_string(uint8_t channel, char *error_text)
{
  SFH_ctx_type *ctx_ptr;

  /* Get the context pointer */
  ctx_ptr = get_ctx(channel);
  if (ctx_ptr)
  {
    /* if (ctx_ptr->status_text[0] == 0) */
    if(error_text && *error_text)
    {
      xstrcpy(ctx_ptr->status_text, error_text);
    }
  }
}

/*-------------------------------------------------------------------------------------*/
SFH_status_enum SFH_control_modem_signals(uint8_t channel,
                                          IFWD_DL_modem_control_signals_type *modem_control_signals,
                                          char *status)
{
  comm_dsr_control_t dsr;

  if (modem_control_signals != NULL)
  {
    if (modem_control_signals->RTS != IFWD_DL_mco_no_change) // any RTS to set?
    {
      if (modem_control_signals->RTS == IFWD_DL_mco_set_to_0)
      {
        COMM_set_rts(channel, comm_rts_control_disable);
      }
      else if (modem_control_signals->RTS == IFWD_DL_mco_set_to_1)
      {
        COMM_set_rts(channel, comm_rts_control_enable);
      }
      else if (modem_control_signals->RTS == IFWD_DL_mco_pulse_0)
      {
        if (status && status[0] != (char)'^')
        {
          COMM_set_rts(channel, comm_rts_control_disable);
        }
        else
        {
          COMM_set_rts(channel, comm_rts_control_enable);
        }
      }
      else // if (modem_control_signals->RTS == IFWD_DL_mco_pulse_1)
      {
        if (status && status[0] != (char)'^')
        {
          COMM_set_rts(channel, comm_rts_control_enable);
        }
        else
        {
          COMM_set_rts(channel, comm_rts_control_disable);
        }
      }
    }

    if (modem_control_signals->DTR != IFWD_DL_mco_no_change) // any DTR to set?
    {
      if (modem_control_signals->DTR == IFWD_DL_mco_set_to_0)
      {
        COMM_set_dtr(channel, comm_dtr_control_disable);
      }
      else if (modem_control_signals->DTR == IFWD_DL_mco_set_to_1)
      {
        COMM_set_dtr(channel, comm_dtr_control_enable);
      }
      else if (modem_control_signals->DTR == IFWD_DL_mco_pulse_0)
      {
        if (status && status[0] != (char)'^')
        {
          COMM_set_dtr(channel, comm_dtr_control_disable);
        }
        else
        {
          COMM_set_dtr(channel, comm_dtr_control_enable);
        }
      }
      else // if (modem_control_signals->DTR == IFWD_DL_mco_pulse_1)
      {
        if (status && status[0] != (char)'^')
        {
          COMM_set_dtr(channel, comm_dtr_control_enable);
        }
        else
        {
          COMM_set_dtr(channel, comm_dtr_control_disable);
        }
      }
    }

    COMM_get_dsr(channel, &dsr);
    modem_control_signals->DSR  = (dsr == comm_dsr_control_enable) ? IFWD_DL_mci_is_1 : IFWD_DL_mci_is_0;
    /* We currently do not support these modem signals, so set them to zero: */
    modem_control_signals->CTS  = IFWD_DL_mci_is_0;
    modem_control_signals->RING = IFWD_DL_mci_is_0;
    modem_control_signals->RLSD = IFWD_DL_mci_is_0;
    return (SFH_status_OK);
  }
  else
  {
    return (SFH_status_Error);
  }
}

/*-------------------------------------------------------------------------------------*/
SFH_status_enum SFH_get_sw_version_data(uint8_t channel,char *fls_file_name,uint8_t *file_data,uint8_t *taget_data,uint16_t data_size,SFH_done_signal_enum use_done_signal,char *status)
{
  /* Open the file to get the SW-version address and the file data. */
  uint32_t needed_version;
  uint32_t file_offset;
  SFH_ctx_type *ctx_ptr;
  FILE_status_enum file_status;

  if (!status)
  {
    Debug("Error in %s: Status not defined", __func__);
    return (SFH_status_Error);
  }

  /* Get the context pointer */
  ctx_ptr = get_ctx(channel);
  if (!ctx_ptr)
  {
    /* The channel does not exist */
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_channel_does_not_exist, channel) );
    goto __END__;
  }
  if (taget_data) /* Read both from file and target */
  {
    ctx_ptr->status_text[0] = 0;
    ctx_ptr->handling_flags = 0;
    if (ctx_ptr->state != SFH_state_idle) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_cant_boot_while_busy) ); goto __END__;  }

    /* Save the boot information in the context */
    xstrcpy(ctx_ptr->load_file_name, fls_file_name);

    GLOBAL_callback(channel,IFWD_DL_ProcessOutlineUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, SFH_INFO_get_sw_ver));

    /* Check if the reference file exists */
    if (!FILE_exists(ctx_ptr->load_file_name))  { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_not_found, ctx_ptr->load_file_name)); goto __END__;  }

    /* Open the file in read mode */
    ctx_ptr->load_file = FILE_open(ctx_ptr->load_file_name, FILE_open_mode_read);
    if (ctx_ptr->load_file == NULL) { FILE_close(ctx_ptr->load_file); xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }
    // --------------------------------
    if (PRGH_FileCheck_File(ctx_ptr->load_file) == INVALID_FILE_TYPE)
    {
      //GLOBAL_callback(channel,IFWD_DL_ProcessOutlineUpdated, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_wrong_file_type));
      xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_wrong_file_type) );
      goto __END__;
    }
    needed_version = PRGH_VersionCheck_File(ctx_ptr->load_file,FLASHTOOL_TOOL_ELEM_TYPE,THIS_DLL_VERSION);
    if (needed_version)
    {
      xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_wrong_vesion, (needed_version >> 16), (needed_version & 0xFFFF)) );
      goto __END__;
    }

    /* Read the generic header info */
    PRGH_GetElementIndex_File(ctx_ptr->load_file,HARDWARE_ELEM_TYPE, GENERIC_UID, &file_offset);
    FILE_seek(ctx_ptr->load_file, file_offset+sizeof(ElementHeaderStructType));
    file_status = FILE_read(ctx_ptr->load_file, (uint8_t*)&(ctx_ptr->load_header.Hardware), sizeof(HardwareStructType));
    if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

    PRGH_GetElementIndex_File(ctx_ptr->load_file,SECURITY_ELEM_TYPE, GENERIC_UID, &file_offset);
    FILE_seek(ctx_ptr->load_file, file_offset+sizeof(ElementHeaderStructType));
    file_status = FILE_read(ctx_ptr->load_file, (uint8_t*)&(ctx_ptr->load_header.Security), sizeof(SecurityStructType/*SecurityElementStructType*/));
    if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

    PRGH_GetElementIndex_File(ctx_ptr->load_file,MEMORYMAP_ELEM_TYPE, GENERIC_UID, &file_offset);
    FILE_seek(ctx_ptr->load_file, file_offset+sizeof(ElementHeaderStructType));
    file_status = FILE_read(ctx_ptr->load_file, (uint8_t*)&(ctx_ptr->load_header.MemoryMap), sizeof(MemoryMapStructType /*MemoryMapElementStructType*/));
    if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

    // --------------------------------

    /* Check the file type */
    //##    if (!(ctx_ptr->load_header.FileType & OUTPUT_FILE_TYPE)) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_wrong_header_type) );  goto __END__;  }

    /* Copy the data from the file */
    memcpy(file_data, ctx_ptr->load_header.Security.DataBlock,data_size);


    /* Save the information's needed for the upload */
    ctx_ptr->handling_flags |= HANDLE_UPLOAD_TO_MEM;

    ctx_ptr->load_data_ptr = taget_data;
    ctx_ptr->load_length = data_size;
    ctx_ptr->load_address = ctx_ptr->load_header.MemoryMap.SwVersionStringLocation;

    /* Set the state of the machine to 'upload' */
    ctx_ptr->state = SFH_state_upload;
    ctx_ptr->done_signal = use_done_signal;

    /* Closing down the file, and returns */
__END__:
    if (ctx_ptr && ctx_ptr->load_file)
    {
      if (FILE_close(ctx_ptr->load_file) != FILE_status_ok)
        xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));
      ctx_ptr->load_file = NULL;
    }

    if (status && status[0])
    {
      GLOBAL_callback(channel,IFWD_DL_ProcessOutlineUpdated, status);
      return (SFH_status_Error);
    }
    else if (use_done_signal != SFH_done_signal_callback)
    {
      return (SFH_status_wait_for_event);
    }
    else
    {
      return (SFH_status_OK);
    }
  }
  else /* Read only from the file */
  {
    /* This function will not work as it is. local_header is e.g. not initialized. So until customers want it - issue an error */
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_not_yet_implemented, SFH_get_sw_version_data));
    goto __END__;
#if 0
    FILE *local_handle;
    NEWH_GenericHeaderStructType local_header;

    /* Check if the reference file exists */
    if (!FILE_exists(fls_file_name))  { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_not_found, fls_file_name)); goto __END__;  }

    /* Open the file in read mode */
    local_handle = FILE_open(fls_file_name, FILE_open_mode_read);
    if (local_handle == NULL) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

    if (PRGH_FileCheck_File(local_handle) == INVALID_FILE_TYPE)
    {
      //GLOBAL_callback(channel,IFWD_DL_ProcessOutlineUpdated, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_wrong_file_type));
      xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_wrong_file_type) );
      goto __END__;
    }
    needed_version = PRGH_VersionCheck_File(local_handle,FLASHTOOL_TOOL_ELEM_TYPE,THIS_DLL_VERSION);
    if (needed_version)
    {
      xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_wrong_vesion, (needed_version >> 16), (needed_version & 0xFFFF)) );
      goto __END__;
    }

    /* Read the generic header info */
    PRGH_GetElementIndex_File(local_handle,HARDWARE_ELEM_TYPE, GENERIC_UID, &file_offset);
    FILE_seek(local_handle, file_offset+sizeof(ElementHeaderStructType));
    file_status = FILE_read(local_handle, (uint8_t*)&(ctx_ptr->load_header.Hardware), sizeof(HardwareStructType));
    if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

    PRGH_GetElementIndex_File(local_handle,SECURITY_ELEM_TYPE, GENERIC_UID, &file_offset);
    FILE_seek(local_handle, file_offset+sizeof(ElementHeaderStructType));
    file_status = FILE_read(local_handle, (uint8_t*)&(ctx_ptr->load_header.Security), sizeof(SecurityElementStructType));
    if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }

    PRGH_GetElementIndex_File(local_handle,MEMORYMAP_ELEM_TYPE, GENERIC_UID, &file_offset);
    FILE_seek(local_handle, file_offset+sizeof(ElementHeaderStructType));
    file_status = FILE_read(local_handle, (uint8_t*)&(ctx_ptr->load_header.MemoryMap), sizeof(MemoryMapElementStructType));
    if (file_status != FILE_status_ok) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  goto __END__;  }


    /* Check the file type */
    //##    if (!(local_header.FileType & OUTPUT_FILE_TYPE)) { xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_wrong_header_type) );  goto __END__;  }

    /* Copy the data from the file */
    memcpy(file_data, local_header.Security.DataBlock,data_size);


    /* All the needed information is read so close the file again */
    file_status = FILE_close(local_handle);
    ctx_ptr->load_file = NULL;

    if (file_status != FILE_status_ok)
    {
      xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));
      goto __END__;
    }

    return (SFH_status_OK);
#endif
  }
}

/*-------------------------------------------------------------------------------------*/
/* The COMM handler calls this funciton in AT-mode when data has been recieved */
void SFH_data_received(uint8_t channel, uint16_t length)
{
  static const char * const errtext = "AT Receive ERROR";
  SFH_ctx_type *ctx_ptr = get_ctx(channel);
  if(ctx_ptr && ctx_ptr->state == SFH_state_AT_mode)
  {
    if(length > 0 && length <= SFH_CNF_PACK_SIZE)
    {
      memcpy(ctx_ptr->cmd_buf, ctx_ptr->cnf_buf, length); /* copy temperary result to "cmd_buf" to free rx buffer again */
      ctx_ptr->cmd_buf[length] = 0; /* NULL-terminate string */
      GLOBAL_callback(channel, IFWD_DL_AT_Command, (char *)ctx_ptr->cmd_buf); /* report the whole received AT command back */
    }
    else
    {
      GLOBAL_callback(channel, IFWD_DL_AT_Command, (char *)errtext); /* report the whole received AT command back */
    }
  }

#if 0
        >  if(length)
        {
          memcpy(ctx_ptr->cmd_buf + ctx_ptr->temp_u16, ctx_ptr->cnf_buf, length); /* copy temperary result to "cmd_buf" to free rx buffer again */
          ctx_ptr->temp_u16 += length;
          if(ctx_ptr->cnf_buf[length-1] == 13 || ctx_ptr->cnf_buf[length-1] == 0 || ctx_ptr->temp_u16 > PACK_DATA_SIZE)
          {
            if(ctx_ptr->temp_u16)
            {

              ctx_ptr->cmd_buf[ctx_ptr->temp_u16] = 0; /* NULL-terminate string */
              ctx_ptr->temp_u16 = 0;  /* reset counter so we are ready to receive new data */
              GLOBAL_callback(channel, IFWD_DL_AT_Command, (char *)ctx_ptr->cmd_buf); /* report the whole received AT command back */
            }
          }
        }
        else /* length == 0 means error, so send what we have: */
        {
          /* any data to send (don't send an empty string) */
          if(ctx_ptr->temp_u16)
          {
            ctx_ptr->cmd_buf[ctx_ptr->temp_u16] = 0; /* NULL-terminate string */
            ctx_ptr->temp_u16 = 0;  /* reset counter so we are ready to receive new data */
            GLOBAL_callback(channel, IFWD_DL_AT_Command, (char *)ctx_ptr->cmd_buf); /* report the whole received AT command back */
          }
        }
#endif
}

/*-------------------------------------------------------------------------------------*/
SFH_status_enum SFH_start_AT_mode(uint8_t channel, char *status)
{
  SFH_ctx_type *ctx_ptr = get_ctx(channel);
  if(ctx_ptr && ctx_ptr->state == SFH_state_idle) /* only allow AT mode from idle state */
  {
    if(COMM_start_AT_mode(channel, ctx_ptr->cnf_buf) == comm_status_ok)
    {
      ctx_ptr->state = SFH_state_AT_mode; /* go into AT mode state */
      /*ctx_ptr->temp_u16 = 0;*/ /* clear the rx buffer length counter */
      if(status)
        status[0] = 0; /* clear any error messages */
      return SFH_status_OK;
    }
  }
  else
  {
    if(status)
    {
        xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code,
            (!ctx_ptr) ? SFH_ERR_channel_is_outside_range : SFH_ERR_not_in_idle, channel) );
    }
  }
  return SFH_status_Error;
}

/*-------------------------------------------------------------------------------------*/
SFH_status_enum SFH_stop_AT_mode(uint8_t channel, char *status)
{
  SFH_ctx_type *ctx_ptr = get_ctx(channel);
  if(ctx_ptr && ctx_ptr->state == SFH_state_AT_mode) /* We can only stop AT mode while we are in AT mode */
  {
#if 0
    if(ctx_ptr->temp_u16 > 0) /* any unsent data in rx buffer? */
    {
      ctx_ptr->cmd_buf[ctx_ptr->temp_u16] = 0; /* NULL-terminate string */
      ctx_ptr->temp_u16 = 0;  /* reset counter so we are ready to receive new data */
      GLOBAL_callback(channel, IFWD_DL_AT_Command, (char *)ctx_ptr->cmd_buf); /* send what we have back */
    }
#endif
    if(COMM_stop_AT_mode(channel) == comm_status_ok)
    {
      ctx_ptr->state = SFH_state_idle;
      if(status)
        status[0] = 0; /* clear any error messages */
      return SFH_status_OK;
    }
    else
    {
      xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_err_AT_mode_stop) );
    }
  }
  else
  {
    if(status)
    {
      xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code,
          (!ctx_ptr) ? SFH_ERR_channel_is_outside_range : SFH_ERR_not_in_AT_mode, channel) );
    }
  }
  return SFH_status_Error;
}

/*-------------------------------------------------------------------------------------*/

void SFH_AT_Command_Sent(uint8_t Channel)
{
  PARAM_NOT_USED(Channel); /* This is ignored for now */
}
/*-------------------------------------------------------------------------------------*/

SFH_status_enum SFH_AT_send(uint8_t channel, char *data, uint16_t length, char *status)
{
  SFH_ctx_type *ctx_ptr = get_ctx(channel);
  if(ctx_ptr && ctx_ptr->state == SFH_state_AT_mode) /* We can only stop AT mode while we are in AT mode */
  {
    /*    ctx_ptr->temp_u16 = 0; *//* clear the rx buffer length counter before we send new command (in case there are unsent data in the buffer by error) */
    COMM_send_AT_command(channel, data, length, SFH_AT_Command_Sent);
  }
  else
  {
    if(status)
    {
      xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code,
          (!ctx_ptr) ? SFH_ERR_channel_is_outside_range : SFH_ERR_not_in_AT_mode, channel) );
    }
    return SFH_status_Error;
  }
  if(status)
    status[0] = 0; /* clear any error messages */
  return SFH_status_OK;
}
/*-------------------------------------------------------------------------------------*/


SFH_status_enum SFH_force_target_reset(uint8_t channel, uint8_t mode, uint8_t p1, uint8_t p2, uint8_t p3, SFH_done_signal_enum use_done_signal, char *status)
{
  SFH_ctx_type *ctx_ptr = get_ctx(channel);

  if (!status)
  {
    Debug("Error in %s: Status not defined", __func__);
    return (SFH_status_Error);
  }

  if (!ctx_ptr)
  {
    /* The channel does not exist */
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_channel_does_not_exist, channel) );
    return (SFH_status_Error);
  }

  /* Prepare the data to be send to the target */
  ctx_ptr->cmd_buf[0] = mode;
  ctx_ptr->cmd_buf[1] = p1;
  ctx_ptr->cmd_buf[2] = p2;
  ctx_ptr->cmd_buf[3] = p3;

  /* Set the state of the machine to 'boot' */
  ctx_ptr->state = SFH_state_force_target_reset;
  ctx_ptr->done_signal = use_done_signal;

  if (use_done_signal == SFH_done_signal_callback)
  {
    return (SFH_status_OK);
  }
  else
  {
    return (SFH_status_wait_for_event);
  }
}
/*-------------------------------------------------------------------------------------*/

SFH_status_enum SFH_end_of_process(uint8_t channel, SFH_done_signal_enum use_done_signal, char *status)
{
  SFH_ctx_type *ctx_ptr = get_ctx(channel);

  PARAM_NOT_USED(status); // To remove warning

  if (!ctx_ptr)
  {
    /* The channel does not exist */
    //  xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_channel_does_not_exist, channel) );
    return (SFH_status_Error);
  }

  /* Prepare the data to be send to the target */
  ctx_ptr->cmd_buf[0] = 0;
  ctx_ptr->cmd_buf[1] = 0;

  /* Set the state of the machine to 'boot' */
  ctx_ptr->state = SFH_state_end_of_process;
  ctx_ptr->done_signal = use_done_signal;

  if (use_done_signal == SFH_done_signal_callback)
  {
    return (SFH_status_OK);
  }
  else
  {
    return (SFH_status_wait_for_event);
  }
}
/*-------------------------------------------------------------------------------------*/

SFH_status_enum  SFH_get_flash_id(uint8_t channel, uint32_t *flash_id)
{

  SFH_ctx_type *ctx_ptr = get_ctx(channel);

  if (!ctx_ptr)
  {
    return (SFH_status_Error);
  }
  *flash_id = ctx_ptr->flash_id;

  return SFH_status_OK;
}

SFH_status_enum  SFH_get_flash_voltage(uint8_t channel, uint32_t *flash_voltage)
{

  SFH_ctx_type *ctx_ptr = get_ctx(channel);

  if (!ctx_ptr)
  {
    return (SFH_status_Error);
  }
  *flash_voltage = ctx_ptr->flash_voltage;

  return SFH_status_OK;
}

SFH_status_enum SFH_out_of_session_comm(uint8_t  channel,
                                        uint32_t action,
                                        uint32_t type,
                                        uint32_t length,
                                        uint32_t spare,
                                        uint8_t  *data,
                                        SFH_done_signal_enum use_done_signal)
{
  SFH_ctx_type *ctx_ptr = get_ctx(channel);

  if (!ctx_ptr)
    return SFH_status_Error;

  ctx_ptr->oos_action   = action;
  ctx_ptr->oos_type     = type;
  ctx_ptr->oos_length   = length;
  ctx_ptr->oos_spare    = spare;
  ctx_ptr->oos_data_ptr = data;

  /* Set the state of the machine  */
  ctx_ptr->state = SFH_state_oos_send_control;
  ctx_ptr->done_signal = use_done_signal;

  if (use_done_signal == SFH_done_signal_callback)
    return SFH_status_OK;
  else
    return SFH_status_wait_for_event;
}

SFH_status_enum  SFH_get_ebl_info(uint8_t code, uint8_t channel, uint32_t *ebl_info)
{

  SFH_ctx_type *ctx_ptr = get_ctx(channel);

  if (!ctx_ptr)
  {
    return (SFH_status_Error);
  }
  switch (code)
  {
    case IFWD_DL_dll_get_parameter_ebl_info_boot_mode:
    {
      *ebl_info = (uint32_t)ctx_ptr->boot_ebl_version.boot_mode;
      break;
    }
    case IFWD_DL_dll_get_parameter_ebl_info_major_ver:
    {
      *ebl_info = (uint32_t)ctx_ptr->boot_ebl_version.major;
      break;
    }
    case IFWD_DL_dll_get_parameter_ebl_info_minor_ver:
    {
      *ebl_info = (uint32_t)ctx_ptr->boot_ebl_version.minor;
      break;
    }
    case IFWD_DL_dll_get_parameter_ebl_info_name:
    {
      *ebl_info = (intptr_t)(ctx_ptr->boot_ebl_version.name);
      break;
    }
    case IFWD_DL_dll_get_parameter_ebl_info_cust_cfg:
    {
      *ebl_info = (uint32_t)ctx_ptr->boot_ebl_version.boot_cfg_value;
      break;
    }
    case IFWD_DL_dll_get_parameter_ebl_info_expected_mem_class:
    {
      *ebl_info = (uint32_t)ctx_ptr->boot_ebl_version.expected_memory_class;
      break;
    }
  }
  return SFH_status_OK;
}

uint32_t SFH_get_nof_progressbars(uint8_t channel, char *filename, uint32_t *weights_output)
{
  TocEntryStructType toc_entry;
  TocStructType toc_main;
  uint32_t TOC_index;
  FILE_status_enum file_status;
  FILE *in;
  uint32_t i;
  uint32_t debug_offset;
  uint32_t file_offset;
  HardwareStructType hardware;
  MemoryMapStructType MemoryMap;
  SecurityStructType Security;
  uint32_t result = 0;
  SFH_ctx_type *ctx_ptr;

  ctx_ptr = get_ctx(channel);

  if(!filename || !ctx_ptr)
    return 0;

  if(filename[0] == (char)'|' )
  {
    //sscanf(filename+1,"%d%n",&TOC_index,&TOC_nof_chars);
    TOC_index = strtoul(filename+1, &filename, 10);
    if (TOC_index > MAX_TOC_LENGTH)        return 0; //{ xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_toc_size_overflow, TOC_index ));  goto __END__;  }
    filename++; /* Skip the "|nn|" part */
  }
  else
  {
    TOC_index = 0;
  }

  in = FILE_open(filename, FILE_open_mode_read);
  if (in == NULL)
    return 0;

  debug_offset = SFH_GetDebugHeaderOffset_file(in);
  PRGH_SetFileOffset(debug_offset);

  if(PRGH_FileCheck_File(in) == INVALID_FILE_TYPE)
  {
    FILE_close(in);
    if (ctx_ptr->handling_flags & HANDLE_BIN)
    {
      if (ctx_ptr->boot_dev_cfg.technology == NAND_FLASH)
      {
        return 1; // Just progress for download
      }
      else
      {
        return 2; // Progress for both erase and download
      }
    }
    return 0;
  }
  if(PRGH_VersionCheck_File(in,FLASHTOOL_TOOL_ELEM_TYPE, THIS_DLL_VERSION))
  {
    FILE_close(in);
    return 0;
  }

  /* Read the generic header info */
  PRGH_GetElementIndex_File(in,HARDWARE_ELEM_TYPE, GENERIC_UID, &file_offset);
  FILE_seek(in, file_offset+sizeof(ElementHeaderStructType));
  file_status = FILE_read(in, (uint8_t*)&hardware, sizeof(HardwareStructType));
  if(file_status != FILE_status_ok)
  {
    FILE_close(in);
    return 0;
  }

  PRGH_GetElementIndex_File(in,MEMORYMAP_ELEM_TYPE, GENERIC_UID, &file_offset);
  FILE_seek(in, file_offset+sizeof(ElementHeaderStructType));
  file_status = FILE_read(in, (uint8_t*)&MemoryMap, sizeof(MemoryMapStructType));
  if (file_status != FILE_status_ok)
  {
    FILE_close(in);
    return 0;
  }

  PRGH_GetElementIndex_File(in, TOC_ELEM_TYPE, GENERIC_UID, &file_offset);
  FILE_seek(in, file_offset+sizeof(ElementHeaderStructType));
  file_status = FILE_read(in, (uint8_t*)&toc_main, sizeof(TocStructType));
  if(file_status != FILE_status_ok)
  {
    FILE_close(in);
    return 0;
  }
  FILE_seek(in, toc_main.DataOffset+debug_offset);

  for (i=0 ; i < TOC_index + 1 ; i++) // step to the corect TOC index:
    file_status = FILE_read(in, (uint8_t*)&toc_entry, sizeof(TocEntryStructType));
  if(file_status != FILE_status_ok)
  {
    FILE_close(in);
    return 0;
  }

  PRGH_GetElementIndex_File(in,SECURITY_ELEM_TYPE, toc_entry.UID, &file_offset);
  FILE_seek(in, file_offset+sizeof(ElementHeaderStructType));
  file_status = FILE_read(in, (uint8_t*)&Security, sizeof(SecurityStructType));
  if(file_status != FILE_status_ok)
  {
    FILE_close(in);
    return 0;
  }
  if (Security.LoadMagic == LOAD_MAGIC)
  {
    if(GLOBAL_use_pre_erase)
    {
       for(i = 0 ; i < 8 ; i++)
      {
        if(Security.LoadMap[i].UsedLength != 0)
        {
          if(weights_output)
          {
            // The first is the erase progress which is way faster than sending data and flashing
            *weights_output++ = (Security.LoadMap[i].UsedLength) / 2;
            // Last is the actual download
            *weights_output++ = Security.LoadMap[i].UsedLength;
          }
          if (toc_entry.MemoryClass == STAT_EEP_CLASS)
          {
            result = result + 1;
          }
          else
          {
            result = result + 2;
          }
        }
      }
    }
    else
    {
      for(i = 0 ; i < 8 ; i++)
      {
        if(Security.LoadMap[i].UsedLength != 0)
        {
          if(weights_output)
            *weights_output++ = Security.LoadMap[i].UsedLength;
          result++;
          if ((GLOBAL_force_area_erase) && (toc_entry.MemoryClass == CODE_CLASS))
          {
            result++;
          }
        }
      }
    }
  }
  else
  {
    if ((toc_entry.MemoryClass == STAT_EEP_CLASS) && (GLOBAL_use_pre_erase))
    {
      result++;
    }

  }
  if(ctx_ptr)
  {
    if ((toc_entry.MemoryClass == CODE_CLASS) && (ctx_ptr->boot_ebl_version.boot_mode != BOOT_SEC_MODE))
    {
      result++;
    }
  }

  FILE_close(in);
  return result;
}

/*---------------------------------------------*/

int SFH_get_used_channel(void)
{
  SFH_ctx_type *ctx_ptr = ctx_base_ptr;
  while(ctx_ptr)
  {
    if(ctx_ptr->channel)
      return ctx_ptr->channel;
    ctx_ptr = ctx_ptr->next_ctx;
  }
  return 0;
}

uint8_t SFH_set_forced_boot_components(uint8_t channel, char *psi_file_name, char *ebl_file_name)
{
  SFH_ctx_type *ctx_ptr;
  // get the context
  ctx_ptr = get_ctx(channel);
  if (ctx_ptr)
  {
    if (psi_file_name)
    {
      snprintf(ctx_ptr->force_use_psi_file_name, MAX_PATH, "%s", psi_file_name);
    }
    else
    {
      ctx_ptr->force_use_psi_file_name[0] = 0;
    }

    if (ebl_file_name)
    {
      snprintf(ctx_ptr->force_use_ebl_file_name, MAX_PATH, "%s", ebl_file_name);
    }
    else
    {
      ctx_ptr->force_use_ebl_file_name[0] = 0;
    }
    return true;
  }
  else
  {
    return false;
  }

}
/*---------------------------------------------*/
SFH_status_enum  SFH_set_nvm_options(uint32_t nof_options, uint8_t *options)
{
  SFH_nvm_nof_options = nof_options;
  SFH_nvm_options = options;

  return SFH_status_OK;
}

SFH_status_enum SFH_prolific_reset_setup(uint8_t channel,
                                         char *reset_port_name,
                                         uint32_t gpio_number,
                                         char *status)
{
  SFH_ctx_type *ctx_ptr;

  SFH_status_enum result = SFH_status_Error; /* assume error for now */
  if ((0 == channel) || (channel > MAX_NUMBER_OF_CHANNELS))
    return result;

  if (!reset_port_name)
  {
    //Debug("Error in %s: Reset port name NULL", __func__);
    return (result);
  }

  if (reset_port_name[0] == 0)
  {
    //Debug("Error in %s: Reset port name not defined", __func__);
    return (result);
  }

  if (!status)
  {
    //Debug("Error in %s: Status not defined", __func__);
    return (result);
  }

  // get the context
  ctx_ptr = get_ctx(channel);
  if (!ctx_ptr)
  {
    // No context ... hmmm .... maybe we are called before an USB port is opened so use channel 0 for now.
    //Debug("SFH Open channel 0\n");
    COMPORT_init_prolific_board_reset((uint8_t)0, reset_port_name, gpio_number);
    result = SFH_status_OK;
  }
  else
  {
    // Check if the specified port matches a port already opened
    if (!strcmp(ctx_ptr->comm_port_name, reset_port_name))
    {
      // the port is the one already opened for this channel
      COMPORT_init_prolific_board_reset(channel, NULL, gpio_number);
      result = SFH_status_OK;

    }
    else
    {
      COMPORT_init_prolific_board_reset(channel, reset_port_name, gpio_number);
      result = SFH_status_OK;
    }
  }
  return (result);
}

/* SWISTART */
#ifndef SIERRA_CTF
SFH_status_enum SFH_secure_channel_setup(uint8_t  channel, uint32_t options, sec_ch_callback_type callback_fx, char **cert_files,  char *status)
{
  SFH_ctx_type *ctx_ptr;
  uint32_t ret_code;

  SFH_status_enum result = SFH_status_Error; // Assume error for now
  secure_channel_cb_fx   = NULL;             // Set to NULL to disable the secure channel feature in case of error.

  if ((channel < 1) || (channel > 32))
  {
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_channel_is_outside_range, channel) );
    return result;
  }

  if (channel != 1)
  {
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_fx_not_multi_ch, "secure_channel_setup") );
    return result;
  }

  ctx_ptr = get_ctx(channel);
  if (!ctx_ptr)
  {
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_channel_does_not_exist, channel) );
    return result;
  }

  // Call the setup function
  ret_code = sec_ch_setup(options, cert_files);
  if (ret_code == SECURE_CH_RES_SUCCESS)
  {
    if (callback_fx)
    {
      Debug("Secure channel: Setup COMPLETED (Private callback)\n");
      secure_channel_cb_fx = callback_fx;
      GLOBAL_use_sec_ch_ext_cb[channel] = 1;
    }
    else if (cert_files && cert_files[0])
    {
      Debug("Secure channel: Setup COMPLETED (Default callback)\n");
      secure_channel_cb_fx = sec_ch_callback;
      GLOBAL_use_sec_ch_ext_cb[channel] = 0;
    }
    else
    {
      Debug("Secure channel: Setup COMPLETED (No callback)\n");
      secure_channel_cb_fx = NULL;
      GLOBAL_use_sec_ch_ext_cb[channel] = 0;
    }

    result = SFH_status_OK;
  }
  else
  {
    char error_txt[32]; // Below the longest text is 25 bytes.

    switch(ret_code)
    {
      case SECURE_CH_RES_ERROR:          { sprintf(error_txt,"General error"); break; }
      case SECURE_CH_RES_INVALID_PARAM:  { sprintf(error_txt,"Invalid parameter"); break; }
      case SECURE_CH_RES_MALLOC_FAILED:  { sprintf(error_txt,"Memory allocation failed"); break; }
      case SECURE_CH_RES_SIGNMOD_ERROR:  { sprintf(error_txt,"Signing error"); break; }
      case SECURE_CH_RES_FILE_ERROR:     { sprintf(error_txt,"File IO error"); break; }
      case SECURE_CH_RES_INVALID_REQ_ID: { sprintf(error_txt,"Invalid request"); break; }
      default: { sprintf(error_txt,"Unknown error"); }
    }
    PrintError("Secure channel: Setup FAILED [%s]\n",error_txt);
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_secure_ch_setup_fail, error_txt) );
    return result;
  }

  return result;
}
#endif /* SIERRA_CTF */
/* SWISTOP */

