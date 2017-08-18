/*
 * \ingroup : loc
 *
 * \file    : qaSlqsLocInjectSensorData.c
 *
 * \brief   : Contains Packing and UnPacking routines for the
 *            eQMI_LOC_INJECT_SENSOR_DATA message.
 *
 * Copyright: © 2011-2016 Sierra Wireless, Inc. all rights reserved
 */

#include "SwiDataTypes.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaQmiBasic.h"

#include "qaSlqsLocInjectSensorData.h"

/******************************************************************************
 * Request handling
 ******************************************************************************/
/*
 * This function packs the Inject Opaque Identifier field to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                 data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
 */
enum eQCWWANError BuildTlvInjectOpaqueId( BYTE *pBuf, BYTE *pParam )
{
	LocInjectSensorDataReq *pReq =
        (LocInjectSensorDataReq *)pParam;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    if ( pReq->pOpaqueIdentifier == NULL )
        return eQCWWAN_ERR_NONE;

    eRCode = PutLong( pBuf, *(pReq->pOpaqueIdentifier));
    return eRCode;
}

/*
 * This function packs the Inject Accelerometer Data field to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                 data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
 */
enum eQCWWANError BuildTlvInjectAcceleroData( BYTE *pBuf, BYTE *pParam )
{
    sensorData *pReq =
        (( LocInjectSensorDataReq *)pParam)->pAcceleroData;

    WORD               lCount;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    if ( NULL == pReq )
        return eQCWWAN_ERR_NONE;

    /* Insert Time of first Sample Info */
    eRCode = PutLong( pBuf, pReq->timeOfFirstSample );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    /* Insert flag Info */
    eRCode = PutByte( pBuf, pReq->flags );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    /* Insert sensor data length */
    eRCode = PutByte( pBuf, pReq->sensorDataLen );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    for ( lCount = 0 ; lCount < pReq->sensorDataLen ; lCount++ )
    {
        eRCode = PutWord( pBuf, pReq->timeOffset[lCount] );
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
        eRCode = PutLong( pBuf, pReq->xAxis[lCount] );
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
        eRCode = PutLong( pBuf, pReq->yAxis[lCount] );
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
        eRCode = PutLong( pBuf, pReq->zAxis[lCount] );
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
    }

    return eRCode;
}

/*
 * This function packs the Inject Gyroscope Data field to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                 data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
 */
enum eQCWWANError BuildTlvInjectGyroData( BYTE *pBuf, BYTE *pParam )
{
    sensorData *pReq =
        (( LocInjectSensorDataReq *)pParam)->pGyroData;

    WORD               lCount;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    if ( NULL == pReq )
        return eQCWWAN_ERR_NONE;

    /* Insert Time of first Sample Info */
    eRCode = PutLong( pBuf, pReq->timeOfFirstSample );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    /* Insert flag Info */
    eRCode = PutByte( pBuf, pReq->flags );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    /* Insert sensor data length */
    eRCode = PutByte( pBuf, pReq->sensorDataLen );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    for ( lCount = 0 ; lCount < pReq->sensorDataLen ; lCount++ )
    {
        eRCode = PutWord( pBuf, pReq->timeOffset[lCount] );
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
        eRCode = PutLong( pBuf, pReq->xAxis[lCount] );
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
        eRCode = PutLong( pBuf, pReq->yAxis[lCount] );
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
        eRCode = PutLong( pBuf, pReq->zAxis[lCount] );
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
    }

    return eRCode;
}

/*
 * This function packs the 3-Axis Accelerometer Data Time Source field
 * to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                 data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
 */
enum eQCWWANError BuildTlvAcceleroDataTimeSrc( BYTE *pBuf, BYTE *pParam )
{
	LocInjectSensorDataReq *pReq =
        (LocInjectSensorDataReq *)pParam;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    if ( pReq->pAcceleroTimeSrc == NULL )
        return eQCWWAN_ERR_NONE;

    eRCode = PutLong( pBuf, *pReq->pAcceleroTimeSrc);
    return eRCode;
}

/*
 * This function packs the 3-Axis Gyroscope Data Time Source field
 * to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                 data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
 */
enum eQCWWANError BuildTlvGyroDataTimeSrc( BYTE *pBuf, BYTE *pParam )
{
	LocInjectSensorDataReq *pReq =
        (LocInjectSensorDataReq *)pParam;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    if ( pReq->pGyroTimeSrc == NULL )
        return eQCWWAN_ERR_NONE;

    eRCode = PutLong( pBuf, *pReq->pGyroTimeSrc);
    return eRCode;
}

/*
 * This function packs the Inject Accelerometer Temperature Data field to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                 data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
 */
enum eQCWWANError BuildTlvInjectAcceleroTempData( BYTE *pBuf, BYTE *pParam )
{
    tempratureData *pReq =
        (( LocInjectSensorDataReq *)pParam)->pAcceleroTempData;

    WORD               lCount;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    if ( NULL == pReq )
        return eQCWWAN_ERR_NONE;

    /* Insert Time source Info */
    eRCode = PutLong( pBuf, pReq->timeSource );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    /* Insert Time of first Sample Info */
    eRCode = PutLong( pBuf, pReq->timeOfFirstSample );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    /* Insert temperature data length */
    eRCode = PutByte( pBuf, pReq->temperatureDataLen );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    for ( lCount = 0 ; lCount < pReq->temperatureDataLen ; lCount++ )
    {
        eRCode = PutWord( pBuf, pReq->timeOffset[lCount] );
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
        eRCode = PutLong( pBuf, pReq->temperature[lCount] );
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
    }

    return eRCode;
}

/*
 * This function packs the Inject Gyroscope Temperature Data field to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                 data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
 */
enum eQCWWANError BuildTlvInjectGyroTempData( BYTE *pBuf, BYTE *pParam )
{
    tempratureData *pReq =
        (( LocInjectSensorDataReq *)pParam)->pGyroTempData;

    WORD               lCount;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    if ( NULL == pReq )
        return eQCWWAN_ERR_NONE;

    /* Insert Time source Info */
    eRCode = PutLong( pBuf, pReq->timeSource );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    /* Insert Time of first Sample Info */
    eRCode = PutLong( pBuf, pReq->timeOfFirstSample );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    /* Insert temperature data length */
    eRCode = PutByte( pBuf, pReq->temperatureDataLen );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    for ( lCount = 0 ; lCount < pReq->temperatureDataLen ; lCount++ )
    {
        eRCode = PutWord( pBuf, pReq->timeOffset[lCount] );
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
        eRCode = PutLong( pBuf, pReq->temperature[lCount] );
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
    }

    return eRCode;
}

/*
 * Packs the PkQmiLocInjectSensorData parameters to the QMI message SDU
 *
 * \param  pParamField[OUT]            - Pointer to storage into which the packed
 *                                     data will be placed by this function.
 *
 * \param  pMlength[OUT]               - Total length of built message.
 *
 * \param  pLocInjectSensorDataReq[IN] - Inject Sensor Data Req
 *
 * \return  eQCWWAN_ERR_NONE on success, eQCWWAN_ERR_XXX on error
 *
 */

enum eQCWWANError PkQmiLocInjectSensorData(
    WORD                    *pMlength,
    BYTE                    *pParamField,
    LocInjectSensorDataReq  *pLocInjectSensorDataReq )
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_INJECT_OPAQUE_ID,             &BuildTlvInjectOpaqueId },
        { eTLV_INJECT_ACCELERO_DATA,         &BuildTlvInjectAcceleroData },
        { eTLV_INJECT_GYRO_DATA,             &BuildTlvInjectGyroData },
        { eTLV_ACCELERO_DATA_TIME_SRC,       &BuildTlvAcceleroDataTimeSrc },
        { eTLV_GYRO_DATA_TIME_SRC,           &BuildTlvGyroDataTimeSrc },
        { eTLV_INJECT_ACCELERO_TEMP_DATA,    &BuildTlvInjectAcceleroTempData },
        { eTLV_INJECT_GYRO_TEMP_DATA,        &BuildTlvInjectGyroTempData },
        { eTLV_TYPE_INVALID, NULL } /* Important. Sentinel.
                                     * Signifies last item in map
                                     */
    };

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    eRCode = qmbuild( pParamField,
                      (BYTE *)pLocInjectSensorDataReq,
                      map,
                      eQMI_LOC_INJECT_SENSOR_DATA,
                      pMlength );
    return eRCode;
}

/******************************************************************************
 * Response handling
 ******************************************************************************

 * This function unpacks the eQMI_LOC_INJECT_SENSOR_DATA response message
 * to a user-provided response structure.
 *
 * \param  pMdmResp  [IN]  - Pointer to packed response from the modem.
 *
 * \param  pApiResp  [OUT] - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE  - on success;  eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError UpkQmiLocInjectSensorData(
                  BYTE                                *pMdmResp,
                  struct QmiLocInjectSensorDataResp   *pApiResp)
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,  &qmUnpackTlvResultCode },
        { eTLV_TYPE_INVALID, NULL } /* Important. Sentinel.
                                     * Signifies last item in map.
                                     */
    };

    eRCode = qmunpackresp( pMdmResp,
                           (BYTE *)pApiResp,
                           map,
                           eQMI_LOC_INJECT_SENSOR_DATA);
    return eRCode;
}
