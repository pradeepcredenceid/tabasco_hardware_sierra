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
 *  $File name:  /msw_tools/FlashTool/libDownloadTool/src/OS_dependent_code/memory_os_if/IFWD_mem.h $
 *  $CC-Version: .../oint_tools_flashtoole2_01/9 $
 *  $Date:       2014-03-13    15:42:29 UTC $
 *   Responsible: bentzen
 *   Comment:
 *     SMS00847521: "Need to remove Lint warnings..."
 */

#ifndef __MEM_H__
#define __MEM_H__

#include "OS.h"

/****************************************************************************
 *                      ENUMERATIONS DEFINITIONS                            *
 ****************************************************************************/
typedef enum mem_status_s
{
  mem_status_ok = 0,
  mem_status_error,
  mem_status_not_valid
} mem_status_t;

/****************************************************************************
 *                          FUNCTIONS TO EXPORT                             *
 ****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
* Function:... uint8_t *MEM_malloc(uint32_t size)
* Parameters:.
*     In  :   uint32_t size       : size of the memory to allocate
*
* Returns:.... The function returns:
*              pointer to the allocated memory. NULL on failure.
* Description:
*
* Created:.... 03.01.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
extern uint8_t *MEM_malloc(uint32_t size);

/******************************************************************************
* Function:... MEM_free(uint8_t *mem_pointer)
* Parameters:.
*     In  :   uint8_t  mem_pointer: memory block to deallocate
*
* Returns:.... The function returns:
*              mem_status_ok if the memory deallocation was succesfull.
*              mem_status_error if the memory deallocation fails.
* Description:
*
* Created:.... 03.01.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
extern mem_status_t MEM_free(uint8_t *mem_pointer);

#ifdef __cplusplus
}
#endif

#endif  /* from #ifndef __MEM_H__ */
