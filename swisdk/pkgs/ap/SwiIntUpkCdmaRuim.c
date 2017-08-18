/**
 *    $Id: SwiIntUpkCdmaRuim.c,v 1.1 2008/10/15 21:22:28 YGao Exp $
 *
 *    Name       : SwiIntUpkCdmaRuim.c
 *    
 *    Purpose    : Unpack functions for CDMA specific RUIM APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */

#include "SwiIntUpkCdmaRuim.h"
#include "apidefs.h"

/**
 *
 * Unpacks the RUIM Status information from the CnS object's 
 * parameter field.
 *
 * \param   paramfieldp - Pointer to the CnS parameter field 
 *                        of the CnS object CNS_RUIM_STATUS.
 * \param   psBsInfo    - Pointer to storage into which the 
 *                        RUIM Status information will be placed by 
 *                        this function.
 *
 */
package void SwiUnpackRUIMStatus(swi_uint8 *paramfieldp,
                                 SWI_STRUCT_RuimStatus* sRuimStatus)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    sRuimStatus->nStatus = piget16(&parmfp);
    sRuimStatus->byActionReq = piget16(&parmfp);
    sRuimStatus->byPrevAction = piget16(&parmfp);
    sRuimStatus->byPrevActRes = piget16(&parmfp);
    sRuimStatus->byRetryPres = piget16(&parmfp);
    sRuimStatus->byRetry = piget16(&parmfp);
    sRuimStatus->byRetryNum = piget16(&parmfp);
}

/* 
 * $Log: SwiIntUpkCdmaRuim.c,v $
 */
