/*
 * \ingroup cbk
 *
 * \file qaCbkLocEventGnssSvInfoInd.c
 *
 * \brief Contains UnPacking routines for the
 *        QMI_LOC_EVENT_GNSS_SV_INFO_IND message.
 *
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */

#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "sludefs.h"
#include "qaCbkLocEventGnssSvInfoInd.h"
#include "qaGobiApiCbk.h"
#include <string.h>

static satelliteInfo satelliteInfoList[255];

/*
 * This function unpacks the Accelerometer Accept Ready TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvAltitudeAssumedInd(
    BYTE *pTlvData,
    BYTE *pResp )
{
    gnssSvInfoNotification *lResp =
        ( gnssSvInfoNotification *)pResp;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Inject Enable Value TLV */
        eRCode = GetByte( pTlvData, &lResp->bAltitudeAssumed );

        return eRCode;
}

/*
 * This function unpacks the Accelerometer temperature Accept Ready TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvSatelliteInfoInd(
    BYTE *pTlvData,
    BYTE *pResp )
{
    gnssSvInfoNotification *lResp =
        ( gnssSvInfoNotification *)pResp;
    BYTE i;

    /* Allocate memory to the pointer */
    lResp->pSatelliteInfo = satelliteInfoList;
    /* Initialize the structure */
    memset(lResp->pSatelliteInfo, 0, sizeof(satelliteInfoList));

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    eRCode = GetByte( pTlvData, &lResp->pSatelliteInfo->svListLen );
    CHECK();

    for ( i = 0; i < lResp->pSatelliteInfo->svListLen; i++ )
    {
        eRCode = GetLong( pTlvData, &lResp->pSatelliteInfo[i].validMask);
        CHECK();

        eRCode = GetLong( pTlvData, &lResp->pSatelliteInfo[i].system );
        CHECK();

        eRCode = GetWord( pTlvData, &lResp->pSatelliteInfo[i].gnssSvId );
        CHECK();

        eRCode = GetByte( pTlvData, &lResp->pSatelliteInfo[i].healthStatus );
        CHECK();

        eRCode = GetLong( pTlvData, &lResp->pSatelliteInfo[i].svStatus );
        CHECK();

        eRCode = GetByte( pTlvData, &lResp->pSatelliteInfo[i].svInfoMask );
        CHECK();

        eRCode = GetLong( pTlvData, (ULONG*)&lResp->pSatelliteInfo[i].elevation );
        CHECK();

        eRCode = GetLong( pTlvData, (ULONG*)&lResp->pSatelliteInfo[i].azimuth );
        CHECK();

        eRCode = GetLong( pTlvData, (ULONG*)&lResp->pSatelliteInfo[i].snr );
    }

    return eRCode;
}

/*
 * This function unpacks the Sensor Streaming Ready Status Indication message to a
 * user-provided response structure.
 *
 * \param  pMdmResp - Pointer to packed response from the modem.
 *
 * \param  pApiResp - Pointer to storage to unpack into.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UpkQmiCbkLocEventGnssSvInfoInd(
        BYTE                              *pMdmResp,
        gnssSvInfoNotification       *pApiResp )
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_IND_ALTITUTE_ASSUMED,      &UnpackCbkTlvAltitudeAssumedInd },
        { eTLV_IND_SATELLITE_INFO,       &UnpackCbkTlvSatelliteInfoInd },
        { eTLV_TYPE_INVALID,                NULL } /* Important. Sentinel.
                                                    * Signifies last item in map.
                                                    */
    };

    eRCode = qmunpackresp( pMdmResp,
                           (BYTE *)pApiResp,
                           map,
                           eQMI_LOC_EVENT_GNSS_SV_INFO_IND );
    return eRCode;
}

