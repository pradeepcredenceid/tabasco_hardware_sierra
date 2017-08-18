/*
 * \ingroup voice
 *
 * \file    qaVoiceSLQSOriginateUSSD.c
 *
 * \brief   Contains Packing and UnPacking routines for the
 *          QMI_VOICE_ORIGINATE_USSD message.
 *
 * Copyright: � 2011 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */

#include "SwiDataTypes.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaVoiceOriginateUSSD.h"
#include "qaGobiApiVoiceCommon.h"
#include "qaVoiceSLQSOriginateUSSD.h"

/******************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the USS Information fields to the QMI message SDU
 *
 * \param  pBuf   [OUT] - Pointer to storage into which the packed
 *                        data will be placed by this function.
 *
 * \param  pParam [IN]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError BuildTlvSLQSUSSInfo( BYTE *pBuf, BYTE *pParam )
{
    struct QmiVoiceOriginateUSSDReq *pReq =
        (struct QmiVoiceOriginateUSSDReq *)pParam;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE  index = 0, USSLen;

    /* Add TLV data */
    eRCode = PutByte( pBuf, pReq->pUSSInfo->ussDCS );
    if ( eQCWWAN_ERR_NONE != eRCode )
        return eRCode;

    eRCode = PutByte( pBuf, pReq->pUSSInfo->ussLen );
    if ( eQCWWAN_ERR_NONE != eRCode )
        return eRCode;

    USSLen = pReq->pUSSInfo->ussLen;
    for( index  = 0; index < USSLen; index++)
    {
        eRCode = PutByte( pBuf, pReq->pUSSInfo->ussData[index] );
        if ( eQCWWAN_ERR_NONE != eRCode )
            return eRCode;
    }

    return eRCode;
}

/*
 * This function packs the SLQS Originate USSD request parameters to the QMI
 * message SDU
 *
 * \param  pMlength     [OUT] - Total length of built message.
 *
 * \param  pParamField  [OUT] - Pointer to storage into which the packed
 *                              data will be placed by this function.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 * \sa qaGobiApiUSSD.h for remaining parameter descriptions.
 */
enum eQCWWANError PkQmiVoiceSLQSOriginateUSSD(
    WORD           *pMlength,
    BYTE           *pParamField,
    struct USSInfo *pUSSInfo )
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_USS_Information, &BuildTlvSLQSUSSInfo },
        { eTLV_TYPE_INVALID,    NULL } /* Important. Sentinel.
                                        * Signifies last item in map
                                        */
    };

    struct QmiVoiceOriginateUSSDReq req;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    slmemset((CHAR *)&req, 0, sizeof(struct QmiVoiceOriginateUSSDReq));
    req.pUSSInfo = pUSSInfo;

    eRCode = qmbuild( pParamField,
                      (BYTE *)&req,
                      map,
                      eQMI_VOICE_ORIGINATE_USSD,
                      pMlength );
    return eRCode;
}

/******************************************************************************
 * Response handling
 ******************************************************************************/
/*************
 *
 * Unpack TLV Failure cause into given buffer
 *
 * \param     pTlvData    Pointer to TLV data to be unpacked.
 * \param     pResp       Pointer to storage provided by the application where
 *                        the Failure cause is returned.
 *
 * \return    eQCWWAN_ERR_NONE
 *
 **************/
global enum eQCWWANError UnpackTlvFailCause(
    swi_uint8 *pTlvData,
    swi_uint8 *pResp)
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    struct QmiVoiceSLQSOriginateUSSDResp *lResp = (struct QmiVoiceSLQSOriginateUSSDResp *)pResp;
    struct USSResp *tResp = lResp->pUSSResp;

    if( NULL == tResp->pfailureCause )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get Failure cause and error values */
    eRCode = GetWord((swi_uint8*)pTlvData, tResp->pfailureCause );
    return eRCode;
}

/*************
 *
 * Unpack TLV Alpha Identifier into given buffer
 *
 * \param     pTlvData    Pointer to TLV data to be unpacked.
 * \param     pResp       Pointer to storage provided by the application where
 *                        the Alpha Identifier is returned.
 *
 * \return    eQCWWAN_ERR_NONE
 *
 **************/
global enum eQCWWANError UnpackTlvAlphaIdntfr(
    swi_uint8 *pTlvData,
    swi_uint8 *pResp)
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    struct QmiVoiceSLQSOriginateUSSDResp *lResp = (struct QmiVoiceSLQSOriginateUSSDResp *)pResp;
    struct USSResp *tResp = lResp->pUSSResp;
    alphaIDInfo * pAlphaIdInfo = tResp->pAlphaIDInfo;

    return UnpackTlvCommonAlphaIdentifier( pTlvData, pAlphaIdInfo );
}

/*************
 *
 * Unpack TLV USSD Data into given buffer
 *
 * \param     pTlvData    Pointer to TLV to be unpacked.
 * \param     pResp       Pointer to storage provided by the application where
 *                        the USSD Data is returned.
 *
 * \return    eQCWWAN_ERR_NONE
 *
 **************/
global enum eQCWWANError UnpackTlvUSSDData(
    swi_uint8 *pTlvData,
    swi_uint8 *pResp)
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    struct QmiVoiceSLQSOriginateUSSDResp *lResp = (struct QmiVoiceSLQSOriginateUSSDResp *)pResp;
    struct USSInfo *tResp = lResp->pUSSResp->pUSSDInfo;

    BYTE Index;

    /*Check for In/Out parameters for NULL*/
    if( NULL == tResp )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Parameters to be unpacked
     * BYTE ussDCS;
     * BYTE ussLen;
     * BYTE ussData[MAXUSSDLENGTH];
     */

    /* Get ussDCS */
    eRCode = GetByte((swi_uint8*)pTlvData, &tResp->ussDCS);
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return (eRCode);
    }

    /* Get ussLen */
    eRCode = GetByte((swi_uint8*)pTlvData, &tResp->ussLen);
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return (eRCode);
    }

    /* Get ussData */
    for( Index=0; Index < tResp->ussLen; Index++ )
    {
        eRCode = GetByte( (swi_uint8*)pTlvData, &tResp->ussData[Index] );
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
            return (eRCode);
        }
    }

    return eQCWWAN_ERR_NONE;

}

/*************
 *
 * Unpack CC Result into given buffer
 *
 * \param     pTlvData    Pointer to TLV to be unpacked.
 * \param     pResp       Pointer to storage provided by the application where
 *                        the CC Result is returned.
 *
 * \return    eQCWWAN_ERR_NONE
 *
 **************/
global enum eQCWWANError UnpackTlvCCResult(
    swi_uint8 *pTlvData,
    swi_uint8 *pResp)
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    struct QmiVoiceSLQSOriginateUSSDResp *lResp = (struct QmiVoiceSLQSOriginateUSSDResp *)pResp;
    struct USSResp *tResp = lResp->pUSSResp;

    /*Check for In/Out parameters for NULL*/
    if( NULL == tResp )
    {
        return eQCWWAN_ERR_NONE;
    }
    /* Get CCResult */
    eRCode = GetByte( (swi_uint8*)pTlvData, tResp->pCcResultType );
    return eRCode;

}

/*************
 *
 * Unpack TLV Caller ID into given buffer
 *
 * \param     pTlvData    Pointer to TLV result code to be unpacked.
 * \param     resp        Pointer to storage provided by the application where
 *                        the Caller ID is returned.
 *
 * \return    eQCWWAN_ERR_NONE
 *
 **************/
global enum eQCWWANError UnpackTlvCallerID(
    swi_uint8 *pTlvData,
    swi_uint8 *pResp)
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    struct QmiVoiceSLQSOriginateUSSDResp *lResp = (struct QmiVoiceSLQSOriginateUSSDResp *)pResp;
    struct USSResp *tResp = lResp->pUSSResp;

    /*Check for In/Out parameters for NULL*/
    if( NULL == tResp )
    {
        return eQCWWAN_ERR_NONE;
    }
    /* Get Caller ID */
    eRCode = GetByte( (swi_uint8*)pTlvData, tResp->pCallId );
    return (eRCode);
}

/*************
 *
 * Unpack CC Supplementary Service type into given buffer
 *
 * \param     pTlvData    Pointer to TLV result code to be unpacked.
 * \param     pResp       Pointer to storage provided by the application where
 *                        the CC Supplementary Service type is returned.
 *
 * \return    eQCWWAN_ERR_NONE
 *
 **************/
global enum eQCWWANError UnpackTlvCCSuppServ(
    swi_uint8 *pTlvData,
    swi_uint8 *pResp)
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    struct QmiVoiceSLQSOriginateUSSDResp *lResp = (struct QmiVoiceSLQSOriginateUSSDResp *)pResp;
    struct USSResp *tResp = lResp->pUSSResp;
    ccSUPSType *pCcSupsType = tResp->pCCSuppsType;

    return UnpackTlvCommonccSupsType( pTlvData, pCcSupsType );
}

/*
 * This function unpacks the Originate USSD response message to a
 * user-provided response structure.
 *
 * \param  pMdmResp           - Pointer to packed response from the modem.
 *
 * \param  pApiResp           - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError UpkQmiVoiceSLQSOriginateUSSD(
    BYTE                                 *pMdmResp,
    struct QmiVoiceSLQSOriginateUSSDResp *pApiResp )
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,     &qmUnpackTlvResultCode },
        { eTLV_FAIL_CAUSE,      &UnpackTlvFailCause    },
        { eTLV_ALPHA_IDNTFR,    &UnpackTlvAlphaIdntfr  },
        { eTLV_USSD_DATA,       &UnpackTlvUSSDData     },
        { eTLV_CC_RESULT,       &UnpackTlvCCResult     },
        { eTLV_CALLER_ID,       &UnpackTlvCallerID     },
        { eTLV_CC_SUPP_SERVICE, &UnpackTlvCCSuppServ   },
        { eTLV_TYPE_INVALID,     NULL } /* Important. Sentinel.
                                         * Signifies last item in map.
                                         */
    };

    eRCode = qmunpackresp( pMdmResp,
                           (BYTE *)pApiResp,
                           map,
                           eQMI_VOICE_ORIGINATE_USSD );
    return eRCode;
}
