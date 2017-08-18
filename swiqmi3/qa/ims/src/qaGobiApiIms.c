/*
 * \ingroup ims
 *
 * \file qaGobiApiIms.c
 *
 * \brief  Entry points for Gobi APIs for the IMS Service ( IMS )
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
#include "qaGobiApiIms.h"
#include "qaImsSLQSSetSIPConfig.h"
#include "qaImsSLQSSetRegMgrConfig.h"
#include "qaImsSLQSSetIMSSMSConfig.h"
#include "qaImsSLQSSetIMSUserConfig.h"
#include "qaImsSLQSSetIMSVoIPConfig.h"
#include "qaImsSLQSGetSIPConfig.h"
#include "qaImsSLQSGetRegMgrConfig.h"
#include "qaImsSLQSGetIMSSMSConfig.h"
#include "qaImsSLQSGetIMSUserConfig.h"
#include "qaImsSLQSGetIMSVoIPConfig.h"
#include "qaImsSLQSImsConfigIndRegister.h"

ULONG SLQSSetSIPConfig (
    SetSIPConfigReq  *pSetSIPConfigReq,
    SetSIPConfigResp *pSetSIPConfigResp )
{
    ULONG  resultcode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiImsSLQSSetSIPConfigResp response;

    /* Checking for Invalid Parameter */
    if ( NULL == pSetSIPConfigReq ||
         NULL == pSetSIPConfigResp )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* pack QMI request */
    resultcode = PkQmiImsSLQSSetSIPConfig( &paramLength,
                                           pOutParam,
                                           pSetSIPConfigReq );
    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_IMS,
                                  paramLength,
                                  eQMI_TIMEOUT_5_S, /* 5 seconds */
                                  &pInParam,
                                  &paramLength );

    /* Only parse out the response data if we got a positive return */
    if ( eQCWWAN_ERR_NONE == resultcode )
    {
        /* Pass the pointer for the OUT parameters */
        response.pSetSIPConfigResp = pSetSIPConfigResp;

        /* Initializing the response with default values */
        if( NULL != pSetSIPConfigResp->pSettingResp )
        {
            *(pSetSIPConfigResp->pSettingResp) = 0xFF;
        }

        /* Copy to the caller's buffer */
        resultcode = UpkQmiImsSLQSSetSIPConfig(pInParam,&response);
    }

    qmrelreqbkp();
    return resultcode;
}

ULONG SLQSSetRegMgrConfig (
    SetRegMgrConfigReq  *pSetRegMgrConfigReq,
    SetRegMgrConfigResp *pSetRegMgrConfigResp )
{
    ULONG  resultcode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiImsSLQSSetRegMgrConfigResp response;

    /* Checking for Invalid Parameter */
    if ( NULL == pSetRegMgrConfigReq ||
         NULL == pSetRegMgrConfigResp )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* pack QMI request */
    resultcode = PkQmiImsSLQSSetRegMgrConfig( &paramLength,
                                              pOutParam,
                                              pSetRegMgrConfigReq );
    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_IMS,
                                  paramLength,
                                  eQMI_TIMEOUT_5_S, /* 5 seconds */
                                  &pInParam,
                                  &paramLength );

    /* Only parse out the response data if we got a positive return */
    if ( eQCWWAN_ERR_NONE == resultcode )
    {
        /* Pass the pointer for the OUT parameters */
        response.pSetRegMgrConfig = pSetRegMgrConfigResp;

        /* Initializing the response with default values */
        if( NULL != pSetRegMgrConfigResp->pSettingResp )
        {
            *(pSetRegMgrConfigResp->pSettingResp) = 0xFF;
        }

        /* Copy to the caller's buffer */
        resultcode = UpkQmiImsSLQSSetRegMgrConfig(pInParam,&response);
    }

    qmrelreqbkp();
    return resultcode;
}

ULONG SLQSSetIMSSMSConfig (
    SetIMSSMSConfigReq  *pSetIMSSMSConfigReq,
    SetIMSSMSConfigResp *pSetIMSSMSConfigResp )
{
    ULONG  resultcode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiImsSLQSSetIMSSMSConfigResp response;

    /* Checking for Invalid Parameter */
    if ( NULL == pSetIMSSMSConfigReq ||
         NULL == pSetIMSSMSConfigResp )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* pack QMI request */
    resultcode = PkQmiImsSLQSSetIMSSMSConfig( &paramLength,
                                              pOutParam,
                                              pSetIMSSMSConfigReq );
    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_IMS,
                                  paramLength,
                                  eQMI_TIMEOUT_5_S, /* 5 seconds */
                                  &pInParam,
                                  &paramLength );

    /* Only parse out the response data if we got a positive return */
    if ( eQCWWAN_ERR_NONE == resultcode )
    {
        /* Pass the pointer for the OUT parameters */
        response.pSetIMSSMSConfigResp = pSetIMSSMSConfigResp;

        /* Initializing the response with default values */
        if( NULL != pSetIMSSMSConfigResp->pSettingResp )
        {
            *(pSetIMSSMSConfigResp->pSettingResp) = 0xFF;
        }

        /* Copy to the caller's buffer */
        resultcode = UpkQmiImsSLQSSetIMSSMSConfig(pInParam,&response);
    }

    qmrelreqbkp();
    return resultcode;
}

ULONG SLQSSetIMSUserConfig (
    SetIMSUserConfigReq  *pSetIMSUserConfigReq,
    SetIMSUserConfigResp *pSetIMSUserConfigResp )
{
    ULONG  resultcode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiImsSLQSSetIMSUserConfigResp response;

    /* Checking for Invalid Parameter */
    if ( NULL == pSetIMSUserConfigReq ||
         NULL == pSetIMSUserConfigResp )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* pack QMI request */
    resultcode = PkQmiImsSLQSSetIMSUserConfig( &paramLength,
                                               pOutParam,
                                               pSetIMSUserConfigReq );
    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_IMS,
                                  paramLength,
                                  eQMI_TIMEOUT_5_S, /* 5 seconds */
                                  &pInParam,
                                  &paramLength );

    /* Only parse out the response data if we got a positive return */
    if ( eQCWWAN_ERR_NONE == resultcode )
    {
        /* Pass the pointer for the OUT parameters */
        response.pSetIMSUserConfigResp = pSetIMSUserConfigResp;

        /* Initializing the response with default values */
        if( NULL != pSetIMSUserConfigResp->pSettingResp )
        {
            *(pSetIMSUserConfigResp->pSettingResp) = 0xFF;
        }

        /* Copy to the caller's buffer */
        resultcode = UpkQmiImsSLQSSetIMSUserConfig(pInParam,&response);
    }

    qmrelreqbkp();
    return resultcode;
}

ULONG SLQSSetIMSVoIPConfig (
    SetIMSVoIPConfigReq  *pSetIMSVoIPConfigReq,
    SetIMSVoIPConfigResp *pSetIMSVoIPConfigResp )
{
    ULONG  resultcode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiImsSLQSSetIMSVoIPConfigResp response;

    /* Checking for Invalid Parameter */
    if ( NULL == pSetIMSVoIPConfigReq ||
         NULL == pSetIMSVoIPConfigResp )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* pack QMI request */
    resultcode = PkQmiImsSLQSSetIMSVoIPConfig( &paramLength,
                                               pOutParam,
                                               pSetIMSVoIPConfigReq );
    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_IMS,
                                  paramLength,
                                  eQMI_TIMEOUT_5_S, /* 5 seconds */
                                  &pInParam,
                                  &paramLength );

    /* Only parse out the response data if we got a positive return */
    if ( eQCWWAN_ERR_NONE == resultcode )
    {
        /* Pass the pointer for the OUT parameters */
        response.pSetIMSVoIPConfigResp = pSetIMSVoIPConfigResp;

        /* Initializing the response with default values */
        if( NULL != pSetIMSVoIPConfigResp->pSettingResp )
        {
            *(pSetIMSVoIPConfigResp->pSettingResp) = 0xFF;
        }

        /* Copy to the caller's buffer */
        resultcode = UpkQmiImsSLQSSetIMSVoIPConfig(pInParam,&response);
    }

    qmrelreqbkp();
    return resultcode;
}

ULONG SLQSGetSIPConfig (
    GetSIPConfigResp *pGetSIPConfigResp )
{
    ULONG  resultcode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiImsSLQSGetSIPConfigResp response;

    /* Checking for Invalid Parameter */
    if ( NULL == pGetSIPConfigResp )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* pack QMI request */
    resultcode = PkQmiImsSLQSGetSIPConfig( &paramLength,
                                           pOutParam );
    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_IMS,
                                  paramLength,
                                  eQMI_TIMEOUT_5_S, /* 5 seconds */
                                  &pInParam,
                                  &paramLength );

    /* Only parse out the response data if we got a positive return */
    if ( eQCWWAN_ERR_NONE == resultcode )
    {
        /* Pass the pointer for the OUT parameters */
        response.pGetSIPConfigResp = pGetSIPConfigResp;

        /* Initializing the response with default values */
        if( NULL != pGetSIPConfigResp->pSettingResp )
        {
            *(pGetSIPConfigResp->pSettingResp)    = 0xFF;
        }
        if( NULL != pGetSIPConfigResp->pSIPLocalPort )
        {
            *(pGetSIPConfigResp->pSIPLocalPort)   = 0xFFFF;
        }
        if( NULL != pGetSIPConfigResp->pTimerSIPReg )
        {
            *(pGetSIPConfigResp->pTimerSIPReg)    = 0xFFFFFFFF;
        }
        if( NULL != pGetSIPConfigResp->pTimerT1 )
        {
            *(pGetSIPConfigResp->pTimerT1)        = 0xFFFFFFFF;
        }
        if( NULL != pGetSIPConfigResp->pTimerT2 )
        {
            *(pGetSIPConfigResp->pTimerT2)        = 0xFFFFFFFF;
        }
        if( NULL != pGetSIPConfigResp->pTimerTf )
        {
            *(pGetSIPConfigResp->pTimerTf)        = 0xFFFFFFFF;
        }
        if( NULL != pGetSIPConfigResp->pSigCompEnabled )
        {
            *(pGetSIPConfigResp->pSigCompEnabled) = 0xFF;
        }

        /* Copy to the caller's buffer */
        resultcode = UpkQmiImsSLQSGetSIPConfig(pInParam,&response);
    }

    qmrelreqbkp();
    return resultcode;
}

ULONG SLQSGetRegMgrConfig (
    GetRegMgrConfigParams *pGetRegMgrConfigParams )
{
    ULONG  resultcode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiImsSLQSGetRegMgrConfigResp response;

    /* Checking for Invalid Parameter */
    if ( NULL == pGetRegMgrConfigParams )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* pack QMI request */
    resultcode = PkQmiImsSLQSGetRegMgrConfig( &paramLength,
                                              pOutParam );
    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_IMS,
                                  paramLength,
                                  eQMI_TIMEOUT_5_S, /* 5 seconds */
                                  &pInParam,
                                  &paramLength );

    /* Only parse out the response data if we got a positive return */
    if ( eQCWWAN_ERR_NONE == resultcode )
    {
        /* Pass the pointer for the OUT parameters */
        response.pGetRegMgrConfigParams = pGetRegMgrConfigParams;

        /* Initializing the response with default values */
        if( NULL != pGetRegMgrConfigParams->pSettingResp )
        {
            *(pGetRegMgrConfigParams->pSettingResp) = 0xFF;
        }
        if( NULL != pGetRegMgrConfigParams->pPCSCFPort )
        {
            *(pGetRegMgrConfigParams->pPCSCFPort)   = 0xFFFF;
        }
        if( NULL != pGetRegMgrConfigParams->pIMSTestMode )
        {
            *(pGetRegMgrConfigParams->pIMSTestMode) = 0xFF;
        }

        /* Copy to the caller's buffer */
        resultcode = UpkQmiImsSLQSGetRegMgrConfig(pInParam,&response);
    }

    qmrelreqbkp();
    return resultcode;
}

ULONG SLQSGetIMSSMSConfig (
    GetIMSSMSConfigParams *pGetIMSSMSConfigParams )
{
    ULONG  resultcode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiImsSLQSGetIMSSMSConfigResp response;

    /* Checking for Invalid Parameter */
    if ( NULL == pGetIMSSMSConfigParams )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* pack QMI request */
    resultcode = PkQmiImsSLQSGetIMSSMSConfig( &paramLength,
                                              pOutParam );
    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_IMS,
                                  paramLength,
                                  eQMI_TIMEOUT_5_S, /* 5 seconds */
                                  &pInParam,
                                  &paramLength );

    /* Only parse out the response data if we got a positive return */
    if ( eQCWWAN_ERR_NONE == resultcode )
    {
        /* Pass the pointer for the OUT parameters */
        response.pGetIMSSMSConfigParams = pGetIMSSMSConfigParams;

        /* Initializing the response with default values */
        if( NULL != pGetIMSSMSConfigParams->pSettingResp )
        {
            *(pGetIMSSMSConfigParams->pSettingResp)    = 0xFF;
        }
        if( NULL != pGetIMSSMSConfigParams->pSMSFormat )
        {
            *(pGetIMSSMSConfigParams->pSMSFormat)      = 0xFF;
        }
        if( NULL != pGetIMSSMSConfigParams->pSMSOverIPNwInd )
        {
            *(pGetIMSSMSConfigParams->pSMSOverIPNwInd) = 0xFF;
        }

        /* Copy to the caller's buffer */
        resultcode = UpkQmiImsSLQSGetIMSSMSConfig(pInParam,&response);
    }

    qmrelreqbkp();
    return resultcode;
}

ULONG SLQSGetIMSUserConfig (
    GetIMSUserConfigParams *pGetIMSUserConfigParams )
{
    ULONG  resultcode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiImsSLQSGetIMSUserConfigResp response;

    /* Checking for Invalid Parameter */
    if ( NULL == pGetIMSUserConfigParams )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* pack QMI request */
    resultcode = PkQmiImsSLQSGetIMSUserConfig( &paramLength,
                                               pOutParam );
    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_IMS,
                                  paramLength,
                                  eQMI_TIMEOUT_5_S, /* 5 seconds */
                                  &pInParam,
                                  &paramLength );

    /* Only parse out the response data if we got a positive return */
    if ( eQCWWAN_ERR_NONE == resultcode )
    {
        /* Pass the pointer for the OUT parameters */
        response.pGetIMSUserConfigParams = pGetIMSUserConfigParams;

        /* Initializing the response with default values */
        if( NULL != pGetIMSUserConfigParams->pSettingResp )
        {
            *(pGetIMSUserConfigParams->pSettingResp)    = 0xFF;
        }

        /* Copy to the caller's buffer */
        resultcode = UpkQmiImsSLQSGetIMSUserConfig(pInParam,&response);
    }

    qmrelreqbkp();
    return resultcode;
}

ULONG SLQSGetIMSVoIPConfig (
    GetIMSVoIPConfigResp *pGetIMSVoIPConfigResp )
{
    ULONG  resultcode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiImsSLQSGetIMSVoIPConfigResp response;

    /* Checking for Invalid Parameter */
    if ( NULL == pGetIMSVoIPConfigResp )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* pack QMI request */
    resultcode = PkQmiImsSLQSGetIMSVoIPConfig( &paramLength,
                                               pOutParam );
    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_IMS,
                                  paramLength,
                                  eQMI_TIMEOUT_5_S, /* 5 seconds */
                                  &pInParam,
                                  &paramLength );

    /* Only parse out the response data if we got a positive return */
    if ( eQCWWAN_ERR_NONE == resultcode )
    {
        /* Pass the pointer for the OUT parameters */
        response.pGetIMSVoIPConfigResp = pGetIMSVoIPConfigResp;

        /* Initializing the response with default values */
        if( NULL != pGetIMSVoIPConfigResp->pSettingResp )
        {
            *(pGetIMSVoIPConfigResp->pSettingResp) = 0xFF;
        }
        if( NULL != pGetIMSVoIPConfigResp->pSessionExpiryTimer )
        {
            *(pGetIMSVoIPConfigResp->pSessionExpiryTimer) = 0xFFFF;
        }
        if( NULL != pGetIMSVoIPConfigResp->pMinSessionExpiryTimer )
        {
            *(pGetIMSVoIPConfigResp->pMinSessionExpiryTimer) = 0xFFFF;
        }
        if( NULL != pGetIMSVoIPConfigResp->pAmrWbEnable )
        {
            *(pGetIMSVoIPConfigResp->pAmrWbEnable) = 0xFF;
        }
        if( NULL != pGetIMSVoIPConfigResp->pScrAmrEnable )
        {
            *(pGetIMSVoIPConfigResp->pScrAmrEnable) = 0xFF;
        }
        if( NULL != pGetIMSVoIPConfigResp->pScrAmrWbEnable )
        {
            *(pGetIMSVoIPConfigResp->pScrAmrWbEnable) = 0xFF;
        }
        if( NULL != pGetIMSVoIPConfigResp->pAmrMode )
        {
            *(pGetIMSVoIPConfigResp->pAmrMode) = 0xFF;
        }
        if( NULL != pGetIMSVoIPConfigResp->pAmrWBMode )
        {
            *(pGetIMSVoIPConfigResp->pAmrWBMode) = 0xFFFF;
        }
        if( NULL != pGetIMSVoIPConfigResp->pAmrOctetAligned )
        {
            *(pGetIMSVoIPConfigResp->pAmrOctetAligned) = 0xFF;
        }
        if( NULL != pGetIMSVoIPConfigResp->pAmrWBOctetAligned )
        {
            *(pGetIMSVoIPConfigResp->pAmrWBOctetAligned) = 0xFF;
        }
        if( NULL != pGetIMSVoIPConfigResp->pRingingTimer )
        {
            *(pGetIMSVoIPConfigResp->pRingingTimer) = 0xFFFF;
        }
        if( NULL != pGetIMSVoIPConfigResp->pRingBackTimer )
        {
            *(pGetIMSVoIPConfigResp->pRingBackTimer) = 0xFFFF;
        }
        if( NULL != pGetIMSVoIPConfigResp->pRTPRTCPInactTimer )
        {
            *(pGetIMSVoIPConfigResp->pRTPRTCPInactTimer) = 0xFFFF;
        }

        /* Copy to the caller's buffer */
        resultcode = UpkQmiImsSLQSGetIMSVoIPConfig(pInParam,&response);
    }

    qmrelreqbkp();
    return resultcode;
}

ULONG SLQSImsConfigIndicationRegister(
    imsCfgIndRegisterInfo *pImsCfgIndRegisterInfo )
{
    ULONG  resultCode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiImsSLQSImsCfgIndRegisterResp response;

    /* structure params has IN/OUT Param, hence validated against NULL */
    if ( !pImsCfgIndRegisterInfo )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs the QMI message */
    resultCode = PkQmiImsSLQSImsCfgIndRegister( &paramLength,
                                                pOutParam,
                                                pImsCfgIndRegisterInfo );

    if ( eQCWWAN_ERR_NONE != resultCode )
    {
        qmrelreqbkp();
        return resultCode; /* No Memory */
    }

    resultCode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_IMS,
                                  paramLength,
                                  eQMI_TIMEOUT_5_S, /* 5 seconds */
                                  &pInParam,
                                  &paramLength );

    /* Only parse out the response data if we got a positive return */
    if ( eQCWWAN_ERR_NONE == resultCode )
    {
        /* Copy to the caller's buffer */
        resultCode = UpkQmiImsSLQSImsCfgIndRegister( pInParam, &response );
    }

    qmrelreqbkp();
    return resultCode;
}
