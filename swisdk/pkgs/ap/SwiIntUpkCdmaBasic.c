/**
 *    $Id: SwiIntUpkCdmaBasic.c,v 1.2 2010/08/16 17:48:58 ckaria Exp $
 *
 *    Name       : SwiIntUpkCdmaBasic.c
 *    
 *    Purpose    : Unpack functions for CDMA specific bassic APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */

#include "SwiIntUpkCdmaBasic.h"
#include "apidefs.h"

/**
 *
 * Unpacks the Meid/Esn information from the CnS object's 
 * parameter field.
 *
 * \param      paramfieldp   - Pointer to the CnS parameter field 
 *                             of the CnS object CNS_MEID_ESN.
 * \param      meidesnp      - Pointer to storage into which the Meid/Esn 
 *                             information will be placed by this function
 *
 */
package void SwiUnpackMeidEsn( swi_uint8 *paramfieldp, 
                               struct SWI_STRUCT_MeidEsn *meidesnp )
{
    swi_uint16 objectversion;
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    /* Assign the local copy */
    parmfp = paramfieldp;

    /* Get the object version value */
    objectversion = piget16( &parmfp );

    /* Get the MEID Enabled value */
    meidesnp->nMeidEnabled = piget16( &parmfp );

    /* Next comes the Meid Present flag */
    meidesnp->nMeidPresent = piget16( &parmfp );

    /* Next fetch the High 32 bits of Meid itself */
    meidesnp->nMeidHigh = piget32( &parmfp );

    /* Now the Low 32 bits of Meid */
    meidesnp->nMeidLow = piget32( &parmfp );

    /* and then comes the PEsn */
    meidesnp->nPEsn = piget32( &parmfp );

    /* Now the tEsn Present field */
    meidesnp->nTEsnPresent = piget16( &parmfp );

    /* Finally comes the ESN itself */
    meidesnp->nEsn = piget32( &parmfp );

    /* Update this structures size of itself */
    meidesnp->sizeStruct = sizeof( struct SWI_STRUCT_MeidEsn );
}


/**
 *
 * Unpacks the HDR MAC index information from the CnS object's 
 * parameter field.
 *
 * \param   paramfieldp         - Pointer to the CnS parameter field 
 *                                of the CnS object CNS_TECH_HDR_MAC_INDEX.
 * \param   pTechHdrMacIndex    - Pointer to storage into which the 
 *                                HDR MAC index information will be placed by 
 *                                this function.
 *
 */
package void SwiUnpackTechHdrMacIndex(
        swi_uint8 *paramfieldp, 
        SWI_STRUCT_Tech_Hdr_Mac_Index *pTechHdrMacIndex)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    piget16(&parmfp);   /* Discard obj_ver */
    pTechHdrMacIndex->mac_index_available = *parmfp++;
    pTechHdrMacIndex->mac_index = *parmfp;
}

/* 
 * $Log: SwiIntUpkCdmaBasic.c,v $
 */
 
