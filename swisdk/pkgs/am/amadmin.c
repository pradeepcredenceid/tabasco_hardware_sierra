/*************
 *
 * $Id: amadmin.c,v 1.10 2010/06/09 23:24:24 epasheva Exp $
 *
 * Filename: amadmin.c
 *
 * Purpose:  Contains routines related to handling ADMIN packets
 *           defined by the AM package.  This module is used for 
 *           both the SDK and API.
 *
 * NOTE:     The functions within this file build and parse
 *           packets that are defined in IDD_SwiAdminMessaging.doc
 *
 * Copyright: © 2008 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* include files */
#include "am/amidefs.h"
#include "sl/sludefs.h"

/* NULL MAC Address */
swi_uint8 amnullmac[AMMACSIZE] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };

/* Functions */

/*
 * Building ADMIN AM packet routines 
 */

/***********
 *
 * Name: ambuildfwdwnrequest
 *
 * Purpose: Build a complete FWDWNREQUEST packet including AM packet header
 *          in the provided storage area given the information contained in 
 *          the parameter structure
 *             
 * Parms:   amsgpp          - Pointer to pointer to storage into which the 
 *                            message will be written
 *          amfwdwnparams   - Pointer to a structure containing the firmware
 *                            download parameters
 *
 * Return:  Number of bytes occupied by the AM packet
 *
 * Abort:   none
 *
 * Notes:   This function builds the FWDWNREQUEST packet defined in 
 *          IDD_SwiAdminMessaging.doc.
 *
 *          FWDWNREQUEST packets travel from the API-side to the SDK
 *          side
 *
 **************/
package swi_uint16 ambuildfwdwnrequest( 
   swi_uint8 **amsgpp, 
   struct amfwdwnrrparms *amfwdwnrrparmp)
{
    swi_uint8  *packetp;     /* Local copy of incoming packet pointer */
    swi_uint32 byteswritten; /* After parsing, contains #bytes just written */

    /* Duplicate the pointer into our scratch location */
    packetp = *amsgpp;

    /* Build AM packet header */
    ambuildamheader(
        AMFWDWNREQUEST, 
        AMHDRVERSION, 
        SWI_AMSUCCESS, 
        AMFWDWNREQTOTAL, 
        &packetp);

    /* Put file name in packet */
    piputmulti((const swi_uint8 *)&amfwdwnrrparmp->amfilename[0], &packetp,
                AMMAXFILENAMESZ); 

    /* Put image type in packet */
    *packetp++ = amfwdwnrrparmp->amfwimagetype;

    /* Put MP flag in packet */
    *packetp++ = amfwdwnrrparmp->ammpdownload;
    
    /* Put chunk size in packet */
    piput32( amfwdwnrrparmp->ammaxchunksize, &packetp );

    /* Update the bytes written variable here */
    byteswritten = packetp - *amsgpp;
    
    /* Ensure that bytes written matches the defined constant for size */
    if( byteswritten != AMFWDWNREQTOTAL )
    {
        /* Combine both sizes into one and abort */
        byteswritten = byteswritten << 16 | AMFWDWNREQTOTAL;
        erAbort("Update AMFWDWNREQTOTAL", byteswritten );
    }

    return byteswritten;
}
/*************
 *
 * Name: ambuildfwdwnresponse
 *
 * Purpose: Build a complete AMFWDWNRESPONSE packet include AM packet
 *          header in the provided storage area given the provide 
 *          information.
 *             
 * Parms:   amsgpp         - Pointer to pointer to storage into which the 
 *                           message will be written
 *          amresultcode   - result code to return in the response packet
 *          errorstringp   - Pointer to error string to be returned
 *          errorstringlen - Length of the error string to be returned
 *
 * Return:  Number of bytes occupied by the AM packet.
 *
 * Abort:   none
 *
 * Notes:   This function builds the AMFWDWNRESPONSE packet defined in 
 *          IDD_SwiAdminMessaging.doc.
 *
 *          AMFWDWNRESPONSE packets travel from the SDK-side to the API
 *          side
 *
 **************/
global swi_uint16 ambuildfwdwnresponse( 
    swi_uint8 **amsgpp, 
    enum swi_amresults amresultcode,
    swi_uint8 *errorstringp,
    swi_uint8 errorstringlen)
{
    swi_uint8  *packetp;     /* Local copy of incoming packet pointer */
    swi_uint32 byteswritten; /* After parsing, contains #bytes just read */

    /* Duplicate the pointer into our scratch location */
    packetp = *amsgpp;

    /* Build AM packet header */
    ambuildamheader(
        AMFWDWNRESPONSE, 
        AMHDRVERSION, 
        amresultcode, 
        AMFWDWNRESPTOTAL, 
        &packetp);
    
    /* if error string is longer than can be accommodated then truncate it */
    if(errorstringlen >= AMMAXERRORSTRINGSZ)
    {
        errorstringlen = AMMAXERRORSTRINGSZ - 1;
    }
    
    /* put error string length into the AM packet */
    *packetp++ = errorstringlen;
    
    /* copy error string over - make sure it is NULL terminated */
    slmemset((char *)packetp, 0, errorstringlen);
    slstrncpy((char *)packetp, (const char *)errorstringp, errorstringlen);
    packetp += AMMAXERRORSTRINGSZ;

    /* Update the bytes read variable here */
    byteswritten = packetp - *amsgpp;

    if( byteswritten != AMFWDWNRESPTOTAL )
    {
        /* Combine both sizes into one and abort */
        byteswritten = byteswritten << 16 | AMFWDWNRESPTOTAL;
        erAbort("AMFWDWNRESPTOTAL needs updating\n", byteswritten );
    }

    return byteswritten;
}

/*************
 *
 * Name: ambuildfwdwnnotify
 *
 * Purpose: Build a complete AMFWDWNNOTIFICATION packet include the
 *          AM packet header in the provided storage area given the 
 *          firmware download state specified by the caller.
 *             
 * Parms:   amsgpp         - Pointer to pointer to storage into which the 
 *                           message will be written
 *          fwdwnstate     - Current Firmware Download state to be used
 *                           in this notification packet.
 *
 * Return:  Number of bytes occupied by the AM packet.
 *
 * Abort:   none
 *
 * Notes:   This function builds the AMFWDWNNOTIFICATION packet defined in 
 *          IDD_SwiAdminMessaging.doc.
 *
 *          AMFWDWNNOTIFICATION packets travel from the SDK-side to the API
 *          side
 *
 **************/
global swi_uint16 ambuildfwdwnnotify( 
    swi_uint8 **amsgpp, 
    enum swi_amfwdwnnotiftype fwdwnstate)
{
    swi_uint8  *packetp;     /* Local copy of incoming packet pointer */
    swi_uint32 byteswritten; /* After parsing, contains #bytes just read */

    /* Duplicate the pointer into our scratch location */
    packetp = *amsgpp;

    /* Build AM packet header */
    ambuildamheader(
        AMFWDWNNOTIFICATION, 
        AMHDRVERSION, 
        SWI_AMSUCCESS, 
        AMFWDWNNOTFTOTAL, 
        &packetp);

    /* Firmware download state */
    *packetp++ = fwdwnstate;

    /* Update the bytes read variable here */
    byteswritten = packetp - *amsgpp;

    if( byteswritten != AMFWDWNNOTFTOTAL )
    {
        /* Combine both sizes into one and abort */
        byteswritten = byteswritten << 16 | AMFWDWNNOTFTOTAL;
        erAbort("Update AMFWDWNNOTFTOTAL\n", byteswritten );
    }

    return byteswritten;
}
/*************
 *
 * Name: ambuildairserverrequest
 *
 * Purpose: Build a AMAIRSRVREQ packet in the provided storage area
 *             
 * Parms:   amsgpp          - Pointer to pointer to storage into which the 
 *                            message will be written
 *          amairsrvrrparmp - Pointer to a caller-allocated structure
 *                            defined by AM package for exchanging 
 *                            Air Server Request/Response 
 *                            parameters.
 *
 * Return:  Number of bytes occupied by the AM packet
 *
 * Abort:   none
 *
 * Notes:   This function builds the AMAIRSRVREQ packet defined in 
 *          IDD_SwiAdminMessaging.doc, including the common AM header
 *
 *          AMAIRSRVREQ packets travel from the API-side to the SDK
 *          side
 *
 **************/
package swi_uint16 ambuildairserverrequest( 
    swi_uint8 **amsgpp, 
    struct amairserverparms *amairsrvrrparmp)
{
    swi_uint8  *packetp;     /* Local copy of incoming packet pointer */
    swi_uint32 byteswritten; /* After parsing, contains #bytes just written */

    /* Duplicate the pointer into our scratch location */
    packetp = *amsgpp;

    /* Proceed with field packing */
    
    /* Build AM packet header */
    ambuildamheader(
        AMAIRSRVREQ, 
        AMHDRVERSION, 
        SWI_AMSUCCESS, 
        AMAIRSRVREQTOTAL, 
        &packetp);
        
    /* pack the request type */
    *packetp++ = amairsrvrrparmp->amairoperation;
    
    /* pack device present flag - this field is unused in requests */
    *packetp++ = 0x00;
    
    /* pack era number - this field is unused in requests */
    piput32( 0x00, &packetp );

    /* pack the device type */
    piput32( amairsrvrrparmp->amairdevicetype, &packetp );

    /* pack the client MAC field, unused and zero-filled */
    piputmulti( amairsrvrrparmp->amairclimac, &packetp, AMMACSIZE );

    /* pack the server MAC field, unused and zero-filled */
    piputmulti( amairsrvrrparmp->amairservmac, &packetp, AMMACSIZE );

    /* Pack the apprunning field */
    *packetp++ = amairsrvrrparmp->amairapprunning;
        
    /* pack the product name string, unused and zero-filled */
    piputmulti( amairsrvrrparmp->amairprodstr, &packetp, AMPRODSTRSIZE );

   /* Pack the Technology type field */
    *packetp++ = amairsrvrrparmp->amairtechtype;
        
    /* Update the bytes written variable here */
    byteswritten = packetp - *amsgpp;
    
    /* Ensure that bytes written matches the defined constant for size */
    if( byteswritten != AMAIRSRVREQTOTAL )
    {
        /* Combine both sizes into one and abort */
        byteswritten = byteswritten << 16 | AMAIRSRVREQTOTAL;
        erAbort("Update AMAIRSRVREQTOTAL", byteswritten );
    }

    return byteswritten;

}
/*************
 *
 * Name: ambuildairserverresponse
 *
 * Purpose: Build a complete AMAIRSRVRESP packet include AM packet
 *          header in the provided storage area given the provide 
 *          information.
 *             
 * Parms:   amsgpp          - Pointer to pointer to storage into which the 
 *                            message will be written
 *          amresultcode    - Result code of requested operation
 *          amairsrvrrparmp - Pointer to a caller-allocated structure
 *                            defined by AM package for exchanging 
 *                            Air Server Request/Response 
 *                            parameters.
 *
 * Return:  Number of bytes occupied by the AM packet.
 *
 * Abort:   none
 *
 * Notes:   This function builds the AMAIRSRVRESP packet defined in 
 *          IDD_SwiAdminMessaging.doc.
 *
 *          AMAIRSRVRESP packets travel from the SDK-side to the API
 *          side
 *
 **************/
global swi_uint16 ambuildairserverresponse( 
    swi_uint8 **amsgpp, 
    enum swi_amresults amresultcode,
    struct amairserverparms *amairsrvrrparmp)
{
    swi_uint8  *packetp;     /* Local copy of incoming packet pointer */
    swi_uint32 byteswritten; /* After parsing, contains #bytes just read */

    /* Duplicate the pointer into our scratch location */
    packetp = *amsgpp;

    /* Build AM packet header */
    ambuildamheader(
        AMAIRSRVRESP, 
        AMHDRVERSION, 
        amresultcode, 
        AMAIRSRVRESPTOTAL, 
        &packetp);
        
    /* pack the request type */
    *packetp++ = amairsrvrrparmp->amairoperation;
    
    /* pack device present flag - this field is unused in requests */
    *packetp++ = amairsrvrrparmp->amairdevicestat;
    
    /* pack era number - this field is unused in requests */
    piput32( amairsrvrrparmp->amairchgera, &packetp );

    /* pack the device type */
    piput32( amairsrvrrparmp->amairdevicetype, &packetp );

    /* pack the client MAC field, unused and zero-filled */
    piputmulti( amairsrvrrparmp->amairclimac, &packetp, AMMACSIZE );

    /* pack the server MAC field, unused and zero-filled */
    piputmulti( amairsrvrrparmp->amairservmac, &packetp, AMMACSIZE );
        
    /* Pack the apprunning field */
    *packetp++ = amairsrvrrparmp->amairapprunning;
        
    /* pack the product name string, unused and zero-filled */
    piputmulti( amairsrvrrparmp->amairprodstr, &packetp, AMPRODSTRSIZE );
        
    /* Pack the Technology type field */
    *packetp++ = amairsrvrrparmp->amairtechtype;
        
    /* Update the bytes read variable here */
    byteswritten = packetp - *amsgpp;

    if( byteswritten != AMAIRSRVRESPTOTAL )
    {
        /* Combine both sizes into one and abort */
        byteswritten = byteswritten << 16 | AMAIRSRVRESPTOTAL;
        erAbort("AMAIRSRVRESPTOTAL needs updating\n", byteswritten );
    }

    return byteswritten;
}
/*************
 *
 * Name: ambuildairchangenotify
 *
 * Purpose: Build a AMAIRSRVCHGNOTIFY packet in the provided storage area
 *             
 * Parms:   amsgpp         - Pointer to pointer to storage into which the 
 *                           message will be written
 *          devicetype     - Code indicating the device type. One of enum
 *                           Swi_Type_Device
 *          devicepresent  - TRUE, device is connected, FALSE, device dis-
 *                           connected. Note, if FALSE is returned, the 
 *                           device type is the last known connected device
 *                           or INVALID
 *          apprunning     - TRUE, the modem is executing its application
 *                           FALSE, the modem is in boot-and-hold mode
 *          airchgera      - era number of the air server status change. 
 *                           Uniquely identifies devicepresent status
 *                           so that changes are propagated in correct order.
 *          prodstrp       - Pointer to product name string
 *
 * Return:  Number of bytes occupied by the AM packet
 *
 * Abort:   none
 *
 * Notes:   This function builds the AMAIRSRVCHGNOTIFY packet defined in 
 *          IDD_SwiAdminMessaging.doc, including the common AM header
 *
 *          AMAIRSRVCHGNOTIFY packets travel from the SDK-side to the API
 *          side
 *
 **************/
global swi_uint16 ambuildairchangenotify( 
    swi_uint8 **amsgpp, 
    swi_uint32 devicetype, 
    swi_bool devicepresent,
    swi_bool apprunning,
    swi_uint32 airchgera,
    swi_uint8 *prodstrp,
    swi_uint32 techtype)
{
    swi_uint8  *packetp;     /* Local copy of incoming packet pointer */
    swi_uint32 byteswritten; /* After parsing, contains #bytes just read */

    /* Duplicate the pointer into our scratch location */
    packetp = *amsgpp;

    /* Build AM packet header */
    ambuildamheader(
        AMAIRSRVCHGNOTIFY, 
        AMHDRVERSION, 
        SWI_AMSUCCESS, 
        AMNOTAIRCTOTAL, 
        &packetp);

    /* Operation Type comes next */
    *packetp++ = AMAIRSERVERCHANGE;

    /* Next indicate the presence of the device */
    *packetp++ = (swi_uint8) devicepresent;
    
    /* Now the era for this change */
    piput32( airchgera, &packetp );

    /* Now the device type */
    piput32( devicetype, &packetp );

    /* Now the client MAC field, unused and zero-filled */
    piputmulti( amnullmac, &packetp, sizeof(amnullmac) );

    /* Now the server MAC field, unused and zero-filled */
    piputmulti( amnullmac, &packetp, sizeof(amnullmac) );

    /* Store the modem's running state */
    *packetp++ = apprunning;
    
    /* Now the Product name string field, unused and zero-filled */
    piputmulti( prodstrp, &packetp, AMPRODSTRSIZE);

    /* Store the modem's Technology type */
    *packetp++ = (swi_uint8) techtype;
    
    /* Update the bytes read variable here */
    byteswritten = packetp - *amsgpp;

    if( byteswritten != AMNOTAIRCTOTAL )
    {
        /* Combine both sizes into one and abort */
        byteswritten = byteswritten << 16 | AMNOTAIRCTOTAL;
        erAbort("Update AMNOTAIRCTOTAL\n", byteswritten );
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
 * Name: ambuildmdmresetrequest
 *
 * Purpose: Build a modem reset request packet. 
 *             
 * Parms:   amsgpp         - Pointer to pointer to storage into which the 
 *                           message will be written
 *          resetp         - Pointer to a modem reset parameter structure
 *                           containing the reset type being requested
 *                           TRUE : Reset to boot and hold operation
 *                           FALSE: Reset to modem application
 *
 * Return:  Number of bytes occupied by the AM packet
 *
 * Abort:   none
 *
 * Notes:   This function builds the AMMDMRESETREQ packet defined in 
 *          IDD_SwiAdminMessaging.doc, including the common AM header
 *
 *          AMMDMRESETREQ packets travel from API side to the SDK-side 
 *
 **************/
global swi_uint16 ambuildmdmresetrequest( swi_uint8 **amsgpp, 
                                          struct ammdmresetparms *resetp )
{
    swi_uint8  *packetp;     /* Local copy of incoming packet pointer */
    swi_uint32 byteswritten; /* After parsing, contains #bytes just read */

    /* Duplicate the pointer into our scratch location */
    packetp = *amsgpp;

    /* Build AM packet header */
    ambuildamheader(
        AMMDMRESETREQ, 
        AMHDRVERSION, 
        SWI_AMSUCCESS, 
        AMRESETMDMREQTOTAL, 
        &packetp);

    /* Reset type comes next */
    *packetp++ = resetp->amresettype;

    /* Update the bytes read variable here */
    byteswritten = packetp - *amsgpp;

    if( byteswritten != AMRESETMDMREQTOTAL )
    {
        /* Combine both sizes into one and abort */
        byteswritten = byteswritten << 16 | AMRESETMDMREQTOTAL;
        erAbort("Update AMRESETMDMREQTOTAL\n", byteswritten );
    }

    /* advance the caller's packet pointer */
    *amsgpp += byteswritten;

    return byteswritten;
}
/*************
 *
 * Name: ambuildmdmresetresp
 *
 * Purpose: Build a modem reset response packet. 
 *             
 * Parms:   amsgpp         - Pointer to pointer to storage into which the 
 *                           message will be written
 *          resetp         - Pointer to a modem reset parameter structure
 *                           containing the reset type that was acted on
 *                           TRUE : modem was Reset to boot and hold operation
 *                           FALSE: modem was Reset to application
 *
 * Return:  Number of bytes occupied by the AM packet
 *
 * Abort:   none
 *
 * Notes:   This function builds the AMMDMRESETRESP packet defined in 
 *          IDD_SwiAdminMessaging.doc, including the common AM header
 *
 *          AMMDMRESETRESP packets travel from the SDK-side to the API
 *          side
 *
 **************/
global swi_uint16 ambuildmdmresetresp( swi_uint8 **amsgpp, 
                                       struct ammdmresetparms *resetp )
{
    swi_uint8  *packetp;     /* Local copy of incoming packet pointer */
    swi_uint32 byteswritten; /* After parsing, contains #bytes just read */

    /* Duplicate the pointer into our scratch location */
    packetp = *amsgpp;

    /* Build AM packet header */
    ambuildamheader(
        AMMDMRESETRESP, 
        AMHDRVERSION, 
        SWI_AMSUCCESS, 
        AMRESETMDMRESPTOTAL, 
        &packetp);

    /* Reset type comes next */
    *packetp++ = resetp->amresettype;

    /* Update the bytes read variable here */
    byteswritten = packetp - *amsgpp;

    if( byteswritten != AMRESETMDMRESPTOTAL )
    {
        /* Combine both sizes into one and abort */
        byteswritten = byteswritten << 16 | AMRESETMDMRESPTOTAL;
        erAbort("Update AMRESETMDMRESPTOTAL\n", byteswritten );
    }

    /* advance the caller's packet pointer */
    *amsgpp += byteswritten;

    return byteswritten;
}

/*************
 *
 * Name: ambuildusbportnamerequest
 *
 * Purpose: Build a AMPORTNAMEREQ packet in the provided storage area
 *             
 * Parms:   amsgpp              - Pointer to pointer to storage into which the 
 *                                message will be written
 *          amusbportnamerparmp - Pointer to a caller-allocated structure
 *                                defined by AM package for exchanging 
 *                                Port Name Request/Response 
 *                                parameters.
 *
 * Return:  Number of bytes occupied by the AM packet
 *
 * Abort:   Packet size does not match number of bytes written
 *
 * Notes:   This function builds the AMPORTNAMEREQ packet defined in 
 *          IDD_SwiAdminMessaging.doc, including the common AM header
 *
 *          AMPORTNAMEREQ packets travel from the API-side to the SDK
 *          side
 *
 **************/
package swi_uint16 ambuildusbportnamerequest( 
    swi_uint8 **amsgpp, 
    struct amusbportnameparms *amusbportnamerparmp)
{
    swi_uint8  *packetp;     /* Local copy of incoming packet pointer */
    swi_uint32 byteswritten; /* After parsing, contains #bytes just written */

    /* Duplicate the pointer into our scratch location */
    packetp = *amsgpp;

    /* Proceed with field packing */
    
    /* Build AM packet header */
    ambuildamheader(
        AMPORTNAMEREQ, 
        AMHDRVERSION, 
        SWI_AMSUCCESS, 
        AMPORTNAMEREQTOTAL, 
        &packetp);
        
    /* pack the port name fields, unused and zero-filled */
    slmemset((char *)packetp, 0, sizeof (struct amusbportnameparms));

    piputmulti( amusbportnamerparmp->amhipport, &packetp, AMPORTNAMESZ );
    piputmulti( amusbportnamerparmp->amdiagport, &packetp, AMPORTNAMESZ );
    piputmulti( amusbportnamerparmp->amnmeaport, &packetp, AMPORTNAMESZ );
    piputmulti( amusbportnamerparmp->amatcmdport, &packetp, AMPORTNAMESZ );
    piputmulti( amusbportnamerparmp->amdata1port, &packetp, AMPORTNAMESZ );
    piputmulti( amusbportnamerparmp->amdata2port, &packetp, AMPORTNAMESZ );
    piputmulti( amusbportnamerparmp->amdata3port, &packetp, AMPORTNAMESZ );
    piputmulti( amusbportnamerparmp->ammstorport, &packetp, AMPORTNAMESZ );
        
    /* Update the bytes written variable here */
    byteswritten = packetp - *amsgpp;
    
    /* Ensure that bytes written matches the defined constant for size */
    if( byteswritten != AMPORTNAMEREQTOTAL )
    {
        /* Combine both sizes into one and abort */
        byteswritten = byteswritten << 16 | AMPORTNAMEREQTOTAL;
        erAbort("Update AMPORTNAMEREQTOTAL", byteswritten );
    }

    return byteswritten;
}

/*************
 *
 * Name: ambuildusbportnameresponse
 *
 * Purpose: Build a complete AMPORTNAMERESP packet include AM packet
 *          header in the provided storage area given the provide 
 *          information.
 *             
 * Parms:   amsgpp              - Pointer to pointer to storage into which the 
 *                                message will be written
 *          amresultcode        - Result code of requested operation
 *          amusbportnamerparmp - Pointer to a caller-allocated structure
 *                                defined by AM package for exchanging 
 *                                Port Name Request/Response parameters.
 *
 * Return:  Number of bytes occupied by the AM packet.
 *
 * Abort:   Packet size does not match number of bytes written
 *
 * Notes:   This function builds the AMPORTNAMERESP packet defined in 
 *          IDD_SwiAdminMessaging.doc.
 *
 *          AMPORTNAMERESP packets travel from the SDK-side to the API
 *          side
 *
 **************/
global swi_uint16 ambuildusbportnameresponse( 
    swi_uint8 **amsgpp, 
    enum swi_amresults amresultcode,
    struct amusbportnameparms *amusbportnamerparmp)
{
    swi_uint8  *packetp;     /* Local copy of incoming packet pointer */
    swi_uint32 byteswritten; /* After parsing, contains #bytes just read */

    /* Duplicate the pointer into our scratch location */
    packetp = *amsgpp;

    /* Build AM packet header */
    ambuildamheader(
        AMPORTNAMERESP, 
        AMHDRVERSION, 
        amresultcode, 
        AMPORTNAMERESPTOTAL, 
        &packetp);
        
    /* pack the port name fields, unused and zero-filled */
    slmemset((char *)packetp, 0, sizeof (struct amusbportnameparms));

    piputmulti( amusbportnamerparmp->amhipport, &packetp, AMPORTNAMESZ );
    piputmulti( amusbportnamerparmp->amdiagport, &packetp, AMPORTNAMESZ );
    piputmulti( amusbportnamerparmp->amnmeaport, &packetp, AMPORTNAMESZ );
    piputmulti( amusbportnamerparmp->amatcmdport, &packetp, AMPORTNAMESZ );
    piputmulti( amusbportnamerparmp->amdata1port, &packetp, AMPORTNAMESZ );
    piputmulti( amusbportnamerparmp->amdata2port, &packetp, AMPORTNAMESZ );
    piputmulti( amusbportnamerparmp->amdata3port, &packetp, AMPORTNAMESZ );
    piputmulti( amusbportnamerparmp->ammstorport, &packetp, AMPORTNAMESZ );
    
    /* Update the bytes read variable here */
    byteswritten = packetp - *amsgpp;

    if( byteswritten != AMPORTNAMERESPTOTAL )
    {
        /* Combine both sizes into one and abort */
        byteswritten = byteswritten << 16 | AMPORTNAMERESPTOTAL;
        erAbort("AMPORTNAMERESPTOTAL needs updating\n", byteswritten );
    }

    return byteswritten;
}

/*************
 *
 * Name:    ambuilddevinforequest
 *
 * Purpose: Build a AMDEVINFOREQ packet in the provided storage area
 *             
 * Parms:   amsgpp           - Pointer to pointer to storage into which the 
 *                             message will be written
 *          amdevinforrparmp - Pointer to a caller-allocated structure
 *                             defined by AM package for exchanging 
 *                             Device Information Request/Response parameters.
 *
 * Return:  Number of bytes occupied by the AM packet
 *
 * Abort:   none
 *
 * Notes:   This function builds the AMDEVINFOREQ packet defined in 
 *          IDD_SwiAdminMessaging.doc, including the common AM header
 *
 *          AMDEVINFOREQ packets travel from the API-side to the SDK
 *          side
 *
 **************/
global swi_uint16 ambuilddevinforequest( 
    swi_uint8 **amsgpp, 
    struct amdevinfoparms *amdevinforrparmp)
{
    swi_uint8  *packetp;     /* Local copy of incoming packet pointer */
    swi_uint32 byteswritten; /* After parsing, contains #bytes just written */

    /* Duplicate the pointer into our scratch location */
    packetp = *amsgpp;

    /* Proceed with field packing */
    
    /* Build AM packet header */
    ambuildamheader(
        AMDEVINFOREQ, 
        AMHDRVERSION, 
        SWI_AMSUCCESS, 
        AMDEVINFOREQTOTAL, 
        &packetp);
        
    /* pack the Request version  */
    piput16( amdevinforrparmp->amdevversion, &packetp );

    /* pack the Request status - unused in request */
    piput16( amdevinforrparmp->amdevreqstatus, &packetp );

    /* pack the Request Info type */
    piput16( amdevinforrparmp->amdevinfotype, &packetp );

    /* pack the Info length - unused in request */
    piput16( amdevinforrparmp->amdevinfolen, &packetp );

    /* pack the Technology type- unused in request */
    piput16( amdevinforrparmp->amdevtechology, &packetp );

    /* pack the product name string, unused and zero-filled */
    piputmulti( amdevinforrparmp->amdevprodstr, &packetp, AMPRODSTRSIZE );

     /* pack the FW download protocol - unused in request */
    piput16( amdevinforrparmp->amdevfwdnldproto, &packetp );

    /* pack the Device ReEnum  - unused in request */
    piput16( amdevinforrparmp->amdevreenum, &packetp );
        
    /* pack the Device ReEnum  - unused in request */
    piput16( amdevinforrparmp->amdevcnssupported, &packetp );
        
    /* Update the bytes written variable here */
    byteswritten = packetp - *amsgpp;
    
    /* Ensure that bytes written matches the defined constant for size */
    if( byteswritten != AMDEVINFOREQTOTAL )
    {
        /* Combine both sizes into one and abort */
        byteswritten = byteswritten << 16 | AMDEVINFOREQTOTAL;
        erAbort("Update AMDEVINFOREQTOTAL", byteswritten );
    }

    return byteswritten;
}

/*************
 *
 * Name:    ambuilddevinforesponse
 *
 * Purpose: Build a complete AMDEVINFORESP packet, include AM packet
 *          header in the provided storage area given the provide 
 *          information.
 *             
 * Parms:   amsgpp           - Pointer to pointer to storage into which the 
 *                             message will be written
 *          amresultcode     - Result code of requested operation
 *          amdevinforrparmp - Pointer to a caller-allocated structure
 *                             defined by AM package for exchanging 
 *                             Device Information Request/Response 
 *                             parameters.
 *
 * Return:  Number of bytes occupied by the AM packet.
 *
 * Abort:   none
 *
 * Notes:   This function builds the AMDEVINFORESP packet defined in 
 *          IDD_SwiAdminMessaging.doc.
 *
 *          AMDEVINFORESP packets travel from the SDK-side to the API
 *          side
 *
 **************/
global swi_uint16 ambuilddevinforesponse( 
    swi_uint8 **amsgpp, 
    enum swi_amresults amresultcode,
    struct amdevinfoparms *amdevinforrparmp)
{
    swi_uint8  *packetp;     /* Local copy of incoming packet pointer */
    swi_uint32 byteswritten; /* After parsing, contains #bytes just read */

    /* Duplicate the pointer into our scratch location */
    packetp = *amsgpp;

    /* Build AM packet header */
    ambuildamheader(
        AMDEVINFORESP, 
        AMHDRVERSION, 
        amresultcode, 
        AMDEVINFORESPTOTAL, 
        &packetp);
        
    /* pack the Request version  */
    piput16( amdevinforrparmp->amdevversion, &packetp );

    /* pack the Request status  */
    piput16( amdevinforrparmp->amdevreqstatus, &packetp );

    /* pack the Request Info type */
    piput16( amdevinforrparmp->amdevinfotype, &packetp );

    /* pack the Info length */
    piput16( amdevinforrparmp->amdevinfolen, &packetp );

    /* pack the Technology type */
    piput16( amdevinforrparmp->amdevtechology, &packetp );

    /* pack the product name string */
    piputmulti( &amdevinforrparmp->amdevprodstr[0], &packetp, AMPRODSTRSIZE );

     /* pack the FW download protocol */
    piput16( amdevinforrparmp->amdevfwdnldproto, &packetp );

    /* pack the Device ReEnum   */
    piput16( amdevinforrparmp->amdevreenum, &packetp );
        
    /* pack the Device ReEnum   */
    piput16( amdevinforrparmp->amdevcnssupported, &packetp );
        
    /* Update the bytes read variable here */
    byteswritten = packetp - *amsgpp;

    if( byteswritten != AMDEVINFORESPTOTAL )
    {
        /* Combine both sizes into one and abort */
        byteswritten = byteswritten << 16 | AMDEVINFORESPTOTAL;
        erAbort("AMDEVINFORESPTOTAL needs updating\n", byteswritten );
    }

    return byteswritten;
}

/*
 * Parsing ADMIN AM packet routines 
 */

/*************
 *
 * Name: amparsefwdwnrequest
 *
 * Purpose: Parses an AM firmware download request message.
 *             
 * Parms:   ampacketpp  - pointer to a pointer to the AM firmware
 *                        download request packet.
 *          amrrparmp   - Pointer to a caller-allocated structure
 *                        defined by AM package for exchanging 
 *                        Firmware Download Request/Response 
 *                        parameters.
 *
 * Return:  none
 *
 * Abort:   Packet size does not match number of bytes parsed
 *
 * Notes:   Parses the fields from a firmware download request AM packet 
 *          and returns the values to the caller in the structure provided
 *
 *          This function parses the FWDWNREQUEST packet defined in 
 *          IDD_SwiAdminMessaging.doc
 *
 **************/
global void amparsefwdwnrequest( 
    swi_uint8 **ampacketpp, 
    struct amfwdwnrrparms *amrrparmp )
{
    swi_uint8  *packetp;  /* Local copy of incoming packet pointer */
    swi_uint32 bytesread; /* After parsing, contains #bytes just read */

    /* Duplicate the pointer into our scratch location */
    packetp = *ampacketpp;

    /* Proceed with field parsing */
    pigetmulti(&packetp, (swi_uint8 *)amrrparmp->amfilename, AMMAXFILENAMESZ);
    amrrparmp->amfwimagetype = *packetp++;
    amrrparmp->ammpdownload = *packetp++;
    amrrparmp->ammaxchunksize = piget32( &packetp );

    /* Update the bytes read variable here */
    bytesread = packetp - *ampacketpp;

    /* Make sure the bytes read matches the packet size */
    if( bytesread != AMFWDWNREQSZ )
    {
        /* Combine both sizes into one and abort */
        bytesread = bytesread << 16 | AMFWDWNREQSZ;
        erAbort("Update AMFWDWNREQSZ", bytesread );
    }

    /* advance the caller's packet pointer pass all parsed fields */
    *ampacketpp += bytesread;
}
/*************
 *
 * Name: amparsefwdwnresponse
 *
 * Purpose: This routine knows how to unpack the contents of an AM
 *          Firmware Download Response message
 *             
 * Parms:   ampacketpp      - pointer to a pointer to the AM Firmware
 *                           Download Response packet.
 *          amfwdwnrrparmp  - Pointer to a caller-allocated structure
 *                           defined by AM package for exchanging 
 *                           Firmware Download  Request/Response parameters. 
 *
 * Return:  none
 *
 * Abort:   Packet size does not match number of bytes parsed
 *
 * Notes:   This function parses the CNSRESPONSE packet defined in 
 *          IDD_SwiAdminMessaging.doc.
 * 
 *          This function advances the passed in packet pointer past
 *          all parsed fields.
 *
 **************/
package void amparsefwdwnresponse( 
    swi_uint8 **ampacketpp, 
    struct amfwdwnrrparms *amfwdwnrrparmp )
{
    swi_uint8  *packetp;  /* Local copy of incoming packet pointer */
    swi_uint32 bytesread; /* After parsing, contains #bytes just read */

    /* Duplicate the pointer into our scratch location */
    packetp = *ampacketpp;

    /* Proceed with field parsing */
    amfwdwnrrparmp->amerrorstrlen = *packetp++;
    pigetmulti(&packetp, (swi_uint8 *)&amfwdwnrrparmp->amerrorstr[0],
               AMMAXERRORSTRINGSZ);

    /* Update the bytes read variable here */
    bytesread = packetp - *ampacketpp;

    /* Ensure the response size matches the constant */
    if( bytesread != AMFWDWNRESPSZ )
    {
        /* Combine both sizes into one variable and abort */
        bytesread = bytesread << 16 | AMFWDWNRESPSZ;
        erAbort("Update AMFWDWNRESPSZ\n", bytesread );
    }
    /* advance the caller's packet pointer to the
     * start of where the errorstring might be.
     */
    *ampacketpp += bytesread;
}

/*************
 *
 * Name: amparsefwdwnnotify
 *
 * Purpose: This routine knows how to unpack the contents of an AM
 *          Firmware Download Notification message
 *             
 * Parms:   ampacketpp  - pointer to a pointer to the AM Firmware Download
 *                        response packet.
 *          amparmp     - Pointer to a caller-allocated structure
 *                        defined by AM package for exchanging 
 *                        Firmware Download request/response parameters.
 *
 * Return:  none
 *
 * Abort:   Packet size does not match number of bytes parsed
 *
 * Notes:   Parses the fields from a Firmware Download Notification AM packet 
 *          and returns the values to the caller in the structure provided
 *
 *          This function parses the FWDOWNLOAD packet defined in 
 *          IDD_SwiAdminMessaging.doc
 *
 **************/
package void amparsefwdwnnotify( 
    swi_uint8 **ampacketpp, 
    struct amfwdwnrrparms *amparmp )
{
    swi_uint8  *packetp;  /* Local copy of incoming packet pointer */
    swi_uint32 bytesread; /* After parsing, contains #bytes just read */

    /* Duplicate the pointer into our scratch location */
    packetp = *ampacketpp;

    /* Proceed with field parsing */
    amparmp->amfwdwnstate = *packetp++;

    /* Update the bytes read variable here */
    bytesread = packetp - *ampacketpp;

    /* Check the sizes match */
    if( bytesread != AMFWDWNNOTIFYSZ )
    {
        /* Combine both sizes into one and abort */
        bytesread = bytesread << 16 | AMFWDWNNOTIFYSZ;
        erAbort("Update AMFWDWNNOTIFYSZ\n", bytesread );
    }
}
/*************
 *
 * Name: amparseairserverrequest
 *
 * Purpose: Parses an AM air server request message.
 *             
 * Parms:   ampacketpp      - pointer to a pointer to the AM air
 *                            server request packet.
 *          amairparmsp     - Pointer to a caller-allocated structure
 *                            defined by AM package for exchanging 
 *                            Air Server Request/Response 
 *                            parameters.
 *
 * Return:  none
 *
 * Abort:   Packet size does not match number of bytes parsed
 *
 * Notes:   Parses the fields from a air server request AM packet 
 *          and returns the values to the caller in the structure provided
 *
 *          This function parses the AMAIRSRVREQ packet defined in 
 *          IDD_SwiAdminMessaging.doc
 *
 **************/
global void amparseairserverrequest( 
    swi_uint8 **ampacketpp, 
    struct amairserverparms *amairparmsp)
{
    swi_uint8  *packetp;  /* Local copy of incoming packet pointer */
    swi_uint32 bytesread; /* After parsing, contains #bytes just read */

    /* Duplicate the pointer into our scratch location */
    packetp = *ampacketpp;

    /* Unpack the contents of the incoming buffer */
    amairparmsp->amairoperation = *packetp++;

    /* Next comes the Device status, present or not */
    amairparmsp->amairdevicestat = *packetp++;
    
    /* Era number associated with Device status needs to be received too */
    amairparmsp->amairchgera = piget32( &packetp );

    /* and now comes the Device Type */
    amairparmsp->amairdevicetype = piget32( &packetp );

    /* next, read out the Client Mac Address */
    pigetmulti( &packetp, &amairparmsp->amairclimac[0], AMMACSIZE );

    /* next, read out the Server Mac Address */
    pigetmulti( &packetp, &amairparmsp->amairservmac[0], AMMACSIZE );

    /* Read out the apprunning field */
    amairparmsp->amairapprunning = *packetp++;

    /* finally, read out the Product name string */
    pigetmulti( &packetp, &amairparmsp->amairprodstr[0], AMPRODSTRSIZE );

    /* Read out the Technology type field */
    amairparmsp->amairtechtype = *packetp++;

    /* Update the bytes read variable here */
    bytesread = packetp - *ampacketpp;

    /* Make sure the bytes read matches the packet size */
    if( bytesread != AMAIRSRVREQSZ )
    {
        /* Combine both sizes into one and abort */
        bytesread = bytesread << 16 | AMAIRSRVREQSZ;
        erAbort("Update AMAIRSRVREQSZ", bytesread );
    }

    /* advance the caller's packet pointer pass all parsed fields */
    *ampacketpp += bytesread;
}
/*************
 *
 * Name: amparseairserverresponse
 *
 * Purpose: This routine knows how to unpack the contents of an AM
 *          air server response message
 *             
 * Parms:   ampacketpp      - pointer to a pointer to the AM Firmware
 *                            Download Response packet.
 *          amairparmsp     - Pointer to a caller-allocated structure
 *                            defined by AM package for exchanging 
 *                            Air Server Request/Response 
 *                            parameters.
 *
 *
 * Return:  none
 *
 * Abort:   Packet size does not match number of bytes parsed
 *
 * Notes:   This function parses the AMAIRSRVRESP packet defined in 
 *          IDD_SwiAdminMessaging.doc.
 * 
 *          This function advances the passed in packet pointer past
 *          all parsed fields.
 *
 **************/
package void amparseairserverresponse( 
    swi_uint8 **ampacketpp, 
    struct amairserverparms *amairparmsp)
{
    swi_uint8  *packetp;  /* Local copy of incoming packet pointer */
    swi_uint32 bytesread; /* After parsing, contains #bytes just read */

    /* Duplicate the pointer into our scratch location */
    packetp = *ampacketpp;

    /* Unpack the contents of the incoming buffer */
    amairparmsp->amairoperation = *packetp++;

    /* Next comes the Device status, present or not */
    amairparmsp->amairdevicestat = *packetp++;
    
    /* Era number associated with Device status needs to be received too */
    amairparmsp->amairchgera = piget32( &packetp );

    /* and now comes the Device Type */
    amairparmsp->amairdevicetype = piget32( &packetp );

    /* next, read out the Client Mac Address */
    pigetmulti( &packetp, &amairparmsp->amairclimac[0], AMMACSIZE );

    /* next, read out the Server Mac Address */
    pigetmulti( &packetp, &amairparmsp->amairservmac[0], AMMACSIZE );
    
    /* Read out the apprunning field */
    amairparmsp->amairapprunning = *packetp++;

    /* finally, read out the Product name string */
    pigetmulti( &packetp, &amairparmsp->amairprodstr[0], AMPRODSTRSIZE );
    
    /* Read out the Technology Type field */
    amairparmsp->amairtechtype = *packetp++;

    /* Update the bytes read variable here */
    bytesread = packetp - *ampacketpp;

    /* Ensure the response size matches the constant */
    if( bytesread != AMAIRSRVRESPSZ )
    {
        /* Combine both sizes into one variable and abort */
        bytesread = bytesread << 16 | AMAIRSRVRESPSZ;
        erAbort("Update AMAIRSRVRESPSZ\n", bytesread );
    }
    /* advance the caller's packet pointer to the
     * start of where the errorstring might be.
     */
    *ampacketpp += bytesread;
}
/*************
 *
 * Name: amparseairchgnotify
 *
 * Purpose: This routine knows how to unpack the contents of an AM
 *          AirServer Change Notification packet
 *             
 * Parms:   amaircpacketpp - pointer to a pointer to the AM AirServer
 *                           Change Response packet buffer.
 *          amairparmsp    - Pointer to a caller-allocated structure
 *                           defined by AM package for exchanging 
 *                           Air Server parameters.
 *
 * Return:  none
 *
 * Abort:   Packet size does not match number of bytes parsed
 *
 *
 * Notes:   Parses an Air Server Change notification packet, placing
 *          the values parsed into the caller-provided structure
 *
 *          On return the packet pointer pointer is advanced to point
 *          to the last byte beyond the end of the notification packet
 *
 **************/
package void amparseairchgnotify( 
    swi_uint8 **amaircpacketpp, 
    struct amairserverparms *amairparmsp )
{
    swi_uint8  *packetp;  /* Local copy of incoming packet pointer */
    swi_uint32 bytesread; /* After parsing, contains #bytes just read */

    /* Make a local copy of the incoming pointer */
    packetp = *amaircpacketpp;

    /* Unpack the contents of the incoming buffer */
    amairparmsp->amairoperation = *packetp++;

    /* Next comes the Device status, present or not */
    amairparmsp->amairdevicestat = *packetp++;
    
    /* Era number associated with Device status needs to be received too */
    amairparmsp->amairchgera = piget32( &packetp );

    /* and now comes the Device Type */
    amairparmsp->amairdevicetype = piget32( &packetp );

    /* next, read out the Client Mac Address */
    pigetmulti( &packetp, &amairparmsp->amairclimac[0], AMMACSIZE );

    /* next, read out the Server Mac Address */
    pigetmulti( &packetp, &amairparmsp->amairservmac[0], AMMACSIZE );

    /* next, read the apprunning field */
    amairparmsp->amairapprunning = *packetp++;
    
    /* next, read the apprunning field */
    pigetmulti( &packetp, &amairparmsp->amairprodstr[0], AMPRODSTRSIZE );
    
    /* next, read the Technology type field */
    amairparmsp->amairtechtype = *packetp++;
    
    /* Update the bytes read variable here */
    bytesread = packetp - *amaircpacketpp;

    /* Make sure the bytes read matches the packet size */
    if( bytesread != AMNOTIFYAIRCSZ )
    {
        /* Combine both sizes into one and abort */
        bytesread = bytesread << 16 | AMNOTIFYAIRCSZ;
        erAbort("Update AMNOTIFYAIRCSZ", bytesread );
    }

    /* advance the caller's packet pointer to the
     * start of the CNS Parameter section using 
     * the incoming length 
     */
    *amaircpacketpp += bytesread;
}

/*************
 *
 * Name: amparseportnameresponse
 *
 * Purpose: This routine knows how to unpack the contents of an AM
 *          port name response message
 *             
 * Parms:   ampacketpp      - pointer to a pointer to the AM Usb
 *                            Port Name packet.
 *          amportnameparmsp- Pointer to a caller-allocated structure
 *                            defined by AM package for exchanging 
 *                            Port Name Request/Response parameters.
 *
 *
 * Return:  none
 *
 * Abort:   Packet size does not match number of bytes parsed
 *
 * Notes:   This function parses the AMPORTNAMERESP packet defined in 
 *          IDD_SwiAdminMessaging.doc.
 * 
 *          This function advances the passed in packet pointer past
 *          all parsed fields.
 *
 **************/
package void amparseportnameresponse( 
    swi_uint8 **ampacketpp, 
    struct amusbportnameparms *amportnameparmsp)
{
    swi_uint8  *packetp;  /* Local copy of incoming packet pointer */
    swi_uint32 bytesread; /* After parsing, contains #bytes just read */

    /* Duplicate the pointer into our scratch location */
    packetp = *ampacketpp;

    /* just copy the structure, zero-fill first */
    slmemset((char *) amportnameparmsp, 0, sizeof (struct amusbportnameparms));
    slmemcpy(amportnameparmsp, packetp, AMPORTNAMERESPSZ);

    packetp += AMPORTNAMERESPSZ;
    /* Update the bytes read variable here */
    bytesread = packetp - *ampacketpp;

    /* Ensure the response size matches the constant */
    if( bytesread != AMPORTNAMERESPSZ )
    {
        /* Combine both sizes into one variable and abort */
        bytesread = bytesread << 16 | AMPORTNAMERESPSZ;
        erAbort("Update AMPORTNAMERESPSZ\n", bytesread );
    }
}

/*************
 *
 * Name: amparsemdmresetrequest
 *
 * Purpose: Parse a modem reset request packet. 
 *             
 * Parms:   ampacketpp     - Pointer to pointer to storage containing the
 *                           modem reset request packet
 *          resetp         - Pointer to the AM modem reset parameters 
 *                           structure into which the parsed information
 *                           will be written
 *
 * Return:  None
 *
 * Abort:   Packet size does not match number of bytes parsed
 *
 * Notes:   None
 *
 **************/
global void amparsemdmresetrequest( swi_uint8 **ampacketpp, 
                                struct ammdmresetparms *resetp )
{
    swi_uint8  *packetp;  /* Local copy of incoming packet pointer */
    swi_uint32 bytesread; /* After parsing, contains #bytes just read */

    /* Make a local copy of the incoming pointer */
    packetp = *ampacketpp;

    /* Unpack the contents of the incoming buffer */
    resetp->amresettype = *packetp++;

    /* Update the bytes read variable here */
    bytesread = packetp - *ampacketpp;

    /* Make sure the bytes read matches the packet size */
    if( bytesread != AMRESETMDMREQSZ )
    {
        /* Combine both sizes into one and abort */
        bytesread = bytesread << 16 | AMRESETMDMREQSZ;
        erAbort("Update AMRESETMDMREQSZ", bytesread );
    }

    /* Adjust the caller's pointer */
    *ampacketpp += bytesread;
}

/*************
 *
 * Name: amparsemdmresetresp
 *
 * Purpose: Parse a modem reset response packet. 
 *             
 * Parms:   ampacketpp     - Pointer to pointer to storage containing the
 *                           modem reset response/notification packet
 *          resetp         - Pointer to the AM modem reset parameters 
 *                           structure into which the parse information
 *                           will be written
 *
 * Return:  None
 *
 * Abort:   Packet size does not match number of bytes parsed
 *
 * Notes:   The response and notification packets have an identical format
 *          so this function is used to parse both, however, if the packets
 *          become different relative to one another, then this function 
 *          will need to be split into two, one for notifications and the 
 *          other for responses. Note also that the size definitions will
 *          also need to be split for the erAbort() check, below.
 *
 **************/
global void amparsemdmresetresp( swi_uint8 **ampacketpp, 
                                 struct ammdmresetparms *resetp )
{
    swi_uint8  *packetp;  /* Local copy of incoming packet pointer */
    swi_uint32 bytesread; /* After parsing, contains #bytes just read */

    /* Make a local copy of the incoming pointer */
    packetp = *ampacketpp;

    /* Unpack the contents of the incoming buffer */
    resetp->amresettype = *packetp++;

    /* Update the bytes read variable here */
    bytesread = packetp - *ampacketpp;

    /* Make sure the bytes read matches the packet size */
    if( bytesread != AMRESETMDMRESPSZ )
    {
        /* Combine both sizes into one and abort */
        bytesread = bytesread << 16 | AMRESETMDMRESPSZ;
        erAbort("Update AMRESETMDMRESPSZ", bytesread );
    }

    /* Adjust the caller's pointer */
    *ampacketpp += bytesread;
}

/*************
 *
 * Name: amparsedevinforequest
 *
 * Purpose: Parse a device information request packet. 
 *             
 * Parms:   ampacketpp     - Pointer to pointer to storage containing the
 *                           device information request packet
 *          devinfop       - Pointer to the AM Device information parameters 
 *                           structure into which the parsed information
 *                           will be written
 *
 * Return:  None
 *
 * Abort:   Packet size does not match number of bytes parsed
 *
 * Notes:   None
 *
 **************/
global void amparsedevinforequest( swi_uint8 **ampacketpp, 
                                   struct amdevinfoparms *devinfop )
{
    swi_uint8  *packetp;  /* Local copy of incoming packet pointer */
    swi_uint32 bytesread; /* After parsing, contains #bytes just read */

    /* Make a local copy of the incoming pointer */
    packetp = *ampacketpp;

    /* Unpack the Request version  */
    devinfop->amdevversion = piget16( &packetp );

    /* Unpack the Request status  */
    devinfop->amdevreqstatus = piget16( &packetp );

    /* Unpack the Request Info type */
    devinfop->amdevinfotype = piget16( &packetp );

    /* Unpack the Info length */
    devinfop->amdevinfolen = piget16( &packetp );

    /* Unpack the Technology type */
    devinfop->amdevtechology = piget16( &packetp );

    /* Unpack the product name string */
    pigetmulti( &packetp, &devinfop->amdevprodstr[0], AMPRODSTRSIZE );
    
     /* Unpack the FW download protocol */
    devinfop->amdevfwdnldproto = piget16( &packetp );

    /* Unpack the Device ReEnum field */
    devinfop->amdevreenum = piget16( &packetp );

   /* Unpack the CNS Supported field */
    devinfop->amdevcnssupported = piget16( &packetp );

    /* Update the bytes read variable here */
    bytesread = packetp - *ampacketpp;

    /* Make sure the bytes read matches the packet size */
    if( bytesread != AMDEVINFOREQSZ )
    {
        /* Combine both sizes into one and abort */
        bytesread = bytesread << 16 | AMDEVINFOREQSZ;
        erAbort("Update AMDEVINFOREQSZ", bytesread );
    }

    /* Adjust the caller's pointer */
    *ampacketpp += bytesread;
}

/*************
 *
 * Name: amparsedevinforesp
 *
 * Purpose: Parse a device information response packet. 
 *             
 * Parms:   ampacketpp     - Pointer to pointer to storage containing the
 *                           device information response packet
 *          devinfop       - Pointer to the AM Device Information parameters 
 *                           structure into which the parse information
 *                           will be written
 *
 * Return:  None
 *
 * Abort:   Packet size does not match number of bytes parsed
 *
 * Notes:   
 * 
 **************/
global void amparsedevinforesp( swi_uint8 **ampacketpp, 
                                struct amdevinfoparms *devinfop )
{
    swi_uint8  *packetp;  /* Local copy of incoming packet pointer */
    swi_uint32 bytesread; /* After parsing, contains #bytes just read */

    /* Make a local copy of the incoming pointer */
    packetp = *ampacketpp;

    /* Unpack the Request version  */
    devinfop->amdevversion = piget16( &packetp );

    /* Unpack the Request status  */
    devinfop->amdevreqstatus = piget16( &packetp );

    /* Unpack the Request Info type */
    devinfop->amdevinfotype = piget16( &packetp );

    /* Unpack the Info length */
    devinfop->amdevinfolen = piget16( &packetp );

    /* Unpack the Technology type */
    devinfop->amdevtechology = piget16( &packetp );

    /* Unpack the product name string */
    pigetmulti( &packetp, &devinfop->amdevprodstr[0], AMPRODSTRSIZE );
    
     /* Unpack the FW download protocol */
    devinfop->amdevfwdnldproto = piget16( &packetp );

    /* Unpack the Device ReEnum field */
    devinfop->amdevreenum = piget16( &packetp );

    /* Unpack the CNS supported field */
    devinfop->amdevcnssupported = piget16( &packetp );

    /* Update the bytes read variable here */
    bytesread = packetp - *ampacketpp;

    /* Make sure the bytes read matches the packet size */
    if( bytesread != AMDEVINFORESPSZ )
    {
        /* Combine both sizes into one and abort */
        bytesread = bytesread << 16 | AMDEVINFORESPSZ;
        erAbort("Update AMDEVINFORESPSZ", bytesread );
    }
}

/*
 * $Log: amadmin.c,v $
 */

