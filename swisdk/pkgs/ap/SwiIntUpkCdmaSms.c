/**
 *    $Id: SwiIntUpkCdmaSms.c,v 1.1 2008/10/15 21:22:28 YGao Exp $
 *
 *    Name       : SwiIntUpkCdmaSms.c
 *    
 *    Purpose    : Unpack functions for CDMA specific SMS APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */

#include "SwiIntUpkCdmaSms.h"
#include "apidefs.h"

/**
 *
 * Unpacks the outstanding SMS message status from the CnS object's 
 * parameter field.
 *
 * \param   paramfieldp - Pointer to the CnS parameter field of the 
 *                        CnS object CNS_SMS_RECEIVED_MSG_STATUS_CDMA.
 * \param   psBsInfo    - Pointer to storage into which the 
 *                        outstanding SMS message status will be placed 
 *                        by this function.
 *
 */
package void SwiUnpackCdmaSMSMessageStatus(swi_uint8 *paramfieldp,
                                      SWI_STRUCT_CDMA_SMS_Status *pSMS_Status)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    pSMS_Status->cntUrgentMsg = piget16(&parmfp);
    pSMS_Status->cntRegularMsg = piget16(&parmfp);
    pSMS_Status->cntVoiceMsg = piget16(&parmfp);
}

/* 
 * $Log: SwiIntUpkCdmaSms.c,v $
 */
