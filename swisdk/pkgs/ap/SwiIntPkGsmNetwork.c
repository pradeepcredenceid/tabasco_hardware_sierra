/**
 *
 * $Id: SwiIntPkGsmNetwork.c,v 1.10 2010/06/10 00:24:10 epasheva Exp $
 *
 * Filename: SwiIntPkGsmNetwork.c
 *
 * Purpose:  Contains Packing routines for CnS objects in the 
 *           GSM Network category
 *
 * Copyright: © 2008 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */
#include "apidefs.h"
#include "SwiApiGsmBasic.h"
#include "SwiApiGsmNetwork.h"


/**
 *
 * This function packs the Gsm Basic Profile  from the 
 * user structure to the CnS  object's parameter field
 *
 * \param sProfile    - user structure with unpacked Basic Profile
 * \param paramfieldp - Pointer to storage into which the packed  
 *                      data will be placed by this function
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint16 SwiPkGsProfileBasic( 
    struct SWI_STRUCT_GsmProfileBasic sProfile, 
    swi_uint8 *paramfieldp)
{
    swi_uint32   i;           /* index variable */
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */
    swi_uint8 nAPNlen;
    
    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    /* Reference ProjectMemo017(UMTS Cns Object Definitions)
     * object: CNS_PROFILE_READ/WRITE 0x7002/0x7003
     */
    *localfieldp++ = sProfile.nProfileId;
    *localfieldp++ = SWI_PROFILEINFO_Basic; 
    *localfieldp++ = sProfile.bValid;
    *localfieldp++ = sProfile.ePdpType;
    *localfieldp++ = sProfile.bHeaderCompression;
    *localfieldp++ = sProfile.bDataCompression;
    if (sProfile.bAutomaticIP)  
    {
        *localfieldp++  = 0; /* PdpLength is set to 0 */
    }
    else
    {
        *localfieldp++  = sProfile.nLengthPdp;
    }
    /* Fixed length of IP/PDP name is 16 - see document referenced at top of
     * function
     */
    for (i = 0; i < 16;  i++)
    {
        *localfieldp++ = sProfile.byPdp[i];
    }

    /* Actual length of the string we're setting, note the maximum of 100 in the
     * next field
     */
    nAPNlen= slstrlen (&(sProfile.szAccessPoint[0]));
    *localfieldp++ = nAPNlen;

    /* Fixed length of AccessPointName is 100 - see document referenced at top
     * of function
     */
    for (i = 0; i < 100;  i++)
    {
        *localfieldp++ = sProfile.szAccessPoint[i];
    }
    *localfieldp++ = sProfile.ePdpInitiated;
    *localfieldp++ = sProfile.nPrimaryProfileId;
    *localfieldp++ = sProfile.eAuthentication;

    /* Fixed length of UserName is 33 - see document referenced at top of
     * function
     */
    for (i = 0; i < 33;  i++)
    {
        *localfieldp++ = sProfile.szUserName[i];
    }
    
    /* Fixed length of Password is 33 - see document referenced at top of
     * function
     */
    for (i = 0; i < 33;  i++)
    {
        *localfieldp++ = sProfile.szPassword[i];
    }
    
    /* Fixed length of Label is 31 - see document referenced at top of function
     */
    for (i = 0; i < 31;  i++)
    {
        *localfieldp++ = sProfile.szLabel[i];
    }
    *localfieldp++ = sProfile.bAutoActivation;
    *localfieldp++ = sProfile.bReadOnly;
    *localfieldp++ = sProfile.bPromptPassword;
    *localfieldp++ = sProfile.bAutoRunVPN;
    piput16( sProfile.nLingerTimer,  &localfieldp); 
    *localfieldp++ = sProfile.nSoftwareOptions;

    /* Fixed length of MccMnc buffer is 3 (packed BCD) - see document referenced
     * at top of function
     */
    for (i = 0; i < 3;  i++)
    {
        *localfieldp++ = sProfile.MccMnc[i];
    }
    
    /* Fixed length of Basic profile reserved bytes is 11 - see document
     * referenced at top of function
     */
    for (i = 0; i < 11;  i++)
    {
        *localfieldp++ = sProfile.Reserved[i];
    }

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
}

/**
 *
 * This function packs the Gsm Basic Profile Username & Password from the CnS
 * object's parameter field
 *
 * \param  pProfile     - Pointer to user structure containing the profile info
 * \param  paramfieldp  - Pointer to storage into which the pack
 *                        data will be placed by this function
 *
 */
package swi_uint16 SwiPkGsProfileAuth(
    struct SWI_STRUCT_GsmProfileBasic *pProfile,
    swi_uint8 *paramfieldp)
{
    swi_uint8    *localparmp; /* local copy of parameter field pointer */
    swi_uint8    *tlvp;       /* local copy of parameter field pointer */

    localparmp = paramfieldp;

    piput16 (APOBJVER1, &localparmp);
    *localparmp++ = pProfile->nProfileId;
    *localparmp++ = OBJ_PROFILE_AUTH_NUMOBJ;

    /* Worker pointer used to walk through a TVL instance. */
    /* First step is to get the start of a TLV instance. */
    tlvp = localparmp;

    /*
     * Copy the username value
     * Set username type
     */
    *tlvp++ = OBJ_TYPE_UserName;

    /* set the length of the username value */
    *tlvp++ = OBJ_LENGTH_UserName;

    /*
     * Each TLV item consists of a 1 byte type value, a 1 byte length value
     * and the actual payload
     */
    piputmulti( (const swi_uint8 *)&(pProfile->szUserName[0]), &tlvp,
                 LENGTH_UserName);

    /*
     * copy the password value
     * Set password type
     */
    *tlvp++ = OBJ_TYPE_Password;

    /* Set the length of the username value */
    *tlvp++ = OBJ_LENGTH_Password;

    /*
     * Each TLV item consists of a 1 byte type value, a 1 byte length value
     * and the actual payload
     */
    piputmulti((const swi_uint8 *)&(pProfile->szPassword[0]), &tlvp,
                LENGTH_Password);

    /* return total length of our packed data */
    return (tlvp - paramfieldp);

}

/**
 *
 * This function packs the Gsm QoS Profile  from the 
 * user structure to the CnS  object's parameter field
 *
 * The number of bytes written into the memory pointed to by
 * paramfieldp
 *
 * \param sProfile           - user structure with unpacked QoS Profile
 * \param paramfieldp        - Pointer to storage into which the packed  
 *                             data will be placed by this function
 *
 */
package swi_uint16 SwiPkGsProfileQos(struct SWI_STRUCT_GsmProfileQos sProfile,
                                     swi_uint8 *paramfieldp)
{
    swi_uint8      *localfieldp;  /* Local copy of paramfieldp */
    swi_uint32      QosInfosz;    /* size of Qos info */

    /* Make a local copy of the parameter field pointer */
    localfieldp    = paramfieldp;

    *localfieldp++ =  sProfile.nProfileId; 
    *localfieldp++ =  SWI_PROFILEINFO_Qos;     
    *localfieldp++ =  sProfile.bValidUmtsReq;         
    *localfieldp++ =  sProfile.eReqTrafficClass;       
    *localfieldp++ =  sProfile.eReqMaxBitrateUl;      
    *localfieldp++ =  sProfile.eReqMaxBitrateDl;     
    *localfieldp++ =  sProfile.eReqGuarBitrateUl;     
    *localfieldp++ =  sProfile.eReqGuarBitrateDl;      
    *localfieldp++ =  sProfile.eReqDelOrder;          
    *localfieldp++ =  sProfile.eReqDelErrSdu; 
    piput16(sProfile.nReqMaxSdu,&localfieldp);           
    *localfieldp++ =  sProfile.eReqSduErrRatio;       
    *localfieldp++ =  sProfile.eReqResBitErrRatio; 
    piput16( sProfile.nReqTransDelay,  &localfieldp);  
    *localfieldp++ =  sProfile.eReqTraffHandPri;     
    *localfieldp++ =  sProfile.eReqSrcStatDesc;       
    *localfieldp++ =  sProfile.eReqSignInd;           
    *localfieldp++ =  sProfile.bValidUmtsMin;        
    *localfieldp++ =  sProfile.eMinTrafficClass;      
    *localfieldp++ =  sProfile.eMinMaxBitrateUl;      
    *localfieldp++ =  sProfile.eMinMaxBitrateDl;      
    *localfieldp++ =  sProfile.eMinGuarBitrateUl;     
    *localfieldp++ =  sProfile.eMinGuarBitrateDl;     
    *localfieldp++ =  sProfile.eMinDelOrder;         
    *localfieldp++ =  sProfile.eMinDelErrSdu; 
    piput16(sProfile.nMinMaxSdu,  &localfieldp);
    *localfieldp++ =  sProfile.eMinSduErrRatio;      
    *localfieldp++ =  sProfile.eMinResBitErrRatio;  
    piput16(sProfile.nMinTransDelay,  &localfieldp);
    *localfieldp++ =  sProfile.eMinTraffHandPri;     
    *localfieldp++ =  sProfile.eMinSrcStatDesc;      
    *localfieldp++ =  sProfile.eMinSignInd;          
    *localfieldp++ =  sProfile.bValidGprsReq;          
    *localfieldp++ =  sProfile.eReqPrecedence;       
    *localfieldp++ =  sProfile.eReqDelay;            
    *localfieldp++ =  sProfile.eReqReliability;      
    *localfieldp++ =  sProfile.eReqPeak;              
    *localfieldp++ =  sProfile.eReqMean;              
    *localfieldp++ =  sProfile.bValidGprsMin;        
    *localfieldp++ =  sProfile.eMinPrecedence;       
    *localfieldp++ =  sProfile.eMinDelay;           
    *localfieldp++ =  sProfile.eMinReliability;       
    *localfieldp++ =  sProfile.eMinPeak;             
    *localfieldp++ =  sProfile.eMinMean; 

    /* ProjMemo017(UMTS CnS Object Definitions)_1.25_CLEAN.pdf 
     * We must send the full 245 bytes (hard-coded here),
     * so we pad the rest of the packet to 0's.
     */
    QosInfosz = localfieldp - paramfieldp;
    slmemset((char *)localfieldp, 0, 245 - QosInfosz);
    
    /* update the pointer after padding */
    localfieldp += 245 - QosInfosz;
    
    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp ); 
}

/**
 *
 * This function packs the Gsm Dns Profile  from the 
 * user structure to the CnS  object's parameter field
 *
 * \param sProfile    - user structure with unpacked Dns Profile
 *                                                                   
 * \param paramfieldp - Pointer to storage into which the packed  
 *                      data will be placed by this function
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint16 SwiPkGsProfileDns(struct SWI_STRUCT_GsmProfileDns  sProfile,
                                     swi_uint8 *paramfieldp )
{
    swi_uint32   i;           /* index variable */
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */
    swi_uint32  DnsInfosz;    /* size of Dns info */
    
    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    /* Pack data */
    *localfieldp++ = sProfile.nProfileId;  
    *localfieldp++ =  SWI_PROFILEINFO_Dns;        
    *localfieldp++ = sProfile.nLengthPriDns;  
    for (i = 0; i < LENGTH_IPAddress;  i++)
    {       
        *localfieldp++ = sProfile.byPriDns[i]; 
    }
    *localfieldp++ = sProfile.nLengthSecDns; 
    for (i = 0; i < LENGTH_IPAddress;  i++)
    {          
        *localfieldp++ = sProfile.bySecDns[i];  
    }

   /* ProjMemo017(UMTS CnS Object Definitions)_1.25_CLEAN.pdf 
     * We must send the full 245 bytes (hard-coded here),
     * so we pad the rest of the packet to 0's.
     */
    DnsInfosz = localfieldp - paramfieldp;
    slmemset((char *)localfieldp, 0, 245 - DnsInfosz);
    
    /* update the pointer after padding */
    localfieldp += 245 - DnsInfosz;

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp ); 
}


/**
 *
 * This function packs the Gsm Tft Profile  from the 
 * user structure to the CnS  object's parameter field
 *
 * \param sProfile     - user structure with unpacked Tft Profile
 *                                                                   
 * \param paramfieldp  - Pointer CnS  object's parameter field
 *                       where the packed  data will be placed 
 *                       by this function
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint16 SwiPkGsProfileTft(struct SWI_STRUCT_GsmProfileTft sProfile, 
                                     swi_uint8 *paramfieldp)
{
    swi_uint32   i;           /* index variable */
    swi_uint8  *localfieldp;  /* Local copy of paramfieldp */
    swi_uint32  TftInfosz;    /* size of TFT info */
    
    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    /* Pack data */
    *localfieldp++ = sProfile.nProfileId; 
    *localfieldp++ =  SWI_PROFILEINFO_Tft; 
    *localfieldp++ = sProfile.nFilterId;
    *localfieldp++ = sProfile.bValid;
    *localfieldp++ = sProfile.nEpi;
    *localfieldp++ = sProfile.nLengthIp;
    for (i = 0; i < LENGTH_IPAddress;  i++)
    { 
        *localfieldp++ = sProfile.bySrcIp[i];
    }
    for (i = 0; i < LENGTH_IPAddress;  i++)
    { 
        *localfieldp++ = sProfile.bySrcIpMask[i];
    }
    *localfieldp++ = sProfile.nProtoNum;                     
    piput16( sProfile.nDestPortFrom, &localfieldp);                  
    piput16( sProfile.nDestPortTo, &localfieldp);                     
    piput16( sProfile.nSrcPortFrom, &localfieldp);                    
    piput16( sProfile.nSrcPortTo, &localfieldp);                     
    piput32( sProfile.nSecurParamInd, &localfieldp);                
    *localfieldp++ = sProfile.nTypeServ;                     
    *localfieldp++ = sProfile.nTypeServMask;                  
    piput32( sProfile.nFlowLabel, & localfieldp);   

   /* ProjMemo017(UMTS CnS Object Definitions)_1.25_CLEAN.pdf 
     * We must send the full 245 bytes (hard-coded here),
     * so we pad the rest of the packet to 0's.
     */
    TftInfosz = localfieldp - paramfieldp;
    slmemset((char *)localfieldp, 0, 245 - TftInfosz);
    
    /* update the pointer after padding */
    localfieldp += 245 - TftInfosz;

    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );     
}

/*
 * $Log: SwiIntPkGsmNetwork.c,v $
 */
