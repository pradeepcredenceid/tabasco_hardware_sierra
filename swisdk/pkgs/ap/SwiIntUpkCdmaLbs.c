/**
 *    $Id: SwiIntUpkCdmaLbs.c,v 1.2 2009/09/25 01:36:27 YGao Exp $
 *
 *    Name       : SwiIntUpkCdmaLbs.c
 *    
 *    Purpose    : Unpack functions for CDMA specific LBS APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */
 
#include "SwiApiCdmaLbs.h"
#include "SwiIntUpkCdmaLbs.h"
#include "apidefs.h"

/**
 *
 * Unpacks the GPS base station information from the CnS object's 
 * parameter field.
 *
 * \param   paramfieldp - Pointer to the CnS parameter field of the CnS 
 *                        object CNS_LBS_PA_BS_INFO.
 * \param   psBsInfo    - Pointer to storage into which the GPS base station 
 *                        information will be placed by this function.
 *
 */
package void SwiUnpackLbsPaBsInfo(swi_uint8 *paramfieldp, 
                                  SWI_STRUCT_LbsPaBsInfo *psBsInfo)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    piget16(&parmfp);   /* Discard obj_ver */

    psBsInfo->bInfoStale = piget16(&parmfp);
    psBsInfo->nId = piget16(&parmfp);
    psBsInfo->nSid = piget16(&parmfp);
    psBsInfo->nNid = piget16(&parmfp);

    psBsInfo->nInfoTimeHigh = piget32(&parmfp);
    psBsInfo->nInfoTimeLow = piget32(&parmfp);

    psBsInfo->nLatitude = (swi_int32)piget32(&parmfp);
    psBsInfo->nLongitude = (swi_int32)piget32(&parmfp);
}

/**
 *
 * Unpacks the GPS fix type information from the CnS object's 
 * parameter field.
 *
 * \param   paramfieldp - Pointer to the CnS parameter field 
 *                        of the CnS object CNS_PST_NV for NV item
 *                        NV_SWI_GPS_MODE_SUPPORT_I.
 * \param   psBsInfo    - Pointer to storage into which the GPS fix type 
 *                        information will be placed by this function.
 *
 */
package void SwiUnpackLbsFixTypeControl(
        swi_uint8 *paramfieldp, 
        SWI_STRUCT_LBS_FixTypeControl *pFixTypeControl)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    piget16(&parmfp);   /* Skip NV index */

    pFixTypeControl->bStandalone = *parmfp++;
    pFixTypeControl->bMSBased = *parmfp++;
    pFixTypeControl->bMSAssisted = *parmfp++;
    pFixTypeControl->bOptimalAccuracy = *parmfp++;
    pFixTypeControl->bOptimalSpeed = *parmfp++;
    pFixTypeControl->bOptimalData = *parmfp++;
}

/**
 *
 * Unpacks the GPS AutoStart information from the CnS object's 
 * parameter field.
 *
 * \param   paramfieldp - Pointer to the CnS parameter field 
 *                        of the CnS object CNS_PST_NV for NV item
 *                        NV_SWI_GPS_AUTO_TRACK_I.
 * \param   psBsInfo    - Pointer to storage into which the GPS AutoStart 
 *                        information will be placed by this function.
 *
 */
package void SwiUnpackLbsAutoStart(swi_uint8 *paramfieldp, 
                                   SWI_STRUCT_LbsAutoStart *psLbsAutoStart)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    psLbsAutoStart->nAllowed = *parmfp++;
    psLbsAutoStart->bEnabled = *parmfp++;
    psLbsAutoStart->nFixType = piget16(&parmfp);
    psLbsAutoStart->nPerformance = piget16(&parmfp);
    psLbsAutoStart->nAccuracy = piget32(&parmfp);
    psLbsAutoStart->nFixCount = piget16(&parmfp);
    psLbsAutoStart->nFixRate = piget32(&parmfp);
}

/* 
 * $Log: SwiIntUpkCdmaLbs.c,v $
 */

