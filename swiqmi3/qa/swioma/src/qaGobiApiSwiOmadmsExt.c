/**
 * \ingroup swioma
 *
 * \file    qaGobiApiSwiOmadmsExt.c
 *
 * \brief   Entry points for Gobi APIs for the Open Mobile Alliance Device
 *          Management Service (OMADMS). This file has OMA-DM code that is specific
 *          to SL9090 module for SPRINT network
 *
 * Copyright: © 2011-2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "amudefs.h"
#include "qaQmiBasic.h"
#include "qaGobiApiSwiOmadmsExt.h"
#include "qaSwiOmaDmSessionStartExt.h"
#include "qaSwiOmaDmSessionCancelExt.h"
#include "qaSwiOmaDmSelectionExt.h"
#include "qaSwiOmaDmSetSettingsExt.h"
#include "qaSwiOmaDmSessionGetInfoExt.h"


/* This function is used to initiate the OMA-DM Session for SL9090 module
   Parameter sessionType should be:
   0: Client Initiated Device Configuration (CIDC)
   1: Client initiated FUMO
   2: Device initiated FUMO
   3: Device Initiated DM
   4. Client Initiated PRL
   5. User Initiated HFA */
   
ULONG SLQSOMADMStartSessionExt(
    ULONG sessionType )
{
    ULONG  resultCode;  /* Result of SwiQmiSendnWait() */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiSwiOmaDmSessionStartRespExt response;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs the QMI message */
    resultCode = PkQmiSwiOmaDmSessionStartExt( &paramLength,
                                               pOutParam,
                                               sessionType );

    if ( eQCWWAN_ERR_NONE != resultCode )
    {
        qmrelreqbkp();
        return resultCode; /* No Memory */
    }

    resultCode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_SWIOMA_MDM6600,
                                  paramLength,
                                  eQMI_TIMEOUT_20_S, /* 20 Seconds */
                                  &pInParam,
                                  &paramLength );

    /* Only parse out the response data if we got a positive return */
    if ( eQCWWAN_ERR_NONE == resultCode )
    {
        /* Copy to the caller's buffer */
        resultCode = UpkQmiSwiOmaDmSessionStartExt( pInParam, &response );
    }

    qmrelreqbkp();
    return resultCode;
}

/* This function is used to cancel the already going OMA-DM Session for SL9090 module
   Parameter sessionType should be:
   0: Client Initiated Device Configuration (CIDC)
   1: Client initiated FUMO
   2: Device initiated FUMO
   3: Device Initiated DM
   4. Client Initiated PRL
   5. User Initiated HFA */
ULONG SLQSOMADMCancelSessionExt(
    ULONG session )
{
    ULONG  resultCode;  /* Result of SwiQmiSendnWait() */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiSwiOmaDmSessionCancelRespExt response;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs the QMI message */
    resultCode = PkQmiSwiOmaDmSessionCancelExt( &paramLength,
                                                pOutParam,
                                                session );

    if ( eQCWWAN_ERR_NONE != resultCode )
    {
        qmrelreqbkp();
        return resultCode; /* No Memory */
    }

    resultCode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_SWIOMA_MDM6600,
                                  paramLength,
                                  eQMI_TIMEOUT_20_S, /* 20 Seconds */
                                  &pInParam,
                                  &paramLength );

    /* Only parse out the response data if we got a positive return */
    if ( eQCWWAN_ERR_NONE == resultCode )
    {
        /* Copy to the caller's buffer */
        resultCode = UpkQmiSwiOmaDmSessionCancelExt( pInParam, &response );
    }

    qmrelreqbkp();
    return resultCode;
}

/* This function is used to select a selection to be sent to the OMA in the module for 
   selecting a particular option. This allows the user to defer the firmware update procedure
   by providing a list of options and values corresponding to that */

ULONG SLQSOMADMSendSelectionExt(
    ULONG selection )
{
    ULONG  resultCode;  /* Result of SwiQmiSendnWait() */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiSwiOmaDmSelectionRespExt response;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs the QMI message */
    resultCode = PkQmiSwiOmaDmSelectionExt( &paramLength,
                                            pOutParam,
                                            selection );

    if ( eQCWWAN_ERR_NONE != resultCode )
    {
        qmrelreqbkp();
        return resultCode; /* No Memory */
    }

    resultCode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_SWIOMA_MDM6600,
                                  paramLength,
                                  eQMI_TIMEOUT_20_S, /* 20 Seconds */
                                  &pInParam,
                                  &paramLength );

    /* Only parse out the response data if we got a positive return */
    if ( eQCWWAN_ERR_NONE == resultCode )
    {
        /* Copy to the caller's buffer */
        resultCode = UpkQmiSwiOmaDmSelectionExt( pInParam, &response );
    }

    qmrelreqbkp();
    return resultCode;
}

/* This function is used to set the settings for the OMA-DM service in the firmware. This function is 
specific to SL9090 module for the SPRINT network */
ULONG SLQSOMADMSetSettingsExt(
    ULONG bFOTAdownload,
    ULONG bFOTAUpdate,
    ULONG bOMADMEnable,
    ULONG bOMALOGEnable,
    ULONG bFUMOEnable,
    ULONG bPRLEnable )
{
    ULONG                         resultCode;  /* Result of SLQSOMADMSetSettings2 */
    SLQSOMADMSettingsReqParamsExt slqsOMADMSettings;

    slqsOMADMSettings.FOTAdownload    = bFOTAdownload;
    slqsOMADMSettings.FOTAUpdate      = bFOTAUpdate;   
    slqsOMADMSettings.OMADMEnable     = bOMADMEnable;
    slqsOMADMSettings.OMADMLogEnable  = bOMALOGEnable;
    slqsOMADMSettings.FUMOEnable      = bFUMOEnable;
    slqsOMADMSettings.PRLEnable       = bPRLEnable;
	

    resultCode = SLQSOMADMSetSettings2Ext( &slqsOMADMSettings );

    return resultCode;
}

ULONG SLQSOMADMSetSettings2Ext(
    SLQSOMADMSettingsReqParamsExt *pSLQSOMADMSettingsReqParams)
{
    ULONG  resultCode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiSwiOmaDmSetSettingsRespExt response;

    /* Check for NULL for input structure*/
    if ( NULL == pSLQSOMADMSettingsReqParams )
        return eQCWWAN_ERR_INVALID_ARG;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs the QMI message */
    resultCode = PkQmiSwiOmaDmSetSettingsExt( &paramLength,
                                              pOutParam,
                                              pSLQSOMADMSettingsReqParams );

    if ( eQCWWAN_ERR_NONE != resultCode )
    {
        qmrelreqbkp();
        return resultCode; /* No Memory */
    }

    resultCode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_SWIOMA_MDM6600,
                                  paramLength,
                                  eQMI_TIMEOUT_20_S, /* 20 Seconds */
                                  &pInParam,
                                  &paramLength );

    /* Only parse out the response data if we got a positive return */
    if ( eQCWWAN_ERR_NONE == resultCode )
    {
        /* Copy to the caller's buffer */
        resultCode = UpkQmiSwiOmaDmSetSettingsExt( pInParam, &response );
    }

    qmrelreqbkp();
    return resultCode;

}

ULONG SLQSOMADMGetSessionInfoExt(
    SLQSOMADMSessionInfoExt *pResp )
{
    ULONG  resultCode;  /* Result of SwiQmiSendnWait() */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiSwiOmaDmSessionGetInfoExtResp response;

    /* Check parameters for NULL */
    if (NULL == pResp )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message and sends the message. */
    resultCode = PkQmiSwiOmaDmSessionGetInfoExt( &paramLength,
                                              pOutParam );

    if ( eQCWWAN_ERR_NONE != resultCode )
    {
        qmrelreqbkp();
        return resultCode; /* No Memory */
    }

    /* Prepare and send the blocking call */
    resultCode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_SWIOMA_MDM6600,
                                  paramLength,
                                  eQMI_TIMEOUT_20_S, /* 20 Seconds */
                                  &pInParam,
                                  &paramLength );

    /* Only parse out the response data if we got a positive return */
    if ( eQCWWAN_ERR_NONE == resultCode )
    {
        /* Copy the obtained values to the function OUT parameters */
        response.pResp = pResp;

        /* Copy to the caller's buffer */
        resultCode = UpkQmiSwiOmaDmSessionGetInfoExt( pInParam, &response );
    }

    qmrelreqbkp();
    return resultCode;
}


