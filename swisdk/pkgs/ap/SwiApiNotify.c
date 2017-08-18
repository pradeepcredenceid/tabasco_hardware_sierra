/**
 *    \if CvsId 
 *    $Id: SwiApiNotify.c,v 1.13 2010/06/10 00:20:41 epasheva Exp $
 *    \endif
 *    \ingroup notify
 *    \file SwiApiNotify.c
 * 
 *    \brief  Contains notifications API
 *
 *    Copyright: © 2008 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */
#include "SwiApiNotify.h"
#include "ap/apidefs.h"


/* defines */

/* RILSTART */
/* Stop and wait timeout to use when creating the 
 * Notification thread
 */
#define SWI_NOTIF_THREAD_TIMEOUT_MS 1000
/* RILSTOP */

/* Global storage */
swi_uint8  inbuf[APNTBUFSIZE];          /* IPC buffer for messages received 
                                         * on the notification channel 
                                         */
struct SwiNotifyVariant sNotifyVariant; /* holds data for notification type */


/************** Internal Functions **************/


/************** Notification APIs **************/

/**
 *
 * This function registers a callback handler to receive event 
 * notifications.        
 *
 * All other notifications are disabled by default and must be 
 * enabled.
 *
 * This release of the API supports multiple callback handlers.
 * Notifications are sent to all registered callback handlers.
 * Each callback function can provide a filter to mask out 
 * unwanted notifications on that particular callback function. 
 *
 * nNotifications should be set to 0xFFFFFFFFFFFFFFFF to receive 
 * all possible notifications.
 *                
 * Callback functions should be re-registered every time 
 * SwiSelectAirServer is called. This ensures that features of 
 * the newly bound device will have access to all the callback 
 * pointers.
 *
 * On termination, SwiDeRegisterCallback should be called for each
 * callback function previously registered.
 *
 * This function must be called immediately after SwiApiOpen to
 * ensure the client application receives critical events.  
 *
 * The following notifications are enabled by default and can not 
 * be disabled:
 *
 * SWI_NOTIFY_AirServerChange
 *
 * \param pCallback: [OUT]     callback function pointer    
 * \param nNotifications: [IN] notification filter (see Notes)    
 *
 * \sa See SWI_RCODE for additional information.
 *
 */
SWI_API SWI_RCODE SwiRegisterCallback(SWI_CALLBACK_EX pCallback, 
                                      swi_uint64 nNotifications)
{
    SWI_RCODE retcode  = SWI_RCODE_OK;
    struct apicb *apicbp;

    /* Do the standard checks */
    if(!SwiApiRunning())
    {
        return SWI_RCODE_API_NOT_OPEN;
    }

    /* Get global control block pointer */
    apicbp = SwiApiGetCbp();

    /* Store the pointer to callback in ap control block */
    apicbp->apcallbackp = pCallback;

    /* store the notification filters */
    apicbp->apnotifications = nNotifications;
    return retcode;
}

/**
 *
 * This function de-registers an application's callback handler.
 *
 * After calling this function, notifications will no longer 
 * be passed to the specified callback handler.
 *
 * \param pCallback: [OUT]  callback function pointer    
 *
 * \return  SWI_RCODE_OK             - callback function successfully 
 *                                     deregistered
 * \return  SWI_RCODE_INVALID_PAR    - an invalid parameter was passed
 *
 * \sa See SWI_RCODE for additional information.
 *
 */
SWI_API SWI_RCODE SwiDeRegisterCallback(SWI_CALLBACK_EX pCallback)
{
    SWI_RCODE retcode  = SWI_RCODE_OK;
    struct apicb *apicbp;

    /* Do the standard checks */
    if(!SwiApiRunning())
    {
        return SWI_RCODE_API_NOT_OPEN;
    }

    /* Get global control block pointer */
    apicbp = SwiApiGetCbp();

    /* if the callback pointer is not NULL then delete it */
    if ( apicbp->apcallbackp != NULL )
    {
        apicbp->apcallbackp = NULL;
    }
    return retcode;
}


/**
 *
 * This function turns the notification ON for the specified event.
 *
 * Calling this function enables event notifications to be 
 * routed to the callback handler registered earlier using 
 * SwiRegisterCallback. If notifications are enabled when there 
 * is no registered callback function, the API will silently 
 * discard them as they arrive.  Similarly, if a filter were 
 * defined, notifications will be silently discarded.
 *
 * Modem resets disable notifications (but leave the callback 
 * function registered). You must explicitly re-enable 
 * notifications if a modem reset is detected.  
 *
 * Once enabled, the modem will always send the current state 
 * of the notification object.  When that notification is 
 * generated will depend on the object itself. After the 
 * initial indication, notifications
 * are only generated when there is a change in state.  
 * Occasionally, notifications are generated at predefined 
 * intervals.  See a description of the notification object 
 * for more information.
 *
 * The following notifications are enabled by default and cannot 
 * be disabled:
 *
 * SWI_NOTIFY_AirServerChange      
 *                
 * All other notifications are disabled by default and must 
 * be enabled.
 *            
 * \param eNotify: [IN]        notification to enable    
 * \param timeout: [IN]        response timeout value in milliseconds    
 *
 * \return SWI_RCODE_OK        - notification successfully enabled
 * \return SWI_RCODE_NOT_INIT  - Notification thread has not been started.
 * 
 * \sa See SWI_RCODE for additional information.
 *
 */
SWI_API SWI_RCODE SwiNotify(enum SWI_TYPE_Notify eNotify, swi_uint32 timeout)
{
    SWI_RCODE          resultcode;  /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;   /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength;  /* Ret'd length of the CnS Parm field */
    struct apicb       *apicbp;     /* pointer to ap package control block */

    /* Get global control block pointer */
    apicbp = SwiApiGetCbp();
    
    /* Do not allow enable notification if the notification thread has not been
     * started.
     */
    if (!apicbp->apwaitnotiffnp)
    {
        return SWI_RCODE_NOT_INIT;
    }

    /* AirServer Change and FW Download notifications are always 
     * enabled, so just return here without doing anything
     */
    if(( eNotify == SWI_NOTIFY_AirServerChange) ||
        (eNotify == SWI_NOTIFY_FwDownload ))
    {
        return SWI_RCODE_OK;
    }
   
    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Normally a Pack function call would go here, but this 
     * request has no CnS Parameter field to pack
     */

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  SwiGetObjid(eNotify),
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_NOTREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &parmlength );
                     
    /* Note: The response is sent immediately after being received. 
     * Therefore the host must assume the request was succesful
     */
    return resultcode;
}


/**
 *
 * This function turns the notification OFF for the specified 
 * event.
 *
 * After calling this function, the specified notifications will 
 * not be forwarded to the client's callback function.
 *
 * The following notifications are enabled by default and can not 
 * be disabled:
 *
 * SWI_NOTIFY_AirServerChange        
 *
 * \param eNotify: [IN]    notification to disable    
 * \param timeout: [IN]    response timeout value in milliseconds    
 *
 * \sa See SWI_RCODE for additional information.
 *
 */
SWI_API SWI_RCODE SwiStopNotify(enum SWI_TYPE_Notify eNotify, swi_uint32 timeout)
{
    SWI_RCODE          resultcode; /* Result of SwiIntSendnWait() */
    swi_uint8          *inparamp;  /* ptr to param field rx'd from modem */
    swi_uint8          *reqbufferp; /* Pointer to ougoing request buffer */
    swi_uint16         parmlength; /* Ret'd length of the CnS Parm field */

    /* Initialize the pointer to the outgoing request buffer pointer */
    reqbufferp = cngetreqbkp();

    /* Normally a Pack function call would go here, but this 
     * request has no CnS Parameter field to pack
     */

    /* Prepare and send the blocking call */
    resultcode = SwiIntSendnWait( reqbufferp, 
                                  SwiGetObjid(eNotify), 
                                  0, /* No CnS Parameter field */
                                  timeout, 
                                  CNS_STOPREQ,
                                  SWI_AMCOMMON, 
                                  &inparamp,
                                  &parmlength );

    /* All done, send the result to the caller */
    return resultcode;
}

/* RILSTART */
/**
 *
 * This function creates the Notification thread for the caller,
 * blocking the caller until the thread creation is complete.
 *
 * \param threadfuncp [IN]
 *        Pointer to the function serving as the thread's entry point
 *
 * \return  SWI_RCODE_OK              - callback function successfully 
 *                                      deregistered
 * \return  SWI_RCODE_REQUEST_TIMEOUT - The request timed out meaning
 *                                      that the notification thread 
 *                                      did not release the waiter
 *                                      in time
 *
 * \sa See SWI_RCODE for additional information.
 *
 */
SWI_API SWI_RCODE SwiCreateNotifThread( void (*threadfuncp)(void) )
{
    struct apicb       *apicbp;     /* pointer to ap package control block */

    /* Get global control block pointer */
    apicbp = SwiApiGetCbp();

/* RILSTART - Bug 36723 - Bad Waiter Initialization in SwiApiNoticy.c */
    /* Initialize the waiter we'll use to block until 
     * the notification thread is up and running 
     */
    swi_osapiwaiterinit( &apicbp->apnotifsynchronizer );

    /* Create the new notification thread */
    swi_osapithreadcreate( threadfuncp );
/* RILSTOP */

    /* Block here until the notification thread 
     * frees us or we timeout waiting 
     */
    if( !swi_osapiwaiterwait( &apicbp->apnotifsynchronizer, 
                             SWI_NOTIF_THREAD_TIMEOUT_MS ) )
    {
        /* Timed out */
        return SWI_RCODE_REQUEST_TIMEOUT;
    }

    /* Normal wakeup */
    return SWI_RCODE_OK;
}
/* RILSTOP */
/**
 *
 * The AP Notification receive API main entry point
 *             
 * When the received packet is wrong type the message "Wrong 
 * packet type" is logged and program terminates.
 *
 * This function waits to receive notifications
 * and invokes the user callback function if registered. 
 * This function does not return unless there is an error.
 *
 * \return  SWI_RCODE_OK              - The function executed properly.
 * \return  SWI_RCODE_FAILED          - The function failed to execute 
 *                                      correctly. 
 * \return  SWI_RCODE_API_NOT_OPEN    - SDK is not running.
 * \sa  See SWI_RCODE for additional information. 
 *
 */
SWI_API SWI_RCODE SwiWaitNotification(void) 
{
    SWI_RCODE retcode            = SWI_RCODE_OK;
    swi_bool result;             /* result of waiting for notification 
                                  * message on IPC channel 
                                  */
    struct apicb *apicbp;        /* pointer to ap package control block */
    swi_uint8  *inbufp;          /* local pointer to global receive buffer
                                  * structure 
                                  */
    swi_uint8  *payloadp;        /* pointer to payload data in the received
                                  * buffer structure */
    swi_uint32 apipcbuflng;      /* length of receive buffer for notification
                                  * message 
                                  */
    swi_uint32 timeout           = NOTTIMEOUT;
    static struct amrrparms   amrrparms;      /* parsing structure */

    /* Get global control block pointer */
    apicbp = SwiApiGetCbp();

    /* Do the standard checks */
    if(!SwiApiRunning())
    {
        return SWI_RCODE_API_NOT_OPEN;
    }
    
    /* Set the wait notification pointer so that we know the notification thread
     * has already been started.
     */
    apicbp->apwaitnotiffnp = SwiWaitNotification;

/* RILSTART */
    /* Release the main thread now that the notification thread is up and running */
    swi_osapiwaiterwake( &apicbp->apnotifsynchronizer );
/* RILSTOP */
    /* Loop until a proper notification is received or the SDK terminates 
     * unsuccessfully 
     */
    while(1)
    {
        /* Make a local copy of the incoming packet pointer */
        inbufp = &inbuf[0];

        /* Initialize ipc buffer length variable */
        apipcbuflng = APNTBUFSIZE;

        /* Wait for IPC message on notification channel */
        result = amapiwaitnotif( inbufp, 
                             &apipcbuflng, 
                             &amrrparms,
                             &payloadp,
                             timeout);

        /* if the IPC channel timed out or received something */
        if(result)
        {
            /* if a notification was received */
            if(apipcbuflng != 0)
            {
                switch(amrrparms.amparmtype)
                {
                    case SWI_PARM_CNS:
                        /* Decode CnS notification */
                        SwiDecodeCnsNotify(amrrparms.amparm.amcns, 
                                    payloadp, &sNotifyVariant );
                        break;
                        
                    case SWI_PARM_FWDWN:
                        /* Decode Firmware Download notification */
                        SwiDecodeFwDwnNotify(amrrparms.amparm.amfwdwn, 
                            &sNotifyVariant );
                        break;
                        
                    case SWI_PARM_AIRSRV:
                        /* Call the special Air Change Notification Decoder */
                        swidecodeairchgnotif( &amrrparms.amparm.amairsrv, 
                            &sNotifyVariant );
                        break;
                        
                    default:
                        erAbort("SwiWaitNotification-unknown parmtype\n",
                             amrrparms.amparmtype);
                }
                
                /* Get global control block pointer */
                apicbp = SwiApiGetCbp();

                /* If there is a registered callback activate the callback*/
                if ( apicbp->apcallbackp != NULL )
                {
                    apicbp->apcallbackp(&sNotifyVariant);
                }
                retcode = SWI_RCODE_OK; 
            }
            /* else no data was received */
            else
            {
                /* Timeout condition */
                /* go back to waiting for message */
            }
        }
        else /* bad result */
        {
            /* error in receiving on notification channel */
            retcode = SWI_RCODE_FAILED;
            break;
        }
    }
    return retcode;
}


/*
 * $Log: SwiApiNotify.c,v $
 */
