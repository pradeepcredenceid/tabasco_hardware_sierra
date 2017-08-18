/*
 * \ingroup cbk
 *
 * \file qaCbkLocCradleMountInd.c
 *
 * \brief Contains UnPacking routines for the
 *        QMI_LOC_SET_CRADLE_MOUNT_CONFIG_IND message.
 *
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */

#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "sludefs.h"
#include "qaCbkLocCradleMountInd.h"

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
package enum eQCWWANError UnpackCbkTlvCradleMountStatus(
    BYTE *pTlvData,
    BYTE *pResp )
{
    ULONG *lResp =
        &(( QmiCbkLocCradleMountInd *)pResp)->cradleMountConfigStatus;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Cradle Mount Status TLV */
    eRCode = GetLong (pTlvData, lResp);

    return eRCode;
}

/*
 * This function unpacks the Event Report Indication message to a
 * user-provided response structure.
 *
 * \param  pMdmResp - Pointer to packed response from the modem.
 *
 * \param  pApiResp - Pointer to storage to unpack into.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UpkQmiCbkLocCradleMountInd(
    BYTE                                 *pMdmResp,
    QmiCbkLocCradleMountInd              *pApiResp )
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_IND_CRADLE_MOUNT_STATUS,   &UnpackCbkTlvCradleMountStatus },
        { eTLV_TYPE_INVALID,              NULL } /* Important. Sentinel.
                                                  * Signifies last item in map.
                                                  */
    };

    eRCode = qmunpackresp( pMdmResp,
                           (BYTE *)pApiResp,
                           map,
                           eQMI_LOC_SET_CRADLE_MOUNT_CONFIG_IND );
    return eRCode;
}
