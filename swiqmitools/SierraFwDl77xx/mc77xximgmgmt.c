/*************
 *
 * Filename:    mc77xximgmgm.c
 *
 * Purpose:     MC7xxx Image Management application
 *
 * Copyright: © 2014 Sierra Wireless Inc., all rights reserved
 *
 **************/
#include "SWIWWANCMAPI.h"
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>

#define  LOG_TAG  "swi_imgmgr"
#include "swiril_log.h"
#include "im/imudefs.h"
#include "qm/qmerrno.h"
#include <swiqmitool_common.h>

/****************************************************************
*                       DEFINES
****************************************************************/
#define OPTION_LEN         4
#define SUCCESS            0
#define DEFAUL_TTIME_OUT   300
#define MAX_WAIT_DEVICE_READY 4
#define MAXPATHSIZE        255
#define MAX_IMAGE_PATH     514
#define ENTER_KEY          0x0A
#define IMG_FW_TYPE_SPK    4

/****************************************************************
*                       DATA STRUCTURES
****************************************************************/

/* firmware download */
static swi_bool fwDwlComplete = FALSE;
static swi_bool fwDeviceReady = FALSE;
static swi_bool verbose = FALSE;
static swi_bool forceDld   = FALSE;
static swi_bool fwdwfail = FALSE;

/* Version information*/
const char * const VERSION = "1.3.0.0";

/* Command Line Options */
const char * const short_options = "h?vglVfp:s:d:i:t:";

const struct option long_options[] = {
    {"help",        0, NULL, 'h'},  /* Provides terse help to users */
    {"help",        0, NULL, '?'},  /* Provides terse help to users */
    {"version",     0, NULL, 'V'},  /* Application version infor */
    {"verbose",     0, NULL, 'v'},  /* Run in Verbose mode */
    {"force",       0, NULL, 'f'},  /* Force to download firmare regardless of host firmware version */
    {"get",         0, NULL, 'g'},  /* current firmware info */
    {"info",        1, NULL, 'i'},  /* Display the information for the executing device image */
    {"download",    1, NULL, 'd'},  /* Download an image to the device */
    {"timeout",     1, NULL, 't'},  /* Seconds of timeout for downloading image to the device */
    {"listimages",  0, NULL, 'l'},
    {"pathhost",    1, NULL, 'p'},
    {"selectimage", 1, NULL, 's'},
    {NULL,          0, NULL,  0  }   /* End of list */
};

struct cwefwinfo{
    CHAR amss[128];
    CHAR boot[128];
    CHAR pri[128];
};

/****************************************************************
*                       FUNCTIONS
****************************************************************/
/*************
 *
 * Name: parse_ps_outputline
 *
 * Purpose:  Parse each line to store PID and CMD value
 *
 * Params:   pLine  - input line
 *           pProc  - process name 
 *
 * Return:   pid
 *
 * Abort:    on error conditions
 *
 * Notes:    
 *
 **************/
static int parse_ps_outputline(char* pLine, char* pProc)
{
    char pid[64];
    char *tempToken = NULL;
    char *lastToken = NULL;

    if( pProc == NULL)
        return -1;

    tempToken = strtok(pLine," \n");
    if(tempToken == NULL)
        return -1;
    
    tempToken = strtok(NULL," \n");
    strcpy(pid,tempToken);

    while(tempToken != NULL)
    {
        lastToken = tempToken;
        tempToken = strtok(NULL," \n");
    }
    if(lastToken == NULL)
        return -1;
    if (!strcmp(lastToken,pProc))
    {
        return atoi(pid);
    }
    return -1;
}

/*************
 *
 * Name: getpidof
 *
 * Purpose:  Alternate implementation of pidof command
 *
 * Params:   ProcName  - string describing the process
 *
 * Return:   pid
 *
 * Abort:    on error conditions
 *
 * Notes:    pidof not avaliable on Android linux platform.
 *
 **************/
static int getpidof(
    char* pProcName )
{
    char line[512];
    int pid = -1;
    int nRet = -1;
    FILE *cmd = NULL;

    memset( line, 0, 512);

    cmd = popen("ps", "r");
    if (!cmd)
    {
        printf("popen failed %d\n", errno);
        return -1;
    }

    /* ps displays output like this on Android :
       USER     PID   PPID  VSIZE  RSS     WCHAN    PC         NAME
       root     1     ?        
       root     2     ?        

       - First line is ignored, From each subsequent line
       - PID and CMD value is stored
       - If CMD matches with pProcName, pid is returned
    */

    /* Ignore the fist line from output */
    fgets( line, 512, cmd );

    /* Traverse each line in the output of "ps" */
    while (fgets(line, 512, cmd) != NULL)
    {
        /* Parse each line to store PID and CMD value */
        pid  = parse_ps_outputline(line, pProcName);
        if ( pid != -1)
        {
            /* we found the target process */
            break;
        }
    }
    pclose( cmd );
    return pid;
}

/*************
 *
 * Name: killSlqssdk
 *
 * Purpose:  kill slqssdk process
 *
 * Params:   none
 *
 * Return:   none
 *
 **************/
void killSlqssdk()
{
    int cPid = -1;
    char *cmd = NULL;
    // Need to kill slqssdk if running
    cPid = getpidof("/system/bin/slqssdk");
    if (cPid != -1) {
        asprintf(&cmd,"kill %d > /dev/null",cPid);
        system(cmd);
        free(cmd);
        cmd = NULL;
    }
}    

/*************
 *
 * Name: reStartSlqssdk
 *
 * Purpose:  restart slqssdk process
 *
 * Params:   none
 *
 * Return:   none
 *
 **************/
void reStartSlqssdk()
{
    ULONG nRet = 0;
    
    /* kill slqssdk process */
    killSlqssdk();
    
    /* wait until the slqssdk exit */
    while(getpidof("slqssdk") != -1)
        sleep(1);
        
    /* set absolute path */
    SetSDKImagePath("/system/bin/slqssdk");

    /* SLQSStart() could return an error if the SLQS daemon is restarted, so
       try again in this case.
    */
    while ( (nRet = SLQSStart()) != eQCWWAN_ERR_NONE) {
        LOGI ("SLQSStart failed.. %lx\n", nRet);
        sleep(1);
    }
}

/**************
 *
 * Name:     printUsage
 *
 * Purpose:  Print the usage information for this executable
 *           
 * Parms:    programname - name of the program
 *
 * Return:   None
 *
 * Notes:    This program can be invoked with the following options:
 *
 *           -h               Print usage information and exit cleanly
 *           -?               Print usage information and exit cleanly
 *           -V               Display version of the application. 
 *           -v               Verbose mode. 
 *           -g               Display the information for the executing
 *                            device image.
 *           -f               Force download, even if host firmware version/format
 *                            is old or different
 *           -d <imagepath>   Use <imagepath> to specific the CWE image path
 *                            to be download to the device.
 *                            Note that the path is just the directory
 *                            where the firmware resides.
 *           -i <imagepath>   Display the information for a particular CWE
 *                            image file located at <CWEimagepath>.
 *                            Note that the path is just the directory
 *                            where the firmware resides.
 *           -t <seconds>     wait <seconds> seconds) for firmware downloaing,
 *                            should be with option -d.
 * 
 *
 **************/
local void printUsage( char *programname )
{
    printf("\n\nUsage: %s:\n", programname );
    printf("  -h  --help                        Display this information and exit\n" );
    printf("  -?  --help                        Display this information and exit\n" );
    printf("  -V  --version                     Display version of the application\n" );
    printf("  -v  --verbose                     Display extra info while running\n");
    printf("                                    NOTE: specify 'verbose' first\n");
    printf("  -g  --get                         Display the information for \n");
    printf("                                    the executing device image\n");
    printf("  -f  --force                       Force download, even if host \n");
    printf("                                    firmware version/format is old\n");
    printf("                                    or different\n");
    printf("  -l  --listimages                  Display images information for 9x30 \n");
    printf("                                    located on device as well as host\n");
    printf("                                    NOTE: Use -p option as well for info of host\n");
    printf("  -d  --download <CWEimagepath>     CWE image to be downloaded is \n");
    printf("                                    at <CWEimagepath>\n");    
    printf("                                    NOTE: this must be an absolute path\n");
    printf("  -i  --info <CWEimagepath>         Display the information for \n");
    printf("                                    a particular CWE image file located\n");
    printf("                                    at <CWEimagepath>\n");
    printf("                                    NOTE: this must be an absolute path\n");
    printf("  -p  --pathhost <imagepath>        Path of 9x30 image located on host \n");
    printf("                                    Provide path as -p /data/EM7455 if firmware is located at /data/EM7455/1 \n");
    printf("                                    NOTE:(Not Mandotory option) It is used with -l and -s.\n");
    printf("                                    Use it with -s option also if used with -l in previous execution\n");
    printf("  -s  --selectimage <imageindex>    Select image for 9x30 with <image index> \n");
    printf("                                    image index is displayed by -l\n");
    printf("                                    NOTE: Use the -p option along with -s option with same path\n");
    printf("                                    if it was earlier used with -l \n");
    printf("  -t  --timeout <seconds>           wait <seconds> seconds for firmware\n");
    printf("                                    downloading, should be with -d option\n");
    printf("\n");
}

/*************
 * Name:     DevStateChgCbk
 *
 * Purpose:  Device State change callback
 *
 * Parms:    devstatus    - Call back status
 *
 * Return:   None
 *
 * Notes:    None
 **************/
void DevStateChgCbk(eDevState devstatus)
{
    LOGD("%s Device State %d", __func__, devstatus );
    /* If device is ready to communicate */
    if( devstatus ==  DEVICE_STATE_READY )
    {
        LOGI( "%s Device Ready", __func__ );

        /* Unsubscribe from the callback */
        SetDeviceStateChangeCbk(NULL);
        fwDeviceReady = TRUE;
    }
}

/*************
 * Name:     FirmwareDwlCbk
 *
 * Purpose:  Firmware download completion callback
 *
 *
 * Parms:    status    - Call back status
 *
 * Return:   None
 *
 * Notes:    None
 **************/
void FirmwareDwlCbk(ULONG status)
{
    if (eQCWWAN_ERR_NONE == status) 
    {
        LOGI("%s Firmware Download Completed", __func__);
    } 
    else if (eQCWWAN_ERR_SWIIM_FIRMWARE_NOT_DOWNLOADED == status) 
    {
        LOGI("%s Firmware already exist, no need to download again", __func__);
    }
    else if ( eQCWWAN_ERR_SWIIM_FW_FLASH_COMPLETE == status)
    {
        LOGI("%s Firmware Download Completed", __func__);
        LOGI("%s Rebooting the module\n",__func__ );
    }
    else
    {
        LOGE( "%s Firmware Not Downloaded, status: %lu", __func__, status);
        fwdwfail = TRUE;
    }

    /* Unsubscribe from the callback */
    SetFwDldCompletionCbk(NULL);

    /* set firmware complete to true */
    fwDwlComplete = TRUE;   
}



/*
 * Name:     cweFwDisplay
 *
 * Purpose:  CWE image info. display
 *
 * Return:   None
 *
 * Notes:    none
 */
static void cweFwDisplay(struct cwefwinfo *pfw)
{
    long rc;
    BYTE device_mode = DCS_DEVICE_MODE_DISCONNECTED;

    rc = SLQSGetDeviceMode ((BYTE *)&device_mode);
    if( eQCWWAN_ERR_NONE != rc ) {
        fprintf( stderr, "Device mode could not be retrieved,cwe information not fetched!\n");
        return;
    }

    /* QMI services are available only if the application is running */
    if ( device_mode == DCS_DEVICE_MODE_READY )
    {
        if( eQCWWAN_ERR_NONE ==
            GetFirmwareRevisions( (BYTE)sizeof(pfw->amss),
                                  pfw->amss,
                                  (BYTE)sizeof(pfw->boot),
                                  pfw->boot,
                                  (BYTE)sizeof(pfw->pri),
                                  pfw->pri ) )
        {
            fprintf( stderr,
                     "\nCWE Image Fields\n"\
                     "----------------\n"\
                     "BOOT Version: %s\nAMSS Version: %s\nPRI Revision: %s\n",
                     pfw->boot, pfw->amss, pfw->pri );
        }
        else
        {
            fprintf( stderr, "AMSS: %s\nBOOT: %s\nPRI: %s\n", "", "", "");
        }
    }
    else
    {
        fprintf( stderr, "Device was started up in boot mode, cwe information "\
                "is not available\n");
    }
}

/*
 * Name:     spkgDevFwDisplay
 *
 * Purpose:  SPKG CWE device image info. display
 *
 * Return:   None
 *
 * Notes:    none
 */
void spkgDevFwDisplay( struct qmifwinfo_s *pin )
{
    fprintf(    stderr,
                "\nSPKG CWE Device Image Fields\n"\
                "----------------------------\n"\
                "Model ID: %s\n"\
                "BOOT Version: %s\n"\
                "AMSS Version: %s\n"\
                "SKU ID: %s\n"\
                "Package ID: %s\n"
                "Carrier: %s\n"\
                "PRI Revision: %s\n",
                pin->dev.s.modelid_str,  pin->dev.s.bootversion_str,
                pin->dev.s.appversion_str, pin->dev.s.sku_str,
                pin->dev.s.packageid_str , pin->dev.s.carrier_str,
                pin->dev.s.priversion_str );
}

/*
 * Name:     spkgHostFwDisplay
 *
 * Purpose:  SPKG CWE host image info. display
 *
 * Return:   None
 *
 * Notes:    none
 */
void spkgHostFwDisplay( struct qmifwinfo_s *pin )
{
    fprintf(    stderr,
                "\nSPKG CWE Host Image Fields\n"\
                "--------------------------\n"\
                "Model ID: %s\n"\
                "BOOT Version: %s\n"\
                "AMSS Version: %s\n"\
                "SKU ID: %s\n"\
                "Package ID: %s\n"
                "Carrier: %s\n"\
                "PRI Version: %s\n",
                pin->dev.s.modelid_str,  pin->dev.s.bootversion_str,
                pin->dev.s.appversion_str, pin->dev.s.sku_str,
                pin->dev.s.packageid_str , pin->dev.s.carrier_str,
                pin->dev.s.priversion_str );
}


/*************
 * Name:     GetDeviceImageInfo
 *
 * Purpose:  Get the information about the image running on the device.
 *           
 * Parms:    None    
 *                       
 * Return:   None
 *
 * Notes:    None
 **************/
void GetDeviceImageInfo()
{
    struct qmifwinfo_s  spkg;
    struct cwefwinfo    cwe;
    ULONG               rc;

    /* Get the information about the image loaded on the device */
    rc = SLQSGetFirmwareInfo( &spkg );

    if( eQCWWAN_ERR_NONE != rc )
    {
        memset( &spkg.dev.s, 0, sizeof(spkg.dev.s) );
    }

    memset(&cwe, 0, sizeof(cwe));
    cweFwDisplay( &cwe );
    spkgDevFwDisplay( &spkg );
}

/*************
 * Name:     FirmwareDownloader
 *
 * Purpose:  Download a CWE image to the device
 *
 * Parms:    pathnamepp    - memory location to place pointer to the image 
 *           iTimeOut      - Seconds of waiting firmware download finished 
 *
 * Return:   None
 *
 * Notes:    None
 **************/
void FirmwareDownloader(char *pathnamep, int iTimeOut)
{    
    struct qmifwinfo_s  spkg;
    struct cwefwinfo    cwe;
    long                rclocal, rcfw, rc;
    BYTE                ctr = 0;
    int                 len;
    char                newpathp[MAXPATHSIZE] = "";
    ULONG               bForceDownload = 0x00;
    BYTE device_mode = DCS_DEVICE_MODE_DISCONNECTED;

    LOGD("%s Entered. with pathnamep: %s, iTimeOut: %d\n", __func__, pathnamep, iTimeOut);
    
    /* Reset the firmware download completion flag at the begining */
    fwDwlComplete = FALSE;
    fwdwfail = FALSE;

    fprintf( stderr, "\n-- Active Device Image Before Download --\n");

    rc = SLQSGetDeviceMode ((BYTE *)&device_mode);
    LOGD("%s Running with device in %s mode\n",
         __func__,
         DCS_DEVICE_MODE_DISCONNECTED == device_mode
         ?  "disconnected"
         :  DCS_DEVICE_MODE_READY == device_mode
         ? "application"
         : "boot and hold" );

    strcpy(newpathp, pathnamep);
    len = strlen(newpathp);
    if (newpathp[len - 1] != '/')
    {
        strcat(newpathp, "/");
    }

    rc = SLQSGetFirmwareInfo( &spkg );
    if( eQCWWAN_ERR_NONE != rc )
    {
        /* no AMSS SPKG support */
        memset( &spkg.dev.s, 0, sizeof(spkg.dev.s) );
    }
    memset(&cwe, 0, sizeof(cwe));
    cweFwDisplay( &cwe );
    spkgDevFwDisplay( &spkg );
    
    /* Get the information about the image located at specified path */
    rclocal = SLQSGetImageInfoMC77xx( newpathp, &spkg );

    /* Display the image information */
    if( eQCWWAN_ERR_NONE == rclocal )
    {
        fprintf( stderr,  "\n-- Preparing to Download SPKG CWE image --\n\n" );
        spkgHostFwDisplay( &spkg );
    }
    else
    {
        fprintf( stderr,  "\n-- Preparing to Download image --\n" );
    }
    LOGD("%s Vaild firmware image found",__func__);

    /* Reaching here means that a CWE image was found at specified path */
    /* Subscribe to Device State Change callback */
    rc = SetDeviceStateChangeCbk(DevStateChgCbk);
    LOGD("%s SetDeviceStateChangeCbk return: %lu\n", __func__, rc);
    if( SUCCESS != rc )
    {
        LOGE("%s REGISTRATION FAILED - Device State Change Callback, Failure Code: %lu", __func__, rc);
        return;
    }

    /* Subscribe to Firmware Download completion callback */
    rc = SetFwDldCompletionCbk(FirmwareDwlCbk);
    LOGD("%s SetFwDldCompletionCbk return: %lu\n", __func__, rc);
    if( SUCCESS != rc )
    {
        LOGE("%s REGISTRATION FAILED - Firmware Download Completion Callback, Failure Code: %lu", __func__, rc);
        return;
    }

    if (forceDld == TRUE)
        bForceDownload = 0x01;

    /* Start downloading the firmware */
    rc = UpgradeFirmware2k( newpathp, bForceDownload );
    LOGD("%s UpgradeFirmware2k return: %lu\n", __func__, rc);
    if( SUCCESS != rc )
    {
        LOGE("%s Firmware Download Failed, Failure Code: %lu", __func__, rc);
        return;
    }

    /* Keep displaying "." until fimrware downloads complete */
    fprintf( stderr, "Downloading Firmware ");
    if(iTimeOut == 0)
    {
        iTimeOut = DEFAUL_TTIME_OUT;
        LOGD("%s use default time out: %d\n", __func__, iTimeOut);
    }
    while( fwDwlComplete == FALSE)
    {
        if (iTimeOut <= 0)
            break;
        fprintf( stderr, " .");
        sleep(2);
        iTimeOut--;
    }
    
    if(iTimeOut == 0)
    {
        LOGD("%s download time out, return %d.\n", __func__, eQCWWAN_ERR_SWICM_TIMEOUT);
        fprintf( stderr,"firmware download time out%d\n", eQCWWAN_ERR_SWICM_TIMEOUT);
        return;
    }

    if (fwdwfail)
    {
        LOGD("%s Firmware Update Failed", __func__);
        fprintf( stderr, "Firmware Update Failed\n");
        return;
    }
    fprintf( stderr, "\nFirmware download complete; waiting for device...\n" );

    /* Give the firmware time to apply any additional CWE files that are
     * not firmware boot and/or application images
     */
    fprintf(stderr, "Applying updates - please wait 20 seconds...\n\n");
    sleep(20);

    /* Get the information about the image loaded on the device */
    rcfw = SLQSGetFirmwareInfo( &spkg );

    /* Display the information of the newly loaded image on the device */
    fprintf( stderr, "\n-- Active Device Image After Download --\n");

    if( eQCWWAN_ERR_NONE != rcfw )
    {
        /* no AMSS SPKG support */
        memset( &spkg.dev.s, 0, sizeof(spkg.dev.s) );
    }

    if (!fwDeviceReady) {
        while( ctr < MAX_WAIT_DEVICE_READY )
        {
            rc = SLQSGetDeviceMode ((BYTE *)&device_mode);
            if( device_mode != DCS_DEVICE_MODE_READY )
            {
                sleep(5);
                ctr++;
            }
            else
            {
                break;
            }
        }
    }

    cweFwDisplay( &cwe );
    spkgDevFwDisplay( &spkg );

    if( device_mode != DCS_DEVICE_MODE_READY )
    {
        qmiDeviceDisconnect();
        sleep(2);
        qmiDeviceConnect();
        rc = SLQSGetDeviceMode ((BYTE *)&device_mode);
        if (device_mode == DCS_DEVICE_MODE_BOOT_READY)
            fprintf( stderr, "\nPlease do a cold reboot of modem to make it ready\n" );
    }
    fprintf( stderr,  "\nFirmware Update Success\n" );
} 

/*************
 * Name:     GetHostImageInfo
 *
 * Purpose:  Get the information about the image located on host at a specified
 *           path.
 *
 * Parms:    pathnamepp    - memory location to place pointer to the image 
 *
 * Return:   None
 *
 * Notes:    none
 **************/
void GetHostImageInfo(char *pathnamep)
{
    struct  qmifwinfo_s spkg;
    struct slqsfwinfo_s slqsImageInfo;
    ULONG               resultCode = SUCCESS;
    char                newpathp[MAXPATHSIZE] = "";
    int                 len;
    
    LOGD("%s Entered. with pathnamep: %s\n", __func__, pathnamep);

    strcpy(newpathp, pathnamep);
    len = strlen(newpathp);
    if (newpathp[len - 1] != '/')
    {
        strcat(newpathp, "/");
    }

    /* Clear the structure */
    memset(&spkg, 0, sizeof(spkg) );
    memset(&slqsImageInfo, 0, sizeof(slqsImageInfo) );

    /* Get the information about the image located at specified path */
    resultCode = SLQSGetImageInfoMC77xx(newpathp, &spkg );

    if( eQCWWAN_ERR_NONE != resultCode )
    {
        resultCode = SLQSGetImageInfo_9x15( newpathp, IMG_PRI_TYPE_NVU, &slqsImageInfo );
        if( eQCWWAN_ERR_NONE == resultCode ) {
            fprintf( stderr,"\n\nNVU Image was found. \n");
            spkg.dev.s = slqsImageInfo;
        }
        else
             fprintf( stderr,"\n\nNo Valid Image was found. \n");              
    }
    else
    {
        fprintf( stderr,"\n\nCWE/NVU or SPK Image was found. \n");
    }

    /* Display the image information */
    spkgHostFwDisplay( &spkg );
}


/*
 * Name:     IsValidImageFileExist
 *
 * Purpose:  check whether the provided path is valid or not
 *
 * Return:   True if two images are the same, False otherwise.
 *
 * Notes:    none
 */
swi_bool IsValidImageFileExist(char *pImgPath)
{
    struct  qmifwinfo_s spkg;
    ULONG               resultCode = SUCCESS;
    char                newpathp[MAXPATHSIZE] = "";
    int                 len;
    
    LOGD("%s Entered. with pathnamep: %s\n", __func__, pImgPath);

    strcpy(newpathp, pImgPath);
    len = strlen(newpathp);
    if (newpathp[len - 1] != '/')
    {
        strcat(newpathp, "/");
    }

    /* Clear the structure */
    memset(&spkg, 0, sizeof(spkg) );

    /* Get the information about the image located at specified path */
    resultCode = SLQSGetImageInfoMC77xx(newpathp, &spkg );

    if( eQCWWAN_ERR_NONE != resultCode )
    {
        fprintf( stderr,
               "\nValid SPKG CWE file was not found. "\
               "Use -f option to download other format.\n");
        memset(&spkg.dev.s, 0, sizeof(spkg.dev.s) );
        if( TRUE == forceDld )
            return TRUE;
        else
            return FALSE;
    }
    return TRUE; 
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
 *           fDownload     - memory location to place download flag
 *           fGetDeviceImgInfor     - memory location to place get device image info flag
 *           iTimeOut      - memory time out for downloading firmware
 *
 * Return:   FALSE         - failed
 *           TRUE          - Succeed
 *
 * Notes:    None
 * 
 **************/
local swi_bool parseCommandLine(
    int argc, 
    char *argv[], 
    char **pathnamepp,
    swi_bool *fDownload,
    swi_bool *fGetDeviceImgInfor,
    int *iTimeOut,
    int *i9x30,
    int *pImageIndex,
    char **pathname9x30)
{
    int next_option;
    int optioncount = 0;
    char *programname;
    
    *pathnamepp    = NULL;

    /* Remember our own name */
    programname = argv[0]; 

    if(argc == 1)
    {
        fprintf( stderr, "\n\n No command found");
        printUsage(programname);
        exit( 0 );
    }        
    
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

            case '?':
                /* Print usage information */
                printUsage(programname);
                exit( 0 );
                break;

            case 'g':
                *fGetDeviceImgInfor = TRUE;
                break;

            case 'i':
                /* caller specifies a pathname to the CWE image to download */
                *pathnamepp = optarg;
                *fDownload = FALSE;
                break;

            case 'd':
                /* caller specifies a pathname to the CWE image to download */
                *pathnamepp = optarg;
                *fDownload = TRUE;
                break;

            case 'l':
                /* caller specifies to display images information for 9x30 */
                *i9x30 = TRUE;
                break;
 
            case 'p':
                /* caller specifies the path of 9x30 image located on Host */
                *pathname9x30 = optarg;
                break;

            case 's':
                /* caller specifies the path of 9x30 image located on Host */
                *pImageIndex = atoi(optarg);
                break;
 
            case 't':
                /* caller specifies time out value */
                *iTimeOut = atoi(optarg);
                break;

            case 'v':
                /* Verbose mode on */
                verbose = TRUE;
                break;

            case 'V':
                printf("Version: %s\n", VERSION);
                exit( 0 );
                break;

            case 'f':
                /* Force download */
                forceDld = TRUE;
                break;

            case -1:
                /* Done with options list */
                if( verbose )
                {
                    printf("%d command line options found\n", optioncount );
                    printf("verbose:     ON\n");
                    printf("imgpathname: %s\n", *pathnamepp );
                    printf("*fDownload: %d\n", *fDownload );
                    printf("*fGetDeviceImgInfor: %d\n", *fGetDeviceImgInfor );
                    printf("*iTimeOut: %d\n", *iTimeOut );
                    printf("*i9x30: %d\n", *i9x30);
                    printf("9x30 host imgpath: %s\n", *pathname9x30 );
                    printf("9x30 select image with index: %d\n", *pImageIndex );                    
                }

                break;

            default:
                return(FALSE);
                break;
        }
        optioncount++;
    }
    while( next_option != -1 );
    
    return(TRUE);
}

/*
 * Name:     FlushStdinStream
 *
 * Purpose:  Flush the stdin stream
 *
 * Return:   None
 *
 * Notes:    fflush does not work for input stream.
 */
void FlushStdinStream( )
{
    int inputChar;

    /* keep on reading until an <New Line> or end of file is received */
    do
    {
        inputChar = getchar();

#ifdef MC7xxxDBG
    fprintf( stderr,  "inputChar: 0x%x\n", inputChar );
#endif
    }
    while ( ( inputChar != ENTER_KEY ) &&
            ( inputChar != EOF ) );
}

/*
 * Name:     ImageInfoDisplay
 *
 * Purpose:  image info. display
 *
 * Return:   None
 *
 * Notes:    none
 */
void ImageInfoDisplay( ULONG *itemsCount, struct SWI_STRUCT_CarrierImage *CarrierImages)
{
    ULONG index =0 ;
    fprintf(stderr,
                   "\nAll Carrier Images on Host and Device \n-------------------------------------\nIndex   CarrierId   FolderId   Storage Type   "\
                   "PriImageId     PriBuildId              FwImageId     FwBuildId"\
                   "\n");

    while(index < *itemsCount)
    {
         fprintf(stderr,
                       "%-8ld"\
                       "%-12ld"\
                       "%-11ld"\
                       "%-15ld"\
                       "%-15s"\
                       "%-24s"\
                       "%-14s"\
                       "%-14s\n",
                       index+1,
                       CarrierImages[index].m_nCarrierId,
                       CarrierImages[index].m_nFolderId,
                       CarrierImages[index].m_nStorage,
                       CarrierImages[index].m_PriImageId,
                       CarrierImages[index].m_PriBuildId,
                       CarrierImages[index].m_FwImageId,
                       CarrierImages[index].m_FwBuildId);
        index++;
    }
}


/*
 * Name:     FirmwareDwldCbk
 *
 * Purpose:  Firmware download completion callback
 *
 * Return:   None
 *
 * Notes:    none
 */
void FirmwareDwldCbk(ULONG status)
{
    if (0 == status)
    {
        fprintf( stderr, "\nFirmware download complete; waiting for device...\n" );
    }
    else if (eQCWWAN_ERR_SWIIM_FIRMWARE_NOT_DOWNLOADED == status)
    {
        fprintf( stderr, "\nNo Firmware download needed!\n" );
    }
    else
    {
        fwdwfail = TRUE;
    }

    /* Unsubscribe from the callback */
    SetFwDldCompletionCbk(NULL);
    fwDwlComplete = TRUE;
}

/*
 * Name:     SWIGet9x30DevImgInfo
 *
 * Purpose:  MDM9x30-based devices are capable of storing multiple
 *           carrier images on device.To display information about
 *           all images located on both host and device
 *
 * Return:   None
 *
 * Notes:    none
 */
void SWIGet9x30DevImgInfo(CHAR *eimagePath)
{
    ULONG  nRet=0, rc;
    ULONG  itemsCount =255;
    CHAR   imagePath[MAX_IMAGE_PATH];
    CHAR    newImagePath[MAX_IMAGE_PATH];
    CHAR    folderName[16];
    unsigned int  len;
    struct SWI_STRUCT_CarrierImage CarrierImages[20];
    struct slqsfwinfo_s slqsImageInfo;
    struct qmifwinfo_s  spkg;
    BYTE device_mode = DCS_DEVICE_MODE_DISCONNECTED;

    rc = SLQSGetDeviceMode ((BYTE *)&device_mode);
    LOGD("%s Running with device in %s mode\n",
         __func__,
         DCS_DEVICE_MODE_DISCONNECTED == device_mode
         ?  "disconnected"
         :  DCS_DEVICE_MODE_READY == device_mode
         ? "application"
         : "boot and hold" );

    if (eimagePath != NULL) {
        strcpy(imagePath, eimagePath);
        len = strlen(imagePath);
        if (imagePath[len - 1] != '/')
        {
            strcat(imagePath, "/");
        }
    }

    memset(&CarrierImages,0 , sizeof(struct SWI_STRUCT_CarrierImage));
    fprintf(stderr, "\n\n-- Active Device Image --\n");
    GetDeviceImageInfo();

    if (eimagePath != NULL) {        
        /* Get the information about the all carrier images located at specified path on Host and device */
        nRet = SLQSSwiGetAllCarrierImages(&itemsCount, CarrierImages, imagePath );
    }
    else {
        nRet = SLQSSwiGetAllCarrierImages(&itemsCount, CarrierImages, NULL );
    }      

    if( eQCWWAN_ERR_NONE != nRet )
    {
       fprintf( stderr,
               "\nValid NVU file was not found. "\
               "No fields are printed for non-NVU files. \n"\
               "Please specify valid path from host\n"  );
       return;
    }
    /* Display the image information */
    ImageInfoDisplay( &itemsCount, CarrierImages );
    return;
}


/*
 * Name:     SWI9x30ImageSwitching
 *
 * Purpose:  MDM9x30-based devices are capable of storing multiple
 *           carrier images on device.To switch image on device or from host.
 *
 *
 * Return:   None
 *
 * Notes:    none
 */
void SWI9x30ImageSwitching(CHAR *eimagePath, int imageIndex)
{
    ULONG  nRet=0, rc;
    ULONG  itemsCount =255;
    CHAR   imagePath[MAX_IMAGE_PATH];
    CHAR    newImagePath[MAX_IMAGE_PATH];
    CHAR    folderName[16];
    ULONG  bForceDownload = 0x00;
    unsigned int  len;
    struct SWI_STRUCT_CarrierImage CarrierImages[20];
    struct slqsfwinfo_s slqsImageInfo;
    struct qmifwinfo_s  spkg;
    BYTE device_mode = DCS_DEVICE_MODE_DISCONNECTED;

    rc = SLQSGetDeviceMode ((BYTE *)&device_mode);
    LOGD("%s Running with device in %s mode\n",
         __func__,
         DCS_DEVICE_MODE_DISCONNECTED == device_mode
         ?  "disconnected"
         :  DCS_DEVICE_MODE_READY == device_mode
         ? "application"
         : "boot and hold" );

    if (eimagePath != NULL)
        strcpy(imagePath, eimagePath);
 
    memset(&CarrierImages,0 , sizeof(struct SWI_STRUCT_CarrierImage));
    /* Reset the firmware download completion and device ready flags */
    fwDwlComplete = FALSE;        
    fwdwfail = FALSE;
        
    if (eimagePath != NULL) {
        len = strlen(imagePath);
        if (imagePath[len - 1] != '/')
        {
            strcat(imagePath, "/");
        }
    }

    fprintf(    stderr, "\n\n-- Active Device Image --\n");
    GetDeviceImageInfo();

    if (eimagePath != NULL) {        
        /* Get the information about the all carrier images located at specified path on Host and device */
        nRet = SLQSSwiGetAllCarrierImages(&itemsCount, CarrierImages, imagePath );
    }
    else {
        nRet = SLQSSwiGetAllCarrierImages(&itemsCount, CarrierImages, NULL );
    }
       

    if( eQCWWAN_ERR_NONE != nRet )
    {
        fprintf( stderr,
                 "\nValid NVU file was not found. "\
                 "No fields are printed for non-NVU files.\n" );
        return;
    }
    /* Display the image information */
    ImageInfoDisplay( &itemsCount, CarrierImages );
        
    if (eimagePath != NULL) {
        strcpy(newImagePath,imagePath);
        sprintf(folderName,"%ld",CarrierImages[imageIndex -1].m_nFolderId);
        strcat(newImagePath,folderName);
        strcat(newImagePath,"/");
    }
    else {
        sprintf(folderName,"%ld",CarrierImages[imageIndex -1].m_nFolderId);
        strcpy(newImagePath,folderName);
        strcat(newImagePath,"/");
    }   

    /*Warning to check if the image type is of SPK format*/
    if(eQCWWAN_ERR_NONE == SLQSGetImageInfo_9x15( newImagePath,
                                         (BYTE)IMG_FW_TYPE_SPK,
                                          &slqsImageInfo ))
    {
        if (forceDld == FALSE) {        
            fprintf( stderr, "\nThis is SPK Image. Please note that 9x30 image"
                   "switching method will not be applied for spk file."
                   "please use -f option as well if you still want to download\n");
            return;
        }
    }
    
    if ( device_mode == DCS_DEVICE_MODE_DISCONNECTED )
    {
        fprintf( stderr, "Firmware upgrade is not supported while the "\
                 "device is disconnected\n" );
    }
    else
    {
         nRet = SetFwDldCompletionCbk( FirmwareDwldCbk );
         if( SUCCESS != nRet )
         {
            fprintf( stderr, "REGISTRATION FAILED - Firmware Download Completion Callback\n"\
                     "Failure Code: %ld\n", nRet );
            return ;
         }

         if (forceDld == TRUE)
             bForceDownload = 0x01;

         /* Start downloading the firmware */
         nRet = UpgradeFirmware2k( newImagePath, bForceDownload );
         if( eQCWWAN_ERR_NONE != nRet )
         {
             /* some firmware of MC7700 may reboot immediately after receiving
              * set firmware id command without sending out respond,
              * neglect this error to bypass issue*/
             if(eQCWWAN_ERR_NO_DEVICE !=nRet )
             {
                 fprintf( stderr, "Firmware Download Failed\n"\
                          "Failure Code: %ld\n", nRet );
                 return ;
             }
             return ;
         }
         fprintf( stderr, "\n\nDownloading Firmware");
         while( !fwDwlComplete )
         {
             /* Display "." while firmware downloads */
             fprintf( stderr, ".");
             sleep(2);
         }
         if (fwdwfail)
         {
             fprintf( stderr, "Firmware Download Failed\n");
             return;
         }

         /* Give the firmware time to apply any additional CWE files that are
          * not firmware boot and/or application images
          */
         fprintf(stderr, "Applying SPKG updates - please wait 20 seconds...\n\n");
         sleep(20);
    }
}

/*************
 * Name:     main
 *
 * Purpose:  Entry point of the application
 *
 * Return:   None
 *
 * Notes:    None
 **************/
int main( int argc, char *argv[] )
{
    ULONG    resultCode = 0;
    char     selOption[OPTION_LEN];
    char     *pathnamep = NULL;
    swi_bool fDownload = FALSE;
    swi_bool fGetDeviceImgInfor = FALSE;
    int      iTimeOut = 0;
    int      i9x30 = FALSE;
    int      imageIndex = 0;
    char     *pathName9x30 = NULL;

    /* Parse the command line  */
    if(!parseCommandLine(argc, argv, &pathnamep, &fDownload, &fGetDeviceImgInfor, 
       &iTimeOut, &i9x30, &imageIndex, &pathName9x30))
    {
        exit(1);
    }

    /* Either firmware download or firmware info should be specified */
    if((pathnamep == NULL) && (fGetDeviceImgInfor == FALSE) && (i9x30 == FALSE) && (imageIndex == 0))
    {
        fprintf( stderr, "\n\nInvalid command combination");
        printUsage(argv[0]);
        exit( 0 );
    }

    /* time out option should be with download option */
    if(iTimeOut != 0 && fDownload == FALSE)
    {
        fprintf( stderr, "\n\nInvalid command combination");
        printUsage(argv[0]);
        exit( 0 );
    }

    /* Either 9x30 or other MC image download/display */ 
    if ((fDownload || fGetDeviceImgInfor) && ((imageIndex != 0) || i9x30))
    {
        fprintf( stderr, "\n\nInvalid command combination");
        printUsage(argv[0]);
        exit( 0 );
    }   

    reStartSlqssdk();
   
    qmiDeviceConnect();
    if(fGetDeviceImgInfor)
    {
        GetDeviceImageInfo();
    }
    else if (i9x30 == TRUE)
    {
        SWIGet9x30DevImgInfo(pathName9x30);
    }
    else if (imageIndex != 0)
    {
        SWI9x30ImageSwitching(pathName9x30, imageIndex);
    }
    else if(pathnamep!= NULL)
    {
        if(fDownload == TRUE) { 
            if(IsValidImageFileExist(pathnamep))
                FirmwareDownloader(pathnamep, iTimeOut);
        }
        else
            GetHostImageInfo(pathnamep);
    }
    qmiDeviceDisconnect();
 
    sleep(2);
    
    killSlqssdk();
    
    exit(EXIT_SUCCESS);
}
