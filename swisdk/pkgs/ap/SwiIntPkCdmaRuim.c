/**
 *    $Id: SwiIntPkCdmaRuim.c,v 1.2 2010/06/10 00:23:06 epasheva Exp $
 *
 *    Name       : SwiIntPkCdmaRuim.c
 *    
 *    Purpose    : Pack functions for CDMA specific RUIM APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */

#include "SwiIntPkCdmaRuim.h"
#include "apidefs.h"

/**
 *
 * This function packs CHV code data to the CnS object's 
 * parameter field for SwiChangeCHV API.
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed  
 *                          data will be placed by this function
 * \param Chv[IN]           Type of CHV
 * \param szOldCHV          Old CHV code
 * \param szNewCHV          New CHV code
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackChangeCHV(swi_uint8 *paramfieldp, swi_uint16 Chv,
                                    const swi_charp szOldCHV, 
                                    const swi_charp szNewCHV)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */
    swi_uint32 nLen;

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(Chv, &paramfieldp);
    nLen = slstrlen(szOldCHV);
    *paramfieldp++ = (swi_uint8)nLen;
    piputmulti((const swi_uint8 *)szOldCHV, &localfieldp, nLen);
    paramfieldp += (LENGTH_Chv-nLen);
    nLen = slstrlen(szNewCHV);
    *paramfieldp++ = (swi_uint8)nLen;
    piputmulti((const swi_uint8 *)szNewCHV, &localfieldp, nLen);

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs unblock CHV code data to the CnS object's 
 * parameter field for SwiUnblockCHV API.
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed  
 *                          data will be placed by this function
 * \param Chv[IN]           Type of CHV
 * \param szUnblockCHV      Unblocking code
 * \param szCHV[IN]         New CHV code to use 
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackUnblockCHV(swi_uint8 *paramfieldp, swi_uint16 Chv,
                                     const swi_charp szUnblockCHV,
                                     const swi_charp szCHV)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */
    swi_uint32 nLen;

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(Chv, &paramfieldp);
    nLen = slstrlen(szUnblockCHV);
    *paramfieldp++ = (swi_uint8)nLen;
    piputmulti((const swi_uint8 *)szUnblockCHV, &localfieldp, nLen);
    paramfieldp += (LENGTH_Chv-nLen);
    nLen = slstrlen(szCHV);
    *paramfieldp++ = (swi_uint8)nLen;
    piputmulti((const swi_uint8 *)szCHV, &localfieldp, nLen);

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/* 
 * $Log: SwiIntPkCdmaRuim.c,v $
 */
