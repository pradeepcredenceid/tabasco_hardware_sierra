/**
 *
 * @ingroup swi_hl_download.c
 *
 * @file
 * - Firmware download for HL modem
 * - It will spawn Intel Firmware DownloadTool
 *
 * @author
 * Copyright: © 2015 Sierra Wireless, Inc.
 *            All rights reserved
 *
 */

/* Linux definitions */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <errno.h>
#include <dirent.h>


#define DOWNLOAD_TOOL_DEFAULT_PATH "/system/bin/DownloadTool"
#define DEFAULT_VERBOSE 4

/******************
 *
 * Name: verify_ttyports()
 *
 * Purpose:  verify the numbe rof ttyACM ports
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
            if (!strcmp(namelist[n]->d_name,"ttyACM0"))
                mask = mask | (1 << 0);
            else if(!strcmp(namelist[n]->d_name,"ttyACM1"))
                mask = mask | (1 << 1);
            else if(!strcmp(namelist[n]->d_name,"ttyACM2"))
                mask = mask | (1 << 2);
            else if(!strcmp(namelist[n]->d_name,"ttyACM3"))
                mask = mask | (1 << 3);
            else if(!strcmp(namelist[n]->d_name,"ttyACM4"))
                mask = mask | (1 << 4);
            free(namelist[n]);
        }
    free(namelist);
    }
    return mask;
}


/******************
 *
 * Name: openAtPort_to_reset
 *
 * Purpose:  Open AT port and reset
 *
 * Params:   None
 *
 * Return:   -1 on failure , 0 on success
 *
 ***************/
static int openAtPort_to_reset()
{
    int fd = -1;
    int retry = 0;
    char atReset[]  = "AT+CFUN=1,1\r";
    bool bReset = 0;

    // Open AT port of HL Modem to execute AT commands
    for (;;) {
        fd = -1;        
        if (fd < 0) {
            // Open ACM0 for HL7 modem
            if (verify_ttyports() == 0x07)
                fd = open ("/dev/ttyACM0",O_RDWR);
            // Open ACM3 for HL8 modem
            else if (verify_ttyports() > 0x07)
                fd = open ("/dev/ttyACM3",O_RDWR);
            else {
                printf("tty port not found to execute AT\n");
                goto exit;
            }

            if (fd < 0 && (retry > 3)){
                printf("Opening USB port to execute AT commands failed!\n");
                goto exit;
            }
            else if (fd < 0){
                printf("Retrying after 2 seconds to open port!\n");
                retry++;
                sleep(2);
                continue;
            }
            break;
        }
    }

    fcntl(fd, F_SETFL, 0);

    printf("\nResetting the device......\n\n");

    // Sometimes, need to send command multiple times to reset device    
    for (retry = 0; retry < 5; retry ++)
    {
        int n = write(fd, atReset, strlen(atReset));
        if (n < 0) {
            sleep(1);
        }
        else {
            break;
        }
    }
    close(fd);
    if (retry >= 5) {
        printf("Reset failed!\n");
        goto exit;
    }

    return 0;
exit:
    return -1;
}

/******************
 *
 * Name: startstopRilDaemon
 *
 * Purpose:  start-stop RILdaemon
 *
 * Params:   bool
 *
 * Return:   none
 *
 ***************/
static void startstopRilDaemon(bool start)
{
    int ret;
    int cPid = -1;
    char *cmd = NULL;

    if (start) {
        ret = system("start ril-daemon");
        if (ret == -1)
            printf("ril-daemon not started\n");
    }
    else {
        ret = system("stop ril-daemon");
        if (ret == -1)
            printf("ril-daemon not stopped\n");
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
 * Return:   none
 *
 * Note:     the caller must specify a HL image path otherwise this
 *           program will exit.
 *
 **************/
static void printUsage( char *programname )
{
    printf("\n\nUsage: %s:\n", programname );
    printf("  -h  help                                         Display this information and exit\n" );
    printf("  -i  images <numImages> <path of each image>      HL Image is located at <path>\n");
    printf("                                                   Note: Mandatory, provide number of images and path of\n");
    printf("                                                   each image with image name\n");
    printf("  -p  ExecutablePath <toolpath>                    Download Tool executable is at <toolpath>\n");
    printf("                                                   NOTE: If not specified the default \n");
    printf("                                                   pathname /system/bin/DownloadTool will be used\n");
    printf("  -v  verbose                                      Verbose output of DownloadTool during execution [0-4]\n");
    printf("                                                   Note: If not specified default is 4\n");
    printf("  Example: HLDownload -i 2 /data/img1.fls /data/img2.fls -p /data/app/Downloadtool -v 4\n");
}

/**************
 *
 * Name:     parseCommandLine
 *
 * Purpose:  This function parses the caller's inputed parameters.
 *           
 * Parms:    argc          - argument count
 *           argv          - argument vector
 *           nImagesp      - number of images to be downloaded
 *           imagepathpp   - memory location to place pointer to the image 
 *                           path string
 *           apppathpp     - memory location to place pointer to the downlaod tool
 *                           path string
 *           verbose       - verbose level
 *
 * Return:   bool
 *
 * Notes:    none
 * 
 **************/
static bool parseCommandLine(
    int argc, 
    char *argv[],
    int  *nImagesp,    
    char *imagepathp[],
    char **applpathpp,
    int *verbose
    )
{
    char *programname;
    int i;
    int lImages;
    int j=0;

    *applpathpp     = DOWNLOAD_TOOL_DEFAULT_PATH;
    *verbose        = DEFAULT_VERBOSE;

    /* Remember our own name */
    programname = argv[0];

    i = 1; /* first argument is name of the executable */
    while (i < argc) {
        if (0 == strcmp(argv[i],"-h")) {
            printUsage(programname);
            exit(0);
        }
        else if (0 == strcmp(argv[i],"-i")) {
            i++;
            if (i == argc)
                goto exit;
            *nImagesp = atoi(argv[i]);
            lImages = *nImagesp;
            while (lImages > 0) {
                i++;
                if (i == argc)
                    goto exit;
                imagepathp[j++] = argv[i];
                lImages--;
            }
        }
        else if (0 == strcmp(argv[i],"-p")) {
            i++;
            if (i == argc)
                goto exit;
            *applpathpp = argv[i];
        }
        else if (0 == strcmp(argv[i],"-v")) {
            i++;
            if (i == argc)
                goto exit;
            *verbose = atoi(argv[i]);
            if((*verbose < 0) || (*verbose > 4))
                goto exit;
        }
        else {
            goto exit;
        }
        i++;
    }

    return(true);

exit:
    printf("\nError: Incorrect arguments\n");
    printf("  Execute as: \n" );
    printf("  HLDownload -i 2 /data/image.fls /data/image2.fls -v 4\n");
    return false;
}



/**************
 *
 * Name:     main
 *
 * Purpose:  Entry point. 
 *           
 * Parms:    argc - number of arguments
 *           argv[]- array of pointers for arguments
 * 
 * Return:   0 - Normal termination
 *           1 - Problem with the execution
 * 
 **************/
int main( int argc, char *argv[] )
{
    char *proc = NULL;
    pid_t cid = -1, wid;
    int status=0;
    int ret = 0;
    int i=0;
    int j=0;
    int nImages=0;
    char *imagepathp[10] = {NULL};
    char *applpathp;
    int verbose;
    char buf[16];
    char buf2[8];
    char **argvlist = NULL;

    // Parse the command line
    if(!parseCommandLine(argc, argv, &nImages, imagepathp, &applpathp, &verbose))
    {
        exit(1);
    }

    argvlist = malloc((3 + nImages) * sizeof(char*));
    if (!argvlist)
        exit(1); //memory allocation failed

    // Process DownloadTool will be executed by execv
    proc = applpathp;

    argvlist[i++] = "DownloadTool";

    while (j < nImages) {
        argvlist[i++] = imagepathp[j++];
    }

    // debug on USB
    argvlist[i++] = "-dUSB";

    strcpy(buf, "-v");
    sprintf(buf2, "%d", verbose);
    strcat(buf,buf2);
    argvlist[i++] = buf;

    cid = fork();
    if(cid == 0) { 
        printf("\nStarting Intel Firmware Download Tool:\n");
        startstopRilDaemon(false);   
        if( (ret = execv(proc, &argvlist[0])) < 0 ) {
           printf("Error: Could not start %s error is %d\n",proc, errno);
           goto exit;
        }
    }
    else if( cid > 0 ) //cid>0, parent, waitfor child
    {
        // do power reset after 10 sec delay
        sleep(10);
        //Before power reset, make sure download tool is still running
        wid = waitpid(cid, &status, WNOHANG);
        if (wid == 0) {
            // download tool is alive
            ret = openAtPort_to_reset();
            if (ret == -1) {
                kill(cid, SIGKILL);
            }
        }
        else {            
            //download tool exited alraedy, no need to reset
            goto exit;
        }

        // blocking wait until download tool is exited
        do {
            wid = waitpid(cid, &status, WUNTRACED | WCONTINUED);
            if (wid == -1) {
                printf("Waitpid error for child\n");
                goto exit;
            }
            if (WIFEXITED(status)) {
                printf("\nIntel Download Tool Exited, status=%d\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                printf("\nIntel Download Tool killed by signal=%d\n", WTERMSIG(status));
            } else if (WIFSTOPPED(status)) {
                printf("\nIntel Download Tool stopped by signal=%d\n", WSTOPSIG(status));
            }
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        if (WEXITSTATUS(status))
            goto exit;
    }
    else 
    {
        printf("Error: child fork failed\n");
        goto exit1;
    }
    startstopRilDaemon(true);
    free(argvlist);
    return 0;
exit:
    startstopRilDaemon(true);
exit1:
    free(argvlist);
    exit(1);
}

