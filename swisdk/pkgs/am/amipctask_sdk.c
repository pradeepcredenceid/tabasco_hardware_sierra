/*************
 *
 * $Id: amipctask_sdk.c,v 1.18 2010/06/09 23:27:55 epasheva Exp $
 *
 * Filename: amipctask_sdk.c
 *
 * Purpose:  AM package IPC Receive task and associated functions
 *
 * Copyright: © 2008 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* include files */
#include "ci/ciudefs.h"
#include "cn/cnudefs.h"
#include "sdk/sdkudefs.h"
#include "su/suudefs.h"
#include "am/amidefs.h"

/* Defines */

/* Local storage */
struct amipccb amipcbk;

/* This next structure is a dummy event block which will
 * be sent to this AM task whenever a response is sent to the host. 
 */
struct amevtblock amresponseevt = { AM_APP_RESP_SENT, NULL, 0, NULL, 0,
                                    {NULL,NULL,NULL} };


/*************
 *
 * Name: amgetipccbp
 *
 * Purpose: Function to get a pointer to the CnS IPC receive task's
 *          control block structure.
 *             
 * Parms:   none
 *
 * Return:  pointer to struct amipctcb
 *
 * Abort:   none
 *
 * Notes:   None
 *
 **************/
local struct amipccb *amgetipccbp( void )
{
    return &amipcbk;
}

/*************
 *
 * Name:    amqueipcrspevt
 *
 * Purpose: This function sends the event AM_APP_RESP_SENT to the AM task
 *             
 * Parms:  ipcchannel  - ipcchannel that this response went out on 
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   No pool is used. Sends a canned event.
 * 
 **************/
local void amqueipcrspevt( swi_uint8 ipcchannel )
{  
    struct amevtblock  *eventbkp;    /* event block pointer for AM task */

    /* Get pointer to the task control block */
    struct amipccb *amipccbp = amgetipccbp();

    /* Get canned event block to send event to AM task */
    eventbkp = &amresponseevt;
    eventbkp->amipcchannel = ipcchannel;

    /* Send the new event to the AM task for processing */
    icsmt_send( &amipccbp->amicmsgque, (swi_uint32) eventbkp );
}

/*************
 *
 * Name:    amipcrcvhandler
 *
 * Purpose: This function sends the event AM_APP_REQUEST to the AM task
 *             
 * Parms:   msgp        - pointer to message being passed in
 *          datalen     - length of data passed in 
 *          memfreep    - pointer to memory to be freed
 *          ipcchannel  - ipcchannel that this request came in from 
 *
 * Return:  none
 *
 * Abort:   When AM is out of event blocks
 *
 * Notes:   This is a callback function whenever DS receives traffic.
 *          No pre-processing is needed on the incoming message, so we queue it
 *          to the AM task immediately.
 *          This runs in the context of the calling task, in this case, DS's 
 *          shells.
 * 
 **************/
local void amipcrcvhandler(
    swi_uint8   *msgp, 
    swi_uint16  datalen,
    swi_uint8   *memfreep,
    swi_size_t  ipcchannel)
{  
    struct amevtblock  *eventbkp;    /* event block pointer for AM task */

    /* Get pointer to the task control block */
    struct amipccb *amipccbp = amgetipccbp();

    /* Allocate event block to send event to AM task */
    eventbkp = (struct amevtblock *) mmbufget( &amipccbp->amevtpool );
          
    /* Don't want NULL event block pointers */
    if( eventbkp )
    {
        /* Fill the event with AM request message */
        eventbkp->amevent        = AM_APP_REQUEST;
        eventbkp->ammemfreep     = memfreep;  
        eventbkp->amdatalen      = datalen;
        eventbkp->amdatap        = msgp;
        eventbkp->amipcchannel   = ipcchannel;
        qlinitlnk( &(eventbkp->ameqlnk), NULL );
                                                    
        /* Send the new event to the AM task for processing */
        icsmt_send( &amipccbp->amicmsgque, (swi_uint32) eventbkp );
    }
    /* NULL Block */
    else
    {
        /* This should never happen */
        erAbort("AM: Out of evt blk memory", 0 );
    }
} 

/*************
 *
 * Name:    amsdk_send
 *
 * Purpose: Global function that sends a RR response to the host. 
 *             
 * Parms:   ipcbufp    - pointer to data which will be sent 
 *          ipbuflen   - length of data to send
 *          ipcchannel - IPC channel
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   None
 *
 **************/
global void amsdk_send( swi_uint8 *ipcbufp, 
                        swi_uint32 ipcbuflen,
                        swi_uint8 ipcchannel )
{
    /* Use dssend instead of icsdk_send since we are using the DS engine */
    dssend( ipcbufp, ipcbuflen, ipcchannel );

    /* Send inter-task message to the response Q to flag that we're done */
    amqueipcrspevt(ipcchannel);
}

/*************
 *
 * Name:    amsdk_sendnot
 *
 * Purpose: Global function that sends a notification to the host. 
 *             
 * Parms:   ipcbufp    - pointer to data which will be sent 
 *          ipbuflen   - length of data to send
 *          ipcchannel - IPC channel. If = ICCNNOTIFYALL, notification is sent
 *                       to all available notification channels.
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   This is used for sending non-CNS notifications directly.It does not
 *          use the locally built CNS notification table.
 *          The Notification channel # is different from the RR channel
 *          The RR channel is used to index into the table of notification
 *          handlers since they are paired up.
 *
 **************/
global void amsdk_sendnot( 
    swi_uint8 *ipcbufp, 
    swi_uint32 ipcbuflen,
    swi_uint8 ipcchannel )
{
    /* Define and allocate the task control block */
    struct amipccb *amipccbp = amgetipccbp();
    swi_uint16 ipcindex;
    swi_uint16 numnotif;
    
    /* Use icsdk_send since we are NOT using the DS engine for notifications */
    
    /* Send to all Notification cahnnels or only requested channel */
    if (ipcchannel == CICNNOTIFYALL)
    {
        numnotif = cigetnumnotifchan();
        for( ipcindex = 0; ipcindex < numnotif; ipcindex++ )
        {
            icsdk_send( cigetnextnotifipcp(ipcindex), ipcbufp, ipcbuflen );
        }
    }
    else
    {
        /* Send only to the requested IPC channel, converted to an index */
        icsdk_send( cigetipcp( ipcchannel), 
                    ipcbufp, 
                    ipcbuflen );
    }
    dlLog(  &amipccbp->amdlcb, AMLOG_CLASSA, "Non-CNS notif on all chan\n",
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL);                   
}

/*************
 *
 * Name:    amsdk_sendcnsnot
 *
 * Purpose: Global function that sends a CNS notification to the host. 
 *             
 * Parms:   ipcbufp    - pointer to data which will be sent 
 *          ipbuflen   - length of data to send
 *          cnsobjid   - CNS object id requiring notification
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   Notifications are sent based on being registered in our Notification
 *          list when AM intercepts notification requests.
 *
 **************/
global void amsdk_sendcnsnot( 
    swi_uint8 *ipcbufp, 
    swi_uint32 ipcbuflen,
    swi_uint16 cnsobjid)
{
    /* Define and allocate the task control block */
    struct amipccb      *amipccbp = amgetipccbp();
    struct icipccb      *nothandlp;
    struct amipcnotifq *notifheadqp;
    struct amipcnotifq *currnotifp;
    struct qllink      *currlinkp;
    int                 ipcchidx;
    swi_uint16          errorcode;
    swi_uint16          local_notifflg;
    swi_uint16          notifbit = 1; /* initialize to bit position 0 */
    swi_bool            bpeerthere;
    swi_uint16          numnotchan;
    
    numnotchan = cigetnumnotifchan();
    notifheadqp = &amipccbp->amnotifheadq;
    
    /* if empty, add notifcation as new  */
    currlinkp = &(notifheadqp->amnqlnk);

    while ((currnotifp = qlgetnext (currlinkp)))
    {
        currlinkp = &currnotifp->amnqlnk;
        
        /* Find notification object in our table */
        if (currnotifp->amcnsobjid == cnsobjid)
        {
            /* Send to whomever that asked for notification */
            for (ipcchidx=0; ipcchidx < numnotchan; ipcchidx++)
            {
                local_notifflg = currnotifp->amchannotifflg;
                /* Do house-keeping in case application has gone away */
                nothandlp = cigetnextnotifipcp (ipcchidx);
                bpeerthere = swi_ossdkpeeravail(&nothandlp->icipcip, 
                                                &errorcode);
                dlLog(  &amipccbp->amdlcb, AMLOG_CLASSA, 
                        "Notif obj/ch/FLAG %04x/%d/0x%04x\n",
                        (swi_uint32)cnsobjid,
                        (swi_uint32)nothandlp->icipcindex,
                        (swi_uint32)currnotifp->amchannotifflg,
                        (swi_uint32)NULL);                   

                /* Update our table if gone and expecting notification */
                if (!bpeerthere && 
                     ((local_notifflg >> ipcchidx) & 0x01) )
                {
                    currnotifp->amchannotifflg &= 
                        ~(notifbit<<ipcchidx) & AMMAXNOTIFBITSINUSE;
                    dlLog(  &amipccbp->amdlcb, AMLOG_CLASSA, 
                            "No app notif!, obj/ch %04x/%d\n",
                            (swi_uint32)cnsobjid,
                            (swi_uint32)nothandlp->icipcindex,
                            (swi_uint32)NULL,
                            (swi_uint32)NULL);                   
                }
                
                /* Everything else checked out, now we really send if needed */
                if ( (local_notifflg >> ipcchidx) & 0x01 )
                {
                    /* Use icsdk_send since we are NOT using the DS engine 
                     * for notifications
                     */
                    icsdk_send( cigetnextnotifipcp (ipcchidx), ipcbufp, ipcbuflen );
                    dlLog(  &amipccbp->amdlcb, AMLOG_CLASSA,
                            "CNS notif sent obj/ch %04x/%d\n",
                            (swi_uint32)cnsobjid,
                            (swi_uint32)nothandlp->icipcindex,
                            (swi_uint32)NULL,
                            (swi_uint32)NULL);                   
                }
            }
        }
    }
}

/*************
 *
 * Name:    amipcaddnewnotif
 *
 * Purpose: Add a new Notification control block to the Notification list
 *             
 * Parms:   cnsobjid   - CNS object id for the notification
 *          ipcchannel - The IPC channel which will receive the notification
 * 
 * Return:  none
 *
 * Abort:   Whem AM is out of notification blocks
 *
 * Notes:   Notifications are added as they come in. They never get deleted 
 *          from the list. Only the channels get disabled when a stop request
 *          is issued.
 *
 **************/
local void amipcaddnewnotif (
    swi_uint16 cnsobjid, 
    swi_uint8 ipcchannel)
{
    struct amipcnotifq *notifheadqp;
    struct amipcnotifq *newnotifp;
    swi_uint16 notifbit = 1; /* initialize to bit position 0 */
    
    /* Get pointer to the task control block */
    struct amipccb *amipccbp = amgetipccbp();
    
    notifheadqp = &amipccbp->amnotifheadq;
    /* Allocate queue block to register Notification for AM task */
    newnotifp = (struct amipcnotifq *) mmbufget( &amipccbp->amnotifpool );
    
    /* We don't want NULL notification block pointers */
    if (newnotifp)
    {
        newnotifp->amcnsobjid = cnsobjid;
        
        /* Initialize receive notification flags to FALSE */
        newnotifp->amchannotifflg = 0;

        /* Set our channel to TRUE to receive notifications */
        newnotifp->amchannotifflg |= 
                                (notifbit << ipcchannel) & AMMAXNOTIFBITSINUSE;
        
        /* Initialize queue link */
        qlinitlnk(&newnotifp->amnqlnk, newnotifp);

        /* Add the registration to head of queue */
        qladdprev(&(notifheadqp->amnqlnk), &newnotifp->amnqlnk);

        dlLog(  &amipccbp->amdlcb, AMLOG_CLASSA, 
                "Notif added:0x%04x, ch:%d flg: 0x%04x\n",
                (swi_uint32)cnsobjid,
                (swi_uint32)ipcchannel,
                (swi_uint32)newnotifp->amchannotifflg,
                (swi_uint32)NULL);
    }
    /* NULL block */
    else
    {
       /* This should never happen as we are guaranteed as many blocks as
        * there are notifications 
        */
        erAbort("AM: Out of notification blk memory", 0 );
    }
}

/*************
 *
 * Name:    amipcenablenotif
 *
 * Purpose: Enable a Notification when a start request is received
 *             
 * Parms:   cnsobjid   - CNS object id for the notification
 *          ipcchannel - The IPC channel which will receive the notification
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   If the notification list is empty, simply add it and enable. 
 *          Otherwise try to find the matching Notification block and 
 *          enable for the given channel. 
 *          If still not found, then add a new block.
 *
 **************/
local void amipcenablenotif (
    swi_uint16 cnsobjid, 
    swi_uint8 ipcchannel)
{
    struct amipcnotifq *notifheadqp;
    struct amipcnotifq *currnotifp;
    struct qllink      *currlinkp;
    swi_bool bexists = FALSE;
    swi_uint16 notifbit = 1; /* initialize to bit position 0 */
    
    /* Get pointer to the task control block */
    struct amipccb *amipccbp = amgetipccbp();
    
    notifheadqp = &amipccbp->amnotifheadq;
    
    /* if empty, add notification as new  */
    currlinkp = &(notifheadqp->amnqlnk);
    
    currnotifp = qlgetnext (currlinkp);
    /* Handle empty list first */
    if ( !currnotifp )
    {
        amipcaddnewnotif (cnsobjid, ipcchannel );
    }
    else
    {
        /* update if exists */
        while (currnotifp)
        {
            if (currnotifp->amcnsobjid == cnsobjid)
            {
                /* found it, update passed-in channel flag, regardless */
                currnotifp->amchannotifflg |= 
                                    (notifbit<<ipcchannel) & AMMAXNOTIFBITSINUSE;
                bexists = TRUE;
                break;
            }
            /* get next */
            currnotifp = qlgetnext (&(currnotifp->amnqlnk)); 
        }
        /* If not found in our existing list, add it as new */
        if ( !bexists)
        {
           amipcaddnewnotif (cnsobjid, ipcchannel );
        }
    }
}

/*************
 *
 * Name:    amipcdisablenotif
 *
 * Purpose: Disable notification of the given object id on the given channel
 *             
 * Parms:   cnsobjid   - CNS object id for the notification
 *          ipcchannel - The IPC channel which will receive the notification
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   If the specified notification is not enabled this function 
 *          takes no action
 *
 **************/
local void amipcdisablenotif (
    swi_uint16 cnsobjid, 
    swi_uint8 ipcchannel)
{
    struct amipcnotifq *notifheadqp;
    struct amipcnotifq *currnotifp;
    struct qllink      *currlinkp;
    swi_uint16 notifbit = 1; /* initialize to bit position 0 */
    
    /* Get pointer to the task control block */
    struct amipccb *amipccbp = amgetipccbp();
    
    /* Get a pointer to our current head */
    notifheadqp = &amipccbp->amnotifheadq;
    currlinkp = &(notifheadqp->amnqlnk);

    /* Loop through the list to find our object id, then disable it */
    while ((currnotifp = qlgetnext (currlinkp)))
    {
        currlinkp = &currnotifp->amnqlnk;
        
        if (currnotifp->amcnsobjid == cnsobjid)
        {
            /* found it, update passed-in channel flag */
            currnotifp->amchannotifflg &= 
                            ~(notifbit<<ipcchannel) & AMMAXNOTIFBITSINUSE;
            dlLog(  &amipccbp->amdlcb, AMLOG_CLASSA, 
                    "Notif disabled:0x%04x, ch:%d\n",
                    (swi_uint32)cnsobjid,
                    (swi_uint32)ipcchannel,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL);
            break;
        }
    }
}

/*************
 *
 * Name:    amipcdisableallnotif
 *
 * Purpose: Disable all notifications 
 *             
 * Parms:   none
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   This is only for CNS notifications.
 *
 **************/
global void amipcdisableallnotif ( void )
{
    struct amipcnotifq *notifheadqp;
    struct amipcnotifq *currnotifp;
    struct qllink      *currlinkp;
    
    /* Get pointer to the task control block */
    struct amipccb *amipccbp = amgetipccbp();
    
    /* Get a pointer to our current head */
    notifheadqp = &amipccbp->amnotifheadq;
    currlinkp = &(notifheadqp->amnqlnk);

    /* Loop through the list to disable notifications */
    while ((currnotifp = qlgetnext (currlinkp)))
    {
        currlinkp = &currnotifp->amnqlnk;
        currnotifp->amchannotifflg = 0;
    }
    dlLog(  &amipccbp->amdlcb, AMLOG_CLASSA, 
            "Notifications disabled\n",
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL);
}

/*************
 *
 * Name:    amipchandlenotifreq
 *
 * Purpose: Handle Notification requests from applications - add new or update
 *          existing and disables notification for given object or all.
 *             
 * Parms:   cnsobjid     - CNS object id for the notification
 *          cnsoperation - one of CNS_NOTREQ, CNS_STOPREQ, (CNS_SETREQ with 
 *                          CNS_DIS_ALL_NOT)
 *          ipcchannel   - The IPC channel which will receive the notification
 *
 * Return: none
 *
 * Abort:   none
 *
 * Notes: 
 *
 **************/
local void amipchandlenotifreq (
    swi_uint16 cnsobjid, 
    swi_uint8  cnsoperation,
    swi_uint8  ipcchannel)
{
    /* Get pointer to the task control block */
    struct amipccb *amipccbp = amgetipccbp();

    switch (cnsoperation)
    {
        case CNS_NOTREQ:
        	switch(cnsobjid)
        	{
        		case CNS_CALL_NOTIFICATION_STATUS:
                    amipcenablenotif (CNS_CALL_INCOMING, ipcchannel);
                    amipcenablenotif (CNS_CALL_CONNECTED, ipcchannel);
                    amipcenablenotif (CNS_CALL_DISCONNECTED, ipcchannel);
                    amipcenablenotif (CNS_CALL_ERROR, ipcchannel);
                    amipcenablenotif (CNS_CALL_CONNECTING, ipcchannel);
                    amipcenablenotif (CNS_CALL_DORMANT, ipcchannel);
                    amipcenablenotif (CNS_CALL_OTASP_STATE, ipcchannel);
                    break;
                case CNS_CALL_CALLER_ID:
                    amipcenablenotif (cnsobjid, ipcchannel);
                    amipcenablenotif (CNS_CALL_EXTENDED_CALLER_ID, ipcchannel);
                    break;
                case CNS_LBS_LOC_NOTIFY_STATUS:
                    amipcenablenotif (CNS_LBS_PD_DATA, ipcchannel);
                    amipcenablenotif (CNS_LBS_PD_DONE, ipcchannel);
                    amipcenablenotif (CNS_LBS_PD_END, ipcchannel);
                    amipcenablenotif (CNS_LBS_PD_BEGIN, ipcchannel);
                    amipcenablenotif (CNS_LBS_PD_UPDATE_FAILURE, ipcchannel);
                    break;
                case CNS_LBS_DLOAD_NOTIFY_STATUS:
                    amipcenablenotif (CNS_LBS_PD_DLOAD_BEGIN, ipcchannel);
                    amipcenablenotif (CNS_LBS_PD_DLOAD_DATA, ipcchannel);
                    amipcenablenotif (CNS_LBS_PD_DLOAD_DONE, ipcchannel);
                    amipcenablenotif (CNS_LBS_PD_DLOAD_END, ipcchannel);
                    break;
                case CNS_LBS_PARAM_NOTIFY_STATUS:
                    amipcenablenotif (CNS_LBS_PA_IP_ADDR, ipcchannel);
                    amipcenablenotif (CNS_LBS_PA_GPS_LOCK, ipcchannel);
                    amipcenablenotif (CNS_LBS_PA_PTLM_MODE, ipcchannel);
                    amipcenablenotif (CNS_LBS_PA_PORT_ID, ipcchannel);
                    amipcenablenotif (CNS_LBS_PA_PRIVACY, ipcchannel);
                    amipcenablenotif (CNS_LBS_PA_NET_ACCESS, ipcchannel);
                    break;
                default:
                    amipcenablenotif (cnsobjid, ipcchannel);
                    break;
            }
            break;

        case CNS_STOPREQ:
            switch(cnsobjid)
            {
                case CNS_CALL_NOTIFICATION_STATUS:
                    amipcdisablenotif (CNS_CALL_INCOMING, ipcchannel);
                    amipcdisablenotif (CNS_CALL_CONNECTED, ipcchannel);
                    amipcdisablenotif (CNS_CALL_DISCONNECTED, ipcchannel);
                    amipcdisablenotif (CNS_CALL_ERROR, ipcchannel);
                    amipcdisablenotif (CNS_CALL_CONNECTING, ipcchannel);
                    amipcdisablenotif (CNS_CALL_DORMANT, ipcchannel);
                    amipcdisablenotif (CNS_CALL_OTASP_STATE, ipcchannel);
                    break;
                case CNS_CALL_CALLER_ID:
                    amipcdisablenotif (cnsobjid, ipcchannel);
                    amipcdisablenotif (CNS_CALL_EXTENDED_CALLER_ID, ipcchannel);
                    break;
                case CNS_LBS_LOC_NOTIFY_STATUS:
                    amipcdisablenotif (CNS_LBS_PD_DATA, ipcchannel);
                    amipcdisablenotif (CNS_LBS_PD_DONE, ipcchannel);
                    amipcdisablenotif (CNS_LBS_PD_END, ipcchannel);
                    amipcdisablenotif (CNS_LBS_PD_BEGIN, ipcchannel);
                    amipcdisablenotif (CNS_LBS_PD_UPDATE_FAILURE, ipcchannel);
                    break;
                case CNS_LBS_DLOAD_NOTIFY_STATUS:
                    amipcdisablenotif (CNS_LBS_PD_DLOAD_BEGIN, ipcchannel);
                    amipcdisablenotif (CNS_LBS_PD_DLOAD_DATA, ipcchannel);
                    amipcdisablenotif (CNS_LBS_PD_DLOAD_DONE, ipcchannel);
                    amipcdisablenotif (CNS_LBS_PD_DLOAD_END, ipcchannel);
                    break;
                case CNS_LBS_PARAM_NOTIFY_STATUS:
                    amipcdisablenotif (CNS_LBS_PA_IP_ADDR, ipcchannel);
                    amipcdisablenotif (CNS_LBS_PA_GPS_LOCK, ipcchannel);
                    amipcdisablenotif (CNS_LBS_PA_PTLM_MODE, ipcchannel);
                    amipcdisablenotif (CNS_LBS_PA_PORT_ID, ipcchannel);
                    amipcdisablenotif (CNS_LBS_PA_PRIVACY, ipcchannel);
                    amipcdisablenotif (CNS_LBS_PA_NET_ACCESS, ipcchannel);
                    break;
                default:
                    amipcdisablenotif (cnsobjid, ipcchannel);
                    break;
            }
            break;
        case CNS_SETREQ:
            if (cnsobjid == CNS_DIS_ALL_NOT)
            {
                amipcdisableallnotif();
            }
            break;
        default:
            dlLog(  &amipccbp->amdlcb, DLCLASSALL, 
                    "Invalid Notif Q operation\n",
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL);
            break;
    }
}

/*************
 *
 * Name:    amreleventbk
 *
 * Purpose: To release the resources assocated with the AM event block.
 *         
 * Parms:   eventbkp - Pointer to the AM event block structure
 *
 * Return:  None
 *
 * Abort:   None
 *
 * Notes:   This function releases the resources associated with the 
 *          AM event block as follows: first the memory buffer 
 *          containing the newly received AM packet is released. 
 *          Next the event buffer itself is released. Both are 
 *          allocated from pools created at system startup time 
 *          by this AM task. 
 *
 **************/
local void amreleventbk( struct amevtblock *eventbkp )
{
    /* Free the AM message buffer (if any) first */
    if ( eventbkp->ammemfreep != NULL )
    {
        mmbufrel( eventbkp->ammemfreep );
    }

    /* Finally, free the event block itself */
    mmbufrel( (swi_uint8 *)eventbkp );
 }

/*************
 *
 * Name: amShortResp
 *
 * Purpose: Build and send a short response to the API side. This
 *          function is used to send a reply to a request received
 *          from the API side when the request cannot be complied
 *          with in the normal manner
 *             
 * Parms:   amtypercv   - AM packet type of the received request.
 *          resultcode  - An AM packet result code to be stuffed into
 *                        the returned packet
 *          ipcchannel   - The IPC channel which will receive the Short response
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
local void amShortResp( 
    swi_uint16 amtypercv,
    enum swi_amresults resultcode,
    swi_uint8 ipcchannel )
{
    swi_uint16 amtype;
    swi_uint8  outputbuf[AMHEADERSZ];
    swi_uint8  *outputbufp = &outputbuf[0];
    
    switch(amtypercv)
    {   
        case AMCNSREQUEST:
            amtype = AMCNSRESPONSE;
            break;
            
        case AMFWDWNREQUEST:
            amtype = AMFWDWNRESPONSE;
            break;
            
        case AMAIRSRVREQ:
            amtype = AMAIRSRVRESP;
            break;
            
        case AMPORTNAMEREQ:
            amtype = AMPORTNAMERESP;
            break;
            
        default:
            amtype = AMINVALID;
    }
    
    /* build an AM packet that consists only of a header */
    ambuildamheader(amtype, AMHDRVERSION, resultcode, 0, &outputbufp);

    /* Send this packet to the host */
    amsdk_send( &outputbuf[0], AMHEADERSZ, ipcchannel );
}

/*************
 *
 * Name: amdispatchrequest
 *
 * Purpose: Based on the received AM packet type the request is
 *          dispatched to the appropriate handler function.
 *             
 * Parms:   amtype    - AM packet type
 *          inipcmsgp - Pointer to received AM request packet payload.
 *          memfreep  - Pointer to be used when freeing memory for this request
 *          ipcchannel- The IPC channel which message came from
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
local void amdispatchrequest(
    swi_uint16 amtype,
    swi_uint8 *inipcmsgp, 
    swi_uint8 *memfreep,
    swi_uint8 ipcchannel)
{
    /* Get pointer to the task control block */
    struct amipccb *amipccbp = amgetipccbp();

    struct amcnsrrparms cnsparm;    /* CnS Arguments from the CNRR_IPC packet */
    struct amdevinfoparms devinfoparm;               /* Device Info parameters*/
    
    swi_uint8 *local_inipcmsgp = inipcmsgp; /* temporary local pointer */
    
    switch( amtype )
    {
        case AMCNSREQUEST:
            /* If notification-related, update our tables */
            amparsecnsrequest (&local_inipcmsgp, &cnsparm);
            if ( (cnsparm.amcnsoperation == CNS_NOTREQ) ||
                 (cnsparm.amcnsoperation == CNS_STOPREQ) ||
                 ((cnsparm.amcnsoperation == CNS_SETREQ) && 
                   (cnsparm.amcnsobjid == CNS_DIS_ALL_NOT)) )
            {
                amipchandlenotifreq ( cnsparm.amcnsobjid, 
                                      cnsparm.amcnsoperation,
                                      ipcchannel);
                switch(cnsparm.amcnsobjid)
                {
                    case CNS_LBS_LOC_NOTIFY_STATUS:
                    case CNS_LBS_DLOAD_NOTIFY_STATUS:
                    case CNS_LBS_PARAM_NOTIFY_STATUS:
                        amsetcnsrequestoperationid (inipcmsgp, CNS_SETREQ);
                        break;
                }
            }
            /* dispatch this to the CN package */
            cnsendrr( inipcmsgp, memfreep );
            break;

        case AMFWDWNREQUEST:
            dlLog(  &amipccbp->amdlcb, AMLOG_CLASSA, 
                    "Rcv'd AM FW Download Request, ch:%d\n",
                    (swi_uint32)ipcchannel,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL);

            /* dispatch this to the HP package */
            hpsendfwdwnrr( inipcmsgp, memfreep, ipcchannel );
            break;
            
        case AMAIRSRVREQ:
            dlLog(  &amipccbp->amdlcb, AMLOG_CLASSA, 
                    "Rcv'd AM AirServer Request, ch:%d\n",
                    (swi_uint32)ipcchannel,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL);
            
            /* dispatch this to the HP package */
            hpsendairserverrr( inipcmsgp, memfreep, ipcchannel );
            break;
 
       case AMPORTNAMEREQ:
            /* Log the event */
            dlLog(  &amipccbp->amdlcb, AMLOG_CLASSA, 
                    "Rcv'd AM Port Name Request, ch:%d\n",
                    (swi_uint32)ipcchannel,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL);

            /* dispatch this to the HP package */
            hpsendportnamerr( inipcmsgp, memfreep, ipcchannel);
            break;

       case AMMDMRESETREQ:
            /* Log the event */
            dlLog(  &amipccbp->amdlcb, AMLOG_CLASSA, 
                    "Rcv'd AM Modem Reset Request, ch:%d\n",
                    (swi_uint32)ipcchannel,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL);

            /* dispatch to the reset handler */
            hpresetmdm( inipcmsgp, memfreep, ipcchannel );
            break;
 
       case AMDEVINFOREQ:
            /* Log the event */
            dlLog(  &amipccbp->amdlcb, AMLOG_CLASSA, 
                    "Rcv'd AM Dev Info Request, ch:%d\n",
                    (swi_uint32)ipcchannel,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL);

            /* Extract info that is required for this operation */
            amparsedevinforequest (&local_inipcmsgp, &devinfoparm);
            /* dispatch to the HP package */
            hpgetdevinfo( NULL, NULL,
                          devinfoparm.amdevversion, 
                          devinfoparm.amdevinfotype,
                          ipcchannel);
            break;
 
        default:
            /* Log that we got an unrecognized message */
            dlLog(  &amipccbp->amdlcb, AMLOG_CLASSA, 
                    "Rcv'd undefined AM type, %x\n", 
                    (swi_uint32)amtype,
                    (swi_uint32)ipcchannel,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL);
            break;
    }
}

/*************
 *
 * Name: amairinttype
 *
 * Purpose: Provide the caller with a value identifying the air interface
 *          technology of the device attached. This translates between the
 *          "raw" air interface technology of the SI package and the 
 *          AM package's internal representation. More of a formality that
 *          follows the rules of good encapsulation.
 *             
 * Parms:   none
 *
 * Return:  one of AMAIRIFTYPES
 *
 * Abort:   none
 *
 * Notes:   None
 *
 **************/
global enum swi_amairiftypes amairinttype( void )
{
    enum swi_amairiftypes retval;

    /* Get the currently connected modem type */
    switch( ddairinttype() )
    {
        case DDGSM:
            retval = SWI_AMGSM;
            break;

        case DDCDMA:
            retval = SWI_AMCDMA;
            break;

        /* No modem is connected, or undefined product */
        default:
            retval = SWI_AMNOMODEM;
            break;
    }

    return retval;
}

/*************
 *
 * Name:    amstartipcs
 *
 * Purpose: Starts all instances of DS shells for AM IPC service for CNRRx
 *             
 * Parms:   none
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   None 
 *
 **************/
package void amstartipcs( )
{   
    swi_uint16 numshells;
    swi_uint16 i;
    /* Define and allocate the task control block */
    struct amipccb *amipccbp = amgetipccbp();
    swi_uint8 *peernamep;
    swi_uint16 channel;
    
    /* Loop through the number of available CNRR/notification channel pairs */
    numshells = cigetnumRRchan();
    for (i = 0; i < numshells; i++)
    {
        /* Get the IPC Name and set callback function */
        amipccbp->amipcsh[i].dsipcshellnamep = cigetnextrrnamep (i);
        amipccbp->amipcsh[i].dsipccbfcnp = (void *)amipcrcvhandler;
        /* Create a thread for that channel */
        swi_ossdkthreadcreate ( dslaunchshell, (void *)&amipccbp->amipcsh[i] );
        
        /* Open the corresponding IPC channels for Notifications 
         * Note: notifications do not use DS shell */
        channel = cigetindex (amipccbp->amipcsh[i].dsipcshellnamep);
        peernamep = cigetpeernamep ( channel );
        ciinitsdkipcchan(peernamep);
    }
}

/*************
 *
 * Name: amipctask
 *
 * Purpose: Implements the main part of the IPC receive task. This task 
 *          exists solely to block waiting for incoming IPC messages and
 *          then forward these messages to the appropriate recipient for
 *          further processing. 
 *             
 * Parms:   paramp - pointer to data that may be passed in when this task
 *                   is started.
 *
 * Return:  never returns
 *
 * Abort:   none
 *
 * Notes:   This task blocks waiting for inbound AM Packets on 
 *          the multiple IPC channels from the APIs
 *
 **************/
local void amipctask( void *paramp )
{
    swi_uint32 loop;
    swi_uint32 vendid;     /* For use in calling usgetinfo() */
    swi_uint32 prodid;     /* For use in calling usgetinfo() */
    swi_uint16 amlength;   /* Length of the msg Data field within the IPC */
    swi_uint16 amtype;     /* Type of the incoming IPC */
    swi_uint8 *inipcmsgp;  /* Storage for incoming IPC packet buffer pointer */
    swi_uint8 *ipcmemfreep; /* Ptr to start of incoming IPC packet pointer */
    swi_uint16 amhdrversion; /* Header version contained in the AM packet */
    swi_uint8  amresultcode; /* Result code */
    swi_uint32         reqmsg;     /* Ptr to incoming msg from 
                                    * SMT requests msg que 
                                    */
    struct amevtblock *eventbkp;   /* Ptr to incoming requests event blk */   
    struct amevtblock *qeventbkp;  /* Ptr to event blk from local Q */   
    struct qllink      *curlinkp;
    struct qllink      *qlp;      /* Temp ptr to q event locally */
    
    /* Define and allocate the task control block */
    struct amipccb *amipccbp = amgetipccbp();
    
    /* Create the Event Pool for events received by AM task */
    mmpcreate( &amipccbp->amevtpool,
               "AMEVTPOOL", 
               AMEVTPOOLSZ, 
               sizeof(struct amevtblock) );

    /* Create the Notification Pool for notification registration per IPC
     * channel by AM task. Note that SWI_NOTIFY_END_NOTIFY is taken from 
     * SwiApiNotify.h which is an API-side include. If future changes break 
     * it such that other API-side includes are involved, we may need to 
     * separate out the definition of this constant. This guarantees that we 
     * have enough memory blocks for the Notification table.
     */
    mmpcreate( &amipccbp->amnotifpool,
               "AMNOTIFPOOL", 
               AMMAXNOTIFY, 
               sizeof(struct amipcnotifq) );
               
     /* Tell the world we've started up */
    dlLog(&amipccbp->amdlcb, DLCLASSALL, "IPC Task started\n",
         (swi_uint32)NULL,(swi_uint32)NULL,(swi_uint32)NULL,(swi_uint32)NULL);

    /* Initialize the SMT Requests message Q */
    icsmt_create( &amipccbp->amicmsgque, "AMMESSAGEQ", AMMSGDEPTH );
    
    /* Activate:
     *  - AM instances of CNRR channels for DS service
     *  - Notification channels 
     */
    amstartipcs();

    /* Disable logging now that we've started up, can be re-enabled via apui */
    dlmasterenable( &amipccbp->amdlcb, FALSE );
    
    amipccbp->amreqbusy = FALSE;
    
    /* Tell SU it's OK to start the next task in line */
    sutaskinitcb( SDK_AMPKG );

    /* Main AM task loop */
    for(loop=0;;loop++)
    {
        /* Wait to receive  a control message from  the Application
         * via DS IPC and AM receive handler
         */
        icsmt_rcv( &amipccbp->amicmsgque, AMSMTRCVTOUT, &reqmsg );
        if (reqmsg)
        {
            eventbkp = (struct amevtblock *) reqmsg;
            switch (eventbkp->amevent)
            {
                case AM_APP_RESP_SENT:     
                    /* Response from modem sent to application, flag to service
                     * next request
                     */
                    amipccbp->amreqbusy = FALSE;
                    dlLog (&amipccbp->amdlcb, DLCLASSALL,
                        "Resp sent evt recvd ch:%d\n", 
                        (swi_uint32)eventbkp->amipcchannel,
                        (swi_uint32)NULL,
                        (swi_uint32)NULL,
                        (swi_uint32)NULL);            
                        /* 
                         * No need to delete event blk, this event block is
                         * a canned-message.
                        */
                    break;
                case AM_APP_REQUEST:      /* Request from the Application */
                        /* Always put such requests on our local queue */
                        qlp = &eventbkp->ameqlnk;
                        qlinitlnk( qlp, (void *) eventbkp );

                        /* add link to end of queue */
                        qladdprev(&(amipccbp->ameventheadq), qlp);
                    break;
                default:
                    /* ignore unknown events */
                    dlLog (&amipccbp->amdlcb, DLCLASSALL,
                        "Ignoring unknown event %x\n", 
                        (swi_uint32)eventbkp->amevent,
                        (swi_uint32)NULL,
                        (swi_uint32)NULL,
                        (swi_uint32)NULL);            
                    break;
            } /* switch */

            /* If we're not busy, i.e., response from previous request sent 
             * to modem.
             */
            if ( !amipccbp->amreqbusy )
            {
                /* 
                 * Remove the next request from local Q and service it.
                 */
                curlinkp = &(amipccbp->ameventheadq);
                qeventbkp = qlremnext(curlinkp);
                if (qeventbkp)
                {
                    /* Dispatch requests accordingly */
                    amipccbp->amreqbusy = TRUE;
                    ipcmemfreep = (swi_uint8 *)qeventbkp;
                    inipcmsgp = qeventbkp->amdatap;
                    
                    /* Parse received packet header */
                    amparseamheader(&amtype,
                                  &amhdrversion, 
                                  &amresultcode, 
                                  &amlength,
                                  &inipcmsgp);
                                  
                    /* check to see if a device is actually connected */
                    if(!usgetinfo( &vendid, &prodid ))
                    {
                        /* No modem connected */
                        amShortResp( amtype, 
                                    SWI_AMNODEVICE, 
                                    qeventbkp->amipcchannel );
                        
                        dlLog( &amipccbp->amdlcb, AMLOG_CLASSA, 
                                "[%d] Request ignored, no modem\n", 
                                (swi_uint32)loop ,
                                (swi_uint32)NULL,
                                (swi_uint32)NULL,
                                (swi_uint32)NULL);
                    }
                    /* see if the SDK supports received AM header version */
                    else if ( amhdrversion != AMHDRVERSION )
                    {
                        /* log an error and tell the API side of the error */
                        dlLog( &amipccbp->amdlcb, AMLOG_CLASSA, 
                                "[%d] AM header mismatch, API: %d, SDK %d\n",
                                (swi_uint32)loop, 
                                (swi_uint32)amhdrversion, 
                                (swi_uint32)AMHDRVERSION,
                                (swi_uint32)NULL);

                        /* Send a packet back to the caller */
                        amShortResp( amtype, 
                                    SWI_AMVERSERROR, 
                                    qeventbkp->amipcchannel );
                    }
                    else
                    {   
                        /* Dispatch received request to handler */
                        amdispatchrequest(  amtype, 
                                            inipcmsgp, 
                                            NULL, 
                                            qeventbkp->amipcchannel);
                    }
                    /* Free up incoming blocks */
                    amreleventbk (qeventbkp);
                }
                else
                {
                    dlLog (&amipccbp->amdlcb, DLCLASSALL,
                        "No more events\n", 
                        (swi_uint32)NULL,
                        (swi_uint32)NULL,
                        (swi_uint32)NULL,
                        (swi_uint32)NULL);
                }
             } /* not busy */
        } /* rmsg */
    } /* for */
}

/*************
 *
 * Name:    aminit
 *
 * Purpose: Initialize internal variables within the AM package
 *          at SDK startup
 *             
 * Parms:   none
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
global void aminit( void )
{
    /* Nothing to do at this time */
    /* Define and allocate the task control block */
    struct amipccb *amipccbp = amgetipccbp();

    /* Initialize the head of notification registration queue */
    qlinitlnk(&(amipccbp->amnotifheadq.amnqlnk), NULL);

    /* Initialize the head of local event queue */
    qlinitlnk(&(amipccbp->ameventheadq), NULL);

    /* Initialize logging facility */
    dlregister( "AM", &amipccbp->amdlcb, FALSE);
}

/*************
 *
 * Name: amipctaskinit
 *
 * Purpose: Creates the task by making it known to the os package.
 *             
 * Parms:   none
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   Must be called *after* aminit() has been called
 *
 **************/
global void amipctaskinit( void )
{
    /* Create the AMIPC task */
    swi_ossdkthreadcreate( amipctask, NULL );
}

/*
 * $Log: amipctask_sdk.c,v $
 */

