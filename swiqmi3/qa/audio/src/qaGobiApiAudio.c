/*
 * \ingroup audio
 *
 * \file qaGobiApiAudio.c
 *
 * \brief  Entry points for Gobi APIs for the Audio Service ( Audio )
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "amudefs.h"
#include "sludefs.h"
#include "qaQmiBasic.h"
#include "qaGobiApiAudio.h"
#include "qaAudioSLQSGetAudioProfile.h"
#include "qaAudioSLQSSetAudioProfile.h"
#include "qaAudioSLQSGetAudioPathConfig.h"
#include "qaAudioSLQSSetAudioPathConfig.h"
#include "qaAudioSLQSGetAudioVolTLBConfig.h"
#include "qaAudioSLQSSetAudioVolTLBConfig.h"

ULONG SLQSGetAudioProfile (
    GetAudioProfileReq  *pGetAudioProfileReq,
    GetAudioProfileResp *pGetAudioProfileResp )
{
    ULONG  resultcode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiAudioSLQSGetProfileResp response;

    /* Checking for Invalid Parameter */
    if ( NULL == pGetAudioProfileReq ||
         NULL == pGetAudioProfileResp )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* pack QMI request */
    resultcode = PkQmiAudioSLQSGetAudioProfile( &paramLength,
                                                pOutParam,
                                                pGetAudioProfileReq );
    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_AUDIO,
                                  paramLength,
                                  eQMI_TIMEOUT_5_S, /* 5 seconds */
                                  &pInParam,
                                  &paramLength );

    /* Only parse out the response data if we got a positive return */
    if ( eQCWWAN_ERR_NONE == resultcode )
    {
        /* Pass the pointer for the OUT parameters */
        response.pGetAudioProfileResp = pGetAudioProfileResp;

        /* Initializing the response with default values */
        pGetAudioProfileResp->Profile = 0xFF;
        pGetAudioProfileResp->EarMute = 0xFF;
        pGetAudioProfileResp->MicMute = 0xFF;
        pGetAudioProfileResp->Volume  = 0xFF;

        /* Copy to the caller's buffer */
        resultcode = UpkQmiAudioSLQSGetAudioProfile(pInParam,&response);
    }

    qmrelreqbkp();
    return resultcode;
}

ULONG SLQSSetAudioProfile (
    SetAudioProfileReq  *pSetAudioProfileReq )
{
    ULONG  resultcode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiAudioSLQSSetProfileResp response;

    /* Checking for Invalid Parameter */
    if ( NULL == pSetAudioProfileReq )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* pack QMI request */
    resultcode = PkQmiAudioSLQSSetAudioProfile( &paramLength,
                                                pOutParam,
                                                pSetAudioProfileReq );
    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_AUDIO,
                                  paramLength,
                                  eQMI_TIMEOUT_5_S, /* 5 seconds */
                                  &pInParam,
                                  &paramLength );

    /* Only parse out the response data if we got a positive return */
    if ( eQCWWAN_ERR_NONE == resultcode )
    {
        /* Copy to the caller's buffer */
        resultcode = UpkQmiAudioSLQSSetAudioProfile(pInParam, &response);
    }

    qmrelreqbkp();
    return resultcode;
}

local void InitGetAudioPath( GetAudioPathConfigResp *pGetAudPathResp )
{
    BYTE ctr = 0;

    if( NULL != pGetAudPathResp->pECMode )
    {
        *(pGetAudPathResp->pECMode) = 0xFF;
    }

    if( NULL != pGetAudPathResp->pNSEnable )
    {
        *(pGetAudPathResp->pNSEnable) = 0xFF;
    }
    if( NULL != pGetAudPathResp->pTXGain )
    {
        *(pGetAudPathResp->pTXGain) = 0xFFFF;
    }
    if( NULL != pGetAudPathResp->pDTMFTXGain )
    {
        *(pGetAudPathResp->pDTMFTXGain) = 0xFFFF;
    }
    if( NULL != pGetAudPathResp->pCodecSTGain )
    {
        *(pGetAudPathResp->pCodecSTGain) = 0xFFFF;
    }
    if( NULL != pGetAudPathResp->pTXPCMIIRFltr )
    {
        TXPCMIIRFltr *ptemp = pGetAudPathResp->pTXPCMIIRFltr;
        *(ptemp->pFlag) = 0xFFFF;
        *(ptemp->pStageCnt) = 0xFFFF;
        for( ctr = 0; ctr < STAGESIZECOUNT; ctr++ )
        {
            ptemp->pStage0Val[ctr] = 0xFF;
            ptemp->pStage1Val[ctr] = 0xFF;
            ptemp->pStage2Val[ctr] = 0xFF;
            ptemp->pStage3Val[ctr] = 0xFF;
            ptemp->pStage4Val[ctr] = 0xFF;
        }
    }
    if( NULL != pGetAudPathResp->pRXPCMIIRFltr )
    {
        RXPCMIIRFltr *ptemp = pGetAudPathResp->pRXPCMIIRFltr;
        *(ptemp->pFlag) = 0xFFFF;
        *(ptemp->pStageCnt) = 0xFFFF;
        for( ctr = 0; ctr < STAGESIZECOUNT; ctr++ )
        {
            ptemp->pStage0Val[ctr] = 0xFF;
            ptemp->pStage1Val[ctr] = 0xFF;
            ptemp->pStage2Val[ctr] = 0xFF;
            ptemp->pStage3Val[ctr] = 0xFF;
            ptemp->pStage4Val[ctr] = 0xFF;
        }
    }
    if( NULL != pGetAudPathResp->pMICGainSelect )
    {
        *(pGetAudPathResp->pMICGainSelect) = 0xFF;
    }
    if( NULL != pGetAudPathResp->pRXAVCAGCSwitch )
    {
        *(pGetAudPathResp->pRXAVCAGCSwitch) = 0xFF;
    }
    if( NULL != pGetAudPathResp->pTXAVCSwitch )
    {
        *(pGetAudPathResp->pTXAVCSwitch) = 0xFF;
    }
    if( NULL != pGetAudPathResp->pRXAGCList )
    {
        RXAGCList *pTemp = pGetAudPathResp->pRXAGCList;
        *(pTemp->pRXStaticGain) = 0xFFFF;
        *(pTemp->pRXAIG) = 0xFFFF;
        *(pTemp->pRXExpThres) = 0xFFFF;
        *(pTemp->pRXExpSlope) = 0xFFFF;
        *(pTemp->pRXComprThres) = 0xFFFF;
        *(pTemp->pRXComprSlope) = 0xFFFF;
    }
    if( NULL != pGetAudPathResp->pRXAVCList )
    {
        RXAVCList *pTemp = pGetAudPathResp->pRXAVCList;
        *(pTemp->pAVRXAVCSens) = 0xFFFF;
        *(pTemp->pAVRXAVCHeadroom) = 0xFFFF;
    }
    if( NULL != pGetAudPathResp->pTXAGCList )
    {
        TXAGCList *pTemp = pGetAudPathResp->pTXAGCList;
        *(pTemp->pTXStaticGain) = 0xFFFF;
        *(pTemp->pTXAIG) = 0xFFFF;
        *(pTemp->pTXExpThres) = 0xFFFF;
        *(pTemp->pTXExpSlope) = 0xFFFF;
        *(pTemp->pTXComprThres) = 0xFFFF;
        *(pTemp->pTXComprSlope) = 0xFFFF;
    }
}

ULONG SLQSGetAudioPathConfig (
    GetAudioPathConfigReq  *pGetAudioPathConfigReq,
    GetAudioPathConfigResp *pGetAudioPathConfigResp )
{
    ULONG  resultcode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiAudioSLQSGetAudioPathConfigResp response;

    /* Checking for Invalid Parameter */
    if ( NULL == pGetAudioPathConfigReq ||
         NULL == pGetAudioPathConfigResp )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* pack QMI request */
    resultcode = PkQmiAudioSLQSGetAudioPathConfig( &paramLength,
                                                   pOutParam,
                                                   pGetAudioPathConfigReq );
    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_AUDIO,
                                  paramLength,
                                  eQMI_TIMEOUT_5_S, /* 5 seconds */
                                  &pInParam,
                                  &paramLength );

    /* Only parse out the response data if we got a positive return */
    if ( eQCWWAN_ERR_NONE == resultcode )
    {
        /* Pass the pointer for the OUT parameters */
        response.pGetAudioPathConfigResp = pGetAudioPathConfigResp;

        /* Initializing the response with default values */
        InitGetAudioPath( pGetAudioPathConfigResp );

        /* Copy to the caller's buffer */
        resultcode = UpkQmiAudioSLQSGetAudioPathConfig(pInParam,&response);
    }

    qmrelreqbkp();
    return resultcode;
}

ULONG SLQSSetAudioPathConfig (
    SetAudioPathConfigReq  *pSetAudioPathConfigReq )
{
    ULONG  resultcode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiAudioSLQSSetAudioPathConfigResp response;

    /* Checking for Invalid Parameter */
    if ( NULL == pSetAudioPathConfigReq )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* pack QMI request */
    resultcode = PkQmiAudioSLQSSetAudioPathConfig( &paramLength,
                                                   pOutParam,
                                                   pSetAudioPathConfigReq );
    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_AUDIO,
                                  paramLength,
                                  eQMI_TIMEOUT_5_S, /* 5 seconds */
                                  &pInParam,
                                  &paramLength );

    /* Only parse out the response data if we got a positive return */
    if ( eQCWWAN_ERR_NONE == resultcode )
    {
        /* Copy to the caller's buffer */
        resultcode = UpkQmiAudioSLQSSetAudioPathConfig(pInParam, &response);
    }

    qmrelreqbkp();
    return resultcode;
}

ULONG SLQSGetAudioVolTLBConfig (
    GetAudioVolTLBConfigReq  *pGetAudioVolTLBCfgReq,
    GetAudioVolTLBConfigResp *pGetAudioVolTLBCfgResp )
{
    ULONG  resultcode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiAudioSLQSGetVolTLBConfigResp response;

    /* Checking for Invalid Parameter */
    if ( NULL == pGetAudioVolTLBCfgReq ||
         NULL == pGetAudioVolTLBCfgResp )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* pack QMI request */
    resultcode = PkQmiAudioSLQSGetAudioVolTLBConfig( &paramLength,
                                                     pOutParam,
                                                     pGetAudioVolTLBCfgReq );
    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_AUDIO,
                                  paramLength,
                                  eQMI_TIMEOUT_5_S, /* 5 seconds */
                                  &pInParam,
                                  &paramLength );

    /* Only parse out the response data if we got a positive return */
    if ( eQCWWAN_ERR_NONE == resultcode )
    {
        /* Pass the pointer for the OUT parameters */
        response.pGetAudioVolTLBConfigResp = pGetAudioVolTLBCfgResp;

        /* Initializing the response with default values */
        pGetAudioVolTLBCfgResp->ResCode = 0xFFFF;

        /* Copy to the caller's buffer */
        resultcode = UpkQmiAudioSLQSGetAudioVolTLBConfig(pInParam,&response);
    }

    qmrelreqbkp();
    return resultcode;
}

ULONG SLQSSetAudioVolTLBConfig (
    SetAudioVolTLBConfigReq  *pSetAudioVolTLBCfgReq,
    SetAudioVolTLBConfigResp *pSetAudioVolTLBCfgResp )
{
    ULONG  resultcode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiAudioSLQSSetVolTLBConfigResp response;

    /* Checking for Invalid Parameter */
    if ( NULL == pSetAudioVolTLBCfgReq ||
         NULL == pSetAudioVolTLBCfgResp )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* pack QMI request */
    resultcode = PkQmiAudioSLQSSetAudioVolTLBConfig( &paramLength,
                                                     pOutParam,
                                                     pSetAudioVolTLBCfgReq );
    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_AUDIO,
                                  paramLength,
                                  eQMI_TIMEOUT_5_S, /* 5 seconds */
                                  &pInParam,
                                  &paramLength );

    /* Only parse out the response data if we got a positive return */
    if ( eQCWWAN_ERR_NONE == resultcode )
    {
        /* Pass the pointer for the OUT parameters */
        response.pSetAudioVolTLBConfigResp = pSetAudioVolTLBCfgResp;

        /* Initializing the response with default values */
        pSetAudioVolTLBCfgResp->ResCode = 0xFFFF;

        /* Copy to the caller's buffer */
        resultcode = UpkQmiAudioSLQSSetAudioVolTLBConfig(pInParam,&response);
    }

    qmrelreqbkp();
    return resultcode;
}
