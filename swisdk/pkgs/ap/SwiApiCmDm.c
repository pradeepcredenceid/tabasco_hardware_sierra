/**
 *    \if CvsId 
 *    $Id: SwiApiCmDm.c,v 1.4 2010/06/10 00:14:35 epasheva Exp $
 *    \endif
 *    \ingroup cmdm
 *    \file SwiApiCmDm.c
 * 
 *    \brief SWI OMA-DM related APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */

#include "ap/apidefs.h"
#include "SwiApiCmDm.h"

/**
 *
 * This function gets the current OMA-DM configuration supported by the device.
 *
 * Notification: SWI_NOTIFY_OMADM_Config returns the same data.
 *
 * \param psConfig[OUT]    Struct containing configuration params. 
 *                         See SWI_STRUCT_OMADM_GET_CONFIG.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetOMADMConfig(SWI_STRUCT_OMADM_GET_CONFIG *psConfig, 
                                    swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         paramlength; /* Returned length of the CnS parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Normally a Pack function call would go here, but this 
     * request has no CnS Parameter field to pack
     */

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_DM_CONFIG, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &paramlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUnpackOMADMConfig( inparamp, psConfig );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function sets the current OMA-DM configuration in the device.
 *
 * Attempting to reset HFA status requires modem to be MSL unlocked.
 *
 * \param psConfig[IN]     Struct containing configuration params.
 *                         See SWI_STRUCT_OMADM_SET_CONFIG.
 * \param pStatus[OUT]     Contains OMADM error status if return code is 
 *                         SWI_RCODE_OMADM_ERROR.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  SWI_RCODE_OMADM_ERROR: The OMADM service returned an error. 
 * \return  Also see SwiRcodes.h.
 *
 */
SWI_API SWI_RCODE SwiSetOMADMConfig(
        const SWI_STRUCT_OMADM_SET_CONFIG *psConfig, 
        swi_uint8 *pStatus, 
        swi_uint32 timeout)
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
    parmlength = SwiPackOMADMConfig( outparamp, psConfig );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_DM_CONFIG, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        swi_uint8 nResult;
        
        piget16(&inparamp); /* Discard obj_ver */
        nResult = *inparamp;
        if(0 != nResult)
        {
            resultcode = SWI_RCODE_OMADM_ERROR;
        }
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function requests the device to start a OMA-DM session.
 *
 * \param psSession[IN]    Struct containing sesssion start params.
 *                         See SWI_STRUCT_OMADM_START_SESSION.
 * \param pStatus[OUT]     Contains OMADM error status if returning code is 
 *                         SWI_RCODE_OMADM_ERROR.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiStartOMADMSession(
        const SWI_STRUCT_OMADM_START_SESSION *psSession, 
        swi_uint8 *pStatus, 
        swi_uint32 timeout)
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
    parmlength = SwiPackStartOMADMSession( outparamp, psSession );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_DM_SESSION_START, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        piget16(&inparamp); /* Discard obj_ver */
        piget16(&inparamp); /* Discard nReserved */
        *pStatus = *inparamp;
        if(0 != *pStatus)
        {
            resultcode = SWI_RCODE_OMADM_ERROR;
        }
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function requests the device to cancel a OMA-DM session.
 *
 * \param pStatus[OUT]     Contains OMADM cancel status information.
 *                         See SWI_STRUCT_OMADM_CANCEL_RESP.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiCancelOMADMSession(SWI_STRUCT_OMADM_CANCEL_RESP *pStatus, 
                                        swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_DM_SESSION_CANCEL, 
                                  0, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        piget16(&inparamp); /* Discard obj_ver */

        pStatus->nStatus = *inparamp++;
        pStatus->bQueuedForReattempt = *inparamp;
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function gets the current OMA-DM session state information.
 *
 * Notification: SWI_NOTIFY_OMADM_State returns the same data.
 *
 * \param psState[OUT]     Contains OMA-DM session state information.
 *                         See SWI_STRUCT_OMADM_SESSION_STATE.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetOMADMSessionState(
        SWI_STRUCT_OMADM_SESSION_STATE *psState, 
        swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         paramlength; /* Returned length of the CnS parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Normally a Pack function call would go here, but this 
     * request has no CnS Parameter field to pack
     */

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_DM_SESSION_STATE, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &paramlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUnpackOMADMSessionState( inparamp, psState );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function gets OMA-DM Network Initiated Alert information stored.
 *
 * \param psAlert[OUT]     Contains OMA-DM NI Alert information.
 *                         See SWI_STRUCT_OMADM_NI_ALERT.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetOMADMNIAlert(SWI_STRUCT_OMADM_NI_ALERT *psAlert, 
                                     swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         paramlength; /* Returned length of the CnS parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Normally a Pack function call would go here, but this 
     * request has no CnS Parameter field to pack
     */

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_DM_NI_ALERT, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &paramlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUnpackOMADMNIAlert( inparamp, psAlert );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function sends a response for the NI Alert to modem.
 *
 * \param nResponse[IN]    0 = Deny; 1 = Allow.
 * \param psStatus[OUT]    Contains OMADM response status.
 *                         See SWI_STRUCT_OMADM_NI_ALERT_RESP.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetOMADMNIAlertResponse(
        swi_uint16 nResponse, 
        SWI_STRUCT_OMADM_NI_ALERT_RESP *psStatus, 
        swi_uint32 timeout)
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
    parmlength = SwiPackOMADMNIAlertResponse( outparamp, nResponse );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_DM_NI_ALERT, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        piget16(&inparamp);     /* Discard obj_ver */
        psStatus->nStatus = *inparamp++;
        psStatus->bQueuedForReattempt = *inparamp;
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function is not implemented in EVDO SWI and SDK7 SWI
 *
 * \param psConfirm[IN]    This is a parameter.
 * \param pConfResp[IN]    This is a parameter.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetOMADMUIAlertConfirm(
        const SWI_STRUCT_OMADM_UI_ALERT_CONFIRM_RESP *psConfirm, 
        swi_uint16 *pConfResp, 
        swi_uint32 timeout)
{
    return SWI_RCODE_NOT_SUP;
}

/**
 *
 * This function is not implemented in EVDO SWI and SDK7 SWI
 *
 * \param psInput[IN]      This is a parameter.
 * \param pInputResp[IN]   This is a parameter.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetOMADMUIAlertInput(
        const SWI_STRUCT_OMADM_UI_ALERT_INPUT_RESP *psInput, 
        swi_uint16 *pInputResp, 
        swi_uint32 timeout)
{
    return SWI_RCODE_NOT_SUP;
}

/**
 *
 * This function is not implemented in EVDO SWI and SDK7 SWI
 *
 * \param psChoice[IN]     This is a parameter.
 * \param pConfResp[IN]    This is a parameter.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetOMADMUIAlertChoice(
        const SWI_STRUCT_OMADM_UI_ALERT_CHOICE_RESP *psChoice, 
        swi_uint16 *pConfResp, 
        swi_uint32 timeout)
{
    return SWI_RCODE_NOT_SUP;
}

/**
 *
 * This function commands the modem to update any per unit DM session 
 * configuration values in the device's management tree.
 *
 * \param pStatus[OUT]     Result of operation.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  SWI_RCODE_OMADM_ERROR: The OMADM service returned an error.
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetOMADMBootAlgorithm(swi_uint16 *pStatus, 
                                           swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* A(start of outgoing CnS buffer) */
    swi_uint16         parmlength; /* Length of the CnS Parameter field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_DM_BOOT_ALGORITHM, 
                                  0, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        piget16(&inparamp);     /* Discard obj_ver */
        *pStatus = *inparamp;
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function gets a DM tree node value.
 *
 * \param psItem[IN]       Node value to retrieve.
 *                         See SWI_STRUCT_OMADM_NODE_VALUE_REQUEST.
 * \param psValue[Out]     Node value data.
 *                         See SWI_STRUCT_OMADM_NODE_VALUE_RESPONSE.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  SWI_RCODE_OMADM_ERROR: The OMADM service returned an error.
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetOMADMNodeValue(
        const SWI_STRUCT_OMADM_NODE_VALUE_REQUEST *psItem, 
        SWI_STRUCT_OMADM_NODE_VALUE_RESPONSE *psValue, 
        swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *outparamp; /* For building outbound param field */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         paramlength; /* Returned length of the CnS parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Parameter Field Construction Zone - get a pointer to the 
     * location to fill in the CnS parameter field 
     */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* Pack the CnS Parameter field with the input parameters */
    paramlength = SwiPackGetOMADMNodeValue( outparamp, psItem );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_DM_NODE_VALUE, 
                                  paramlength,
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &paramlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUnpackOMADMNodeValue( inparamp, psValue );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function sets a DM tree node value.
 *
 * \param psValue[IN]      Node value item and data.
 *                         See SWI_STRUCT_OMADM_NODE_VALUE.
 * \param pStatus[OUT]     Status of operation.
 *                         See SWI_STRUCT_OMADM_NODE_VALUE_RESPONSE.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetOMADMNodeValue(
        const SWI_STRUCT_OMADM_NODE_VALUE *psValue, 
        swi_uint8 *pStatus, 
        swi_uint32 timeout)
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
    parmlength = SwiPackSetOMADMNodeValue( outparamp, psValue );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_DM_NODE_VALUE, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        piget16(&inparamp);     /* Discard obj_ver */
        piget16(&inparamp);     /* Discard Reserved field */
        *pStatus = *inparamp;
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function sets a DM Download Alert Response for a SWI_NOTIFY_DM_DL_Alert.
 *
 * This alert notification is sent from the modem when a DM DL package is 
 * ready for download and/or install.
 * 
 * \param nResponse[IN]    User response to alert.
 *                         See SWI_TYPE_DM_DL_ALERT_RESP
 * \param pStatus[OUT]     Status of response from modem:
 *                           0:Success
 *                           1:Fail - unspecified
 *                           2:Fail - invalid response
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  SWI_RCODE_OMADM_ERROR: The OMADM service returned an error.
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiSetDMDLAlertResponse(swi_uint8 nResponse, 
                                          swi_uint8* pStatus, 
                                          swi_uint32 timeout)
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
    parmlength = SwiPackDMDLAlertResponse( outparamp, nResponse );

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_DM_DL_ALERT, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &parmlength ); /* Re-used for OUT arg */

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        piget16(&inparamp);     /* Discard obj_ver */
        *pStatus = *inparamp;
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function gets the progress of the DM Download.
 *
 * This same information is provided in the SWI_NOTIFY_DM_DL_Progress 
 * notification when a download is in progress.
 *
 * \param psProgress[IN]   Contains progress info.
 *                         See SWI_STRUCT_DM_DL_PROGRESS.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetDMDLProgress(SWI_STRUCT_DM_DL_PROGRESS *psProgress, 
                                     swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         paramlength; /* Returned length of the CnS parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_DM_DL_PROGRESS, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &paramlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUnpackDMDLProgress( inparamp, psProgress );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function gets the the DM Download description text if available.
 *
 * When the SWI_NOTIFY_DM_DL_Alert notification is received, the nDescAvail 
 * parameter will be set if a description is available.
 *
 * \param psDescription[OUT] Description.
 *                           see SWI_STRUCT_DM_DL_DESCRIPTION
 * \param timeout[IN]        Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetDMDLDescription(
        SWI_STRUCT_DM_DL_DESCRIPTION *psDescription, 
        swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         paramlength; /* Returned length of the CnS parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_DM_DL_DESCRIPTION, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &paramlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        resultcode = SwiUnpackDMDLDescription( inparamp, psDescription );
    }
    
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function gets the status of the DM Download install.
 *
 * When the install begins, the modem will send a 
 * SWI_NOTIFY_DM_DL_InstallStatus notification and the modem will de-enumerate 
 * from the system (Device Removal) and complete the install. Once the install 
 * is complete, the modem will re-enumerate on the system (Device Arrival) and 
 * this API should be call (after normal API re-init) to determine the status 
 * of the install.
 *
 * \param psStatus[OUT]    Status of install.
 *                         See SWI_STRUCT_DM_DL_INSTALL_STATUS.
 * \param timeout[IN]      Function timeout in milliseconds.
 *
 * \return  See SwiRcodes.h
 *
 */
SWI_API SWI_RCODE SwiGetDMDLInstallStatus(
        SWI_STRUCT_DM_DL_INSTALL_STATUS *psStatus, 
        swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of call to cnSendWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         paramlength; /* Returned length of the CnS parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_DM_DL_INSTALL_STATUS, 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &paramlength );

    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUnpackDMDLInstallStatus( inparamp, psStatus );
    }

    /* All done, send the result to the caller */
    return resultcode;
}

/* 
 * $Log: SwiApiCmDm.c,v $
 */

