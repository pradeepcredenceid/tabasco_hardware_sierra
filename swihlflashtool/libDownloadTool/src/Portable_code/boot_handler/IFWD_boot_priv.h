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
 *   <hardcoded file description - the only part to be modified manually!>
 *
 * Revision Information:
 *  $File name:  /msw_tools/FlashTool/libDownloadTool/src/Portable_code/boot_handler/IFWD_boot_priv.h $
 *  $CC-Version: .../oint_tools_flashtoole2_01/19 $
 *  $Date:       2014-03-13    15:41:33 UTC $
 *   Responsible: kofod, hilmar
 *   Comment:
 *     SMS01246967 merging XMM1100 stuff in order to compile
 */

#ifndef __IFWD_BOOT_PRIV_H__
#define __IFWD_BOOT_PRIV_H__

#include "IFWD_boot.h"
#include "IFWD_timer.h"

/*****************************************************************************
 *                            CONSTANT DEFINITIONS                           *
 *****************************************************************************/
#define BOOT_VERSION                      "1.0"

#define BOOT_MANAGER_ID                   0xFF
#define MAX_NUMBER_OF_CHANNELS            32
#define MAX_NUMBER_OF_INSTANCES           MAX_NUMBER_OF_CHANNELS + 1
#define MAX_BOOT_BUFFER_LENGTH              256
#define TRANSMIT_BOOT_BUFFER_LENGTH         256
#define MAX_EGOLD_BOOT_BUFFER_LENGTH        1
#define MAX_SGOLD_BOOT_BUFFER_LENGTH        2
#define MAX_SGOLD_USB_BOOT_BUFFER_LENGTH   4

#define TRANSMIT_SGOLD_BOOT_BUFFER_LENGTH   2
#define SGOLD_IND_PATTERN_LENGTH            1

/* SGOLD ID definitions */
#define EGOLDLITE_CHIPSET_ID         0x06
#define AT_CODE                      "AT"
#define BOOT_RAMLOAD_IND             0x30
#define CHECKSUM_OK                  0xA5
#define CHECKSUM_ERROR               0x5A
#define RX_BOOT_ACK_SGOLD_V2_V3         3
#define RX_BOOT_ACK_SGOLD_V1            1


/* EBL transfer Timeout duration  for SGOLD and EGOLD */
#define BOOT_TIME_EBL_TRANSFER      30000 //milliseconds

/* SGOLD ID definitions */
#define SGOLD2_CHIPSET_ID            0x1B
#define SGOLD3_CHIPSET_ID            0x1C
#define PSI_SIZE                     0x4000

#define SEND_NEXT_BOOT_CODE_TIME        20 /* Time given msec */


/*****************************************************************************
 *                                ENUMERATIONS                               *
 *****************************************************************************/
typedef enum boot_chip_id_s
{
  boot_chip_egold = 0,
  boot_chip_egoldlite,
  boot_chip_egoldvoice,
  boot_chip_egoldradio,
  boot_chip_sgold_v1,
  boot_chip_sgoldlite,
  boot_chip_sgold_v2,
  boot_chip_sgold_v3,
  boot_chip_id_not_valid = 0xFFFF
} boot_chip_id_t;

typedef enum boot_manager_state_s
{
  boot_manager_idle = 0,
  boot_manager_instance_running,
  boot_manager_delete_channel,
  boot_manager_state_not_valid = 0xFFFF
} boot_manager_state_t;

typedef enum boot_state_s
{
  boot_state_init = 0,
  boot_state_idle,
  boot_state_verify_comm,
  boot_state_verify_boot_ack,
  boot_state_verify_header,
  boot_state_boot_process,
  boot_state_rx,
  boot_state_rx_boot_ack,
  boot_state_rx_boot_usb_ack,
  boot_state_rx_waiting_boot_ack,    //< 9
  boot_state_rx_waiting_boot_ack_2,  //< 10
  boot_state_rx_in_progress,         //< 11
  boot_state_rx_completed,
  boot_state_rx_timeout,
  boot_state_rx_waiting_ebl_loader_chksum,
  boot_state_tx_ebl_loader_delay,
  boot_state_tx_ebl_loader_length,
  boot_state_tx,
  boot_state_tx_start,
  boot_state_tx_boot_code,
  boot_state_tx_ebl_loader,
  boot_state_tx_ebl,            //< 21
  boot_state_tx_in_progress,
  boot_state_tx_completed,
  boot_state_tx_timeout,
  boot_state_tx_waiting_boot_ack,
  boot_state_tx_ebl_loader_payload,
  boot_state_tx_ebl_loader_checksum,
  boot_state_rx_ebl_loader_checksum,
  boot_state_rx_waiting_ebl_chksum,
  boot_state_rx_ebl_version,
  boot_state_tx_ebl_payload,    //< 31
  boot_state_tx_waiting_ebl_chksum,
  boot_state_tx_ebl_checksum,
  boot_state_rx_ebl_checksum,
  boot_state_rx_ebl_loader_ready, //< 35
  boot_state_ebl_running,
  boot_state_tx_ind_byte,
  boot_state_rx_psi_error,
  boot_state_rx_ebl_loader_payload_ind,
  boot_state_tx_ebl_length,
  boot_state_boot_timeout,
  boot_state_error,
  boot_state_baud_set_1,
  boot_state_baud_set_2,
  boot_state_rx_2nd_boot_response,  //< 45
  boot_state_rx_tlv1,
  boot_state_rx_tlv2,
  boot_state_rpsi_idle,
  boot_state_rpsi_cmd_new_baud_tx,
  boot_state_rpsi_cmd_new_baud_rx, //< 50
  boot_state_rpsi_cmd_send_ebl,
  boot_state_rpsi_set_baud,
  boot_state_rpsi_set_baud_delay,
  boot_state_dut_usb_test,
  boot_state_dut_prep_usb_test_out,
  boot_state_dut_start_usb_test_out,
  boot_state_dut_usb_test_out,
  boot_state_dut_prep_usb_test_in,
  boot_state_dut_start_usb_test_in,
  boot_state_dut_usb_test_in, //< 60
  boot_state_dut_prep_usb_test_loopback,
  boot_state_dut_start_usb_test_loopback,
  boot_state_dut_usb_test_loopback,
  boot_state_dut_start_usb_chapter_9_test,
  boot_state_dut_usb_test_chapter_9_test,
  boot_state_dut_stop_usb_test,
  boot_state_dut_usb_send_zero_pack,
  boot_state_dut_usb_prep_rpsi_rsp,
  boot_state_dut_usb_send_rpsi_cmd,
  boot_state_dut_usb_test_idle, //< 70
  boot_state_dut_in_test_mode,
  boot_state_ping_tx,
  boot_state_ping_rx,
  boot_state_ping_rx_wait,
  boot_state_ping_done,
  boot_state_rom_baud_tx,
  boot_state_rom_baud_rx,
  boot_state_rom_baud_rx_wait,
  boot_state_rom_baud_delay,
  boot_state_rom_baud_done, //< 80
  boot_state_pre_boot_ok,
  boot_state_act_on_chip_response,
  boot_state_rom_skip_psi_load,
  boot_state_rom_skip_psi_rx,
  boot_state_get_psi_enh_ack,
  boot_state_psi_validate_sys,
  boot_state_get_psi_val_sys_rsp,
  boot_state_psi_start_sys,
  boot_state_rpsi_cmd_new_comm_tx,
  boot_state_rpsi_cmd_new_comm_rx, //< 90
  boot_state_rpsi_set_comm,
  boot_state_rpsi_set_comm_delay,
  boot_state_send_ebl_exec,
  boot_state_send_ebl_exec_delay,
  boot_state_send_ebl_exec_wait,

  boot_state_not_valid_state = 0xFFFF
} boot_state_t;

typedef enum boot_channel_state_s
{
  boot_channel_state_enabled = 0,
  boot_channel_state_disabled,
  boot_channel_state_port_ready,
  boot_channel_state_error,
  boot_channel_state_not_valid = 0xFFFF
} boot_channel_state_t;

typedef enum boot_debug_s
{
  db_BOOT_init,
  db_BOOT_close,
  db_BOOT_add_channel,
  db_BOOT_add_channel_result,
  db_BOOT_delete_channel,
  db_BOOT_abort_process,
  db_BOOT_start_boot,
  db_BOOT_get_status,
  db_BOOT_clear_status,
  db_BOOT_get_last_error,
  db_BOOT_clear_last_error,
  db_BOOT_egold,
  db_BOOT_egold_result,
  db_BOOT_tx_boot_code_not,
  db_BOOT_tx_ebl_loader,
  db_BOOT_rx_boot_ack_code,
  db_BOOT_rx_boot_ack_not,
  db_BOOT_tx_boot_code,
  db_BOOT_tx_ebl_loader_not,
  db_BOOT_rx_ebl_loader_checksum_not,
  db_BOOT_rx_ebl_loader_checksum_not_no_instance,
  db_BOOT_rx_ebl_loader_checksum_not_channel_invalid,
  db_BOOT_tx_ebl,
  db_BOOT_tx_ebl_payload,
  db_BOOT_tx_ebl_checksum,
  db_BOOT_rx_ebl_checksum,
  db_BOOT_rx_ebl_version,
  db_BOOT_rx_ebl_version_not,
  db_BOOT_rx_ebl_version_not_no_instance,
  db_BOOT_rx_ebl_version_not_invalid_channel,
  db_BOOT_rx_ebl_checksum_not,
  db_BOOT_rx_ebl_checksum_not_no_instance,
  db_BOOT_rx_ebl_checksum_not_invalid_channel,
  db_BOOT_tx_ebl_loader_payload,
  db_BOOT_tx_ebl_loader_payload_not,
  db_BOOT_tx_ebl_loader_payload_not_no_instance,
  db_BOOT_tx_ebl_loader_payload_not_invalid_channel,
  db_BOOT_tx_ebl_loader_checksum,
  db_BOOT_rx_ebl_loader_checksum,
  db_BOOT_tx_ebl_loader_checksum_not,
  db_BOOT_tx_ebl_length_not,
  db_BOOT_tx_ebl_payload_not,
  db_BOOT_tx_ebl_checksum_not,
  db_BOOT_tx_ebl_loader_length_not,
  db_BOOT_tx_ebl_loader_length,
  db_BOOT_process_ack_boot_code,
  db_BOOT_tx_ebl_loader_ind_not,
  db_BOOT_rx_ebl_loader_length_not,
  db_id_dummy,
  db_id_no_instance,
  db_id_invalid_channel,
  db_BOOT_sgold,
  db_BOOT_rx_ebl_payload_ind_not,
  db_BOOT_rx_ebl_length_not,
  db_BOOT_rx_ebl_ready_ind_not,
  db_entry_not_valid = 0xFFFF
} boot_debug_t;


typedef enum rpsi_cmd_code_s
{
  rpsi_cmd_code_nop,
  rpsi_cmd_code_set_baud,
  rpsi_cmd_code_tx_ebl,
  rpsi_cmd_usb_out_test,
  rpsi_cmd_usb_in_test,
  rpsi_cmd_usb_loopback_test,
  rpsi_cmd_usb_chapter_9_test,
  rpsi_cmd_validate_sys_sw,
  rpsi_cmd_code_new_comm,
  rpsi_cmd_dummy = 0xFFFF
} rpsi_cmd_code_t;

typedef enum rpsi_rsp_code_s
{
  rpsi_rsp_ok = 0,
  rpsi_rsp_fail,
  rpsi_rsp_cmd_unknown,
  rpsi_rsp_cmd_crc_err,
  rpsi_rsp_sys_sw_not_valid,
  rpsi_rsp_dummy = 0xFFFF
} rpsi_rsp_code_t;

typedef enum usb_test_case_s
{
  usb_test_out,
  usb_test_in,
  usb_test_loopback
} usb_test_case_t;


/*****************************************************************************
 *                        STRUCTURES DEFINITIONS                             *
 *****************************************************************************/

typedef union
{
  uint16_t word[2];
  uint32_t dword;
} rpsi_param_u;

typedef struct rpsi_cmd_type_s
{
  rpsi_param_u param;
  uint16_t          cmd;
  uint16_t          crc;
} rpsi_cmd_type;

typedef struct rpsi_rsp_type_s
{
  rpsi_param_u param;
  uint16_t          rsp;
  uint16_t          crc;
} rpsi_rsp_type;


typedef struct boot_buffer_s
{
  uint8_t   *buffer;
  uint32_t   max_length;
  uint32_t   length;
  uint8_t    checksum;
  uint8_t    boot_buffer_allocated;
} boot_buffer_t;

typedef struct psi_buffer_s
{
  uint16_t   max_length;
  uint8_t    *buffer;
  uint8_t    checksum;
} psi_buffer_t;


typedef struct boot_tx_buffer_s
{
  uint8_t   *buffer;
  uint32_t   max_length;
  uint32_t   length;
  uint32_t   send_zero_pack;
} boot_tx_buffer_t;

typedef struct boot_rx_buffer_s
{
  uint8_t    buffer[MAX_BOOT_BUFFER_LENGTH];
  uint32_t   max_length;
  uint32_t   length;
} boot_rx_buffer_t;

typedef struct boot_debug_info_s
{
  uint32_t  nr_of_abort_call;
  uint32_t  nr_of_overflow_bytes_tx;
  uint32_t  bytes_tx;
  uint32_t  nr_of_overflow_bytes_rx;
  uint32_t  bytes_rx;
  uint32_t  idle_time;
  uint32_t  busy_time;
  uint32_t  total_time;
  timer_chronom_t  chronom_idle;
  timer_chronom_t  chronom_busy;
  timer_chronom_t  chronom_tx;
  timer_chronom_t  chronom_rx;
  uint32_t  nr_of_mem_malloc_calls;
  uint32_t  nr_of_mem_free_calls;
} boot_debug_info_t;

/**
 * Doing "Structure Padding" for packing the uint32_t
 */
#ifdef __GNUC__
  #define PACKED __attribute__((packed))
#else
  #define PACKED
  #pragma option push -a1     ///< Packing the uint32_t up against the uint8_t's
#endif
typedef struct boot_link_est_response_s
{
  uint8_t size;
  uint8_t chip_id;
  uint8_t bc_ver;
  uint8_t bo_cfg;
  uint8_t sec_lvl;
  uint8_t fl_stat;
  uint8_t cus_cfg;
  uint8_t num_x;
  uint8_t num_y;

  uint32_t snum_1 PACKED;
  uint32_t snum_2 PACKED;
  uint32_t snum_3 PACKED;
  uint8_t  v1_boot_code_ver;
  uint8_t  v1_es_ver;
  uint8_t  v1_sub_ver;
  uint8_t  v1_patch_img_ver;
  uint8_t  v1_spare_usif_config;
  uint8_t  v1_spare;
} boot_link_est_response_t;
#ifndef __GNUC__
  #pragma option pop
#endif

typedef struct boot_instance_s
{
  uint8_t                     channel_id;
  boot_dev_cfg_t             *p_dev_cfg;
  boot_chip_id_t              chip_id;
  boot_state_t                state;
  boot_state_t                last_state;
  uint8_t                     buffer[MAX_BOOT_BUFFER_LENGTH];
  boot_buffer_t               boot_buffer;
  uint8_t                     boot_buffer_allocated;
  psi_buffer_t                psi_buffer;
  boot_rx_buffer_t            rx_buffer;
  boot_buffer_t               rx_test_buffer;
  boot_tx_buffer_t            tx_buffer;
  boot_ebl_version_t         *p_ebl_version;
  timer_event_timer_t         timer;
  timer_event_timer_t         boot_timer;
  timer_event_timer_t         tx_timer;
  timer_event_timer_t         rx_timer;
  timer_event_timer_t         tx_boot_code_timer;
  uint32_t                    boot_time;
  uint32_t                    tx_time;
  uint32_t                    rx_time;
  uint32_t                    tx_boot_code_time;
  timer_chronom_t             chronom;
  boot_status_t               status;
  boot_error_t                error;
  boot_status_t               last_status;
  boot_error_t                last_error;
  boot_fx_t                   fx;
  uint8_t                     nof_ebl_length_bytes;
  uint8_t                     chip_return_code;
  uint8_t                     link_est_struct_ver;
  uint32_t                    EBL_load_baudrate;
  timer_event_timer_t         baud_set_delay_timer;
  uint8_t                     hack_hack_tlv_buffer[65536];
#ifdef DEBUG_BOOT_MODULE
  boot_debug_info_t           debug_info;
#endif
  uint32_t                    waiting_for_chip_id;
  uint8_t                     waiting_for_tlv_response;
  boot_link_est_response_t    link_est_response;
  rpsi_cmd_type               rpsi_cmd;
  rpsi_rsp_type               rpsi_rsp;
  uint32_t                    sync_AT_count;
  uint32_t                    current_dual_boot_speed;
  boot_state_t                usb_state;
  usb_test_case_t             usb_test_case;
  uint8_t                     usb_test_buffer_random_size;
  uint32_t                    buffer_test_size;
  uint32_t                    use_test_buffer1;
  uint32_t                    use_test_buffer2;
  uint32_t                    total_bytes_sent;
  uint32_t                    total_bytes_recv;
  timer_event_timer_t         usb_out_timer;
  timer_chronom_t             usb_out_time_run;
  timer_event_timer_t         usb_in_timer;
  timer_chronom_t             usb_in_time_run;
  uint8_t                     dut_in_test_mode;
  uint32_t                    rx_read_ahead_bytes;
  uint32_t                    last_timestamp;
} boot_instance_t;


typedef struct boot_manager_error_s
{
  uint8_t              channel_id;
  boot_error_t         code;
} boot_manager_error_t;

typedef struct boot_manager_status_s
{
  uint8_t              channel_id;
  boot_status_t        code;
} boot_manager_status_t;

typedef struct boot_manager_s
{
  uint8_t                manager_id;
  uint8_t                nr_of_instances_running;
  boot_manager_state_t   state;
  boot_instance_t       *boot_handler[MAX_NUMBER_OF_INSTANCES];
  boot_manager_status_t  status;
  boot_manager_error_t   error;
} boot_manager_t;

/*****************************************************************************
 *                        PROTOTYPES DEFINITIONS                             *
 *****************************************************************************/
typedef void (*boot_handler_t) (boot_instance_t *p_boot_ins);


/*****************************************************************************
 *                         VARIABLES TO EXPORT                               *
 *****************************************************************************/
extern boot_manager_t  boot_manager;
extern boot_manager_t *p_boot_manager;

/*****************************************************************************
 *                         PROTOTYPES TO EXPORT                              *
 *****************************************************************************/
extern boot_status_t BOOT_sgold(boot_instance_t *p_boot_ins, boot_manager_t *p_boot_mgn);
extern void BOOT_sgold_handler(boot_instance_t *p_boot_ins);
extern boot_status_t BOOT_egold(boot_instance_t *p_boot_ins, boot_manager_t *p_boot_mgn);
extern void BOOT_egold_handler(boot_instance_t *p_boot_ins);

#if defined (TEXT_CLASS_IFWD_ERRORS)
  extern void BOOT_Err(uint8_t channel, uint32_t err);
#endif

#if 0
  extern void BOOT_sgold_handler(boot_instance_t *p_boot_ins);
#endif

#endif
