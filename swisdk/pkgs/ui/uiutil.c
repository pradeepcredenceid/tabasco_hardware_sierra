/*************
 *
 * $Id: uiutil.c,v 1.4 2010/06/09 23:55:26 epasheva Exp $
 *
 * Filename: uiutil.c
 *
 * Purpose:  UI command line processing utility functions
 *
 * Copyright: © 2009 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* include files */
#include "ui/uiidefs.h"

/* Defines */

/*************
*
* Name:      uiOutput 
*
* Purpose:   To send a message/data string to the API using the UI task IPC 
*            channel
*             
* Parms:     msgp - pointer to the string (null terminated) to be sent
*
* Return:    None
*
* Abort:     None
*
* Notes:     
*
**************/
package void uiOutput(char *msgp)
{
    swi_uint32          numChars;
    swi_uint16          outputsize;
    swi_uint8           replybuf[ UIBUFSZ ];
    swi_uint8           *replyp;                   /* Pointer to reply buffer */
    swi_uint8           amresultcode = SWI_AMSUCCESS;
    swi_uint16          channel;
    
    /* Get pointer to the task control block */   
    struct uitaskcb     *uicbp = uigetcbp(); 

    slmemset ( (char *)&replybuf[0], 0, sizeof (replybuf));
    
    replyp = &replybuf[0];
    
    /* calculate number of characters in the string */
    numChars=slstrlen(msgp);

    /* Build the message to be sent to the API side. */
    outputsize =  ambuilduiresponse( &replyp, numChars, amresultcode);

    /* Add the data portion */
    slmemcpy ( (replyp+outputsize), msgp, numChars);

    /* send response to the host */
    channel = cigetchannelnum( CIIPCUI );
    dssend(&replybuf[0], outputsize+numChars, channel);
      
    dlLog(  &uicbp->uidlcb, UILOG_CLASSA, ">>API: %s",
            (swi_uint32)&replybuf[outputsize],
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL);
}

/*
 * $Log: uiutil.c,v $
 */

