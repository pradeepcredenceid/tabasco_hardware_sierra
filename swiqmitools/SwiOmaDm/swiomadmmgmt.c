/**
 *
 * @ingroup swiomadmmgmt
 *
 * @file
 * main fuction of OMADM management for MC73xx devices 
 *
 * @author
 * Copyright: 2015 Sierra Wireless, Inc.
 *            All rights reserved
 *
 */

/* include files */
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include "SwiDataTypes.h"
#include "stdbool.h"

#define  LOG_TAG  "swi_omadm"
#include "swiril_log.h"
#include "swiqmitool_common.h"
#include "SWIWWANCMAPI.h"
#include "qmerrno.h"

#define SwiOmaStartSession_CMDLINE           "--startsession"
#define SwiOmaCancelInterval_CMDLINE         "--cancelinterval"
#define SwiOmaAceeptUpdate_CMDLINE           "--selection"
#define SwiOmaSetSettings_CMDLINE            "--setomadmsettings"
#define SwiOmaGetSettings_CMDLINE            "--getomadmsettings"

#define DEFAULT_CANCEL_INTERVAL 60
#define DEFAULT_SELECTION_OMADM 1
#define MAX_SESSION_WAIT_RETRY 5

#define SESSION_TYPE_FOTA   0x00
#define SESSION_TYPE_CONFIG 0x01
#define SESSION_TYPE_NOTIFICATION 0x02

/* call state information for FOTA */
enum eSTATE_FOTA
{
    eNO_FIRMWARE_AVAILABLE    = 0x01,
    eQUERY_FIRMWARE_DOWNLOAD  = 0x02,
    eFIRMWARE_DOWNLOADING     = 0x03,
    eFIRMWARE_DOWNLOADED      = 0x04,
    eQUERY_FIRMWARE_UPDATE    = 0x05,
    eFIRMWARE_UPDATING        = 0x06,
    eFIRMWARE_UPDATED         = 0x07,
    eSEL_READ_REQUEST         = 0x08,
    eSEL_CHANGE_REQUEST       = 0x09
};

/* call state information for CONFIG */
enum eSTATE_CONFIG
{
    eREAD_REQUEST    = 0x01,
    eCHANGE_REQUEST  = 0x02,
    eCONFIG_COMPLETE = 0x03,
};

/* call state information for NOTIFICATION */
enum eSTATE_NOTIFICATION
{
    eGPS_SETTINGS_CHANGE    = 0x01,
    eDEVICE_RESET  = 0x02,
    eDEVICE_FACTORY_RESET = 0x03,
    eCIDC_SESSION_START = 0x04,
    eCIDC_SESSION_END = 0x05,
    eCIPRL_SESSION_START = 0x06,
    eCIPRL_SESSION_END = 0x07,
    eCIFUMO_SESSION_START = 0x08,
    eCIFUMO_SESSION_END = 0x09,
    eHFADC_START = 0x0A,
    eHFADC_END = 0x0B,
    eHFAPRL_START = 0x0C,
    eHFAPRL_END = 0x0D,
    eHFAFUMO_START = 0x0E,
    eHFAFUMO_END = 0x0F,
    eNI_SESSION_START = 0x10,
    eNI_SESSION_END = 0x11   
};

/* call state information for CONFIG */
enum eSWIOMA_SETTING
{
    eENABLE =1,
    eDISABLE,
};


BYTE sessionState = 0xFF;
ULONG g_selection = DEFAULT_SELECTION_OMADM;
bool bSessionDone = false;

static void usage(char *s)
{
    fprintf(stderr,
             "usage: \n   %s   --startsession <session type> \t 1 for FOTA, 2 for DM, 3 for PRL \n \
             --cancelinterval <time> \t\t time interval in seconds to cancel session, default is 60 seconds \n \
             --getomadmsettings \t\t get current OMADM settings \n \
             --selection <value> \t\t accept/reject/defer OMADM updates, accept:1 reject:2 defer:3, default is 1 \n \
             --setomadmsettings <value> \t enable/disable firmware auto download/update mode enable:1, disable:0 \n", s);
    exit(-1);
}


void SendSelection(ULONG selection)
{
    ULONG deferTime;
    ULONG rejectReason;
    ULONG nRet = eQCWWAN_ERR_GENERAL;

    switch (selection)
    {
        case 1:  
             nRet = SLQSOMADMSendSelection2(selection, NULL, NULL);
             break;
        case 2:
             /* provide the reason of reject, 0 - user, 1 - enterprise policy */
             rejectReason = 0;
             nRet = SLQSOMADMSendSelection2(selection, NULL, &rejectReason);
             break;            
        case 3:
             /* specify the defer time in minutes, A value of 0 will cause the prompt to be resent immediately
                here we defer it 10 minutes */
             deferTime = 10;
             nRet = SLQSOMADMSendSelection2(selection, &deferTime, NULL);
             break;
    }
    if ( eQCWWAN_ERR_NONE != nRet )
    {
        fprintf( stderr, "Not Able To Send Selection\n" );
        fprintf( stderr, "Error Code : %ld \n",nRet );
    }
    else
    {
        fprintf( stderr, "Selection Sent Successfully\n");
    }
    return;    
}


void cbkSetSLQSOMADMAlertCallback(
    ULONG sessionType,
    BYTE* psessionTypeFields )
{    
    static BYTE downloading = 0, updating = 0;
    if ( sessionType == SESSION_TYPE_FOTA )
    {
        if ( psessionTypeFields )
        {
            struct omaDmFotaTlv *sessionInfoFota;
            sessionInfoFota = (struct omaDmFotaTlv *)psessionTypeFields;

            /* Update notification variable */
            sessionState = sessionInfoFota->state;

            switch( sessionState )
            {
                case eNO_FIRMWARE_AVAILABLE:
                    fprintf ( stderr, "\n\nNo Firmware Available!!!\n" );
                    bSessionDone = true;
                    break;

                case eQUERY_FIRMWARE_DOWNLOAD:
                    fprintf ( stderr, "\nProceed to Download firmware \n");
                    fprintf( stderr, "Selection value is %lu\n", g_selection);
                    SendSelection(g_selection);
                    if (g_selection == 2)
                        bSessionDone = true;
                    break;

                case eFIRMWARE_DOWNLOADING:
                    downloading++;
                    if (downloading == 1)
                    {
                        fprintf( stderr, "\n\nDownloading Firmware" );
                    }
                    else
                    {
                        fprintf( stderr, "... Please wait!!" );
                    }
                    break;

                case eFIRMWARE_DOWNLOADED:
                    fprintf ( stderr, "\nFirmware Downloaded\n" );
                    downloading = 0;
                    bSessionDone = true;
                    break;

                case eQUERY_FIRMWARE_UPDATE:
                    fprintf ( stderr, "\nProceed to Update firmware \n");
                    fprintf( stderr, "Selection value is %lu\n", g_selection);
                    SendSelection(g_selection);
                    if (g_selection == 2)
                        bSessionDone = true;
                    break;

                case eFIRMWARE_UPDATING:
                    updating++;
                    if (updating == 1)
                    {
                        fprintf( stderr, "\n\nUpdating Firmware" );
                    }
                    fprintf( stderr, " ... Plesae wait!!" );
                    break;

                case eFIRMWARE_UPDATED:
                    updating = 0;
                    fprintf ( stderr, "\nFirmware Updated\n" );
                    bSessionDone = true;
                    break;

                default:
                    fprintf ( stderr, "\n\nInvalid Response from Callback!! \n" );
                    sessionState = 0xFF;
                    break;
            }
        }
    }
    else if( sessionType == SESSION_TYPE_CONFIG )
    {
        if ( psessionTypeFields )
        {
            struct omaDmConfigTlv *sessionInfoConfig = (struct omaDmConfigTlv *)psessionTypeFields;
            sessionState = sessionInfoConfig->state;
            switch( sessionState )
            {
                case eREAD_REQUEST:
                    fprintf ( stderr, "\nProceed to Read Request \n");
                    fprintf( stderr, "Selection value is %lu\n", g_selection);
                    SendSelection(g_selection);
                    if (g_selection == 2)
                        bSessionDone = true;
                    break;

                case eCHANGE_REQUEST:
                    fprintf ( stderr, "\nProceed to Change Request \n");
                    fprintf( stderr, "Selection value is %lu\n", g_selection);
                    SendSelection(g_selection);
                    if (g_selection == 2)
                        bSessionDone = true;
                    break;

                case eCONFIG_COMPLETE:
                    fprintf ( stderr, "\nConfig Completed\n" );
                    bSessionDone = true;
                    break;

                default:
                    fprintf ( stderr, "Invalid Response from Callback \n" );
                    sessionState = 0xFF;
                    break;
            }
        }
    }
    else if( sessionType == SESSION_TYPE_NOTIFICATION )
    {
        if ( psessionTypeFields )
        {
            struct omaDmNotificationsTlv *sessionInfoNotify = (struct omaDmNotificationsTlv *)psessionTypeFields;
            sessionState = sessionInfoNotify->notification;
            switch( sessionState )
            {
                case eCIDC_SESSION_START:
                    fprintf ( stderr, "\nCI-DC session started \n");
                    break;
                case eCIDC_SESSION_END:
                    fprintf ( stderr, "\nCI-DC session ended \n");
                    break;
                case eCIPRL_SESSION_START:
                    fprintf ( stderr, "\nCI-PRL session started \n");
                    break;
                case eCIPRL_SESSION_END:
                    fprintf ( stderr, "\nCI-PRL session ended \n");
                    break;
                case eCIFUMO_SESSION_START:
                    fprintf ( stderr, "\nCI-FUMO session started \n");
                    break;
                case eCIFUMO_SESSION_END:
                    fprintf ( stderr, "\nCI-FUMO session ended \n");
                    break;
                case eHFADC_START:
                    fprintf ( stderr, "\nHFA-DC session started \n");
                    break;
                case eHFADC_END:
                    fprintf ( stderr, "\nHFA-DC session ended \n");
                    break;
                case eHFAPRL_START:
                    fprintf ( stderr, "\nHFA-PRL session started \n");
                    break;
                case eHFAPRL_END:
                    fprintf ( stderr, "\nHFA-PRL session ended \n");
                    break;
                case eHFAFUMO_START:
                    fprintf ( stderr, "\nHFA-FUMO session started \n");
                    break;
                case eHFAFUMO_END:
                    fprintf ( stderr, "\nHFA-FUMO session ended \n");
                    break;
                case eNI_SESSION_START:
                    fprintf ( stderr, "\nNI session started \n");
                    break;
                case eNI_SESSION_END:
                    fprintf ( stderr, "\nNI session ended \n");
                    break;
                default:
                    fprintf ( stderr, "unprocessd respone %d, \n",sessionState );
                    sessionState = 0xFF;
                    break;
            }
            fprintf ( stderr, "session status value %d, \n",sessionInfoNotify->sessionStatus );
        }
    }
}

void SetCallBack(void)
{
    ULONG rc;
    rc = SetSLQSOMADMAlertCallback( &cbkSetSLQSOMADMAlertCallback );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "SetCallBack: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "SetCallback: Enabled callback\r\n" );
}

void ClearCallBack(void)
{
    ULONG rc;
    rc = SetSLQSOMADMAlertCallback( NULL );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "Callback: Failed : %lu\n", rc );
    else
        fprintf( stderr, "Callback: Disabled\n" );
}

bool StartSession(ULONG sessionType)
{
    ULONG nRet;
    ULONG startSessionResponse;

    fprintf( stderr, "Start OMADM session with type %lu\n", sessionType );
    nRet  = SLQSOMADMStartSession2( sessionType,  &startSessionResponse);
    if ( eQCWWAN_ERR_NONE != nRet )
    {
        fprintf( stderr, "Not Able To Start Session\n" );
        fprintf( stderr, "Error Code : %ld \n",nRet );
        return false;
    }
    else
    {
        fprintf( stderr, "Session Started\n"\
               "Firmware Availiability: %lu\n", startSessionResponse );
        return true;
    }
}

void CancelSession(ULONG sessionType)
{
    ULONG nRet;
   
    nRet  = SLQSOMADMCancelSession( sessionType );
    if ( eQCWWAN_ERR_NONE != nRet )
    {
        fprintf( stderr, "Not Able To Cancel Session\n" );
        fprintf( stderr, "Error Code : %ld \n",nRet );
    }
    else
    {
        fprintf( stderr, "Session Cancelled\n" );
    }
    return;
}

void GetSessionInfo(ULONG sessionType)
{
    BYTE Status        = 0xFF;
    ULONG nRet;
    SLQSOMADMSessionInfo sessionState;

    memset(&sessionState, 0, sizeof(sessionState));
    sessionState.pStatus = &Status;

    nRet  = SLQSOMADMGetSessionInfo( &sessionType, &sessionState );

    if ( eQCWWAN_ERR_NONE != nRet )
    {
        fprintf( stderr, "Not Able To Get Session Info\n" );
        fprintf( stderr, "Error Code : %ld \n",nRet );
        return;
    }

    fprintf( stderr, "\nSession Type : %ld \n",sessionType );
    fprintf( stderr, "Session State : %d \n", *sessionState.pStatus );
    return;
}

void SetSwiOMADMSetting(ULONG setOMADM)
{
    ULONG bFOTAdownload;
    ULONG bFOTAUpdate;
    ULONG nRet;

    bFOTAdownload = setOMADM;
    bFOTAUpdate = setOMADM;

    fprintf( stderr, "set OMADM settings as %lu\n", setOMADM );
    nRet  = SLQSOMADMSetSettings( bFOTAdownload, bFOTAUpdate );

    if ( eQCWWAN_ERR_NONE != nRet )
    {
        fprintf( stderr, "Not Able To Set SWIOMADM Setting\n" );
        fprintf( stderr, "Error Code : %ld \n",nRet );
        return;
    }

    fprintf(stderr, "SetSwiOmaDMSetting Successful\n");
    return;
}

void GetSwiOMADMSetting(void)
{
    SLQSOMADMSettings  slqsOMADMSettings;
    ULONG bOMADMEnabled;
    BYTE bFOTADownload;
    BYTE bFOTAUpdate;
    BYTE  autoSdm;
    BYTE  autoCheck;

    slqsOMADMSettings.pOMADMEnabled = &bOMADMEnabled;
    slqsOMADMSettings.pFOTAdownload = &bFOTADownload;
    slqsOMADMSettings.pFOTAUpdate = &bFOTAUpdate;
    slqsOMADMSettings.pAutosdm = &autoSdm;
    slqsOMADMSettings.pFwAutoCheck = &autoCheck;

    ULONG nRet  = SLQSOMADMGetSettings2( &slqsOMADMSettings );

    if ( eQCWWAN_ERR_NONE != nRet )
    {
        fprintf( stderr, "Not Able To Get SWIOMADM Setting\n" );
        fprintf( stderr, "Error Code : %ld \n",nRet );
        return;
    }

    fprintf(stderr, "GetSwiOmaDMSetting Successful\n");
    fprintf(stderr, "OMA-DM Enabled : 0x%08lx\n", *slqsOMADMSettings.pOMADMEnabled);
    fprintf(stderr, "FOTA Download : 0x%02x\n", *slqsOMADMSettings.pFOTAdownload);
    fprintf(stderr, "FOTA Update: 0x%02x\n", *slqsOMADMSettings.pFOTAUpdate);
    fprintf(stderr, "Auto UI Alert Response: 0x%02x\n", *slqsOMADMSettings.pAutosdm);
    fprintf(stderr, "Firmware Auto check response: 0x%02x\n", *slqsOMADMSettings.pFwAutoCheck);
    return;
}

/**
 *
 * main function for swioma dm for MC73xx modem
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
    bool fStartSession = false;
    bool fCancelInterval = false;
    bool fAcceptUpdate = false;
    bool fSetOMADM = false;
    bool fGetOMADM = false;
    ULONG sessionType;
    ULONG setOMADM;
    ULONG cancelInterval = DEFAULT_CANCEL_INTERVAL;
    bool bSessionStarted;
    int session_retry = 0;

    if (argc < 2) 
        goto dumpusage;

    else if (argc == 2)
    {
        if (0 != strcmp(argv[1], SwiOmaGetSettings_CMDLINE)) 
            goto dumpusage;
        else
            fGetOMADM = true;
    }

    else if (argc == 3)
    {
        if (0 == strcmp(argv[1], SwiOmaStartSession_CMDLINE))
        {
            fStartSession = true;
            sessionType = atoi(argv[2]);
            if ((sessionType < 1) || (sessionType > 3))
                goto dumpusage;            
        }
        else if (0 == strcmp(argv[1], SwiOmaSetSettings_CMDLINE))
        {            
            fSetOMADM = true;
            setOMADM = atoi(argv[2]);
            if ((setOMADM != 0) && (setOMADM != 1))
                goto dumpusage;
        }
        else
        {
            goto dumpusage;
        }
    }   
    else if ((argc == 4) || (argc == 6) || (argc == 8) || (argc > 9)) 
    {
        goto dumpusage;
    }
    else if (argc == 5)
    {
        if (0 == strcmp(argv[1], SwiOmaStartSession_CMDLINE))
        {
            fStartSession = true;
            sessionType = atoi(argv[2]);
            if ((sessionType < 1) || (sessionType > 3))
                goto dumpusage;       
        }
        else
        {
            goto dumpusage;
        }           
        
        if (0 == strcmp(argv[3], SwiOmaCancelInterval_CMDLINE))
        {            
            cancelInterval = atoi(argv[4]);
        }
        else if (0 == strcmp(argv[3], SwiOmaAceeptUpdate_CMDLINE))
        {            
            g_selection = atoi(argv[4]);
        }
        else
        {
           goto dumpusage;
        }
    }
    else if (argc == 7)
    {
        if (0 == strcmp(argv[1], SwiOmaStartSession_CMDLINE))
        {
            fStartSession = true;
            sessionType = atoi(argv[2]);
            if ((sessionType < 1) || (sessionType > 3))
                goto dumpusage;       
        }
        else
        {
            goto dumpusage;
        }           
        
        if (0 == strcmp(argv[3], SwiOmaCancelInterval_CMDLINE))
        {
            fCancelInterval = true;
            cancelInterval = atoi(argv[4]);
        }
        else if (0 == strcmp(argv[3], SwiOmaAceeptUpdate_CMDLINE))
        {
            fAcceptUpdate = true;
            g_selection = atoi(argv[4]);
        }
        else
        {
           goto dumpusage;
        }

        if (fCancelInterval)
        {
            if (0 == strcmp(argv[5], SwiOmaAceeptUpdate_CMDLINE))            
            {
                
                g_selection = atoi(argv[6]);
            }
            else
                goto dumpusage;
        }
        else if (fAcceptUpdate)
        {
           if (0 == strcmp(argv[5], SwiOmaCancelInterval_CMDLINE))
            {
                cancelInterval = atoi(argv[6]);
            }
            else
                goto dumpusage;
        }
        else
        {
           goto dumpusage;
        }
    }
    else
    {
        goto dumpusage;
    }

    qmiDeviceConnect();
    printf("\r\n");

    SetCallBack();

    if(fStartSession == true)
    {
        bSessionStarted = StartSession(sessionType);
        if (!bSessionStarted)
            goto quit;
    }
    else if (fSetOMADM == true)
    {
        SetSwiOMADMSetting(setOMADM);
        goto quit;
    }
    else if (fGetOMADM == true)
    {
        GetSwiOMADMSetting();
        goto quit; 
    }
    
    while (session_retry++ < MAX_SESSION_WAIT_RETRY) {
        sleep(cancelInterval);
        if((fStartSession) && (sessionType == 1))
            GetSessionInfo(sessionType);
        if (bSessionDone)
            break;        
    }    
    
    if ((bSessionDone == false) && (fStartSession == true))
        CancelSession(sessionType);

quit:
    ClearCallBack();
    qmiDeviceDisconnect();
    printf("\n");
    return 0;

dumpusage:
    usage(argv[0]);
    return -1;
}

