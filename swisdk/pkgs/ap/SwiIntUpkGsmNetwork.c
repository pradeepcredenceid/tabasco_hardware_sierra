/**
 *
 * $Id: SwiIntUpkGsmNetwork.c,v 1.14 2010/06/10 00:25:24 epasheva Exp $
 *
 * Filename: SwiIntUpkGsmNetwork.c
 *
 * Purpose:  Contains Unpacking routines for CnS objects in the 
 *           GSM Network category
 *
 * Copyright: © 2008 Sierra Wireless, Inc. all rights reserved
 *
 */
#include "apidefs.h"
#include "SwiApiGsmBasic.h"
#include "SwiApiGsmNetwork.h"
#include "SwiApiNotify.h"
#include <string.h>

/**
 *
 * This function unpacks the 16-bit RSSI from the CnS object's parameter field
 *
 * \param      paramfieldp   - Pointer to the CnS parameter field 
 *                             of the CnS object containing the RSSI
 * \param      rssip         - Pointer to storage into which the RSSI
 *                             and Ber will be placed by this function
 *
 */
package void SwiUpkRssiGsm( swi_uint8 *paramfieldp, 
                               struct SWI_STRUCT_Rssi *rssip )
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    swi_int16  siglevel;
    swi_int16  ber;

    parmfp = paramfieldp;

    /* Unpack the rssi field into the local storage */
    siglevel = piget16( &parmfp );
    ber = piget16( &parmfp );

    /* Convert the signal level to dBm based upon the table
     * in "GPRS CnS Object Definitions.doc"
     */
    rssip->iRssi = (swi_int32 )siglevel + GSMRSSIBASE;

    /* TBD Rory - Convert the Ber according to the same document */
    rssip->iBer =  (swi_int32 )ber;
}

/**
 *
 * This function unpacks the receive and transmit data byte 
 * counters from the CnS object's parameter field.
 *
 * \param      paramfieldp - Pointer to the CnS parameter field 
 *                           of the CnS object containing the data counters
 * \param      rssip       - Pointer to storage into which the data counters
 *                           will be placed by this function
 *
 */
package void SwiUpkByteCounters( swi_uint8 *paramfieldp, 
                               swi_uint32 *sentp, swi_uint32 *receivedp)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    /* Unpack the transmitted byte counter */
    *sentp = (swi_int32)piget32( &parmfp );

    /* Unpack the received byte counter */
    *receivedp = (swi_int32)piget32( &parmfp );
}

/**
 *
 * This function unpacks the Gsm Summary Profile from the CnS
 * object's parameter field 
 *
 * \param  paramfieldp        - Pointer to the CnS parameter field of the 
 *                              CnS object containing the profile info
 * \param  pProfileSummary    - Pointer to storage into which the unpack 
 *                              data will be placed by this function
 *
 * 
 */
package void SwiUpkGsProfileSummary(
    swi_uint8 *paramfieldp, 
    struct SWI_STRUCT_GsmProfileSummary* pProfileSummary)
{
    swi_uint32   i;           /* index variable */
    swi_uint8    *parmfp;     /* local copy of parameter field pointer */

    parmfp = paramfieldp;
    /* Clear the structure */
    slmemset((char *)pProfileSummary, 0, sizeof(struct SWI_STRUCT_GsmProfileSummary));
    /* Unpack data */
    pProfileSummary->sizeStruct  = sizeof(struct SWI_STRUCT_GsmProfileSummary);
    pProfileSummary->nMaxProfiles = *parmfp++;
    pProfileSummary->nMaxTft      = *parmfp++;
    pProfileSummary->nDefaultProfileId = *parmfp++; 
    for (i = 0; i < MAX_3G_GSM_PROFILES;  i++)
    {
        pProfileSummary->arrState[i]  = *parmfp++;
    }
}


/**
 *
 * This function unpacks the Gsm Basic Profile  from the CnS
 * object's parameter field
 *
 * \param  paramfieldp  - Pointer to the CnS parameter field
 *                        of the CnS object containing the profile info
 * \param  pProfile     - Pointer to storage into which the unpack 
 *                        data will be placed by this function
 *
 */
package swi_bool SwiUpkGsProfileBasic(
    swi_uint8 *paramfieldp, 
    struct SWI_STRUCT_GsmProfileBasic* pProfile)
{
    swi_uint8    *parmfp;     /* local copy of parameter field pointer */
    swi_uint8    infotype;    
    swi_uint8    nLengthAccessPoint;
    
    parmfp = paramfieldp;
    /* Clear the structure */
    slmemset((char *)pProfile, 0, sizeof(struct SWI_STRUCT_GsmProfileBasic));

    /* Reference ProjectMemo017(UMTS Cns Object Definitions)
     * object: CNS_PROFILE_READ 0x7002
     */
    
    /* Unpack data */
    pProfile->sizeStruct           = sizeof(struct SWI_STRUCT_GsmProfileBasic);
    pProfile->nProfileId           = *parmfp++;
    infotype                       = *parmfp++;  /* get info type */ 
    if (infotype != SWI_PROFILEINFO_Basic)
    {
        /* tbd Elina: log error and return false */
        return FALSE;
    }   
    pProfile->bValid               = *parmfp++;
    pProfile->ePdpType             = *parmfp++;
    /* Note: Header compression and Data compression fields are filled in the
     *  order they appear in CNS packet. Please note that in the 
     *  SWI_STRUCT_GsmProfileBasic these fields are defined in reverse order.
     */
    pProfile->bHeaderCompression   = *parmfp++;
    pProfile->bDataCompression     = *parmfp++;

    /* Note: AutomaticIP is set to TRUE for now. */
    pProfile->bAutomaticIP         = TRUE;

    /* actual length of IP/Pdp name */
    pProfile->nLengthPdp           = *parmfp++; 
    /* Fixed length of IP/PDP name is 16 - see document referenced at top of
     * function
     */
    pigetmulti(&parmfp, (swi_uint8 *)&(pProfile->byPdp[0]), 16); 
    
    /* Get the length of APN field - this is not saved in the struct since
     * the string should be NULL terminated */
    nLengthAccessPoint = *parmfp++;
    /* Copy the name into the previously set to 0 field. Copy only the
     * given length. 
     * Fixed length of AccessPointName is 100 - see document referenced at top
     * of function
     */
    slmemcpy(&(pProfile->szAccessPoint[0]), parmfp, nLengthAccessPoint);
    parmfp += 100;
    
    pProfile->ePdpInitiated        = *parmfp++;
    pProfile->nPrimaryProfileId    = *parmfp++; 
    pProfile->eAuthentication      = *parmfp++;
    
    /* Fixed length of UserName is 33 - see document referenced at top of
     * function
     */
    pigetmulti(&parmfp, (swi_uint8 *)&(pProfile->szUserName[0]), 33);
    
    /* Fixed length of Password is 33 - see document referenced at top of
     * function
     */
    pigetmulti(&parmfp, (swi_uint8 *)&(pProfile->szPassword[0]), 33);
    
    /* Fixed length of Label is 31 - see document referenced at top of function
     */
    pigetmulti(&parmfp, (swi_uint8 *)&(pProfile->szLabel[0]), 31);
    
    pProfile->bAutoActivation      = *parmfp++;
    pProfile->bReadOnly            = *parmfp++;
    pProfile->bPromptPassword      = *parmfp++;
    pProfile->bAutoRunVPN          = *parmfp++;
    pProfile->nLingerTimer         = piget16( &parmfp ); 
    pProfile->nSoftwareOptions     = *parmfp++;
    
    /* Fixed length of MccMnc buffer is 3 (packed BCD) - see document referenced
     * at top of function
     */
    pigetmulti(&parmfp, (swi_uint8 *)&(pProfile->MccMnc[0]), 3);

    /* Fixed length of Basic profile reserved bytes is 11 - see document
     * referenced at top of function
     */
    pigetmulti(&parmfp, (swi_uint8 *)&(pProfile->Reserved[0]), 
               11);
               
    return TRUE;
}

/**
 *
 * This function unpacks the Gsm Basic Profile Username & Password from the CnS
 * object's parameter field
 *
 * \param  paramfieldp  - Pointer to the CnS parameter field
 *                        of the CnS object containing the profile info
 * \param  pProfile     - Pointer to storage into which the unpack
 *                        data will be placed by this function
 *
 */
package swi_bool SwiUpkGsProfileAuth(
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_GsmProfileBasic* pProfile)
{
    swi_uint32   i;           /* index variable */
    swi_uint8    *parmfp;     /* local copy of parameter field pointer */
    swi_uint8    *tlvp;       /* local copy of parameter field pointer */
    swi_uint8    profileid;
    swi_uint8    numobjects;
    swi_uint16   objversion;
    swi_uint8    objtype;
    swi_uint8    objlength;

    parmfp = paramfieldp;
    /* Clear the structure */
    slmemset((char *)pProfile->szUserName, 0, LENGTH_UserName);
    slmemset((char *)pProfile->szPassword, 0, LENGTH_Password);

    objversion = piget16( &parmfp ); /* ignore for now */

    profileid  = *parmfp++;
    numobjects = *parmfp++;

    if (numobjects < 1)
    {
        return FALSE;
    }

    tlvp = parmfp;  /* Worker pointer used to walk through a TVL instance */
    for (i=0; i<numobjects; i++)
    {
        objtype   = *tlvp++;  /* cache object type */
        objlength = *tlvp++;  /* increment pointer and cache object length */

        /* process TLV object based on type */
        switch (objtype)
        {
            case PROFILEAUTH_Username:
                /* unpack Username string */
                pigetmulti(&tlvp, (swi_uint8 *)&(pProfile->szUserName[0]),
                           LENGTH_UserName);

                /* terminate with NULL char */
                pProfile->szUserName[LENGTH_UserName-1] = '\0';
                break;
            case PROFILEAUTH_Password:
                /* unpack password string */
                pigetmulti(&tlvp, (swi_uint8 *)&(pProfile->szPassword[0]),
                            LENGTH_Password);

                /* terminate with NULL char */
                pProfile->szPassword[LENGTH_Password-1] = '\0';
                break;
            case PROFILEAUTH_MaxUsername:
                /* Copy maximum username value to user structure */
                pProfile->nMaxUsernameLength = *tlvp++;
                /* Firmware includes NULL char as part of the length */
                pProfile->nMaxUsernameLength--;
                break;
            case PROFILEAUTH_MaxPassword:
                /* Copy maximum password value to user structure */
                pProfile->nMaxPasswordLength = *tlvp++;
                /* Firmware includes NULL char as part of the length */
                pProfile->nMaxPasswordLength--;
                break;
            default:
                return FALSE;
                break;
        } /* switch */

        /* worker pointer already points to the next TLV instance */

    } /* for */

    return TRUE;
}

/**
 *
 * This function unpacks the Gsm Profile Summary from the CnS
 * object's parameter field
 *
 * \param      paramfieldp - Pointer to the CnS parameter field
 *                           of the CnS object containing the profile info
 * \param      pProfile    - Pointer to storage into which the unpack 
 *                           data will be placed by this function
 *
 */
package swi_bool SwiUpkGsProfileQos(
    swi_uint8 *paramfieldp, 
    struct SWI_STRUCT_GsmProfileQos* pProfile)
{
    swi_uint8    *parmfp;     /* local copy of parameter field pointer */
    swi_uint8    infotype;

    parmfp = paramfieldp;
    /* Clear the structure */
    slmemset((char *)pProfile, 0, sizeof(struct SWI_STRUCT_GsmProfileQos));
    /* Unpack data */
    pProfile->sizeStruct          = sizeof(struct SWI_STRUCT_GsmProfileQos);    
    pProfile->nProfileId          = *parmfp++; 
    infotype                      =  *parmfp++;  /* get info type */ 
    if (infotype != SWI_PROFILEINFO_Qos)
    {
        /* tbd Elina: log error and return false */
        return FALSE;
    }       
    pProfile->bValidUmtsReq       = *parmfp++;         
    pProfile->eReqTrafficClass    = *parmfp++;       
    pProfile->eReqMaxBitrateUl    = *parmfp++;      
    pProfile->eReqMaxBitrateDl    = *parmfp++;     
    pProfile->eReqGuarBitrateUl   = *parmfp++;     
    pProfile->eReqGuarBitrateDl   = *parmfp++;      
    pProfile->eReqDelOrder        = *parmfp++;          
    pProfile->eReqDelErrSdu       = *parmfp++;         
    pProfile->nReqMaxSdu          = piget16( &parmfp );           
    pProfile->eReqSduErrRatio     = *parmfp++;       
    pProfile->eReqResBitErrRatio  = *parmfp++;    
    pProfile->nReqTransDelay      = piget16( &parmfp );         
    pProfile->eReqTraffHandPri    = *parmfp++;     
    pProfile->eReqSrcStatDesc     = *parmfp++;       
    pProfile->eReqSignInd         = *parmfp++;           
    pProfile->bValidUmtsMin       = *parmfp++;        
    pProfile->eMinTrafficClass    = *parmfp++;      
    pProfile->eMinMaxBitrateUl    = *parmfp++;      
    pProfile->eMinMaxBitrateDl    = *parmfp++;      
    pProfile->eMinGuarBitrateUl   = *parmfp++;     
    pProfile->eMinGuarBitrateDl   = *parmfp++;     
    pProfile->eMinDelOrder        = *parmfp++;         
    pProfile->eMinDelErrSdu       = *parmfp++;         
    pProfile->nMinMaxSdu          = piget16( &parmfp );            
    pProfile->eMinSduErrRatio     = *parmfp++;      
    pProfile->eMinResBitErrRatio  = *parmfp++;   
    pProfile->nMinTransDelay      = piget16( &parmfp );         
    pProfile->eMinTraffHandPri    = *parmfp++;     
    pProfile->eMinSrcStatDesc     = *parmfp++;      
    pProfile->eMinSignInd         = *parmfp++;          
    pProfile->bValidGprsReq       = *parmfp++;          
    pProfile->eReqPrecedence      = *parmfp++;       
    pProfile->eReqDelay           = *parmfp++;            
    pProfile->eReqReliability     = *parmfp++;      
    pProfile->eReqPeak            = *parmfp++;              
    pProfile->eReqMean            = *parmfp++;              
    pProfile->bValidGprsMin       = *parmfp++;        
    pProfile->eMinPrecedence      = *parmfp++;       
    pProfile->eMinDelay           = *parmfp++;           
    pProfile->eMinReliability     = *parmfp++;       
    pProfile->eMinPeak            = *parmfp++;             
    pProfile->eMinMean            = *parmfp++; 
    return TRUE; 
}

/**
 *
 * This function unpacks the Gsm Profile Summary from the CnS
 * object's parameter field
 *
 * \param  paramfieldp    - Pointer to the CnS parameter field
 *                          of the CnS object containing the profile info
 * \param  pProfile       - Pointer to storage into which the unpack 
 *                          data will be placed by this function
 *
 */
package swi_bool SwiUpkGsProfileDns(
    swi_uint8 *paramfieldp, 
    struct SWI_STRUCT_GsmProfileDns* pProfile)
{
    swi_uint32   i;           /* index variable */
    swi_uint8    *parmfp;     /* local copy of parameter field pointer */
    swi_uint8    infotype;

    parmfp = paramfieldp;
    /* Clear the structure */
    slmemset((char *)pProfile, 0, sizeof(struct SWI_STRUCT_GsmProfileDns));
    /* Unpack data */
    pProfile->sizeStruct          =  sizeof (struct SWI_STRUCT_GsmProfileDns);
    pProfile->nProfileId          =  *parmfp++;  
    infotype                      =  *parmfp++;  /* get info type */ 
    if (infotype != SWI_PROFILEINFO_Dns)
    {
        /* tbd Elina: log error */
        return FALSE;
    }     
    pProfile->nLengthPriDns       =  *parmfp++;  
    for (i = 0; i < LENGTH_IPAddress;  i++)
    {       
        pProfile->byPriDns[i]     = *parmfp++; 
    }
    pProfile->nLengthSecDns       = *parmfp++; 
    for (i = 0; i < LENGTH_IPAddress;  i++)
    {          
        pProfile->bySecDns[i]     = *parmfp++;  
    }
    return TRUE;
}


/**
 *
 * This function unpacks the Gsm Tft Profile from the CnS
 * object's parameter field
 *
 * \param  paramfieldp    - Pointer to the CnS parameter field
 *                          of the CnS object containing the profile info
 *         pProfile       - Pointer to storage into which the unpack 
 *                          data will be placed by this function
 *
 */
package swi_bool SwiUpkGsProfileTft(
    swi_uint8 *paramfieldp, 
    struct SWI_STRUCT_GsmProfileTft* pProfile)
{
    swi_uint32   i;           /* index variable */
    swi_uint8    *parmfp;     /* local copy of parameter field pointer */
    swi_uint8    infotype;

    parmfp = paramfieldp;
    /* Clear the structure */
    slmemset((char *)pProfile, 0, sizeof(struct SWI_STRUCT_GsmProfileTft));
    /* Unpack data */
    pProfile->sizeStruct          = sizeof(struct SWI_STRUCT_GsmProfileTft);
    pProfile->nProfileId          = *parmfp++; 
    infotype                      = *parmfp++;  /* get info type */ 
    if (infotype != SWI_PROFILEINFO_Tft)
    {
        /* tbd Elina: log error */
        return FALSE;
    }   
    pProfile->nFilterId           = *parmfp++;
    pProfile->bValid              = *parmfp++;
    pProfile->nEpi                = *parmfp++;
    pProfile->nLengthIp           = *parmfp++;
    for (i = 0; i < LENGTH_IPAddress;  i++)
    { 
        pProfile->bySrcIp[i]      = *parmfp++;
    }
    for (i = 0; i < LENGTH_IPAddress;  i++)
    { 
        pProfile->bySrcIpMask[i]  = *parmfp++;
    }
    pProfile->nProtoNum           = *parmfp++;                     
    pProfile->nDestPortFrom       = piget16( &parmfp );                   
    pProfile->nDestPortTo         = piget16( &parmfp );                     
    pProfile->nSrcPortFrom        = piget16( &parmfp );                    
    pProfile->nSrcPortTo          = piget16( &parmfp );                     
    pProfile->nSecurParamInd      = piget32( &parmfp );                 
    pProfile->nTypeServ           = *parmfp++;                     
    pProfile->nTypeServMask       = *parmfp++;                  
    pProfile->nFlowLabel         = piget32( &parmfp );  
    return TRUE;  
}

/**
 *
 * This function unpacks the Gsm Notification for Profile Change 
 * from the CnS object's parameter field
 *
 * \param  paramfieldp    - Pointer to the CnS parameter field
 *                          of the CnS object containing the profile info
 * \param  pProfileChange - Pointer to storage into which the unpack 
 *                          data will be placed by this function
 *
 */
package void SwiUpkGsProfileChange( 
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_GsmProfileChange* pProfileChange)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp                       = paramfieldp;
    /* Clear the structure */
    slmemset((char *)pProfileChange, 0, sizeof(struct SWI_STRUCT_GsmProfileChange));
    /* Unpack data */
    pProfileChange->sizeStruct   = sizeof(struct SWI_STRUCT_GsmProfileChange);
    pProfileChange->nProfileId   = *parmfp++;
    pProfileChange->eType        = *parmfp++;
    pProfileChange->nFilterId    = *parmfp++;
}

/**
 *
 * This function unpacks the Plmn Ready Notification from the CnS 
 * object's parameter field
 *
 * \param  paramfieldp    - Pointer to the CnS parameter field
 *                          of the CnS object containing the plmn
 *                          ready info
 * \param  pPlmnReady     - Pointer to storage into which the unpack 
 *                          data will be placed by this function
 *
 */
package void SwiUpkGsPlmnReady( 
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_PlmnReady* pPlmnReady)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp                    = paramfieldp;
    /* Clear the structure */
    slmemset((char *)pPlmnReady, 0, sizeof(struct SWI_STRUCT_PlmnReady));
    /* Unpack data */
    pPlmnReady->sizeStruct    = sizeof(struct SWI_STRUCT_PlmnReady);
    pPlmnReady->bReady        = *parmfp++;
}


/**
 *
 * This function unpacks the Plmn Mode Notification from the CnS 
 * object's parameter field
 *
 * \param paramfieldp - Pointer to the CnS parameter field
 *                      of the CnS object containing the plmn 
 *                      mode info
 * \param pPlmnMode   - Pointer to storage into which the unpack 
 *                      data will be placed by this function
 * 
 */
package void SwiUpkGsPlmnMode( 
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_PlmnMode* pPlmnMode)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp                   = paramfieldp;
    slmemset((char *)pPlmnMode, 0, sizeof(struct SWI_STRUCT_PlmnMode));
    /* Unpack data */
    pPlmnMode->sizeStruct    = sizeof(struct SWI_STRUCT_PlmnMode);
    pPlmnMode->eResult       = *parmfp++;
}

/**
 *
 * This function unpacks the Gsm Packet Session Status from the CnS
 * object's parameter field
 *
 * \param  paramfieldp    - Pointer to the CnS parameter field
 *                          of the CnS object containing the profile info
 * \param  pSessionStatus - Pointer to storage into which the unpack 
 *                          data will be placed by this function
 *
 */
package void SwiUpkGsPktSessionStatus(
    swi_uint8 *paramfieldp, 
    struct SWI_STRUCT_PktSessionStatus *pSessionStatus)
{

    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    parmfp = paramfieldp;
    /* Clear the structure */
    slmemset((char *)pSessionStatus, 0, sizeof(struct SWI_STRUCT_PktSessionStatus));
    pSessionStatus->sizeStruct = sizeof( struct SWI_STRUCT_PktSessionStatus);
    pSessionStatus->bPacketSessionActive  = *parmfp++;
    pSessionStatus->bValidQos             = *parmfp++;
    pSessionStatus->eNegoPrecedence       = *parmfp++;
    pSessionStatus->eNegoDelay            = *parmfp++;
    pSessionStatus->eNegoReliability      = *parmfp++;
    pSessionStatus->eNegoMeanThroughput   = *parmfp++;
    pSessionStatus->eNegoPeakThroughput   = *parmfp++;
    pSessionStatus->eInactivityReason     = piget16( &parmfp );

}

/**
 *
 * This function unpacks the Gsm Packet Session Call Status from the 
 * CnS object's parameter field. This unpack function is used by 
 * CNS_PKT_SESSION CALL Notification and by CNS_PKT_SESSION_STATUS Set 
 * Response.
 *
 * \param  paramfieldp    - Pointer to the CnS parameter field
 *                          of the CnS object containing the profile info
 * \param  pPktSessionCall - Pointer to storage into which the unpack 
 *                          data will be placed by this function
 *
 */
package void SwiUpkGsPktSessionStatusExp(
    swi_uint8 *paramfieldp, 
    struct SWI_STRUCT_PktSessionCall *pPktSessionCall)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    parmfp                      = paramfieldp;
    /* Clear the structure */
    slmemset((char *)pPktSessionCall, 0,
              sizeof(struct SWI_STRUCT_PktSessionCall));
    pPktSessionCall->sizeStruct = sizeof( struct SWI_STRUCT_PktSessionCall);              
    pPktSessionCall->nProfileId           = *parmfp++;            
    pPktSessionCall->bPacketSessionActive = *parmfp++;   
    pPktSessionCall->bValidQos            = *parmfp++;            
    pPktSessionCall->ePrec                = *parmfp++;            
    pPktSessionCall->eDelay               = *parmfp++;                 
    pPktSessionCall->eReli                = *parmfp++; 
    pPktSessionCall->ePeak                = *parmfp++;                 
    pPktSessionCall->eMean                = *parmfp++;                  
    pPktSessionCall->eTrafficClass        = *parmfp++;          
    pPktSessionCall->eMaxBitrateUl        = *parmfp++;           
    pPktSessionCall->eMaxBitrateDl        = *parmfp++;        
    pPktSessionCall->eGuarBitrateUl       = *parmfp++;           
    pPktSessionCall->eGuarBitrateDl       = *parmfp++;           
    pPktSessionCall->eDelOrder            = *parmfp++; 
    pPktSessionCall->eDelErrSdu           = *parmfp++;  
    pPktSessionCall->nMaxSdu              = piget16( &parmfp );                 
    pPktSessionCall->eSduErrRatio         = *parmfp++;           
    pPktSessionCall->eResBitErrRatio      = *parmfp++;        
    pPktSessionCall->nTransDelay          = piget16( &parmfp );         
    pPktSessionCall->eTraffHandPri        = *parmfp++;          
    pPktSessionCall->eSrcStatDesc         = *parmfp++;           
    pPktSessionCall->eSignInd             = *parmfp++;               
    pPktSessionCall->eInactivityReason    = piget16( &parmfp );
}


/**
 *
 * This function unpacks the Gsm Received Signal Code Power (RSCP)
 * and the total energy per chip per power density (Ec/Io) value 
 * of the active set's strongest cells  from the CnS
 * object's parameter field
 *
 * \param  paramfieldp    - Pointer to the CnS parameter field
 *                          of the CnS object containing the profile info
 * \param  pRscpEcIo      - Pointer to storage into which the unpack 
 *                          data will be placed by this function
 *
 */
package void SwiUpkGsRscpEcIo(
    swi_uint8 *paramfieldp, 
    struct SWI_STRUCT_RscpEcIo* pRscpEcIo)
{
    swi_uint32   i;       /* index variable */
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    swi_uint16 objectversion;

    parmfp                   = paramfieldp;
    objectversion            = piget16( &parmfp );
    /* Clear the structure */
    slmemset((char *)pRscpEcIo, 0, sizeof(struct SWI_STRUCT_RscpEcIo));
    pRscpEcIo->sizeStruct    = sizeof( struct SWI_STRUCT_RscpEcIo );
    pRscpEcIo->nNumOfCells   = piget16( &parmfp );
    for ( i=0; i<MAX_CELLS; i++ )
    {
        pRscpEcIo->arCellInfo[i].nPriScramCode = piget16( &parmfp );
        pRscpEcIo->arCellInfo[i].nRscp         = piget16( &parmfp );
        pRscpEcIo->arCellInfo[i].nEcIo         = piget16( &parmfp );
    }       
}

/**
 *
 * This function unpacks System and Network Status information
 * from the CnS object's parameter field
 *
 * \param  paramfieldp    - Pointer to the CnS parameter field
 *                          of the CnS object containing the profile info
 * \param  pSessionStatus - Pointer to storage into which the unpack 
 *                          data will be placed by this function
 *
 */
package void SwiUpkGsSystemStatus(
    swi_uint8 *paramfieldp, 
    struct SWI_STRUCT_GsmSystemStatus* pSysStatus)
{
    swi_uint8 *parmfp;         /* local copy of parameter field pointer */
    swi_uint8 numchars;
    swi_uint16 objectversion;  /* object version variable */

    parmfp = paramfieldp;
    /* Clear the structure */
    slmemset((char *)pSysStatus, 0, sizeof(struct SWI_STRUCT_GsmSystemStatus));
    pSysStatus->sizeStruct       = sizeof(struct SWI_STRUCT_GsmSystemStatus);
    
    /*  Get object version number  */
    objectversion                = piget16( &parmfp );
    /* Note: Object version determines the size of the object for unpacking */
    pSysStatus->eModemStatus     = piget16( &parmfp );
    pSysStatus->eServiceStatus   = piget16( &parmfp );
    pSysStatus->eServiceErr      = piget16( &parmfp );
    pSysStatus->eServiceType     = *parmfp++;
    pSysStatus->eSystemMode      = *parmfp++;
    pSysStatus->eCurrentBand     = piget16( &parmfp );
    pSysStatus->bRoaming         = *parmfp++;
    pSysStatus->eManualMode      = *parmfp++;

    /* Unpack Country string */
    /* Get the length of the field */
    numchars = *parmfp++;
    
    /* unpack Country string (convert from UCS2 to UTF8) */
    SwiIntUcs2Unpack(&parmfp, 
                     &pSysStatus->szCountry[0], 
                     LENGTH_Country, 
                     numchars, 
                     6);

    /* Unpack Network string */
    /* Get the length of network name field */
    numchars = *parmfp++;
    
    /* unpack Network Name string (convert from UCS2 to UTF8) */
    SwiIntUcs2Unpack(&parmfp, 
                     &pSysStatus->szNetwork[0], 
                     LENGTH_Network,
                     numchars, 
                     16);

    pSysStatus->nMcc             = piget16( &parmfp );
    pSysStatus->nMnc             = piget16( &parmfp );
    pSysStatus->nLac             = piget16( &parmfp );
    pSysStatus->nRac             = piget16( &parmfp );
    pSysStatus->nCellId          = piget16( &parmfp );
    pSysStatus->nChannelNum      = piget16( &parmfp );
    pSysStatus->nPriScramCode    = piget16( &parmfp ); /* 0 for Gsm, 
                                                        * WCDMA only
                                                        */
    /* Unpack these fields only if object revision is 2 or higher */
    if ( objectversion >= APOBJVER2 )
    {
        pSysStatus->eMncFormat          = *parmfp++;
        pSysStatus->nPlmnServiceErrMask = *parmfp++;
    }
    /* Unpack this field only if object revision is 3 or higher */
    if ( objectversion >= APOBJVER3 )
    {
        pSysStatus->nCellId      = piget32( &parmfp ); /* overwritten for 
                                                        * object revision
                                                        * that is 3 or higher
                                                        */
    }
}

/**
 *
 * This function unpacks IP address information from the CnS object's 
 * parameter field
 *
 * \param  paramfieldp    - Pointer to the CnS parameter field
 *                          of the CnS object with profile info
 * \param  pSessionStatus - Pointer to storage into which the unpack 
 *                          data will be placed by this function
 *
 */
package void SwiUpkGsIPAddress(
    swi_uint8 *paramfieldp, 
    struct SWI_STRUCT_IpAddress* pIp)
{
    swi_uint32   i;       /* index variable */
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;
    /* Clear the structure */
    slmemset((char *)pIp, 0, sizeof(struct SWI_STRUCT_IpAddress));
    pIp->sizeStruct          = sizeof(struct SWI_STRUCT_IpAddress); 
    pIp->nProfileId          = *parmfp++;
    pIp->nIpLength           = *parmfp++; 
    for (i = 0; i < LENGTH_IPAddress;  i++)
    {
        pIp->byIp[i]         = *parmfp++;
    }
}


/**
 *
 * This function unpacks PLMNSelection information from the CnS 
 * object's parameter field
 *
 * \param  paramfieldp    - Pointer to the CnS parameter field
 *                          of the CnS object containing the profile info
 * \param  pPlmn          - Pointer to storage into which the unpack 
 *                          data will be placed by this function
 *
 */
package void  SwiUpkGsPLMNSelection(
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_PlmnSelection* pPlmn)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    swi_uint8 namelen, abvnamelen;
    swi_uint8 plmnstatus;
    enum SWI_TYPE_PLMN_Mode eMode;
    
    parmfp = paramfieldp;
    /* Clear the structure, preserving fields not in the CNS object */
    eMode = pPlmn->eMode;
    slmemset((char *)pPlmn, 0, sizeof(struct SWI_STRUCT_PlmnSelection));
    pPlmn->sizeStruct          = sizeof(struct SWI_STRUCT_PlmnSelection); 
    pPlmn->eMode               = eMode;

    /* Reference: ProMemo036_GPRS Cns Object definitions
     * CNS_MANUAL_SELECT_EXP (0x1042)
     */

    /* Unpack full PLMN name */
    /* Get the length of full PLMN name */
    namelen = *parmfp++;
    
    /* unpack full PLMN string (convert from UCS2 to UTF8) */
    SwiIntUcs2Unpack(&parmfp, 
                     &pPlmn->szName[0], 
                     LENGTH_PLMN,
                     namelen, 
                     62);

    /* Unpack abreviated PLMN name */
    /* Get the length of abreviated PLMN name field */
    abvnamelen = *parmfp++;
 
    /* unpack full PLMN string (convert from UCS2 to UTF8) */
    SwiIntUcs2Unpack(&parmfp, 
                     &pPlmn->szAbvPLMN[0], 
                     LENGTH_AbvPLMN,
                     abvnamelen, 
                     26);   

    pPlmn->nMCC                = piget16( &parmfp );
    pPlmn->nMNC                = piget16( &parmfp );
    
    plmnstatus                 = *parmfp++;
    if (PLMN_REGISTERED & plmnstatus)
        pPlmn->bRegistered         = TRUE;
    if (PLMN_FORBIDDEN & plmnstatus)
        pPlmn->bForbidden          = TRUE;
    if (PLMN_HOME & plmnstatus)
        pPlmn->bHome               = TRUE;
    if (PLMN_WEAK & plmnstatus)
        pPlmn->bWeak               = TRUE;
    if (PLMN_GPRS_BIT & plmnstatus)
        pPlmn->bGprsSupport        = TRUE;
        
    pPlmn->eNetwork            = *parmfp++;
    pPlmn->bMorePlmn           = *parmfp++;
}


/**
 *
 * This function unpacks PLMNSelection information from the CnS 
 * object's parameter field
 *
 * \param  paramfieldp - Pointer to the CnS parameter field
 *                       of the CnS object containing the profile info
 * \param  pPlmn       - Pointer to storage into which the unpack 
 *                       data will be placed by this function
 *
 */
package void  SwiUpkGsRegistration(swi_uint8 *paramfieldp,
    struct SWI_STRUCT_Registration *pRegistration)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    swi_uint8 length;

    parmfp = paramfieldp;

    /* Clear the structure */
    slmemset((char *)pRegistration, 0, sizeof(struct SWI_STRUCT_Registration));
    pRegistration->sizeStruct = sizeof(struct SWI_STRUCT_Registration); 

    /* Get display home string flag */
    pRegistration->bDisplayHomeString = *parmfp++; 

    /* Get the length of SPN field */
    length = *parmfp++;
    
    /* unpack SPN string (convert from UCS2 to UTF8) */
    SwiIntUcs2Unpack(&parmfp, 
                     &pRegistration->szSPN[0], 
                     LENGTH_SPN,
                     length, 
                     32);
 
    /* Get the length of PLMN field */
    length = *parmfp++;

    /* unpack PLMN string (convert from UCS2 to UTF8) */
    SwiIntUcs2Unpack(&parmfp, 
                     &pRegistration->szPLMN[0], 
                     LENGTH_PLMN,
                     length, 
                     62);

    /* Get the length of AbvPLMN field */
    length = *parmfp++;

    /* unpack AbvPLMN string (convert from UCS2 to UTF8) */
    SwiIntUcs2Unpack(&parmfp, 
                    &pRegistration->szAbvPLMN[0], 
                    LENGTH_AbvPLMN,
                    length, 
                    26);

    /* get display home country roaming flag */
    pRegistration->bDisplayHCR = *parmfp++; 
}

/**
 *
 * This function unpacks Service Icon  information from the CnS object's 
 * parameter field
 *
 * \param  paramfieldp    - Pointer to the CnS parameter field
 *                          of the CnS object containing the service
 *                          icon info
 * \param  pPlmn          - Pointer to storage into which the unpack 
 *                          data will be placed by this function
 *
 */
package void  SwiUpkGsServiceIcon( 
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_ServiceIcon* pServiceIcon)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;
    /* Clear the structure */
    slmemset((char *)pServiceIcon, 0, sizeof(struct SWI_STRUCT_ServiceIcon));
    pServiceIcon->sizeStruct        = sizeof(struct SWI_STRUCT_ServiceIcon); 
    pServiceIcon->nVersion          = piget16( &parmfp ); 
    pServiceIcon->bDisplayIcon      = *parmfp++; 
    pServiceIcon->eService          = *parmfp++; 
    pServiceIcon->bAttached         = *parmfp++; 
    pServiceIcon->bPktSessionActive = *parmfp++; 
}

/**
 *
 * This function unpacks Service Icon  information from the CnS object's 
 * parameter field
 *
 * \param  paramfieldp    - Pointer to the CnS parameter field
 *                          of the CnS object containing the PLMN
 *                          available info
 * \param  pPlmn          - Pointer to storage into which the unpack 
 *                          data will be placed by this function
 *
 */
package void  SwiUpkGsPlmnAvailable( 
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_PlmnAvailable* pPlmn)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp             = paramfieldp;
    pPlmn->sizeStruct  = sizeof(struct SWI_STRUCT_PlmnAvailable);
    pPlmn->bAvailable  = *parmfp++; 
}

/**
 *
 * This function unpacks Network Status  information from the CnS 
 * object's parameter field
 *
 * \param  paramfieldp    - Pointer to the CnS parameter field
 *                          of the CnS object containing the service
 *                          icon info
 * \param  pNetStatus     - Pointer to storage into which the unpack 
 *                          data will be placed by this function
 *
 */
package void  SwiUpkGsNetworkStatus( 
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_GSM_NetworkStatus* pNetStatus)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;
    /* Clear the structure */
    slmemset((char *)pNetStatus, 0, sizeof(struct SWI_STRUCT_GSM_NetworkStatus));
    pNetStatus->sizeStruct      = sizeof(struct SWI_STRUCT_GSM_NetworkStatus); 
    pNetStatus->eServiceStatus  = piget16( &parmfp ); 
    pNetStatus->eServiceType    = *parmfp++; 
    pNetStatus->eRoaming        = *parmfp++; 
    pNetStatus->eModeSelection  = *parmfp++; 
    /* Get the length of Country field */
    pNetStatus->szCountry[0] = *parmfp++;
    /* Copy the name into the previously set to 0 field */
    pigetmulti(&paramfieldp, (swi_uint8 *)&(pNetStatus->szCountry[1]), 
               pNetStatus->szCountry[0]); 
    /* Get the length of Network field */
    pNetStatus->szNetwork[0] = *parmfp++;
    /* Copy the name into the previously set to 0 field */
    pigetmulti(&paramfieldp, (swi_uint8 *)&(pNetStatus->szNetwork[1]), 
               pNetStatus->szNetwork[0]); 
    pNetStatus->nMCC    = piget16( &parmfp );              
    pNetStatus->nMNC    = piget16( &parmfp );               
    pNetStatus->nLAC    = piget16( &parmfp );               
    pNetStatus->nCellID = piget16( &parmfp );    
}

/**
 *
 * This function unpacks the Cell Info from the CnS object's parameter field
 *
 * \param  paramfieldp   - Pointer to the CnS parameter field
 *                         of the CnS object containing the cell info
 * \param  pCellInfo     - Pointer to storage into which the cell info
 *                         will be placed by this function
 * \param  versionp      - Pointer to flag indicating 
 *                         whether the notification version
 *                         is supported
 *
 */
package void SwiUpkGsCellInfo (swi_uint8 *paramfieldp, 
                               struct SWI_STRUCT_GsmCellInfo *pCellInfo,
                               swi_bool *versionp) 
{
    swi_uint32   i;           /* index variable */
    swi_uint32   objectver;   /* object version */
    swi_uint8    *parmfp;     /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    /* Check whether we support the object */  
    objectver = piget16(&parmfp);   /* read object version and 
                                     * advance pointer to param. field 
                                     */
    if (objectver < APOBJVER2)
    {
        /* Note: we do not support object version less than 
         * APOBJVER2 
         */
        /* TBD ("CNS_GSM_CELL_INFO object version not supported: %i"), ObjectVersion); */
        *versionp = FALSE;  /* unsupported object version */
        /* Clear the structure */
        slmemset ((char *)pCellInfo, 0, sizeof(struct SWI_STRUCT_GsmCellInfo) );
    }
    else
    {
        *versionp = TRUE;  /* supported object version */
        /* Unpack the packet only if supported version */
        slmemset ((char *)pCellInfo, 0, sizeof(struct SWI_STRUCT_GsmCellInfo) );
        pCellInfo->sizeStruct = sizeof(struct SWI_STRUCT_GsmCellInfo);

        if ( *versionp )
        {
            /* unpack 1 byte */
            pigetmulti(&parmfp, (swi_uint8 *)&(pCellInfo->bValid), 1 );
        }
        else
        {
            pCellInfo->bValid = 0;  /* indicate invalid object */
        }
        pCellInfo->nMcc        = piget16(&parmfp); 
        pCellInfo->nMnc        = piget16(&parmfp);
        pCellInfo->nLac        = piget16(&parmfp);
        pCellInfo->nCellId     = piget16(&parmfp);
        pigetmulti(&parmfp, (swi_uint8 *)&(pCellInfo->nBsic), 1 ); 
        pigetmulti(&parmfp, (swi_uint8 *)&(pCellInfo->nNetColorCode), 1 ); 
        pigetmulti(&parmfp, (swi_uint8 *)&(pCellInfo->nBsColorCode), 1 ); 
        pCellInfo->nRac        = piget16(&parmfp);
        pCellInfo->nMinRxLevel = piget16(&parmfp); 
        pCellInfo->nMaxRach    = piget16(&parmfp);

        /* unpack SWI_STRUCT_GsmCellMeasure   sMeasurements; */
        pCellInfo->sMeasurements.sizeStruct = sizeof(struct SWI_STRUCT_GsmCellMeasure);
        pigetmulti(&parmfp, (swi_uint8 *)&(pCellInfo->sMeasurements.eServStatus),
                   1);
        pigetmulti(&parmfp, (swi_uint8 *)&(pCellInfo->sMeasurements.eRadioBand),
                   1);
        pCellInfo->sMeasurements.nArfcn = piget16(&parmfp);
        pCellInfo->sMeasurements.nRxLevel = piget16(&parmfp);
        pigetmulti(&parmfp, (swi_uint8 *)&(pCellInfo->sMeasurements.nC1), 1);
        pigetmulti(&parmfp, (swi_uint8 *)&(pCellInfo->sMeasurements.nC2), 1);
        pigetmulti(&parmfp, (swi_uint8 *)&(pCellInfo->sMeasurements.nC31), 1);
        pigetmulti(&parmfp, (swi_uint8 *)&(pCellInfo->sMeasurements.nC32), 1);
        if ( *versionp )
        {
            pigetmulti(&parmfp, (swi_uint8 *)&(pCellInfo->nNumValidNeighborCells),
                       1 ); 
        }
        else
        {
            pCellInfo->nNumValidNeighborCells = 0; /* invalid object setup */  
        }
        /* unpack arNeighborCells[] only if valid */
        if ( 0 < (pCellInfo->nNumValidNeighborCells) )
        {
            for (i = 0; i < MAX_CELLS;  i++)
            {
                pCellInfo->arNeighborCells[i].sizeStruct =
                    sizeof(struct SWI_STRUCT_GsmCellMeasure);
                pigetmulti(&parmfp,
                           (swi_uint8 *)&(pCellInfo->arNeighborCells[i].eServStatus), 1);
                pigetmulti(&parmfp,
                           (swi_uint8 *)&(pCellInfo->arNeighborCells[i].eRadioBand), 1);
                pCellInfo->arNeighborCells[i].nArfcn = piget16(&parmfp);
                pCellInfo->arNeighborCells[i].nRxLevel = piget16(&parmfp);
                pigetmulti(&parmfp,
                          (swi_uint8 *)&(pCellInfo->arNeighborCells[i].nC1), 1);
                pigetmulti(&parmfp,
                          (swi_uint8 *)&(pCellInfo->arNeighborCells[i].nC2), 1);
                pigetmulti(&parmfp,
                          (swi_uint8 *)&(pCellInfo->arNeighborCells[i].nC31), 1);
                pigetmulti(&parmfp,
                          (swi_uint8 *)&(pCellInfo->arNeighborCells[i].nC32), 1);
            } 
        }
    } /* end unpack valid packet */
}

/**
 * This function unpacks the RAT data from the CnS
 * object's parameter field for the SwiGetRat API
 *
 * \param  paramfieldp   - Pointer to the CnS parameter field of the CnS
 *                         object containing the RAT mode
 * \param  psRat         - Pointer to storage into which the RAT mode
 *                         will be placed by this function
 * \param  pbversion     - Pointer to flag indicating whether the object version
 *                         is supported
 *
 */
package void SwiUpkGsGetRat (
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_Rat *psRat,
    swi_bool *pbversion)
{
    swi_uint16  objectver;

    memset (psRat, 0, sizeof (struct SWI_STRUCT_Rat));

    psRat->sizeStruct = sizeof (struct SWI_STRUCT_Rat);

    /* See CNS object definitions */
    *pbversion = FALSE;
    objectver   = piget16 (&paramfieldp);
    if (objectver < APOBJVER2)
    {
        *pbversion = TRUE;
    }

    psRat->eRatMode = *paramfieldp;
}

/**
 * This function unpacks the RAT data from the CnS
 * object's parameter field for the SwiSetRat API
 *
 * \param  paramfieldp   - Pointer to the CnS parameter field of the CnS
 *                         object containing the RAT mode
 * \param  psRat         - Pointer to storage into which the set RAT mode
 *                         result will be placed by this function
 * \param  pbversion     - Pointer to flag indicating whether the object version
 *                         is supported
 *
 */
package void SwiUpkGsSetRat (
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_Rat *psRat,
    swi_bool *pbversion)
{
    swi_uint16  objectver;

    /* See CNS object definitions */
    *pbversion = FALSE;
    objectver   = piget16 (&paramfieldp);
    if (objectver < APOBJVER2)
    {
        *pbversion = TRUE;
    }

    psRat->bSuccess = *paramfieldp;
}

/**
 * This function unpacks the Prl Region data from the CnS
 * object's parameter field for the SwiGetPrlRegion API
 *
 * \param  paramfieldp   - Pointer to the CnS parameter field of the CnS
 *                         object containing the Prl Region
 * \param  psPrl         - Pointer to storage into which the Prl Region data
 *                         will be placed by this function
 * \param  pbversion     - Pointer to flag indicating whether the object version
 *                         is supported
 *
 */
package void SwiUpkGsGetPrlRegion (
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_PrlRegion *psPrl,
    swi_bool *pbversion)
{
    swi_uint16  objectver;

    memset (psPrl, 0, sizeof (struct SWI_STRUCT_PrlRegion));

    psPrl->sizeStruct = sizeof (struct SWI_STRUCT_PrlRegion);
    /* See CNS object definitions */
    *pbversion = FALSE;
    objectver   = piget16 (&paramfieldp);
    if (objectver < APOBJVER2)
    {
        *pbversion = TRUE;
    }

    psPrl->ePrlRegion = *paramfieldp;
}

/**
 * This function unpacks the Prl Region data from the CnS
 * object's parameter field for the SwiSetPrlRegion API
 *
 * \param  paramfieldp   - Pointer to the CnS parameter field of the CnS
 *                         object containing the Prl Region
 * \param  psPrl         - Pointer to storage into which the set  Prl Region
 *                         result will be placed by this function
 * \param  pbversion     - Pointer to flag indicating whether the object version
 *                         is supported
 *
 */
package void SwiUpkGsSetPrlRegion (
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_PrlRegion *psPrl,
    swi_bool *pbversion)
{
    swi_uint16  objectver;

    /* See CNS object definitions */
    *pbversion = FALSE;
    objectver   = piget16 (&paramfieldp);
    if (objectver < APOBJVER2)
    {
        *pbversion = TRUE;
    }
    psPrl->ePrlChangeStatus = *paramfieldp;
}

/*
 * $Log: SwiIntUpkGsmNetwork.c,v $
 */
