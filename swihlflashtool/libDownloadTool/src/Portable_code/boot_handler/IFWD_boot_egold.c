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
 *  $File name:  /msw_tools/FlashTool/libDownloadTool/src/Portable_code/boot_handler/IFWD_boot_egold.c $
 *  $CC-Version: .../dhkristx_140314_v0/1 $
 *  $Date:       2014-04-23    10:51:37 UTC $
 *   Responsible: kofod, Hilmar
 *   Comment:
 *     Add trace to the reading of the boot response.
 */

#define THIS_MODULE MODULE_BOOT

#include "OS.h"
#include "devices.h"
#include "DebugModule.h"
#include "IFWD_global_func.h"
#include "IFWD_timer.h"
#include "IFWD_mem.h"
#include "IFWD_boot.h"
#include "IFWD_boot_priv.h"
#include "IFWD_comm.h"

/*****************************************************************************
*                            CONSTANT DEFINITIONS                           *
*****************************************************************************/
#include "boot/psiload_elite.h"
#include "boot/bootload_evoice.h"
#include "boot/bootload_evoice_v2.h"
#include "boot/bootload_evoice_v3.h"
#include "ebl/flshload_evoice.h"
#include "ebl/flshload_evoice_v2.h"
#include "ebl/flshload_evoice_v3.h"
#include "boot/bootload_elite.h"
#include "ebl/flshload_elite.h"

/* NO SEC */
#define EGOLD_BOOT_ACK               0xA0

/* General */
#define EGOLD_BOOT_CODE              0x55

#define EGOLDVOICE_CHECKSUM_OK       0x01
#define EGOLD_CHECKSUM_OK            0xA5

#define EBL_VERSION_LENGTH             76  /* Length given in bytes */
#define EBL_LOADER_CHECKSUM_LENGTH      1
#define EBL_LOADER_SIGNATURE_LENGTH     1
#define EBL_CHECKSUM_LENGTH             1

#define NOF_KNOWN_TARGETS 3

typedef struct
{
  uint16_t snum5;
  uint16_t snum6;
  uint16_t snum7;
  char target_name[30];
} known_targets_t;

known_targets_t known_targets[NOF_KNOWN_TARGETS] = {{0x1802,0x0201,0x00A3,"B2"},
                                                    {0x3802,0x0209,0x00A7,"B3"},
                                                    {0x3802,0x0207,0x00A9,"F1.1"}};

/*****************************************************************************
*                                ENUMERATIONS                               *
*****************************************************************************/

/*****************************************************************************
*                            PUBLIC PROTOTYPES                              *
*****************************************************************************/
boot_status_t BOOT_egold(boot_instance_t *p_boot_ins, boot_manager_t *p_boot_mgn);
void BOOT_egold_handler(boot_instance_t *p_boot_ins);

/*****************************************************************************
*                             LOCAL PROTOTYPES                              *
*****************************************************************************/
static void BOOT_tx_boot_code(boot_instance_t *p_boot_ins);
static void BOOT_tx_boot_code_not(uint8_t channel);
static void BOOT_rx_boot_ack_not(uint8_t id, uint32_t size);
static void BOOT_rx_boot_ack_code(boot_instance_t *p_boot_ins);

static void BOOT_rx_ebl_loader_checksum_not(uint8_t id, uint32_t size);
static void BOOT_tx_ebl_length_not(uint8_t channel);
static void BOOT_tx_ebl_loader_length_not(uint8_t channel);
static void BOOT_tx_ebl_loader_payload_not(uint8_t channel);
static void BOOT_tx_ebl_loader_length(boot_instance_t *p_boot_ins);
static void BOOT_tx_ebl_loader_payload(boot_instance_t *p_boot_ins);
static void BOOT_tx_ebl_loader_checksum(boot_instance_t *p_boot_ins);
static void BOOT_tx_ebl_loader_checksum_not(uint8_t channel);
static void BOOT_rx_ebl_loader_checksum(boot_instance_t *p_boot_ins);
static void BOOT_tx_ebl(boot_instance_t *p_boot_ins);
static void BOOT_tx_ebl_payload_not(uint8_t channel);
static void BOOT_tx_ebl_payload(boot_instance_t *p_boot_ins);
static void BOOT_tx_ebl_checksum_not(uint8_t channel);
static void BOOT_tx_ebl_checksum(boot_instance_t *p_boot_ins);
static void BOOT_rx_ebl_checksum_not(uint8_t channel, uint32_t size);
static void BOOT_rx_ebl_checksum(boot_instance_t *p_boot_ins);
static void BOOT_rx_ebl_version(boot_instance_t *p_boot_ins);
static void BOOT_rx_ebl_version_not(uint8_t channel, uint32_t size);
static void BOOT_update_timeout_report(boot_instance_t *p_boot_ins);
static void BOOT_Generic_tx_callback_fx(uint8_t channel, boot_state_t next_state, uint16_t db_id);
static void BOOT_rx_boot_tlv1(boot_instance_t *p_boot_ins);
static void BOOT_rx_boot_tlv2(boot_instance_t *p_boot_ins);


/*****************************************************************************
*                       Local variables declaration                         *
*****************************************************************************/
extern boot_manager_t *p_boot_manager;
static uint8_t egold_checksum_ok;

/*****************************************************************************
*                            IMPLEMENTATION                                 *
*****************************************************************************/

/******************************************************************************
* Function:... boot_status_t BOOT_sgold(boot_instance_t *p_boot_ins)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
boot_status_t BOOT_egold(boot_instance_t *p_boot_ins, boot_manager_t *p_boot_mgn)
{
  boot_status_t boot_status = boot_status_ok;
  comm_status_t comm_status;
  boot_error_t  boot_error = boot_error_no_error;

  if ((p_boot_ins->p_dev_cfg->platform == E_GOLD_VOICE_V1) ||
    (p_boot_ins->p_dev_cfg->platform == X_GOLD_102) ||
    (p_boot_ins->p_dev_cfg->platform == X_GOLD_110))
  {
    egold_checksum_ok = EGOLDVOICE_CHECKSUM_OK;
  }
  else
  {
    egold_checksum_ok = EGOLD_CHECKSUM_OK;
  }

  p_boot_manager = p_boot_mgn;
  /* Clear buffer before boot start */
  comm_status = COMM_flush_rx_buffer(p_boot_ins->channel_id);
  if (comm_status == comm_status_flush_buffers_ok)
  { /* rx buffer has been flushed successful */
    comm_status = COMM_flush_tx_buffer(p_boot_ins->channel_id);
    if (comm_status == comm_status_flush_buffers_ok)
    { /* tx buffer has been flushed successful */
      /* TX 0x55 stream and start boot timer */
      BOOT_tx_boot_code(p_boot_ins);
      p_boot_ins->boot_time = GLOBAL_boot_timeout;
      TIMER_new_timer(&p_boot_ins->boot_timer, p_boot_ins->boot_time);
      p_boot_ins->waiting_for_tlv_response = false;
    }
  }
  else
  { /* Error flushing the internal rx buffer in uart HW */
    boot_status = boot_status_error;
    boot_error = boot_error_comm_buffer_flush_error;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
    BOOT_Err(p_boot_ins->channel_id, BOOT_ERR_error_comm_buffer_flush_error);
#endif
  }
  p_boot_ins->last_status = boot_status;
  p_boot_ins->last_error = boot_error;

  p_boot_ins->psi_buffer.buffer = NULL;  /* xxxxx */
  p_boot_ins->boot_buffer.buffer = NULL;
  p_boot_ins->boot_buffer.boot_buffer_allocated = 0;
  p_boot_ins->chip_return_code = 0;
  return boot_status;
}

/******************************************************************************
* Function:... void BOOT_handler(boot_instance_t *p_boot_ins)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
void BOOT_egold_handler(boot_instance_t *p_boot_ins)
{
  #ifdef STATE_TRACE
    static boot_state_t bootstate;
    if (bootstate != p_boot_ins->state) {
      Debug("Egold boot state: %d\n", bootstate);
      bootstate = p_boot_ins->state;
    }
  #endif

  switch (p_boot_ins->state)
  {
  case boot_state_idle:
    break;

  case boot_state_tx_boot_code:
    BOOT_tx_boot_code(p_boot_ins);
    break;

  case boot_state_rx_boot_ack:
#if 0
    if (TIMER_timer_expired(&p_boot_ins->tx_boot_code_timer))
#endif
      BOOT_rx_boot_ack_code(p_boot_ins);
    break;

  case boot_state_tx_waiting_boot_ack:
  case boot_state_rx_waiting_boot_ack:
    if (TIMER_timer_expired(&p_boot_ins->boot_timer))
    {
      p_boot_ins->state = boot_state_error;
      p_boot_ins->last_status = boot_status_boot_timeout;
      p_boot_ins->last_error = boot_error_RX_boot_ack_timeout;
    }
    else
      if (TIMER_timer_expired(&p_boot_ins->tx_boot_code_timer))
        p_boot_ins->state = boot_state_tx_boot_code;
    break;

  case boot_state_baud_set_1:
    TIMER_new_timer(&p_boot_ins->baud_set_delay_timer,5);
    p_boot_ins->state = boot_state_baud_set_2;
    break;

  case boot_state_baud_set_2:
    if (TIMER_timer_expired(&p_boot_ins->baud_set_delay_timer))
    {
      //p_boot_ins->EBL_load_baudrate = 460800;
      //Debug("Setting baudrate to %d", p_boot_ins->EBL_load_baudrate);
      COMM_set_baudrate(p_boot_ins->channel_id, p_boot_ins->EBL_load_baudrate);
      Sleep(20);
      if (p_boot_ins->waiting_for_tlv_response)
      {
        p_boot_ins->state = boot_state_rx_tlv1;
      }
      else
      {
        p_boot_ins->state = boot_state_tx_ebl;
      }
    }
    break;

  case boot_state_rx_tlv1:
    BOOT_rx_boot_tlv1(p_boot_ins);
    break;

  case boot_state_rx_tlv2:
    BOOT_rx_boot_tlv2(p_boot_ins);
    break;

  case boot_state_tx_ebl:
    BOOT_tx_ebl(p_boot_ins);
    break;

  case boot_state_tx_ebl_loader:
    BOOT_tx_ebl_loader_length(p_boot_ins);
    break;
  case boot_state_tx_ebl_loader_payload:
    BOOT_tx_ebl_loader_payload(p_boot_ins);
    break;

  case boot_state_tx_ebl_loader_checksum:
    BOOT_tx_ebl_loader_checksum(p_boot_ins);
    break;

  case boot_state_rx_ebl_loader_checksum:
    BOOT_rx_ebl_loader_checksum(p_boot_ins);
    break;

  case boot_state_tx_ebl_payload:
    BOOT_tx_ebl_payload(p_boot_ins);
    break;

  case boot_state_tx_ebl_checksum:
    BOOT_tx_ebl_checksum(p_boot_ins);
    break;

  case boot_state_rx_ebl_checksum:
    BOOT_rx_ebl_checksum(p_boot_ins);
    break;

  case boot_state_rx_ebl_version:
    BOOT_rx_ebl_version(p_boot_ins);
    break;

  case boot_state_ebl_running:
    p_boot_ins->status = boot_status_ebl_running;
    p_boot_ins->last_error = boot_error_no_error;
    p_boot_ins->state = boot_state_idle;
    if(p_boot_ins->boot_buffer.buffer != NULL && p_boot_ins->boot_buffer_allocated)
    {
      MEM_free(p_boot_ins->boot_buffer.buffer);
      p_boot_ins->boot_buffer.buffer = NULL;
      p_boot_ins->boot_buffer_allocated = false;
    }
    TIMER_stop_timer(&p_boot_ins->boot_timer);  /* Remember to stop the timer when done OK */
    if (p_boot_ins->fx != NULL)
      p_boot_ins->fx(p_boot_ins->channel_id, EBL_RUNNING, p_boot_ins->p_ebl_version);
    break;

  case boot_state_error:
    TIMER_stop_timer(&p_boot_ins->boot_timer);  /* Remember to stop the timer when an error is detected. */
    p_boot_ins->state = boot_state_error;
    if(p_boot_ins->boot_buffer.buffer != NULL && p_boot_ins->boot_buffer_allocated)
    {
      MEM_free(p_boot_ins->boot_buffer.buffer);
      p_boot_ins->boot_buffer.buffer = NULL;
      p_boot_ins->boot_buffer_allocated = false;
    }
    p_boot_ins->state = boot_state_idle;
    if (p_boot_ins->fx != NULL)
    {
      p_boot_ins->p_ebl_version = 0;
      p_boot_ins->fx(p_boot_ins->channel_id, EBL_NOT_RUNNING, p_boot_ins->p_ebl_version);
      p_boot_ins->state = boot_state_idle;
    }
    break;

  default:
    ;
  }
  /* Remember to check the running timers */
  if ((TIMER_timer_expired(&p_boot_ins->boot_timer)) &&
    ((p_boot_ins->state != boot_state_ebl_running) ||
    (p_boot_ins->state != boot_state_idle)))
  {
    p_boot_ins->state = boot_state_idle;
    if(p_boot_ins->boot_buffer.buffer != NULL && p_boot_ins->boot_buffer_allocated)
    {
      MEM_free(p_boot_ins->boot_buffer.buffer);
      p_boot_ins->boot_buffer.buffer = NULL;
      p_boot_ins->boot_buffer_allocated = false;
    }
    TIMER_stop_timer(&p_boot_ins->boot_timer);
    BOOT_update_timeout_report(p_boot_ins);
    if (p_boot_ins->fx != NULL)
    {
      p_boot_ins->p_ebl_version = 0;
      p_boot_ins->fx(p_boot_ins->channel_id, EBL_NOT_RUNNING, p_boot_ins->p_ebl_version);
      p_boot_ins->state = boot_state_idle;
    }
    return;
  }
}


/*****************************************************************************
*                            Private functions                              *
*****************************************************************************/

/******************************************************************************
* Function:... BOOT_tx_bootloader_notification(uint8_t id)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static void BOOT_tx_boot_code_not(uint8_t channel)
{
  boot_instance_t *p_boot_ins;
  comm_status_t    comm_status;
  char status[2];


  p_boot_ins = p_boot_manager->boot_handler[channel];

  comm_status = COMM_get_status(channel, status);
  if (comm_status != comm_status_write_finished)
    p_boot_ins->state = boot_state_tx_boot_code;
  else
    p_boot_ins->state = boot_state_rx_boot_ack;
}

/******************************************************************************
* Function:... void BOOT_rx_boot_ack_code(p_boot_ins)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static void BOOT_rx_boot_ack_code(boot_instance_t *p_boot_ins)
{
  if ((p_boot_ins->p_dev_cfg->platform == E_GOLD_VOICE_V1) ||
    (p_boot_ins->p_dev_cfg->platform == X_GOLD_102))
  {
    p_boot_ins->rx_buffer.max_length = 3;
  }
  else if (p_boot_ins->p_dev_cfg->platform == X_GOLD_110)
  {
    p_boot_ins->rx_buffer.max_length = 5;
  }
  else
  {
    p_boot_ins->rx_buffer.max_length = 1;
  }

  (void)COMM_receive_buffer(p_boot_ins->channel_id,
                      (char *)p_boot_ins->rx_buffer.buffer,
                      p_boot_ins->rx_buffer.max_length,
                      &p_boot_ins->rx_buffer.length,
                      BOOT_rx_boot_ack_not);
  p_boot_ins->state = boot_state_rx_waiting_boot_ack;
}


/******************************************************************************
* Function:... void BOOT_tx_ebl_loader_length(comm_instance_t *p_boot_ins)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static void BOOT_tx_ebl_loader_length(boot_instance_t *p_boot_ins)
{
  comm_status_t comm_status = comm_status_ok;
  uint32_t        i;
  uint8_t         found_target;
  uint8_t         exp_cust_cfg1;
  uint8_t         exp_cust_cfg2;
  uint8_t         exp_cust_cfg3;

  comm_status = COMM_flush_rx_buffer(p_boot_ins->channel_id);
  if (comm_status != comm_status_flush_buffers_ok)
  { /* Error flushing internal RX buffer in comport HW device */
    p_boot_ins->state = boot_state_error;
    p_boot_ins->last_error = boot_error_comm_buffer_flush_error;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
    BOOT_Err(p_boot_ins->channel_id, BOOT_ERR_error_comm_buffer_flush_error);
#endif
  }
  else
  {
#if 0
    if ((p_boot_ins->p_dev_cfg->platform == E_GOLD_VOICE_V1) ||
      (p_boot_ins->p_dev_cfg->platform == X_GOLD_102) ||
      (p_boot_ins->p_dev_cfg->platform == X_GOLD_110))
    {
      /* if target in secure mode, send PSI */
      if ((p_boot_ins->rx_buffer.buffer[2]&0xF0) == 0x10)
      {
        p_boot_ins->p_dev_cfg->psi_offset = (uint8_t *)&psiload_elite[0];   // enter psi pointer
        p_boot_ins->p_dev_cfg->psi_size = psiload_eliteLength;     // enter variable name of psi length
      }
      else
      {
        p_boot_ins->p_dev_cfg->psi_offset = NULL;
        p_boot_ins->p_dev_cfg->psi_size = 0;
      }
    }
#endif
    /* Transmit the embedded ebl loader or psi depending on the information */
    /* stored in dev_cfg */
    if ((p_boot_ins->p_dev_cfg->psi_offset == NULL) &&
      (p_boot_ins->p_dev_cfg->psi_size == 0))
    {  /* Transmit the embedded ebl */
      p_boot_ins->boot_buffer_allocated = false;
      if (p_boot_ins->p_dev_cfg->platform == E_GOLD_VOICE_V1)
      {
        p_boot_ins->boot_buffer.buffer = (uint8_t *)&bootload_evoice[2];
        p_boot_ins->boot_buffer.max_length = bootload_evoiceLength;
        p_boot_ins->boot_buffer.checksum = (uint8_t)bootload_evoice[bootload_evoiceLength+2];
      }
      else if (p_boot_ins->p_dev_cfg->platform == X_GOLD_102)
      {
        p_boot_ins->boot_buffer.buffer = (uint8_t *)&bootload_evoice_v2[2];
        p_boot_ins->boot_buffer.max_length = bootload_evoice_v2Length;
        p_boot_ins->boot_buffer.checksum = (uint8_t)bootload_evoice_v2[bootload_evoice_v2Length+2];
      }
      else if (p_boot_ins->p_dev_cfg->platform == X_GOLD_110)
      {
        p_boot_ins->boot_buffer.buffer = (uint8_t *)&bootload_evoice_v3[2];
        p_boot_ins->boot_buffer.max_length = bootload_evoice_v3Length;
        p_boot_ins->boot_buffer.checksum = (uint8_t)bootload_evoice_v3[bootload_evoice_v3Length+2];
      }
      else
      {

        p_boot_ins->boot_buffer.buffer = (uint8_t *)&bootload_elite[1];
        p_boot_ins->boot_buffer.max_length = bootload_eliteLength;
        p_boot_ins->boot_buffer.checksum = (uint8_t)bootload_elite[bootload_eliteLength+1];
      }
    }
    else
    { /* Transmit the injected psi */

#if defined (USE_TEXT_CLASS_IFWD_INFO)
      BOOT_info(p_boot_ins->channel_id, BOOT_INFO_inject_ebl_ldr);
#endif
      // HACK HACK
      //p_boot_ins->p_dev_cfg->psi_size -= 13;

      p_boot_ins->boot_buffer.buffer = p_boot_ins->p_dev_cfg->psi_offset;
      p_boot_ins->boot_buffer.max_length = p_boot_ins->p_dev_cfg->psi_size;
      p_boot_ins->boot_buffer.length = 0;
      p_boot_ins->boot_buffer.checksum = 0;
      if (p_boot_ins->p_dev_cfg->psi_offset != NULL)
      {
        for (i = 0; i < p_boot_ins->boot_buffer.max_length; i++)
          p_boot_ins->boot_buffer.checksum ^= p_boot_ins->boot_buffer.buffer[i];
      }
    }

    if ((p_boot_ins->p_dev_cfg->platform == E_GOLD_VOICE_V1) ||
      (p_boot_ins->p_dev_cfg->platform == X_GOLD_102) ||
      (p_boot_ins->p_dev_cfg->platform == X_GOLD_110))
    {

       /* Print out the target type */
      found_target = false;
      for (i=0; i<NOF_KNOWN_TARGETS; i++)
      {
        exp_cust_cfg1 = (uint8_t)((uint16_t)((known_targets[i].snum5 >> 7) & 0x0030) |
                             (uint16_t)((known_targets[i].snum7 >> 1) & 0x000F));

        exp_cust_cfg2 = (uint8_t)((uint16_t)((known_targets[i].snum7 >> 3) & 0x00F0) |
                             (uint16_t)((known_targets[i].snum7 >> 5) & 0x0003));

        exp_cust_cfg3 = (uint8_t)((uint16_t)((known_targets[i].snum6 >> 6) & 0x0001));

        if ((p_boot_ins->rx_buffer.buffer[2] == exp_cust_cfg1) &&
            (p_boot_ins->rx_buffer.buffer[3] == exp_cust_cfg2) &&
            (p_boot_ins->rx_buffer.buffer[4] == exp_cust_cfg3))
        {
          found_target = true;
          GLOBAL_callback(p_boot_ins->channel_id, IFWD_DL_ProcessOutlineUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, BOOT_INFO_TARGET, known_targets[i].target_name));
          break;
        }
      }
      if (!found_target)
      {
        GLOBAL_callback(p_boot_ins->channel_id, IFWD_DL_ProcessOutlineUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, BOOT_INFO_TARGET, "Unknown"));
      }

      /* Print out specific values */
      GLOBAL_callback(p_boot_ins->channel_id, IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, BOOT_INFO_SEC_LEVEL, (p_boot_ins->rx_buffer.buffer[2]>>4)));
      GLOBAL_callback(p_boot_ins->channel_id, IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, BOOT_INFO_CUST_ID, (p_boot_ins->rx_buffer.buffer[2]&0x0F)));
      GLOBAL_callback(p_boot_ins->channel_id, IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, BOOT_INFO_SEC_X, (p_boot_ins->rx_buffer.buffer[3]&0x0F)));
      GLOBAL_callback(p_boot_ins->channel_id, IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, BOOT_INFO_SEC_Y, (p_boot_ins->rx_buffer.buffer[3]>>4)));
      GLOBAL_callback(p_boot_ins->channel_id, IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, BOOT_INFO_ROW_BIT, (p_boot_ins->rx_buffer.buffer[4]&0x0F)));

      if ( ((((p_boot_ins->rx_buffer.buffer[2]&0xF0) == 0x10)
          && ((p_boot_ins->p_dev_cfg->platform == E_GOLD_VOICE_V1) || (p_boot_ins->p_dev_cfg->platform == X_GOLD_102)) )
          || (((p_boot_ins->rx_buffer.buffer[2]&0xF0) == 0x30)
          && (p_boot_ins->p_dev_cfg->platform == X_GOLD_110)) )
           && GLOBAL_check_sign_hw_cfg_value)
      {
        // In secure mode and EGV check the CUSTOMER_ID bits in the GPuK against the ones in the CUST_CFG byte
        uint8_t cid_target = p_boot_ins->rx_buffer.buffer[2]&0x0F;
        uint8_t cid_rampsi;
        uint32_t GPuK_offset;
        uint32_t SecY;
        uint32_t KeyLength;
        uint32_t Exponent;

        if (p_boot_ins->boot_buffer.buffer)
          GPuK_offset = *((uint32_t*)(&p_boot_ins->boot_buffer.buffer[20]));
        else
          GPuK_offset = 0;

        if (!GPuK_offset || GPuK_offset + 24 > p_boot_ins->boot_buffer.max_length)
        {
          // Error
          p_boot_ins->state = boot_state_error;
          //p_boot_ins->last_error = boot_error_gpuk_fail;
          BOOT_Err(p_boot_ins->channel_id, BOOT_ERR_gpuk_invalid);
          p_boot_ins->last_error = boot_error_gpuk_fail;
          return;
        }
        else
        {
          SecY      = *((uint32_t*)(&p_boot_ins->boot_buffer.buffer[(int)(GPuK_offset+12)]));
          KeyLength = *((uint32_t*)(&p_boot_ins->boot_buffer.buffer[(int)(GPuK_offset+16)]));
          Exponent  = *((uint32_t*)(&p_boot_ins->boot_buffer.buffer[(int)(GPuK_offset+20)]));
          if ((KeyLength != 0x400) || (Exponent != 3))
          {
            // Error
            p_boot_ins->state = boot_state_error;
            p_boot_ins->last_error = boot_error_gpuk_fail;
            BOOT_Err(p_boot_ins->channel_id, BOOT_ERR_gpuk_invalid);
            return;
          }
          else
          {
            cid_rampsi = (SecY >> 4)&0x0f;
            if (cid_rampsi != cid_target)
            {
              // Error
              p_boot_ins->state = boot_state_error;
              p_boot_ins->last_error = boot_error_gpuk_fail;
              BOOT_Err(p_boot_ins->channel_id, BOOT_ERR_gpuk_cust_id);
              return;
            }
          }
        }
      }



      /* Transmit ebl loader length */
      p_boot_ins->buffer[0] = 0x30;
      *(uint16_t*)&p_boot_ins->buffer[1] = *(uint16_t*)&p_boot_ins->boot_buffer.max_length;

      p_boot_ins->state = boot_state_idle;
      (void)COMM_send_buffer(p_boot_ins->channel_id,
                      (char*)p_boot_ins->buffer,
                      3,   // 1,
                      &p_boot_ins->boot_buffer.length,
                      BOOT_tx_ebl_loader_length_not);

      TR_BEGIN(TR_Tx,TR_boot_tx_load_length,p_boot_ins->channel_id);
      TR_8(p_boot_ins->buffer[0]);            /* uint8_t:  RAMload (hex) */
      TR_16P(&p_boot_ins->buffer[1]);         /* uint16_t: Length  (dec) */
      TR_END;
    }
    else
    {
      (void)COMM_send_buffer(p_boot_ins->channel_id,
                             (char *)&p_boot_ins->boot_buffer.max_length,
                             1,
                             &p_boot_ins->boot_buffer.length,
                             BOOT_tx_ebl_loader_length_not);

      TR_BEGIN(TR_Tx,TR_boot_tx_load_length2,p_boot_ins->channel_id);
      TR_8(p_boot_ins->boot_buffer.max_length);  /* uint8_t:  Length (dec) */
      TR_END;
    }
  }

}

/******************************************************************************
* Function:... void BOOT_rx_boot_ack_not(uint8_t id, char *buffer, uint32_t size)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static void BOOT_rx_boot_ack_not(uint8_t id, uint32_t size)
{
  boot_instance_t *p_boot_ins;
  boot_status_t    boot_status = boot_status_error;
  boot_error_t     boot_error = boot_error_comm_error_unknown;
  //  char i = 0;

  p_boot_ins = p_boot_manager->boot_handler[id];

  TR_BEGIN(TR_Rx,TR_boot_rx_ack_byte,p_boot_ins->channel_id);
  TR_DATA(p_boot_ins->rx_buffer.buffer, size);  /* ACK: uint8_t (hex) */
  TR_END;

  if ((p_boot_ins->p_dev_cfg->platform == E_GOLD_VOICE_V1) ||
    (p_boot_ins->p_dev_cfg->platform == X_GOLD_102) ||
    (p_boot_ins->p_dev_cfg->platform == X_GOLD_110))
  {
    if (((uint8_t)(p_boot_ins->rx_buffer.buffer[0]&0xFE) == 0x82) ||   // XG101 Eint2_t or Eint3_t only
      ((uint8_t)(p_boot_ins->rx_buffer.buffer[0]&0xF0) == 0x90) ||   // XG102 any ES
      ((uint8_t)(p_boot_ins->rx_buffer.buffer[0]&0xF0) == 0xA0))     // XG110 any ES
    {
      p_boot_ins->p_ebl_version->boot_cfg_value = p_boot_ins->rx_buffer.buffer[2];
      BOOT_show(p_boot_ins->channel_id, BOOT_INFO_target_sync);
      p_boot_ins->state = boot_state_tx_ebl_loader;
      boot_status = boot_status_booting_device;
      boot_error = boot_error_no_error;
    }
  }
  else
  {
    switch (p_boot_ins->rx_buffer.buffer[0])
    {
    case EGOLD_BOOT_ACK:
#if defined (USE_TEXT_CLASS_IFWD_INFO)
      BOOT_show(p_boot_ins->channel_id, BOOT_INFO_target_sync);
#endif
      if (p_boot_ins->fx != NULL)
        p_boot_ins->fx(p_boot_ins->channel_id, BOOT_SYNC, p_boot_ins->p_ebl_version);
      p_boot_ins->state = boot_state_tx_ebl_loader;
      boot_status = boot_status_booting_device;
      boot_error = boot_error_no_error;
      break;

    case 0:
      //         i = 0;

      break;

    default:
      p_boot_ins->state = boot_state_error;
      boot_status = boot_status_booting_device;
      boot_error = boot_error_wrong_ack_boot_code;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
      BOOT_Err(p_boot_ins->channel_id, BOOT_ERR_wrong_ack_boot_code);
#endif
      break;
    }
  }
  p_boot_ins->last_status = boot_status;
  p_boot_ins->last_error = boot_error;
}

/******************************************************************************
* Function:... void BOOT_tx_boot_code(boot_instance_t *p_boot_ins)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static void BOOT_tx_boot_code(boot_instance_t *p_boot_ins)
{
  comm_status_t comm_status = comm_status_ok;
  boot_status_t boot_status = boot_status_ok;
  char status_text[1];

  boot_error_t  boot_error = boot_error_no_error;
  comm_status = COMM_get_status(p_boot_ins->channel_id, status_text);

  if (comm_status == comm_status_write_ongoing)
    return;

  comm_status = COMM_flush_rx_buffer(p_boot_ins->channel_id);
  if (comm_status == comm_status_flush_buffers_ok)
  { /* rx buffer has been flushed successful */
    comm_status = COMM_flush_tx_buffer(p_boot_ins->channel_id);
    if (comm_status == comm_status_flush_buffers_ok)
    { /* tx buffer has been flushed successful */
      /* TX 0x55 stream and start boot timer */

      p_boot_ins->tx_boot_code_time = SEND_NEXT_BOOT_CODE_TIME;

      memset(p_boot_ins->buffer, EGOLD_BOOT_CODE, TRANSMIT_BOOT_BUFFER_LENGTH);
      p_boot_ins->tx_buffer.buffer = p_boot_ins->buffer;
      p_boot_ins->tx_buffer.max_length = MAX_EGOLD_BOOT_BUFFER_LENGTH;
      p_boot_ins->state = boot_state_tx_boot_code;
      TIMER_new_timer(&p_boot_ins->tx_boot_code_timer, p_boot_ins->tx_boot_code_time);

      (void)COMM_send_buffer(p_boot_ins->channel_id,
                            (char *)p_boot_ins->tx_buffer.buffer,
                            p_boot_ins->tx_buffer.max_length,
                            &p_boot_ins->tx_buffer.length,
                            BOOT_tx_boot_code_not);

      TR_BEGIN(TR_Tx,TR_boot_tx_at,p_boot_ins->channel_id);
      TR_DATA(p_boot_ins->tx_buffer.buffer, p_boot_ins->tx_buffer.max_length); /* Data: uint16_t */
      TR_END;

      if (p_boot_ins->fx != NULL)
        p_boot_ins->fx(p_boot_ins->channel_id, BOOT_START, p_boot_ins->p_ebl_version);
      p_boot_ins->state = boot_state_tx_waiting_boot_ack;
    }
    else
    { /* Error flushing the internal rx buffer in uart HW */
      boot_status = boot_status_error;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
      BOOT_Err(p_boot_ins->channel_id, BOOT_ERR_error_comm_buffer_flush_error);
#endif
    }
  }
  else
  { /* Error flushing the internal rx buffer in uart HW */
    boot_status = boot_status_error;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
    BOOT_Err(p_boot_ins->channel_id, BOOT_ERR_error_comm_buffer_flush_error);
#endif
  }

  p_boot_ins->last_status = boot_status;
  p_boot_ins->last_error = boot_error;
}

/******************************************************************************
* Function:... void BOOT_rx_ebl_loader_checksum_not
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static void BOOT_tx_ebl_load_baudrate_not(uint8_t channel)
{
  boot_instance_t  *p_boot_ins;
  p_boot_ins = p_boot_manager->boot_handler[channel];
  p_boot_ins->state = boot_state_baud_set_1;
}

static void BOOT_rx_ebl_read_tlv1st_not(uint8_t channel, uint32_t size)
{
  boot_instance_t  *p_boot_ins;
  PARAM_NOT_USED(size);  /* remove warning: size unused */

  if ((0 < channel) && (channel < MAX_NUMBER_OF_INSTANCES))
  {
    p_boot_ins = p_boot_manager->boot_handler[channel];
    if (p_boot_ins != NULL)
    {
      if (p_boot_ins->hack_hack_tlv_buffer)
      {
         TR_BEGIN(TR_Rx,TR_boot_rx_ebl_loader_tlv1,channel);
         TR_32(*((uint32_t*)&p_boot_ins->hack_hack_tlv_buffer[0]));
         TR_32(*((uint32_t*)&p_boot_ins->hack_hack_tlv_buffer[4]));
         TR_END;
         p_boot_ins->state = boot_state_rx_tlv2;
      }
    }
  }
}


#define SWAP(iN) (uint16_t)(((iN)>>8) | ((iN)<<8))
static void BOOT_rx_ebl_read_tlv2nd_not(uint8_t channel, uint32_t size)
{
  boot_instance_t  *p_boot_ins;
  uint32_t tlv_length;
  uint32_t i;
  uint8_t found_target;

  PARAM_NOT_USED(size);  /* remove warning: size unused */

  found_target = false;
  if ((0 < channel) && (channel < MAX_NUMBER_OF_INSTANCES))
  {
    p_boot_ins = p_boot_manager->boot_handler[channel];
    if (p_boot_ins != NULL)
    {
       tlv_length = *((uint32_t*)&p_boot_ins->hack_hack_tlv_buffer[4]);

       TR_BEGIN(TR_Rx,TR_boot_rx_ebl_loader_tlv2,channel);
       TR_DATA(&p_boot_ins->hack_hack_tlv_buffer[8], tlv_length);  /* ACK: uint8_t (hex) */
       TR_END;

       /* Printout the ext boot response */
       GLOBAL_callback(channel,IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, BOOT_INFO_PSI_version, &p_boot_ins->hack_hack_tlv_buffer[24],
                      *((uint32_t*)&p_boot_ins->hack_hack_tlv_buffer[16]),
                      *((uint32_t*)&p_boot_ins->hack_hack_tlv_buffer[20]) ));

       for (i=0; i<NOF_KNOWN_TARGETS; i++)
       {
         if ((((uint16_t)*((uint32_t*)&p_boot_ins->hack_hack_tlv_buffer[72])) == known_targets[i].snum5) &&
             (((uint16_t)*((uint32_t*)&p_boot_ins->hack_hack_tlv_buffer[76])) == known_targets[i].snum6) &&
             (((uint16_t)*((uint32_t*)&p_boot_ins->hack_hack_tlv_buffer[80])) == known_targets[i].snum7) )
         {
           found_target = true;
           GLOBAL_callback(channel,IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, BOOT_INFO_FUSING, known_targets[i].target_name));
           break;
         }
       }
       if (!found_target)
       {
         GLOBAL_callback(channel,IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, BOOT_INFO_FUSING, "Unknown"));
       }
       GLOBAL_callback(channel,IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, BOOT_INFO_Bootmode, *((uint32_t*)&p_boot_ins->hack_hack_tlv_buffer[12])));
       for (i=0; i<8; i++)
       {
         GLOBAL_callback(channel,IFWD_DL_ProcessDetailUpdated, 
                     GLOBAL_format_text(IFWD_DL_text_class_process_info, BOOT_INFO_SNUM, i,
                                        *((uint32_t*)&p_boot_ins->hack_hack_tlv_buffer[(uint32_t)(52+(i*4))])));
       }

       for (i=0; i<15; i++)
       {
         GLOBAL_callback(channel,IFWD_DL_ProcessDetailUpdated, 
                     GLOBAL_format_text(IFWD_DL_text_class_process_info, BOOT_INFO_HASH, i,
                                        *((uint32_t*)&p_boot_ins->hack_hack_tlv_buffer[(uint32_t)(88+(i*4))])));
       }

       p_boot_ins->state = boot_state_tx_ebl;
    }
  }
}

static void BOOT_rx_boot_tlv1(boot_instance_t *p_boot_ins)
{
  p_boot_ins->state = boot_state_idle;
  p_boot_ins->rx_buffer.max_length = 8;
  //bzero(p_boot_ins->hack_hack_tlv_buffer, sizeof(p_boot_ins->hack_hack_tlv_buffer));
  (void)COMM_receive_buffer(p_boot_ins->channel_id,
                           (char*)p_boot_ins->hack_hack_tlv_buffer,
                           p_boot_ins->rx_buffer.max_length,
                           &p_boot_ins->rx_buffer.length,
                           BOOT_rx_ebl_read_tlv1st_not);

  p_boot_ins->last_status = boot_status_booting_device;
  p_boot_ins->last_error = boot_error_no_error;

}

static void BOOT_rx_boot_tlv2(boot_instance_t *p_boot_ins)
{
  uint32_t tlv_length;

  p_boot_ins->state = boot_state_idle;
  tlv_length = *((uint32_t*)&p_boot_ins->hack_hack_tlv_buffer[4]);
  p_boot_ins->rx_buffer.max_length = tlv_length;

  (void)COMM_receive_buffer(p_boot_ins->channel_id,
                           (char*)&p_boot_ins->hack_hack_tlv_buffer[8],
                           p_boot_ins->rx_buffer.max_length,
                           &p_boot_ins->rx_buffer.length,
                           BOOT_rx_ebl_read_tlv2nd_not);

  p_boot_ins->last_status = boot_status_booting_device;
  p_boot_ins->last_error = boot_error_no_error;
}


static void BOOT_rx_ebl_loader_checksum_not(uint8_t id, uint32_t size)
{
  boot_status_t    boot_status = boot_status_ok;
  boot_error_t     boot_error = boot_error_no_error;
  boot_instance_t  *p_boot_ins;

  if ((0 < id) && (id < MAX_NUMBER_OF_INSTANCES))
  {
    p_boot_ins = p_boot_manager->boot_handler[id];
    if (p_boot_ins != NULL)
    {
      p_boot_ins->rx_buffer.length = size;

      TR_BEGIN(TR_Rx,TR_boot_rx_ebl_loader_checksum_result,p_boot_ins->channel_id);
      TR_DATA(p_boot_ins->rx_buffer.buffer, size);           /* Checksum (uint8_t) hex */
      TR_END;

      if (p_boot_ins->rx_buffer.buffer[0] == egold_checksum_ok)
      {
        if ((p_boot_ins->rx_buffer.buffer[1] == 0xAB) ||  // EBL-Loader can change to 921600
            (p_boot_ins->rx_buffer.buffer[1] == 0xA3))    // Signals that more PSI response data will follow.
        {

          // Change the baudrate here
          uint32_t baudrate = p_boot_ins->EBL_load_baudrate;
          uint16_t BG = 30;
          uint32_t ebl_load_baudrate_BG_FDV;

          if (p_boot_ins->rx_buffer.buffer[1] == 0xA3)    // Signals that more PSI response data will follow.
          {
            p_boot_ins->waiting_for_tlv_response = true;
          }

          do
          {
            BG--;
            ebl_load_baudrate_BG_FDV = (uint32_t)(0.5 + ((float)baudrate /(6348.0 / (BG+1.0))));
          } while (BG && (ebl_load_baudrate_BG_FDV > 511 ));
          if ((BG == 0) && (ebl_load_baudrate_BG_FDV > 511))
          {
            ebl_load_baudrate_BG_FDV = 511;
          }

          ebl_load_baudrate_BG_FDV |= (BG<<16);

          (void)COMM_send_buffer(p_boot_ins->channel_id,
                                (char *)&ebl_load_baudrate_BG_FDV,
                                4,
                                &p_boot_ins->boot_buffer.length,
                                BOOT_tx_ebl_load_baudrate_not);

          TR_BEGIN(TR_Tx,TR_boot_tx_baud,p_boot_ins->channel_id);
          TR_32(ebl_load_baudrate_BG_FDV);
          TR_END;

          p_boot_ins->state = boot_state_idle;
        }
        else
        {
          p_boot_ins->state = boot_state_tx_ebl;
        }

        boot_status = boot_status_booting_device;
        boot_error = boot_error_no_error;
      }
      else
      {
        p_boot_ins->state = boot_state_error;
        boot_status = boot_status_error;
        boot_error = boot_error_ebl_loader_checksum_error;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
        BOOT_Err(id, BOOT_ERR_ebl_loader_checksum_error);
#endif
      }

      p_boot_ins->last_status = boot_status;
      p_boot_ins->last_error = boot_error;
    }
    else
    {
      p_boot_manager->status.channel_id = id;
      p_boot_manager->status.code = boot_status_channel_not_added;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
      BOOT_Err(id, BOOT_ERR_channel_is_outside_range);
#endif
    }
  }
  else
  { /* Invalid channel number */
    p_boot_manager->status.channel_id = id;
    p_boot_manager->status.code = boot_status_channel_not_added;
  }

}

/******************************************************************************
* Function:... void BOOT_tx_ebl(boot_instance_t *p_boot_ins)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static void BOOT_tx_ebl(boot_instance_t *p_boot_ins)
{
  uint32_t i;

  /* Transmit the embedded ebl or the injected ebl depending on the */
  /* information stored in p_dev_cfg */
  if ((p_boot_ins->p_dev_cfg->ebl_offset == NULL) &&
    (p_boot_ins->p_dev_cfg->ebl_size == 0))
  { /* Load embedded ebl loader */
    if (p_boot_ins->p_dev_cfg->platform == E_GOLD_VOICE_V1)
    {
      p_boot_ins->boot_buffer.buffer = (uint8_t*)&flshload_evoice[2];
      p_boot_ins->boot_buffer.max_length = flshload_evoiceLength;
      p_boot_ins->boot_buffer.checksum = flshload_evoice[flshload_evoiceLength + 2];
    }
    else if ((p_boot_ins->p_dev_cfg->platform == X_GOLD_102))
    {
      p_boot_ins->boot_buffer.buffer = (uint8_t*)&flshload_evoice_v2[2];
      p_boot_ins->boot_buffer.max_length = flshload_evoice_v2Length;
      p_boot_ins->boot_buffer.checksum = flshload_evoice_v2[flshload_evoice_v2Length + 2];
    }
    else if ((p_boot_ins->p_dev_cfg->platform == X_GOLD_110))
    {
      p_boot_ins->boot_buffer.buffer = (uint8_t*)&flshload_evoice_v3[2];
      p_boot_ins->boot_buffer.max_length = flshload_evoice_v3Length;
      p_boot_ins->boot_buffer.checksum = flshload_evoice_v3[flshload_evoice_v3Length + 2];
    }
    else

    {
      p_boot_ins->boot_buffer.buffer = (uint8_t*)&flshload_elite[2];
      p_boot_ins->boot_buffer.max_length = flshload_eliteLength;
      p_boot_ins->boot_buffer.checksum = flshload_elite[flshload_eliteLength + 2];
    }

  }
  else
  { /* Load injected ebl */
#if defined (USE_TEXT_CLASS_IFWD_INFO)
    BOOT_info(p_boot_ins->channel_id, BOOT_INFO_inject_ebl);
#endif

    p_boot_ins->boot_buffer.buffer = p_boot_ins->p_dev_cfg->ebl_offset;
    p_boot_ins->boot_buffer.max_length = p_boot_ins->p_dev_cfg->ebl_size;
    p_boot_ins->boot_buffer.checksum = 0;
    if (p_boot_ins->p_dev_cfg->ebl_offset != NULL)
    {
      for (i = 0; i < p_boot_ins->boot_buffer.max_length; i++)
        p_boot_ins->boot_buffer.checksum ^= p_boot_ins->boot_buffer.buffer[i];
    }
  }

  //  if (p_boot_ins->p_dev_cfg->cfg->Platform == E_GOLD_VOICE_V1)
  //  {
  /* This is a HACK to handle variants of the loader without change in the protocol */
  /* The high byte of the handshake mode is XOR'ed to the real checksum */
  /* The target try to XOR with 3 or 4 in case of checksum error and the use CS_t or CS_t */
  /* in case the checksum then becomes OK */
  //    p_boot_ins->boot_buffer.checksum ^= (uint8_t)((p_boot_ins->p_dev_cfg->cfg->HandshakeMode)>>24);
  //  }

  /* Transmit ebl loader length */
  p_boot_ins->state = boot_state_tx_in_progress;
  (void)COMM_send_buffer(p_boot_ins->channel_id,
                        (char*)&p_boot_ins->boot_buffer.max_length,
                        2,
                        &p_boot_ins->boot_buffer.length,
                        BOOT_tx_ebl_length_not);

  TR_BEGIN(TR_Tx,TR_boot_tx_ebl_loader_length,p_boot_ins->channel_id);
  TR_16(p_boot_ins->boot_buffer.max_length /*p_boot_ins->boot_buffer.length*/);         /* Length (uint16_t) dec */
  TR_END;

  p_boot_ins->last_status = boot_status_booting_device;
  p_boot_ins->last_error = boot_error_no_error;
}

/******************************************************************************
* Function:... void BOOT_tx_ebl_payload(boot_instance_t *p_boot_ins)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static void BOOT_tx_ebl_payload(boot_instance_t *p_boot_ins)
{
  p_boot_ins->state = boot_state_tx_in_progress;
  (void)COMM_send_buffer(p_boot_ins->channel_id,
                         (char*)p_boot_ins->boot_buffer.buffer,
                         p_boot_ins->boot_buffer.max_length,
                         &p_boot_ins->boot_buffer.length,
                         BOOT_tx_ebl_payload_not);

  TR_BEGIN(TR_Tx,TR_boot_tx_ebl_payload_data,p_boot_ins->channel_id);
  TR_DATA(p_boot_ins->boot_buffer.buffer,p_boot_ins->boot_buffer.max_length);
  TR_END;

  p_boot_ins->last_status = boot_status_booting_device;
  p_boot_ins->last_error = boot_error_no_error;
}

/******************************************************************************
* Function:... void BOOT_tx_ebl_checksum(boot_instance_t *p_boot_ins)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static void BOOT_tx_ebl_checksum(boot_instance_t *p_boot_ins)
{
  (void)COMM_flush_rx_buffer(p_boot_ins->channel_id);
  (void)COMM_send_buffer(p_boot_ins->channel_id,
    (char*)&p_boot_ins->boot_buffer.checksum,
    1,
    &p_boot_ins->boot_buffer.length,
    BOOT_tx_ebl_checksum_not);

  TR_BEGIN(TR_Tx,TR_boot_tx_ebl_checksum,p_boot_ins->channel_id);
  TR_8(p_boot_ins->boot_buffer.checksum);
  TR_END;

  p_boot_ins->state = boot_state_tx_in_progress;
  p_boot_ins->last_status = boot_status_booting_device;
  p_boot_ins->last_error = boot_error_no_error;
}

/******************************************************************************
* Function:... void BOOT_rx_ebl_checksum(boot_instance_t *p_boot_ins)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static void BOOT_rx_ebl_checksum(boot_instance_t *p_boot_ins)
{

  p_boot_ins->rx_buffer.max_length = EBL_CHECKSUM_LENGTH;
  (void)COMM_receive_buffer(p_boot_ins->channel_id,
                            (char*)p_boot_ins->rx_buffer.buffer,
                            p_boot_ins->rx_buffer.max_length,
                            &p_boot_ins->rx_buffer.length,
                            BOOT_rx_ebl_checksum_not);

  p_boot_ins->state = boot_state_rx_in_progress;
  p_boot_ins->last_status = boot_status_booting_device;
  p_boot_ins->last_error = boot_error_no_error;
}

/******************************************************************************
* Function:... BOOT_rx_ebl_version(boot_instance_t *p_boot_ins)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static void BOOT_rx_ebl_version(boot_instance_t *p_boot_ins)
{
  p_boot_ins->rx_buffer.max_length = EBL_VERSION_LENGTH;
  if(p_boot_ins->p_ebl_version)
  {
    p_boot_ins->state = boot_state_rx_in_progress;
    (void)COMM_receive_buffer(p_boot_ins->channel_id,
                            (char*)p_boot_ins->p_ebl_version,
                            p_boot_ins->rx_buffer.max_length,
                            &p_boot_ins->rx_buffer.length,
                            BOOT_rx_ebl_version_not);
  }
//  p_boot_ins->last_status = boot_status_booting_device;
//  p_boot_ins->last_error = boot_error_no_error;
}


/******************************************************************************
* Function:... void BOOT_rx_ebl_version_not(uint8_t channel)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static void BOOT_rx_ebl_version_not(uint8_t channel, uint32_t size)
{
  boot_instance_t  *p_boot_ins;

  if ((0 < channel) && (channel < MAX_NUMBER_OF_INSTANCES))
  {
    p_boot_ins = p_boot_manager->boot_handler[channel];
    if (p_boot_ins != NULL)
    {
      p_boot_ins->state = boot_state_ebl_running;
      p_boot_ins->status = boot_status_ebl_running;
      p_boot_ins->last_error = boot_error_no_error;
      p_boot_ins->rx_buffer.length = size;

      TR_BEGIN(TR_Rx,TR_boot_rx_ebl_version,p_boot_ins->channel_id);
      if(p_boot_ins->p_ebl_version)
      {
        TR_DATA((char*)p_boot_ins->p_ebl_version, size);
      }
      else
      {
        char undef[]="undefined";
        TR_DATA(undef, 10);
      }
      TR_END;
    }
    else
    {
      p_boot_manager->status.channel_id = channel;
      p_boot_manager->status.code = boot_status_channel_not_added;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
      BOOT_Err(channel, BOOT_ERR_channel_is_outside_range);
#endif
    }
  }
  else
  { /* Invalid channel number */
    p_boot_manager->status.channel_id = channel;
    p_boot_manager->status.code = boot_status_channel_not_added;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
    BOOT_Err(channel, BOOT_ERR_channel_is_outside_range);
#endif
  }
}

/******************************************************************************
* Function:... void BOOT_tx_ebl_length_not(uint8_t channel)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static void BOOT_tx_ebl_length_not(uint8_t channel)
{
  BOOT_Generic_tx_callback_fx(channel, boot_state_tx_ebl_payload, db_BOOT_tx_ebl_length_not);
}

/******************************************************************************
* Function:... void BOOT_tx_ebl_payload_not(uint8_t channel)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static void BOOT_tx_ebl_payload_not(uint8_t channel)
{
  BOOT_Generic_tx_callback_fx(channel, boot_state_tx_ebl_checksum, db_BOOT_tx_ebl_payload_not);
}

/******************************************************************************
* Function:... void BOOT_tx_ebl_checksum_not(uint8_t channel)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static void BOOT_tx_ebl_checksum_not(uint8_t channel)
{
  BOOT_Generic_tx_callback_fx(channel, boot_state_rx_ebl_checksum, db_BOOT_tx_ebl_checksum_not);
}


/******************************************************************************
* Function:... void BOOT_rx_ebl_checksum_not(uint8_t channel)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static void BOOT_rx_ebl_checksum_not(uint8_t channel, uint32_t size)
{
  boot_instance_t  *p_boot_ins;

  if ((0 < channel) && (channel < MAX_NUMBER_OF_INSTANCES))
  {
    p_boot_ins = p_boot_manager->boot_handler[channel];
    if (p_boot_ins != NULL)
    {
      p_boot_ins->state = boot_state_rx_ebl_version;
      p_boot_ins->rx_buffer.length = size;

      TR_BEGIN(TR_Rx,TR_boot_rx_ebl_checksum_result,p_boot_ins->channel_id);
      TR_DATA(p_boot_ins->rx_buffer.buffer, size);
      TR_END;
    }
    else
    {
      p_boot_manager->status.channel_id = channel;
      p_boot_manager->status.code = boot_status_channel_not_added;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
      BOOT_Err(channel, BOOT_ERR_channel_is_outside_range);
#endif
    }
  }
  else
  { /* Invalid channel number */
    p_boot_manager->status.channel_id = channel;
    p_boot_manager->status.code = boot_status_channel_not_added;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
    BOOT_Err(channel, BOOT_ERR_channel_is_outside_range);
#endif
  }
}

/******************************************************************************
* Function:... void BOOT_tx_ebl_loader_length_not(uint8_t channel)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static void BOOT_tx_ebl_loader_length_not(uint8_t channel)
{
  BOOT_Generic_tx_callback_fx(channel,
    boot_state_tx_ebl_loader_payload,
    db_BOOT_tx_ebl_loader_length_not);
}

/******************************************************************************
* Function:... void BOOT_tx_ebl_loader_payload(uint8_t channel)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static void BOOT_tx_ebl_loader_payload(boot_instance_t *p_boot_ins)
{
   p_boot_ins->state = boot_state_tx_in_progress;
  (void)COMM_send_buffer(p_boot_ins->channel_id,
                        (char*)p_boot_ins->boot_buffer.buffer,
                        p_boot_ins->boot_buffer.max_length,
                        &p_boot_ins->boot_buffer.length,
                        BOOT_tx_ebl_loader_payload_not);

  TR_BEGIN(TR_Tx,TR_boot_tx_ebl_loader_payload_data,p_boot_ins->channel_id);
  TR_DATA(p_boot_ins->boot_buffer.buffer, p_boot_ins->boot_buffer.max_length);
  TR_END;

}

/******************************************************************************
* Function:... void BOOT_tx_ebl_loader_payload_not(uint8_t channel)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static void BOOT_tx_ebl_loader_payload_not(uint8_t channel)
{
  boot_instance_t  *p_boot_ins;

  if ((0 < channel) && (channel < MAX_NUMBER_OF_INSTANCES))
  {
    p_boot_ins = p_boot_manager->boot_handler[channel];
    if (p_boot_ins != NULL)
    {
      p_boot_ins->state = boot_state_tx_ebl_loader_checksum;
    }
    else
    {
      p_boot_manager->status.channel_id = channel;
      p_boot_manager->status.code = boot_status_channel_not_added;
    }
  }
  else
  { /* Invalid channel number */
    p_boot_manager->status.channel_id = channel;
    p_boot_manager->status.code = boot_status_channel_not_added;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
    BOOT_Err(channel, BOOT_ERR_channel_is_outside_range);
#endif
  }
}


/******************************************************************************
* Function:... void BOOT_tx_ebl_loader_checksum_not(uint8_t channel)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static void BOOT_tx_ebl_loader_checksum_not(uint8_t channel)
{
  BOOT_Generic_tx_callback_fx(channel,
    boot_state_rx_ebl_loader_checksum,
    db_BOOT_tx_ebl_loader_checksum_not);

}


/******************************************************************************
* Function:... void BOOT_tx_ebl_loader_checksum(boot_instance_t *p_boot_ins)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static void BOOT_tx_ebl_loader_checksum(boot_instance_t *p_boot_ins)
{
  boot_status_t boot_status;
  boot_error_t  boot_error;
  (void)COMM_flush_rx_buffer(p_boot_ins->channel_id);
  (void)COMM_send_buffer(p_boot_ins->channel_id,
                        (char*)&p_boot_ins->boot_buffer.checksum,
                        1,
                        &p_boot_ins->boot_buffer.length,
                        BOOT_tx_ebl_loader_checksum_not);

  TR_BEGIN(TR_Tx,TR_boot_tx_ebl_loader_checksum,p_boot_ins->channel_id);
  TR_8(p_boot_ins->boot_buffer.checksum); //(hex)
  TR_END;

  /* The transmission is ongoing */
  p_boot_ins->state = boot_state_tx_in_progress;
  boot_status = boot_status_booting_device;
  boot_error = boot_error_no_error;

  p_boot_ins->last_status = boot_status;
  p_boot_ins->last_error = boot_error;
}

/******************************************************************************
* Function:... void BOOT_rx_ebl_loader_checksum(boot_instance_t *p_boot_ins)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static void BOOT_rx_ebl_loader_checksum(boot_instance_t *p_boot_ins)
{

  p_boot_ins->rx_buffer.max_length = EBL_LOADER_CHECKSUM_LENGTH + EBL_LOADER_SIGNATURE_LENGTH;
  p_boot_ins->state = boot_state_rx_in_progress;
  (void)COMM_receive_buffer(p_boot_ins->channel_id,
                            (char*)p_boot_ins->rx_buffer.buffer,
                            p_boot_ins->rx_buffer.max_length,
                            &p_boot_ins->rx_buffer.length,
                            BOOT_rx_ebl_loader_checksum_not);

  p_boot_ins->last_status = boot_status_booting_device;
  p_boot_ins->last_error = boot_error_no_error;
}

/******************************************************************************
* Function:... void BOOT_Generic_tx_callback_fx(uint8_t channel, boot_state_t next_state, uint16_t db_id)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/

static void BOOT_Generic_tx_callback_fx(uint8_t channel, boot_state_t next_state, uint16_t db_id)
{
  boot_instance_t  *p_boot_ins;

  PARAM_NOT_USED(db_id);

  if ((0 < channel) && (channel < MAX_NUMBER_OF_INSTANCES))
  {
    p_boot_ins = p_boot_manager->boot_handler[channel];
    if (p_boot_ins != NULL)
    {
      p_boot_ins->state = next_state;
    }
    else
    {
      p_boot_manager->status.channel_id = channel;
      p_boot_manager->status.code = boot_status_channel_not_added;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
      BOOT_Err(channel, BOOT_ERR_channel_is_outside_range);
#endif
    }
  }
  else
  { /* Invalid channel number */
    p_boot_manager->status.channel_id = channel;
    p_boot_manager->status.code = boot_status_channel_not_added;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
    BOOT_Err(channel, BOOT_ERR_channel_is_outside_range);
#endif
  }
}

/******************************************************************************
* Function:... void BOOT_update_timeout_report(boot_instance_t *p_boot_ins)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static void BOOT_update_timeout_report(boot_instance_t *p_boot_ins)
{
  boot_error_t boot_error;

  p_boot_ins->last_status = boot_status_boot_timeout;


  switch (p_boot_ins->state)
  {
  case boot_state_init:
  case boot_state_idle:
    boot_error = boot_error_boot_not_started;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
    BOOT_Err(p_boot_ins->channel_id, BOOT_ERR_boot_not_started);
#endif
    break;

  case boot_state_tx_boot_code:
  case boot_state_tx_waiting_boot_ack:
    boot_error = boot_error_TX_boot_code;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
    BOOT_Err(p_boot_ins->channel_id, BOOT_ERR_TX_ack_boot_code);
#endif
    break;

  case boot_state_rx_boot_ack:
  case boot_state_rx_waiting_boot_ack:
    boot_error = boot_error_RX_boot_ack_timeout;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
    BOOT_Err(p_boot_ins->channel_id, BOOT_ERR_RX_boot_ack_timeout);
#endif
    break;

  case boot_state_rx_waiting_ebl_loader_chksum:
    boot_error = boot_error_ebl_loader_checksum_error;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
    BOOT_Err(p_boot_ins->channel_id, BOOT_ERR_ebl_loader_checksum_error);
#endif
    break;

  case boot_state_tx_ebl_loader:
  case boot_state_tx_ebl_loader_payload:
  case boot_state_tx_ebl_loader_length:
  case boot_state_tx_ebl_loader_checksum:
  case boot_state_rx_ebl_loader_checksum:
    boot_error = boot_error_ebl_loader_load_timeout;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
    BOOT_Err(p_boot_ins->channel_id, BOOT_ERR_ebl_loader_load_timeout);
#endif
    break;


  case boot_state_tx_ebl:
  case boot_state_tx_ebl_payload:
  case boot_state_tx_ebl_checksum:
  case boot_state_tx_waiting_ebl_chksum:
    boot_error = boot_error_ebl_load_timeout;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
    BOOT_Err(p_boot_ins->channel_id, BOOT_ERR_ebl_load_timeout);
#endif
    break;

  case boot_state_rx_ebl_version:
    boot_error = boot_error_rx_ebl_version;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
    BOOT_Err(p_boot_ins->channel_id, BOOT_ERR_ebl_version);
#endif
    break;
  case boot_state_error:
  case boot_state_rx_in_progress:
    {
      boot_error = boot_error_rx_timeout;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
      BOOT_Err(p_boot_ins->channel_id, BOOT_ERR_rx_timeout);
#endif
      break;
    }
  default:
    boot_error = boot_error_unknown;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
    BOOT_Err(p_boot_ins->channel_id, BOOT_ERR_unknown);
#endif
    break;


  }
  p_boot_ins->last_error = boot_error;
}

