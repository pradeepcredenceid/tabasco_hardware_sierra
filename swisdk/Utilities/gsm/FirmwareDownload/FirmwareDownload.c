/*
 *  $Id: FirmwareDownload.c,v 1.2 2009/03/20 00:38:22 rfiler Exp $
 *
 *  Filename: FirmwareDownload.c
 *
 *  Purpose:  This file contains a sample of a Firmware Download
 *            application,
 *
 * Copyright: © 2008 Sierra Wireless, Inc., all rights reserved
 *
 */

/* Linux definitions */
#include <pthread.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

/* include files */
#include "SwiDataTypes.h"
#include "SwiRcodes.h"
#include "SwiApiGsmBasic.h"
#include "SwiApiGsmNetwork.h"
#include "SwiApiGsmSim.h"
#include "SwiApiGsmSs.h"
#include "SwiApiCmBasic.h"
#include "SwiApiCmUpgrade.h"
#include "SwiApiNotify.h"

/* global data */
#define THREESECONDS      3000
#define SDKDEFAULT        "./swisdk"
#define DEFAULTCHUNKSIZE  1600
#define MAXCHUNKSIZE      1900
#define MINCHUNKSIZE       400
#define SUCCESS           0x00
#define UNSUCCESSFUL      0x01

/*
 * Name: imageInfo
 *
 * Purpose: This structure contains information about the caller given
 *          image file.
 *
 * Members: imageType       - type of image
 *          imageVersionStr - string containing the version of the image
 *          imageTypeStr    - string containing the type of the image
 *          numberofchunks  - number of chunks required to download the
 *                            entire image 
 *
 * Note:    none
 *
 */
struct imageInfo
{
    enum SWI_TYPE_FW_IMAGE imageType;
    char imageVersionStr[85];
    char imageTypeStr[5];
    int  numberofchunks;
};

/* image information gathered from the caller supplied image file */
struct imageInfo givenimage;

/* Running in verbose mode or not */
swi_bool verbose = FALSE;

/* Timeout value to be used */
swi_uint32 timeout = THREESECONDS;

/* Last error string buffer */
unsigned char lasterror[80];

/* NV backup variables */
swi_bool nvStoreResult = UNSUCCESSFUL;
swi_bool nvBackUpInProgress = FALSE;

/* Device restarting */
swi_bool resetting = FALSE;

/* Device inserted */
swi_bool devInserted = FALSE;

/* Intial state of the device */
swi_bool boothold = FALSE;

/* RILSTART */
/* Graceful exit flag for verbose mode */
swi_bool exitdaemon = FALSE;

/* Command Line Options */
const char * const short_options = "hf:p:vt:Ps:xn";
/* RILSTOP */

const struct option long_options[] = {
    {"help",    0, NULL, 'h'},     /* Provides terse help to users */
    {"imgpathname", 1, NULL, 'f'}, /* Specify the pathname to download image */
    {"sdkpathname", 1, NULL, 'p'}, /* Specify the pathname to sdk */
    {"verbose", 0, NULL, 'v'},     /* Run in Verbose mode */
    {"timeout", 1, NULL, 't'},     /* Timeout to use for StopnWait calls */
    {"prompt", 0, NULL, 'P'},      /* Prompts before beginning download */
    {"size", 1, NULL, 's'},        /* Chunk size to use during download */
	/* RILSTART */
    {"exit", 0, NULL, 'x'},        /* Stop SDK daemon gracefully when done */
    {"nvbackup", 0, NULL, 'n'},    /* skip NV backup as it is not supported in SL9090RD */
    /* RILSTOP */
    {NULL,      0, NULL, 0  }      /* End of list */
};

/* RILSTART */
/*************
 *
 * Name:     fwd_exit
 *
 * Purpose:  Function registered with the OS using "atexit()" to be
 *           called whenever this program terminates. The sdk daemon
 *           is killed with this function if the caller has specified
 *           the "-x" or "--exit" switch.
 *           
 * Parms:    None
 * 
 * Return:   None
 *
 * Notes:    This function will not be called if the program terminates
 *           abnormally - e.g. by ^C or something similar.
 * 
 **************/
void fwd_exit( void ) 
{
    SwiTerminateSDK();
}
/* RILSTOP */

/**************
 *
 * Name:     waitOnCondition
 *
 * Purpose:  To pause execution of the main thread waiting
 *           for a particular condition to come true.  
 *           
 *           As an option this function can give an indication
 *           to the user in the form of a "spinning" character
 *           that the program is "waiting". 
 *           
 * Parms:    condition - function which returns if the condition is TRUE or not
 *           spin      - spin flag: TRUE if spinning character should be
 *                                  displayed to the user.
 *                                  FALSE otherwise
 * 
 * Return:   none
 *
 * Notes:    none
 * 
 **************/
local void waitOnCondition(swi_bool (*condition)(), swi_bool spin)
{
    /* character to print '\' '|'  '/' '-' */
    char outputchars[] = {92, 124, 47, 45};
    int index = 0;  /* index to allow for cycling through outputchars chars */
    
    /* while the condition is not yet true */
    while(!condition())
    {
        /* if spinning character should be displayed */
        if(spin)
        {
            /* output next character to be displayed */
            putchar(outputchars[index%4]);
        
            /* increment index into outputchars[] */
            index++;
            
            /* force character to be displayed now */
            fflush(stdout);
            
            
        }
        
        /* wait a little bit */
        sleep(1);
        
        /* if spinning characters should be displayed */
        if(spin)
        {
            /* 
             * output a backspace character so that 
             * the next character is output over top of the last one.
             */
            putchar(8);
        }
    }
    
    /* if a spinning character has been outputted */
    if(spin)
    {
        printf("\n");
    }   
}
/**************
 *
 * Name:     devResetting
 *
 * Purpose:  To be used in a call to waitOnCondition() when the caller
 *           needs to wait until the device has begun to reset.
 * 
 *           The event that triggers this condition is the FW_DONE state
 *           received in the SWI_NOTIFY_FwDownload notification.
 *           
 * Parms:    none 
 * 
 * Return:   TRUE - if the device is resetting after completing the 
 *                  firmware downlaod
 *           FALSE - if the device has not yet reset.
 *
 * Notes:    none
 * 
 **************/
local swi_bool devResetting(void)
{
    return(resetting);
}
/**************
 *
 * Name:     devRestarted
 *
 * Purpose:  To be used in a call to waitOnCondition() when the caller
 *           needs to wait until the device has become available after
 *           the device starts up.
 * 
 *           The event that triggers this condition is the bInsert == TRUE
 *           received in the SWI_NOTIFY_AirServerChange notification.
 *           
 * Parms:    none 
 * 
 * Return:   TRUE - if the device has been found to be inserted. 
 *           FALSE - if the device is not yet available.
 *
 * Notes:    none
 * 
 **************/
local swi_bool devRestarted(void)
{
    return(devInserted);
}
/**************
 *
 * Name:     nvBackUpComplete
 *
 * Purpose:  To be used in a call to waitOnCondition() when the caller
 *           needs to wait until the device has completed its' NV
 *           storage backup.
 * 
 *           The event that triggers this condition is the bSuccess == 0x00
 *           flag received in the SWI_NOTIFY_NvStore notification.
 *           
 * Parms:    none 
 * 
 * Return:   TRUE - if the device has been found to be inserted. 
 *           FALSE - if the device is not yet available.
 *
 * Notes:    none
 * 
 **************/
local swi_bool nvBackUpComplete(void)
{
    return(!nvBackUpInProgress);
}
/**************
 *
 * Name:     updatePercentage
 *
 * Purpose:  To be used indicate the percentage complete of the CWE image
 *           file download.  This function can be called every time the
 *           received SWI_NOTIFY_FwDownload notification's state indicates
 *           another CWE image file chunk has been downloaded.
 * 
 *           This function will overwrite the previous percentage completed
 *           displayed to the user with the new percentage.
 *           
 * Parms:    number of chunks sent thus far 
 * 
 * Return:   none
 *
 * Notes:    none
 * 
 **************/
local void updatePercentage(int numsent)
{
    int i;                    /* loop counter */
    static int numchars = 0;  /* total number of chars written by this */
                              /* function since the application was launched */
                              
    /* calculate the percentage complete */
    swi_uint32 percentage = (numsent* 100)/givenimage.numberofchunks;
    
    /*
     * output the backspace character so that the previous displayed 
     * percentage is overwritten by the new percentage
     */
    for(i = 0; i < numchars; i++)
    {
        /* put a single backspace character */
        putchar(8);
    }
    
    /* reset number of chars */
    numchars = 0;
    
    /* if percentage is less than 10 */
    if(percentage < 10)
    {
        putchar(percentage+0x30);
        putchar('%');
        numchars +=2;
    }
    /* else if percentage is less than 100 */
    else if(percentage < 100)
    {
        putchar(percentage/10 + 0x30);
        putchar(percentage%10 + 0x30);
        putchar('%');
        numchars += 3;
    }
    /* else percentage is 100 */
    else
    {
        putchar('1');
        putchar('0');
        putchar('0');
        putchar('%');
        numchars +=4;
    }
    
    /* force character to be displayed now */
    fflush(stdout);
}
/**************
 *
 * Name:     convertASCII2Decimal
 *
 * Purpose:  To convert an ASCII string to a decimal value.
 *           
 * Parms:    asciistrp - pointer to ASCII string
 * 
 * Return:   decimal value
 * 
 * Notes:    none
 * 
 **************/
local swi_uint32 convertASCII2Decimal(char *asciistrp)
{
    swi_uint8 i;                 /* loop counter*/
    swi_uint8 length;            /* length of ASCII string */
    swi_uint32 decimalvalue = 0; /* used to calculate the decimal value */
    
    /* get the length of the ASCII String */
    length = strlen(asciistrp);
    
    /* loop through all characters in the ASCII string */
    for(i = 0; i < length; i++)
    {
        /* multiple current decimal value by 10 to shift digits over */
        decimalvalue *= 10;
        
        /* add next digit to the ONES place */
        decimalvalue += *asciistrp++ - 0x30;
    }
    
    /* return calculated decimal value */
    return(decimalvalue);
}

/**************
 *
 * Name:     notificationcb
 *
 * Purpose:  This function handles received notifications.
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
SWI_CALLBACK_EX notificationcb( struct SwiNotifyVariant *sVariantp)
{
    static int chunkssent; /* used to track download progress */
    
    /* if Firmware Download notification is received */
    if(SWI_NOTIFY_FwDownload == sVariantp->eNotify)
    {
        /* report firmware download status to the user */
        switch(sVariantp->_FwDownload.State)
        {
            case FW_INIT:
                printf("Initializing procedure\n");
                break;
            case FW_INIT_ERR:
                printf("ERROR: Initializing!!!\n");
                break;
            case FW_SOFT_RESET:
                printf("Reset to bootloader\n");
                break;
            case FW_SOFT_RESET_ERR:
                printf("ERROR: bootloader\n");
                break;
            case FW_START_REQ:
                printf("Downloading image\n");
                chunkssent = 1;
                updatePercentage(chunkssent);
                break;
            case FW_START_REQ_ERR:
                printf("ERROR: Starting download\n");
                break;
            case FW_FILE_ERR:
                printf("ERROR: Image file\n");
                break;
            case FW_DWN:
                chunkssent++;
                updatePercentage(chunkssent);
                break;
            case FW_DWN_ERR:
                printf("\nERROR: Downloading\n");
                break;
            case FW_CHKSUM:
                printf("\nVerifying checksum\n");
                break;
            case FW_CHKSUM_ERR:
                printf("ERROR: Checking checksum\n");
                break;
            case FW_FLASH:
                printf("Programming flash\n");
                break;
            case FW_FLASH_ERR:
                printf("ERROR: Programming flash\n");
                break;
            case FW_LAUNCH_FRAG:
                printf("Launching fragment\n");
                break;
            case FW_LAUNCH_FRAG_ERR:
                printf("ERROR: Launching fragment\n");
                break;
            case FW_RESET:
                printf("Device resetting\n");
                break;
            case FW_RESET_ERR:
                printf("ERROR: Resetting device\n");
                break;
            case FW_DONE:
                printf("Procedure completed\n");
                resetting = TRUE;
                devInserted = FALSE;
                break;
            default:
                printf(" UNKNOWN DOWNLOAD STATE - %d\n", sVariantp->_FwDownload.State);
        }
    }
    /* else if the state of the devices availability has changed */
    else if (SWI_NOTIFY_AirServerChange == sVariantp->eNotify)
    {
        /* cache the new device state */
        devInserted = sVariantp->_AirServerChange.bInsert;
    }
    /* else if the NV backup operation has been completed */
    else if (SWI_NOTIFY_NvStore == sVariantp->eNotify)
    {
        /* store the result of the backup operation */
        nvStoreResult = sVariantp->_NvStore.bSuccess;
        
        /* indicate that the back up is no longer in progress */
        nvBackUpInProgress = FALSE;
    }

    return 0;
}

/**************
 *
 * Name:     doNotifications
 *
 * Purpose:  This function sets up callback, enables notification and waits
 *           for notification
 *           
 * Parms:    None
 *
 * Return:   never returns - CTRL-C the program to exit
 *
 * Notes:    This function runs in its' own thread.
 * 
 **************/
 /* RILSTART */
void doNotifications(void)
/* RILSTOP */
{
    static SWI_RCODE retcode;
    
    /* Wait forever for notifications */
    retcode     = SwiWaitNotification();
    if( retcode != SWI_RCODE_OK ) 
    { 
        printf("\nSwiWaitNotification returned error: %d\n", retcode );
    }

/* RILSTART */
   //return(&retcode);
/* RILSTOP */

}

/**************
 *
 * Name:     printUsage
 *
 * Purpose:  Print the usage information for this executable
 *           
 * Parms:    programname - name of the program
 *
 * Return:   none
 *
 * Notes:    This program can be invoked with the following options:
 *
 *           -h               Print usage information and exit cleanly
 *           -v               Verbose mode. 
 *           -f <imagepath>   Use <imagepath> to specific the CWE image file
 *                            to be download to the device.
 *                            Note that the path must include the name of
 *                            the file itself, not just the directory
 *                            where it resides.
 *           -p <sdkpath>     Use <sdkpath> to launch the SDK executable.
 *                            Note that the path must include the name of
 *                            the executable itself, not just the directory
 *                            where it resides. If this option is not present
 *                            the default path ./swisdk is used.
 *           -t <timeout>     Timeout value to use when calling Stop and Wait
 *                            APIs (i.e. Get or Set routines). Specified in 
 *                            milliseconds. The SDK won't run properly for 
 *                            values that are too small. Recommend using 
 *                            minimum 3000 for this value
 *           -F               Prompt the user to verify the download before
 *                            proceeding. 
 * 
 *           The caller must specify a CWE image to download otherwise this
 *           program will exit.
 *
 **************/
local void printUsage( char *programname )
{
    printf("\n\nUsage: %s:\n", programname );
    printf("  -h  --help                        Display this information and exit\n" );
    printf("  -v  --verbose                     Display extra info while running\n");
    printf("                                    NOTE: specify 'verbose' first\n");
    printf("  -t  --timeout  <timeout>          timeout (ms) for Stop and Wait APIs\n");
    printf("  -f  --imgpathname <CWEimagepath>  CWE image to be downloaded is \n");
    printf("                                    at <CWEimagepath>\n");    
    printf("                                    NOTE: this must be an absolute path\n");
    printf("  -p  --sdkpathname <sdkpath>       SDK executable is at <sdkpath>\n");
    printf("                                    NOTE: if not specified the defult \n");
    printf("                                    pathname ./swisdk is used\n");    
    printf("  -P  --prompt                      Prompt user to verify the \n");
    printf("                                    download before proceeding\n");
    printf("  -s  --size                        Download chunk size in bytes\n");
    printf("                                    Min: 400 bytes Max: 1900 bytes\n");
    printf("                                    Default: 1600 bytes\n");
    printf("  -x  --exit                        Gracefully terminate the SDK daemon\n");
    printf("                                    when the download is complete\n\n\n");
    printf("  -n  --no nv backup                Disable NV backup \n");
    printf("                                    SL9090RD didn't support NV backup\n\n\n");
}
/**************
 *
 * Name:     parseCommandLine
 *
 * Purpose:  This function parses the caller's inputed parameters.
 *           
 * Parms:    argc          - argument count
 *           argv          - argument vector
 *           pathnamepp    - memory location to place pointer to the image 
 *                           path string
 *           sdkpathnamepp - memory location to place pointer to the sdk 
 *                           path string
 *           promptp       - memory location to place prompt flag
 *           chunksizep    - Pointer to location to store the chunk size
 *
 * Return:   none
 *
 * Notes:    none
 * 
 **************/
local swi_bool parseCommandLine(
    int argc, 
    char *argv[], 
    char **pathnamepp,
    char **sdkpathnamepp,
    swi_bool *promptp,
    int *chunksizep,
    swi_bool *nvbackup
    )
{
    int result;
    int next_option;
    int optioncount = 0;
    char *programname;
    char *timeoutp;
    char *sizep;
    
    *pathnamepp    = NULL;
    *sdkpathnamepp = SDKDEFAULT;
    *promptp       = FALSE;
    *nvbackup      = TRUE;

    /* Remember our own name */
    programname = argv[0]; 
    
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
                printUsage(programname);
                exit( 0 );
                break;

            case 'f':
                /* caller specifies a pathname to the CWE image to download */
                *pathnamepp = optarg;
                break;
                
            case 'p':
                /* caller specifies a pathname to SDK */
                *sdkpathnamepp = optarg;
                break;

            case 'v':
                /* Verbose mode on */
                verbose = TRUE;
                break;

            case 't':
                /* Timeout */
                timeoutp = optarg;
                timeout = convertASCII2Decimal(timeoutp); 
                break;
                
            case 'P':
                /* Prompt */
                *promptp = TRUE;
                break;
   
            case 's':
                /* Chunk size specified */
                sizep = optarg;
                *chunksizep = convertASCII2Decimal(sizep); 
                break;

           case 'x': 
                /* Gracefully exit SDK daemon when done */
                exitdaemon = TRUE;
                result = atexit(fwd_exit);
                if( result )
                    printf("Error %d: can't install exit function", result);
                break;

           case 'n':
               /* nvbackup */
               *nvbackup = FALSE;
               break;

            case '?':
                /* Caller has used an invalid option */
                if( verbose )
                    printf("Invalid option\n" );

                /* Print the usage info and exit */
                printUsage(programname);
                return(FALSE);

                break;

            case -1:
                /* Done with options list */
                if( verbose )
                {
                    printf("%d command line options found\n", optioncount );
                    printf("verbose:     ON\n");
                    printf("timeout:     %d\n", (int)timeout); 
                    printf("imgpathname: %s\n", *pathnamepp );
                    printf("sdkpathname: %s\n", *sdkpathnamepp );
                    printf("prompt:      %s\n", (*promptp)?"ON":"OFF");
                    printf("chunksize:   %d\n", *chunksizep );
                    printf("exit daemon: %d\n\n", exitdaemon);
                    printf("nvbackup:     %s\n", (*promptp)?"ON":"OFF");
                }

                break;

            default:
                return(FALSE);
                break;
        }
        optioncount++;
    }
    while( next_option != -1 );
    
    /* if the CWE image file name and path were not given */
    if(*pathnamepp == NULL)
    {
        printf("\nError: CWE image path not specified\n");

        /* Add in usage information as well */
        printUsage(programname);
        return(FALSE);
    }

    return(TRUE);
}
/**************
 *
 * Name:     validateSDK
 *
 * Purpose:  Fetch and validate the path to the SDK executable from 
 *           the command line argument
 *           
 * Parms:    pathstrp  - Pointer to a string containing the path to the SDK
 *           openparmp - Pointer to the API Open parameter structure. Will 
 *                       have the path information stored and enabled set to 
 *                       TRUE if the validation is successful
 * 
 * Return:   TRUE      - pathstrp was valid and Enable is set to TRUE
 *           FALSE     - pathstrp invalid and enable is set to FALSE
 *
 * Notes:    On return from this program, the bEnable flag in the openparmp
 *           structure is guaranteed to be either TRUE or FALSE
 *
 **************/
local swi_bool validateSDK( 
    char *pathnamep, 
    struct SWI_STRUCT_ApiStartup *openparmp )
{
    swi_uint8 c;          /* character read from the string */
    swi_uint32 i;         /* FOR loop index */
    swi_uint32 strlength;
    char *strp;      /* Local copy of the string pointer */
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
                printf("Found %s\n\n", pathnamep );

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
 * Name:     initializeAPI
 *
 * Purpose:  This function initializes the API.  It is responsible for
 *           launching the API, registering for notifications, creating
 *           a notification thread and enabling the necessary notifications.
 *           
 * Parms:    openparmp -  Pointer to the API Open parameter structure.
 *
 * Return:   TRUE  - if API was started successful
 *           FALSE - otherwise
 *
 * Notes:    validateSDK() must be called before this function with the 
 *           same openparmp.
 * 
 **************/
local swi_bool initializeAPI( 
    struct SWI_STRUCT_ApiStartup *openparmp , swi_bool nvbackup)
{
    SWI_RCODE retcode;
    struct SWI_STRUCT_AirServerList   airservers;
    swi_uint64              nNotifications;
    /* RILSTART */
    //pthread_t notifthread;
    /* RILSTOP */

    /* Set notifications filter */
    nNotifications = 0xffffffff;
    
    /* start API */
    retcode = SwiApiStartup( openparmp );
    
    /* if API was started successfully */
    if(retcode == SWI_RCODE_OK)
    {
        /* create notification thread */
/* RILSTART */
        if( SwiCreateNotifThread( doNotifications ) != SWI_RCODE_OK )
        {
            printf("Error - Timed out creating notification thread!\n");
            printf("        Please ensure 'doNotifications' calls\n");
            printf("        SwiWaitNotification()\n\n");
            return(FALSE);
        }
/* RILSTOP */
        
        /* register callback to received notifications */
        if(SwiRegisterCallback((void *)notificationcb, 
                                  nNotifications) != SWI_RCODE_OK )
        {
            printf("SwiRegisterCallback failed \n");
            return(FALSE);
        }
        
        /* if the device is not available */
        if(SwiGetAvailAirServers(&airservers) != SWI_RCODE_OK)
        {
            printf("Looking for device\n");
            
            /* wait for device to become available */
            waitOnCondition(devRestarted, TRUE);
        }
        
        /* figure out what mode the device is currently in */
        if(SwiGetBootAndHoldMode( &boothold, timeout ) != SWI_RCODE_OK )
        {
            printf("SwiGetBootAndHoldMode failed \n");
            return(FALSE);
        }
        
        /* if the application is running on the device then */
        if((!boothold)&&(nvbackup))
        {
            if(SwiNotify(SWI_NOTIFY_NvStore, timeout) != SWI_RCODE_OK)
            {
                printf("SwiNotify failed \n");
                return(FALSE);
            }
        }
        
        return(TRUE);
    }
    else
        printf("SwiApiStartup failed \n");
    
    /* SwiApiStartup() failed */
    return(FALSE);
}
/**************
 *
 * Name:     validateCWEImageFile
 *
 * Purpose:  This function validates the caller specified CWE image file.
 *           First it checks that the file exists.  Next it determines
 *           how many chunks will it take to download the entire file and
 *           what is the version and type of the image.
 *           
 * Parms:    filepathp - path and filename of the CWE image to be downloaded
 *           imagep    - memory location to place image information
 *           chunksize - chunksize to use by downloader in SDK 
 *
 * Return:   TRUE  - if file is alright
 *           FALSE - otherwise
 *
 * Notes:    none
 * 
 **************/
local swi_bool validateCWEImageFile(
    char *filepathp,
    struct imageInfo *imagep,
    int chunksize)
{
    FILE *fp;
    struct stat finfo;;
    
    /* attempt to open the file */
    if(!(fp = fopen(filepathp, "rb")))
    {
        printf("Can not open file: %s\n", filepathp);
        return(FALSE);
    }

    /* get the image type string from the CWE header */
    fseek(fp, 0x10C, SEEK_SET);
    fgets(&imagep->imageTypeStr[0], 5, fp);
    rewind(fp);
    
    /* get the image version string from the CWE header */
    fseek(fp, 0x11C, SEEK_SET);
    fgets(&imagep->imageVersionStr[0], 85, fp);
    
    /* close the file */
    fclose(fp );
    
    /* attempt to get the length information about the file */
    if(stat(filepathp, &finfo) != 0)
    {
        printf("Can not determine size of file: %s\n", filepathp);
        return FALSE;
    }

    /* 
     * calculate the number of chunks required to sent to completely
     * download this file
     */
    imagep->numberofchunks = (finfo.st_size + chunksize - 1)/chunksize;
    
    /* convert the image type string to an API image type */
    if(strncmp(&imagep->imageTypeStr[0], "APPL", 4) == 0)
        imagep->imageType = FW_DWN_APPL;
    else if(strncmp(&imagep->imageTypeStr[0], "FOTO", 4) == 0)
        imagep->imageType = FW_DWN_APPL;
    else if (strncmp(&imagep->imageTypeStr[0], "BOOT", 4) == 0)
        imagep->imageType = FW_DWN_BOOT;
    else if (strncmp(&imagep->imageTypeStr[0], "QCOM", 4) == 0)
        imagep->imageType = FW_DWN_QCOM;
    else if (strncmp(&imagep->imageTypeStr[0], "USBD", 4) == 0)
        imagep->imageType = FW_DWN_USBD;
    else if (strncmp(&imagep->imageTypeStr[0], "EXEC", 4) == 0)
        imagep->imageType = FW_DWN_EXEC;
    else if (strncmp(&imagep->imageTypeStr[0], "SWOC", 4) == 0)
        imagep->imageType = FW_DWN_SWOC;
    else
    {
        printf("Unsupported CWE Image type: %s\n", &imagep->imageTypeStr[0]);
        return(FALSE);
    }
    
    return(TRUE);
}
/**************
 *
 * Name:     verifyDownload
 *
 * Purpose:  This function prompts the user to confirm that the given
 *           CWE image should be download to the device.  If available
 *           the current CWE image information from the device is given in 
 *           the prompt. 
 *           
 * Parms:    none
 *
 * Return:   TRUE  - if download should proceed
 *           FALSE - otherwise
 *
 * Notes:    none
 * 
 **************/
local swi_bool verifyDownload(void)
{
    SWI_RCODE                      retcode;   /* return code */
    struct SWI_STRUCT_FlashImgInfo flashInfo; /* image info structure */
    char                           verify;    /* user prompt input */
   
    /* prepare image info request structure */
    memset ((char *)&flashInfo, 0, sizeof(struct SWI_STRUCT_FlashImgInfo));     
    flashInfo.sizeStruct = sizeof(struct SWI_STRUCT_FlashImgInfo); 
    strcpy((char *)&flashInfo.szImageType[0], &givenimage.imageTypeStr[0]);
    
    /* fetch the current image information from the device */
    retcode = SwiGetFlashImgInfo(&flashInfo, timeout);
    
    /* if successful */
    if(retcode == SWI_RCODE_OK)
    {
        printf("The current firmware image is\n");
        printf("\t%s\n\n", &flashInfo.szVersion[0]);
        printf("Do you want to overwrite it with\n");
        printf("\t%s\n\n", &givenimage.imageVersionStr[0]);

    }
    else
    {
        printf("\nDo you want to proceed downloading\n");
        printf("\t%s\n\n", &givenimage.imageVersionStr[0]);
    }
    
    printf("(Y)es or (N)o?    ");
    scanf("%c", &verify); 
    printf("\n");
    
    if(verify == 'Y' || verify == 'y')
        return(TRUE);
    else
        return(FALSE);
}
/**************
 *
 * Name:     backupNvStorage
 *
 * Purpose:  This function attempts to backup the NV storage on the device.
 *           This action must occur before beginning the firwmare download
 *           
 * Parms:    none
 *
 * Return:   TRUE  - if backup was successful
 *           FALSE - otherwise
 *
 * Notes:    none
 * 
 **************/
local swi_bool backupNvStorage(void)
{
    struct SWI_STRUCT_NvStore sNvStore;
    SWI_RCODE retcode;
    
    printf("Backing up NV settings\n");
    nvBackUpInProgress = TRUE;
    nvStoreResult = UNSUCCESSFUL;
    
    /* preparing NV store request structure */
    sNvStore.sizeStruct = sizeof(struct SWI_STRUCT_NvStore);
    sNvStore.eOperation = SWI_NVOPER_Backup;
    sNvStore.eCategory = SWI_NVCAT_User;
    
    /* request NV settings are backed up */
    retcode = SwiNvStore(sNvStore, timeout);
        
    /* if request was rejected */
    if( retcode == SWI_RCODE_REQUEST_REJECTED )
    {
        SwiGetLastError( (swi_charp) lasterror, sizeof(lasterror) );
        printf("  Mdm reports CnS Error: %s\n", lasterror );
    }
    /* else request is being processed by device */
    else
    {
        /* wait for backup to complete */
        waitOnCondition(nvBackUpComplete, FALSE);
    }

    /* if back up was successful */
    if(nvStoreResult == SUCCESS)
    {
        return(TRUE);
    }
    return(FALSE);
}
/**************
 *
 * Name:     downloadImage
 *
 * Purpose:  This function attempts to download the given CWE image to the 
 *           device.  
 *           
 * Parms:    pathnamep - pointer to path name string
 *           chunksize - size parameter for SwiFirmwareDownload()
 *
 * Return:   TRUE  - if download was successful
 *           FALSE - otherwise
 *
 * Notes:    none
 * 
 **************/
local swi_bool downloadImage(char *pathnamep, int chunksize)
{
    SWI_RCODE retcode;
    
    /* request download of image */
    retcode = SwiFirmwareDownload(
                    (swi_charp) pathnamep, 
                    givenimage.imageType,
                    FALSE,
                    chunksize, 
                    3000);  
                    
    /* if request failed */
    if(retcode != SWI_RCODE_OK)
    {
        if(verbose)
        {
            switch(retcode)
            {
                case SWI_RCODE_REQUEST_TIMEOUT:
                    printf("SwiFirmwareDownload() timeout\n");
                    break;
                case SWI_RCODE_REQUEST_REJECTED:
                    SwiGetLastError( (swi_charp) lasterror, sizeof(lasterror) );
                    printf("SwiFirmwareDownload() rejection: %s\n", lasterror);
                    break;
                default:
                    printf("SwiFirmwareDownload() failure: retcode[%d]\n", retcode);
            }
        }
        return(FALSE);
    } 
    return(TRUE);
}

/**************
 *
 * Name:     main
 *
 * Purpose:  Entry point where testing of the API package starts. 
 *           
 * Parms:    argc   - Not used
 *           argv[] - Not used
 * 
 * Return:   0 - Normal termination
 *           1 - Problem with the execution
 *
 * Notes:    see function apprintusage()
 * 
 **************/
int main( int argc, char *argv[] )
{
    struct SWI_STRUCT_ApiStartup openparm;
    char *pathnamep;
    char *sdkpathnamep;
    swi_bool prompt;
    swi_bool nvbackup;
    int chunksize;
    
    /* Initialize this value to the default */
    chunksize = DEFAULTCHUNKSIZE;

    /* Parse the command line  */
    if(!parseCommandLine(argc, argv, &pathnamep, &sdkpathnamep, &prompt, &chunksize, &nvbackup))
    {
        exit(1);
    }

    /* Validate the Chunk Size */
    if( chunksize < MINCHUNKSIZE || chunksize > MAXCHUNKSIZE )
    {
        printf("Chunk size of %d is not in valid range\n", chunksize );
        exit(1);
    }
    
    /* validate SDK image */
    if(!validateSDK(sdkpathnamep, &openparm))
    {
        printf("No executable called %s found\n", SDKDEFAULT);
        exit(1);
    }
    
    /* initialize API */
    if(!initializeAPI(&openparm,nvbackup))
    {
        printf("API Open failed\n" );
        exit( 1 );
    }

    /* get filename from the user */
    if(!validateCWEImageFile(pathnamep, &givenimage, chunksize))
    {
        exit(1);
    }
    
    /* if prompt was requested in the command line */
    if(prompt)
    {
        /* prompt the user to decide if the download should proceed */
        if(!verifyDownload())
        {
            exit(0);
        }
    }

    /* if not in boot-n-hold mode */
    if((!boothold)&&(nvbackup))
    {
        //SL9090RD doesn't support NV backup
        /* back up user settings in NV storage on the device */
        if(!backupNvStorage())
        {
            printf("NV backup failed\n");
            exit(1);
        }
    }
 
    /* download the given image */
    if(!downloadImage(pathnamep, chunksize))
    {
        printf("Download procedure failed\n");
        exit(1);
    }
    
    /* wait for the download procedure to have completed */
    waitOnCondition(devResetting, FALSE);
    
    /* 
     * wait for the device to restart before indicating that the 
     * download is complete 
     */
    printf("Device restarting\n");
    waitOnCondition(devRestarted, TRUE); 

    printf("Download successfully completed\n");
    exit( 0 );
}


/*
 * $Log: FirmwareDownload.c,v $
 */
