/*************
 *
 * $Id: hprx.c,v 1.20 2010/06/09 23:45:48 epasheva Exp $
 *
 * Filename:  hpdispatch 
 *
 * Purpose:   To dispatch received messages. Used only by the 
 *            hprxtask.
 *
 * Copyright: © 2008 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* include files */
#include "aa/aaglobal.h"
#include "er/erudefs.h"
#include "pi/piudefs.h"
#include "ql/qludefs.h"
#include "sl/sludefs.h"
#include "hp/hpudefs.h"
#include "hp/hpidefs.h"

/* 
 * Local storage 
 */

/*************
 *
 * Name:     hplookup - lookup table for short message ids 
 *
 * Purpose:  To access memory allocation and task delivery functions 
 *           given a short message id             
 *
 * Notes:    none
 *
 **************/
local struct hpregblk hplookup[HPMAXFMSGID - HPMINFMSGID + 1];

/*************
 *
 * Name:     hplookup_extended - lookup table for extended message ids 
 *
 * Purpose:  To access memory allocation and task delivery functions 
 *           given a extended message id             
 *
 * Notes:    none
 *
 **************/
local struct hpregblk hplookup_extended[HPMAXFEXTMSGID - HPMINFEXTMSGID + 1];

/*
 * Functions
 */
/*************
 *
 * Name: hpparsehdr
 *
 * Purpose: Copies the fields in a HIP message header into individual 
 *          arguments and returns them to the caller
 *             
 * Parms:   msghdrp        - pointer to the message header
 *          msgdesignatorp - mem location to return the message id to
 *          payloadp       - mem location to return the beginning of the 
 *                           payload pointer
 *          payloadlenp    - mem location to return the payload length to
 *          msgparmp       - mem location to return the message specific parm to
 *
 * Return:  TRUE  - if message ids are in valid range
 *          FALSE - otherwise
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
local swi_bool hpparsehdr(
    swi_uint16 *msghdrp, 
    swi_uint32 *msgdesignatorp,
    swi_uint8  **payloadpp,
    swi_uint16 *payloadlenp, 
    swi_uint8 *msgparmp)
{
    swi_uint8 *hdrp;             /* temporary pointer to the message header */
    swi_bool padbyte = FALSE;
    swi_bool badmsgid = FALSE;
    swi_uint32 msgid;
    swi_uint32 extmsgid;

    /* initialize message header pointer */
    hdrp = (swi_uint8 *)msghdrp;

    /* 
     * Parse original HIP header 
     */

    /* get payload length */
    *payloadlenp = piget16(&hdrp);

    /* get message id */
    msgid = HPMSGIDMSK & *hdrp;

    /* if the pad bit is set */
    if (HPPADBMSK & *hdrp++)
    {
      padbyte = TRUE;
    }

    /* get message specific parameter */
    *msgparmp = *hdrp++;
    
    /* validate message id */
    if (msgid > HPMAXFMSGID || msgid < HPMINFMSGID)
    {
        badmsgid = TRUE;
    }
   
    /* if the message id indicates is an extended HIP header present */
    else if (msgid == HPHEXTENDED)
    {
      /* get extended message id */
      extmsgid = piget16(&hdrp) & HPEXTMSGIDMSK;
     
      /* validate extended message id */
      if (extmsgid > HPMAXFEXTMSGID || extmsgid < HPMINFEXTMSGID)
      {
          badmsgid = TRUE;
      }
     
      /* mask off the extended HIP payload max length */
      *payloadlenp &= HPEXTPAYLENMSK; 
    }
    /* else this is the original header */
    else
    {
      /* clear the extended message id */
      extmsgid = 0;
      
      /* mask off the original HIP payload max length */
      *payloadlenp &= HPPAYLENMSK; 
    }
   
    /*
     * convert the short and extended message ids to the HP 
     * package message designator 
     */
    hpmsgid2designator(msgdesignatorp, msgid, extmsgid);
   
    /* get the pointer to the beginning of the payload */
    *payloadpp = hdrp;
   
    /* if there is a pad byte */
    if((padbyte) && (*payloadlenp > 0))
    {    
        /* increment payload pointer past pad byte */
        (*payloadpp)++;
        
        /* decrement payload length to not include the pad byte */
        (*payloadlenp)--;
    }
    
    /* return that the HIP header is valid */
    return(!badmsgid);
}
/*************
 *
 * Name:    hplookupinfo
 *
 * Purpose: Looks up the registered information block for the 
 *          specified HIP message designator. 
 *             
 * Parms:   msgdesignator - message designator
 *
 * Return:  pointer to register block info
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
local struct hpregblk *hplookupinfo(swi_uint32 msgdesignator)
{
  struct hpregblk *msginfop;
  swi_uint32 msgid;
  swi_uint32 extmsgid;
  swi_bool extheader;
  
  hpdesignator2msgid(msgdesignator, &msgid, &extheader, &extmsgid);
  
  if(extheader)
  {
    /* get pointer to registered information block for this message */
    msginfop = &hplookup_extended[extmsgid - HPMINFEXTMSGID];
  }
  else
  {
    /* get pointer to registered information block for this message */
    msginfop = &hplookup[msgid - HPMINFMSGID];
  }

  return msginfop;
}
/*************
 *
 * Name: hpdispatch
 *
 * Purpose: Receive a new message from the HD layer, validate it 
 *          and forward it to the registered function, if any
 *             
 * Parms:   memrelp   - Ptr to addr. to use when releasing this buffer
 *          bufp      - Ptr to the Rx'd frame that needs to be 
 *                      recognized and dispatched
 *          buflength - Length of the received packet in bytes
 *          errorflg  - TRUE, the incoming message length doesn't 
 *                      match the expected length
 *                    - FALSE, the message length matches the expected length
 *                      
 * Return:  none
 *
 * Abort:   No function registered for the HIP message
 *
 * Notes:   The buffer containing the newly-received message (in buff) is 
 *          allocated from the MM package and must be released when the 
 *          message has been handled. 
 *
 **************/
global void hpdispatch( 
    swi_uint8 *memrelp, 
    swi_uint8 *bufp, 
    swi_uint16 buflength, 
    swi_bool errorflg )
{
    swi_uint8 *payloadp;       /* pointer to beginning of HIP payload */
    swi_uint16 payloadlength;  /* payload length */
    swi_uint32 msgdesignator;  /* message designator */
    swi_uint8  msgparm;        /* message specific parameter */
    struct hpregblk *regblkp;
    struct hprxcb *hprxcbp = hpgetrxcbp();

    /* parse the HIP header and determine if the message ids are valid */                     
    if(!hpparsehdr((swi_uint16 *) bufp, 
                   &msgdesignator, &payloadp, &payloadlength, &msgparm))
    {
        /* Need to release the message buffer */
        mmbufrel( memrelp );

        /* Log that we got an invalid HIP message */
        dlLog( &hprxcbp->hprxdluser, HPLOG_CLASSB, 
               "Invalid msg Id rx'd: designator[%x]\n", 
               (swi_uint32)msgdesignator,
               (swi_uint32)NULL,
               (swi_uint32)NULL,
               (swi_uint32)NULL);
        
        /* silently drop message */
        return;
    
    }
   
    /* look up registered information for this message */
    regblkp = hplookupinfo(msgdesignator);
    
    /* if there is no registered function for this messages then */
    if (regblkp->hpdeliverbuffp == NULL)
    {
        /* Need to release the message buffer */
        mmbufrel( memrelp);

	      /* Log that we dropped a message */
        dlLog( &hprxcbp->hprxdluser, HPLOG_CLASSB, 
               "No reg'd handler. Dropped HIP %x\n", 
               (swi_uint32)msgdesignator,
               (swi_uint32)NULL,
               (swi_uint32)NULL,
               (swi_uint32)NULL);
        
        /* silently drop message */
        return;
    } 

    /* call deliver function */
    regblkp->hpdeliverbuffp(msgdesignator,      /* HIP Msg ID */
                            msgparm,            /* HIP Msg-specific parameter */
                            memrelp,            /* beginning of buffer */
                            payloadp,           /* Loc'n of payload */
                            payloadlength,      /* Length of payload */
                            errorflg);          /* Size overrun flag */
}

/*************
 *
 * Name: hpregisterhip
 *
 * Purpose: Entry point to HP package for registration to receive HIP 
 *          messages from the modem
 *             
 * Parms:   msgid        - message designator
 *          deliverbuffp - pointer to function to deliver message buffer,
 *                         see below for prototype.
 *
 * Return:  no value
 *
 * Abort:   Invalid HIP message ID
 *
 * Notes:   The prototype for the caller's registered function is:
 *          void (*deliveryfnp)( swi_uint8 msgdesignator, 
 *                               swi_uint8 msgparm, 
 *                               swi_uint8 *memfreep, 
 *                               swi_uint8 *payloadp, 
 *                               swi_uint16 paylength, 
 *                               swi_bool errorflag )
 *          Where:
 *          ======
 *          msgdesignator is the HIP Message designator the caller has 
 *                        registered to intercept messages for. It is returned 
 *                        whenever HIP calls the registered function
 *          msgparm       is the byte contained in the HIP message parameter
 *                        field. The exact value is specific to the msgid and 
 *                        may be unused
 *          memfreep      Callers must use this address for releasing the 
 *                        buffer memory when they are done handling it. Do NOT
 *                        use payloadp to release the memory
 *          payloadp      is a pointer to the beginning of the caller's message
 *          paylength     is the length in bytes of the data pointed-to by 
 *                        payloadp
 *          errorflag     indicates whether there was an error in the inter-
 *                        pretation of the incoming HIP frame. TRUE means the
 *                        incoming message overflowed the storage available,
 *                        FALSE means there were no errors
 *
 *          Registered functions are responsible for releasing the 
 *          buffer when they are finished with it. To do so, call 
 *          mmbufrel( bufferp );
 *
 **************/
global void hpregisterhip(
    swi_uint32 msgdesignator, 
    void (*deliverbuffp)( swi_uint32 msgdesignator, 
                          swi_uint8 msgparm, 
                          swi_uint8 *memfreep, 
                          swi_uint8 *payloadp, 
                          swi_uint16 paylength, 
                          swi_bool errorflg))
{
    struct hpregblk *msginfop;    /* pointer to message info record */
    swi_uint32 msgid;
    swi_uint32 extmsgid;
    swi_bool extheader;
     
    hpdesignator2msgid(msgdesignator, &msgid, &extheader, &extmsgid);

    /* check for valid message id */
    if(msgid > HPMAXFMSGID || msgid < HPMINFMSGID)
    {
        erAbort("hpreg: invalid fwd message id", msgid);
    }

    if (extheader && (extmsgid > HPMAXFEXTMSGID || extmsgid < HPMINFEXTMSGID)) 
    {
        erAbort("hpreg: invalid fwd extended message id", extmsgid);
    }

    /* get pointer lookup record for this specific msgid */
    msginfop = hplookupinfo(msgdesignator);

    /* store message id info in table */
    msginfop->hpdeliverbuffp = deliverbuffp;
    msginfop->hpnotregister = 0;
    msginfop->hptoolong = 0;
}

/*************
 *
 * Name: hprxinit
 *
 * Purpose: To initialize the queue headers for the registration tables
 *             
 * Parms:   none
 *
 * Return:  no value
 *
 * Abort:   none
 *
 * Notes:   To be called during Task Startup
 *
 **************/
package void hprxinit(void)
{
    struct hprxcb *hprxcbp = hpgetrxcbp();

    /* create a memory pool for receive buffer allocation */
    mmpcreate( &hprxcbp->hpRxPool, "hpRxPool", HPRXPOOLSZ, HPRXMSGSZ );

    /* Register with the logger process, initially enabled */
    dlregister ("HPRX", &hprxcbp->hprxdluser, FALSE);

    hprxcbp->hprxpktsdropped = 0;
}

/*************
 *
 * Name: hpinit
 *
 * Purpose: Package initialization routine
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
global void hpinit( void )
{
    /* Initialize the structure. Strictly speaking, 
     * this isn't needed, but it's good practice
     */
    slmemset( (char *) hplookup, 0, sizeof( hplookup ) );
    slmemset( (char *) hplookup_extended, 0, sizeof( hplookup_extended ) );

}


/*
 * $Log: hprx.c,v $
 */
