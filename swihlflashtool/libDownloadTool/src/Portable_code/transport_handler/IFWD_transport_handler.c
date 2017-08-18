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
 *   IFWD transport handler - code
 *
 * Revision Information:
 *  $File name:  /msw_tools/FlashTool/libDownloadTool/src/Portable_code/transport_handler/IFWD_transport_handler.c $
 *  $CC-Version: .../oint_tools_flashtoole2_01/15 $
 *  $Date:       2014-03-13    15:40:25 UTC $
 *   Resonsible: njk
 *   Comment:
 *     SMS00906364 - Added call to clock in RX-Callback to allow state change in case of skipping of write confirm packs.
 */

#define THIS_MODULE MODULE_TRA

/*---------------------------------------------*/
/*  Include files.                             */
/*---------------------------------------------*/
#include <stdio.h>
#include "OS.h"
#include "FileIO.h"
#include "DebugModule.h"

#include "IFWD_global_func.h"
#include "IFWD_text_def.h"

#include "IFWD_timer.h"
#include "IFWD_mem.h"
#include "IFWD_boot.h"
#include "IFWD_transport_handler.h"
#include "IFWD_protocol_handler.h"
#include "IFWD_single_file_handler.h"
#include "IFWD_boot.h"

//#include "comm_handler.h"
#include "IFWD_comm.h"


/*---------------------------------------------*/
/* typedef                                     */
/*---------------------------------------------*/

typedef enum
{
  TRA_state_idle,
  TRA_state_Tx,
  TRA_state_Tx_Fail,
  TRA_state_Tx_Ok,
  TRA_state_Rx,
  TRA_state_Rx_Fail,
  TRA_state_Rx_Ok
} TRA_state_type;

typedef struct
{
  uint32_t nof_total;
  uint32_t nof_remain;
  uint32_t curr_len;
  uint8_t *curr_pos;
  uint8_t *orig_pos;
  FILE *file;
} TRA_instance_store_type;

typedef struct
{
  uint16_t package_code;
  uint8_t *data_pointer;
  uint8_t data_pointer_allocated;
  uint32_t data_length;
}TRA_cmd_buffer_type;

typedef enum
{
  TRA_none,
  TRA_memory,
  TRA_file
} TRA_dest_name_type;

typedef struct
{
  uint16_t package_code;
  uint8_t SendProgress;
  TRA_callback  done_callback;
  TRA_dest_name_type dest;
  TRA_instance_store_type data;
  prot_header_t *p_hdr;
  prot_payload_t *p_buffer; /* rx or tx buffer */
} TRA_dest_type;

typedef struct
{
  TRA_state_type  state;
  uint8_t IgnoreTempRx;
  uint8_t CalcChecksum;
  int8_t last_percent;
  TRA_cmd_buffer_type *repeat_cmd; /* temp. buffer used to store command to get target to repeat Tx so we can Rx all needed data */
  TRA_dest_type   tx;              /* setup information for tx data (where to send data from and how much)     */
  TRA_dest_type   rx;              /* setup information for rx data (where to receive data from, and how much) */
  uint16_t SkippedPackages;
  uint16_t LastSkippedPackages;
  uint16_t data_checksum;
  uint16_t SkipHeader; /* == 0 normally, == TRA_COMPRESSION_HEADER_BYTES if we are skipping FF packages */
  uint32_t PackSizeLimit;
  uint8_t preloaded;
  uint8_t preloaded_result;
  uint32_t preloaded_length;
} TRA_instance_type;

/*---------------------------------------------*/
/* Local defines.                              */
/*---------------------------------------------*/

/*---------------------------------------------*/
/* Local const                                 */
/*---------------------------------------------*/

/*---------------------------------------------*/
/* globale variabler                           */
/*---------------------------------------------*/

/* allocate one more pointer to avoid subtracting 1 on every access */
static TRA_instance_type *TRA_instances[MAX_NUMBER_OF_CHANNELS + 1];
static uint8_t TRA_activeList[MAX_NUMBER_OF_CHANNELS + 1];
static uint16_t TRA_activeListNof = 0;

/*---------------------------------------------*/
/* local variables                             */
/*---------------------------------------------*/
/*---------------------------------------------*/
/* local prototypes                            */
/*---------------------------------------------*/
void TRA_callback_tx(uint8_t channel, uint8_t success);
void TRA_callback_rx(uint8_t channel, uint8_t success);
uint16_t TRA_FastChecksum(uint16_t *data_ptr, int byte_length, uint16_t initial_value);
static uint8_t TRA_IsFF(uint8_t *data_ptr, int byte_length);

/*---------------------------------------------*/
/* local functions.                            */
/*---------------------------------------------*/

static void free_repeat_cmd(TRA_instance_type *inst)
{
  if(inst)
  {
    if(inst->repeat_cmd)
    {
      if(inst->repeat_cmd->data_pointer && inst->repeat_cmd->data_pointer_allocated)
        MEM_free((uint8_t*)inst->repeat_cmd->data_pointer);
      MEM_free((uint8_t*)inst->repeat_cmd);
    }
    inst->repeat_cmd = NULL;
  }
}
/***************************************************************************************/
static void goto_idle(TRA_instance_type *inst)
{
  if(inst)
  {
    inst->tx.dest = TRA_none;
    inst->rx.dest = TRA_none;      /* Mark Rx as done */
    inst->state = TRA_state_idle;  /* go back to idle */
    free_repeat_cmd(inst);         /* free any allocated repeat command */
    inst->IgnoreTempRx = false;
    inst->SkippedPackages = 0;     /* reset any skipped packages count  */
    inst->LastSkippedPackages = 0;
    inst->preloaded = false;

    if(inst->rx.data.file)
    {
      FILE_close(inst->rx.data.file);
      inst->rx.data.file = NULL;
    }
    if(inst->tx.data.file)
    {
      FILE_close(inst->tx.data.file);
      inst->tx.data.file = NULL;
    }
    inst->last_percent = -1; /* make sure we update percentage no matter what it is */
  }
}
/***************************************************************************************/

/* Simple wrapper for the SFH_set_error_string() function to reduce code usage and increase readability */
static void TRA_Err(uint8_t channel, uint32_t err)
{
 if ((channel == 0) || (channel > MAX_NUMBER_OF_CHANNELS))
	 return;
 SFH_set_error_string(channel, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, (uint16_t)err, channel));
}
/***************************************************************************************/
static void TRA_ErrStr(uint8_t channel, uint32_t err, char *str)
{
  if ((channel == 0) || (channel > MAX_NUMBER_OF_CHANNELS))
    return;
  SFH_set_error_string(channel, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, (uint16_t)err, str));
}
/***************************************************************************************/
static TRA_instance_type *GetInstance(uint8_t channel)
{
  if(channel > 0 && channel <= MAX_NUMBER_OF_CHANNELS)
    return TRA_instances[channel];
  return NULL;
}
/***************************************************************************************/
static void SendPercent(uint8_t channel, int percent)
{
  static char percentStr[4];
  TRA_instance_type *inst = GetInstance(channel);
  if(inst)
  {
    if(percent > 100) /* this should never happen, but in case of rounding errors */
      percent = 100;
    if(percent < 0)
      percent = 0;
    if(inst->last_percent != percent)
    {
      inst->last_percent = (int8_t)percent;
      snprintf(percentStr, 4,"%i",percent);
      GLOBAL_callback(channel, IFWD_DL_ProgressUpdated, percentStr);
    }
  }
}
/***************************************************************************************/
static uint8_t resetup_receive(TRA_instance_type *inst)
{
  if(inst && inst->repeat_cmd)
  {
    inst->rx.data.nof_total  = inst->repeat_cmd->data_length;
    inst->rx.data.nof_remain = inst->repeat_cmd->data_length; /* store total to receive */
    inst->rx.data.curr_pos   = inst->rx.data.orig_pos; /* go back to beginning again */
    if(inst->rx.dest == TRA_file && inst->rx.data.file)
      FILE_seek(inst->rx.data.file, 0);
    if(inst->repeat_cmd->data_length > inst->PackSizeLimit) /* Limit the amount of data we receive in one go */
      inst->rx.data.curr_len = inst->PackSizeLimit;
    else
      inst->rx.data.curr_len = inst->repeat_cmd->data_length;
    return true;
  }
  return false;
}
/***************************************************************************************/


/*---------------------------------------------*/
/* External functions.                         */
/*---------------------------------------------*/

/****************************************************************************************
* Function:... TRA_init
* Parameters:. -
* Returns:.... -
* Description: This function initialize this handler
* Created:.... DD.MM.YY by (Full name / initials)
* Modified:... DD.MM.YY by (Full name / initials)
*                Modifications note.
****************************************************************************************/
void TRA_init(void)
{
  memset((void*)TRA_instances,0,sizeof(TRA_instances));
  TRA_activeListNof = 0;
  TRA_Err(0,0);
  TRA_ErrStr(0,0,"void");
  TRA_receive_mem_data_no_tx(0,0,NULL,0,(TRA_progress_report_enum)0,(TRA_callback)0);
  TRA_IsFF((uint8_t*)"voidvoid",4);
}

/****************************************************************************************
* Function:... TRA_close
* Parameters:. -
* Returns:.... -
* Description: This function shut-down this handler
* Created:.... DD.MM.YY by (Full name / initials)
* Modified:... DD.MM.YY by (Full name / initials)
*                Modifications note.
****************************************************************************************/
void TRA_close(void)
{
  int n;
  /* free memory:*/
  TRA_instance_type *p;
  for(n = 1 ; n <= MAX_NUMBER_OF_CHANNELS ; n++)
  {
    p = TRA_instances[n];
    if(p)
    {
      free_repeat_cmd(p);
      if(p->rx.p_hdr)
        MEM_free((uint8_t*)p->rx.p_hdr);
      if(p->rx.p_buffer)
        MEM_free((uint8_t*)p->rx.p_buffer);
      if(p->tx.p_hdr)
        MEM_free((uint8_t*)p->tx.p_hdr);
      if(p->tx.p_buffer)
        MEM_free((uint8_t*)p->tx.p_buffer);
      MEM_free((uint8_t*)p);
      TRA_instances[n] = NULL;
    }
  }
  TRA_activeListNof = 0;
}

/***************************************************************************************/
static uint16_t CalcPercent(uint32_t position, uint32_t total)
{
  if(total > 10000000)   /* Possible overflow ? */
  {
    total = total / 1000;
    position = position / 1000;
  }
  return (uint16_t)(100 - (position * 100) / total);
}
/***************************************************************************************/


static uint8_t TRA_Prepare_Send(uint8_t channel)
{
  TRA_instance_type *inst = GetInstance(channel);
  if(inst)
  {
    uint32_t len;
    /* send progress report */
    if(inst->tx.SendProgress)
      SendPercent(channel,CalcPercent(inst->tx.data.nof_remain, inst->tx.data.nof_total));

    len = inst->tx.data.nof_remain;
    if(len > inst->PackSizeLimit) /* Limit the amount of data we send */
      len = inst->PackSizeLimit;

    if(inst->tx.dest == TRA_memory)
    {
      memcpy(inst->tx.p_buffer->data + inst->SkipHeader, inst->tx.data.curr_pos, len); /* copy data from source */
      inst->tx.data.curr_pos += len;
    }
    else if(inst->tx.dest == TRA_file)
    {
      if(FILE_read(inst->tx.data.file, inst->tx.p_buffer->data + inst->SkipHeader, len) != FILE_status_ok)  /* read data from file */
      {
        return false; /* aaargh! read error - then abort the process: return error */
      }
    }

    if(inst->preloaded)
      inst->preloaded_length = len;
    else
      inst->tx.data.nof_remain -= len;
    inst->tx.p_hdr->len = (uint32_t)len; /* tell protocol layer length in buffer */
    inst->tx.p_hdr->type = (package_type_t)inst->tx.package_code; /* make sure the correct package code is used */
  }
  return true;
}
/***************************************************************************************/

static void TRA_Send(uint8_t channel)
{
  TRA_instance_type *inst = GetInstance(channel);

  if(inst)
  {
    prot_status_t status;

    if(inst->SkipHeader) //compression
    {
      if(inst->tx.data.nof_remain && TRA_IsFF(inst->tx.p_buffer->data + inst->SkipHeader, inst->tx.p_hdr->len)) // skip if FF's and not the last one
      {
        if(inst->SkippedPackages < 0xFFFF) /* We can "only" skip 127 MB's in one go, so send an empty package if the limit is reached */
        {
          inst->state = TRA_state_Tx_Ok;  /* Fake that everything went 100% okay and that the package was sent (it was all FF's so no need to send it) */
          inst->SkippedPackages++;
          inst->LastSkippedPackages = inst->SkippedPackages;
          return;
        }
      }
    }

    inst->state = TRA_state_Tx; /* go into sending state */

    /* Calc the checksum on the payload data */
    if(inst->CalcChecksum)
      inst->data_checksum = TRA_FastChecksum((uint16_t*)(inst->tx.p_buffer->data + inst->SkipHeader), inst->tx.p_hdr->len, inst->data_checksum);

    if(inst->SkipHeader) //compression
    {
      inst->tx.p_hdr->len += inst->SkipHeader; /* remember to send the header as well */
      *((uint16_t*)inst->tx.p_buffer->data) = inst->SkippedPackages; /* Fill in with skipped package count (if any) */
      inst->LastSkippedPackages = inst->SkippedPackages;
      inst->SkippedPackages = 0; /* Remember that we already have skipped the packages */
    }
    status = PROT_tx_pack(channel, inst->tx.p_hdr, inst->tx.p_buffer, TRA_callback_tx);
    if(inst->state == TRA_state_Tx) /* still in Tx state (otherwise we don't case about the return value)? */
    {
      switch(status)
      {
      case prot_status_tx_pack_done:
        inst->state = TRA_state_Tx_Ok;
        break;
      case prot_status_tx_pack_ongoing: break;
      default:
        inst->state = TRA_state_Tx_Fail;
        break;
      }
    }
  }
}

/***************************************************************************************/
static void TRA_Receive(uint8_t channel)
{
  TRA_instance_type *inst = GetInstance(channel);
  if(inst)
  {
    prot_status_t prot_status;
    inst->state = TRA_state_Rx;
    prot_status = PROT_rx_pack(channel, inst->rx.p_hdr, inst->rx.p_buffer, TRA_callback_rx);
    if(inst->state == TRA_state_Rx) /* still in Rx state (otherwise we don't case about the return value)? */
    {
      switch(prot_status)
      {
      case prot_status_rx_pack_ready:
        inst->state = TRA_state_Rx_Ok; /* we are done receiving */
        break;
      case prot_status_rx_pack_ongoing:
        break; /* ignore - we will get callback later */
      default:
        inst->state = TRA_state_Rx_Fail;
        break;
      }
    }
  }
}

/****************************************************************************************
* Function:... TRA_clock
* Parameters:. -
* Returns:.... -
* Description: This function must be called repeatedly to clock the handler's internal state machine.
* Created:.... DD.MM.YY by (Full name / initials)
* Modified:... DD.MM.YY by (Full name / initials)
*                Modifications note.
****************************************************************************************/
void TRA_clock(void)
{
  uint16_t curr;
  if(TRA_activeListNof) /* any open channels? */
  {
    TRA_instance_type *inst;
    uint8_t channel;
    for(curr = 0 ; curr < TRA_activeListNof ; curr++)
    {
      channel = TRA_activeList[curr];
      inst = TRA_instances[channel];
      if(inst)
      {
        static TRA_state_type last_state;
        if (last_state != inst->state)
        {
          Debug3("TRA State: %d -> %d\n", last_state, inst->state);
          last_state = inst->state;
        }

        switch(inst->state)
        {
        case TRA_state_Tx_Ok:
          if(inst->tx.dest != TRA_none)
          {
            if(inst->tx.data.nof_remain == 0) /* done ? */
            {
              inst->SkippedPackages = 0; /* make absolutly sure that we reset any skipped packages count */
              if(inst->tx.data.file)
              {
                FILE_close(inst->tx.data.file);
                inst->tx.data.file = NULL;
              }
              inst->tx.dest = TRA_none;     /* mark Tx as done  */
              inst->state = TRA_state_idle; /* go to idle state for now (we may go into rx later) */
              if(inst->tx.SendProgress)
                SendPercent(channel,100);
              if(inst->tx.done_callback)
                inst->tx.done_callback( channel, TRA_status_ok, "" );
              if(inst->rx.dest != TRA_none) /* Any data to receive? */
              {
                inst->IgnoreTempRx = false; /* don't ignore rx anymore (we have sent all we want now, so Rx the last confirm) */
                inst->rx.p_hdr->len = inst->rx.data.curr_len; /* store what we expect to receive now */
                inst->rx.p_hdr->type = (package_type_t)inst->rx.package_code; /* make sure the correct package code is used */
                TRA_Receive(channel);
              }
            }
            else /* Send next package: */
            {
              if(inst->IgnoreTempRx && inst->rx.dest != TRA_none && !inst->SkippedPackages) /* should we Rx something before sending next block? */
              {
                resetup_receive(inst);
                TRA_Receive(channel); /* receive dummy data first */
                inst->preloaded = true;
                inst->preloaded_result = TRA_Prepare_Send(channel);
              }
              else
              {
                if(inst->preloaded)
                {
                  if(inst->preloaded_result)
                  {
                    inst->tx.data.nof_remain -= inst->preloaded_length;
                    inst->preloaded = false;
                    TRA_Send(channel);
                  }
                  else
                    inst->state = TRA_state_Tx_Fail;
                }
                else if(TRA_Prepare_Send(channel))
                  TRA_Send(channel);
                else
                  inst->state = TRA_state_Tx_Fail;
              }
            }
          }
          break;

        case TRA_state_Tx_Fail:
          if(inst->tx.done_callback)
            inst->tx.done_callback( channel, TRA_status_fail, "" );
          if(inst->rx.done_callback)
            inst->rx.done_callback( channel, TRA_status_fail, "" );
          goto_idle(inst);
          break;

        case TRA_state_Rx_Ok:
          if(inst->rx.dest == TRA_memory) /* Copy received data into destination: */
          {
            memcpy(inst->rx.data.curr_pos, inst->rx.p_buffer->data, inst->rx.data.curr_len);
            inst->rx.data.curr_pos += inst->rx.data.curr_len;
          }
          else if(inst->rx.dest == TRA_file)
          {
            FILE_write(inst->rx.data.file, inst->rx.p_buffer->data, inst->rx.data.curr_len);
          }
          inst->rx.data.nof_remain -= inst->rx.data.curr_len;

          /* send progress report */
          if(inst->rx.SendProgress)
            SendPercent(channel,CalcPercent(inst->rx.data.nof_remain, inst->rx.data.nof_total));
          if(inst->rx.data.nof_remain == 0) /* done ? */
          {
            /* should be setup up Rx again because we are sending a large chunks? */
            if(inst->IgnoreTempRx) /* sending large amounts of data and getting confirms? */
            {
              /* setup Rx again */
              resetup_receive(inst);

              /* Send next block: */
              if(inst->preloaded)
              {
                if(inst->preloaded_result)
                {
                  inst->tx.data.nof_remain -= inst->preloaded_length;
                  inst->preloaded = false;
                  TRA_Send(channel);
                }
                else
                  inst->state = TRA_state_Tx_Fail;
              }
              else if(TRA_Prepare_Send(channel))
                TRA_Send(channel);
              else
                inst->state = TRA_state_Tx_Fail;
            }
            else
            {
              goto_idle(inst);
              if(inst->rx.done_callback)
                inst->rx.done_callback( channel, TRA_status_ok, "" );
            }
          }
          else /* request next package (done by repeating last command): */
          {
            if(!inst->repeat_cmd) /* no repeat cmd stored? (should not happen) */
            {
              goto_idle(inst);
              if(inst->rx.done_callback)
                inst->rx.done_callback( channel, TRA_status_fail, "" );
            }
            else
            {
              inst->tx.done_callback = NULL; /* make sure that upper layers don't get multiple callbacks just because we repeat the command */

              if(inst->rx.data.nof_remain > *((uint16_t*)inst->repeat_cmd->data_pointer))
                inst->rx.data.curr_len = *((uint16_t*)inst->repeat_cmd->data_pointer);
              else
                inst->rx.data.curr_len = inst->rx.data.nof_remain;

              inst->tx.dest = TRA_memory; /* remember that we send from memory */
              inst->tx.package_code    = inst->repeat_cmd->package_code; /* fill in details again */
              inst->tx.data.curr_pos   = inst->repeat_cmd->data_pointer;
              inst->tx.data.nof_total  = inst->repeat_cmd->data_length;

              inst->tx.p_hdr->type   = inst->repeat_cmd->package_code; /* fill in the repeat command */
              inst->tx.p_hdr->len = inst->repeat_cmd->data_length;
              memcpy(inst->tx.p_buffer->data + inst->SkipHeader, inst->repeat_cmd->data_pointer, inst->tx.p_hdr->len);
              *(uint16_t*)(inst->tx.p_buffer->data + inst->SkipHeader) = inst->rx.data.curr_len;
              inst->tx.data.nof_remain = 0;
              inst->SkippedPackages = 0; /* Make sure we clear any skipped package count from previous sends */
              inst->LastSkippedPackages = 0;
              TRA_Send(channel);
            }
          }
          break;

        case TRA_state_Rx_Fail:
          goto_idle(inst);
          if(inst->rx.done_callback)
            inst->rx.done_callback( channel, TRA_status_fail, "" );
          break;
	    default:
	      break;
        }
      }
    }
  }
}

/****************************************************************************************
* Function:... TRA_callback_rx
* Parameters:. channel In  Specifies which channel instance a data block has been received for
* Returns:.... -
* Description: This is a callback function called by the protocol handler once the wanted data
*              has been received.
* Created:.... DD.MM.YY by (Full name / initials)
* Modified:... DD.MM.YY by (Full name / initials)
*                Modifications note.
****************************************************************************************/
void TRA_callback_rx(uint8_t channel, uint8_t success)
{
  TRA_instance_type *inst = GetInstance(channel);

  if(inst && inst->state == TRA_state_Tx_Ok)  /* If in TX_OK state clock once to allow state to come around to RX */
  {                                           /* This is in case the write confirm packages are skipped */
    TRA_clock();
  }

  if(inst && inst->state == TRA_state_Rx)  /* accept only rx callbacks while in receiving state */
  {
    if(!success) /* failure? */
      inst->state = TRA_state_Rx_Fail;
    else
      inst->state = TRA_state_Rx_Ok;
  }
}

/****************************************************************************************
* Function:... TRA_callback_tx
* Parameters:. channel In  Specifies which channel instance a data block has been sent for
* Returns:.... -
* Description: This is a callback function called by the protocol handler once the data has
*              been sent.
* Created:.... DD.MM.YY by (Full name / initials)
* Modified:... DD.MM.YY by (Full name / initials)
*                Modifications note.
****************************************************************************************/
void TRA_callback_tx(uint8_t channel, uint8_t success)
{
  TRA_instance_type *inst = GetInstance(channel);

  if(inst && inst->state == TRA_state_Tx)  /* accept only tx callbacks while in sending state */
  {
    if(success)
      inst->state = TRA_state_Tx_Ok;
    else
      inst->state = TRA_state_Tx_Fail;
  }
}


/****************************************************************************************
* Function:... TRA_add_channel
* Parameters:. channel  In  Number of the logical channel (1..N)
* Returns:.... Status
* Description: This function is used to add a channel to the handler.
*              It must allocate the needed memory and initialize all variables.
* Created:.... DD.MM.YY by (Full name / initials)
* Modified:... DD.MM.YY by (Full name / initials)
*                Modifications note.
****************************************************************************************/
uint8_t TRA_add_channel(uint8_t channel, char *status)
{
  TRA_instance_type *new_ch;
  uint8_t result = false;
  PARAM_NOT_USED(status);

  if(TRA_instances[channel]) /* channel already exists ? */
  {
    TRA_Err(channel, TRA_ERR_channel_already_opened);
  }
  else
  {
    new_ch = (TRA_instance_type *)MEM_malloc(sizeof(TRA_instance_type));
    if(new_ch)
    {
      int nof_bytes_hdr;
      int nof_bytes_payload;

      nof_bytes_hdr = sizeof(prot_header_t);
      if(target_capabilities[channel].package_size > 0)   /* 0 = 2048 bytes, else N*512 bytes */
        nof_bytes_payload = sizeof(prot_payload_t);
      else
        nof_bytes_payload = sizeof(prot_payload_t);

      new_ch->rx.p_hdr = (prot_header_t*)MEM_malloc(nof_bytes_hdr);
      new_ch->rx.p_buffer = (prot_payload_t*)MEM_malloc(nof_bytes_payload);
      new_ch->tx.p_hdr = (prot_header_t*)MEM_malloc(nof_bytes_hdr);
      new_ch->tx.p_buffer = (prot_payload_t*)MEM_malloc(nof_bytes_payload);

      new_ch->PackSizeLimit = GLOBAL_use_pack_size[channel];

      new_ch->SkipHeader = (target_capabilities[channel].compression == 1) ? TRA_COMPRESSION_HEADER_BYTES : 0;
    }

    if(!new_ch || !new_ch->rx.p_buffer || !new_ch->tx.p_buffer ||
       !new_ch->rx.p_hdr || !new_ch->tx.p_hdr)
    {
      TRA_Err(channel, TRA_ERR_malloc);
      if (new_ch)
      {
        if (new_ch->rx.p_hdr)    free(new_ch->rx.p_hdr);
        if (new_ch->rx.p_buffer) free(new_ch->rx.p_buffer);
        if (new_ch->tx.p_hdr)    free(new_ch->tx.p_hdr);
        if (new_ch->tx.p_buffer) free(new_ch->tx.p_buffer);
        free(new_ch);
      }
    }
    else
    {
      new_ch->state   = TRA_state_idle;
      new_ch->tx.dest = TRA_none; /* no send setup */
      new_ch->rx.dest = TRA_none; /* no receive setup */
      new_ch->repeat_cmd = NULL;  /* don't allocate repeat command before we need it */
      new_ch->rx.done_callback = NULL;
      new_ch->tx.done_callback = NULL;
      new_ch->rx.data.file = NULL;
      new_ch->tx.data.file = NULL;
      new_ch->IgnoreTempRx = false;
      new_ch->CalcChecksum = false;
      new_ch->preloaded = false;

      new_ch->last_percent = -1;
      TRA_instances[channel] = new_ch; /* store pointer to newly allocated channel */
      TRA_activeList[TRA_activeListNof] = channel; /* store new channel in active list */
      TRA_activeListNof++;                         /* remember that we have another one*/
      result = true;
    }
  }
  return result;
}

/****************************************************************************************
* Function:... TRA_delete_channel
* Parameters:. channel  In  Number of the logical channel (1..N)
* Returns:.... Status
* Description: This function is used to delete a channel from the handler.
*              It must free the allocated memory.
* Created:.... DD.MM.YY by (Full name / initials)
* Modified:... DD.MM.YY by (Full name / initials)
*                Modifications note.
****************************************************************************************/
uint8_t TRA_delete_channel(uint8_t channel, char *status)
{
  uint8_t result = false;
  TRA_instance_type *inst = GetInstance(channel);
  PARAM_NOT_USED(status);

  if(!inst) /* The channel does not exist? */
  {
    //    TRA_Err(channel, TRA_ERR_channel_does_not_exist);
  }
  else
  {
    if(inst->state != TRA_state_idle) /* The channel is busy? */
    {
      TRA_Err(channel, TRA_ERR_channel_close_busy);
    }
    else
    {
      int n;
      free_repeat_cmd(inst);
      if(inst->tx.p_hdr)
        MEM_free((uint8_t*)inst->tx.p_hdr);
      if(inst->tx.p_buffer)
        MEM_free((uint8_t*)inst->tx.p_buffer);
      if(inst->rx.p_hdr)
        MEM_free((uint8_t*)inst->rx.p_hdr);
      if(inst->rx.p_buffer)
        MEM_free((uint8_t*)inst->rx.p_buffer);
      MEM_free((uint8_t*)inst);
      TRA_instances[channel] = NULL;

      /* just rebuild active list instead of adding code to memmove in the list: */
      TRA_activeListNof = 0;
      for(n = 1 ; n <= MAX_NUMBER_OF_CHANNELS ; n++)
      {
        if(TRA_instances[n])
        {
          TRA_activeList[TRA_activeListNof] = (uint8_t)n;
          TRA_activeListNof++;
        }
      }
      result = true;
    }
  }
  return result;
}

/****************************************************************************************
* Function:... TRA_get_status
* Parameters:. channel  In  Number of the logical channel (1..N)
*              status_text  Out  Status text
* Returns:.... Status
* Description: Detailed function description
* Created:.... DD.MM.YY by (Full name / initials)
* Modified:... DD.MM.YY by (Full name / initials)
*                Modifications note.
****************************************************************************************/
#ifdef INCLUDE_UNUSED_FX
TRA_status_enum TRA_get_status(uint8_t channel, char *status_text)
{
  static const char * const status_strings[7] = {"Idle", "Tx", "Tx Fail", "Tx Ok", "Rx", "Rx Fail", "Rx Ok" }; /* only stored in ROM */
  TRA_instance_type *inst = GetInstance(channel);
  if(inst)
  {
    if(status_text)
      xstrcpy(status_text, status_strings[inst->state]);
    return TRA_status_ok;
  }
  if(status_text)
    xstrcpy(status_text, "None");
  return TRA_status_fail;
}
#endif


/****************************************************************************************
* Function:... TRA_abort_process
* Parameters:. channel  In  Number of the logical channel (1..N)
* Returns:.... -
* Description: This function terminates the current process on the selected channel.
*              It will not return until the underlying handler has reported that it has
*              finished the abort process and is returned to its initial state.
* Created:.... DD.MM.YY by (Full name / initials)
* Modified:... DD.MM.YY by (Full name / initials)
*                Modifications note.
****************************************************************************************/
void TRA_abort_process(uint8_t channel)
{
  TRA_instance_type *inst = GetInstance(channel);

  if(inst)
  {
    goto_idle(inst);
    inst->rx.done_callback = NULL;
    inst->tx.done_callback = NULL;
  }
}

/***************************************************************************************/

TRA_status_enum TRA_send_file_data(uint8_t channel,
                                   uint16_t package_code,
                                   char *file_name,
                                   uint32_t data_offset,
                                   uint32_t data_length,
                                   TRA_progress_report_enum report_progress,
                                   TRA_callback status_callback)
{
  TRA_status_enum result = TRA_status_fail;
  TRA_instance_type *inst = GetInstance(channel);

  if(inst)
  {
    if(inst->state == TRA_state_idle) /* allow send now? */
    {
      /* Copy input arguments to internal structures: */
      inst->tx.package_code = package_code;
      inst->tx.data.nof_remain = data_length;
      inst->tx.data.nof_total = data_length;
      inst->tx.SendProgress = (uint8_t)(report_progress != TRA_progress_report_disabled);
      inst->tx.done_callback = status_callback;
      inst->data_checksum = 0;
      inst->SkippedPackages = 0;
      inst->LastSkippedPackages = 0;

      //      inst->SkipHeader = (target_capabilities[channel].compression == 1) ? TRA_COMPRESSION_HEADER_BYTES : 0;
      //      inst->PackSizeLimit = PACK_DATA_SIZE;


      free_repeat_cmd(inst);
      inst->IgnoreTempRx = false;
      if(inst->rx.dest != TRA_none) /* expecting rx confirm? */
      {
        if(inst->rx.data.nof_total > inst->PackSizeLimit || inst->rx.dest == TRA_file) /* receiving LARGE data amounts after sending? */
        {
          TRA_Err(channel, TRA_ERR_cannot_tx_file_with_rx);
          return TRA_status_fail;
        }

        if(data_length > inst->PackSizeLimit) /* sending more than one block, so we need to setup rx again each time? */
        {
          inst->repeat_cmd = (TRA_cmd_buffer_type *)MEM_malloc(sizeof(TRA_cmd_buffer_type));
          inst->repeat_cmd->data_pointer = (uint8_t*)inst->rx.data.orig_pos;
          inst->repeat_cmd->data_pointer_allocated = false;
          inst->repeat_cmd->package_code = package_code;
          inst->repeat_cmd->data_length = inst->rx.data.nof_total;
          inst->IgnoreTempRx = true; /* remember that we should setup Rx and ignore it between each block we send */
        }
      }

      inst->tx.data.file = FILE_open(file_name, FILE_open_mode_read);
      if(inst->tx.data.file)
      {
        if(FILE_seek(inst->tx.data.file, data_offset) == FILE_status_ok)
        {
          inst->tx.dest = TRA_file; /* remember that we send from file */
          inst->preloaded = false; // remember that we haven't preloaded
          if(!TRA_Prepare_Send(channel))
          {
            TRA_ErrStr(channel, TRA_ERR_file_read_error, file_name);
            goto_idle(inst);
          }
          else
          {
            TRA_Send(channel);
            if(inst->state == TRA_state_Tx || inst->state == TRA_state_Tx_Ok)
              result = TRA_status_ok;
            else
            {
              FILE_close(inst->tx.data.file);
              inst->tx.data.file = NULL;
            }
          }
        }
        else
        {
          TRA_ErrStr(channel, TRA_ERR_file_seek_error, file_name);
          FILE_close(inst->tx.data.file);
          inst->tx.data.file = NULL;
        }
      }
      else
      {
        TRA_ErrStr(channel, TRA_ERR_file_not_found, file_name);
      }
    }
    else
    {
      TRA_Err(channel, TRA_ERR_channel_busy);
    }
  }
  return result;
}

/***************************************************************************************/


TRA_status_enum  TRA_send_mem_data(uint8_t channel,
                                   uint16_t package_code,
                                   uint8_t *data_pointer,
                                   uint32_t data_length,
                                   TRA_progress_report_enum report_progress,
                                   TRA_callback status_callback)
{
  TRA_status_enum result = TRA_status_fail;
  TRA_instance_type *inst = GetInstance(channel);

  if(inst)
  {
    if(inst->state == TRA_state_idle) /* allow send now? */
    {
      /* Copy input arguments to internal structures: */
      inst->tx.package_code = package_code;
      inst->tx.data.curr_pos = data_pointer;
      inst->tx.data.nof_remain = data_length;
      inst->tx.data.nof_total = data_length;
      inst->tx.SendProgress = (uint8_t)(report_progress != TRA_progress_report_disabled);
      inst->tx.done_callback = status_callback;
      inst->data_checksum = 0;
      inst->SkippedPackages = 0;
      inst->LastSkippedPackages = 0;

      //      inst->SkipHeader = (target_capabilities[channel].compression == 1) ? TRA_COMPRESSION_HEADER_BYTES : 0;
      //      inst->PackSizeLimit = PACK_DATA_SIZE;

      free_repeat_cmd(inst);
      inst->IgnoreTempRx = false;

      /* Set state variables: */
      inst->tx.dest = TRA_memory; /* remember that we send from memory */

      if(inst->rx.dest != TRA_none) /* expecting rx confirm? */
      {
#if 0
        if(inst->rx.dest == TRA_file) /* receiving LARGE data amounts after sending? */
        {
          TRA_Err(channel, TRA_ERR_cannot_tx_file_with_rx);
          return TRA_status_fail;
        }
#endif
        if(data_length > inst->PackSizeLimit) /* sending more than one block, so we need to setup rx again each time? */
        {
          inst->repeat_cmd = (TRA_cmd_buffer_type *)MEM_malloc(sizeof(TRA_cmd_buffer_type));
          inst->repeat_cmd->data_pointer = (uint8_t*)inst->rx.data.orig_pos;
          inst->repeat_cmd->data_pointer_allocated = false;
          inst->repeat_cmd->package_code = package_code;
          inst->repeat_cmd->data_length = inst->rx.data.nof_total;
          inst->IgnoreTempRx = true; /* remember that we should setup Rx and ignore it between each block we send */
        }
        else if(inst->rx.data.nof_total > inst->PackSizeLimit) /* receiving more than one block, so we need to setup rx again each time? */
        {
          inst->repeat_cmd = (TRA_cmd_buffer_type *)MEM_malloc(sizeof(TRA_cmd_buffer_type));
          inst->repeat_cmd->data_pointer = data_pointer;
          /*           inst->repeat_cmd->data_pointer = (uint8_t*)inst->rx.data.orig_pos;  */
          inst->repeat_cmd->data_pointer_allocated = false;
          inst->repeat_cmd->package_code = package_code;
          inst->repeat_cmd->data_length = inst->tx.data.nof_total;
          inst->IgnoreTempRx = true; /* remember that we should setup Rx and ignore it between each block we send */
        }
      }
      inst->preloaded = false; // remember that we haven't preloaded
      TRA_Prepare_Send(channel);
      TRA_Send(channel);
      if(inst->state == TRA_state_Tx || inst->state == TRA_state_Tx_Ok)
        result = TRA_status_ok;
    }
    else
    {
      TRA_Err(channel, TRA_ERR_channel_busy);
    }
  }
  return result;
}
/***************************************************************************************/


TRA_status_enum TRA_receive_file_data(uint8_t channel,
                                      uint16_t package_code,
                                      char *file_name,
                                      uint32_t force_package_size,
                                      uint32_t data_max_length,
                                      TRA_progress_report_enum report_progress,
                                      TRA_callback status_callback)
{
  TRA_status_enum result = TRA_status_fail;
  TRA_instance_type *inst = GetInstance(channel);

  if(inst)
  {
    if(data_max_length == 0)
      return TRA_status_ok;
    if(inst->rx.dest == TRA_none) /* no rx setup for this channel ? */
    {
      if(inst->rx.data.file)            /* any old files open? (should never happen) */
        FILE_close(inst->rx.data.file);
      inst->rx.data.file = FILE_open(file_name, FILE_open_mode_write_create);
      if(!inst->rx.data.file)
      {
        TRA_ErrStr(channel, TRA_ERR_cannot_write_to_file, file_name);
      }
      else
      {
        /* Copy input arguments to internal structures: */
        inst->rx.data.nof_total  = data_max_length;
        inst->rx.data.nof_remain = data_max_length; /* store total to receive */

        if(force_package_size > inst->PackSizeLimit) /* Limit the amount of data we receive in one go */
        	inst->rx.data.curr_len = (uint16_t)inst->PackSizeLimit;  /* current package size that we want next time */
        else
        	inst->rx.data.curr_len = (uint16_t)force_package_size;

        inst->rx.SendProgress = (uint8_t)(report_progress != TRA_progress_report_disabled);
        inst->rx.done_callback = status_callback;
        inst->rx.package_code = package_code;
        inst->rx.dest = TRA_file; /* remember that we want to rx to file */
        inst->IgnoreTempRx = false; /* dont ignore rx until Tx wants it */
        result = TRA_status_ok;  /* indicate ok back for now (it will not receive until something has been sent) */

        /**
         * Inserted FILE_close() because of Klocwork issue,
         * then removed to make upload_bin work again...
         **/
        //FILE_close(inst->rx.data.file);
        //inst->rx.data.file = NULL;
      }
    }
    else
    {
      TRA_Err(channel, TRA_ERR_channel_already_rx);
    }
  }
  return result;
}

/***************************************************************************************/

TRA_status_enum TRA_receive_mem_data(uint8_t channel,
                                     uint16_t package_code,
                                     uint8_t *data_pointer,
                                     uint32_t data_max_length,
                                     TRA_progress_report_enum report_progress,
                                     TRA_callback status_callback)
{
  TRA_status_enum result = TRA_status_fail;
  TRA_instance_type *inst = GetInstance(channel);

  if(inst)
  {
    if(data_max_length == 0)
      return TRA_status_ok;
    if(data_max_length > 0x40000000) /* more than 1 GB to rx to memory? Must be an error */
    {
      return TRA_status_fail;
    }

    if(inst->rx.dest == TRA_none) /* no rx setup for this channel ? */
    {
      /* Copy input arguments to internal structures: */
      inst->rx.data.orig_pos   = data_pointer;
      inst->rx.data.curr_pos   = data_pointer;
      inst->rx.data.nof_total  = data_max_length;
      inst->rx.data.nof_remain = data_max_length; /* store total to receive */
      if(data_max_length > inst->PackSizeLimit) /* Limit the amount of data we receive in one go */
        data_max_length = inst->PackSizeLimit;

      inst->rx.data.curr_len = data_max_length; /* current package size that we want next time */
      inst->rx.SendProgress = (uint8_t)(report_progress != TRA_progress_report_disabled);
      inst->rx.done_callback = status_callback;
      inst->rx.package_code = package_code;
      inst->rx.dest = TRA_memory; /* remember that we want to rx to memory */
      inst->IgnoreTempRx = false; /* dont ignore rx until Tx wants it */
      result = TRA_status_ok;  /* indicate ok back for now (it will not receive until something has been sent) */
    }
    else
    {
      TRA_Err(channel, TRA_ERR_channel_already_rx);
    }
  }


  return result;
}

TRA_status_enum TRA_receive_mem_data_no_tx(uint8_t channel,
                                           uint16_t package_code,
                                           uint8_t *data_pointer,
                                           uint32_t data_max_length,
                                           TRA_progress_report_enum report_progress,
                                           TRA_callback status_callback)
{
  TRA_status_enum result;
  TRA_instance_type *inst;

  if ((channel == 0) || (channel > MAX_NUMBER_OF_CHANNELS))
    return TRA_status_fail;

  result = TRA_status_fail;
  inst = GetInstance(channel);

  if(inst)
  {
    if(data_max_length == 0)
      return TRA_status_ok;
    if(data_max_length > 0x40000000) /* more than 1 GB to rx to memory? Must be an error */
    {
      return TRA_status_fail;
    }

    if(inst->rx.dest == TRA_none) /* no rx setup for this channel ? */
    {
      /* Copy input arguments to internal structures: */
      inst->rx.data.orig_pos   = data_pointer;
      inst->rx.data.curr_pos   = data_pointer;
      inst->rx.data.nof_total  = data_max_length;
      inst->rx.data.nof_remain = data_max_length; /* store total to receive */
      if(data_max_length > inst->PackSizeLimit) /* Limit the amount of data we receive in one go */
        data_max_length = inst->PackSizeLimit;

      inst->rx.data.curr_len = data_max_length; /* current package size that we want next time */
      inst->rx.SendProgress = (uint8_t)(report_progress != TRA_progress_report_disabled);
      inst->rx.done_callback = status_callback;
      inst->rx.package_code = package_code;
      inst->rx.dest = TRA_memory; /* remember that we want to rx to memory */
      inst->IgnoreTempRx = false; /* dont ignore rx until Tx wants it */

      inst->rx.p_hdr->len = inst->rx.data.curr_len; /* store what we expect to receive now */
      inst->rx.p_hdr->type = (package_type_t)inst->rx.package_code; /* make sure the correct package code is used */
      TRA_Receive(channel);

      inst->state = TRA_state_Rx; /* Indicate status as if there was a prewious TX */

      result = TRA_status_ok;  /* indicate ok back */
    }
    else
    {
      TRA_Err(channel, TRA_ERR_channel_already_rx);
    }
  }


  return result;
}

/***************************************************************************************/

uint16_t TRA_get_last_checksum(uint8_t channel)
{
  TRA_instance_type *inst = GetInstance(channel);
  if(inst)
    return inst->data_checksum;
  return 0;
}

/***************************************************************************************/
#ifdef INCLUDE_UNUSED_FX
void TRA_clear_checksum(uint8_t channel)
{
  TRA_instance_type *inst = GetInstance(channel);
  if(inst)
    inst->data_checksum = 0;
}
#endif
/***************************************************************************************/

#ifdef __BORLANDC__
// We don't want a warnings just because Borland doesn't like duff's device code
#pragma warn -8066 // Unreachable code
#endif

uint16_t TRA_FastChecksum(uint16_t *data_ptr, int byte_length, uint16_t initial_value)
{
#ifndef _WIN32  /* We may risk accessing 32 bits at uneven addresses, so check that we are on a Windows platform before proceeding */
  uint32_t *p, check = initial_value;

  if(byte_length & 3) // not 4 bytes length => read first uint16_t and proceed as normal
    check ^= *data_ptr++;
  p = (uint32_t*)data_ptr;
  byte_length /= (int)sizeof(uint32_t);
/*lint -e646 case/default within do loop; may have been misplaced (duffs device) */
/*lint -e616 control flows into case/default (needed and used by duffs device) */
  switch (byte_length & 7)  /* Use duffs device to accelerate unrolling of the loop */
  {
    do
    {
      check ^= *p++;
      case 7:  check ^= *p++;
      case 6:  check ^= *p++;
      case 5:  check ^= *p++;
      case 4:  check ^= *p++;
      case 3:  check ^= *p++;
      case 2:  check ^= *p++;
      case 1:  check ^= *p++;
      case 0:;
    } while ((byte_length -= 8) >= 0);
  }
/*lint +e646 case/default within do loop; may have been misplaced (duffs device) */
/*lint +e616 control flows into case/default (needed and used by duffs device) */
  return (check >> 16) ^ (check & 0xFFFF); /* combine upper and lower words to form final checksum */
#else  /* just use 16 bits accesses: */
  byte_length /= (int)sizeof(uint16_t);
/*lint -e646 case/default within do loop; may have been misplaced (duffs device) */
/*lint -e616 control flows into case/default (needed and used by duffs device) */
  switch (byte_length & 7)  /* Use duffs device to accelerate unrolling of the loop */
  {
    do
    {
      initial_value ^= *data_ptr++;
      case 7:  initial_value ^= *data_ptr++;
      case 6:  initial_value ^= *data_ptr++;
      case 5:  initial_value ^= *data_ptr++;
      case 4:  initial_value ^= *data_ptr++;
      case 3:  initial_value ^= *data_ptr++;
      case 2:  initial_value ^= *data_ptr++;
      case 1:  initial_value ^= *data_ptr++;
      case 0:;
    } while ((byte_length -= 8) >= 0);
  }
/*lint +e646 case/default within do loop; may have been misplaced (duffs device) */
/*lint +e616 control flows into case/default (needed and used by duffs device) */
  return initial_value;
#endif
}
#ifdef __BORLANDC__
// We don't want a warnings just because Borland doesn't like duff's device code
#pragma warn +8066 // Unreachable code
#endif

/***************************************************************************************/
void TRA_CalcChecksum(uint8_t channel, uint8_t calc)
{
  TRA_instance_type *inst = GetInstance(channel);
  if(inst)
    inst->CalcChecksum = calc;
}
/***************************************************************************************/
#ifdef __BORLANDC__
// We don't want a warnings just because Borland doesn't like duff's device code
#pragma warn -8066 // Unreachable code
#endif
static uint8_t TRA_IsFF(uint8_t *data_ptr, int byte_length)
{
  int len, count;
  uint32_t *p;
  /* skip to first 32 bit aligned address: */
  while((((intptr_t)data_ptr) & 3) && byte_length > 0)
  {
    if(*data_ptr++ != 0xFF)
      return false;
    byte_length--;
  }

  /* Now check 32 bits at a time: */
  p = (uint32_t *)data_ptr;
  len = byte_length / (int)sizeof(uint32_t);
  count = 0;
/*lint -e646 case/default within do loop; may have been misplaced (duffs device) */
/*lint -e616 control flows into case/default (needed and used by duffs device) */

  switch (len & 7)  /* Use duffs device to accelerate unrolling of the loop */
  {
    do
    {
      count += (int)(*p++ != 0xFFFFFFFF); /* we use a sum, since adding it much faster */
      case 7:  count += (int)(*p++ != 0xFFFFFFFF); /* than a jump that an if() would demand.    */
      case 6:  count += (int)(*p++ != 0xFFFFFFFF);
      case 5:  count += (int)(*p++ != 0xFFFFFFFF);
      case 4:  count += (int)(*p++ != 0xFFFFFFFF);
      case 3:  count += (int)(*p++ != 0xFFFFFFFF);
      case 2:  count += (int)(*p++ != 0xFFFFFFFF);
      case 1:  count += (int)(*p++ != 0xFFFFFFFF);
      case 0:;
    } while ((len -= 8) >= 0 && count == 0); /* now we check if we should exit */
  }
/*lint +e646 case/default within do loop; may have been misplaced (duffs device) */
/*lint +e616 control flows into case/default (needed and used by duffs device) */
  if(count > 0)
    return false;

  /* check the rest: */
  byte_length = byte_length & 3;
  data_ptr = (uint8_t *)p;
  while(byte_length > 0)
  {
    if(*data_ptr++ != 0xFF)
      return false;
    byte_length--;
  }
  return true;
}
#ifdef __BORLANDC__
// We don't want a warnings just because Borland doesn't like duff's device code
#pragma warn +8066 // Unreachable code
#endif
/***************************************************************************************/
void TRA_CapabilitiesChanged(uint8_t channel)
{
  TRA_instance_type *inst = GetInstance(channel);
  if(inst)
  {
    if(target_capabilities[channel].package_size > 0)   /* 0 = 2048 bytes, else N*512 bytes */
    {
      int extra_bytes = (target_capabilities[channel].package_size) * 1024;
      if(inst->rx.p_buffer)
        MEM_free((uint8_t*)inst->rx.p_buffer);
      if(inst->tx.p_buffer)
        MEM_free((uint8_t*)inst->tx.p_buffer);
      inst->rx.p_buffer = (prot_payload_t*)MEM_malloc(sizeof(prot_payload_t) + extra_bytes);
      inst->tx.p_buffer = (prot_payload_t*)MEM_malloc(sizeof(prot_payload_t) + extra_bytes);
    }
    inst->PackSizeLimit = GLOBAL_use_pack_size[channel];

    inst->SkipHeader = (target_capabilities[channel].compression == 1) ? TRA_COMPRESSION_HEADER_BYTES : 0;
  }
}
/***************************************************************************************/

uint32_t TRA_GetCommTimeOut(uint8_t channel)
{
  TRA_instance_type *inst = GetInstance(channel);
  if(inst)
  {
    if(inst->SkipHeader && inst->LastSkippedPackages)
    {
      return GLOBAL_comm_timeout + 32*inst->LastSkippedPackages + 1000;
    }
    return GLOBAL_comm_timeout;
  }
  return DEFAULT_GLOBAL_COMM_TIMEOUT; /* Hmm instance is not known yet - so give the default value back */
}
/***************************************************************************************/

