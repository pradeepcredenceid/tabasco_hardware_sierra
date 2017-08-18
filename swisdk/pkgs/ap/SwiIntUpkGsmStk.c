/**
 *
 * $Id: SwiIntUpkGsmStk.c,v 1.2 2010/06/10 00:25:24 epasheva Exp $
 *
 * Filename: SwiIntUpkGsmStk.c
 *
 * Purpose:  Contains unpacking routines for GSM Objects in the SIM Toolkit category.
 *
 * Copyright: © 2009 Sierra Wireless, Inc. all rights reserved
 *
 */
#include "apidefs.h"
#include "SwiApiGsmStk.h"
#include <string.h>
/**
 *
 * This function unpacks the STK Command from the CnS
 * object's parameter field
 *
 * \param  paramfieldp   - Pointer to the CnS parameter field of the 
 *                         CnS object containing the Phonebook entry
 * \param  STKCommandp   - Pointer to storage into which the STK Command
 *                         will be placed by this function
 *
 */
package void SwiUpkGsStkCmd(
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_STK_Command *STKCommandp)
{    
    /* Reference ProjectMemo036(GPRS Cns Object Definitions)
     * object: CNS_STK_CMD  0x1073 - 245 Bytes:
     */

    /* STK Handle */   
    STKCommandp->nSTK_Handle = piget32(&paramfieldp); 
    
    /* Text Type */
    pigetmulti(&paramfieldp, (swi_uint8 *)&(STKCommandp->eTextType), 1 ); 
    
    /* Menu Selection  */
    pigetmulti(&paramfieldp, (swi_uint8 *)&(STKCommandp->nMenuSelectionNumber), 1 ); 
    
    /* More flag */
    pigetmulti(&paramfieldp, (swi_uint8 *)&(STKCommandp->bMore), 1 ); 
    
    /* Number of UCS2 characters */
    pigetmulti(&paramfieldp, (swi_uint8 *)&(STKCommandp->cntWCHAR), 1 ); 
      
    /* unpack UCS2 string (convert from UCS2 to UTF8) */
    memset (&(STKCommandp->wText[0]), 0, sizeof (STKCommandp->wText) );
    SwiIntUcs2Unpack(&paramfieldp, 
                     &(STKCommandp->wText[0]), 
                     LENGTH_STKCommand_utf8, 
                     STKCommandp->cntWCHAR, 
                     OBJ_CNS_STK_CMD_STRING_LENGTH);

}

/**
 *
 * This function unpacks the STK Command Notification from the CnS
 * object's parameter field
 *
 * \param  paramfieldp   - Pointer to the CnS parameter field of the 
 *                         CnS object containing the STK COmmand Notification
 * \param  STKCmdNotifp  - Pointer to storage into which the STK Command
 *                         will be placed by this function
 *
 */
package void SwiUpkGsStkCmdNotif(
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_STK_Notif_Command *STKCmdNotifp)
{    
    /* Reference ProjectMemo036(GPRS Cns Object Definitions)
     * object: CNS_STK_CMD  0x1073 - 17 Bytes:
     */

    /* set structure length */ 
    STKCmdNotifp->sizeStruct = sizeof(struct SWI_STRUCT_STK_Notif_Command);
    
    /* STK Handle */   
    STKCmdNotifp->nSTK_Handle = piget32(&paramfieldp); 
    
    /* Timeout */   
    STKCmdNotifp->nTimeout    = piget32(&paramfieldp); 
    
    /* STK Command */
    pigetmulti(&paramfieldp, (swi_uint8 *)&(STKCmdNotifp->eSTKCommand), 1 ); 
    
    /* Operation */
    pigetmulti(&paramfieldp, (swi_uint8 *)&(STKCmdNotifp->eOperation), 1 ); 
    
    /* Text Type */
    pigetmulti(&paramfieldp, (swi_uint8 *)&(STKCmdNotifp->eTextType), 1 ); 
    
    /* Minimum Input length */   
    pigetmulti(&paramfieldp, (swi_uint8 *)&(STKCmdNotifp->nMinLengthOfInputExpected), 1); 
    
    /* Max Input length */   
    pigetmulti(&paramfieldp, (swi_uint8 *)&(STKCmdNotifp->nMaxLengthOfInputExpected), 1); 
    
    /* Echo */
    pigetmulti(&paramfieldp, (swi_uint8 *)&(STKCmdNotifp->bEcho), 1 ); 
    
    /* Key Type  */
    pigetmulti(&paramfieldp, (swi_uint8 *)&(STKCmdNotifp->eKeyType), 1 ); 
    
    /* Count of Menu Items */   
    pigetmulti(&paramfieldp, (swi_uint8 *)&(STKCmdNotifp->cntItemsInMenu), 1); 

    /* STK Priority  */
    pigetmulti(&paramfieldp, (swi_uint8 *)&(STKCmdNotifp->ePriority), 1 ); 
}

/*
 * $Log: SwiIntUpkGsmStk.c,v $
 */

