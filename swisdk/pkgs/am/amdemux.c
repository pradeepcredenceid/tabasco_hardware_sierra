/*************
 *
 * $Id: amdemux.c,v 1.4 2010/06/09 23:27:29 epasheva Exp $
 *
 * Filename: amdemux.c
 *
 * Purpose:  Contains routines related to handling DEMUX packets
 *           defined by the AM package.  This module is used for 
 *           both the SDK and API.
 *
 * NOTE:     The functions within this file build and parse
 *           packets that are defined in IDD_SwiDemuxMessaging.doc
 *
 * Copyright: © 2008 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* include files */
#include "am/amidefs.h"

/* Functions */

/*
 * Building DEMUX AM packet routines 
 */

/***********
 *
 * Name: ambuilddemuxrequest
 *
 * Purpose: Build a specific AM request packet including AM 
 *          packet header in the provided storage area given 
 *          the information contained in the parameter structure
 *             
 * Parms:   amsgpp          - Pointer to pointer to storage into which 
 *                            the message will be written
 *          deviceservice   - Device service as defined by Data Relay
 *          opcode          - operation code: 
 *                                0x0 Start Device Service, 
 *                                0x1 Stop Device Service
 *
 * Return:  Number of bytes occupied by the AM packet
 *
 * Abort:   none
 *
 * Notes:   This function builds an AM Demux Request packet defined in 
 *          IDD_SwiDemuxMessaging.doc.
 *
 *          AMDEMUXREQUEST packets travel from the API-side to the SDK
 *          side
 *
 **************/
global swi_uint16 ambuilddemuxrequest( 
    swi_uint8 **amsgpp, 
    swi_uint16 deviceservice, 
    swi_uint16 opcode)
{
    swi_uint8  *packetp;      /* Local copy of incoming packet pointer */
    swi_uint32  byteswritten; /* After parsing, contains 
                               * #bytes just written 
                               */

    /* Duplicate the pointer into our scratch location */
    packetp = *amsgpp;

    /* Build AM packet header */
    ambuildamheader( 
        AMDEMUXREQUEST, 
        AMHDRVERSION, 
        0x00, 
        AMDEMUXREQTOTAL, 
        &packetp);
        
    /* Add device service type */
    piput16( deviceservice, &packetp );
    
    /* Add operation code */
    piput16( opcode, &packetp );    

    /* Update the bytes written variable here */
    byteswritten = packetp - *amsgpp;
    
    /* Ensure that bytes written matches the defined constant for size */
    if( byteswritten != AMDEMUXREQTOTAL )
    {
        /* Combine both sizes into one and abort */
        byteswritten = byteswritten << 16 | AMDEMUXREQTOTAL;
        erAbort("Update AMDEMUXREQTOTAL", byteswritten );
    }
    return byteswritten;
}

/*************
 *
 * Name: ambuilddemuxdataind
 *
 * Purpose: Build a complete AMDEMUXDATAIND packet include the
 *          AM packet header in the provided storage area 
 *             
 * Parms:   amsgpp         - Pointer to pointer to storage into which the 
 *                           message will be written
 *          deviceservice  - Device service as defined by Data Relay
 *
 * Return:  Number of bytes occupied by the AM packet.
 *
 * Abort:   none
 *
 * Notes:   This function builds the AMDEMUXDATAIND packet defined in 
 *          IDD_SwiDemuxMessaging.doc.
 *
 *          AMDEMUXDATAIND packets travel from the SDK-side to the API
 *          side
 *
 **************/
global swi_uint32 ambuilddemuxdataind( 
    swi_uint8 **amsgpp,
    swi_uint16 deviceservice)
{
    swi_uint8  *packetp;     /* Local copy of incoming packet pointer */
    swi_uint32 byteswritten; /* After parsing, contains #bytes just read */

    /* Duplicate the pointer into our scratch location */
    packetp = *amsgpp;

    /* Build AM packet header */
    ambuildamheader(
        AMDEMUXDATAIND, 
        AMHDRVERSION, 
        0x00, 
        AMDEMUXDATAINDTOTAL, 
        &packetp);

    /* Add device service type */
    piput16( deviceservice, &packetp );

    /* Update the bytes read variable here */
    byteswritten = packetp - *amsgpp;

    if( byteswritten != AMDEMUXDATAINDTOTAL )
    {
        /* Combine both sizes into one and abort */
        byteswritten = byteswritten << 16 | AMDEMUXDATAINDTOTAL;
        erAbort("Update AMDEMUXDATAINDTOTAL\n", byteswritten );
    }
    /* Note: An externally added and variable-length part of the message
     *       follows within the packet after this point 
     */ 

    return byteswritten;
}

/*************
 *
 * Name: ambuilddemuxctlind
 *
 * Purpose: Build a complete AMDEMUXCTLIND packet include AM packet
 *          header in the provided storage area given the provide 
 *          information.
 *             
 * Parms:   amsgpp         - Pointer to pointer to storage into which the 
 *                           message will be written
 *          amresultcode   - result code to return in the response packet
 *          errorstringp   - Pointer to error string to be returned
 *          deviceservice  - Device service as defined by Data Relay task
 *          opcode         - Indicates the type of operation
 *          devicepresent  - Flag for device present
 *          eranumber      - Identifies unambiguously the Control Indication 
 *
 * Return:  Number of bytes occupied by the AM packet.
 *
 * Abort:   none
 *
 * Notes:   This function builds the AMDEMUXCTLIND packet defined in 
 *          IDD_SwiDemuxMessaging.doc.
 *
 *          AMDEMUXCTLIND packets travel from the SDK-side to the API
 *          side
 *
 **************/
global swi_uint16 ambuilddemuxctlind( 
    swi_uint8 **amsgpp, 
    enum swi_amresults amresultcode,
    swi_uint8 *errorstringp,
    swi_uint16 deviceservice,
    swi_uint8 opcode,
    swi_uint8 devicepresent,
    swi_uint32 eranumber)
{
    swi_uint8  *packetp;     /* Local copy of incoming packet pointer */
    swi_uint32 byteswritten; /* After parsing, contains #bytes just read */

    /* Duplicate the pointer into our scratch location */
    packetp = *amsgpp;

    /* Build AM packet header */
    ambuildamheader(
        AMDEMUXCTLIND, 
        AMHDRVERSION, 
        amresultcode, 
        AMDEMUXCTLINDTOTAL, 
        &packetp);
        
    /* Add device service type */
    piput16( deviceservice, &packetp );
    
    /* Add operation type */
    *packetp++ = opcode;
    
    /* Add device present */
    *packetp++ = devicepresent;
    
    /* Add era number */
    piput32( eranumber, &packetp );
    
    /* copy error string over - make sure it is NULL terminated */
    slmemset((char *)packetp, 0, AMMAXERRORSTRINGSZ);
    slstrncpy((char *)packetp, (const char *)errorstringp, AMMAXERRORSTRINGSZ);
    packetp += AMMAXERRORSTRINGSZ;
    
    /* Update the bytes read variable here */
    byteswritten = packetp - *amsgpp;

    if( byteswritten != AMDEMUXCTLINDTOTAL )
    {
        /* Combine both sizes into one and abort */
        byteswritten = byteswritten << 16 | AMDEMUXCTLINDTOTAL;
        erAbort("AMDEMUXCTLINDTOTAL needs updating\n", byteswritten );
    }

    return byteswritten;
}

/*
 * Parsing demux AM packet routines 
 */

/*************
 *
 * Name: amparsedemuxrequest
 *
 * Purpose: Parses an AM Demux request message.
 *             
 * Parms:   ampacketpp     - pointer to a pointer to the AM Demux
 *                           request packet.
 *          devservp       - Pointer to device service storage
 *          operationp     - Pointer to request operation field
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   Parses the fields from a Demux request AM packet 
 *          and returns the values to the caller in the parameters provided
 *
 *          This function parses the AM Demux request  packets defined in 
 *          IDD_SwiDemuxMessaging.doc
 *
 **************/
global void amparsedemuxrequest( 
    swi_uint8 **ampacketpp, 
    swi_uint16 *devservp,
    swi_uint16 *operationp)
{
    swi_uint8  *packetp;   /* Local copy of incoming packet pointer */
    swi_uint32 bytesread;  /* After parsing, contains #bytes just read */

    /* Duplicate the pointer into our scratch location */
    packetp = *ampacketpp;
    
    /* Proceed with field parsing */

    /* Extract device service */
    *devservp = piget16( &packetp );
    
    /* Extract operation code */
    *operationp = piget16( &packetp );
 
    /* Update the bytes read variable here */
    bytesread = packetp - *ampacketpp;

    /* Make sure the bytes read matches the packet size */
    if( bytesread != AMDEMUXREQSZ)
    {
        /* Combine both sizes into one and abort */
        bytesread = bytesread << 16 | AMDEMUXREQSZ;
        erAbort("Update AMDEMUXREQSZ", bytesread );
    }

    /* advance the caller's packet pointer pass all parsed fields */
    *ampacketpp += bytesread;
}

/*************
 *
 * Name: amparsedemuxdataind
 *
 * Purpose: This routine knows how to unpack the contents of an AM
 *          Demux Data Indication message
 *             
 * Parms:   ampacketpp     - pointer to a pointer to the AM Demux
 *                           Data Indication packet.
 *          deviceservice  - Device service as defined by Data Relay task
 *  
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   This function parses the AMDEMUXDATAIND packet defined in 
 *          IDD_SwiDemuxMessaging.doc.
 * 
 *          This function advances the passed in packet pointer past
 *          all parsed fields.
 *
 **************/
global void amparsedemuxdataind( 
    swi_uint8 **ampacketpp, 
    swi_uint16 *devservp)
{
    swi_uint8  *packetp;  /* Local copy of incoming packet pointer */
    swi_uint32 bytesread; /* After parsing, contains #bytes just read */

    /* Duplicate the pointer into our scratch location */
    packetp = *ampacketpp;

    /* Proceed with field parsing */
    /* Extract device service */
    *devservp = piget16( &packetp );

    /* Update the bytes read variable here */
    bytesread = packetp - *ampacketpp;

    /* Ensure the response size matches the constant */
    if( bytesread != AMDEMUXDATAINDSZ )
    {
        /* Combine both sizes into one variable and abort */
        bytesread = bytesread << 16 | AMDEMUXDATAINDSZ;
        erAbort("Update AMDEMUXDATAINDSZ\n", bytesread );
    }
    /* advance the caller's packet pointer to the
     * start of where the data might be.
     */
    *ampacketpp += bytesread;
}

/*************
 *
 * Name: amparsedemuxctlind
 *
 * Purpose: This routine knows how to unpack the contents of an AM
 *          Demux Control Indication message
 *             
 * Parms:   ampacketpp     - Pointer to a pointer to the AM Demux Control 
 *                           Indication packet
 *          devservp       - Pointer to device service storage   
 *          opcodep        - Pointer to operation type storage
 *          devicepresentp - Pointer to device present flag storage
 *          eranumberp     - Pointer to era number storage
 *          errorstringp   - Pointer to error string storage
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   This function parses the AMDEMUXCTLIND packet defined in 
 *          IDD_SwiDemuxMessaging.doc.
 * 
 *          This function advances the passed in packet pointer past
 *          all parsed fields.
 *
 **************/
global void amparsedemuxctlind( 
    swi_uint8 **ampacketpp, 
    swi_uint16 *devservp,
    swi_uint8 *opcodep,
    swi_uint8 *devicepresentp,
    swi_uint32 *eranumberp,
    swi_uint8 *errorstringp)
{
    swi_uint8  *packetp;  /* Local copy of incoming packet pointer */
    swi_uint32 bytesread; /* After parsing, contains #bytes just read */

    /* Duplicate the pointer into our scratch location */
    packetp = *ampacketpp;
    
    /* Proceed with field parsing */
    /* Extract device service */
    *devservp = piget16( &packetp );
    
    /* Extract operation code */    
    *opcodep = *packetp++;
    
    /* Extract device present flag */    
    *devicepresentp = *packetp++;
    
    /* Extract era number */    
    *eranumberp = piget32( &packetp );
    
    /* Extract error string */
    /* copy error string over - make sure it is NULL terminated */
    slstrncpy((char *)errorstringp, (const char *)packetp, AMMAXERRORSTRINGSZ);
    packetp += AMMAXERRORSTRINGSZ;
    
    /* Update the bytes read variable here */
    bytesread = packetp - *ampacketpp;

    /* Ensure the response size matches the constant */
    if( bytesread != AMDEMUXCTLINDSZ )
    {
        /* Combine both sizes into one variable and abort */
        bytesread = bytesread << 16 | AMDEMUXCTLINDSZ;
        erAbort("Update AMDEMUXCTLINDSZ\n", bytesread );
    }
    /* advance the caller's packet pointer pass all parsed fields */
    *ampacketpp += bytesread;
}

/*
 * $Log: amdemux.c,v $
 */

