/*
 * \ingroup cbk
 *
 * \file    qaCbkDmsSwiEventReportInd.c
 *
 * \brief   Contains UnPacking routines for the
 *          QMI_DMS_SWI_EVENT_REPORT_IND message.
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */

#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaCbkDmsSwiEventReportInd.h"

/* Functions */

/*
 * This function unpacks the Temperature TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvTemperature(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    struct TemperatureTlv *lResp =
        &((struct QmiCbkDmsSwiEventStatusReportInd *)pResp)->TempTlv;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    BYTE lByte = 0;
    WORD lTemp = 0;

    /* Extract the Temperature State */
    eRCode = GetByte( pTlvData, &lByte );
    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }
    lResp->TempState = lByte;

    /* Extract the Temperature */
    eRCode = GetWord( pTlvData, &lTemp );
    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }
    lResp->Temperature = lTemp;
    lResp->TlvPresent = TRUE;

    return eRCode;
}

/*
 * This function unpacks the Voltage TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvVoltage(
    BYTE *pTlvData,
    BYTE *pResp )
{
	/* Get the required TLV structure to the local pointer */
	struct VoltageTlv *lResp =
        &((struct QmiCbkDmsSwiEventStatusReportInd *)pResp)->VoltTlv;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE lByte = 0;
    WORD lTemp = 0;

    /* Extract the voltage state */
    eRCode = GetByte( pTlvData, &lByte );
    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }
    lResp->VoltState = lByte;

    /* Extract the voltage  */
    eRCode = GetWord( pTlvData, &lTemp );
    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }
    lResp->Voltage = lTemp;
    lResp->TlvPresent = TRUE;

    return eRCode;
}

/*
 * This function unpacks the Event Report Indication message to a
 * user-provided response structure.
 *
 * \param   pMdmResp - Pointer to packed response from the modem.
 *
 * \param   pApiResp - Pointer to storage to unpack into.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UpkQmiCbkDmsSwiEventReportInd(
    BYTE                                    *pMdmResp,
    struct QmiCbkDmsSwiEventStatusReportInd *pApiResp )
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_IND_TEMPERATURE, &UnpackCbkTlvTemperature },
        { eTLV_IND_VOLTAGE,     &UnpackCbkTlvVoltage },
        { eTLV_TYPE_INVALID,    NULL } /* Important. Sentinel.
                                        * Signifies last item in map.
                                        */
    };

    eRCode = qmunpackresp( pMdmResp,
                           (BYTE *)pApiResp,
                           map,
                           eQMI_DMS_SWI_EVENT_IND );
    return eRCode;
}
