/**
 *    \if CvsId 
 *    $Id: SwiApiCdmaRuim.c,v 1.3 2009/02/10 01:40:07 bsiu Exp $
 *    \endif
 *    \ingroup cdmaruim
 *    \file SwiApiCdmaRuim.c
 * 
 *    \brief SWI CDMA specific Ruim related APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */

#include "SwiApiCdmaRuim.h"
#include "SwiIntPkCdmaRuim.h"
#include "SwiIntUpkCdmaRuim.h"
#include "ap/apidefs.h"

/**
 *
 * This function retrieves the current RUIM Status.
 *
 * \param sRuimStatus[OUT]   Data Structure to contain the RUIM status.
 * \param timeout[IN]        Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */

SWI_API SWI_RCODE SwiGetRUIMStatus(SWI_STRUCT_RuimStatus* sRuimStatus,
                                   swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         paramlength; /* Returned length of the CnS parm field*/

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Normally a Pack function call would go here, but this 
     * request has no CnS Parameter field to pack
     */

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_RUIM_STATUS, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &paramlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUnpackRUIMStatus( inparamp, sRuimStatus );
    }

    /* All done, send the result to the caller */
    return resultcode;
}
/**
 *
 * This function request the modem to verify the passed in CHV code.
 *
 * \param Chv[IN]          Type of CHV.
 * \param szCHV[IN]        CHV code.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiVerifyCHV(swi_uint16 Chv, const swi_charp szCHV,
                               swi_uint32 timeout)
{
    return SwiIntSetU16sU8LenString(CNS_RUIM_VERIFY_CHV, &Chv, 1, szCHV,
                                    LENGTH_Chv, 0, SWI_AMCDMA, timeout);
}

/**
 *
 * This function enables  or disable CHV Security.
 *
 * \param bEnable[IN]      TRUE:Enable or FALSE:disable
 * \param szCHV[IN]        CHV code
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiEnableCHV(swi_bool bEnable, const swi_charp szCHV,
                               swi_uint32 timeout)
{
    swi_uint16 nValue;

    nValue = (swi_uint16)bEnable;
    
    return SwiIntSetU16sU8LenString(CNS_RUIM_ENDIS_CHV, &nValue, 1, szCHV,
                                    LENGTH_Chv, 0, SWI_AMCDMA, timeout);
}

/**
 *
 * This function changes the CHV code
 *
 * \param Chv[IN]          Type of CHV
 * \param szOldCHV         Old CHV code
 * \param szNewCHV         New CHV code
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiChangeCHV(swi_uint16 Chv, const swi_charp szOldCHV, 
                               const swi_charp szNewCHV, swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Parameter Field Construction Zone - get a pointer to the 
     * location to fill in the CnS parameter field 
     */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* Pack the CnS Parameter field with the input parameters */
    parmlength = SwiPackChangeCHV( outparamp, Chv, szOldCHV, szNewCHV );
    
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_RUIM_CHANGE_CHV, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* All done, send the result to the caller */
    return resultcode;
}
/**
 *
 * This function unblocks the CHV code
 *
 * \param Chv[IN]          Type of CHV
 * \param szUnblockCHV     Unblocking code
 * \param szCHV[IN]        New CHV code to use 
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiUnblockCHV(swi_uint16 Chv, const swi_charp szUnblockCHV, 
                                const swi_charp szCHV, swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */

    if (slstrlen (szUnblockCHV) > LENGTH_Chv || slstrlen(szCHV) > LENGTH_Chv)
    {
        return SWI_RCODE_INVALID_PAR;
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Parameter Field Construction Zone - get a pointer to the 
     * location to fill in the CnS parameter field 
     */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* Pack the CnS Parameter field with the input parameters */
    parmlength = SwiPackUnblockCHV( outparamp, Chv, szUnblockCHV, szCHV );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_RUIM_UNBLOCK_CHV, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCDMA, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* All done, send the result to the caller */
    return resultcode;
}
/**
 *
 * This function retrieves CHV enabled status
 *
 * \param bEnable[OUT]     TRUE:CHV Enabled; FALSE: CHV disabled.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetCHVEnableStatus(swi_bool* bEnable, swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint16 nValue;

    resultcode = SwiIntGetU16(CNS_RUIM_ENDIS_CHV, &nValue, SWI_AMCDMA,
                              timeout);
    if( SWI_RCODE_OK == resultcode )
    {
        *bEnable = nValue;
    }

    return resultcode;
}
/* 
 * $Log: SwiApiCdmaRuim.c,v $
 */

