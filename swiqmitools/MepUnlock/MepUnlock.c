/*************
 *
 * Filename:    MepUnlock.c
 *
 * Purpose:     MEP check and unlock application
 *
 * Copyright: © 2016 Sierra Wireless Inc., all rights reserved
 *
 **************/
#include "SWIWWANCMAPI.h"
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>

#define  LOG_TAG  "swi_mepunlock"
#include "swiril_log.h"
#include "im/imudefs.h"
#include "qaGobiApiUim.h"
#include "qm/qmerrno.h"
#include <swiqmitool_common.h>

/****************************************************************
*                       DEFINES
****************************************************************/
#define MAX_PASS_SIZE 256
/****************************************************************
*                       DATA STRUCTURES
****************************************************************/

/* firmware download */
static swi_bool verbose = FALSE;

/* Version infor*/
const char * const VERSION = "1.0.1603.0";
/* Command Line Options */
const char * const short_options = "h?vViup:";

const struct option long_options[] = {
    {"help",        0, NULL, 'h'},  /* Provides terse help to users */
    {"help",        0, NULL, '?'},  /* Provides terse help to users */
    {"version",     0, NULL, 'V'},  /* Application version infor */
    {"verbose",     0, NULL, 'v'},  /* Run in Verbose mode */
    {"info",        0, NULL, 'i'},  /* Display the MEP status on modem */
    {"unlock",      0, NULL, 'u'},  /* MEP unlock */
    {"password",    1, NULL, 'p'},  /* MEP unlock password */
    {NULL,          0, NULL,  0  }  /* End of list */
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
 *           -h               Print usage information and exit cleanly.
 *           -?               Print usage information and exit cleanly.
 *           -V               Display version of the application. 
 *           -v               Verbose mode. 
 *           -i               Display the modem MEP status.
 *           -u               unlock the modem, should be with option -p.
 *           -p <password>    MEP password, should be with option -u.
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
    printf("  -i  --info                        Display the MEP status for \n");
    printf("                                    	the modem\n");
    printf("  -u  --unlock                      MEP unlock \n");
    printf("                                    	should be used with -p option\n");
    printf("  -p  --password <password>         MEP unlock password \n");
    printf("                                    	should be used with -u option\n");
    printf("\n");
}

/*************
 * Name:     GetMEPStatus
 *
 * Purpose:  Get the information about MEP status on the device.
 *           
 * Parms:    fMepLocked    - Whether modem was MEP unlocked or not
 *                       
 * Return:   FALSE         - Failed
 *           TRUE          - Succeed
 *
 * Notes:    None
 **************/
swi_bool GetMEPStatus(swi_bool * fMepLocked)
{
    UIMGetConfigurationReq  req;
    UIMGetConfigurationResp resp;
    
    BYTE AutoSelection = 0xFF;
    BYTE HaltSubscription = 0xFF;
    personalizationStatus personalizationStatus;

    memset(&personalizationStatus, 0, sizeof(personalizationStatus));
    memset(&req, 0, sizeof(UIMGetConfigurationReq));
    memset(&resp, 0, sizeof(UIMGetConfigurationResp));
    
    int i = 0;
    for(i=0; i<MAX_ACTIVE_PERS_FEATURES; i++)
        personalizationStatus.feature[i] = 0xFF;

    /* Initilize parmaters */
    req.pConfigurationMask        = NULL;

    resp.pAutoSelection           = &AutoSelection;
    resp.pPersonalizationStatus   = &personalizationStatus;
    resp.pHaltSubscription        = &HaltSubscription;

    ULONG nRet =SLQSUIMGetConfiguration(&req, &resp);
    if(nRet != 0)
    {
        LOGE("%s Modem MEP status unknown: nRet = %lu.", __func__, nRet);
        fprintf(stderr,"\nModem MEP status unknown: %lu.\n", nRet);
    	return FALSE;
    }

    if(verbose)
    {
        LOGI("%s Automatic selection: 0x%X", __func__, AutoSelection);
        fprintf(stderr,"\nAutomatic selection: 0x%X\n", AutoSelection);
        
        LOGI("%s feature:", __func__);
        fprintf(stderr,"\nfeature: ");
        for(i=0; i<MAX_ACTIVE_PERS_FEATURES; i++)
        {
            fprintf(stderr,"0x%X ", personalizationStatus.feature[i]);
            LOGI( "%s: 0x%X ",  __func__, personalizationStatus.feature[i]);
        }
        fprintf(stderr,"\n");

        LOGI( "%s Number of features: %d", __func__, personalizationStatus.numFeatures);
        fprintf(stderr,"\nNumber of features: %d\n", personalizationStatus.numFeatures);

        for(i = 0; i < personalizationStatus.numFeatures; i++)
        {
            LOGI( "%s feature[%d] = 0x%X", __func__, i, personalizationStatus.feature[i]);
            fprintf(stderr,"\nfeature[%d] = 0x%X\n", i, personalizationStatus.feature[i]);
            LOGI( "%s verifyLeft[%d] = 0x%X", __func__, i, personalizationStatus.verifyLeft[i]);
            fprintf(stderr,"\verifyLeft[%d] = 0x%X\n", i, personalizationStatus.verifyLeft[i]);
            LOGI( "%s unblockLeft[%d] = 0x%X", __func__, i, personalizationStatus.unblockLeft[i]);
            fprintf(stderr,"\nunblockLeft[%d] = 0x%X\n", i, personalizationStatus.unblockLeft[i]);
        }
        
        LOGI( "%s HaltSubscription: 0x%X", __func__, HaltSubscription);
        fprintf(stderr,"\nHaltSubscription: 0x%X\n", HaltSubscription);
    }
 
    /* Check whether modem is in MEP unlock status */
    if (personalizationStatus.numFeatures != 1 && personalizationStatus.feature[0] != 0)
    {
        if(verbose)
        {
            LOGI( "%s Modem already unlocked!", __func__);
            fprintf(stderr,"\nModem already unlocked!\n");
        }
        *fMepLocked = FALSE;
    	return TRUE;
    }
    else
    {
        if(verbose)
        {
            LOGI( "%s Modem is locked!", __func__);
            fprintf(stderr,"\nModem is locked!\n");
        }
        *fMepLocked = TRUE;
    }
    return TRUE;
}

/*************
 * Name:     MepUnlock
 *
 * Purpose:  Unlock the modem if it is MEP locked
 *
 * Parms:    MepUnlocPassword   - MEP unlock password 
 *
 * Return:   FALSE         	- Failed
 *           TRUE          	- Succeed
 *
 * Notes:    None
 **************/
local swi_bool MepUnlock(char *MepUnlocPassword)
{    

    LOGD("%s Entered. with password: %s\n", __func__, MepUnlocPassword);
    if(verbose)
    {
        fprintf(stderr,"%s Entered. with password: %s\n", __func__, MepUnlocPassword);
    }
    
    swi_bool fMepLocked = FALSE;
    if(!GetMEPStatus(&fMepLocked))
    {
        fprintf( stderr, "\nGetMEPStatus failed");
        LOGE( "%s nGetMEPStatus failed!", __func__);
        return FALSE;
    }

    if(fMepLocked == FALSE)
    {
        fprintf( stderr, "\nModem already unlocked!");
        LOGD( "%s Modem already unlocked!", __func__);
        return FALSE;
    }

    UIMDepersonalizationReq  req;
    UIMDepersonalizationResp resp;
    remainingRetries         RemainingRetrys;

    memset(&req, 0, sizeof(UIMDepersonalizationReq));
    memset(&resp, 0, sizeof(UIMDepersonalizationResp));
    memset(&RemainingRetrys, 0, sizeof(remainingRetries));

    req.depersonilisationInfo.feature = 0;
    req.depersonilisationInfo.operation = 0;
    req.depersonilisationInfo.ckLen = (BYTE)strlen(MepUnlocPassword);
    
    strcpy((char *)&req.depersonilisationInfo.ckVal[0], MepUnlocPassword);
    if(verbose)
    {
        fprintf( stderr, "\npass:%s", req.depersonilisationInfo.ckVal);
        LOGI( "%s pass: %s!", __func__, req.depersonilisationInfo.ckVal);
    }

    RemainingRetrys.verifyLeft = 0;
    RemainingRetrys.unblockLeft = 0;
    resp.pRemainingRetries = &RemainingRetrys;
 
    if(verbose)
    {
        LOGI( "%s feature: 0x%X", __func__, req.depersonilisationInfo.feature);
        fprintf(stderr,"feature: 0x%X\n", req.depersonilisationInfo.feature);
        LOGI( "%s operation: 0x%X", __func__, req.depersonilisationInfo.operation);
        fprintf(stderr,"operation: 0x%X\n", req.depersonilisationInfo.operation);
        LOGI( "%s Control Key Length: 0x%X", __func__, req.depersonilisationInfo.ckLen);
        fprintf(stderr,"Control Key Length: 0x%X\n", req.depersonilisationInfo.ckLen);
        LOGI( "%s Control Key Value: %s", __func__, req.depersonilisationInfo.ckVal);
        fprintf(stderr,"Control Key Value: %s\n", req.depersonilisationInfo.ckVal);
    }

    ULONG nRet = SLQSUIMDepersonalization( &req, &resp );
    if(nRet == 0)
    {
        fprintf( stderr, "\nMEP unlock succeed!");
        LOGI( "%s MEP unlock succeed!", __func__);
    }
    else
    {
        fprintf( stderr, "\nError: %lu, Verify left: %d, Unblock left: %d", 
            nRet, RemainingRetrys.verifyLeft, RemainingRetrys.unblockLeft);
        LOGE( "%s Error: %lu, Verify left: %d, Unblock left: %d", __func__,
            nRet, (int)RemainingRetrys.verifyLeft, (int)RemainingRetrys.unblockLeft);
    }
    return TRUE;
} 

/**************
 *
 * Name:     parseCommandLine
 *
 * Purpose:  This function parses the caller's inputed parameters.
 *           
 * Parms:    argc              - argument count
 *           argv              - argument vector
 *           fGetMepStatus     - memory location to place get MEP status flag
 *           fMepUnlock        - memory location to place MEP unlock flag
 *           MepUnlocPassword  - memory location to place pointer to MEP unlock password 
 *
 * Return:   FALSE             - Failed
 *           TRUE              - Succeed
 *
 * Notes:    None
 * 
 **************/
swi_bool parseCommandLine(
    int argc, 
    char *argv[], 
    swi_bool *fGetMepStatus,
    swi_bool *fMepUnlock,
    char **MepUnlocPassword)
{
    int next_option;
    int optioncount = 0;
    char *programname;
    
    *MepUnlocPassword    = NULL;

    /* Remember our own name */
    programname = argv[0]; 

    if(argc == 1)
    {
        fprintf( stderr, "\n\n No command found");
        printUsage(programname);
        return FALSE;
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

            case 'i':
                *fGetMepStatus = TRUE;
                break;

            case 'u':
                *fMepUnlock = TRUE;
                break;

            case 'p':
                *MepUnlocPassword = optarg;
                break;

            case 'v':
                /* Verbose mode on */
                verbose = TRUE;
                break;

            case 'V':
                printf("Version: %s\n", VERSION);
                exit( 0 );
                break;

            case -1:
                /* Done with options list */
                if( verbose )
                {
                    printf("%d command line options found\n", optioncount );
                    printf("verbose:     ON\n");
                    printf("*fGetMepStatus: %d\n", *fGetMepStatus );
                    printf("*fMepUnlock: %d\n", *fMepUnlock );
                    printf("MepUnlocPassword: %s\n", *MepUnlocPassword );
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

/*************
 * Name:     main
 *
 * Purpose:  Entry point of the application
 *
 * Parms:    argc  - argument count
 *           argv  - argument vector
 *
 * Return:   exit code
 *
 * Notes:    None
 **************/
int main(int argc, char *argv[])
{
    swi_bool fGetMEPStatus = FALSE;
    swi_bool fMepUnlock = FALSE;
    swi_bool fMepLocked = FALSE;
    char     *MepUnlocPassword = NULL;

    /* Parse the command line  */
    if(!parseCommandLine(argc, argv, &fGetMEPStatus, &fMepUnlock, &MepUnlocPassword))
    {
        fprintf( stderr, "\n\nparseCommandLine failed");
        printUsage(argv[0]);
        exit(1);
    }

    /* Check input option combination */
    if(((MepUnlocPassword == NULL) && (fMepUnlock == TRUE)) || 
        ((fGetMEPStatus == TRUE) && (fMepUnlock == TRUE)) ||
        ((fGetMEPStatus == TRUE) && (MepUnlocPassword != NULL)))
    {
        fprintf( stderr, "\n\nInvalid command combination");
        printUsage(argv[0]);
        exit( 0 );
    }

    reStartSlqssdk();
   
    qmiDeviceConnect();
    if(fGetMEPStatus)
    {
        if(!GetMEPStatus(&fMepLocked))
        {
            fprintf( stderr, "\nGetMEPStatus failed");
            LOGE( "%s GetMEPStatus failed!", __func__);
            exit( 0 );
        }
        else
        {
            if(fMepLocked)
            {
                LOGI( "%s Modem is MEP locked!", __func__);
                fprintf(stderr,"\nModem is MEP locked!\n");
            }
            else
            {
                LOGI( "%s Modem is MEP unlocked!", __func__);
                fprintf(stderr,"\nModem is MEP unlocked!\n");
            }
        }
    }
    else if(fMepUnlock)
    {
        if(MepUnlocPassword != NULL)
        {
            if(!MepUnlock(MepUnlocPassword))
            {
                fprintf( stderr, "\nMepUnlock failed!");
                LOGE( "%s MepUnlock failed!", __func__);
            }
        }
    }

    qmiDeviceDisconnect();

    sleep(2);
    
    killSlqssdk();
    
    exit(0);
}
