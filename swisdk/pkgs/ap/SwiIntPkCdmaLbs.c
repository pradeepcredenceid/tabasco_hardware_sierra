/**
 *    $Id: SwiIntPkCdmaLbs.c,v 1.1 2008/10/15 21:22:28 YGao Exp $
 *
 *    Name       : SwiIntPkCdmaLbs.c
 *    
 *    Purpose    : Pack functions for  CDMA specific LBS APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */

#include "SwiApiCdmaLbs.h"
#include "SwiIntPkCdmaLbs.h"
#include "apidefs.h"

/**
 *
 * This function packs the LBS location notification status to the CnS 
 * object's parameter field.
 *
 * \param paramfieldp[OUT]      Pointer to storage into which the packed 
 *                              data will be placed by this function.
 * \param psNotifyStatus[IN]    Device LBS location notification status.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackLbsLocNotifyStatus(
        swi_uint8 *paramfieldp, 
        const SWI_STRUCT_LbsLocNotifyStatus *psNotifyStatus)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(1, &localfieldp);
    piput16(psNotifyStatus->eNotifyFlag, &localfieldp);

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs the LBS download notification status to the CnS 
 * object's parameter field.
 *
 * \param paramfieldp[OUT]      Pointer to storage into which the packed 
 *                              data will be placed by this function.
 * \param psNotifyStatus[IN]    Device LBS download notification status.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackLbsDloadNotifyStatus(
        swi_uint8 *paramfieldp, 
        const SWI_STRUCT_LbsDloadNotifyStatus *psNotifyStatus)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(1, &localfieldp);
    piput16(psNotifyStatus->eNotifyFlag, &localfieldp);
    
    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs the LBS parameter notification status to the CnS 
 * object's parameter field.
 *
 * \param paramfieldp[OUT]      Pointer to storage into which the packed 
 *                              data will be placed by this function.
 * \param psNotifyStatus[IN]    Device LBS parameter notification status.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackLbsParamNotifyStatus(
        swi_uint8 *paramfieldp, 
        const SWI_STRUCT_LbsParamNotifyStatus *psNotifyStatus)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(1, &localfieldp);
    piput16(psNotifyStatus->eNotifyFlag, &localfieldp);

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs the LBS PD download data  to the CnS 
 * object's parameter field.
 *
 * \param paramfieldp[OUT]      Pointer to storage into which the packed 
 *                              data will be placed by this function.
 * \param psLbsPdDownload[IN]   PD download data to be packed.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackLbsPdDownload(
        swi_uint8 *paramfieldp, 
        const SWI_STRUCT_LbsPdDownload *psLbsPdDownload)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(1, &localfieldp);
    piput16(psLbsPdDownload->eOption, &localfieldp);
    piput16(psLbsPdDownload->nDuration, &localfieldp);

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs the GPS lock state data to the CnS 
 * object's parameter field.
 *
 * \param paramfieldp[OUT]      Pointer to storage into which the packed 
 *                              data will be placed by this function.
 * \param psPaGpsLock[IN]       GPS lock state data to be packed.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackLbsPaGpsLock(
        swi_uint8 *paramfieldp, 
        const SWI_STRUCT_LbsPaGpsLock *psPaGpsLock)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(1, &localfieldp);
    piput16(psPaGpsLock->eGpsLock, &localfieldp);

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs the LBS transportation mechanism data to the CnS 
 * object's parameter field.
 *
 * \param paramfieldp[OUT]      Pointer to storage into which the packed 
 *                              data will be placed by this function.
 * \param psPaPtlmMode[IN]      Point to LBS transportation mechanism data.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackLbsPaPtlmMode(
        swi_uint8 *paramfieldp, 
        const SWI_STRUCT_LbsPaPtlmMode *psPaPtlmMode)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(1, &localfieldp);
    piput16(psPaPtlmMode->eTransportMech, &localfieldp);

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs the LBS privacy level data to the CnS 
 * object's parameter field.
 *
 * \param paramfieldp[OUT]      Pointer to storage into which the packed 
 *                              data will be placed by this function.
 * \param psPaPtlmMode[IN]      Point to LBS privacy level data.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackLbsPaPrivacy(
        swi_uint8 *paramfieldp, 
        const SWI_STRUCT_LbsPaPrivacy *psPaPrivacy)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(1, &localfieldp);
    piput16(psPaPrivacy->ePrivacy, &localfieldp);

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs the LBS network access permission data to the CnS 
 * object's parameter field.
 *
 * \param paramfieldp[OUT]      Pointer to storage into which the packed 
 *                              data will be placed by this function.
 * \param psPaPtlmMode[IN]      Point to LBS network access permission data.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackLbsPaNetAccess(
        swi_uint8 *paramfieldp, 
        const SWI_STRUCT_LbsPaNetAccess *psPaNetAccess)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(1, &localfieldp);
    piput16(psPaNetAccess->eNetAccess, &localfieldp);

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs the LBS fix type control data to the CnS 
 * object's parameter field.
 *
 * \param paramfieldp[OUT]      Pointer to storage into which the packed 
 *                              data will be placed by this function.
 * \param psPaPtlmMode[IN]      Point to LBS fix type control data.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackLbsFixTypeControl(
        swi_uint8 *paramfieldp, 
        const SWI_STRUCT_LBS_FixTypeControl *pFixTypeControl)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(NV_SWI_GPS_MODE_SUPPORT_I, &localfieldp);
    
    *localfieldp++ = pFixTypeControl->bStandalone;
    *localfieldp++ = pFixTypeControl->bMSBased;
    *localfieldp++ = pFixTypeControl->bMSAssisted;
    *localfieldp++ = pFixTypeControl->bOptimalAccuracy;
    *localfieldp++ = pFixTypeControl->bOptimalSpeed;
    *localfieldp++ = pFixTypeControl->bOptimalData;

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs the LBS auto-start settings to the CnS 
 * object's parameter field.
 *
 * \param paramfieldp[OUT]      Pointer to storage into which the packed 
 *                              data will be placed by this function.
 * \param psPaPtlmMode[IN]      Point to LBS auto-start settings.
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint32 SwiPackLbsAutoStart(
        swi_uint8 *paramfieldp, 
        swi_uint16 nNvIndex, 
        const SWI_STRUCT_LbsAutoStart *psLbsAutoStart)
{
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */

    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    piput16(nNvIndex, &localfieldp);
    *localfieldp++ = psLbsAutoStart->nAllowed;
    *localfieldp++ = psLbsAutoStart->bEnabled;
    piput16(psLbsAutoStart->nFixType, &localfieldp);
    piput16(psLbsAutoStart->nPerformance, &localfieldp);
    piput32(psLbsAutoStart->nAccuracy, &localfieldp);
    piput16(psLbsAutoStart->nFixCount, &localfieldp);
    piput32(psLbsAutoStart->nFixRate, &localfieldp);

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/* 
 * $Log: SwiIntPkCdmaLbs.c,v $
 */

