/*
 *
 *  $Id: apuitest.c,v 1.3 2010/06/09 23:32:14 epasheva Exp $
 * 
 *  Filename: apuitest.c
 *
 *  Purpose:  This file contains routine(s) that perform basic UI with the SDK,
 *            e.g., turning logging on or off. 
 *
 * Copyright: © 2009 Sierra Wireless, Inc., all rights reserved
 *
 */

/* Linux definitions */
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* include files */
#include "apui/apuiidefs.h"
#include "SwiApiUi.h"

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
const char * const short_options = "hp:vt:";

const struct option long_options[] = {
    {"help",    0, NULL, 'h'},      /* Provides terse help to users */
    {"pathname", 1, NULL, 'p'},     /* Specify the pathname */
    {"verbose", 0, NULL, 'v'},      /* Run in Verbose mode */
    {"timeout", 1, NULL, 't'},      /* Timeout to use for StopnWait calls */
    {NULL,      0, NULL, 0  }       /* End of list */
};


/* For safe condition variable usage, must use a boolean predicate and   */
/* a mutex with the condition.                                           */
int                 conditionMet = 0;
pthread_cond_t      cond  = PTHREAD_COND_INITIALIZER;
pthread_mutex_t     mutex = PTHREAD_MUTEX_INITIALIZER;

/**************
 *
 * Name:     apuiprintusage
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
local void apuiprintusage( void )
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
 * Name:     apuigettimeout
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
local swi_bool apuigettimeout( char *timeoutp, unsigned int *timevalp )
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
 * Name:     apuigetpathname
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
local swi_bool apuigetpathname( swi_uint8 *pathnamep, 
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
 * Name:     doUiReceiveThread
 *
 * Purpose:  This is the thread function started by the main program to prepare
 *           UI Receive handling in a separate thread. 
 *           
 * Parms:    dummyp    - unused input parameter added for consistency
 *                       with the pthread prototype
 * 
 * Return:   none
 *
 * Notes:    This thread never returns.
 *           Ctrl-C  interrupts the test.
 *           Actions: Receives a message from the SDK side and prints it, 
 *                    assuming data is printable.
 *
 *           This thread is NOT allowed to initiate stop-and-wait requests under
 *           any circumstances
 *
 **************/
local void* doUiReceiveThread( void *dummyp )
{
    SWI_RCODE retcode;
    swi_uint8 *resp;
    int rc;
    swi_uint32 timeout = (swi_uint32) dummyp;
    
    while(1)
    {   
        /* Invoke receive handler for UI service */
        retcode = SwiApiUiReceive( &resp, timeout );   
        if( retcode != SWI_RCODE_OK ) 
        { 
            if ( verbose )
                printf("\nSwiApiUiReceive returned error: %d\n", retcode );
        }
        else
        {
            /* Print our stuff and then give the other thread a chance */
            rc = pthread_mutex_lock(&mutex);
            if (slstrncmp ((char *)resp, "OK", 2) != 0)
                printf("<< %s\n", resp );
            else
                conditionMet = 1;
            if (conditionMet)
                rc = pthread_cond_signal(&cond);
            rc = pthread_mutex_unlock(&mutex);
            
        }
    }
    return (void*)NULL;
}


/**************
 *
 * Name:     Run_App
 *
 * Purpose:  This function runs the main part of the sample code application
 *           
 * Parms:    timeout - API call timeout
 *
 * Return:   never returns - 'q' or CTRL-C the program to exit
 *
 * Notes:    None
 * 
 **************/
swi_bool Run_App(unsigned int timeout)
{
    SWI_RCODE retcode;
    char inputp[LINE_MAX];
    int rc;
    
    /* loop until quit or CTRL-C */
    /* Do not use scanf() - endless problems with hidden \r ! */
    printf (">> ");
    while(1)
    {
        /* wait for user to type in command */
        slmemset (inputp, 0, sizeof (inputp));
        fgets (inputp, LINE_MAX, stdin);

        if ( (slstrncmp (inputp, "QUIT", 4) == 0) ||
             (slstrncmp (inputp, "quit", 4) == 0) )
            return FALSE;

        /* Throw away LF and send the command to the SDK */
        inputp[strlen (inputp)-1] = '\0';
        
        /* This is a non-blocking call */
        retcode = SwiApiUiSend((swi_uint8 *)inputp, strlen (inputp)) ;
        
        /* Wait for our chance to print and prompt */
        rc = pthread_mutex_lock(&mutex);
        if (conditionMet == 0)
        {
           rc = pthread_cond_wait(&cond, &mutex);
        }

        printf (">> ");
        
        conditionMet = 0;
        rc = pthread_mutex_unlock(&mutex);
    }
    return TRUE;
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
    pthread_t UiReceiveThread; /* thread for receiving  indication 
                                * messages from SDK
                                */ 
    unsigned int timeout;
 
     /*** Set up default values ***/
    
     /* Remember our own name */
    programname = argv[0];

    /*** Set up default values ***/

    /* Default timeout */
    timeout   = 0;    /* Set timeout to 0, ie wait forever or until the SDK 
                       * has a message for us.
                       */
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
                apuiprintusage();
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
                apuiprintusage();
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
        apuiprintusage();
        exit(1);        
    }
    
    /* If the pathnamep points to a valid executable */
    if( !apuigetpathname( (swi_uint8 *)pathnamep, &openparms ) )
    {
        printf("\n\nInvalid path specified; test aborted\n");
        exit(1);
    }

    /* Validate the timeout value, if specified */
    if( timeoutp )
    {
        /* If the specified timeout is valid, convert from ASCII */
        apuigettimeout( timeoutp, &timeout );
    }

    /*****************************************************
     * STARTUP THE SDK AND INTERPRET THE RESULT CODE     *
     *****************************************************/
    if( verbose )
    {
        printf("\nLaunching SDK at path %s\n", pathnamep );
    }

    /* Call the API that starts the SDK up */
    retcode = SwiApiUiStartup(&openparms);

    switch( retcode )
    {
        case SWI_RCODE_OK:
            printf("\nUI API Startup Successful\n");
            break;
            
        case SWI_RCODE_NOT_COMPATIBLE:
        case SWI_RCODE_FAILED:
        case SWI_RCODE_SYSTEM_ERROR:
        case SWI_RCODE_BUSY:
            printf("\n\nUI API Startup failed %d\n", retcode );
            exit( 1 );
            break;

        default:
            printf("\n\nAPUI: Unrecognized error: %d\n", retcode );
            exit( 1 );
            break;
    }
 
    /* Create a separate thread to receive UI messages */    
    pthread_create (&UiReceiveThread, NULL, &doUiReceiveThread, &timeout);
   
    printf("\nPress Ctrl-C or enter 'quit' to exit\n");     
    printf("\nLinuxSDK UI\n");     
    
    /* Do some app work here... */
    if (!Run_App(  timeout ))
        exit(1);

    /* Wait for thread, don't leave yet - need ctrl-C to quit */
    pthread_join (UiReceiveThread, NULL); 
     
    /* Normal exit */
    exit( 0 );
}

/*
 * $Log: apuitest.c,v $
 */
