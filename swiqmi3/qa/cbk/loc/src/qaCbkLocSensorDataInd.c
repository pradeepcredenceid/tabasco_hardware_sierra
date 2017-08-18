/*
 * \ingroup cbk
 *
 * \file qaCbkLocSensorDataInd.c
 *
 * \brief Contains UnPacking routines for the
 *        QMI_LOC_INJECT_SENSOR_DATA_IND message.
 *
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */

#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "sludefs.h"
#include "qaCbkLocSensorDataInd.h"

static    ULONG    opaqueIdentifier;
static    BYTE     accelSamplesAccepted;
static    BYTE     gyroSamplesAccepted;
static    BYTE     accelTempSamplesAccepted;
static    BYTE     gyroTempSamplesAccepted;
/*
 * This function unpacks the Status TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvInjectSensorStatus(
    BYTE *pTlvData,
    BYTE *pResp )
{
    ULONG *lResp =
        &(( QmiCbkLocInjectSensorDataInd *)pResp)->injectSensorDataStatus;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Inject Sensor Data TLV */
    eRCode = GetLong (pTlvData, lResp);

    return eRCode;
}
/*
 * This function unpacks the Status TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvOpaqueIdentifier(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocInjectSensorDataInd *lResp =
        ( QmiCbkLocInjectSensorDataInd *)pResp;

    /* Allocate memory to pointer */
    lResp->pOpaqueIdentifier = &opaqueIdentifier;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Inject Sensor Data TLV */
    eRCode = GetLong (pTlvData, lResp->pOpaqueIdentifier);

    return eRCode;
}
/*
 * This function unpacks the Status TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvAccelSamplesAccept(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocInjectSensorDataInd *lResp =
        ( QmiCbkLocInjectSensorDataInd *)pResp;

    /* Allocate memory to pointer */
    lResp->pAccelSamplesAccepted = &accelSamplesAccepted;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Inject Sensor Data TLV */
    eRCode = GetByte (pTlvData, lResp->pAccelSamplesAccepted);

    return eRCode;
}
/*
 * This function unpacks the Status TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvGyroSamplesAccept(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocInjectSensorDataInd *lResp =
        ( QmiCbkLocInjectSensorDataInd *)pResp;

    /* Allocate memory to pointer */
    lResp->pGyroSamplesAccepted = &gyroSamplesAccepted;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Inject Sensor Data TLV */
    eRCode = GetByte (pTlvData, lResp->pGyroSamplesAccepted);

    return eRCode;
}
/*
 * This function unpacks the Status TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvAccelTempSamples(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocInjectSensorDataInd *lResp =
        ( QmiCbkLocInjectSensorDataInd *)pResp;

    /* Allocate memory to pointer */
    lResp->pAccelTempSamplesAccepted = &accelTempSamplesAccepted;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Inject Sensor Data TLV */
    eRCode = GetByte (pTlvData, lResp->pAccelTempSamplesAccepted);

    return eRCode;
}
/*
 * This function unpacks the Status TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvGyroTempSamples(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocInjectSensorDataInd *lResp =
        ( QmiCbkLocInjectSensorDataInd *)pResp;

    /* Allocate memory to pointer */
    lResp->pGyroTempSamplesAccepted = &gyroTempSamplesAccepted;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Inject Sensor Data TLV */
    eRCode = GetByte (pTlvData, lResp->pGyroTempSamplesAccepted);

    return eRCode;
}
/*
 * This function unpacks the Inject Sensor Data Indication message to a
 * user-provided response structure.
 *
 * \param  pMdmResp - Pointer to packed response from the modem.
 *
 * \param  pApiResp - Pointer to storage to unpack into.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UpkQmiCbkLocSensorDataInd(
        BYTE                              *pMdmResp,
        QmiCbkLocInjectSensorDataInd      *pApiResp )
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_IND_INJECT_SENSOR_STATUS,      &UnpackCbkTlvInjectSensorStatus },
        { eTLV_IND_OPAQUE_IDENTIFIER,         &UnpackCbkTlvOpaqueIdentifier },
        { eTLV_IND_ACCEL_SAMPLES_ACCEPT,      &UnpackCbkTlvAccelSamplesAccept },
        { eTLV_IND_GYRO_SAMPLES_ACCEPT,       &UnpackCbkTlvGyroSamplesAccept },
        { eTLV_IND_ACCEL_TEMP_SAMPLES,        &UnpackCbkTlvAccelTempSamples },
        { eTLV_IND_GYRO_TEMP_SAMPLES,         &UnpackCbkTlvGyroTempSamples },
        { eTLV_TYPE_INVALID,                  NULL } /* Important. Sentinel.
                                                      * Signifies last item in map.
                                                      */
    };

    eRCode = qmunpackresp( pMdmResp,
                           (BYTE *)pApiResp,
                           map,
                           eQMI_LOC_INJECT_SENSOR_DATA_IND );
    return eRCode;
}
