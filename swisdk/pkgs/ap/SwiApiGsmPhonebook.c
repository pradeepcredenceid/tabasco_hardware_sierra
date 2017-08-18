/**
 * \if CvsId
 * $Id: SwiApiGsmPhonebook.c,v 1.2 2009/03/25 17:56:38 bsiu Exp $
 * \endif
 * \ingroup gsmvoice
 * \file SwiApiGsmPhonebook.c
 *
 * \brief  Source code for GSM APIs related to Phonebook Maintenance.
 *
 * Copyright: © 2009 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */
#include "SwiApiGsmPhonebook.h"
#include "ap/apidefs.h"

/*@{*/

/**
 *
 * Use this function to clear the entire last numbers dialed, last numbers 
 * missed or last numbers received phonebooks.  
 * 
 * This function should not be called until the SWI_NOTIFY_PhonebookReady
 * has been received indicating phonebooks are ready. 
 *         
 * \param  ePhoneBookType: [IN]
 *         Phonebook to be cleared
 *
 * \param  timeout:  [IN] 
 *         Response timeout value in milliseconds
 *
 * \return SWI_RCODE_OK:
 *         The request has been processed and values returned are valid.
 *
 * \return See SWI_RCODE for other error return codes.
 *
 * \sa SWI_TYPE_Phonebook
 * \sa SWI_NOTIFY_PhonebookReady
 * \sa SwiGetPhonebookEntry
 * \sa SwiAddPhonebookEntry
 * \sa SwiEditPhonebookEntry
 * \sa SwiDeletePhonebookEntry
 *
 */
SWI_API SWI_RCODE SwiDeletePhonebookLastNum(
    const enum SWI_TYPE_Phonebook ePhoneBookType, 
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
    parmlength = 1;
    *outparamp = (swi_uint8) ePhoneBookType;
            
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_LAST_NUM_DEL, 
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
 * This function is used to retrieve a phonebook entry.
 *
 * This function should not be called until the SWI_NOTIFY_PhonebookReady
 * has been received indicating phonebooks are ready. 
 * 
 * There are several different phonebooks on the modem and SIM.  This function
 * can request either the first or next entry be read from a particular 
 * phonebook.  To get the entire contents of a phonebook call this function 
 * requesting the first entry and then repeatedly request the next entry for 
 * all subsequent function calls.    
 *         
 * \param  Phonebookp: [IN/OUT] 
 *         Phonebook entry information.  The structure size, phonebook type,
 *         and bReadFromStart flag must be populated by the caller.  The other
 *         fields are returned from the modem.  Note that you can not read
 *         a phonebook entry directly by specifying the entry ID.
 *
 * \param  timeout:  [IN]  
 *         Response timeout value in milliseconds
 *
 * \return SWI_RCODE_OK:
 *         The request has been processed and values returned are valid.
 *         See Phonebookp->eResultCode for result of the operation.
 *
 * \return See SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_PHONEBK_Entry
 * \sa SWI_NOTIFY_PhonebookReady
 * \sa SwiAddPhonebookEntry
 * \sa SwiEditPhonebookEntry
 * \sa SwiDeletePhonebookEntry
 * \sa SwiDeletePhonebookLastNum
 */
SWI_API SWI_RCODE SwiGetPhonebookEntryExp(
    struct SWI_STRUCT_PHONEBK_Entry* sPhonebookp,
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
    parmlength = 2;
    *outparamp++ = (swi_uint8) sPhonebookp->ePhoneBookType;
    *outparamp++ = (swi_uint8) sPhonebookp->bReadFromStart;
            
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_PHONE_NUM_READ_EXP, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );
    
   /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUpkGsPhbkGetEntry( inparamp, sPhonebookp );
    }

    /* All done, send the result to the caller */
    return resultcode;
    
}

/**
 *
 * Use this function to add a phonebook entry.
 *
 * This function should not be called until the SWI_NOTIFY_PhonebookReady
 * has been received indicating phonebooks are ready. 
 * 
 * \param  Phonebookp: [IN/OUT] 
 *         Phonebook entry to be added.  The structure size, phonebook type,
 *         name, and number must be supplied by the caller.  The phonebook 
 *         entry ID is returned in the same structure and should be stored
 *         to be used to for modify or deleting the entry later.  
 *
 * \param  timeout: [IN]   
 *         Response timeout value in milliseconds.
 *
 * \return SWI_RCODE_OK:
 *         The request has been processed and values returned are valid.
 *         See Phonebookp->eResultCode for result of the operation.
 *
 * \return See SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_PHONEBK_Entry
 * \sa SWI_NOTIFY_PhonebookReady
 * \sa SwiGetPhonebookEntry
 * \sa SwiEditPhonebookEntry
 * \sa SwiDeletePhonebookEntry
 * \sa SwiDeletePhonebookLastNum
 *
 */
SWI_API SWI_RCODE SwiAddPhonebookEntryExp(
    struct SWI_STRUCT_PHONEBK_Entry* sPhonebookp,
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
    parmlength = SwiPkGsPhbkAdd( sPhonebookp, outparamp ); 
                
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_PHONE_NUM_ADD_EXP, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );
    
    /* Get index of addition, and check result code of add operation */
    sPhonebookp->nID = *inparamp++;
    sPhonebookp->eResultCode = *inparamp++;
    if (sPhonebookp->eResultCode != SWI_PHONEBOOKRESULT_Success)
    {
        resultcode = SWI_RCODE_FAILED;
    }
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function is used to modify an existing phonebook entry.
 *
 * This function should not be called until the SWI_NOTIFY_PhonebookReady
 * has been received indicating phonebooks are ready.  
 *
 * The phonebook entry must be known ahead of time (either returned from 
 * SwiAddPhonebookEntry or SwiGetPhonebookEntry).
 *
 * \param  Phonebookp: [IN/OUT] 
 *         Replacement phonebook entry.  The structure size, phonebook entry,
 *         phonebook type, name, and number must be supplied by the caller.
 *
 * \param  timeout: [IN]  
 *         Response timeout value in milliseconds.
 *
 * \return SWI_RCODE_OK:
 *         The request has been processed and values returned are valid.
 *         See Phonebookp->eResultCode for result of the operation.
 *
 * \return See SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_PHONEBK_Entry
 * \sa SWI_NOTIFY_PhonebookReady
 * \sa SwiGetPhonebookEntry
 * \sa SwiAddPhonebookEntry
 * \sa SwiDeletePhonebookEntry
 * \sa SwiDeletePhonebookLastNum
 */
SWI_API SWI_RCODE SwiEditPhonebookEntryExp(
    struct SWI_STRUCT_PHONEBK_Entry* Phonebookp,
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
    parmlength = SwiPkGsPhbkEdit( Phonebookp, outparamp );
            
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_PHONE_NUM_EDIT_EXP, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );
    
    /* Check result of operation */
    Phonebookp->eResultCode = *inparamp;
    if (Phonebookp->eResultCode != SWI_PHONEBOOKRESULT_Success)
    {
        resultcode = SWI_RCODE_FAILED;
    }
    /* All done, send the result to the caller */
    return resultcode;
}      

/**
 *
 * This function is used to determine what phonebooks are available.
 *
 * This function should only be called after a SWI_NOTIFY_PhonebookReady 
 * notification has been received indicating phonebooks are ready.
 *         
 * \param  Phonebookp: [OUT] 
 *         Phonebook availability status
 *
 * \param  timeout: [IN]   
 *         Response timeout value in milliseconds
 *
 * \return SWI_RCODE_OK:
 *         The request has been processed and values returned are valid.
 *
 * \return See SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_PHONEBK_Available
 * \sa SWI_NOTIFY_PhonebookReady
 *
 */
SWI_API SWI_RCODE SwiGetPhonebookAvailable(
    struct SWI_STRUCT_PHONEBK_Available* Phonebookp,
    swi_uint32 timeout)
{
    SWI_RCODE          resultcode;  /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;   /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength;  /* Ret'd length of the CnS Parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* no outgoing parameters */
    parmlength = 0;

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_PHONE_BOOK_AVAIL, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );
    
    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUpkGsPhbkAvail( inparamp, Phonebookp );
    }

    /* All done, send the result to the caller */
    return resultcode;
    
}

/**
 *
 * This function is used to query the number of remaining free entries and the
 * size of all of the phonebooks on the SIM.
 *
 * This function should not be called until the SWI_NOTIFY_PhonebookReady
 * has been received indicating phonebooks are ready. 
 *
 * \param  Phonebookp: [OUT] 
 *         Phonebook size 
 *
 * \param  timeout: [IN]    
 *         Response timeout value in milliseconds
 *
 * \return SWI_RCODE_OK:
 *         The request has been processed and values returned are valid.
 *
 * \return See SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_PHONEBK_Size
 * \sa SWI_NOTIFY_PhonebookReady
 *
 */
SWI_API SWI_RCODE SwiGetPhonebookSize(
    struct SWI_STRUCT_PHONEBK_Size*  Phonebookp,
    swi_uint32 timeout)
{
    
    SWI_RCODE          resultcode;  /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;   /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength;  /* Ret'd length of the CnS Parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* no outgoing parameters */
    parmlength = 0;
            
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_PHONE_BOOK_SIZE_EXP, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );
    
    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUpkGsPhbkSize( inparamp, Phonebookp );
    }

    /* All done, send the result to the caller */
    return resultcode;
    
}

/**
 *
 * The modem contains a list of emergency numbers.  This function is used
 * to fetch the emergency numbers one at a time.  Set the first flag when 
 * calling this function to retrieve the first entry in the list.  To 
 * retrieve the entire list call this function repeatedly.  
 *
 * This function sends a request to the modem.  The modem's response will
 * contain an emergency number string and indicate if this is the last
 * number in the list.    
 *
 * \param EmergencyNumberp:  [IN/OUT]  
 *        Phonebook entry
 *
 * \param bfirst: [IN] 
 *        First emergency number flag (this flag should only be 
 *        set the first time this function is called)
 *
 * \param timeout: [IN]    
 *        Response timeout value in milliseconds.
 *
 * \return SWI_RCODE_OK: 
 *         The request is being processed.
 *
 * \return SWI_RCODE_REQUEST_TIMEOUT
 *         The request has timed out
 *
 * \return See SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_Emergency_Number
 *
 */
SWI_API SWI_RCODE SwiGetEmergencyEntry(
    struct SWI_STRUCT_Emergency_Number *EmergencyNumberp,
    swi_bool bFirst, 
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
    parmlength = 1;
    *outparamp++ = (swi_uint8) bFirst;    /* first or next */
            
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_EMERG_NUM_EXP,
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ, /* Set in order to read */
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );
    
    /* Only parse out the response data if we got a positive return */
    if( resultcode == SWI_RCODE_OK )
    {
        SwiUpkGsPhbkGetEmerNum( inparamp, EmergencyNumberp );
    }

    /* All done, send the result to the caller */
    return resultcode;
}


/**
 *
 * Use this function to delete a phonebook entry.
 *
 * This function should not be called until the SWI_NOTIFY_PhonebookReady
 * has been received indicating phonebooks are ready. 
 *         
 * \param  Phonebookp: [IN/OUT] 
 *         Phonebook entry to be deleted.  The structure size, phonebook entry,
 *         and phonebook type must be supplied by the caller.
 *
 * \param  timeout: [IN]    
 *         Response timeout value in milliseconds.
 *
 * \return SWI_RCODE_OK:
 *         The request has been processed and values returned are valid.
 *         See Phonebookp->eResultCode for result of the operation.
 *
 * \return See SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_PHONEBK_DeleteEntry
 * \sa SWI_NOTIFY_PhonebookReady
 * \sa SwiGetPhonebookEntry
 * \sa SwiAddPhonebookEntry
 * \sa SwiEditPhonebookEntry
 * \sa SwiDeletePhonebookLastNum
 *
 */
SWI_API SWI_RCODE SwiDeletePhonebookEntry(
    struct SWI_STRUCT_PHONEBK_DeleteEntry* Phonebookp,
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
    parmlength = 2;
    *outparamp++ = (swi_uint8) Phonebookp->ePhoneBookType;
    *outparamp++ = (swi_uint8) Phonebookp->nID;
            
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_PHONE_NUM_DEL_EXP, 
                                  parmlength, 
                                  timeout, 
                                  CNS_SETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );

    /* Get the result of operation */
    Phonebookp->eResultCode = *inparamp++;
    if (Phonebookp->eResultCode != SWI_PHONEBOOKRESULT_Success)
    {
        resultcode = SWI_RCODE_FAILED;
    }
    
    /* All done, send the result to the caller */
    return resultcode;  
} 
 
/**
 *
 * This function is used to request the fixed number dialing (FDN) mode
 * from the modem.  
 *         
 * \param  pFdnEnabled: [OUT]  
 *         True if FDN is enabled, false otherwise. 
 *
 * \param  timeout: [IN] 
 *         Response timeout value in milliseconds.
 *
 * \return SWI_RCODE_OK:
 *         The request has been processed and values returned are valid.
 *
 * \return See SWI_RCODE for other error return codes.
 *
 * \sa SwiSetFdnMode
 *
 */
SWI_API SWI_RCODE SwiGetFdnMode(swi_bool* FdnEnabledp, swi_uint32 timeout)
{       
    SWI_RCODE          resultcode;  /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;   /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength;  /* Ret'd length of the CnS Parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* no outgoing parameters */
    parmlength = 0;
            
    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  CNS_FDN_MODE, 
                                  parmlength, 
                                  timeout, 
                                  CNS_GETREQ,
                                  SWI_AMGSM, 
                                  &inparamp,
                                  &parmlength );

    /* return what was asked for */
    *FdnEnabledp = *inparamp;
    
    /* All done, send the result to the caller */
    return resultcode;
}

/**
 *
 * This function is used to set the fixed number dialing (FDN) mode
 * from the modem.
 * Before calling this function, CHV2 must be verified.  CHV2 is prompted for
 * by calling SwiChv2StatusKick.
 *
 * \param   Fdnp: [IN/OUT] 
 *          Set bEnabled to true to enable FDN, false otherwise.  eResultCode
 * 	        contains the result of the request.
 *
 * \param   timeout: [IN] 
 *          Response timeout value in milliseconds.
 *
 * \return  SWI_RCODE_OK:
 *          The request has been processed and values returned are valid.
 *
 * \return  See SWI_RCODE for other error return codes.
 *
 * \sa SwiSetFdnMode
 *
 */
SWI_API SWI_RCODE SwiSetFdnMode(
    struct SWI_STRUCT_FdnMode *Fdnp,
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
    parmlength = 1;
    *outparamp++ = (swi_uint8)Fdnp->bEnabled;

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp,
                                  CNS_FDN_MODE,
                                  parmlength,
                                  timeout,
                                  CNS_SETREQ,
                                  SWI_AMGSM,
                                  &inparamp,
                                  &parmlength );
    /* Get the set result */
    Fdnp->eResultCode = *inparamp;
    if (Fdnp->eResultCode != SWI_PHONEBOOKRESULT_Success)
    {
        resultcode = SWI_RCODE_FAILED;
    }
    /* All done, send the result to the caller */
    return resultcode;
}

/*@}*/ /* End of Phonebook group */    

/*
 * $Log: SwiApiGsmPhonebook.c,v $
 */

