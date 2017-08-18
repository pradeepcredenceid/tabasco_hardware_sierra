/*
 * \ingroup cbk
 *
 * \file qaCbkLocSensorStreamInd.c
 *
 * \brief Contains UnPacking routines for the
 *        QMI_LOC_EVENT_SENSOR_STREAMING_READY_STATUS_IND message.
 *
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */

#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "sludefs.h"
#include "qaCbkLocSensorStreamInd.h"


static  accelAcceptReady       accelAccept;
static  gyroAcceptReady        gyroAccept;
static  accelTempAcceptReady   accelTempAccept;
static  gyroTempAcceptReady    gyroTempAccept;
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
package enum eQCWWANError UnpackCbkTlvAccelAcceptReady(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocSensorStreamingInd *lResp =
        ( QmiCbkLocSensorStreamingInd *)pResp;

    /* Allocate memory to the pointer */
    lResp->pAccelAcceptReady = &accelAccept;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Inject Enable Value TLV */
        eRCode = GetByte( pTlvData, &lResp->pAccelAcceptReady->injectEnable );
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
    /* Get the parameter from Samples Per Batch TLV */
        eRCode = GetWord( pTlvData, &lResp->pAccelAcceptReady->samplesPerBatch );
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
             return eRCode;
        }
    /* Get the parameter from Batch Per second TLV */
        eRCode = GetWord( pTlvData, &lResp->pAccelAcceptReady->batchPerSec );

        return eRCode;
}

/*
 * This function unpacks the Gyroscope Accept Ready TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvGyroAcceptReady(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocSensorStreamingInd *lResp =
        ( QmiCbkLocSensorStreamingInd *)pResp;

    /* Allocate memory to the pointer */
    lResp->pGyroAcceptReady = &gyroAccept;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Inject Enable Value TLV */
        eRCode = GetByte( pTlvData, &lResp->pGyroAcceptReady->injectEnable );
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
    /* Get the parameter from Samples Per Batch TLV */
        eRCode = GetWord( pTlvData, &lResp->pGyroAcceptReady->samplesPerBatch );
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
             return eRCode;
        }
    /* Get the parameter from Batch Per second TLV */
        eRCode = GetWord( pTlvData, &lResp->pGyroAcceptReady->batchPerSec );

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
package enum eQCWWANError UnpackCbkTlvAccelTempAccept(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocSensorStreamingInd *lResp =
        ( QmiCbkLocSensorStreamingInd *)pResp;

    /* Allocate memory to the pointer */
    lResp->pAccelTempAcceptReady = &accelTempAccept;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Inject Enable Value TLV */
        eRCode = GetByte( pTlvData, &lResp->pAccelTempAcceptReady->injectEnable );
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
    /* Get the parameter from Samples Per Batch TLV */
        eRCode = GetWord( pTlvData, &lResp->pAccelTempAcceptReady->samplesPerBatch );
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
             return eRCode;
        }
    /* Get the parameter from Batch Per second TLV */
        eRCode = GetWord( pTlvData, &lResp->pAccelTempAcceptReady->batchPerSec );

        return eRCode;
}
/*
 * This function unpacks the Gyroscope temperature Accept Ready TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvGyroTempAccept(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocSensorStreamingInd *lResp =
        ( QmiCbkLocSensorStreamingInd *)pResp;

    /* Allocate memory to the pointer */
    lResp->pGyroTempAcceptReady = &gyroTempAccept;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Inject Enable Value TLV */
        eRCode = GetByte( pTlvData, &lResp->pGyroTempAcceptReady->injectEnable );
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
    /* Get the parameter from Samples Per Batch TLV */
        eRCode = GetWord( pTlvData, &lResp->pGyroTempAcceptReady->samplesPerBatch );
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
             return eRCode;
        }
    /* Get the parameter from Batch Per second TLV */
        eRCode = GetWord( pTlvData, &lResp->pGyroTempAcceptReady->batchPerSec );

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
package enum eQCWWANError UpkQmiCbkLocSensorStreamInd(
        BYTE                              *pMdmResp,
        QmiCbkLocSensorStreamingInd       *pApiResp )
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_IND_ACCEL_ACCEPT_READY,      &UnpackCbkTlvAccelAcceptReady },
        { eTLV_IND_GYRO_ACCEPT_READY,       &UnpackCbkTlvGyroAcceptReady },
        { eTLV_IND_ACCEL_TEMP_ACCEPT,       &UnpackCbkTlvAccelTempAccept },
        { eTLV_IND_GYRO_TEMP_ACCEPT,        &UnpackCbkTlvGyroTempAccept },
        { eTLV_TYPE_INVALID,                NULL } /* Important. Sentinel.
                                                    * Signifies last item in map.
                                                    */
    };

    eRCode = qmunpackresp( pMdmResp,
                           (BYTE *)pApiResp,
                           map,
                           eQMI_LOC_SENSOR_STREAMING_STATUS_IND );
    return eRCode;
}
