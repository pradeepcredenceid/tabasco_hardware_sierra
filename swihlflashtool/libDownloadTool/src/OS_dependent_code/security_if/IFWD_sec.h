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
 *  $File name:  /msw_tools/FlashTool/libDownloadTool/src/OS_dependent_code/security_if/IFWD_sec.h $
 *  $CC-Version: .../oint_tools_flashtoole2_01/6 $
 *  $Date:       2014-03-13    15:42:22 UTC $
 *   Responsible: bentzen
 *   Comment:
 *     SMS00847521: "Need to remove Lint warnings..."
 */

#ifndef IFWD_secH
#define IFWD_secH

#include "OS.h"
#include "header.h"  /* defines "SecurityStructType" */


/****************************************************************************
 *                          FUNCTIONS TO EXPORT                             *
 ****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
* Function:... uint8_t DecryptSecPack(SecurityStructType *Security, char **status_text, uint8_t UseHash)
* Parameters:.
*     In/Out  :   SecurityStructType *Security       : pointer to sec pack for investigation/modification
*     In/Out  :   char **status_text             : On fail this is set to a description of why it failed
*     In/Out  :   uint8_t UseHash                   : If true the SecPack is HASH'ed against the stored value.
*
* Returns:.... The function returns:
*              true on success, false if there is an error
* Description:
*
* Created:....
* Modified:...
*              Modifications note.
******************************************************************************/
extern uint8_t DecryptSecPack(SecurityStructType *Security, char **status_text, FILE *hash_file);

#ifdef __cplusplus
}
#endif

#endif  /* from #ifndef IFWD_secH */
