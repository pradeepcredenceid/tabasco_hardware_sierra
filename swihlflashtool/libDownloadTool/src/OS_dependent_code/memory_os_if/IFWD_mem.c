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
 *  $File name:  /msw_tools/FlashTool/libDownloadTool/src/OS_dependent_code/memory_os_if/IFWD_mem.c $
 *  $CC-Version: .../dhkristx_140314_v0/1 $
 *  $Date:       2014-04-23    10:53:20 UTC $
 *   Responsible: bentzen
 *   Comment:
 *     SMS00847521: "Need to remove Lint warnings..."
 */

#define THIS_MODULE MODULE_NONE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "OS.h"
#include "DebugModule.h"
#include "IFWD_mem.h"

#ifdef __APPLE__
	#include <malloc/malloc.h>
#else
	#include <malloc.h>
#endif

/*****************************************************************************
 *                            Type definitions                               *
 *****************************************************************************/

typedef struct mem_status_inf_s
{
  mem_status_t mem_last_reported_error;
  mem_status_t mem_current_error;
} mem_status_inf_t;

/*****************************************************************************
 *                            PUBLIC PROTOTYPES                              *
 *****************************************************************************/
uint8_t *MEM_malloc(uint32_t size);
mem_status_t MEM_free(uint8_t *mem_pointer);

/***************************** IMPLEMENTATION ********************************/

/******************************************************************************
* Function:... uint8_t *MEM_malloc(uint32_t size)
* Parameters:.
*     In  :   uint32_t size       : size of the memory to allocate
*
* Returns:.... The function returns:
*              pointer to the allocated memory. NULL on failure
* Description:
*
* Created:.... 03.01.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
uint8_t *MEM_malloc(uint32_t size)
{
  uint8_t *result = (uint8_t *)malloc(size);
  if (!result)
  {
     // TODO: Report memory error
    PrintError("Memory allocation failed!\n");
    exit(0);
  }
  memset((uint8_t*)result, 0x00, size);
  return result;
}

/******************************************************************************
* Function:... MEM_free(uint8_t *mem_pointer)
* Parameters:.
*     In  :   uint8_t  mem_pointer: memory block to deallocate
*
* Returns:.... The function returns:
*              mem_status_ok if the memory deallocation was succesful.
*              mem_status_error if the memory deallocation fails (can only occur.
*              if the "mem_pointer" is NULL). The function does not validate that
*              the given pointer has been allocated.
* Description:
*
* Created:.... 03.01.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
mem_status_t MEM_free(uint8_t *mem_pointer)
{
  if(mem_pointer)
  {
    free((void *)mem_pointer);
    return mem_status_ok;
  }
  return mem_status_error;
}
