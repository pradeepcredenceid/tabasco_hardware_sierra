/**
 *    \if CvsId 
 *    $Id: SwiApiGsmSim.c,v 1.12 2010/06/10 00:19:42 epasheva Exp $
 *    \endif
 *    \ingroup gsmsim
 *    \file SwiApiGsmSim.c
 * 
 *    \brief  Contains source code for GSM APIs related to the SIM.
 *
 *    Copyright: © 2008 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */
#include "SwiApiGsmSim.h"
#include "ap/apidefs.h"
#include "sl/sludefs.h"

/**
 *
 * This function is used to enter a SIM requested code.
 * 
 * SIM API functions can not be called until a SWI_NOTIFY_SimsStatusExp
 * notification is received.  This indicates that the SIM is ready to 
 * process user requests.
 *
 * The SIM can request the user to enter any one of the following four codes:  
 * CHV1, CHV2, unblocking CHV1 and unblocking CHV2.  The SIM uses this to 
 * verify the identify of the user.  This function allows the caller to pass 
 * the user entered code to the SIM on the modem.  A request containing the 
 * code is sent to the modem.  The modem will respond to the request to 
 * indicate that the code is being verified by the SIM.  The result of the SIM
 * verification will be reflected in the SWI_NOTIFY_SimStatusExp notification.
 *            
 * If three failures in entering a correct CHV1 or CHV2 occur, this 
 * notification shows that the SIM has become blocked (CHV1) or the CHV2
 * features have become blocked.
 *
 * GSM standards designate 10 as the number of permitted attempts at correctly
 * entering PUK codes. This number is SIM dependent and carriers do not 
 * necessarily adhere to this standard.  If 10 failures in entering the PUK 
 * occur, this notification shows the SIM has become permanently blocked or 
 * the CHV2 features have become permanently blocked.
 *
 * \param      pSim: [IN]    
 *                     SIM requested code.
 *
 * \param      timeout: [IN]        
 *                     Response timeout value in milliseconds
 *
 * \return     SWI_RCODE_OK:
 *                     The request has been processed.  
 *                     The SWI_NOTIFY_SimStatusExp notification indicates 
 *                     whether the designated code matched.  It also shows 
 *                     the number of attempts at entering the code correctly.
 *
 * \sa     See SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_SIM_Verify
 * \sa SWI_NOTIFY_SimStatusExp
 *
 */
SWI_API SWI_RCODE SwiSetSimVerify (
    const struct SWI_STRUCT_SIM_Verify* pSim, 
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *outparamp; /* ptr to outbound param field */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Get a pointer to the start of the outbound CnS Parameter field */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* pack outgoing parameters */
    parmlength = SwiPkGsmSimVerify( outparamp, pSim);

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_SIM_VERIFY_EXP, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );
  
    /* All done, send the result to the caller */
    return resultcode;
}
/**
 *
 * This function is used to enable or disable CHV1.  
 * 
 * SIM API functions can not be called until a SWI_NOTIFY_SimsStatusExp
 * notification is received.  This indicates that the SIM is ready to 
 * process user requests.
 *
 * \param      pSim: [IN]    
 *                     SIM CHV1 lock information.  The caller must provide 
 *                     the password required to enable/disable CHV1.
 *
 * \param      timeout: [IN]        
 *                     Response timeout value in milliseconds
 *
 * \return     SWI_RCODE_OK:
 *                     The request has been processed.  The 
 *                     SWI_NOTIFY_SimStatusExp notification indicates the 
 *                     result of the operation. 
 *
 * \sa     See SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_SIM_EnableLock
 * \sa SWI_NOTIFY_SimStatusExp
 *
 */
SWI_API SWI_RCODE SwiSetSimLock (
    const struct SWI_STRUCT_SIM_EnableLock*  pSim, 
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *outparamp; /* ptr to outbound param field */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Get a pointer to the start of the outbound CnS Parameter field */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* pack outgoing parameters */
    parmlength = SwiPkGsmSimLock( outparamp, pSim);

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_CHV_EN_EXP, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );
  
    /* All done, send the result to the caller */
    return resultcode;
}
/**
 *
 * This function is used to determine whether CHV1 is enabled. 
 *            
 * SIM API functions can not be called until a SWI_NOTIFY_SimsStatusExp
 * notification is received.  This indicates that the SIM is ready to 
 * process user requests.
 *
 * \param      pSim: [OUT]            
 *                     SIM CHV1 lock status
 *
 * \param      timeout: [IN]        
 *                     Response timeout value in milliseconds
 *
 * \return     SWI_RCODE_OK:
 *                     The request has been processed and values returned 
 *                     are valid.
 *
 * \sa     See SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_SIM_LockStatus
 * \sa SWI_NOTIFY_SimsStatusExp
 *
 */
SWI_API SWI_RCODE SwiGetSimLock (
    struct SWI_STRUCT_SIM_LockStatus* pSim, 
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* no outgoing parameters */
    parmlength = 0;

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_CHV_EN_EXP, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUpkGsmSimLock( inparamp, pSim );
    }
    /* All done, send the result to the caller */
    return resultcode;
}
/**
 * This function is used to change CHV1 or CHV2 codes.  
 *
 * SIM API functions can not be called until a SWI_NOTIFY_SimsStatusExp
 * notification is received.  This indicates that the SIM is ready to 
 * process user requests.
 * 
 * The CHV1 code can only be changed when enabled (using SwiSetSimLock). 
 * Emergency numbers cannot be used as CHV1 codes, or as the beginning of 
 * CHV1 codes.  PUK1 and PUK2 are not valid parameters, although the data 
 * structure includes them.
 *
 * \param      pSim: [IN]            
 *                     SIM password information
 *
 * \param      timeout: [IN]        
 *                     Response timeout value in milliseconds
 *
 * \return     SWI_RCODE_OK:
 *                     The request has been processed.  The 
 *                     SWI_NOTIFY_SimStatusExp notification indicates the 
 *                     result of the operation. 
 *
 * \sa     See SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_SIM_Password
 * \sa SWI_NOTIFY_SimStatusExp
 *
 */
SWI_API SWI_RCODE SwiSetSimPassword (
    const struct SWI_STRUCT_SIM_Password* pSim, 
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *outparamp; /* ptr to outbound param field */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Get a pointer to the start of the outbound CnS Parameter field */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* pack outgoing parameters */
    parmlength = SwiPkGsmSimPassword( outparamp, pSim);

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_CHV_CHG_EXP, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );
   
    /* All done, send the result to the caller */
    return resultcode;
}
/**
 *
 * This function is used to get CHV2 status from the modem.  
 *
 * SIM API functions can not be called until a SWI_NOTIFY_SimsStatusExp
 * notification is received.  This indicates that the SIM is ready to 
 * process user requests.
 * 
 * The kick type determines if the user is prompted to enter the CHV2.  For
 * example, if the card is kicked for CHV2 verify, then 
 * SWI_NOTIFY_SimsStatusExp will contain a user prompt to enter CHV2.  If the
 * card is kicked for CHV2 change, SWI_NOTIFY_SimsStatusExp will not contain
 * user prompts.
 *
 * \param      eKickType: [IN]    
 *                     CHV2 kick type
 *
 * \param      timeout: [IN]    
 *                     Response timeout value in milliseconds
 *
 * \return     SWI_RCODE_OK:
 *                     The request has been processed.  The 
 *                     SWI_NOTIFY_SimStatusExp notification indicates the 
 *                     result of the operation and any additional actions 
 *                     required by the user. 
 *
 * \sa     See SWI_RCODE for other error return codes.
 *
 * \sa SWI_TYPE_SIM_Chv2KickType
 * \sa SWI_NOTIFY_SimStatusExp
 *
 */
SWI_API SWI_RCODE SwiChv2StatusKick(
    enum SWI_TYPE_SIM_Chv2KickType eKickType, 
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *outparamp; /* ptr to outbound param field */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Get a pointer to the start of the outbound CnS Parameter field */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* pack outgoing parameters */
    parmlength = SwiPkChv2StatusKick( outparamp, eKickType);

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_CHV2_STATUS_KICK, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );
                     
    /* All done, send the result to the caller */
    return resultcode;
}
/**
 *
 * This function is used to enter the MEP code and disable the mechanism 
 * that restricts which SIMs can be used with the modem.  
 * 
 * SIM API functions can not be called until a SWI_NOTIFY_SimsStatusExp
 * notification is received.  This indicates that the SIM is ready to 
 * process user requests.
 *
 * Once this function is called do not call it again until the result from the
 * current request has been received in the next SWI_NOTIFY_SimStatusExp 
 * notification.  
 *
 * The first time that this function is called, there is a 30 second delay 
 * before the SWI_NOTIFY_SimStatusExp notification responds. This delay is 
 * doubled with each attempt. (That is, the second time the function is
 * called, there is a one minute delay. The third time it is called, there 
 * is a two minute delay.) The delay is intended to discourage users from 
 * trying new codes until the SIM becomes blocked.
 *
 * \param      pMEP: [IN]    
 *                     MEP unlocking code
 *
 * \param      timeout: [IN]        
 *                     Response timeout value in milliseconds
 *
 * \return     SWI_RCODE_OK:
 *                     The request has been processed.  The 
 *                     SWI_NOTIFY_SimStatusExp notification indicates the 
 *                     result of the operation. 
 *
 * \sa     SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_MEP_Unlock
 * \sa SWI_NOTIFY_SimStatusExp
 *
 */
SWI_API SWI_RCODE SwiSetMEPUnlock (
    const struct SWI_STRUCT_MEP_Unlock* pMEP, 
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *outparamp; /* ptr to outbound param field */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Get a pointer to the start of the outbound CnS Parameter field */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* pack outgoing parameters */
    parmlength = pMEP->sizeStruct;

     /* Pack outgoing parameter field */
    piputmulti ((const swi_uint8 *)&(pMEP->szUnLockCode[0]), &outparamp,
                pMEP->sizeStruct);

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_MEP_UNLOCK, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        /* Nothing to do, we're SETTING, and there's no notification */
        /* Check results using AT+CLCK="PN",2 command                */
    }
    /* All done, send the result to the caller */
    return resultcode;
}
/**
 *
 * This function is used to retrieve the IMSI from the SIM.  
 * 
 * The IMSI is comprised of three elements: the MSIN, the MCC and the MNC. 
 *            
 * \param      szMSIN: [OUT]        
 *                     MSIN (NULL terminated) 
 *
 * \param      sizeBuffer: [IN]        
 *                     The size of the buffer provided for the MSIN.
 *
 * \param      pMCC: [OUT]            
 *                     Mobile country code
 *
 * \param      pMNC: [OUT]            
 *                     Mobile network code
 *
 * \param      timeout: [IN]        
 *                     Response timeout value in milliseconds.
 *
 * \return     SWI_RCODE_OK:
 *                     The request has been processed and values returned 
 *                     are valid.
 *
 * \sa     See SWI_RCODE for other error return codes.
 *
 */
SWI_API SWI_RCODE SwiGetGsmIMSI(
    swi_charp szMSIN, 
    swi_uint32 sizeBuffer, 
    swi_uint32 *pMCC, 
    swi_uint32 *pMNC, 
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */

    /* Check that a buffer has been given */
    if((szMSIN == NULL) ||
       (sizeBuffer < 11))
    {
        return(SWI_RCODE_SMALL_BUF);
    }
    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* no outgoing parameters */
    parmlength = 0;

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_IMSI, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUpkGsmIMSI( inparamp, szMSIN, pMCC, pMNC );
    }
    /* All done, send the result to the caller */
    return resultcode;
}
/**
 *
 * This function is used to retrieve the ICC ID of the SIM.
 *    
 * This function should not be called until the SWI_NOTIFY_SimStatusExp
 * has been received indicating the SIM is ready. 
 * 
 * \param      sIddId: [OUT]            
 *                     ICC ID
 *
 * \param      timeout: [IN]    
 *                     Response timeout value in milliseconds
 *
 * \return     SWI_RCODE_OK:
 *                     The request has been processed and values returned 
 *                     are valid.
 *
 * \sa    See SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_IccId
 * \sa SWI_NOTIFY_SimStatusExp
 *
 */
SWI_API SWI_RCODE SwiGetIccId(
    struct SWI_STRUCT_IccId *sIccId, 
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* no outgoing parameters */
    parmlength = 0;

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_ICC_ID, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );

        /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUpkIccId( inparamp, sIccId );
    }
    /* All done, send the result to the caller */
    return resultcode;
}
/**
 *
 * This function reports the current service provider's identification code.  
 *
 * The provider ID for GSM is a string provided by the carrier on the SIM.
 *            
 * \param      szName: [OUT]    
 *                     Network provider name (NULL terminated)
 *
 * \param      sizeBuffer: [IN]    
 *                     Size of buffer provided for the network provider name
 *
 * \param      timeout: [IN]            
 *                     Response timeout value in milliseconds
 *
 * \sa     See SWI_RCODE for additional information.
 *
 */
SWI_API SWI_RCODE SwiGetNetworkProviderID(
    swi_charp szName, 
    swi_uint32 sizeBuffer, 
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */
    
    /* Check that a buffer has been given */
    if((szName == NULL) ||
       (sizeBuffer == 0))
    {
        return(SWI_RCODE_SMALL_BUF);
    }

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* no outgoing parameters */
    parmlength = 0;

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_SERVICE_PROVIDER, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );
   
    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUpkSpn( inparamp, szName, sizeBuffer - 1 );
    }
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function is used to send a sixteen byte random number to the SIM.
 * The SIM will run its algorithm on the provided random number and it will
 * output a thirty two bit SIM response and eight byte ciphering key. 
 * 
 * SIM API functions can not be called until a SWI_NOTIFY_SimsStatusExp
 * notification is received.  This indicates that the SIM is ready to 
 * process user requests.
 *            
 * \param    pSimAuth: [IN/OUT]         
 *            SIM authentication information.  The size of the structure and
 *            random number must be provided by the caller.  The response
 *            will contain the 32-bit SIM response and ciphering key.
 *
 * \param    timeout: [IN]    
 *            Response timeout value in milliseconds
 *
 * \return    SWI_RCODE_OK:
 *            The request has been processed and values returned are valid.
 *
 * \sa    See SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_SimAuth
 * \sa SWI_NOTIFY_SimsStatusExp
 *
 */
SWI_API SWI_RCODE SwiSetSimAuthentication (
    struct SWI_STRUCT_SimAuth  *pSimAuth, 
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *outparamp; /* ptr to outbound param field */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Get a pointer to the start of the outbound CnS Parameter field */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* pack outgoing parameters */
    parmlength = LENGTH_RandNum;

    /* Pack outgoing parameter field */
    piputmulti ( &(pSimAuth->RandNum[0]), &outparamp, LENGTH_RandNum );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_SIM_AUTH, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );
 
    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUpkGsSimAuth( inparamp, pSimAuth );
    }
    /* All done, send the result to the caller */
    return resultcode;
}

/* 
 * $Log: SwiApiGsmSim.c,v $
 */

