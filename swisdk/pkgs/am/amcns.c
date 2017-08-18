/*************
 *
 * $Id: amcns.c,v 1.3 2010/06/09 23:27:04 epasheva Exp $
 *
 * Filename: amcns.c
 *
 * Purpose:  Contains routines related to handling CnS packets
 *           defined by the AM package. Runs on the API-side
 *
 * NOTE:     The functions within this file build and parse
 *           packets that are defined in IDD_SwiCnSMessaging.doc
 *
 * Copyright:  2008 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* include files */
#include "am/amidefs.h"
#include "am/amudefs.h"

/* Local storage */


/* 
 ******************* Packet Building Routines ********************
 */

/*************
 *
 * Name: amsetcnsrequestoperationid
 *
 * Purpose: Set operation ID for CNSREQUEST packet in the provided storage area
 *             
 * Parms:   amcnspacketp   - Pointer to storage into which the message
 *                           will be written
 *          operationid    - CnS operation ID, e.g. CNS_NOTREQ, CNS_STOPREQ, 
 *                           etc. Refer to enum cnsoptypes.
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   CNSREQUEST packet defined in IDD_SwiCnsMessaging.doc
 *
 *          CNSREQUEST packets travel from the API-side to the SDK
 *          side
 *
 **************/
package void amsetcnsrequestoperationid( 
    swi_uint8 *amcnspacketp, 
    swi_uint8 operationid)
{
    /* Operation Type */
    *amcnspacketp = operationid;
}

/*************
 *
 * Name: ambuildcnsrequest
 *
 * Purpose: Build a CNSREQUEST packet in the provided storage area
 *          given the information contained in the parameter structure
 *             
 * Parms:   amsgp          - Pointer to storage into which the message
 *                           will be written
 *          amcnsparams    - Pointer to a structure containing the CnS
 *                           parameters
 *
 * Return:  Number of bytes occupied by the AM packet, not including
 *          the size of the cns parameter field, if any
 *
 * Abort:   none
 *
 * Notes:   This function builds the CNSREQUEST packet defined in 
 *          IDD_SwiCnsMessaging.doc, including the common AM header
 *          but not including the CnS Parameter field portion. 
 *
 *          CNSREQUEST packets travel from the API-side to the SDK
 *          side
 *
 **************/
package swi_uint16 ambuildcnsrequest( 
    swi_uint8 **amsgpp, 
    struct amcnsrrparms *amcnsrrparmp)
{
    swi_uint8  *packetp;     /* Local copy of incoming packet pointer */
    swi_uint32 byteswritten; /* After parsing, contains #bytes just read */

    /* Duplicate the pointer into our scratch location */
    packetp = *amsgpp;

    /* Build AM packet header */
    ambuildamheader(
        AMCNSREQUEST, 
        AMHDRVERSION, 
        0x00, 
        AMCNSREQTOTAL + amcnsrrparmp->amcnslength, 
        &packetp);

    /* Operation Type comes first */
    *packetp++ = amcnsrrparmp->amcnsoperation;

    /* Next put in the Air Interface type */
    *packetp++ = amcnsrrparmp->amcnsairint;

    /* Next is the CnS object ID */
    piput16( amcnsrrparmp->amcnsobjid, &packetp );
    
    /* Next the CnS Parameter field length */
    piput16( amcnsrrparmp->amcnslength, &packetp );

    /* Next the timeout field length */
    piput16( amcnsrrparmp->amcnstimeout, &packetp );

    /* Finally the application parameter field */
    piput32( amcnsrrparmp->amcnsparameter, &packetp );

    /* Update the bytes read variable here */
    byteswritten = packetp - *amsgpp;
    
    /* Ensure that bytes written matches the defined constant for size */
    if( byteswritten != AMCNSREQTOTAL )
    {
        /* Combine both sizes into one and abort */
        byteswritten = byteswritten << 16 | AMCNSREQTOTAL;
        erAbort("Update AMCNSREQTOTAL", byteswritten );
    }

    /* advance the caller's packet pointer to the
     * start of the CNS Parameter section using 
     * the incoming length 
     */
    *amsgpp += byteswritten;

    return byteswritten;
}

/*************
 *
 * Name: ambuildcnsresponse
 *
 * Purpose: Build a CNSRESPONSE packet in the provided storage area
 *          given the information contained in the parameter structure
 *             
 * Parms:   amsgp          - Pointer to storage into which the message
 *                           will be written
 *          amcnsparams    - Pointer to a structure containing the CnS
 *                           parameters to be written into the packet
 *
 * Return:  Number of bytes occupied by the AM packet, not including
 *          the size of the cns parameter field, if any
 *
 * Abort:   none
 *
 * Notes:   This function builds the CNSRESPONSE packet defined in 
 *          IDD_SwiCnsMessaging.doc, including the common AM header
 *          but not including the CnS Parameter field portion. 
 *
 *          CNSRESPONSE packets travel from the SDK-side to the API
 *          side
 *
 **************/
global swi_uint16 ambuildcnsresponse( 
    swi_uint8 **amsgpp, 
    struct amcnsrrparms *amcnsrrparmp)
{
    swi_uint8  *packetp;     /* Local copy of incoming packet pointer */
    swi_uint32 byteswritten; /* After parsing, contains #bytes just read */

    /* Duplicate the pointer into our scratch location */
    packetp = *amsgpp;

    /* Build AM packet header */
    ambuildamheader(
        AMCNSRESPONSE, 
        AMHDRVERSION, 
        amcnsrrparmp->amresultcode, 
        AMCNSRESPTOTAL + amcnsrrparmp->amcnslength, 
        &packetp);
        
    /* Operation Type comes first */
    *packetp++ = amcnsrrparmp->amcnsoperation;

    /* Next is the CnS object ID */
    piput16( amcnsrrparmp->amcnsobjid, &packetp );
    
    /* Next is the Application Parameter 32 bit value */
    piput32( amcnsrrparmp->amcnsparameter, &packetp );

    /* Next the CnS Parameter field length */
    piput16( amcnsrrparmp->amcnslength, &packetp );

    /* Update the bytes read variable here */
    byteswritten = packetp - *amsgpp;

    if( byteswritten != AMCNSRESPTOTAL )
    {
        /* Combine both sizes into one and abort */
        byteswritten = byteswritten << 16 | AMCNSRESPTOTAL;
        erAbort("AMCNSRESPTOTAL needs updating\n", byteswritten );
    }

    /* advance the caller's packet pointer to the
     * start of the CNS Parameter section using 
     * the incoming length 
     */
    *amsgpp += byteswritten;

    return byteswritten;
}

/*************
 *
 * Name: ambuildcnsnotify
 *
 * Purpose: Build a CNSNOTIFICATION packet in the provided storage area
 *          given the information contained in the parameter structure
 *             
 * Parms:   amsgp          - Pointer to storage into which the message
 *                           will be written
 *          amcnsparams    - Pointer to a structure containing the CnS
 *                           parameters to be written into the packet
 *
 * Return:  Number of bytes occupied by the AM packet, not including
 *          the size of the cns parameter field, if any
 *
 * Abort:   none
 *
 * Notes:   This function builds the CNSNOTIFICATION packet defined in 
 *          IDD_SwiCnsMessaging.doc, including the common AM header
 *          but not including the CnS Parameter field portion. 
 *
 *          CNSNOTIFICATION packets travel from the SDK-side to the API
 *          side
 *
 **************/
global swi_uint16 ambuildcnsnotify( 
    swi_uint8 **amsgpp, 
    struct amcnsrrparms *amcnsrrparmp)
{
    swi_uint8  *packetp;     /* Local copy of incoming packet pointer */
    swi_uint32 byteswritten; /* After parsing, contains #bytes just read */

    /* Duplicate the pointer into our scratch location */
    packetp = *amsgpp;

    /* Build AM packet header */
    ambuildamheader(
        AMCNSNOTIFICATION, 
        AMHDRVERSION, 
        0x00, 
        AMCNSNOTFTOTAL + amcnsrrparmp->amcnslength, 
        &packetp);
    
    /* Operation Type comes next */
    *packetp++ = amcnsrrparmp->amcnsoperation;

    /* Next is the CnS object ID */
    piput16( amcnsrrparmp->amcnsobjid, &packetp );
    
    /* Next the CnS Parameter field length */
    piput16( amcnsrrparmp->amcnslength, &packetp );

    /* Update the bytes read variable here */
    byteswritten = packetp - *amsgpp;

    if( byteswritten != AMCNSNOTFTOTAL )
    {
        /* Combine both sizes into one and abort */
        byteswritten = byteswritten << 16 | AMCNSNOTFTOTAL;
        erAbort("Update AMCNSNOTFTOTAL\n", byteswritten );
    }

    /* advance the caller's packet pointer to the
     * start of the CNS Parameter section using 
     * the incoming length 
     */
    *amsgpp += byteswritten;

    return byteswritten;
}

/* 
 ******************* Parsing Routines ********************
 */
/*************
 *
 * Name: amparsecnsrequest
 *
 * Purpose: Dispatch routine for incoming CnS messages. Parses the 
 *          message and sends it to an appropriate handler 
 *             
 * Parms:   amcnspacketpp  - pointer to a pointer to the AM CnS Request
 *                           packet.
 *          amcnsrrparmp   - Pointer to a caller-allocated structure
 *                           defined by AM package for exchanging 
 *                           CnS Request/Response parameters. Saves
 *                           long lists of calling arguments
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   Parses the fields from a CnS Request AM packet and returns
 *          the values to the caller in the structure provided
 *
 *          This function parses the CNSREQUEST packet defined in 
 *          IDD_SwiCnsMessaging.doc
 *
 *          On return the packet pointer pointer is advanced to point
 *          to the CnS Parameter field
 *
 **************/
global void amparsecnsrequest( 
    swi_uint8 **amcnspacketpp, 
    struct amcnsrrparms *amcnsrrparmp )
{
    swi_uint8  *packetp;  /* Local copy of incoming packet pointer */
    swi_uint32 bytesread; /* After parsing, contains #bytes just read */

    /* Duplicate the pointer into our scratch location */
    packetp = *amcnspacketpp;

    /* Proceed with field parsing */
    amcnsrrparmp->amcnsoperation = *packetp++;
    amcnsrrparmp->amcnsairint = *packetp++;
    amcnsrrparmp->amcnsobjid = piget16( &packetp );
    amcnsrrparmp->amcnslength = piget16( &packetp );
    amcnsrrparmp->amcnstimeout = piget16( &packetp );
    amcnsrrparmp->amcnsparameter = piget32( &packetp );

    /* Update the bytes read variable here */
    bytesread = packetp - *amcnspacketpp;

    /* Make sure the bytes read matches the packet size */
    if( bytesread != AMCNSREQSZ )
    {
        /* Combine both sizes into one and abort */
        bytesread = bytesread << 16 | AMCNSREQSZ;
        erAbort("Update AMCNSREQSZ", bytesread );
    }

    /* advance the caller's packet pointer to the
     * start of the CNS Parameter section using 
     * the incoming length 
     */
    *amcnspacketpp += bytesread;
}

/*************
 *
 * Name: amparsecnsresponse
 *
 * Purpose: This routine knows how to unpack the contents of an AM
 *          CnS Response message
 *             
 * Parms:   amcnspacketpp  - pointer to a pointer to the AM CnS Response
 *                           packet.
 *          amcnsrrparmp   - Pointer to a caller-allocated structure
 *                           defined by AM package for exchanging 
 *                           CnS Request/Response parameters. Saves
 *                           long lists of calling arguments
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   Parses the fields from a CnS Response AM packet and returns
 *          the values to the caller in the structure provided
 *
 *          This function parses the CNSRESPONSE packet defined in 
 *          IDD_SwiCnsMessaging.doc
 *
 *          On returm the packet pointer pointer is advanced to point
 *          to the CnS Parameter field
 *
 **************/
package enum swi_amresults amparsecnsresponse( 
    swi_uint8 **amcnspacketpp, 
    struct amcnsrrparms *amcnsrrparmp )
{
    swi_uint8  *packetp;  /* Local copy of incoming packet pointer */
    swi_uint32 bytesread; /* After parsing, contains #bytes just read */

    /* Duplicate the pointer into our scratch location */
    packetp = *amcnspacketpp;

    /* Proceed with field parsing */
    amcnsrrparmp->amcnsoperation = *packetp++;
    amcnsrrparmp->amcnsobjid = piget16( &packetp );
    amcnsrrparmp->amcnsparameter = piget32( &packetp );
    amcnsrrparmp->amcnslength = piget16( &packetp );

    /* Update the bytes read variable here */
    bytesread = packetp - *amcnspacketpp;

    /* Ensure the response size matches the constant */
    if( bytesread != AMCNSRESPONSESZ )
    {
        /* Combine both sizes into one variable and abort */
        bytesread = bytesread << 16 | AMCNSRESPONSESZ;
        erAbort("Update AMCNSRESPONSESZ\n", bytesread );
    }
    /* advance the caller's packet pointer to the
     * start of the CNS Parameter section using 
     * the incoming length 
     */
    *amcnspacketpp += bytesread;
    
    return(amcnsrrparmp->amresultcode);
}

/*************
 *
 * Name: amparsecnsnotify
 *
 * Purpose: This routine knows how to unpack the contents of an AM
 *          CnS Notification message
 *             
 * Parms:   amcnspacketpp  - pointer to a pointer to the AM CnS Response
 *                           packet.
 *          amcnsrrparmp   - Pointer to a caller-allocated structure
 *                           defined by AM package for exchanging 
 *                           CnS Request/Response parameters. Saves
 *                           long lists of calling arguments
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   Parses the fields from a CnS Notification AM packet and 
 *          returns the values to the caller in the structure provided
 *
 *          This function parses the CNSNOTIFICATION packet defined in 
 *          IDD_SwiCnsMessaging.doc
 *
 *          On returm the packet pointer pointer is advanced to point
 *          to the CnS Parameter field
 *
 **************/
package void amparsecnsnotify( 
    swi_uint8 **amcnspacketpp, 
    struct amcnsrrparms *amcnsrrparmp )
{
    swi_uint8  *packetp;  /* Local copy of incoming packet pointer */
    swi_uint32 bytesread; /* After parsing, contains #bytes just read */

    /* Duplicate the pointer into our scratch location */
    packetp = *amcnspacketpp;

    /* Proceed with field parsing */
    amcnsrrparmp->amcnsoperation = *packetp++;
    amcnsrrparmp->amcnsobjid = piget16( &packetp );
    amcnsrrparmp->amcnslength = piget16( &packetp );

    /* Update the bytes read variable here */
    bytesread = packetp - *amcnspacketpp;

    /* Check the sizes match */
    if( bytesread != AMCNSNOTIFYSZ )
    {
        /* Combine both sizes into one and abort */
        bytesread = bytesread << 16 | AMCNSNOTIFYSZ;
        erAbort("Update AMCNSNOTIFYSZ\n", bytesread );
    }

    /* advance the caller's packet pointer to the
     * start of the CNS Parameter section using 
     * the incoming length 
     */
    *amcnspacketpp += bytesread;
}


/* 
 ******************* Utility Routines ********************
 */



/*
 * $Log: amcns.c,v $
 */

