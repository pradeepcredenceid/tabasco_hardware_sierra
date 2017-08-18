/*
 *  $Id: apdxtest.c,v 1.8 2010/06/09 23:31:02 epasheva Exp $
 *
 *  Filename: apdxtest.c
 *
 *  Purpose:  This file contains routine(s) that exercise the 
 *            Demux API package. 
 *
 * Copyright: © 2008 Sierra Wireless, Inc., all rights reserved
 *
 */

/* Linux definitions */
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* include files */
#include "apdx/apdxidefs.h"

/* global data */
swi_uint8 devicepresent = 0; /* device status storage */

/* Running in verbose mode or not */
swi_bool verbose = FALSE;

/* Name of this program */
const char *programname;

/* Buffer to build a Demux Data Indication Message */
swi_uint8 apdxtestmsgarray[APDXBUFSIZE ];

/* Selected Diagnostic device service requests for future tests */
/* Timestamp Request */
swi_uint8 timestampreq[] = {
    0x1d, 0x1c, 0x3b, 0x7e 
};

/* Version Request */
swi_uint8 versionreq[] = {
    0x0, 0x78, 0xf0, 0x7e
};

/* Extended Build ID */
swi_uint8 extbuildid[] = {
    0x7c, 0x93, 0x49, 0x7e 
};

/* Command line options, short form. Note, a ":"
 * character following an option indicates to
 * the operating system that that option takes
 * an argument(s). 
 */
const char * const short_options = "hp:vt:";

const struct option long_options[] = {
    {"help",    0, NULL, 'h'},      /* Provides terse help to users */
    {"pathname", 1, NULL, 'p'},     /* Specify the pathname */
    {"verbose", 0, NULL, 'v'},      /* Run in Verbose mode */
    {"timeout", 1, NULL, 't'},      /* Timeout to use for StopnWait calls */
    {NULL,      0, NULL, 0  }       /* End of list */
};


/**************
 *
 * Name:     apdxdatacallback
 *
 * Purpose:  Callback invoked when there are data received for specific 
 *           device service 
 *           
 * Parms:    deviceservice     - device service - diagnostic or NMEA
 *           datap             - pointer to data received 
 *           datalen           - length of data packet in number of bytes
 *           userp             - user parameter supplied during registartion
 * 
 * Return:   None.
 * 
 * Notes:    None.
 * 
 **************/
package void apdxdatacallback( swi_uint16 deviceservice, swi_uint8 *datap, 
    swi_uint32 datalen, void *userp)
{
    struct SWI_STRUCT_DemuxDataInd DemuxDataInd; /* storage for data 
                                                  * indication 
                                                  */
    struct SWI_STRUCT_DemuxDataInd *dataindp;
    swi_uint16 rcvdevsrv; /* received device service */
    swi_uint32 i;         /* index */
    swi_uint8 *localp;    /* local pointer of  data indication packet */
    
    /* Set local pointer to data indication */
    localp = datap;
     
    printf("\nDemux Data callback 0x%x\n", (unsigned int)userp);
    
    /* Initialize pointer to API Data Indication structure */   
    dataindp = &DemuxDataInd;

    /* Clear the API Data Indication structure */
    slmemset((char *)dataindp, 0, sizeof(struct SWI_STRUCT_DemuxDataInd));
    
    /* Set Application storage with data indication parameters */
    dataindp->sizeStruct     = sizeof(struct SWI_STRUCT_DemuxDataInd);
    dataindp->eDeviceService = rcvdevsrv;

    /* Extract Data */
    for (i = 0; i < datalen;  i++)
    {
        dataindp->nData[i]  = *localp++;
        printf("Data byte %d: %x\n", (int)i, (unsigned int)dataindp->nData[i]);
    }
}

/**************
 *
 * Name:     apdxctlcallback
 *
 * Purpose:  Callback invoked when Control Indication is received for specific 
 *           device service 
 *           
 * Parms:    deviceservice     - device service - diagnostic or NMEA
 *           ctlindp           - pointer to control data received 
 *           userp             - user parameter supplied during registartion
 * 
 * Return:   None.
 *           
 * Notes:    None.
 * 
 **************/
package void apdxctlcallback( swi_uint16 deviceservice, swi_uint8 *ctldatap, 
    void *userp)
{
    /* Get control data */
    devicepresent = *ctldatap;
    if (devicepresent == SWI_ONLINE)
        printf("\nDemux Ctl Ind callback: device On-Line\n");
    else
        printf("\nDemux Ctl Ind callback: device Off-Line\n");       
}

/**************
 *
 * Name:     apdxprintusage
 *
 * Purpose:  Print the usage information for this executable
 *           
 * Parms:    none
 *
 * Return:   none
 *
 * Notes:    This program can be invoked with the following options:
 *
 *           -h               Print usage information and exit cleanly
 *           -v               Verbose mode. This option should come before
 *                            any of the following for best results
 *           -p <sdkpath>     Use <sdkpath> to launch the SDK executable.
 *                            Note that the path must include the name of
 *                            the executable itself, not just the directory
 *                            where it resides. This option must be specified;
 *                            otherwise the program aborts all tests.
 *           -t <timeout>     Timeout value to use when calling Stop and Wait
 *                            APIs (i.e. Receive Demux messages). Specified in 
 *                            milliseconds. The SDK won't run properly for 
 *                            values that are too small. Recommend using 
 *                            minimum 3000 for this value
 * 
 *           If the caller does not specify a path, this program will not
 *           start executing and will abort all tests.
 *
 *
 **************/
local void apdxprintusage( void )
{
    printf("\n\nUsage: %s:\n", programname );
    printf("   -h  --help                Display this information and exit\n" );
    printf("   -v  --verbose             Display extra info while running\n");
    printf("                             NOTE: specify 'verbose' first\n");
    printf("   -t  --timeout  <timeout>  timeout (ms) for Stop and Wait APIs\n");
    printf("   -p  --pathname <sdkpath>  SDK executable is at <sdkpath>\n");
    printf("                             NOTE: always specify pathname\n");
}

/**************
 *
 * Name:     apdxgettimeout
 *
 * Purpose:  Validate the timeout value passed on the command line 
 *           
 * Parms:    timeoutp  - Pointer to the string containing the timeout
 *           timevalp  - Pointer to storage to write the converted timeout
 *                       if the ASCII value contains valid digits
 *
 * Return:   TRUE  - A valid timeout value is returned in timevalp
 *           FALSE - The string contains invalid digits
 *
 * Notes:    none
 *
 **************/
local swi_bool apdxgettimeout( char *timeoutp, unsigned int *timevalp )
{
    swi_bool retval;
    int i, strlength; 

    retval = FALSE;    /* Assume bad conversion */

    /* See how many chars to check */
    strlength = strlen( timeoutp );

    if( strlength )
    {
        for( i = 0; i<strlength; i++ )
        {
            /* One false digit and we're done */
            if( !( timeoutp[i] >= '0' && timeoutp[i] <= '9' ) )
                return FALSE;

        }

        /* If we get here, the timeoutp points to a valid ASCII number */
        *timevalp = atoi( timeoutp );
        retval = TRUE;
    }

    return retval;
}


/**************
 *
 * Name:     apdxgetpathname
 *
 * Purpose:  Fetch and validate the path to the SDK executable from 
 *           the command line argument
 *           
 * Parms:    pathstrp  - Pointer to a string containing the path to the SDK
 *           openparmp - Pointer to the API Open parameter structure. Will 
 *                       have the path information stored and enabled set to 
 *                       TRUE if the validation is successful
 * Return:   TRUE      - Pathname was valid and Enable is set to TRUE
 *           FALSE     - Pathname invalid and enable is set to FALSE
 *
 * Notes:    On return from this program, the bEnable flag in the openparmp
 *           structure is guaranteed to be either TRUE or FALSE
 *
 **************/
local swi_bool apdxgetpathname( swi_uint8 *pathnamep, 
                          struct SWI_STRUCT_ApiStartup *openparmp )
{
    swi_uint8 c;          /* character read from the string */
    swi_uint32 i;         /* FOR loop index */
    swi_uint32 strlength;
    swi_uint8 *strp;      /* Local copy of the string pointer */
    swi_bool retval;
    FILE *fp;             /* Temporary handle for the executable file */

    /* Get the length of the incoming path string */
    strlength = strlen( (char *)pathnamep );

    /* Assume the file will not be opened */
    openparmp->sSdkPath.bEnable = FALSE;
    retval = FALSE;

    /* If there is a non NULL string length check it for printable ASCII */
    if( strlength )
    {
        strp = pathnamep;
        for( i=0; i<strlength; i++ )
        {
            /* Fetch the next character */
            c = *strp++;
            if( !(c >= '.' && c <= 'z' ) )
            {
                if( verbose )
                    printf("%s, invalid path/filename\n", pathnamep );

                /* Not a valid pathname and we definitely want to return here */
                return FALSE;
            }
        }

        /* If we get here, the pathname contains printable ASCIIs */
        if ( !(fp = fopen( (char *)pathnamep, "rb" )) )
        {
            if( verbose )
                printf("No executable called %s found\n", pathnamep );
            /* Couldn't open the file */
            retval = FALSE;
        }
        /* File opened successfully, clean up and return */
        else
        {
            if( verbose )
                printf("\nFile found %s\n", pathnamep );

            /* Close the file */
            fclose( fp );

            /* Copy the pathname into the open parameters string structure */
            memcpy( openparmp->sSdkPath.szSdkPath, pathnamep, strlength+1 );
            /* Indicate the pathname is valid */
            openparmp->sSdkPath.bEnable = TRUE;
 
            retval = TRUE;
        }
    }
    else
    {
        /* NULL pathname length - invalid pathname */
        openparmp->sSdkPath.bEnable = FALSE;
        retval = FALSE;
    }

    return retval;

}

                      
/**************
 *
 * Name:     doDxReceiveThread
 *
 * Purpose:  This is the thread function started by the main program to prepare
 *           Dx Receive handling in a separate thread. 
 *           
 * Parms:    dummyp    - unused input parameter added for consistency
 *                       with the pthread prototype
 * 
 * Return:   none
 *
 * Notes:    This thread never returns.
 *           Ctrl-C  interrupts the test.
 *           Actions:
 *              register a callback function, 
 *              wait for Demux Indications
 *
 *           This thread is NOT allowed to initiate stop-and-wait requests under
 *           any circumstances
 *
 **************/
local void* doDxReceiveThread( void *dummyp )
{
    SWI_RCODE retcode;
    swi_uint16 deviceservice;
    /* Optional callback argument for data traffic from modem */
    void                          *datauserp="DataInd";

    /* Optional callback argument for status traffic from modem */
    void                          *ctluserp="CtlInd";
    
    /* Set device service to Diagnostic */
    deviceservice = SWI_DIAG;
    
        
    /* Register for Diagnostic device service */
    retcode =  SwiApiDxRegister(deviceservice, apdxdatacallback, datauserp, 
                                apdxctlcallback, ctluserp );  
    if( verbose )
    {
        switch( retcode )
        {
            case SWI_RCODE_OK:
                printf("\nDIAG: registered the following callbacks:\n");
                printf("DIAG: Data Callback: 0x%x, Data User Parameter: %s\n",
                    (unsigned int)apdxdatacallback, (char *)datauserp); 
                printf("DIAG: Ctl  Callback: 0x%x, Ctl  User Parameter: %s\n",
                    (unsigned int)apdxctlcallback, (char *)ctluserp);     
                break;

            case SWI_RCODE_FAILED:
            case SWI_RCODE_SYSTEM_ERROR:
            case SWI_RCODE_BUSY:
                printf("\n\nDIAG: registration failed %d\n", retcode );
                exit( 1 );
                break;
            
            case SWI_RCODE_API_NOT_OPEN:
                printf("\n\nDIAG: Registration failed, API not opened %d\n", 
                    retcode );
                exit( 1 );
                break;
            
            case SWI_RCODE_DEVICE_INUSE:
                printf("\n\nDIAG: Registration failed, channel in use %d\n", 
                    retcode );
                exit( 1 );
                break;
            
            default:
                printf("\n\nDIAG: Unrecognized error: %d\n", retcode );
                exit( 1 );
                break;
        }
    }
    
    while(1)
    {   
        /* Invoke receive handler for Diagnostic device service */
        retcode = SwiWaitDxIndication( deviceservice );   
        if( retcode != SWI_RCODE_OK ) 
        { 
            if ( verbose )
                printf("\nSwiWaitDxIndication returned error: %d\n", retcode );
        }
    }
    return (void*)NULL;
}



/**************
 *
 * Name:     main
 *
 * Purpose:  Entry point where testing of the Demux API package starts. 
 *           
 * Parms:    argc   - Not used
 *           argv[] - Not used
 * Return:   0 - Normal termination
 *           1 - Problem with the execution
 *
 * Notes:    see function apprintusage()
 * 
 **************/
int main( int argc, char *argv[] )
{
    int next_option;
    int optioncount = 0;
    struct SWI_STRUCT_ApiStartup      openparms;
    char *timeoutp;
    char *pathnamep = NULL;
    SWI_RCODE retcode;
    pthread_t DxReceiveThread; /* thread for receiving  indication 
                                * messages from SDK
                                */ 
    unsigned int timeout = APDXDEFAULTTIMEOUT;
    swi_uint16 deviceservice = DRDIAG; /* device service; assume Diagnostic */
    swi_uint8 *payloadp;     /* pointer to payload section of demux data 
                              * indication AM packet
                              */
    swi_uint32 payloadlen;   /* length of payload message */
    swi_uint32 bytesoffset;  /* to store the calculated bytes offset 
                              * in packet header 
                              */
    payloadp = &(apdxtestmsgarray[0]);

    /*** Set up default values ***/
    
     /* Remember our own name */
    programname = argv[0];

    /*** Set up default values ***/

    /* Default timeout */
    timeout   = APDXDEFAULTTIMEOUT;
    timeoutp  = NULL;
    pathnamep = NULL; /* path to SDK  must always be specified with command line 
                       * option '-p'
                       */

    /******************************************************
     * PARSING BLOCK - READ ALL THE COMMAND LINE SWITCHES *
     ******************************************************/

    /* Parse the command line before doing anything else */
    do 
    {
        /* Read the next option until there are no more */
        next_option = getopt_long( argc, argv, 
                                   short_options, 
                                   long_options, NULL );

        switch( next_option )
        {
            case 'h':
                /* Print usage information */
                apdxprintusage();
                exit( 0 );
                break;

            case 'p':
                /* caller specifies a pathname to the SDK executable */
                pathnamep = optarg;
                if( verbose )
                    printf("\npathname: %s\n", pathnamep );
                break;

            case 'v':
                /* Verbose mode on */
                verbose = TRUE;
                break;

            case 't':
                /* Timeout */
                timeoutp = optarg;
                break;

            case '?':
                /* Caller has used an invalid option */
                if( verbose )
                    printf("\nInvalid option\n" );

                /* Print the usage info and exit */
                apdxprintusage();
                exit(1);

                break;

            case -1:
                /* Done with options list */
                if( verbose )
                    printf("\n%d options parsed\n", optioncount );
                break;

            default:
                exit(1);
                break;
        }
        optioncount++;
    }
    while( next_option != -1 );
    
    /*************************************************************
     * SANITY CHECK. DON'T PROCEED WITHOUT CERTAIN CRITICAL INFO *
     *************************************************************/

    /* Perform some basic checks before proceeding. 
     * Can't proceed if these checks fail
     */
    if (pathnamep == NULL)
    {
        printf("\n\nError: please enter valid path for SDK executable\n\n");
        apdxprintusage();
        exit(1);        
    }
    
    /* If the pathnamep points to a valid executable */
    if( !apdxgetpathname( (swi_uint8 *)pathnamep, &openparms ) )
    {
        printf("\n\nInvalid path specified; test aborted\n");
        exit(1);
    }

    /* Validate the timeout value, if specified */
    if( timeoutp )
    {
        /* If the specified timeout is valid, convert from ASCII */
        apdxgettimeout( timeoutp, &timeout );
    }

    /*****************************************************
     * STARTUP THE SDK AND INTERPRET THE RESULT CODE     *
     *****************************************************/
    if( verbose )
    {
        printf("\nLaunching SDK at path %s\n", pathnamep );
        printf("\nA timeout of %d ms will be used\n", timeout );
    }


    /* Call the API that starts the SDK up with 1 channel */
    retcode = SwiApiDxStartup(1, &openparms);

    switch( retcode )
    {
        case SWI_RCODE_OK:
            printf("\nDemux API Startup Successful\n");
            break;
            
        case SWI_RCODE_NOT_COMPATIBLE:
        case SWI_RCODE_FAILED:
        case SWI_RCODE_SYSTEM_ERROR:
        case SWI_RCODE_BUSY:
            printf("\n\nDemux API Startup failed %d\n", retcode );
            exit( 1 );
            break;

        default:
            printf("\n\nAPDX: Unrecognized error: %d\n", retcode );
            exit( 1 );
            break;
    }
    
    /* Create a separate thread to receive DIAG demux messages */    
    pthread_create (&DxReceiveThread, NULL, &doDxReceiveThread, &timeout);
 
    if ( verbose )
        printf("\nTesting SwiApiDxBegin \n");   
    /* Start DIAG device service */    
    deviceservice = DRDIAG;    
    retcode       = SwiApiDxBegin(deviceservice);  
    printf("\nDIAG Service started\n");  
    
    /* Wait for modem detection */
    if ( verbose )
    {
        printf("\nWaiting for Air Server - SLEEPING! \n"); 
    }
    while(devicepresent == SWI_OFFLINE)
    {
        sleep(1);    
    } 
    
    if ( verbose )
        printf("\nTesting SwiApiDxSend \n");
 
    /* Prepare a data indication packet by requesting to build the appropriate
     * AM header
     */ 
     /* Set the requested payload length in bytes */
    payloadlen = APDXDIAGNSPLDLEN; 
        
    /* Obtain pointer to the payload section of data packet */    
    retcode = SwiGetDataPldOffset( &apdxtestmsgarray[0], payloadlen, &payloadp, 
        &bytesoffset);    
        
    if (retcode == SWI_RCODE_OK)
    { 
        /* Set timestamp request message in the payload section */
        /* Timestamp request */
        *payloadp++ = 0x1d;
        *payloadp++ = 0x1c;
        *payloadp++ = 0x3b;
        *payloadp++ = 0x7e;
    
        /* Adjust the length to include the header */
        payloadlen += bytesoffset; 
        
        /* Send Data Indication to SDK */
        retcode = SwiApiDxSend(deviceservice,&apdxtestmsgarray[0],payloadlen);
        if (retcode == SWI_RCODE_DEVICE_UNAVAIL)
        {
            printf("\nDevice is not loaded onto the system \n");       
        } 
    }
    else
    {
        /* Indicate that Data Indication packet cannot be built due to
         * unsufficient memory allocated by the caller
         */
        printf("\nData Indication packet cannot be build - no memory \n");        
    }
    
    if (verbose)
    {
        printf("\n\nPause for a while \n");
    }
    
    sleep( APDXTESTPAUSE);   

    if ( verbose )
        printf("\nTesting SwiApiDxEnd \n");     
    /* Stop Diagnostic device service */
    retcode =  SwiApiDxEnd(deviceservice);  
    printf("\n\nDiagnostic Service stopped\n"); 

    printf("\nPress Ctrl-C to quit\n");     
    /* Wait for thread, don't leave yet - need ctrl-C to quit */
    pthread_join (DxReceiveThread, NULL); 

    /* Normal exit */
    exit( 0 );
}

/*
 * $Log: apdxtest.c,v $
 */
