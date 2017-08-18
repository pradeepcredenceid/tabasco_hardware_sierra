/*************
 *
 * $Id: amui_sdk.c,v 1.2 2010/06/09 23:29:31 epasheva Exp $
 *
 * Filename: amui_sdk.c
 *
 * Purpose:  Contains routines related to handling UI packets
 *           defined by the AM package.This module is used by the SDK side.
 *
 * NOTE:     The functions within this file build and parse
 *           packets that are defined in IDD_UIMessaging.doc
 *
 * Copyright:  2009 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* include files */
#include "am/amidefs.h"

/* Functions */

/*
 * Parsing UI AM packet routines 
 */

/*************
 *
 * Name: amparseuirequest
 *
 * Purpose: Parses an AM UI request message.
 *             
 * Parms:   ampacketpp     - pointer to a pointer to the AM UI
 *                           request packet.
 *          uidatalenp     - Pointer to data length
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   Parses the fields from a UI request AM packet 
 *          and returns the values to the caller in the parameters provided
 *
 *          This function parses the AM UI request  packets defined in 
 *          IDD_UIMessaging.doc
 *
 **************/
global void amparseuirequest( 
    swi_uint8 **ampacketpp, 
    swi_uint16 *uidatalenp)
{
    swi_uint8  *packetp;   /* Local copy of incoming packet pointer */
    swi_uint32 bytesread;  /* After parsing, contains #bytes just read */

    /* Duplicate the pointer into our scratch location */
    packetp = *ampacketpp;
    
    /* Proceed with field parsing */

     /* Extract UI data length */
    *uidatalenp = piget16( &packetp );
    
    /* Update the bytes read variable here */
    bytesread = packetp - *ampacketpp;

    /* Make sure the bytes read matches the packet size */
    if( bytesread != AMUIREQSZ)
    {
        /* Combine both sizes into one and abort */
        bytesread = bytesread << 16 | AMUIREQSZ;
        erAbort("Update AMUIREQSZ", bytesread );
    }

    /* advance the caller's packet pointer pass all parsed fields */
    *ampacketpp += bytesread;
}

/***********
 *
 * Name: ambuilduiresponse
 *
 * Purpose: Build a specific AM request packet including AM 
 *          packet header in the provided storage area given 
 *          the information contained in the parameter structure
 *             
 * Parms:   amsgpp       - Pointer to pointer to storage into which 
 *                         the message will be written
 *          uidatalen    - data length
 *          amresultcode - result code to put into message
 *
 * Return:  Number of bytes occupied by the AM packet
 *
 * Abort:   none
 *
 * Notes:   This function builds an AM UI response packet defined in 
 *          IDD_UIMessaging.doc.
 *
 *          AMUIRESPONSE packets travel from the SDK side to the API side
 *
 **************/
global swi_uint16 ambuilduiresponse( 
    swi_uint8 **amsgpp, 
    swi_uint16 uidatalen,
    enum swi_amresults amresultcode)
{
    swi_uint8  *packetp;     /* Local copy of incoming packet pointer */
    swi_uint32 byteswritten; /* After parsing, contains #bytes just read */

    /* Duplicate the pointer into our scratch location */
    packetp = *amsgpp;
    /* Build AM packet header */
    ambuildamheader(
        AMUIRESPONSE, 
        AMHDRVERSION, 
        amresultcode, 
        AMUIRESPTOTAL, 
        &packetp);

    /* pack the ui fields, unused and zero-filled */
    slmemset((char *)packetp, 0, AMUIRESPSZ);


    piput16( uidatalen, &packetp );
     
    /* Update the bytes read variable here */
    byteswritten = packetp - *amsgpp;

    if( byteswritten != AMUIRESPTOTAL )
    {
        /* Combine both sizes into one and abort */
        byteswritten = byteswritten << 16 | AMUIRESPTOTAL;
        erAbort("AMUIRESPTOTAL needs updating\n", byteswritten );
    }

    return byteswritten;
}

/*
 * $Log: amui_sdk.c,v $
 */

