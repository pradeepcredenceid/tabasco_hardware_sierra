/**
 *    $Id: SwiIntPkCdmaBasic.c,v 1.2 2010/06/10 00:23:06 epasheva Exp $
 *
 *    Name       : SwiIntPkCdmaBasic.c
 *    
 *    Purpose    : Pack functions for  CDMA specific bassic APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */

#include "SwiIntPkCdmaBasic.h"
#include "apidefs.h"

/**
 *
 * This function packs the old and new password to the CnS object's parameter 
 * field
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed 
 *                          data will be placed by this function.
 * \param OldPassword[IN]   Old password
 * \param NewPassword[IN]   Old password
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackChangeLockPIN(swi_uint8 *paramfieldp, 
                                        const swi_charp OldPassword, 
                                        const swi_charp NewPassword)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piputmulti((const swi_uint8 *)OldPassword, &localfieldp, slstrlen(OldPassword));
    piputmulti((const swi_uint8 *)NewPassword, &localfieldp, slstrlen(NewPassword));

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs the device EVDO settings to the CnS object's parameter 
 * field
 *
 * \param paramfieldp[OUT]  Pointer to storage into which the packed 
 *                          data will be placed by this function.
 * \param wMask[IN]         Device EVDO settings.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
extern swi_uint32 SwiPackDeviceEVDOSetting(swi_uint8 *paramfieldp, 
                                       swi_uint16 wMask)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(1, &localfieldp);
    
    switch(wMask)
    {
    case SWI_REV_A:
        piput32(SCP_PROT_SUBTYPE_MASK, &localfieldp); 
        piput32(SCP_BCMCS_SUBTYPE_MASK, &localfieldp);
        piput32(SCP_APP_SUBTYPE_MASK, &localfieldp);
        break;
    
    default:
        piput32(0, &localfieldp); 
        piput32(0, &localfieldp);
        piput32(0, &localfieldp);
        break;
    }
    

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/* 
 * $Log: SwiIntPkCdmaBasic.c,v $
 */

