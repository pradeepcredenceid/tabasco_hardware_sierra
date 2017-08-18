/*
 *  $Id: amtest.c,v 1.7 2010/06/09 23:28:39 epasheva Exp $
 *
 *  Filename: amtest.c
 *
 *  Purpose:  Contains code to test the AM package
 *
 * Copyright: © 2008 Sierra Wireless, Inc., all rights reserved
 *
 * Note:      none
 *
 */

/* Linux definitions */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>

/* include files */
#include "aa/aaglobal.h"
#include "am/amudefs.h"
#include "am/amidefs.h"
/* 
 * Name:    amcnsreqpacket
 *
 * Purpose: Contains a CNSREQUEST packet formatted according to the 
 *          definition in IDD_CnSMessaging, stored in the project's 
 *          IDD documentation folder
 *
 * Members: Array of bytes as follows,
 *
 *
 * Note:    None
 *
 */
swi_uint8 amcnsreqpacket[] = {
    0x00,
    AMCNSREQUEST,     /* Packet type */
    0x00, 
    AMHDRVERSION,
    0x01,             /* 0x01 - dummy result code */
    0x00,             /* of bytes following */
    0x11,             /* Message data length, total no. */
    0x03,             /* Operation Type - Set Request */
    SWI_AMGSM,        /* Air Interface type */
    0x10,             /* 0x1043 */
    0x43,             /* Byte 1 of the CnS object ID */
    0x00,             /* msbyte. 0x0010 bytes long */
    0x0A,             /* Lsbyte of the parameter length */
    0x00,             /* msbyte of timeout, 100 ms */
    0x64,             /* lsbyte of the timeout */
    0x77,             /* Application parameter field */
    0x77,
    0x77,
    0x77,
    0x01,             /* Cns Parameter follows, 1..10 */
    0x02,
    0x03,
    0x04,
    0x05,
    0x06,
    0x07,
    0x08,
    0x09,
    0x0A
};

/* 
 * Name:    amcnsresppacket
 *
 * Purpose: Contains a CNSRESPONSE packet formatted according to the 
 *          definition in IDD_CnSMessaging, stored in the project's 
 *          IDD documentation folder
 *
 * Members: Array of bytes as follows,
 *
 *
 * Note:    None
 *
 */
swi_uint8 amcnsresppacket[] = {
    0x00,
    AMCNSRESPONSE,    /* Packet type */
    0x00,
    AMHDRVERSION,
    SWI_AMNODEVICE,   /* error result code */
    0x00,             /* of bytes following */
    0x11,             /* Message data length, total no. */
    0x03,             /* Operation Type - Get Response */
    SWI_AMCNSERROR,   /* Result code */
    0x10,             /* 0x1003 */
    0x03,             
    0x77,             /* Application parameter field */
    0x77,
    0x77,
    0x77, 
    0x00,             /* msbyte. 0x0010 bytes long */
    0x0A,             /* Lsbyte of the parameter length */
    0x41,             /* Cns Parameter follows, 1..10 */
    0x42,
    0x43,
    0x44,
    0x45,
    0x46,
    0x47,
    0x48,
    0x49,
    0x4A
};

/* 
 * Name:    amdemuxrequestpacket
 *
 * Purpose: Contains a AMDEMUXREQUEST packet formatted according to the 
 *          definition in IDD_DemuxMessaging, stored in the project's 
 *          IDD documentation folder
 *
 * Members: Array of bytes as follows,
 *
 *
 * Note:    None
 *
 */
swi_uint8 amdemuxrequestpacket[] = {
    AMDEMUXREQUEST,     /* Packet type */
    0x00, 
    AMHDRVERSION,
    0x01,       /* 0x01 - dummy result code */
    0x00,       /* don't care for now... */
    0x0,        /*  */
    0x0,        /*  */
    0x0,        /* Device Service Type 2 bytes */
    0x0,        /* DM service */
    0x0,        /* operation code byte 1 */
    0x00        /* operation code byte 2 - start request */
};

/* 
 * Name:    amdemuxdataindpacket
 *
 * Purpose: Contains a AMDEMUXDATAIND packet formatted according to the 
 *          definition in IDD_DemuxMessaging, stored in the project's 
 *          IDD documentation folder
 *
 * Members: Array of bytes as follows,
 *
 *
 * Note:    None
 *
 */
swi_uint8 amdemuxdataindpacket[] = {
    AMDEMUXDATAIND,     /* Packet type */
    0x00, 
    AMHDRVERSION,
    0x01,        /* 0x01 - dummy result code */
    0x00,        /* don't care for now... */
    0x0,         /*  */
    0x0,         /*  */
    0x00,        /* Device Service Type (byte 1) */
    0x00,        /* DM service (byte 2)*/
    0xA1,        /* data byte 1 */
    0xA2,        /* data byte 2 */
    0xA3,        /* data byte 3 */
    0xA4,        /* data byte 4 */
    0xA5         /* data byte 5 */
};

/* 
 * Name:    amdemuxctlindpacket
 *
 * Purpose: Contains a AMDEMUXCTLIND packet formatted according to the 
 *          definition in IDD_DemuxMessaging, stored in the project's 
 *          IDD documentation folder
 *
 * Members: Array of bytes as follows,
 *
 *
 * Note:    None
 *
 */
swi_uint8 amdemuxctlindpacket[] = {
    AMDEMUXCTLIND,     /* Packet type */
    0x00, 
    AMHDRVERSION,
    0x01,             /* 0x01 - dummy result code */
    0x00,       /* don't care for now... */
    0x0,        /*  */
    0x0,        /*  */
    0x0,        /* Device Service Type 2 bytes */
    0x0,        /* DM service */
    0x00,       /* operation type modem change */
    0x00,       /* device present - online */
    0x00,       /* Era number  byte 1 */
    0x00,       /* era number byte 2 */
    0x00,       /* era number byte 3 */
    0x00,       /* era number  byte 4 */  
    0x20,       /* error string 40 bytes */ 
    0x21,       /* error string 40 bytes */ 
    0x22,       /* error string 40 bytes */ 
    0x23,       /* error string 40 bytes */ 
    0x24,       /* error string 40 bytes */ 
    0x25,       /* error string 40 bytes */ 
    0x26,       /* error string 40 bytes */ 
    0x27,       /* error string 40 bytes */ 
    0x28,       /* error string 40 bytes */ 
    0x29,       /* error string 40 bytes */ 
    0x2a,       /* error string 40 bytes */ 
    0x2b,       /* error string 40 bytes */ 
    0x2c,       /* error string 40 bytes */ 
    0x2d,       /* error string 40 bytes */ 
    0x2e,       /* error string 40 bytes */ 
    0x2f,       /* error string 40 bytes */ 
    0x30,       /* error string 40 bytes */ 
    0x31,       /* error string 40 bytes */ 
    0x32,       /* error string 40 bytes */ 
    0x33,       /* error string 40 bytes */  
    0x34,       /* error string 40 bytes */ 
    0x35,       /* error string 40 bytes */ 
    0x36,       /* error string 40 bytes */ 
    0x37,       /* error string 40 bytes */ 
    0x38,       /* error string 40 bytes */ 
    0x39,       /* error string 40 bytes */ 
    0x3a,       /* error string 40 bytes */ 
    0x3b,       /* error string 40 bytes */ 
    0x3c,       /* error string 40 bytes */ 
    0x3d,       /* error string 40 bytes */ 
    0x3e,       /* error string 40 bytes */ 
    0x3f,       /* error string 40 bytes */ 
    0x40,       /* error string 40 bytes */ 
    0x41,       /* error string 40 bytes */ 
    0x42,       /* error string 40 bytes */ 
    0x43,       /* error string 40 bytes */ 
    0x44,       /* error string 40 bytes */ 
    0x45,       /* error string 40 bytes */ 
    0x46,       /* error string 40 bytes */ 
    0x47        /* error string 40 bytes */      
};

    

/**************
 *
 * Name:     main
 *
 * Purpose:  Entry point where testing of the AM package starts. 
 *           
 * Parms:    argc   - Not used
 *           argv[] - Not used
 * Return:   0 - Normal termination
 *           1 - Problem with the execution
 *
 * Notes:    The test involves parsing the packet above and printing
 *           out the contents of each of the fields for verification.
 *
 **************/
int main( int argc, char *argv[] )
{
    swi_uint32 i;
    swi_uint16 type;
    swi_uint16 hdrver;
    swi_uint16 pktlength;
    swi_uint8  result;
    swi_uint8 *amcnspktp;
    struct amcnsrrparms amcnsparms;
    /* Demux variables */
    swi_uint8 *amdemuxreqpktp     = &amdemuxrequestpacket[0];
    swi_uint16 bytesbuilt;
    swi_uint8 msgarray[AMMAXREQBKLEN];
    swi_uint8 *msgarrayp = &msgarray[0];
    enum swi_amresults amresultcode = SWI_AMSUCCESS;
    swi_uint8 devicepresent = 1;
    swi_uint32 eranumber = 1234;
    swi_uint16 deviceservice = 0; 
    swi_uint16 opcode = 1; /* stop request */
    swi_uint8 errorstring[40];
    swi_uint8 *errorstringp =  &errorstring[0];
    swi_uint8 operation;
    
    amcnspktp = &amcnsreqpacket[0];

    /* Parse the Cnsrequest packet at the top of this file */
    amparseamheader( &type, &hdrver, &result, &pktlength, &amcnspktp); 
   
    /* Print out the result */
    printf( "\n\n\nPacket Type: %d, Total length: %d\n", type, pktlength );
    printf( "	Header Version: %d, Result Code:: %d\n", hdrver, result );

    /* Continue parsing the remainder of the header */
    amparsecnsrequest( &amcnspktp, &amcnsparms );

    /* Print out each one */
    printf("Contents of the CNS Header:\n" );
    printf("===========================\n" );
    printf("CnS Object ID:       0x%x\n", amcnsparms.amcnsobjid );
    printf("CnS Operation:            %d\n", amcnsparms.amcnsoperation );
    printf("CnS Timeout:            %d\n", amcnsparms.amcnstimeout );
    printf("App'n Parameter: 0x%x\n", (unsigned int)amcnsparms.amcnsparameter );
    printf("CnS Parm lnth:           %d\n", 
           (unsigned int)amcnsparms.amcnslength );
    printf("\n\nContents of the parameter field\n" );

    /* Contents of the parameter field */
    for( i=0; i<amcnsparms.amcnslength; i++ )
    {
        /* print each byte */
        printf("%x", *amcnspktp++ );
    }

    printf("\n\n******* Switching to Next Packet Type ******************\n\n");

    /* Check the CNS Response parsing function */
    amcnspktp = &amcnsresppacket[0];

    /* Parse the Cns Response packet at the top of this file */
    amparseamheader( &type, &hdrver, &result, &pktlength, &amcnspktp); 

    /* Print out the result */
    printf( "\n\n\nPacket Type: %d, Total length: %d\n", type, pktlength );
    printf( "	Header Version: %d, Result Code:: %d\n", hdrver, result );

    /* Continue parsing the remainder of the header */
    amparsecnsresponse( &amcnspktp, &amcnsparms );

    /* Print out each one */
    printf("Contents of the CNS Header:\n" );
    printf("===========================\n" );
    printf("CnS Object ID:       0x%x\n", amcnsparms.amcnsobjid );
    printf("CnS Operation:            %d\n", amcnsparms.amcnsoperation );
    printf("App'n Parm:      0x%x\n", (int)amcnsparms.amcnsparameter );
    printf("CnS Parm lnth:           %d\n",
           (int)amcnsparms.amcnslength );
    printf("\n\nContents of the parameter field\n" );

    /* Contents of the parameter field */
    for( i=0; i<amcnsparms.amcnslength; i++ )
    {
        /* print each byte */
        printf("%x", (unsigned int)*amcnspktp++ );
    }
    
    /* AM Demux packets testing */
       for (i=0; i<AMMAXREQBKLEN; i++)
    {
        msgarray[i] = i;        
    }
    
    errorstring[0] = 0x41;
    errorstring[1] = 0x42;
    errorstring[2] = 0x43;
    errorstring[3] = 0;

    
    /* amdemux.c */
    bytesbuilt = ambuilddemuxrequest( &msgarrayp, deviceservice, opcode);
    printf("Demux Request bytes built: %d\n",bytesbuilt);
    
    
    bytesbuilt = ambuilddemuxctlind( &msgarrayp,  amresultcode, errorstringp,
        deviceservice,opcode, devicepresent, eranumber);
    printf("Demux Ctl Ind bytes built: %d\n",bytesbuilt);        
        
    bytesbuilt = ambuilddemuxdataind( &msgarrayp, deviceservice);
    printf("Demux Data Ind bytes built: %d\n",(int)bytesbuilt); 
    
    /* Testing parse routines - stepping through using gdb */
    printf("Demux Parse Request \n"); 
    amparsedemuxrequest( &amdemuxreqpktp,&deviceservice,&opcode);
    printf("Demux Parse Ctl Ind \n"); 
    amparsedemuxctlind( &amdemuxreqpktp, &deviceservice,&operation, 
        &devicepresent, &eranumber, errorstringp);
    printf("Demux Parse Data Ind \n"); 
    amparsedemuxdataind( &amdemuxreqpktp, &deviceservice);

    /* Done with the test */
    printf("\n\n\nDone AM Demux Build/Parse Testing\n" );
    
    /* Done with the test */
    printf("\n\n\nDone\n" );

    return 0;

}

/*
 * $Log: amtest.c,v $
 */
