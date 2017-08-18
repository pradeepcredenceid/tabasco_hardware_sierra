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
 *  $File name:  /msw_tools/FlashTool/libDownloadTool/src/Portable_code/comm_handler/IFWD_comm.h $
 *  $CC-Version: .../oint_tools_flashtoole2_01/8 $
 *  $Date:       2014-03-13    15:41:18 UTC $
 *   Responsible: bentzen
 *   Comment:
 *     Fixed suppert for Quantum USB port names and Certificate upload
 */
#if defined (TEXT_CLASS_IFWD_ERRORS)
TEXT_DEF(COMM_ERR_instance_not_created,      "The channel instance has not been created")
TEXT_DEF(COMM_ERR_comport_not_open,          "The comport must be open before adding channel")
TEXT_DEF(COMM_ERR_instance_number_not_valid, "Instance number out of range")
TEXT_DEF(COMM_ERR_comport_not_ready,         "Communication comport must be ready before openning the control comport")
TEXT_DEF(COMM_ERR_baudrate_comport_not_ready,"Comport must be opened before setting the baudrate")
TEXT_DEF(COMM_ERR_send_buffer,               "Comport must be opened before a transmittion operation")
TEXT_DEF(COMM_ERR_tx_rx_buffer_not_config,   "TX and RX internal buffers must be configured before TX transaction")
TEXT_DEF(COMM_ERR_tx_rx_transaction_ongoing, "TX transaction aborted. Last RX transaction not completed")
TEXT_DEF(COMM_ERR_tx_tx_transaction_ongoing, "TX transaction aborted. Last TX transaction not completed")
TEXT_DEF(COMM_ERR_tx_transaction_error,      "COMM state machine in error state. TX transaction aborted")
TEXT_DEF(COMM_ERR_tx_transaction_error_unknown_state, "COMM state machine in unknown state. TX transaction aborted")
TEXT_DEF(COMM_ERR_rx_tx_buffer_not_config,   "TX and RX internal buffers must be configured before TX transaction")
TEXT_DEF(COMM_ERR_rx_rx_transaction_ongoing, "RX transaction aborted. Last RX transaction not completed")
TEXT_DEF(COMM_ERR_rx_tx_transaction_ongoing, "RX transaction aborted. Last TX transaction not completed")
TEXT_DEF(COMM_ERR_rx_transaction_error,      "COMM state machine in error state. RX transaction aborted")
#ifdef _WIN32
TEXT_DEF(COMM_ERR_comport_error,             "WIN32 Comport error %d. Line %d")
#else
TEXT_DEF(COMM_ERR_comport_error,             "Communication error %d. Line %d")
#endif
TEXT_DEF(COMM_ERR_close_failed,              "Comport close failure ")          /* 17 */
TEXT_DEF(COMM_ERR_spare_18,   "")          /* 18 */
TEXT_DEF(COMM_ERR_spare_19,   "")          /* 19 */
TEXT_DEF(COMM_ERR_spare_20,   "")          /* 20 */
TEXT_DEF(COMM_ERR_spare_21,   "")          /* 21 */
TEXT_DEF(COMM_ERR_spare_22,   "")          /* 22 */
TEXT_DEF(COMM_ERR_spare_23,   "")          /* 23 */
TEXT_DEF(COMM_ERR_spare_24,   "")          /* 24 */
TEXT_DEF(COMM_ERR_spare_25,   "")          /* 25 */
TEXT_DEF(COMM_ERR_spare_26,   "")          /* 26 */
TEXT_DEF(COMM_ERR_spare_27,   "")          /* 27 */
TEXT_DEF(COMM_ERR_spare_28,   "")          /* 28 */
TEXT_DEF(COMM_ERR_spare_29,   "")          /* 29 */
TEXT_DEF(COMM_ERR_spare_30,   "")          /* 30 */
TEXT_DEF(COMM_ERR_spare_31,   "")          /* 31 */
TEXT_DEF(COMM_ERR_spare_32,   "")          /* 32 */
TEXT_DEF(COMM_ERR_spare_33,   "")          /* 33 */
TEXT_DEF(COMM_ERR_spare_34,   "")          /* 34 */
TEXT_DEF(COMM_ERR_spare_35,   "")          /* 35 */
TEXT_DEF(COMM_ERR_spare_36,   "")          /* 36 */
TEXT_DEF(COMM_ERR_spare_37,   "")          /* 37 */
TEXT_DEF(COMM_ERR_spare_38,   "")          /* 38 */
TEXT_DEF(COMM_ERR_spare_39,   "")          /* 39 */
TEXT_DEF(COMM_ERR_spare_40,   "")          /* 40 */
TEXT_DEF(COMM_ERR_spare_41,   "")          /* 41 */
TEXT_DEF(COMM_ERR_spare_42,   "")          /* 42 */
TEXT_DEF(COMM_ERR_spare_43,   "")          /* 43 */
TEXT_DEF(COMM_ERR_spare_44,   "")          /* 44 */
TEXT_DEF(COMM_ERR_spare_45,   "")          /* 45 */
TEXT_DEF(COMM_ERR_spare_46,   "")          /* 46 */
TEXT_DEF(COMM_ERR_spare_47,   "")          /* 47 */
TEXT_DEF(COMM_ERR_spare_48,   "")          /* 48 */
TEXT_DEF(COMM_ERR_spare_49,   "")          /* 49 */
TEXT_DEF(COMM_ERR_spare_50,   "")          /* 50 */

#elif defined (TEXT_CLASS_IFWD_INFO)
TEXT_DEF(COMM_INFO_spare_01,  "")          /* 01 */
TEXT_DEF(COMM_INFO_spare_02,  "")          /* 02 */
TEXT_DEF(COMM_INFO_spare_03,  "")          /* 03 */
TEXT_DEF(COMM_INFO_spare_04,  "")          /* 04 */
TEXT_DEF(COMM_INFO_spare_05,  "")          /* 05 */
TEXT_DEF(COMM_INFO_spare_06,  "")          /* 06 */
TEXT_DEF(COMM_INFO_spare_07,  "")          /* 07 */
TEXT_DEF(COMM_INFO_spare_08,  "")          /* 08 */
TEXT_DEF(COMM_INFO_spare_09,  "")          /* 09 */
TEXT_DEF(COMM_INFO_spare_10,  "")          /* 10 */
TEXT_DEF(COMM_INFO_spare_11,  "")          /* 11 */
TEXT_DEF(COMM_INFO_spare_12,  "")          /* 12 */
TEXT_DEF(COMM_INFO_spare_13,  "")          /* 13 */
TEXT_DEF(COMM_INFO_spare_14,  "")          /* 14 */
TEXT_DEF(COMM_INFO_spare_15,  "")          /* 15 */
TEXT_DEF(COMM_INFO_spare_16,  "")          /* 16 */
TEXT_DEF(COMM_INFO_spare_17,  "")          /* 17 */
TEXT_DEF(COMM_INFO_spare_18,  "")          /* 18 */
TEXT_DEF(COMM_INFO_spare_19,  "")          /* 19 */
TEXT_DEF(COMM_INFO_spare_20,  "")          /* 20 */
TEXT_DEF(COMM_INFO_spare_21,  "")          /* 21 */
TEXT_DEF(COMM_INFO_spare_22,  "")          /* 22 */
TEXT_DEF(COMM_INFO_spare_23,  "")          /* 23 */
TEXT_DEF(COMM_INFO_spare_24,  "")          /* 24 */
TEXT_DEF(COMM_INFO_spare_25,  "")          /* 25 */
TEXT_DEF(COMM_INFO_spare_26,  "")          /* 26 */
TEXT_DEF(COMM_INFO_spare_27,  "")          /* 27 */
TEXT_DEF(COMM_INFO_spare_28,  "")          /* 28 */
TEXT_DEF(COMM_INFO_spare_29,  "")          /* 29 */
TEXT_DEF(COMM_INFO_spare_30,  "")          /* 30 */
TEXT_DEF(COMM_INFO_spare_31,  "")          /* 31 */
TEXT_DEF(COMM_INFO_spare_32,  "")          /* 32 */
TEXT_DEF(COMM_INFO_spare_33,  "")          /* 33 */
TEXT_DEF(COMM_INFO_spare_34,  "")          /* 34 */
TEXT_DEF(COMM_INFO_spare_35,  "")          /* 35 */
TEXT_DEF(COMM_INFO_spare_36,  "")          /* 36 */
TEXT_DEF(COMM_INFO_spare_37,  "")          /* 37 */
TEXT_DEF(COMM_INFO_spare_38,  "")          /* 38 */
TEXT_DEF(COMM_INFO_spare_39,  "")          /* 39 */
TEXT_DEF(COMM_INFO_spare_40,  "")          /* 40 */
TEXT_DEF(COMM_INFO_spare_41,  "")          /* 41 */
TEXT_DEF(COMM_INFO_spare_42,  "")          /* 42 */
TEXT_DEF(COMM_INFO_spare_43,  "")          /* 43 */
TEXT_DEF(COMM_INFO_spare_44,  "")          /* 44 */
TEXT_DEF(COMM_INFO_spare_45,  "")          /* 45 */
TEXT_DEF(COMM_INFO_spare_46,  "")          /* 46 */
TEXT_DEF(COMM_INFO_spare_47,  "")          /* 47 */
TEXT_DEF(COMM_INFO_spare_48,  "")          /* 48 */
TEXT_DEF(COMM_INFO_spare_49,  "")          /* 49 */
TEXT_DEF(COMM_INFO_spare_50,  "")          /* 50 */

#else

#ifndef __COMM_H__
#define __COMM_H__


/*****************************************************************************
 *                          CONSTANT DEFINITIONS                             *
 *****************************************************************************/
#define MAX_BUFFER_SIZE   32

/*****************************************************************************
 *                       ENUMERATION DEFINITIONS                             *
 *****************************************************************************/
 typedef enum comm_status_s
 {
   comm_status_ok = 0,
   comm_status_ready,
   comm_status_no_resources_to_start_manager,
   comm_status_channel_instance_not_exist,
   comm_status_channel_added,
   comm_status_channel_port_already_attached,
   comm_status_no_resources_to_add_channel,
   comm_status_no_resources_to_open_port,
   comm_status_channel_number_not_valid,
   comm_status_channel_port_not_open,
   comm_status_channel_deleted,
   comm_status_channel_buffer_config_ok,
   comm_status_channel_port_open_ok,
   comm_status_channel_ctrl_port_open_ok,
   comm_status_channel_port_already_open,
   comm_status_channel_ctrl_port_already_open,
   comm_status_channel_open_port_error,
   comm_status_channel_open_ctrl_port_error,
   comm_status_channel_port_close_ok,
   comm_status_channel_port_close_error,
   comm_status_channel_ctrl_port_close_ok,
   comm_status_channel_ctrl_port_close_error,
   comm_status_set_baudrate_ok,
   comm_status_set_baudrate_error,
   comm_status_read_finished,
   comm_status_read_ongoing,
   comm_status_read_error,
   comm_status_read_timeout,
   comm_status_write_finished,
   comm_status_write_ongoing,
   comm_status_write_error,
   comm_status_write_timeout,
   comm_status_set_buffer_size_ok,
   comm_status_error_set_buffer_size,
   comm_status_error_flush_buffers,
   comm_status_flush_buffers_ok,
   comm_status_error,
   comm_status_error_unknown,
   comm_status_not_valid = 0xFFFF
 } comm_status_t;

 typedef enum comm_errors_s
 {
   comm_ce_break = 0,/* The HW detected a break condition                    */
   comm_ce_frame,    /* The HW detected a frame error                        */
   comm_ce_ioe,      /* An I/O error occurred during communications with the */
                     /* device .                                             */
   comm_ce_mode,     /* The requested mode is not supported, or the hFile    */
                     /* parameter is invalid. If this value is specified, it */
                     /* is the only valid error.                             */
   comm_ce_overrun,  /* A character-buffer overrun has occurred. The next    */
                     /* character is lost.                                   */
   comm_ce_rxover,   /* An input buffer overflow has occurred. There is      */
                     /* either no room in the input buffer, or a character   */
                     /* was received after the end-of-file (EOF) character.  */
   comm_ce_rxparity, /* The hardware detected a parity error.                */
   comm_ce_txfull,   /* The application tried to transmit a character, but   */
                     /* the output buffer was full.                          */
   comm_error_channel_not_exist,
   comm_error_channel_number_not_valid,
   comm_error_channel_in_error_state,
   comm_error_channel_in_unknown_state,
   comm_error_buffers_no_config,
   comm_error_read_timeout,
   comm_error_read_error,
   comm_error_no_mem_for_read_operation,
   comm_error_read_already_ongoing,
   comm_error_write_timeout,
   comm_error_write_error,
   comm_error_write_already_ongoing,
   comm_error_serial_not_open,
   comm_error_open_failed,
   comm_error_serial_no_device,
   comm_error_no_more_devices,
   comm_error_device_already_assigned,
   comm_error_device_not_ready,
   comm_error_busy,
   comm_error_set_buffer_size,
   comm_error_set_baudrate_error,
   comm_error_channel_port_close_error,
   comm_error_flush_buffers_error,
   comm_error_rts_not_valid,
   comm_error_dtr_not_valid,
   comm_error_no_error,
   comm_error_unkown = 0xFFFF
 } comm_error_t;

 typedef enum comm_debug_s
 {
  comm_debug_COMM_add_channel,
  comm_debug_COMM_delete_channel,
  comm_debug_COMM_abort_process,
  comm_debug_COMM_open_port,
  comm_debug_COMM_open_port_result,
  comm_debug_COMM_ctrl_port_open_port,
  comm_debug_COMM_ctrl_port_open_port_result,
  comm_debug_COMM_close_comport,
  comm_debug_COMM_ctrl_port_close_port,
  comm_debug_COMM_set_baudrate,
  comm_debug_COMM_set_comport_buffers,
  comm_debug_COMM_send_buffer,
  comm_debug_COMM_send_buffer_result,
  comm_debug_COMM_receive_buffer,
  comm_debug_COMM_receive_buffer_result,
  comm_debug_COMM_flush_rx_buffer,
  comm_debug_COMM_flush_tx_buffer,
  comm_debug_COMM_get_rts,
  comm_debug_COMM_set_rts,
  comm_debug_COMM_get_dtr,
  comm_debug_COMM_set_dtr,
  comm_debug_COMM_get_dsr,
  comm_debug_COMM_ctrl_port_get_rts,
  comm_debug_COMM_ctrl_port_set_rts,
  comm_debug_COMM_ctrl_port_get_dtr,
  comm_debug_COMM_ctrl_port_set_dtr,
  comm_debug_COMM_ctrl_port_get_dsr,
  comm_debug_COMM_get_status,
  comm_debug_COMM_clear_status,
  comm_debug_COMM_set_status,
  comm_debug_COMM_get_last_error,
  comm_debug_COMM_clear_last_error,
  comm_debug_COMM_tx_notification,
  comm_debug_COMM_tx_process,
  comm_debug_COMM_tx_process_result,
  comm_debug_COMM_rx_notification,
  comm_debug_COMM_rx_process,
  comm_debug_COMM_stop_AT_mode,
  comm_debug_COMM_start_AT_mode,
  comm_debug_COMM_send_AT_command,
  comm_debug_COMM_tx_process_at_mode,
  comm_debug_COMM_rx_process_at_mode,
  comm_debug_COMM_rx_notification_at_mode,
  comm_debug_COMM_tx_at_mode_not,
  comm_debug_COMM_entry_not_valid = 0xFFFF
 } comm_debug_t;

 typedef enum comm_rts_control_s
 {
   comm_rts_control_disable,
   comm_rts_control_enable,
   comm_rts_control_handshake,
   comm_rts_control_toggle
 } comm_rts_control_t;

 typedef enum comm_dtr_control_s
 {
   comm_dtr_control_disable,
   comm_dtr_control_enable,
   comm_dtr_control_handshake
 } comm_dtr_control_t;

 typedef enum comm_dsr_control_s
 {
   comm_dsr_control_disable,
   comm_dsr_control_enable
 } comm_dsr_control_t;

/*****************************************************************************
 *                            TYPE DEFINITIONS                               *
 *****************************************************************************/
#define MAX_COMPORT_NAME_LENGTH 127
typedef struct comm_cfg_s
{
  char  comport_name[MAX_COMPORT_NAME_LENGTH+1];
  uint32_t   baudrate;
} comm_cfg_t;


typedef struct ctrl_cfg_s
{
  comm_cfg_t         com_cfg;
  comm_rts_control_t rts;
  comm_dtr_control_t dtr;
  comm_dsr_control_t dsr;
} ctrl_cfg_t;

/*****************************************************************************
 *                        PROTOTYPES DEFINITIONS                             *
 *****************************************************************************/

 typedef void (*comm_fx_tx_t) (uint8_t id);
 typedef void (*comm_fx_rx_t) (uint8_t id, uint32_t size);

 typedef comm_status_t  (*comm_receive_buffer_t)(uint8_t channel,
                                                 char *buffer,
                                                 uint32_t   bytes_to_read,
                                                 uint32_t  *bytes_read,
                                                 comm_fx_rx_t  fx);

 /****************************************************************************
 * Function:     void COMM_init(void)
 *
 *---------------------------------------------------------------------------
 * Purpose:    This function initialize the globals structures used by the
 *             communication manager
 *
 * Parameters: None
 *
 * Returns:    Nothing
 ******************************************************************************/
 extern void COMM_init(void);

/****************************************************************************
 * Function:     void COMM_close(void)
 *
 *---------------------------------------------------------------------------
 * Purpose:    This function deallocate the resources allocated by the
 *             communication manager
 *
 * Parameters: None
 *
 * Returns:    Nothing
 ******************************************************************************/
 extern void COMM_close(void);

/****************************************************************************
 * Function:     void COMM_clock(void)
 *
 *---------------------------------------------------------------------------
 * Purpose:    This function execute one time the communication handler
 *             internal state machine.
 *
 * Parameters: None
 *
 * Returns:    Nothing
 ******************************************************************************/
 extern void COMM_clock(void);


/****************************************************************************
 * Function:     void           COMM_error_report(uint8_t comm_channel_id,
 *                                                uint32_t error, uint32_t line)
 *
 *---------------------------------------------------------------------------
 * Purpose:    This function reports an error occurred in the comport interface
 *
 * IN
 * channel      channel id
 * error        WIN32 system error number
 * line         line number in the file comport.c
 *
 * Returns:    Nothing
 ******************************************************************************/
 extern void           COMM_error_report(uint8_t comm_channel_id, uint32_t error, uint32_t line);

 /****************************************************************************
 * Function:     comm_status_t  COMM_open_port(uint8_t channel,
 *                                             comm_cfg_t *p_comm_cfg);
 *
 *---------------------------------------------------------------------------
 * Purpose:    This function open a comport.
 *
 * Parameters:
 * IN
 * channel      channel id
 * comm_cfg     Pointer to the name of the port to open and the baudrate to use
 *
 * IN/OUT
 *
 * Returns:    One of the values given in the enumeration comm_status_t
 ******************************************************************************/
 extern comm_status_t  COMM_open_port(uint8_t channel, comm_cfg_t *p_comm_cfg);

 /****************************************************************************
 * Function:   comm_status_t  COMM_close_comport(uint8_t channel)
 *
 *---------------------------------------------------------------------------
 * Purpose:    This function close the comport.
 *
 * Parameters:
 * IN
 * channel      channel id
 *
 * IN/OUT
 *
 * Returns:    One of the values given in the enumeration comm_status_t
 ******************************************************************************/
 extern comm_status_t  COMM_close_comport(uint8_t channel);

 /****************************************************************************
 * Function:   comm_status_t  COMM_set_baudrate(uint8_t channel, uint32_t baudrate);
 *
 *---------------------------------------------------------------------------
 * Purpose:    This function open a comport.
 *
 * Parameters:
 * IN
 * channel      channel id
 * baudrate     Baudrate to set
 *
 * IN/OUT
 *
 * Returns:    One of the values given in the enumeration comm_status_t
 ******************************************************************************/
 extern comm_status_t  COMM_set_baudrate(uint8_t channel, uint32_t baudrate);

 /****************************************************************************
 * Function:     comm_status_t  COMM_set_rx_buffer(uint8_t channel, uint32_t buf_size);
 *
 *---------------------------------------------------------------------------
 * Purpose:    This function configures the internal RX and TX buffer in the
 *             UART hw device.
 *
 * Parameters:
 * IN
 * channel      channel id
 * buf_size     internal buffer size
 *
 * IN/OUT
 *
 * Returns:    One of the values given in the enumeration comm_status_t
 ******************************************************************************/
 extern comm_status_t  COMM_set_comport_buffers(uint8_t  channel,
                                                uint32_t rx_buf_size,
                                                uint32_t tx_buf_size);

/****************************************************************************
 * Function:     comm_status_t  COMM_add_channel(uint8_t channel);
 *
 *---------------------------------------------------------------------------
 * Purpose:    This function creates a communication handler instance
 *
 * Parameters:
 * IN
 * channel      channel id
 *
 * IN/OUT
 *
 * Returns:    One of the values given in the enumeration comm_status_t
 ******************************************************************************/
 extern comm_status_t  COMM_add_channel(uint8_t channel);

/****************************************************************************
 * Function:     comm_status_t  COMM_delete_channel(uint8_t channel);
 *
 *---------------------------------------------------------------------------
 * Purpose:    This function dispose the communication handler instance given
 *             in the parameter channel
 *
 * Parameters:
 * IN
 * channel      channel id
 *
 * IN/OUT
 *
 * Returns:    One of the values given in the enumeration comm_status_t
 ******************************************************************************/
 extern comm_status_t  COMM_delete_channel(uint8_t channel);

/****************************************************************************
 * Function:     comm_status_t  COMM_abort_process(uint8_t channel);
 *
 *---------------------------------------------------------------------------
 * Purpose:    This function abort any oustanding activity and set the state
 *             machine (for the instance given in the parameter "channel") to
 *             idle state.
 *
 * Parameters:
 * IN
 * channel      channel id
 *
 * IN/OUT
 *
 * Returns:    One of the values given in the enumeration comm_status_t
 ******************************************************************************/
 extern comm_status_t  COMM_abort_process(uint8_t channel);

 /****************************************************************************
 * Function:     comm_status_t  COMM_receive_buffer(uint8_t      channel,
 *                                                  char    *buffer,
 *                                                  uint32_t      bytes_to_read
 *                                                  uint32_t     *bytes_read
 *                                                  fx_tx_t  fx);
 *
 *---------------------------------------------------------------------------
 * Purpose:    This function read a buffer from the communication interface
 *             of length bytes_to_read. The parameter *bytes_read is the number
 *             bytes read.
 *
 * Parameters:
 * IN
 * channel        channel id
 * bytes_to_read  length in bytes of the buffer to transmit.
 * fx:            callback function to call when the buffer has been read
 *
 * OUT
 * buffer         pointer to the buffer where the read .
 * bytes_read     length in bytes of the buffer read.
 *
 * Returns:    One of the values given in the enumeration comm_status_t
 ******************************************************************************
 * extern comm_status_t  COMM_receive_buffer(uint8_t channel,
 *                                           char    *buffer,
 *                                           uint32_t bytes_to_read,
 *                                           uint32_t     *bytes_read,
 *                                           comm_fx_rx_t fx);
 *
 ******************************************************************************/
 extern comm_receive_buffer_t COMM_receive_buffer;

 /****************************************************************************
 * Function:     comm_status_t  COMM_send_buffer(uint8_t      channel,
 *                                               char    *buffer,
 *                                               uint32_t      bytes_to_send
 *                                               uint32_t     *bytes_sent
 *                                               fx_tx_t  fx);
 *
 *---------------------------------------------------------------------------
 * Purpose:    This function transmit the buffer given in the parameter buffer
 *             of length "bytes_to_send".
 *
 * Parameters:
 * IN
 * channel        channel id
 * buffer         pointer to the buffer to transmit.
 * bytes_to_send  length in bytes of the buffer to transmit.
 * bytes_sent     length in bytes of the transmitted buffer.
 * fx:            callback function to call when the buffer has been transmitted
 *
 * IN/OUT
 *
 * Returns:    One of the values given in the enumeration comm_status_t
 ******************************************************************************/
 extern comm_status_t  COMM_send_buffer(uint8_t channel,       char    *buffer,
                                        uint32_t bytes_to_send, uint32_t     *bytes_sent,
                                        comm_fx_tx_t  fx);

/****************************************************************************
 * Function:     comm_status_t  COMM_flush_rx_buffer(uint8_t channel);
 *
 *---------------------------------------------------------------------------
 * Purpose:    This function clears the internal rx buffer in the comport
 *             HW device.
 *
 * Parameters:
 * IN
 * channel      channel id
 *
 * IN/OUT
 *
 * Returns:    One of the values given in the enumeration comm_status_t
 ******************************************************************************/
 extern comm_status_t  COMM_flush_rx_buffer(uint8_t channel);

/****************************************************************************
 * Function:     comm_status_t  COMM_get_rts(uint8_t channel,
 *                                           comm_rts_control_t *rts);
 *
 *---------------------------------------------------------------------------
 * Purpose:    This function returns the state of the rts line
 *
 * Parameters:
 * IN
 * channel      channel id
 *
 * OUT
 * *rts        The *rts have following valid values
 *               comm_rts_control_disable,
 *               comm_rts_control_enable,
 *
 *
 * Returns:    The function returns the value comm_status_ok
 *
 ******************************************************************************/
comm_status_t  COMM_get_rts(uint8_t channel, comm_rts_control_t *rts);

/****************************************************************************
 * Function:     comm_status_t  COMM_set_rts(uint8_t channel, comm_rts_control_t rts);
 *
 *---------------------------------------------------------------------------
 * Purpose:    This function clears the internal rx buffer in the comport
 *             HW device.
 *
 * Parameters:
 * IN
 * channel      channel id
 * rts          rts state to set
 *
 * IN/OUT
 *
 * Returns:    The function returns the value comm_status_ok
 *
 ******************************************************************************/
 extern comm_status_t  COMM_set_rts(uint8_t channel, comm_rts_control_t rts);

/****************************************************************************
 * Function:   comm_status_t  COMM_get_dtr(uint8_t channel,
 *                                         comm_dtr_control_t *dtr)
 *
 *---------------------------------------------------------------------------
 * Purpose:    This function returns the state of the dtr line.
 *
 * Parameters:
 * IN
 * channel      channel id
 *
 * OUT
 * *dtr        This parameter returns one of the following values:
 *               comm_dtr_control_disable,
 *               comm_dtr_control_enable,
 *
 *
 * Returns:    The function returns the value comm_status_ok
 *
 ******************************************************************************/
 extern comm_status_t  COMM_get_dtr(uint8_t channel, comm_dtr_control_t *dtr);

/****************************************************************************
 * Function:     comm_status_t  COMM_set_dtr(uint8_t channel, comm_dtr_control_t dtr);
 *
 *---------------------------------------------------------------------------
 * Purpose:    This function clears the internal rx buffer in the comport
 *             HW device.
 *
 * Parameters:
 * IN
 * channel      channel id
 *
 * IN/OUT
 *
 * Returns:    The function returns the value comm_status_ok
 *
 ******************************************************************************/
 extern comm_status_t  COMM_set_dtr(uint8_t channel, comm_dtr_control_t dtr);

/****************************************************************************
 * Function:     comm_status_t  COMM_get_dsr(uint8_t channel,
 *                                           comm_dsr_control_t *dsr);
 *
 *---------------------------------------------------------------------------
 * Purpose:    This function returns the state of the dsr on the control
 *             port.
 *
 * Parameters:
 * IN
 * channel      channel id
 *
 * OUT
 * *dsr        This function returns one of the following values:
 *               comm_dsr_control_disable,
 *               comm_dsr_control_enable,
 *
 * Returns:    The function returns the value comm_status_ok
 *
 ******************************************************************************/
 extern comm_status_t  COMM_get_dsr(uint8_t channel, comm_dsr_control_t *dsr);

/****************************************************************************
 * Function:     comm_status_t  COMM_flush_tx_buffer(uint8_t channel);
 *
 *---------------------------------------------------------------------------
 * Purpose:    This function clears the internal tx buffer in the comport
 *             HW device.
 *
 * Parameters:
 * IN
 * channel      channel id
 *
 * IN/OUT
 *
 * Returns:    One of the values given in the enumeration comm_status_t
 ******************************************************************************/
 extern comm_status_t  COMM_flush_tx_buffer(uint8_t channel);

/****************************************************************************
 * Function:     comm_status_t  COMM_get_status(uint8_t channel, char *status_text);
 *
 *---------------------------------------------------------------------------
 * Purpose:    This function returns the last status registred in the state
 *             machine instance.
 *
 * Parameters:
 * IN
 * channel      channel id
 *
 * IN/OUT
 *
 * Returns:    One of the values given in the enumeration comm_status_t
 ******************************************************************************/
 extern comm_status_t  COMM_get_status(uint8_t channel, char *status_text);

 /****************************************************************************
 * Function:     comm_status_t  COMM_clear_status(uint8_t channel);
 *
 *---------------------------------------------------------------------------
 * Purpose:    This function clears the last registred status to "comm_status_ok".
 *
 * Parameters:
 * IN
 * channel      channel id
 *
 * IN/OUT
 *
 * Returns:    One of the values given in the enumeration comm_status_t
 ******************************************************************************/
 extern comm_status_t  COMM_clear_status(uint8_t channel);

 /****************************************************************************
 * Function:     comm_error_t  COMM_get_last_error(uint8_t channel, char *error_text);
 *
 *---------------------------------------------------------------------------
 * Purpose:    This function returns the last error registred in the
 *             communication module
 *
 * Parameters:
 * IN
 * channel      channel id
 *
 * IN/OUT
 *
 * Returns:    One of the values given in the enumeration comm_error_t
 ******************************************************************************/
 extern comm_error_t  COMM_get_last_error(uint8_t channel);


 /****************************************************************************
 * Function:     comm_status_t  COMM_clear_last_error(uint8_t channel);
 *
 *---------------------------------------------------------------------------
 * Purpose:    This function clears the last registred error to
 *             "comm_error_no_error".
 *
 * Parameters:
 * IN
 * channel      channel id
 *
 * IN/OUT
 *
 * Returns:    One of the values given in the enumeration comm_status_t
 ******************************************************************************/
 extern comm_status_t  COMM_clear_last_error(uint8_t channel);


/****************************************************************************
 * Function:     comm_status_t  COMM_ctrl_port_open_port(uint8_t channel,
 *                                                       ctrl_cfg_t *p_ctrl_cfg);
 *
 *---------------------------------------------------------------------------
 * Purpose:    This function open a comport.
 *
 * Parameters:
 * IN
 * channel      channel id
 * comm_cfg     Pointer to the name of the port to open and the baudrate to use
 *
 * IN/OUT
 *
 * Returns:    One of the values given in the enumeration comm_status_t
 ******************************************************************************/
 extern comm_status_t  COMM_ctrl_port_open_port(uint8_t channel, ctrl_cfg_t *p_ctrl_cfg);


 /****************************************************************************
 * Function:   comm_status_t COMM_close_ctrl_port(uint8_t channel)
 *
 *---------------------------------------------------------------------------
 * Purpose:    This function close the control comport.
 *
 * Parameters:
 * IN
 * channel      channel id
 *
 * IN/OUT
 *
 * Returns:    One of the values given in the enumeration comm_status_t
 ******************************************************************************/
 extern comm_status_t COMM_close_ctrl_port(uint8_t channel);


 /****************************************************************************
 * Function:     comm_status_t  COMM_ctrl_port_get_rts(uint8_t channel,
 *                                                     comm_rts_control_t *rts);
 *
 *---------------------------------------------------------------------------
 * Purpose:    This function clears the internal rx buffer in the comport
 *             HW device.
 *
 * Parameters:
 * IN
 * channel      channel id
 *
 * IN/OUT
 * *rts        The *rts have following valid values
 *               comm_rts_control_disable,
 *               comm_rts_control_enable,
 *
 *
 * Returns:    The function returns the value comm_status_ok
 ******************************************************************************/
comm_status_t  COMM_ctrl_port_get_rts(uint8_t channel, comm_rts_control_t *rts);

/****************************************************************************
 * Function:     comm_status_t  COMM_ctrl_portset_rts(uint8_t channel,
 *                                                    comm_rts_control_t rts);
 *
 *---------------------------------------------------------------------------
 * Purpose:    This function set the rts on the control comport to the value
 *             specified in the parameter dtr.
 *
 *
 * Parameters:
 * IN
 * channel      channel id
 * rts          rts state to set
 *
 * IN/OUT
 *
 * Returns:    The function returns the value comm_status_ok
 *
 ******************************************************************************/
 extern comm_status_t  COMM_ctrl_port_set_rts(uint8_t channel, comm_rts_control_t rts);

/****************************************************************************
 * Function:     comm_status_t  COMM_ctrl_port_get_dtr(uint8_t channel,
 *                                                     comm_dtr_control_t *dtr)
 *
 *---------------------------------------------------------------------------
 * Purpose:    This function returns the state of the dtr on the control
 *             port.
 *
 * Parameters:
 * IN
 * channel      channel id
 *
 * IN/OUT
 *
 * Returns:    The function returns the value comm_status_ok
 *
 ******************************************************************************/
 extern comm_status_t  COMM_ctrl_port_get_dtr(uint8_t channel,
                                              comm_dtr_control_t *dtr);

/****************************************************************************
 * Function:     comm_status_t  COMM_ctrl_port_set_dtr(uint8_t channel,
 *                                                     comm_dtr_control_t dtr);
 *
 *---------------------------------------------------------------------------
 * Purpose:    This function set the dtr on the control comport to the value
 *             specified in the parameter dtr.
 *
 * Parameters:
 * IN
 * channel      channel id
 * dtr          dtr state to set.
 *
 * IN/OUT
 *
 * Returns:    The function returns the value comm_status_ok
 *
 ******************************************************************************/
 extern comm_status_t  COMM_ctrl_port_set_dtr(uint8_t channel, comm_dtr_control_t dtr);


/****************************************************************************
 * Function:     comm_status_t  COMM_ctrl_port_get_dsr(uint8_t channel,
 *                                                     comm_dsr_control_t *dsr);
 *
 *---------------------------------------------------------------------------
 * Purpose:    This function returns the state of the dsr on the control
 *             port.
 *
 * Parameters:
 * IN
 * channel      channel id
 * dtr          dtr state to set.
 *
 * IN/OUT
 *
 * Returns:    The function returns the value comm_status_ok
 *
 ******************************************************************************/
 extern comm_status_t  COMM_ctrl_port_get_dsr(uint8_t channel, comm_dsr_control_t *dsr);

/****************************************************************************
 * Function:     comm_status_t  COMM_start_AT_mode(uint8_t channel, uint8_t *rx_buffer);
 *
 *---------------------------------------------------------------------------
 * Purpose:    This function set the communication handler in AT state mode
 *             (only if the communication handler is in Idle state).
 *             In this state the Communication handler will transmit and
 *
 * Parameters:
 * IN
 * channel      channel id
 * *rx_buffer   Pointer to the buffer where the AT response commnand will be
 *              stored.
 *
 * IN/OUT
 *
 * Returns:    The function returns one of the following values
 *             comm_status_ok                         : The communication
 *                                                      handler is in state
 *                                                      AT mode.
 *             comm_status_channel_instance_not_exist :  The communication
 *                                                       handler for the
 *             given channel does not exist.
 *             comm_status_channel_number_not_valid   : The channel number
 *                                                      given is not valid

 *
 ******************************************************************************/
 extern comm_status_t  COMM_start_AT_mode(uint8_t channel, uint8_t *Rx_buffer);

/****************************************************************************
 * Function:     comm_status_t  COMM_stop_AT_mode(uint8_t channel);
 *
 *---------------------------------------------------------------------------
 * Purpose:    This function set the communication handler in AT state mode
 *             (only if the communication handler is in Idle state).
 *             In this state the Communication handler will transmit and
 *
 * Parameters:
 * IN
 * channel      channel id
 * *rx_buffer   Pointer to the buffer where the AT response commnand will be
 *              stored.
 *
 * IN/OUT
 *
 * Returns:    The function returns one of the following values
 *             comm_status_ok                         : The communication
 *                                                      handler leaves the AT
 *             mode state and enters in Idle state.
 *
 *             comm_status_channel_instance_not_exist :  The communication
 *                                                       handler for the
 *             given channel does not exist.
 *
 *             comm_status_channel_number_not_valid   : The channel number
 *                                                      given is not valid

 *
 ******************************************************************************/
 extern comm_status_t  COMM_stop_AT_mode(uint8_t channel);


 /****************************************************************************
 * Function:     comm_status_t COMM_send_AT_command(uint8_t channel,
 *                                                  char *data,
 *                                                  uint16_t data_length)
 *
 *---------------------------------------------------------------------------
 * Purpose:    This function set the communication handler in AT state mode.
 *             In this state the Communication handler will transmit and
 *
 * Parameters:
 * IN
 * channel      channel id
 * dtr          dtr state to set.
 *
 * IN/OUT
 *
 * Returns:    The function returns the value comm_status_ok
 *
 ******************************************************************************/
comm_status_t COMM_send_AT_command(uint8_t channel, char *data, uint16_t data_length,
                                   comm_fx_tx_t  fx);


 /****************************************************************************
 * Function:   comm_status_t COMM_close_physical_device_only(uint8_t channel)
 *
 *---------------------------------------------------------------------------
 * Purpose:    This function close the physical device.
 *
 * Parameters:
 * IN
 * channel      channel id
 *
 * IN/OUT
 *
 * Returns:    result
 ******************************************************************************/
extern uint8_t COMM_close_physical_device_only(uint8_t channel);


/****************************************************************************
* Function:   comm_status_t open_fw_download_port(uint8_t channel, char *p_fw_download_port_name, uint32_t baud_rate)
*
*---------------------------------------------------------------------------
* Purpose:    This function open the physical device.
*
* Parameters:
* IN
* channel      channel id
* p_fw_download_port_name   device name
* baud_rate   baud rate
*
* IN/OUT
*
* Returns:    void
******************************************************************************/
extern void open_fw_download_port(uint8_t channel, char *p_fw_download_port_name, uint32_t baud_rate);

#endif  /* #ifndef __COMM_H__ */

#endif

