/*************
 *
 * $Id: cnpkg_api.c,v 1.8 2008/08/06 22:21:13 rfiler Exp $
 *
 * Filename: cnpkg_api.c
 *
 * Purpose:  CnS Package functions required by the API-side 
 *
 * Copyright: © 2008 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* include files */
#include "aa/aaglobal.h"
#include "am/amudefs.h"
#include "ic/icudefs.h"
#include "cn/cnidefs.h"

/* Defines */

/* Local storage */

/*************
 *
 * Name: cngetreqbkp
 *
 * Purpose: Return a pointer to sufficient storage for holding a 
 *          CNS request for transmission to the SDK side. 
 *             
 * Parms:   none
 *
 * Return:  Pointer to an array of swi_uint8 large enough to contain
 *          the biggest possible CnS request
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
global swi_uint8 *cngetreqbkp( void )
{
    return(amgetreqbufp());
}

/*************
 *
 * Name: cngetappparm
 *
 * Purpose: Return a value to be filled into the application parameter
 *          field, ultimately within the CNS packet that is sent to
 *          the modem. 
 *             
 * Parms:   none
 *
 * Return:  32 bit value to be written into the application parameter
 *          field of a CnS packet
 *
 * Abort:   none
 *
 * Notes:   This function could easily be replaced by a defined 
 *          constant because the field within the CnS packet isn't
 *          really used for anything yet. Whatever value is placed
 *          in this field in requests sent TO the modem must be echo'd
 *          back in all responses sent FROM the modem. 
 *
 *          This function was created as a hedge against future changes
 *          which make the application parameter field more significant. 
 *          The API side changes would be easier and localized (hopefully)
 *          to this function
 *
 **************/
global swi_uint32 cngetappparm( void )
{
    return CNSAPPPARAMVALUE;
}

/*************
 *
 * Name: cnsendnwait
 *
 * Purpose: Format an AMCNSREQUEST packet using the calling arguments
 *          send it on its way and block until a response is received.
 *          Pass the response back to the caller
 *             
 * Parms:   amcnshdrp  - Pointer to the beginning of the packet that
 *                       will contain an AM Message
 *          amparamsp  - [IN] Pointer to a structure containing valid AM
 *                       parameters sufficient to build an AMCNSREQUEST
 *                       packet
 *                       [OUT] Contains the parsed fields from the AM
 *                       CNSRESPONSE packet
 *          responsepp - Pointer to a pointer which will be loaded with
 *                       the address of the CnS Parameter field received
 *                       from the modem
 *
 * Return:  Value of SWI_RCODE
 *
 * Abort:   none
 *
 * Notes:   This is a blocking call which returns only after the modem
 *          has sent its response or the timeout (stored in amparamsp)
 *          has expired, whichever is sooner
 *
 *          This function assumes the CnS Parameter field that will be
 *          sent to the modem is correctly formatted and at the offset
 *          immediately following the header(s) that will be added by
 *          the call to ambuildcnsrequest(), below 
 *
 *          This function returns the caller a pointer to the received
 *          buffer data in responsepp. The actual storage holding this
 *          data is global to this package. It cannot be overwritten 
 *          until the next icapi_rcv() call is posted which can only
 *          happen when the application sends the next message. There
 *          are NO RESOURCES TO RELEASE on the return path
 *
 **************/
global SWI_RCODE cnsendnwait( swi_uint8 *amcnshdrp, 
                              struct amcnsrrparms *amcnsparamsp, 
                              swi_uint8 **responsepp )
{
    struct amrrparms amparams; 
    SWI_RCODE retval;

    amparams.amparmtype = SWI_PARM_CNS; 
    amparams.amtimeout = amcnsparamsp->amcnstimeout;
    amparams.amparm.amcns = *amcnsparamsp;

    retval = amsendnwait(amcnshdrp, &amparams, responsepp);

    /* Copy the parameters from amparams into the
     * equivalent place in the caller-specified 
     * block
     */
    *amcnsparamsp = amparams.amparm.amcns;

    return retval;
}


/*
 * $Log: cnpkg_api.c,v $
 */

