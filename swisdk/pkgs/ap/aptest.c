/*
 *  $Id: aptest.c,v 1.42 2010/06/10 00:26:12 epasheva Exp $
 *
 *  Filename: aptest.c
 *
 *  Purpose:  This file contains routine(s) that exercise the 
 *            API package. 
 *
 * Copyright: © 2008 Sierra Wireless, Inc., all rights reserved
 *
 */

/* Linux definitions */
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

/* include files */
#include "SwiDataTypes.h"
#include "sl/sludefs.h"

#include "SwiRcodes.h"
#include "SwiApiGsmBasic.h"
#include "SwiApiGsmNetwork.h"
#include "SwiApiGsmSim.h"
#include "SwiApiGsmSs.h"
#include "SwiApiCmBasic.h"
#include "SwiApiCmUpgrade.h"
#include "SwiApiCmLbs.h"
#include "SwiApiGsmLbs.h"
#include "SwiApiGsmVoice.h"

#include "SwiApiCdmaBasic.h"
#include "SwiIntCdma.h"
#include "SwiApiCdmaIota.h"
#include "SwiApiCdmaLbs.h"
#include "SwiApiCdmaNetwork.h"
#include "SwiApiCdmaRuim.h"
#include "SwiApiCdmaSms.h"
#include "SwiApiCdmaVoice.h"
#include "SwiApiCmDm.h"

#include "SwiApiNotify.h"

#include "aptestdefs.h"


/* global data */

/* Running in verbose mode or not */
swi_bool verbose = FALSE;

/* Name of this program */
const char *programname;

/* Command line options, short form. Note, a ":"
 * character following an option indicates to
 * the operating system that that option takes
 * an argument(s). 
 */
const char * const short_options = "hp:vt:mn:rf:";

const struct option long_options[] = {
    {"help",    0, NULL, 'h'},      /* Provides terse help to users */
    {"pathname", 1, NULL, 'p'},     /* Specify the pathname */
    {"verbose", 0, NULL, 'v'},      /* Run in Verbose mode */
    {"timeout", 1, NULL, 't'},      /* Timeout to use for StopnWait calls */
    {"monitor", 0, NULL, 'm'},      /* Enable not'ns and monitor them */
    {"numtest", 1, NULL, 'n'},      /* Test number or list of comma separated 
                                     * test numbers to run
                                     */
    {"repeat",  1, NULL, 'r'},      /* Repeat test until ^C */
    {"file",    1, NULL, 'f'},      /* file name with list of tests to run */
    {NULL,      0, NULL, 0  }       /* End of list */
};

/**************
 *
 * Name:     apprintusage
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
 *                            where it resides. This option must always be 
 *                            present; otherwise the application does not start.
 *           -t <timeout>     Timeout value to use when calling Stop and Wait
 *                            APIs (i.e. Get or Set routines). Specified in 
 *                            milliseconds. The SDK won't run properly for 
 *                            values that are too small. Recommend using 
 *                            minimum 3000 for this value
 *           -m               Enable notifications and remain active, 
 *                            monitoring incoming notifications from the modem
 *                            and displaying them to the console. Omitting this
 *                            option causes this program to terminate as soon as
 *                            the specified test case(s) has completed
 *           -n <x or test #> x or X displays cross-reference of test tags.
 *                            test # can be a single test tag, e.g., t10, or 
 *                            a list of comma-seaprated test tags, e.g.,
 *                            t1,t10,t6
 *           -r --repeat      Repeat the specified test(s) until the user 
 *                            terminates with ^C
 *           -f <filename>    file name with list of test tags as in -n above.
 * 
 *           If the caller does not specify a path, this program will return 
 *           an error and will not start any execution.
 *
 *
 **************/
local void apprintusage( void )
{
    printf("\n\nUsage: %s:\n", programname );
    printf("   -h  --help                Display this information and exit\n" );
    printf("   -v  --verbose             Display extra info while running\n");
    printf("                             NOTE: specify 'verbose' first\n");
    printf("   -t  --timeout  <timeout>  timeout (ms) for Stop and Wait APIs\n");
    printf("   -p  --pathname <sdkpath>  SDK executable is at <sdkpath>\n");
    printf("                             NOTE: always specify pathname\n");
    printf("   -m  --monitor             Enable notifications and continue\n");
    printf("                             executing, displaying notification\n");
    printf("                             messages until stopped with ^C\n");
    printf("   -n  --numtest  <test #>   Test number or list of\n");
    printf("                             comma-separated test numbers to run\n");
    printf("                  <x>        List cross reference of test tag \n");
    printf("                             to API functions name\n");
    printf("   -r  --repeat              Repeat the test(s) until stopped\n" );
    printf("                             with ^C\n");
    printf("   -f  --file <filename>     file name with list of tests to run\n\n\n"); 
}

/**************
 *
 * Name:     apgettimeout
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
local swi_bool apgettimeout( char *timeoutp, unsigned int *timevalp )
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
 * Name:     apgetpathname
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
local swi_bool apgetpathname( swi_uint8 *pathnamep, 
                          struct SWI_STRUCT_ApiStartup *openparmp )
{
    swi_uint8 c;          /* character read from the string */
    swi_uint32 i;         /* FOR loop index */
    swi_uint32 strlength;
    swi_uint8 *strp;      /* Local copy of the string pointer */
    swi_bool retval;
    FILE *fp;             /* Temporary handle for the executable file */

    /* Get the length of the incoming path string */
    strlength = strlen( (const char *)pathnamep );

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
        if ( !(fp = fopen( (const char *)pathnamep, "rb" )) )
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
 * Name:     main
 *
 * Purpose:  Entry point where testing of the API package starts. 
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
    unsigned int timeout;
    char *timeoutp;
    char *pathnamep=NULL;
    char *testsp=NULL;         /* pointer to test number or list of... */
    char *testsfilenamep=NULL; /* pointer to file containing list of tests */
    SWI_RCODE retcode;
    struct SWI_STRUCT_ApiStartup      openparms;
    struct SWI_STRUCT_AirServerList   airservers;
    swi_bool monitornotify = FALSE; /* Assume caller doesn't request not'ns */
    swi_bool boothold;              /* Modem's boot and hold state */
    swi_bool repeatest;             /* Repeat test flag */
    swi_uint32 testslength;         /* Length (bytes) of "-n" command line */
    pthread_t NotifThread;
    char intestlist[APMAXTESTS];    /* Copy of the requested tests */
    char testlist[APMAXTESTS];      /* List of tests passed to the handler */
    swi_charp   sdkversionp;
    
    /* Remember our own name */
    programname = argv[0];

    /* The length of the test string specified using the "-n" option
     * must be zero unless someone uses this option. The zero length
     * ensures there will be no stack dump in the call to slstrncpy()
     * below where the repetition option is being handled
     */
    testslength = 0;

    /*** Set up default values ***/

    /* Default timeout */
    timeout = DEFAULTTIMEOUT;
    timeoutp = NULL;
    pathnamep = NULL; /* one must always specify path name for SDK executable */
    repeatest = FALSE;

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
                apprintusage();
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
   
            case 'm': 
                /* Enable notifications and remain active until
                 * the tester hits Ctrl C to end the test
                 */
                monitornotify = TRUE;
                printf("\n\nNOTE: Terminate execution with Ctrl-C\n");
                break;
  
            case 'n': 
                /* caller specifies test(s) on the command line */
                testsp = optarg;

                /* Pre-fill the copy array with NULLs */
                slmemset( intestlist, 0x0, APMAXTESTS );
                slmemset( testlist, 0x0, APMAXTESTS );

                /* Make a copy of the requested command line 
                 * tests in case the "-r" option is also 
                 * specified
                 */
                testslength = slstrlen( testsp );

                /* Only copy if there is enough space in the destination */
                if( testslength < APMAXTESTS )
                {
                    slstrncpy( intestlist, testsp, testslength );
                }
                else
                {
                    printf("Test list too long, %d max.\n", APMAXTESTS );
                    exit(1);
                }

                if( verbose )
                    printf("\nTest cases: %s\n", testsp );
                break;

           case 'r': 
                /* Repeat the test until stopped with ^C */
                repeatest = TRUE;
                if( verbose )
                    printf("\nRepetition mode enabled\n");
                break;

           case 'f': 
                /* caller specifies a tests filename to the SDK executable */
                testsfilenamep = optarg;
                if( verbose )
                    printf("\ntests filename: %s\n", testsfilenamep );
                break;

            case '?':
                /* Caller has used an invalid option */
                if( verbose )
                    printf("\nInvalid option\n" );

                /* Print the usage info and exit */
                apprintusage();
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
        apprintusage();
        exit(1);        
    }
    else if (testsp && testsfilenamep)
    {
        printf("\n\nCannot combine options -n and -f\n");
        exit(1);
    }
    else if ((testsp == NULL) && (testsfilenamep == NULL))
    {
        printf("\n\nError: please enter at least 1 test case\n\n");
        apprintusage();
        exit(1);
    }

    /* If the pathnamep points to an invalid executable */
    if( !apgetpathname( (swi_uint8 *)pathnamep, &openparms ) )
    {
        printf("\n\nInvalid path specified; test aborted\n");
        exit(1);
    }

    /* Validate the timeout value, if specified */
    if( timeoutp )
    {
        /* If the specified timeout is valid, convert from ASCII */
        apgettimeout( timeoutp, &timeout );
    }

    /*****************************************************
     * STARTUP THE SDK AND INTERPRET THE RESULT CODE     *
     *****************************************************/
    if( verbose )
    {
        printf("\nLaunching SDK at path %s\n", openparms.sSdkPath.szSdkPath );
        printf("\nA timeout of %d ms will be used\n", timeout );
    }

    /* Call the API that starts the SDK up */
    retcode = SwiApiStartup( &openparms );

    switch( retcode )
    {
        case SWI_RCODE_OK:
            printf("\nAPI Opened Successfully\n");
            break;

        case SWI_RCODE_FAILED:
        case SWI_RCODE_SYSTEM_ERROR:
        case SWI_RCODE_BUSY:
            printf("\n\nAPI Open failed %d\n", retcode );
            exit( 1 );
            break;

        default:
            printf("\n\nUnrecognized error: %d\n", retcode );
            exit( 1 );
            break;
    }
   
    /* Print the SDK version used, SDK must be tagged for correct information */
    retcode = SwiGetSdkVersion (&sdkversionp);
    printf ("\nUsing SDK version %s\n", sdkversionp);
    
    /********************************************************************
     * MANAGE THE APP. DON'T SEND APIs UNLESS THE AIRSERVER IS SELECTED *
     ********************************************************************/
   
    /* Obtain a list of the available Air Servers and pick one */
    /* NOTE - This is not required for the Linux v2 SDK as
     * only 1 airserver is supported at this time. The SDK 
     * will attach to the first one it finds whose USB 
     * Vendor ID matches the Sierra Wireless ID
     */
    while(SwiGetAvailAirServers(&airservers) != SWI_RCODE_OK)
    {
        printf("\nAir Server not available - SLEEPING! \n");
        sleep(10);;
    }

    /* Now that there is a modem present, fetch its boot and hold state */
    if( SwiGetBootAndHoldMode( &boothold, timeout ) != SWI_RCODE_OK )
    {
        /* Should never happen */
        printf("\nUnexpected device unavailable condition\n");
        exit(1);
    }

    /* Boot and hold operating state is useful to know sometimes */
    if( verbose )
    {
        /* Report the boot and hold operating state */
        printf("\nModem's boot and hold state is %d\n\n", boothold );
    }


    /************************************************************************
     * IMPLEMENT THE CALLER'S REQUESTED RUNNING MODE, MONITOR OR API-&-EXIT *
     ************************************************************************/
    
    /* Start the notification thread. !!!Must be done first!!!
     * The check for x is if a cross-reference for the test cases is
     * required. If so, no tests are run, i.e., don't do thread stuff.
     */

    if ( testsp == NULL || *testsp != 'x')
    {
        pthread_create(&NotifThread, NULL, (void *)&doNotificationThread, &timeout);
        apaddcbkfp((void *)apnotifcallback);
    }
    /* Obtain a list of the available Air Servers and pick one */
    /* If the caller has requested notifications, 
     * then we need to first enable them. Then
     * to terminate this program the user must 
     * enter ^C
     */
    if( monitornotify )
    { 
        /* Enable notifications */
        doSwiNotify(timeout);

        /* If the "-r" switch was used, then this do loop will be
         * repeated until ^C is used to stop execution. Note, this
         * loop is identical to the one in the "else" clause, below
         */
        do
        {
            if( testslength )
            {
                /* Put a fresh copy of the intests into the working list */
                slmemcpy( testlist, intestlist, testslength );
            }

            /* Run the requested APIs then exit */
            apProcessTests (testlist, testsfilenamep, timeout);
        }
        while( repeatest );

        /* wait for thread, don't leave yet - need ctrl-C to quit */
        if ( *testsp != 'x')
            pthread_join (NotifThread, NULL);
    }
    else
    {
        /* If the "-r" switch was used, then this do loop will be
         * repeated until ^C is used to stop execution. Note, this
         * loop is identical to the one above for handling the "-m" 
         * switch.
         */
        do
        {
            if( testslength )
            {
                /* Put a fresh copy of the intests into the working list */
                slmemcpy( testlist, intestlist, testslength );
            }

            /* Run the requested APIs then exit */
            apProcessTests (testlist, testsfilenamep, timeout);
        }
        while( repeatest );

        printf("\nTest Completed ----------------------------------\n");
    }

    /* Normal exit */
    exit( 0 );
}

/*
 * $Log: aptest.c,v $
 */
