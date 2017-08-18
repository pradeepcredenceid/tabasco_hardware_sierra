/*************
 *
 * $Id: hpfwdownload.c,v 1.11 2010/06/09 23:44:00 epasheva Exp $
 *
 * Filename: hpfwdownload.c
 * 
 * Purpose:  Contains Firmware Download state machine.
 *
 * Copyright: © 2008 Sierra Wireless, Inc. all rights reserved
 *
 **************/

/* include files */
#include "hp/hpidefs.h"
#include "am/amudefs.h"

/* constants */

/* 
 * Message specific parameter field use to indicate BootnHold is requested
 * in the HIP RESET request
 */
#define HPBOOTNHOLD    0x01 

/* 
 * Message specific parameter field receive in the HIP Loopback response
 * indicating that device is in BootnHold
 */
#define HPBOOTLOADER   0x00

/* 
 * Message specific parameter field indicating SUCCESSFUL request used
 * in a number of HIP responses received from the device.
 */
#define HPSUCCESS      0x00

/*
 * HIP payload length for the HIP End Request message
 */
#define HPDLENDREQSZ   0x02

/* 
 * Firmware Download response timeout interval in seconds.
 * This is how long the firmware download state
 * machine will wait to receive a response from
 * the device.
 */ 
#define HPFWDWNRESPTIMEOUT 600 

/*
 * Name:    hpblk
 *
 * Purpose: Structure contains all global variables for the firmware
 *          download module.
 *
 * Members: hpdevchgreqblk - modem change state request block
 *          hpparms        - Received AM Firmware Download request info
 *          hpfileopened   - TRUE if image file is open, FALSE otherwise
 *          hpfp           - image file pointer
 *          hpchecksum     - accumulate checksum for current download
 *          hpchunks2send  - number of chunks of the image left to send
 *          hptimeron      - device response timeout timer ON state
 *          hpticksleft    - ticks left before ON timer expires
 *
 * Note:    The global instance of the structure can be initialized 
 *          by hpstateclear()
 *
 */
struct hpblk
{
    struct hpmdmreghndl       hpdevchgreqblk;
    struct amfwdwnrrparms     hpparms;
    swi_bool                  hpfileopened;
    struct swi_osfile         hpfp; 
    swi_uint16                hpchecksum;    
    swi_uint16                hpchunks2send;
    swi_bool                  hptimeron;
    swi_uint16                hpticksleft;
};

/* HP Firmware Download Global control block */
struct hpblk hpgcb;

/* 
 * Forward Local Prototypes 
 * Having these prototypes here allow for the module to be
 * organized by groups of related functions
 */
local void hpstateupdate(
    enum swi_amfwdwnnotiftype newstate,
    swi_uint8 *errorstringp,
    swi_uint8 errorstringlen);
local void hpstateclear(void);

/* 
 * Utility functions
 */

/*************
 * Name:    hpadd2checksum
 *
 * Purpose: To add to the cumulative checksum for the current
 *          firmware download request been procesed.
 *
 * Parms:   payloadp - Pointer to chunk of data to do the checksum on
 *          length   - length of the data.

 * Return:  None
 *
 * Note:    This function is called from the HPPH task context.
 *
 */
local void hpadd2checksum(
    swi_uint8 *payloadp,
    swi_uint16 length)
{
    int i;  /* loop counter */
    
    /* loop through the passed in data */
    for(i=0; i < length; i++)
    {
        /* checksum is calculated by summing all bytes of the payload */
        hpgcb.hpchecksum += *payloadp++;
    }
}

/*
 * Firmware Download transaction functions
 */

/*************
 * Name:    hpamsendshortresp
 *
 * Purpose: This function is called to reject a received Firmware Download
 *          request from the API because there is already a firmware being
 *          downloaded by this module.  
 * 
 *          This function will not clear the Firmware Download state variables.
 *
 * Parms:   None
 *
 * Return:  None
 *
 * Note:    This function is called from the HPPH task context.
 *
 */
local void hpamsendshortresp(void)
{
    swi_uint16 outputsize;
    swi_uint8  outputbuf[AMFWDWNRESPTOTAL];
    swi_uint8  *outputbufp = &outputbuf[0];
    
    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();

    /* prepare outgoing AM firmare download response packet */
    outputsize =  ambuildfwdwnresponse( &outputbufp, SWI_AMFWDWNBUSY, NULL, 0);
    
    /* send "in-progress" response to the host */
    amsdk_send( &outputbuf[0], outputsize, hpphcbp->hpphipcchanrr );
}
/*************
 * Name:    hpamfwdwnresp
 *
 * Purpose: This function is called to end a Firmware Download transaction.
 *          It can be used only if the Firmware Download procedure was kicked 
 *          off in hpamfwdwnreq() by changing the state to SWI_FW_INIT.
 * 
 *          This function will clear the Firmware Download state variables 
 *          after it sends the  AM Firmware Download response to the API.
 * 
 * Parms:   resultcode     - result of the firmware download procedure
 *          errorstringp   - string that may indicate the reason for a firmware
 *                           download error result (not-NULL terminated ASCII 
 *                           string)
 *          errorstringlen - length of the error string (number of ASCII bytes
 *                           not including NULL-termination)
 *
 * Return:  None
 *
 * Note:    This function is called from the HPPH task context.
 *
 */
local void hpamfwdwnresp(
   enum swi_amresults resultcode,
   swi_uint8 *errorstringp,
   swi_uint8 errorstringlen)
{
    swi_uint16 outputsize;
    swi_uint8  outputbuf[AMFWDWNRESPTOTAL];
    swi_uint8  *outputbufp = &outputbuf[0];

    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();
    
    dlLog(  &hpphcbp->hpphdluser, DLCLASSALL, 
            "Constructing firmware download response %d", 
            (swi_uint32)resultcode,
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL);
    
    /* prepare outgoing AM firmare download response packet */
    outputsize =  ambuildfwdwnresponse( &outputbufp, 
                      resultcode,
                      errorstringp,
                      errorstringlen);
    
    /* send response to the host */
    amsdk_send( &outputbuf[0], outputsize, hpphcbp->hpphipcchanrr);

    /* if the image file is open */
    if(hpgcb.hpfileopened)
    {
        /* close image file */
        swi_ossdkfileclose(&hpgcb.hpfp);
    }
    
    /* clear state variables */
    hpstateclear();
}
/*************
 * Name:    hpamfwdwnreq
 *
 * Purpose: This function is called when a HPPHFWDWNREQ event is received
 *          by the HPPH task.  It parses the received AM Firmware Download
 *          request and kicks off the Firmware Download procedure moving the 
 *          state machine into SWI_FW_INIT.
 * 
 *          If the state machine is not currently in SWI_FW_DONE then
 *          another firmware download procedure must be in progress.
 *
 * Parms:   hipmsgp - pointer to received HIP message
 *
 * Return:  None
 *
 * Note:    This function is called from the HPPH task context.
 *
 */
package void hpamfwdwnreq(swi_uint8 *packetp)
{
    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();

/* Parse the contents of the firmware download request packet */
    amparsefwdwnrequest( &packetp, &hpgcb.hpparms );

    /* Log the new firmware download request */
    dlLog(  &hpphcbp->hpphdluser, DLCLASSALL, 
            "Firmware download request received",
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL);
                                    
    /* check if a firmware download is currently in progress */
    if (hpgcb.hpparms.amfwdwnstate != SWI_FW_DONE)
    {
        dlLog(  &hpphcbp->hpphdluser, DLCLASSALL, 
                "FWDWN: INIT ERR: download already in progress",
                (swi_uint32)NULL,
                (swi_uint32)NULL,
                (swi_uint32)NULL,
                (swi_uint32)NULL);
            
        /* send short response to API indicating an error */
        hpamsendshortresp();
        return;
    }
    
    /* Log that firmware download is initiated */
    dlLog(  &hpphcbp->hpphdluser, DLCLASSALL, 
            "FWDWN: INIT: kicking off download procedure",
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL);
            
    /* start the process by verifying the given image file */
    hpstateupdate(SWI_FW_INIT, NULL, 0);
}


/*************
 * Name:    hpamfwdwnnotify
 *
 * Purpose: This function sends an AM Firmware Download Notification packet
 *          to the API.  This notification only contains the progress of
 *          the current firmware download procedure.  
 * 
 * Parms:   progress     - current firmware download state
 *
 * Return:  None
 *
 * Note:    This function is called from the HPPH task context.
 *
 */
local void hpamfwdwnnotify(enum swi_amfwdwnnotiftype progress)
{
    swi_uint16 outputsize;
    swi_uint8  outputbuf[AMFWDWNNOTFTOTAL];
    swi_uint8  *outputbufp = &outputbuf[0];
    swi_uint16 notifchan;
    
    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();
    
    dlLog(  &hpphcbp->hpphdluser, DLCLASSALL, 
            "Constructing firmware download notification %d", 
            (swi_uint32)progress,
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL);
        
    /* prepare outgoing AM firmare download response packet */
    outputsize =  ambuildfwdwnnotify( &outputbufp, progress);
    
    /* Get the corresponding Notification channel */
    notifchan = cigetpeerchan (hpphcbp->hpphipcchanrr);
     
    /* send response to the host */
    amsdk_sendnot ( &outputbuf[0], outputsize, notifchan);
}
/*
 * Timer functions
 */
/*************
 * Name:    hpfwdwntimer
 *
 * Purpose: This function receives timer ticks from HPPH main task loop.
 *          It is responsible for decrementing the Firmware Download timer
 *          tick variable when the timer is "ON".
 *
 * Parms:   None

 * Return:  None
 *
 * Note:    This function is called from the HPPH task context.
 *
 */
package void hpfwdwntimer(void)
{
    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();

/* if the Firmware Download timer is not "ON".  */
    if (!hpgcb.hptimeron)
    {
        /* exit function */
        return;
    }
    
    /* decrement number of ticks left to wait */
    hpgcb.hpticksleft -= 1;
    
    /* if there are no more ticks to wait */
    if(hpgcb.hpticksleft == 0)
    {
        /* log the expiring of the timer */
        dlLog(  &hpphcbp->hpphdluser, DLCLASSALL, 
                "FWDWN: Timed out waiting for modem's response",
                (swi_uint32)NULL,
                (swi_uint32)NULL,
                (swi_uint32)NULL,
                (swi_uint32)NULL);
            
        /* 
         * End the processing of the current firmware download request
         * due to the SDK timing out waiting for the modem's response
         */
        hpamfwdwnresp(SWI_AMTIMEOUT, NULL, 0);
    }
}
/*************
 * Name:    hptimerstart
 *
 * Purpose: To start the Firmware Download module counting down HPPH timer
 *          ticks.  This turns the Firmware Download timer "ON".  If all ticks
 *          are counted before hptimerstop() is called the timer effectively
 *          expires.
 *
 * Parms:   None

 * Return:  None
 *
 * Note:    This function is called from the HPPH task context.
 *
 */
local void hptimerstart(void)
{
    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();

    /* 
     * Calculate the number of ticks required to reach the Firmware
     * Download response timeout value in seconds 
     */
    hpgcb.hpticksleft = ((HPFWDWNRESPTIMEOUT * 1000) / ICTTTICK) + 1;
    
    /* Log the starting of the timer */
    dlLog(  &hpphcbp->hpphdluser, DLCLASSALL, 
            "FWDWN: Starting response timeout timer %d ticks", 
            (swi_uint32)hpgcb.hpticksleft,
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL);
    
    /* Start counting ticks */
    hpgcb.hptimeron = TRUE;
}
/*************
 * Name:    hptimerstop
 *
 * Purpose: To stop the Firmware Download module from counting down HPPH timer
 *          ticks.  This turns the Firmware Download timer "OFF".  
 *
 * Parms:   None

 * Return:  None
 *
 * Note:    This function is called from the HPPH task context.
 *
 */
local void hptimerstop(void)
{
    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();

    /* Log the stopping of the timer */
    dlLog(  &hpphcbp->hpphdluser, DLCLASSALL, 
            "FWDWN: Stopping response timeout timer",
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL);
    
    /* Stop counting ticks */
    hpgcb.hptimeron = FALSE;
}


/*
 * SEND HIP packet functions to the device
 */
/*************
 * Name:    hpsendreset
 *
 * Purpose: This function sends a HIP Reset message to the device for the
 *          SWI_FW_SOFT_RESET state.  The Firmware Download response timer
 *          is started once the message has been sent.
 *
 * Parms:   None
 *
 * Return:  None
 *
 * Note:    This function is called from the HPPH task context.
 *
 */
local void hpsendreset(void)
{
    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();

    swi_uint8 *bufp            = NULL; 
    
    /* Get a HIP transmission buffer */
    bufp = hpgetbuf();
    
    /* Log the sending of the HIP RESET message */
    dlLog(  &hpphcbp->hpphdluser, DLCLASSALL, 
            "FWDWN: Sending HIP RESET",
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL);
    
    /* Send reset request to the device */
    hpTxPacket( HPRESETREQ, HPBOOTNHOLD, bufp, 0 );
    
    /* Start response timeout timer */
    hptimerstart();
}
/*************
 * Name:    hpsendstartreq
 *
 * Purpose: This function sends a HIP Start Request message to the device 
 *          for the SWI_FW_START_REQ state.  
 * 
 *          The Firmware Download response timer is started once the 
 *          message has been sent.
 *
 * Parms:   None
 *
 * Return:  None
 *
 * Note:    This function is called from the HPPH task context.
 *
 */
local void hpsendstartreq(void)
{
    swi_uint8 *bufp       = NULL; 
    swi_uint8 *payloadp   = NULL;
    swi_size_t length     = (swi_size_t)hpgcb.hpparms.ammaxchunksize;
    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();
    
    /* Get a HIP transmission buffer */
    bufp = hpgetbuf();

    /* Get a pointer to the payload portion */
    payloadp = hpgetpayp( bufp, HPDLSTRTREQ); 
    
    /* copy a chunk size of data from the image file into the HIP packet */
    if(!swi_ossdkfileread(&hpgcb.hpfp, payloadp, &length))
    {
        /* 
         * if read failed then move from SWI_FW_START_REQ to 
         * SWI_FW_START_REQ_ERR
         */
        hpstateupdate(SWI_FW_START_REQ_ERR, NULL, 0);
        return;
    }
    
    /* Add payload info to running checksum */
    hpadd2checksum(payloadp, length);
    
    /* Decrement the number of chunks left to be written */
    hpgcb.hpchunks2send--;
    
    /* Log sending of the HIP start request */
    dlLog(  &hpphcbp->hpphdluser, DLCLASSALL, 
            "FWDWN: Sending HIP Start Req",
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL);
    
    /* Send reset request to the device */
    hpTxPacket( HPDLSTRTREQ, 0x00, bufp, length);
    
    /* Start response timeout timer */
    hptimerstart();
}
/*************
 * Name:    hpsendcontreq
 *
 * Purpose: This function sends a HIP Continuation Request message to the   
 *          device for the SWI_FW_DWN state.  
 * 
 *          The Firmware Download response timer is started once the 
 *          message has been sent.
 *
 * Parms:   None
 *
 * Return:  None
 *
 * Note:    This function is called from the HPPH task context.
 *
 */
local void hpsendcontreq(void)
{
    swi_uint8 *bufp       = NULL; 
    swi_uint8 *payloadp   = NULL;
    swi_size_t length     = (swi_size_t)hpgcb.hpparms.ammaxchunksize;
    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();
    
    /* Get a HIP transmission buffer */
    bufp = hpgetbuf();

    /* Get a pointer to the payload portion */
    payloadp = hpgetpayp( bufp, HPDLCONTREQ); 

    /* copy a chunk size of data from the image file into the HIP packet */
    if(!swi_ossdkfileread(&hpgcb.hpfp, payloadp, &length))
    {
        /* 
         * if read failed then move from SWI_FW_DWN to 
         * SWI_FW_DWN_ERR
         */
        hpstateupdate(SWI_FW_DWN_ERR, NULL, 0);
        return;
    }
    
    /* Add payload info to running checksum */
    hpadd2checksum(payloadp, length);
    
    /* Decrement the nubmer of chunks left to be written */
    hpgcb.hpchunks2send--;

    /* Log Sending of HIP Continuation Request */
    dlLog(  &hpphcbp->hpphdluser, DLCLASSALL, 
            "FWDWN: Sending HIP Continue Req left[%d]", 
            (swi_uint32)hpgcb.hpchunks2send,
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL);
            
    /* Send reset request to the device */
    hpTxPacket( HPDLCONTREQ, 0x00, bufp, length );
}
/*************
 * Name:    hpsendendreq
 *
 * Purpose: This function sends a HIP End Request message to the   
 *          device for the SWI_FW_CHKSUM state.  
 * 
 *          The Firmware Download response timer is started once the 
 *          message has been sent.
 *
 * Parms:   None
 *
 * Return:  None
 *
 * Note:    This function is called from the HPPH task context.
 *
 */
local void hpsendendreq(void)
{
    swi_uint8 *bufp            = NULL; 
    swi_uint8 *payloadp        = NULL;
    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();
    
    /* Get a HIP transmission buffer */
    bufp = hpgetbuf();

    /* Get a pointer to the payload portion */
    payloadp = hpgetpayp( bufp, HPDLENDREQ); 

    /* Copy downloaded image checksum into payload */
    piput16(hpgcb.hpchecksum, &payloadp);

    /* Log sending of HIP End Request */
    dlLog(  &hpphcbp->hpphdluser, DLCLASSALL, 
            "FWDWN: Sending HIP End Req checksum[%x]", 
            (swi_uint32)hpgcb.hpchecksum,
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL);
    
    /* Send reset request to the device */
    hpTxPacket( HPDLENDREQ, 0x00, bufp, HPDLENDREQSZ);
    
    /* Start response timeout timer */
    hptimerstart();
}
/*************
 * Name:    hpsendlaunchind
 *
 * Purpose: This function sends a HIP Launch Fragment indication message to the   
 *          device for the SWI_FW_LAUNCH_FRAG state.  
 * 
 *          No timer is started for this message because there will be no
 *          modem response to the HIP Launch Fragment indication.
 *          
 *
 * Parms:   None
 *
 * Return:  None
 *
 * Note:    This function is called from the HPPH task context.
 *
 */
local void hpsendlaunchind(void)
{
    swi_uint8 *bufp            = NULL; 
    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();
    
    /* Get a HIP transmission buffer */
    bufp = hpgetbuf();
    
    /* Log sending HIP Launch Fragment indication */
    dlLog(  &hpphcbp->hpphdluser, DLCLASSALL, 
            "FWDWN: Sending HIP Launch Ind",
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL);
    
    /* Send reset request to the device */
    hpTxPacket( HPMLAUNIND, 0x00, bufp, 0 );
}
/*************
 * Name:    hpsendprogreq
 *
 * Purpose: This function sends a HIP Flash Program  request message to the   
 *          device for the SWI_FW_FLASH state.  
 * 
 *          The Firmware Download response timer is started once the 
 *          message has been sent.
 *
 * Parms:   None
 *
 * Return:  None
 *
 * Note:    This function is called from the HPPH task context.
 *
 */
local void hpsendprogreq(void)
{
    swi_uint8 *bufp            = NULL; 
    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();
    
    /* Get a HIP transmission buffer */
    bufp = hpgetbuf();
    
    /* Log sending HIP Flash Program request to the device */
    dlLog(  &hpphcbp->hpphdluser, DLCLASSALL, 
            "FWDWN: Sending HIP Flash Program",
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL);
    
    /* Send reset request to the device */
    hpTxPacket( HPPROGREQ, 0x00, bufp, 0 );
   
    /* Start response timeout timer */
    hptimerstart();
}

/*
 * Firmware download image file functions 
 */

/*************
 * Name:    hpimageinit
 *
 * Purpose: This function takes the action of the SWI_FW_INIT state.  It is 
 *          responsible for initializing the firmware download image file
 *          variable in the global control block.
 * 
 *          State changes from SWI_FW_INIT:
 *              - SWI_FW_INIT_ERR if the firmware download image file can not 
 *              be opened or is found to be empty
 *              - SWI_FW_START_REQ if the device is in Boot-n-Hold mode
 *              - SWI_FW_SOFT_RESET if the device is running an Application
 *
 * Parms:   None
 *
 * Return:  None
 *
 * Note:    This function is called from the HPPH task context.
 *
 */
local void hpimageinit(void)
{
    swi_size_t filesize;
    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();
   
    /* attempt to open file */ 
    if(!swi_ossdkfileopenread(&hpgcb.hpfp, hpgcb.hpparms.amfilename))
    {
        dlLog(  &hpphcbp->hpphdluser, DLCLASSALL, 
                "FWDWN: INIT->INIT ERR: could not open file",
                (swi_uint32)NULL,
                (swi_uint32)NULL,
                (swi_uint32)NULL,
                (swi_uint32)NULL);
        /* send error back to API */
        hpstateupdate(SWI_FW_INIT_ERR, NULL, 0);
        return;
    }
    
    /* get the size of the file */
    if(!swi_ossdkfilesize(&hpgcb.hpfp, &filesize))
    {
        dlLog(  &hpphcbp->hpphdluser, DLCLASSALL, 
                "FWDWN: INIT->INIT ERR: could not get file size",
                (swi_uint32)NULL,
                (swi_uint32)NULL,
                (swi_uint32)NULL,
                (swi_uint32)NULL);
        hpstateupdate(SWI_FW_INIT_ERR, NULL, 0);
        return;
    }
    
    /* set open file flag to TRUE */
    hpgcb.hpfileopened = TRUE;
    
    /* if the file is empty */
    if(filesize == 0)
    {
        dlLog(  &hpphcbp->hpphdluser, DLCLASSALL, 
                "FWDWN: INIT->INIT ERR: file is empty",
                (swi_uint32)NULL,
                (swi_uint32)NULL,
                (swi_uint32)NULL,
                (swi_uint32)NULL);
        hpstateupdate(SWI_FW_INIT_ERR, NULL, 0);
        return;
    }
    
    /* 
     * Calculate the number of send request necessary to copy the entire 
     *  image to the device 
     */
    hpgcb.hpchunks2send = 
    (filesize + hpgcb.hpparms.ammaxchunksize - 1)/hpgcb.hpparms.ammaxchunksize;
    
    /* if the application is running */
    if(hpapprunning())
    {
        /* Log the changing of the state */
        dlLog(  &hpphcbp->hpphdluser, DLCLASSALL,
                "FWDWN: INIT->RESET",
                (swi_uint32)NULL,
                (swi_uint32)NULL,
                (swi_uint32)NULL,
                (swi_uint32)NULL);
    
        /* move to the next state */
        hpstateupdate(SWI_FW_SOFT_RESET, NULL, 0);
    }
    /* else already in boot-n-hold mode */
    else
    {
        /* Log the state change */
        dlLog(  &hpphcbp->hpphdluser, DLCLASSALL,
                "FWDWN: INIT->START REQ",
                (swi_uint32)NULL,
                (swi_uint32)NULL,
                (swi_uint32)NULL,
                (swi_uint32)NULL);
        
        /* move to the next state: SWI_FW_START_REQ */
        hpstateupdate(SWI_FW_START_REQ, NULL, 0);
    }
}

/*
 * Firmware Download state functions
 */

/*************
 * Name:    hpstateclear
 *
 * Purpose: This function clears all Firmware Download state variables.
 *          It is called at started up to initialize the global control
 *          block and used to reinitize this block after every firmware
 *          download attempt.
 *
 * Parms:   None
 *
 * Return:  None
 *
 * Note:    This function is called from the HPPH task context.
 *
 */
local void hpstateclear(void)
{
    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();

    /* Log that the Firmware Download state variable are cleared */
    dlLog(  &hpphcbp->hpphdluser, DLCLASSALL, 
            "Clearing Firmware download state variables",
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL);
        
    /* Clear all state variables */
    hpgcb.hpfileopened = FALSE;
    hpgcb.hpparms.amfwdwnstate = SWI_FW_DONE;
    slmemset(hpgcb.hpparms.amfilename, 0, AMMAXFILENAMESZ);
    hpgcb.hpchecksum = 0;
    hpgcb.hpchunks2send = 0;
    hpgcb.hptimeron = FALSE;
    
    hpphcbp->hpphipcchanrr  = CIMAXIPCCHAN;  /* set to invalid index */
    hpphcbp->hpphipcchannot = CIMAXIPCCHAN;  /* set to invalid index */
}

/*************
 * Name:    hpstateupdate
 *
 * Purpose: This function is the main state machine for the Firmware Download
 *          procedure.  It is called to move from one state to another,  
 *          changing states always initiates an action.  
 *          
 *          A Firmware Download notification will be sent to the API every
 *          time this function is called.
 * 
 *          It is this function that completes the Firmware Download procedure
 *          by sending a Firmware Download Response.  It will do this if
 *          an error has occurred or if the download has been successful.  The
 *          hpfwdwntimer() function can also complete the Firmware Download 
 *          procedure if the SDK times out waiting for a response from the 
 *          device.
 *
 * Parms:   newstate       - new state
 *          errorstringp   - error string that may be present to explain the
 *                         new state if the new state is an error one.
 *          errorstringlen - error string length (number of ASCII bytes not 
 *                         including a NULL-terminator)
 *
 * Return:  None
 *
 * Note:    This function is called from the HPPH task context.
 *
 */
local void hpstateupdate(
   enum swi_amfwdwnnotiftype newstate,
   swi_uint8 *errorstringp,
   swi_uint8 errorstringlen)
{
    /* store the new state in the global control block */
    hpgcb.hpparms.amfwdwnstate = newstate;
    
    /* Send Firmware Download Progress indication to the API. */
    hpamfwdwnnotify(newstate);
    
    /* take the required action given the new state */
    switch(newstate)
    {
        case  SWI_FW_INIT:
            /* 
             * Validate that gvien Image file can be opened 
             */
            hpimageinit();
            break;
        
        case SWI_FW_SOFT_RESET:
            /* 
             * Reset the device to bring it into boot-n-hold mode
             */
            hpsendreset();
            break;
            
        case SWI_FW_START_REQ:
            /*
             * Send the HIP Firmware Download start request
             * message to the device.
             */
            hpsendstartreq();
            break;
            
        case SWI_FW_DWN:
            /*
             * Send the HIP Firmware Download continue request
             * message to the device.
             */
            hpsendcontreq();
            break;
        
        case SWI_FW_CHKSUM:
            /* 
             * Send the HIP Firmware Download End request 
             * message to the device.
             */
            hpsendendreq();
            break;
            
        case SWI_FW_FLASH:
            /*
             * Send HIP Flash Program request message
             * to the device.
             */
             hpsendprogreq();
             break;
             
        case SWI_FW_LAUNCH_FRAG:
            /*
             * Send HIP Launch Fragment indication message
             * to the device.
             */
            hpsendlaunchind();
            break;
              
        case SWI_FW_DONE:
            /*
            * The Firmware Download has been sucessfully completed.
            * Send a Firmware Download Response AM packet to
            * indicate to the API that this transaction has ended
            * successfully.  
            */
            hpamfwdwnresp(SWI_AMSUCCESS, NULL, 0);
            break;
                
        case SWI_FW_INIT_ERR:
        case SWI_FW_SOFT_RESET_ERR:
        case SWI_FW_START_REQ_ERR:
        case SWI_FW_FILE_ERR:
        case SWI_FW_DWN_ERR:
        case SWI_FW_CHKSUM_ERR:
        case SWI_FW_FLASH_ERR:
        case SWI_FW_RESET_ERR:
           /* 
            * The Firmware Download was not successful.
            * Send a Firmware Download Response AM packet
            * to indicate to the API that this transaction ended
            * in error.
            */
            hpamfwdwnresp(SWI_AMFWDWNABORTED, errorstringp, errorstringlen);
           break;
        
        default:
           break;
    }
}
/*************
 * Name:    hpstatesoftreset
 *
 * Purpose: This function verifies the action taken by the SWI_FW_SOFT_RESET 
 *          state.  It is called after the device has completed its reset.
 *          It is responsible for detemining if the modem is in Boot-n-Hold 
 *          mode.
 * 
 *          State changes from SWI_FW_SOFT_RESET:
 *              - SWI_FW_SOFT_RESET_ERR if the modem is not in Boot-n-Hold mode
 *              - SWI_FW_START_REQ if everything is in order
 *
 * Parms:   eventbkp - pointer to received PH event block
 * 
 * Return:  None
 *
 * Note:    This function is called from the HPPH task context.
 *
 */
local void hpstatesoftreset(struct hpphevtblock  *eventbkp)
{
    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();

    /* if the device has come back up after the reset request */
    if(eventbkp->hpevtmember.hpdevchg.modemstate == HPMDMONLINE)
    {
        /* Stop timeout timer since device response has been received */
        hptimerstop();
                
        /* if the device is not in Boot-n-Hold mode */
        if(eventbkp->hpevtmember.hpdevchg.apprunning)
        {
            /* Log the Reset error */
            dlLog(  &hpphcbp->hpphdluser, DLCLASSALL, 
                    "FWDWN: RESET->RESET ERR",
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL);
            
            /* Move to the error state */
            hpstateupdate(SWI_FW_SOFT_RESET_ERR, NULL, 0);
        }
        /* else in Boot-n-Hold mode */
        else
        {
            /* Log the state change */
            dlLog(  &hpphcbp->hpphdluser, DLCLASSALL,  
                    "FWDWN: RESET->START REQ",
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL);
                
            /* move from SWI_FW_SOFT_RESET to SWI_FW_START_REQ */
            hpstateupdate(SWI_FW_START_REQ, NULL, 0);  
        }
    }
}
/*************
 * Name:    hpstatefwstartreq
 *
 * Purpose: This function verifies the action taken by the SWI_FW_START_REQ 
 *          state.  It is called when the HIP Firmware Download Start
 *          Response message is received.  It is responsible for stopping
 *          the firmware download response timeout timer and determining the
 *          success of the HIP Firmware Download Start Request.
 * 
 *          State changes from SWI_FW_START_REQ:
 *              - SWI_FW_START_REQ_ERR if the Start Request failed
 *              - SWI_FW_DWN if there is more of the image to download
 *              - SWI_FW_CHKSUM if there is no more of the image to download
 *
 * Parms:   hipmsgp - pointer to received HIP message
 * 
 * Return:  None
 *
 * Note:    This function is called from the HPPH task context.
 *
 */
local void hpstatefwstartreq(struct hphipevt *hipmsgp)
{
    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();

    /* if the received HIP message is the Start Response */
    if(hipmsgp->hpphmsgdesignator == HPDLSTRTRESP)
    {
        /* Stop timeout timer since device response has been received */
        hptimerstop();
    
        /* if the request was not successful */
        if(hipmsgp->hpphmsgparm != HPSUCCESS)
        {
            /* Log the error state change */
            dlLog(  &hpphcbp->hpphdluser, DLCLASSALL, 
                    "FWDWN: START REQ->START ERR",
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL);
            
            /* move from SWI_FW_START_REQ to SWI_FW_START_REQ_ERR */
            hpstateupdate(SWI_FW_START_REQ_ERR, 
                hipmsgp->hpphdatap, hipmsgp->hpphdatasize);
            return;
        }
        
        /* check if there is more to send */
        if(hpgcb.hpchunks2send)
        {
            /* Log the state change */
            dlLog(  &hpphcbp->hpphdluser, DLCLASSALL, 
                    "FWDWN: START REQ->CONT REQ",
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL);
            
            /* move from SWI_FW_START_REQ to SWI_FW_DWN */
            hpstateupdate(SWI_FW_DWN, NULL, 0);
        }
        /* if there is nothing left to send */
        else
        {
            /* Log the stage change */
            dlLog(  &hpphcbp->hpphdluser, DLCLASSALL,
                    "FWDWN: START REQ->END REQ",
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL);
            
            /* move from SWI_FW_START_REQ to SWI_FW_CHKSUM */
            hpstateupdate(SWI_FW_CHKSUM, NULL, 0);
        }
    }
}
/*************
 * Name:    hpstatefwdownload
 *
 * Purpose: This function verifies the action taken by the SWI_FW_CONT_REQ 
 *          state.  It is called when the HIP Firmware Download Continuation
 *          Response message is received.  It is responsible for stopping
 *          the firmware download response timeout timer and determining the
 *          success of the HIP Firmware Download Continuation Request.
 * 
 *          State changes from SWI_FW_DWN:
 *              - SWI_FW_DWN_ERR if the Continuation Request failed
 *              - SWI_FW_DWN if there is more of the image to download
 *              - SWI_FW_CHKSUM if there is no more of the image to download
 *
 * Parms:   hipmsgp - pointer to received HIP message
 * 
 * Return:  None
 *
 * Note:    This function is called from the HPPH task context.
 *
 */
local void hpstatefwdownload(struct hphipevt *hipmsgp)
{
    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();

    /* if the received HIP message is the Continuation Response */
    if (hipmsgp->hpphmsgdesignator == HPDLCONTRESP)
    {
        /* Stop timeout timer since device response has been received */
        hptimerstop();
        
        /* if the request was not successful */
        if(hipmsgp->hpphmsgparm != HPSUCCESS)
        {
            /* Log the error state change */
            dlLog(  &hpphcbp->hpphdluser, DLCLASSALL, 
                    "FWDWN: CONT REQ->CONT ERR",
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL);
            
            /* Move from SWI_FW_DWN to SWI_FW_DWN_ERR */
            hpstateupdate(SWI_FW_DWN_ERR, 
                hipmsgp->hpphdatap, hipmsgp->hpphdatasize);
            return;
        }
        
        /* Check if there is more to send */
        if(hpgcb.hpchunks2send)
        {
            /* Log the not changing state */
            dlLog(  &hpphcbp->hpphdluser, DLCLASSALL, 
                    "FWDWN: CONT REQ->CONT REQ",
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL);
            
            /* 
             * Move from SWI_FW_DWN to SWI_FW_DWN
             * State machine will circle through this state until
             * the entire image has been downloaded to the device 
             */
            hpstateupdate(SWI_FW_DWN, NULL, 0);
        }
        /* if there is no more to send */
        else
        {
            /* Log the changing state */
            dlLog(  &hpphcbp->hpphdluser, DLCLASSALL, 
                    "FWDWN: CONT REQ->END REQ",
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL);
            
            /* Move from SWI_FW_DWN to SWI_FW_CHKSUM */
            hpstateupdate(SWI_FW_CHKSUM, NULL, 0);
        }
    }
}
/*************
 * Name:    hpstatefwchksum
 *
 * Purpose: This function verifies the action taken by the SWI_FW_CHKSUM 
 *          state.  It is called when the HIP Firmware Download End
 *          Response message is received.  It is responsible for stopping
 *          the firmware download response timeout timer and determining the
 *          success of the HIP Firmware Download End Request.
 * 
 *          State changes from SWI_FW_CHKSUM:
 *              - SWI_FW_CHKSUM_ERR if the End Request failed
 *              - SWI_FW_FLASH if an application is being downloaded
 *              - SWI_FW_LAUNCH_FRAG otherwise and then immediately moved on
 *                to SWI_FW_DONE
 *
 * Parms:   hipmsgp - pointer to received HIP message
 * 
 * Return:  None
 *
 * Note:    This function is called from the HPPH task context.
 *
 */
local void hpstatefwchksum(struct hphipevt *hipmsgp)
{
    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();

    /* if the received HIP message is the Continuation Response */
    if(hipmsgp->hpphmsgdesignator == HPDLENDRESP)
    {
        /* Stop timeout timer since device response has been received */
        hptimerstop();
        
        /* if the request was not successful */
        if(hipmsgp->hpphmsgparm != HPSUCCESS)
        {
            /* Log error state change */
            dlLog(  &hpphcbp->hpphdluser, DLCLASSALL, 
                    "FWDWN: END REQ->END ERR",
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL);
            
            /* Move from SWI_FW_CHKSUM to SWI_FW_CHKSUM_ERR */
            hpstateupdate(SWI_FW_CHKSUM_ERR, 
                hipmsgp->hpphdatap, hipmsgp->hpphdatasize);
            return;
        }
              
        /* if an application image was downloaded */
        if(hpgcb.hpparms.amfwimagetype == SWI_FW_DWN_APPL)
        {
            /* Log the state change */
            dlLog(  &hpphcbp->hpphdluser, DLCLASSALL, 
                    "FWDWN: END REQ->FLASH PROGRAM",
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL);
            
            /* Move from SWI_FW_CHKSUM to SWI_FW_FLASH */
            hpstateupdate(SWI_FW_FLASH, NULL, 0);
        }
        /* not an application image */
        else
        {   
            /* log state change */
            dlLog(  &hpphcbp->hpphdluser, DLCLASSALL, 
                    "FWDWN: END REQ->LAUNCH FRAG",
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL);
            
            /* Move from SWI_FW_CHKSUM to SWI_FW_LAUNCH_FRAG */
            hpstateupdate(SWI_FW_LAUNCH_FRAG, NULL, 0);
            
            /* Log final state change */
            dlLog(  &hpphcbp->hpphdluser, DLCLASSALL, 
                    "FWDWN: END REQ->DONE!!",
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL);
            
            /* 
             * Move immediately from SWI_FW_LAUNCH_FRAG to SWI_FW_DONE
             * since there is not modem response to SWI_FW_LAUNCH_FRAG
             */
            hpstateupdate(SWI_FW_DONE, NULL, 0);
        }
    }
}
/*************
 * Name:    hpstatefwflash
 *
 * Purpose: This function verifies the action taken by the SWI_FW_FLASH 
 *          state.  It is called when the HIP Firmware Download Flash Program
 *          Response message is received.  It is responsible for stopping
 *          the firmware download response timeout timer and determining the
 *          success of the HIP Firmware Download Flash Program Request.
 * 
 *          State changes from SWI_FW_FLASH:
 *              - SWI_FW_FLASH_ERR if the Flash Program Request failed
 *              - SWI_FW_DONE otherwise (this is the final state)
 *
 * Parms:   hipmsgp - pointer to received HIP message
 *
 * Return:  None
 *
 * Note:    This function is called from the HPPH task context.
 *
 */
local void hpstatefwflash(struct hphipevt *hipmsgp)
{
    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();

    /* if the received HIP message is the Flash Program Response */
    if(hipmsgp->hpphmsgdesignator == HPPROGRESP)
    {
        /* Stop timeout timer since device response has been received */
        hptimerstop();
        
        /* if the request was not successful */
        if(hipmsgp->hpphmsgparm != HPSUCCESS)
        {
            /* Log error state change */
            dlLog(  &hpphcbp->hpphdluser, DLCLASSALL,
                    "FWDWN: FLASH PROG->FLASH ERR",
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL);
            
            /* Move from SWI_FW_FLASH to SWI_FW_FLASH_ERR */
            hpstateupdate(SWI_FW_FLASH_ERR,
                hipmsgp->hpphdatap, hipmsgp->hpphdatasize);
        }
        /* else the request was succesful */
        else
        {   
            /* Log the final state change */
            dlLog(  &hpphcbp->hpphdluser, DLCLASSALL, 
                    "FWDWN: FLASH PROG->DONE!!",
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL);
            
            /* Move from SWI_FW_FLASH to SWI_FW_DONE */
            hpstateupdate(SWI_FW_DONE, NULL, 0);
        }
    }
}

/*
 * HIP callback handling functions 
 */

/* 
 * Name: hpfwdwncbmsg
 *
 * Purpose: This function receives incoming HIP messages registered for
 *          by the Firmware Download module.  Based on the current
 *          firmware download state it will call the state handling
 *          function that is waiting for a response from the device. 
 *
 * Parms:   eventbkp - Pointer to HPPH task received event

 * Return:  None
 *
 * Note:    This function is called from the HPPH task context.
 */
package void hpfwdwncbmsg(struct hpphevtblock  *eventbkp)
{

    struct hphipevt *hipmsgp = &eventbkp->hpevtmember.hpmdmpkt;

    switch(hpgcb.hpparms.amfwdwnstate)
    {
        case SWI_FW_DONE:
            /* 
             * SWI_FW_DONE -
             * nothing to be done - firmware download is not in progress 
             */
            break;
        
        case SWI_FW_SOFT_RESET:
            /* 
             * SWI_FW_SOFT_RESET - 
             * waiting for indication that device is in BootnHold.
             * 
             * This state is handled in hpmodestatechangemsg().
             */
            break;
          
        case SWI_FW_START_REQ:
            /* 
             * SWI_FW_START_REQ - 
             * waiting for HIP Start Response
             */
            hpstatefwstartreq(hipmsgp);
            break;
            
        case SWI_FW_DWN:
            /* 
             * SWI_FW_DWN - 
             * waiting for HIP Continuation Response
             */
            hpstatefwdownload(hipmsgp);
            break;
        
        case SWI_FW_CHKSUM:
            /* 
             * SWI_FW_CHKSUM - 
             * waiting for HIP End Response
             */
            hpstatefwchksum(hipmsgp);
            break;
            
        case SWI_FW_FLASH:
            /* 
             * SWI_FW_FLASH - 
             * waiting for HIP Flash Program Response
             */
            hpstatefwflash(hipmsgp);
            break;
            
        default:
            break;
    }
}
/*************
 * Name:    hpfwdwnreceive
 *
 * Purpose: Callback for receipt of a HIP Packet Handler
 *          request response. 
 *
 * Parms:   msgdesignator - The received message ID
 *          msgparm       - The contents of the HIP parameter field
 *          memfreep      - Pointer to memory to be reased by caller
 *          payloadp  - Pointer to the payload part of the message
 *          paylength - The length of the payload 
 *          errorflg  - flag for message validity
 *
 * Return:  None
 *
 * Note:    This is a registered callback.  It is not called from the 
 *          HPPH task context.  It will construct a event message
 *          to convey the received HIP message to the HPPH task.
 *
 */
local void hpfwdwnreceive( 
    swi_uint32 msgdesignator, 
    swi_uint8 msgparm, 
    swi_uint8 *memfreep, 
    swi_uint8 *payloadp, 
    swi_uint16 paylength, 
    swi_bool errorflg)
{
    struct hpphevtblock  *eventbkp;    /* event block for HPPH task */

    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();

    /* Log HIP message received by callback */
    dlLog(  &hpphcbp->hpphdluser, DLCLASSALL, 
            "FWDWN: HIP callback called for %x", 
            (swi_uint32)msgdesignator,
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL);
            
    /* Get an event block pointer */
    eventbkp = (struct hpphevtblock *) mmbufget( &hpphcbp->hpphevtpool);
    
    /* Don't want NULL event block pointers */
    if( eventbkp )
    {
        /* Fill the event with Firmware Download Callback message */
        eventbkp->hpphevent         = HPPHFWDWNCB;
        eventbkp->hpphmemfreep      = memfreep;   
        eventbkp->hpevtmember.hpmdmpkt.hpphmsgdesignator = msgdesignator;
        eventbkp->hpevtmember.hpmdmpkt.hpphmsgparm       = msgparm;
        eventbkp->hpevtmember.hpmdmpkt.hpphdatap         = payloadp;
        eventbkp->hpevtmember.hpmdmpkt.hpphdatasize      = paylength;
        eventbkp->hpevtmember.hpmdmpkt.hppherrorflg      = errorflg;
        
        /* Send the new event to the HPPH task  for processing */
        icsmt_send( &hpphcbp->hpphicmsgque, (swi_uint32) eventbkp );
    }
    /* NULL Block */
    else
    {
        /* This should never happen */
        erAbort("HPPH: Out of memory", 0);
    }
} 
/* 
 * Name: hpmodestatechangemsg
 *
 * Purpose: This function acts on a HPPH task received HPPHMODEMSTATECB 
 *          event.  This event conveys modem state change information
 *          to the firmware download module.
 * 
 *          If in the SWI_FW_SOFT_RESET state and the device is reportedly
 *          ONLINE then a loopback request will be sent
 *
 * Parms:   eventbkp - Pointer to HPPH task received event

 * Return:  None
 *
 * Note:    This function is called from the HPPH task context.
 */
package void hpmodestatechangemsg(struct hpphevtblock  *eventbkp)
{
    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();

    /* If a firmware download is in progress */
    if(hpgcb.hpparms.amfwdwnstate != SWI_FW_DONE)
    {
        /* If currently in the state that resets the device into */
        /* Boot-n-Hold mode */
        if(hpgcb.hpparms.amfwdwnstate == SWI_FW_SOFT_RESET)
        {
            /* 
             * SWI_FW_SOFT_RESET - 
             * waiting for indication that device is in BootnHold.
             */
            hpstatesoftreset(eventbkp);
        }
        /* 
         * else a firmware download is in progress and this is an 
         * unexpected reset
         */
        else if(eventbkp->hpevtmember.hpdevchg.modemstate != HPMDMONLINE)
        {
            /* Log the error state change */
            dlLog(  &hpphcbp->hpphdluser, DLCLASSALL, 
                    "FWDWN: unexpected RESET",
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL);
            
            /* Move to SWI_FW_RESET_ERR */
            hpstateupdate(SWI_FW_RESET_ERR, NULL, 0);
        }
    }
}
/*************
 * Name:    hpdevchgstatecb
 *
 * Purpose: Firmware Download module's registered Modem State Change
 *          callback.  It will construct an event to send to the firmware
 *          download via the HPPH task.
 *
 * Parms:   modemstate - new device state (ONLINE/OFFLINE)
 *          apprunning - TRUE, the modem is executing its application image
 *                       FALSE the modem is executing its boot loader image
 *          devtype    - device type
 *          userp      - user provided pointer (unused by this module)
 *
 * Return:  None
 *
 * Note:    This is a registered callback.  It is not called from the 
 *          HPPH task context.  It will construct a event message
 *          to convey the received HIP message to the HPPH task.
 *
 */
local void hpdevchgstatecb(
    enum hpmdmstate      modemstate, 
    swi_bool             apprunning,
    enum SWI_TYPE_Device devtype,
    void                 *userp )
{
    struct hpphevtblock  *eventbkp;    /* event block for HPPH task */

    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();

    /* Get an event block pointer */
    eventbkp = (struct hpphevtblock *) mmbufget( &hpphcbp->hpphevtpool);
    
    /* Log that this callback has been called -indicate the new device state */
    dlLog ( &hpphcbp->hpphdluser, DLCLASSALL,
            "FWDWN: Modem state change callback - modemOn[%d]\n", \
            (swi_uint32)modemstate,
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL);  
           
    /* Don't want NULL event block pointers */
    if( eventbkp )
    {
        /* Fill the event with Loopback response message */
        eventbkp->hpphevent         = HPPHMODEMSTATECB;
        eventbkp->hpphmemfreep      = NULL;   
        eventbkp->hpevtmember.hpdevchg.modemstate = modemstate;
        eventbkp->hpevtmember.hpdevchg.devtype    = devtype;
        eventbkp->hpevtmember.hpdevchg.apprunning = apprunning;
        
        /* Send the new event to the HPPH task  for processing */
        icsmt_send( &hpphcbp->hpphicmsgque, (swi_uint32) eventbkp );
    }
    /* NULL Block */
    else
    {
        /* This should never happen */
        erAbort("HPPH: Out of memory", 0);
    }
}
/*************
 *
 * Name:    hpsendfwdwnrr
 *
 * Purpose: Send a new Firmware Download request IPC message to 
 *          the HP task 
 *             
 * Parms:   inipcmsgp - Pointer to the Message Data portion of the
 *                      inbound (i.e. from the application) AM message
 *          memfreep  - Pointer to the memory release address for the
 *                      inbound message. HP task will manage the
 *                      release of this packet when done processing it
 *          ipcchannel - IPC channel whence the request came in from.
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   Allocate an event block, then store the calling arguments
 *          into the appropriate fields and send the packet to the 
 *          HP task. Buffer releasing is the responsibility of the 
 *          HP task, callers must not do this
 *
 **************/
global void hpsendfwdwnrr( 
    swi_uint8 *inipcmsgp, 
    swi_uint8 *memfreep,
    swi_uint8 ipcchannel)
{
    struct hpphevtblock  *eventbkp;    /* event block for HPPH task */

    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();

    /* Save the IPC channels if not already downloading */
    if (  hpphcbp->hpphipcchanrr == CIMAXIPCCHAN)
    {
        hpphcbp->hpphipcchanrr  = ipcchannel;
    }
    else
    {
        /* only one download allowed */
        return;
    }
    /* Get an event block pointer */
    eventbkp = (struct hpphevtblock *) mmbufget( &hpphcbp->hpphevtpool);
    
    /* Don't want NULL event block pointers */
    if( eventbkp )
    {
        eventbkp->hpphevent     = HPPHFWDWNREQ;
        eventbkp->hpphmemfreep  = memfreep;
        eventbkp->hpevtmember.hppham.hpmsgdatap = inipcmsgp;
        
        /* Send the event to the HPPH task  for processing */
        icsmt_send( &hpphcbp->hpphicmsgque, (swi_uint32) eventbkp );
    }
    /* NULL Block */
    else
    {
        /* This should never happen */
        erAbort("HPPH: Out of memory", 0);
    }
}
/*************
 *
 * Name: hpfwdownloadinit
 *
 * Purpose: To initialize the Firmware Download module
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
package void hpfwdownloadinit(void)
{
    /* initialize global control block variables */
    hpstateclear();
    
    /* Register for modem state changes */
    hpmdmregister( hpdevchgstatecb, &hpgcb.hpdevchgreqblk, "FWDWN", NULL);

    /* Register for incoming HIP loopback responses */
    hpregisterhip( HPLOOPRESP, hpfwdwnreceive);

    /* Register for incoming HIP download start responses */
    hpregisterhip( HPDLSTRTRESP, hpfwdwnreceive);

    /* Register for incoming HIP download continuation responses */
    hpregisterhip( HPDLCONTRESP, hpfwdwnreceive);

    /* Register for incoming HIP download end responses */
    hpregisterhip( HPDLENDRESP, hpfwdwnreceive);

    /* Register for incoming HIP flash program responses */
    hpregisterhip( HPPROGRESP, hpfwdwnreceive);
    

}
/* 
 * $Log: hpfwdownload.c,v $
 */
