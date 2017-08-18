/*
 *  $Id: ramain.c,v 1.4 2009/04/30 23:08:43 blee Exp $
 *
 *  Filename: ramain.c
 *
 *  Purpose:  This program is a small utility that acts as a relay agent to
 *            the diagnostic, NMEA, and AT services.
 *
 * Copyright: © 2009 Sierra Wireless, Inc., all rights reserved
 *
 */

/* Linux definitions */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <getopt.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

/* include files */
#include "SwiDataTypes.h"
#include "SwiRcodes.h"
#include "SwiApiGsmBasic.h"

#include "SwiApiNotify.h"
#include "SwiApiDemux.h"
#include "ra.h"


/* Running in verbose mode or not */
swi_bool verbose = FALSE;

/* Name of this program */
const char *programname;

/* Command line options, short form. Note, a ":" character following an option
 * indicates to the operating system that that option takes an argument(s).
 * a "::" means for that option, an optional value may be entered, eg, 
 * -k or -k1234 (note that for some reason/bug, there is no space separating 
 * the option and the value.
 */
const char * const short_options = "hp:vt:q:s::j:u::k:w::x:y:z:";

const struct option long_options[] = {
    {"help",    0, NULL, 'h'},      /* Provides terse help to users */
    {"pathname",1, NULL, 'p'},      /* Specify the pathname */
    {"verbose", 0, NULL, 'v'},      /* Run in Verbose mode */
    {"timeout", 1, NULL, 't'},      /* Timeout to use for StopnWait calls */
    
    {"qhost",    1, NULL, 's'},      /* DIAG host name for re-route */
    {"sport",    optional_argument, NULL, 's'},   /* DIAG socket for re-route */
    {"jhost",    1, NULL, 'j'},      /* NMEA host name for re-route */
    {"uport",    optional_argument, NULL, 'u'},   /* NMEA socket for re-route */
    {"khost",    1, NULL, 'k'},      /* AT host name for re-route */
    {"wport",    optional_argument, NULL, 'w'},   /* AT socket for re-route */

    {"xcom",     1, NULL, 'x'},   /* Linux serial port outgoing to DIAG */
    {"ycom",     1, NULL, 'y'},   /* Linux's serial port outgoing to NMEA */
    {"zcom",     1, NULL, 'z'},   /* Linux's serial port outgoing to AT */
    {NULL,      0, NULL, 0  }     /* End of list */
};

/* Our threads */
pthread_t   RaThreads[RA_MAX_THREADS];


/**************
 *
 * Name:     raprintusage
 *
 * Purpose:  Print the usage information for this executable
 *
 * Parms:    none
 *
 * Return:   none
 *
 * Notes:    Rather than duplicating the info here, please see code below.
 *
 *           If the caller does not specify a path, this program will not
 *           start executing and will abort all tests.
 *
 *
 **************/
local void raprintusage( void )
{
    printf("\n\nUsage: %s:\n", programname );
    printf("   -h  --help                Display this information and exit\n" );
    printf("   -v  --verbose             Display extra info while running\n");
    printf("                             NOTE: specify 'verbose' first\n");
    printf("   -t  --timeout  <timeout>  timeout (ms) for Stop and Wait APIs\n");
    printf("   -p  --pathname <sdkpath>  SDK executable is at <sdkpath>\n");
    printf("                             NOTE: always specify pathname\n");
    
    printf("   -q  --qhost               Host name of remote diagnostic\n" );
    printf("                             server PC\n" );
    printf("                             if Host name is given, we are \n" );
    printf("                             client, host port is required\n" );
    printf("                             if Host name is NOT given, we are \n" );
    printf("                             server and our port defaults to \n" );
    printf("                             33600 unless specified differently\n" );
    printf("   -s  --sport               Port number of remote diagnostic\n" );
    printf("                             server PC\n" );
    printf("                             NOTE: defaults to 33600\n");
    printf("                                   no space if specified, e.g.:\n");
    printf("                                   -k12345\n");
    printf("   -j  --jhost               Host name of remote NMEA server PC\n" );
    printf("                             see -q description \n");
    printf("   -u  --uport               Port # of remote NMEA server PC\n");
    printf("                             NOTE: defaults to 33601\n");
    printf("                                   no space if specified, e.g.:\n");
    printf("                                   -k12345\n");
    printf("   -k  --khost               Host name of remote AT server PC\n" );
    printf("                             see -q description \n");
    printf("   -w  --wport               Port number of remote AT server PC\n" );
    printf("                             NOTE: defaults to 33602\n");
    printf("                                   no space if specified, e.g.:\n");
    printf("                                   -k12345\n");
    printf("   -x  --xcom <port name>    serial port name to route the\n" );
    printf("                             diagnostic output to the external \n");
    printf("                             host\n");
    printf("   -y  --ycom <port name>    serial port name to route the\n" );
    printf("                             NMEA output to the external \n");
    printf("                             host\n");
    printf("   -z  --zcom <port name>    serial port name to route the\n" );
    printf("                             AT output to the external \n");
    printf("                             host\n");

}

/**************
 *
 * Name:     ragettimeout
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
local swi_bool ragettimeout( char *timeoutp, unsigned int *timevalp )
{
    /* Return value */
    swi_bool retval;

    /* Index variable */
    int i;

    /* Length of string */
    int strlength;

    /************************
     * End of declarations
     ************************/

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
 * Name:     ragetpathname
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
 *           Filenames can only consists of printable ASCII characters
 *
 **************/
local swi_bool ragetpathname( swi_uint8 *pathnamep,
                          struct SWI_STRUCT_ApiStartup *openparmp )
{
    /* Local copy of the string pointer */
    swi_uint8 *strp;

    /* character read from the string */
    swi_uint8 c;

    /* FOR loop index */
    swi_uint32 i;

    /* Length of string containing the pathname */
    swi_uint32 strlength;

    /* Return value */
    swi_bool retval;

    /* Temporary handle for the executable file */
    FILE *fp;

    /************************
     * End of declarations
     ************************/

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
 * Name:     sigproc
 *
 * Purpose:  Handler for when CTRL-C is entered
 *
 * Parms:    none
 *
 * Return:   exits the program
 *
 * Notes:    Cleanup. Do not hit more than once. Otherwise it  will try to
 *           call APIs once for each ctrl-c
 *
 **************/
void sigproc()
{
    /* NOTE some versions of UNIX will reset signal to default
    after each call. So for portability reset signal each time */
    int i;

    printf("You have pressed ctrl-c, cleaning up...\n");

    /* Clean up - Close API channels, serial port descriptors and sockets */
    for (i=RA_SVC_MIN; i<RA_SVC_MAX; i++)
    {
        raservicestop ( i );
        raservicecleanup ( i );
    }

    /* Cancel threads */
    for (i=RA_MIN_THREADS; i < RA_MAX_THREADS; i++)
    {
        if (RaThreads[i])
            pthread_cancel (RaThreads[i]);
    }

    fflush (NULL);
    exit (0);
}

/*
 * Name:     StartServiceThreads
 *
 * Purpose:  Start all required threads for the given service
 *
 * Parms:    rasvcp - pointer to services information.
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void StartServiceThreads(struct raservice *rasvcp)
{
    if (rasvcp->rasvcon)
    {
        /* A separate thread manages incoming SERVICE messages from modem
         * Note: This must be created before any SwiApiDx calls that expect the
         * modem to send a message,  eg, Airserver. Otherwise, we may miss the
         * message(s).
         */
        /* Register our callbacks */
        if (rasvcp->rasvctype == RA_DIAG)  /* no AT service for these yet */
        {
            doRaRegisterService (   rasvcp->rasvctype, 
                                    radatacallback, 
                                    ractlcallback );

            pthread_create (&RaThreads[ rasvcp->rarcvthrd ],
                NULL,
                &doRaReceiveThread,
                &rasvcp->rasvctype);
                
            /* Enable service channel */
            raservicestart(rasvcp->rasvctype);
        }

        /* Do this anyways, won't be set if null */
        racommpairinit (rasvcp->rasvctype, rasvcp->raspmdmp, rasvcp->rasphostp);
        
        /* Only one of serial port, TCP server or TCP client should be running for
         * each Service at a time. Start up threads to handle messages going both ways.
         */
        if (rasvcp->raspmdmp) /* using direct serial SERVICE interface */
        {
             pthread_create (&RaThreads[ rasvcp->racomportm2hthrd ],
                            NULL,
                            &doRaComPortModem2HostThread,
                            (void *)rasvcp);
        }

        if (rasvcp->rasphostp) 
        {
            /* Assuming we're not using the modem serial port to use SERVICE 
             * ie, coming over hip to go to serial host.
             */
            pthread_create (&RaThreads[ rasvcp->racomporth2mthrd ],
                            NULL,
                            &doRaComPortHost2ModemThread,
                            (void*)rasvcp);
        }
        else if (rasvcp->ratcphostnamep)
        {
            rasettcphost (  rasvcp->rasvctype, 
                            rasvcp->ratcphostnamep, 
                            rasvcp->rasocketp);
            /* TCP has 2 modes - server or client.
            * Server: We can run on default port of 3360x or user can provide one.
            *         Then we need ethernet-2-serial converter working with one end
            *         of the virtual comm port pair on the Service's Host machine.
            *         The other end of the virtual comm port connects to the
            *         Service's application program.
            * Client: We need the host name and port number on which the
            *         Service's program is set up.
            */
            /* Client mode */
            pthread_create (&RaThreads[ rasvcp->ratcpclnth2mthrd ],
                            NULL,
                            &doRaTCPClientModeHost2ModemThread,
                            (void *)rasvcp);
        }
        else if (rasvcp->braSocket)
        {
            /* Server mode */
            pthread_create (&RaThreads[ rasvcp-> ratcpsvrh2mthrd ],
                            NULL,
                            &doRaTCPServerModeHost2ModemThread,
                            (void *)rasvcp);
        }
    }
}

/**************
 *
 * Name:     main
 *
 * Purpose:  Entry point for the Relay Agent pass through utility
 *
 * Parms:    argc   - Count of incoming command line arguments
 *           argv[] - list of pointers to characters for each command line arg
 * Return:   0 - Normal termination
 *           1 - Problem with the execution
 *
 * Notes:    see function apprintusage()
 *
 **************/
int main( int argc, char *argv[] )
{
    struct SWI_STRUCT_ApiStartup  openparms; /* Structure for starting API    */
    SWI_RCODE    retcode;            /* Return code from API calls            */
    unsigned int timeout;            /* The actual timeout for API calls      */
    int          next_option;        /* For parsing command line switches     */
    int          optioncount  = 0;   /* # of command line options detected    */
    char         *timeoutp;          /* Pointer to the command line timeou    */
    char         *pathnamep   = NULL;/* Pathname to SDK executable image      */
    
    /* Pointer to DIAG control block         */ 
    struct raservice      *radiagp = ragetservicecbp(RA_DIAG);
    /* Pointer to NMEA control block         */
    struct raservice      *ranmeap = ragetservicecbp(RA_NMEA); 
    /* Pointer to AT control block           */
    struct raservice      *raatp   = ragetservicecbp(RA_AT);   
    
    int i; 

    /* Remember our own name */
    programname = argv[0];

    /*** Set up default values ***/

    /* Default timeout */
    timeout   = DIAGDEFAULTTIMEOUT;
    timeoutp  = NULL;

    /* path to SDK  must always be specified
     * with command line option '-p'
     */
    pathnamep = NULL;

    /* Initialize main program variables */
    rainit();
    /* Initialize service control blocks */
    for (i=RA_SVC_MIN; i < RA_SVC_MAX; i++)
    {
        raserviceinit( i );
    }

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
                raprintusage();
                exit( 0 );
                break;

            case 'p':
                /* caller specifies a pathname to the SDK executable */
                pathnamep = optarg;
                if( verbose )
                    printf("\nSDK executable pathname: %s\n", pathnamep );
                break;

            case 'v':
                /* Verbose mode on */
                verbose = TRUE;
                break;

            case 't':
                /* Timeout */
                timeoutp = optarg;
                break;
                                
           case 'q':
                /* Diagnostic host name */
                radiagp->ratcphostnamep = optarg;
                /* Turn Diagnostic service ON */
                radiagp->rasvcon = TRUE;
                break;

            case 's':
                /* Diagnostic socket */
                radiagp->braSocket= TRUE;
                if (optarg)
                {
                    radiagp->rasocketp = optarg;
                }
                /* Turn Diagnostic service ON */
                radiagp->rasvcon = TRUE;
                break;

            case 'x':
                /* Diagnostic Host Serial port */
                radiagp->rasphostp = optarg;
                /* Turn Diagnostic service ON */
                radiagp->rasvcon = TRUE;
                break;

            case 'j':
                /* NMEA host name */
                ranmeap->ratcphostnamep = optarg;
                /* Turn NMEA service ON */
                ranmeap->rasvcon = TRUE;
                break;

            case 'u':
                /* NMEA socket */
                ranmeap->braSocket = TRUE;
                if (optarg)
                {
                    ranmeap->rasocketp = optarg;
                }
                /* Turn NMEA service ON */
                ranmeap->rasvcon = TRUE;
                break;

            case 'y':
                /* NMEA Host Serial port */
                ranmeap->rasphostp = optarg;
                /* Turn NMEA service ON */
                ranmeap->rasvcon = TRUE;
                break;

             case 'k':
                /*AT host name */
                raatp->ratcphostnamep = optarg;
                /* Turn AT service ON */
                raatp->rasvcon = TRUE;
                break;

            case 'w':
                /* AT socket */
                raatp->braSocket = TRUE;
                if (optarg)
                {
                    raatp->rasocketp = optarg;
                }
                /* Turn AT service ON */
                raatp->rasvcon = TRUE;
                break;
                
            case 'z':
                /* AT Serial port to external host - */
                raatp->rasphostp = optarg;
                /* Turn AT service ON */
                raatp->rasvcon = TRUE;
                break;

            case '?':
                /* Caller has used an invalid option */
                if( verbose )
                    printf("\nInvalid option\n" );

                /* Print the usage info and exit */
                raprintusage();
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
        raprintusage();
        exit(1);
    }

    /* If the pathnamep points to a valid executable */
    if( !ragetpathname( (unsigned char *)pathnamep, &openparms ) )
    {
        printf("\n\nInvalid path for SDK executable specified; test terminated\n");
        exit(1);
    }

    /* Validate the timeout value, if specified */
    if( timeoutp )
    {
        /* If the specified timeout is valid, convert from ASCII */
        ragettimeout( timeoutp, &timeout );
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

    if( verbose )
    {
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
                printf("\n\nDemux: Unrecognized error: %d\n", retcode );
                exit( 1 );
                break;
        }
    }

    /* Handle CTRL-C */
    signal(SIGINT, sigproc);
    
    /* Initialize threads */
    for (i=RA_MIN_THREADS; i < RA_MAX_THREADS; i++)
    {
        RaThreads[i] = (pthread_t )NULL;
    }

    /* Start threads for each Service */
    StartServiceThreads(radiagp);
    
    StartServiceThreads(ranmeap);
    
    StartServiceThreads(raatp);

    /***************************************************************************
     * Wait for threads, i.e., don't exit until CTRL-C
     */

    /* Join threads, ie, main program never exits */
    for (i=RA_MIN_THREADS; i < RA_MAX_THREADS; i++)
    {
        if (RaThreads[i])
            pthread_join(RaThreads[i], NULL);
    }

    /* This program never exits - CTRL-C to exit */
    return 0;
}

/*
 * $Log: ramain.c,v $
 * Revision 1.4  2009/04/30 23:08:43  blee
 * BUG #15847
 * Relay Agent direct interface using serial not working for DIAG service.
 * Re-vamped command-line options. Combine Service thread start into
 * one common function.
 *
 * Revision 1.3  2009/04/21 18:39:12  rfiler
 * BZ 1547, added code around:
 *  pthread_create (&RaThreads[ RADIAGCOMPORTMODEM2HOSTTHREAD ]
 *
 * Revision 1.2  2009/04/14 18:22:52  blee
 * BUG #15716
 * Added missing options descriptions to print usage function.
 *
 * Revision 1.1  2009/03/30 21:59:51  blee
 * BUG # 15436
 * Initial check-in of Relay Agent for Diagnostics, NMEA and AT services
 *
 *
 */
