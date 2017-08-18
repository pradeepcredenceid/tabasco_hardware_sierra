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
 *  $File name:  /msw_tools/FlashTool/libDownloadTool/src/OS_dependent_code/comm_if/IFWD_emuport.c $
 *  $CC-Version: .../dhkristx_140314_v0/1 $
 *  $Date:       2014-04-30    11:57:41 UTC $
 *   Responsible: bentzen
 *   Comment:
 *     Initial emulation version needed for module testing
 */

#define THIS_MODULE MODULE_COMM

#include "OS.h"
#include "IFWD_emuport.h"
#include "DebugModule.h"

/******************************************************************************
* Function:... uint8_t EMU_ReadFile(uint32_t h_comm,
*                                   void *block,
*                                   uint32_t  number_of_bytes_to_read,
*                                   uint32_t *number_of_bytes_read);
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 06.01.09 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
uint8_t EMU_ReadFile(uint32_t h_comm,
                     void *block,
                     uint32_t  number_of_bytes_to_read,
                     uint32_t *number_of_bytes_read)
{
  Debug2("Reading %d bytes...\n", number_of_bytes_to_read);

  uint8_t result = 1;
  GLOBAL_EmulateRx(h_comm, (void *)block, number_of_bytes_to_read, number_of_bytes_read);
//  if(*number_of_bytes_read)
//  {
//    Sleep(10); // give the test target emulatator som time to process
//  }
  uint32_t nread = *number_of_bytes_read;
  Debug("Read %d bytes [ %s ]\n", nread, getHex(block, nread>25 ? 25 : nread));
  return result;
}


/******************************************************************************
* Function:... uint8_t EMU_WriteFile(uint32_t h_comm,
*                                    void *block,
*                                    uint32_t  number_of_bytes_to_write,
*                                    uint32_t *number_of_bytes_written);
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 06.01.09 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
uint8_t EMU_WriteFile(uint32_t h_comm,
                      void *block,
                      uint32_t  number_of_bytes_to_write,
                      uint32_t *number_of_bytes_written)
{
  Debug2("Writing %d bytes...\n", number_of_bytes_to_write);
  GLOBAL_EmulateTx(h_comm, (uint8_t *)block, number_of_bytes_to_write, number_of_bytes_written);
  uint32_t wrote = *number_of_bytes_written;
  Debug("Wrote %d bytes [ %s ]\n", wrote, getHex(block, wrote>25 ? 25 : wrote));
  return 0;
}

