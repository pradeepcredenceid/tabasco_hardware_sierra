/*
 *  $Id: aptestgsmnet.c,v 1.18 2010/06/10 00:27:06 epasheva Exp $
 *
 *  Filename: aptestgsmnet.c
 *
 *  Purpose:  Contains functions for testing GSM Network APIs.
 *
 * Copyright: © 2008 Sierra Wireless, Inc., all rights reserved
 *
 */

/* Linux definitions */
#include <stdio.h>
#include <unistd.h>

/* include files */
#include "SwiDataTypes.h"
#include "SwiApiGsmNetwork.h"
#include "sl/sludefs.h"
#include "apudefs.h"
#include "aptestdefs.h"

/*************
 *
 * Name:    modemstatusmap - Modem status mapping table
 *
 * Purpose: This table maps Modem status enum returned in 
 *          struct  SWI_STRUCT_GsmSystemStatus. 
 *
 *************/
char modemstatusmap[][20] = 
  {{"Offline"}, 
   {"Online" }, 
   {"Low Power Mode"}};
    
/*************
 *
 * Name:    servicestatusmap - service status mapping table
 *
 * Purpose: This table maps service status enum returned in 
 *          struct  SWI_STRUCT_GsmSystemStatus. 
 *
 *************/
char servicestatusmap[][20] = 
  {{"Normal"}, 
   {"Emergency" }, 
   {"NoService" }, 
   {"AccessDifficulty" }, 
   {"ForbiddenPLMN" }, 
   {"ForbiddenArea" }, 
   {"ForbiddenRoaming" }, 
   {"IllegalMobStation" }, 
   {"IllegalMobEquip" }, 
   {"UnknownIMSI" }, 
   {"AuthFailure"},
   {"PRSFailed"}};
   
/*************
 *
 * Name:    servicetypemap - service type  mapping table
 *
 * Purpose: This table maps service type enum returned in 
 *          struct  SWI_STRUCT_GsmSystemStatus. 
 *
 *************/
char servicetypemap[][20] = 
  {{"CircuitSwitched"},
   {"GPRS" }, 
   {"Combined" }, 
   {"InvalidService"}};

/* Functions */

/**************
 *
 * Name:     doSwiGetGsmProfileSummary
 *
 * Purpose:  Runs the test to request and receive a GSM profile summary from
 *           the modem 
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    none
 *            
 *
 **************/
package void doSwiGetGsmProfileSummary(unsigned int timeout)
{
    SWI_RCODE       retcode = 0xFFFF;
    swi_char   lasterror[80];
    swi_int32 i;
    struct SWI_STRUCT_GsmProfileSummary ProfileSummary;

    /* Request GSM Profile */
    retcode     = SwiGetGsmProfileSummary(&ProfileSummary,timeout);
    if( retcode == SWI_RCODE_OK )
    {
        printf ("SwiGetGsmProfileSummary command is being processed\n");
        printf("    sizeStruct:  %ld\n", ProfileSummary.sizeStruct);
        printf("    MaxProfiles:  %ld\n", ProfileSummary.nMaxProfiles);
        printf("    MaxTft:  %ld\n", ProfileSummary.nMaxTft );
        printf("    DefaultProfileId:  %ld\n",ProfileSummary.nDefaultProfileId);
        for (i = 0; i < MAX_3G_GSM_PROFILES;  i++)
        {   
            switch (ProfileSummary.arrState[i])
            {
                case SWI_PROFILE_STATE_Inactive:
                    printf("    State: Inactive\n" );
                    break;
                case SWI_PROFILE_STATE_Activated:
                    printf("    State: Activated\n" );
                    break;
                case SWI_PROFILE_STATE_Deleted:
                    printf("    State: Deleted\n" );
                    break;
                default:
                    printf("    State: Unknown?\n" );
                    break;
            }
        }
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("SwiGetGsmProfileSummary command timed out\n");
    }
    else if (retcode == 0xFFFF)
    {
        printf("SwiGetGsmProfileSummary command has been commented out\n");
    }
    else
    {
        printf ("SwiGetGsmProfileSummary command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\n");
}

/**************
 *
 * Name:     doSwiSetDefaultProfile
 *
 * Purpose:  Runs the test to set the GSM default profile in the modem 
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    none
 *
 **************/
package void doSwiSetDefaultProfile(unsigned int timeout)
{
    SWI_RCODE       retcode = 0xFFFF;
    swi_char   lasterror[80];
    swi_uint32      nProfileID;

    nProfileID = 5;

    /* Set default profile */

    retcode = SwiSetDefaultProfile(nProfileID, timeout );
    if( retcode == SWI_RCODE_OK )
    {
        printf ("SwiSetDefaultProfile command is being processed\n");
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("SwiSetDefaultProfile command timed out\n");
    }
    else if (retcode == 0xFFFF)
    {
        printf("SwiSetDefaultProfile command has been commented out\n");
    }
    else
    {
        printf ("SwiSetDefaultProfile command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\n");
}


/**************
 *
 * Name:     doSwiGetGsmProfileBasic
 *
 * Purpose:  Runs the test to request and receive the GSM Basic profile from
 *           the modem 
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    none
 *
 **************/
package void doSwiGetGsmProfileBasic(unsigned int timeout)
{
    SWI_RCODE       retcode = 0xFFFF;
    swi_char   lasterror[80];
    struct SWI_STRUCT_GsmProfileBasic ProfileBasic;

    slmemset ((char *)&ProfileBasic, 0, 
              sizeof (struct SWI_STRUCT_GsmProfileBasic));

    /* Set parameters for CNS_PROFILE_READ request */
    ProfileBasic.nProfileId  = 5;
   
    /* Request GSM Profile */
    retcode     = SwiGetGsmProfileBasic(&ProfileBasic,timeout);
    if( retcode == SWI_RCODE_OK )
    {
        printf ("SwiGetGsmProfileBasic command is being processed\n");

        printf("    sizeStruct:         %ld\n", ProfileBasic.sizeStruct);
        printf("    nProfileId:         %d\n", ProfileBasic.nProfileId);   
        printf("    Valid:              %d\n", ProfileBasic.bValid);
        printf("    ePdpType:           %d\n", ProfileBasic.ePdpType); /* ipv4 PDP type */
        /* Note: Header compression and Data compression fields are filled in the
         *  order they appear in CNS packet. Please note that in the 
         *  SWI_STRUCT_GsmProfileBasic these fields are defined in reverse order.
         */
        printf("    bHeaderCompression: %d\n", ProfileBasic.bHeaderCompression); /* no compression */
        printf("    bDataCompression:   %d\n", ProfileBasic.bDataCompression); /* no compression */

        /* Note: AutomaticIP is set to TRUE for now. */
        printf("    bAutomaticIP:       %d\n", ProfileBasic.bAutomaticIP); /* dynamic addressing */
        printf("    nLengthPdp:         %ld\n", ProfileBasic.nLengthPdp); /* dynamic addressing */
        printf("    byPdp:              %s\n", &ProfileBasic.byPdp[0]); /* dynamic addressing */

        printf("    szAccessPoint:      %s\n", &ProfileBasic.szAccessPoint[0]); /* no APN specified */

        printf("    ePdpInitiated:      %d\n", ProfileBasic.ePdpInitiated);/* Primary MobileInitiated */
        printf("    nPrimaryProfileId:  %ld\n", ProfileBasic.nPrimaryProfileId); /* valid only if PDP initiated
                                             * type is Secondary Mobile
                                             * initiated 
                                             */
        printf("    eAuthentication:    %d\n", ProfileBasic.eAuthentication); /* no authentication */
        printf("    szUserName:         %s\n", &ProfileBasic.szUserName[0]);
        printf("    szPassword:         %s\n", &ProfileBasic.szPassword[0]); 
        printf("    szLabel:            %s\n", &ProfileBasic.szLabel[0]); 
        printf("    bAutoActivation:    %d\n", ProfileBasic.bAutoActivation);/* always connected configuration */
        printf("    bReadOnly:          %d\n", ProfileBasic.bReadOnly); /* user can read/write the profile */
        printf("    bPromptPassword:    %d\n", ProfileBasic.bPromptPassword);
        printf("    bAutoRunVPN:        %d\n", ProfileBasic.bAutoRunVPN); /* don't auto launch VPN */
        printf("    nLingerTimer:       %ld\n", ProfileBasic.nLingerTimer); /* disable timer */
        printf("    nSoftwareOptions:   %d\n", ProfileBasic.nSoftwareOptions); /* HSDPA profile */
        /* Note: pProfile->MccMnc set to 0 with the structure initialization */

    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("SwiGetGsmProfileBasic command timed out\n");
    }
    else if (retcode == 0xFFFF)
    {
        printf("SwiGetGsmProfileBasic command has been commented out\n");
    }
    else
    {
        printf ("SwiGetGsmProfileBasic command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\n");
} 

/**************
 *
 * Name:     doSwiGetGsmProfileQos
 *
 * Purpose:  Runs the test to request and receive the GSM Qos profile from
 *           the modem 
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    none 
 *
 **************/
package void doSwiGetGsmProfileQos(unsigned int timeout)
{
    SWI_RCODE       retcode = 0xFFFF;
    swi_char        lasterror[80];
    struct SWI_STRUCT_GsmProfileQos ProfileQos;

    /* Set parameters for CNS_PROFILE_READ request */
    ProfileQos.nProfileId  = 4;

    /* Request GSM Profile */
    retcode     = SwiGetGsmProfileQos(&ProfileQos,timeout);

    if( retcode == SWI_RCODE_OK )
    {
        printf ("SwiGetGsmProfileQos command is being processed\n");
        printf("    sizeStruct:          %ld\n", ProfileQos.sizeStruct);
        printf("    nProfileId:          %d\n", ProfileQos.nProfileId); 
        printf("    bValidUmtsReq:       %d\n", ProfileQos.bValidUmtsReq );         
        printf("    eReqTrafficClass:    %d\n", ProfileQos.eReqTrafficClass);  /* subscribed */    
        printf("    eReqMaxBitrateUl:    %d\n", ProfileQos.eReqMaxBitrateUl);  /* subscribed */     
        printf("    eReqMaxBitrateDl:    %d\n", ProfileQos.eReqMaxBitrateDl);     
        printf("    eReqGuarBitrateUl:   %d\n", ProfileQos.eReqGuarBitrateUl);     
        printf("    eReqGuarBitrateDl:   %d\n", ProfileQos.eReqGuarBitrateDl);      
        printf("    eReqDelOrder:        %d\n", ProfileQos.eReqDelOrder);          
        printf("    eReqDelErrSdu:       %d\n", ProfileQos.eReqDelErrSdu);         
        printf("    nReqMaxSdu:          %ld\n", ProfileQos.nReqMaxSdu); /* max value test */
        printf("    eReqSduErrRatio:     %d\n", ProfileQos.eReqSduErrRatio);       
        printf("    eReqResBitErrRatio:  %d\n", ProfileQos.eReqResBitErrRatio);    
        printf("    nReqTransDelay:      %ld\n", ProfileQos.nReqTransDelay); /* max value test */
        printf("    eReqTraffHandPri:    %d\n", ProfileQos.eReqTraffHandPri);     
        printf("    eReqSrcStatDesc:     %d\n", ProfileQos.eReqSrcStatDesc);  /* unknown */    
        printf("    eReqSignInd:         %d\n", ProfileQos.eReqSignInd);           
        printf("    bValidUmtsMin:       %d\n", ProfileQos.bValidUmtsMin);        
        printf("    eMinTrafficClass:    %d\n", ProfileQos.eMinTrafficClass );      
        printf("    eMinMaxBitrateUl:    %d\n", ProfileQos.eMinMaxBitrateUl);      
        printf("    eMinMaxBitrateDl:    %d\n", ProfileQos.eMinMaxBitrateDl);      
        printf("    eMinGuarBitrateUl:   %d\n", ProfileQos.eMinGuarBitrateUl);     
        printf("    eMinGuarBitrateDl:   %d\n", ProfileQos.eMinGuarBitrateDl);     
        printf("    eMinDelOrder:        %d\n", ProfileQos.eMinDelOrder);         
        printf("    eMinDelErrSdu:       %d\n", ProfileQos.eMinDelErrSdu);         
        printf("    nMinMaxSdu:          %ld\n", ProfileQos.nMinMaxSdu);  /* max value test */
        printf("    eMinSduErrRatio:     %d\n", ProfileQos.eMinSduErrRatio);      
        printf("    eMinResBitErrRatio:  %d\n", ProfileQos.eMinResBitErrRatio);   
        printf("    nMinTransDelay:      %ld\n", ProfileQos.nMinTransDelay);  /* max value test */
        printf("    eMinTraffHandPri:    %d\n", ProfileQos.eMinTraffHandPri);     
        printf("    eMinSrcStatDesc:     %d\n", ProfileQos.eMinSrcStatDesc); /* unknown */     
        printf("    eMinSignInd:         %d\n", ProfileQos.eMinSignInd);          
        printf("    bValidGprsReq:       %d\n", ProfileQos.bValidGprsReq);          
        printf("    eReqPrecedence:      %d\n", ProfileQos.eReqPrecedence);       
        printf("    eReqDelay:           %d\n", ProfileQos.eReqDelay);            
        printf("    eReqReliability:     %d\n", ProfileQos.eReqReliability);      
        printf("    eReqPeak:            %d\n", ProfileQos.eReqPeak);              
        printf("    eReqMean:            %d\n", ProfileQos.eReqMean);              
        printf("    bValidGprsMin:       %d\n", ProfileQos.bValidGprsMin);        
        printf("    eMinPrecedence:      %d\n", ProfileQos.eMinPrecedence);       
        printf("    eMinDelay:           %d\n", ProfileQos.eMinDelay);           
        printf("    eMinReliability:     %d\n", ProfileQos.eMinReliability);       
        printf("    eMinPeak:            %d\n", ProfileQos.eMinPeak);             
        printf("    eMinMean:            %d\n", ProfileQos.eMinMean); 
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("SwiGetGsmProfileQos command timed out\n");
    }
    else if (retcode == 0xFFFF)
    {
        printf("SwiGetGsmProfileQos command has been commented out\n");
    }
    else
    {
        printf ("SwiGetGsmProfileQos command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\n");
} 

/**************
 *
 * Name:     doSwiActivateProfile
 *
 * Purpose:  Starts up a data connection using the specified profile ID
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    none 
 *
 **************/
package void doSwiActivateProfile( unsigned int timeout)
{
    swi_uint32      ProfileID;
    SWI_RCODE       retcode = 0xFFFF;

    /* Hard-coded ID of profile to activate */
    ProfileID = 1;

    /* Activating data connection on hard-coded profile */
    printf("\nActivating connection on profile %ld\n", ProfileID );

    /* Issue the request to activate the profile */
    retcode = SwiActivateProfile( ProfileID, TRUE, timeout );

    /* If the request failed, indicate it to the user */
    if( retcode != SWI_RCODE_OK )
    {
        printf("Connection not established\n\n");
    }
    else
    {
        printf("Connection established as requested\n\n");
    }
}

/**************
 *
 * Name:     doSwiDeActivateProfile
 *
 * Purpose:  Shuts down a data connection using the specified profile ID
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    none 
 *
 **************/
package void doSwiDeActivateProfile( unsigned int timeout)
{
    swi_uint32      ProfileID;
    SWI_RCODE       retcode = 0xFFFF;

    /* Hard-coded ID of profile to activate */
    ProfileID = 1;

    /* Activating data connection on hard-coded profile */
    printf("\nDeactivating connection on profile %ld\n", ProfileID );

    /* Issue the request to activate the profile */
    retcode = SwiActivateProfile( ProfileID, FALSE, timeout );

    /* If the request failed, indicate it to the user */
    if( retcode != SWI_RCODE_OK )
    {
        printf("Connection termination error\n\n");
    }
    else
    {
        printf("Connection terminated as requested\n\n");
    }
}

/**************
 *
 * Name:     doSwiGetGsmProfileDns
 *
 * Purpose:  Runs the test to request and receive a GSM Dns profile from
 *           the modem 
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    none 
 *
 **************/
package void doSwiGetGsmProfileDns(unsigned int timeout)
{
    SWI_RCODE       retcode = 0xFFFF;
    swi_char   lasterror[80];
    struct SWI_STRUCT_GsmProfileDns ProfileDns;

    /* Set parameters for CNS_PROFILE_READ request */
    ProfileDns.nProfileId  = 5;

    /* Request GSM Profile */
    retcode     = SwiGetGsmProfileDns(&ProfileDns,timeout);
    if( retcode == SWI_RCODE_OK )
    {
        printf ("SwiGetGsmProfileDns command is being processed\n");
        printf("    sizeStruct:    %ld\n", ProfileDns.sizeStruct);
        printf("    nProfileId     %d\n", ProfileDns.nProfileId);
        printf("    nLengthPriDns  %ld\n", ProfileDns.nLengthPriDns);
        printf("    byPriDns       %d.%d.%d.%d\n", ProfileDns.byPriDns[0],
                                                   ProfileDns.byPriDns[1],
                                                   ProfileDns.byPriDns[2],
                                                   ProfileDns.byPriDns[3]);
        printf("    nLengthSecDns  %ld\n", ProfileDns.nLengthSecDns);
        printf("    bySecDns       %d.%d.%d.%d\n", ProfileDns.bySecDns[0],
                                                   ProfileDns.bySecDns[1],
                                                   ProfileDns.bySecDns[2],
                                                   ProfileDns.bySecDns[3]);
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("SwiGetGsmProfileDns command timed out\n");
    }
    else if (retcode == 0xFFFF)
    {
        printf("SwiGetGsmProfileDns command has been commented out\n");
    }
    else
    {
        printf ("SwiGetGsmProfileDns command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\n");
}
/**************
 *
 * Name:     doSwiGetGsmProfileTft
 *
 * Purpose:  Runs the test to request and receive a GSM Tft profile from
 *           the modem 
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    none
 *
 **************/
package void doSwiGetGsmProfileTft(unsigned int timeout)
{
    SWI_RCODE       retcode = 0xFFFF;
    swi_char   lasterror[80];
    swi_int32                       i;
    struct SWI_STRUCT_GsmProfileTft ProfileTft;

    /* Set parameters for CNS_PROFILE_READ request */
    ProfileTft.nProfileId  = 4;
    ProfileTft.nFilterId   = 2; /* currently support 1-2 only */
   

    /* Request GSM Profile */
    retcode      = SwiGetGsmProfileTft(&ProfileTft,timeout);
    if( retcode == SWI_RCODE_OK )
    {
        printf ("SwiGetGsmProfileTft command is being processed\n");
        printf("SwiGetGsmProfileTft: \n");
        printf("    sizeStruct:       %ld\n", ProfileTft.sizeStruct);
        printf("    nProfileId        %d\n", ProfileTft.nProfileId);
        printf("    nFilterId         %ld\n", ProfileTft.nFilterId);
        printf("    bValid:           %d\n", ProfileTft.bValid);
        printf("    nEpi              %ld\n", ProfileTft.nEpi);
        printf("    nLengthIp:        %ld\n", ProfileTft.nLengthIp );
        printf("    bySrcIp:          " );
        for (i = 0; i < LENGTH_IPAddress;  i++)
        {   
            printf("%i", ProfileTft.bySrcIp[i]);
        }
        printf("\n");
        printf("    bySrcIpMask:      " );
        for (i = 0; i < LENGTH_IPAddress;  i++)
        {   
            printf("%i", ProfileTft.bySrcIpMask[i]);
        }
        printf("\n");
        printf("    nProtoNum:        %ld\n", ProfileTft.nProtoNum);
        printf("    nDestPortFrom:    %ld\n", ProfileTft.nDestPortFrom  );
        printf("    nDestPortTo:      %ld\n", ProfileTft.nDestPortTo    );
        printf("    nSrcPortFrom:     %ld\n", ProfileTft.nSrcPortFrom  );
        printf("    nSrcPortTo:       %ld\n", ProfileTft. nSrcPortTo );
        printf("    nSecurParamInd:   %ld\n", ProfileTft.nSecurParamInd  );
        printf("    nTypeServ:        %ld\n", ProfileTft.nTypeServ  );
        printf("    nTypeServMask:    %ld\n", ProfileTft.nTypeServMask  );
        printf("    nFlowLabel:       %ld\n", ProfileTft.nFlowLabel  );
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("SwiGetGsmProfileTft command timed out\n");
    }
    else if (retcode == 0xFFFF)
    {
        printf("SwiGetGsmProfileTft command has been commented out\n");
    }
    else
    {
        printf ("SwiGetGsmProfileTft command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\n");
}
/**************
 *
 * Name:     doSwiSetPLMNMode
 *
 * Purpose:  Runs the test to call selected GSM Network Status APIs
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    This test is also used to invokes all non-profile GSM Status APIs.
 *           This way one can use a debugger such as 'gdb' to step through 
 *           these and inspect data. Please note that the current setup values
 *           are fictional and probably incorrect. These are used for testing
 *           the data structures and pack/unpack functions during the initial
 *           stage of LinuxSDK development.
 *            
 *
 **************/
package void doSwiSetPLMNMode( unsigned int timeout )
{
    SWI_RCODE       retcode;
    swi_char   lasterror[80];
    struct SWI_STRUCT_PlmnSelection sPlmn;

    slmemset ((char *)&sPlmn,0, sizeof(struct SWI_STRUCT_PlmnSelection)); 

    /* Brief test of PLMN APIs (used to step with gdb) */
    retcode     = SwiSetPLMNMode(sPlmn, timeout);
    if( retcode == SWI_RCODE_OK )
    {
        printf ("SwiSetPLMNMode command is being processed\n");
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("SwiSetPLMNMode command timed out\n");
    }
    else
    {
        printf ("SwiSetPLMNMode command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\n");
}
/**************
 *
 * Name:     doSwiGetPLMNMode
 *
 * Purpose:  Runs the test to call selected GSM Network Status APIs
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    This test is also used to invokes all non-profile GSM Status APIs.
 *           This way one can use a debugger such as 'gdb' to step through 
 *           these and inspect data. Please note that the current setup values
 *           are fictional and probably incorrect. These are used for testing
 *           the data structures and pack/unpack functions during the initial
 *           stage of LinuxSDK development.
 *            
 *
 **************/
package void doSwiGetPLMNMode( unsigned int timeout )
{
    SWI_RCODE       retcode;
    swi_char   lasterror[80];
    enum SWI_TYPE_PLMN_Mode         eMode;

    retcode     = SwiGetPLMNMode( &eMode, timeout);
    if( retcode == SWI_RCODE_OK )
    {
        printf ("SwiGetPLMNMode command is being processed\n");
        printf("      PLMN Mode: %d\n", eMode );
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("SwiGetPLMNMode command timed out\n");
    }
    else if (retcode == 0xFFFF)
    {
        printf("SwiGetPLMNMode command has been commented out\n");
    }
    else
    {
        printf ("SwiGetPLMNMode command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\n");

}

/**************
 *
 * Name:     doSwiStartPLMNSearch
 *
 * Purpose:  Runs the test to call selected GSM Network Status APIs
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    This test is also used to invokes all non-profile GSM Status APIs.
 *           This way one can use a debugger such as 'gdb' to step through 
 *           these and inspect data. Please note that the current setup values
 *           are fictional and probably incorrect. These are used for testing
 *           the data structures and pack/unpack functions during the initial
 *           stage of LinuxSDK development.
 *            
 *
 **************/
package void doSwiStartPLMNSearch( unsigned int timeout )
{
    SWI_RCODE       retcode;
    swi_char   lasterror[80];
 
    retcode     = SwiStartPLMNSearch(timeout);
    if( retcode == SWI_RCODE_OK )
    {
        printf ("SwiStartPLMNSearch has initiated network scan\n");
        printf (" WAIT for 'PLMN Available' notification before calling\n");
        printf (" SwiGetPLMNSelection to fetch found PLMNs\n");
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("SwiStartPLMNSearch command timed out\n");
    }
    else
    {
        printf ("SwiStartPLMNSearch command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
 
    printf("\n");
}
/**************
 *
 * Name:     doSwiGsmPLMN
 *
 * Purpose:  Direct users to use the PlmnSelect sample app instead
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none            
 *
 **************/
package void doSwiGetPLMNSelection( unsigned int timeout )
{
	printf("\nTest deprecated\n" );
	printf("\nPlease use the PlmnSelect sample app to test this function.\n" );    
}

/**************
 *
 * Name:     doSwiGsmStatus
 *
 * Purpose:  Runs the test to call selected GSM Network Status APIs
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    This test is also used to invokes all non-profile GSM Status APIs.
 *           This way one can use a debugger such as 'gdb' to step through
 *           these and inspect data. Please note that the current setup values
 *           are fictional and probably incorrect. These are used for testing
 *           the data structures and pack/unpack functions during the initial
 *           stage of LinuxSDK development.
 *            
 *
 **************/
package void doSwiGsmStatus(unsigned int timeout)
{
    SWI_RCODE  retcode = 0xFFFF;
    swi_char   lasterror[80];
    swi_bool   bStartup;
    swi_uint32 index;

    struct SWI_STRUCT_RscpEcIo RscpEcIo;
    struct SWI_STRUCT_IpAddress pIp;
    struct SWI_STRUCT_GsmSystemStatus SysStatus;
    struct SWI_STRUCT_PktSessionStatus sSessionStatus;

    /* Test setup only (dummy); used to step through the APIs using
     * a debugger such as gdb 
     */
    slmemset ((char *)&RscpEcIo, 0, sizeof (struct SWI_STRUCT_RscpEcIo)); 
    slmemset ((char *)&pIp, 0, sizeof (struct SWI_STRUCT_IpAddress)); 
    slmemset ((char *)&SysStatus, 0, 
        sizeof (struct SWI_STRUCT_GsmSystemStatus)); 
    slmemset ((char *)&sSessionStatus, 0, 
        sizeof (struct SWI_STRUCT_PktSessionStatus)); 

    /* Tell the modem the host application is starting up */
    bStartup = 1;
    retcode     = SwiSetHostStartup(bStartup, timeout);
    if( retcode == SWI_RCODE_OK )
    {
        printf ("SwiSetHostStartup command successful\n");
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("SwiSetHostStartup command timed out\n");
    }
    else if (retcode == 0xFFFF)
    {
        printf("SwiSetHostStartup command has been commented out\n");
    }
    else
    {
        printf ("SwiSetHostStartup command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\n");

    retcode = SwiGetRscpEcIo( &RscpEcIo, timeout);
    if( retcode == SWI_RCODE_OK )
    {
        printf ("SwiGetRscpEcIo Number of Cells Detected: %ld\n", RscpEcIo.nNumOfCells );
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("SwiGetRscpEcIo command timed out\n");
    }
    else if (retcode == 0xFFFF)
    {
        printf("SwiGetRscpEcIo command has been commented out\n");
    }
    else
    {
        printf ("SwiGetRscpEcIo command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\n");

    pIp.nProfileId = 1;
    retcode = SwiGetIPAddressEx(&pIp, timeout);
    if( retcode == SWI_RCODE_OK )
    {
        printf ("\nModem IP Address is: ");
        for( index = 0; index < pIp.nIpLength; index++ )
        {
            printf("%d.", pIp.byIp[index] );
        }
        printf("\n");
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("SwiGetIPAddressEx command timed out\n");
    }
    else if (retcode == 0xFFFF)
    {
        printf("SwiGetIPAddressEx command has been commented out\n");
    }
    else
    {
        printf ("SwiGetIPAddressEx command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\n");

    retcode     = SwiGetGsmSystemStatus( &SysStatus, timeout);
    if( retcode == SWI_RCODE_OK )
    {
        printf ("SwiGetGsmSystemStatus() has been completed\n");
        printf (" Modem status: %s\n", modemstatusmap[SysStatus.eModemStatus]);
        printf (" Service status: %s\n", servicestatusmap[SysStatus.eServiceStatus]);
        printf (" Service type: %s\n", servicetypemap[SysStatus.eServiceType]);
        printf (" Country Initials: %s\n", SysStatus.szCountry);
        printf (" Network name: %s\n", SysStatus.szNetwork);
        printf (" MCC: %ld MNC: %ld\n", SysStatus.nMcc, SysStatus.nMnc);
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("SwiGetGsmSystemStatus( command timed out\n");
    }
    else if (retcode == 0xFFFF)
    {
        printf("SwiGetGsmSystemStatus( command has been commented out\n");
    }
    else
    {
        printf ("SwiGetGsmSystemStatus( command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\n");

    retcode     = SwiGetPktSessionStatus( &sSessionStatus, timeout);
    if( retcode == SWI_RCODE_OK )
    {
        printf ("SwiGetPktSessionStatus() result:\n" );
        printf ("Session Active Status: %d\n", sSessionStatus.bPacketSessionActive );
        printf ("QoS Valid:             %d\n", sSessionStatus.bValidQos );
        printf ("QoS Precedence:        %d\n", sSessionStatus.eNegoPrecedence );
        printf ("QoS Delay:             %d\n", sSessionStatus.eNegoDelay );
        printf ("QoS Reliability:       %d\n", sSessionStatus.eNegoReliability );
        printf ("QoS Mean Throughput:   %d\n", sSessionStatus.eNegoMeanThroughput );
        printf ("QoS Peak Throughput:   %d\n", sSessionStatus.eNegoPeakThroughput );
        printf ("Inactivity Reason:     %d\n\n", sSessionStatus.eInactivityReason );

    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("SwiGetPktSessionStatus( command timed out\n");
    }
    else if (retcode == SWI_RCODE_NOT_SUP)
    {
        printf("SwiGetPktSessionStatus() - Function Deprecated\n  - Use SwiGetPktSessionStatusEx() instead\n");
    }
    else
    {
        printf ("SwiGetPktSessionStatus( command is rejected: %d\n)", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\n");
}

/**************
 *
 * Name:     doSwiGetServiceStatusEx
 *
 * Purpose:  Runs the test to request and receive a GSM Service Status from
 *           the modem 
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    
 *            
 *
 **************/ 
package void doSwiGetServiceStatusEx( unsigned int timeout )
{
    SWI_RCODE       retcode = 0xFFFF;
    swi_char   lasterror[80];
    enum SWI_TYPE_ServiceStatus eServiceStatus;

    retcode = SwiGetServiceStatusEx( &eServiceStatus, timeout);
    if( retcode == SWI_RCODE_OK )
    {
        printf ("SwiGetServiceStatusEx command is being processed\n");
        printf("    Service Status:  %d\n",eServiceStatus );
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("SwiGetServiceStatusEx command timed out\n");
    }
    else if (retcode == 0xFFFF)
    {
        printf("SwiGetServiceStatusEx command has been commented out\n");
    }
    else
    {
        printf ("SwiGetServiceStatusEx command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\n");
}
 

/**************
 *
 * Name:     doSwiGetPktSessionStatusEx
 *
 * Purpose:  Runs the test to request and receive a GSM Packet Session
 *           status from the modem
 *           the modem 
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    This API replaces the old API SwiGetPktSessionStatus which uses 
 *           an obsolete data structure.
 *            
 *
 **************/ 
package void doSwiGetPktSessionStatusEx( unsigned int timeout )
{
    SWI_RCODE       retcode = 0xFFFF;
    swi_char   lasterror[80];
    struct SWI_STRUCT_PktSessionCall sPktSessionCall;

    /* Get status of packet session assuming <cid> 1 is in progress */
    sPktSessionCall.nProfileId = 1;

    retcode = SwiGetPktSessionStatusEx( &sPktSessionCall, timeout);
    if( retcode == SWI_RCODE_OK )
    {
        printf ("SwiGetPktSessionStatusEx command is being processed\n");
        printf("  Profile Id                            :  %ld\n",
            sPktSessionCall.nProfileId );
        printf("  Size of structure                     :  %ld\n",
            sPktSessionCall.sizeStruct );
        printf("  Activation status                     :  %d\n", 
            sPktSessionCall.bPacketSessionActive );   
        printf("  Valid Qos Valid Flag                  :  %d\n", 
            sPktSessionCall.bValidQos );              
        printf("  Negotiated Precedence                 :  %d\n", 
            sPktSessionCall.ePrec );                 
        printf("  Negotiated Delay                      :  %d\n", 
            sPktSessionCall.eDelay );                
        printf("  Negotiated Reliability                :  %d\n", 
            sPktSessionCall.eReli );                 
        /* Note: Printed in the order of the CnS object */               
        printf("  Negotiated peak                       :  %d\n", 
            sPktSessionCall.ePeak );  
        printf("  Negotiated mean                       :  %d\n", 
            sPktSessionCall.eMean );                
        printf("  Negotiated Traffic Class              :  %d\n", 
            sPktSessionCall.eTrafficClass );        
        printf("  Negotiated  Max Bitrate UL            :  %d\n", 
            sPktSessionCall.eMaxBitrateUl );         
        printf("  Negotiated Max Bitrate DL             :  %d\n", 
            sPktSessionCall.eMaxBitrateDl );    
        printf("  Negotiated Guaranteed Bitrate UL      :  %d\n", 
            sPktSessionCall.eGuarBitrateUl );         
        printf("  Negotiated Guaranteed Bitrate DL      :  %d\n", 
            sPktSessionCall.eGuarBitrateDl );         
        printf("  Negotiated Delivery Order             :  %d\n", 
            sPktSessionCall.eDelOrder );              
        printf("  Negotiated Delivery of Err SDU        :  %d\n", 
            sPktSessionCall.eDelErrSdu );            
        printf("  Negotiated Max SDU Size               :  %ld\n",
            sPktSessionCall.nMaxSdu );                
        printf("  Negotiated SDU Error Ratio            :  %d\n", 
            sPktSessionCall.eSduErrRatio );          
        printf("  Negotiated Residual Bit Error Ratio   :  %d\n", 
            sPktSessionCall.eResBitErrRatio );        
        printf("  Negotiated Transfer Delay             :  %ld\n",
            sPktSessionCall.nTransDelay );            
        printf("  Negotiated Traffic Handling Priority  :  %d\n", 
            sPktSessionCall.eTraffHandPri );         
        printf("  Negotiated Source Statistic Descriptor:  %d\n", 
            sPktSessionCall.eSrcStatDesc );          
        printf("  NegotiatedSignalling Indication       :  %d\n", 
            sPktSessionCall.eSignInd );               
        printf("  Inactivity Reason Code:  %d\n", 
            sPktSessionCall.eInactivityReason );
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("SwiGetPktSessionStatusEx command timed out\n");
    }
    else if (retcode == 0xFFFF)
    {
        printf("SwiGetPktSessionStatusEx command has been commented out\n");
    }
    else
    {
        printf ("SwiGetPktSessionStatusEx command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\n");
}
 
/**************
 *
 * Name:     doSwiSetGsmProfileBasic
 *
 * Purpose:  Runs the test to set the GSM basic profile in the modem 
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    none
 *
 **************/
package void doSwiSetGsmProfileBasic(unsigned int timeout)
{
    SWI_RCODE               retcode;
    swi_char           lasterror[80];
    struct SWI_STRUCT_GsmProfileBasic ProfileBasic;
    struct SWI_STRUCT_GsmProfileBasic *pProfile;

    pProfile = &ProfileBasic;
    /* Clear the structure */
    slmemset((char *)pProfile, 0, 
             sizeof(struct SWI_STRUCT_GsmProfileBasic));
    /* Set profile data */
    pProfile->sizeStruct           = sizeof(struct SWI_STRUCT_GsmProfileBasic);
    pProfile->nProfileId           = 5;
    /* Note: infotype is internal and is set to SWI_PROFILEINFO_Basic during 
     * packing 
     */ 
    pProfile->bValid               = TRUE;
    pProfile->ePdpType             = 0; /* ipv4 PDP type */
    /* Note: Header compression and Data compression fields are filled in the
     *  order they appear in CNS packet. Please note that in the 
     *  SWI_STRUCT_GsmProfileBasic these fields are defined in reverse order.
     */
    pProfile->bHeaderCompression   = 0; /* no compression */
    pProfile->bDataCompression     = 0; /* no compression */

    /* Note: AutomaticIP is set to TRUE for now. */
    pProfile->bAutomaticIP         = FALSE; /* dynamic addressing */
    pProfile->nLengthPdp           = 0; /* dynamic addressing */
    pProfile->byPdp[0]             = 0; /* dynamic addressing */
    slmemcpy (&pProfile->szAccessPoint[0], "access5.com", sizeof ("access5.com"));
    pProfile->ePdpInitiated        = 0;/* Primary MobileInitiated */
    pProfile->nPrimaryProfileId    = 1; /* valid only if PDP initiated 
                                         * type is Secondary Mobile
                                         * initiated 
                                         */
    pProfile->eAuthentication      = 0; /* no authentication */
    slmemcpy (&pProfile->szUserName[0], "testuser5", sizeof ("testuser5"));
    slmemcpy (&pProfile->szPassword[0], "password", sizeof ("password"));
    slmemcpy (&pProfile->szLabel[0], "testlabel5", sizeof ("testlabel5")); 
    pProfile->bAutoActivation      = 1;/* always connected configuration */
    pProfile->bReadOnly            = 0; /* user can read/write the profile */
    pProfile->bPromptPassword      = 0;
    pProfile->bAutoRunVPN          = 0; /* don't auto launch VPN */
    pProfile->nLingerTimer         = 0; /* disable timer */
    pProfile->nSoftwareOptions     = 1; /* HSDPA profile */
    /* Note: pProfile->MccMnc set to 0 with the structure initialization */
    /* Note: pProfile->Reserved set to 0 with the structure initialization */

    retcode     = SwiSetGsmProfileBasic( ProfileBasic, timeout);
    if( retcode == SWI_RCODE_OK )
    {
        printf ("SwiSetGsmProfileBasic command is being processed\n");
        printf("    nProfileId:         %d\n", ProfileBasic.nProfileId);  
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("SwiSetGsmProfileBasic command timed out\n");
    }
    else if (retcode == 0xFFFF)
    {
        printf("SwiSetGsmProfileBasic command has been commented out\n");
    }
    else
    {
        printf ("SwiSetGsmProfileBasic command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\n");
}

/**************
 *
 * Name:     doSwiSetGsmProfileQos
 *
 * Purpose:  Runs the test to set the GSM Qos profile in the modem 
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    none
 *
 **************/
package void doSwiSetGsmProfileQos(unsigned int timeout)
{
    SWI_RCODE               retcode;
    swi_char           lasterror[80];
    struct SWI_STRUCT_GsmProfileQos ProfileQos;
    struct SWI_STRUCT_GsmProfileQos *pProfile;

    pProfile = &ProfileQos;
    /* Clear the structure */
    slmemset((char *)pProfile, 0, sizeof(struct SWI_STRUCT_GsmProfileQos));
    /* Set profile data */
    pProfile->sizeStruct          = sizeof(struct SWI_STRUCT_GsmProfileQos);    
    pProfile->nProfileId          = 4; 
    /* Note: infotype is internal and is set to SWI_PROFILEINFO_Qos (0x01)
     * during packing 
     */ 
    pProfile->bValidUmtsReq       = 1;         
    pProfile->eReqTrafficClass    = 0;  /* subscribed */    
    pProfile->eReqMaxBitrateUl    = 0;  /* subscribed */     
    pProfile->eReqMaxBitrateDl    = 0;     
    pProfile->eReqGuarBitrateUl   = 0;     
    pProfile->eReqGuarBitrateDl   = 0;      
    pProfile->eReqDelOrder        = 0;          
    pProfile->eReqDelErrSdu       = 0;         
    pProfile->nReqMaxSdu          = 1520; /* max value test */        
    pProfile->eReqSduErrRatio     = 0;       
    pProfile->eReqResBitErrRatio  = 0;    
    pProfile->nReqTransDelay      = 4000; /* max value test */         
    pProfile->eReqTraffHandPri    = 0;     
    pProfile->eReqSrcStatDesc     = 0;  /* unknown */    
    pProfile->eReqSignInd         = 0;           
    pProfile->bValidUmtsMin       = 1;        
    pProfile->eMinTrafficClass    = 0;      
    pProfile->eMinMaxBitrateUl    = 0;      
    pProfile->eMinMaxBitrateDl    = 0;      
    pProfile->eMinGuarBitrateUl   = 0;     
    pProfile->eMinGuarBitrateDl   = 0;     
    pProfile->eMinDelOrder        = 0;         
    pProfile->eMinDelErrSdu       = 0;         
    pProfile->nMinMaxSdu          = 1520;  /* max value test */           
    pProfile->eMinSduErrRatio     = 0;      
    pProfile->eMinResBitErrRatio  = 0;   
    pProfile->nMinTransDelay      = 4000;  /* max value test */       
    pProfile->eMinTraffHandPri    = 0;     
    pProfile->eMinSrcStatDesc     = 0; /* unknown */     
    pProfile->eMinSignInd         = 0;          
    pProfile->bValidGprsReq       = 1;          
    pProfile->eReqPrecedence      = 0;       
    pProfile->eReqDelay           = 0;            
    pProfile->eReqReliability     = 0;      
    pProfile->eReqPeak            = 0;              
    pProfile->eReqMean            = 0;              
    pProfile->bValidGprsMin       = 1;        
    pProfile->eMinPrecedence      = 0;       
    pProfile->eMinDelay           = 0;           
    pProfile->eMinReliability     = 0;       
    pProfile->eMinPeak            = 0;             
    pProfile->eMinMean            = 0; 

    retcode     = SwiSetGsmProfileQos( ProfileQos, timeout);
    if( retcode == SWI_RCODE_OK )
    {
        printf ("SwiSetGsmProfileQos command is being processed\n");
        printf("    nProfileId:          %d\n", ProfileQos.nProfileId); 
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("SwiSetGsmProfileQos command timed out\n");
    }
    else if (retcode == 0xFFFF)
    {
        printf("SwiSetGsmProfileQos command has been commented out\n");
    }
    else
    {
        printf ("SwiSetGsmProfileQos command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\n");
}
/**************
 *
 * Name:     doSwiSetGsmProfileDns
 *
 * Purpose:  Runs the test to set the GSM Dns profile in the modem 
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    none
 *
 **************/
package void doSwiSetGsmProfileDns(unsigned int timeout)
{
    SWI_RCODE               retcode;
    swi_char           lasterror[80];
    struct SWI_STRUCT_GsmProfileDns ProfileDns;
    struct SWI_STRUCT_GsmProfileDns *pProfile;

    pProfile = &ProfileDns;
    /* Clear the structure */
    slmemset((char *)pProfile, 0, sizeof(struct SWI_STRUCT_GsmProfileDns));
    /* Set profile data */
    pProfile->sizeStruct          =  sizeof (struct SWI_STRUCT_GsmProfileDns);
    pProfile->nProfileId          =  5;  
    /* Note: infotype is internal and is set to SWI_PROFILEINFO_Dns (0x02) during 
     * packing 
     */ 
    pProfile->nLengthPriDns       = 4;

    pProfile->byPriDns[0] = 127;
    pProfile->byPriDns[1] = 0;
    pProfile->byPriDns[3] = 0;
    pProfile->byPriDns[3] = 1;
    
    pProfile->nLengthSecDns       = 4;

    pProfile->bySecDns[0] = 0x7f;
    pProfile->bySecDns[1] = 0x00;
    pProfile->bySecDns[2] = 0x01;
    pProfile->bySecDns[3] = 0x01;
    
    retcode     = SwiSetGsmProfileDns(ProfileDns, timeout);

    if( retcode == SWI_RCODE_OK )
    {
        printf ("SwiSetGsmProfileDns command is being processed\n");
        printf("    nProfileId:         %d\n", ProfileDns.nProfileId);
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("SwiSetGsmProfileDns command timed out\n");
    }
    else if (retcode == 0xFFFF)
    {
        printf("SwiSetGsmProfileDns command has been commented out\n");
    }
    else
    {
        printf ("SwiSetGsmProfileDns command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\n");
}
/**************
 *
 * Name:     doSwiSetGsmProfileTft
 *
 * Purpose:  Runs the test to set the GSM Tft profile in the modem 
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    none
 *
 **************/
package void doSwiSetGsmProfileTft(unsigned int timeout)
{
    SWI_RCODE                       retcode = 0xFFFF;
    swi_char                        lasterror[80];
    struct SWI_STRUCT_GsmProfileTft ProfileTft;     /* for testing set APIs */
    struct SWI_STRUCT_GsmProfileTft *pProfile = &ProfileTft;

    /* Clear the structure */
    slmemset((char *)pProfile, 0, sizeof(struct SWI_STRUCT_GsmProfileTft));
    /* Set profile data */
    pProfile->sizeStruct          = sizeof(struct SWI_STRUCT_GsmProfileTft);
    pProfile->nProfileId          = 4; 
    /* Note: profile info type is internal and is set to 
     * SWI_PROFILEINFO_Tft (0x03) during packing 
     */ 
    pProfile->nFilterId           = 2; /* current firmware only supports 
                                        * 2 filters, so the Tft valid 
                                        * filter range is 1-2 for now,
                                        * instead of 1-8.
                                        */
    pProfile->bValid              = 0; /* This Tft is not valid */
    pProfile->nEpi                = 0; /* Evaluation precedence index
                                        * (EPI) 0-255
                                        */
    pProfile->nLengthIp           = 4; 
    pProfile->bySrcIp[0]          = 127;
    pProfile->bySrcIp[1]          = 0;
    pProfile->bySrcIp[2]          = 0;
    pProfile->bySrcIp[3]          = 1;    
    pProfile->bySrcIpMask[0]      = 255;    
    pProfile->bySrcIpMask[1]      = 255;    
    pProfile->bySrcIpMask[2]      = 255;    
    pProfile->bySrcIpMask[3]      = 0;    
    pProfile->nProtoNum           = 0;                     
    pProfile->nDestPortFrom       = 0;                   
    pProfile->nDestPortTo         = 0;                     
    pProfile->nSrcPortFrom        = 0;                    
    pProfile->nSrcPortTo          = 0;                     
    pProfile->nSecurParamInd      = 0;                 
    pProfile->nTypeServ           = 0;                     
    pProfile->nTypeServMask       = 0;                  
    pProfile->nFlowLabel          = 0;  

    retcode     = SwiSetGsmProfileTft( ProfileTft, timeout);

    if( retcode == SWI_RCODE_OK )
    {
        printf ("SwiSetGsmProfileTft command is being processed\n");
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("SwiSetGsmProfileTft command timed out\n");
    }
    else if (retcode == 0xFFFF)
    {
        printf("SwiSetGsmProfileTft command has been commented out\n");
    }
    else
    {
        printf ("SwiSetGsmProfileTft command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\n");
}
/**************
 *
 * Name:     doSwiEraseProfile
 *
 * Purpose:  Runs the test to set the GSM basic profile in the modem 
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response 
 * 
 * Return:   none
 *
 * Notes:    none
 *
 **************/
package void doSwiEraseProfile(unsigned int timeout)
{
    SWI_RCODE       retcode = 0xFFFF;
    swi_char   lasterror[80];

    retcode     = SwiEraseProfile( (const swi_uint32)1, timeout);

    if( retcode == SWI_RCODE_OK )
    {
        printf ("SwiEraseProfile command is being processed\n");
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("SwiEraseProfile command timed out\n");
    }
    else if (retcode == 0xFFFF)
    {
        printf("SwiEraseProfile command has been commented out\n");
    }
    else
    {
        printf ("SwiEraseProfile command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\n");
}       
/**************
 *
 * Name:     doSwiGetRat
 *
 * Purpose:  Runs the test to get the RAT mode from the modem
 *
 * Parms:    timeout - the timeout value to use when awaiting a modem response
 *
 * Return:   none
 *
 * Notes:    none
 *
 **************/
package void doSwiGetRat(unsigned int timeout)
{
    SWI_RCODE   retcode = 0xFFFF;
    swi_char    lasterror[80];
    struct SWI_STRUCT_Rat sRat;
    char szRAT[][30] = {"Automatic",
                        "UMTS 3G Only",
                        "GSM 2G Only",
                        "UMTS 3G Preferred",
                        "GSM 2G Preferred"};

    retcode     = SwiGetRat( &sRat, timeout);

    if( retcode == SWI_RCODE_OK )
    {
        printf ("SwiGetRat successful\n");
        printf ("  RAT Mode: %d, %s\n", sRat.eRatMode,
                 &(szRAT[sRat.eRatMode][0]));
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("SwiGetRat command timed out\n");
    }
    else if (retcode == 0xFFFF)
    {
        printf("SwiGetRat command has been commented out\n");
    }
    else
    {
        printf ("SwiGetRat command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\n");
}
/**************
 *
 * Name:     doSwiSetRat
 *
 * Purpose:  Runs the test to Set the RAT mode in the modem
 *
 * Parms:    timeout - the timeout value to use when awaiting a modem response
 *
 * Return:   none
 *
 * Notes:    none
 *
 **************/
package void doSwiSetRat(unsigned int timeout)
{
    SWI_RCODE   retcode = 0xFFFF;
    swi_char    lasterror[80];
    struct SWI_STRUCT_Rat sRat;
    int i;

    enum SWI_TYPE_RatMode saveRat;

    /* Save original RAT mode */
    retcode     = SwiGetRat( &sRat, timeout);

    if( retcode == SWI_RCODE_OK )
    {
         saveRat = sRat.eRatMode;
    }

    /* Loop through all RAT modes */
    for (i=0; i <= SWI_RATMODE_GSM2G_Preferred; i++)
    {
        /* Set RAT mode and read it back */
        sRat.eRatMode = i;
        retcode     = SwiSetRat( &sRat, timeout);

        if( retcode == SWI_RCODE_OK )
        {
            printf ("SwiSetRat successful\n");
            doSwiGetRat (timeout);
        }
        else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
        {
            printf ("SwiSetRat command timed out\n");
        }
        else if (retcode == 0xFFFF)
        {
            printf("SwiSetRat command has been commented out\n");
        }
        else
        {
            printf ("SwiSetRat command is rejected: %d\n", retcode);
            if( retcode == SWI_RCODE_REQUEST_REJECTED )
            {
                SwiGetLastError( lasterror, sizeof(lasterror) );
                printf("  Mdm reports CnS Error: %s\n", lasterror );
            }
        }
        printf("\n");
        sleep (2);
    }

    /* Restore original RAT mode */
    sRat.eRatMode = saveRat;
    retcode     = SwiSetRat( &sRat, timeout);

    if( retcode == SWI_RCODE_OK )
    {
        printf ("SwiSetRat successfully restored\n");
        doSwiGetRat (timeout);
    }

}

/**************
 *
 * Name:     doSwiGetPrlRegion
 *
 * Purpose:  Runs the test to get the Prl Region from the modem
 *
 * Parms:    timeout - the timeout value to use when awaiting a modem response
 *
 * Return:   none
 *
 * Notes:    none
 *
 **************/
package void doSwiGetPrlRegion(unsigned int timeout)
{
    SWI_RCODE   retcode = 0xFFFF;
    swi_char    lasterror[80];
    struct SWI_STRUCT_PrlRegion sPrl;
    char szPRL[][30] = {"00 – default (internal)",
                        "01 – Europe",
                        "02 – North America",
                        "03 – Australia",
                        "04 – Japan",
                        "05 - reserved",
                        "06 – Italy"};

    retcode     = SwiGetPrlRegion( &sPrl, timeout);

    if( retcode == SWI_RCODE_OK )
    {
        printf ("SwiGetPrlRegion successful\n");
        printf ("  PRL Region : %d, %s\n", sPrl.ePrlRegion,
                 &(szPRL[sPrl.ePrlRegion][0]));
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("SwiGetPrlRegion command timed out\n");
    }
    else if (retcode == 0xFFFF)
    {
        printf("SwiGetPrlRegion command has been commented out\n");
    }
    else
    {
        printf ("SwiGetPrlRegion command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\n");
}
/**************
 *
 * Name:     doSwiSetPrlRegion
 *
 * Purpose:  Runs the test to Set the Prl Region in the modem
 *
 * Parms:    timeout - the timeout value to use when awaiting a modem response
 *
 * Return:   none
 *
 * Notes:    none
 *
 **************/
package void doSwiSetPrlRegion(unsigned int timeout)
{
    SWI_RCODE   retcode = 0xFFFF;
    swi_char    lasterror[80];
    struct SWI_STRUCT_PrlRegion sPrl;
    enum SWI_TYPE_PrlRegion savePrl;
    int i;

    /* Save the modem's initial PRL value for restore later */
    retcode     = SwiGetPrlRegion( &sPrl, timeout);

    if( retcode == SWI_RCODE_OK )
    {
        printf ("Initial PRL region value saved\n");
        savePrl = sPrl.ePrlRegion;
    }

    /* This assumes the canned AT!UNLOCK="A710" password */
    slmemset ((char *)sPrl.szUnlockKey, 0, sizeof (sPrl.szUnlockKey));
    slmemcpy ((char *)&sPrl.szUnlockKey[0], "A710", sizeof ("A710"));

    /* Loop through all the defined regions */
    for (i=0; i <= SWI_RRLREGION_Italy; i++)
    {
        /* Set desired region, then read the value back */
        sPrl.ePrlRegion = i;
        retcode     = SwiSetPrlRegion( &sPrl, timeout);

        if( retcode == SWI_RCODE_OK )
        {
            if (sPrl.ePrlChangeStatus == SWI_PRLCHANGE_Success)
            {
                printf ("SwiSetPrlRegion successfully set to %d\n", i);
                doSwiGetPrlRegion (timeout);
            }
            else
            {
                printf ("SwiSetPrlRegion failed with status %d\n",
                        sPrl.ePrlChangeStatus  );
            }
        }
        else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
        {
            printf ("SwiSetPrlRegion command timed out\n");
        }
        else if (retcode == 0xFFFF)
        {
            printf("SwiSetPrlRegion command has been commented out\n");
        }
        else
        {
            printf ("SwiSetPrlRegion command is rejected: %d\n", retcode);
            if( retcode == SWI_RCODE_REQUEST_REJECTED )
            {
                SwiGetLastError( lasterror, sizeof(lasterror) );
                printf("  Mdm reports CnS Error: %s\n", lasterror );
            }
        }
        printf("\n");
        sleep (2);
    }

    /* Restore original value */
    sPrl.ePrlRegion = savePrl;
    retcode     = SwiSetPrlRegion( &sPrl, timeout);

    if( retcode == SWI_RCODE_OK )
    {
        printf ("SwiSetPrlRegion successfully restored\n");
        doSwiGetPrlRegion (timeout);
    }

}

/*
 * $Log: aptestgsmnet.c,v $
 */
