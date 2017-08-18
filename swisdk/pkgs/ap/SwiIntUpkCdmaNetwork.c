/**
 *    $Id: SwiIntUpkCdmaNetwork.c,v 1.1 2008/10/15 21:22:28 YGao Exp $
 *
 *    Name       : SwiIntUpkCdmaNetwork.c
 *    
 *    Purpose    : Unpack functions for CDMA specific network APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */

#include "SwiIntUpkCdmaNetwork.h"
#include "apidefs.h"

/**
 *
 * Unpacks SID and NID values from the CnS object's 
 * parameter field.
 *
 * \param   paramfieldp - Pointer to the CnS parameter field of the CnS 
 *                        object CNS_PST_NV for NV item NV_HOME_SID_NID.
 * \param   psBsInfo    - Pointer to storage into which the SID and NID 
 *                        values will be placed by this function.
 *
 */
package void SwiPstUnpackHomeSidNid(swi_uint8 *paramfieldp, 
                                    SWI_STRUCT_HomeSidNid *HomeSidNid)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    swi_uint16 idx;

    parmfp = paramfieldp;

    piget16(&parmfp);   /* discard NV index */
    piget16(&parmfp);   /* discard Nam */

    for (idx = 0; idx < SWI_SIDNIDNUM; idx++) 
    { 
        HomeSidNid->SidNid[idx].Sid = piget16(&parmfp); 
        HomeSidNid->SidNid[idx].Nid = piget16(&parmfp);
    } 
}

/**
 *
 * Unpacks the connection state information from the CnS object's 
 * parameter field.
 *
 * \param   paramfieldp - Pointer to the CnS parameter field of the CnS 
 *                        object CNS_CALL_CONNECTED.
 * \param   psBsInfo    - Pointer to storage into which the connection state 
 *                        information will be placed by this function.
 *
 */
package void SwiUnpackCallConnectionStateEx(
        swi_uint8 *paramfieldp, 
        SWI_STRUCT_CallConnected* CallState)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    CallState->eState = piget16(&parmfp);
    CallState->nSo = piget16(&parmfp);
}

/**
 *
 * Unpacks the RSSI and EcIo values from the CnS object's parameter field.
 *
 * \param   paramfieldp - Pointer to the CnS parameter field of the CnS 
 *                        object CNS_RSSI_ECIO.
 * \param   psBsInfo    - Pointer to storage into which the RSSI and EcIo 
 *                        values will be placed by this function.
 *
 */
package void SwiUnpackRssiEcio(swi_uint8 *paramfieldp, 
                               SWI_STRUCT_Rssi_Ecio* stRssiEcio)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    stRssiEcio->iCdmaRssi = piget16(&parmfp);
    stRssiEcio->iCdmaEcio = piget16(&parmfp);
    stRssiEcio->iHdrRssi = piget16(&parmfp);
    stRssiEcio->iHdrEcio = piget16(&parmfp);
}

/**
 *
 * Unpacks the Active Set information from the CnS object's parameter field.
 *
 * \param   paramfieldp - Pointer to the CnS parameter field of the CnS 
 *                        object CNS_TECH_ACTIVE_SET.
 * \param   psBsInfo    - Pointer to storage into which the Active Set 
 *                        information will be placed by this function.
 *
 */
package void SwiUnpackTechActiveSet(swi_uint8 *paramfieldp, 
                                    SWI_STRUCT_TechActiveSet* pTechActSet)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    pTechActSet->nCount = piget16(&parmfp);
    if(pTechActSet->nCount <= MAX_ActiveSet)
    {
        swi_uint16 idx;

        for(idx = 0; idx < pTechActSet->nCount; idx++)
        {
            pTechActSet->ActiveSet[idx].iPilot = (swi_int16)piget16(&parmfp);
            pTechActSet->ActiveSet[idx].iEcio = (swi_int16)piget16(&parmfp);
        }
    }
}

/**
 * Name:     SwiUnpackRssiCdma
 *
 * Purpose:  This function unpacks the 16-bit RSSI from the CnS
 *           object's parameter field
 *
 * Parameters: paramfieldp           - Pointer to the CnS parameter field 
 *                                     of the CnS object containing the RSSI
 *             rssip                 - Pointer to storage into which the RSSI
 *                                     will be placed by this function
 *
 * Return:    None
 *
 * Note:      Applies to:
 *            ==========
 *            CnS Object ID 0x1001 (CNS_RSSI for CDMA)
 *
 *                                 CDMA                    GSM
 *                                 =====                   ====
 *            Get                   YES                     NO
 *            Set                   N/A                     NO
 *            Notifications         YES                     NO
 *
 */
package void SwiUnpackRssiCdma( swi_uint8 *paramfieldp, swi_int32 *rssip )
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    swi_int32 rssi;

    /* Assign the local copy */
    parmfp = paramfieldp;

    /* Unpack the rssi field into the local storage */
    rssi = piget16( &parmfp );

    /* Convert to a 32 bit integer */
    *rssip =  (swi_int32 )rssi;
}

/* 
 * $Log: SwiIntUpkCdmaNetwork.c,v $
 */

