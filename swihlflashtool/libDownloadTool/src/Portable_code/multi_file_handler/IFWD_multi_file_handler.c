/*
*       Copyright (C) 2011-2014 Intel Mobile Communications GmbH
*
*            Sec Class: Intel Confidential (IC)
*
*
* This document contains proprietary information belonging to IMC.
* Denmark A/S. Passing on and copying of this document, use and communication
* of its contents is not permitted without prior written authorisation.
*
* Description:
*   IFWD multi file handler - code
*     The purpose of the system handler is to do the sequencing in case multiple
*     files download or update processes must be carried out as a group.
*     This is the case when using SPK (Software pack) files.
*
* Revision Information:
*  $File name:  /msw_tools/FlashTool/libDownloadTool/src/Portable_code/multi_file_handler/IFWD_multi_file_handler.c $
*  $CC-Version: .../dhkristx_140314_v0/2 $
*  $Date:       2014-05-01    14:41:09 UTC $
*   Responsible: kofod
*   Comment:
*     SMS00887853 Error - not handling FLS file load offset when looking for the NVM table is fixed.
*/

#define THIS_MODULE MODULE_MFH

/*---------------------------------------------*/
/* Include files.                              */
/*---------------------------------------------*/

#include "OS.h"
#include "FileIO.h"
#include "PrgHandler.h"

#include "DebugModule.h"
#include "EBL2_nvm.h"
#include "IFWD_global_func.h"
#include "IFWD_single_file_handler.h"
#include "IFWD_multi_file_handler.h"

#include "FlsFile.h"

#define HANDLE_SFFS             0x00000001
#define HANDLE_DFFS             0x00000002
#define HANDLE_DFAT             0x00000004

#define NAND_SYSTEM_IMAGE_OFFSET       0x40000

#if 0
/* Defines used by the NVM functions */
#define NVM_STRUCT_OFFSET  0x00040414UL     /* Offset to the NVM struct in the FLS download image            */
#define NVM_STRUCT_MAGIC   0x5D60B742UL     /* Magic word must match the first word read at the given offset */
#define NVM_OPTION_DISABLE 0x00             /* Options disabled .. default if not set */
#define NVM_OPTION_ENABLE  0x01             /* Options enabled .. by the user         */

#define NVM_STRUCT_MAGIC_SIZE 3*sizeof(uint32_t) /* Size of the first header part */

#define NVM_STRUCT_MAGIC_OFS           0    /* Offset into the first header to read the magic word */
#define NVM_STRUCT_HEAD_LEN_OFS        0    /* Offset into the item header to read the length word */
#define NVM_STRUCT_HEAD_TAG_OFS        1    /* Offset into the item header to read the tag word    */
#define NVM_STRUCT_PAYLOAD_ERASE_PARAM 2    /* Offert into the item payload to read the erase parameter for erase items */
#define NVM_STRUCT_NO_TAG              0    /* Value to signal that a tag is not present           */
#define NVM_NOF_OPTIONS                256  /* Max number of options we can handle */
#endif
/*---------------------------------------------*/
/* typedef                                     */
/*---------------------------------------------*/
typedef enum
{
  MFH_state_idle = 0,

  /* Stages to wait for callback */
  MFH_state_load_ffs,
  MFH_state_load_ffs_2,

  /* pass or fail ... ALWAYS end here !!!!! */
  MFH_state_done_error,
  MFH_state_done_ok,

  MFH_state_load_dummy
} MFH_state_type;


typedef struct
{
  /* Linked list management */
  void *prev_ctx;                /* Pointer to the previous context in the linked list  */
  void *next_ctx;                /* Pointer to the previous context in the linked list  */
  uint8_t channel;                    /* The logical channel assigned to this context        */

  /* For the state machine */
  MFH_state_type state;          /* The current state of the SFH state machine          */
  MFH_state_type next_state;     /* The next state of the SFH state machine, (used for macro states)          */
  char status_text[500];     /* Used to hold status messages that can not be returned immidiately */
  uint32_t handling_flags;             /* Flags to trigger special handling in the state machine */
  MFH_status_enum status;        /* The status of the last opperation */
  MFH_done_signal_enum done_signal;    /* Tell us how to report the error or dons state  */

  /* For download handling */
  char load_file_name[500];

} MFH_ctx_type;

/*---------------------------------------------*/
/* globale variabler                           */
/*---------------------------------------------*/


/*---------------------------------------------*/
/* Local defines.                              */
/*---------------------------------------------*/


/*---------------------------------------------*/
/* Local const                                 */
/*---------------------------------------------*/


/*---------------------------------------------*/
/* local variables                             */
/*---------------------------------------------*/
static MFH_ctx_type *ctx_base_ptr = NULL;
static MFH_ctx_type *active_ctx_ptr = NULL;
static int MFH_nof_active_instances = 0;
static int MFH_nof_idle = 0;

/* NVM stuff */
static uint32_t MFH_nof_options = 0;
static uint8_t  MFH_options[NVM_NOF_OPTIONS];
static uint32_t MFH_option_tags[NVM_NOF_OPTIONS];
static uint32_t MFH_NVM_struct_version = 0;

void (*mfh_callback_store)(uint8_t channel, IFWD_DL_status_enum status, char *status_text); /* Pointer to the CallBack function */


/*---------------------------------------------*/
/* local functions.                            */
/*---------------------------------------------*/



static MFH_ctx_type *get_ctx(uint8_t channel)
{
  MFH_ctx_type *ctx_ptr;
  if (ctx_base_ptr == NULL)
  {
    return (NULL);
  }
  else
  {
    ctx_ptr = ctx_base_ptr;
    do
    {
      if (ctx_ptr->channel == channel)
      {
        return(ctx_ptr);
      }
      else
      {
        ctx_ptr = ctx_ptr->next_ctx;
      }
    } while(ctx_ptr);
  }
  /* could not find a match */
  return (NULL);
}

void MFH_callback (uint8_t channel, IFWD_DL_status_enum status, char *status_text)
{
  MFH_ctx_type *ctx_ptr;
  if ((0 < channel) && (channel <= MAX_NUMBER_OF_CHANNELS))
  {
    ctx_ptr = get_ctx(channel);
    if(ctx_ptr)
    {
      switch (status)
      {
        case IFWD_DL_OK:
        {
          ctx_ptr->state = ctx_ptr->next_state;
          break;
        }
        case IFWD_DL_Error:
        {
          ctx_ptr->state = MFH_state_done_error;
          break;
        }
        default:
        {
          mfh_callback_store (channel, status, status_text);
        }
      }
    }
  }
}

void mfh_init_ctx(uint8_t channel, MFH_ctx_type *this_ctx, char *status)
{

  /* init the context here */
  this_ctx->handling_flags = 0;
  this_ctx->channel = channel;
  this_ctx->state = MFH_state_idle;
  if (status)
    status[0] = 0;
  MFH_callback(0,(IFWD_DL_status_enum) 0, NULL);
}

void mfh_close_ctx(MFH_ctx_type *this_ctx)
{
  PARAM_NOT_USED(this_ctx);
}


static void clock_the_machine(MFH_ctx_type *act_ctx)
{

  switch (act_ctx->state)
  {
    /* ==================================================================== */
    case MFH_state_idle:
    /* ==================================================================== */
    {
      break;
    }
    /* ==================================================================== */
    case MFH_state_load_ffs:
    /* ==================================================================== */
    {
      mfh_callback_store = callback;
      callback = MFH_callback;
      if (act_ctx->handling_flags & HANDLE_DFAT)
      {
        if (SFH_download_dffs_file (act_ctx->channel, act_ctx->load_file_name, SFH_dffs_load_sel_nand_disk, SFH_done_signal_callback, act_ctx->status_text) == SFH_status_Error)
        {
          act_ctx->state = MFH_state_done_error; break;
        }
        act_ctx->state = MFH_state_idle;
        act_ctx->next_state = MFH_state_done_ok;
      }
      else
      {
        if (act_ctx->handling_flags & HANDLE_SFFS)
        {
          if (SFH_download_dffs_file (act_ctx->channel, act_ctx->load_file_name, SFH_dffs_load_sel_static, SFH_done_signal_callback, act_ctx->status_text) == SFH_status_Error)
          {
            act_ctx->state = MFH_state_done_error; break;
          }
          act_ctx->state = MFH_state_idle;
          act_ctx->next_state = MFH_state_load_ffs_2;
        }
        else
        {
          act_ctx->state = MFH_state_load_ffs_2;
        }
      }
      break;
    }
    /* ==================================================================== */
    case MFH_state_load_ffs_2:
    /* ==================================================================== */
    {
      if (act_ctx->handling_flags & HANDLE_DFFS)
      {
        if (SFH_download_dffs_file (act_ctx->channel, act_ctx->load_file_name, SFH_dffs_load_sel_dynamic, SFH_done_signal_callback, act_ctx->status_text) == SFH_status_Error)
        {
          act_ctx->state = MFH_state_done_error; break;
        }
        act_ctx->state = MFH_state_idle;
        act_ctx->next_state = MFH_state_done_ok;
      }
      else
      {
        act_ctx->state = MFH_state_done_ok;
      }
      break;
    }

    /* ==================================================================== */
    case MFH_state_done_error:
    /* ==================================================================== */
    {
      callback = mfh_callback_store;

      if (act_ctx->done_signal == MFH_done_signal_semaphore)
      {
        SetEvent( done_event );
      }
      else if (act_ctx->done_signal == MFH_done_signal_callback)
      {
        GLOBAL_callback(act_ctx->channel,IFWD_DL_Error, act_ctx->status_text);
      }
      /* Make sure no one is stuck in an error state in this channel: */
      SFH_abort_process(act_ctx->channel, act_ctx->status_text);

      act_ctx->state = MFH_state_idle;
      break;
    }

    /* ==================================================================== */
    case MFH_state_done_ok:
    /* ==================================================================== */
    {
      callback = mfh_callback_store;

      if (act_ctx->done_signal == MFH_done_signal_semaphore)
      {
        SetEvent( done_event );
      }
      else if (act_ctx->done_signal == MFH_done_signal_callback)
      {
        GLOBAL_callback(act_ctx->channel,IFWD_DL_OK, "");
      }
      act_ctx->state = MFH_state_idle;
      break;
    }
    default:
      break;
  }
}


/****************************************************************************************
* Function:... MFH_init
* Parameters:. -
* Returns:.... -
* Description: This function initialize this handler
* Created:.... DD.MM.YY by (Full name / initials)
* Modified:... DD.MM.YY by (Full name / initials)
*                Modifications note.
****************************************************************************************/
void MFH_init(void)
{
  ctx_base_ptr = NULL;
  MFH_nof_active_instances = 0;
  MFH_nof_idle = 0;
  GLOBAL_MFH_in_idle = true;
}


/****************************************************************************************
* Function:... MFH_clock
* Parameters:. -
* Returns:.... -
* Description: This function must be called repeatedly to clock the handler's internal state machine.
* Created:.... DD.MM.YY by (Full name / initials)
* Modified:... DD.MM.YY by (Full name / initials)
*                Modifications note.
****************************************************************************************/
void MFH_clock(void)
{
  if (active_ctx_ptr != NULL)
  {
    if(active_ctx_ptr->state == MFH_state_idle)
      MFH_nof_idle++;
    clock_the_machine(active_ctx_ptr);

    /* now find the next context to be active */
    active_ctx_ptr = active_ctx_ptr->next_ctx;
    if (active_ctx_ptr == NULL)
    {
      active_ctx_ptr = ctx_base_ptr;
      if(MFH_nof_idle != MFH_nof_active_instances && GLOBAL_MFH_in_idle) /* just changed from idle to active? */
        GLOBAL_signal_not_in_idle();  /* notify main loop that MFH is no longer in idle (needs OS dependent code to signal events to bring main loop out of idle)*/
      GLOBAL_MFH_in_idle = (MFH_nof_idle == MFH_nof_active_instances);
      MFH_nof_idle = 0;
    }
  }
  else
  {
    GLOBAL_MFH_in_idle = true;
    MFH_nof_idle = 0;
  }
}

/****************************************************************************************
* Function:... MFH_add_channel
* Parameters:. channel  In  Number of the logical channel (1..N)
* Returns:.... Status
* Description: This function is used to add a channel to the handler.
*              It must allocate the needed memory and initialize all variables.
* Created:.... DD.MM.YY by (Full name / initials)
* Modified:... DD.MM.YY by (Full name / initials)
*                Modifications note.
****************************************************************************************/
uint8_t MFH_add_channel(uint8_t channel,
                        char *status)
{
  MFH_ctx_type *new_ctx_ptr;
  MFH_ctx_type *ctx_ptr;

  if (!status)
  {
    Debug("Error in %s: Status not defined", __func__);
    return false;
  }

  /* Check if the channel exists */
  if (get_ctx(channel))
  {
    /* The channel existed */
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, MFH_ERR_channel_already_opened, channel) );
    return (false);
  }

  /* OK - so add the channel */
  /* Get the memory needed for the new context */
  new_ctx_ptr = (MFH_ctx_type *)MEM_malloc(sizeof(MFH_ctx_type));
  if (!new_ctx_ptr)
  {
    xstrcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, MFH_ERR_malloc) );
    return (false);
  }
  /* Got the memory so init the context */
  mfh_init_ctx(channel, new_ctx_ptr, status);
  MFH_nof_active_instances++; /* remember we have one more in the list */
  GLOBAL_signal_not_in_idle();  /* notify main loop that MFH is no longer in idle (needs OS dependent code to signal events to bring main loop out of idle)*/

  /* Insert the context in the linked list */
  if (ctx_base_ptr == NULL)
  {
    /* Its the first instance */
    new_ctx_ptr->prev_ctx = NULL;
    new_ctx_ptr->next_ctx = NULL;
    ctx_base_ptr = new_ctx_ptr;
  }
  else
  {
    /* insert the context as the first item in the list */
    ctx_ptr = ctx_base_ptr;

    /* Update the current 'first' context to see the new context as the previous */
    ctx_ptr->prev_ctx = new_ctx_ptr;

    /* Update the new context to see the current 'first' context as the next */
    new_ctx_ptr->next_ctx = ctx_ptr;
    new_ctx_ptr->prev_ctx = NULL;

    ctx_base_ptr = new_ctx_ptr;
  }

  /* Set the active context to whatever is pointed to by the base pointer */
  active_ctx_ptr = ctx_base_ptr;

  return (true);
}

/****************************************************************************************
* Function:... MFH_delete_channel
* Parameters:. channel  In  Number of the logical channel (1..N)
* Returns:.... Status
* Description: This function is used to delete a channel from the handler.
*              It must free the allocated memory.
* Created:.... DD.MM.YY by (Full name / initials)
* Modified:... DD.MM.YY by (Full name / initials)
*                Modifications note.
****************************************************************************************/
uint8_t MFH_delete_channel(uint8_t channel, char *status)
{
  MFH_ctx_type *ctx_ptr;

  PARAM_NOT_USED(status); /* to remove warning */

  /* Check if the channel exists */
  ctx_ptr = get_ctx(channel);

  if (!ctx_ptr)
  {
    /* The channel does not exist */
    //    strcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, MFH_ERR_channel_does_not_exist, channel) );
    return (false);
  }

  /* Use this function to free memory used by the context */
  mfh_close_ctx(ctx_ptr);

  /* Remove the context from the list */
  if (ctx_ptr->prev_ctx == NULL)
  {
    if (ctx_ptr->next_ctx == NULL)
    {
      /* Context is both first and last */
      ctx_base_ptr = NULL;
    }
    else
    {
      /* context is first but not last */
      ctx_base_ptr = ctx_ptr->next_ctx;
      ((MFH_ctx_type*)ctx_ptr->next_ctx)->prev_ctx = NULL;
    }
  }
  else
  {
    if (ctx_ptr->next_ctx == NULL)
    {
      /* Context is not first but last */
      ((MFH_ctx_type*)ctx_ptr->prev_ctx)->next_ctx = NULL;
    }
    else
    {
      /* Context is somewhere in the middle. */
      ((MFH_ctx_type*)ctx_ptr->prev_ctx)->next_ctx = ctx_ptr->next_ctx;
      ((MFH_ctx_type*)ctx_ptr->next_ctx)->prev_ctx = ctx_ptr->prev_ctx;
    }
  }
  /* Free the context structure */
  MEM_free((uint8_t*)ctx_ptr);
  MFH_nof_active_instances--;  /* remember we have one less in the list */
  MFH_nof_idle = 0;            /* Make sure we do not go into idle this time around the clock (in case we deleted an idle channel with an active channel still present) */
  GLOBAL_signal_not_in_idle(); /* notify main loop that MFH is no longer in idle (needs OS dependent code to signal events to bring main loop out of idle)*/

  /* Set the active context to whatever is pointed to by the base pointer */
  active_ctx_ptr = ctx_base_ptr;
  return (true);
}


MFH_status_enum MFH_open_comm_port(uint8_t channel,
                                   char *comm_port_name,
                                   char *ctrl_port_name,
                                   uint32_t baud_rate,
                                   char *status)
{
  MFH_add_channel(channel, status);

  if(SFH_open_comm_port(channel, comm_port_name, ctrl_port_name, baud_rate, status) == SFH_status_OK)
    return MFH_status_OK;
  /* Open was unsuccesful, so delete channel again, and return error: */
  MFH_delete_channel(channel, status);
  return MFH_status_Error;
}

MFH_status_enum MFH_close_comm_port(uint8_t channel,
                                    char *status)
{
  if(SFH_close_comm_port(channel, status) == SFH_status_OK)
  {
    MFH_delete_channel(channel, status);
    return MFH_status_OK;
  }
  return MFH_status_Error;
}

uint32_t MFH_GetDebugHeaderOffset(FILE *file)
{
  uint8_t offset[4];
  uint32_t marker;
  FILE_seek(file,0);
  FILE_read(file,(uint8_t*)&marker,4);
  if (marker != 0xED1D6491)
  {
    return(0);
  }
  FILE_seek(file,0x26);
  FILE_read(file,offset,4);
  marker  = offset[0] << 24;
  marker |= offset[1] << 16;
  marker |= offset[2] << 8;
  marker |= offset[3];
  return (marker);
}

MFH_status_enum  MFH_TOC_get_nof_items(char *filename, uint32_t *nof_output)
{
  MFH_status_enum result = MFH_status_Error;
  if(nof_output)
  {
    FILE *file;
    uint32_t file_offset;
    uint32_t debug_offset;
    TocStructType toc_main;
    //    FILE_status_enum file_status;

    *nof_output = 0; /* Assume zero for now */
    file = FILE_open(filename, FILE_open_mode_read);
    if(file)
    {
      debug_offset = MFH_GetDebugHeaderOffset(file);
      PRGH_SetFileOffset(debug_offset);
      if(PRGH_FileCheck_File(file) != INVALID_FILE_TYPE) /* Valid file version? */
      {
        PRGH_GetElementIndex_File(file, TOC_ELEM_TYPE, GENERIC_UID, &file_offset);
        FILE_seek(file, file_offset+sizeof(ElementHeaderStructType));
        /*file_status =*/ FILE_read(file, (uint8_t*)&(toc_main), sizeof(TocStructType));
        //   if (file_status != FILE_status_ok) { strcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  return (SFH_status_Error);  }
        *nof_output = toc_main.NoOfEntries;
        result = MFH_status_OK;
      } //else Debug("Invalid file type!!!\n");

      FILE_close(file);
    }
  }
  PRGH_SetFileOffset(0);
  return result;
}
/*-------------------------------------------------------------------------------------*/

MFH_status_enum  MFH_TOC_get_memory_class(char *filename, uint32_t toc_index, uint32_t *mem_class_output)
{
  MFH_status_enum result = MFH_status_Error;
  if(mem_class_output)
  {
    FILE *file;
    uint32_t file_offset;
    uint32_t debug_offset;
    TocStructType toc_main;
    TocEntryStructType toc_entry;
    //    FILE_status_enum file_status;
    uint32_t i;

    file = FILE_open(filename, FILE_open_mode_read);
    if(file)
    {
      debug_offset = MFH_GetDebugHeaderOffset(file);
      PRGH_SetFileOffset(debug_offset);
      if(PRGH_FileCheck_File(file) != INVALID_FILE_TYPE) /* Valid file version? */
      {
        PRGH_GetElementIndex_File(file, TOC_ELEM_TYPE, GENERIC_UID, &file_offset);
        FILE_seek(file, file_offset+sizeof(ElementHeaderStructType));
        /*file_status = */ FILE_read(file, (uint8_t*)&(toc_main), sizeof(TocStructType));
        //   if (file_status != FILE_status_ok) { strcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  return (SFH_status_Error);  }
        FILE_seek(file, toc_main.DataOffset+debug_offset);

        if(toc_index < toc_main.NoOfEntries)
        {
          for (i = 0 ; i < toc_index + 1 ; i++)
          {
            /*file_status =*/ FILE_read(file, (uint8_t*)&(toc_entry), sizeof(TocEntryStructType));
          }
          *mem_class_output = toc_entry.MemoryClass;
          result = MFH_status_OK;
        }
      }
      FILE_close(file);
    }
  }
  PRGH_SetFileOffset(0);
  return result;
}
/*-------------------------------------------------------------------------------------*/

MFH_status_enum  MFH_TOC_get_filename(char *filename, uint32_t toc_index, char *filename_output)
{
  MFH_status_enum result = MFH_status_Error;
  if(filename_output)
  {
    FILE *file;
    uint32_t file_offset;
    uint32_t debug_offset;
    TocStructType toc_main;
    TocEntryStructType toc_entry;
    //    FILE_status_enum file_status;
    uint32_t i;

    file = FILE_open(filename, FILE_open_mode_read);
    if(file)
    {
      debug_offset = MFH_GetDebugHeaderOffset(file);
      PRGH_SetFileOffset(debug_offset);
      if(PRGH_FileCheck_File(file) != INVALID_FILE_TYPE) /* Valid file version? */
      {
        PRGH_GetElementIndex_File(file, TOC_ELEM_TYPE, GENERIC_UID, &file_offset);
        FILE_seek(file, file_offset+sizeof(ElementHeaderStructType));
        /*file_status =*/ FILE_read(file, (uint8_t*)&(toc_main), sizeof(TocStructType));
        FILE_seek(file, toc_main.DataOffset+debug_offset);

        if (toc_index < toc_main.NoOfEntries)
        {
          for (i=0; i<toc_index+1; i++)
          {
            /*file_status = */ FILE_read(file, (uint8_t*)&(toc_entry), sizeof(TocEntryStructType));
          }
          sprintf(filename_output,"%s",(const char*)toc_entry.FileName);
          result = MFH_status_OK;
        }
      }
      FILE_close(file);
    }
  }
  PRGH_SetFileOffset(0);
  return result;
}

MFH_status_enum  MFH_TOC_get_sw_option(char *filename,   uint32_t toc_index,   /* IFWD_DL_sw_option_code_enum */ uint32_t option_code,   uint32_t *option_result)
{
  MFH_status_enum result = MFH_status_Error;

  if (option_result)
  {
	*option_result = 0; // Set result ot FALSE by default.

	switch((IFWD_DL_sw_option_code_enum)option_code)
	{
	  case IFWD_DL_sw_option_flashless_enabled:
	  {
		FILE *file;
		uint32_t file_offset;
		uint32_t debug_offset;
		TocStructType toc_main;
		TocEntryStructType toc_entry;
		// DownloadDataStructType download_data_struct;
		SecurityStructType  Security;
		MemoryMapStructType memory_map;
		HardwareStructType  hardware_struct;

		uint32_t m;
		uint32_t i; // counter for toc and load_map
		uint32_t tag_count = 0;
		uint32_t addr_match_count = 0;
		uint32_t start_addr;

		file = FILE_open(filename, FILE_open_mode_read);
		if(file)
		{
		  debug_offset = MFH_GetDebugHeaderOffset(file);
		  PRGH_SetFileOffset(debug_offset);
		  if(PRGH_FileCheck_File(file) != INVALID_FILE_TYPE) /* Valid file version? */
		  {
			// Get the main TOC
			PRGH_GetElementIndex_File(file, TOC_ELEM_TYPE, GENERIC_UID, &file_offset);
			FILE_seek(file, file_offset+sizeof(ElementHeaderStructType));
			/*file_status =*/ FILE_read(file, (uint8_t*)&(toc_main), sizeof(TocStructType));
			FILE_seek(file, toc_main.DataOffset+debug_offset);

			// Find the requested TOC entry
			memset((void*)&toc_entry,0,sizeof(toc_entry));
			if (toc_index < toc_main.NoOfEntries)
			{
			  for (i=0; i<toc_index+1; i++)
			  {
				/*file_status =*/ FILE_read(file, (uint8_t*)&(toc_entry), sizeof(TocEntryStructType));
			  }
			}
			else
			{
              FILE_close(file);
			  return(result); // toc_index out of range for the selected file.
			}

			// Get the flash technology
			PRGH_GetElementIndex_File(file, HARDWARE_ELEM_TYPE, GENERIC_UID, &file_offset);
			FILE_seek(file, file_offset+sizeof(ElementHeaderStructType));
			FILE_read(file, (uint8_t*)&(hardware_struct), sizeof(HardwareStructType));

			// Get the security structure
			PRGH_GetElementIndex_File(file,SECURITY_ELEM_TYPE, toc_entry.UID, &file_offset);
			FILE_seek(file, file_offset+sizeof(ElementHeaderStructType));
			FILE_read(file, (uint8_t*)&Security, sizeof(SecurityStructType));

			// Get the memory-map structure
			PRGH_GetElementIndex_File(file,MEMORYMAP_ELEM_TYPE, toc_entry.UID, &file_offset);
			FILE_seek(file, file_offset+sizeof(ElementHeaderStructType));
			FILE_read(file, (uint8_t*)&memory_map, sizeof(MemoryMapStructType));

			// were able to get the needed info so consider function run to be successful.
			result = MFH_status_OK;

			// For each of the entries in the loadmap having "_NVM" as part of the format specifier (TAG) ..
			// .. see if the start address matches a start address in the load-map.
			// Only if the all sets of addresses match the result may be set to TRUE.

			if (hardware_struct.FlashTechnology == FLASH_LESS)
			{
			  for (m = 0; m < MAX_MAP_ENTRIES; m++)
			  {
				if (strstr((const char*)memory_map.Entry[m].FormatSpecify,"_NVM"))
				{
				  start_addr = memory_map.Entry[m].Start;
				  tag_count++;

				  for (i = 0; i < NUMBER_OF_LOAD_MAPS; i++)
				  {
					if (Security.LoadMap[i].StartAddr == start_addr)
					{
					  addr_match_count++;
					}
				  }
				}
			  }
			  if (tag_count && (tag_count == addr_match_count))
			  {
				*option_result = 1; // Match yes yes yes
			  }
			}

		  } // if file type check
		  PRGH_SetFileOffset(0);
		  FILE_close(file);

		} // if file open
		else
		{
		  return(result); // file could not be opened.
		}
		break;
	  } // case
    default:
      break;
	} // switch
  } // if answer paramter not NULL
  return(result);
}

/*-------------------------------------------------------------------------------------*/

MFH_status_enum  MFH_get_sys_version(const char *filename, SystemVersionLabelType *labels)
{
  if (!filename || !labels) {
    PrintError("Missing SystemVersionLabelType structure\n");
    return MFH_status_Error;
  }
  int i;
  size_t ffc_head = 0;
  size_t read_ptr = 0;
  ElementHeaderStructType header;

  if (!verify_fls(filename))
  {
    return MFH_status_Error;
  }

  FILE* file = FILE_open((char*)filename, FILE_open_mode_read);
  if (!file) {
    PrintError("Cannot open File %s!\n", filename);
    return MFH_status_Error;
  }

  // Read first header (The Generic Header)
  FILE_read(file, (uint8_t*)&header, sizeof(header));
  if (header.Type != GENERIC_HEADER_ELEM_TYPE)
  {
    if (header.Type == 0xED1D6491)
    {
      char offset[4];
      FILE_seek(file, 0x26);
      FILE_read(file, (uint8_t*)&offset, sizeof(offset));

      ffc_head  = offset[0] << 24;
      ffc_head |= offset[1] << 16;
      ffc_head |= offset[2] << 8;
      ffc_head |= offset[3];

      Debug("This is an FFC header (Real header at 0x%X)\n", ffc_head);
      FILE_seek(file, ffc_head);
      FILE_read(file, (uint8_t*)&header, sizeof(header));
    }

    if (header.Type != GENERIC_HEADER_ELEM_TYPE)
    {
      PrintError("Invalid File Type!\n");
      FILE_close(file);
      return MFH_status_Error;
    }
  }

  read_ptr = ffc_head + header.Size;
  FILE_seek(file, read_ptr);

  // Looking for DOWNLOADDATA_ELEM_TYPE
  while (!FILE_EOF(file))
  {
    FILE_read(file, (uint8_t*)&header, sizeof(header));
    Debug3("Type %d: (Size: 0x%08X, UID: %d)\n",
           header.Type, header.Size, header.UID);

    read_ptr += header.Size;
    size_t body_size = header.Size - sizeof(header);
    if (body_size <= 0) {
      //PrintError("Body size less/equal to zero?!\n");
      break;
    }

    switch (header.Type)
    {
      case END_MARKER_ELEM_TYPE:
        break;

      case DOWNLOADDATA_ELEM_TYPE:
      {
        DownloadDataStructType elem;
        FILE_read(file, (uint8_t*)&elem, sizeof(DownloadDataStructType));

        Debug2("  DOWNLOADDATA_ELEM_TYPE!\n");

        char *ptr;
        size_t pos;
        static const char SwVersion[] = "!SYSTEM_VERSION DATA STRUCTURE!";

        // Don't allocate too much memory (Max 1MB)
        if (body_size >= 1024*1024)
          body_size = 1024*1024;

        char *data = (char*)MEM_malloc(body_size);
        FILE_read(file, (uint8_t*)data, body_size);

        // Search for the SYSTEM_VERSION string
        for(ptr = data, pos = 0; pos < body_size; ptr++, ++pos)
        {
          if (ptr[0] != SwVersion[0])
            continue;

          // Found first character. Now test whole string.
          if (strncmp(ptr, SwVersion, sizeof(SwVersion)) == 0)
          {
            // GoTo & read the SwVersion pointer of 4 bytes
            ptr += sizeof(SwVersion) + sizeof(int)*3;
            int low = (ptr[0] & 0x000000FF);
            int high = (ptr[1] & 0x000000FF);

            Debug(" -> Found SysVersion string at 0x%X: (ffc_head: 0x%X, elem.DataOffset: 0x%X, HighLow: 0x%X, +Offset: 0x%X, Pos: 0x%X)\n", pos+elem.DataOffset,
                  ffc_head, elem.DataOffset, (high<<8) + low, NAND_SYSTEM_IMAGE_OFFSET, pos);

            size_t SwVersionOffset = ffc_head + elem.DataOffset + low + (int)(high<<8);
            if (elem.DataOffset+pos > SwVersionOffset)
              SwVersionOffset += NAND_SYSTEM_IMAGE_OFFSET;

            Debug(" -> SwVersionOffset: 0x%X\n", SwVersionOffset);

            /* Parsing the strings into our Labels structure */
            char memory[1024*10];
            FILE_seek(file, SwVersionOffset);
            FILE_read(file, (uint8_t*)&memory, sizeof(memory));
            //hexDump(memory, sizeof(memory), 0);

            if (!isalnum(memory[0]) && !isblank(memory[0]) && !ispunct(memory[0]))
            {
              PrintError("Software Version string not correct (%d)!\n", memory[0]);
              FILE_close(file);
              free(data);
              return MFH_status_Error;
            }

            ptr = memory;
            labels->filename = strndup(filename, 512);
            labels->version = strndup(ptr, 512);
            ptr += strnlen(ptr, 512)+1;

            for(i=0;i<3;++i)
            {
              if (!isalnum(*ptr) && !isblank(*ptr) && !ispunct(*ptr))
              {
                 ptr++;
                 continue;
              }
              labels->customer[i] = strndup(ptr, 512);
              ptr += strnlen(ptr, 512)+1;
            }

            for(i=0;i<10;++i)
            {
              if (!isalnum(*ptr) && !isblank(*ptr) && !ispunct(*ptr))
              {
                 ptr++;
                 continue;
              }
               labels->subsystem[i] = strndup(ptr, 512);
               ptr += strnlen(ptr, 512)+1;
            }

            FILE_close(file);
            free(data);
            return MFH_status_OK;
          }
        }
        free(data);
        break;
      }

      default:
        break;
    }
    // GoTo next FlsHeaderElement
    FILE_seek(file, read_ptr);
  }

  FILE_close(file);
  //PrintError("System Version string not found\n");
  return MFH_status_Error;
}

/*-------------------------------------------------------------------------------------*/

MFH_status_enum  MFH_TOC_get_checksum(char *filename, uint32_t toc_index, uint32_t *checksum_output)
{
  MFH_status_enum result = MFH_status_Error;

  if(checksum_output)
  {
    FILE *file;
    uint32_t file_offset;
    uint32_t debug_offset;
    TocStructType toc_main;
    TocEntryStructType toc_entry;
    DownloadDataStructType download_data_struct;
    SecurityStructType Security;

    //    FILE_status_enum file_status;
    uint32_t i;

    file = FILE_open(filename, FILE_open_mode_read);
    if(file)
    {
      debug_offset = MFH_GetDebugHeaderOffset(file);
      PRGH_SetFileOffset(debug_offset);
      if(PRGH_FileCheck_File(file) != INVALID_FILE_TYPE) /* Valid file version? */
      {


        PRGH_GetElementIndex_File(file, TOC_ELEM_TYPE, GENERIC_UID, &file_offset);
        FILE_seek(file, file_offset+sizeof(ElementHeaderStructType));
        /*file_status =*/ FILE_read(file, (uint8_t*)&(toc_main), sizeof(TocStructType));
        //   if (file_status != FILE_status_ok) { strcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  return (SFH_status_Error);  }
        FILE_seek(file, toc_main.DataOffset+debug_offset);

        memset((void*)&toc_entry,0,sizeof(toc_entry));
        if (toc_index < toc_main.NoOfEntries)
        {
          for (i=0; i<toc_index+1; i++)
          {
            /*file_status =*/ FILE_read(file, (uint8_t*)&(toc_entry), sizeof(TocEntryStructType));
          }
        }

        PRGH_GetElementIndex_File(file,SECURITY_ELEM_TYPE, toc_entry.UID, &file_offset);
        FILE_seek(file, file_offset+sizeof(ElementHeaderStructType));
        FILE_read(file, (uint8_t*)&Security, sizeof(SecurityStructType));

        PRGH_GetElementIndex_File(file,DOWNLOADDATA_ELEM_TYPE, toc_entry.UID, &file_offset);
        FILE_seek(file, file_offset+sizeof(ElementHeaderStructType));
        FILE_read(file, (uint8_t*)&(download_data_struct), sizeof(DownloadDataStructType));
        *checksum_output = download_data_struct.CRC;

        for (i = 1; i < 8; i++) // The
        {
          if (Security.LoadMap[i].UsedLength)
          {
            PRGH_GetNextElementIndex_File(file, &file_offset);
            FILE_seek(file, file_offset+sizeof(ElementHeaderStructType));
            FILE_read(file, (uint8_t*)&(download_data_struct), sizeof(DownloadDataStructType));
            *checksum_output = *checksum_output ^ download_data_struct.CRC;
          }
        }
        result = MFH_status_OK;
      }
      FILE_close(file);
    }
  }
  PRGH_SetFileOffset(0);
  return result;
}
/*-------------------------------------------------------------------------------------*/

MFH_status_enum  MFH_TOC_get_filesize(char *filename, uint32_t toc_index, uint32_t *filesize_output)
{
  MFH_status_enum result = MFH_status_Error;

  if(filesize_output)
  {
    FILE *file;
    uint32_t file_offset;
    uint32_t debug_offset;
    TocStructType toc_main;
    TocEntryStructType toc_entry;
    DownloadDataStructType download_data_struct;
    //    FILE_status_enum file_status;
    uint32_t i;

    file = FILE_open(filename, FILE_open_mode_read);
    if(file)
    {
      debug_offset = MFH_GetDebugHeaderOffset(file);
      PRGH_SetFileOffset(debug_offset);
      if(PRGH_FileCheck_File(file) != INVALID_FILE_TYPE) /* Valid file version? */
      {
        PRGH_GetElementIndex_File(file, TOC_ELEM_TYPE, GENERIC_UID, &file_offset);
        FILE_seek(file, file_offset+sizeof(ElementHeaderStructType));
        /*file_status =*/ FILE_read(file, (uint8_t*)&(toc_main), sizeof(TocStructType));
        //   if (file_status != FILE_status_ok) { strcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  return (SFH_status_Error);  }
        FILE_seek(file, toc_main.DataOffset+debug_offset);

        memset((void*)&toc_entry,0,sizeof(toc_entry));
        if (toc_index < toc_main.NoOfEntries)
        {
          for (i=0; i<toc_index+1; i++)
          {
            /*file_status =*/ FILE_read(file, (uint8_t*)&(toc_entry), sizeof(TocEntryStructType));
          }
        }

        //    ctx_ptr->load_uid = toc_entry.UID
        PRGH_GetElementIndex_File(file,DOWNLOADDATA_ELEM_TYPE, toc_entry.UID, &file_offset);
        FILE_seek(file, file_offset+sizeof(ElementHeaderStructType));
        FILE_read(file, (uint8_t*)&(download_data_struct), sizeof(DownloadDataStructType));
        *filesize_output = download_data_struct.DataLength;
        result = MFH_status_OK;
      }
      FILE_close(file);
    }
  }
  PRGH_SetFileOffset(0);
  return result;
}

MFH_status_enum  MFH_TOC_get_flash_technogy(char *filename, uint32_t *flash_technology_output)
{
  MFH_status_enum result = MFH_status_Error;

  if(flash_technology_output)
  {
    FILE *file;
    uint32_t file_offset;
    uint32_t debug_offset;
    HardwareStructType hardware_struct;

    //    FILE_status_enum file_status;

    file = FILE_open(filename, FILE_open_mode_read);
    if(file)
    {
      debug_offset = MFH_GetDebugHeaderOffset(file);
      PRGH_SetFileOffset(debug_offset);
      if(PRGH_FileCheck_File(file) != INVALID_FILE_TYPE) /* Valid file version? */
      {
        PRGH_GetElementIndex_File(file, HARDWARE_ELEM_TYPE, GENERIC_UID, &file_offset);
        FILE_seek(file, file_offset+sizeof(ElementHeaderStructType));
        /*file_status =*/ FILE_read(file, (uint8_t*)&(hardware_struct), sizeof(HardwareStructType));
        //   if (file_status != FILE_status_ok) { strcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  return (SFH_status_Error);  }
        *flash_technology_output = hardware_struct.FlashTechnology;
        result = MFH_status_OK;
       }
      FILE_close(file);
    }
  }
  PRGH_SetFileOffset(0);

  return result;
}

MFH_status_enum  MFH_TOC_get_hw_platform(char *filename, uint32_t *hw_platform_output)
{
 MFH_status_enum result = MFH_status_Error;

 if(hw_platform_output)
 {
   FILE *file;
   uint32_t file_offset;
   uint32_t debug_offset;
   HardwareStructType hardware_struct;

   //    FILE_status_enum file_status;

   file = FILE_open(filename, FILE_open_mode_read);
   if(file)
   {
     debug_offset = MFH_GetDebugHeaderOffset(file);
     PRGH_SetFileOffset(debug_offset);
     if(PRGH_FileCheck_File(file) != INVALID_FILE_TYPE) /* Valid file version? */
     {
       PRGH_GetElementIndex_File(file, HARDWARE_ELEM_TYPE, GENERIC_UID, &file_offset);
       FILE_seek(file, file_offset+sizeof(ElementHeaderStructType));
       /*file_status =*/ FILE_read(file, (uint8_t*)&(hardware_struct), sizeof(HardwareStructType));
       //   if (file_status != FILE_status_ok) { strcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  return (SFH_status_Error);  }
       *hw_platform_output = hardware_struct.Platform;
       result = MFH_status_OK;
      }
     FILE_close(file);
   }
 }
 PRGH_SetFileOffset(0);

 return result;
}

/*-------------------------------------------------------------------------------------*/
MFH_status_enum  MFH_ZIP_get_size(uint32_t *size, char *filename)
{
#if 1
  MFH_status_enum result = MFH_status_Error;
  *size = 0;

  if (filename)
  {
    FILE *file;
    uint32_t file_offset;
    uint32_t debug_offset;
    InjectionDataStructType zip_element;

    file = FILE_open(filename, FILE_open_mode_read);
    if(file)
    {
      debug_offset = MFH_GetDebugHeaderOffset(file);
      PRGH_SetFileOffset(debug_offset);
      if(PRGH_FileCheck_File(file) != INVALID_FILE_TYPE) /* Valid file version? */
      {
        if (PRGH_GetElementIndex_File(file, INJECTED_INI_ELEM_TYPE, GENERIC_UID, &file_offset))
        {
          FILE_seek(file, file_offset+sizeof(ElementHeaderStructType));
          FILE_read(file, (uint8_t*)&(zip_element), sizeof(InjectionDataStructType));
          *size = zip_element.DataLength;
          result = MFH_status_OK;
        }
      }
    }
    FILE_close(file);
  }
  PRGH_SetFileOffset(0);
#else
  MFH_status_enum result = MFH_status_OK;
  *size = 5;
#endif
  return result;
}



MFH_status_enum  MFH_ZIP_get_data(uint32_t *data, char *filename)
{
#if 1
  MFH_status_enum result = MFH_status_Error;
  *data = 0;

  if (filename)
  {
    FILE *file;
    uint32_t file_offset;
    uint32_t debug_offset;
    InjectionDataStructType zip_element;

    file = FILE_open(filename, FILE_open_mode_read);
    if(file)
    {
      debug_offset = MFH_GetDebugHeaderOffset(file);
      PRGH_SetFileOffset(debug_offset);
      if(PRGH_FileCheck_File(file) != INVALID_FILE_TYPE) /* Valid file version? */
      {
        if (PRGH_GetElementIndex_File(file, INJECTED_INI_ELEM_TYPE, GENERIC_UID, &file_offset))
        {
          FILE_seek(file, file_offset+sizeof(ElementHeaderStructType));
          FILE_read(file, (uint8_t*)&(zip_element), sizeof(InjectionDataStructType));
          *data = zip_element.DataOffset+debug_offset;
          result = MFH_status_OK;
        }
      }
    }
    FILE_close(file);
  }
  PRGH_SetFileOffset(0);
#else
  MFH_status_enum result = MFH_status_OK;
  *data = *filename;
#endif
  return result;
}

MFH_status_enum MFH_NVM_get_nof_user_options(char *filename, uint32_t *nof_options)
{
  MFH_status_enum result = MFH_status_Error;

  /* Clear the lists */

  if (filename)
  {
    FILE *file;
    uint32_t file_offset;
    uint32_t debug_offset = 0;
    uint32_t nvm_tag;
    uint32_t nvm_termination_tag;
    uint32_t memory_tech = 0;
    uint32_t i;
    uint8_t tag_found;
    uint8_t found_code_class = false;
    uint32_t entry_head[2];
    uint32_t entry_payload[64];
    TocStructType toc_main;
    TocEntryStructType toc_entry;
    DownloadDataStructType download_data_struct;
    SecurityStructType Security;
    uint32_t nof_opt = 0;

    MFH_TOC_get_flash_technogy(filename, &memory_tech);

    memset((void*)&Security,0,sizeof(Security));
    file = FILE_open(filename, FILE_open_mode_read);
    if(file)
    {
      PRGH_SetFileOffset(debug_offset);
      if(PRGH_FileCheck_File(file) != INVALID_FILE_TYPE) /* Valid file version? */
      {
        PRGH_GetElementIndex_File(file, TOC_ELEM_TYPE, GENERIC_UID, &file_offset);
        FILE_seek(file, file_offset+sizeof(ElementHeaderStructType));
        /*file_status =*/ FILE_read(file, (uint8_t*)&(toc_main), sizeof(TocStructType));
        //   if (file_status != FILE_status_ok) { strcpy(status, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, SFH_ERR_file_error, FILE_get_last_error() ));  return (SFH_status_Error);  }
        FILE_seek(file, toc_main.DataOffset+debug_offset);

        memset((void*)&toc_entry,0,sizeof(toc_entry));

        for (i=0; i<toc_main.NoOfEntries+1; i++)
        {
          FILE_read(file, (uint8_t*)&(toc_entry), sizeof(TocEntryStructType));
          if (toc_entry.MemoryClass == CODE_CLASS)
          {

            PRGH_GetElementIndex_File(file,SECURITY_ELEM_TYPE, toc_entry.UID, &file_offset);
            FILE_seek(file, file_offset+sizeof(ElementHeaderStructType));
            FILE_read(file, (uint8_t*)&Security, sizeof(SecurityStructType));

            PRGH_GetElementIndex_File(file,DOWNLOADDATA_ELEM_TYPE, toc_entry.UID, &file_offset);
            FILE_seek(file, file_offset+sizeof(ElementHeaderStructType));
            FILE_read(file, (uint8_t*)&(download_data_struct), sizeof(DownloadDataStructType));
            debug_offset += download_data_struct.DataOffset; /* Add offset to the start of the CODE image */
            PRGH_SetFileOffset(0);

            // Check if this CODE image contains an NVM table
            Debug2("Offsets:\n   NVM_STRUCT_OFFSET: 0x%X \n   NVM_STRUCT_START_ADDR: 0x%X \n   NVM_STRUCT_OFFSET_NAND: 0x%X \n",
                NVM_STRUCT_OFFSET, NVM_STRUCT_START_ADDR, NVM_STRUCT_OFFSET_NAND);

            if (memory_tech == NOR_FLASH)
            {
              /* Read the initial 3 values and check the magic word. */
              file_offset = NVM_STRUCT_START_ADDR + debug_offset;
              Debug(" * Reading NAND NVM Default offset: 0x%X (NVM_STRUCT_OFFSET: 0x%X + FileOffset: 0x%X) * \n",
                     file_offset, NVM_STRUCT_OFFSET, debug_offset);
              FILE_seek(file, file_offset);
              FILE_read(file, (uint8_t*)entry_payload, NVM_STRUCT_MAGIC_SIZE); // Read the NVM struct header part.
              MFH_NVM_struct_version = 1; // Force the version to 1 for NOR systems as the EBLs are not ver2 ready
            }
            else
            {
              /* Read the initial 3 values and check the magic word. */
              file_offset = NVM_STRUCT_OFFSET_NAND + debug_offset;
              Debug(" * Reading NAND NVM Default offset: 0x%X (NVM_STRUCT_OFFSET_NAND: 0x%X + FileOffset: 0x%X) * \n",
                     file_offset, NVM_STRUCT_OFFSET_NAND, debug_offset);
              FILE_seek(file, file_offset);
              FILE_read(file, (uint8_t*)entry_payload, NVM_STRUCT_MAGIC_SIZE); // Read the NVM struct header part.

              /**
                * Note: Added because we simply don't know!!!!
                **/
              if (entry_payload[NVM_STRUCT_MAGIC_OFS] != NVM_STRUCT_MAGIC)
              {
                file_offset = NVM_STRUCT_OFFSET + debug_offset;
                Debug(" * Trying NVM_STRUCT_OFFSET (0x%X, MemTech: %d, FileOffset: 0x%X) * \n",
                      NVM_STRUCT_OFFSET, memory_tech, debug_offset);
                FILE_seek(file, file_offset);
                FILE_read(file, (uint8_t*)entry_payload, NVM_STRUCT_MAGIC_SIZE); // Read the NVM struct header part.
              }
              MFH_NVM_struct_version = entry_payload[NVM_STRUCT_VER_OFS];
            }

            if (entry_payload[NVM_STRUCT_MAGIC_OFS] == NVM_STRUCT_MAGIC)
            {
              /* Magic word is OK */
              found_code_class = true;
              break;
            }
          }
        }
      }

      if (found_code_class)
      {
        if (MFH_NVM_struct_version == 2)
        {
          nvm_termination_tag = NVM_TAG_TERMINATOR_V2;
        }
        else
        {
          nvm_termination_tag = NVM_TAG_TERMINATOR;
        }

        /* Magic word is OK */
        memset((void*)MFH_options, (int)NVM_OPTION_DISABLE, sizeof(MFH_options));        /* Clear the options field */
        memset((void*)MFH_option_tags, (int)NVM_STRUCT_NO_TAG, sizeof(MFH_option_tags)); /* Clear the options tag field */
        do
        {
          /* Read item header */
          FILE_read(file, (uint8_t*)entry_head, sizeof(entry_head)); // Read the NVM struct header part.
          nvm_tag = entry_head[NVM_STRUCT_HEAD_TAG_OFS];
          if (entry_head[NVM_STRUCT_HEAD_LEN_OFS])
          {
            /* Read item payload */
            FILE_read(file, (uint8_t*)entry_payload, entry_head[NVM_STRUCT_HEAD_LEN_OFS]-sizeof(entry_head)); // Read the NVM struct header part.

            /* Check if the tag is already in the list, if it is then no action is needed. */
            tag_found = false;
            for (i = 0; i<nof_opt; i++)
            {
              if (MFH_option_tags[i] == nvm_tag)
              {
                tag_found = true;
                break;
              }
            }

            /* If this tag is not yet marked .. check if it triggers an user option. */
            /* But only if the NVM version is 1 */
            if (!tag_found && (MFH_NVM_struct_version == 1))
            {
              switch (nvm_tag)
              {
                case NVM_TAG_ERASE_CAL:   //lint -fallthrough
                case NVM_TAG_ERASE_FIX:   //lint -fallthrough
                case NVM_TAG_ERASE_DYN:
                {
                  if (entry_payload[NVM_STRUCT_PAYLOAD_ERASE_PARAM] == NVM_ERASE_OPTIONAL)
                  {
                    MFH_option_tags[nof_opt] = nvm_tag;
                    nof_opt++;
                  }
                  break;
                }
                case NVM_TAG_STARTUP_MODE_TEST:    //lint -fallthrough
                case NVM_TAG_STARTUP_MODE_NORMAL:
                case NVM_TAG_BLTDO:
                {
                  MFH_option_tags[nof_opt] = nvm_tag;
                  nof_opt++;
                  break;
                }
              }
            }
            /* If this tag is not yet marked .. check if it triggers an user option. */
            /* But only if the NVM version is 2 */
            if (!tag_found && (MFH_NVM_struct_version == 2))
            {
              switch (nvm_tag)
              {
                case NVM_TAG_ERASE_CAL_V2:   //lint -fallthrough
                case NVM_TAG_ERASE_FIX_V2:   //lint -fallthrough
                case NVM_TAG_ERASE_DYN_V2:
                case NVM_TAG_ERASE_MISC_V2:
                {
                  if (entry_payload[NVM_STRUCT_PAYLOAD_ERASE_PARAM + 10] == NVM_ERASE_OPTIONAL)
                  {
                    MFH_option_tags[nof_opt] = nvm_tag;
                    nof_opt++;
                  }
                  break;
                }
                case NVM_TAG_STARTUP_MODE_TEST_V2:    //lint -fallthrough
                case NVM_TAG_STARTUP_MODE_NORMAL_V2:
                case NVM_TAG_BLTDO_V2:
                {
                  MFH_option_tags[nof_opt] = nvm_tag;
                  nof_opt++;
                  break;
                }
              }
            }
          }
        } while (nvm_tag != nvm_termination_tag);
        MFH_nof_options = nof_opt;
      }
      result = MFH_status_OK;
      FILE_close(file);
    }
    if (nof_options)
    {
      *nof_options = nof_opt;
    }
  }
  return result;
}

MFH_status_enum MFH_NVM_get_user_option(char *filename, uint32_t opt, uint32_t *nvm_tag)
{
  MFH_status_enum result = MFH_status_Error;

  if (filename && (opt < MFH_nof_options))
  {
    if(nvm_tag)
    {
      *nvm_tag = MFH_option_tags[opt];
    }
    result = MFH_status_OK;
  }
  return result;
}


MFH_status_enum MFH_NVM_set_user_option(char *filename, uint32_t opt, uint8_t value)
{
  MFH_status_enum result = MFH_status_Error;

  if (filename && (opt < MFH_nof_options))
  {
    if (value)
    {
      MFH_options[MFH_option_tags[opt]] = NVM_OPTION_ENABLE;
    }
    else
    {
      MFH_options[MFH_option_tags[opt]] = NVM_OPTION_DISABLE;
    }

    if (SFH_set_nvm_options(NVM_NOF_OPTIONS, (uint8_t*)MFH_options) == SFH_status_OK)
    {
      result = MFH_status_OK;
    }
  }

  return result;
}
