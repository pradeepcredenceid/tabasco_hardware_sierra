/*
 *  $Id: aptestcmbasic.c,v 1.18 2010/06/10 00:26:12 epasheva Exp $
 *
 *  Filename: aptestcmbasic.c
 *
 *  Purpose:  Contains functions for testing Common Basic APIs
 *
 * Copyright: © 2008 Sierra Wireless, Inc., all rights reserved
 *
 */

/* Linux definitions */
#include <stdio.h>

/* include files */
#include "SwiDataTypes.h"
#include "sl/sludefs.h"
#include "apudefs.h"
#include "aptestdefs.h"

/* global data */
swi_char fwver[256];
swi_char fwdate[20];
swi_char hwver[256];
swi_char bootver[256];
swi_char bootdate[20];

/* last received CnS error buffer */
local swi_char lasterror[80];
/*************
 *
 * Name:    appowermodemap- Radio Power Mode mapping table
 *
 * Purpose: This table maps Power Mode enum received in CNS_RADIO_POWER resp
 *          to a string that can be printed to the user.
 *
 *************/
char appowermodemap[][20] = 
  {{"Low Power"}, 
   {"Online"}};

/*************
 *
 * Name:    appowerreasonmap - Radio Power Reason mapping table
 *
 * Purpose: This table maps Mode Reason enum received in CNS_RADIO_POWER resp
 *          to a string that can be printed to the user.
 *
 *************/
char appowerreasonmap[][20] = 
  {{"W_DISABLE asserted"},
   {"User request"},
   {"Out of temp range"},
   {"Out of volt range"}};

/*************
 *
 * Name:    apradiotempmap - Radio Temperature State mapping table
 *
 * Purpose: This table maps Temperature State enum received in 
 *          CNS_RADIO_TEMPERATURE_STATE resp to a string that can be 
 *          printed to the user.
 *
 *************/
char apradiotempmap[][20] = 
  {{"Normal"},
   {"High Temp Warning"},
   {"High Temp Critical"},
   {"Low Temp Critical"}};

/*************
 *
 * Name:    apradiobattmap- Radio Battery State mapping table
 *
 * Purpose: This table maps Battery State enum received in 
 *          CNS_RADIO_BATTERY resp to a string that can be 
 *          printed to the user.
 *
 *************/
char apradiobattmap[][20] = 
  {{"Normal"},
   {"Low BATT Warning"},
   {"Low BATT Critical"},
   {"High BATT Critical"}};


/*************
 *
 * Name:    apMapRadioType - Radio Type mapping table
 *
 * Purpose: This table maps radio type enum received in 
 *          CNS_CURRENT_RADIO_BAND resp to a string that can be 
 *          printed to the user.
 *
 *************/
char apMapRadioType[][20] =
{ {"1xRTT"},
  {"EVDO"},
  {"GSM/WCDMA"}};
  
/*************
 *
 * Name:    apMapRadioCDMABand - CDMA RadioBand Type mapping table
 *
 * Purpose: This table maps CDMA portion of the radio band enum received in 
 *          CNS_CURRENT_RADIO_BAND resp to a string that can be 
 *          printed to the user.
 *
 *************/
char apMapRadioCDMABand[][20] =
{ {"CDMA_CLASS0"},
  {"CDMA_CLASS1"},
  {"CDMA_CLASS2"},
  {"CDMA_CLASS3"},
  {"CDMA_CLASS4"},
  {"CDMA_CLASS5"},
  {"CDMA_CLASS6"},
  {"CDMA_CLASS7"},
  {"CDMA_CLASS8"},
  {"CDMA_CLASS9"},
  {"CDMA_CLASS10"},
  {"CDMA_CLASS11"},
  {"CDMA_CLASS12"},
  {"CDMA_CLASS5"},
  {"CDMA_CLASS6"},
  {"CDMA_CLASS7"},
  {"CDMA_CLASS8"}};
  
/*************
 *
 * Name:    apMapRadioGSMBand - GSM RadioBand Type mapping table
 *
 * Purpose: This table maps GSM portion of the radio band enum received in 
 *          CNS_CURRENT_RADIO_BAND resp to a string that can be 
 *          printed to the user.
 *
 *************/
char apMapRadioGSMBand[][20] = 
 {{"GSM450"},
  {"GSM480"},
  {"GSM750"},
  {"GSM850"},
  {"GSM/EGSM900"},
  {"GSM/PGSM900"},
  {"GSM/RGSM900"},
  {"GSM/DCS1800"},
  {"GSM/PCS1900"}};
  
/*************
 *
 * Name:    apMapRadioWCDMABand - WCDMA RadioBand Type mapping table
 *
 * Purpose: This table maps WCDMA portion of the radio band enum received in 
 *          CNS_CURRENT_RADIO_BAND resp to a string that can be 
 *          printed to the user.
 *
 *************/
char apMapRadioWCDMABand[][20] = 
{ {"WCDMA_I_IMT2000"},
  {"WCDMA_II_PCS1900"},
  {"WCDMA_III_DCS1800"},
  {"WCDMA_IV_1700"},
  {"WCDMA_V_850"},
  {"WCDMA_VI_800"},
  {"WCDMA_VII_2600"},
  {"WCDMA_VIII_900"},
  {"WCDMA_IX_1700"}};
  
/*************
 *
 * Name:    apMapObjRngRev - CnS Object Range Revision Types mapping table
 *
 * Purpose: This table maps CnS Object Range Revision enum received in 
 *          CNS_OBJECT_RANGE_REVISION resp to a string that can be 
 *          printed to the user.
 *
 *************/
char apMapObjRngRev[][5] =
{{"CWE"},
 {"LBS"},
 {"GPRS"},
 {"CDMA"},
 {"MP"},
 {"UMTS"},
 {"DM"}};
 
/*************
 *
 * Name:    apMapTechType - HIP device Info request mapping table
 *
 * Purpose: This table maps HIP device Info request enum received in 
 *          HIP Device Information resp to a string that can be 
 *          printed to the user.
 *
 *************/
char apMapTechType[][8] =
{{"Unknown"},
 {"CDMA"},
 {"GSM"}
};

/*************
 *
 * Name:    apMapCNSsupported - HIP device Info request mapping table
 *
 * Purpose: This table maps HIP device Info request enum received in 
 *          HIP Device Information resp to a string that can be 
 *          printed to the user.
 *
 *************/
char apMapCNSsupported[][4] =
{{"YES"},
 {"NO"}
};
 
/* Functions */

/**************
 *
 * Name:     apMapRadioBandConfig
 *
 * Purpose:  This function maps the Radio Band Group constants to a string.
 *           
 * Parms:    bandgroup   - band group value
 * 
 * Return:   pointer to string representation of the radio band group setting.
 *
 * Notes:    None
 *
 **************/
local char *apMapRadioBandConfig(swi_uint64 bandgroup)
{
    static char bandgroupstring[25];
    
    slmemset(bandgroupstring, 0, 25);
    
    switch(bandgroup)
    {
        case SWI_RADIOBANDGROUP_CdmaClass0A:
            slstrncpy(bandgroupstring, "CDMA CLASS 0A", 25);
            break;
        case SWI_RADIOBANDGROUP_CdmaClass0B:
            slstrncpy(bandgroupstring,  "CDMA CLASS 0B", 25);
            break;
        case SWI_RADIOBANDGROUP_CdmaClass0ABo:
            slstrncpy(bandgroupstring, "CDMA CLASS 0AB0", 25);
            break;
        case SWI_RADIOBANDGROUP_CdmaClass1:
            slstrncpy(bandgroupstring, "CDMA CLASS 1", 25);
            break;
        case SWI_RADIOBANDGROUP_CdmaClass2:
            slstrncpy(bandgroupstring,  "CDMA CLASS 2", 25);
            break;
        case SWI_RADIOBANDGROUP_CdmaClass3:
            slstrncpy(bandgroupstring,  "CDMA CLASS 3", 25);
            break;
        case SWI_RADIOBANDGROUP_CdmaClass4:
            slstrncpy(bandgroupstring,  "CDMA CLASS 4", 25);
            break;
        case SWI_RADIOBANDGROUP_CdmaClass5:
            slstrncpy(bandgroupstring,  "CDMA CLASS 5", 25);
            break;
        case SWI_RADIOBANDGROUP_CdmaClass6:
            slstrncpy(bandgroupstring,  "CDMA CLASS 6", 25);
            break;
        case SWI_RADIOBANDGROUP_CdmaClass7:
            slstrncpy(bandgroupstring,  "CDMA CLASS 7", 25);
            break;
        case SWI_RADIOBANDGROUP_CdmaClass8:
            slstrncpy(bandgroupstring,  "CDMA CLASS 8", 25);
            break;
        case SWI_RADIOBANDGROUP_CdmaClass9:
            slstrncpy(bandgroupstring,  "CDMA CLASS 9", 25);
            break;
        case SWI_RADIOBANDGROUP_CdmaClass10:
            slstrncpy(bandgroupstring,  "CDMA CLASS 10", 25);
            break;
        case SWI_RADIOBANDGROUP_CdmaClass11:
            slstrncpy(bandgroupstring,  "CDMA CLASS 11", 25);
            break;
        case SWI_RADIOBANDGROUP_CdmaClass12:
            slstrncpy(bandgroupstring,  "CDMA CLASS 12", 25);
            break;
        case SWI_RADIOBANDGROUP_CdmaPcsCellular:
            slstrncpy(bandgroupstring,  "CDMA PCS Cellular", 25);
            break;
        case SWI_RADIOBANDGROUP_Gsm450:
            slstrncpy(bandgroupstring, "GSM 450", 25);
            break;
        case SWI_RADIOBANDGROUP_Gsm480:
            slstrncpy(bandgroupstring,  "GSM 480", 25);
            break;
        case SWI_RADIOBANDGROUP_Gsm750:
            slstrncpy(bandgroupstring,  "GSM 750", 25);
            break;
        case SWI_RADIOBANDGROUP_Gsm850:
            slstrncpy(bandgroupstring,  "GSM 850", 25);
            break;
        case SWI_RADIOBANDGROUP_GsmRGsm900:
            slstrncpy(bandgroupstring, "RGSM 900", 25);
            break;
        case SWI_RADIOBANDGROUP_GsmPcs1900:
            slstrncpy(bandgroupstring,  "GSM PCS 1900 ", 25);
            break;
        case SWI_RADIOBANDGROUP_GsmDcs1800:
            slstrncpy(bandgroupstring,  "GSM DCS 1800", 25);
            break;
        case SWI_RADIOBANDGROUP_GsmEGsm900:
            slstrncpy(bandgroupstring,  "GSM EGSM 900", 25);
            break;
        case (SWI_RADIOBANDGROUP_GsmPGsm900):
            slstrncpy(bandgroupstring,  "GSM PGSM 900", 25);
            break;
        case SWI_RADIOBANDGROUP_GsmPcs850:
            slstrncpy(bandgroupstring,  "GSM PCS 850", 25);
            break;
        case SWI_RADIOBANDGROUP_GsmEGsmDcs:
            slstrncpy(bandgroupstring,  "GSM EGSM DCS", 25);
            break;
        case SWI_RADIOBANDGROUP_GsmAll:
            slstrncpy(bandgroupstring,  "GSM ALL", 25);
            break;
        case SWI_RADIOBANDGROUP_WcdmaIImt2000:
            slstrncpy(bandgroupstring,  "WCDMA I: IMT 2000", 25);
            break;
        case SWI_RADIOBANDGROUP_WcdmaIIPcs1900:
            slstrncpy(bandgroupstring,  "WCDMA II: PCS 1900", 25);
            break;
        case SWI_RADIOBANDGROUP_WcdmaIII1700:
            slstrncpy(bandgroupstring,  "WCDMA III: 1700", 25);
            break;
        case SWI_RADIOBANDGROUP_WcdmaIV1700:
            slstrncpy(bandgroupstring,  "WCDMA IV: 1700", 25);
            break;
        case SWI_RADIOBANDGROUP_WcdmaV850:
            slstrncpy(bandgroupstring,  "WCDMA V: 850", 25);
            break;
        case SWI_RADIOBANDGROUP_WcdmaVI800:
            slstrncpy(bandgroupstring,  "WCDMA VI: 800", 25);
            break;
        case SWI_RADIOBANDGROUP_WcdmaVII2600:
            slstrncpy(bandgroupstring,  "WCDMA VII: 2600", 25);
            break;
        case SWI_RADIOBANDGROUP_WcdmaVIII900:
            slstrncpy(bandgroupstring,  "WCDMA VIII: 900", 25);
            break;
        case SWI_RADIOBANDGROUP_WcdmaIX1700:
            slstrncpy(bandgroupstring,  "WCDMA IX: 1700", 25);
            break;
        case SWI_RADIOBANDGROUP_WcdmaNa:
            slstrncpy(bandgroupstring,  "WCDMA: NA", 25);
            break;
        case SWI_RADIOBANDGROUP_WcdmaGsmEu:
            slstrncpy(bandgroupstring,  "WCDMA GSM EU", 25);
            break;
        case SWI_RADIOBANDGROUP_WcdmaGsmNa:
            slstrncpy(bandgroupstring,  "WCDMA GSM NA", 25);
            break;
        case SWI_RADIOBANDGROUP_WcdmaAustralia:
            slstrncpy(bandgroupstring,  "WCDMA AUSTRALIA", 25);
            break;
        case SWI_RADIOBANDGROUP_WcdmaAustrailiaGsmEu:
            slstrncpy(bandgroupstring,  "WCDMA AUSTRAILIA GSM EU", 25);
            break;
        case SWI_RADIOBANDGROUP_WcdmaJapan:
            slstrncpy(bandgroupstring,  "WCDMA JAPAN", 25);
            break;
        case SWI_RADIOBANDGROUP_WcdmaAll:
            slstrncpy(bandgroupstring,  "WCDMA ALL", 25);
            break;
        case SWI_RADIOBANDGROUP_AutoBand:
            slstrncpy(bandgroupstring,  "AUTO BAND", 25);
            break;
        default:
            slstrncpy(bandgroupstring,  "Unknown Band Group", 25);
            break;
    }
    return(&bandgroupstring[0]);
}
/**************
 *
 * Name:     doSwiGetFirmwareVersion
 *
 * Purpose:  Exercises the SwiGetFirmwareVersion API - calls the API and prints
 *           information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiGetFirmwareVersion(unsigned int timeout)
{
    SWI_RCODE retcode;

    /*
     * TEST CASE A: FIRMWARE VERSION fetched using standard size buffer 
     */
    printf("\nTest A: successful fetching of firmware version\n");

    slmemset(fwver, 0, sizeof( fwver));
    retcode = SwiGetFirmwareVersion( fwver, sizeof( fwver ), timeout );

    if( retcode == SWI_RCODE_OK )
    {
        if(!slstrlen(&fwver[0]))
            printf("No Modem Firmware Version present\n");
        else
            printf("Modem Firmware Version:\n    %s\n\n", fwver );
    }
    else if (retcode == SWI_RCODE_SMALL_BUF)
        printf("Buffer too small!\n");
    else
        printf("GetFwVersion returned error: %d\n", retcode );
        
    /*
     * TEST CASE B; FIRMWARE VERSION requested with NULL buffer pointer 
     */
    printf("\nTest B: Empty firmware version buffer given\n");

    slmemset(fwver, 0, sizeof( fwver));
    retcode = SwiGetFirmwareVersion( NULL, 0, timeout );

    if( retcode == SWI_RCODE_OK )
    {
        if(!slstrlen(&fwver[0]))
            printf("No Modem Firmware Version present\n");
        else
            printf("Modem Firmware Version:\n    %s\n\n", fwver );
    }
    else if (retcode == SWI_RCODE_SMALL_BUF)
        printf("Buffer too small!\n");
    else
        printf("GetFwVersion returned error: %d\n", retcode );


    /*
     * TEST CASE C: FIRMWARE VERSION requested with small buffer
     *              expect the version string to be truncated
     */
    printf("\nTest C: Small firmware version buffer given\n");

    slmemset(fwver, 0, sizeof( fwver));
    retcode = SwiGetFirmwareVersion( fwver, 10, timeout );

    if( retcode == SWI_RCODE_OK )
    {
        if(!slstrlen(&fwver[0]))
            printf("No Modem Firmware Version present\n");
        else
            printf("Modem Firmware Version:\n    %s\n\n", fwver );
    }
    else if (retcode == SWI_RCODE_SMALL_BUF)
        printf("Buffer too small!\n");
    else
        printf("GetFwVersion returned error: %d\n", retcode );

}
/**************
 *
 * Name:     doSwiGetFirmwareBuildDate
 *
 * Purpose:  Exercises the SwiGetFirmwareBuildDate API - calls the API and 
 *           prints information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiGetFirmwareBuildDate(unsigned int timeout)
{
    SWI_RCODE retcode;

    printf("\nTEST A: Fetch Firmware Build Date\n");
    slmemset(fwdate, 0, sizeof(fwdate));
    retcode = SwiGetFirmwareBuildDate( fwdate, sizeof( fwdate ), timeout );
    if( retcode == SWI_RCODE_OK )
    {
        if(!slstrlen(&fwdate[0]))
            printf("No Modem Firmware Build Date present\n");
        else
            printf("Modem Firmware Build Date:\n    %s\n\n", fwdate );
    }
    else if (retcode == SWI_RCODE_SMALL_BUF)
        printf("Buffer too small!\n");
    else
        printf("GetFwBuildDate returned error: %d\n", retcode );

    printf("\nTEST B: Attempt to fetch Firmware Build Date with NULL buffer\n");
    slmemset(fwdate, 0, sizeof(fwdate));
    retcode = SwiGetFirmwareBuildDate( NULL, 0, timeout );
    if( retcode == SWI_RCODE_OK )
    {
        if(!slstrlen(&fwdate[0]))
            printf("No Modem Firmware Build Date present\n");
        else
            printf("Modem Firmware Build Date:\n    %s\n\n", fwdate );
    }
    else if (retcode == SWI_RCODE_SMALL_BUF)
        printf("Buffer too small!\n");
    else
        printf("GetFwBuildDate returned error: %d\n", retcode );
        
    printf("\nTEST C: Attempt to fetch Firmware Build Date with small buffer\n");
    slmemset(fwdate, 0, sizeof(fwdate));
    retcode = SwiGetFirmwareBuildDate( fwdate, 3, timeout );
    if( retcode == SWI_RCODE_OK )
    {
        if(!slstrlen(&fwdate[0]))
            printf("No Modem Firmware Build Date present\n");
        else
            printf("Modem Firmware Build Date:\n    %s\n\n", fwdate );
    }
    else if (retcode == SWI_RCODE_SMALL_BUF)
        printf("Buffer too small!\n");
    else
        printf("GetFwBuildDate returned error: %d\n", retcode );
}
/**************
 *
 * Name:     doSwiGetHardwareVersion
 *
 * Purpose:  Exercises the SwiGetHardwareVersion API - calls the API and prints
 *           information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiGetHardwareVersion(unsigned int timeout)
{
    SWI_RCODE retcode;

    printf("\nTest A: Fetch Hardware Version\n");
    slmemset(hwver, 0, sizeof( hwver));
    retcode = SwiGetHardwareVersion( hwver, sizeof( hwver ), timeout );

    if( retcode == SWI_RCODE_OK )
    {
        if(!slstrlen(&hwver[0]))
            printf("No Modem Hardware Version present\n");
        else
            printf("Modem Hardware Version:\n    %s\n", hwver );
    }
    else if (retcode == SWI_RCODE_SMALL_BUF)
        printf("Buffer too small!\n");
    else
        printf("GetHwVersion returned error: %d\n", retcode );

    printf("\nTest B: Attempt to fetch Hardware Version with NULL buffer\n");
    slmemset(hwver, 0, sizeof( hwver));
    retcode = SwiGetHardwareVersion( NULL, 0, timeout );

    if( retcode == SWI_RCODE_OK )
    {
        if(!slstrlen(&hwver[0]))
            printf("No Modem Hardware Version present\n");
        else
            printf("Modem Hardware Version:\n    %s\n", hwver );
    }
    else if (retcode == SWI_RCODE_SMALL_BUF)
        printf("Buffer too small!\n");
    else
        printf("GetHwVersion returned error: %d\n", retcode );
        
    printf("\nTest C: Attempt to fetch Hardware Version in small buffer\n");
    slmemset(hwver, 0, sizeof( hwver));
    retcode = SwiGetHardwareVersion( hwver, 1, timeout );

    if( retcode == SWI_RCODE_OK )
    {
        if(!slstrlen(&hwver[0]))
            printf("No Modem Hardware Version present\n");
        else
            printf("Modem Hardware Version:\n    %s\n", hwver );
    }
    else if (retcode == SWI_RCODE_SMALL_BUF)
        printf("Buffer too small!\n");
    else
        printf("GetHwVersion returned error: %d\n", retcode );
}
/**************
 *
 * Name:     doSwiGetBootVersion
 *
 * Purpose:  Exercises the SwiGetBootVersion API - calls the API and prints
 *           information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiGetBootVersion(unsigned int timeout)
{
    SWI_RCODE retcode;

    printf("\nTEST A: Fetch Boot Version\n");
    slmemset(bootver, 0, sizeof(bootver));
    retcode = SwiGetBootVersion( bootver, sizeof( bootver ), timeout );

    if( retcode == SWI_RCODE_OK )
    {
        if(!slstrlen(&bootver[0]))
            printf("No Modem Boot Version present\n");
        else
            printf("Modem Boot Version:\n    %s\n\n", bootver );
    }
    else if (retcode == SWI_RCODE_SMALL_BUF)
        printf("Buffer too small!\n");
    else
        printf("GetBootVersion returned error: %d\n", retcode );

    printf("\nTEST B: Attempt to fetch Boot Version with NULL buffer pointer\n");
    slmemset(bootver, 0, sizeof(bootver));
    retcode = SwiGetBootVersion( NULL, 0, timeout );

    if( retcode == SWI_RCODE_OK )
    {
        if(!slstrlen(&bootver[0]))
            printf("No Modem Boot Version present\n");
        else
            printf("Modem Boot Version:\n    %s\n\n", bootver );
    }
    else if (retcode == SWI_RCODE_SMALL_BUF)
        printf("Buffer too small!\n");
    else
        printf("GetBootVersion returned error: %d\n", retcode );
        
    printf("\nTEST C: Attempt to fetch Boot Version with small buffer\n");
    slmemset(bootver, 0, sizeof(bootver));
    retcode = SwiGetBootVersion( bootver, 8, timeout );

    if( retcode == SWI_RCODE_OK )
    {
        if(!slstrlen(&bootver[0]))
            printf("No Modem Boot Version present\n");
        else
            printf("Modem Boot Version:\n    %s\n\n", bootver );
    }
    else if (retcode == SWI_RCODE_SMALL_BUF)
        printf("Buffer too small!\n");
    else
        printf("GetBootVersion returned error: %d\n", retcode );
}
/**************
 *
 * Name:     doSwiGetBootloaderBuildDate
 *
 * Purpose:  Exercises the SwiGetBootloaderBuildDate API - calls the API and 
 *           prints information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiGetBootloaderBuildDate(unsigned int timeout)
{
    SWI_RCODE retcode;
    swi_uint32 length = sizeof(bootdate);

    printf("\nTest A: Fetch BootLoader Build Date\n");
    slmemset(bootdate, 0, sizeof(bootdate));
    retcode = SwiGetBootloaderBuildDate( 
                 bootdate, &length, timeout );

    if( retcode == SWI_RCODE_OK )
    {
        if(!slstrlen(&bootdate[0]))
            printf("No Modem Boot Build Date present\n");
        else
            printf("Modem Bootloader Build Date:\n    %s\n\n", bootdate );
    }
    else if (retcode == SWI_RCODE_SMALL_BUF)
        printf("Buffer too small!\n");
    else
        printf("GetBootloaderBuildDate returned error: %d\n", retcode );

    printf("\nTest B: Attempt to fetch BootLoader Build Date with NULL buffer\n");
    slmemset(bootdate, 0, sizeof(bootdate));
    length = 0;
    retcode = SwiGetBootloaderBuildDate( 
                 NULL, &length, timeout );

    if( retcode == SWI_RCODE_OK )
    {
        if(!slstrlen(&bootdate[0]))
            printf("No Modem Boot Build Date present\n");
        else
            printf("Modem Bootloader Build Date:\n    %s\n\n", bootdate );
    }
    else if (retcode == SWI_RCODE_SMALL_BUF)
        printf("Buffer too small!\n");
    else
        printf("GetBootloaderBuildDate returned error: %d\n", retcode );
        
    printf("\nTest C: Attempt to fetch BootLoader Build Date with small buffer\n");
    slmemset(bootdate, 0, sizeof(bootdate));
    length = 3;
    retcode = SwiGetBootloaderBuildDate( 
                 bootdate, &length, timeout );

    if( retcode == SWI_RCODE_OK )
    {
        if(!slstrlen(&bootdate[0]))
            printf("No Modem Boot Build Date present\n");
        else
            printf("Modem Bootloader Build Date:\n    %s\n\n", bootdate );
    }
    else if (retcode == SWI_RCODE_SMALL_BUF)
        printf("Buffer too small!\n");
    else
        printf("GetBootloaderBuildDate returned error: %d\n", retcode );
}

/**************
 *
 * Name:     doSwiGetUsbdInfo
 *
 * Purpose:  Exercises the SwiGetBootloaderBuildDate API - calls the API and 
 *           prints information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiGetUsbdInfo(unsigned int timeout)
{
    SWI_RCODE retcode;
    struct SWI_STRUCT_UsbdInfo  UsbdInfo;
    swi_char lasterror[80];
    
    retcode = SwiGetUsbdInfo(&UsbdInfo, timeout);

    if( retcode == SWI_RCODE_OK )
    {
        printf("\nUsbd Info:\n    %s\n\n", UsbdInfo.szVersion );
    }
    else
    {
        printf("\nSwiGetUsbdInfo returned error: %d\n", retcode );
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }

}

/**************
 *
 * Name:     doSwiGetPriInfo
 *
 * Purpose:  Exercises the SwiGetPriInfo API - calls the API and 
 *           prints information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiGetPriInfo(unsigned int timeout)
{
    SWI_RCODE retcode;
    struct SWI_STRUCT_PriInfo  PriInfo;
    
    retcode = SwiGetPriInfo(&PriInfo, timeout);

    if( retcode == SWI_RCODE_OK )
    {
        printf("\nPri Info:\n    %d.%d.%d.%d. SKUID:%ld\n", 
               (int)PriInfo.nMajorVer, 
               (int)PriInfo.nMinorVer, 
               (int)PriInfo.nSubMinorVer, 
               (int)PriInfo.nSubOemVer, 
               PriInfo.nSkuId);
    }
    else
        printf("\nSwiGetPriInfo returned error: %d\n", retcode );

}

/**************
 *
 * Name:     doSwiGetModemType
 *
 * Purpose:  Exercises the SwiGetModemType API - calls the API and 
 *           prints information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiGetModemType(unsigned int timeout)
{
    SWI_RCODE retcode;
    struct SWI_STRUCT_ModemType  ModemType;
    
    printf("\nTest A: Fetch Modem Type Info\n");
    retcode = SwiGetModemType(&ModemType, timeout);

    if( retcode == SWI_RCODE_OK )
    {
        printf("    Modem Type: 0x%x\n", ModemType.nModemType); 
        printf("    Modem Wireless Technology: %s\n", (ModemType.nWirelessTech==2)?"GSM/UMTS":"CDMA/EVDO");
        printf("    Modem Product String: %s\n", ModemType.szProductName);
    }
    else
        printf("\nModem Type returned error: %d\n", retcode );

}

/**************
 *
 * Name:     doSwiGetDeviceTime
 *
 * Purpose:  Exercises the SwiGetDeviceTime API - calls the API and 
 *           prints information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiGetDeviceTime(unsigned int timeout)
{
    SWI_RCODE retcode;
    struct SWI_STRUCT_DeviceTime DeviceTime;
    swi_char lasterror[80];
    swi_int32 tz;
    /* Note: This test requires the modem to be connected to a network,
     * i.e., SIM card is required
     */
    retcode = SwiGetDeviceTime(&DeviceTime, timeout);

    if( retcode == SWI_RCODE_OK )
    {
        printf("\nDevice Time:\n"); 
        printf("  Year     : %d\n", (int)DeviceTime.nYear); 
        printf("  Month    : %d\n", (int)DeviceTime.eMonth);
        printf("  Day      : %d\n", (int)DeviceTime.nDay);
        printf("  Weekday  : %d\n", (int)DeviceTime.eWeekDay);
        printf("  Hour     : %d\n", (int)DeviceTime.nHour);
        printf("  Minute   : %d\n", (int)DeviceTime.nMinute);
        printf("  Second   : %d\n", (int)DeviceTime.nSecond);
        if ((DeviceTime.nTimeZone & 0x8000) == 0X8000)
        {
            tz = ((0 - (0xFFFF - DeviceTime.nTimeZone) - 1 ) * 15)/60 ;
            printf("  Time Zone: %d \n", (int)tz );
        }
        else
        {
            tz = (DeviceTime.nTimeZone* 5)/60 ;
            printf("  Time Zone: %d\n", (int)tz);
        }
        printf("  Daylight Savings: %d\n", DeviceTime.eDaylightSavings);
    }
    else
    {
        printf("\nDevice Time returned error: %d\n", retcode );
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }

}

/**************
 *
 * Name:     doSwiGetDeviceID
 *
 * Purpose:  Exercises the SwiGetDeviceID API - calls the API and 
 *           prints information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiGetDeviceID(unsigned int timeout)
{
    SWI_RCODE retcode;
    swi_char deviceid[32];;
    
    printf("\nTEST A: Fetch Device ID\n");
    slmemset(deviceid, 0, sizeof(deviceid));
    retcode = SwiGetDeviceID(deviceid, sizeof (deviceid), timeout);

    if( retcode == SWI_RCODE_OK )
        printf("Device ID: %s\n", deviceid); 
    else if (retcode == SWI_RCODE_SMALL_BUF)
        printf("Buffer too small!\n");
    else
        printf("Failed to fetch Device ID  retcode: %d\n", retcode );

    printf("\nTEST B: Attempted to fetch Device ID with NULL buffer\n");
    slmemset(deviceid, 0, sizeof(deviceid));
    retcode = SwiGetDeviceID(NULL, 0, timeout);

    if( retcode == SWI_RCODE_OK )
        printf("Device ID: %s\n", deviceid); 
    else if (retcode == SWI_RCODE_SMALL_BUF)
        printf("Buffer too small!\n");
    else
        printf("Failed to fetch Device ID  retcode: %d\n", retcode );
        
    printf("\nTEST C: Attempted to fetch Device ID with small buffer\n");
    slmemset(deviceid, 0, sizeof(deviceid));
    retcode = SwiGetDeviceID(deviceid, 5, timeout);

    if( retcode == SWI_RCODE_OK )
        printf("Device ID: %s\n", deviceid); 
    else if (retcode == SWI_RCODE_SMALL_BUF)
        printf("Buffer too small!\n");
    else
        printf("Failed to fetch Device ID  retcode: %d\n", retcode );
}

/**************
 *
 * Name:     doSwiGetFlashImageInfo
 *
 * Purpose:  Exercises the SwiGetFlashImgInfo API - calls the API and 
 *           prints information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiGetFlashImageInfo(unsigned int timeout)
{
    SWI_RCODE retcode;
    struct SWI_STRUCT_FlashImgInfo FlashImgInfo;
    int i;
    char imageType[][5] = {{"QCOM"}, {"BOOT"}, {"APPL"}, {"SWOC"}, {"USBD"}};
    
    for(i = 0; i < 5; i++)
    {
        printf("\nTestA-%d: Fetch %s version string\n", i+1, imageType[i]);
     
        slmemset ((char *)&FlashImgInfo, 0,
                   sizeof(struct SWI_STRUCT_FlashImgInfo));     
        FlashImgInfo.sizeStruct = sizeof(struct SWI_STRUCT_FlashImgInfo);
        slmemcpy (FlashImgInfo.szImageType, imageType[i], 4);
    
        retcode = SwiGetFlashImgInfo(&FlashImgInfo, timeout);

        if( retcode == SWI_RCODE_OK )
        {
            if(!slstrlen(FlashImgInfo.szVersion))
            {
                printf("Flash Image %s: no string available\n", imageType[i]);
            }
            else
            {
                printf("Flash Image %s: version string: %s\n", 
                    imageType[i],
                    FlashImgInfo.szVersion);
            }
        }
        else
        {
            printf("\nGet Flash Image info: %d\n", retcode );
            if( retcode == SWI_RCODE_REQUEST_REJECTED )
            {
                SwiGetLastError( lasterror, sizeof(lasterror) );
                printf("  Mdm reports CnS Error: %s\n", lasterror );
            }
        }
    }
}


/**************
 *
 * Name:     doSwiGetObjRngRev
 *
 * Purpose:  Exercises the SwiGetObjRngRev API - calls the API and 
 *           prints information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiGetObjRngRev(unsigned int timeout)
{
    SWI_RCODE retcode;
    struct SWI_STRUCT_ObjRngRev ObjRngRev;
    swi_uint8 i;
    
    retcode = SwiGetObjRngRev(&ObjRngRev, timeout);

    if( retcode == SWI_RCODE_OK )
    {
        printf("\nGet Object Range Revision: \n"); 

        for (i = 0; i < ObjRngRev.NumOfEntries; i++)
        {
          printf("  %s: reverse: %d, forward: %d\n",
                  apMapObjRngRev[ObjRngRev.arInfoBlock[i].eRngSpecifier],
                  (int)ObjRngRev.arInfoBlock[i].nModemReverseObjRevNum,
                  (int)ObjRngRev.arInfoBlock[i].nModemForwardObjRevNum);
        }
    }
    else
    {
        printf("\nGet Object Range Revision: %d\n", retcode );
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }

}
/**************
 *
 * Name:     doSwiGetCurrentRadioBand
 *
 * Purpose:  Exercises the SwiGetCurrentRadioBandAPI - calls the API and 
 *           prints information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiGetCurrentRadioBand(unsigned int timeout)
{
    struct SWI_STRUCT_RadioBandList radioBand;
    SWI_RCODE retcode;
    int i;

    retcode = SwiGetCurrentRadioBand( &radioBand, timeout );

    printf("\n");

    if( retcode == SWI_RCODE_OK )
    {

        printf("Current Radio Band Get Response:\n");
        printf("  Number of current bands: %d\n", (int)radioBand.nNumBands);

        for(i = 0; i < MAX_BAND_LIST; i++)
        {
            if (i == radioBand.nNumBands)
                break; 
            
            if(radioBand.sList[i].eBand == SWI_RADIOBAND_Invalid)
            {
                printf("   %d-Radio Type:   %s  Current Band: %s\n", i,
                   apMapRadioType[radioBand.sList[i].eType], 
                   "Invalid");
            }
            else if (radioBand.sList[i].eBand >= SWI_RADIOBAND_WcdmaIImt2000)
            {
                printf("   %d-Radio Type:   %s  Current Band: %s\n", i,
                   apMapRadioType[radioBand.sList[i].eType], 
                   apMapRadioWCDMABand[radioBand.sList[i].eBand - SWI_RADIOBAND_WcdmaIImt2000]);
            }
            else if (radioBand.sList[i].eBand >= SWI_RADIOBAND_Gsm450)
            {
                printf("   %d-Radio Type:   %s  Current Band: %s\n", i,
                   apMapRadioType[radioBand.sList[i].eType], 
                   apMapRadioGSMBand[radioBand.sList[i].eBand - SWI_RADIOBAND_Gsm450]);
            }
            else
            {
                printf("   %d-Radio Type:   %s  Current Band: %s\n", i,
                   apMapRadioType[radioBand.sList[i].eType], 
                   apMapRadioCDMABand[radioBand.sList[i].eBand]);
            }
        }
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
        printf ("Current Radio Band Get Request timed out\n");
    else
    {
        printf ("Current Radio Band Get Request is rejected \n");
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
}
/**************
 *
 * Name:     doSwiGetRadioBandCfg
 *
 * Purpose:  Exercises the SwiGetRadioBandCfg API - calls the API and 
 *           prints information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiGetRadioBandCfg(unsigned int timeout)
{
    struct SWI_STRUCT_RadioBandConfig radioConfig;
    SWI_RCODE retcode;
    int i;
   
    retcode = SwiGetRadioBandCfg( &radioConfig, timeout );

    printf("\n");
    if( retcode == SWI_RCODE_OK )
    {
        printf("Radio Band Config Get Successful:\n");
        printf("  Last Selected:  %s\n\t\t(0x%016Lx)\n", 
            apMapRadioBandConfig(radioConfig.nLastSelected),
            radioConfig.nLastSelected);
        printf("  Num in Group:   %d\n", (int)radioConfig.nNumInGroup);
        for(i = 0; i < MAX_BAND_GROUPS; i++)
        {
           if(i == radioConfig.nNumInGroup)
               break;

            printf("    %d- %s \n\t\t(0x%016Lx)\n", 
                i+1, 
                apMapRadioBandConfig(radioConfig.arBandSetting[i]),
                radioConfig.arBandSetting[i]);
        }
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
        printf ("Radio Band Config Get Request timed out\n");
    else
    {
        printf ("Radio Band Config Get Request is rejected \n");
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
}
/**************
 *
 * Name:     doSwiSetRadioBandCfg
 *
 * Purpose:  Exercises the SwiSetRadioBandCfg API - calls the API and 
 *           prints information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiSetRadioBandCfg(unsigned int timeout)
{
    SWI_RCODE retcode;

    retcode = SwiSetRadioBandCfg( SWI_RADIOBANDGROUP_AutoBand, timeout );
    /* Note: The following are other possible input parameters for this test.
     *       To run these please comment out the line above and uncomment one 
     *       of the lines below.  It is recommended to run the test which reads 
     *       the RadioBand Configuration to verify the setup.
     */
    /* retcode = SwiSetRadioBandCfg( SWI_RADIOBANDGROUP_WcdmaIImt2000, timeout ); */
    /* retcode = SwiSetRadioBandCfg( SWI_RADIOBANDGROUP_GsmEGsmDcs, timeout ); */
    /* retcode = SwiSetRadioBandCfg( SWI_RADIOBANDGROUP_GsmAll, timeout ); */
    /* retcode = SwiSetRadioBandCfg( SWI_RADIOBANDGROUP_WcdmaAll, timeout ); */

    printf("\n");

    if( retcode == SWI_RCODE_OK )
        printf("Radio Band Config Set Successful:\n");
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
        printf ("Radio Band Config Set Request timed out\n");
    else
    {
        printf ("Radio Band Config Set Request is rejected \n");
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
}
/**************
 *
 * Name:     doSwiGetRadioPower
 *
 * Purpose:  Exercises the SwiGetRadioPower API - calls the API and 
 *           prints information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiGetRadioPower(unsigned int timeout)
{
    struct SWI_STRUCT_RadioPower radioPower;
    SWI_RCODE retcode;

    retcode = SwiGetRadioPower( &radioPower, timeout );

    printf("\n");

    if( retcode == SWI_RCODE_OK )
    {
        printf("Radio Power Response:\n");
        printf("    Power Mode:        %s\n", 
            appowermodemap[radioPower.eMode]);
        printf("    Low Power Reason:  %s\n", 
            appowerreasonmap[radioPower.nLowPowerReason]);
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
        printf ("Radio Power Get Request timed out\n");
    else
    {
        printf ("Radio Power Get Request is rejected \n");
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
}
/**************
 *
 * Name:     doSwiSetRadioPower
 *
 * Purpose:  Exercises the SwiSetRadioPower API - calls the API and 
 *           prints information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiSetRadioPower(unsigned int timeout)
{
    struct SWI_STRUCT_RadioPower radioPower;
    SWI_RCODE retcode;
   
    /* the other valid mode for this request is */ 
    /*    radioPower.eMode =  SWI_RADIOPOWERMODE_Low;  */
    radioPower.eMode =  SWI_RADIOPOWERMODE_Online;

    retcode = SwiSetRadioPower( &radioPower, timeout );

    printf("\n");
    if( retcode == SWI_RCODE_OK )
    {
        printf("Radio Power Response:\n");
        printf("    Power Mode:        %s\n", 
            appowermodemap[radioPower.eMode]);
        printf("    Low Power Reason:  %s\n", 
            appowerreasonmap[radioPower.nLowPowerReason]);
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
        printf ("Radio Power Set Request timed out\n");
    else
    {
        printf ("Radio Power Set Request is rejected \n");
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }

}
/**************
 *
 * Name:     doSwiGetRadioTempState
 *
 * Purpose:  Exercises the SwiGetRadioTempState API - calls the API and 
 *           prints information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiGetRadioTempState(unsigned int timeout)
{
    struct SWI_STRUCT_RadioTempState radioTemperature;
    SWI_RCODE retcode;

    retcode = SwiGetRadioTempState( &radioTemperature, timeout );

    printf("\n");
    if( retcode == SWI_RCODE_OK )
    { 
        printf("Radio Temperature Response:\n");
        printf("    Temperature State:   %s\n", 
            apradiotempmap[radioTemperature.eState]);
        printf("    Current Temperature: %d\n", (int)radioTemperature.nCurrent);
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
        printf ("Radio Temp Get Request timed out\n");
    else
    {
        printf ("Radio Temp Get Request is rejected \n");
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }

}
/**************
 *
 * Name:     doSwiGetRadioBattery
 *
 * Purpose:  Exercises the SwiGetRadioBattery API - calls the API and 
 *           prints information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiGetRadioBattery(unsigned int timeout)
{
    struct SWI_STRUCT_RadioBattery radioBattery;
    SWI_RCODE retcode;

    retcode = SwiGetRadioBattery( &radioBattery, timeout );

    printf("\n");
    if( retcode == SWI_RCODE_OK )
    {
        printf("Radio Battery Response:\n");
        printf("    Battery State:   %s\n", 
            apradiobattmap[radioBattery.eState]);
        printf("    Current Voltage: %d\n", (int)radioBattery.nCurrent);
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
        printf ("Radio Battery Get Request timed out\n");
    else
    {
        printf ("Radio Battery Get Request is rejected \n");
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }

}

/**************
 *
 * Name:     doSwiGetUsbPortName
 *
 * Purpose:  Exercises the doSwiGetUsbPortName API - calls the API and 
 *           prints information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiGetUsbPortName(unsigned int timeout)
{
    struct SWI_STRUCT_UsbPortName sPortName;
    SWI_RCODE retcode;

    slmemset ((char *)&sPortName, 0, sizeof (struct SWI_STRUCT_UsbPortName));
    
    retcode = SwiGetUsbPortName( &sPortName );

    printf("\n");
    if( retcode == SWI_RCODE_OK )
    {
        printf("Usb Port Name Response:\n");
        printf("  HIP:          %s\n", &sPortName.HipPort[0]);
        printf("  DIAG          %s\n", &sPortName.DiagPort[0]);
        printf("  NMEA:         %s\n", &sPortName.NmeaPort[0]);
        printf("  AT:           %s\n", &sPortName.AtCmdPort[0]);
        printf("  DATA1:        %s\n", &sPortName.Data1Port[0]);
        printf("  DATA2:        %s\n", &sPortName.Data2Port[0]);
        printf("  DATA3:        %s\n", &sPortName.Data3Port[0]);
        printf("  Direct IP:    %s\n", &sPortName.DirectIPPort[0]);
        printf("  QMI:          %s\n", &sPortName.QMIPort[0]);
        printf("  Mass Storage: %s\n", &sPortName.MassStoragePort[0]);
        printf("Note: Depending on modem and configuration, not all names may be available.\n");
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
        printf ("Usb Port Name Get Request timed out\n");
    else
    {
        printf ("Usb Port Name Get Request is rejected \n");
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
}

/**************
 *
 * Name:     doSwiSoftModemReset
 *
 * Purpose:  Requests that the modem perform a soft reset which will
 *           result in two AirServerChange notifications.
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    None
 *
 **************/
package void doSwiSoftModemReset(unsigned int timeout)
{
    SWI_RCODE retcode;

    retcode = SwiSoftModemReset();

    if( retcode == SWI_RCODE_OK )
    {
        printf("Modem reset requested\n");
    }
}

/**************
 *
 * Name:     doSwiResetToBootHold
 *
 * Purpose:  Requests that the modem reset into boot and hold operation.
 *           This call will result in a pair of AirServerChange Notifications.
 *           When the modem reboots it will be operating in boot and hold
 *           mode and will not return from that state without user intervention
 *           in the form of a firmware download or subsequent reboot
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    None
 *
 **************/
package void doSwiResetToBootHold(unsigned int timeout)
{
    SWI_RCODE retcode;

    retcode = SwiResetToBootHold( timeout );

    if( retcode == SWI_RCODE_OK )
    {
        printf("Modem reset to boot and hold op'n requested\n" );
    }
}

/**************
 *
 * Name:     dofailSwiGetDeviceInformation
 *
 * Purpose:  Requests Device Information messages 
 *           
 * Parms:    retcode   - Return code from call to API
 *           typep     - pointer to name of info type.
 * 
 * Return:   none
 *
 * Notes:    Simply print error message
 *
 **************/
local void dofailSwiGetDeviceInformation(
    SWI_RCODE retcode,
    char *typep)
{
    if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf("  %s failed with return code: %d-%s\n", typep, retcode, 
               "SWI_RCODE_REQUEST_TIMEOUT");
    }
    else if (retcode == SWI_RCODE_DEVICE_NOT_SUP)
    {
        printf(  "  %s failed with return code: %d-%s\n", typep, retcode, 
               "SWI_RCODE_DEVICE_NOT_SUP");
    }
    else
        printf("  %s failed with return code: %d\n", typep, retcode);
}

/**************
 *
 * Name:     doSwiGetDeviceInformation
 *
 * Purpose:  Requests Device Information from the modem 
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply request each parameter at a time
 *
 **************/
package void doSwiGetDeviceInformation(unsigned int timeout)
{
    struct SWI_STRUCT_DeviceInformation_Request  sDevInfo_Request; 
    struct SWI_STRUCT_DeviceInformation_Response sDevInfo_Response;

    SWI_RCODE retcode;

    sDevInfo_Request.Version = 2;
    sDevInfo_Request.eDeviceInformationType = SWI_DEVICEINFORMATION_Technology;
    
    retcode = SwiGetDeviceInformation( &sDevInfo_Request,
                                       &sDevInfo_Response,
                                       timeout );
    if( retcode == SWI_RCODE_OK )
    {
        printf("Device Information\n" );
        printf("  Version          %d\n", (int)sDevInfo_Response.Version);
        printf("  Status           %s\n", 
                        sDevInfo_Response.eRequestStatus ? "Failed" : "OK");
        printf("  Technology       %s\n", 
                        apMapTechType[sDevInfo_Response.eTechnology]);
    }
    else
    {
        dofailSwiGetDeviceInformation(retcode, "Technology");
    }

    sDevInfo_Request.eDeviceInformationType = SWI_DEVICEINFORMATION_Product;
    retcode = SwiGetDeviceInformation( &sDevInfo_Request,
                                       &sDevInfo_Response,
                                       timeout );
    if( retcode == SWI_RCODE_OK )
    {
        printf("  Product string   %s\n", sDevInfo_Response.szProduct);
    }
    else
    {
        dofailSwiGetDeviceInformation(retcode, "Product string");
    }
    
    sDevInfo_Request.eDeviceInformationType = 
                                        SWI_DEVICEINFORMATION_FrmDwlProtocol;
    retcode = SwiGetDeviceInformation( &sDevInfo_Request,
                                       &sDevInfo_Response,
                                       timeout );
    if( retcode == SWI_RCODE_OK )
    {
        printf("  FW dnld protocol %d\n", sDevInfo_Response.eFrmDlwProtocol);
    }
    else
    {
        dofailSwiGetDeviceInformation(retcode, "FW dnld protocol");
    }
    
    sDevInfo_Request.eDeviceInformationType = 
                                            SWI_DEVICEINFORMATION_DeviceReEnum;
    retcode = SwiGetDeviceInformation( &sDevInfo_Request,
                                       &sDevInfo_Response,
                                       timeout );
    if( retcode == SWI_RCODE_OK )
    {
        printf("  DeviceReEnum     %d\n", sDevInfo_Response.eDeviceReEnum);
    }
    else
    {
        dofailSwiGetDeviceInformation(retcode, "DeviceReEnum");
    }
    
    sDevInfo_Request.eDeviceInformationType = 
                                            SWI_DEVICEINFORMATION_CNSsupported;
    retcode = SwiGetDeviceInformation( &sDevInfo_Request,
                                       &sDevInfo_Response,
                                       timeout );
    if( retcode == SWI_RCODE_OK )
    {
        printf("  CNSsupported     %s\n", 
                        apMapCNSsupported[sDevInfo_Response.eCNSsupported]);
    }
    else
    {
        dofailSwiGetDeviceInformation(retcode, "CNSsupported");
    }
}

/*
 * $Log: aptestcmbasic.c,v $
 */
