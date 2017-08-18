/*
 * @ingroup swidm
 *
 *  Filename: swidm_main.c
 *
 *  Purpose:  This file contains routine(s) that deal with DM log
 *            
 *
 * Copyright: © 2011 Sierra Wireless, Inc., all rights reserved
 *
 */

#include "swidm.h"
#include <strings.h>

#define MAX_FILTER_SZ (1024*1024)
#define MAX_RESP_SZ (1024)
#define TAIL (0x7e)
#define VER_LEN (3)

static void usage(char *s)
{
    fprintf(stderr, " Local Logging with 9x15 Modem Series: \n \
    %s -l [-d device] [-c 9x15] [-f filter] [-o logfile]: \n \
Local Logging with other Modem Series: \n \
    %s -l [-d device] [-f filter] [-o logfile]: \n \
         - Logs DM packets to \"logfile\" if specified, or to a script generated \n \
           log file stored in the /data directory with name swidmlog if not specified. \n \
         - For 9x15 Modem series, -c option is provided along with -f option to \n \
           concatenate filter with DM port. \n\n \
Remote Logging: \n \
    %s [-d device] [-f filter] [-p netport] [-r rhost] \n \
         - Establishes a TCP connection with a remote machine \"rhost\" using port number \n \
           \"netport\", or a default port number of 2500 if not specified. DM packets are  \n\
           exchanged over the TCP connection.\n \
         - Without the -r option, acts as a server application waiting for an incoming \n\
           connection request. Otherwise, acts as a client and attempts to establish a \n\
           connection with \"rhost\". \n\n \
     OPTIONS: \n \
       -l            - Local logging if specified, remote logging otherwise \n \
       -d device     - (Optional) /dev/ttyUSBx port for DM logging \n \
       -c chipset    - (Optional) In Local Logging to concatenate filter with DM port for 9x15 modem series \n \
       -f filter     - (Optional) DM filter to send to the device prior to logging \n \
       -p netport    - (Optional) Remote logging TCP port (defaults to 2500) \n \
       -r rhost      - (Optional) remote host to connect to \n \
       -o logfile    - (Optional) fully qualified DM log (output) file name \n ", s, s, s);
   
}

const char *getVersion(void)
{
    return SWIDM_VERSION_STRING;
}

/**
 *
 * main function for DM logging
 *
 * @param argc
 *     argument count
 * @param[in] argv 
 *     Pointer to a array of arguments
 *
 * @return
 *     exit code
 *
 * @par Abort:<br> 
 *     none
 *
 * @note
 *     none
 *
 */       
int main(int argc, char **argv)
{
    int  netport = 0;
    int  c, fd;
    int  iRet = 0;
    char *logfile = NULL;
    char *device = NULL;
    char *filter = NULL;
    char *rhost = NULL;
    char *chipset = NULL;
    bool fLocal = false;
    char systemcmd[MAX_PATH] = ""; 
    char time_string[40] = "";
    char defaultfile[MAX_PATH] = "";
    struct timeval tv;
    struct tm* ptm;
    struct termios oldtiop, newtiop;
    
    /* show swidm application version */
    LOGD("swidm:: version: %s\n", getVersion());

    /* Clear structures for serial port open and close */
    bzero(&oldtiop, sizeof(struct termios));
    bzero(&newtiop, sizeof(struct termios));

    /* Get command line arguments */
    while((c = getopt(argc, argv, "?hvld:c:f:o:p:r:"))!= EOF){
        switch   (c)  
        {  
        case 'l':  
            fLocal = true;  
            break;  
        case 'd':  
            device = optarg;  
            LOGD("%s:: device: %s\n", __func__, device);
            break;  
        case 'c':
            chipset = optarg;  
            LOGD("%s:: chipset: %s\n", __func__, chipset);
            break;  
        case 'f':
            filter = optarg;  
            LOGD("%s:: filter: %s\n", __func__, filter);
            break;  
        case 'o':  
            logfile = optarg; 
            LOGD("%s:: logfile: %s\n", __func__, logfile);
            break;  
        case 'p':  
            netport = atoi(optarg); 
            LOGD("%s:: TCP port: %d\n", __func__, netport);
            break;  
        case 'r':  
            rhost = optarg; 
            LOGD("%s:: remote host: %s\n", __func__, rhost);
            break;  
        case '?':
        case 'h':
            usage(argv[0]);
            exit(0);
        case 'v':
            fprintf(stderr,"%s\n", getVersion());
            exit(0);
        }   
    }  

    /* if the device is not specified, it will be automatically detected */
    if(device == NULL){ 
        swims_ossdkscaninit();
        LOGI("%s:: Opening tty device automatically\n", __func__);

        device = swims_ossdkgetdmifname();
        while(device == NULL){
            LOGI("%s:: Device not found. wait for usb scanning \n", __func__);
            device = swims_ossdkgetdmifname();
            sleep(1);
        }
        LOGI("%s:: Device '%s' found for DM log \n", __func__, device);
        printf("Device '%s' found for DM log.\r\n", device);
    }

    /* The logic for DM logging
     * 1) Open the DM port and set input mode.
     * 2.a) For local logging, cat filter to DM port.
     * 2.b) For remote logging, use nc to route the logs.
     * 3) For local logging, cat the log to a file. */

    /* Step 1. open device for logging */
    fd = open_dev(device, &oldtiop, &newtiop);
    LOGD("%s:: open_dev: %s fd: %d\n", __func__, device, fd);
    if(fd < 0) {
        LOGE("%s:: open_dev failed for %s\n", __func__, device);
        exit(1);
    }

    /*  open the filter for logging */
    FILE * filter_fd =fopen(filter, "rb");
    LOGD("%s:: fopen: %s filter_fd: 0x%p\n", __func__, filter, filter_fd);
    if(filter_fd == NULL) {
        LOGE("%s:: fopen failed for %s\n", __func__, filter);
        exit(1);
    }

    if(fLocal) {
        LOGD("DM logging ---local logging begin---\n");
        if(filter != NULL) {

            /* Step 2.a) For local logging, cat filter to DM port. */
            if ((chipset != NULL) && (strcmp(chipset, "9x15") == 0))
            {
                int olen, ilen;
                uint8_t *start, *stop;
                int osize;

                uint8_t *buf = malloc(MAX_FILTER_SZ);
                uint8_t *resp = malloc(MAX_RESP_SZ);
                osize = fread(buf, 1, MAX_FILTER_SZ, filter_fd);

                start = buf + VER_LEN;
                stop = start;
                while( (stop - start) < osize) {
                    if ( *stop != TAIL) {
                        stop++;
                    } else {
                        olen = write(fd, start, stop-start+1);
                        ilen = read(fd, resp, MAX_RESP_SZ);
                        //printf("olen, ilen: %d, %d\n", olen, ilen);
                        start = stop+1;
                        stop = start;
                    }
                }

                free(resp);
                free(buf);
                fclose(filter_fd);
                close(fd);
            }
            else
            {
                LOGD("%s:: write filter: %s to device: %s\n", __func__, filter, device);
                sprintf(systemcmd, "cat %s > %s", filter, device);
                LOGD("%s:: Execute system command: \"%s\"\n", __func__, systemcmd);
                iRet = system(systemcmd);
                if(iRet == -1){
                    LOGE("%s:: %s failed \n", __func__, systemcmd);
                    goto error;
                }
                sleep(5);
            }
        }

        /* Step 3 For local logging, cat the log to a file. */
        if(logfile == NULL){
            /* use default file name + time */
            gettimeofday(&tv, NULL);
            ptm = localtime(&tv.tv_sec);
            strftime (time_string, sizeof (time_string), "%Y-%m-%d-%H-%M", ptm);
            sprintf(defaultfile,"%s-%s", SWIDM_DEFAULT_LOG_FILE, time_string);
            sprintf(systemcmd, "cat %s > %s", device, defaultfile);
        } else    
            sprintf(systemcmd, "cat %s > %s", device, logfile);
        
        LOGD("%s:: Execute system command: \"%s\"\n", __func__, systemcmd);
        iRet = system(systemcmd);
        if(iRet == -1){
            LOGE("%s:: %s failed \n", __func__, systemcmd);
            goto error;
        }
    } else {
        LOGD("DM logging ---remote logging begin---\n");

        if(filter != NULL) 
        {
            /* Step 2.a) For local logging, cat filter to DM port. */
            LOGD("%s:: write filter: %s to device: %s\n", __func__, filter, device);
            sprintf(systemcmd, "cat %s > %s", filter, device);
            LOGD("%s:: Execute system command: \"%s\"\n", __func__, systemcmd);
            iRet = system(systemcmd);
            if(iRet == -1){
                LOGE("%s:: %s failed \n", __func__, systemcmd);
                goto error;
            }
            sleep(5);
        }

        /* Step 2.b) For remote logging, use nc to route the logs. */
        if(netport == 0)
            netport = NETPORTDFLT; 
        if(rhost == NULL){
           /* Without the -r option, acts as a server application waiting for an incoming connection request. */
            sprintf(systemcmd,  "nc 127.0.0.1 %d <%s> %s", netport, device, device);
            LOGD("%s:: Execute command: \"%s\" on host site\n", __func__, systemcmd);
            iRet = system(systemcmd);
            if(iRet == -1){
                LOGE("%s:: %s failed \n", __func__, systemcmd);
                goto error;
            }
        } else {
            /* With -r option, acts as a client and attempts to establish a connection with \"rhost\". */
            sprintf(systemcmd, "nc %s %d <%s> %s", rhost, netport, device, device);
            LOGD("%s:: Execute system command: \"%s\"\n", __func__, systemcmd);
            iRet = system(systemcmd);
            if(iRet == -1){
                LOGE("%s:: %s failed \n", __func__, systemcmd);
                goto error;
            }
        }
    }

    exit(0);
error:
    iRet = close_dev(fd, &oldtiop);
    if(iRet == -1)
        LOGE("%s:: close_dev for %s failed \n", __func__, device);
    else
        LOGD("%s close_dev %s rv: %d\n", __func__, device, iRet);
    exit(1);
}
