/*
 * \ingroup ims
 *
 * \file qaGobiApiImsa.c
 *
 * \brief  Entry points for Gobi APIs for the IMS Service ( IMS )
 *
 * Copyright: © 2014 Sierra Wireless, Inc. all rights reserved
 *
 */

#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "amudefs.h"
#include "sludefs.h"
#include "qaQmiBasic.h"
#include "qaGobiApiImsa.h"
#include "qaImsaSLQSIndRegister.h"

ULONG SLQSRegisterIMSAIndication (
    IMSAIndRegisterInfo *pImsaIndRegisterInfo )
{
    ULONG  resultCode;  /* Result code to be returned by this function */
    BYTE   *pInParam;   /* ptr to param field rx'd from modem */
    BYTE   *pOutParam;  /* ptr to outbound param field */
    BYTE   *pReqBuf;    /* Pointer to outgoing request buffer */
    USHORT paramLength; /* Ret'd length of the QMI Param field */

    /* Storage for results and response variable */
    struct QmiImsaSLQSIndRegisterResp response;

    /* structure params has IN/OUT Param, hence validated against NULL */
    if ( !pImsaIndRegisterInfo )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    pReqBuf = qmgetreqbkp();

    /* Get a pointer to the start of the outbound QMI Parameter field */
    pOutParam = amgetparamp( AMTYPEQMIREQUEST, pReqBuf );

    /* Invoke the function which packs the QMI message */
    resultCode = PkQmiImsaSLQSIndRegister( &paramLength,
                                           pOutParam,
                                           pImsaIndRegisterInfo );

    if ( eQCWWAN_ERR_NONE != resultCode )
    {
        qmrelreqbkp();
        return resultCode; /* No Memory */
    }

    resultCode = SwiQmiSendnWait( pReqBuf,
                                  eQMI_SVC_IMSA,
                                  paramLength,
                                  eQMI_TIMEOUT_5_S, /* 5 seconds */
                                  &pInParam,
                                  &paramLength );

    /* Only parse out the response data if we got a positive return */
    if ( eQCWWAN_ERR_NONE == resultCode )
    {
        /* Copy to the caller's buffer */
        resultCode = UpkQmiImsaSLQSIndRegister( pInParam, &response );
    }

    qmrelreqbkp();
    return resultCode;
}

