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
 *  $File name:  /msw_tools/FlashTool/libDownloadTool/src/Portable_code/boot_handler/IFWD_boot.c $
 *  $CC-Version: .../dhkristx_140314_v0/1 $
 *  $Date:       2014-04-23    10:51:30 UTC $
 *   Responsible: kofod, Hilmar
 *   Comment:
 *     Merging support for XG110 from temp branch
 */

#define THIS_MODULE MODULE_BOOT

/*****************************************************************************
*                           Include files                                   *
*****************************************************************************/

#include "OS.h"
#include "DebugModule.h"
#include "IFWD_global_func.h"

#include "IFWD_timer.h"
#include "IFWD_mem.h"
#include "IFWD_boot.h"
#include "IFWD_boot_priv.h"
#include "IFWD_single_file_handler.h"

//#include "comm_handler.h"
//#include "IFWD_comport.h"
#include "IFWD_comm.h"

/*****************************************************************************
*                            PUBLIC PROTOTYPES                              *
*****************************************************************************/
void BOOT_init(void);
void BOOT_close(void);
void BOOT_clock(void);
boot_status_t BOOT_add_channel(uint8_t channel);
boot_status_t BOOT_delete_channel(uint8_t channel);
boot_status_t BOOT_abort_process(uint8_t channel);
boot_status_t BOOT_start_boot(uint8_t channel, boot_dev_cfg_t *p_dev_cfg,
                              boot_ebl_version_t *p_ebl_version, boot_fx_t fx);

/*****************************************************************************
*                             LOCAL PROTOTYPES                              *
*****************************************************************************/
static void BOOT_dummy_handler(boot_instance_t *p_boot_ins);

/*****************************************************************************
*                       Global variables declaration                         *
*****************************************************************************/
capabilities_t dll_capabilities;
capabilities_t target_capabilities[33]; /* 32 channels but index is 1..32  */
ext_capabilities_t ext_dll_capabilities;
ext_capabilities_t ext_target_capabilities[33]; /* 32 channels but index is 1..32  */

boot_manager_t  boot_manager;
boot_manager_t *p_boot_manager = NULL;


/*****************************************************************************
*                       Local variables declaration                         *
*****************************************************************************/

static boot_handler_t BOOT_handler = NULL;

/*****************************************************************************
*                            IMPLEMENTATION                                 *
*****************************************************************************/

/******************************************************************************
* Function:... void BOOT_init(void)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
void BOOT_init(void)
{
  int i;

  p_boot_manager = &boot_manager;
  /* Clear the capability structures */
  memset((void *)target_capabilities, 0x00, sizeof(target_capabilities));
  memset((void *)&dll_capabilities,   0x00, sizeof(dll_capabilities));
  memset((void *)ext_target_capabilities, 0x00, sizeof(ext_target_capabilities));
  memset((void *)&ext_dll_capabilities,   0x00, sizeof(ext_dll_capabilities));

  for (i = 0; i < MAX_NUMBER_OF_INSTANCES; i++)
    p_boot_manager->boot_handler[i] = NULL;

  p_boot_manager->manager_id = BOOT_MANAGER_ID;
  p_boot_manager->state = boot_manager_idle;
  p_boot_manager->nr_of_instances_running = 0;
  for (i = 0; i < MAX_NUMBER_OF_INSTANCES; i++)
  {
    p_boot_manager->status.channel_id = i;
    p_boot_manager->status.code  = boot_status_ok;
    p_boot_manager->error.channel_id = i;
    p_boot_manager->error.code = boot_error_no_error;
  }

  BOOT_handler = BOOT_dummy_handler;
}

/******************************************************************************
* Function:... void BOOT_close(void)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
void BOOT_close(void)
{
  int i;

  for (i = 0; i < MAX_NUMBER_OF_INSTANCES; i++)
  {
    if (p_boot_manager->boot_handler[i] != NULL)
    {
      MEM_free((uint8_t *)p_boot_manager->boot_handler[i]);
      p_boot_manager->boot_handler[i] = NULL;
    }
  }
  p_boot_manager->nr_of_instances_running = 0;
  p_boot_manager->state = boot_manager_idle;
}

/******************************************************************************
* Function:... void BOOT_clock(void)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
void BOOT_clock(void)
{
  uint8_t i;

  if (p_boot_manager != NULL)
  { /* The comm manager process has been initialized */
    for (i = 0; i < MAX_NUMBER_OF_INSTANCES; i++)
    {
      if (p_boot_manager->boot_handler[i] != NULL)
      {  /* The instance exist, run the internal state machine */
        BOOT_handler(p_boot_manager->boot_handler[i]);
      }
    }
  }
}
void BOOT_set_EBL_load_speed(uint8_t channel, uint32_t baudrate)
{
  boot_instance_t  *p_boot_ins;

  if ((0 < channel) && (channel < MAX_NUMBER_OF_INSTANCES))
  {
    p_boot_ins = p_boot_manager->boot_handler[channel];
    p_boot_ins->EBL_load_baudrate = baudrate;
  }
}

/******************************************************************************
* Function:... void BOOT_add_channel(uint8_t channel)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
boot_status_t BOOT_add_channel(uint8_t channel)
{
  boot_instance_t  *p_boot_ins;
  boot_status_t    boot_status = boot_status_not_valid_status;
  comm_status_t    comm_status;
  char           status_text[10] = "";

  if ((0 < channel) && (channel < MAX_NUMBER_OF_INSTANCES))
  {
    if (p_boot_manager->boot_handler[channel] == NULL)
    { /* Verify if the communication handler for the channel is running */
      comm_status = COMM_get_status(channel, status_text);
      if (comm_status == comm_status_channel_added)
      {
        /* Create the channel */
        p_boot_ins = (boot_instance_t *)MEM_malloc(sizeof(boot_instance_t));

        if (!p_boot_ins)
        { /* Not enough memory to create a instance */
          p_boot_manager->status.channel_id = channel;
          p_boot_manager->status.code = boot_status_not_resources_to_add_channel;
          boot_status = p_boot_manager->status.code;
          BOOT_Err(channel, BOOT_ERR_malloc);
          return boot_status;
        }

        /* Initialize the instance and start it. */
        memset((void *)p_boot_ins, 0x00, sizeof(boot_instance_t));
        p_boot_manager->nr_of_instances_running += 1;
        p_boot_manager->boot_handler[channel] = p_boot_ins;
        /* Instance for the channel can now be initialzed and started */
        p_boot_ins->channel_id = channel;
        p_boot_ins->EBL_load_baudrate = 115200;
        p_boot_ins->last_error = boot_error_no_error;
        p_boot_ins->last_status = boot_status_channel_added;
        p_boot_ins->state = boot_state_idle;
        p_boot_ins->dut_in_test_mode = false;

        boot_status = p_boot_ins->last_status;
      }
      else
      {/* The communication channel for the requested channel is not running */
        p_boot_manager->status.channel_id = channel;
        p_boot_manager->status.code = boot_status_comm_channel_not_running;
        boot_status = p_boot_manager->status.code;
        BOOT_Err(channel, BOOT_ERR_channel_does_not_exist);
      }
    }
  }
  else
  { /* Invalid channel number */
    p_boot_manager->status.channel_id = channel;
    p_boot_manager->status.code= boot_status_channel_not_added;
    boot_status = p_boot_manager->status.code;
    BOOT_Err(channel, BOOT_ERR_channel_is_outside_range);
  }
  return boot_status;
}

/******************************************************************************
* Function:... void BOOT_delete_channel(uint8_t channel)
* Parameters:. I :
* Returns:....b
* Description:
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
boot_status_t BOOT_delete_channel(uint8_t channel)
{
  boot_status_t    boot_status;

  if ((0 < channel) && (channel < MAX_NUMBER_OF_INSTANCES))
  {
    if (p_boot_manager->boot_handler[channel] != NULL)
    { /* Now delete the instance */
      MEM_free((uint8_t *)p_boot_manager->boot_handler[channel]);
      p_boot_manager->boot_handler[channel] = NULL;
      p_boot_manager->nr_of_instances_running -= 1;
    }
    p_boot_manager->status.channel_id = channel;
    p_boot_manager->status.code = boot_status_channel_deleted;
    boot_status = p_boot_manager->status.code;
  }
  else
  { /* Invalid channel number */
    p_boot_manager->status.channel_id = channel;
    p_boot_manager->status.code = boot_status_channel_not_added;
    boot_status = p_boot_manager->status.code;
    BOOT_Err(channel, BOOT_ERR_channel_is_outside_range);
  }
  return boot_status;
}

/******************************************************************************
* Function:... boot_status_t BOOT_abort_process(uint8_t channel)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
boot_status_t BOOT_abort_process(uint8_t channel)
{
  boot_instance_t  *p_boot_ins;
  boot_status_t    boot_status;

  if ((0 < channel) && (channel < MAX_NUMBER_OF_INSTANCES))
  {
    p_boot_ins = p_boot_manager->boot_handler[channel];
    if (p_boot_ins != NULL)
    { /* The instance exist. Abort the instance */
      /* Abort any pending transfers over the comport */
      p_boot_manager->status.channel_id = channel;
      p_boot_manager->status.code = boot_status_ok;
      boot_status = p_boot_manager->status.code;
      p_boot_manager->error.channel_id = channel;
      p_boot_manager->error.code = boot_error_no_error;
      p_boot_ins->state = boot_state_idle;
#ifdef DEBUG_BOOT_MODULE
      TIMER_start_chronom_sec(&p_boot_ins->chronom_idle);
      p_boot_ins->debug_info.nr_of_abort_call += 1;
      p_boot_ins->debug_info.nr_of_overflow_bytes_rx = 0;
      p_boot_ins->debug_info.nr_of_overflow_bytes_tx = 0;
      p_boot_ins->debug_info.bytes_rx = 0;
      p_boot_ins->debug_info.bytes_tx = 0;
      p_boot_ins->debug_info.busy_time = 0;
      p_boot_ins->debug_info.idle_time = 0;
      p_boot_ins->debug_info.max_peak_baudrate_rx = 0;
      p_boot_ins->debug_info.max_peak_baudrate_tx = 0;
      p_boot_ins->debug_info.min_peak_baudrate_rx = 0;
      p_boot_ins->debug_info.min_peak_baudrate_tx = 0;
      p_boot_ins->debug_info.avg_baudrate_rx = 0;
      p_boot_ins->debug_info.avg_baudrate_tx = 0;
      TIMER_stop_chronom_sec(&p_boot_ins->debug_info.chronom_busy);
      TIMER_stop_chronom_sec(&p_boot_ins->debug_info.chronom_idle);
      TIMER_stop_chronom_sec(&p_boot_ins->debug_info.chronom_rx);
      TIMER_stop_chronom_sec(&p_boot_ins->debug_info.chronom_tx);
      p_boot_ins->debug_info.nr_of_mem_malloc_calls = 0;
      p_boot_ins->debug_info.nr_of_mem_free_calls = 0;
#endif
    }
    else
    {
      p_boot_manager->status.channel_id = channel;
      p_boot_manager->status.code = boot_status_channel_not_added;
      boot_status = p_boot_manager->status.code;
      //      BOOT_Err(channel, BOOT_ERR_channel_does_not_exist);
    }
  }
  else
  { /* Invalid channel number */
    p_boot_manager->status.channel_id = channel;
    p_boot_manager->status.code = boot_status_channel_not_added;
    boot_status = p_boot_manager->status.code;
    BOOT_Err(channel, BOOT_ERR_channel_is_outside_range);
  }
  return boot_status;
}

/******************************************************************************
* Function:... boot_status_t BOOT_start_boot(uint8_t channel,
*                                            boot_dev_cfg_t *p_dev_cfg,
*                                            boot_ebl_version_t *p_ebl_version
*                                            boot_fx_t fx)
*
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
boot_status_t BOOT_start_boot(uint8_t channel, boot_dev_cfg_t *p_dev_cfg,
                              boot_ebl_version_t *p_ebl_version, boot_fx_t fx)
{
  boot_instance_t  *p_boot_ins;
  boot_status_t    boot_status;

  if ((0 < channel) && (channel < MAX_NUMBER_OF_INSTANCES))
  {
    p_boot_ins = p_boot_manager->boot_handler[channel];
  	if (GLOBAL_pre_boot[channel] == PRE_BOOT_DONE)
    {
      GLOBAL_platform_group = GLOBAL_platform_group_sgold;
      BOOT_handler = BOOT_sgold_handler;

      if(p_ebl_version) /* make sure we clear all bytes regardless of how many we read from target */
      {
        memset((char*)p_ebl_version, 0, sizeof(boot_ebl_version_t));
        p_boot_ins->p_ebl_version = p_ebl_version;
      }

      boot_status = BOOT_sgold(p_boot_ins, p_boot_manager);
      return boot_status;
    }

    if (p_boot_ins != NULL)
    { /* The instance exist. Abort the instance */
      /* Abort any pending transfers over the comport */
      /* Verify header */

      p_boot_ins->p_dev_cfg = p_dev_cfg;
      if(p_ebl_version) /* make sure we clear all bytes regardless of how many we read from target */
        memset((char*)p_ebl_version, 0, sizeof(boot_ebl_version_t));
      p_boot_ins->p_ebl_version = p_ebl_version;
      p_boot_ins->fx = fx;

      // Clear the Link Response
      memset((void*)&p_boot_ins->link_est_response, 0, sizeof(boot_link_est_response_t));

      /* GLOBAL_callback(channel,IFWD_DL_PhoneOn, GLOBAL_format_text(IFWD_DL_text_class_process_info, BOOT_INFO_sync));  */
      switch (p_boot_ins->p_dev_cfg->platform)
      {
      case E_GOLD_RADIO_V1:
      case E_GOLD_VOICE_V1:
      case E_GOLD_LITE_CHIPSET:
      case X_GOLD_102:
      case X_GOLD_110:
        GLOBAL_platform_group = GLOBAL_platform_group_egold;
        BOOT_handler = BOOT_egold_handler;
        boot_status = BOOT_egold(p_boot_ins, p_boot_manager);
        break;
      case S_GOLD_CHIPSET_V2:
      case S_GOLD_CHIPSET_V3:
      case S_GOLD_CHIPSET_V1:
      case S_GOLD_RADIO_V1:
      case X_GOLD_618:
      case X_GOLD_626:
      case X_GOLD_706:
      case X_GOLD_716:
      case X_GOLD_726:
      case X_GOLD_636:
      case X_GOLD_631:
      case X_GOLD_632:
      case X_GOLD_633:
      case X_GOLD_221:
      case X_GOLD_222:
      case X_GOLD_223:
      case X_GOLD_225:
      case X_GOLD_215:
      case X_GOLD_213:
        GLOBAL_platform_group = GLOBAL_platform_group_sgold;
        BOOT_handler = BOOT_sgold_handler;
        boot_status = BOOT_sgold(p_boot_ins, p_boot_manager);
        break;
      default:
        GLOBAL_platform_group = GLOBAL_platform_group_unknown;
        p_boot_manager->status.channel_id = channel;
        p_boot_manager->status.code = boot_status_platform_not_supported;
        boot_status = p_boot_manager->status.code;
      }
    }
    else
    {
      p_boot_manager->status.channel_id = channel;
      p_boot_manager->status.code = boot_status_channel_not_added;
      boot_status = p_boot_manager->status.code;
      BOOT_Err(channel, BOOT_ERR_channel_does_not_exist);
    }
  }
  else
  { /* Invalid channel number */
    p_boot_manager->status.channel_id = channel;
    p_boot_manager->status.code = boot_status_channel_not_added;
    boot_status = p_boot_manager->status.code;
    BOOT_Err(channel, BOOT_ERR_channel_is_outside_range);
  }
  return boot_status;
}


boot_status_t BOOT_get_chip_response(uint8_t channel, uint8_t *response, uint32_t alloc_size)
{
  boot_instance_t  *p_boot_ins;
  uint32_t copy_len;
  boot_link_est_response_t    temp_response;

  // Zero-set the response
  memset((void*)response, 0, alloc_size);

  if ((0 < channel) && (channel < MAX_NUMBER_OF_INSTANCES))
  { // Channel number is OK
    p_boot_ins = p_boot_manager->boot_handler[channel];
    if (p_boot_ins != NULL)
    {   // Have an instance for the channel ... so go ahead
    	// Make a copy of the original response and clear chip-unique data
    	memcpy((void*)&temp_response, (void*)&p_boot_ins->link_est_response, sizeof(temp_response));
    	temp_response.snum_1 = 0x00000000;
    	temp_response.snum_2 = 0x00000000;
    	temp_response.snum_3 = 0x00000000;

    	// Figure out how much to copy
    	copy_len = (uint32_t)(temp_response.size) + 1; // + 1 as the length byte itself is not included in the length.
    	if (copy_len > alloc_size) // Do not copy outside the allocated buffer
        {
    		copy_len = alloc_size;
        }
    	memcpy((void*)response, (void*)&temp_response, copy_len);
    	return boot_status_ok;
    }
  }
  // If called before BOOT_start_boot() channel is not assigned, so zero-set the response data and return FAIL.
  return boot_status_channel_not_added;
}


/******************************************************************************
* Function:... static void BOOT_Err(uint8_t channel, uint32_t err)
*
* Parameters:. I :
* Returns:....
* Description: Simple wrapper for the SFH_set_error_string() function to
*              reduce code usage and increase readability
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
void BOOT_Err(uint8_t channel, uint32_t err)
{
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
  SFH_set_error_string(channel, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, (uint16_t)err, channel));
#endif
}

/******************************************************************************
* Function:... static void BOOT_info(uint8_t channel, uint32_t err)
*
* Parameters:. I :
* Returns:....
* Description: Simple wrapper for the SFH_set_error_string() function to
*              reduce code usage and increase readability
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
void BOOT_info(uint8_t channel, uint32_t info)
{
#if defined (USE_TEXT_CLASS_IFWD_INFO)
  GLOBAL_callback(channel, IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, (uint16_t)info));
#endif
}

/******************************************************************************
* Function:... static void BOOT_show(uint8_t channel, uint32_t err)
*
* Parameters:. I :
* Returns:....
* Description: This is identical to "BOOT_info()", but the information is always shown (detailed info can be filtered out)
*
******************************************************************************/
void BOOT_show(uint8_t channel, uint32_t info)
{
#if defined (USE_TEXT_CLASS_IFWD_INFO)
  GLOBAL_callback(channel, IFWD_DL_ProcessOutlineUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, (uint16_t)info));
#endif
}

/*****************************************************************************
*                            Private functions                              *
*****************************************************************************/

/******************************************************************************
* Function:... void BOOT_dummy_handler(boot_instance_t *p_boot_ins)
*
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static void BOOT_dummy_handler(boot_instance_t *p_boot_ins)
{
  PARAM_NOT_USED(p_boot_ins);
}

