/*************
 *
 * $Id: am.c,v 1.9 2010/06/09 23:23:23 epasheva Exp $
 *
 * Filename: am.c
 *
 * Purpose:  AM Package common routines
 *
 * Copyright: © 2008-2011 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* include files */
#include "am/amidefs.h"
#include <string.h>


/* Length of our initial string before expansion */
#define BLANKTAGLEN 9
/* Number of characters to skip when extracting the Tag name */
#define NUMSKIP 7
#define MAX_TAGNAME_LENGTH 128

/* Local storage */

/* When a CVS file is tagged, the following is expanded to
 * $Name:  $
 * Note: Do not change the length of string $Name:  $ 
 *       without changing BLANKTAGLEN
 */
local char *TagNamep = "$Name:  $";
local char lTagname[MAX_TAGNAME_LENGTH];
local char *lTagnamep = lTagname;
/*
 * Use hardcode for now , until we can figure out scripted way
 */
 #define SDK_VERSION "1.4.0.2"

/*************
 *
 * Name: amgethdrptr 
 *
 * Purpose: To accept a pointer to any AM packet's Parameter field and
 *          return a pointer to where this particular  AM packet should
 *          begin. 
 *             
 * Parms:   parmp     - Pointer to the first byte of the parameter
 *                      field (payload)
 *          amtype    - Which AM Message type to compute the offset
 *                      for.
 *          bufstartp - Pointer to the absolute beginning of the buffer
 *                      needed to ensure we don't back past the beginning
 *                      of this allocated memory buffer
 *
 * Return:  Pointer to the first byte of where to start building the AM
 *          packet
 *
 * Abort:   If reversing the pointer goes before the beginning of the
 *          memory buffer. This means that the AM packet header is
 *          bigger than the combined sizes of all  headers
 *          which could only happen if modifications were recently made
 *          to one or more of the headers
 *
 * Notes:   Reuses memory and avoids copying of the payload.
 *
 **************/
global swi_uint8 *amgethdrptr( swi_uint8 *parmp, 
                               swi_uint16 amtype, 
                               swi_uint8 *bufstartp )
{
    swi_uint8 *amhdrp;

    switch( amtype )
    {
        case AMCNSNOTIFICATION:
            /* Take the Cns parameter field pointer
             * and subtract the size of the AM hdr
             */
            amhdrp = parmp - AMCNSNOTFTOTAL;
            break;

        case AMCNSRESPONSE:
            /* Take the Cns parameter field pointer
             * and subtract the size of the AM hdr
             */
            amhdrp = parmp - AMCNSRESPTOTAL;
            break;
            /* Elina: will add all AM packet types shortly */            
            case AMDEMUXREQUEST:
             /* Take the parameter field pointer
             * and subtract the size of the AM hdr
             */
            amhdrp = parmp - AMDEMUXREQTOTAL;
            break;
            
            case AMDEMUXDATAIND:
            /* Take the parameter field pointer
             * and subtract the size of the AM hdr
             */
            amhdrp = parmp - AMDEMUXDATAINDTOTAL;
            break;
            
            case AMDEMUXCTLIND:  
            /* Take the parameter field pointer
             * and subtract the size of the AM hdr
             */
            amhdrp = parmp - AMDEMUXCTLINDTOTAL;
            break;

        default: 
           erAbort("No amtype specified", amtype);
    }

    if( bufstartp > amhdrp )
        erAbort("AM CNS header too big", amtype );
    
    return amhdrp;
}


/*************
 *
 * Name: ambuildamheader
 *
 * Purpose: To build an AM packet header in the given packet buffer
 *             
 * Parms:   amtype          - AM packet type to be put in header
 *          amheaderversion - AM header version to be put in header
 *          amresultcode    - Result Code to be put in header
 *          ampacketlength  - Length of AM packet to be put in header
 *          packetpp        - Memory location of packet buffer pointer
 *                          in which AM packet header will be constructed.
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   The packet header will be advanced past the AM packet header
 *          by this function.
 *
 **************/
package void ambuildamheader(
    swi_uint16 amtype,
    swi_uint16 amheaderversion,
    swi_uint8  amresultcode,
    swi_uint16 ampacketlength,
    swi_uint8 **packetpp)
{
    swi_uint8 *packetp;  /* temporary packet pointer */
    
    /* initialize packet pointer to the beginning of the packet */
    packetp = *packetpp;
    
    /* pack the AM packet type */
    piput16( amtype, &packetp );

    /* pack the current AM packet protocol header version */
    piput16( amheaderversion, &packetp );
    
    /* pack the AM result code */
    *packetp++ = amresultcode;

    /* pack the total length of the AM packet */
    piput16( ampacketlength, &packetp );
    
    /* 
     * return a packet pointer that has been incremented 
     * past the header fields 
     */
    *packetpp = packetp;
}
/*************
 *
 * Name: amparseamheader
 *
 * Purpose: To parse an AM packet header in the given packet buffer
 *             
 * Parms:   amtypep          - Location to place parsed AM packet type
 *          amheaderversionp - Location to place parsed AM header version
 *          amresultcodep    - Location to place parsed result code 
 *          ampacketlengthp  - Location to place parsed length of AM packet 
 *          packetpp         - Location of packet buffer pointer which is to
 *                           be parsed.
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   The packet header will be advanced past the AM packet header
 *          by this function.
 *
 **************/
package void amparseamheader(
    swi_uint16 *amtypep,
    swi_uint16 *amheaderversionp,
    swi_uint8  *amresultcodep,
    swi_uint16 *ampacketlengthp,
    swi_uint8 **packetpp)
{
    swi_uint8 *packetp;  /* temporary packet pointer */
    
    /* initialize packet pointer to the beginning of the packet */
    packetp = *packetpp;
    
    /* unpack the AM packet type */
    *amtypep = piget16( &packetp );
    
    /* 
     * unpack the AM packet protocol header version 
     * to which this packet conforms
     */
    *amheaderversionp = piget16( &packetp );
    
    /* pack the total length of the AM packet */
    *amresultcodep = *packetp++;
    
    /* 
     * return a packet pointer that has been incremented 
     * past the header fields 
     */
    *ampacketlengthp = piget16( &packetp );
    
    /* return the formwarded packet pointer */
    *packetpp = packetp;
}

/*************
 *
 * Name: amgettagname
 *
 * Purpose: To return a pointer to a string indicating the tag name or 
 *          (NO TAG)
 *             
 * Parms:   none
 *
 * Return:  Pointer to the tag name string
 *
 * Abort:   None
 *
 * Notes:   This feature is here because this object is available to both
 *          the SDK daemon and Application program.
 *          This is useful for tracking the SDK version out in the field, which
 *          should be CVS tagged.
 *          *** This will break if we move away from CVS ** 
 *
 **************/
global swi_uint8 *amgettagname( void )
{
    /* Return checked-out substituted name or (NO TAG) if untagged */
    slmemset ((char *)lTagnamep, 0, MAX_TAGNAME_LENGTH);
    if ( slstrlen(TagNamep) == BLANKTAGLEN )
    {
        slmemcpy (lTagnamep, SDK_VERSION, strlen (SDK_VERSION));
    }
    else
    {
        /* Strip leading and ending $ and "Name: " */
        slmemcpy (lTagnamep, TagNamep+NUMSKIP, strlen (TagNamep) - NUMSKIP -1);
    }
    return ((swi_uint8 *)lTagnamep);
}
/*
 * $Log: am.c,v $
 */

