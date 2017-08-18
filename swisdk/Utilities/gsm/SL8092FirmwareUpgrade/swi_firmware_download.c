/**
 *
 * @ingroup swi_firmware_download.c
 *
 * @file
 * - Firmware download from 1x to 3x for SL8092
 * - It will spawn swifwdnld application
 *
 * @author
 * Copyright: © 2015 Sierra Wireless, Inc.
 *            All rights reserved
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
#include <fcntl.h>
#include <termios.h>
#include <stdbool.h>
#include <errno.h>
#include <dirent.h>

#include "swi_at_channel.h"
#include "swi_at_tok.h"

#define SDKDEFAULT        "/system/bin/swisdk"
#define SWIFWDNLDDEFAULT  "/system/bin/swifwdnld"
#define LOOPCNT_DEFAULT   100


/* Called on command thread. */
static void onATTimeout()
{
    printf("AT channel timeout; restarting..\n");

    /* Last resort, throw escape on the line, close the channel
       and hope for the best. */
    at_send_escape();
}

/** 
 * Common initialization for all AT channels.
 *
 * Note that most of the AT unsolicited result code reporting is only turned
 * on once the SIM is ready, in onSIMReady().
 */
static char initializeATChannel(void)
{
    int err = 0;

    if (at_handshake() < 0) {
        printf("Handshake failed!\n");
        goto error;
    }

    /* Configure/set
     *   command echo (E), result code suppression (Q), DCE response format (V)
     *
     *  E0 = DCE does not echo characters during command state and online
     *       command state
     *  Q0 = DCE transmits result codes
     *  V1 = Display verbose result codes
     */
    err = at_send_command("ATE0Q0V1", NULL);
    if (err < 0)
        goto error;

    return 0;
error:
    return 1;
}

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
/******************
 *
 * Name: verify_ttyports()
 *
 * Purpose:  verify that ttyUSB0 to 4 exists
 *
 * Params:   none
 *
 * Return:   int
 *
 ***************/
static int verify_ttyports() 
{
    struct dirent **namelist;
    int n;
    int mask = 0;

    n = scandir("/dev", &namelist, NULL, NULL);
    if (n < 0) {
        printf("scandir error \n");
    }        
    else {
        while (n--) {
            if (!strcmp(namelist[n]->d_name,"ttyUSB0"))
                mask = mask | (1 << 0);
            else if(!strcmp(namelist[n]->d_name,"ttyUSB1"))
                mask = mask | (1 << 1);
            else if(!strcmp(namelist[n]->d_name,"ttyUSB2"))
                mask = mask | (1 << 2);
            else if(!strcmp(namelist[n]->d_name,"ttyUSB3"))
                mask = mask | (1 << 3);
            else if(!strcmp(namelist[n]->d_name,"ttyUSB4"))
                mask = mask | (1 << 4);
            free(namelist[n]);
        }
    free(namelist);
    }
    return mask;
}


/******************
 *
 * Name: startstopRilDaemon_Slqssdk
 *
 * Purpose:  start-stop RILdaemon and slqssdk
 *
 * Params:   bool
 *
 * Return:   none
 *
 ***************/
static void startstopRilDaemon_Slqssdk(bool start)
{
    int ret;
    int cPid = -1;
    char *cmd = NULL;

    if (start) {
        ret = system("start ril-daemon-8092");
        if (ret == -1)
            printf("ril-daemon not started\n");
        ret = system("start ril-daemon");
        if (ret == -1)
            printf("ril-daemon not started\n");
    }
    else {
        ret = system("stop ril-daemon-8092");
        if (ret == -1)
            printf("ril-daemon not stopped\n");
        ret = system("stop ril-daemon");
        if (ret == -1)
            printf("ril-daemon not stopped\n");

        // Need to kill slqssdk if running
        cPid = getpidof("/system/bin/slqssdk");
        if (cPid != -1) {
            printf("slqssdk pid = %d\n",cPid);
            asprintf(&cmd,"kill %d > /dev/null",cPid);
            system(cmd);
            free(cmd);
            cmd = NULL;
        }
    }
}

/******************
 *
 * Name: openAtPort_to_ModifyPid
 *
 * Purpose:  Open AT port and modify PID
 *
 * Params:   None
 *
 * Return:   -1 on failure , 0 on success
 *
 ***************/
static int openAtPort_to_ModifyPid()
{
    int fd = -1;
    int retry_1 = 0, retry_2 = 0, retry_3 = 0;
    ATResponse *atresponse = NULL;
    int err = 0, ret = 0;
    char *line = NULL;
    int cPid = -1;
    bool bRestorePid = false;
    const int sPid = 0x68A3;
    char *cmd = NULL;
    struct termios ios;

    /* Open AT port of SL8092 to execute AT commands*/
    for (;;) {
        fd = -1;        
        if (fd < 0) {
            if (verify_ttyports() == 0x1F)
                fd = open ("/dev/ttyUSB3",O_RDWR);
            else if (verify_ttyports() == 0x0F)
                fd = open ("/dev/ttyUSB2",O_RDWR);
            else {
                printf("tty port not found to execute AT\n");
                goto exit;
            }
            if (fd < 0 && (retry_1 > 3)){
                printf("Opening USB port to execute AT commands failed!\n");
                goto exit;
            }
            else if (fd < 0){
                printf("Retrying after 2 seconds to open port!\n");
                retry_1++;
                sleep(2);
                continue;
            }

            tcgetattr(fd, &ios);
            cfmakeraw(&ios);
            cfsetospeed(&ios, B115200);
            cfsetispeed(&ios, B115200);
            ios.c_cflag |= CREAD | CLOCAL;
            tcflush(fd, TCIOFLUSH);
            tcsetattr(fd, TCSANOW, &ios);
            retry_1 = 0;
            
            printf("AT port opened successfully!\n");
        }
        /* Open the AT channel */
        ret = at_open(fd, NULL);
        if (ret < 0 && retry_2 <= 3) {
            printf("AT error %d on at_open, retrying in 1 second!\n", ret);
            retry_2++;
            at_close();
            sleep(1);
            continue;
        }
        else if (ret < 0) {
            printf("at_open failed! exiting\n");
            /* close port and exit */
            at_close();
            goto exit;
        }
        
        retry_2 = 0;
        at_set_on_timeout(onATTimeout);
        printf("Initializing AT channel\n");
        ret = initializeATChannel();
        if (ret && retry_3 <= 3) {
            printf("AT channel initialization failed, retrying in 1 second!\n");
            retry_3++;
            at_close();
            sleep(1);
            continue;
        }
        else if (ret) {
            printf("AT channel initialzation failed! exiting!\n");
            /* close port and exit */
            at_close();
            goto exit;
        }
        printf("AT channel initialization success!\n");
        at_make_default_channel();
        at_set_timeout_msec(1000 * 30);
        break;        
    }    
    err = at_send_command("AT!ENTERCND=\"A710\"", &atresponse);
    if (err < 0 || atresponse->success == 0)
        printf("Error by AT!ENTERCND command!\n");
    at_response_free(atresponse);
    atresponse = NULL; 

    /* Fetch PID */
    err = at_send_command_singleline("AT!UDPID?","!UDPID:",&atresponse);
    if (err < 0 || atresponse->success == 0)
        printf("Error by AT!ENTERCND command!\n");
    else {
        line = atresponse->p_intermediates->line;
        err = at_tok_start(&line);
        if (err < 0)
            goto error;
        err = at_tok_nexthexint(&line, &cPid);
        if (err < 0)
            goto error;
        printf("PID = 0x%X\n",cPid);
error:
        at_response_free(atresponse);
        atresponse = NULL;
        if (cPid != 0x68A3) 
        {
            bRestorePid = true;
            asprintf(&cmd,"AT!UDPID=%X",sPid);
            at_send_command(cmd,&atresponse);
            if (err < 0 || atresponse->success == 0)
                printf("Error by AT!UDPID= command!\n");
            free(cmd);
            cmd = NULL;
            at_response_free(atresponse);
            atresponse = NULL;
        }
    }
    at_close();
    return 0;
exit:
    return -1;
}

/**************
 *
 * Name: downloadImageByExecv
 *
 * Purpose:  start download CWE image
 *
 * Params: 
 *           imagepathp    - CWE image path
 *           swifwdnldpathp- swifwdnld app path
 *           sdkpathname   - SWISDK path
 *           promptp       - prompt flag on/off
 *           verbose       - verbose mode on/off  
 *
 * Return:   -1 on failure , 0 on success
 *
 **************/
static int downloadImageByExecv(
    char *imagepathp,
    char *swifwdnldpathp,
    char *sdkpathnamep, 
    bool prompt, 
    bool verbose
    )
{
    char *proc = NULL;
    pid_t cid1 = -1, wid;
    int status=0;
    int waitoptions = 0;
    int i = 0;
    int ret = 0;
    char *argvlist[10] = {NULL};

    // Process swifwdnld will be executed by execv
    proc = swifwdnldpathp;

    // Prepare the argument list to execute swifwdnld    
    argvlist[i++] = "swifwdnld";
    argvlist[i++] = "-p";
    argvlist[i++] = sdkpathnamep;
    argvlist[i++] = "-f";

    //At index 4, provide path for image
    argvlist[i++] = imagepathp;
    if (prompt)
          argvlist[i++] = "-P";
    argvlist[i++] = "-x";
    if (verbose)          
        argvlist[i++] = "-v";

    cid1 = fork();
    if(cid1 == 0) { 
        printf("In child process\n");
            
        if( (ret = execv(proc, &argvlist[0])) < 0 ) {
           printf("Error: child: %d exec failed %d\n", cid1, errno);
           printf("Error: cannot execv %s\n",proc);
           goto exit1;
        }
    }
    else if( cid1 > 0 ) //cid>0, parent, waitfor child
    {
        do {
            wid = waitpid(cid1, &status, WUNTRACED | WCONTINUED);
            if (wid == -1) {
                printf("Waitpid error for child1\n");
                goto exit;
            }
            if (WIFEXITED(status)) {
                printf("Child exited, status=%d\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                printf("Child killed by signal=%d\n", WTERMSIG(status));
            } else if (WIFSTOPPED(status)) {
                printf("Child stopped by signal=%d\n", WSTOPSIG(status));
            }
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        if (WEXITSTATUS(status))
            goto exit;
    }
    else 
    {
        printf("Error: child1 fork failed\n");
        goto exit;
    }
    return 0;
exit1:
    return -2;
exit:
    return -1;
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
 *            -h  help                             Display this information and exit\n" );
 * 
 *            -a  1xBootImagepath <CWEimagepath>   Latest Boot CWE image for Version 1.x to be downloaded is \n"); *
 *                                                 at <CWEimagepath>\n");    
 *                                                 NOTE: This must be an absolute path,\n");
 *                                                 Not mandatory if latest 1x Boot image is already upgraded\n");
 *                                                 at <CWEimagepath>\n");    
 *                                                 NOTE: This must be an absolute path,\n");
 *                                                 Not mandatory if latest 1x Boot image is already upgraded\n");
 *            -b  3xBootImagepath <CWEimagepath>   Boot CWE image for Version 3.x to be downloaded is \n");
 *                                                 at <CWEimagepath>\n");    
 *                                                 NOTE: Mandatory and this must be an absolute path\n");
 *            -c  3xApplImagepath <CWEimagepath>   Appl CWE image for Version3.x to be downloaded is \n");
 *                                                 at <CWEimagepath>\n");    
 *                                                 NOTE: Mandatory and this must be an absolute path\n");
 *            -f  swifwdnldpathname <swifwdpath>   swifwdnld executable is at <swifwdpath>\n");
 *                                                 NOTE: if not specified the default \n");
 *                                                 pathname /system/bin/swifwdnld will be used\n");   
 *            -p  sdkpathname <sdkpath>            SDK executable is at <sdkpath>\n");
 *                                                 NOTE: if not specified the default \n");
 *                                                 pathname /system/bin/swisdk will be used\n");    
 *            -P  prompt                           Prompt user to verify the \n");
 *                                                 download before proceeding\n");
 *            -t  autotest                         Enable auto test mode\n");
 *                                                  -t 1: ***Test1***: 3x->1x->3x\n");
 *                                                  -t 2: ***Test2***: 1x->3x\n");
 *            -l  loopcount                        Loop count for auto test, default is 100\n");
 *
 *             The caller must specify a CWE image to download otherwise this
 *             program will exit.
 *
 **************/
static void printUsage( char *programname )
{
    printf("\n\nUsage: %s:\n", programname );
    printf("  -h  help                             Display this information and exit\n" );
    printf("  -a  1xBootImagepath <CWEimagepath>   Latest Boot CWE image for Version 1.x to be downloaded is \n");
    printf("                                       at <CWEimagepath>\n");    
    printf("                                       NOTE: This must be an absolute path,\n");
    printf("                                       Mandatory if current 1x boot loader is not latest \n");
    printf("  -c  3xBootImagepath <CWEimagepath>   Boot CWE image for Version 3.x to be downloaded is \n");
    printf("                                       at <CWEimagepath>\n");    
    printf("                                       Mandatory to upgrade at 3x image\n");
    printf("  -d  3xApplImagepath <CWEimagepath>   Appl CWE image for Version3.x to be downloaded is \n");
    printf("                                       at <CWEimagepath>\n");    
    printf("                                       NOTE: Mandatory to upgrade at 3x image\n");
    printf("  -f  swifwdnldpathname <swifwdpath>   swifwdnld executable is at <swifwdpath>\n");
    printf("                                       NOTE: if not specified the default \n");
    printf("                                       pathname /system/bin/swifwdnld will be used\n");
    printf("  -p  sdkpathname <sdkpath>            SDK executable is at <sdkpath>\n");
    printf("                                       NOTE: if not specified the default \n");
    printf("                                       pathname /system/bin/swisdk will be used\n");
    printf("  -P  prompt                           Prompt user to verify the \n");
    printf("                                       download before proceeding\n");
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
 *           autotest_mode - autotest mode
 *           loop_count    - loop count for auto mode
 *
 * Return:   bool
 *
 * Notes:    none
 * 
 **************/
static bool parseCommandLine(
    int argc, 
    char *argv[],    
    char **bootpathpp_1x,
    char **applpathpp_1x,
    char **bootpathpp_3x,
    char **applpathpp_3x,
    char **swifwdnldpathpp,
    char **sdkpathnamepp,
    bool *promptp,
    bool *autotest_mode,
    int *test_num,
    int  *loop_count
    )
{
    int result;
    int next_option;
    int optioncount = 0;
    char *programname;
    bool boot_1x = false;
    bool appl_1x = false;
    bool boot_3x = false;
    bool appl_3x = false;
    
    *bootpathpp_1x    = NULL;
    *applpathpp_1x    = NULL;
    *bootpathpp_3x    = NULL;
    *applpathpp_3x    = NULL;
    *sdkpathnamepp    = SDKDEFAULT;
    *swifwdnldpathpp  = SWIFWDNLDDEFAULT;
    *promptp          = false;
    *loop_count       = LOOPCNT_DEFAULT;
    *autotest_mode    = false;
    *test_num         = 0;

    /* Remember our own name */
    programname = argv[0]; 
    
    /* Parse the command line before doing anything else */
    do 
    {
        /* Read the next option until there are no more */
        next_option = getopt( argc, argv, "ha:b:c:d:f:p:l:t:P" );

        switch( next_option )
        {
            case 'h':
                /* Print usage information */
                printUsage(programname);
                exit( 0 );
                break;            

            case 'a':
                /* caller specifies a pathname to the latest 1x Boot CWE image to download */
                *bootpathpp_1x = optarg;
                boot_1x = true;
                break;

            case 'b':
                /* caller specifies a pathname to the latest 1x Appl CWE image to download */
                *applpathpp_1x = optarg;
                appl_1x = true;
                break;

            case 'c':
                /* caller specifies a pathname to the 3x Boot CWE image to download */
                *bootpathpp_3x = optarg;
                boot_3x = true;
                break;

            case 'd':
                /* caller specifies a pathname to the 3x Appl CWE image to download */
                *applpathpp_3x = optarg;
                appl_3x = true;
                break;

            case 'f':
                /* caller specifies a pathname to the swifwdnld app */
                *swifwdnldpathpp = optarg;
                break;
                
            case 'p':
                /* caller specifies a pathname to SDK */
                *sdkpathnamepp = optarg;
                break;
                           
            case 'P':
                /* Prompt */
                *promptp = true;
                break;

            case 'l':
                /* loop count */
                *loop_count = atoi(optarg);
                break;

            case 't':
                /* auto test mode */
                *autotest_mode = true;
                *test_num = atoi(optarg);
                break;
   
            case '?':
                /* Caller has used an invalid option */
                printf("\nInvalid option\n" );

                /* Print the usage info and exit */
                printUsage(programname);
                return(false);
                break;

            case -1:
                /* Done with arguments */
                break;

            default:
                return(false);
                break;
        }
        optioncount++;
    }
    while( next_option != -1 );
    
    /* If the CWE image file names and path were not given */
    if((boot_3x == false) || (appl_3x == false))  {
        printf("\nError: All 3x version images path not specified\n");

        /* Add in usage information as well */
        printUsage(programname);
        return(false);
    }
    if (*autotest_mode == true) {        
        if ((*test_num != 1) && (*test_num != 2)) {
            printf("\nError: In auto test mode, only Test 1 and Test 2 are supported, Execute as:\n");
            goto exit;
        }
        if ((*test_num == 1) &&
        ((boot_1x == false) || (boot_3x == false) || (appl_3x == false) || (appl_1x == false))) {
            printf("\nError: In auto test mode, ***Test1*** all 4 images (Boot_1x, Boot_3x, Appl_1x and Appl_3x) are required, Execute as:\n");

            goto exit;
        }
        if ((*test_num == 2) && ((boot_3x == false) || (appl_3x == false))) {
            printf("\nError: In auto test mode, ***Test2*** 2 images (Boot_3x and Appl_3x) are required, Execute as:\n");

            goto exit;
        }
    }        
    return(true);

exit:
    printf("  <app_name> [-f <swifwdnld_path>] [-p <swisdk path>] <-a 1x_boot> <-b 1x_appl> <-c 3x_boot> <-d 3x_appl> <-t test_number> [-l <loop_count>]\n");
    printf("  Execute test 1 as: \n" );
    printf("  fwdnld8092 -a 1x_boot -b 1x_appl -c 3x_boot -d 3x_appl -t 1 -l 10 \n");
    printf("  Execute test 2 as: \n" );
    printf("  fwdnld8092 -c 3x_boot -d 3x_appl -t 2 -l 10 \n");
    return false;
}


/**************
 *
 * Name:     main
 *
 * Purpose:  Entry point. 
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
    int ret = 0;    
    int j = 0;
    char *bootpathp_1x;
    char *applpathp_1x;
    char *bootpathp_3x;
    char *applpathp_3x;
    char *sdkpathnamep;
    char *swifwdnldpathp;
    bool prompt;
    bool verbose = true;
    bool autotest;
    int test_number;
    int loop_count;
    int retry;

    // Parse the command line
    if(!parseCommandLine(argc, argv, &bootpathp_1x, &applpathp_1x, 
                          &bootpathp_3x, &applpathp_3x, &swifwdnldpathp, 
                          &sdkpathnamep, &prompt, &autotest, &test_number, 
                          &loop_count))
    {
        exit(1);
    }
    
    if (autotest) {       
        printf("*****Auto test mode is enabled, firmware upgrade procedure will be repeated %d times*****\n", loop_count);

        // stop RIL daemon
        startstopRilDaemon_Slqssdk(false);
        verbose = false;
        prompt = false;
        
        while ( j < loop_count) {
            sleep(1);
            printf("\n*****Firmware upgrade procedure is being repeated %d time*****\n", j+1);

            if (test_number == 1) {
                // Download 1x images
                ret = downloadImageByExecv(bootpathp_1x, swifwdnldpathp, sdkpathnamep, prompt, verbose);
                if ( ret == -1)
                    goto exit;
                else if (ret == -2)
                    goto exit1;
                else
                    printf("Image successfully downloaded from %s\n", bootpathp_1x);

                ret = downloadImageByExecv(applpathp_1x, swifwdnldpathp, sdkpathnamep, prompt, verbose);
                if ( ret == -1)
                    goto exit;
                else if (ret == -2)
                    goto exit1;
                else
                    printf("Image successfully downloaded from %s\n", applpathp_1x);

                retry = 0;
                while (1) {
                    // Sleep for a while, drivers will be reloaded
                    sleep(2);

                    if ((verify_ttyports() != 0x0F) && (verify_ttyports() != 0x1F)) {
                        printf("ttyUSB ports were not found in /dev, retrying \n" );
                        if (retry == 5) {
                            printf("ttyUSB ports were not found in /dev even after 5 retries, exit! \n" );
                            goto exit;
                        }
                    }
                    else {
                        printf("ttyUSB ports were found in /dev \n");
                        break;
                    }
                    retry++;
                }
            }
 
            // open AT port and change PID
            ret = openAtPort_to_ModifyPid();
            if (ret == -1)
                goto exit;

            // Download 3x image
            sleep(1);

            ret = downloadImageByExecv(bootpathp_3x, swifwdnldpathp, sdkpathnamep, prompt, verbose);
            if ( ret == -1)
                goto exit;
            else if (ret == -2)
                goto exit1;
            else
                printf("Image successfully downloaded from %s\n", bootpathp_3x);

            ret = downloadImageByExecv(applpathp_3x, swifwdnldpathp, sdkpathnamep, prompt, verbose);
            if ( ret == -1)
                goto exit;
            else if (ret == -2)
                goto exit1;
            else
                printf("Image successfully downloaded from %s\n", applpathp_3x);

            retry = 0;
            while (1) {
                // Sleep for a while, drivers will be reloaded
                sleep(2);

                if ((verify_ttyports() != 0x0F) && (verify_ttyports() != 0x1F)) {
                    printf("ttyUSB ports were not found in /dev, retrying \n" );
                    if (retry == 5) {
                        printf("ttyUSB ports were not found in /dev even after 5 retries, exit! \n" );
                        goto exit;
                    }
                }
                else {
                    printf("ttyUSB ports were found in /dev \n");
                    break;
                }
                retry++;
            }
            j++;
        }
        startstopRilDaemon_Slqssdk(true);
        return 0;
    }
    else {
        startstopRilDaemon_Slqssdk(false);
        sleep(1);

        // open AT port and change PID
        ret = openAtPort_to_ModifyPid();
        if (ret == -1)
            goto exit;

        if (bootpathp_1x) {
            ret = downloadImageByExecv(bootpathp_1x, swifwdnldpathp, sdkpathnamep, prompt, verbose);
            if ( ret == -1)
                goto exit;
            else if (ret == -2)
                goto exit1;
            else
                printf("Image successfully downloaded from %s\n", bootpathp_1x);

            // sleep for a while
            sleep(2);
        }

        if (bootpathp_3x && applpathp_3x) {
            ret = downloadImageByExecv(bootpathp_3x, swifwdnldpathp, sdkpathnamep, prompt, verbose);
            if ( ret == -1)
                goto exit;
            else if (ret == -2)
                goto exit1;
            else
                printf("Image successfully downloaded from %s\n", bootpathp_3x);

            ret = downloadImageByExecv(applpathp_3x, swifwdnldpathp, sdkpathnamep, prompt, verbose);
            if ( ret == -1)
                goto exit;
            else if (ret == -2)
                goto exit1;
            else
                printf("Image successfully downloaded from %s\n", applpathp_3x);
        }
       startstopRilDaemon_Slqssdk(true);
       return 0;  
    } 
    
exit: 
    startstopRilDaemon_Slqssdk(true);
exit1:
    exit(1); 
}

