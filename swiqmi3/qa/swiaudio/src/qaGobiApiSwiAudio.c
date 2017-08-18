/*
 * \ingroup swiaudio
 *
 * \file qaGobiApiSwiAudio.c
 *
 * \brief  Entry points for Gobi APIs for the SWI Audio Service ( SwiAudio )
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
#include "qaGobiApiSwiAudio.h"
#include "qaAudioSLQSGetM2MAudioProfile.h"
#include "qaAudioSLQSSetM2MAudioProfile.h"
#include "qaAudioSLQSGetM2MAudioVolume.h"
#include "qaAudioSLQSSetM2MAudioVolume.h"
#include "qaAudioSLQSSetM2MAudioAVCFG.h"
#include "qaAudioSLQSSetM2MAudioLPBK.h"
#include "qaAudioSLQSSetM2MAudioNVDef.h"
#include "qaAudioSLQSGetM2MSpkrGain.h"
#include "qaAudioSLQSSetM2MSpkrGain.h"
#include "qaAudioSLQSGetM2MAVMute.h"
#include "qaAudioSLQSSetM2MAVMute.h"

ULONG SLQSGetM2MAudioProfile (
    GetM2MAudioProfileReq  *pGetM2MAudioProfileReq,
    GetM2MAudioProfileResp *pGetM2MAudioProfileResp )
{
    ULONG  resultcode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiAudioSLQSGetM2MAudioProfileResp response;

    /* Checking for Invalid Parameter */
    if ( NULL == pGetM2MAudioProfileResp )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* pack QMI request */
    resultcode = PkQmiAudioSLQSGetM2MAudioProfile( &paramLength,
                                                   pOutParam,
                                                   pGetM2MAudioProfileReq );
    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_SWIAUDIO,
                                  paramLength,
                                  eQMI_TIMEOUT_5_S, /* 5 seconds */
                                  &pInParam,
                                  &paramLength );

    /* Only parse out the response data if we got a positive return */
    if ( eQCWWAN_ERR_NONE == resultcode )
    {
        /* Pass the pointer for the OUT parameters */
        response.pGetM2MAudioProfileResp = pGetM2MAudioProfileResp;

        /* Initializing the response with default values */
        pGetM2MAudioProfileResp->Profile   = 0xFF;
        pGetM2MAudioProfileResp->EarMute   = 0xFF;
        pGetM2MAudioProfileResp->MicMute   = 0xFF;
        pGetM2MAudioProfileResp->CwtMute   = 0xFF;
        pGetM2MAudioProfileResp->Generator = 0xFF;
        pGetM2MAudioProfileResp->Volume    = 0xFF;

        /* Copy to the caller's buffer */
        resultcode = UpkQmiAudioSLQSGetM2MAudioProfile( pInParam, &response );
    }

    qmrelreqbkp();
    return resultcode;
}

ULONG SLQSSetM2MAudioProfile (
    SetM2MAudioProfileReq *pSetM2MAudioProfileReq )
{
    ULONG  resultcode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiAudioSLQSSetM2MAudioProfileResp response;

    /* Checking for Invalid Parameter */
    if ( NULL == pSetM2MAudioProfileReq )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* pack QMI request */
    resultcode = PkQmiAudioSLQSSetM2MAudioProfile( &paramLength,
                                                   pOutParam,
                                                   pSetM2MAudioProfileReq );
    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_SWIAUDIO,
                                  paramLength,
                                  eQMI_TIMEOUT_5_S, /* 5 seconds */
                                  &pInParam,
                                  &paramLength );

    /* Only parse out the response data if we got a positive return */
    if ( eQCWWAN_ERR_NONE == resultcode )
    {
        /* Copy to the caller's buffer */
        resultcode = UpkQmiAudioSLQSSetM2MAudioProfile( pInParam, &response );
    }

    qmrelreqbkp();
    return resultcode;
}

ULONG SLQSGetM2MAudioVolume (
    GetM2MAudioVolumeReq  *pGetM2MAudioVolumeReq,
    GetM2MAudioVolumeResp *pGetM2MAudioVolumeResp )
{
    ULONG  resultcode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiAudioSLQSGetM2MAudioVolumeResp response;

    /* Checking for Invalid Parameter */
    if ( NULL == pGetM2MAudioVolumeReq ||
         NULL == pGetM2MAudioVolumeResp )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* pack QMI request */
    resultcode = PkQmiAudioSLQSGetM2MAudioVolume( &paramLength,
                                                  pOutParam,
                                                  pGetM2MAudioVolumeReq );
    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_SWIAUDIO,
                                  paramLength,
                                  eQMI_TIMEOUT_5_S, /* 5 seconds */
                                  &pInParam,
                                  &paramLength );

    /* Only parse out the response data if we got a positive return */
    if ( eQCWWAN_ERR_NONE == resultcode )
        {
            /* Pass the pointer for the OUT parameters */
            response.pGetM2MAudioVolumeResp = pGetM2MAudioVolumeResp;

            /* Initializing the response with default values */
            pGetM2MAudioVolumeResp->Level = 0xFF;

            /* Copy to the caller's buffer */
            resultcode = UpkQmiAudioSLQSGetM2MAudioVolume( pInParam, &response );
        }

    qmrelreqbkp();
    return resultcode;
}

ULONG SLQSSetM2MAudioVolume (
    SetM2MAudioVolumeReq *pSetM2MAudioVolumeReq )
{
    ULONG  resultcode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiAudioSLQSSetM2MAudioVolumeResp response;

    /* Checking for Invalid Parameter */
    if ( NULL == pSetM2MAudioVolumeReq )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* pack QMI request */
    resultcode = PkQmiAudioSLQSSetM2MAudioVolume( &paramLength,
                                                   pOutParam,
                                                   pSetM2MAudioVolumeReq );
    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_SWIAUDIO,
                                  paramLength,
                                  eQMI_TIMEOUT_5_S, /* 5 seconds */
                                  &pInParam,
                                  &paramLength );

    /* Only parse out the response data if we got a positive return */
    if ( eQCWWAN_ERR_NONE == resultcode )
    {
        /* Copy to the caller's buffer */
        resultcode = UpkQmiAudioSLQSSetM2MAudioVolume( pInParam, &response );
    }

    qmrelreqbkp();
    return resultcode;
}

ULONG SLQSSetM2MAudioAVCFG (
    SetM2MAudioAVCFGReq *pSetM2MAudioAVCFGReq )
{
    ULONG  resultcode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiAudioSLQSSetM2MAudioAVCFGResp response;

    /* Checking for Invalid Parameter */
    if ( NULL == pSetM2MAudioAVCFGReq )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* pack QMI request */
    resultcode = PkQmiAudioSLQSSetM2MAudioAVCFG( &paramLength,
                                                 pOutParam,
                                                 pSetM2MAudioAVCFGReq );
    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_SWIAUDIO,
                                  paramLength,
                                  eQMI_TIMEOUT_5_S, /* 5 seconds */
                                  &pInParam,
                                  &paramLength );

    /* Only parse out the response data if we got a positive return */
    if ( eQCWWAN_ERR_NONE == resultcode )
    {
        /* Copy to the caller's buffer */
        resultcode = UpkQmiAudioSLQSSetM2MAudioAVCFG( pInParam, &response );
    }

    qmrelreqbkp();
    return resultcode;
}

ULONG SLQSSetM2MAudioLPBK (
    SetM2MAudioLPBKReq *pSetM2MAudioLPBKReq )
{
    ULONG  resultcode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiAudioSLQSSetM2MAudioLPBKResp response;

    /* Checking for Invalid Parameter */
    if( NULL == pSetM2MAudioLPBKReq )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* pack QMI request */
    resultcode = PkQmiAudioSLQSSetM2MAudioLPBK( &paramLength,
                                                pOutParam,
                                                pSetM2MAudioLPBKReq );
    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_SWIAUDIO,
                                  paramLength,
                                  eQMI_TIMEOUT_5_S, /* 5 seconds */
                                  &pInParam,
                                  &paramLength );

    /* Only parse out the response data if we got a positive return */
    if ( eQCWWAN_ERR_NONE == resultcode )
    {
        /* Copy to the caller's buffer */
        resultcode = UpkQmiAudioSLQSSetM2MAudioLPBK( pInParam, &response );
    }

    qmrelreqbkp();
    return resultcode;
}

ULONG SLQSSetM2MAudioNVDef( )
{
    ULONG  resultcode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiAudioSLQSSetM2MAudioNVDefResp response;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* pack QMI request */
    resultcode = PkQmiAudioSLQSSetM2MAudioNVDef( &paramLength,
                                                 pOutParam );
    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_SWIAUDIO,
                                  paramLength,
                                  eQMI_TIMEOUT_5_S, /* 5 seconds */
                                  &pInParam,
                                  &paramLength );

    /* Only parse out the response data if we got a positive return */
    if ( eQCWWAN_ERR_NONE == resultcode )
    {
        /* Copy to the caller's buffer */
        resultcode = UpkQmiAudioSLQSSetM2MAudioNVDef( pInParam, &response );
    }

    qmrelreqbkp();
    return resultcode;
}

ULONG SLQSGetM2MSpkrGain (
    GetM2MSpkrGainReq  *pSpkrGainReq,
    GetM2MSpkrGainResp *pSpkrGainResp )
{
    ULONG  resultcode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiAudioSLQSGetM2MSpkrGainResp response;

    /* Checking for Invalid Parameter */
    if ( NULL == pSpkrGainReq  ||
         NULL == pSpkrGainResp )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* pack QMI request */
    resultcode = PkQmiAudioSLQSGetM2MSpkrGain( &paramLength,
                                               pOutParam,
                                               pSpkrGainReq );
    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_SWIAUDIO,
                                  paramLength,
                                  eQMI_TIMEOUT_5_S, /* 5 seconds */
                                  &pInParam,
                                  &paramLength );

    /* Only parse out the response data if we got a positive return */
    if ( eQCWWAN_ERR_NONE == resultcode )
        {
            /* Pass the pointer for the OUT parameters */
            response.pSpkrGainResp = pSpkrGainResp;

            /* Initializing the response with default values */
            pSpkrGainResp->Value = 0xFFFF;

            /* Copy to the caller's buffer */
            resultcode = UpkQmiAudioSLQSGetM2MSpkrGain( pInParam, &response );
        }

    qmrelreqbkp();
    return resultcode;
}

ULONG SLQSSetM2MSpkrGain (
    SetM2MSpkrGainReq *pSpkrGainReq )
{
    ULONG  resultcode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiAudioSLQSSetM2MSpkrGainResp response;

    /* Checking for Invalid Parameter */
    if ( NULL == pSpkrGainReq )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* pack QMI request */
    resultcode = PkQmiAudioSLQSSetM2MSpkrGain( &paramLength,
                                               pOutParam,
                                               pSpkrGainReq );
    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_SWIAUDIO,
                                  paramLength,
                                  eQMI_TIMEOUT_5_S, /* 5 seconds */
                                  &pInParam,
                                  &paramLength );

    /* Only parse out the response data if we got a positive return */
    if ( eQCWWAN_ERR_NONE == resultcode )
        {
            /* Copy to the caller's buffer */
            resultcode = UpkQmiAudioSLQSSetM2MSpkrGain( pInParam, &response );
        }

    qmrelreqbkp();
    return resultcode;
}

ULONG SLQSGetM2MAVMute (
    GetM2MAVMuteReq  *pGetM2MAVMuteReq,
    GetM2MAVMuteResp *pGetM2MAVMuteResp )
{
    ULONG  resultcode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiAudioSLQSGetM2MAVMuteResp response;

    /* Checking for Invalid Parameter */
    if ( NULL == pGetM2MAVMuteReq  ||
         NULL == pGetM2MAVMuteResp )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* pack QMI request */
    resultcode = PkQmiAudioSLQSGetM2MAVMute( &paramLength,
                                                  pOutParam,
                                                  pGetM2MAVMuteReq );
    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_SWIAUDIO,
                                  paramLength,
                                  eQMI_TIMEOUT_5_S, /* 5 seconds */
                                  &pInParam,
                                  &paramLength );

    /* Only parse out the response data if we got a positive return */
    if ( eQCWWAN_ERR_NONE == resultcode )
        {
            /* Pass the pointer for the OUT parameters */
            response.pGetM2MAVMuteResp = pGetM2MAVMuteResp;

            /* Initializing the response with default values */
            pGetM2MAVMuteResp->CwtMute = 0xFF;
            pGetM2MAVMuteResp->EarMute = 0xFF;
            pGetM2MAVMuteResp->MicMute = 0xFF;

            /* Copy to the caller's buffer */
            resultcode = UpkQmiAudioSLQSGetM2MAVMute( pInParam, &response );
        }

    qmrelreqbkp();
    return resultcode;
}

ULONG SLQSSetM2MAVMute (
    SetM2MAVMuteReq *pSetM2MAVMuteReq )
{
    ULONG  resultcode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiAudioSLQSSetM2MAVMuteResp response;

    /* Checking for Invalid Parameter */
    if ( NULL == pSetM2MAVMuteReq )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* pack QMI request */
    resultcode = PkQmiAudioSLQSSetM2MAVMute( &paramLength,
                                             pOutParam,
                                             pSetM2MAVMuteReq );
    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_SWIAUDIO,
                                  paramLength,
                                  eQMI_TIMEOUT_5_S, /* 5 seconds */
                                  &pInParam,
                                  &paramLength );

    /* Only parse out the response data if we got a positive return */
    if ( eQCWWAN_ERR_NONE == resultcode )
    {
        /* Copy to the caller's buffer */
        resultcode = UpkQmiAudioSLQSSetM2MAVMute( pInParam, &response );
    }

    qmrelreqbkp();
    return resultcode;
}
