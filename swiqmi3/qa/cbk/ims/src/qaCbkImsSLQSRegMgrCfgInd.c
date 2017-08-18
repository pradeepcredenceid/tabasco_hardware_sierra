/*
 * \ingroup cbk
 *
 * \file    qaCbkImsSLQSRegMgrCfgInd.c
 *
 * \brief   Contains UnPacking routines for the QMI_IMS_REG_MGR_CONFIG_IND message.
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */

#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaCbkImsSLQSRegMgrCfgInd.h"

/* Locals */

/* Functions */

/*
 * This function unpacks the Primary CSCF port info TLV parameters from the QMI
 * indication message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvPCSCFPort(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    struct PCSCFPortTlv *lResp =
        &((struct QmiCbkImsRegMgrConfigInd *)pResp)->PCTlv;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    WORD              lTemp  = 0;

    /* Extract the PCSCF Port */
    eRCode = GetWord( pTlvData, &lTemp );
    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }

    lResp->priCSCFPort = lTemp;
    lResp->TlvPresent   = TRUE;

    return eRCode;
}

/*
 * This function unpacks the CSCF port TLV parameters from the QMI
 * indication message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvCSCFPort(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    struct CSCFPortNameTlv *lResp =
        &((struct QmiCbkImsRegMgrConfigInd *)pResp)->PNTlv;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Extract the CSCF Port Name */
    eRCode = qmQmiExtractString( pTlvData,
                                 (CHAR *)lResp->cscfPortName,
                                 (ULONG)MAX_CSCF_PORT_NAME_LEN );

    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }

    lResp->TlvPresent = TRUE;
    return eRCode;
}

/*
 * This function unpacks the IMS Test Mode TLV parameters from the QMI
 * indication message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvIMSTestMode(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    struct IMSTestModeTlv *lResp =
        &((struct QmiCbkImsRegMgrConfigInd *)pResp)->TMTlv;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              lTemp  = 0;

    /* Extract the IMS Test mode */
    eRCode = GetByte( pTlvData, &lTemp );
    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }

    lResp->imsTestMode = lTemp;
    lResp->TlvPresent   = TRUE;

    return eRCode;
}
/*
 * This function unpacks the REG Mgr Config Info Indication message to a
 * user-provided response structure.
 *
 * \param   pMdmResp - Pointer to packed response from the modem.
 *
 * \param   pApiResp - Pointer to storage to unpack into.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UpkQmiCbkImsRegMgrCfgInd(
    BYTE                            *pMdmResp,
    struct QmiCbkImsRegMgrConfigInd *pApiResp )
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_PRI_PCSCF_PORT, &UnpackCbkTlvPCSCFPort },
        { eTLV_CSCF_PORT_NAME, &UnpackCbkTlvCSCFPort },
        { eTLV_IMS_TST_MODE,   &UnpackCbkTlvIMSTestMode },
        { eTLV_TYPE_INVALID,   NULL } /* Important. Sentinel.
                                       * Signifies last item in map.
                                       */
    };

    eRCode = qmunpackresp( pMdmResp,
                           (BYTE *)pApiResp,
                           map,
                           eQMI_IMS_REG_MGR_CONFIG_IND );
    return eRCode;
}
