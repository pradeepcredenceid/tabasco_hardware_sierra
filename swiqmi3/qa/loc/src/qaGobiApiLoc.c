/*
 * \ingroup loc
 *
 * \file qaGobiApiLoc.c
 *
 * \brief  Entry points for Gobi APIs for the Location ( LOC )
 *
 * Copyright: © 2011-2015 Sierra Wireless, Inc. all rights reserved
 *
 */
#include <string.h>
#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "amudefs.h"
#include "sludefs.h"
#include "qaQmiBasic.h"
#include "qaGobiApiLoc.h"
#include "qaSlqsLocEventRegister.h"
#include "qaSlqsLocExtPowerState.h"
#include "qaSlqsLocStart.h"
#include "qaSlqsLocStop.h"
#include "qaSlqsLocSetOpMode.h"
#include "qaSlqsLocDeleteAssistData.h"
#include "qaSlqsLocInjectUTCTime.h"
#include "qaSlqsLocGetServer.h"
#include "qaSlqsLocSetServer.h"
#include "qaSlqsLocGetOpMode.h"
#include "qaSlqsLocGetFixCriteria.h"
#include "SwiLocAutoStart.h"
#include "qaSlqsLocInjectPosition.h"
#include "qaSlqsLocSetCradleMountConfig.h"
#include "qaSlqsLocInjectSensorData.h"

ULONG SLQSLOCEventRegister (
        LOCEventRegisterReqResp *pLOCEventRegisterReqResp )
{
    ULONG  resultCode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiLocSlqsEventRegisterResp response;

    /* Checking for Invalid Parameter */
    if ( NULL == pLOCEventRegisterReqResp )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();
    
    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );
    
    /* Invoke the function which packs the QMI message */
    resultCode = PkQmiLocSlqsEventRegister( &paramLength,
                                            pOutParam,
                                            pLOCEventRegisterReqResp );
    
    if ( eQCWWAN_ERR_NONE != resultCode )
        {
            qmrelreqbkp();
            return resultCode; /* No Memory */
        }
    
        resultCode = SwiQmiSendnWait( pReqBuf,
                                      eQMI_SVC_LOC,
                                      paramLength,
                                      eQMI_TIMEOUT_5_S, /* 5 seconds */
                                      &pInParam,
                                      &paramLength );
    
        /* Only parse out the response data if we got a positive return */
        if ( eQCWWAN_ERR_NONE == resultCode )
        {
            response.eventRegister = 0x00000000;
    
            /* Copy to the caller's buffer */
            resultCode = UpkQmiLocSlqsEventRegister( pInParam, &response );
        }
    
        qmrelreqbkp();
        return resultCode;
}

ULONG SLQSLOCSetExtPowerState(
              LOCExtPowerStateReqResp *pLOCExtPowerStateReqResp)
{
    ULONG  resultCode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiLocSlqsExtPowerStateResp response;

    /* Checking for Invalid Parameter */
    if ( NULL == pLOCExtPowerStateReqResp )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();
    
    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );
    
    /* Invoke the function which packs the QMI message */
    resultCode = PkQmiLocSlqsExtPowerState( &paramLength,
                                            pOutParam,
                                            pLOCExtPowerStateReqResp );
    
    if ( eQCWWAN_ERR_NONE != resultCode )
        {
            qmrelreqbkp();
            return resultCode; /* No Memory */
        }
    
        resultCode = SwiQmiSendnWait( pReqBuf,
                                      eQMI_SVC_LOC,
                                      paramLength,
                                      eQMI_TIMEOUT_5_S, /* 5 seconds */
                                      &pInParam,
                                      &paramLength );
    
        /* Only parse out the response data if we got a positive return */
        if ( eQCWWAN_ERR_NONE == resultCode )
        {
            response.extPowerState = pLOCExtPowerStateReqResp->extPowerState;
    
    
            /* Copy to the caller's buffer */
            resultCode = UpkQmiLocSlqsExtPowerState( pInParam, &response );
        }
    
        qmrelreqbkp();
        return resultCode;
    
}

ULONG SLQSLOCStart(LOCStartReq *pLOCStartReq)
{
    ULONG  resultCode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiLocStartResp response;

    /* Check if all parameters are null */
    if( !pLOCStartReq->SessionId                &&
        !pLOCStartReq->pRecurrenceType          &&
        !pLOCStartReq->pHorizontalAccuracyLvl   &&
        !pLOCStartReq->pIntermediateReportState &&
        !pLOCStartReq->pMinIntervalTime         &&
        !pLOCStartReq->pApplicationInfo         &&
        !pLOCStartReq->pConfigAltitudeAssumed)
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();
    
    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message */
    resultCode = PkQmiLocStartReq( &paramLength,
                                    pOutParam ,
                                    pLOCStartReq );

    if ( resultCode != eQCWWAN_ERR_NONE )
    {
        qmrelreqbkp();
        return resultCode; /* No memory */
    }
    /* Prepare and send the LOC Start Req */
    resultCode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_LOC,
                                  paramLength,
                                  eQMI_TIMEOUT_5_S, /* 5 Seconds */
                                  &pInParam,
                                  &paramLength );

    /* Only parse out the response data if we get a positive return */
    if ( eQCWWAN_ERR_NONE == resultCode )
    {
        /* Copy to the caller's buffer */
        resultCode = UpkQmiLocStartResp( pInParam, &response );
    }

    qmrelreqbkp();
    return resultCode;
}

ULONG SLQSLOCStop(LOCStopReq *pLOCStopReq)
{
    ULONG  resultCode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiLocStopResp response;

    /* Checking for Invalid Parameter */
    if ( NULL == pLOCStopReq )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }
    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message */
    resultCode = PkQmiLocStopReq( &paramLength,
                                    pOutParam ,
                                    pLOCStopReq );

    if ( resultCode != eQCWWAN_ERR_NONE )
    {
        qmrelreqbkp();
        return resultCode; /* No memory */
    }
    /* Prepare and send the LOC Start Req */
    resultCode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_LOC,
                                  paramLength,
                                  eQMI_TIMEOUT_5_S, /* 5 Seconds */
                                  &pInParam,
                                  &paramLength );

    /* Only parse out the response data if we get a positive return */
    if ( eQCWWAN_ERR_NONE == resultCode )
    {
        /* Copy to the caller's buffer */
        resultCode = UpkQmiLocStopResp( pInParam, &response );
    }

    qmrelreqbkp();
    return resultCode;
}

ULONG SLQSLOCSetOpMode(ULONG mode)
{
    ULONG  resultCode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiLocSetOpModeResp response;

    if( mode > 6 ) //TODO switch to enum max
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message */
    resultCode = PkQmiLocSetOpMode( &paramLength,
            pOutParam ,
            mode );

    if ( resultCode != eQCWWAN_ERR_NONE )
    {
        qmrelreqbkp();
        return resultCode; /* No memory */
    }
    /* Prepare and send the LOC Start Req */
    resultCode = SwiQmiSendnWait( pReqBuf,
            eQMI_SVC_LOC,
            paramLength,
            eQMI_TIMEOUT_5_S, /* 5 Seconds */
            &pInParam,
            &paramLength );

    /* Only parse out the response data if we get a positive return */
    if ( eQCWWAN_ERR_NONE == resultCode )
    {
        /* Copy to the caller's buffer */
        resultCode = UpkQmiLocSetOpMode( pInParam, &response );
    }

    qmrelreqbkp();
    return resultCode;
}

ULONG SLQSLOCDelAssData(LocDelAssDataReq req)
{
    ULONG  resultCode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiLocDelAssDataResp response;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message */
    resultCode = PkQmiLocDelAssData( &paramLength, pOutParam, &req);

    if ( resultCode != eQCWWAN_ERR_NONE )
    {
        qmrelreqbkp();
        return resultCode; /* No memory */
    }
    /* Prepare and send the LOC Start Req */
    resultCode = SwiQmiSendnWait( pReqBuf,
            eQMI_SVC_LOC,
            paramLength,
            eQMI_TIMEOUT_5_S, /* 5 Seconds */
            &pInParam,
            &paramLength );

    /* Only parse out the response data if we get a positive return */
    if ( eQCWWAN_ERR_NONE == resultCode )
    {
        /* Copy to the caller's buffer */
        resultCode = UpkQmiLocDelAssData( pInParam, &response );
    }

    qmrelreqbkp();
    return resultCode;
}


ULONG SLQSLOCInjectUTCTime(
    ULONGLONG timeMsec,
    ULONG     timeUncMsec)
{
    ULONG  resultCode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiLocInjectUTCTimeResp response;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message */
    resultCode = PkQmiLocInjectUTCTime( &paramLength, 
                                        pOutParam,
                                        timeMsec,
                                        timeUncMsec);

    if ( resultCode != eQCWWAN_ERR_NONE )
    {
        qmrelreqbkp();
        return resultCode; /* No memory */
    }
    /* Prepare and send the LOC Inject UTC time Req */
    resultCode = SwiQmiSendnWait( pReqBuf,
            eQMI_SVC_LOC,
            paramLength,
            eQMI_TIMEOUT_5_S, /* 5 Seconds */
            &pInParam,
            &paramLength );

    /* Only parse out the response data if we get a positive return */
    if ( eQCWWAN_ERR_NONE == resultCode )
    {
        /* Copy to the caller's buffer */
        resultCode = UpkQmiLocInjectUTCTime( pInParam, &response );
    }

    qmrelreqbkp();
    return resultCode;
}

ULONG SLQSLOCGetServer(LOCGetServerReq req)
{
    ULONG  resultCode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiLocGetServerResp response;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message */
    resultCode = PkQmiLocGetServerReq( &paramLength, pOutParam, &req);

    if ( resultCode != eQCWWAN_ERR_NONE )
    {
        qmrelreqbkp();
        return resultCode; /* No memory */
    }
    /* Prepare and send the LOC Get Server Req */
    resultCode = SwiQmiSendnWait( pReqBuf,
            eQMI_SVC_LOC,
            paramLength,
            eQMI_TIMEOUT_5_S, /* 5 Seconds */
            &pInParam,
            &paramLength );

    /* Only parse out the response data if we get a positive return */
    if ( eQCWWAN_ERR_NONE == resultCode )
    {
        /* Copy to the caller's buffer */
        resultCode = UpkQmiLocGetServerResp( pInParam, &response );
    }

    qmrelreqbkp();
    return resultCode;
}

ULONG SLQSLOCSetServer(LOCSetServerReq req)
{
    ULONG  resultCode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiLocSetServerResp response;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message */
    resultCode = PkQmiLocSetServerReq( &paramLength, pOutParam, &req);

    if ( resultCode != eQCWWAN_ERR_NONE )
    {
        qmrelreqbkp();
        return resultCode; /* No memory */
    }
    /* Prepare and send the LOC Set Server Req */
    resultCode = SwiQmiSendnWait( pReqBuf,
            eQMI_SVC_LOC,
            paramLength,
            eQMI_TIMEOUT_5_S, /* 5 Seconds */
            &pInParam,
            &paramLength );

    /* Only parse out the response data if we get a positive return */
    if ( eQCWWAN_ERR_NONE == resultCode )
    {
        /* Copy to the caller's buffer */
        resultCode = UpkQmiLocSetServerResp( pInParam, &response );
    }

    qmrelreqbkp();
    return resultCode;
}

ULONG SLQSLOCGetOpMode()
{
    ULONG  resultCode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiLocGetOpModeResp response;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message */
    resultCode = PkQmiLocGetOpModeReq( &paramLength, pOutParam);

    if ( resultCode != eQCWWAN_ERR_NONE )
    {
        qmrelreqbkp();
        return resultCode; /* No memory */
    }
    /* Prepare and send the LOC Get Operation Mode Req */
    resultCode = SwiQmiSendnWait( pReqBuf,
            eQMI_SVC_LOC,
            paramLength,
            eQMI_TIMEOUT_5_S, /* 5 Seconds */
            &pInParam,
            &paramLength );

    /* Only parse out the response data if we get a positive return */
    if ( eQCWWAN_ERR_NONE == resultCode )
    {
        /* Copy to the caller's buffer */
        resultCode = UpkQmiLocGetOpModeResp( pInParam, &response );
    }

    qmrelreqbkp();
    return resultCode;
}

ULONG SLQSLOCGetFixCriteria()
{
    ULONG  resultCode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiLocGetFixCriteriaResp response;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message */
    resultCode = PkQmiLocGetFixCriteriaReq( &paramLength, pOutParam);

    if ( resultCode != eQCWWAN_ERR_NONE )
    {
        qmrelreqbkp();
        return resultCode; /* No memory */
    }
    /* Prepare and send the LOC Get Fix Criteria Req */
    resultCode = SwiQmiSendnWait( pReqBuf,
            eQMI_SVC_LOC,
            paramLength,
            eQMI_TIMEOUT_5_S, /* 5 Seconds */
            &pInParam,
            &paramLength );

    /* Only parse out the response data if we get a positive return */
    if ( eQCWWAN_ERR_NONE == resultCode )
    {
        /* Copy to the caller's buffer */
        resultCode = UpkQmiLocGetFixCriteriaResp( pInParam, &response );
    }

    qmrelreqbkp();
    return resultCode;
}

ULONG SwiLocGetAutoStart(SwiLocGetAutoStartResp *resp)
{
    ULONG  resultCode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiSwiLocGetAutoStartResp response;

    if (resp == NULL)
        return eQCWWAN_ERR_INVALID_ARG;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();
    
    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message */
    resultCode = PkSwiLocGetAutoStart( &paramLength, pOutParam);

    if ( resultCode != eQCWWAN_ERR_NONE )
    {
        qmrelreqbkp();
        return resultCode; /* No memory */
    }

    /* Prepare and send the LOC Start Req */
    resultCode = SwiQmiSendnWait( pReqBuf,
            eQMI_SVC_SWI_LOC,
            paramLength,
            eQMI_TIMEOUT_5_S, /* 5 Seconds */
            &pInParam,
            &paramLength );

    /* Only parse out the response data if we get a positive return */
    if ( eQCWWAN_ERR_NONE == resultCode )
    {
        memset(&response, 0, sizeof(response));
        memset(resp, 0, sizeof(SwiLocGetAutoStartResp));
        response.resp = resp;
        /* Copy to the caller's buffer */
        resultCode = UpkSwiLocGetAutoStart( pInParam, &response );
    }

    qmrelreqbkp();
    return resultCode;
}

ULONG SwiLocSetAutoStart(SwiLocSetAutoStartReq *req)
{
    ULONG  resultCode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiSwiLocSetAutoStartResp response;

    if ( NULL == req )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();
   
    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message */
    resultCode = PkSwiLocSetAutoStart( &paramLength, pOutParam, req);

    if ( resultCode != eQCWWAN_ERR_NONE )
    {
        qmrelreqbkp();
        return resultCode; /* No memory */
    }

    /* Prepare and send the LOC Start Req */
    resultCode = SwiQmiSendnWait( pReqBuf,
            eQMI_SVC_SWI_LOC,
            paramLength,
            eQMI_TIMEOUT_5_S, /* 5 Seconds */
            &pInParam,
            &paramLength );

    /* Only parse out the response data if we get a positive return */
    if ( eQCWWAN_ERR_NONE == resultCode )
    {
        memset(&response, 0, sizeof(response));
        /* Copy to the caller's buffer */
        resultCode = UpkSwiLocSetAutoStart( pInParam, &response );
    }

    qmrelreqbkp();
    return resultCode;
}


ULONG SLQSLOCInjectPosition(LocInjectPositionReq *pLocInjectPositionReq)
{
    ULONG  resultCode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiLocInjectPositionResp response;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();
   
    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message */
    resultCode = PkQmiLocInjectPosition( &paramLength, pOutParam, pLocInjectPositionReq);

    if ( resultCode != eQCWWAN_ERR_NONE )
    {
        qmrelreqbkp();
        return resultCode; /* No memory */
    }
    /* Prepare and send the LOC Start Req */
    resultCode = SwiQmiSendnWait( pReqBuf,
            eQMI_SVC_LOC,
            paramLength,
            eQMI_TIMEOUT_5_S, /* 5 Seconds */
            &pInParam,
            &paramLength );

    /* Only parse out the response data if we get a positive return */
    if ( eQCWWAN_ERR_NONE == resultCode )
    {
        /* Copy to the caller's buffer */
        resultCode = UpkQmiLocInjectPosition( pInParam, &response );
    }

    qmrelreqbkp();
    return resultCode;
}

ULONG SLQSLOCSetCradleMountConfig(LocSetCradleMountReq *pLocSetCradleMountReq)
{
    ULONG  resultCode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiLocSetCradleMountConfigResp response;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();
    
    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message */
    resultCode = PkQmiLocSetCradleMountConfig( &paramLength, pOutParam, pLocSetCradleMountReq);

    if ( resultCode != eQCWWAN_ERR_NONE )
    {
        qmrelreqbkp();
        return resultCode; /* No memory */
    }

    resultCode = SwiQmiSendnWait( pReqBuf,
            eQMI_SVC_LOC,
            paramLength,
            eQMI_TIMEOUT_5_S, /* 5 Seconds */
            &pInParam,
            &paramLength );

    /* Only parse out the response data if we get a positive return */
    if ( eQCWWAN_ERR_NONE == resultCode )
    {
        /* Copy to the caller's buffer */
        resultCode = UpkQmiLocSetCradleMountConfig( pInParam, &response );
    }

    qmrelreqbkp();
    return resultCode;
}

ULONG SLQSLOCInjectSensorData(LocInjectSensorDataReq *pLocInjectSensorDataReq)
{
    ULONG  resultCode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiLocInjectSensorDataResp response;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();
    
    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message */
    resultCode = PkQmiLocInjectSensorData( &paramLength, pOutParam, pLocInjectSensorDataReq);

    if ( resultCode != eQCWWAN_ERR_NONE )
    {
        qmrelreqbkp();
        return resultCode; /* No memory */
    }

    resultCode = SwiQmiSendnWait( pReqBuf,
            eQMI_SVC_LOC,
            paramLength,
            eQMI_TIMEOUT_5_S, /* 5 Seconds */
            &pInParam,
            &paramLength );

    /* Only parse out the response data if we get a positive return */
    if ( eQCWWAN_ERR_NONE == resultCode )
    {
        /* Copy to the caller's buffer */
        resultCode = UpkQmiLocInjectSensorData( pInParam, &response );
    }

    qmrelreqbkp();
    return resultCode;
}
