/*
 * \ingroup dms
 *
 * \file qaGobiApiDms.c
 *
 * \brief  Entry points for Gobi APIs for the Device Management Service ( DMS )
 *
 * Copyright: © 2011 Sierra Wireless, Inc. all rights reserved
 *
 */

#include "SwiDataTypes.h"
#include "qaGobiApiDms.h"
#include "qaGobiApiQos.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "amudefs.h"
#include "sludefs.h"
#include "qaQmiBasic.h"
#include "qaQosGetNWStatus.h"
#include "qaQosReq.h"
#include "qaQosRel.h"
#include "qaQosGetGranted.h"
#include "qaQosGetFlowStatus.h"
#include "qaQosSuspend.h"
#include "qaQosResume.h"
#include "qaQosReset.h"
#include "qaQosGetNWProf.h"

extern BYTE cached_qos_flow[255];

ULONG SLQSQosGetNetworkStatus(
        BYTE    instance,
        BYTE    *pStatus
    )
{
    ULONG               resultcode;     /* Result of SwiQmiSendnWait() */
    BYTE                *pInParam;      /* ptr to param field rx'd from modem */
    BYTE                *pOutParam;     /* ptr to outbound param field */
    BYTE                *pReqBuf;       /* Pointer to outgoing request buffer */
    USHORT              ParamLength;    /* Ret'd length of the  QMI Param field */

    /* Storage for results and response variable */
    struct QmiQosGetNWStatusResp response;

    if (!pStatus)
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message */
    resultcode = PkQmiQosGetNWStatus( &ParamLength,
                                       pOutParam );
    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiMISendnWait( pReqBuf,
                                  eQMI_SVC_QOS,
                                  ParamLength,
                                  eQMI_TIMEOUT_2_S,
                                  &pInParam,
                                  &ParamLength,
                                  instance);
    /* Only parse out the response data if we got a positive return */
    if (resultcode == eQCWWAN_ERR_NONE)
    {
        /* Pass the pointer for the OUT parameters */
        response.pStatus = pStatus;

        /* Copy to the caller's buffer */
        resultcode = UpkQmiQosGetNWStatus(pInParam, &response);
    }

    qmrelreqbkp();
    return resultcode;
}

ULONG SLQSQosReq(
    BYTE        instance,
    swiQosReq   *pQosReq,
    swiQosIds   *pQosIds )
{
    ULONG               resultcode;     /* Result of SwiQmiSendnWait() */
    BYTE                *pInParam;      /* ptr to param field rx'd from modem */
    BYTE                *pOutParam;     /* ptr to outbound param field */
    BYTE                *pReqBuf;       /* Pointer to outgoing request buffer */
    USHORT              ParamLength;    /* Ret'd length of the  QMI Param field */

    if (!pQosReq || !pQosIds)
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Storage for results and response variable */
    struct QmiQosRequestResp response;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message */
    resultcode = PkQmiQosReq( &ParamLength, pOutParam, pQosReq);
    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiMISendnWait( pReqBuf,
                                  eQMI_SVC_QOS,
                                  ParamLength,
                                  eQMI_TIMEOUT_2_S,
                                  &pInParam,
                                  &ParamLength,
                                  instance);

    /* Only parse out the response data if we got a positive return */
    if (resultcode == eQCWWAN_ERR_NONE)
    {
        /* Pass the pointer for the OUT parameters */
        response.pQosIds = pQosIds;

        /* Copy to the caller's buffer */
        resultcode = UpkQmiQosRequest(pInParam, &response);
    }

    qmrelreqbkp();
    return resultcode;
}

ULONG SLQSQosRel(
    BYTE        instance,
    swiQosIds   *pQosIds
    )
{
    //TODO some many code duplication in every API, any nicer way?
    ULONG               resultcode;     /* Result of SwiQmiSendnWait() */
    BYTE                *pInParam;      /* ptr to param field rx'd from modem */
    BYTE                *pOutParam;     /* ptr to outbound param field */
    BYTE                *pReqBuf;       /* Pointer to outgoing request buffer */
    USHORT              ParamLength;    /* Ret'd length of the  QMI Param field */

    struct QmiQosRelResp response;

    if (!pQosIds)
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message */
    resultcode = PkQmiQosRel( &ParamLength, pOutParam, pQosIds);
    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiMISendnWait( pReqBuf,
                                  eQMI_SVC_QOS,
                                  ParamLength,
                                  eQMI_TIMEOUT_2_S,
                                  &pInParam,
                                  &ParamLength,
                                  instance);

    /* Only parse out the response data if we got a positive return */
    if (resultcode == eQCWWAN_ERR_NONE)
    {
        /* Copy to the caller's buffer */
        resultcode = UpkQmiQosRel(pInParam, &response);
    }

    qmrelreqbkp();
    return resultcode;
}

ULONG SLQSQosGetGranted(
    BYTE            instance,
    ULONG           id,
    swiQosGranted   *pGranted )
{
    //TODO some many code duplication in every API, any nicer way?
    ULONG               resultcode;     /* Result of SwiQmiSendnWait() */
    BYTE                *pInParam;      /* ptr to param field rx'd from modem */
    BYTE                *pOutParam;     /* ptr to outbound param field */
    BYTE                *pReqBuf;       /* Pointer to outgoing request buffer */
    USHORT              ParamLength;    /* Ret'd length of the  QMI Param field */

    if (!pGranted)
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Storage for results and response variable */
    struct QmiQosGetGrantedResp response;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message */
    resultcode = PkQmiQosGetGranted( &ParamLength, pOutParam, id);
    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiMISendnWait( pReqBuf,
                                  eQMI_SVC_QOS,
                                  ParamLength,
                                  eQMI_TIMEOUT_2_S,
                                  &pInParam,
                                  &ParamLength,
                                  instance);

#ifdef QOS_SIMULATE
    BYTE response_hack[255];
    memcpy(response_hack, pInParam, ParamLength + 4);   //including message id & length
    //change type from 0x10 to 0x11 to satisfy GRANTED_QOS_RESP
    cached_qos_flow[0] = 0x11;
    //append
    memcpy(&response_hack[ParamLength+4], cached_qos_flow, cached_qos_flow[1] + 3);
    //update original length
    response_hack[2] = ParamLength + cached_qos_flow[1] + 3;

    resultcode = eQCWWAN_ERR_NONE;
#endif

    /* Only parse out the response data if we got a positive return */
    if (resultcode == eQCWWAN_ERR_NONE)
    {
        /* Pass the pointer for the OUT parameters */
        response.pGranted = pGranted;

        /* Copy to the caller's buffer */
#ifdef QOS_SIMULATE
        resultcode = UpkQmiQosGetGranted(response_hack, &response);
        resultcode = eQCWWAN_ERR_NONE;
#else
        resultcode = UpkQmiQosGetGranted(pInParam, &response);
#endif
    }

    qmrelreqbkp();
    return resultcode;
}

ULONG SLQSQosGetFlowStatus(
    BYTE    instance,
    ULONG   id,
    BYTE    *pStatus)
{
    //TODO some many code duplication in every API, any nicer way?
    ULONG               resultcode;     /* Result of SwiQmiSendnWait() */
    BYTE                *pInParam;      /* ptr to param field rx'd from modem */
    BYTE                *pOutParam;     /* ptr to outbound param field */
    BYTE                *pReqBuf;       /* Pointer to outgoing request buffer */
    USHORT              ParamLength;    /* Ret'd length of the  QMI Param field */

    /* Storage for results and response variable */
    struct QmiQosGetFlowStatusResp response;

    if (!pStatus)
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message */
    resultcode = PkQmiQosGetFlowStatus( &ParamLength, pOutParam, id);
    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiMISendnWait( pReqBuf,
                                  eQMI_SVC_QOS,
                                  ParamLength,
                                  eQMI_TIMEOUT_2_S,
                                  &pInParam,
                                  &ParamLength,
                                  instance);

    /* Only parse out the response data if we got a positive return */
    if (resultcode == eQCWWAN_ERR_NONE)
    {
        /* Pass the pointer for the OUT parameters */
        response.pStatus = pStatus;

        /* Copy to the caller's buffer */
        resultcode = UpkQmiQosGetFlowStatus(pInParam, &response);
    }

    qmrelreqbkp();
    return resultcode;
}

ULONG SLQSQosSuspend(
    BYTE        instance,
    swiQosIds   *pQosIds )
{
    ULONG               resultcode;     /* Result of SwiQmiSendnWait() */
    BYTE                *pInParam;      /* ptr to param field rx'd from modem */
    BYTE                *pOutParam;     /* ptr to outbound param field */
    BYTE                *pReqBuf;       /* Pointer to outgoing request buffer */
    USHORT              ParamLength;    /* Ret'd length of the  QMI Param field */

    /* Storage for results and response variable */
    struct QmiQosSuspendResp response;

    if (!pQosIds)
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message */
    resultcode = PkQmiQosSuspend( &ParamLength,
                                       pOutParam , pQosIds);
    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiMISendnWait( pReqBuf,
                                  eQMI_SVC_QOS,
                                  ParamLength,
                                  eQMI_TIMEOUT_2_S,
                                  &pInParam,
                                  &ParamLength,
                                  instance);
    /* Only parse out the response data if we got a positive return */
    if (resultcode == eQCWWAN_ERR_NONE)
    {
        /* Copy to the caller's buffer */
        resultcode = UpkQmiQosSuspend(pInParam, &response);
    }

    qmrelreqbkp();
    return resultcode;
}

ULONG SLQSQosResume(
    BYTE        instance,
    swiQosIds   *pQosIds )
{
    ULONG               resultcode;     /* Result of SwiQmiSendnWait() */
    BYTE                *pInParam;      /* ptr to param field rx'd from modem */
    BYTE                *pOutParam;     /* ptr to outbound param field */
    BYTE                *pReqBuf;       /* Pointer to outgoing request buffer */
    USHORT              ParamLength;    /* Ret'd length of the  QMI Param field */

    /* Storage for results and response variable */
    struct QmiQosResumeResp response;

    if (!pQosIds)
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message */
    resultcode = PkQmiQosResume( &ParamLength,
                                       pOutParam , pQosIds);
    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiMISendnWait( pReqBuf,
                                  eQMI_SVC_QOS,
                                  ParamLength,
                                  eQMI_TIMEOUT_2_S,
                                  &pInParam,
                                  &ParamLength,
                                  instance);
    /* Only parse out the response data if we got a positive return */
    if (resultcode == eQCWWAN_ERR_NONE)
    {
        /* Copy to the caller's buffer */
        resultcode = UpkQmiQosResume(pInParam, &response);
    }

    qmrelreqbkp();
    return resultcode;
}

ULONG SLQSQosReset(
        BYTE        instance
        )
{
    ULONG               resultcode;     /* Result of SwiQmiSendnWait() */
    BYTE                *pInParam;      /* ptr to param field rx'd from modem */
    BYTE                *pOutParam;     /* ptr to outbound param field */
    BYTE                *pReqBuf;       /* Pointer to outgoing request buffer */
    USHORT              ParamLength;    /* Ret'd length of the  QMI Param field */

    /* Storage for results and response variable */
    struct QmiQosResetResp response;

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message */
    resultcode = PkQmiQosReset( &ParamLength, pOutParam);
    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiMISendnWait( pReqBuf,
                                  eQMI_SVC_QOS,
                                  ParamLength,
                                  eQMI_TIMEOUT_2_S,
                                  &pInParam,
                                  &ParamLength,
                                  instance);
    /* Only parse out the response data if we got a positive return */
    if (resultcode == eQCWWAN_ERR_NONE)
    {
        /* Copy to the caller's buffer */
        resultcode = UpkQmiQosReset(pInParam, &response);
    }

    qmrelreqbkp();
    return resultcode;
}

ULONG SLQSQosGetNWProf(
    BYTE        instance,
    BYTE        *pSz,
    NWProfile   *pProfile)
{
    ULONG               resultcode;     /* Result of SwiQmiSendnWait() */
    BYTE                *pInParam;      /* ptr to param field rx'd from modem */
    BYTE                *pOutParam;     /* ptr to outbound param field */
    BYTE                *pReqBuf;       /* Pointer to outgoing request buffer */
    USHORT              ParamLength;    /* Ret'd length of the  QMI Param field */

    /* Storage for results and response variable */
    struct QmiQosGetNWProfResp response;

    if (!pSz || !pProfile)
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs QMI message */
    resultcode = PkQmiQosGetNWProf( &ParamLength, pOutParam);
    if (resultcode != eQCWWAN_ERR_NONE)
    {
        qmrelreqbkp();
        return resultcode;
    }

    /* Prepare and send the blocking call */
    resultcode = SwiQmiMISendnWait( pReqBuf,
                                  eQMI_SVC_QOS,
                                  ParamLength,
                                  eQMI_TIMEOUT_2_S,
                                  &pInParam,
                                  &ParamLength,
                                  instance);
    /* Only parse out the response data if we got a positive return */
    if (resultcode == eQCWWAN_ERR_NONE)
    {
        /* Pass the pointer for the OUT parameters */
        response.pSz = pSz;
        response.pProfile = pProfile;

        /* Copy to the caller's buffer */
        resultcode = UpkQmiQosGetNWProf(pInParam, &response);
    }

    qmrelreqbkp();
    return resultcode;
}
