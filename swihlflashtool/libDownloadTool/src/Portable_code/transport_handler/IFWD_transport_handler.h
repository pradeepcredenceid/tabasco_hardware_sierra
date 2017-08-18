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
 *
 * Revision Information:
 *  $File name:  /msw_tools/FlashTool/libDownloadTool/src/Portable_code/transport_handler/IFWD_transport_handler.h $
 *  $CC-Version: .../oint_tools_flashtoole2_01/6 $
 *  $Date:       2014-03-13    15:40:22 UTC $
 *   Responsible: bentzen
 *   Comment:
 *     SMS00847521: "Need to remove Lint warnings..."
 */


/* open / close give channel numer...otherwise */

#if defined (TEXT_CLASS_IFWD_ERRORS)
TEXT_DEF(TRA_ERR_channel_is_outside_range,  "Channel number %d is outside the valid range.")             /* 01 */
TEXT_DEF(TRA_ERR_channel_already_rx,        "Channel is already setup for receiving.")                   /* 02 */
TEXT_DEF(TRA_ERR_channel_already_opened,    "Channel %d cannot be opened twice.")                        /* 03 */
TEXT_DEF(TRA_ERR_channel_busy,              "Channel is busy - send rejected.")                          /* 04 */
TEXT_DEF(TRA_ERR_channel_does_not_exist,    "Cannot delete the non-existing channel %d.")                /* 05 */
TEXT_DEF(TRA_ERR_channel_close_busy,        "Channel is in use, so it cannot be closed.")                /* 06 */
TEXT_DEF(TRA_ERR_cannot_write_to_file,      "Cannot write to file: \"%s\".")                             /* 07 */
TEXT_DEF(TRA_ERR_cannot_tx_file_with_rx,    "Send from file while receiving large data not allowed")     /* 08 */
TEXT_DEF(TRA_ERR_cmd_too_large,             "Command too large.")                                        /* 09 */
TEXT_DEF(TRA_ERR_malloc,                    "Could not allocate memory.")                                /* 10 */
TEXT_DEF(TRA_ERR_free,                      "Could not free memory.")                                    /* 11 */
TEXT_DEF(TRA_ERR_file_not_found,            "File %s not found.")                                        /* 12 */
TEXT_DEF(TRA_ERR_file_seek_error,           "Seek error in file %s.")                                    /* 13 */
TEXT_DEF(TRA_ERR_file_error,                "File Error: %s")                                            /* 14 */
TEXT_DEF(TRA_ERR_file_read_error,           "File Read Error: %s")                                       /* 15 */
TEXT_DEF(TRA_ERR_spare_16,                  "")                                                          /* 16 */
TEXT_DEF(TRA_ERR_spare_17,                  "")                                                          /* 17 */
TEXT_DEF(TRA_ERR_spare_18,                  "")                                                          /* 18 */
TEXT_DEF(TRA_ERR_spare_19,                  "")                                                          /* 19 */
TEXT_DEF(TRA_ERR_spare_20,                  "")                                                          /* 20 */
TEXT_DEF(TRA_ERR_spare_21,                  "")                                                          /* 21 */
TEXT_DEF(TRA_ERR_spare_22,                  "")                                                          /* 22 */
TEXT_DEF(TRA_ERR_spare_23,                  "")                                                          /* 23 */
TEXT_DEF(TRA_ERR_spare_24,                  "")                                                          /* 24 */
TEXT_DEF(TRA_ERR_spare_25,                  "")                                                          /* 25 */
TEXT_DEF(TRA_ERR_spare_26,                  "")                                                          /* 26 */
TEXT_DEF(TRA_ERR_spare_27,                  "")                                                          /* 27 */
TEXT_DEF(TRA_ERR_spare_28,                  "")                                                          /* 28 */
TEXT_DEF(TRA_ERR_spare_29,                  "")                                                          /* 29 */
TEXT_DEF(TRA_ERR_spare_30,                  "")                                                          /* 30 */
TEXT_DEF(TRA_ERR_spare_31,                  "")                                                          /* 31 */
TEXT_DEF(TRA_ERR_spare_32,                  "")                                                          /* 32 */
TEXT_DEF(TRA_ERR_spare_33,                  "")                                                          /* 33 */
TEXT_DEF(TRA_ERR_spare_34,                  "")                                                          /* 34 */
TEXT_DEF(TRA_ERR_spare_35,                  "")                                                          /* 35 */
TEXT_DEF(TRA_ERR_spare_36,                  "")                                                          /* 36 */
TEXT_DEF(TRA_ERR_spare_37,                  "")                                                          /* 37 */
TEXT_DEF(TRA_ERR_spare_38,                  "")                                                          /* 38 */
TEXT_DEF(TRA_ERR_spare_39,                  "")                                                          /* 39 */
TEXT_DEF(TRA_ERR_spare_40,                  "")                                                          /* 40 */
TEXT_DEF(TRA_ERR_spare_41,                  "")                                                          /* 41 */
TEXT_DEF(TRA_ERR_spare_42,                  "")                                                          /* 42 */
TEXT_DEF(TRA_ERR_spare_43,                  "")                                                          /* 43 */
TEXT_DEF(TRA_ERR_spare_44,                  "")                                                          /* 44 */
TEXT_DEF(TRA_ERR_spare_45,                  "")                                                          /* 45 */
TEXT_DEF(TRA_ERR_spare_46,                  "")                                                          /* 46 */
TEXT_DEF(TRA_ERR_spare_47,                  "")                                                          /* 47 */
TEXT_DEF(TRA_ERR_spare_48,                  "")                                                          /* 48 */
TEXT_DEF(TRA_ERR_spare_49,                  "")                                                          /* 49 */
TEXT_DEF(TRA_ERR_spare_50,                  "")                                                          /* 50 */

#elif defined (TEXT_CLASS_IFWD_INFO)
TEXT_DEF(TRA_INFO_channel_opened,  "Channel number %d is succesfully opened.")                           /* 01 */
TEXT_DEF(TRA_INFO_channel_closed,  "Channel number %d is succesfully closed.")                           /* 02 */
TEXT_DEF(TRA_INFO_spare_03,        "")                                                                   /* 03 */
TEXT_DEF(TRA_INFO_spare_04,        "")                                                                   /* 04 */
TEXT_DEF(TRA_INFO_spare_05,        "")                                                                   /* 05 */
TEXT_DEF(TRA_INFO_spare_06,        "")                                                                   /* 06 */
TEXT_DEF(TRA_INFO_spare_07,        "")                                                                   /* 07 */
TEXT_DEF(TRA_INFO_spare_08,        "")                                                                   /* 08 */
TEXT_DEF(TRA_INFO_spare_09,        "")                                                                   /* 09 */
TEXT_DEF(TRA_INFO_spare_10,        "")                                                                   /* 10 */
TEXT_DEF(TRA_INFO_spare_11,        "")                                                                   /* 11 */
TEXT_DEF(TRA_INFO_spare_12,        "")                                                                   /* 12 */
TEXT_DEF(TRA_INFO_spare_13,        "")                                                                   /* 13 */
TEXT_DEF(TRA_INFO_spare_14,        "")                                                                   /* 14 */
TEXT_DEF(TRA_INFO_spare_15,        "")                                                                   /* 15 */
TEXT_DEF(TRA_INFO_spare_16,        "")                                                                   /* 16 */
TEXT_DEF(TRA_INFO_spare_17,        "")                                                                   /* 17 */
TEXT_DEF(TRA_INFO_spare_18,        "")                                                                   /* 18 */
TEXT_DEF(TRA_INFO_spare_19,        "")                                                                   /* 19 */
TEXT_DEF(TRA_INFO_spare_20,        "")                                                                   /* 20 */
TEXT_DEF(TRA_INFO_spare_21,        "")                                                                   /* 21 */
TEXT_DEF(TRA_INFO_spare_22,        "")                                                                   /* 22 */
TEXT_DEF(TRA_INFO_spare_23,        "")                                                                   /* 23 */
TEXT_DEF(TRA_INFO_spare_24,        "")                                                                   /* 24 */
TEXT_DEF(TRA_INFO_spare_25,        "")                                                                   /* 25 */
TEXT_DEF(TRA_INFO_spare_26,        "")                                                                   /* 26 */
TEXT_DEF(TRA_INFO_spare_27,        "")                                                                   /* 27 */
TEXT_DEF(TRA_INFO_spare_28,        "")                                                                   /* 28 */
TEXT_DEF(TRA_INFO_spare_29,        "")                                                                   /* 29 */
TEXT_DEF(TRA_INFO_spare_30,        "")                                                                   /* 30 */
TEXT_DEF(TRA_INFO_spare_31,        "")                                                                   /* 31 */
TEXT_DEF(TRA_INFO_spare_32,        "")                                                                   /* 32 */
TEXT_DEF(TRA_INFO_spare_33,        "")                                                                   /* 33 */
TEXT_DEF(TRA_INFO_spare_34,        "")                                                                   /* 34 */
TEXT_DEF(TRA_INFO_spare_35,        "")                                                                   /* 35 */
TEXT_DEF(TRA_INFO_spare_36,        "")                                                                   /* 36 */
TEXT_DEF(TRA_INFO_spare_37,        "")                                                                   /* 37 */
TEXT_DEF(TRA_INFO_spare_38,        "")                                                                   /* 38 */
TEXT_DEF(TRA_INFO_spare_39,        "")                                                                   /* 39 */
TEXT_DEF(TRA_INFO_spare_40,        "")                                                                   /* 40 */
TEXT_DEF(TRA_INFO_spare_41,        "")                                                                   /* 41 */
TEXT_DEF(TRA_INFO_spare_42,        "")                                                                   /* 42 */
TEXT_DEF(TRA_INFO_spare_43,        "")                                                                   /* 43 */
TEXT_DEF(TRA_INFO_spare_44,        "")                                                                   /* 44 */
TEXT_DEF(TRA_INFO_spare_45,        "")                                                                   /* 45 */
TEXT_DEF(TRA_INFO_spare_46,        "")                                                                   /* 46 */
TEXT_DEF(TRA_INFO_spare_47,        "")                                                                   /* 47 */
TEXT_DEF(TRA_INFO_spare_48,        "")                                                                   /* 48 */
TEXT_DEF(TRA_INFO_spare_49,        "")                                                                   /* 49 */
TEXT_DEF(TRA_INFO_spare_50,        "")                                                                   /* 50 */

#else

#ifndef _INC_TRA
#define _INC_TRA

/*---------------------------------------------*/
/* Enumeration                                 */
/*---------------------------------------------*/

enum
{
  TRA_db_callback_rx,
  TRA_db_callback_tx,
  TRA_db_add_channel,
  TRA_db_delete_channel,
  TRA_db_abort,
  TRA_db_send_file,
  TRA_db_send_memory,
  TRA_db_receive_file,
  TRA_db_receive_memory,
  TRA_db_sending_zeros_from_file
};

typedef enum
{

  TRA_status_ok,
  TRA_status_fail
} TRA_status_enum;

typedef enum
{
  TRA_result_OK = 0,
  TRA_result_Error
} TRA_result_enum;

typedef enum
{
  TRA_progress_report_disabled = 0,
  TRA_progress_report_enabled
} TRA_progress_report_enum;

/*---------------------------------------------*/
/* Structures                                  */
/*---------------------------------------------*/
typedef void (*TRA_callback)( uint8_t channel, TRA_status_enum status, char *status_text);

/*---------------------------------------------*/
/* External defines                            */
/*---------------------------------------------*/
#define TRA_COMPRESSION_HEADER_BYTES 4

/*---------------------------------------------*/
/* External declarations                       */
/*---------------------------------------------*/

/*---------------------------------------------*/
/* External functions                          */
/*---------------------------------------------*/
extern void TRA_clock(void);
extern void TRA_init(void);
extern void TRA_close(void);

extern uint8_t TRA_add_channel(uint8_t channel, char *status);
extern uint8_t TRA_delete_channel(uint8_t channel, char *status);

extern TRA_status_enum TRA_get_status(uint8_t channel, char *status_text);
extern void TRA_abort_process(uint8_t channel);

extern TRA_status_enum TRA_send_file_data(uint8_t channel,
                                          uint16_t package_code,
                                          char *file_name,
                                          uint32_t data_offset,
                                          uint32_t data_length,
                                          TRA_progress_report_enum report_progress,
                                          TRA_callback status_callback);

extern TRA_status_enum TRA_send_mem_data(uint8_t channel,
                                         uint16_t package_code,
                                         uint8_t *data_pointer,
                                         uint32_t data_length,
                                         TRA_progress_report_enum report_progress,
                                         TRA_callback status_callback);

extern TRA_status_enum TRA_receive_file_data(uint8_t channel,
                                             uint16_t package_code,
                                             char *file_name,
                                             uint32_t data_offset,
                                             uint32_t data_max_length,
                                             TRA_progress_report_enum report_progress,
                                             TRA_callback status_callback);

extern TRA_status_enum TRA_receive_mem_data(uint8_t channel,
                                            uint16_t package_code,
                                            uint8_t *data_pointer,
                                            uint32_t data_max_length,
                                            TRA_progress_report_enum report_progress,
                                            TRA_callback status_callback);

extern TRA_status_enum TRA_receive_mem_data_no_tx(uint8_t channel,
                                                  uint16_t package_code,
                                                  uint8_t *data_pointer,
                                                  uint32_t data_max_length,
                                                  TRA_progress_report_enum report_progress,
                                                  TRA_callback status_callback);


extern uint16_t TRA_get_last_checksum(uint8_t channel);
extern void TRA_clear_checksum(uint8_t channel);
extern void TRA_CalcChecksum(uint8_t channel, uint8_t calc);
extern void TRA_CapabilitiesChanged(uint8_t channel);
extern uint32_t TRA_GetCommTimeOut(uint8_t channel);

#endif
#endif
