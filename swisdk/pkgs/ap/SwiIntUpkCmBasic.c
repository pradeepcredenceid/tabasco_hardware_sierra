/**
 *
 * $Id: SwiIntUpkCmBasic.c,v 1.15 2010/06/10 00:24:10 epasheva Exp $
 *
 * Filename: SwiIntUpkCmBasic
 *
 * Purpose:  Unpack functions for outgoing request for APIs related to the 
 *           basic common functionality.
 *
 * Copyright: © 2008 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */
#include "SwiApiCmBasic.h"
#include "ap/apidefs.h"

#define SwiRadioPowerUsedBytes 14

/**
 *
 * Unpacks the Radio Power notification. Currently supports object version 2
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   radiop: [OUT] memory location to store the unpacked contents of
 *                        the Radio Power notification
 *
 * \return  none
 *
 */
package void SwiUpkRadioPowerNotify(
    swi_uint8                    *paramp, 
    struct SWI_STRUCT_RadioPower *radiop)
{
    swi_uint16 objversion;

    slmemset((char *)radiop, 0, sizeof(struct SWI_STRUCT_RadioPower));

    objversion = piget16(&paramp);

    /* check if object version 1 fields are present */
    if(objversion >= APOBJVER1)
    {
      /* unpack object version 1 fields */
      radiop->eMode           = piget16(&paramp);
      radiop->nLowPowerReason = piget16(&paramp);
    }
   
    /* check if object version 2 fields are present */
    if(objversion >= APOBJVER2)
    {
      /* unpack object version 2 fields */
      radiop->bModuleReqPowerDown = piget16(&paramp);
      radiop->eRequestReason = piget16(&paramp);
    }
}
/**
 *
 * Unpacks the Radio Power response. Currently supports object version 2
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   radiop: [OUT] memory location to store the unpacked contents of
 *                        the Radio Power set or get response.
 *
 */
package void SwiUpkRadioPowerResp(
    swi_uint8                    *paramp, 
    struct SWI_STRUCT_RadioPower *radiop)
{
    swi_uint16 objversion;

    slmemset((char *)radiop, 0, sizeof(struct SWI_STRUCT_RadioPower));

    objversion = piget16(&paramp);

    /* check if object version 1 fields are present */
    if(objversion >= APOBJVER1)
    {
      radiop->eMode           = piget16(&paramp);
      radiop->nLowPowerReason = piget16(&paramp);
      paramp += SwiRadioPowerUsedBytes;
    }
   
    /* check if object version 2 fields are present */
    if(objversion >= APOBJVER2)
    {
        /* there are no object version 2 fields associated with  */
        /* this response */
    }
}
/**
 *
 * Unpacks the Radio Temperature State parameter field. Currently 
 * supports object version 1
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   radiop: [OUT] memory location to store the unpacked contents of
 *                        the Radio Temperature get response or notification.
 *
 */
package void SwiUpkRadioTemp(
    swi_uint8                        *paramp, 
    struct SWI_STRUCT_RadioTempState *radiop)
{
    swi_uint16 objversion;

    slmemset((char *)radiop, 0, sizeof(struct SWI_STRUCT_RadioTempState));

    objversion = piget16(&paramp);

    /* check if object version 1 fields are present */
    if(objversion >= APOBJVER1)
    {
        radiop->eState   = piget16(&paramp);
        radiop->nCurrent = piget16(&paramp);
        paramp += SwiRadioPowerUsedBytes;
    }
   
    /* check if object version 2 fields are present */
    if(objversion >= APOBJVER2)
    {
        /* for future use */
        /* unpack object version 2 fields */
    }
}
/**
 *
 * Unpacks the Radio Battery Temperature State parameter field. Currently 
 * supports object version 1
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   radiop: [OUT] memory location to store the unpacked contents of
 *                        the Radio Temperature get response or notification.
 *
 */
package void SwiUpkRadioBattery(
    swi_uint8                       *paramp, 
    struct SWI_STRUCT_RadioBattery  *radiop)
{
    swi_uint16 objversion;

    slmemset((char *)radiop, 0, sizeof(struct SWI_STRUCT_RadioBattery));

    objversion = piget16(&paramp);

    /* check if object version 1 fields are present */
    if(objversion >= APOBJVER1)
    {
        radiop->eState   = piget16(&paramp);
        radiop->nCurrent = piget16(&paramp);
        paramp += SwiRadioPowerUsedBytes;
    }
   
    /* check if object version 2 fields are present */
    if(objversion >= APOBJVER2)
    {
        /* for future use */
        /* unpack object version 2 fields */
    }
}
/**
 *
 * Unpacks the Current Radio Band parameter field. Currently supports 
 * object version 1
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   radiop: [OUT] memory location to store the unpacked contents of
 *                        the Current Radio Band get response or notification.
 *
 */
package void SwiUpkCurrentRadioBand(
    swi_uint8                       *paramp, 
    struct SWI_STRUCT_RadioBandList *radiop)
{
    swi_uint16 objversion;
    int i;

    slmemset((char *)radiop, 0, sizeof(struct SWI_STRUCT_RadioBandList));

    objversion = piget16(&paramp);

    /* check if object version 1 fields are present */
    if(objversion >= APOBJVER1)
    {
        radiop->nNumBands = *paramp++;

        for(i = 0; i < MAX_BAND_LIST; i++)
        {
            radiop->sList[i].eType = *paramp++;
            radiop->sList[i].eBand = *paramp++;
        }
    }
   
    /* check if object version 2 fields are present */
    if(objversion >= APOBJVER2)
    {
        /* for future use */
        /* unpack object version 2 fields */
    }
}
/**
 *
 * Unpacks the Radio Band Group Configuration parameter field. Currently 
 * supports object version 1
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   radiop: [OUT] memory location to store the unpacked contents of
 *                        the Radio Band Configuration get response.
 *
 */
package void SwiUpkRadioBandCfg(
    swi_uint8                         *paramp, 
    struct SWI_STRUCT_RadioBandConfig *radiop)
{
    swi_uint16 objversion;
    int i;
    swi_uint64 field;   /* used in unpacking 64-bit values */
    swi_uint8 *packetp; /* used in unpacking 64-bit values */

    slmemset((char *)radiop, 0, sizeof(struct SWI_STRUCT_RadioBandConfig));

    objversion = piget16(&paramp);

    /* check if object version 1 fields are present */
    if(objversion >= APOBJVER1)
    {
        /* Note: The following block of code replaces the call to piget64(). 
         * Please see the comments for that function for more details.
         */
        packetp = paramp;
        field = *packetp++;
        field <<= 8;
        field += *packetp++;
        field <<= 8;
        field += *packetp++;
        field <<= 8;
        field += *packetp++;
        field <<= 8;
        field += *packetp++;
        field <<= 8;
        field += *packetp++;
        field <<= 8;
        field += *packetp++;
        field <<= 8;
        field += *packetp++;
        paramp = packetp;
        radiop->nLastSelected = field;
        
        radiop->nNumInGroup   = *paramp++;

        for(i = 0; i < MAX_BAND_LIST; i++)
        {
            /* Note: The following block of code replaces the call to piget64(). 
             * Please see the comments for that function for more details.
             */
            packetp = paramp;
            field = *packetp++;
            field <<= 8;
            field += *packetp++;
            field <<= 8;
            field += *packetp++;
            field <<= 8;
            field += *packetp++;
            field <<= 8;
            field += *packetp++;
            field <<= 8;
            field += *packetp++;
            field <<= 8;
            field += *packetp++;
            field <<= 8;
            field += *packetp++;
            paramp = packetp;
            radiop->arBandSetting[i] = field;        
        }
    }
   
    /* check if object version 2 fields are present */
    if(objversion >= APOBJVER2)
    {
        /* for future use */
        /* unpack object version 2 fields */
    }
}

/**
 *
 * Unpacks the UsbdInfo. Currently supports object version 1
 *             
 * \param   paramp: [IN]    pointer to parameter field in set request
 * \param   priinfop: [OUT] memory location to store the unpacked contents of
 *                          the Usbd info.
 *
 */
package void SwiUpkPriInfo( 
    swi_uint8                 *paramp, 
    struct SWI_STRUCT_PriInfo *priinfop)
    
{
    swi_uint16 objversion;
    
    objversion = piget16(&paramp);
    
    /*! Major PRI version. */
    priinfop->nMajorVer = piget16(&paramp);

    /*! Minor version. */
    priinfop->nMinorVer = piget16(&paramp);

    /*! Sub-minor version (0xFFFF if unused). */
    priinfop->nSubMinorVer = piget16(&paramp);

    /*! Sub-OEM version (0xFFFF if unused). */
    priinfop->nSubOemVer = piget16(&paramp);

    /*! SKU ID or part number. */
    priinfop->nSkuId= piget32(&paramp);

}
/**
 *
 * Unpacks the Modem Type. Currently supports object version 1
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   pModemType: [OUT] memory location to store the unpacked contents of
 *                            the Modem Type
 *
 */
package void SwiUpkModemType( 
    swi_uint8                    *paramp, 
    struct SWI_STRUCT_ModemType  *pModemType)    
{
    swi_uint16 objversion;
    swi_uint8  nPstringlen;;
    int i;

    objversion = piget16(&paramp);
    
    /*! Major PRI version. */
    pModemType->nModemType = piget16(&paramp);

    /*! Minor version. */
    pModemType->nWirelessTech = piget32(&paramp);
    
    if(objversion > APOBJVER1)
    {
        paramp += 16; 
        nPstringlen = *paramp++;

        for (i=0; i< nPstringlen; i++)
        {
            /*!  */
            pModemType->szProductName[i] = *paramp++;
        }
    }
}

/**
 *
 * Unpacks the Device Time.  Currently supports object version 1
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   pDeviceTime: [OUT] memory location to store the unpacked contents of
 *                             the device time structure
 *
 */
package void SwiUpkDeviceTime( 
    swi_uint8                    *paramp, 
    struct SWI_STRUCT_DeviceTime *pDeviceTime)    
{
    swi_uint16 objversion;

    objversion = piget16(&paramp);
 
    /*! Numerical value representing the year. */
    pDeviceTime->nYear = piget16(&paramp);
    
    /*! Month enumeration.  */
    pDeviceTime->eMonth = piget16(&paramp);
   
    /*! Numerical value representing the day of the month.  Valid values
        range between 1-31. */
    pDeviceTime->nDay = piget16(&paramp);
    
    /*! Day of the week enumeration. */
    pDeviceTime->eWeekDay = piget16(&paramp);

    /*! Numerical value representing the hour.  Valid values range between
        0-23 in UTC. */
     pDeviceTime->nHour = piget16(&paramp);

    /*! Numerical value representing minutes.  Valid values range between
        0-59 in UTC. */
    pDeviceTime->nMinute = piget16(&paramp);

    /*! Numerical value representing seconds.  Valid values range between
        0-59 in UTC. */
     pDeviceTime->nSecond = piget16(&paramp);

    /*! Numerical value representing the time zone.  Values range from -48
        to 48 where 1 equals 15 minutes.  For example:
            - -4 is UTC minus 60 minutes
            - -3 is UTC minus 45 minutes
            - -2 is UTC minus 30 minutes
            - -1 is UTC minus 15 minutes
            - 0 is UTC time
            - 1 is UTC plus 15 minutes
            - 2 is UTC plus 30 minutes */
     pDeviceTime->nTimeZone = piget16(&paramp);

    /*! Daylight saving enumeration.  See SWI_TYPE_DaylightSavings for more
        details. */
    pDeviceTime->eDaylightSavings = *paramp;

}

/**
 *
 * Unpacks the Flash Image Info response. Currently supports object version 1
 *             
 * \param   paramp: [IN]         pointer to parameter field in set request
 * \param   FlashImgInfop: [OUT] memory loc'n to store the unpacked contents of
 *                               the Flash Image Info  set or get response.
 *
 */
package void SwiUpkFlashImgInfoResp(
    swi_uint8                      *paramp, 
    struct SWI_STRUCT_FlashImgInfo *FlashImgInfop)
{
    swi_uint16 objversion;
    swi_uint8  type;
    swi_uint8  length;
    swi_uint8  i;
    swi_uint8  numberofitems;

    slmemset((char *)FlashImgInfop, 0, sizeof(struct SWI_STRUCT_FlashImgInfo));

    objversion = piget16(&paramp);

    /* check if object version 1 fields are present */
    if(objversion >= APOBJVER1)
    {
      FlashImgInfop->eResult = *paramp++;
      pigetmulti(&paramp, (swi_uint8 *)&FlashImgInfop->szImageType[0], 4);
      
      /* get the number of TLV items in the list */
      numberofitems = *paramp++;
      
      for(i=0; i< numberofitems; i++)
      {
          type = *paramp++;
          length = *paramp++;
          
          switch(type)
          {
              case SWI_CDMA_IMAGEVERSION:
              case SWI_CDMA_USDBVERSION:
              case SWI_UMTS_IMAGEVERSION:
              case SWI_UMTS_USDBVERSION:
              case SWI_SWOC_IMAGEVERSION:
              case SWI_GENERIC_IMAGEVERSION:
                pigetmulti(&paramp, (swi_uint8 *)&FlashImgInfop->szVersion[0],
                            length);
                break;
              case SWI_RELEASEDATE:
                pigetmulti(&paramp, (swi_uint8 *)&FlashImgInfop->szRelDate[0],
                            length);
                break;
              case SWI_PRODUCTTYPE:
                pigetmulti(&paramp, (swi_uint8 *)&FlashImgInfop->szProdType[0],
                            length);
                break;
              case SWI_IMAGESIZE:
                if(length == 4)
                {
                    FlashImgInfop->nImgSize = piget32(&paramp);
                }
              default:
                paramp += length;
            }
        }
    }
   
    /* check if object version 2 fields are present */
    if(objversion >= APOBJVER2)
    {
        /* there are no object version 2 fields associated with  */
        /* this response */
    }
}

/**
 *
 * Unpacks the Object Range Revision response. Currently supports object 
 * version 2
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   ObjRngRevp: [OUT] memory location to store the unpacked contents of
 *                            the Object Range Revision get response.
 *
 *
 */
package swi_bool SwiUpkObjRngRev(
    swi_uint8                   *paramp, 
    struct SWI_STRUCT_ObjRngRev *ObjRngRevp)
{
    swi_uint16 objversion;
    swi_uint8    i;

    objversion = piget16(&paramp);
 
    /* if corrupted data */
    if (MAX_OBJRNGINFOBLOCKS < ObjRngRevp->NumOfEntries)
    {
        return FALSE;
    }
    
    /* check if object version 1 fields are present */
    if(objversion >= APOBJVER1)
    {
      ObjRngRevp->NumOfEntries = *paramp++;
      
      for (i = 0; i < ObjRngRevp->NumOfEntries; i++)
      {
          /*! Identifies the CnS object type */
          ObjRngRevp->arInfoBlock[i].eRngSpecifier = *paramp++;
          
          /*! Modem reverse object revision number */
          ObjRngRevp->arInfoBlock[i].nModemReverseObjRevNum = piget16(&paramp);
          
          /*! Modem forward object revision number */
          ObjRngRevp->arInfoBlock[i].nModemForwardObjRevNum = piget16(&paramp);
          
          /* 5 unused bytes in block */
          paramp += 5;
   
          /*! API object revision number */
       }
    }
   
    /* check if object version 2 fields are present */
    if(objversion >= APOBJVER2)
    {
        /* there are no object version 2 fields associated with  */
        /* this response */
    }
    return TRUE;
}

/**
 *
 * Unpacks the Usbd Info response. Currently supports object
 * version 1
 *
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   UsbdInfop: [OUT] memory location to store the unpacked contents of
 *                            the Usnd info get response.
 *
 *
 */
package swi_bool SwiUpkUsbdInfo(
    swi_uint8                   *paramp,
    struct SWI_STRUCT_UsbdInfo  *UsbdInfop)
{
    swi_uint16 objversion;

    swi_uint8 *localparmfp;    /* Local copy of parameter field pointer */

    localparmfp = paramp;

    objversion = piget16(&localparmfp);

    /* check if object version 1 fields are present */
    if(objversion >= APOBJVER1)
    {
        /* Reference: 2110635_CWE_Control_and_Status_Objects_R12_V1.36_CLEAN
         * 3.1.11 USB Descriptor Build Info (CNS_USBD_BUILD_INFO – 0x000E)
         * for the sizes
         */
        slmemcpy(UsbdInfop->szVersion, localparmfp, 84);
        localparmfp += 84;

         slmemcpy(UsbdInfop->szBuildDate, localparmfp, 8);
         /*! API object revision number */
    }

    /* check if object version 2 fields are present */
    if(objversion >= APOBJVER2)
    {
        /* there are no object version 2 fields associated with  */
        /* this response */
    }
    return TRUE;
}

/**
 *
 * Unpacks the NV update from file response. Currently supports object 
 * version 1
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   pNVUpdateFromFile: [OUT] memory location to store the unpacked 
 *                            contents of NV update from file get response.
 *
 *
 */
package void SwiUnpackNVUpdateFromFile(
        swi_uint8 *paramfieldp, 
        SWI_STRUCT_NVUpdateFromFile *pNVUpdateFromFile)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    piget16(&parmfp);   /* Discard obj_ver */

    pNVUpdateFromFile->nStatus = *parmfp++;
    pNVUpdateFromFile->nItemsUpdated = piget32(&parmfp);
    pNVUpdateFromFile->nItemsSipped = piget32(&parmfp);
}

/**
 *
 * Unpacks the remote diag agent support response. Currently supports object 
 * version 1
 *             
 * \param   paramp: [IN]  pointer to parameter field in set request
 * \param   psRDA: [OUT] memory location to store the unpacked 
 *                            contents of remote diag agent support  get 
 *                            response.
 *
 *
 */
package void SwiUnpackRemoteDiagAgentSupport(swi_uint8 *paramfieldp, 
                                             SWI_STRUCT_RDA_SUPPORT *psRDA)
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */

    parmfp = paramfieldp;

    piget16(&parmfp);   /* Discard obj_ver */
    psRDA->nRDASupport = *parmfp++;
    psRDA->nNameLength = *parmfp++;
    if(psRDA->nNameLength <= SWI_RDA_NAME_MAX)
    {
        pigetmulti(&parmfp, (swi_uint8 *)psRDA->Name, psRDA->nNameLength);
    }
    psRDA->nVersionLength = *parmfp++;
    if(psRDA->nVersionLength <= SWI_RDA_VERSION_MAX)
    {
        pigetmulti(&parmfp, (swi_uint8 *)psRDA->Version, psRDA->nVersionLength);
    }
}

/**
 * Name:     SwiUpkAudioProfile
 *
 * Purpose:  This function unpacks the Audio Profile info from the CnS
 *           object's parameter field
 *
 * Parameters: paramfieldp           - Pointer to the CnS parameter field 
 *             pAudioProfile         - Pointer to storage into which the Audio
 *                                     Profile will be placed by this function.
 *
 * Return:    None
 *
 * Note:      None
 *
 */
package void SwiUpkAudioProfile(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_AudioProfile *pAudioProfile )
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    parmfp = paramfieldp;
    swi_uint16 objver;
    
    objver = piget16(&parmfp);
    pAudioProfile->eCurrent = piget16( &parmfp );
    
    if( objver >= 2)
    {
        pAudioProfile->bEarPieceMute = *parmfp++;
        pAudioProfile->bMicMute = *parmfp++;
        pAudioProfile->eVolLevel = *parmfp++;
    }
}

/**
 * Name:     SwiUpkAudioProfileResp
 *
 * Purpose:  This function unpacks the Audio Profile response info from the CnS
 *           object's parameter field
 *
 * Parameters: paramfieldp           - Pointer to the CnS parameter field 
 *             pAudioProfile         - Pointer to storage into which the Audio
 *                                     Profile will be placed by this function.
 *
 * Return:    None
 *
 * Note:      None
 *
 */
package void SwiUpkAudioProfileResp(
    swi_uint8 *paramfieldp, struct SWI_STRUCT_AudioProfile *pAudioProfile )
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    parmfp = paramfieldp;

    pAudioProfile->eActionResult = *parmfp++;
}

/**
 * Name:     SwiUpkDeviceInformationResp
 *
 * Purpose:  This function unpacks the Device Information response info from the 
 *           Device Information object's parameter field
 *
 * Parameters: paramfieldp           - Pointer to the CnS parameter field 
 *             pDeviceInfo           - Pointer to storage into which the Device
 *                                     information will be placed by this 
 *                                     function.
 *
 * Return:    None
 *
 * Note:      None
 *
 */
package void SwiUpkDeviceInformationResp(
    swi_uint8 *paramfieldp, 
    struct SWI_STRUCT_DeviceInformation_Response *pDeviceInfo )
{
    swi_uint8 *parmfp;    /* Local copy of parameter field pointer */
    parmfp = paramfieldp;

    pDeviceInfo->Version                = piget16( &parmfp );
    pDeviceInfo->eRequestStatus         = piget16( &parmfp );
    pDeviceInfo->eDeviceInformationType = piget16( &parmfp );
    pDeviceInfo->nLength                = piget16( &parmfp );
    pDeviceInfo->eTechnology            = piget16( &parmfp );

    pigetmulti ( &parmfp, (swi_uint8 *)&pDeviceInfo->szProduct[0],
                     AMPRODSTRSIZE-1);
    
    pDeviceInfo->eFrmDlwProtocol = piget16( &parmfp );
    pDeviceInfo->eDeviceReEnum   = piget16( &parmfp );
    pDeviceInfo->eCNSsupported   = piget16( &parmfp );
}
/**
 * Name:     SwiUpkSwocCfg
 *
 * Purpose:  This function unpacks the SWOC config response info from the
 *           Device Information object's parameter field
 *
 * Parameters: paramfieldp           - Pointer to the CnS parameter field
 *             pSwocCfg              - Pointer to storage into which the SWOC
 *                                     config will be placed by this
 *                                     function.
 *
 * Return:    None
 *
 * Note:      None
 *
 */
package void SwiUpkSwocCfg (
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_GetSwocCfg *pSwocCfg )
{
    swi_uint16 dummy;

    /* Discard obj_ver - maybe will check at later date */
    dummy                    = piget16( &paramfieldp);
    pSwocCfg->FeatureSupport = piget16( &paramfieldp );
    pSwocCfg->FeatureState   = piget16( &paramfieldp );
    pSwocCfg->FeatureCap     = piget16( &paramfieldp );
    pSwocCfg->LockReq        = piget16( &paramfieldp );
};

/*
 * $Log: SwiIntUpkCmBasic.c,v $
 */
