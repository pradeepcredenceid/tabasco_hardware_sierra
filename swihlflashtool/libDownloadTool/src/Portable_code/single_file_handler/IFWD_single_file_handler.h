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
 *   IFWD single file handler - interface
 *     The purpose of this handler is to do the processing and sequencing
 *     needed to carry out single downloads or updates.
 *
 * Revision Information:
 *  $File name:  /msw_tools/FlashTool/libDownloadTool/src/Portable_code/single_file_handler/IFWD_single_file_handler.h $
 *  $CC-Version: .../oint_tools_flashtoole2_01/14 $
 *  $Date:       2014-03-13    15:40:34 UTC $
 *   Comment:
 *     SMS00934737 Solution for certificate flashing
 */



#if defined (TEXT_CLASS_IFWD_ERRORS)
TEXT_DEF(SFH_ERR_not_yet_implemented,       "Function %s is not yet implemented.")                        /* 01 */
TEXT_DEF(SFH_ERR_channel_is_outside_range,  "Channel number %d is outside the valid range.")              /* 02 */
TEXT_DEF(SFH_ERR_open_port,                 "Port %s could not be opened.")                               /* 03 */
TEXT_DEF(SFH_ERR_close_port,                "Channel %d cannot be closed.")                               /* 04 */
TEXT_DEF(SFH_ERR_channel_already_opened,    "Channel %d cannot be opened twice.")                         /* 05 */
TEXT_DEF(SFH_ERR_channel_does_not_exist,    "Channel %d is not in use.")                                  /* 06 */
TEXT_DEF(SFH_ERR_channel_close,             "Channel %d cannot be closed.")                               /* 07 */
TEXT_DEF(SFH_ERR_wrong_header_type,         "The file header version is wrong.")                          /* 08 */
TEXT_DEF(SFH_ERR_malloc,                    "Could not allocate memory.")                                 /* 09 */
TEXT_DEF(SFH_ERR_free,                      "Could not free memory.")                                     /* 10 */
TEXT_DEF(SFH_ERR_cant_boot_while_busy,      "Can not start boot when not in idle state.")                 /* 11 */
TEXT_DEF(SFH_ERR_file_not_found,            "File %s not found.")                                         /* 12 */
TEXT_DEF(SFH_ERR_file_error,                "File Error: %s")                                             /* 13 */
TEXT_DEF(SFH_ERR_boot_error,                "Error during boot")                                          /* 14 */
TEXT_DEF(SFH_ERR_baudrate_error,            "Could not change baudrate")                                  /* 15 */
TEXT_DEF(SFH_ERR_sending_command,           "Could not send command.")                                    /* 16 */
TEXT_DEF(SFH_ERR_verify,                    "Could not verify downloaded image.")                         /* 17 */
TEXT_DEF(SFH_ERR_setup_confirm,             "Could not set-up confirm parameters.")                       /* 18 */
TEXT_DEF(SFH_ERR_multi_dffs,                "Multi DFFS sections are not handled yet.")                   /* 19 */
TEXT_DEF(SFH_ERR_less_than_loadmap,         "Address is less than the current loadmap address.")          /* 20 */
TEXT_DEF(SFH_ERR_memory_map_mismatch,       "The memory map does not match between files %s and %s")      /* 21 */
TEXT_DEF(SFH_ERR_checksum_error,            "Expected checksum %04X got %04X")                            /* 22 */
TEXT_DEF(SFH_ERR_wrong_split_id,            "File split-ID %08X does not match target %08X")              /* 23 */
TEXT_DEF(SFH_ERR_not_in_AT_mode,            "Channel %d is not in AT mode. Nothing done.")                /* 24 */
TEXT_DEF(SFH_ERR_err_AT_mode_stop,          "Error stopping AT mode.")                                    /* 25 */
TEXT_DEF(SFH_ERR_not_in_idle,               "Channel %d is not in idle. Nothing done.")                   /* 26 */
TEXT_DEF(SFH_ERR_not_enough_mem_to_read_eep,"Not enough memory to read eeprom file.")                                            /* 27 */
TEXT_DEF(SFH_ERR_startup_address_error,     "Address for startup mode (Test/Normal mode) outside eeprom memory address range")   /* 28 */
TEXT_DEF(SFH_ERR_ffs_only_stat,             "The DFFS file does not suport download of static part alone.")                      /* 29 */
TEXT_DEF(SFH_ERR_ffs_only_dyn,              "The DFFS file does not suport download of dynamic part alone.")                     /* 30 */
TEXT_DEF(SFH_ERR_eep_linearize,             "The EEPROM linearize procedure failed.")                                            /* 31 */
TEXT_DEF(SFH_ERR_wrong_file_type,           "Wrong file type")                                                                   /* 32 */
TEXT_DEF(SFH_ERR_wrong_vesion,              "Can't use this version of FlashTool; Use version: %d.%d")                           /* 33 */
TEXT_DEF(SFH_ERR_mem_class_not_found,       "Could not find a suitable memory class in the TOC")                                 /* 34 */
TEXT_DEF(SFH_ERR_erase_all_NOR,             "The erase-all mode is not implemented for NOR flash types yet")                     /* 35 */
TEXT_DEF(SFH_ERR_erase_all_CODE,            "The erase-all mode is only allowed for boot-system/code images")                    /* 36 */
TEXT_DEF(SFH_ERR_no_nand_ebl,               "An EBL must be injected. None was found in the downloaded file.")                   /* 37 */
TEXT_DEF(SFH_ERR_dep_filename_error,        "DEP filename naming convention not correct")                                        /* 38 */
TEXT_DEF(SFH_ERR_emulation_cb_not_set,      "Cannot set emulation before the Tx,Rx and RxAvail callbacks are set.")              /* 39 */
TEXT_DEF(SFH_ERR_data_size_overflow,        "%s too large: %d bytes (Maximum is 1 MB)")                                          /* 40 */
TEXT_DEF(SFH_ERR_toc_size_overflow,         "Too many files in Table Of Content: %d files (Maximum is 256)")                     /* 41 */
TEXT_DEF(SFH_ERR_unknown_mem_class,         "Memory Class %d unknown")                                                           /* 42 */
TEXT_DEF(SFH_ERR_boot_timeout,              "USB Boot Timeout")                                                                  /* 43 */
TEXT_DEF(SFH_ERR_raw_download,              "Communication error during RAW download")                                           /* 44 */
TEXT_DEF(SFH_ERR_fx_not_multi_ch,           "Function %s does not support multi channel use")                                    /* 45 */
TEXT_DEF(SFH_ERR_secure_ch_setup_fail,      "Secure channel setup failed.  %s")                                                  /* 46 */
TEXT_DEF(SFH_ERR_secure_ch_cb_fail,         "Secure channel call-back failed fith code %d")                                      /* 47 */
TEXT_DEF(SFH_ERR_secure_ch_sign_fail,       "Secure channel SW version verdict: %s Error: Authentication failed!")               /* 48 */
TEXT_DEF(SFH_ERR_option_not_supported,      "Option [%s] is not supported - process aborted")                                    /* 49 */
// TEXT_DEF(SFH_ERR_spare_50,         "")                                                                                        /* 50 */

#elif defined (TEXT_CLASS_IFWD_INFO)
TEXT_DEF(SFH_INFO_channel_opened,   "Channel %d is succesfully opened.")       /* 01 */
TEXT_DEF(SFH_INFO_channel_closed,   "Channel %d is succesfully closed.")       /* 02 */
TEXT_DEF(SFH_INFO_boot_start,       "Start boot process.")                     /* 03 */
TEXT_DEF(SFH_INFO_bootloader_run,   "Boot-loader is active")                   /* 04 */
TEXT_DEF(SFH_INFO_boot_ok,          "Boot process finished\n")                 /* 05 */
TEXT_DEF(SFH_INFO_boot_get_id,      "Get flash id.")                           /* 06 */
TEXT_DEF(SFH_INFO_boot_cfi_1,       "CFI stage 1")                             /* 07 */
TEXT_DEF(SFH_INFO_boot_cfi_2,       "CFI stage 2")                             /* 08 */
TEXT_DEF(SFH_INFO_boot_high_speed,  "Baud rate set to %d")                     /* 09 */
TEXT_DEF(SFH_INFO_load_start,       "Start downloading item '%s:%s' from file '%s'.") /* 10 */
TEXT_DEF(SFH_INFO_upload_start,     "Start uploading to file %s.")             /* 11 */
TEXT_DEF(SFH_INFO_load_preerase,    "  - Pre-erasing.")                            /* 12 */
TEXT_DEF(SFH_INFO_load_send_data,   "  - Sending data. (%s)")                      /* 13 */
TEXT_DEF(SFH_INFO_upload_rx_data,   "  - Receiving data.")                         /* 14 */
TEXT_DEF(SFH_INFO_load_crc_ok,      "Checksum OK (0x%04X)")                    /* 15 */
TEXT_DEF(SFH_INFO_load_send_sec,    "Sending sec-pack.")                       /* 16 */
TEXT_DEF(SFH_INFO_load_send_end,    "Sending end-pack.")                       /* 17 */
TEXT_DEF(SFH_INFO_load_time,        "Process time was %d min %d sec.\n")       /* 18 */
TEXT_DEF(SFH_INFO_verify,           "Verify OK")                               /* 19 */
TEXT_DEF(SFH_INFO_upload_ok,        "Upload OK")                               /* 20 */
TEXT_DEF(SFH_INFO_erase,            "  - Erasing. (%s)")                           /* 21 */
TEXT_DEF(SFH_INFO_erase_all_bad,    "Erasing all flash memory incl bad blocks") /* 22 */
TEXT_DEF(SFH_INFO_secure_ch_ok,     "Secure channel SW version verdict: %s")   /* 23 */
TEXT_DEF(SFH_INFO_use_special_option, "PLEASE NOTE: Special Option [%s] is enabled") /* 24 */
TEXT_DEF(SFH_INFO_load_region,      "  Load region %d")                          /* 25 */
TEXT_DEF(SFH_INFO_erase_dyn_eep,    "Erasing the dynamic eeprom area")         /* 26 */
TEXT_DEF(SFH_INFO_skip_sec_loader,  "Skip secure loader")                      /* 27 */
TEXT_DEF(SFH_INFO_get_sw_ver,       "Reading SW version data")                 /* 28 */
TEXT_DEF(SFH_INFO_loader_ver,       "EBL version: %s %d.%d")                   /* 29 */
TEXT_DEF(SFH_INFO_boot_mode,        "Boot mode is: %02X")                      /* 30 */
TEXT_DEF(SFH_INFO_flash_id,         "Flash ID is: %08X")                       /* 31 */
TEXT_DEF(SFH_INFO_update_eep,       "Update eeprom from file: %s")             /* 32 */
TEXT_DEF(SFH_INFO_cur_eep_cfg,      "Reading eeprom using file: %s")           /* 33 */
TEXT_DEF(SFH_INFO_new_eep_cfg,      "Writing eeprom using file: %s")           /* 34 */
TEXT_DEF(SFH_INFO_load_time_sec,    "Process time was %s sec.\n")              /* 35 */
TEXT_DEF(SFH_INFO_load_time_msec,   "Process time was %d msec.\n")             /* 36 */
TEXT_DEF(SFH_INFO_boot_timeout,     "USB Boot Timeout")                        /* 37 */
TEXT_DEF(SFH_INFO_package_len,      "Package length is %i - protocol: %s")     /* 38 */
TEXT_DEF(SFH_INFO_using,            "Using")                                   /* 39 */
TEXT_DEF(SFH_INFO_faster_crc,       "Faster CRC")                              /* 40 */
TEXT_DEF(SFH_INFO_normal_crc,       "Normal CRC")                              /* 41 */
TEXT_DEF(SFH_INFO_skip_blocks,      "Skipping Empty blocks")                   /* 42 */
TEXT_DEF(SFH_INFO_all_blocks,       "Writing all blocks")                      /* 43 */
TEXT_DEF(SFH_INFO_erase_all,        "Erasing all flash memory")                /* 44 */
TEXT_DEF(SFH_INFO_no_prot_crc,      ", Skip protocol CRC")                     /* 45 */
TEXT_DEF(SFH_INFO_no_data_crc,      ", Skip data CRC")                         /* 46 */
TEXT_DEF(SFH_INFO_abort_process,    "Process stopped due to special option set")  /* 47 */
TEXT_DEF(SFH_INFO_tot_load_time,    "Total time was %d min %d sec.")           /* 48 */
TEXT_DEF(SFH_INFO_tot_load_time_sec,"Total time was %s sec.")                  /* 49 */
TEXT_DEF(SFH_INFO_tot_load_time_msec,"Total time was %d msec.")                /* 50 */

#else

#ifndef _INC_SFH
#define _INC_SFH
#include "IMC_DownloadDll.h"

/*---------------------------------------------*/
/* Enumeration                                 */
/*---------------------------------------------*/

typedef enum
{
  GLOBAL_SFH_ext_debug_type_open_port_ctx = 0,
  GLOBAL_SFH_ext_debug_type_ctx_after_del_ch
} GLOBAL_SFH_ext_debug_type_enum;

typedef enum
{
  SFH_status_OK = 0,
  SFH_status_wait_for_event,
  SFH_status_Error
} SFH_status_enum;

typedef enum
{
  SFH_done_signal_void = 0,  /* Do not signal 'done' or 'error' in any way */
  SFH_done_signal_callback,  /* Use the call back function to signal 'done' and 'error' */
  SFH_done_signal_semaphore  /* Use a semaphore or and event to signal 'done' and 'error' */
} SFH_done_signal_enum;

typedef enum
{
  SFH_target_boot_mode_normal = 0, /* The phone start in normal mode after EEP file download */
  SFH_target_boot_mode_test        /* The phone start in test mode after EEP file download */
} SFH_target_boot_mode_enum;

typedef enum
{
  SFH_dffs_load_sel_none = 0, /* No part of the DFFS file is downloaded.  */
  SFH_dffs_load_sel_static,   /* Only the static part of the DFFS file is downloaded */
  SFH_dffs_load_sel_dynamic,  /* Only the dynamic part of the DFFS file is downloaded */
  SFH_dffs_load_sel_both,      /* Both parts are downloaded */
  SFH_dffs_load_sel_nand_disk  /* nand disk type are donwloaded */
} SFH_dffs_load_sel_enum;

typedef enum SFH_debug_s
 {
  SFH_db_add_channel,
  SFH_db_delete_channel,
  SFH_db_get_status,
  SFH_db_abort_process,
  SFH_db_download_fls_file,
  SFH_db_download_eep_file,
  SFH_db_download_dffs_file,
  SFH_db_download_cust_file,
  SFH_db_upload_bin_image,
  SFH_db_download_bin_image,
  SFH_db_erase_image,
  SFH_db_open_comm_port,
  SFH_db_close_comm_port,
  SFH_db_boot_target,
  SFH_db_boot_bin_target,
  SFH_db_control_modem_signals,
  SFH_db_set_error_string,
  SFH_db_get_sw_version_data,
  SFH_db_update_eep,
  SFH_db_entry_not_valid = 0xFFFF
 } SFH_debug_t;

/*---------------------------------------------*/
/* Structures                                  */
/*---------------------------------------------*/
typedef struct SectionInfoStructTag
{
   uint32_t Sectors;
   uint32_t Size;
}SectionInfoStruct;

typedef struct FlashInfoStructTag
{
   uint32_t CfiCompliant;
   uint32_t Manufacturer;
   uint32_t ID;
   uint32_t Algorithm;
   uint32_t PreErase;
   uint32_t MaxBufferWrite;
   uint32_t Regions;
   uint32_t Size;
   SectionInfoStruct Sections[4];
} FlashInfoStruct;

typedef FlashInfoStruct flash_cfi_info_typ[4];
/*---------------------------------------------*/
/* External defines                            */
/*---------------------------------------------*/
extern char ini_file_name[400];

/*---------------------------------------------*/
/* External functions                          */
/*---------------------------------------------*/
void SFH_clock(void);
void SFH_init(void);
void SFH_close(void);

uint8_t SFH_add_channel(uint8_t channel, char *status);

uint8_t SFH_delete_channel(uint8_t channel, char *status);

SFH_status_enum SFH_get_status(uint8_t channel, char *status_text);

SFH_status_enum SFH_abort_process(uint8_t channel, char *status_text);

SFH_status_enum SFH_download_fls_file (uint8_t channel,
                            char *fls_file_name,
                            SFH_done_signal_enum use_done_signal,
                            char *status);

SFH_status_enum SFH_download_eep_file (uint8_t channel,
                            char *eep_file_name,
                            SFH_target_boot_mode_enum boot_mode,
                            SFH_done_signal_enum use_done_signal,
                            char *status);

SFH_status_enum SFH_download_dffs_file (uint8_t channel,
                             char *dffs_file_name,
                             SFH_dffs_load_sel_enum load_selection,
                             SFH_done_signal_enum use_done_signal,
                             char *status);

SFH_status_enum SFH_download_cust_file (uint8_t channel,
                             char *cust_file_name,
                             SFH_done_signal_enum use_done_signal,
                             char *status);

SFH_status_enum SFH_update_static_eeprom (uint8_t channel,
                               char *dep_file_name,
                               char *project_prefix_string,
                               char *ref_eep_file_name,
                               SFH_done_signal_enum use_done_signal,
                               char *status);

SFH_status_enum SFH_upload_bin_image(uint8_t channel,
                          uint32_t image_start_address,
                          uint32_t image_length,
                          char *bin_file_name,
                          char *ref_file_name,
                          SFH_done_signal_enum use_done_signal,
                          char *status);

SFH_status_enum SFH_download_bin_image(uint8_t channel,
                          uint32_t image_start_address,
                          uint32_t image_length,
                          uint32_t image_offset,
                          char *bin_file_name,
                          char *ref_file_name,
                          SFH_done_signal_enum use_done_signal,
                          char *status);

SFH_status_enum SFH_erase_image(uint8_t channel,
                          uint32_t image_start_address,
                          uint32_t image_length,
                          char *ref_file_name,
                          SFH_done_signal_enum use_done_signal,
                          char *status);

SFH_status_enum SFH_open_comm_port(uint8_t channel,
                        char *comm_port_name,
                        char *ctrl_port_name,
                        uint32_t baud_rate,
                        char *status);

SFH_status_enum SFH_close_comm_port(uint8_t channel, char *status);

SFH_status_enum SFH_boot_target(uint8_t channel,
                     char *ref_file,
                     IFWD_DL_modem_control_signals_type *modem_control_signals,
                     SFH_done_signal_enum use_done_signal,
                     char *status);

SFH_status_enum SFH_pre_boot_target(uint8_t channel,
                     uint32_t platform_code,
                     SFH_done_signal_enum use_done_signal,
                     char *status);

SFH_status_enum SFH_boot_bin_target(uint8_t channel,
                     uint8_t *ref_file_header,
                     IFWD_DL_modem_control_signals_type *modem_control_signals,
                     SFH_done_signal_enum use_done_signal,
                     char *status);

SFH_status_enum SFH_control_modem_signals(uint8_t channel,
                     IFWD_DL_modem_control_signals_type *modem_control_signals,
                     char *status);

void SFH_set_error_string(uint8_t channel, char *error_text);

SFH_status_enum SFH_get_sw_version_data(uint8_t channel,
                                        char *fls_file_name,
                                        uint8_t *file_data,
                                        uint8_t *taget_data,
                                        uint16_t data_size,
                                        SFH_done_signal_enum use_done_signal,
                                        char *status);

SFH_status_enum SFH_force_target_reset(uint8_t channel,
                                       uint8_t mode,
                                       uint8_t p1,
                                       uint8_t p2,
                                       uint8_t p3,
                                       SFH_done_signal_enum use_done_signal,
                                       char *status);

SFH_status_enum SFH_end_of_process(uint8_t channel,
                                   SFH_done_signal_enum use_done_signal,
                                   char *status);

/* Function for handling AT-mode: */

void SFH_data_received(uint8_t channel, uint16_t length); /* COMM reports received data by calling this function */

SFH_status_enum  SFH_start_AT_mode(uint8_t channel, char *status);
SFH_status_enum  SFH_stop_AT_mode(uint8_t channel, char *status);
SFH_status_enum  SFH_AT_send(uint8_t channel, char *data, uint16_t length, char *status);


SFH_status_enum  SFH_get_flash_id(uint8_t channel, uint32_t *flash_id);
SFH_status_enum  SFH_get_flash_voltage(uint8_t channel, uint32_t *flash_voltage);
SFH_status_enum  SFH_get_ebl_info(uint8_t code, uint8_t channel, uint32_t *ebl_info);

SFH_status_enum  SFH_set_nvm_options(uint32_t nof_options, uint8_t *options);

uint32_t SFH_get_nof_progressbars(uint8_t channel, char *filename, uint32_t *weights_output);
int SFH_get_used_channel(void);
SFH_status_enum SFH_prolific_reset_setup(uint8_t channel, char *reset_port_name, uint32_t gpio_number, char *status);

uint8_t SFH_set_forced_boot_components(uint8_t channel, char *psi_file_name, char *ebl_file_name);


/**
  Call this function to perform an action over the OutOfSessionComm interface.

   @param channel  The channel to download to (1 to N). If only one channel is used, then 1 is used.
   @param action   An action as defined by #IFWD_DL_oosc_action_enum.
   @param type     A data type as defined by #IFWD_DL_oosc_type_enum.
   @param length   The length in bytes of the data to be transferred.
   @param spare    Currently not used. Must be set to 0x00000000.
   @param *data    Pointer to the structure owned by the caller where the data is read from or written to
   @return #SFH_status_enum  SFH_status_OK or SFH_status_Error. If SFH_status_OK is return and callbacks are used, then wait for the callback result.
*/
SFH_status_enum SFH_out_of_session_comm(uint8_t  channel,
                                               uint32_t action,
                                               uint32_t type,
                                               uint32_t length,
                                               uint32_t spare,
                                               uint8_t  *data,
                                               SFH_done_signal_enum use_done_signal);

SFH_status_enum SFH_secure_channel_setup(uint8_t  channel, uint32_t options, sec_ch_callback_type callback_fx, char **cert_files,  char *status);

#endif
#endif
