/*
 * \ingroup cbk
 *
 * \file    qaCbkWdsGetPktSrvcStatusInd.c
 *
 * \brief   Contains UnPacking routines for the
 *          QMI_WDS_PKT_SERVC_STATUS_IND message.
 *
 * Copyright: © 2011-2013 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */

#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaCbkWdsGetPktSrvcStatusInd.h"

/* Functions */

/*
 * This function unpacks the Verbose Call end reason TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvVerboseCallEndReason(
    BYTE* pTlvData,
    BYTE *pResp )
{
    struct QmiCbkWdsSessionStateInd *lResp =
              (struct QmiCbkWdsSessionStateInd *)pResp;
    struct VerbSessEndReasonTlv *tResp = &lResp->vSERTlv;

    ULONG eRCode;

    /* Get Both the parameters from Verbose Call End reason TLV */
    eRCode = GetWord ( pTlvData, &tResp->verboseSessnEndReasonType );
    if (  eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    eRCode = GetWord ( pTlvData, &tResp->verboseSessnEndReason );
    tResp->TlvPresent = TRUE;

    return eRCode;
}

/*
 * This function unpacks the Call end reason TLV from the QMI indication message
 * to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvCallEndReason(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiCbkWdsSessionStateInd *lResp =
              (struct QmiCbkWdsSessionStateInd *)pResp;
    struct SessEndReasonTlv *tResp = &lResp->sERTlv;

    WORD lTemp = 0;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    eRCode = GetWord ( pTlvData, &lTemp );
    if ( eQCWWAN_ERR_NONE == eRCode )
        tResp->sessionEndReason = (ULONG) lTemp;

    tResp->TlvPresent = TRUE;
    return eRCode;
}

/*
 * This function unpacks the Packet Service Status TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvPktServiceStatus(
    BYTE* pTlvData,
    BYTE *pResp )
{
    struct QmiCbkWdsSessionStateInd *lResp =
            (struct QmiCbkWdsSessionStateInd *)pResp;
    struct PktSrvStatusTlv *tResp = &lResp->pktStatTlv;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    BYTE lByte = 0;

    eRCode = GetByte ( pTlvData, &lByte );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }
    tResp->state = lByte;

    /* Unpack reconfiguration_required */
    eRCode = GetByte ( pTlvData, &lByte );
    if ( eQCWWAN_ERR_NONE == eRCode )
        tResp->reconfiguration_required = lByte;

    tResp->TlvPresent = TRUE;
    return eRCode;
}

/*
 * This function unpacks the IP Family TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvIPFamily(
    BYTE* pTlvData,
    BYTE *pResp )
{
    struct QmiCbkWdsSessionStateInd *lResp =
            (struct QmiCbkWdsSessionStateInd *)pResp;
    struct IPFamilyTlv *tResp = &lResp->IPFTlv;

    tResp->TlvPresent = TRUE;
    return GetByte ( pTlvData, &tResp->ipFamily );
}

/*
 * This function unpacks the Technology Name TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvTechName(
    BYTE* pTlvData,
    BYTE *pResp )
{
    struct QmiCbkWdsSessionStateInd *lResp =
            (struct QmiCbkWdsSessionStateInd *)pResp;
    struct TechNameTlv *tResp = &lResp->tNamTlv;

    tResp->TlvPresent = TRUE;
    return GetWord ( pTlvData, &tResp->techName );
}

/*
 * This function unpacks the Packet Service Status Indication to a
 * user-provided response structure.
 *
 * \param   pMdmResp - Pointer to packed response from the modem.
 *
 * \param   pApiResp - Pointer to storage to unpack into.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UpkQmiCbkWdsGetPktSrvcStatusInd(
    BYTE    *pMdmResp,
    struct  QmiCbkWdsSessionStateInd *pApiResp )
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_PACKET_SERVICE_STATUS,    &UnpackCbkTlvPktServiceStatus },
        { eTLV_CALL_END_REASON,          &UnpackCbkTlvCallEndReason },
        { eTLV_VERBOSE_CALL_END_REASON,  &UnpackCbkTlvVerboseCallEndReason },
        { eTLV_IP_FAMILY,                &UnpackCbkTlvIPFamily },
        { eTLV_TECH_NAME,                &UnpackCbkTlvTechName },
        { eTLV_TYPE_INVALID,             NULL }  /* Important. Sentinel.
                                                  * Signifies last item in map.
                                                  */
    };

    eRCode = qmunpackresp( pMdmResp,
                          (BYTE*)pApiResp,
                          map,
                          eQMI_WDS_GET_PKT_STATUS );
    return eRCode;
}
