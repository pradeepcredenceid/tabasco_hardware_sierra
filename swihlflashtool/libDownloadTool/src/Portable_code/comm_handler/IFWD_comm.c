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
 *  $File name:  /msw_tools/FlashTool/libDownloadTool/src/Portable_code/comm_handler/IFWD_comm.c $
 *  $CC-Version: .../dhkristx_140314_v0/1 $
 *  $Date:       2014-04-23    10:41:59 UTC $
 *   Responsible: bentzen, Hilmar
 *   Comment:
 *     SMS00897775
 */

#define THIS_MODULE MODULE_COMM
//#define STATE_TRACE

#include "OS.h"
#include "DebugModule.h"

#include <stdio.h>
#include <string.h>
#include "IFWD_global_func.h"
#include "IFWD_timer.h"
#include "IFWD_mem.h"
#include "IFWD_comport.h"
#include "IFWD_comm.h"
#include "IFWD_single_file_handler.h"


/*****************************************************************************
*                            CONSTANT DEFINITIONS                           *
*****************************************************************************/
#define COMM_VERSION                      "1.0"

#define MAX_NUMBER_OF_INSTANCES           MAX_NUMBER_OF_CHANNELS + 1
#define COMM_MANAGER_ID                   0xFF

#define COMPORT_RX_BUFFER_SIZE           (16384*2)
#define COMPORT_TX_BUFFER_SIZE           (16384*2)

/* GenericUSB I/O Buffer (100 MB) */
#define USB_RX_BUFFER_SIZE           (1024*1024*100)
#define USB_TX_BUFFER_SIZE           (1024*1024*100)

//< Previous USB chunk size: 0x01000000
#define USB_CHUNK_SIZE    0x020000

//< Target MIPI DMA has a max of 256kB (including header)
#define MIPI_CHUNK_SIZE   256*1024-sizeof(uint32_t)
#define MIPI_HEADER_SIZE  MIPI_CHUNK_SIZE+sizeof(uint32_t)

/*****************************************************************************
*                                ENUMERATIONS                               *
*****************************************************************************/

typedef enum comm_state_s
{
  comm_state_idle = 0,
  comm_state_rx_tx_ready,//1
  comm_state_rx,
  comm_state_rx_in_progress,
  comm_state_rx_wait_block,
  comm_state_rx_next_block,
  comm_state_rx_completed,
  comm_state_rx_timeout,
  comm_state_tx,
  comm_state_tx_start,
  comm_state_tx_in_progress,
  comm_state_tx_next_block,//11
  comm_state_tx_completed,
  comm_state_at_mode,
  comm_state_tx_at_mode,
  comm_state_tx_timeout,
  comm_state_error,
  comm_state_tx_completed_delay,
  comm_state_not_valid_state = 0xFFFF
} comm_state_t;

typedef enum comm_at_mode_state_s
{
  comm_state_at_mode_enable = 0,
  comm_state_at_mode_disable,
  comm_state_at_mode_tx,
  comm_state_at_mode_rx,
  comm_state_at_mode_not_valid_state = 0xFFFF
} comm_at_mode_state_t;

typedef enum comm_channel_state_s
{
  comm_channel_state_enabled = 0,
  comm_channel_state_disabled,
  comm_channel_state_port_ready,
  comm_channel_state_error,
  comm_channel_state_not_valid = 0xFFFF
} comm_channel_state_t;



/*****************************************************************************
*                         PROTOTYPES DEFINITIONS                            *
*****************************************************************************/
void comm_rebuild_active_list(void);
/*****************************************************************************
*                             TYPE DEFINITIONS                              *
*****************************************************************************/

typedef struct tx_comm_buffer_s
{
  char  *buffer;
  uint32_t        total_length;
  uint32_t        rest_length;
  uint32_t        tx_length;
  uint32_t        length_txed;
  uint32_t        length;
} tx_comm_buffer_t;

typedef struct rx_comm_buffer_s
{
  char  *buffer;
  uint32_t        total_length;
  uint32_t        rest_length;
  uint32_t        rx_length;
  uint32_t        length_rxed;
  uint32_t        length;
} rx_comm_buffer_t;

typedef struct comm_debug_info_s
{
  uint32_t  nr_of_abort_call;
  uint32_t  nr_of_overflow_bytes_tx;
  uint32_t  bytes_tx;
  uint32_t  nr_of_overflow_bytes_rx;
  uint32_t  bytes_rx;
  uint32_t  idle_time;
  uint32_t  busy_time;
  uint32_t  total_time;
  uint32_t  max_peak_baudrate_tx;
  uint32_t  min_peak_baudrate_tx;
  uint32_t  max_peak_baudrate_rx;
  uint32_t  min_peak_baudrate_rx;
  uint32_t  avg_baudrate_tx;
  uint32_t  avg_baudrate_rx;
  timer_chronom_t  chronom_idle;
  timer_chronom_t  chronom_busy;
  timer_chronom_t  chronom_tx;
  timer_chronom_t  chronom_rx;
  uint32_t  nr_of_mem_malloc_calls;
  uint32_t  nr_of_mem_free_calls;
} comm_debug_info_t;

/*typedef void (*comm_handler_t)(void *p_comm_ins);*/

typedef struct comm_instance_s
{
  uint8_t                        channel_id;
  comm_cfg_t                     comm_cfg;
  ctrl_cfg_t                     ctrl_cfg;
  comm_state_t                   state;
  void (*handler)(struct comm_instance_s *);  /* handler = a function pointer that takes a pointer to this struct as argument */
/*  comm_handler_t                 handler; */
  comm_at_mode_state_t           at_mode_state;
  comm_channel_state_t           channel_state;
  comport_handler_t             *comport;
  comport_handler_t             *ctrl_comport;
  comport_modem_status_line_t    modem_status;
  rx_comm_buffer_t               rx_buffer;
  uint8_t                       *temp_rx_buffer;
  comm_fx_rx_t                   fx_rx;
  tx_comm_buffer_t               tx_buffer;
  comm_fx_tx_t                   fx_tx;
  timer_event_timer_t            rx_timer;
  timer_event_timer_t            tx_timer;
  timer_event_timer_t            tx_timer_vp_delay;
  uint32_t                       at_cmd_buf_len_txd;
  comm_status_t                  last_status;
  comm_error_t                   last_error;
  uint32_t                       rx_buffer_size;
  uint32_t                       tx_buffer_size;
  uint16_t                       debug_enabled;
  uint16_t                       done;
  uint16_t                       instance_unning;
} comm_instance_t;


typedef struct comm_manager_error_s
{
  uint8_t              channel_id;
  comm_error_t         code;
} comm_manager_error_t;

typedef struct comm_manager_status_s
{
  uint8_t              channel_id;
  comm_status_t        code;
} comm_manager_status_t;

#if 0
typedef struct comm_manager_s
{
  uint8_t                manager_id;
  uint8_t                nr_of_instances_running;
  comm_manager_state_t   state;
  comm_instance_t       *comm_handler[MAX_NUMBER_OF_INSTANCES];
  uint8_t                active_comm_handlers[MAX_NUMBER_OF_INSTANCES]; /* list of active channels */
  comm_manager_status_t  status;
  comm_manager_error_t   error;
} comm_manager_t;
#endif



/*****************************************************************************
*                             LOCAL PROTOTYPES                              *
*****************************************************************************/
static void COMM_handler(comm_instance_t *p_comm_ins);
static void COMM_handler_at_mode(comm_instance_t *p_comm_ins);
static void COMM_rx_notification(uint8_t channel, uint32_t length);
static void COMM_rx_not_at_mode(uint8_t channel, uint32_t length);
static void COMM_rx_process(comm_instance_t *p_comm_ins);
static void COMM_rx_process_at_mode(comm_instance_t *p_comm_ins);
static void COMM_tx_notification(uint8_t channel, uint32_t bytes_txed);
static void COMM_tx_process(comm_instance_t *p_comm_ins);
static void COMM_tx_process_at_mode(comm_instance_t *p_comm_ins);
static void COMM_open_comport(uint8_t id, comport_handler_t **p_comport, comm_cfg_t *p_comm_cfg);
static comm_status_t  COMM_usb_receive_buffer(uint8_t channel,        char *buffer,
                                              uint32_t bytes_to_read, uint32_t *bytes_read,
                                              comm_fx_rx_t       fx);

#ifdef _WIN32
  static comm_status_t  COMM_com_receive_buffer(uint8_t    channel,
                                                 char *buffer,
                                                 uint32_t   bytes_to_read,
                                                 uint32_t  *bytes_read,
                                                 comm_fx_rx_t  fx);
#endif

#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
static void COMM_err(uint8_t channel, uint32_t err);
#endif
/*****************************************************************************
*                            PUBLIC PROTOTYPES                              *
*****************************************************************************
* void           COMM_init(void);
* void           COMM_close(void);
* void           COMM_clock(void);
* void           COMM_error_report(uint8_t comm_channel_id, uint32_t error, uint32_t line);
* comm_status_t  COMM_open_port(uint8_t channel, comm_cfg_t *p_comm_cfg);
* comm_status_t  COMM_close_comport(uint8_t channel);
* comm_status_t  COMM_set_baudrate(uint8_t channel, uint32_t baudrate);
* comm_status_t  COMM_set_comport_buffers(uint8_t  channel, uint32_t rx_buf_size,
uint32_t tx_buf_size);
* comm_status_t  COMM_add_channel(uint8_t channel);
* comm_status_t  COMM_delete_channel(uint8_t channel);
* comm_status_t  COMM_abort_process(uint8_t channel);
* comm_status_t  COMM_send_buffer(uint8_t  channel,              char *buffer,
uint32_t  bytes_to_send, uint32_t  *bytes_sent,
comm_fx_tx_t              fx);

* comm_status_t  COMM_flush_rx_buffer(uint8_t channel);
* comm_status_t  COMM_flush_tx_buffer(uint8_t channel);
* comm_status_t  COMM_get_status(uint8_t channel, char *status_text);
* comm_status_t  COMM_clear_status(uint8_t channel);
* comm_error_t   COMM_get_last_error(uint8_t channel, char *error_text);
* comm_status_t  COMM_clear_last_error(uint8_t channel);
* comm_status_t  COMM_get_rts(uint8_t channel, comm_rts_control_t *rts);
* comm_status_t  COMM_set_rts(uint8_t channel, comm_rts_control_t rts);
* comm_status_t  COMM_get_dtr(uint8_t channel, comm_dtr_control_t *dtr);
* comm_status_t  COMM_set_dtr(uint8_t channel, comm_dtr_control_t dtr);
* comm_status_t  COMM_get_dsr(uint8_t channel, comm_dsr_control_t *dsr);
* comm_status_t  COMM_ctrl_port_open_port(uint8_t channel, ctrl_cfg_t *p_ctrl_cfg);
* comm_status_t  COMM_close_ctrl_port(uint8_t channel);
* comm_status_t  COMM_ctrl_port_get_rts(uint8_t channel, comm_rts_control_t *rts);
* comm_status_t  COMM_ctrl_port_set_rts(uint8_t channel, comm_rts_control_t rts);
* comm_status_t  COMM_ctrl_port_get_dtr(uint8_t channel, comm_dtr_control_t *dtr);
* comm_status_t  COMM_stop_AT_mode(uint8_t channel);
* comm_status_t  COMM_start_AT_mode(uint8_t channel, uint8_t *Rx_buffer);
*/

comm_receive_buffer_t COMM_receive_buffer = NULL;
/*****************************************************************************
*                       Local variables declaration                         *
*****************************************************************************/
/*static comm_manager_t  comm_manager;*/
static uint8_t                cm_nr_of_instances_running;
static comm_instance_t       *cm_comm_handler[MAX_NUMBER_OF_INSTANCES];
static uint8_t                cm_active_comm_handlers[MAX_NUMBER_OF_INSTANCES]; /* list of active channels */
static comm_manager_status_t  cm_status;


/*****************************************************************************
*                            IMPLEMENTATION                                 *
*****************************************************************************/

/******************************************************************************
* Function:... void COMM_init(void)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 13.02.05 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
void COMM_init(void)
{
  uint8_t i;

  for (i = 1; i < MAX_NUMBER_OF_INSTANCES; i++)
    cm_comm_handler[i] = NULL;

  cm_nr_of_instances_running = 0;

  cm_status.channel_id = COMM_MANAGER_ID;
  cm_status.code = comm_status_ready;

  // To get coverage for function not reachable in the BlackBoxTest
  COMM_get_last_error(0);
  COMM_error_report(0,0,0);
  COMM_err(0,0);
  COMM_close_physical_device_only(0);
  open_fw_download_port(0,"void",0);
}

/******************************************************************************
* Function:... void COMM_close(void)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 13.02.05 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
void COMM_close(void)
{
  uint8_t i;
  comport_handler_t *p_comport;

  for (i = 1; i < MAX_NUMBER_OF_INSTANCES; i++)
  {
    if (cm_comm_handler[i] != NULL)
    {
      if (cm_comm_handler[i]->comport != NULL)
      {
        p_comport = cm_comm_handler[i]->comport;
        (void)p_comport->close(p_comport->id);
        COMM_delete_channel(i);
      }
      MEM_free((uint8_t *)cm_comm_handler[i]);
      cm_comm_handler[i] = NULL;
    }
  }
  cm_nr_of_instances_running = 0;
}

/******************************************************************************
* Function:... void COMM_add_channel(uint8_t channel)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 13.02.05 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
comm_status_t COMM_add_channel(uint8_t channel)
{
  comm_status_t    comm_status;

  if ((0 < channel) && (channel < MAX_NUMBER_OF_INSTANCES))
  {
    if (cm_comm_handler[channel] != NULL)
    {
      /* The instance for the requested channel is already created */
      /* Check if it has been created by Open_port function. */
      if ((cm_comm_handler[channel]->channel_state == comm_channel_state_port_ready) || GLOBAL_usb_device) /* If delayed open just go on */
      {
        /* The instance was created by the Open_port function. */
        /* Enable the instance */
        cm_active_comm_handlers[cm_nr_of_instances_running] = channel; /* Add to list so Clock gets called with the instance */
        cm_nr_of_instances_running++;
        comm_status = comm_status_channel_added;
        cm_comm_handler[channel]->last_status = comm_status;
        cm_comm_handler[channel]->channel_state = comm_channel_state_enabled;
        cm_comm_handler[channel]->handler = COMM_handler;
      }
      else
      { /* Some error has occurred. the Open_port has not created this */
        /* instance */
        cm_status.channel_id = channel;
        cm_status.code = comm_status_channel_port_not_open;
        comm_status = cm_status.code;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
        COMM_err(channel, COMM_ERR_instance_not_created);
#endif
      }
    }
    else
    { /* The comport must be open before a channel can be added */
      cm_status.channel_id = channel;
      cm_status.code = comm_status_channel_port_not_open;
      comm_status = cm_status.code;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
      COMM_err(channel, COMM_ERR_comport_not_open);
#endif
    }
  }
  else
  { /* Invalid channel number */
    cm_status.channel_id = channel;
    cm_status.code = comm_status_channel_number_not_valid;
    comm_status = cm_status.code;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
    COMM_err(channel, COMM_ERR_instance_number_not_valid);
#endif
  }

  return comm_status;
}

/******************************************************************************
* Function:... void COMM_delete_channel(uint8_t channel)
* Parameters:. I :
* Returns:....b
* Description:
*
* Created:.... 12.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
comm_status_t COMM_delete_channel(uint8_t channel)
{
  comm_instance_t  *p_comm_ins;
  comm_status_t    comm_status;

  if ((0 < channel) && (channel < MAX_NUMBER_OF_INSTANCES))
  {
    if (cm_comm_handler[channel] == NULL)
    { /* The instance for the requested channel is already deleted*/
      cm_status.channel_id = channel;
      cm_status.code = comm_status_channel_deleted;
      comm_status = cm_status.code;
    }
    else
    { /* channel instance exist. Check the communication interface for
      the instance for deleting the instance */
      p_comm_ins = cm_comm_handler[channel];
      if (p_comm_ins && p_comm_ins->comport != NULL)
      {
        (void)p_comm_ins->comport->close(p_comm_ins->comport->id);
        p_comm_ins->comport = NULL;
      }
      /* Now delete the instance */
      MEM_free((uint8_t *)cm_comm_handler[channel]);
      cm_comm_handler[channel] = NULL;
      cm_status.channel_id = channel;
      cm_status.code = comm_status_channel_deleted;
      comm_status = cm_status.code;
      comm_rebuild_active_list();
    }
  }
  else
  { /* Invalid channel number */
    cm_status.channel_id = channel;
    cm_status.code = comm_status_channel_number_not_valid;
    comm_status = cm_status.code;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
    COMM_err(channel, COMM_ERR_instance_number_not_valid);
#endif
  }
  return comm_status;
}

/******************************************************************************
* Function:... void COMM_clock(void)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 12.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
void COMM_clock(void)
{
  int i; /* always use machine dependant "int" to yield max performance in loops (we don't need more than S8) */
  comm_instance_t *p;
  for (i = cm_nr_of_instances_running - 1; i >= 0; i--) /* Count down to yield faster compare with zero */
  {
    p = cm_comm_handler[cm_active_comm_handlers[i]];
    if (p && p->handler != NULL)
      p->handler((void*)p);  /* run the internal state machine */
  }
}

/******************************************************************************
* Function:... comm_status_t COMM_abort_process(uint8_t channel)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 12.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
comm_status_t COMM_abort_process(uint8_t channel)
{
  comm_instance_t  *p_comm_ins;
  comm_status_t    comm_status;

  if ((0 < channel) && (channel < MAX_NUMBER_OF_INSTANCES))
  {
    p_comm_ins = cm_comm_handler[channel];
    if (p_comm_ins != NULL)
    { /* The instance exist. Abort the instance */
      /* Abort any pending transfers over the comport */
      if (!GLOBAL_usb_device && p_comm_ins->comport)
      {
        (void)p_comm_ins->comport->flush_rx_buffer(p_comm_ins->comport->id);
        (void)p_comm_ins->comport->flush_tx_buffer(p_comm_ins->comport->id);
      }

      cm_status.channel_id = channel;
      cm_status.code = comm_status_ready;
      comm_status = cm_status.code;
      p_comm_ins->state = comm_state_rx_tx_ready;
      p_comm_ins->at_mode_state = comm_state_at_mode_disable;
      p_comm_ins->tx_buffer.buffer = NULL;
      p_comm_ins->tx_buffer.length = 0;
      p_comm_ins->tx_buffer.tx_length = 0;
      p_comm_ins->tx_buffer.length_txed = 0;
      p_comm_ins->tx_buffer.total_length = 0;
      p_comm_ins->tx_buffer.rest_length = 0;
      p_comm_ins->rx_buffer.buffer = NULL;
      p_comm_ins->rx_buffer.length = 0;
      p_comm_ins->rx_buffer.total_length = 0;
      p_comm_ins->rx_buffer.rx_length = 0;
      p_comm_ins->rx_buffer.length_rxed = 0;
      p_comm_ins->rx_buffer.rest_length = 0;
    }
    else
    {
      cm_status.channel_id = channel;
      cm_status.code = comm_status_channel_instance_not_exist;
      comm_status = cm_status.code;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
      COMM_err(channel, COMM_ERR_instance_not_created);
#endif

    }
  }
  else
  { /* Invalid channel number */
    cm_status.channel_id = channel;
    cm_status.code = comm_status_channel_number_not_valid;
    comm_status = cm_status.code;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
    COMM_err(channel, COMM_ERR_instance_number_not_valid);
#endif
  }
  return comm_status;
}

/******************************************************************************
* Function:... comm_status_t  COMM_open_port(uint8_t channel,
*                                            comm_cfg_t *p_comm_cfg)
*
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 14.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
comm_status_t  COMM_open_port(uint8_t channel, comm_cfg_t *p_comm_cfg)
{
  comm_instance_t  *p_comm_ins;
  comm_status_t    comm_status;

  if ((0 < channel) && (channel < MAX_NUMBER_OF_INSTANCES))
  {
    p_comm_ins = cm_comm_handler[channel];
    if (p_comm_ins != NULL && !GLOBAL_usb_device)
    {
      p_comm_ins->last_status = comm_status_channel_port_open_ok;
      comm_status = comm_status_channel_port_open_ok;
    }
    else
    {
      /* Create the instance and open the port */
      if(p_comm_ins == NULL)
      {
        p_comm_ins = (comm_instance_t *)MEM_malloc(sizeof(comm_instance_t));
        if(p_comm_ins)
        {
          cm_comm_handler[channel] = p_comm_ins;
          memset((void *)p_comm_ins, 0x00, sizeof(comm_instance_t));
        }
      }

      if (p_comm_ins == NULL)
      { /* Not enough memory to create a instance */
        cm_status.code = comm_status_no_resources_to_open_port;
        comm_status = cm_status.code;
      }
      else
      {
        /* Instance for the channel can now be initialzed and started */
        p_comm_ins->channel_id = channel;
        COMM_open_comport(channel, &p_comm_ins->comport, p_comm_cfg);
        if (p_comm_ins->comport != NULL)
        {
          comm_status = comm_status_channel_port_open_ok;
          if (GLOBAL_usb_device)
          {
            cm_comm_handler[channel]->channel_state = comm_channel_state_enabled;
          }
          else
          {
            cm_comm_handler[channel]->channel_state = comm_channel_state_port_ready;
          }

          p_comm_ins->handler = COMM_handler;
          p_comm_ins->state = comm_state_idle;
          p_comm_ins->at_mode_state = comm_state_at_mode_disable;
          p_comm_ins->debug_enabled = 0;
          p_comm_ins->done = 0;
          p_comm_ins->instance_unning = 1;
          p_comm_ins->rx_buffer.buffer = NULL;
          p_comm_ins->rx_buffer.total_length = 0;
          p_comm_ins->rx_buffer.rest_length = 0;
          p_comm_ins->rx_buffer.length_rxed = 0;
          p_comm_ins->rx_buffer.length = 0;
          p_comm_ins->rx_buffer.rx_length = 0;
          p_comm_ins->tx_buffer.buffer = NULL;
          p_comm_ins->tx_buffer.total_length = 0;
          p_comm_ins->tx_buffer.rest_length = 0;
          p_comm_ins->tx_buffer.tx_length = 0;
          p_comm_ins->tx_buffer.length = 0;
          p_comm_ins->last_error = comm_error_no_error;
          p_comm_ins->last_status = comm_status_ok;
          comm_status = p_comm_ins->last_status;
          /*

          */
          #ifdef _WIN32
            if (GLOBAL_usb_device)
              COMM_receive_buffer = COMM_usb_receive_buffer;
            else
              COMM_receive_buffer = COMM_com_receive_buffer;
          #else
            COMM_receive_buffer = COMM_usb_receive_buffer;
          #endif

        if (GLOBAL_use_GenericUSB)
        {
          Debug(" *** Setting default GenericUSB buffer size to %u *** \n", USB_RX_BUFFER_SIZE);
          COMM_set_comport_buffers(channel, USB_RX_BUFFER_SIZE, USB_TX_BUFFER_SIZE);
          cm_comm_handler[channel]->tx_buffer_size = USB_TX_BUFFER_SIZE;
          cm_comm_handler[channel]->rx_buffer_size = USB_TX_BUFFER_SIZE;
        }
        else if (GLOBAL_usb_device)
        {
          Debug(" *** Setting default USB buffer size to %u *** \n", COMPORT_TX_BUFFER_SIZE);
          COMM_set_comport_buffers(channel, COMPORT_RX_BUFFER_SIZE, COMPORT_TX_BUFFER_SIZE);
          cm_comm_handler[channel]->tx_buffer_size = COMPORT_TX_BUFFER_SIZE;
          cm_comm_handler[channel]->rx_buffer_size = COMPORT_TX_BUFFER_SIZE;
        }
        else if (GLOBAL_comm_technology[channel])
        {
          Debug(" *** Setting default MIPI buffer size to %u *** \n", MIPI_HEADER_SIZE);
          COMM_set_comport_buffers(channel, MIPI_HEADER_SIZE, MIPI_HEADER_SIZE);
          cm_comm_handler[channel]->tx_buffer_size = MIPI_HEADER_SIZE;
          cm_comm_handler[channel]->rx_buffer_size = MIPI_HEADER_SIZE;
        }
        else
        {
          Debug(" *** Setting default COMM buffer size to %u *** \n", COMPORT_TX_BUFFER_SIZE);
          COMM_set_comport_buffers(channel, COMPORT_RX_BUFFER_SIZE, COMPORT_TX_BUFFER_SIZE);
          cm_comm_handler[channel]->tx_buffer_size = COMPORT_TX_BUFFER_SIZE;
          cm_comm_handler[channel]->rx_buffer_size = COMPORT_RX_BUFFER_SIZE;
        }

        }
        else
        {
          if (GLOBAL_usb_device && !GLOBAL_usb_auto_mode)
          {
            /* return ok if not usb auto mode is enabled */
            comm_status = comm_status_channel_port_open_ok;
            cm_comm_handler[channel]->channel_state = comm_channel_state_enabled;
            cm_comm_handler[channel]->handler = COMM_handler;
            cm_status.channel_id = channel;
          }
          else
          {
            PrintError("COMM_open_comport failed\n");
            /* Error handled in the IFWD_comport.c module */
            /* return not ok if usb auto mode is enabled. */
            /* USB device should already be found by MMI before getting to this point. */
            MEM_free((uint8_t *)cm_comm_handler[channel]);
            cm_comm_handler[channel] = NULL;
            comm_status = comm_status_channel_open_port_error;
          }
        }
      }
    }
  }
  else
  { /* Invalid channel number */
    cm_status.channel_id = channel;
    cm_status.code = comm_status_channel_number_not_valid;
    comm_status = cm_status.code;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
    COMM_err(channel, COMM_ERR_instance_number_not_valid);
#endif
  }

  return comm_status;
}

/******************************************************************************
* Function:... comm_status_t  COMM_close_comport(uint8_t channel)
*
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 14.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
comm_status_t COMM_close_comport(uint8_t channel)
{
  comm_error_t     comm_error;
  comm_status_t    comm_status = comm_status_ok;
  uint8_t               result;
  comm_instance_t *p_comm_ins;

  if ((0 < channel) && (channel < MAX_NUMBER_OF_INSTANCES))
  {
    p_comm_ins = cm_comm_handler[channel];
    if (p_comm_ins != NULL)
    {
      if(p_comm_ins->comport)
        result = p_comm_ins->comport->close(channel);
      else
        result = 0xFF;
      if (result == 0x00)
      {
        /* Error managed from IFWD_comport.c */
        comm_status = comm_status_channel_port_close_error;
        comm_error = comm_error_channel_port_close_error;
        p_comm_ins->last_error = comm_error;
        COMM_error_report(channel, COMM_ERR_close_failed,__LINE__);

      }
      else
      {
        comm_status = comm_status_channel_port_close_ok;
      }
      /* Now delete the instance */
      MEM_free((uint8_t *)cm_comm_handler[channel]);
      cm_comm_handler[channel] = NULL;
      cm_status.channel_id = channel;
      /*      cm_status.code = comm_status_channel_port_close_ok;     by NNJK
      comm_status = cm_status.code;                                     */
      comm_rebuild_active_list();
    }
    else
    {
      comm_status = comm_status_channel_port_close_ok;
    }
    cm_status.channel_id = channel;
    cm_status.code= comm_status;
  }
  else
  { /* Invalid channel number */
    comm_status = comm_status_channel_number_not_valid;
    cm_status.channel_id = channel;
    cm_status.code= comm_status_channel_number_not_valid;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
    COMM_err(channel, COMM_ERR_instance_number_not_valid);
#endif
  }

  return comm_status;
}


/******************************************************************************
* Function:... comm_status_t  COMM_set_baudrate(uint8_t channel, uint32_t baudrate)
*
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 14.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
comm_status_t  COMM_set_baudrate(uint8_t channel, uint32_t baudrate)
{
  comm_status_t    comm_status = comm_status_ok;
  uint8_t          result;
  comm_instance_t *p_comm_ins;

  if ((0 < channel) && (channel < MAX_NUMBER_OF_INSTANCES))
  {
    p_comm_ins = cm_comm_handler[channel];
    if (p_comm_ins != NULL)
    {
      if ((p_comm_ins->channel_state == comm_channel_state_enabled) ||
        (p_comm_ins->channel_state == comm_channel_state_port_ready))
      {
        if(p_comm_ins->comport)
        {
          Debug(" Setting comport Baudrate to %d\n", baudrate);
          result = p_comm_ins->comport->set_baud(p_comm_ins->comport->id, baudrate);
        }
        else
        {
          result = 0xFF;
        }
        if (result != 0xFF)
        {
          /* Error managed in IFWD_comport.c */
          comm_status = comm_status_set_baudrate_error;
          p_comm_ins->state = comm_state_error;
        }
        else
        {
          comm_status = comm_status_set_baudrate_ok;
        }
      }
      else
      {
        comm_status = comm_status_channel_port_not_open;
        p_comm_ins->state = comm_state_error;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
        COMM_err(channel, COMM_ERR_baudrate_comport_not_ready);
#endif

      }
    }
    else
    {
      comm_status = comm_status_channel_instance_not_exist;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
      COMM_err(channel, COMM_ERR_instance_not_created);
#endif
    }
    cm_status.channel_id = channel;
    cm_status.code = comm_status;
  }
  else
  { /* Invalid channel number */
    comm_status = comm_status_channel_number_not_valid;
    cm_status.channel_id = channel;
    cm_status.code = comm_status;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
    COMM_err(channel, COMM_ERR_instance_number_not_valid);
#endif
  }

  return comm_status;
}

/******************************************************************************
* Function:... comm_status_t COMM_set_comport_buffers(uint8_t channel,
*                                                     uint32_t rx_buf_size,
*                                                     uint32_t tx_buf_size);
*
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 14.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
comm_status_t COMM_set_comport_buffers(uint8_t channel, uint32_t rx_buf_size, uint32_t tx_buf_size)
{
  comm_error_t     comm_error = comm_error_no_error;
  comm_status_t    comm_status = comm_status_ok;
  uint8_t          result;
  comm_instance_t *p_comm_ins;

  /* Virtual prototyping HACK - Avoiding I/O overflow */
#ifdef _WIN32
  GLOBAL_use_vp_tx_chunk_delay = GLOBAL_boot_timeout % 1000;
  GLOBAL_use_vp_tx_chunk_size  = GLOBAL_comm_timeout % 1000;

  if(GLOBAL_use_vp_tx_chunk_delay)
  {
    tx_buf_size = GLOBAL_use_vp_tx_chunk_size;
  }
#endif
  /* HACK - End */

  if ((0 < channel) && (channel < MAX_NUMBER_OF_INSTANCES))
  {
    p_comm_ins = cm_comm_handler[channel];
    if (p_comm_ins != NULL)
    {
      if ((p_comm_ins->channel_state == comm_channel_state_enabled) ||
        (p_comm_ins->channel_state == comm_channel_state_port_ready))
      {
        if(p_comm_ins->comport)
          result = p_comm_ins->comport->set_buffer_size(p_comm_ins->comport->id, rx_buf_size, tx_buf_size);
        else
          result = 0xFF;
        if (result != 0xFF)
        {
          /* Error managed in IFWD_comport.c */
          comm_status = comm_status_error_set_buffer_size;
          comm_error = comm_error_set_buffer_size;
          p_comm_ins->state = comm_state_error;
        }
        else
        {
          comm_status = comm_status_channel_added;
          p_comm_ins->rx_buffer_size = rx_buf_size;
          p_comm_ins->tx_buffer_size = tx_buf_size;
          if (p_comm_ins->channel_state == comm_channel_state_enabled)
          {
            p_comm_ins->state = comm_state_rx_tx_ready;
          }
        }
      }
      else
      {
        comm_status = comm_status_channel_port_not_open;
        p_comm_ins->state = comm_state_error;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
#if 0
        COMM_err(channel, COMM_ERR_set_buffer_size);
#endif
#endif

      }
      p_comm_ins->last_status = comm_status;
      p_comm_ins->last_error = comm_error;
    }
    else
    {
      comm_status = comm_status_channel_instance_not_exist;
      cm_status.channel_id = channel;
      cm_status.code = comm_status;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
      COMM_err(channel, COMM_ERR_instance_not_created);
#endif
    }

  }
  else
  { /* Invalid channel number */
    comm_status = comm_status_channel_number_not_valid;
    cm_status.channel_id = channel;
    cm_status.code = comm_status_channel_number_not_valid;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
    COMM_err(channel, COMM_ERR_instance_number_not_valid);
#endif
  }

  return comm_status;
}

/******************************************************************************
* Function:... comm_status_t COMM_send_buffer(uint8_t            channel,
*                                             char         *buffer,
*                                             uint32_t           bytes_to_send,
*                                             uint32_t          *bytes_sent,
*                                             comm_fx_tx_t  fx)
*
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 14.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
comm_status_t  COMM_send_buffer(uint8_t       channel,       char *buffer,
                                uint32_t      bytes_to_send, uint32_t  *bytes_sent,
                                comm_fx_tx_t  fx)
{
  comm_error_t     comm_error = comm_error_no_error;
  comm_status_t    comm_status = comm_status_ok;
  uint8_t          tx_status;
  comm_instance_t  *p_comm_ins;
  *bytes_sent = *bytes_sent;

  if ((0 < channel) && (channel < MAX_NUMBER_OF_INSTANCES))
  {
    p_comm_ins = cm_comm_handler[channel];
    if (p_comm_ins != NULL)
    {
      if (p_comm_ins->channel_state == comm_channel_state_enabled)
      {
        /* The instance exist and it is running. Start RX transaction if
        the state machine is in comm_state_rx_tx_ready */
        switch(p_comm_ins->state)
        {
        case comm_state_rx_tx_ready:
        case comm_state_rx_wait_block:
          /* Finally start the write operation */
          p_comm_ins->state = comm_state_tx;
          comm_status = comm_status_write_ongoing;
          p_comm_ins->tx_buffer.buffer = buffer;
          p_comm_ins->tx_buffer.total_length = bytes_to_send;
          p_comm_ins->tx_buffer.rest_length = bytes_to_send;
          p_comm_ins->tx_buffer.length_txed = 0;
          p_comm_ins->tx_buffer.length = 0;
          if (p_comm_ins->tx_buffer.total_length > p_comm_ins->tx_buffer_size)
          {
            p_comm_ins->tx_buffer.tx_length = p_comm_ins->tx_buffer_size;
            p_comm_ins->state = comm_state_tx_in_progress;
          }
          else
          {
            p_comm_ins->tx_buffer.tx_length = p_comm_ins->tx_buffer.total_length;
            p_comm_ins->state = comm_state_rx_tx_ready;
          }
          p_comm_ins->fx_tx = fx;
          p_comm_ins->state = comm_state_tx_start;
          // TODO: Remove either this timeout or the above (boot/comm)
          TIMER_new_timer_secs(&p_comm_ins->tx_timer, GLOBAL_comm_process_timeout);
          if(p_comm_ins->comport)
          {
            tx_status = p_comm_ins->comport->tx( p_comm_ins->comport->id,
                                                (uint8_t *)p_comm_ins->tx_buffer.buffer,
                                                p_comm_ins->tx_buffer.tx_length,
                                                &p_comm_ins->tx_buffer.length,
                                                COMM_tx_notification);
          }
          else
            tx_status = 0;
          if (tx_status == 0)
          { /* The transaction has been completed */
            p_comm_ins->tx_buffer.rest_length -= p_comm_ins->tx_buffer.length;
            if (p_comm_ins->state == comm_state_rx_tx_ready)
              comm_status = comm_status_write_finished;
            else
            {
              comm_status = comm_status_write_ongoing;
              p_comm_ins->state = comm_state_tx_in_progress;
            }
          }
          else if (tx_status == 2) // ERROR
          {
            comm_status = comm_status_write_error;
            comm_error = comm_error_buffers_no_config;
            p_comm_ins->state = comm_state_error;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
            COMM_error_report(channel, COMM_ERR_tx_transaction_error,__LINE__);
#endif
          }
          else
          {
            comm_status = comm_status_write_ongoing;
            p_comm_ins->state = comm_state_tx_in_progress;
          }
          break;
        case comm_state_tx_next_block:
          /* Transmit the next block */
          comm_status = comm_status_write_ongoing;
          if (p_comm_ins->tx_buffer.rest_length > p_comm_ins->tx_buffer_size)
          {
            p_comm_ins->tx_buffer.tx_length = p_comm_ins->tx_buffer_size;
            p_comm_ins->state = comm_state_tx_in_progress;
          }
          else
          {
            p_comm_ins->tx_buffer.tx_length = p_comm_ins->tx_buffer.rest_length;
            p_comm_ins->state = comm_state_rx_tx_ready;
          }
          p_comm_ins->tx_buffer.length_txed += p_comm_ins->tx_buffer.length;
          TIMER_new_timer_secs(&p_comm_ins->tx_timer, GLOBAL_comm_process_timeout);
          if(p_comm_ins->comport)
          {
            tx_status = p_comm_ins->comport->tx( p_comm_ins->comport->id,
                                                (uint8_t *)&p_comm_ins->tx_buffer.buffer[p_comm_ins->tx_buffer.length_txed],
                                                p_comm_ins->tx_buffer.tx_length,
                                                &p_comm_ins->tx_buffer.length,
                                                COMM_tx_notification);
          }
          else
            tx_status = 0;

          if (tx_status == 0)
          { /* The transaction has been completed */
            #ifndef _WIN32
              p_comm_ins->tx_buffer.rest_length -= p_comm_ins->tx_buffer.length;
            #else
              if (GLOBAL_use_GenericUSB)
                p_comm_ins->tx_buffer.rest_length -= p_comm_ins->tx_buffer.length;
            #endif
            if (p_comm_ins->state == comm_state_rx_tx_ready)
            {
              comm_status = comm_status_write_finished;
              p_comm_ins->state = comm_state_tx_completed;
            }
            else
            {
              comm_status = comm_status_write_ongoing;
              p_comm_ins->state = comm_state_tx_in_progress;
            }
          }
          else if (tx_status == 2) // ERROR
          {
            comm_status = comm_status_write_error;
            comm_error = comm_error_buffers_no_config;
            p_comm_ins->state = comm_state_error;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
            COMM_error_report(channel, COMM_ERR_tx_transaction_error,__LINE__);
#endif
          }
          else
          {
            comm_status = comm_status_write_ongoing;
            p_comm_ins->state = comm_state_tx_in_progress;
          }

          break;

        case comm_state_idle:
          comm_status = comm_status_write_error;
          comm_error = comm_error_buffers_no_config;
          p_comm_ins->state = comm_state_error;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
          COMM_err(channel, COMM_ERR_tx_rx_buffer_not_config);
#endif
          break;

        case comm_state_rx:
          comm_error = comm_error_read_already_ongoing;
          p_comm_ins->state = comm_state_error;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
          COMM_err(channel, COMM_ERR_tx_rx_transaction_ongoing);
#endif
          break;

        case comm_state_tx:
          comm_status = comm_status_write_error;
          comm_error = comm_error_write_already_ongoing;
          p_comm_ins->state = comm_state_error;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
          COMM_err(channel, COMM_ERR_tx_tx_transaction_ongoing);
#endif
          break;

        case comm_state_error:
          comm_status = comm_status_write_error;
          comm_error = comm_error_channel_in_error_state;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
          COMM_err(channel, COMM_ERR_tx_transaction_error);
#endif
          break;

        default:
          comm_status = comm_status_write_error;
          comm_error = comm_error_channel_in_unknown_state;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
#if 0
          COMM_err(channel, COMM_ERR_tx_transaction_error_unknown state);
#endif
#endif
        }
      }
      else
      {
        comm_status = comm_status_channel_port_not_open;
        p_comm_ins->state = comm_state_error;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
        COMM_err(channel, COMM_ERR_send_buffer);
#endif
      }
      p_comm_ins->last_status = comm_status;
      p_comm_ins->last_error = comm_error;
    }
    else
    {
      comm_status = comm_status_channel_instance_not_exist;
      cm_status.channel_id = channel;
      cm_status.code = comm_status;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
      COMM_err(channel, COMM_ERR_instance_not_created);
#endif
    }
  }
  else
  { /* Invalid channel number */
    comm_status = comm_status_channel_number_not_valid;
    cm_status.channel_id = channel;
    cm_status.code = comm_status;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
    COMM_err(channel, COMM_ERR_instance_number_not_valid);
#endif
  }

  return comm_status;
}

/******************************************************************************
* Function:... comm_status_t COMM_com_receive_buffer(uint8_t       channel,
*                                                    char         *buffer,
*                                                    uint32_t      bytes_to_read,
*                                                    uint32_t     *bytes_read,
*                                                    comm_fx_rx_t  fx)
*
* Parameters:. I :
* Returns:....
*  Description:
*
* Created:.... 14.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
#ifdef _WIN32
comm_status_t  COMM_com_receive_buffer(uint8_t channel,       char *buffer,
                                       uint32_t bytes_to_read, uint32_t *bytes_read,
                                       comm_fx_rx_t            fx)
{
  comm_error_t     comm_error = comm_error_no_error;
  comm_status_t    comm_status = comm_status_ok;
  comm_instance_t  *p_comm_ins;

  if ((0 < channel) && (channel < MAX_NUMBER_OF_INSTANCES))
  {
    p_comm_ins = cm_comm_handler[channel];
    if (p_comm_ins != NULL)
    {
      if (p_comm_ins->channel_state == comm_channel_state_enabled)
      {
        #ifdef STATE_TRACE
          static comm_state_t last_state;
          if (last_state != p_comm_ins->state)
          {
            Debug("COMM_usb_receive_buffer State Change: %d -> %d\n",
                  last_state, p_comm_ins->state);
            last_state = p_comm_ins->state;
          }
        #endif

        /* The instance exist and it is running. Start RX transaction if
        the state machine is in comm_state_rx_tx_ready */
        switch (p_comm_ins->state)
        {
        case comm_state_rx_tx_ready:
          /* Finally start the read operation */
          p_comm_ins->state = comm_state_rx;
          comm_status = comm_status_read_ongoing;
          p_comm_ins->rx_buffer.buffer = buffer;
          p_comm_ins->rx_buffer.total_length = bytes_to_read;
          p_comm_ins->rx_buffer.rest_length = bytes_to_read;
          p_comm_ins->rx_buffer.length_rxed = 0;
          p_comm_ins->rx_buffer.length = 0;
          p_comm_ins->fx_rx = fx;
          if (p_comm_ins->rx_buffer.total_length > p_comm_ins->rx_buffer_size)
          {
            p_comm_ins->rx_buffer.rx_length = p_comm_ins->rx_buffer_size;
            p_comm_ins->state = comm_state_rx_in_progress;
          }
          else
          {
            p_comm_ins->rx_buffer.rx_length = p_comm_ins->rx_buffer.total_length;
            p_comm_ins->state = comm_state_rx_tx_ready;
          }
          TIMER_new_timer_secs(&p_comm_ins->rx_timer, GLOBAL_comm_process_timeout);
          p_comm_ins->rx_buffer.length_rxed = p_comm_ins->comport->get_rx_used_buffer_size(p_comm_ins->comport->id);
          if (p_comm_ins->rx_buffer.rx_length <= p_comm_ins->rx_buffer.length_rxed)
          {
            comm_status = comm_status_read_ongoing;
            p_comm_ins->rx_buffer.length_rxed = 0;
            p_comm_ins->state = comm_state_rx_in_progress;
            p_comm_ins->rx_buffer.length = 0;
            (void)p_comm_ins->comport->rx( p_comm_ins->comport->id,
                                          (uint8_t **)&p_comm_ins->rx_buffer.buffer,
                                          p_comm_ins->rx_buffer.rx_length,
                                          &p_comm_ins->rx_buffer.length,
                                          COMM_rx_notification);
          }
          else
          {
            *bytes_read = 0;
            comm_status = comm_status_read_ongoing;
            p_comm_ins->state = comm_state_rx_wait_block;
      }
          break;

        case comm_state_rx_wait_block:
          p_comm_ins->rx_buffer.length_rxed = p_comm_ins->comport->get_rx_used_buffer_size(p_comm_ins->comport->id);
          if (p_comm_ins->rx_buffer.rx_length <= p_comm_ins->rx_buffer.length_rxed)
          {
            comm_status = comm_status_read_ongoing;
            p_comm_ins->rx_buffer.length_rxed = 0;
            p_comm_ins->state = comm_state_rx_in_progress;
            p_comm_ins->rx_buffer.length = 0;
            (void)p_comm_ins->comport->rx( p_comm_ins->comport->id,
                                          (uint8_t **)&p_comm_ins->rx_buffer.buffer,
                                          p_comm_ins->rx_buffer.total_length,
                                          &p_comm_ins->rx_buffer.length,
                                          COMM_rx_notification);
          }
          else
          {
            *bytes_read = 0;
            comm_status = comm_status_read_ongoing;
            p_comm_ins->state = comm_state_rx_wait_block;
          }
          break;

        case comm_state_rx_next_block:
          /* Receive the next block */
          comm_status = comm_status_read_ongoing;
          if (p_comm_ins->rx_buffer.rest_length > p_comm_ins->rx_buffer_size)
          {
            p_comm_ins->rx_buffer.rx_length = p_comm_ins->rx_buffer_size;
            p_comm_ins->state = comm_state_rx_in_progress;
          }
          else
          {
            p_comm_ins->rx_buffer.rx_length = p_comm_ins->rx_buffer.rest_length;
            p_comm_ins->state = comm_state_rx_tx_ready;
          }
          p_comm_ins->rx_buffer.length_rxed += p_comm_ins->rx_buffer.length;
          TIMER_new_timer_secs(&p_comm_ins->rx_timer, GLOBAL_comm_process_timeout);
          comm_status = comm_status_read_ongoing;
          p_comm_ins->state = comm_state_rx_in_progress;
          p_comm_ins->rx_buffer.length = 0;
          (void)p_comm_ins->comport->rx( p_comm_ins->comport->id,
                                      (uint8_t **)&p_comm_ins->rx_buffer.buffer,
                                      p_comm_ins->rx_buffer.total_length,
                                      &p_comm_ins->rx_buffer.length,
                                      COMM_rx_notification);
          break;

        case comm_state_idle:
          comm_status = comm_status_read_error;
          comm_error = comm_error_buffers_no_config;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
          COMM_err(channel, COMM_ERR_rx_tx_buffer_not_config);
#endif
          break;

        case comm_state_rx:
          comm_status = comm_status_read_error;
          comm_error = comm_error_read_already_ongoing;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
          COMM_err(channel, COMM_ERR_rx_rx_transaction_ongoing);
#endif
          break;

        case comm_state_tx:
        case comm_state_tx_in_progress:
        case comm_state_tx_next_block:
        case comm_state_tx_completed:
        case comm_state_tx_completed_delay:
          comm_status = comm_status_read_error;
          comm_error = comm_error_write_already_ongoing;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
          COMM_err(channel, COMM_ERR_rx_tx_transaction_ongoing);
#endif
          break;

        case comm_state_error:
          comm_status = comm_status_read_error;
          p_comm_ins->state = comm_state_error;
          comm_error = comm_error_channel_in_error_state;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
          COMM_err(channel, COMM_ERR_rx_transaction_error);
#endif
          break;

        default:
          comm_status = comm_status_read_error;
          p_comm_ins->state = comm_state_error;
          comm_error = comm_error_channel_in_unknown_state;
          break;
        }
      }
      else
      {
        comm_status = comm_status_channel_port_not_open;
        p_comm_ins->state = comm_state_error;
      }

      p_comm_ins->last_status = comm_status;
      p_comm_ins->last_error = comm_error;
    }
    else
    {
      comm_status = comm_status_channel_instance_not_exist;
      cm_status.channel_id = channel;
      cm_status.code = comm_status;
    }
  }
  else
  { /* Invalid channel number */
    comm_status = comm_status_channel_number_not_valid;
    cm_status.channel_id = channel;
    cm_status.code = comm_status;
  }
  return comm_status;
}
#endif

/******************************************************************************
* Function:... comm_status_t COMM_usb_receive_buffer(uint8_t       channel,
*                                                    char         *buffer,
*                                                    uint32_t      bytes_to_read,
*                                                    uint32_t     *bytes_read,
*                                                    comm_fx_rx_t  fx)
*
* Parameters:. I :
* Returns:....
*  Description:
*
* Created:.... 14.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
comm_status_t  COMM_usb_receive_buffer(uint8_t channel,       char *buffer,
                                       uint32_t bytes_to_read, uint32_t *bytes_read,
                                       comm_fx_rx_t            fx)
{
  comm_error_t     comm_error = comm_error_no_error;
  comm_status_t    comm_status = comm_status_ok;

  comm_instance_t  *p_comm_ins;

  if ((0 < channel) && (channel < MAX_NUMBER_OF_INSTANCES))
  {
    p_comm_ins = cm_comm_handler[channel];
    if (p_comm_ins != NULL)
    {
      if (p_comm_ins->channel_state == comm_channel_state_enabled)
      {
        #ifdef STATE_TRACE
          static comm_state_t last_state;
          if (last_state != p_comm_ins->state)
          {
            Debug2("COMM_usb_receive_buffer (state: %d)!", p_comm_ins->state);
            last_state = p_comm_ins->state;
          }
        #endif

        /* The instance exist and it is running. Start RX transaction if
        the state machine is in comm_state_rx_tx_ready */
        switch (p_comm_ins->state)
        {
        case comm_state_rx_tx_ready:
          /* Finally start the read operation */
          p_comm_ins->state = comm_state_rx;
          comm_status = comm_status_read_ongoing;
          p_comm_ins->rx_buffer.buffer = buffer;
          p_comm_ins->rx_buffer.total_length = bytes_to_read;
          p_comm_ins->rx_buffer.rest_length = bytes_to_read;
          p_comm_ins->rx_buffer.length_rxed = 0;
          p_comm_ins->fx_rx = fx;
          if (p_comm_ins->rx_buffer.total_length > p_comm_ins->rx_buffer_size)
          {
            p_comm_ins->rx_buffer.rx_length = p_comm_ins->rx_buffer_size;
            p_comm_ins->state = comm_state_rx_in_progress;
          }
          else
          {
            p_comm_ins->rx_buffer.rx_length = p_comm_ins->rx_buffer.total_length;
            p_comm_ins->state = comm_state_rx_tx_ready;
          }
          TIMER_new_timer_secs(&p_comm_ins->rx_timer, GLOBAL_comm_process_timeout);
          comm_status = comm_status_read_ongoing;
          p_comm_ins->state = comm_state_rx_in_progress;
          p_comm_ins->rx_buffer.length = 0;

          //p_comm_ins->rx_buffer.rx_length = p_comm_ins->rx_buffer.total_length;

          (void)p_comm_ins->comport->rx( p_comm_ins->comport->id,
                                        (uint8_t **)&p_comm_ins->rx_buffer.buffer,
                                        p_comm_ins->rx_buffer.rx_length,
                                        &p_comm_ins->rx_buffer.length,
                                        COMM_rx_notification);
          p_comm_ins->state = comm_state_rx_in_progress;
          break;

        case comm_state_rx_wait_block:
          if (p_comm_ins->rx_buffer.total_length > p_comm_ins->rx_buffer_size)
          {
            p_comm_ins->rx_buffer.rx_length = p_comm_ins->rx_buffer_size;
            p_comm_ins->state = comm_state_rx_in_progress;
          }
          else
          {
            p_comm_ins->rx_buffer.rx_length = p_comm_ins->rx_buffer.total_length;
            p_comm_ins->state = comm_state_rx_tx_ready;
          }
          if (p_comm_ins->rx_buffer.rx_length <= p_comm_ins->rx_buffer.length_rxed)
          {
            comm_status = comm_status_read_ongoing;
            p_comm_ins->state = comm_state_rx_in_progress;
            p_comm_ins->rx_buffer.length = 0;
            (void)p_comm_ins->comport->rx( p_comm_ins->comport->id,
                                          (uint8_t **)&p_comm_ins->rx_buffer.buffer,
                                          p_comm_ins->rx_buffer.rx_length,
                                          &p_comm_ins->rx_buffer.length,
                                          COMM_rx_notification);
          }
          else
          {
            *bytes_read = 0;
            comm_status = comm_status_read_ongoing;
            p_comm_ins->state = comm_state_rx_wait_block;
          }
          break;

        case comm_state_rx_next_block:
          /* Receive the next block */
          comm_status = comm_status_read_ongoing;
          if (p_comm_ins->rx_buffer.rest_length > p_comm_ins->rx_buffer_size)
          {
            p_comm_ins->rx_buffer.rx_length = p_comm_ins->rx_buffer_size;
            p_comm_ins->state = comm_state_rx_in_progress;
          }
          else
          {
            p_comm_ins->rx_buffer.rx_length = p_comm_ins->rx_buffer.rest_length;
            p_comm_ins->state = comm_state_rx_tx_ready;
          }
          p_comm_ins->rx_buffer.length_rxed += p_comm_ins->rx_buffer.length;
          TIMER_new_timer_secs(&p_comm_ins->rx_timer, GLOBAL_comm_process_timeout);
          comm_status = comm_status_read_ongoing;
          p_comm_ins->state = comm_state_rx_in_progress;
          p_comm_ins->rx_buffer.length = 0;
          (void)p_comm_ins->comport->rx( p_comm_ins->comport->id,
                                        (uint8_t **)&p_comm_ins->rx_buffer.buffer,
                                        p_comm_ins->rx_buffer.rx_length,
                                        &p_comm_ins->rx_buffer.length,
                                        COMM_rx_notification);
          break;

        case comm_state_idle:
          comm_status = comm_status_read_error;
          comm_error = comm_error_buffers_no_config;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
          COMM_err(channel, COMM_ERR_rx_tx_buffer_not_config);
#endif
          break;

        case comm_state_rx:
          comm_status = comm_status_read_error;
          comm_error = comm_error_read_already_ongoing;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
          COMM_err(channel, COMM_ERR_rx_rx_transaction_ongoing);
#endif
          break;

        case comm_state_tx:
        case comm_state_tx_in_progress:
        case comm_state_tx_next_block:
        case comm_state_tx_completed:
        case comm_state_tx_completed_delay:
          comm_status = comm_status_read_error;
          comm_error = comm_error_write_already_ongoing;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
          COMM_err(channel, COMM_ERR_rx_tx_transaction_ongoing);
#endif
          break;

        case comm_state_error:
          comm_status = comm_status_read_error;
          p_comm_ins->state = comm_state_error;
          comm_error = comm_error_channel_in_error_state;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
          COMM_err(channel, COMM_ERR_rx_transaction_error);
#endif
          break;

        default:
          comm_status = comm_status_read_error;
          p_comm_ins->state = comm_state_error;
          comm_error = comm_error_channel_in_unknown_state;
        }
      }
      else
      {
        comm_status = comm_status_channel_port_not_open;
        p_comm_ins->state = comm_state_error;
      }

      p_comm_ins->last_status = comm_status;
      p_comm_ins->last_error = comm_error;
    }
    else
    {
      comm_status = comm_status_channel_instance_not_exist;
      cm_status.channel_id = channel;
      cm_status.code = comm_status;
    }
  }
  else
  { /* Invalid channel number */
    comm_status = comm_status_channel_number_not_valid;
    cm_status.channel_id = channel;
    cm_status.code = comm_status;
  }
  return comm_status;
}

/******************************************************************************
* Function:... comm_status_t  COMM_flush_rx_buffer(uint8_t channel)
*
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 14.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
comm_status_t  COMM_flush_rx_buffer(uint8_t channel)
{
  comm_status_t    comm_status = comm_status_ok;
  comm_error_t     comm_error = comm_error_no_error;
  comm_instance_t *p_comm_ins;
  uint8_t          comport_result;

  if ((0 < channel) && (channel < MAX_NUMBER_OF_INSTANCES))
  {
    p_comm_ins = cm_comm_handler[channel];
    if (p_comm_ins != NULL)
    {
      if(p_comm_ins->comport)
        comport_result = p_comm_ins->comport->flush_rx_buffer(p_comm_ins->comport->id);
      else
        comport_result = 0xFF;
      if (comport_result == 0xFF)
      {
        comm_status = comm_status_flush_buffers_ok;
        p_comm_ins->state = comm_state_rx_tx_ready;
      }
      else
      {
        comm_status = comm_status_error_flush_buffers;
        comm_error = comm_error_flush_buffers_error;
      }

      p_comm_ins->last_status = comm_status;
      p_comm_ins->last_error = comm_error;
    }
    else
    {
      comm_status = comm_status_channel_instance_not_exist;
      cm_status.channel_id = channel;
      cm_status.code = comm_status;

    }
  }
  else
  { /* Invalid channel number */
    comm_status = comm_status_channel_number_not_valid;
    cm_status.channel_id = channel;
    cm_status.code = comm_status;
  }
  return comm_status;
}

/******************************************************************************
* Function:... comm_status_t  COMM_flush_tx_buffer(uint8_t channel)
*
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 14.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
comm_status_t  COMM_flush_tx_buffer(uint8_t channel)
{
  comm_status_t    comm_status = comm_status_ok;
  comm_error_t     comm_error = comm_error_no_error;
  comm_instance_t *p_comm_ins;
  uint8_t          comport_result;

  if ((0 < channel) && (channel < MAX_NUMBER_OF_INSTANCES))
  {
    p_comm_ins = cm_comm_handler[channel];
    if (p_comm_ins != NULL)
    {
      if(p_comm_ins->comport)
        comport_result = p_comm_ins->comport->flush_tx_buffer(p_comm_ins->comport->id);
      else
        comport_result = 0xFF;
      if (comport_result == 0xFF)
      {
        comm_status = comm_status_flush_buffers_ok;
        p_comm_ins->state = comm_state_rx_tx_ready;
      }
      else
      {
        comm_status = comm_status_error_flush_buffers;
        comm_error = comm_error_flush_buffers_error;
      }

      p_comm_ins->last_status = comm_status;
      p_comm_ins->last_error = comm_error;
    }
    else
    {
      comm_status = comm_status_channel_instance_not_exist;
      cm_status.channel_id = channel;
      cm_status.code = comm_status;

    }
  }
  else
  { /* Invalid channel number */
    comm_status = comm_status_channel_number_not_valid;
    cm_status.channel_id = channel;
    cm_status.code = comm_status;
  }
  return comm_status;
}


/******************************************************************************
* Function:... comm_status_t  COMM_set_rts(uint8_t channel, comm_rts_control_t rts)
*
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 14.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
comm_status_t  COMM_set_rts(uint8_t channel, comm_rts_control_t rts)
{
  comm_status_t     comm_status = comm_status_ok;
  comm_instance_t  *p_comm_ins;

  if ((0 < channel) && (channel < MAX_NUMBER_OF_INSTANCES))
  {
    p_comm_ins = cm_comm_handler[channel];
    if (p_comm_ins != NULL)
    {
      if(p_comm_ins->comport)
      {
        if (rts == comm_rts_control_enable)
        {
          (void)p_comm_ins->comport->set_rts(p_comm_ins->comport->id, 0xFF);
        }
        else
          if (rts == comm_rts_control_disable)
          {
            (void)p_comm_ins->comport->set_rts(p_comm_ins->comport->id, 0);
          }
          else
          {
            comm_status = comm_status_error;
            p_comm_ins->last_error = comm_error_rts_not_valid;
          }
      }
    }
    else
      comm_status = comm_status_channel_instance_not_exist;
  }
  else
  { /* Invalid channel number */
    comm_status = comm_status_channel_number_not_valid;
  }
  return comm_status;
}


/******************************************************************************
* Function:... comm_status_t  COMM_set_dtr(uint8_t channel, comm_dtr_control_t dtr)
*
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 14.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
comm_status_t  COMM_set_dtr(uint8_t channel, comm_dtr_control_t dtr)
{
  comm_status_t     comm_status = comm_status_ok;
  comm_instance_t  *p_comm_ins;

  if ((0 < channel) && (channel < MAX_NUMBER_OF_INSTANCES))
  {
    p_comm_ins = cm_comm_handler[channel];
    if (p_comm_ins && p_comm_ins->comport)
    {
      if (dtr == comm_dtr_control_enable)
        (void)p_comm_ins->comport->set_dtr(p_comm_ins->comport->id, 0xFF);
      else
        if (dtr == comm_dtr_control_disable)
          (void)p_comm_ins->comport->set_dtr(p_comm_ins->comport->id, 0);
        else
        {
          comm_status = comm_status_error;
          p_comm_ins->last_error = comm_error_dtr_not_valid;
        }
    }
    else
      comm_status = comm_status_channel_instance_not_exist;
  }
  else
  { /* Invalid channel number */
    comm_status = comm_status_channel_number_not_valid;
  }
  return comm_status;
}


/******************************************************************************
* Function:... comm_status_t  COMM_get_dsr(uint8_t channel, comm_dsr_control_t *dsr)
*
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 14.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
comm_status_t  COMM_get_dsr(uint8_t channel, comm_dsr_control_t *dsr)
{
  comm_status_t     comm_status = comm_status_ok;
  comm_instance_t  *p_comm_ins;

  if ((0 < channel) && (channel < MAX_NUMBER_OF_INSTANCES))
  {
    p_comm_ins = cm_comm_handler[channel];
    if (p_comm_ins && p_comm_ins->comport)
    {
      (void)p_comm_ins->comport->get_modem_status(p_comm_ins->comport->id,
                                                 &p_comm_ins->modem_status);
      if ((p_comm_ins->modem_status.comport_ms_dsr_on > 0))
        *dsr = comm_dsr_control_enable;
      else
        *dsr = comm_dsr_control_disable;
    }
    else
      comm_status = comm_status_channel_instance_not_exist;
  }
  else
  { /* Invalid channel number */
    comm_status = comm_status_channel_number_not_valid;
  }
  return comm_status;
}


/******************************************************************************
* Function:... comm_status_t COMM_get_status(uint8_t channel, char *status_text);
*
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 14.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
comm_status_t COMM_get_status(uint8_t channel, char *status_text)
{
  comm_status_t    comm_status = comm_status_ok;
  comm_instance_t  *p_comm_ins;

  PARAM_NOT_USED(status_text);     /* remove warning about unused variable */
  if ((0 < channel) && (channel < MAX_NUMBER_OF_INSTANCES))
  {
    p_comm_ins = cm_comm_handler[channel];
    if (p_comm_ins != NULL)
      comm_status = p_comm_ins->last_status;
    else
      comm_status = comm_status_channel_instance_not_exist;
  }
  else
  { /* Invalid channel number */
    comm_status = comm_status_channel_number_not_valid;
  }
  return comm_status;
}

/******************************************************************************
* Function:... comm_status_t COMM_get_last_error(uint8_t channel, char *error_text);
*
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 14.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
comm_error_t COMM_get_last_error(uint8_t channel)
{
  comm_error_t     comm_error = comm_error_no_error;
  comm_instance_t  *p_comm_ins;

  if ((0 < channel) && (channel < MAX_NUMBER_OF_INSTANCES))
  {
    p_comm_ins = cm_comm_handler[channel];
    if (p_comm_ins != NULL)
    {
      comm_error = p_comm_ins->last_error;
    }
    else
      comm_error = comm_error_channel_not_exist;
  }
  else
  { /* Invalid channel number */
    comm_error = comm_error_channel_number_not_valid;
  }
  return comm_error;
}


/*****************************************************************************
*                            Private functions                              *
*****************************************************************************/

/******************************************************************************
* Function:... void COMM_handler(comm_instance_t *p_comm_ins)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 14.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static void COMM_handler(comm_instance_t *p_comm_ins)
{
  if(p_comm_ins)  /* Make sure we don't access NULL pointers */
  {
    static comm_state_t last_state;
    if (last_state != p_comm_ins->state)
    {
      Debug3("COMM State Change: %d -> %d\n", last_state, p_comm_ins->state);
      last_state = p_comm_ins->state;
    }

    switch (p_comm_ins->state)
    {
    case comm_state_idle:
    case comm_state_rx:
    case comm_state_rx_tx_ready:
      break;

    case comm_state_rx_timeout:
      if(p_comm_ins->fx_rx)
      {
        p_comm_ins->fx_rx(p_comm_ins->channel_id,0);
        p_comm_ins->fx_rx = NULL;
      }
      Debug("Rx Timeout!!!!");
      p_comm_ins->state = comm_state_error;
      break;

    case comm_state_rx_in_progress:
      p_comm_ins->comport->process_comm(p_comm_ins->comport->id);
      COMM_rx_process(p_comm_ins);
      break;

    case comm_state_rx_wait_block:
      (void)COMM_receive_buffer(p_comm_ins->channel_id, p_comm_ins->rx_buffer.buffer,
                                p_comm_ins->rx_buffer.rx_length,
                                &p_comm_ins->rx_buffer.length,
                                p_comm_ins->fx_rx);
      break;

    case comm_state_rx_next_block:
      (void)COMM_receive_buffer(p_comm_ins->channel_id, p_comm_ins->rx_buffer.buffer,
                                p_comm_ins->rx_buffer.rx_length,
                                &p_comm_ins->rx_buffer.length,
                                p_comm_ins->fx_rx);

      break;

    case comm_state_rx_completed:
      COMM_rx_process(p_comm_ins);
      break;
    case comm_state_tx:
      break;
    case comm_state_tx_in_progress:
      if(p_comm_ins->comport)
      {
        /* In Linux the writing on a serial device is done in "one shot",
                  do not pass to the process_comm which is reserved for reading operation only.
                  In Emulation deferred writing operations are possible. */
        p_comm_ins->comport->process_comm(p_comm_ins->comport->id);
        COMM_tx_process(p_comm_ins);
      }
      break;

    case comm_state_tx_next_block:
      COMM_send_buffer(p_comm_ins->channel_id, p_comm_ins->tx_buffer.buffer,
        p_comm_ins->tx_buffer.tx_length,
        &p_comm_ins->tx_buffer.length,
        p_comm_ins->fx_tx);

      break;
    case comm_state_tx_completed_delay:
    {
      if (TIMER_timer_expired(&p_comm_ins->tx_timer_vp_delay))
      {
        p_comm_ins->state = comm_state_tx_completed;
      }
      break;
    }
    case comm_state_tx_completed:
      COMM_tx_process(p_comm_ins);
      break;
    case comm_state_tx_timeout:
      p_comm_ins->last_status = comm_status_write_timeout;
      p_comm_ins->last_error = comm_error_write_timeout;
      break;
    case comm_state_error:
      break;
    default:
      break;
    }
  }
}

/******************************************************************************
* Function:... void COMM_handler_at_mode(comm_instance_t *p_comm_ins)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 15.05.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static void COMM_handler_at_mode(comm_instance_t *p_comm_ins)
{
  uint8_t  rx_status;
  uint8_t  *buf;
  uint32_t bytesReceived;

  if(p_comm_ins)  /* Make sure we don't access NULL pointers */
  {
    static comm_state_t last_state;
    if (last_state != p_comm_ins->state)
    {
      Debug3("COMM AT State: %d -> %d\n", last_state, p_comm_ins->state);
      last_state = p_comm_ins->state;
    }

    COMM_rx_process_at_mode(NULL); // For coverage

    switch (p_comm_ins->state)
    {
    case comm_state_rx_tx_ready:
      p_comm_ins->rx_buffer.rx_length = p_comm_ins->comport->get_rx_used_buffer_size(p_comm_ins->comport->id);
      if(p_comm_ins->rx_buffer.rx_length)
      {
        do {
          // Keep receiving until there is new data with a max 100ms delay between bytes
          bytesReceived = p_comm_ins->rx_buffer.rx_length;
          Sleep(100);
          p_comm_ins->rx_buffer.rx_length = p_comm_ins->comport->get_rx_used_buffer_size(p_comm_ins->comport->id);
        } while (bytesReceived < p_comm_ins->rx_buffer.rx_length);

        buf = (uint8_t *)&p_comm_ins->rx_buffer.buffer[0];
        rx_status = p_comm_ins->comport->rx( p_comm_ins->comport->id,
                                            &buf,
                                            p_comm_ins->rx_buffer.rx_length,
                                            &p_comm_ins->rx_buffer.length,
                                            COMM_rx_not_at_mode);
        if (rx_status == 0)
        { /* The transaction has been completed */
          p_comm_ins->rx_buffer.total_length += p_comm_ins->rx_buffer.length;
          SFH_data_received((uint8_t)p_comm_ins->channel_id, (uint16_t)p_comm_ins->rx_buffer.length);
          p_comm_ins->last_status = comm_status_read_finished;
        }
        else
        {
          p_comm_ins->last_status = comm_status_read_ongoing;
          p_comm_ins->state = comm_state_rx_in_progress;
        }
      }
      break;

    case comm_state_tx:
    case comm_state_tx_at_mode:
      break;

    case comm_state_rx_in_progress:
      p_comm_ins->comport->process_comm(p_comm_ins->comport->id);
      COMM_rx_process_at_mode(p_comm_ins);
      break;

    case comm_state_tx_in_progress:
      p_comm_ins->comport->process_comm(p_comm_ins->comport->id);
      COMM_tx_process_at_mode(p_comm_ins);
      break;
    case comm_state_tx_next_block:
      COMM_send_buffer(p_comm_ins->channel_id, p_comm_ins->tx_buffer.buffer,
        p_comm_ins->tx_buffer.tx_length,
        &p_comm_ins->tx_buffer.length,
        p_comm_ins->fx_tx);

      break;
    case comm_state_tx_completed:
      COMM_tx_process(p_comm_ins);
      break;
    case comm_state_tx_timeout:
      p_comm_ins->last_status = comm_status_write_timeout;
      p_comm_ins->last_error = comm_error_write_timeout;
      break;
    case comm_state_error:
      break;
    default:
      break;
    }
  }
}

/******************************************************************************
* Function:... void COMM_tx_notification(void)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 15.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static void COMM_tx_notification(uint8_t channel, uint32_t bytes_txed)
{
  comm_instance_t *p_comm_ins;

  p_comm_ins = cm_comm_handler[channel];

  if (!p_comm_ins)
  {
    PrintError("NO COMM instance for channel %d!\n", channel);
    return;
  }

  /* The read operation has been completed. Get timeout status and read  */
  /* operation status before calling application callback function */
  if (TIMER_timer_expired(&p_comm_ins->tx_timer))
    p_comm_ins->state = comm_state_tx_timeout;
  else
  {
    if (bytes_txed)
    {
      p_comm_ins->tx_buffer.length = bytes_txed;

      if(GLOBAL_use_vp_tx_chunk_delay)
      {
        TIMER_new_timer(&p_comm_ins->tx_timer_vp_delay, GLOBAL_use_vp_tx_chunk_delay);

        p_comm_ins->state = comm_state_tx_completed_delay;
      }
      else
      {
        p_comm_ins->state = comm_state_tx_completed;
      }
    }
    else
    {
      // No bytes sent signals error
      p_comm_ins->state = comm_state_error;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
      COMM_error_report(channel, COMM_ERR_tx_transaction_error, __LINE__);
#endif
    }
  }
}

/******************************************************************************
* Function:... void COMM_tx_process(comm_instance_t *p_comm_ins)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 15.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static void COMM_tx_process(comm_instance_t *p_comm_ins)
{
  comm_status_t   comm_status;
  comm_error_t    comm_error = comm_error_no_error;
  comport_error_t comport_error;
  comport_status_t comport_status;


  if ((p_comm_ins->state == comm_state_tx_completed_delay) &&
      (!TIMER_timer_expired(&p_comm_ins->tx_timer_vp_delay)))
  {
    return;
  }
  comport_status = p_comm_ins->comport->get_status(p_comm_ins->comport->id);

  if (comport_status == comport_write_operation_ongoing)
  {
    return;
  }

  if (comport_status == comport_write_operation_finished)
  {
    comm_status = comm_status_write_finished;
    p_comm_ins->state = comm_state_rx_tx_ready;
    /* The p_comm_ins->tx_buffer.rest_length -= p_comm_ins->tx_buffer.length;
            seems to be a bug if the write operation is done in "one shot" as in Linux,
            since the tx_buffer.length has been already subtracted.
            In Emulation the write operation seems not to be performed
            at the first WriteFile call i.e. the subtraction is done here only.
            What about if the write operation is done at the first WriteFile call? */
    #ifndef _WIN32
      if(GLOBAL_UseCOMPortEmulation)
    #else
      if (!GLOBAL_use_GenericUSB)
    #endif
        p_comm_ins->tx_buffer.rest_length -= p_comm_ins->tx_buffer.length;

    if (p_comm_ins->tx_buffer.rest_length > 0)
    { /* Prepare to transmit a new block */
      p_comm_ins->state = comm_state_tx_next_block;
    }
    else
    {
      p_comm_ins->last_status = comm_status;
      p_comm_ins->last_error = comm_error;

      if ((p_comm_ins->fx_tx != NULL) && (p_comm_ins->at_mode_state == comm_state_at_mode_disable))
        p_comm_ins->fx_tx(p_comm_ins->channel_id);
    }
  }
  else
  {  /* An error has occurred in the read transaction */
    comm_status = comm_status_write_error;
    comport_error = p_comm_ins->comport->get_last_error(p_comm_ins->comport->id);

    switch (comport_error)
    {
    case comport_ce_break:
    case comport_ce_frame:
    case comport_ce_ioe:
    case comport_ce_mode:
    case comport_ce_overrun:
    case comport_ce_rxover:
    case comport_ce_rxparity:
    case comport_error_busy:
      comm_error = comm_error_write_error;
      break;

    case comport_error_invalid_handle:
    case comport_error_access_denied:
    case comport_error_device_not_ready:
    case comport_error_io_device:
    case comport_error_serial_no_device:
      comm_error = comm_error_serial_not_open;
      break;

    case comport_error_unkown:
      comm_error = comm_error_unkown;
      break;
    default:
      break;
    }

    p_comm_ins->last_status = comm_status;
    p_comm_ins->last_error = comm_error;
    p_comm_ins->state = comm_state_error;

    if ((p_comm_ins->fx_tx != NULL) && (p_comm_ins->at_mode_state == comm_state_at_mode_disable))
      p_comm_ins->fx_tx(p_comm_ins->channel_id);
  }
}

/******************************************************************************
* Function:... void COMM_tx_process_at_mode(comm_instance_t *p_comm_ins)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 15.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static void COMM_tx_process_at_mode(comm_instance_t *p_comm_ins)
{
  comm_status_t   comm_status;
  comm_error_t    comm_error = comm_error_no_error;
  comport_error_t comport_error;
  comport_status_t comport_status;

  if(!p_comm_ins || !p_comm_ins->comport)
    return;
  comport_status = p_comm_ins->comport->get_status(p_comm_ins->comport->id);

  if (comport_status == comport_write_operation_ongoing)
  {
    return;
  }

  if (comport_status == comport_write_operation_finished)
  {
    comm_status = comm_status_write_finished;
    p_comm_ins->state = comm_state_rx_tx_ready;
    /* The p_comm_ins->tx_buffer.rest_length -= p_comm_ins->tx_buffer.length;
            seems to be a bug if the write operation is done in "one shot" as in Linux,
            since the tx_buffer.length has been already subtracted.
            In Emulation the write operation seems not to be performed
            at the first WriteFile call i.e. the subtraction is done here only.
            What about if the write operation is done at the first WriteFile call? */
    #ifndef _WIN32
      if(GLOBAL_UseCOMPortEmulation)
    #else
      if (!GLOBAL_use_GenericUSB)
    #endif
          p_comm_ins->tx_buffer.rest_length -= p_comm_ins->tx_buffer.length;

    (void)p_comm_ins->comport->flush_rx_buffer(p_comm_ins->comport->id);
    if (p_comm_ins->tx_buffer.rest_length > 0)
    { /* Prepare to transmit a new block */
      p_comm_ins->state = comm_state_tx_next_block;
    }
    else
    {
      p_comm_ins->last_status = comm_status;
      p_comm_ins->last_error = comm_error;

      if ((p_comm_ins->fx_tx != NULL))
        p_comm_ins->fx_tx(p_comm_ins->channel_id);
    }
  }
  else
  {  /* An error has occurred in the read transaction */
    comm_status = comm_status_write_error;
    comport_error = p_comm_ins->comport->get_last_error(p_comm_ins->comport->id);

    switch (comport_error)
    {
    case comport_ce_break:
    case comport_ce_frame:
    case comport_ce_ioe:
    case comport_ce_mode:
    case comport_ce_overrun:
    case comport_ce_rxover:
    case comport_ce_rxparity:
    case comport_error_busy:
      comm_error = comm_error_write_error;
      break;

    case comport_error_invalid_handle:
    case comport_error_access_denied:
    case comport_error_device_not_ready:
    case comport_error_io_device:
    case comport_error_serial_no_device:
      comm_error = comm_error_serial_not_open;
      break;

    case comport_error_unkown:
      comm_error = comm_error_unkown;
      break;
    default:
      break;
    }

    p_comm_ins->last_status = comm_status;
    p_comm_ins->last_error = comm_error;
    p_comm_ins->state = comm_state_error;

    if (p_comm_ins->fx_tx != NULL)
      p_comm_ins->fx_tx(p_comm_ins->channel_id);
  }
}

/******************************************************************************
* Function:... void COMM_rx_notification(uint8_t channel, uint32_t length)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 15.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static void COMM_rx_notification(uint8_t channel, uint32_t length)
{
  comm_instance_t *p_comm_ins;

  p_comm_ins = cm_comm_handler[channel];
  /* The read operation has been completed. Get timeout status and read  */
  /* operation status before calling application callback function */
  if (TIMER_timer_expired(&p_comm_ins->rx_timer))
    p_comm_ins->state = comm_state_rx_timeout;
  else
  {
    p_comm_ins->rx_buffer.length = length;
    p_comm_ins->state = comm_state_rx_completed;
    if (!GLOBAL_usb_device && (GLOBAL_platform_group == GLOBAL_platform_group_sgold )) // The ekstra clock is only good for non-USB and SGold
    {
      COMM_clock(); // to allow the callback to the upper layers to take place also during boot rece-conditions.
    }
  }
}

/******************************************************************************
* Function:... void COMM_rx_not_at_mode(uint8_t channel)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 15.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static void COMM_rx_not_at_mode(uint8_t channel, uint32_t length)
{
  comm_instance_t *p_comm_ins;

  p_comm_ins = cm_comm_handler[channel];
  /* The read operation has been completed. Get timeout status and read  */
  /* operation status before calling application callback function */
  if (TIMER_timer_expired(&p_comm_ins->rx_timer))
    p_comm_ins->state = comm_state_rx_timeout;
  else
  {
    p_comm_ins->rx_buffer.length = length;
    p_comm_ins->state = comm_state_rx_completed;
  }
}

/******************************************************************************
* Function:... void COMM_rx_process(comm_instance_t *p_comm_ins)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 15.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static void COMM_rx_process(comm_instance_t *p_comm_ins)
{
  comm_status_t   comm_status = comm_status_ok;
  comm_error_t    comm_error = comm_error_no_error;
  comport_error_t comport_error;
  comport_status_t comport_status;

  /* The read operation has been completed. */
  comport_status = p_comm_ins->comport->get_status(p_comm_ins->comport->id);
  if ( comport_status == comport_read_operation_finished)
  {
    comm_status = comm_status_read_finished;
    p_comm_ins->state = comm_state_rx_tx_ready;

    // Note: I don't know where ".length" is set, but here I'm covering for a mistake...
    if (p_comm_ins->rx_buffer.rest_length > p_comm_ins->rx_buffer.length)
      p_comm_ins->rx_buffer.rest_length -= p_comm_ins->rx_buffer.length;
    else
      p_comm_ins->rx_buffer.rest_length = 0;

    if (p_comm_ins->rx_buffer.rest_length > 0)
    { /* Prepare to receive new block */
	    p_comm_ins->rx_buffer.buffer += p_comm_ins->rx_buffer.length;
      p_comm_ins->state = comm_state_rx_next_block;
    }
    else
    {
      p_comm_ins->last_status = comm_status;
      p_comm_ins->last_error = comm_error;
      p_comm_ins->rx_buffer.length_rxed += p_comm_ins->rx_buffer.length;
      if (p_comm_ins->fx_rx != NULL)
      {
        p_comm_ins->fx_rx(p_comm_ins->channel_id, p_comm_ins->rx_buffer.length_rxed);
        p_comm_ins->fx_rx = NULL;
      }
    }
  }
  else if (comport_status != comport_read_operation_ongoing)
  {  /* An error has occurred in the read transaction */
    comm_status = comm_status_read_error;
    comport_error = p_comm_ins->comport->get_last_error(p_comm_ins->comport->id);

    switch (comport_error)
    {
    case comport_ce_break:
    case comport_ce_frame:
    case comport_ce_ioe:
    case comport_ce_mode:
    case comport_ce_overrun:
    case comport_ce_rxover:
    case comport_ce_rxparity:
    case comport_error_busy:
      comm_error = comm_error_read_error;
      break;

    case comport_error_invalid_handle:
    case comport_error_access_denied:
    case comport_error_device_not_ready:
    case comport_error_io_device:
    case comport_error_serial_no_device:
      comm_error = comm_error_serial_not_open;
      break;

    case comport_error_unkown:
      comm_error = comm_error_unkown;
      break;
    default:
      break;
    }

    p_comm_ins->last_status = comm_status;
    p_comm_ins->last_error = comm_error;

    if (p_comm_ins->fx_rx != NULL)
    {
      p_comm_ins->fx_rx(p_comm_ins->channel_id, 0);
      p_comm_ins->fx_rx = NULL;
    }
    p_comm_ins->state = comm_state_error;
  }

  p_comm_ins->last_status = comm_status;
  p_comm_ins->last_error = comm_error;
}


/******************************************************************************
* Function:... void COMM_rx_process(comm_instance_t *p_comm_ins)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 15.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static void COMM_rx_process_at_mode(comm_instance_t *p_comm_ins)
{
  comm_status_t   comm_status;
  comm_error_t    comm_error = comm_error_no_error;
  comport_error_t comport_error;

  if (p_comm_ins == NULL)
	  return;

  /* The read operation has been completed. */
  if (p_comm_ins->comport->get_status(p_comm_ins->comport->id) == comport_read_operation_finished)
  {
    comm_status = comm_status_read_finished;
    p_comm_ins->state = comm_state_rx_tx_ready;
    p_comm_ins->rx_buffer.rest_length -= p_comm_ins->rx_buffer.length;
    p_comm_ins->last_status = comm_status;
    p_comm_ins->last_error = comm_error;
    p_comm_ins->rx_buffer.length_rxed += p_comm_ins->rx_buffer.length;
    SFH_data_received(p_comm_ins->channel_id, (uint16_t)p_comm_ins->rx_buffer.length_rxed);
  }
  else
  {  /* An error has occurred in the read transaction */
    comm_status = comm_status_read_error;
    comport_error = p_comm_ins->comport->get_last_error(p_comm_ins->comport->id);

    switch (comport_error)
    {
    case comport_ce_break:
    case comport_ce_frame:
    case comport_ce_ioe:
    case comport_ce_mode:
    case comport_ce_overrun:
    case comport_ce_rxover:
    case comport_ce_rxparity:
    case comport_error_busy:
      comm_error = comm_error_read_error;
      break;

    case comport_error_invalid_handle:
    case comport_error_access_denied:
    case comport_error_device_not_ready:
    case comport_error_io_device:
    case comport_error_serial_no_device:
      comm_error = comm_error_serial_not_open;
      break;

    case comport_error_unkown:
      comm_error = comm_error_unkown;
      break;

    default:
      break;
    }

    p_comm_ins->last_status = comm_status;
    p_comm_ins->last_error = comm_error;
    SFH_data_received(p_comm_ins->channel_id, 0);
    p_comm_ins->state = comm_state_error;
  }

  p_comm_ins->last_status = comm_status;
  p_comm_ins->last_error = comm_error;

}

/******************************************************************************
* Function:... void COMM_open_comport(uint8_t id, comport_handler_t *p_comport,
*                                     comm_cfg_t *p_comm_cfg);
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 15.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static void COMM_open_comport(uint8_t id, comport_handler_t **p_comport, comm_cfg_t *p_comm_cfg)
{
  comport_cfg_t comport_cfg;

  comport_cfg.id = id;
  comport_cfg.baudrate = p_comm_cfg->baudrate;
  comport_cfg.comport_name = (uint8_t *)p_comm_cfg->comport_name;
  comport_cfg.databits = 8;
  comport_cfg.parity = comport_no_parity;
  comport_cfg.stop_bits = comport_one_stop_bit;
  comport_cfg.rts_control = comport_rts_control_disable;
  comport_cfg.dtr_control = comport_dtr_control_disable;

  *p_comport = COMPORT_open(id, &comport_cfg, COMM_error_report);
}


/****************************************************************************
* Function:     comm_status_t  COMM_start_AT_mode(uint8_t channel,
*                                                 uint8_t *Rx_buffer);
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
comm_status_t  COMM_start_AT_mode(uint8_t channel, uint8_t *Rx_buffer)
{
  comm_status_t    comm_status = comm_status_ok;
  comm_instance_t  *p_comm_ins;

  if ((0 < channel) && (channel < MAX_NUMBER_OF_INSTANCES))
  {
    p_comm_ins = cm_comm_handler[channel];
    if (p_comm_ins != NULL)
    { /* set the COMM handler in "comm_state_at_mode" for the specified channel: */
      if(p_comm_ins->state == comm_state_rx_tx_ready)
      {
        p_comm_ins->at_mode_state = comm_state_at_mode_enable;
        p_comm_ins->handler = COMM_handler_at_mode;
        p_comm_ins->rx_buffer.buffer = (char*)Rx_buffer;
        p_comm_ins->rx_buffer.rx_length = 0;
        p_comm_ins->rx_buffer.length = 0;
        p_comm_ins->rx_buffer.total_length = 0;
      }
    }
    else
      comm_status = comm_status_channel_instance_not_exist;
  }
  else
  { /* Invalid channel number */
    comm_status = comm_status_channel_number_not_valid;
  }
  return comm_status;
}

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
comm_status_t  COMM_stop_AT_mode(uint8_t channel)
{
  comm_status_t    comm_status = comm_status_ok;
  comm_instance_t  *p_comm_ins;

  if ((0 < channel) && (channel < MAX_NUMBER_OF_INSTANCES))
  {
    p_comm_ins = cm_comm_handler[channel];
    if (p_comm_ins != NULL)
    {
      if(p_comm_ins->at_mode_state == comm_state_at_mode_enable) /* Are we in AT mode?*/
      {
        p_comm_ins->handler = COMM_handler;
        /* Re-init the instance before setting the instance in normal mode */
        COMM_abort_process(channel);
        p_comm_ins->state = comm_state_rx_tx_ready;
      }
    }
    else
      comm_status = comm_status_channel_instance_not_exist;
  }
  else
  { /* Invalid channel number */
    comm_status = comm_status_channel_number_not_valid;
  }
  return comm_status;
}

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
                                   comm_fx_tx_t  fx)
{
  comm_status_t    comm_status = comm_status_ok;
  comm_instance_t  *p_comm_ins;

  if ((0 < channel) && (channel < MAX_NUMBER_OF_INSTANCES))
  {
    p_comm_ins = cm_comm_handler[channel];
    if (p_comm_ins != NULL)
    { /* set the COMM handler in "comm_state_at_mode" for the specified channel: */
      p_comm_ins->rx_buffer.total_length = 0;
      comm_status = COMM_send_buffer(channel, data,
        data_length,
        &p_comm_ins->at_cmd_buf_len_txd,
        fx);
      p_comm_ins->state = comm_state_tx_in_progress;
    }
    else
      comm_status = comm_status_channel_instance_not_exist;
  }
  else
  { /* Invalid channel number */
    comm_status = comm_status_channel_number_not_valid;
  }
  return comm_status;
}

/******************************************************************************
* Function:... void comm_rebuild_active_list(void);
* Parameters:. :
* Returns:....
* Description: Rebuilds a list of active instances to avoid that COMM_Clock need to
*              run through all possible instances.
*
* Created:.... 2.03.06 by JMB
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
void comm_rebuild_active_list(void)
{
  int i;
  cm_nr_of_instances_running = 0;
  for (i = 1 ; i <= MAX_NUMBER_OF_CHANNELS ; i++)
  {
    if(cm_comm_handler[i])
    {
      cm_active_comm_handlers[cm_nr_of_instances_running] = i;
      cm_nr_of_instances_running++;
    }
  }
}

/******************************************************************************
* Function:... static void COMM_error_report(uint8_t comm_channel_id,
*                                            uint32_t error, uint32_t line)
*
* Parameters:. I :
* Returns:....
* Description: This function reports an error occurred in the comport interface
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
void COMM_error_report(uint8_t comm_channel_id, uint32_t error, uint32_t line)
{
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
  if ((0 < comm_channel_id) && (comm_channel_id < MAX_NUMBER_OF_INSTANCES))
  {
    SFH_set_error_string(comm_channel_id, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, COMM_ERR_comport_error, error, line));
    GLOBAL_callback(comm_channel_id, (IFWD_DL_status_enum)IFWD_DL_text_class_os_api_error_code , GLOBAL_text);
  }
#endif
}
/******************************************************************************/

#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
void COMM_err(uint8_t channel, uint32_t err)
{
  if ((0 < channel) && (channel < MAX_NUMBER_OF_INSTANCES))
  {
    SFH_set_error_string(channel, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, (uint16_t)err, channel));
  }
}
#endif

/******************************************************************************
* Function:... uint8_t  COMM_close_physical_device_only(uint8_t channel)
*
* Parameters:. I :
* Returns:....
* Description:
*
* Created:....
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
uint8_t COMM_close_physical_device_only(uint8_t channel)
{
  uint8_t          result = 0xFF;
  comm_instance_t *p_comm_ins;

  if ((0 < channel) && (channel < MAX_NUMBER_OF_INSTANCES))
  {
    p_comm_ins = cm_comm_handler[channel];
    if (p_comm_ins != NULL)
    {
      if(p_comm_ins->comport)
        result = p_comm_ins->comport->close(channel);
    }
  }
  else
  { /* Invalid channel number */
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
    COMM_err(channel, COMM_ERR_instance_number_not_valid);
#endif
  }
  return result;
}

/******************************************************************************
* Function:... open_fw_download_port(uint8_t channel, char *p_fw_download_port_name)
*
* Parameters:. I :
* Returns:....
* Description:
*
* Created:....
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
void open_fw_download_port(uint8_t channel, char *p_fw_download_port_name, uint32_t baud_rate)
{
  /* this part need to be adapted when SPI download will be implemented*/
  comm_instance_t  *p_comm_ins;
  comm_cfg_t comm_cfg;
  if ((0 < channel) && (channel < MAX_NUMBER_OF_INSTANCES))
  {
     p_comm_ins = cm_comm_handler[channel]; /* we use the same boot logical channel */

     strncpy(comm_cfg.comport_name, p_fw_download_port_name,MAX_COMPORT_NAME_LENGTH);
     comm_cfg.baudrate = baud_rate;
     COMM_open_comport(channel, &p_comm_ins->comport, &comm_cfg);
  }
}

