/**
 * \if CvsId
 * $Id: SwiApiGsmStk.c,v 1.2 2009/03/25 18:31:10 bsiu Exp $
 * \endif
 * \ingroup stk
 * \file SwiApiGsmStk.c
 *
 * \brief  Source code for GSM APIs related to Sim ToolKit.
 *         See ETSI TS 131 111 V8.4.0 (2009-01)
 *
 * Copyright: © 2009 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */
#include "SwiApiGsmStk.h"
#include "ap/apidefs.h"

/*@{*/


/**
 * This function is used to set up a notification to the host anytime the SIM 
 * issues a Sim Application Toolkit command. The notification will contain all 
 * the necessary information to inform the host of any further action required. 
 * If the Display Text Available flag is set then the host should first get this
 * display text so that the user can be informed of any required information 
 * and then the other information can be fetched using the Set Request 
 * function.
 *
 * \param  STKCommandp: [IN/OUT] STK Command data 
 *
 * \param  timeout: [IN]  
 *         Response timeout value in milliseconds.
 *
 * \return SWI_RCODE_OK:
 *         The request has been processed and values returned are valid.
 *
 * \return See SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_STK_Command
 */
SWI_API SWI_RCODE SwiGetSIMToolKitCommand(
     struct SWI_STRUCT_STK_Command *STKCommandp, 
     swi_uint32 timeout)   
{  
    SWI_RCODE          resultcode;  /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;   /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength;  /* Ret'd length of the CnS Parm field */
    swi_uint8          *outparamp;  /* ptr to outbound param field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Get a pointer to the start of the outbound CnS Parameter field */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* Set outgoing parameters */
    parmlength = SwiPkGsStkCmd( STKCommandp, outparamp );
    
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_STK_CMD, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );
    
    /* Check result of operation */
    if (resultcode == SWI_RCODE_OK)
    {
        SwiUpkGsStkCmd(inparamp, STKCommandp);
    }
    /* All done, send the result to the caller */
    return resultcode;
}      

/**
 * This function is used to send a user acknowledge to the SIM. If the user
 * has acknowledged the displayed message then this object is used to return the
 * OK to the SIM. If the STK command times out, then nothing is returned to 
 * the SIM and the SIM will handle its own timeout.
 * If the user has requested to back out of the current STK command then this 
 * object informs the SIM of this user action. Note that the backwards request
 * will have no affect on the first level of the main menu.
 * If the user has requested to abort the current STK command session then
 * this object informs the SIM of this user action. This command has no effect 
 * on the first level of the main menu.
 * If the user interface is busy when a normal priority display text command 
 * is received then the host must use this object to inform the SIM that 
 * the MMI is busy right now.
 *
 * \param  RespUserAckp: [IN] User return type
 *
 * \param  timeout: [IN]  
 *         Response timeout value in milliseconds.
 *
 * \return SWI_RCODE_OK:
 *         The request has been processed and values returned are valid.
 *
 * \return See SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_STK_RsepUserAck
 * 
 */
SWI_API SWI_RCODE SwiSetSTKUserAcknowledge (
    const struct SWI_STRUCT_STK_RsepUserAck *RespUserAckp, 
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode;  /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;   /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength;  /* Ret'd length of the CnS Parm field */
    swi_uint8          *outparamp;  /* ptr to outbound param field */
    swi_uint8          userreturn;
    
    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Get a pointer to the start of the outbound CnS Parameter field */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* Set outgoing parameters */
    parmlength = OBJ_CNS_STK_RESP_USER_ACK_LENGTH;
    piput32(RespUserAckp->nSTK_Handle, &outparamp);
    userreturn = (swi_uint8)RespUserAckp->eSTKReturn;
    piputmulti (&userreturn,  &outparamp, 1);
    
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_STK_RESP_USER_ACK, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );
    
    /* No response is expected from this set, simply return result code */

    /* All done, send the result to the caller */
    return resultcode;    
}

/**
 * This function is used by the host to return to the SIM the response to 
 * get input command. The returned string is a maximum length of 120 characters
 * coded in UCS2 (ie 240 bytes).In case of timeouts, nothing is sent to
 * the modem, the modem will handle its own timeouts.
 *
 * \param  RespGetInputp: [IN] User input info 
 *
 * \param  timeout: [IN]  
 *         Response timeout value in milliseconds.
 *
 * \return SWI_RCODE_OK:
 *         The request has been processed and values returned are valid.
 *
 * \return See SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_STK_RsepGetInput
 * 
 */
SWI_API SWI_RCODE SwiSetSTKSetInput (
    const struct SWI_STRUCT_STK_RsepGetInput *RespGetInputp, 
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode;  /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;   /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength;  /* Ret'd length of the CnS Parm field */
    swi_uint8          *outparamp;  /* ptr to outbound param field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Get a pointer to the start of the outbound CnS Parameter field */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* Set outgoing parameters */
    parmlength = SwiPkGsStkRespGetInput(RespGetInputp, outparamp);
    
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_STK_RESP_GET_INPUT, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );
    
    /* No response is expected from this set, simply return result code */

    /* All done, send the result to the caller */
    return resultcode;    
}

/**
 * This function is used by the host to return a user confirmation back 
 * to the modem. The host can only return a Yes or No response using 
 * this object. In case of timeouts, nothing is sent to the modem, the modem
 * will handle its own timeouts.
 *
 * \param  RsepUserConfirmp: [IN] User response info 
 *
 * \param  timeout: [IN]  
 *         Response timeout value in milliseconds.
 *
 * \return SWI_RCODE_OK:
 *         The request has been processed and values returned are valid.
 *
 * \return See SWI_RCODE for other error return codes.
 *
 * \sa SwiSetSTKUserConfirmationResponse
 * 
 */
SWI_API SWI_RCODE SwiSetSTKUserConfirmationResponse (
    const struct SWI_STRUCT_STK_RsepUserConfirm *RsepUserConfirmp, 
    swi_uint32 timeout)
{
   SWI_RCODE          resultcode;  /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;   /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength;  /* Ret'd length of the CnS Parm field */
    swi_uint8          *outparamp;  /* ptr to outbound param field */
    swi_uint8          userresp;
    
    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Get a pointer to the start of the outbound CnS Parameter field */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* Set outgoing parameters */

    piput32(RsepUserConfirmp->nSTK_Handle, &outparamp);
    
    userresp = (swi_uint8)RsepUserConfirmp->eUserResponse;
    piputmulti (&userresp,  &outparamp, 1);
 
    parmlength = OBJ_CNS_STK_RESP_USER_CNF_LENGTH;   
    
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_STK_RESP_USER_CNF, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );
    
    /* No response is expected from this set, simply return result code */

    /* All done, send the result to the caller */
    return resultcode;    
}

/**
 * This function is used by the host to select an item in a menu.. If there is
 * no proactive command in progress then it would select an item from the 
 * main menu, otherwise if a Select Item proactive command had been issued, 
 * then it would select an item from that list.
 *
 * \param  RespSelectItemp: [IN] User selected menu item info 
 *
 * \param  timeout: [IN]  
 *         Response timeout value in milliseconds.
 *
 * \return SWI_RCODE_OK:
 *         The request has been processed and values returned are valid.
 *
 * \return See SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_STK_RsepSelectItem
 * 
 */
SWI_API SWI_RCODE SwiSetSTKUserSelectedMenuItem (
    const struct SWI_STRUCT_STK_RsepSelectItem *RespSelectItemp, 
    swi_bool    *Resultp, 
    swi_uint32  timeout)
{
    SWI_RCODE          resultcode;  /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;   /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength;  /* Ret'd length of the CnS Parm field */
    swi_uint8          *outparamp;  /* ptr to outbound param field */
    swi_uint8          menuitem;
    
    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Get a pointer to the start of the outbound CnS Parameter field */
    outparamp = amgetparamp( AMCNSREQUEST, reqbufferp );

    /* Set outgoing parameters */

    piput32(RespSelectItemp->nSTK_Handle, &outparamp);
    
    menuitem = (swi_uint8)RespSelectItemp->nMenuItem;
    piputmulti (&menuitem,  &outparamp, 1);
 
    parmlength = OBJ_CNS_STK_RESP_SELECT_ITEM_LENGTH;   
    
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_STK_RESP_SELECT_ITEM, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );
    

    /* Return result flag to user - note: 0 means OK */
    *Resultp = (*inparamp == 0) ? TRUE : FALSE;         

    /* Check API result code */
    if (*inparamp)
    {
        /* SIM application is busy, menu item cannot be selected at this time */
        resultcode = SWI_RCODE_BUSY;

    }
    /* All done, send the result to the caller */
    return resultcode;    
}

/*@}*/ /* End of SIM Toolkit group */    

/*
 * $Log: SwiApiGsmStk.c,v $
 */

