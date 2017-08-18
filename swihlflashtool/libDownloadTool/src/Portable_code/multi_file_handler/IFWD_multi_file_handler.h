
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
 *   IFWD multi file handler - interface
 *     The purpose of the system handler is to do the sequencing in case multiple
 *     files download or update processes must be carried out as a group.
 *     This is the case when using SPK (Software pack) files.
 *
 * Revision Information:
 *  $File name:  /msw_tools/FlashTool/libDownloadTool/src/Portable_code/multi_file_handler/IFWD_multi_file_handler.h $
 *  $CC-Version: .../dhkristx_140314_v0/2 $
 *  $Date:       2014-05-01    12:44:47 UTC $
 *   Responsible: kofod
 *   Comment:
 *     Solution for NVM handling
 */



#if defined (TEXT_CLASS_IFWD_ERRORS)
TEXT_DEF(MFH_ERR_not_yet_implemented,       "Function %s is not yet implemented.")                        /* 01 */
TEXT_DEF(MFH_ERR_channel_already_opened,    "Channel %d cannot be opened twice.")                         /* 02 */
TEXT_DEF(MFH_ERR_malloc,                    "Could not allocate memory.")                                 /* 03 */
TEXT_DEF(MFH_ERR_channel_does_not_exist,    "Can not delete the non-existing channel %d.")                /* 04 */
TEXT_DEF(MFH_ERR_sys_version_not_found,     "Could not find System Version string")                       /* 05 */
TEXT_DEF(MFH_ERR_spare_06,             "")                                                                /* 06 */
TEXT_DEF(MFH_ERR_spare_07,             "")                                                                /* 07 */
TEXT_DEF(MFH_ERR_spare_08,             "")                                                                /* 08 */
TEXT_DEF(MFH_ERR_spare_09,             "")                                                                /* 09 */
TEXT_DEF(MFH_ERR_spare_10,             "")                                                                /* 10 */
TEXT_DEF(MFH_ERR_spare_11,             "")                                                                /* 11 */
TEXT_DEF(MFH_ERR_spare_12,             "")                                                                /* 12 */
TEXT_DEF(MFH_ERR_spare_13,             "")                                                                /* 13 */
TEXT_DEF(MFH_ERR_spare_14,             "")                                                                /* 14 */
TEXT_DEF(MFH_ERR_spare_15,             "")                                                                /* 15 */
TEXT_DEF(MFH_ERR_spare_16,             "")                                                                /* 16 */
TEXT_DEF(MFH_ERR_spare_17,             "")                                                                /* 17 */
TEXT_DEF(MFH_ERR_spare_18,             "")                                                                /* 18 */
TEXT_DEF(MFH_ERR_spare_19,             "")                                                                /* 19 */
TEXT_DEF(MFH_ERR_spare_20,             "")                                                                /* 20 */
TEXT_DEF(MFH_ERR_spare_21,             "")                                                                /* 21 */
TEXT_DEF(MFH_ERR_spare_22,             "")                                                                /* 22 */
TEXT_DEF(MFH_ERR_spare_23,             "")                                                                /* 23 */
TEXT_DEF(MFH_ERR_spare_24,             "")                                                                /* 24 */
TEXT_DEF(MFH_ERR_spare_25,             "")                                                                /* 25 */
TEXT_DEF(MFH_ERR_spare_26,             "")                                                                /* 26 */
TEXT_DEF(MFH_ERR_spare_27,             "")                                                                /* 27 */
TEXT_DEF(MFH_ERR_spare_28,             "")                                                                /* 28 */
TEXT_DEF(MFH_ERR_spare_29,             "")                                                                /* 29 */
TEXT_DEF(MFH_ERR_spare_30,             "")                                                                /* 30 */
TEXT_DEF(MFH_ERR_spare_31,             "")                                                                /* 31 */
TEXT_DEF(MFH_ERR_spare_32,             "")                                                                /* 32 */
TEXT_DEF(MFH_ERR_spare_33,             "")                                                                /* 33 */
TEXT_DEF(MFH_ERR_spare_34,             "")                                                                /* 34 */
TEXT_DEF(MFH_ERR_spare_35,             "")                                                                /* 35 */
TEXT_DEF(MFH_ERR_spare_36,             "")                                                                /* 36 */
TEXT_DEF(MFH_ERR_spare_37,             "")                                                                /* 37 */
TEXT_DEF(MFH_ERR_spare_38,             "")                                                                /* 38 */
TEXT_DEF(MFH_ERR_spare_39,             "")                                                                /* 39 */
TEXT_DEF(MFH_ERR_spare_40,             "")                                                                /* 40 */
TEXT_DEF(MFH_ERR_spare_41,             "")                                                                /* 41 */
TEXT_DEF(MFH_ERR_spare_42,             "")                                                                /* 42 */
TEXT_DEF(MFH_ERR_spare_43,             "")                                                                /* 43 */
TEXT_DEF(MFH_ERR_spare_44,             "")                                                                /* 44 */
TEXT_DEF(MFH_ERR_spare_45,             "")                                                                /* 45 */
TEXT_DEF(MFH_ERR_spare_46,             "")                                                                /* 46 */
TEXT_DEF(MFH_ERR_spare_47,             "")                                                                /* 47 */
TEXT_DEF(MFH_ERR_spare_48,             "")                                                                /* 48 */
TEXT_DEF(MFH_ERR_spare_49,             "")                                                                /* 49 */
TEXT_DEF(MFH_ERR_spare_50,             "")                                                                /* 50 */

#elif defined (TEXT_CLASS_IFWD_INFO)
TEXT_DEF(MFH_INFO_channel_opened,  "Channel number %d is succesfully opened.")      /* 01 */
TEXT_DEF(MFH_INFO_spare_02,        "")                                              /* 02 */
TEXT_DEF(MFH_INFO_spare_03,        "")                                              /* 03 */
TEXT_DEF(MFH_INFO_spare_04,        "")                                              /* 04 */
TEXT_DEF(MFH_INFO_spare_05,        "")                                              /* 05 */
TEXT_DEF(MFH_INFO_spare_06,        "")                                              /* 06 */
TEXT_DEF(MFH_INFO_spare_07,        "")                                              /* 07 */
TEXT_DEF(MFH_INFO_spare_08,        "")                                              /* 08 */
TEXT_DEF(MFH_INFO_spare_09,        "")                                              /* 09 */
TEXT_DEF(MFH_INFO_spare_10,        "")                                              /* 10 */
TEXT_DEF(MFH_INFO_spare_11,        "")                                              /* 11 */
TEXT_DEF(MFH_INFO_spare_12,        "")                                              /* 12 */
TEXT_DEF(MFH_INFO_spare_13,        "")                                              /* 13 */
TEXT_DEF(MFH_INFO_spare_14,        "")                                              /* 14 */
TEXT_DEF(MFH_INFO_spare_15,        "")                                              /* 15 */
TEXT_DEF(MFH_INFO_spare_16,        "")                                              /* 16 */
TEXT_DEF(MFH_INFO_spare_17,        "")                                              /* 17 */
TEXT_DEF(MFH_INFO_spare_18,        "")                                              /* 18 */
TEXT_DEF(MFH_INFO_spare_19,        "")                                              /* 19 */
TEXT_DEF(MFH_INFO_spare_20,        "")                                              /* 20 */
TEXT_DEF(MFH_INFO_spare_21,        "")                                              /* 21 */
TEXT_DEF(MFH_INFO_spare_22,        "")                                              /* 22 */
TEXT_DEF(MFH_INFO_spare_23,        "")                                              /* 23 */
TEXT_DEF(MFH_INFO_spare_24,        "")                                              /* 24 */
TEXT_DEF(MFH_INFO_spare_25,        "")                                              /* 25 */
TEXT_DEF(MFH_INFO_spare_26,        "")                                              /* 26 */
TEXT_DEF(MFH_INFO_spare_27,        "")                                              /* 27 */
TEXT_DEF(MFH_INFO_spare_28,        "")                                              /* 28 */
TEXT_DEF(MFH_INFO_spare_29,        "")                                              /* 29 */
TEXT_DEF(MFH_INFO_spare_30,        "")                                              /* 30 */
TEXT_DEF(MFH_INFO_spare_31,        "")                                              /* 31 */
TEXT_DEF(MFH_INFO_spare_32,        "")                                              /* 32 */
TEXT_DEF(MFH_INFO_spare_33,        "")                                              /* 33 */
TEXT_DEF(MFH_INFO_spare_34,        "")                                              /* 34 */
TEXT_DEF(MFH_INFO_spare_35,        "")                                              /* 35 */
TEXT_DEF(MFH_INFO_spare_36,        "")                                              /* 36 */
TEXT_DEF(MFH_INFO_spare_37,        "")                                              /* 37 */
TEXT_DEF(MFH_INFO_spare_38,        "")                                              /* 38 */
TEXT_DEF(MFH_INFO_spare_39,        "")                                              /* 39 */
TEXT_DEF(MFH_INFO_spare_40,        "")                                              /* 40 */
TEXT_DEF(MFH_INFO_spare_41,        "")                                              /* 41 */
TEXT_DEF(MFH_INFO_spare_42,        "")                                              /* 42 */
TEXT_DEF(MFH_INFO_spare_43,        "")                                              /* 43 */
TEXT_DEF(MFH_INFO_spare_44,        "")                                              /* 44 */
TEXT_DEF(MFH_INFO_spare_45,        "")                                              /* 45 */
TEXT_DEF(MFH_INFO_spare_46,        "")                                              /* 46 */
TEXT_DEF(MFH_INFO_spare_47,        "")                                              /* 47 */
TEXT_DEF(MFH_INFO_spare_48,        "")                                              /* 48 */
TEXT_DEF(MFH_INFO_spare_49,        "")                                              /* 49 */
TEXT_DEF(MFH_INFO_spare_50,        "")                                              /* 50 */
#else

#ifndef _INC_MFH
#define _INC_MFH

#include "FlsFile.h"

/*---------------------------------------------*/
/* Enumeration                                 */
/*---------------------------------------------*/
typedef enum
{
  MFH_status_OK = 0,
  MFH_status_wait_for_event,
  MFH_status_Error
} MFH_status_enum;

typedef enum
{
  MFH_done_signal_void = 0,  /* Do not signal 'done' or 'error' in any way */
  MFH_done_signal_callback,  /* Use the call back function to signal 'done' and 'error' */
  MFH_done_signal_semaphore  /* Use a semaphore or and event to signal 'done' and 'error' */
} MFH_done_signal_enum;


typedef enum MFH_debug_s
 {
  MFH_db_add_channel,
  MFH_db_delete_channel,
  MFH_db_get_status,
  MFH_db_abort_process,
  MFH_db_entry_not_valid = 0xFFFF
} MFH_debug_t;

typedef enum
{
  MFH_dffs_load_sel_none = 0, /* No part of the DFFS file is downloaded.  */
  MFH_dffs_load_sel_static,   /* Only the static part of the DFFS file is downloaded */
  MFH_dffs_load_sel_dynamic,  /* Only the dynamic part of the DFFS file is downloaded */
  MFH_dffs_load_sel_both,      /* Both parts are downloaded */
  MFH_dffs_load_sel_nand_disk  /* nand disk type are donwloaded */
} MFH_dffs_load_sel_enum;

/*---------------------------------------------*/
/* Structures                                  */
/*---------------------------------------------*/

/*---------------------------------------------*/
/* External defines                            */
/*---------------------------------------------*/

/*---------------------------------------------*/
/* External declarations                       */
/*---------------------------------------------*/

/*---------------------------------------------*/
/* External functions                          */
/*---------------------------------------------*/

void MFH_clock(void);
void MFH_init(void);
void MFH_close(void);

uint8_t MFH_add_channel(uint8_t channel, char *status);

uint8_t MFH_delete_channel(uint8_t channel, char *status);

MFH_status_enum MFH_get_status(uint8_t channel, char *status_text);

MFH_status_enum MFH_abort_process(uint8_t channel, char *status_text);

MFH_status_enum MFH_open_comm_port(uint8_t channel,
                                   char *comm_port_name,
                                   char *ctrl_port_name,
                                   uint32_t baud_rate,
                                   char *status);

MFH_status_enum MFH_close_comm_port(uint8_t channel, char *status);

//static TocStructType* MFH_TOC_read_from_file(char* filename);

MFH_status_enum  MFH_TOC_get_nof_items(char *filename, uint32_t *nof_output);
MFH_status_enum  MFH_TOC_get_memory_class(char *filename, uint32_t toc_index, uint32_t *mem_class_output);
MFH_status_enum  MFH_TOC_get_filename(char *filename, uint32_t toc_index, char *filename_output);
MFH_status_enum  MFH_TOC_get_checksum(char *filename, uint32_t toc_index, uint32_t *checksum_output);
MFH_status_enum  MFH_TOC_get_filesize(char *filename, uint32_t toc_index, uint32_t *filesize_output);
MFH_status_enum  MFH_TOC_get_flash_technogy(char *filename, uint32_t *flash_technology_output);
MFH_status_enum  MFH_TOC_get_hw_platform(char *filename, uint32_t *hw_platform_output);
MFH_status_enum  MFH_TOC_get_sw_option(char *filename, uint32_t toc_index, uint32_t option_code, uint32_t *option_result);

MFH_status_enum  MFH_get_sys_version(const char *filename, SystemVersionLabelType *option_result);

MFH_status_enum  MFH_ZIP_get_size(uint32_t *size, char *filename);
MFH_status_enum  MFH_ZIP_get_data(uint32_t *data, char *filename);

MFH_status_enum MFH_NVM_get_nof_user_options(char *filename, uint32_t *nof_options);
MFH_status_enum MFH_NVM_get_user_option(char *filename, uint32_t index, uint32_t *nvm_tag);
MFH_status_enum MFH_NVM_set_user_option(char *filename,uint32_t index, uint8_t value);

#endif
#endif


