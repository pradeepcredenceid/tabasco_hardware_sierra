/**
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
 *  $File name:  /msw_tools/FlashTool/libDownloadTool/src/Portable_code/boot_handler/IFWD_boot_sgold.c $
 *  $CC-Version: .../dhkristx_140314_v0/2 $
 *  $Date:       2014-04-23    10:51:39 UTC $
 *   Responsible: bentzen, Hilmar
 *   Comment:
 *     SMS00897775
 */

#define THIS_MODULE MODULE_BOOT


#include "OS.h"
#include "header.h"
#include "DebugModule.h"

#include <stdio.h>

#include "ebl/fastload.h"            /* SG1 Platform(s) */
#include "boot/bootload_sg2.h"       /* SG2 Platform.   */
#include "boot/bootload_sgr.h"       /* SGR Platform. ES1  */
#include "boot/bootload_sgr_es2.h"   /* SGR Platform. ES2  */
#include "boot/bootload_sg3.h"       /* SG3 Platform.  */
#include "boot/bootload_sgr_mpelite_nand.h"       /* MPELITE NAND platform */
#include "boot/bootload_sgr_es2_mpelite_nand.h"   /* MPELITE NAND platform */
#include "boot/bootload_xgold213.h"  /* XMM2130 Platform (a.k.a. ULC3) */
#include "boot/bootload_xg618.h"     /* XGOLD618 Platform.  */
#include "boot/bootload_xg618_es2.h" /* XGOLD618 ES2 Platform.  */

#include "ebl/ebl_sg2.h"             /* SG2 Platform(s)  */
#include "ebl/ebl_sg3.h"             /* SG3 Platform(s)  */
#include "ebl/ebl_sg3_nand.h"        /* SG3 Platform(s) NAND  */
#include "ebl/ebl_sgr.h"             /* SGR */
#include "ebl/ebl_xgold213.h"        /* XMM2130 Platform (a.k.a. ULC3) */
#include "ebl/ebl_xg618.h"           /* XGOLD618 Platform */
#include "ebl/ebl_xg618_es2.h"       /* XGOLD618 Platform */

#include "IFWD_timer.h"
#include "IFWD_mem.h"
#include "IFWD_global_func.h"
#include "IFWD_single_file_handler.h"  /* Because IFWD_boot.h does not have an BOOT_Err() with a string input */
#include "IFWD_comm.h"
#include "IFWD_boot.h"
#include "IFWD_boot_priv.h"
#include "devices.h"

#ifdef INCLUDE_DUT_USB_TEST
  #include "IFWD_dut_usb_test.h"

  static uint8_t IFWD_BOOT_dut_to_test_mode(void);
  static void BOOT_dut_process_test(boot_instance_t *p_ins);
#endif

#include "FlashLess.h"
#include "IFWD_sec_channel.h"
/*****************************************************************************
*                            CONSTANT DEFINITIONS                           *
*****************************************************************************/
#define BOOT_CHAR_SKIP                 0

/* NO SEC  */
#define BOOT_CHKSUM_OK_SG3_SGR       0x01
#define BOOT_CHKSUM_ERR_SG3          0xFF
#define BOOT_CHKSUM_OK_SG1           0xB1
#define BOOT_CHKSUM_OK_SG2           0xC1



#define XG626_MINOR_BC_VER_CODE_ES1_0  0x01
#define XG626_MINOR_BC_VER_CODE_ES1_1  0x02
#define XG626_MINOR_BC_VER_CODE_ES1_1A 0x03
#define XG626_MINOR_BC_VER_CODE_ES2_0  0x04

#define XG626_MINOR_BC_VER_SHIFT       27
#define XG626_MINOR_BC_VER_MASK        0x0F

#define BOOT_ACK_USB_NORMAL          0x0B
#define BOOT_ACK_USB_SG3_NORMAL_EXT_EBL  0x0D


#define BOOT_RAMLOAD_IND             0x30
#define CHECKSUM_ERROR               0x5A

/* General */
#define SGOLD_BOOT_CODE              "AT"
#define CHECKSUM_OK                  0xA5
#define CHECKSUM_ERROR               0x5A
#define BOOT_ACK_CODE_LENGTH            1
#define SG1_BOOT_ACK_CODE_LENGTH        1
#define SG2_BOOT_ACK_CODE_LENGTH        3
#define SG3_BOOT_ACK_CODE_LENGTH        3
#define XG213_BOOT_ACK_CODE_LENGTH     12
#define XG618_ES2_BOOT_ACK_CODE_LENGTH   22
#define LINK_EST_RESPONSE_V1_LENGTH      26 //
#define EBL_VERSION_LENGTH             76  /* Length given in bytes */
#define EBL_LOADER_CHECKSUM_LENGTH      1
#define X213_EBL_LOADER_CHECKSUM_LENGTH      2
#define XG618_ES2_EBL_LOADER_CHECKSUM_LENGTH     2
#define EBL_LOADER_SIGNATURE_LENGTH     1
#define EBL_CHECKSUM_LENGTH             2
#define EBL_READY_IND_LENGTH            2
#define EBL_READY_IND_PAYLOAD_LENGTH    2

// Defines for the readoud and test of the CID for RPSI and Target HW
#define RPSI_SECX_OFFSET   0x218
#define RPSI_SECX_LSHIFT   0x00
#define SECX_MASK          0x03

#define RPSI_SECY_OFFSET   0x21C
#define RPSI_SECY_LSHIFT   0x00
#define SECY_MASK          0x0F

#define RPSI_CUSCFG_OFFSET 0x21C
#define RPSI_CUSCFG_LSHIFT 0x04
#define CUSCFG_MASK        0x0F

#define SECLVL_LSHIFT      0x01
#define SECLVL_MASK        0x03

#define SECLVL_CERTIFICATE_MODE 0x01

#define CID_STR_LEN 0x04

// Bootrom command definitions
#define BOOT_PING_CMD    0x10
#define BOOT_PING_RSP    0x10
#define BOOT_BAUD_CMD    0x20
#define BOOT_SKIP_PSI_LD 0x40
#define BOOT_BAUD_RSP_OK   0xAA
#define BOOT_BAUD_RSP_FAIL 0xBB

#define BOOT_BAUD_115200 0x00
#define BOOT_BAUD_230400 0x01
#define BOOT_BAUD_460800 0x02
#define BOOT_BAUD_921600 0x03

#define BOOT_EXIT_STATUS 0x20
#define PSI_ACK_ENH      0xDD

/*****************************************************************************/
#define USB_TARGET_TEST_KEEP_CMD   0
#define USB_TARGET_IN_TEST         1
#define USB_TARGET_OUT_TEST        2
#define USB_TARGET_LOOPBACK_TEST   3
#define USB_TARGET_CHAPTER_9_TEST  4
/*****************************************************************************/


/*****************************************************************************
*                                ENUMERATIONS                               *
*****************************************************************************/
typedef enum boot_code_ack_type_s
{
  bootcore_ack = 0,
  usb_ack,
  invalid_ack = 0xFFFF
} boot_code_ack_type_t;

typedef union
{
  uint32_t dword;
  uint8_t  bytes[4];
} swap_type;

/*****************************************************************************
*                            PUBLIC PROTOTYPES                              *
*****************************************************************************/
boot_status_t BOOT_sgold(boot_instance_t *p_boot_ins, boot_manager_t *p_boot_mgn);
void BOOT_sgold_handler(boot_instance_t *p_boot_ins);

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
static void BOOT_rx_ebl_payload_ind(boot_instance_t *p_boot_ins);
static void BOOT_psi_error(uint8_t channel, uint32_t size);
static void BOOT_rx_ebl_payload_ind_not(uint8_t channel, uint32_t size);
static void BOOT_rx_ebl_ready_ind(boot_instance_t *p_boot_ins);
static void BOOT_rx_ebl_ready_ind_not(uint8_t channel, uint32_t size);
static void BOOT_rx_boot_ack_code_2nd_stage_618_ES2(boot_instance_t *p_boot_ins);
static void BOOT_rpsi_baud_rx_not(uint8_t channel, uint32_t size);
static void BOOT_rpsi_baud_tx_not(uint8_t channel);
static void BOOT_rpsi_send_ebl_not(uint8_t channel);

static void BOOT_rpsi_comm_rx_not(uint8_t channel, uint32_t size);
static void BOOT_rpsi_comm_tx_not(uint8_t channel);

static void BOOT_tx_ping_not(uint8_t channel);
static void BOOT_rx_ping_not(uint8_t channel, uint32_t size);
static void BOOT_tx_rom_baud_not(uint8_t channel);
static void BOOT_rx_rom_baud_not(uint8_t channel, uint32_t size);

static void BOOT_tx_rom_skip_psi_not(uint8_t channel);
static void BOOT_rx_rom_skip_psi_not(uint8_t channel, uint32_t size);
static void BOOT_rx_psi_ack_not(uint8_t channel, uint32_t size);
static void BOOT_tx_rpsi_val_sys_not(uint8_t channel);
static void BOOT_rx_rpsi_val_sys_not(uint8_t channel, uint32_t size);
static void BOOT_tx_psi_start_sys_not(uint8_t channel);
static void act_on_boot_response(uint8_t id);

static void BOOT_send_exec_tx_not(uint8_t channel);


/*****************************************************************************
*                       Local variables declaration                         *
*****************************************************************************/
extern boot_manager_t *p_boot_manager;

/*****************************************************************************
*                            IMPLEMENTATION                                 *
*****************************************************************************/

static void BOOT_endian_swap(uint8_t *buf, uint32_t length)
{
  swap_type temp;
  uint32_t *temp_ptr = (uint32_t*)buf;
  uint32_t progress = 0;

  do
  {
    temp.dword = *temp_ptr;
    *buf++ = temp.bytes[3];
    *buf++ = temp.bytes[2];
    *buf++ = temp.bytes[1];
    *buf++ = temp.bytes[0];

    progress += 4;
    temp_ptr++;
  } while (progress < length);
}

static void BOOT_shift_buf_left(uint8_t *buf, uint32_t shift, uint32_t length)
{
  uint32_t i;
  for (i=0; i<length; i++)
  {
    buf[i] = buf[(int)(i+shift)];
  }
}

/******************************************************************************
* Function:... boot_status_t BOOT_sgold(boot_instance_t *p_boot_ins,
*                                       boot_manager_t *p_boot_mgn)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
boot_status_t BOOT_sgold(boot_instance_t *p_boot_ins, boot_manager_t *p_boot_mgn)
{
  boot_status_t boot_status = boot_status_ok;
  comm_status_t comm_status;
  boot_error_t  boot_error = boot_error_no_error;

  uint8_t temp[8];
  BOOT_endian_swap(temp,4);
  BOOT_shift_buf_left(temp,0,4);

  p_boot_ins->nof_ebl_length_bytes = 2;  // Ebl length is 16 bit pr default.
  p_boot_manager = p_boot_mgn;
  if (GLOBAL_pre_boot[p_boot_ins->channel_id] == PRE_BOOT_DONE)
  {
    GLOBAL_pre_boot[p_boot_ins->channel_id] = PRE_BOOT_NOT_USED;
    p_boot_ins->state = boot_state_act_on_chip_response;  // pick up boot from here
    return boot_status;
  }
  /* Clear buffer before boot start */
  comm_status = COMM_flush_rx_buffer(p_boot_ins->channel_id);
  if (comm_status == comm_status_flush_buffers_ok)
  { /* rx buffer has been flushed successful */
    comm_status = COMM_flush_tx_buffer(p_boot_ins->channel_id);
    if (comm_status == comm_status_flush_buffers_ok)
    { /* tx buffer has been flushed successful */
      /* TX 0x55 stream and start boot timer */
      if (GLOBAL_special_boot_seq == IFWD_DL_boot_seq_use_sw_resident_ebl)
      {
        p_boot_ins->state = boot_state_send_ebl_exec;
      }
      else
      {
        p_boot_ins->sync_AT_count = 0;
        BOOT_tx_boot_code(p_boot_ins);
        p_boot_ins->boot_time = GLOBAL_boot_timeout;
        TIMER_new_timer(&p_boot_ins->boot_timer, p_boot_ins->boot_time);
      }
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
  p_boot_ins->link_est_struct_ver = 0;
  p_boot_ins->last_timestamp = 0;
  return boot_status;
}

/******************************************************************************
* Function:... void BOOT_sgold_handler(boot_instance_t *p_boot_ins)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
void BOOT_sgold_handler(boot_instance_t *p_boot_ins)
{
  /* Log the state transitions */
  static boot_state_t sgold_state;
  if (sgold_state != p_boot_ins->state)
  {
    Debug3("BOOT_Sgold State Change: %d -> %d\n",
           sgold_state, p_boot_ins->state);
    sgold_state = p_boot_ins->state;
  }

  switch (p_boot_ins->state)
  {
  case boot_state_rpsi_idle:
  {
    break;
  }

  case boot_state_rpsi_cmd_new_comm_tx:
  {
    uint32_t        sent_length;
//    comm_status_t comm_status;

    // Send the RPSI command to change Baudrate
    p_boot_ins->rpsi_rsp.param.dword = 0;
    p_boot_ins->rpsi_rsp.rsp         = 0;
    p_boot_ins->rpsi_rsp.crc         = 0;

    //TODO: HowTo detect USB or HSIC from port name.

#ifdef ANDROID
    p_boot_ins->rpsi_cmd.param.dword = (uint32_t)EXT_CAP_COMM_FORMAT_HSIC;
#else
    p_boot_ins->rpsi_cmd.param.dword = (uint32_t)EXT_CAP_COMM_FORMAT_USB;
#endif

    p_boot_ins->rpsi_cmd.cmd         = rpsi_cmd_code_new_comm;
    p_boot_ins->rpsi_cmd.crc         = p_boot_ins->rpsi_cmd.param.word[0] ^
                                       p_boot_ins->rpsi_cmd.param.word[1] ^
                                       p_boot_ins->rpsi_cmd.cmd;

    TIMER_new_timer(&p_boot_ins->boot_timer, p_boot_ins->boot_time);

    GLOBAL_callback(p_boot_ins->channel_id, IFWD_DL_ProcessOutlineUpdated, "    - Switch comm to USB");

    TR_BEGIN(TR_Tx,TR_boot_rpsi_tx_cmd,p_boot_ins->channel_id);
    TR_32(p_boot_ins->rpsi_cmd.param.dword);
    TR_16(p_boot_ins->rpsi_cmd.cmd);
    TR_16(p_boot_ins->rpsi_cmd.crc);
    TR_END;

    p_boot_ins->state = boot_state_rpsi_idle; // Go to idle state until calback is received.
    /*comm_status =*/ COMM_send_buffer(p_boot_ins->channel_id,
                                       (char *)&p_boot_ins->rpsi_cmd,
                                       sizeof(p_boot_ins->rpsi_cmd),
                                       &sent_length,
                                       BOOT_rpsi_comm_tx_not);

    break;
  }


  case boot_state_rpsi_cmd_new_comm_rx:
  {
    uint32_t received_length;
    p_boot_ins->state = boot_state_rpsi_idle; // Go to idle state until calback is received.
    TIMER_new_timer(&p_boot_ins->boot_timer, p_boot_ins->boot_time);

    (void)COMM_receive_buffer(p_boot_ins->channel_id,
                              (char *)&p_boot_ins->rpsi_rsp,
                              sizeof(p_boot_ins->rpsi_rsp),
                              &received_length,
                              BOOT_rpsi_comm_rx_not);


    break;
  }

  case boot_state_rpsi_set_comm:
  {
      // Got the response from target so do the actual change.
      // Close the comport
      COMM_close_comport(p_boot_ins->channel_id);
      COMM_delete_channel(p_boot_ins->channel_id);

     p_boot_ins->state = boot_state_rpsi_set_comm_delay;
     TIMER_new_timer(&p_boot_ins->tx_boot_code_timer, 20);
     break;
  }

  case boot_state_rpsi_set_comm_delay:
  {
     if (TIMER_timer_expired(&p_boot_ins->tx_boot_code_timer))
     {
       comm_cfg_t port_param;
       // Open the comport ... or another comm-driver
       snprintf(port_param.comport_name, sizeof(port_param.comport_name)-1, "%s", &GLOBAL_ctrl_port_name[p_boot_ins->channel_id][0]);  // the ctrl_port_name may be different from comm_port_name ... so use it instead

       if (COMM_open_port(p_boot_ins->channel_id, &port_param) == comm_status_ok)
       {
         COMM_add_channel(p_boot_ins->channel_id);
         p_boot_ins->state = boot_state_rpsi_cmd_send_ebl;
       }
       else
       {
       // Start time again if port did not open
         TIMER_new_timer(&p_boot_ins->tx_boot_code_timer, 20);
       }
     }
     break;
  }

  case boot_state_send_ebl_exec:
  {
    uint32_t sent_length;

    memcpy(p_boot_ins->buffer, "EXEC\0", 5);
    p_boot_ins->tx_buffer.max_length = 4;
    p_boot_ins->tx_buffer.buffer = p_boot_ins->buffer;

    p_boot_ins->state = boot_state_rpsi_idle; // Go to idle state until calback is received.
    COMM_abort_process(p_boot_ins->channel_id);
     /*comm_status =*/ COMM_send_buffer(p_boot_ins->channel_id,
                                       (char*)p_boot_ins->tx_buffer.buffer,
                                       p_boot_ins->tx_buffer.max_length,
                                       &sent_length,
                                       BOOT_send_exec_tx_not);

    TIMER_new_timer(&p_boot_ins->tx_boot_code_timer, 50);

    break;
  }
  case boot_state_send_ebl_exec_wait:
  {
    COMM_abort_process(p_boot_ins->channel_id);
    BOOT_rx_ebl_version(p_boot_ins); // Ask to receive the EBL version
    p_boot_ins->state = boot_state_send_ebl_exec_delay;
    break;
  }
  case boot_state_send_ebl_exec_delay:
  {
    if (TIMER_timer_expired(&p_boot_ins->tx_boot_code_timer))
    {
      p_boot_ins->state = boot_state_send_ebl_exec;
    }
    break;
  }

  case boot_state_rpsi_cmd_new_baud_tx:
  {
    uint32_t        sent_length;
//    comm_status_t comm_status;

    // Send the RPSI command to change Baudrate
    p_boot_ins->rpsi_rsp.param.dword = 0;
    p_boot_ins->rpsi_rsp.rsp         = 0;
    p_boot_ins->rpsi_rsp.crc         = 0;

    p_boot_ins->rpsi_cmd.param.dword = p_boot_ins->EBL_load_baudrate;
    p_boot_ins->rpsi_cmd.cmd         = rpsi_cmd_code_set_baud;
    p_boot_ins->rpsi_cmd.crc         = p_boot_ins->rpsi_cmd.param.word[0] ^
                                       p_boot_ins->rpsi_cmd.param.word[1] ^
                                       p_boot_ins->rpsi_cmd.cmd;

    TIMER_new_timer(&p_boot_ins->boot_timer, p_boot_ins->boot_time);

    GLOBAL_callback(p_boot_ins->channel_id, IFWD_DL_ProcessOutlineUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, BOOT_INFO_RPSI_BAUD, p_boot_ins->EBL_load_baudrate));

    TR_BEGIN(TR_Tx,TR_boot_rpsi_tx_cmd,p_boot_ins->channel_id);
    TR_32(p_boot_ins->rpsi_cmd.param.dword);
    TR_16(p_boot_ins->rpsi_cmd.cmd);
    TR_16(p_boot_ins->rpsi_cmd.crc);
    TR_END;

    p_boot_ins->state = boot_state_rpsi_idle; // Go to idle state until calback is received.
    /*comm_status =*/ COMM_send_buffer(p_boot_ins->channel_id,
                                       (char *)&p_boot_ins->rpsi_cmd,
                                       sizeof(p_boot_ins->rpsi_cmd),
                                       &sent_length,
                                       BOOT_rpsi_baud_tx_not);

    break;
  }

  case boot_state_rpsi_cmd_new_baud_rx:
  {
    uint32_t received_length;
    p_boot_ins->state = boot_state_rpsi_idle; // Go to idle state until calback is received.
    TIMER_new_timer(&p_boot_ins->boot_timer, p_boot_ins->boot_time);

    (void)COMM_receive_buffer(p_boot_ins->channel_id,
                              (char *)&p_boot_ins->rpsi_rsp,
                              sizeof(p_boot_ins->rpsi_rsp),
                              &received_length,
                              BOOT_rpsi_baud_rx_not);


    break;
  }

  case boot_state_rpsi_set_baud:
  {
     COMM_set_baudrate(p_boot_ins->channel_id, p_boot_ins->EBL_load_baudrate);
     p_boot_ins->state = boot_state_rpsi_set_baud_delay;
     TIMER_new_timer(&p_boot_ins->tx_boot_code_timer, 20);
     break;
  }

  case boot_state_rpsi_set_baud_delay:
  {
     if (TIMER_timer_expired(&p_boot_ins->tx_boot_code_timer))
     {
       p_boot_ins->state = boot_state_rpsi_cmd_send_ebl;
     }
     break;
  }

  case boot_state_rpsi_cmd_send_ebl:
  {
    uint32_t        sent_length;

    // Send the RPSI command to change Baudrate
    p_boot_ins->rpsi_rsp.param.dword = 0;
    p_boot_ins->rpsi_rsp.rsp         = 0;
    p_boot_ins->rpsi_rsp.crc         = 0;

    p_boot_ins->rpsi_cmd.param.dword = 0;
    p_boot_ins->rpsi_cmd.cmd         = rpsi_cmd_code_tx_ebl;
    p_boot_ins->rpsi_cmd.crc         = p_boot_ins->rpsi_cmd.param.word[0] ^
                                       p_boot_ins->rpsi_cmd.param.word[1] ^
                                       p_boot_ins->rpsi_cmd.cmd;

    TIMER_new_timer(&p_boot_ins->boot_timer, p_boot_ins->boot_time);

    GLOBAL_callback(p_boot_ins->channel_id, IFWD_DL_ProcessOutlineUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, BOOT_INFO_RPSI_LD_EBL));

    TR_BEGIN(TR_Tx,TR_boot_rpsi_tx_cmd,p_boot_ins->channel_id);
    TR_32(p_boot_ins->rpsi_cmd.param.dword);
    TR_16(p_boot_ins->rpsi_cmd.cmd);
    TR_16(p_boot_ins->rpsi_cmd.crc);
    TR_END;

    p_boot_ins->state = boot_state_rpsi_idle; // Go to idle state until calback is received.
    /*comm_status =*/ COMM_send_buffer(p_boot_ins->channel_id,
                                       (char *)&p_boot_ins->rpsi_cmd,
                                       sizeof(p_boot_ins->rpsi_cmd),
                                       &sent_length,
                                       BOOT_rpsi_send_ebl_not);
    break;
  }

  case boot_state_tx_boot_code:
//PES
#if !defined(PES_DEBUG_ONE_AT)
    BOOT_tx_boot_code(p_boot_ins);
#endif
    break;

  case boot_state_rx_boot_usb_ack:
    BOOT_rx_boot_ack_code(p_boot_ins);
    break;

  case boot_state_rx_boot_ack:
    BOOT_rx_boot_ack_code(p_boot_ins);
    break;

  case boot_state_rx_2nd_boot_response:
    BOOT_rx_boot_ack_code_2nd_stage_618_ES2(p_boot_ins);
    break;

  case boot_state_tx_waiting_boot_ack:
  case boot_state_rx_waiting_boot_ack:
    if (TIMER_timer_expired(&p_boot_ins->boot_timer))
    {
      p_boot_ins->state = boot_state_error;
      p_boot_ins->last_status = boot_status_boot_timeout;
      p_boot_ins->last_error = boot_error_RX_boot_ack_timeout;
    }
    else if (TIMER_timer_expired(&p_boot_ins->tx_boot_code_timer))
    {
      p_boot_ins->state = boot_state_tx_boot_code;
//PES
#if !defined(PES_DEBUG_ONE_AT)
      BOOT_tx_boot_code(p_boot_ins); // Don't wait for next clock - do it now
#endif
    }
    break;

  case boot_state_tx_ebl:
    BOOT_tx_ebl(p_boot_ins);
    break;

  case boot_state_tx_ebl_loader:
    TIMER_new_timer(&p_boot_ins->tx_boot_code_timer, 20);
    p_boot_ins->state = boot_state_tx_ebl_loader_delay;
    break;

  case boot_state_tx_ebl_loader_delay:
  {
   if (TIMER_timer_expired(&p_boot_ins->tx_boot_code_timer))
   {
     if (GLOBAL_usb_device || COMM_TECH_IS_MIPI(p_boot_ins->channel_id))
     {
       BOOT_tx_ebl_loader_length(p_boot_ins);
     }
     else if (p_boot_ins->link_est_struct_ver == 0xF1)
     {
      p_boot_ins->state = boot_state_rom_baud_tx;
     }
     else
     {
      BOOT_tx_ebl_loader_length(p_boot_ins);
     }
   }
   break;
  }

  case boot_state_rom_baud_tx:
  {
    uint32_t sent_length;
    uint8_t  baud_cmd[2];

    baud_cmd[0] = BOOT_BAUD_CMD;
    if (p_boot_ins->EBL_load_baudrate > 921600)
    {
      baud_cmd[1] = BOOT_BAUD_921600;
    }
    else
    {
      switch(p_boot_ins->EBL_load_baudrate)
      {
        case 115200:
        {
           baud_cmd[1] = BOOT_BAUD_115200;
           break;
        }
        case 230400:
        {
           baud_cmd[1] = BOOT_BAUD_230400;
           break;
        }
        case 460800:
        {
           baud_cmd[1] = BOOT_BAUD_460800;
           break;
        }
        case  921600:
        {
           baud_cmd[1] = BOOT_BAUD_921600;
           break;
        }
      }
    }

   TR_BEGIN(TR_Tx,TR_boot_rom_baud_cmd,p_boot_ins->channel_id);
   TR_8(baud_cmd[0]);
   TR_8(baud_cmd[1]);
   TR_END;

   p_boot_ins->state = boot_state_rpsi_idle; // Go to idle state until calback is received.
   /*comm_status =*/ COMM_send_buffer(p_boot_ins->channel_id,
                              (char *)&baud_cmd,
                              sizeof(baud_cmd),
                              &sent_length,
                              BOOT_tx_rom_baud_not);
   break;

  }

  case boot_state_rom_baud_rx:
  {
    uint32_t received_length;
    p_boot_ins->state = boot_state_rom_baud_rx_wait; // Go to wait state until calback is received.
    TIMER_new_timer(&p_boot_ins->tx_boot_code_timer, 20);

    (void)COMM_receive_buffer(p_boot_ins->channel_id,
                              (char *)&p_boot_ins->buffer,
                              sizeof(uint8_t),
                              &received_length,
                              BOOT_rx_rom_baud_not);
    break;
  }

  case boot_state_rom_skip_psi_load:
  {
    uint32_t sent_length;
    uint8_t  skip_psi_cmd = BOOT_SKIP_PSI_LD;

    TR_BEGIN(TR_Tx,TR_boot_rom_skip_psi_cmd,p_boot_ins->channel_id);
    TR_8(skip_psi_cmd);
    TR_END;

    p_boot_ins->state = boot_state_rpsi_idle; // Wait until callback is received.
    (void)COMM_send_buffer(p_boot_ins->channel_id,
                           (char *)&skip_psi_cmd,
                           sizeof(skip_psi_cmd),
                           &sent_length,
                           BOOT_tx_rom_skip_psi_not);
    break;
  }

  case boot_state_rom_skip_psi_rx:
  {
    uint32_t received_length;
    p_boot_ins->state = boot_state_rom_baud_rx_wait; // Go to wait state until calback is received.
    TIMER_new_timer(&p_boot_ins->tx_boot_code_timer, 20);
    (void)COMM_receive_buffer(p_boot_ins->channel_id,
                              (char *)&p_boot_ins->buffer,
                              sizeof(uint8_t),
                              &received_length,
                              BOOT_rx_rom_skip_psi_not);
    break;
  }
  case boot_state_get_psi_enh_ack:
  {
    uint32_t received_length;
    p_boot_ins->state = boot_state_rom_baud_rx_wait; // Go to wait state until calback is received.

    (void)COMM_receive_buffer(p_boot_ins->channel_id,
                              (char *)&p_boot_ins->buffer,
                              sizeof(uint8_t),
                              &received_length,
                              BOOT_rx_psi_ack_not);
    break;
  }
  case boot_state_psi_validate_sys:
  {
    uint32_t        sent_length;

    // Send the RPSI command to change Baudrate
    p_boot_ins->rpsi_rsp.param.dword = 0;
    p_boot_ins->rpsi_rsp.rsp         = 0;
    p_boot_ins->rpsi_rsp.crc         = 0;

    p_boot_ins->rpsi_cmd.param.dword = 0;
    p_boot_ins->rpsi_cmd.cmd         = rpsi_cmd_validate_sys_sw;
    p_boot_ins->rpsi_cmd.crc         = p_boot_ins->rpsi_cmd.param.word[0] ^
                                       p_boot_ins->rpsi_cmd.param.word[1] ^
                                       p_boot_ins->rpsi_cmd.cmd;


    TR_BEGIN(TR_Tx,TR_boot_rpsi_tx_cmd,p_boot_ins->channel_id);
    TR_32(p_boot_ins->rpsi_cmd.param.dword);
    TR_16(p_boot_ins->rpsi_cmd.cmd);
    TR_16(p_boot_ins->rpsi_cmd.crc);
    TR_END;

    p_boot_ins->state = boot_state_rpsi_idle; // Go to idle state until calback is received.
    (void)COMM_send_buffer(p_boot_ins->channel_id,
                           (char *)&p_boot_ins->rpsi_cmd,
                           sizeof(p_boot_ins->rpsi_cmd),
                           &sent_length,
                           BOOT_tx_rpsi_val_sys_not);
    break;
  }
  case boot_state_get_psi_val_sys_rsp:
  {
    uint32_t received_length;
    p_boot_ins->state = boot_state_rpsi_idle; // Go to idle state until calback is received.
    TIMER_new_timer(&p_boot_ins->boot_timer, p_boot_ins->boot_time);
    (void)COMM_receive_buffer(p_boot_ins->channel_id,
                              (char *)&p_boot_ins->rpsi_rsp,
                              sizeof(p_boot_ins->rpsi_rsp),
                              &received_length,
                              BOOT_rx_rpsi_val_sys_not);
    break;
  }
  case boot_state_psi_start_sys:
  {
    uint32_t sent_length;

    // Send the RPSI command to change Baudrate
    p_boot_ins->rpsi_rsp.param.dword = 0;
    p_boot_ins->rpsi_rsp.rsp         = 0;
    p_boot_ins->rpsi_rsp.crc         = 0;

    p_boot_ins->rpsi_cmd.param.dword = 0;
    p_boot_ins->rpsi_cmd.cmd         = rpsi_cmd_code_tx_ebl;
    p_boot_ins->rpsi_cmd.crc         = p_boot_ins->rpsi_cmd.param.word[0] ^
                                       p_boot_ins->rpsi_cmd.param.word[1] ^
                                       p_boot_ins->rpsi_cmd.cmd;

    TIMER_new_timer(&p_boot_ins->boot_timer, p_boot_ins->boot_time);

    GLOBAL_callback(p_boot_ins->channel_id, IFWD_DL_ProcessOutlineUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, BOOT_INFO_RPSI_LD_EBL));

    TR_BEGIN(TR_Tx,TR_boot_rpsi_tx_cmd,p_boot_ins->channel_id);
    TR_32(p_boot_ins->rpsi_cmd.param.dword);
    TR_16(p_boot_ins->rpsi_cmd.cmd);
    TR_16(p_boot_ins->rpsi_cmd.crc);
    TR_END;

    p_boot_ins->state = boot_state_rpsi_idle; // Wait until callback is received.
    (void)COMM_send_buffer(p_boot_ins->channel_id,
                           (char *)&p_boot_ins->rpsi_cmd,
                           sizeof(p_boot_ins->rpsi_cmd),
                           &sent_length,
                           BOOT_tx_psi_start_sys_not);
    break;
  }
  case boot_state_rx_in_progress:
  case boot_state_tx_in_progress:
  case boot_state_rom_baud_rx_wait:
  {
   if (TIMER_timer_expired(&p_boot_ins->boot_timer))
   {
     p_boot_ins->state = boot_state_error;
     p_boot_ins->last_status = boot_status_boot_timeout;
     p_boot_ins->last_error = boot_error_RX_boot_ack_timeout;
   }
   break;
  }

  case boot_state_rom_baud_delay:
  {
    if (p_boot_ins->EBL_load_baudrate >= 921600)
    {
      COMM_set_baudrate(p_boot_ins->channel_id, 921600);
    }
    else
    {
      COMM_set_baudrate(p_boot_ins->channel_id, p_boot_ins->EBL_load_baudrate);
    }
    p_boot_ins->state = boot_state_rom_baud_done;
    TIMER_new_timer(&p_boot_ins->tx_boot_code_timer, 20);
    break;
  }
  case boot_state_rom_baud_done:
  {
    if (TIMER_timer_expired(&p_boot_ins->tx_boot_code_timer))
    {
       p_boot_ins->state = boot_state_ping_tx;
    }
    break;
  }

  case boot_state_ping_tx:
  {
   uint32_t sent_length;
   uint8_t  ping_cmd = BOOT_PING_CMD;

   TR_BEGIN(TR_Tx,TR_boot_ping_cmd,p_boot_ins->channel_id);
   TR_8(ping_cmd);
   TR_END;


   p_boot_ins->state = boot_state_rpsi_idle; // Go to idle state until calback is received.
   /*comm_status =*/ COMM_send_buffer(p_boot_ins->channel_id,
                              (char *)&ping_cmd,
                              sizeof(ping_cmd),
                              &sent_length,
                              BOOT_tx_ping_not);
   break;
  }

  case boot_state_ping_rx:
  {

   uint32_t received_length;
   p_boot_ins->state = boot_state_ping_rx_wait; // Go to wait state until calback is received.
   TIMER_new_timer(&p_boot_ins->tx_boot_code_timer, 20);

   (void)COMM_receive_buffer(p_boot_ins->channel_id,
                       (char *)&p_boot_ins->buffer,
                        sizeof(uint8_t),
                        &received_length,
                        BOOT_rx_ping_not);
   break;
  }
  case boot_state_ping_rx_wait:
  {
   if (TIMER_timer_expired(&p_boot_ins->boot_timer))
   {
     p_boot_ins->state = boot_state_error;
     p_boot_ins->last_status = boot_status_boot_timeout;
     p_boot_ins->last_error = boot_error_RX_boot_ack_timeout;
   }
   else if (TIMER_timer_expired(&p_boot_ins->tx_boot_code_timer))
   {
     p_boot_ins->state = boot_state_ping_tx;
   }
   break;
  }
  case boot_state_ping_done:
  {
   BOOT_tx_ebl_loader_length(p_boot_ins);
   break;
  }
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

  case boot_state_act_on_chip_response:
    act_on_boot_response(p_boot_ins->channel_id);
    break;

  case boot_state_pre_boot_ok:
    p_boot_ins->status = boot_status_pre_boot_ok;
    p_boot_ins->last_error = boot_error_no_error;
    p_boot_ins->state = boot_state_idle;
    if ((p_boot_ins->boot_buffer.buffer != NULL) && (p_boot_ins->boot_buffer_allocated))
    {
      MEM_free(p_boot_ins->boot_buffer.buffer);
      p_boot_ins->boot_buffer.buffer = NULL;
      p_boot_ins->boot_buffer_allocated = false;
    }
    TIMER_stop_timer(&p_boot_ins->boot_timer);  /* Remember to stop the timer when done OK */
    if (p_boot_ins->fx != NULL)
    {
      p_boot_ins->fx(p_boot_ins->channel_id, PRE_BOOT_OK, p_boot_ins->p_ebl_version);
    }
    break;

  case boot_state_ebl_running:
    p_boot_ins->status = boot_status_ebl_running;
    p_boot_ins->last_error = boot_error_no_error;
    p_boot_ins->state = boot_state_idle;
    if ((p_boot_ins->boot_buffer.buffer != NULL) && (p_boot_ins->boot_buffer_allocated))
    {
      MEM_free(p_boot_ins->boot_buffer.buffer);
      p_boot_ins->boot_buffer.buffer = NULL;
      p_boot_ins->boot_buffer_allocated = false;
    }
    TIMER_stop_timer(&p_boot_ins->boot_timer);  /* Remember to stop the timer when done OK */
#ifdef INCLUDE_DUT_USB_TEST
    if (IFWD_BOOT_dut_to_test_mode())
    {
      p_boot_ins->boot_time = 0;
      TIMER_new_timer(&p_boot_ins->boot_timer, p_boot_ins->boot_time);
      p_boot_ins->dut_in_test_mode = true;
      p_boot_ins->state = boot_state_dut_in_test_mode;
    }
#endif
    GLOBAL_is_in_time_critical_section = false;
    if (p_boot_ins->fx != NULL)
    {
      p_boot_ins->fx(p_boot_ins->channel_id, EBL_RUNNING, p_boot_ins->p_ebl_version);
    }
    break;

  case boot_state_error:
    TIMER_stop_timer(&p_boot_ins->boot_timer);  /* Remember to stop the timer when an error is detected. */
    p_boot_ins->state = boot_state_error;
    if ((p_boot_ins->boot_buffer.buffer != NULL) && (p_boot_ins->boot_buffer_allocated))
    {
      MEM_free(p_boot_ins->boot_buffer.buffer);
      p_boot_ins->boot_buffer.buffer = NULL;
      p_boot_ins->boot_buffer_allocated = false;
    }
    p_boot_ins->state = boot_state_idle;
    if (p_boot_ins->fx != NULL)
    {
      p_boot_ins->p_ebl_version = NULL;
      p_boot_ins->fx(p_boot_ins->channel_id, EBL_NOT_RUNNING, p_boot_ins->p_ebl_version);
    }

    break;

  case boot_state_rx_psi_error:

    p_boot_ins->state = boot_state_rpsi_idle; // Go to idle state until calback is received.
    p_boot_ins->rx_buffer.max_length = sizeof(memtest_error_t);
    (void)COMM_receive_buffer(p_boot_ins->channel_id,
                              (char *)p_boot_ins->rx_buffer.buffer,
                              p_boot_ins->rx_buffer.max_length,
                              &p_boot_ins->rx_buffer.length,
                              BOOT_psi_error);
    break;

  case boot_state_rx_ebl_loader_payload_ind:
    BOOT_rx_ebl_payload_ind(p_boot_ins);
    break;

  case boot_state_rx_ebl_loader_ready:
    {
      BOOT_rx_ebl_ready_ind(p_boot_ins);
      break;
    }

#ifdef INCLUDE_DUT_USB_TEST
  case boot_state_dut_in_test_mode:
    BOOT_dut_process_test(p_boot_ins);
    break;

  case boot_state_dut_usb_test:
  case boot_state_dut_prep_usb_test_out:
  case boot_state_dut_start_usb_test_out:
  case boot_state_dut_usb_test_out:
  case boot_state_dut_prep_usb_test_in:
  case boot_state_dut_start_usb_test_in:
  case boot_state_dut_usb_test_in:
  case boot_state_dut_usb_send_zero_pack:
  case boot_state_dut_prep_usb_test_loopback:
  case boot_state_dut_start_usb_test_loopback:
  case boot_state_dut_usb_test_loopback:
  case boot_state_dut_usb_test_idle:
  case boot_state_dut_usb_prep_rpsi_rsp:
    DUT_usb_test(p_boot_ins);
    break;
#endif
  case boot_state_idle:
    break;

  default:
    ;
  }

  /* Remember to check the runnig timers */
  if ((TIMER_timer_expired(&p_boot_ins->boot_timer)) &&
    ((p_boot_ins->state != boot_state_ebl_running) ||
    (p_boot_ins->state != boot_state_idle)))
  {
    if ((p_boot_ins->boot_buffer.buffer != NULL) && (p_boot_ins->boot_buffer_allocated))
    {
      MEM_free(p_boot_ins->boot_buffer.buffer);
      p_boot_ins->boot_buffer.buffer = NULL;
      p_boot_ins->boot_buffer_allocated = false;
    }
    TIMER_stop_timer(&p_boot_ins->boot_timer);
    BOOT_update_timeout_report(p_boot_ins);
    p_boot_ins->state = boot_state_idle;
    if (p_boot_ins->fx != NULL)
    {
      p_boot_ins->p_ebl_version = NULL;
      p_boot_ins->fx(p_boot_ins->channel_id, EBL_NOT_RUNNING, p_boot_ins->p_ebl_version);
    }
    return;
  }
}

/*****************************************************************************
*                            Private functions                              *
*****************************************************************************/
static void BOOT_send_exec_tx_not(uint8_t channel)
{
  boot_instance_t *p_boot_ins;

  p_boot_ins = p_boot_manager->boot_handler[channel];
  p_boot_ins->state = boot_state_send_ebl_exec_wait;

  TR_BEGIN(TR_Tx,TR_boot_tx_at,p_boot_ins->channel_id);
  TR_DATA(p_boot_ins->tx_buffer.buffer, p_boot_ins->tx_buffer.max_length);
  TR_END;
}

static void BOOT_tx_ping_not(uint8_t channel)
{
  boot_instance_t *p_boot_ins;

  p_boot_ins = p_boot_manager->boot_handler[channel];
  p_boot_ins->state = boot_state_ping_rx;

}

static void BOOT_rx_ping_not(uint8_t channel, uint32_t size)
{
  boot_instance_t *p_boot_ins;

  p_boot_ins = p_boot_manager->boot_handler[channel];
  if (size)
  {
   TR_BEGIN(TR_Rx,TR_boot_ping_rsp,p_boot_ins->channel_id);
   TR_8(p_boot_ins->buffer[0]);
   TR_END;

   if(p_boot_ins->buffer[0] == BOOT_PING_RSP)
   {
     p_boot_ins->state = boot_state_ping_done;
   }
  }
}

static void BOOT_tx_rom_baud_not(uint8_t channel)
{
  boot_instance_t *p_boot_ins;

  p_boot_ins = p_boot_manager->boot_handler[channel];
  p_boot_ins->state = boot_state_rom_baud_rx;

}

static void BOOT_rx_rom_baud_not(uint8_t channel, uint32_t size)
{
  boot_instance_t *p_boot_ins;

  p_boot_ins = p_boot_manager->boot_handler[channel];
  if (size)
  {
   TR_BEGIN(TR_Rx,TR_boot_rom_baud_rsp,p_boot_ins->channel_id);
   TR_8(p_boot_ins->buffer[0]);
   TR_END;

   if(p_boot_ins->buffer[0] == BOOT_BAUD_RSP_OK)
   {
     p_boot_ins->state = boot_state_rom_baud_delay;
   }
   else
   {
     p_boot_ins->state = boot_state_rpsi_idle;
   }
  }
}
static void BOOT_tx_rom_skip_psi_not(uint8_t channel)
{
  boot_instance_t *p_boot_ins;
  p_boot_ins = p_boot_manager->boot_handler[channel];
  p_boot_ins->state = boot_state_rom_skip_psi_rx;

}


static void BOOT_rx_rom_skip_psi_not(uint8_t channel, uint32_t size)
{
  boot_instance_t *p_boot_ins;

  p_boot_ins = p_boot_manager->boot_handler[channel];
  if (size)
  {
    TR_BEGIN(TR_Rx,TR_boot_rom_skip_psi_rsp,p_boot_ins->channel_id);
    TR_8(p_boot_ins->buffer[0]);
    TR_END;

    if((p_boot_ins->buffer[0] == 1) || (p_boot_ins->buffer[0] == 0x10) || (p_boot_ins->buffer[0] == 0x20))
    {
      p_boot_ins->state = boot_state_get_psi_enh_ack;
    }
    else
    {
      p_boot_ins->state = boot_state_error;
      BOOT_Err(channel, BOOT_ERR_boot_skip_psi);
    }
  }
}

static void BOOT_rx_psi_ack_not(uint8_t channel, uint32_t size)
{
  boot_instance_t *p_boot_ins;

  p_boot_ins = p_boot_manager->boot_handler[channel];
  if (size)
  {
   TR_BEGIN(TR_Rx,TR_boot_rx_rpsi_ack,p_boot_ins->channel_id);
   TR_8(p_boot_ins->buffer[0]);
   TR_END;

   if(p_boot_ins->buffer[0] == PSI_ACK_ENH)
   {
     p_boot_ins->state = boot_state_psi_validate_sys;
   }
   else
   {
      p_boot_ins->state = boot_state_error;
      BOOT_Err(channel, BOOT_ERR_rpsi_wrong_ack);
   }
  }
}

static void BOOT_tx_rpsi_val_sys_not(uint8_t channel)
{
  boot_instance_t *p_boot_ins;
  p_boot_ins = p_boot_manager->boot_handler[channel];
  p_boot_ins->state = boot_state_get_psi_val_sys_rsp;

}

static void BOOT_tx_psi_start_sys_not(uint8_t channel)
{
  boot_instance_t *p_boot_ins;
  p_boot_ins = p_boot_manager->boot_handler[channel];

  // System SW was validated to be OK
  // No more action needed
  TIMER_stop_timer(&p_boot_ins->boot_timer);
  p_boot_ins->state = boot_state_idle;
  if (p_boot_ins->fx != NULL)
  {
    p_boot_ins->fx(p_boot_ins->channel_id, SYS_SW_RUNNING, p_boot_ins->p_ebl_version);
  }

}

static void BOOT_rx_rpsi_val_sys_not(uint8_t channel, uint32_t size)
{
  boot_instance_t *p_boot_ins;
  uint16_t crc;

  p_boot_ins = p_boot_manager->boot_handler[channel];
  if (size != sizeof(p_boot_ins->rpsi_rsp))
  {
    // Baud could not be set
    p_boot_ins->state = boot_state_error;
    BOOT_Err(channel, BOOT_ERR_rpsi_rx_fail);
  }
  else
  {
    TR_BEGIN(TR_Rx,TR_boot_rpsi_rx_rsp,p_boot_ins->channel_id);
    TR_32(p_boot_ins->rpsi_rsp.param.dword);
    TR_16(p_boot_ins->rpsi_rsp.rsp);
    TR_16(p_boot_ins->rpsi_rsp.crc);
    TR_END;
    crc = p_boot_ins->rpsi_rsp.param.word[0] ^ p_boot_ins->rpsi_rsp.param.word[1] ^ p_boot_ins->rpsi_rsp.rsp;
    if (p_boot_ins->rpsi_rsp.crc == crc)
    {
      switch (p_boot_ins->rpsi_rsp.rsp)
      {
        case rpsi_rsp_ok:
        {
          p_boot_ins->state = boot_state_psi_start_sys;
          break;
        }

        case rpsi_rsp_sys_sw_not_valid:
        {
          // Could not validate the system SW ...
          // jump to normal flow where the PSI sends normal ACK.
          p_boot_ins->state = boot_state_rpsi_cmd_send_ebl;
          break;
        }

        case rpsi_rsp_cmd_unknown:
        {
          // error cmd not known.
          p_boot_ins->state = boot_state_error;
          BOOT_Err(channel, BOOT_ERR_rpsi_cmd_unknown);
          break;
        }

        case rpsi_rsp_cmd_crc_err:
        {
          // error cmd not known.
          p_boot_ins->state = boot_state_error;
          BOOT_Err(channel, BOOT_ERR_rpsi_cmd_crc);
          break;
        }

        default:
        {
          // error unknown response
          p_boot_ins->state = boot_state_error;
          BOOT_Err(channel, BOOT_ERR_rpsi_rsp_unknown);
        }
      }
    }
  }
}
static void BOOT_rpsi_baud_tx_not(uint8_t channel)
{
  boot_instance_t *p_boot_ins;

  p_boot_ins = p_boot_manager->boot_handler[channel];
  p_boot_ins->state = boot_state_rpsi_cmd_new_baud_rx;
}

static void BOOT_rpsi_comm_tx_not(uint8_t channel)
{
  boot_instance_t *p_boot_ins;

  p_boot_ins = p_boot_manager->boot_handler[channel];
  p_boot_ins->state = boot_state_rpsi_cmd_new_comm_rx;
}

static void BOOT_rpsi_send_ebl_not(uint8_t channel)
{
  boot_instance_t *p_boot_ins;

  p_boot_ins = p_boot_manager->boot_handler[channel];
  p_boot_ins->state = boot_state_rx_ebl_loader_ready;
}

static void BOOT_rpsi_baud_rx_not(uint8_t channel, uint32_t size)
{
  boot_instance_t *p_boot_ins;
  uint16_t crc;

  p_boot_ins = p_boot_manager->boot_handler[channel];
  if (size != sizeof(p_boot_ins->rpsi_rsp))
  {
    // Baud could not be set
    p_boot_ins->state = boot_state_error;
    BOOT_Err(channel, BOOT_ERR_rpsi_rx_fail);
  }
  else
  {
    TR_BEGIN(TR_Rx,TR_boot_rpsi_rx_rsp,p_boot_ins->channel_id);
    TR_32(p_boot_ins->rpsi_rsp.param.dword);
    TR_16(p_boot_ins->rpsi_rsp.rsp);
    TR_16(p_boot_ins->rpsi_rsp.crc);
    TR_END;

    crc = p_boot_ins->rpsi_rsp.param.word[0] ^ p_boot_ins->rpsi_rsp.param.word[1] ^ p_boot_ins->rpsi_rsp.rsp;
    if (p_boot_ins->rpsi_rsp.crc == crc)
     {
       switch (p_boot_ins->rpsi_rsp.rsp)
       {
         case rpsi_rsp_ok:
         {
           p_boot_ins->state = boot_state_rpsi_set_baud;
           break;
         }

         case rpsi_rsp_fail:
         {
           // Baud could not be set
           p_boot_ins->state = boot_state_error;
   #if defined (USE_TEXT_CLASS_IFWD_ERRORS)
       BOOT_Err(channel, BOOT_ERR_rpsi_baud_could_not_be_set);
   #endif
           break;
         }

         case rpsi_rsp_cmd_unknown:
         {
           // error cmd not known.
           p_boot_ins->state = boot_state_error;
   #if defined (USE_TEXT_CLASS_IFWD_ERRORS)
       BOOT_Err(channel, BOOT_ERR_rpsi_cmd_unknown);
   #endif
          break;
         }

         case rpsi_rsp_cmd_crc_err:
         {
           // error cmd not known.
           p_boot_ins->state = boot_state_error;
   #if defined (USE_TEXT_CLASS_IFWD_ERRORS)
       BOOT_Err(channel, BOOT_ERR_rpsi_cmd_crc);
   #endif
          break;
         }

         default:
         {
           // error unknown response
           p_boot_ins->state = boot_state_error;
           BOOT_Err(channel, BOOT_ERR_rpsi_rsp_unknown);
         }
       }
     }
     else
     {
       // crc error
       p_boot_ins->state = boot_state_error;
   #if defined (USE_TEXT_CLASS_IFWD_ERRORS)
       BOOT_Err(channel, BOOT_ERR_rpsi_rsp_crc);
   #endif
    }
  }
}

static void BOOT_rpsi_comm_rx_not(uint8_t channel, uint32_t size)
{
  boot_instance_t *p_boot_ins;
  uint16_t crc;

  p_boot_ins = p_boot_manager->boot_handler[channel];
  if (size != sizeof(p_boot_ins->rpsi_rsp))
  {
    // Baud could not be set
    p_boot_ins->state = boot_state_error;
    BOOT_Err(channel, BOOT_ERR_rpsi_rx_fail);
  }
  else
  {
    TR_BEGIN(TR_Rx,TR_boot_rpsi_rx_rsp,p_boot_ins->channel_id);
    TR_32(p_boot_ins->rpsi_rsp.param.dword);
    TR_16(p_boot_ins->rpsi_rsp.rsp);
    TR_16(p_boot_ins->rpsi_rsp.crc);
    TR_END;

    crc = p_boot_ins->rpsi_rsp.param.word[0] ^ p_boot_ins->rpsi_rsp.param.word[1] ^ p_boot_ins->rpsi_rsp.rsp;
    if (p_boot_ins->rpsi_rsp.crc == crc)
     {
       switch (p_boot_ins->rpsi_rsp.rsp)
       {
         case rpsi_rsp_ok:
         {
           p_boot_ins->state = boot_state_rpsi_set_comm;
           break;
         }

         case rpsi_rsp_fail:
         {
           // Baud could not be set
           p_boot_ins->state = boot_state_rpsi_cmd_send_ebl;
           break;
         }

         case rpsi_rsp_cmd_unknown:
         {
           // error cmd not known.
           p_boot_ins->state = boot_state_rpsi_cmd_send_ebl;
          break;
         }

         case rpsi_rsp_cmd_crc_err:
         {
           // error cmd not known.
           p_boot_ins->state = boot_state_error;
           BOOT_Err(channel, BOOT_ERR_rpsi_rsp_unknown);
         break;
         }

         default:
         {
           // error unknown response
           p_boot_ins->state = boot_state_error;
           BOOT_Err(channel, BOOT_ERR_rpsi_rsp_unknown);
         }
       }
     }
     else
     {
       // crc error
       p_boot_ins->state = boot_state_error;
       BOOT_Err(channel, BOOT_ERR_rpsi_rsp_crc);
    }
  }
}

/******************************************************************************
* Function:... void BOOT_tx_boot_code_not(uint8_t channel)
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
  {
    p_boot_ins->state = boot_state_tx_boot_code;
  }
  else
  {
    TR_BEGIN(TR_Tx,TR_boot_tx_at,p_boot_ins->channel_id);
    TR_DATA(p_boot_ins->tx_buffer.buffer, p_boot_ins->tx_buffer.max_length); /* Data: uint16_t */
    TR_END;
    p_boot_ins->sync_AT_count += 1;
    if ((p_boot_ins->sync_AT_count < 2 ) && ((p_boot_ins->p_dev_cfg->platform == X_GOLD_221) ||
                                             (p_boot_ins->p_dev_cfg->platform == X_GOLD_222) ||
                                             (p_boot_ins->p_dev_cfg->platform == X_GOLD_223) ||
                                             (p_boot_ins->p_dev_cfg->platform == X_GOLD_225) ||
                                             (p_boot_ins->p_dev_cfg->platform == X_GOLD_716) ||
                                             (p_boot_ins->p_dev_cfg->platform == X_GOLD_726) ||
                                             (p_boot_ins->p_dev_cfg->platform == X_GOLD_631) ||
                                             (p_boot_ins->p_dev_cfg->platform == X_GOLD_632) ||
                                             (p_boot_ins->p_dev_cfg->platform == X_GOLD_633) ||
                                             (p_boot_ins->p_dev_cfg->platform == X_GOLD_636)))
    {
      p_boot_ins->state = boot_state_tx_boot_code;
    }
    else
    {
      if(GLOBAL_usb_device)
      {
        p_boot_ins->state = boot_state_rx_boot_usb_ack;
      }
      else
      {
        p_boot_ins->state = boot_state_rx_boot_ack;
      }
    }
  }
}


/******************************************************************************
* Function:... void BOOT_rx_boot_ack_code(boot_instance_t *p_boot_ins)
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
  uint32_t temp_rx_length;
  p_boot_ins->rx_buffer.max_length = BOOT_ACK_CODE_LENGTH;

  if (COMM_TECH_IS_MIPI(p_boot_ins->channel_id))
  {
    temp_rx_length = GLOBAL_comm_align_bytes[p_boot_ins->channel_id];
  }
  else
  {
    temp_rx_length = p_boot_ins->rx_buffer.max_length;
  }
  p_boot_ins->rx_read_ahead_bytes = 0;

   p_boot_ins->state = boot_state_rx_waiting_boot_ack;
  (void)COMM_receive_buffer(p_boot_ins->channel_id,
                            (char *)p_boot_ins->rx_buffer.buffer,
                            temp_rx_length,
                            &p_boot_ins->rx_buffer.length,
                            BOOT_rx_boot_ack_not);
}

/******************************************************************************
* Function:... void BOOT_rx_boot_ack_code(boot_instance_t *p_boot_ins)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static void BOOT_rx_boot_ack_code_2nd_stage_618_ES2(boot_instance_t *p_boot_ins)
{
  uint32_t rx_temp_length;
  char *rx_temp_buf;
  if ((p_boot_ins->p_dev_cfg->platform == X_GOLD_213) ||
      (p_boot_ins->p_dev_cfg->platform == X_GOLD_215))
  {
    p_boot_ins->rx_buffer.max_length = XG213_BOOT_ACK_CODE_LENGTH - 1;
  }
  else
  {
   if (p_boot_ins->link_est_struct_ver == 0xF1)
   {
     p_boot_ins->rx_buffer.max_length = LINK_EST_RESPONSE_V1_LENGTH + 1;
   }
   else
   {
     p_boot_ins->rx_buffer.max_length = XG618_ES2_BOOT_ACK_CODE_LENGTH + 1;
   }
  }
  p_boot_ins->state = boot_state_rx_waiting_boot_ack;

  rx_temp_buf = (char *)p_boot_ins->rx_buffer.buffer + p_boot_ins->rx_read_ahead_bytes;
  rx_temp_length = p_boot_ins->rx_buffer.max_length  - p_boot_ins->rx_read_ahead_bytes;
  rx_temp_length += rx_temp_length % GLOBAL_comm_align_bytes[p_boot_ins->channel_id];

  (void)COMM_receive_buffer(p_boot_ins->channel_id,
   rx_temp_buf,
   rx_temp_length,
   &p_boot_ins->rx_buffer.length,
   BOOT_rx_boot_ack_not);
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
  boot_status_t boot_status = p_boot_ins->last_status;
  boot_error_t  boot_error = p_boot_ins->last_error;
  comm_status_t comm_status = comm_status_ok;
  int id = p_boot_ins->channel_id;
  unsigned int i;
  uint8_t tx_length;
  uint8_t temp_tx_buf[4];

  comm_status = COMM_flush_rx_buffer(id);
  if (comm_status != comm_status_flush_buffers_ok)
  { /* Error flushing internal RX buffer in comport HW device */
    p_boot_ins->state = boot_state_error;
    boot_status = boot_status_comm_error;
    boot_error = boot_error_comm_buffer_flush_error;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
    BOOT_Err(id, BOOT_ERR_error_comm_buffer_flush_error);
#endif
  }
  else
  {
    /* Transmit the embedded ebl loader or psi depending on the information */
    /* stored in dev_cfg */
    if ((p_boot_ins->p_dev_cfg->psi_offset == NULL) &&
      (p_boot_ins->p_dev_cfg->psi_size == 0))
    {
    if ((p_boot_ins->p_dev_cfg->platform != X_GOLD_213) &&    // Only for 213 allow the use of embedded files.
        (GLOBAL_usb_device ||
        p_boot_ins->p_dev_cfg->platform == X_GOLD_221 ||  // New platforms must have bootcomponents injected
        p_boot_ins->p_dev_cfg->platform == X_GOLD_222 ||  // New platforms must have bootcomponents injected
        p_boot_ins->p_dev_cfg->platform == X_GOLD_223 ||  // New platforms must have bootcomponents injected
        p_boot_ins->p_dev_cfg->platform == X_GOLD_225 ||  // New platforms must have bootcomponents injected
        p_boot_ins->p_dev_cfg->platform == X_GOLD_631 ||
        p_boot_ins->p_dev_cfg->platform == X_GOLD_632 ||
        p_boot_ins->p_dev_cfg->platform == X_GOLD_633 ||
        p_boot_ins->p_dev_cfg->platform == X_GOLD_215))
      {
  #if defined (USE_TEXT_CLASS_IFWD_ERRORS)
        if (GLOBAL_usb_device)
        {
          BOOT_Err(id, BOOT_ERR_missing_psi);
        }
        else
        {
          BOOT_Err(id, BOOT_ERR_missing_psi_p);
        }
  #endif
        if (p_boot_ins->fx != NULL)
        {
          p_boot_ins->p_ebl_version = NULL;
          p_boot_ins->fx(id, EBL_NOT_RUNNING, p_boot_ins->p_ebl_version);
        }
        return;
      }
      else
      {
        p_boot_ins->boot_buffer_allocated = false;

        /* Transmit the embedded ebl loader*/
        switch (p_boot_ins->p_dev_cfg->platform)
        {
        case S_GOLD_CHIPSET_V1:
          p_boot_ins->boot_buffer.buffer = (uint8_t *)fastload;
          p_boot_ins->boot_buffer.max_length = fastloadLength;
          p_boot_ins->boot_buffer.checksum = (uint8_t)fastload[fastloadLength];
          break;

        case S_GOLD_CHIPSET_V2:
          p_boot_ins->boot_buffer.buffer = (uint8_t *)(bootload_sg2 + 2);
          p_boot_ins->boot_buffer.max_length = bootload_sg2Length;
          p_boot_ins->boot_buffer.checksum = (uint8_t)bootload_sg2[bootload_sg2Length + 2];
          break;

        case S_GOLD_CHIPSET_V3:
          p_boot_ins->boot_buffer.buffer = (uint8_t *)(bootload_sg3 + 2);
          p_boot_ins->boot_buffer.max_length = bootload_sg3Length;
          p_boot_ins->boot_buffer.checksum = (uint8_t)bootload_sg3[bootload_sg3Length + 2];
          break;

        case X_GOLD_213:
          p_boot_ins->boot_buffer.buffer = (uint8_t *)(bootload_xgold213 + 2);
          p_boot_ins->boot_buffer.max_length = bootload_xgold213Length;
          p_boot_ins->boot_buffer.checksum = (uint8_t)bootload_xgold213[bootload_xgold213Length + 2];
          break;

        case X_GOLD_626:
        case X_GOLD_618:
          if(p_boot_ins->chip_return_code == BOOT_ACK_NOSEC_XG618_ES2)
          {
            p_boot_ins->boot_buffer.buffer = (uint8_t *)(bootload_xg618_es2 + 2);
            p_boot_ins->boot_buffer.max_length = bootload_xg618_es2Length;
            p_boot_ins->boot_buffer.checksum = (uint8_t)bootload_xg618_es2[bootload_xg618_es2Length + 2];
          }else
          {
            p_boot_ins->boot_buffer.buffer = (uint8_t *)(bootload_xg618 + 2);
            p_boot_ins->boot_buffer.max_length = bootload_xg618Length;
            p_boot_ins->boot_buffer.checksum = (uint8_t)bootload_xg618[bootload_xg618Length + 2];
          }
          break;

        case S_GOLD_RADIO_V1:
          if (p_boot_ins->p_dev_cfg->technology == NAND_FLASH)
          {
            if (p_boot_ins->chip_return_code == BOOT_ACK_NOSEC_SGR_BC_V1)
            {
              p_boot_ins->boot_buffer.buffer = (uint8_t *)&bootload_sgr_mpelite_nand[2];
              p_boot_ins->boot_buffer.max_length = bootload_sgr_mpelite_nandLength;
              p_boot_ins->boot_buffer.checksum = (uint8_t)bootload_sgr_mpelite_nand[bootload_sgr_mpelite_nandLength + 2];
            }
            else
              if (p_boot_ins->chip_return_code == BOOT_ACK_NOSEC_SGR_BC_V2)
              {
                p_boot_ins->boot_buffer.buffer = (uint8_t *)&bootload_sgr_es2_mpelite_nand[2];
                p_boot_ins->boot_buffer.max_length = bootload_sgr_es2_mpelite_nandLength;
                p_boot_ins->boot_buffer.checksum = (uint8_t)bootload_sgr_es2_mpelite_nand[bootload_sgr_mpelite_nandLength + 2];
              }
          }
          else
          {
            switch (p_boot_ins->chip_return_code)
            {
            case BOOT_ACK_NOSEC_SGR_BC_V2:  /* SG-radio ES2 and future ES's use the ES2 H file */
            case BOOT_ACK_NOSEC_SGR_BC_V3:
            case BOOT_ACK_NOSEC_SGR_BC_V4:
            case BOOT_ACK_NOSEC_SGR_BC_V5:
            case BOOT_ACK_NOSEC_SGR_BC_V6:
              {
                p_boot_ins->boot_buffer.buffer = (uint8_t *)(bootload_sgr_es2 + 2);
                p_boot_ins->boot_buffer.max_length = bootload_sgr_es2Length;
                p_boot_ins->boot_buffer.checksum = (uint8_t)bootload_sgr_es2[bootload_sgr_es2Length + 2];
                break;
              }
            default:   /* All other use the normal file */
              {
                p_boot_ins->boot_buffer.buffer = (uint8_t *)(bootload_sgr + 2);
                p_boot_ins->boot_buffer.max_length = bootload_sgrLength;
                p_boot_ins->boot_buffer.checksum = (uint8_t)bootload_sgr[bootload_sgrLength + 2];
              }
            }
          }
          break;

        default:
          p_boot_ins->last_status = boot_status_platform_not_supported;
          boot_status = p_boot_ins->last_status;
          break;
        }
      }
    }
    else
    { /* Transmit the injected psi */
#if defined (USE_TEXT_CLASS_IFWD_INFO)
      BOOT_info(id, BOOT_INFO_inject_ebl_ldr);
#endif

      p_boot_ins->boot_buffer.max_length = p_boot_ins->p_dev_cfg->psi_size;
      p_boot_ins->boot_buffer.checksum = 0;
      if ((p_boot_ins->boot_buffer.buffer != NULL) && (p_boot_ins->boot_buffer_allocated))
        MEM_free (p_boot_ins->boot_buffer.buffer);

      if (((p_boot_ins->p_dev_cfg->psi_size) % 64) == 0)
      {
        // No reminder so the length is a multiple of 64
        // So in case of USB we would in some cases have needed a zero length package
        // To avoid that we adjust the length by 4 and send two extra bytes
        // Four 'random' bytes will be sent but as they are covered by the crc-as calculated below it will not cause error.
        // And there are no 'security' features that rely on the number of bytes transmitted for the PSI load.
        p_boot_ins->p_dev_cfg->psi_size += 4;
     }
      p_boot_ins->boot_buffer.buffer = MEM_malloc(p_boot_ins->p_dev_cfg->psi_size);
      p_boot_ins->boot_buffer_allocated = true;
      p_boot_ins->boot_buffer.max_length = p_boot_ins->p_dev_cfg->psi_size;
      p_boot_ins->boot_buffer.length = 0;
      if (p_boot_ins->p_dev_cfg->psi_offset == NULL)
      {
        MEM_free(p_boot_ins->boot_buffer.buffer);
        p_boot_ins->boot_buffer.buffer    = 0;
        p_boot_ins->boot_buffer.checksum  = 0;
      }
      else
      {
        for (i = 0; i < p_boot_ins->boot_buffer.max_length; i++)
        {
          p_boot_ins->boot_buffer.buffer[i] = *p_boot_ins->p_dev_cfg->psi_offset;
          p_boot_ins->boot_buffer.checksum ^= *p_boot_ins->p_dev_cfg->psi_offset++;
        }
      }
    }

    /* Transmit ebl loader length */
    p_boot_ins->buffer[0] = BOOT_RAMLOAD_IND;
    *(uint32_t*)&p_boot_ins->buffer[1] = *(uint32_t*)&p_boot_ins->boot_buffer.max_length;
//    *(uint16_t*)&p_boot_ins->buffer[1] = *(uint16_t*)&p_boot_ins->boot_buffer.max_length;

    // SMint02152605_t: The XMM2230 bootrom is checking the padding byte for 0xFF, so this is required.
    if ((p_boot_ins->link_est_response.chip_id == BOOT_ACK_NOSEC_XG221) ||
        (p_boot_ins->link_est_response.chip_id == BOOT_ACK_NOSEC_XG222) ||
        (p_boot_ins->link_est_response.chip_id == BOOT_ACK_NOSEC_XG223) ||
        (p_boot_ins->link_est_response.chip_id == BOOT_ACK_NOSEC_XG225) ||
        (p_boot_ins->link_est_response.chip_id == BOOT_ACK_NOSEC_XG631) ||
        (p_boot_ins->link_est_response.chip_id == BOOT_ACK_NOSEC_XG632) ||
        (p_boot_ins->link_est_response.chip_id == BOOT_ACK_NOSEC_XG632_ES1))
    {
      p_boot_ins->buffer[3] = 0xFF;  /* 32bit alignment */
    }

    /* Figure out if there is a need for 32bit alignment */
    tx_length = 3; /* Default */
    if (COMM_TECH_IS_MIPI(p_boot_ins->channel_id))
    {
      tx_length = 4; /* 32bit if MIPI is used */
    }
    else if(p_boot_ins->link_est_response.chip_id == BOOT_ACK_NOSEC_XG626)      // 0x51
    {
      if (p_boot_ins->link_est_response.bc_ver >= 0x93)
      {
        tx_length = 4; /* 32bit align for XG626-ES1.1A+ for both USB and USIF */
      }
      else
      {
        uint8_t minor_ver = (p_boot_ins->link_est_response.snum_3 >> XG626_MINOR_BC_VER_SHIFT) & XG626_MINOR_BC_VER_MASK;
        if (minor_ver >= XG626_MINOR_BC_VER_CODE_ES1_1A)
        {
          tx_length = 4; /* 32bit align for XG626-ES1.1A+ for both USB and USIF */
        }
      }
    }
    else if((p_boot_ins->link_est_response.chip_id == BOOT_ACK_NOSEC_XG221) ||
            (p_boot_ins->link_est_response.chip_id == BOOT_ACK_NOSEC_XG222) ||
            (p_boot_ins->link_est_response.chip_id == BOOT_ACK_NOSEC_XG223) ||
            (p_boot_ins->link_est_response.chip_id == BOOT_ACK_NOSEC_XG225) ||
            (p_boot_ins->link_est_response.chip_id == BOOT_ACK_NOSEC_XG706) ||
            (p_boot_ins->link_est_response.chip_id == BOOT_ACK_NOSEC_XG716) ||
            (p_boot_ins->link_est_response.chip_id == BOOT_ACK_NOSEC_XG726) ||
            (p_boot_ins->link_est_response.chip_id == BOOT_ACK_NOSEC_XG631) ||
            (p_boot_ins->link_est_response.chip_id == BOOT_ACK_NOSEC_XG632) ||
            (p_boot_ins->link_est_response.chip_id == BOOT_ACK_NOSEC_XG632_ES1) ||
            (p_boot_ins->link_est_response.chip_id == BOOT_ACK_NOSEC_XG633) ||
            (p_boot_ins->link_est_response.chip_id == BOOT_ACK_NOSEC_XG636))
    {
      tx_length = 4; /* 32bit align for XG223-EX* and both USB and USIF */
    }
    else if(p_boot_ins->p_dev_cfg->platform > X_GOLD_215) /* Last known type at time of coding */
    {
      tx_length = 4; /* 32bit align for future chips and USB and USIF */
    }

    /* The transmission is ongoing */
    p_boot_ins->state = boot_state_tx_in_progress;
    memcpy(temp_tx_buf, p_boot_ins->buffer, 4);
    if (COMM_TECH_IS_MIPI(p_boot_ins->channel_id))
    {
     BOOT_endian_swap(temp_tx_buf, tx_length);
    }
    COMM_send_buffer(p_boot_ins->channel_id,
                     (char*)temp_tx_buf,
                     tx_length,
                     &p_boot_ins->boot_buffer.length,
                     BOOT_tx_ebl_loader_length_not);
      if (tx_length == 3)
      {
        TR_BEGIN(TR_Tx,TR_boot_tx_load_length,p_boot_ins->channel_id);
        TR_8(p_boot_ins->buffer[0]);            /* uint8_t:  RAMload (hex) */
        TR_16P(&p_boot_ins->buffer[1]);         /* uint16_t: Length  (dec) */
        TR_END;
      }
      else
      {
        TR_BEGIN(TR_Tx,TR_boot_tx_load_length,p_boot_ins->channel_id);
        TR_8(p_boot_ins->buffer[0]);            /* uint8_t:  RAMload (hex) */
        TR_16P(&p_boot_ins->buffer[1]);         /* uint16_t: Length  (dec) */
        TR_8(p_boot_ins->buffer[3]);            /* uint8_t:  RAMload (hex) */
        TR_END;
      }
    /* The transmission is ongoing */
  }

  p_boot_ins->last_status = boot_status;
  p_boot_ins->last_error = boot_error;

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
  uint32_t              x_gold_618_es2_booting = 0;
  uint8_t               temp_ler_ver;

  p_boot_ins = p_boot_manager->boot_handler[id];

  /* This is much easier than the trace log :) */
  Debug(" Boot response [ %s ] \n", getHex(p_boot_ins->rx_buffer.buffer, size) );
  if (COMM_TECH_IS_MIPI(p_boot_ins->channel_id))
  {
    BOOT_endian_swap(p_boot_ins->rx_buffer.buffer + p_boot_ins->rx_read_ahead_bytes, size);
  }
  temp_ler_ver = p_boot_ins->rx_buffer.buffer[0];
  if ((temp_ler_ver == 0xF0) ||
      (temp_ler_ver == 0xF1))
  {
    TIMER_stop_timer(&p_boot_ins->tx_boot_code_timer);
    p_boot_ins->link_est_struct_ver = temp_ler_ver;
    p_boot_ins->fx(id, BOOT_SYNC, p_boot_ins->p_ebl_version);  // Callback to indicate sync to SFH
    p_boot_ins->state = boot_state_rx_2nd_boot_response;
    x_gold_618_es2_booting = 0xFF;
    p_boot_ins->waiting_for_chip_id = 0xFF;

    TR_BEGIN(TR_Rx,TR_boot_rx_ack_byte0,p_boot_ins->channel_id);
    TR_DATA(p_boot_ins->rx_buffer.buffer, size);  /* ACK: uint8_t (hex) */
    TR_END;
    p_boot_ins->rx_read_ahead_bytes = size - p_boot_ins->rx_buffer.max_length;
    if (COMM_TECH_IS_MIPI(p_boot_ins->channel_id))
    {
      BOOT_shift_buf_left(p_boot_ins->rx_buffer.buffer,p_boot_ins->rx_buffer.max_length,size);
    }
  }
  else
  {
    p_boot_ins->rx_read_ahead_bytes = 0;
    if (p_boot_ins->waiting_for_chip_id == 0xFF )
    {
      if (p_boot_ins->p_dev_cfg->platform == X_GOLD_213)
      {
        p_boot_ins->chip_return_code = p_boot_ins->rx_buffer.buffer[2];
      }
      else
      {
        p_boot_ins->chip_return_code = p_boot_ins->rx_buffer.buffer[1];
      }
      x_gold_618_es2_booting = 0;
    }
    else
    {
      p_boot_ins->chip_return_code = p_boot_ins->rx_buffer.buffer[0];
    }
  }
  if (x_gold_618_es2_booting == 0)
  {
    TR_BEGIN(TR_Rx,TR_boot_rx_ack_byte,p_boot_ins->channel_id);
    TR_DATA(p_boot_ins->rx_buffer.buffer, size);  /* ACK: uint8_t (hex) */
    TR_END;

    /* Copying the Target Link Response */
    memcpy((void*)&p_boot_ins->link_est_response, (const void*)(p_boot_ins->rx_buffer.buffer), sizeof(p_boot_ins->link_est_response));
    /* Printing out the above structure: */
    Debug3("\nJust received target information (size: 0x%02X):\n", sizeof(boot_link_est_response_t));
    Debug3("Size:     0x%02X\n", p_boot_ins->link_est_response.size);
    Debug3("Chip ID:  0x%02X\n", p_boot_ins->link_est_response.chip_id);
    Debug3("Version:  0x%02X\n", p_boot_ins->link_est_response.bc_ver);
    Debug3("Bo Conf:  0x%02X\n", p_boot_ins->link_est_response.bo_cfg);
    Debug3("Sec lvl:  0x%02X\n", p_boot_ins->link_est_response.sec_lvl);
    Debug3("FL Stat:  0x%02X\n", p_boot_ins->link_est_response.fl_stat);
    Debug3("CusConf:  0x%02X\n", p_boot_ins->link_est_response.cus_cfg);
    Debug3("Num(X,Y): 0x%02X , 0x%02X\n", p_boot_ins->link_est_response.num_x, p_boot_ins->link_est_response.num_y);
    Debug3("SNUM:     [ 0x%X 0x%X 0x%X ] \n\n", p_boot_ins->link_est_response.snum_1,
           p_boot_ins->link_est_response.snum_2, p_boot_ins->link_est_response.snum_3);


    if (DEVICE_get_imc_name(NO_ID, NO_ID, p_boot_ins->link_est_response.chip_id, p_boot_ins->p_dev_cfg->platform) == NULL)
    {
      char warning[100];
      const char* HW = DEVICE_get_imc_name(NO_ID, NO_ID, p_boot_ins->link_est_response.chip_id, NO_ID);
      const char* SW = DEVICE_get_imc_name(NO_ID, NO_ID, NO_ID, p_boot_ins->p_dev_cfg->platform);
      sprintf(warning,"Warning: Chip mismatch. HW is [%s]  SW expects [%s]",HW ? HW : "None", SW ? SW : "None");
      GLOBAL_callback(p_boot_ins->channel_id, IFWD_DL_ProcessOutlineUpdated, warning);

    }

#if defined (USE_TEXT_CLASS_IFWD_INFO)
    BOOT_show(p_boot_ins->channel_id, BOOT_INFO_target_sync);
#endif

    // HAndle pre boot
    if (GLOBAL_pre_boot[p_boot_ins->channel_id] == PRE_BOOT_ARMED)
    {
      p_boot_ins->state = boot_state_pre_boot_ok;
      GLOBAL_pre_boot[p_boot_ins->channel_id] = PRE_BOOT_DONE;
    }
    else
    {
      p_boot_ins->state = boot_state_act_on_chip_response;
    }
      // ************************************
  }
}

static void act_on_boot_response(uint8_t id)
{
  boot_instance_t *p_boot_ins;
  boot_status_t    boot_status = boot_status_error;
  boot_error_t     boot_error = boot_error_unknown;

  p_boot_ins = p_boot_manager->boot_handler[id];

  switch (p_boot_ins->chip_return_code)
  {
    case BOOT_ACK_NOSEC_SG1:
    case BOOT_ACK_NOSEC_SG2:
    case BOOT_ACK_NOSEC_SG3_BC_V1:
    case BOOT_ACK_NOSEC_SG3_BC_V2:
    case BOOT_ACK_NOSEC_SG3_BC_V3:
    case BOOT_ACK_NOSEC_SG3_BC_V4:
    case BOOT_ACK_NOSEC_SG3_BC_V5:
    case BOOT_ACK_NOSEC_SG3_BC_V6:
    case BOOT_ACK_NOSEC_SGR_BC_V1:
    case BOOT_ACK_NOSEC_SGR_BC_V2:
    case BOOT_ACK_NOSEC_SGR_BC_V3:
    case BOOT_ACK_NOSEC_SGR_BC_V4:
    case BOOT_ACK_NOSEC_SGR_BC_V5:
    case BOOT_ACK_NOSEC_SGR_BC_V6:
    case BOOT_ACK_NOSEC_XGOLD213_ES1:
    case BOOT_ACK_NOSEC_XGOLD213_ES2:
    case BOOT_ACK_NOSEC_XGOLD213_ES3:
    case BOOT_ACK_NOSEC_XG618_ES1:
      // Check the CID for the RPSI and target HW only if in certificate mode
      if (((p_boot_ins->link_est_response.sec_lvl >> SECLVL_LSHIFT) & SECLVL_MASK) == SECLVL_CERTIFICATE_MODE)
      {
        uint8_t sw_num_x, sw_num_y, sw_cus_cfg;
        uint8_t hw_num_x, hw_num_y, hw_cus_cfg;
        char sw_cid[CID_STR_LEN];
        char hw_cid[CID_STR_LEN];

        // Get the CID from the RPSI  - Values are found at fixed offset from start of the RPSI
        sw_num_x   = ((*(p_boot_ins->p_dev_cfg->psi_offset + RPSI_SECX_OFFSET))   >> RPSI_SECX_LSHIFT)   & SECX_MASK;
        sw_num_y   = ((*(p_boot_ins->p_dev_cfg->psi_offset + RPSI_SECY_OFFSET))   >> RPSI_SECY_LSHIFT)   & SECY_MASK;
        sw_cus_cfg = ((*(p_boot_ins->p_dev_cfg->psi_offset + RPSI_CUSCFG_OFFSET)) >> RPSI_CUSCFG_LSHIFT) & CUSCFG_MASK ;
        snprintf(sw_cid,CID_STR_LEN,"%d%d%d",sw_num_x, sw_num_y, sw_cus_cfg);

        // Get the CID from the Link Establishment Response - from the target
        hw_num_x   =  p_boot_ins->link_est_response.num_x   & SECX_MASK;
        hw_num_y   =  p_boot_ins->link_est_response.num_y   & SECY_MASK;
        hw_cus_cfg =  p_boot_ins->link_est_response.cus_cfg & CUSCFG_MASK;
        snprintf(hw_cid,CID_STR_LEN,"%d%d%d",hw_num_x, hw_num_y, hw_cus_cfg);

        // Call-back to print out the CIDs on the MMI log screen. Use Outline mode as it shall be visible even if detailed
        GLOBAL_callback(p_boot_ins->channel_id, IFWD_DL_ProcessOutlineUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, BOOT_INFO_RPSI_CID, sw_cid));
        GLOBAL_callback(p_boot_ins->channel_id, IFWD_DL_ProcessOutlineUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, BOOT_INFO_HW_CID,   hw_cid));

        // Compare the two CID values
        if (memcmp(sw_cid, hw_cid, CID_STR_LEN))
        {
          // Error .. no match
          boot_status = boot_status_error;
          p_boot_ins->state = boot_state_error;
          BOOT_Err(p_boot_ins->channel_id, BOOT_ERR_CID_mismatch);
          break;
        }
      }

      if(p_boot_ins->boot_time == 0) // no boot time out?
      {
        p_boot_ins->boot_time = BOOT_TIME_EBL_TRANSFER;  // well we syncronized, so set a timeout anyway in case we fail
        TIMER_new_timer(&p_boot_ins->boot_timer, p_boot_ins->boot_time);
      }

      p_boot_ins->state = boot_state_tx_ebl_loader;
      boot_status = boot_status_booting_device;
      boot_error = boot_error_no_error;
      break;

    case BOOT_ACK_NOSEC_XG618_ES2:
    case BOOT_ACK_NOSEC_XG626:
    case BOOT_ACK_NOSEC_XG631:
    case BOOT_ACK_NOSEC_XG632:
    case BOOT_ACK_NOSEC_XG632_ES1:
    case BOOT_ACK_NOSEC_XG633:
    case BOOT_ACK_NOSEC_XG636:
    case BOOT_ACK_NOSEC_XG706:
    case BOOT_ACK_NOSEC_XG716:
    case BOOT_ACK_NOSEC_XG726:
    case BOOT_ACK_NOSEC_XG221:
    case BOOT_ACK_NOSEC_XG222:
    case BOOT_ACK_NOSEC_XG223:
    case BOOT_ACK_NOSEC_XG225:
    case BOOT_ACK_NOSEC_XG215:
    {
//      p_boot_ins->p_dev_cfg->cfg->Platform = X_GOLD_618_ES2;  //No ES2 platform defined, use chip_return_code
      boot_status = boot_status_booting_device;
      boot_error = boot_error_no_error;
      p_boot_ins->waiting_for_chip_id = 0xFF;


      if(p_boot_ins->boot_time == 0) // no boot time out?
      {
        p_boot_ins->boot_time = BOOT_TIME_EBL_TRANSFER;  // well we syncronized, so set a timeout anyway in case we fail
        TIMER_new_timer(&p_boot_ins->boot_timer, p_boot_ins->boot_time);
      }

#ifndef ANDROID
    if (GLOBAL_NVM_flashless_path)
    {
      Debug("Setting up NVM FlashLess path (%s, Options: %X)\n",
            GLOBAL_NVM_flashless_path, GLOBAL_NVM_flashless_options);

      volatile uint8_t  temp_crc; // CRC is calculated when the EBL is transmitted
      uint32_t result;
      //FlashlessSetVerbose(true);    //< Debugging FlashLess
      if (secure_channel_cb_fx) // If secure channel is enabled then EBL shall not be modified.
      {
          // uint8_t **EblData = NULL;
          // uint8_t *EblCrc = NULL;
          // uint32_t *EblLength = NULL;

          result = FlashlessSetPath(p_boot_ins->rx_buffer.buffer,
                           (uint8_t**)&p_boot_ins->p_dev_cfg->boot_filename,
                           GLOBAL_NVM_flashless_path,
                           GLOBAL_NVM_flashless_options,
                           NULL,
                           NULL,
                           NULL);
      }
      else
      {
         result = FlashlessSetPath(p_boot_ins->rx_buffer.buffer,
                          (uint8_t**)&p_boot_ins->p_dev_cfg->boot_filename,
                          GLOBAL_NVM_flashless_path,
                          GLOBAL_NVM_flashless_options,
                          &p_boot_ins->p_dev_cfg->ebl_offset,
                          (uint8_t*)&temp_crc,
                          &p_boot_ins->p_dev_cfg->ebl_size);
      }

      if (!result)
      {
        char fl_error[500];
        boot_status = boot_status_error;
        p_boot_ins->state = boot_state_error;
        GLOBAL_callback(p_boot_ins->channel_id, IFWD_DL_Error, FlashlessGetErrorString(fl_error, 500));
        break;
      }
    }
    // Here we are handling flashless also in case of secure channel
    // So handle here if the loading of RPSI should be skipped.
    if (GLOBAL_skip_sw_load_if_sys_val_ok
        && ((p_boot_ins->link_est_response.fl_stat == 21)
             || (p_boot_ins->link_est_response.fl_stat == 28)))
    {
      // Set state to request boot rom to skip load of PSI.
      p_boot_ins->state = boot_state_rom_skip_psi_load;
      boot_status = boot_status_booting_device;
      boot_error = boot_error_no_error;
      break;
    }
#endif

      p_boot_ins->state = boot_state_tx_ebl_loader;
      boot_status = boot_status_booting_device;
      boot_error = boot_error_no_error;
      break;
    }

    case BOOT_ACK_USB_SG3_NORMAL_EXT_EBL:
    {
      if ((p_boot_ins->p_dev_cfg->platform == S_GOLD_CHIPSET_V3) ||
          (p_boot_ins->p_dev_cfg->platform == X_GOLD_716) ||
          (p_boot_ins->p_dev_cfg->platform == X_GOLD_726) ||
          (p_boot_ins->p_dev_cfg->platform == X_GOLD_626) ||
          (p_boot_ins->p_dev_cfg->platform == X_GOLD_636) ||
          (p_boot_ins->p_dev_cfg->platform == X_GOLD_213) ||
          (p_boot_ins->p_dev_cfg->platform == X_GOLD_221) ||
          (p_boot_ins->p_dev_cfg->platform == X_GOLD_222) ||
          (p_boot_ins->p_dev_cfg->platform == X_GOLD_223) ||
          (p_boot_ins->p_dev_cfg->platform == X_GOLD_225) ||
          (p_boot_ins->p_dev_cfg->platform == X_GOLD_631) ||
          (p_boot_ins->p_dev_cfg->platform == X_GOLD_632) ||
          (p_boot_ins->p_dev_cfg->platform == X_GOLD_633))
      {
        if(p_boot_ins->boot_time == 0) // no boot time out?
        {
          p_boot_ins->boot_time = BOOT_TIME_EBL_TRANSFER;  // well we synchronized, so set a timeout anyway in case we fail
          TIMER_new_timer(&p_boot_ins->boot_timer, p_boot_ins->boot_time);
        }

        p_boot_ins->state = boot_state_rx_ebl_loader_ready;
        boot_status = boot_status_booting_device;
        boot_error = boot_error_no_error;
      }
      break;
    }

    case BOOT_ACK_USB_NORMAL:

      if(p_boot_ins->boot_time == 0) // no boot time out?
      {
        p_boot_ins->boot_time = 10000;  // well we syncronized, so set a timeout anyway in case we fail
        TIMER_new_timer(&p_boot_ins->boot_timer, p_boot_ins->boot_time);
      }

      p_boot_ins->state = boot_state_rx_ebl_version;
      boot_status = boot_status_booting_device;
      boot_error = boot_error_no_error;
      break;

    default:
      p_boot_ins->last_state = p_boot_ins->state;
      p_boot_ins->state = boot_state_tx_boot_code;
      break;
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

  if(GLOBAL_usb_device)
  {
//    comm_status = comm_status_flush_buffers_ok;
  }
  else
  {
    comm_status = COMM_flush_rx_buffer(p_boot_ins->channel_id);
    if (comm_status != comm_status_flush_buffers_ok)
    {
      comm_status = COMM_flush_tx_buffer(p_boot_ins->channel_id);
    }

    if (comm_status != comm_status_flush_buffers_ok)
    {
      boot_status = boot_status_error;
      #if defined (USE_TEXT_CLASS_IFWD_ERRORS)
        BOOT_Err(p_boot_ins->channel_id, BOOT_ERR_error_comm_buffer_flush_error);
      #endif
    }
  }

  /* TX at stream and start boot timer */
  memcpy(p_boot_ins->buffer, "ATAT\0", 5);
  p_boot_ins->tx_buffer.buffer = p_boot_ins->buffer;

  if (GLOBAL_comm_technology[p_boot_ins->channel_id])
  {
    p_boot_ins->tx_buffer.max_length = MAX_SGOLD_USB_BOOT_BUFFER_LENGTH;
  }
  else if (GLOBAL_usb_device)
  {
    if ((p_boot_ins->p_dev_cfg->platform == X_GOLD_221) ||
        (p_boot_ins->p_dev_cfg->platform == X_GOLD_222) ||
        (p_boot_ins->p_dev_cfg->platform == X_GOLD_223) ||
        (p_boot_ins->p_dev_cfg->platform == X_GOLD_225) ||
        (p_boot_ins->p_dev_cfg->platform == X_GOLD_716) ||
        (p_boot_ins->p_dev_cfg->platform == X_GOLD_726) ||
        (p_boot_ins->p_dev_cfg->platform == X_GOLD_631) ||
        (p_boot_ins->p_dev_cfg->platform == X_GOLD_632) ||
        (p_boot_ins->p_dev_cfg->platform == X_GOLD_633) ||
        (p_boot_ins->p_dev_cfg->platform == X_GOLD_636))
    {
      p_boot_ins->tx_buffer.max_length = 2;
    }
    else
    {
      p_boot_ins->tx_buffer.max_length = MAX_SGOLD_USB_BOOT_BUFFER_LENGTH;
    }
  }
  else
  {
  #if defined(PES_DEBUG_ONE_AT)
    p_boot_ins->tx_buffer.max_length = MAX_SGOLD_USB_BOOT_BUFFER_LENGTH; // Send ATAT
  #else
    p_boot_ins->tx_buffer.max_length = MAX_SGOLD_BOOT_BUFFER_LENGTH;
  #endif
  }


  //p_boot_ins->state = boot_state_tx_boot_code;
  p_boot_ins->tx_boot_code_time = SEND_NEXT_BOOT_CODE_TIME;   // 20ms delay
  TIMER_new_timer(&p_boot_ins->tx_boot_code_timer, p_boot_ins->tx_boot_code_time);

  p_boot_ins->state = boot_state_tx_waiting_boot_ack;
  p_boot_ins->tx_buffer.length = 0;
  if (COMM_TECH_IS_MIPI(p_boot_ins->channel_id))
  {
    BOOT_endian_swap(p_boot_ins->tx_buffer.buffer, p_boot_ins->tx_buffer.max_length);
  }

  COMM_send_buffer(p_boot_ins->channel_id,
      (char *)p_boot_ins->tx_buffer.buffer,
      p_boot_ins->tx_buffer.max_length,
      &p_boot_ins->tx_buffer.length,
      BOOT_tx_boot_code_not);

  if (p_boot_ins->fx != NULL)
  {
    p_boot_ins->fx(p_boot_ins->channel_id, BOOT_START, p_boot_ins->p_ebl_version);
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
      GLOBAL_is_in_time_critical_section = false;
      if (COMM_TECH_IS_MIPI(p_boot_ins->channel_id))
      {
        BOOT_endian_swap(p_boot_ins->rx_buffer.buffer, size);
      }
      p_boot_ins->rx_buffer.length = size;

      TR_BEGIN(TR_Rx,TR_boot_rx_ebl_loader_checksum_result,p_boot_ins->channel_id);
      TR_DATA(p_boot_ins->rx_buffer.buffer, size);           /* Checksum (uint8_t) hex */
      TR_END;

      switch (p_boot_ins->rx_buffer.buffer[0])
      {
      case BOOT_CHKSUM_OK_SG1:
      case BOOT_ACK_USB_NORMAL:
        p_boot_ins->state = boot_state_rx_ebl_version;
        boot_status = boot_status_booting_device;
        boot_error = boot_error_no_error;
        break;

      case BOOT_CHKSUM_OK_SG2:
      case BOOT_CHKSUM_OK_SG3_SGR:
        p_boot_ins->last_state = p_boot_ins->state;
        p_boot_ins->state = boot_state_rx_ebl_loader_ready;
        boot_status = boot_status_booting_device;
        boot_error = boot_error_no_error;
        break;

      default:
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
  uint32_t temp_tx_length;
  uint8_t  *temp_tx_buf_ptr;
  uint32_t temp_tx_buf[3]; // room to hold MIPI header and the length, plus the MIPI header for the payload.

  if ((p_boot_ins->boot_buffer.buffer != NULL) && (p_boot_ins->boot_buffer_allocated))
  {
    MEM_free (p_boot_ins->boot_buffer.buffer);
    p_boot_ins->boot_buffer.buffer = NULL;
    p_boot_ins->boot_buffer_allocated = false;
  }
  /* Transmit the embedded ebl or the injected ebl depending on the */
  /* information stored in p_dev_cfg */
  /* Transmit the embedded ebl*/

  /* Transmit the embedded ebl or injected ebl*/
  if ((p_boot_ins->p_dev_cfg->ebl_offset == NULL) &&
    (p_boot_ins->p_dev_cfg->ebl_size == 0))
  {
    if ((p_boot_ins->p_dev_cfg->platform != X_GOLD_213) &&    // Only for 213 allow the use of embedded files.
        (GLOBAL_usb_device ||
        p_boot_ins->p_dev_cfg->platform == X_GOLD_221 ||  // New platforms must have bootcomponents injected
        p_boot_ins->p_dev_cfg->platform == X_GOLD_222 ||  // New platforms must have bootcomponents injected
        p_boot_ins->p_dev_cfg->platform == X_GOLD_223 ||  // New platforms must have bootcomponents injected
        p_boot_ins->p_dev_cfg->platform == X_GOLD_225 ||  // New platforms must have bootcomponents injected
        p_boot_ins->p_dev_cfg->platform == X_GOLD_631 ||
        p_boot_ins->p_dev_cfg->platform == X_GOLD_632 ||
        p_boot_ins->p_dev_cfg->platform == X_GOLD_633 ||
        p_boot_ins->p_dev_cfg->platform == X_GOLD_215))
    {
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
      if (GLOBAL_usb_device)
      {
        BOOT_Err(p_boot_ins->channel_id, BOOT_ERR_missing_ebl);
      }
      else
      {
        BOOT_Err(p_boot_ins->channel_id, BOOT_ERR_missing_ebl_p);
      }
#endif
      if (p_boot_ins->fx != NULL)
      {
        p_boot_ins->p_ebl_version = NULL;
        p_boot_ins->fx(p_boot_ins->channel_id, EBL_NOT_RUNNING, p_boot_ins->p_ebl_version);
      }

      return;
    }
    else
    {
      p_boot_ins->boot_buffer_allocated = false;
      /* Transmit the embedded ebl loader*/
      switch (p_boot_ins->p_dev_cfg->platform)
      {
      case S_GOLD_CHIPSET_V2:
        p_boot_ins->boot_buffer.buffer = (uint8_t *)&ebl_sg2[2];
        p_boot_ins->boot_buffer.max_length = ebl_sg2Length;
        p_boot_ins->boot_buffer.checksum = (uint8_t)ebl_sg2[ebl_sg2Length + 2];
        break;

      case S_GOLD_CHIPSET_V3:
        if (p_boot_ins->p_dev_cfg->technology == NAND_FLASH)
        {
          p_boot_ins->boot_buffer.buffer = (uint8_t *)&ebl_sg3_nand[2];
          p_boot_ins->boot_buffer.max_length = ebl_sg3_nandLength;
          p_boot_ins->boot_buffer.checksum = (uint8_t)ebl_sg3_nand[ebl_sg3_nandLength + 2];
        }
        else
        {
          p_boot_ins->boot_buffer.buffer = (uint8_t *)&ebl_sg3[2];
          p_boot_ins->boot_buffer.max_length = ebl_sg3Length;
          p_boot_ins->boot_buffer.checksum = (uint8_t)ebl_sg3[ebl_sg3Length + 2];
        }
        break;

      case S_GOLD_RADIO_V1:
        p_boot_ins->boot_buffer.buffer = (uint8_t *)&ebl_sgr[2];
        p_boot_ins->boot_buffer.max_length = ebl_sgrLength;
        p_boot_ins->boot_buffer.checksum = (uint8_t)ebl_sgr[ebl_sgrLength + 2];
        break;

      case X_GOLD_213:
        p_boot_ins->boot_buffer.buffer = (uint8_t *)&ebl_xgold213[2];
        p_boot_ins->boot_buffer.max_length = ebl_xgold213Length;
        p_boot_ins->boot_buffer.checksum = (uint8_t)ebl_xgold213[ebl_xgold213Length + 2];
        break;

      case X_GOLD_636:
      case X_GOLD_706:
      case X_GOLD_716:
      case X_GOLD_726:
      case X_GOLD_626:
      case X_GOLD_618:
        if (p_boot_ins->chip_return_code == BOOT_ACK_NOSEC_XG618_ES2)
        {
          p_boot_ins->boot_buffer.buffer = (uint8_t *)&ebl_xg618_es2[2];
          p_boot_ins->boot_buffer.max_length = ebl_xg618_es2Length;
          p_boot_ins->boot_buffer.checksum = (uint8_t)ebl_xg618_es2[ebl_xg618_es2Length + 2];
        }else
        {
          p_boot_ins->boot_buffer.buffer = (uint8_t *)&ebl_xg618[2];
          p_boot_ins->boot_buffer.max_length = ebl_xg618Length;
          p_boot_ins->boot_buffer.checksum = (uint8_t)ebl_xg618[ebl_xg618Length + 2];
        }
        break;

      default:
        p_boot_ins->state = boot_state_error;
        p_boot_ins->last_status = boot_status_platform_not_supported;
      }
    }
  }
  else if ((p_boot_ins->p_dev_cfg->ebl_offset != NULL) &&
           (p_boot_ins->p_dev_cfg->ebl_size != 0))

  { /* Load injected ebl */
#if defined (USE_TEXT_CLASS_IFWD_INFO)
    BOOT_info(p_boot_ins->channel_id, BOOT_INFO_inject_ebl);
#endif

    if ((p_boot_ins->boot_buffer.buffer != NULL) && (p_boot_ins->boot_buffer_allocated))
      MEM_free (p_boot_ins->boot_buffer.buffer);

    // Secure channel handling start
    if (secure_channel_cb_fx) // Handle secure channel if the call-back pointer is not NULL
    {
      uint32_t cb_err_code   = SECURE_CH_RES_SUCCESS;
      size_t cb_data_length = 0;
      uint32_t cb_data_type = 0;
      uint8_t  *cb_data_ptr = NULL;

      if (GLOBAL_use_sec_ch_ext_cb[p_boot_ins->channel_id])
      {
        uint32_t snum[3];

        // Setup to send the SNUM value
        cb_data_type   = SECURE_CH_DATA_SNUM;
        cb_data_length = sizeof(snum);
        cb_data_ptr    = (uint8_t*)&snum;
// Fixing LINT warning
        snum[0] = p_boot_ins->link_est_response.snum_1;
        snum[1] = p_boot_ins->link_est_response.snum_2;
        snum[2] = p_boot_ins->link_est_response.snum_3;
//      memcpy((void*) snum, (const void*)&p_boot_ins->link_est_response.snum_1, cb_data_length); // LINT dont like that we 3 uint32_t from a source that is a single uint32_t :-|
        Debug("SNUM Calling call-back type=0x%08X  length=0x%08X\n, SNUM[0]=0x%08X", cb_data_type, cb_data_length, snum[0]);

        cb_err_code = secure_channel_cb_fx(&cb_data_type, &cb_data_length, &cb_data_ptr );

        Debug("SNUM returned from call-back\n");

        sec_generate_cert_block(SEC_CERT_INJECT,&cb_data_length,&cb_data_ptr);
      }
      sec_generate_cert_block(SEC_CERT_AREA_READ,&cb_data_length,&cb_data_ptr);


      // Expect the TLV structure to be returned via the parameters.
      if (cb_data_length && (cb_data_ptr != NULL))
      {
        Debug("TLV size = 0x%08X\n", cb_data_length);

        // call-back parameters holds the TLV ... prepend it to the EBL
        p_boot_ins->p_dev_cfg->ebl_size += cb_data_length; // Size of TLV is added to the EBL size

        // Maloc space for the EBL and the TLV structure
        p_boot_ins->boot_buffer.buffer = MEM_malloc(p_boot_ins->p_dev_cfg->ebl_size + cb_data_length);
        p_boot_ins->boot_buffer_allocated = true;
        p_boot_ins->boot_buffer.max_length = p_boot_ins->p_dev_cfg->ebl_size + cb_data_length;
        p_boot_ins->boot_buffer.checksum = 0;

        // Copy the TLV structure first
        for (i = 0; i < cb_data_length; i++)
        {
          p_boot_ins->boot_buffer.buffer[i] = *cb_data_ptr;
          p_boot_ins->boot_buffer.checksum ^= *cb_data_ptr++;
        }

        // Copy the EBL
        for (i = 0; i < p_boot_ins->p_dev_cfg->ebl_size; i++)
        {
          p_boot_ins->boot_buffer.buffer[(uint32_t)(i+cb_data_length)] = *p_boot_ins->p_dev_cfg->ebl_offset;
          p_boot_ins->boot_buffer.checksum                ^= *p_boot_ins->p_dev_cfg->ebl_offset++;
        }

        Debug("Total part checksum 0x%02X",p_boot_ins->boot_buffer.checksum);

        // Call to free data
        cb_data_type   = SECURE_CH_DATA_FREE_RETURN_DATA;
        cb_data_length = 0;
        cb_data_ptr    = NULL;
        (void) secure_channel_cb_fx(&cb_data_type, &cb_data_length, &cb_data_ptr );
        sec_generate_cert_block(SEC_CERT_AREA_FREE,&cb_data_length,&cb_data_ptr);
      }
      else
      {
        // Error reported from secure channel call-back function
        // TODO transfer the error code to the error message.
        // TODO give error details in case type, length or ptr fails.
        Debug("SNUM CallBack error 0x%02X",cb_err_code);
        p_boot_ins->state = boot_state_error;
        BOOT_Err(p_boot_ins->channel_id, BOOT_ERR_secure_channel_cb_fail);
        return;  // Do not just continue
      }
    } // Secure channel handling end
    else
    {
      p_boot_ins->boot_buffer.buffer = MEM_malloc(p_boot_ins->p_dev_cfg->ebl_size);
      p_boot_ins->boot_buffer_allocated = true;
      p_boot_ins->boot_buffer.max_length = p_boot_ins->p_dev_cfg->ebl_size;
      p_boot_ins->boot_buffer.checksum = 0;

      if (p_boot_ins->p_dev_cfg->ebl_offset == NULL)
      {
        MEM_free(p_boot_ins->boot_buffer.buffer);
        p_boot_ins->boot_buffer.buffer    = 0;
        p_boot_ins->boot_buffer.checksum  = 0;
      }
      else
      {
        for (i = 0; i < p_boot_ins->boot_buffer.max_length; i++)
        {
          p_boot_ins->boot_buffer.buffer[i] = *p_boot_ins->p_dev_cfg->ebl_offset;
          p_boot_ins->boot_buffer.checksum ^= *p_boot_ins->p_dev_cfg->ebl_offset++;
        }
      }
    }
  }

  ////////
  else
  {
      p_boot_ins->state = boot_state_error;
      p_boot_ins->last_status = boot_status_platform_not_supported;
      return;
  }
  ///////
  p_boot_ins->state = boot_state_tx_in_progress;

  temp_tx_buf_ptr = (uint8_t*)&p_boot_ins->boot_buffer.max_length;
  temp_tx_length = p_boot_ins->nof_ebl_length_bytes;

  if (COMM_TECH_IS_MIPI_ADD_HEADER(p_boot_ins->channel_id))
  {
    temp_tx_buf[0] = p_boot_ins->nof_ebl_length_bytes + (p_boot_ins->nof_ebl_length_bytes % GLOBAL_comm_align_bytes[p_boot_ins->channel_id]);
    temp_tx_buf[1] = p_boot_ins->boot_buffer.max_length;
    temp_tx_buf[2] = p_boot_ins->boot_buffer.max_length + (p_boot_ins->boot_buffer.max_length % GLOBAL_comm_align_bytes[p_boot_ins->channel_id]) + sizeof(uint32_t); // Add MIPI header for the payload now.
    temp_tx_buf_ptr = (uint8_t*)temp_tx_buf;
    temp_tx_length  = sizeof(temp_tx_buf);
  }
  /* Transmit ebl loader length */
  (void)COMM_send_buffer(p_boot_ins->channel_id,
                          (char *)temp_tx_buf_ptr,
                          temp_tx_length,
                          &p_boot_ins->boot_buffer.length,
                          BOOT_tx_ebl_length_not);

  TR_BEGIN(TR_Tx,TR_boot_tx_ebl_loader_length,p_boot_ins->channel_id);
  TR_32(p_boot_ins->boot_buffer.max_length);     /* Length (uint32_t) dec */
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
  uint32_t temp_tx_length;

  p_boot_ins->state = boot_state_tx_in_progress;

  temp_tx_length = p_boot_ins->boot_buffer.max_length + (p_boot_ins->boot_buffer.max_length % GLOBAL_comm_align_bytes[p_boot_ins->channel_id]);

  (void)COMM_send_buffer(p_boot_ins->channel_id,
                        (char *)p_boot_ins->boot_buffer.buffer,
                        temp_tx_length,
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
  uint32_t temp_tx_buf[1];
  uint8_t  *temp_tx_buf_ptr;
  uint32_t temp_tx_length;

  p_boot_ins->state = boot_state_tx_in_progress;
  p_boot_ins->boot_buffer.max_length = 1;

  temp_tx_length = p_boot_ins->boot_buffer.max_length;
  temp_tx_buf_ptr = &p_boot_ins->boot_buffer.checksum;

  if (COMM_TECH_IS_MIPI(p_boot_ins->channel_id))
  {
    temp_tx_buf[0] = (uint32_t)p_boot_ins->boot_buffer.checksum;
    temp_tx_buf_ptr = (uint8_t*)temp_tx_buf;
    temp_tx_length  = sizeof(temp_tx_buf);
  }

  GLOBAL_is_in_time_critical_section = true;

  (void)COMM_send_buffer(p_boot_ins->channel_id,
                        (char *)temp_tx_buf_ptr,
                        temp_tx_length,
                        &p_boot_ins->boot_buffer.length,
                        BOOT_tx_ebl_checksum_not);

  TR_BEGIN(TR_Tx,TR_boot_tx_ebl_checksum,p_boot_ins->channel_id);
  TR_8(p_boot_ins->boot_buffer.checksum);
  TR_END;

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
  uint32_t temp_rx_length;

  p_boot_ins->rx_buffer.max_length = EBL_CHECKSUM_LENGTH;
  p_boot_ins->state = boot_state_rx_in_progress;

  temp_rx_length = p_boot_ins->rx_buffer.max_length;

  if(COMM_TECH_IS_MIPI_ADD_HEADER(p_boot_ins->channel_id))
    temp_rx_length += sizeof(uint32_t);

  if (COMM_TECH_IS_MIPI(p_boot_ins->channel_id))
    temp_rx_length += temp_rx_length % GLOBAL_comm_align_bytes[p_boot_ins->channel_id];

  (void)COMM_receive_buffer(p_boot_ins->channel_id,
                            (char *)p_boot_ins->rx_buffer.buffer,
                            temp_rx_length,
                            &p_boot_ins->rx_buffer.length,
                            BOOT_rx_ebl_checksum_not);

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
  uint32_t temp_rx_length;
  p_boot_ins->rx_buffer.max_length = EBL_VERSION_LENGTH;
  if(p_boot_ins->p_ebl_version)
  {
    p_boot_ins->state = boot_state_rx_in_progress;
    temp_rx_length = p_boot_ins->rx_buffer.max_length;

    if (COMM_TECH_IS_MIPI_ADD_HEADER(p_boot_ins->channel_id))
    {
      temp_rx_length += sizeof(uint32_t);
    }

    if (COMM_TECH_IS_MIPI(p_boot_ins->channel_id))
    {
      temp_rx_length += temp_rx_length % GLOBAL_comm_align_bytes[p_boot_ins->channel_id];
    }
    (void)COMM_receive_buffer(p_boot_ins->channel_id,
                              (char *)p_boot_ins->rx_buffer.buffer,
                              //(char *)p_boot_ins->p_ebl_version,
                              temp_rx_length,
                              &p_boot_ins->rx_buffer.length,
                              BOOT_rx_ebl_version_not);
  }
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
#ifdef __BORLANDC__
  // "Assigned a value that is never used warning disable (they are define dependant, so Borland should not complain)
  #pragma warn -8004
#endif
static void BOOT_rx_ebl_version_not(uint8_t channel, uint32_t size)
{
  uint8_t *temp_rx_buf_ptr;
  boot_instance_t  *p_boot_ins;

  if ((0 < channel) && (channel < MAX_NUMBER_OF_INSTANCES))
  {
    p_boot_ins = p_boot_manager->boot_handler[channel];
    if (p_boot_ins != NULL)
    {
      temp_rx_buf_ptr = p_boot_ins->rx_buffer.buffer;
      if (COMM_TECH_IS_MIPI_ADD_HEADER(channel))
      {
        temp_rx_buf_ptr += sizeof(uint32_t);
        //< Assuring we don't run into the next check
        size = p_boot_ins->rx_buffer.max_length;
      }

      if (size != p_boot_ins->rx_buffer.max_length)
      {
        PrintError("BOOT_rx_ebl_version_not: Read %d bytes when requesting %d?!\n",
                   size, p_boot_ins->rx_buffer.max_length);
        //size = p_boot_ins->rx_buffer.max_length;
        return; //< Didn't read the EBL version :'(
      }

      memcpy(p_boot_ins->p_ebl_version,temp_rx_buf_ptr, size);

      p_boot_ins->state = boot_state_ebl_running;
      p_boot_ins->status = boot_status_ebl_running;
      p_boot_ins->last_error = boot_error_no_error;
      p_boot_ins->rx_buffer.length = size;
      TR_BEGIN(TR_Rx,TR_boot_rx_ebl_version,p_boot_ins->channel_id);
      if(p_boot_ins->p_ebl_version)
      {
        TR_DATA((char *)p_boot_ins->p_ebl_version, size);
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
#ifdef __BORLANDC__
  #pragma warn +8004
#endif


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
  BOOT_Generic_tx_callback_fx(channel, boot_state_rx_ebl_loader_payload_ind, db_BOOT_tx_ebl_length_not);
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
  //uint8_t *temp_rx_buf_ptr;
  boot_instance_t  *p_boot_ins;
  PARAM_NOT_USED(size);

  if ((0 < channel) && (channel < MAX_NUMBER_OF_INSTANCES))
  {
    p_boot_ins = p_boot_manager->boot_handler[channel];
    if (p_boot_ins != NULL)
    {
      //temp_rx_buf_ptr = p_boot_ins->rx_buffer.buffer;
      //if (COMM_TECH_IS_MIPI_ADD_HEADER(p_boot_ins->channel_id))
      //  temp_rx_buf_ptr += sizeof(uint32_t);
      p_boot_ins->state = boot_state_rx_ebl_version;
      p_boot_ins->rx_buffer.length = size;

      TR_BEGIN(TR_Rx,TR_boot_rx_ebl_checksum_result,p_boot_ins->channel_id);
      TR_DATA(p_boot_ins->rx_buffer.buffer, size);
      TR_END;

      int CRC = COMM_TECH_IS_MIPI(p_boot_ins->channel_id) ? 5 : 1;
      if (p_boot_ins->rx_buffer.buffer[CRC] != CHECKSUM_OK)
      {
          PrintError("CRC Error> Got '%02X' instead of '%02X'!\n Buffer: [ %s ]\n",
                p_boot_ins->rx_buffer.buffer[CRC], CHECKSUM_OK,
                getHex(p_boot_ins->rx_buffer.buffer, size));

          p_boot_manager->status.channel_id = channel;
          p_boot_manager->status.code = boot_status_channel_not_added;
          BOOT_Err(channel, BOOT_ERR_ebl_checksum_error);
          p_boot_ins->state = boot_state_error;
      }
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
  uint32_t tx_temp_length;
  p_boot_ins->state = boot_state_tx_in_progress;
  tx_temp_length = p_boot_ins->boot_buffer.max_length + (p_boot_ins->boot_buffer.max_length % GLOBAL_comm_align_bytes[p_boot_ins->channel_id]);

  (void)COMM_send_buffer(p_boot_ins->channel_id,
                        (char *)p_boot_ins->boot_buffer.buffer,
                        tx_temp_length,
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
// "Assigned a value that is never used warning disable (they are define dependant, so Borland should not complain)
static void BOOT_tx_ebl_loader_checksum(boot_instance_t *p_boot_ins)
{
  boot_status_t boot_status;
  boot_error_t  boot_error;
  uint8_t temp_tx_buf[4];
  //
  // NOTE:
  //  Last operation was Tx... Shouldn't this flush the Tx buffer then????
  //
  (void)COMM_flush_rx_buffer(p_boot_ins->channel_id);
  temp_tx_buf[0] = p_boot_ins->boot_buffer.checksum;  // if patch is needed then why not use the checksum so endian swap is not needed.
  temp_tx_buf[1] = 0x00;
  temp_tx_buf[2] = 0x00;
  temp_tx_buf[3] = p_boot_ins->boot_buffer.checksum;

  (void)COMM_send_buffer(p_boot_ins->channel_id,
                        (char *)temp_tx_buf,
                        GLOBAL_comm_align_bytes[p_boot_ins->channel_id],
                        &p_boot_ins->boot_buffer.length,
                        BOOT_tx_ebl_loader_checksum_not);

  //Debug("Flusing I/O buffers!\n");
  //(void)COMM_flush_tx_buffer(p_boot_ins->channel_id);

  TR_BEGIN(TR_Tx,TR_boot_tx_ebl_loader_checksum,p_boot_ins->channel_id);
  TR_8(p_boot_ins->boot_buffer.checksum);
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
  uint32_t temp_rx_length;

  if (p_boot_ins->p_dev_cfg->platform == X_GOLD_213)
  {
    p_boot_ins->rx_buffer.max_length = X213_EBL_LOADER_CHECKSUM_LENGTH;
   }
  else
  {
    if (p_boot_ins->chip_return_code == BOOT_ACK_NOSEC_XG618_ES2 ||
        p_boot_ins->chip_return_code == BOOT_ACK_NOSEC_XG626 ||
        p_boot_ins->chip_return_code == BOOT_ACK_NOSEC_XG631 ||
        p_boot_ins->chip_return_code == BOOT_ACK_NOSEC_XG632 ||
        p_boot_ins->chip_return_code == BOOT_ACK_NOSEC_XG632_ES1 ||
        p_boot_ins->chip_return_code == BOOT_ACK_NOSEC_XG633 ||
        p_boot_ins->chip_return_code == BOOT_ACK_NOSEC_XG636 ||
        p_boot_ins->chip_return_code == BOOT_ACK_NOSEC_XG706 ||
        p_boot_ins->chip_return_code == BOOT_ACK_NOSEC_XG716 ||
        p_boot_ins->chip_return_code == BOOT_ACK_NOSEC_XG726 ||
        p_boot_ins->chip_return_code == BOOT_ACK_NOSEC_XG221 ||
        p_boot_ins->chip_return_code == BOOT_ACK_NOSEC_XG222 ||
        p_boot_ins->chip_return_code == BOOT_ACK_NOSEC_XG223 ||
        p_boot_ins->chip_return_code == BOOT_ACK_NOSEC_XG225 ||
        p_boot_ins->chip_return_code == BOOT_ACK_NOSEC_XG215)
    {
      p_boot_ins->rx_buffer.max_length = XG618_ES2_EBL_LOADER_CHECKSUM_LENGTH;
    }
    else
    {
      p_boot_ins->rx_buffer.max_length = EBL_LOADER_CHECKSUM_LENGTH;
    }
  }

  p_boot_ins->state = boot_state_rx_in_progress;
  if (COMM_TECH_IS_MIPI(p_boot_ins->channel_id))
  {
    temp_rx_length = 8;  // The ACK and STATUS are send in two frames.
  }
  else
  {
    temp_rx_length = p_boot_ins->rx_buffer.max_length; // + (p_boot_ins->rx_buffer.max_length % GLOBAL_comm_align_bytes[p_boot_ins->channel_id]);
  }
  GLOBAL_is_in_time_critical_section = true;

  (void)COMM_receive_buffer(p_boot_ins->channel_id,
                            (char *)p_boot_ins->rx_buffer.buffer,
                            temp_rx_length,
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
  case boot_state_rx_boot_usb_ack:
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


/******************************************************************************
* Function:... void BOOT_rx_ebl_loader_payload_ind(boot_instance_t *p_boot_ins)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static void BOOT_rx_ebl_payload_ind(boot_instance_t *p_boot_ins)
{
  uint32_t temp_rx_length;
  p_boot_ins->rx_buffer.max_length = EBL_READY_IND_PAYLOAD_LENGTH;
  p_boot_ins->state = boot_state_rx_in_progress;

  temp_rx_length = p_boot_ins->rx_buffer.max_length;

  if (COMM_TECH_IS_MIPI_ADD_HEADER(p_boot_ins->channel_id))
    temp_rx_length += sizeof(uint32_t);

  if (COMM_TECH_IS_MIPI(p_boot_ins->channel_id))
    temp_rx_length += temp_rx_length % GLOBAL_comm_align_bytes[p_boot_ins->channel_id];

  (void)COMM_receive_buffer(p_boot_ins->channel_id,
                            (char *)p_boot_ins->rx_buffer.buffer,
                            temp_rx_length,
                            &p_boot_ins->rx_buffer.length,
                            BOOT_rx_ebl_payload_ind_not);

  p_boot_ins->last_status = boot_status_booting_device;
  p_boot_ins->last_error = boot_error_no_error;
}

/******************************************************************************
* Function:... void BOOT_rx_ebl_loader_payload_ind(boot_instance_t *p_boot_ins)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static void BOOT_rx_ebl_payload_ind_not(uint8_t channel, uint32_t size)
{
  uint8_t *temp_rx_buf_ptr;
  boot_status_t    boot_status = boot_status_ok;
  boot_error_t     boot_error = boot_error_no_error;
  boot_instance_t  *p_boot_ins;
  PARAM_NOT_USED(size);

  if ((0 < channel) && (channel < MAX_NUMBER_OF_INSTANCES))
  {
    p_boot_ins = p_boot_manager->boot_handler[channel];
    if (p_boot_ins != NULL)
    {
      temp_rx_buf_ptr = p_boot_ins->rx_buffer.buffer;
      p_boot_ins->rx_buffer.length = p_boot_ins->rx_buffer.max_length;

      if (COMM_TECH_IS_MIPI_ADD_HEADER(p_boot_ins->channel_id))
      {
        temp_rx_buf_ptr += sizeof(uint32_t);
      }


      TR_BEGIN(TR_Rx,TR_boot_rx_payload_ind,p_boot_ins->channel_id);  /*  "Sync Tag" uint16_t: (hex) */
      TR_DATA(temp_rx_buf_ptr, p_boot_ins->rx_buffer.length);
      TR_END;
      if ((temp_rx_buf_ptr[0] == 0xCC) &&
         ((temp_rx_buf_ptr[1] & 0xCC) == 0xCC))
      {
//        BOOT_tx_ebl_payload(p_boot_ins);
        p_boot_ins->state = boot_state_tx_ebl_payload;
      }
      else
      {
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
        BOOT_Err(p_boot_ins->channel_id, BOOT_ERR_boot_device_error);
#endif
        boot_status = boot_status_error;
        boot_error = p_boot_ins->last_error;
      }

      p_boot_ins->last_status = boot_status;
      p_boot_ins->last_error = boot_error;
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
  }
}

/******************************************************************************
* Function:... void BOOT_rx_ebl_ready_ind(boot_instance_t *p_boot_ins)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static void BOOT_rx_ebl_ready_ind(boot_instance_t *p_boot_ins)
{
  uint32_t temp_rx_length;
  p_boot_ins->state = boot_state_rx_in_progress;
  p_boot_ins->rx_buffer.max_length = EBL_READY_IND_LENGTH;
  temp_rx_length = p_boot_ins->rx_buffer.max_length;
  if (COMM_TECH_IS_MIPI_ADD_HEADER(p_boot_ins->channel_id))
  {
     temp_rx_length += sizeof(uint32_t); // Need to receive the header also.
  }
  temp_rx_length += temp_rx_length % GLOBAL_comm_align_bytes[p_boot_ins->channel_id];

  (void)COMM_receive_buffer(p_boot_ins->channel_id,
                            (char *)p_boot_ins->rx_buffer.buffer,
                            temp_rx_length,
                            &p_boot_ins->rx_buffer.length,
                            BOOT_rx_ebl_ready_ind_not);

  p_boot_ins->last_status = boot_status_booting_device;
  p_boot_ins->last_error = boot_error_no_error;


}

/******************************************************************************
* Function:... void BOOT_psi_error(uint8_t channel, uint32_t size)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 18.02.06 by (Dennis Hilmar)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static void BOOT_psi_error(uint8_t channel, uint32_t size)
{
  memtest_error_t error;
  boot_instance_t  *p_boot_ins;
  p_boot_ins = p_boot_manager->boot_handler[channel];
  if (p_boot_ins != NULL)
  {
    TR_BEGIN(TR_Rx,TR_boot_rx_psi_error,p_boot_ins->channel_id);
    TR_DATA(p_boot_ins->rx_buffer.buffer, size);
    TR_END;

    /* Retreiving the MemTest Error structure (psi_std_errors.h) */
    memcpy((void*)&error, (const void*)(p_boot_ins->rx_buffer.buffer), sizeof(memtest_error_t));

    SFH_set_error_string(p_boot_ins->channel_id,
        GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, BOOT_ERR_mem_test_fail,
                           GLOBAL_get_raw_text(IFWD_PSI_error_code, error.failed_test),
                           error.failed_address, error.expected_value, error.read_value));

    p_boot_ins->state = boot_state_error;
    p_boot_ins->rx_buffer.length = size;

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


/******************************************************************************
* Function:... void BOOT_rx_ebl_loader_ready_not(comm_instance_t *p_boot_ins)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static void BOOT_rx_ebl_ready_ind_not(uint8_t channel, uint32_t size)
{
  boot_status_t    boot_status = boot_status_ok;
  boot_error_t     boot_error = boot_error_no_error;
  boot_instance_t *p_boot_ins;
  uint8_t         *temp_rx_buf_ptr;
  uint16_t         psi_ack_type;

  if ((0 < channel) && (channel < MAX_NUMBER_OF_INSTANCES))
  {
    p_boot_ins = p_boot_manager->boot_handler[channel];
    if (p_boot_ins != NULL)
    {
      p_boot_ins->rx_buffer.length = size;
      temp_rx_buf_ptr = p_boot_ins->rx_buffer.buffer;
      if (COMM_TECH_IS_MIPI_ADD_HEADER(p_boot_ins->channel_id))
        temp_rx_buf_ptr += sizeof(uint32_t);  // Advance pointer past MIPI header.

      TR_BEGIN(TR_Rx,TR_boot_rx_ebl_ready,p_boot_ins->channel_id); /* uint16_t: "Sync Tag" (hex) */
      TR_DATA(temp_rx_buf_ptr, size);
      TR_END;

      /* Transmit the embedded ebl loader*/
      psi_ack_type = *((uint16_t*) temp_rx_buf_ptr );
      switch(psi_ack_type)
      {
      case PSI_ACK:
        p_boot_ins->state = boot_state_tx_ebl;
        p_boot_ins->nof_ebl_length_bytes = 2;
        break;

      case PSI_ACK_EXT:
        p_boot_ins->state = boot_state_tx_ebl;
        p_boot_ins->nof_ebl_length_bytes = 4;
        break;

      case PSI_ACK_ENHANCED:   // New RPSI protocol
        GLOBAL_callback(p_boot_ins->channel_id, IFWD_DL_ProcessOutlineUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, BOOT_INFO_RPSI_CMD_MODE));
        if (GLOBAL_usb_device)
        {
#ifdef INCLUDE_DUT_USB_TEST
          if (IFWD_BOOT_dut_to_test_mode())
          {
            p_boot_ins->dut_in_test_mode = true;
            p_boot_ins->state = boot_state_dut_in_test_mode;
          }
          else
#endif
          {
            p_boot_ins->state = boot_state_rpsi_cmd_send_ebl; // For USB there is no need to change Baud rate.
          }
        }
        else
        {
          // check if the comm channel needs to be changed
          if (strcmp(GLOBAL_comm_port_name[p_boot_ins->channel_id], GLOBAL_ctrl_port_name[p_boot_ins->channel_id]) != 0)
          {
            // TODO: figure out if the port is a USB port. Assume it is for now.
            p_boot_ins->state = boot_state_rpsi_cmd_new_comm_tx;
          }
          else
          {
            p_boot_ins->state = boot_state_rpsi_cmd_new_baud_tx;
          }
        }
        p_boot_ins->nof_ebl_length_bytes = 4;
        break;

        /**
         * Target failed the memtest
         */
      case PSI_NACK:
        p_boot_ins->state = boot_state_rx_psi_error;
        break;

#ifdef INCLUDE_DUT_USB_TEST
      case DUT_TEST_MODE:
        if (IFWD_BOOT_dut_to_test_mode())
        {
          p_boot_ins->dut_in_test_mode = true;
          p_boot_ins->state = boot_state_dut_in_test_mode;
        }
        break;
#endif
      default:
        //if ( (PSI_NACK && 0xFF00) == (retcode && 0xFF00) )
        #if defined (USE_TEXT_CLASS_IFWD_ERRORS)
          BOOT_Err(p_boot_ins->channel_id, BOOT_ERR_boot_device_error);
        #endif
        p_boot_ins->state = boot_state_error;
        boot_status = boot_status_error;
        boot_error = p_boot_ins->last_error;
      } // switch ( packet type )

      p_boot_ins->last_status = boot_status;
      p_boot_ins->last_error = boot_error;
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
  }
}

/******************************************************************************
* Function:... void BOOT_dut_process_test(boot_instance_t *p_boot_ins)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 02.09.11 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
#ifdef INCLUDE_DUT_USB_TEST
static void BOOT_dut_process_test(boot_instance_t *p_ins)
{
  uint8_t msg[216] = "USB certification test";
  uint32_t size= strlen((const char*) msg);

  switch (GLOBAL_dut_test_request)
  {
    case GLOBAL_dut_test_usb_certification:
      /* Report back to FlashTool_E2 to close application and *
         launch USB20CV */
      p_ins->state = boot_state_dut_usb_test;
      DUT_usb_test(p_ins);
#if defined (USE_TEXT_CLASS_IFWD_INFO)
      BOOT_show(p_ins->channel_id, BOOT_INFO_USB_CERTIFICATION_TEST);
#endif

      TR_BEGIN(TR_Tx,TR_boot_dut_usb_certification_test,p_ins->channel_id);
      TR_DATA(msg, size);
      TR_END;
      break;
    case GLOBAL_dut_test_usb_in_token:
      p_ins->state = boot_state_dut_usb_test;
      p_ins->usb_state = boot_state_dut_usb_test;
      p_ins->usb_test_case = usb_test_in;
      DUT_usb_test(p_ins);
      break;
    case GLOBAL_dut_test_usb_out_token:
      p_ins->usb_state = boot_state_dut_usb_test;
      p_ins->state = boot_state_dut_usb_test;
      p_ins->usb_test_case = usb_test_out;
      DUT_usb_test(p_ins);
      break;
    case GLOBAL_dut_test_usb_loopback:
      p_ins->usb_state = boot_state_dut_usb_test;
      p_ins->state = boot_state_dut_usb_test;
      p_ins->usb_test_case = usb_test_loopback;
      DUT_usb_test(p_ins);
      break;
    default:
      ;
  }
}

/******************************************************************************
* Function:... uint8_t IFWD_BOOT_dut_to_test_mode(boot_instance_t *p_boot_ins)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 02.09.11 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static uint8_t IFWD_BOOT_dut_to_test_mode(void)
{
  switch (GLOBAL_dut_test_request)
  {
    case GLOBAL_dut_test_usb_certification:
    case GLOBAL_dut_test_usb_in_token:
    case GLOBAL_dut_test_usb_out_token:
    case GLOBAL_dut_test_usb_loopback:
      GLOBAL_dut_in_test_mode = true;
      break;
    default:
      break;
  }

  return GLOBAL_dut_in_test_mode;

}
#endif
