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
 *  $File name:  /msw_tools/FlashTool/libDownloadTool/src/OS_dependent_code/comm_if/IFWD_emuport.h $
 *  $CC-Version: .../oint_tools_flashtoole2_01/7 $
 *  $Date:       2014-03-13    15:42:48 UTC $
 *   Responsible: bentzen
 *   Comment:
 *     Initial emulation version needed for module testing
 */

#ifndef __EMUPORT_H__
#define __EMUPORT_H__

#include "IFWD_global_func.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 *                          CONSTANT DEFINITIONS                             *
 *****************************************************************************/

  uint8_t EMU_ReadFile(uint32_t h_comm,
                  void *block,
                  uint32_t  number_of_bytes_to_read,
                  uint32_t *number_of_bytes_read);

  uint8_t EMU_WriteFile(uint32_t h_comm,
                   void *block,
                   uint32_t  number_of_bytes_to_write,
                   uint32_t *number_of_bytes_written);

#ifdef __cplusplus
}
#endif

 #endif  /* #ifndef _COMPORT_H_ */

