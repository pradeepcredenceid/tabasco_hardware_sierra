/*
 *  $Id: aptestgsmbasic.c,v 1.9 2010/06/10 00:26:12 epasheva Exp $
 *
 *  Filename: aptestgsmbasic.c
 *
 *  Purpose:  Contains functions implementing specific tests for GSM BASIC APIs
  *
 * Copyright: © 2008 Sierra Wireless, Inc., all rights reserved
 *
 */

/* Linux definitions */
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

/* include files */
#include "SwiDataTypes.h"
#include "SwiApiGsmBasic.h"
#include "sl/sludefs.h"

#include "apudefs.h"
#include "aptestdefs.h"

/* Defines */

/* global data */
swi_char fwver[256];
swi_char imei[32];

/*************
 *
 * Name:    mapBandType- Band type mapping table
 *
 * Purpose: This table maps band type enum received in CNS_BAND resp
 *          to a string that can be printed to the user.
 *
 *************/
char mapBandType[][20] =
    {{"NONE"},
     {"PGSM"},
     {"EGSM"},
     {"DCS"},
     {"PGSM-DCS"},
     {"EGSM-DCS"},
     {"PCS"},
     {"PGSM-PCS"},
     {"EGSM-PCS"},
     {"GSM850"},
     {"GSM850-PCS"},
     {"GSM ALL"},
     {"WCDMA-EU"},
     {"WCDMA-NA"},
     {"WCDMA-GSM-NA"},
     {"WCDMA-GSM-EU"},
     {"WCDMA-ALL"},
     {"WCDMA-AU"},
     {"WCDMA-JAP"},
     {"WCDMA-GSM-AU"},
     {"WCDMA-GSM-AU2"},
     {"WCDMA-GSM-AU3"}};
     
/**************
 *
 * Name:     doSwiGetImei
 *
 * Purpose:  Exercises the SwiGetImei API - calls the API and prints
 *           information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiGetImei(unsigned int timeout)
{
    SWI_RCODE retcode;
    struct SWI_STRUCT_Imei Imei;
    
    slmemset((char *)&Imei, 0, sizeof( struct SWI_STRUCT_Imei ));
    retcode = SwiGetImei( &Imei, timeout );

    if( retcode == SWI_RCODE_OK )
    {
        printf("\nModem IMEI     : %s\n", Imei.szImei );
        printf("Modem TAC      : %s\n", Imei.szTac );
        printf("Modem FAC      : %s\n", Imei.szFac );
        printf("Modem SNR      : %s\n", Imei.szSnr );
        printf("Modem CHECKBYTE: %s\n", Imei.szCheckByte );
    }
    else if (retcode == SWI_RCODE_SMALL_BUF)
        printf("Buffer too small!\n");
    else
        printf("\nGetImei returned error: %d\n", retcode );
        
}

/**************
 *
 * Name:     doSwiGetBandInfo
 *
 * Purpose:  Exercises the SwiGetBandInfo API - calls the API and prints
 *           information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiGetBandInfo(unsigned int timeout)
{
    SWI_RCODE retcode;
    static struct SWI_STRUCT_Band sBand;
    int i;
    char *bandnamep;

    retcode = SwiGetBandInfo( &sBand, sizeof(struct SWI_STRUCT_Band), timeout );
  
    if( retcode == SWI_RCODE_OK )
    {   
        if((sBand.eBandType= SWI_BAND_AUTO))
            printf("\nCurrent Band Type: AUTO\n");
        else
            printf("\nCurrent Band Type: %s\n", mapBandType[sBand.eBandType]);
        printf("BandInfo: # bands = %ld\n", sBand.cntValidBands );
        for (i = 0; i < sBand.cntValidBands;  ++i)
        {
            switch (sBand.arrBand[i])
            {
                case SWI_BAND_PGSM:
                    bandnamep  = "PGSM";
                break;
                case SWI_BAND_EGSM:
                    bandnamep  = "EGSM";
                break;
                case SWI_BAND_DCS:
                    bandnamep  = "DCS";
                break;
                case SWI_BAND_PGSM_DCS:
                    bandnamep  = "PGSM_DCS";
                break;
                case SWI_BAND_EGSM_DCS:
                    bandnamep  = "EGSM_DCS";
                break;
                case SWI_BAND_PCS:
                    bandnamep  = "PCS";
                break;
                case SWI_BAND_PGSM_PCS:
                    bandnamep  = "PGSM_PCS";
                break;
                case SWI_BAND_EGSM_PCS:
                    bandnamep  = "EGSM_PCS";
                break;
                case SWI_BAND_GSM850:
                    bandnamep  = "GSM850";
                break;
                case SWI_BAND_GSM850_PCS:
                    bandnamep  = "GSM850_PCS";
                break;
                case SWI_BAND_GSM_ALL:
                    bandnamep  = "GSM_ALL";
                break;
                case SWI_BAND_WCDMA_EU:
                    bandnamep  = "WCDMA_EU";
                break;
                case SWI_BAND_WCDMA_NA:
                    bandnamep  = "WCDMA_NA";
                break;
                case SWI_BAND_WCDMA_GSM_NA:
                    bandnamep  = "WCDMA_GSM_NA";
                break;
                case SWI_BAND_WCDMA_GSM_EU:
                    bandnamep  = "WCDMA_GSM_EU";
                break;
                case SWI_BAND_WCDMA_ALL:
                    bandnamep  = "WCDMA_ALL";
                break;
                case SWI_BAND_WCDMA_AU:
                    bandnamep  = "WCDMA_AU";
                break;
                case SWI_BAND_WCDMA_JAP:
                    bandnamep  = "WCDMA_JAP";
                break;
                case SWI_BAND_WCDMA_GSM_AU:
                    bandnamep  = "WCDMA_GSM_AU";
                break;
                case SWI_BAND_WCDMA_GSM_AU_2:
                    bandnamep  = "WCDMA_GSM_AU_2";
                break;
                case SWI_BAND_WCDMA_GSM_AU_3:
                    bandnamep  = "WCDMA_GSM_AU_3";
                break;
                case SWI_BAND_AUTO:
                    bandnamep  = "AUTO";
                break;
                
                case SWI_BAND_None:
                default:
                    bandnamep  = "None";
                break;
            }
   
            printf("  Band: %d = %d, %s\n", i, sBand.arrBand[i], bandnamep );
        }
    }
    else
        printf("\nSwiGetBandInfo returned error: %d\n", retcode );
        
}

/**************
 *
 * Name:     doSwiGetAvailableFeatures
 *
 * Purpose:  Exercises the SwiGetAvailableFeatures API - calls the API 
 *           and prints information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiGetAvailableFeatures(unsigned int timeout)
{
    SWI_RCODE retcode;
    static struct SWI_STRUCT_AvailFeatures sFeatures;
    char *pdptypep;

    /* Note that struct is uninitialized, so sometimes you may get invalid data. */
    /* If you are connected online, the data may get re-loaded after a couple 
     * of retries 
     */
    
    retcode = SwiGetAvailableFeatures( &sFeatures, 
                            sizeof(struct SWI_STRUCT_AvailFeatures), timeout );

    if( retcode == SWI_RCODE_OK )
    {
        printf("\nAvailable Features:\n");
        if (sFeatures.bVoice)
            printf ("  Voice available, %x\n", sFeatures.bVoice);
        else
            printf ("  No Voice available %x\n", sFeatures.bVoice);
            
        if (sFeatures.bTriBand)
            printf ("  Tri-band device, %x\n", sFeatures.bTriBand);
        else
            printf ("  Single-band device, %x\n", sFeatures.bTriBand);
        
        /* Print PPP type result in ASCII */
        switch (sFeatures.pdpType)
        {
            case SWI_PDP_IP:
                pdptypep = "IP";
                break;
            case SWI_PDP_PPP:
                pdptypep = "PPP";
                break;
            case SWI_PDP_IPV6:
                pdptypep = "IPV6";
                break;
            default:
                pdptypep = "None !";
                break;
        }
        printf ("  PDP type %i = %s\n", sFeatures.pdpType, pdptypep);
       
    }
    else
        printf("\nGetImei returned error: %d\n", retcode );
}


/**************
 *
 * Name:     doSwiGetRssiBer
 *
 * Purpose:  Runs the test to get the Rssi/Ber information from a GSM
 *           modem 
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiGetRssiBer(unsigned int timeout)
{
    SWI_RCODE               retcode;
    swi_uint32               numbars;
    struct SWI_STRUCT_Rssi  rssiber;

    /* Get the RSSI */
    retcode = SwiGetRssiBer( &rssiber, timeout );
    retcode = SwiGetRssiBars( rssiber.iRssi, &numbars );
    
    printf("\n");

    if( retcode == SWI_RCODE_OK )
    {
        printf("RSSI Info:\n");
        printf("  Modem RSSI:    -%lddBm\n", -(rssiber.iRssi) );
        printf("  No. of bars:    %ld\n", numbars );
        printf("  Modem BER:      %lx\n", rssiber.iBer );
    }
    else
        printf("SwiGetRssi returned error: %d\n", retcode );
}

/**************
 *
 * Name:     doSwiGetSerialNumber
 *
 * Purpose:  Exercises the SwiGetSerialNumber API - calls the API and prints
 *           information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiGetSerialNumber(unsigned int timeout)
{
    struct SWI_STRUCT_Fsn sFsn;
    SWI_RCODE retcode;

    sFsn.sizeStruct = LENGTH_Fsn;
    slmemset (&(sFsn.szFsn[0]), 0, sFsn.sizeStruct);   

    retcode = SwiGetSerialNumber(&sFsn, timeout );

    if( retcode == SWI_RCODE_OK )
        printf("\nFSN:    %s\n", sFsn.szFsn );
    else
        printf("\nSwiGetSerialNumber returned error: %d\n", retcode );
}

/**************
 *
 * Name:     doSwiGetTestHistoryByte
 *
 * Purpose:  Exercises the SwiGetTestHistoryByte API - calls the API and prints
 *           information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiGetTestHistoryByte(unsigned int timeout)
{
    swi_char lasterror[80];
    struct SWI_STRUCT_Thb sThb;
    SWI_RCODE retcode;

    sThb.sizeStruct = 4*(sizeof (swi_bool)) ;
    sThb.bInitialTest = 0xFF;   
    sThb.bRFScreen = 0xFF;   
    sThb.bCalibration = 0xFF;   
    sThb.bSystem = 0xFF;   

    retcode = SwiGetTestHistoryByte(&sThb, timeout );

    if( retcode == SWI_RCODE_OK )
    {
        printf("\nTest History bInitialTest: %x\n", sThb.bInitialTest );
        printf("             bRFScreen   : %x\n", sThb.bRFScreen );
        printf("             bCalibration: %x\n", sThb.bCalibration );
        printf("             bSystem     : %x\n", sThb.bSystem );
    }
    else
    {
        printf("\nSwiGetTestHistoryByte returned error: %d\n", retcode );
        /* In addition, print the error reported by the modem */
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
}

/**************
 *
 * Name:     doSwiGetSetModemDisable
 *
 * Purpose:  Exercises the SwiGetModemDisable, SwiSetModemDisable API
 *           calls the API and prints
 *           information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiGetSetModemDisable(unsigned int timeout)
{
    swi_char lasterror[80];
    swi_bool modemDisable = FALSE;
    SWI_RCODE retcode;
    pthread_t NotifThread2;
    int i;
    
    /* set up for notification */
    pthread_create (&NotifThread2, NULL, (void *)&doNotificationThread,
                    &timeout);
    retcode = SwiNotify(SWI_NOTIFY_ModemDisabled, timeout);
   
    for ( i = 0; i < 2; i++)
    {
        retcode = SwiGetModemDisable (&modemDisable, timeout );

        if( retcode == SWI_RCODE_OK )
        {
            printf("\nCurrent Modem Radio State: %s\n", 
                modemDisable?"Enable":"Disable" );
        }
        else
        {
            printf("\nSwiGetModemDisable returned error: %d\n", retcode );
            /* In addition, print the error reported by the modem */
            if( retcode == SWI_RCODE_REQUEST_REJECTED )
            {
                SwiGetLastError( lasterror, sizeof(lasterror) );
                printf("  Mdm reports CnS Error: %s\n", lasterror );
            }
        }
    
        if(modemDisable)
        {
            printf("Attempt to Disable Modem Radio\n");
            modemDisable = FALSE;
        }
        else
        {
            printf("Attempt to Enable Modem Radio\n");
            modemDisable = TRUE;
        }
    
        retcode = SwiSetModemDisable (modemDisable, timeout );

        if( retcode == SWI_RCODE_OK )
        {
            printf("Radio change is occurring...\n");
        }
        else
        {
            printf("\nSwiSetModemDisable returned error: %d\n", retcode );
            /* In addition, print the error reported by the modem */
            if( retcode == SWI_RCODE_REQUEST_REJECTED )
            {
                SwiGetLastError( lasterror, sizeof(lasterror) );
                printf("  Mdm reports CnS Error: %s\n", lasterror );
            }   
        }
        sleep(5);
    }
}

/**************
 *
 * Name:     doSwiGetFeatureCustomizations
 *
 * Purpose:  Exercises the doSwiGetFeatureCustomizations API - calls the API 
 *           and prints information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiGetFeatureCustomizations(unsigned int timeout)
{
    SWI_RCODE retcode;
    static struct SWI_STRUCT_FeatureCustomizations sFeatureCust;
    
    /* Note that struct is uninitialized, so sometimes you may get invalid data. */
    /* If you are connected online, the data may get re-loaded after a couple of retries */
    
    retcode = SwiGetFeatureCustomizations( &sFeatureCust, timeout );

    if( retcode == SWI_RCODE_OK )
    {
        printf("\nFeature Customizations:\n");
        printf("  Version:           %ld\n", sFeatureCust.nVersion);
        printf("  bAutoConnect:      %i\n", sFeatureCust.bAutoConnect);
        printf("  bAdvanceProfile:   %i\n", sFeatureCust.bAdvanceProfile);
        printf("  bDisplayDun    :   %i\n", sFeatureCust.bDisplayDun);
        printf("  bHideByteCounters: %i\n", sFeatureCust.bHideByteCounters);
        printf("  bAllowCsd:         %i\n", sFeatureCust.bAllowCsd);
        printf("  bHSDPASupport:     %i\n", sFeatureCust.bHSDPASupport);
        printf("  bScanProfile:      %i\n", sFeatureCust.bScanProfile);
        printf("  bBluetooth:        %i\n", sFeatureCust.bBluetooth);
        printf("  bGps:              %i\n", sFeatureCust.bGps);
     }
}

/**************
 *
 * Name:     doSwiStopAllNotif
 *
 * Purpose:  Exercises the SwiStopAllNotif API 
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    The following notifications are enabled by default and can not 
 *            be disabled:
 *
 * SWI_NOTIFY_AirServerChange        
 *
 **************/
package void doSwiStopAllNotif(unsigned int timeout)
{
    SWI_RCODE retcode;

    retcode = SwiStopAllNotif( timeout );

    if( retcode == SWI_RCODE_OK )
        printf("\nStop All Notifications processed\n");
    else
        printf("\nStop All Notifications failed: %d\n", retcode );
}

/**************
 *
 * Name:     doSwiGetSwocCfg
 *
 * Purpose:  Exercises the doSwiGetFeatureCustomizations API - calls the API
 *           and prints information related to the API
 *
 * Parms:    timeout   - timeout value for stop and wait functions as
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiGetSwocCfg(unsigned int timeout)
{
    SWI_RCODE retcode;
    struct SWI_STRUCT_GetSwocCfg sSwocCfg;
    swi_char lasterror[80];

    /* Note that struct is uninitialized, so sometimes you may get invalid data. */
    /* If you are connected online, the data may get re-loaded after a couple of retries */

    retcode = SwiGetSwocCfg( &sSwocCfg, timeout );

    if( retcode == SWI_RCODE_OK )
    {
        printf("\nSWOC Config Features:\n");
        printf("  Support:       %i\n", sSwocCfg.FeatureSupport);
        printf("  State:         %i\n", sSwocCfg.FeatureState);
        printf("  Capabilities:  %i\n", sSwocCfg.FeatureCap);
        printf("  Lock Required: %i\n", sSwocCfg.LockReq);
    }
    else
    {
        printf("\nSwiGetSwocCfg returned error: %u\n", retcode );
        /* In addition, print the error reported by the modem */
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }

}

/*
 * $Log: aptestgsmbasic.c,v $
 */
