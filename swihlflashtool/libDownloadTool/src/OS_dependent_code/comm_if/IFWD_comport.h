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
 *  $File name:  /msw_tools/FlashTool/libDownloadTool/src/OS_dependent_code/comm_if/IFWD_comport.h $
 *  $CC-Version: .../dhkristx_140314_v0/1 $
 *  $Date:       2014-04-23    11:01:37 UTC $
 *   Responsible: bentzen
 *   Comment:
 *     SMS00897775
 */

#ifndef __COMPORT_H__
#define __COMPORT_H__

#include "OS.h"

#ifndef WIN32
  #include <stdarg.h>
#endif

#ifndef COMM_HANDLE
  #ifdef WIN32
    #define COMM_HANDLE HANDLE
  #else
    #define COMM_HANDLE int
  #endif
#endif

/*****************************************************************************
 *                          CONSTANT DEFINITIONS                             *
 *****************************************************************************/
#define MAX_BUFFER_SIZE   32

/*****************************************************************************
 *                       ENUMERATION DEFINITIONS                             *
 *****************************************************************************/
 typedef enum comport_stop_bits_s
 {
   comport_one_stop_bit = 0,
   comport_one_5_stop_bit,
   comport_two_stop_bits,
   comport_invalid_stop_bits
 } comport_stop_bits_t;

 typedef enum comport_parity_s
 {
   comport_even_parity = 0,
   comport_mark_parity,
   comport_no_parity,
   comport_odd_parity,
   comport_space_parity,
   comport_invalid_parity
 } comport_parity_t;

 typedef enum comport_baudrate_s
 {
   comport_baud_57600 = 0,
   comport_baud_115200,
   comport_baud_230400,
   comport_baud_460800,
   comport_baud_921600,
   comport_baud_not_valid
 } comport_baudrate_t;

 typedef enum comport_rts_control_s
 {
   comport_rts_control_disable,
   comport_rts_control_enable,
   comport_rts_control_handshake,
   comport_rts_control_toggle
 } comport_rts_control_t;

 typedef enum comport_dtr_control_s
 {
   comport_dtr_control_disable,
   comport_dtr_control_enable,
   comport_dtr_control_handshake
 } comport_dtr_control_t;

 typedef enum comport_errors_s
 {
   comport_ce_break,    /* The HW detected a break condition                    */
   comport_ce_frame,    /* The HW detected a frame error                        */
   comport_ce_ioe,      /* An I/O error occurred during communications with the */
                     /* device .                                             */
   comport_ce_mode,     /* The requested mode is not supported, or the hFile    */
                     /* parameter is invalid. If this value is specified, it */
                     /* is the only valid error.                             */
   comport_ce_overrun,  /* A character-buffer overrun has occurred. The next    */
                     /* character is lost.                                   */
   comport_ce_rxover,   /* An input buffer overflow has occurred. There is      */
                     /* either no room in the input buffer, or a character   */
                     /* was received after the end-of-file (EOF) character.  */
   comport_ce_rxparity, /* The hardware detected a parity error.                */
   comport_ce_txfull,   /* The application tried to transmit a character, but   */
                     /* the output buffer was full.                          */
   comport_error_invalid_handle,
   comport_error_access_denied,
   comport_error_device_not_ready,
   comport_error_already_assigned,
   comport_error_open_failed,
   comport_error_busy,
   comport_error_already_exist,
   comport_error_io_device,
   comport_error_serial_no_device,
   comport_error_more_writes,
   comport_error_counter_timeout,
   comport_error_no_more_devices,
   comport_error_flow_control_not_supported,
   comport_error_baudrate_not_supported,
   comport_error_no_error,
   comport_error_unkown = 0xFFFF
 } comport_error_t;

 typedef enum comport_status_s
 {
   comport_status_no_event = 0,
   comport_read_operation_finished,
   comport_read_operation_ongoing,
   comport_read_operation_finished_with_error,
   comport_write_operation_ongoing,
   comport_write_operation_finished_with_error,
   comport_write_operation_finished = 0xFFFF,
   comport_event_not_valid
 } comport_status_t;


 typedef struct comport_modem_status_line_s
 {
   uint8_t comport_ms_cts_on;
   uint8_t comport_ms_dsr_on;
   uint8_t comport_ms_ring_on;
   uint8_t comport_ms_rlsd_on;
   uint8_t comport_ms_rts_on;
   uint8_t comport_ms_dtr_on;
 } comport_modem_status_line_t;

/*****************************************************************************
 *                        PROTOTYPES DEFINITIONS                             *
 *****************************************************************************/

 typedef void (*comport_fx_tx_t) (uint8_t h_comm, uint32_t bytes_txed);

 typedef void (*comport_fx_rx_t) (uint8_t h_comm, uint32_t length);

 typedef uint8_t   (*comport_set_baud_t)(uint8_t h_comm, uint32_t baudrate);

 typedef uint8_t   (*comport_close_t)(uint8_t h_comm);

 typedef uint8_t   (*comport_get_modem_status_t)(uint8_t h_comm, comport_modem_status_line_t *modem_status_line);

 typedef uint8_t   (*comport_set_buffer_size_t)(uint8_t h_comm, uint32_t rx_buf_size, uint32_t tx_buf_size);

 typedef uint32_t  (*comport_get_rx_buffer_size_t)(uint8_t h_comm);

 typedef uint32_t  (*comport_get_rx_used_buffer_size_t)(uint8_t h_comm);

 typedef uint32_t  (*comport_get_rx_free_buffer_size_t)(uint8_t h_comm);

 typedef uint8_t   (*comport_rx_t)(uint8_t h_comm, uint8_t **block, uint32_t bytes_to_read, uint32_t *bytes_read, comport_fx_rx_t fx);

 typedef uint32_t  (*comport_get_tx_buffer_size_t)(uint8_t h_comm);

 typedef uint32_t  (*comport_get_tx_used_buffer_size_t)(uint8_t h_comm);

 typedef uint32_t  (*comport_get_tx_free_buffer_size_t)(uint8_t h_comm);

 typedef uint8_t   (*comport_tx_t)(uint8_t id, uint8_t *block, uint32_t bytes_to_send, uint32_t *bytes_send, comport_fx_tx_t fx);

 typedef uint8_t   (*comport_set_flowctrl_t)(uint8_t h_comm, comport_rts_control_t rts_control);

 typedef uint8_t   (*comport_get_flowctrl_t)(uint8_t h_comm, comport_rts_control_t *rts_control);

 typedef uint8_t   (*comport_set_rts_t)(uint8_t h_comm, uint8_t new_rts);

 typedef uint8_t   (*comport_set_dtr_t)(uint8_t h_comm, uint8_t new_dtr);

 typedef uint8_t   (*comport_flush_rx_buffer_t)(uint8_t h_comm);

 typedef uint8_t   (*comport_flush_tx_buffer_t)(uint8_t h_comm);

 typedef comport_error_t (*comport_get_last_error_t)(uint8_t h_comm);

 typedef void (*comport_set_last_error_t)(uint8_t h_comm);

 typedef comport_status_t (*comport_get_status_t)(uint8_t h_comm);

 typedef void (*comport_process_comm_t)(uint8_t h_comm);

 typedef void (*comport_get_version_t)(uint8_t h_comm, uint8_t *version);

 typedef void (*comm_error_report_t)(uint8_t id, uint32_t error, uint32_t line);
 /*****************************************************************************
 *                        STRUCTURES DEFINITIONS                             *
 *****************************************************************************/
 typedef struct buffer_s
 {
   uint8_t  data[MAX_BUFFER_SIZE];
   uint16_t rd_pos;
   uint16_t wr_pos;
   uint16_t size;
 } buffer_t;

 typedef struct comport_cfg_s
 {
   uint8_t               id;
   uint8_t              *comport_name;
   uint32_t              baudrate;
   uint16_t              databits;
   comport_stop_bits_t   stop_bits;
   comport_parity_t      parity;
   comport_rts_control_t rts_control;
   comport_dtr_control_t dtr_control;
 } comport_cfg_t;


 typedef struct comport_handler_s
 {
    uint8_t                           id;
    comport_set_baud_t                set_baud;
    comport_get_modem_status_t        get_modem_status;
    comport_close_t                   close;
    comport_set_buffer_size_t         set_buffer_size;
    comport_get_rx_used_buffer_size_t get_rx_used_buffer_size;
    comport_rx_t                      rx;
    comport_tx_t                      tx;
    comport_set_rts_t                 set_rts;
    comport_set_dtr_t                 set_dtr;
    comport_flush_rx_buffer_t         flush_rx_buffer;
    comport_flush_tx_buffer_t         flush_tx_buffer;
    comport_get_last_error_t          get_last_error;
    comport_get_status_t              get_status;
    comport_process_comm_t            process_comm;
    comport_fx_tx_t                   on_buffer_sent_manager;
    comport_fx_rx_t                   on_buffer_arrival_manager;
    comport_get_version_t             get_version;
 } comport_handler_t;

/*****************************************************************************
 *                               PUBLIC FUNCTIONS                            *
 *****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

  extern uint8_t COMPORT_init(void);
  extern comport_handler_t *COMPORT_open(uint8_t comm_channel_id,
                                         comport_cfg_t *comport_cfg,
                                         comm_error_report_t error_report);

  extern COMM_HANDLE COMPORT_get_handle_I(uint8_t id);
  //extern int COMPORT_ioctl_I(uint8_t id, unsigned int req, ...);

  extern uint8_t (*COMPORT_close)(void);

  extern uint8_t COMPORT_set_gp0(uint8_t channel, uint8_t value);

  // Prolific resetting port
  extern uint8_t COMPORT_init_prolific_board_reset(uint8_t channel, char *reset_port_name, uint32_t gpio_mask);
  extern uint8_t COMPORT_stop_prolific_board_reset(uint8_t channel);
  extern void COMPORT_prolific_port_reset_cb(uint8_t channel);

  // Debug function for dumping what is in the Rx buffer
  extern void COMPORT_rx_dump(uint8_t id);

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef _COMPORT_H_ */

