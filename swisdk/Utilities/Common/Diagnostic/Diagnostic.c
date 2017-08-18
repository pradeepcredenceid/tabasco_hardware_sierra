/*
 *  $Id: Diagnostic.c,v 1.3 2009/04/15 00:51:11 epasheva Exp $
 *
 *  Filename: Diagnostic.c
 *
 *  Purpose:  This program is a small utility that starts up a diagnostic 
 *            session on the modem and collects and stores the resulting 
 *            information to a file on disk
 *
 * Copyright: © 2008 Sierra Wireless, Inc., all rights reserved
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
#include "SwiApiGsmNetwork.h"
#include "SwiApiGsmSim.h"
#include "SwiApiGsmSs.h"
#include "SwiApiCmBasic.h"
#include "SwiApiCmUpgrade.h"
#include "SwiApiNotify.h"
#include "SwiApiDemux.h"

extern void slmemset(char *srcp, swi_uint8 value, swi_uint32 length);
    
/* Internal Definitions */

/* Maximum buffer size for data indication message */
#define MAXBUFSIZE          3000 

/* Default timeout value */
#define DIAGDEFAULTTIMEOUT  3000  

/* Pause value */
/* Note: This delay is needed to be able to log properly the modem responses
 *       to all commands
 */
#define DIAGPAUSE           1

/* Maximum size of data sent to modem (in number of bytes) */
#define MAXCHUNKSIZE      200  

/* Maximum size of log file */
#define MAXSIZELOGF 10000000

/* Current version bytes definition of the filter */
#define FLTVER_BYTE1     0x76
#define FLTVER_BYTE2     0x00
#define FLTVER_BYTE3     0x01

/* global data */

/* Structure for controlling receive path thread */
pthread_t                     DxReceiveThread; 

/* Running in verbose mode or not */
swi_bool verbose = FALSE;

/* Name of this program */
const char *programname;

/* Storage for Control Indication */
swi_uint8 devicepresent = SWI_OFFLINE;

/* Log file descriptor pointer */ 
FILE* ofp = NULL;

/* Storage for log file name */
char lfile[200]   = "SwiDiagOutput0.ldk";   

/* Pointer to log file name storage */
char *pathoutp  = &lfile[0]; 

/* Log file index variable */  
swi_uint32 indexfname = 0;  
    
/* Buffer to build a Demux Data Indication Message */
swi_uint8 diagtestmsgarray[MAXBUFSIZE];

/* Stop device service requests */
swi_uint8  stopEventLogging[] = {
    0X60, 
    0x00, 
    0x12, 
    0x6a, 
    0x7e
};

swi_uint8  stopMessageLogging[] = {
    0x7d, 
    0x5d, 
    0x05, 
    0x00, 
    0x00, 
    0x00, 
    0x00, 
    0x00, 
    0x00, 
    0x74, 
    0x41, 
    0x7e
};

swi_uint8  stopItemLogging[] = {
    0x73, 
    0x00, 
    0x00, 
    0x00, 
    0x00, 
    0x00, 
    0x00, 
    0x00, 
    0xda, 
    0x81,
    0x7e
};

struct tm *now = NULL;

/*
 * Name: inFilterInfo
 *
 * Purpose: This structure contains information about the caller given
 *          input filter file.
 *
 * Members: versionStr        - string containing the version of the inFilter
 *          filterlength      - length of the filter 
 *          numofchunks       - number of chunks required to download the
 *                              entire inFilter 
 *
 * Note:    The version string of the filter needs to be stripped; it is not
 *          sent to the  modem
 *
 */
struct inFilterInfo
{
    char versionStr[3];
    int  filterlength;
    int  numofchunks;
};

/* Filter file information gathered from the caller supplied file */
struct inFilterInfo givenfilter;

/* Command line options, short form. Note, a ":" character following an option 
 * indicates to the operating system that that option takes an argument(s). 
 */
const char * const short_options = "hi:p:vt:";

const struct option long_options[] = {
    {"help",    0, NULL, 'h'},      /* Provides terse help to users */
    {"infile",  1, NULL, 'i'},      /* path to file holding filter settings */
    {"pathname",1, NULL, 'p'},      /* Specify the pathname */
    {"verbose", 0, NULL, 'v'},      /* Run in Verbose mode */
    {"timeout", 1, NULL, 't'},      /* Timeout to use for StopnWait calls */
    {NULL,      0, NULL, 0  }       /* End of list */
};

/**************
 *
 * Name:     diagfilename
 *
 * Purpose:  Update the log filename that the app is currently filling with
 *           data
 *
 * Parms:    index             - current log file in log file series
 * 
 * Return:   None.
 * 
 * Notes:    None.
 * 
 **************/
package void diagfilename( swi_uint32 index )
{
    sprintf(&lfile[0], "SwiDiagOutput%.2d-%.2d-20%.2d_%d-%d_%ld.ldk", 
    now->tm_mday, (now->tm_mon + 1), (now->tm_year - 100),  
    now->tm_hour, now->tm_min, index);
}

/**************
 *
 * Name:     diagdatacallback
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
package void diagdatacallback( swi_uint16 deviceservice, swi_uint8 *datap, 
    swi_uint32 datalen, void *userp )
{
    /* Local pointer of data indication packet */
    swi_uint8  *localp; 
    
    /* File info storage */              
    struct stat finfo;   
    
    /* Flag for valid output file */             
    swi_bool flagopenfile = FALSE; 

    /* Number of bytes written to disk */
    size_t byteswritten;     
    
    /************************
     * End of declarations 
     ************************/

    /* Set local pointer to data indication */
    localp = datap;
    
    /* Open the log file */
    if ( !(ofp = fopen( (char *)pathoutp, "awb" )) )
    {
        if( verbose )
            printf( "\nCannot open file%s\n", pathoutp );
        flagopenfile = FALSE;
    }
    else
    {
        flagopenfile = TRUE;    
    }
    if ( flagopenfile )
    {
        /* Extract data and write to disk */
        byteswritten = fwrite( datap, 1, datalen, ofp);
    }
    else
    {
        printf( "\nCannot Log To File!!!\n" );        
    }
    
    /* Close the file */
    fclose( ofp );
    
    /* attempt to get the length information about the file */
    if(stat(pathoutp, &finfo) != 0)
    {
        printf("Can not determine size of file: %s\n", pathoutp);
    }
    /* If the log file has exceeded a threshold (currently 10MBytes) */    
    if( finfo.st_size > MAXSIZELOGF ) 
    {
        indexfname++;
        diagfilename(indexfname);
    }
}

/**************
 *
 * Name:     diagctlcallback
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
package void diagctlcallback( swi_uint16 deviceservice, swi_uint8 *ctldatap, 
    void *userp)
{
    /* Get control data */
    devicepresent = *ctldatap;
    if (devicepresent == SWI_ONLINE)
        printf("\n\nDemux Ctl Ind callback: device On-Line\n");
    else
        printf("\n\nDemux Ctl Ind callback: device Off-Line\n");     
}

/**************
 *
 * Name:     diagprintusage
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
 *           -i <filterpath>  Use <filterpath> to specify the input filter
 *                            file that is required to start the diagnostics
 *                            stream
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
local void diagprintusage( void )
{
    printf("\n\nUsage: %s:\n", programname );
    printf("   -h  --help                Display this information and exit\n" );
    printf("   -v  --verbose             Display extra info while running\n");
    printf("                             NOTE: specify 'verbose' first\n");
    printf("   -t  --timeout  <timeout>  timeout (ms) for Stop and Wait APIs\n");
    printf("   -p  --pathname <sdkpath>  SDK executable is at <sdkpath>\n");
    printf("                             NOTE: always specify pathname\n");
    printf("   -i  --infile <inputpath>  Path to the file containing the\n" );
    printf("                             input info needed to start diags \n" );
    printf("                             in the modem\n" );
}

/**************
 *
 * Name:     diaggettimeout
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
local swi_bool diaggettimeout( char *timeoutp, unsigned int *timevalp )
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
 * Name:     diaggetpathname
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
local swi_bool diaggetpathname( swi_uint8 *pathnamep, 
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
    /* Return code from API calls */
    SWI_RCODE retcode;
    
    /* Device service type storage */
    swi_uint16 deviceservice;
    
    /* Optional callback argument for data traffic from modem */
    void                          *datauserp="DataInd";

    /* Optional callback argument for status traffic from modem */
    void                          *ctluserp="CtlInd";
    
    /************************
     * End of declarations 
     ************************/
    
    /* Set device service to Diagnostic */
    deviceservice = SWI_DIAG;
    
    /* Register for Diagnostic device service */
    retcode =  SwiApiDxRegister(deviceservice, diagdatacallback, datauserp, 
                                diagctlcallback, ctluserp );  
    if( verbose )
    {
        switch( retcode )
        {
            case SWI_RCODE_OK:
                printf("\nDIAG: registered the following callbacks:\n");
                printf("DIAG: Data Callback: 0x%x, Data User Parameter: %s\n",
                    (unsigned int)diagdatacallback, (char *)datauserp); 
                printf("DIAG: Ctl  Callback: 0x%x, Ctl  User Parameter: %s\n",
                    (unsigned int)diagctlcallback, (char *)ctluserp);     
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
 * Name:     validateInputFilterFile
 *
 * Purpose:  This function validates the caller specified input filter file.
 *           First it checks that the file exists.  Next it determines
 *           how many chunks will it take to download the entire file 
 *           
 * Parms:    filepathp - path and filename of the filter file to be downloaded
 *           inFilterp - memory location to place inFilter information
 *
 * Return:   TRUE  - if file is alright
 *           FALSE - otherwise
 *
 * Notes:    none
 * 
 **************/
local swi_bool validateInputFilterFile(
    char *filepathp,
    struct inFilterInfo *inFilterp)
{
    /* File descriptor pointer */
    FILE *fp;
    
    /* File information structure */
    struct stat finfo;
    
    /************************
     * End of declarations 
     ************************/
    
    /* attempt to open the file */
    if(!(fp = fopen(filepathp, "rb")))
    {
        printf("Can not open file: %s\n", filepathp);
        return(FALSE);
    }   
    /* get the version string from the filter file */
    inFilterp->versionStr[0] = fgetc(fp);
    inFilterp->versionStr[1] = fgetc(fp);
    inFilterp->versionStr[2] = fgetc(fp);
    fclose(fp );
    
    /* Validate the version string */
    if ((inFilterp->versionStr[0] == FLTVER_BYTE1) &&
        (inFilterp->versionStr[1] == FLTVER_BYTE2) &&
        (inFilterp->versionStr[2] == FLTVER_BYTE3) )
    {
        if (verbose)
            printf("Good file version of file: %s\n", filepathp);       
    }
    else
    {
        printf("Wrong version of file: %s\n", filepathp);
        return FALSE;        
    }    
    /* attempt to get the length information about the file */
    if(stat(filepathp, &finfo) != 0)
    {
        printf("Can not determine size of file: %s\n", filepathp);
        return FALSE;
    }
    /* if the file is empty */
    if(finfo.st_size == 0)
    {
        printf("File is empty: %s\n", filepathp);
        return(FALSE);        
    }

    /* 
     * calculate the number of chunks required to send to completely
     * download this file
     */
    /* Note: The first 3 bytes of the filter file containing its version
     * are not sent to the modem
     */
    inFilterp->filterlength   = finfo.st_size - 3;    
    inFilterp->numofchunks = (inFilterp->filterlength + 
        MAXCHUNKSIZE - 1)/MAXCHUNKSIZE;
    
    return(TRUE);
}

/**************
 *
 * Name:     downloadFilter
 *
 * Purpose:  This function attempts to download the given filter file  to the 
 *           device.  
 *           
 * Parms:    infilternamep - pointer to filter name string
 *
 * Return:   TRUE  - if download was successful
 *           FALSE - otherwise
 *
 * Notes:    There filter file is sent in chunks.
 * 
 **************/
local swi_bool downloadFilter(char *infilternamep)
{
    /* Return code from API calls */
    SWI_RCODE retcode = SWI_RCODE_OK;
    
    /* File descriptor pointer */
    FILE                          *fp;
    
    /* Index variable */
    swi_uint32 i;
    
    /* Length of payload pointed to by payloadp */
    swi_uint32                    payloadlen;  
     
    /* Offset in buffer where can write our payload leaving room for header */
    swi_uint32                    bytesoffset; 
    
    /* Pointer to payload data to send to diagnostic agent in modem */
    swi_uint8                     *payloadp;
    
    /* Variable to test the read input */
    int                            testread;
    
    /************************
     * End of declarations 
     ************************/

    /* Open file first */
    fp = fopen (infilternamep, "r");
    
    /* Make sure file open is OK */
    if (fp == NULL)
    {
        printf ("Error: opening %s for reading\n", infilternamep);
        exit(1);
    }
    
    printf ("Downloading filter...\n");
    
    /* The first 3 bytes are not to be sent to the modem */
    testread = fgetc(fp);
    testread = fgetc(fp);
    testread = fgetc(fp);
     
    while (givenfilter.numofchunks > 0)
    {   
        /* Determine the packet length */
        if (givenfilter.numofchunks > 1)
        {
            payloadlen = MAXCHUNKSIZE;
        }
        else if (givenfilter.numofchunks == 1)
        {
            payloadlen = givenfilter.filterlength; /* updated in the loop */        
        }
 
        /* Obtain pointer to the payload section of data packet */    
        retcode = SwiGetDataPldOffset( &diagtestmsgarray[0], 
                                   payloadlen, 
                                   &payloadp, 
                                   &bytesoffset); 
        if (retcode != SWI_RCODE_OK)
        { 
            /* Indicate that Data Indication packet cannot be built due to
             * insufficient memory allocated by the caller
             */
            printf("\nData packet cannot be built - no memory \n");
            return(FALSE);        
        }

        /* Copy filter into payload buffer */                               
        for ( i= 0; i< payloadlen; i++ )
        {
            testread = fgetc(fp);
            if (testread != EOF)
                *payloadp++ = (swi_uint8)testread;
            else
                break;
            
        } 
        /* Adjust the length to include the header */
        payloadlen = i+bytesoffset; 

        /* Decrement the number of chunks left to be written */
        givenfilter.numofchunks--;
        if (givenfilter.numofchunks == 0)
        {
            givenfilter.filterlength = 0; /* set to 0 to prevent negative values
                                           * for the last portion when is less 
                                           * than MAXCHUNKSIZE 
                                           */
        }
        else
        {
            givenfilter.filterlength -= MAXCHUNKSIZE;   
        }
        
        /* Send Data Indication to SDK */
        retcode = SwiApiDxSend(SWI_DIAG, &diagtestmsgarray[0], payloadlen); 
    
        /* If send failed */
        if(retcode != SWI_RCODE_OK)
        {
            if(verbose)
            {
                switch(retcode)
                {
                    case SWI_RCODE_REQUEST_TIMEOUT:
                        printf("SwiApiDxSend() timeout\n");
                        break;
                    case SWI_RCODE_INVALID_PAR:
                        printf("SwiApiDxSend() failure: invalid channel\n");
                        break;
                    case  SWI_RCODE_API_NOT_OPEN:
                        printf("SwiApiDxSend() failure: AP not open\n");
                        break;
                    case  SWI_RCODE_NOT_INIT:
                        printf("SwiApiDxSend() failure: Old packet\n");
                        break;
                    case SWI_RCODE_DEVICE_UNAVAIL:
                        printf("SwiApiDxSend() failure: No device\n");
                        break;
                    default:
                        printf("SwiApiDxSend() failure: retcode[%d]\n",retcode);
                }
            }
            /* Close the file */
            fclose (fp);
            return(FALSE);
        } 
        /* yield */
        sleep(0);
        /* clear the buffer */
       slmemset((char *)&diagtestmsgarray[0], 0, MAXBUFSIZE);
    }

    /* Done  downloading the filter */
    /* Close the file */
    fclose (fp);
    printf ("Filter downloaded\n");
    return(TRUE);
}

/**************
 *
 * Name:     sendtoDevice
 *
 * Purpose:  This function attempts to send up to 2K user specified data buffer
 *           to the  device.  
 *           
 * Parms:    datatosendp - pointer to data to send to device
 *           datalen     - length of data to send
 *
 * Return:   none
 *
 * Notes:    This function assumes data to send are less than 2K bytes size.
 *           It is used to send predefined test commands to the device.
 * 
 **************/
swi_bool sendtoDevice(swi_uint8 *datatosendp, swi_uint32 datalen)
{
    
    /* Return code from API calls */
    SWI_RCODE                     retcode = SWI_RCODE_OK;
    
    /* Pointer to test data to send to diagnostic agent in modem */
    swi_uint8                     *datap = NULL;
    
    /* Pointer to payload data to send to diagnostic agent in modem */
    swi_uint8                     *payloadp = NULL;

    /* Length of payload pointed to by payloadp */
    swi_uint32                    payloadlen = 0;  
    
    /* Offset in buffer where can write our payload leaving room for header */
    swi_uint32                    bytesoffset = 0; 
    
    /* Index variable */
    swi_uint32                    i = 0;
    
    /************************
     * End of declarations 
     ************************/
    
    /* Clear the buffer */
    slmemset((char *)&diagtestmsgarray[0], 0, MAXBUFSIZE );
    
    /* Obtain pointer to the payload section of data packet */    
    retcode = SwiGetDataPldOffset( &diagtestmsgarray[0], 
                                   datalen, 
                                   &payloadp, 
                                   &bytesoffset);    
        
    if (retcode == SWI_RCODE_OK)
    { 
        /* Build the message */
        datap = datatosendp;
        for( i=0; i < datalen; i++ )
        {
            /* Copy data */
            *payloadp++ = *datap++;
        }
    
        /* Adjust the length to include the header */
        payloadlen = i+bytesoffset; 
        
        /* Send Data Indication to SDK */
        retcode = SwiApiDxSend(SWI_DIAG,&diagtestmsgarray[0],payloadlen); 
        if (retcode == SWI_RCODE_DEVICE_UNAVAIL)
        {
            printf("\nDevice is not loaded onto the system \n");
            return FALSE;       
        } 
    }
    else
    {
        /* Indicate that Data Indication packet cannot be built due to
         * insufficient memory allocated by the caller
         */
        if (verbose)
            printf("\nData packet cannot be built - no memory \n");  
        return FALSE;
    }
    return TRUE;    
}

/*
 * Name:     cancelDxReceiveThread
 *
 * Purpose:  This function cancels the receive thread.
 *
 * Parms:    None.
 *
 * Return:   TRUE if successfull, FALSE otherwise.
 *
 * Notes:    None
 *
 */
swi_bool cancelDxReceiveThread()
{
   /* The doDxReceiveThread never returns.... */
    pthread_cancel (DxReceiveThread);

    return TRUE;
}

/**************
 *
 * Name:     main
 *
 * Purpose:  Entry point for the Diagnostics demultiplexing utility
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
    /* For parsing incoming command line switches */
    int                           next_option;

    /* Number of command line options detected */
    int                           optioncount = 0;

    /* Structure required for starting up the API */
    struct SWI_STRUCT_ApiStartup  openparms;

    /* Pointer to the command line timeout duration */
    char                          *timeoutp;

    /* Pathname to SDK executable image */
    char                          *pathnamep = NULL;

    /* Path to input file, NULL if none required */
    char                          *inpathp = NULL;

    /* Return code from API calls */
    SWI_RCODE                     retcode;

    /* The actual timeout to be used for this execution */
    unsigned int                  timeout = DIAGDEFAULTTIMEOUT;

    /* Status variables*/
    swi_bool status = FALSE;
    swi_bool bContinue = TRUE;
    
    /* Menu selection variables */
    int choice = -1;
    char szchoice[30];  

    /************************
     * End of declarations 
     ************************/
    
    /* Establish startup time for filename */
    time_t time_value = time(NULL); 
    now = localtime(&time_value);
    diagfilename(indexfname);
    
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
                diagprintusage();
                exit( 0 );
                break;

            case 'i':
                /* caller specifies an optional input filter file */
                inpathp = optarg;
                if( verbose )
                    printf("\ninput file: %s\n", pathnamep );
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

            case '?':
                /* Caller has used an invalid option */
                if( verbose )
                    printf("\nInvalid option\n" );

                /* Print the usage info and exit */
                diagprintusage();
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
        diagprintusage();
        exit(1);        
    }

    /* If the pathnamep points to a valid executable */
    if( !diaggetpathname( (unsigned char *)pathnamep, &openparms ) )
    {
        printf("\n\nInvalid path for SDK executable specified; test terminated\n");
        exit(1);
    }

    /* Validate the timeout value, if specified */
    if( timeoutp )
    {
        /* If the specified timeout is valid, convert from ASCII */
        diaggettimeout( timeoutp, &timeout );
    }
    
    /* Initialize the filter structure */
    givenfilter.versionStr[0]  = 0;
    givenfilter.versionStr[1]  = 0;
    givenfilter.versionStr[2]  = 0;
    givenfilter.filterlength   = 0;
    givenfilter.numofchunks    = 0;

    /* Check for mandatory filter argument */
    if (inpathp == NULL)
    {
        printf("\n\nNo filter specified; test terminated\n");
        exit(1);
    }
    else
    {
        /* Ensure the input file exists, if one was specified */
        if(!validateInputFilterFile(inpathp, &givenfilter))
        {
            printf("\n\nInvalid filter specified; test terminated\n");
            exit(1);
        }        
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
                printf("\n\nDIAG: Unrecognized error: %d\n", retcode );
                exit( 1 );
                break;
        }
    }
    
    /* A separate thread manages incoming DIAG messages */    
    pthread_create (&DxReceiveThread, NULL, &doDxReceiveThread, &timeout);
    /* Note: Registration for Control and Data Indication is performed by 
     *       doDxReceiveThread()
     */  

    while( bContinue )
    {
        /* Print menu items */
 
            printf("\n*******SWI Diagnostic SAMPLE MENU******\n");
            printf(" 1: Start DIAG Service\n");
            printf(" 2: Stop DIAG Service & Exit\n");
            printf("Enter choice or Press Ctrl-C to quit:");
            scanf("%s", szchoice );
            choice = atoi(szchoice);
        
        switch(choice)
        {
            case 1: /* Start DIAG Service */
            {
                /* Start DIAG device service */    
                retcode = SwiApiDxBegin( SWI_DIAG );  
                if( retcode == SWI_RCODE_OK )
                {
                    printf("\nDIAG Service started\n");   
                }
                else
                {
                    printf("\nDIAG: Error %d, cannot begin the service; test aborted\n", 
                        retcode);
                    exit(1);
                }
            
                /* Wait for modem detection */
                if ( verbose )
                printf("\nWaiting for Air Server - SLEEPING! \n"); 
                while(devicepresent == SWI_OFFLINE)
                {
                    sleep(DIAGPAUSE);    
                } 
                /* Download user specified filter to modem */
                status = downloadFilter( inpathp );
                if (verbose)
                {
                    if (status)
                    {
                        printf("\ndownloadFilter Successful\n");        
                    }
                    else
                    {
                        printf("\ndownloadFilter Failed!\n"); 
                        exit(1);        
                    }
                }
            }
            break;

            case 2: /* Stop */
            {
                printf("\n\nDiagnostic Service stopping...\n"); 
                /* Send stop commands to the device first */
                if (!sendtoDevice(&stopItemLogging[0],sizeof(stopItemLogging)))
                {
                    printf("\nCannot send StopItemLogging command\n");
                    printf("SwiApiDxSend() failure: No device\n");                    
                } 
                if (!sendtoDevice(&stopMessageLogging[0],
                        sizeof(stopMessageLogging)))
                {
                    printf("\nCannot send StopMessageLogging command\n");
                    printf("SwiApiDxSend() failure: No device\n");                            
                }
                if (!sendtoDevice(&stopEventLogging[0], 
                       sizeof(stopEventLogging)))
                {
                    printf("\nCannot send StopEventLogging command\n");
                    printf("SwiApiDxSend() failure: No device\n");                           
                }
                
                /* Give the modem a chance to reply */
                sleep(DIAGPAUSE);  
                
                /* Disable the communication pipe */  
                retcode =  SwiApiDxEnd( SWI_DIAG );  
                printf("\n\nDiagnostic Service stopped\n"); 
                /* Note: The log file is closed in the data callback */
                
                /* Give the modem a chance to reply */
                sleep(DIAGPAUSE); 
                
                /* Exit */
                bContinue = FALSE;
            }
            break;

            default: /* Error */
                printf("Incorrect choice. Please retry\n");
            break;

        }/*End of Switch */

        if( bContinue )
        {
            printf("--------------------------------------\n");
            printf("Enter Zero (0) to return to the Main Menu:");

            scanf("%s", szchoice );
            system("clear");

        }
    }/*End of while loop */
    
    cancelDxReceiveThread();
    return 0;

}

/*
 * $Log: Diagnostic.c,v $
 * Revision 1.3  2009/04/15 00:51:11  epasheva
 * BZ 15519.
 *
 * Revision 1.2  2009/03/04 22:58:46  bsiu
 * Log files now has date so that each series of log files would be unique and separate. Menu
 * option 2 and 3 are merged together since diagnostic can not be started again after
 * stopping without exiting the app.
 *
 * Revision 1.1  2009/03/03 20:24:05  bsiu
 * Moved to Common directory as utility should be usable for CDMA as well
 *
 * Revision 1.4  2008/12/12 23:33:47  epasheva
 * Fix for compiler warning. Changed "aborted" to "terminated".
 *
 * Revision 1.3  2008/12/12 02:43:27  epasheva
 * Improvements for filter downloading in small portions so that responses
 * are logged properly.
 *
 * Revision 1.2  2008/12/05 01:56:50  epasheva
 * BZ13739
 *
 * Revision 1.1  2008/12/02 03:52:39  epasheva
 * Initial checkin
 *
 */
