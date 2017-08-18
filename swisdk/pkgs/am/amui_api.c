/*************
 *
 * $Id: amui_api.c,v 1.1 2009/01/27 21:25:51 blee Exp $
 *
 * Filename: amui_api.c
 *
 * Purpose:  Contains routines related to handling UI packets
 *           defined by the AM package.This module is used by the API side.
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
 * Building UI AM packet routines 
 */

/***********
 *
 * Name: ambuilduirequest
 *
 * Purpose: Build a specific AM request packet including AM 
 *          packet header in the provided storage area given 
 *          the information contained in the parameter structure
 *             
 * Parms:   amsgpp          - Pointer to pointer to storage into which 
 *                            the message will be written
 *          datalen         - length of data 
 *
 * Return:  Number of bytes occupied by the AM packet
 *
 * Abort:   none
 *
 * Notes:   This function builds an AM UI Request packet defined in 
 *          IDD_UIMessaging.doc.
 *
 *          AMUIREQUEST packets travel from the API-side to the SDK
 *          side
 *
 **************/
global swi_uint16 ambuilduirequest( 
    swi_uint8 **amsgpp, 
    swi_uint16 datalen)
{
    swi_uint8  *packetp;      /* Local copy of incoming packet pointer */
    swi_uint32  byteswritten; /* After parsing, contains 
                               * #bytes just written 
                               */

    /* Duplicate the pointer into our scratch location */
    packetp = *amsgpp;

    /* Build AM packet header */
    ambuildamheader( 
        AMUIREQUEST, 
        AMHDRVERSION, 
        0x00, 
        AMUIREQTOTAL, 
        &packetp);
        
    /* Add ui additional data length */
    piput16( datalen, &packetp);
    
    /* Update the bytes written variable here */
    byteswritten = packetp - *amsgpp;
    
    /* Ensure that bytes written matches the defined constant for size */
    if( byteswritten != AMUIREQTOTAL )
    {
        /* Combine both sizes into one and abort */
        byteswritten = byteswritten << 16 | AMUIREQTOTAL;
        erAbort("Update AMUIREQTOTAL", byteswritten );
    }
    
    /* Note: An externally added and variable-length part of the message
     *       follows within the packet after this point 
     */ 

    return byteswritten;
}

/*
 * Parsing ui AM packet routines 
 */

/*************
 *
 * Name: amparseuiresponse
 *
 * Purpose: Parses an AM Demux request message.
 *             
 * Parms:   ampacketpp     - pointer to a pointer to the AM Demux
 *                           request packet.
 *          uidatalenp     - Pointer to data length
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   Parses the fields from a UI response AM packet 
 *          and returns the values to the caller in the parameters provided
 *
 *          This function parses the AM UI response packets defined in 
 *          IDD_UIMessaging.doc
 *
 **************/
global void amparseuiresponse( 
    swi_uint8 **ampacketpp, 
    swi_uint16 *uidatalenp)
{
    swi_uint8  *packetp;   /* Local copy of incoming packet pointer */
    swi_uint32 bytesread;  /* After parsing, contains #bytes just read */

    /* Duplicate the pointer into our scratch location */
    packetp = *ampacketpp;
    
    /* Proceed with field parsing */

    /* Extract data length */
    *uidatalenp = piget16( &packetp );
    
    /* Update the bytes read variable here */
    bytesread = packetp - *ampacketpp;

    /* Make sure the bytes read matches the packet size */
    if( bytesread != AMUIRESPSZ)
    {
        /* Combine both sizes into one and abort */
        bytesread = bytesread << 16 | AMUIRESPSZ;
        erAbort("Update AMUIRESPSZ", bytesread );
    }

    /* advance the caller's packet pointer pass all parsed fields */
    *ampacketpp += bytesread;
}

/*
 * $Log: amui_api.c,v $
 */

