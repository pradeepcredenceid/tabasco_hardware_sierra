/**
 *    $Id: SwiIntUpkCmLbs.c,v 1.2 2010/06/10 00:25:24 epasheva Exp $
 *
 *    Name       : SwiIntUpkCmLbs.c
 *    
 *    Purpose    : Unpack functions for Common LBS APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */

#include "SwiApiCmLbs.h"
#include "SwiIntUpkCmLbs.h"
#include "apidefs.h"

/**
 *
 * Unpacks the LBS satelite information from the CnS object's 
 * parameter field.
 *
 * \param   paramfieldp - Pointer to the CnS parameter field 
 *                        of the CnS object CNS_LBS_SAT_INFO.
 * \param   psBsInfo    - Pointer to storage into which the 
 *                        LBS satelite information will be placed by 
 *                        this function.
 *
 */
package void SwiUnpackLbsSatInfo(swi_uint8 *paramfieldp, 
                                 struct SWI_STRUCT_LbsSatInfo *psSatInfo)
{
    swi_uint8 i;
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    piget16(&parmfp);   /* Discard obj_ver */

    /*zero the memory before unpacking*/
    slmemset( (char*)psSatInfo, 0 , sizeof(SWI_STRUCT_LbsSatInfo) );

   /* set structure length */
    psSatInfo->sizeStruct = sizeof(struct SWI_STRUCT_LbsSatInfo);

    /*Unpack */
    psSatInfo->nSvInView = *parmfp++;

    if(psSatInfo->nSvInView > MAX_NUM_LBS_SV )
    {
        /*Satellite number is incorrect, no need to go furthur*/
        /*simply return from here*/
        return;
    }

    for(i = 0; i < psSatInfo->nSvInView; i++)
    {
        psSatInfo->sSatView[i].nSvNumber = *parmfp++;
        psSatInfo->sSatView[i].nSvElevation = *parmfp++;
        psSatInfo->sSatView[i].nSvAzimuth = *parmfp++;
        psSatInfo->sSatView[i].nSvSnr = *parmfp++;
    }
}

/**
 *
 * Unpacks the Lbs Data information from the CnS object's 
 * parameter field.
 *
 * \param   paramfieldp - Pointer to the CnS parameter field 
 *                        of the CnS object CNS_LBS_PD_DATA.
 * \param   psBsInfo    - Pointer to storage into which the 
 *                        Lbs Data information will be placed by 
 *                        this function.
 *
 */
package void SwiUnpackLbsPdData(swi_uint8 *paramfieldp, 
                                struct SWI_STRUCT_LbsPdData *psLbsPdData)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    swi_uint16 objver;

    parmfp = paramfieldp;

    /*zero the memory before unpacking*/
    slmemset( (char*)psLbsPdData, 0 , sizeof(SWI_STRUCT_LbsPdData) );

   /* set structure length */
    psLbsPdData->sizeStruct = sizeof(struct SWI_STRUCT_LbsPdData);

    /*Unpack */
    objver = piget16(&parmfp);
    
    if( objver >= 1)
    {
        psLbsPdData->nLatitude = piget32(&parmfp);
        psLbsPdData->nLongitude = piget32(&parmfp);
        psLbsPdData->nTimeStamp = piget32(&parmfp);

        psLbsPdData->nLocUncAngle = piget16(&parmfp);
        psLbsPdData->eLocUncA = piget16(&parmfp);
        psLbsPdData->eLocUncP = piget16(&parmfp);

        psLbsPdData->eFix = piget16(&parmfp);
        psLbsPdData->nHeightIncl = piget16(&parmfp);
        psLbsPdData->nHeight = piget16(&parmfp);
        psLbsPdData->eLocUncVe = piget16(&parmfp);
        psLbsPdData->nVelocityIncl = piget16(&parmfp);
        psLbsPdData->nHeading = piget16(&parmfp);
        psLbsPdData->nVelHoriz = piget16(&parmfp);
        psLbsPdData->nVelVert = piget16(&parmfp);
    }
    
    if( objver >= 2)
    {
        psLbsPdData->nUncertaintyA = piget16(&parmfp);
        psLbsPdData->nUncertaintyP = piget16(&parmfp);
        psLbsPdData->nUncertaintyV = piget16(&parmfp);
        psLbsPdData->nHEPE = piget16(&parmfp);
        psLbsPdData->nSatelliteNum = *parmfp++;
    }
    /*All the unpacking for Obj ver = 3 should be done here.
      Currently SWI_STRUCT_LbsPdData does not contain fields
      from Obj ver= 3, once SWI_STRUCT_LbsPdData is updated
      this function needs to be updated. TODO*/

}

/**
 *
 * Unpacks the Lbs Param information from the CnS object's 
 * parameter field.
 *
 * \param   paramfieldp - Pointer to the CnS parameter field 
 *                        of the CnS object CNS_LBS_PA_GET_PARAM.
 * \param   psBsInfo    - Pointer to storage into which the 
 *                        LBS param information will be placed by 
 *                        this function.
 *
 */
package void SwiUnpackLbsPaParam(swi_uint8 *paramfieldp, 
                           struct SWI_STRUCT_LbsPaGetParam *psLbsPaGetParam)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    /*zero the memory before unpacking*/
    slmemset( (char*)psLbsPaGetParam, 0 , sizeof(SWI_STRUCT_LbsPaGetParam) );

   /* set structure length */
    psLbsPaGetParam->sizeStruct = sizeof(struct SWI_STRUCT_LbsPaGetParam);

    /*Unpack */
    piget16(&parmfp);   /* Discard obj_ver */

    psLbsPaGetParam->nIpAddress = piget32(&parmfp);
    psLbsPaGetParam->nPortId = piget32(&parmfp);

    psLbsPaGetParam->eGpsLock = piget16(&parmfp);
    psLbsPaGetParam->eTransportMech = piget16(&parmfp);
    psLbsPaGetParam->ePrivacy = piget16(&parmfp);
    psLbsPaGetParam->eNetworkAccess = piget16(&parmfp);
}

/**
 *
 * Unpacks the LBS PD Status information from the CnS object's 
 * parameter field.
 *
 * \param   paramfieldp - Pointer to the CnS parameter field 
 *                        of the CnS object CNS_LBS_PD_GET_STATUS.
 * \param   psBsInfo    - Pointer to storage into which the 
 *                        LBS PD Status information will be 
 *                        placed by this function.
 *
 */
package void SwiUnpackLbsPdStatus(swi_uint8 *paramfieldp, 
                           struct SWI_STRUCT_LbsPdGetStatus *psLbsPdGetStatus)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    /*zero the memory before unpacking*/
    slmemset( (char*)psLbsPdGetStatus, 0 , sizeof(SWI_STRUCT_LbsPdGetStatus) );

   /* set structure length */
    psLbsPdGetStatus->sizeStruct = sizeof(struct SWI_STRUCT_LbsPdGetStatus);

    /*Unpack */
    piget16(&parmfp);   /* Discard obj_ver */

    pigetmulti(&parmfp,(swi_uint8 *)psLbsPdGetStatus->szLastFixTimeStamp,
               LENGTH_TimeStamp);
    psLbsPdGetStatus->eLastFixStatus = piget16(&parmfp);
    psLbsPdGetStatus->eLastFixError = piget16(&parmfp);

    pigetmulti(&parmfp,(swi_uint8 *)psLbsPdGetStatus->szLastDownloadTimeStamp,
               LENGTH_TimeStamp);
    psLbsPdGetStatus->eLastDownloadStatus = piget16(&parmfp);
    psLbsPdGetStatus->eLastDownloadError = piget16(&parmfp);

    pigetmulti(&parmfp,(swi_uint8 *)psLbsPdGetStatus->szLastFixSessionTimeStamp,
               LENGTH_TimeStamp);
    psLbsPdGetStatus->eLastFixSessionStatus = piget16(&parmfp);
    psLbsPdGetStatus->eLastFixSessionError = piget16(&parmfp);

    pigetmulti(&parmfp,(swi_uint8 *)psLbsPdGetStatus->szLastDownloadSessionTimeStamp,
               LENGTH_TimeStamp);
    psLbsPdGetStatus->eLastDownloadSessionStatus = piget16(&parmfp);
    psLbsPdGetStatus->eLastDownloadSessionError = piget16(&parmfp);
}


/**
 *
 * Unpacks the LBS PD Dload Data information from the CnS object's 
 * parameter field.
 *
 * \param   paramfieldp - Pointer to the CnS parameter field 
 *                        of the CnS object CNS_LBS_PD_DLOAD_DATA.
 * \param   psBsInfo    - Pointer to storage into which the 
 *                        LBS PD Status information will be 
 *                        placed by this function.
 *
 */

package void SwiUnpackLbsPdDloadData(swi_uint8 *paramfieldp, 
                           struct SWI_STRUCT_LbsPdDloadData *psLbsPdDloadData)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    /*zero the memory before unpacking*/
    slmemset( (char*)psLbsPdDloadData, 0 , sizeof(SWI_STRUCT_LbsPdDloadData) );

   /* set structure length */
    psLbsPdDloadData->sizeStruct = sizeof(struct SWI_STRUCT_LbsPdDloadData);

    /*Unpack */
    piget16(&parmfp);   /* Discard obj_ver */

    psLbsPdDloadData->nValidEntryPairNum = piget16(&parmfp);

    psLbsPdDloadData->eDataType1 = piget16(&parmfp);
    psLbsPdDloadData->eSource1 = piget16(&parmfp);

    
    psLbsPdDloadData->eDataType2 = piget16(&parmfp);
    psLbsPdDloadData->eSource2 = piget16(&parmfp);

    psLbsPdDloadData->eDataType3 = piget16(&parmfp);
    psLbsPdDloadData->eSource3 = piget16(&parmfp);

    psLbsPdDloadData->eDataType4 = piget16(&parmfp);
    psLbsPdDloadData->eSource4 = piget16(&parmfp);

    psLbsPdDloadData->eDataType5 = piget16(&parmfp);
    psLbsPdDloadData->eSource5 = piget16(&parmfp);

}


/* 
 * $Log: SwiIntUpkCmLbs.c,v $
 */

