/*
 * \ingroup cbk
 *
 * \file    qaCbkImsNotify.c
 *
 * \brief   Contains routines for the IMS Notifications
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */

#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaGobiApiCbk.h"
#include "qaQmiNotify.h"
#include "sludefs.h"

/*************
 *
 * Name:    qaQmiIMSConfigNotify
 *
 * Purpose: To notify all the Callbacks associated with the IMS SIP Config
 *          indication.
 *
 * Parms:   pQmiIndication  [IN] - pointer to structure used to store all QMI
 *                                 Notification parameters.
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   Must be called after qminit() has been called
 *
 **************/
local void qaQmiIMSConfigNotify(
    struct QmiNotification *pQmiIndication )
{
    void  *pCallback;
    enum  eQMICallbackIndex CbkIndex;
    ULONG CbkDefValLong = 0xFFFFFFFF;
    WORD  CbkDefValWord = 0xFFFF;
    BYTE  CbkDefValByte = 0xFF;
    BYTE  CbkDefValStr  = EOS;

    CbkIndex  = eQMI_CB_IMS_SIP_CONFIG_IND;
    pCallback = qaQmiGetCallback( CbkIndex );
    if ( pCallback )
    {
        imsSIPConfigInfo SIPConfigNotification;

        /* Initialize the data structure */
        slmemset( (char*) &SIPConfigNotification,
                  0,
                  sizeof( imsSIPConfigInfo ) );

        struct SIPPortTlv *pSPResp =
                &pQmiIndication->QmiInd.qaQmiCbkImsSIPConfigInfo.SPTlv;

        /* The TLV was present in the Indication - hence process */
        if ( pSPResp->TlvPresent )
        {
            /* Extract the Parameters */
            SIPConfigNotification.pSIPLocalPort = &pSPResp->SIPLocalPort;
        }
        else
        {
            SIPConfigNotification.pSIPLocalPort = &CbkDefValWord;
        }

        struct SIPRegnTmrTlv *pSRResp =
                &pQmiIndication->QmiInd.qaQmiCbkImsSIPConfigInfo.SRTlv;

        /* The TLV was present in the Indication - hence process */
        if ( pSRResp->TlvPresent )
        {
            /* Extract the Parameters */
            SIPConfigNotification.pTimerSIPReg = &pSRResp->tmrSIPRegn;
        }
        else
        {
            SIPConfigNotification.pTimerSIPReg = &CbkDefValLong;
        }

        struct SubscrTmrTlv *pSTResp =
                &pQmiIndication->QmiInd.qaQmiCbkImsSIPConfigInfo.STTlv;

        /* The TLV was present in the Indication - hence process */
        if ( pSTResp->TlvPresent )
        {
            /* Extract the Parameters */
            SIPConfigNotification.pSubscribeTimer = &pSTResp->subscrTmr;
        }
        else
        {
            SIPConfigNotification.pSubscribeTimer = &CbkDefValLong;
        }

        struct TmrT1Tlv *pTT1Resp =
                &pQmiIndication->QmiInd.qaQmiCbkImsSIPConfigInfo.TT1Tlv;

        /* The TLV was present in the Indication - hence process */
        if ( pTT1Resp->TlvPresent )
        {
            /* Extract the Parameters */
            SIPConfigNotification.pTimerT1 = &pTT1Resp->tmrT1;
        }
        else
        {
            SIPConfigNotification.pTimerT1 = &CbkDefValLong;
        }

        struct TmrT2Tlv *pTT2Resp =
                &pQmiIndication->QmiInd.qaQmiCbkImsSIPConfigInfo.TT2Tlv;

        /* The TLV was present in the Indication - hence process */
        if ( pTT2Resp->TlvPresent )
        {
            /* Extract the Parameters */
            SIPConfigNotification.pTimerT2 = &pTT2Resp->tmrT2;
        }
        else
        {
            SIPConfigNotification.pTimerT2 = &CbkDefValLong;
        }

        struct TmrTfTlv *pTTfResp =
                &pQmiIndication->QmiInd.qaQmiCbkImsSIPConfigInfo.TTfTlv;

        /* The TLV was present in the Indication - hence process */
        if ( pTTfResp->TlvPresent )
        {
            /* Extract the Parameters */
            SIPConfigNotification.pTimerTf = &pTTfResp->tmrTf;
        }
        else
        {
            SIPConfigNotification.pTimerTf = &CbkDefValLong;
        }

        struct SigCompEnTlv *pSCResp =
                &pQmiIndication->QmiInd.qaQmiCbkImsSIPConfigInfo.SCTlv;

        /* The TLV was present in the Indication - hence process */
        if ( pSCResp->TlvPresent )
        {
            /* Extract the Parameters */
            SIPConfigNotification.pSigCompEnabled = &pSCResp->SigCompEn;
        }
        else
        {
            SIPConfigNotification.pSigCompEnabled = &CbkDefValByte;
        }

        /* Invoke the callback */
        ((tFNImsSIPConfig)pCallback) ( &SIPConfigNotification );
    }

    CbkIndex  = eQMI_CB_IMS_REG_MGR_CONFIG_IND;
    pCallback = qaQmiGetCallback( CbkIndex );
    if ( pCallback )
    {
        imsRegMgrConfigInfo RegMgrConfigNotification;

        /* Initialize the data structure */
        slmemset( (char*) &RegMgrConfigNotification,
                  0,
                  sizeof( imsRegMgrConfigInfo ) );

        struct PCSCFPortTlv *pPCResp =
                &pQmiIndication->QmiInd.qaQmiCbkImsRegMgrConfigInfo.PCTlv;

        /* The TLV was present in the Indication - hence process */
        if ( pPCResp->TlvPresent )
        {
            /* Extract the Parameters */
            RegMgrConfigNotification.pPriCSCFPort = &pPCResp->priCSCFPort;
        }
        else
        {
            RegMgrConfigNotification.pPriCSCFPort = &CbkDefValWord;
        }

        struct CSCFPortNameTlv *pPNResp =
                &pQmiIndication->QmiInd.qaQmiCbkImsRegMgrConfigInfo.PNTlv;

        /* The TLV was present in the Indication - hence process */
        if ( pPNResp->TlvPresent )
        {
            /* Extract the Parameters */
            RegMgrConfigNotification.pCSCFPortName = pPNResp->cscfPortName;
        }
        else
        {
            RegMgrConfigNotification.pCSCFPortName = (BYTE *) &CbkDefValStr;
        }

        struct IMSTestModeTlv *pTMResp =
                &pQmiIndication->QmiInd.qaQmiCbkImsRegMgrConfigInfo.TMTlv;

        /* The TLV was present in the Indication - hence process */
        if ( pTMResp->TlvPresent )
        {
            /* Extract the Parameters */
            RegMgrConfigNotification.pIMSTestMode = &pTMResp->imsTestMode;
        }
        else
        {
            RegMgrConfigNotification.pIMSTestMode = &CbkDefValByte;
        }

        /* Invoke the callback */
        ((tFNImsRegMgrConfig)pCallback) ( &RegMgrConfigNotification );
    }

    CbkIndex  = eQMI_CB_IMS_SMS_CONFIG_IND;
    pCallback = qaQmiGetCallback( CbkIndex );
    if ( pCallback )
    {
        imsSMSConfigInfo SMSConfigNotification;

        /* Initialize the data structure */
        slmemset( (char*) &SMSConfigNotification,
                  0,
                  sizeof( imsSMSConfigInfo ) );

        struct SMSFmtTlv *pSFResp =
                &pQmiIndication->QmiInd.qaQmiCbkImsSMSConfigInfo.SFTlv;

        /* The TLV was present in the Indication - hence process */
        if ( pSFResp->TlvPresent )
        {
            /* Extract the Parameters */
            SMSConfigNotification.pSMSFormat = &pSFResp->smsFormat;
        }
        else
        {
            SMSConfigNotification.pSMSFormat = &CbkDefValByte;
        }

        struct SMSoIPNwTlv *pSINResp =
                &pQmiIndication->QmiInd.qaQmiCbkImsSMSConfigInfo.SINTlv;

        /* The TLV was present in the Indication - hence process */
        if ( pSINResp->TlvPresent )
        {
            /* Extract the Parameters */
            SMSConfigNotification.pSMSOverIPNwInd = &pSINResp->smsoIPNW;
        }
        else
        {
            SMSConfigNotification.pSMSOverIPNwInd = &CbkDefValByte;
        }

        struct PhCtxtURITlv *pPCUResp =
                &pQmiIndication->QmiInd.qaQmiCbkImsSMSConfigInfo.PCURTlv;

        /* The TLV was present in the Indication - hence process */
        if ( pPCUResp->TlvPresent )
        {
            /* Extract the Parameters */
            SMSConfigNotification.pPhoneCtxtURI = pPCUResp->PhCtxtURI;
        }
        else
        {
            SMSConfigNotification.pPhoneCtxtURI = (BYTE *) &CbkDefValStr;
        }

        /* Invoke the callback */
        ((tFNImsSMSConfig)pCallback) ( &SMSConfigNotification );
    }

    CbkIndex  = eQMI_CB_IMS_USER_CONFIG_IND;
    pCallback = qaQmiGetCallback( CbkIndex );
    if ( pCallback )
    {
        imsUserConfigInfo USERConfigNotification;

        /* Initialize the data structure */
        slmemset( (char*) &USERConfigNotification,
                  0,
                  sizeof( imsUserConfigInfo ) );

        struct IMSDomainTlv *pIDResp =
                &pQmiIndication->QmiInd.qaQmiCbkImsUserConfigInfo.IDTlv;

        /* The TLV was present in the Indication - hence process */
        if ( pIDResp->TlvPresent )
        {
            /* Extract the Parameters */
            USERConfigNotification.pIMSDomain = pIDResp->imsDomainName;
        }
        else
        {
            USERConfigNotification.pIMSDomain = ( BYTE * )&CbkDefValStr;
        }

        /* Invoke the callback */
        ((tFNImsUserConfig)pCallback) ( &USERConfigNotification );
    }

    CbkIndex  = eQMI_CB_IMS_VOIP_CONFIG_IND;
    pCallback = qaQmiGetCallback( CbkIndex );
    if ( pCallback )
    {
        imsVoIPConfigInfo VOIPConfigNotification;

        /* Initialize the data structure */
        slmemset( (char*) &VOIPConfigNotification,
                  0,
                  sizeof( imsVoIPConfigInfo ) );

        struct SessDurTlv *pSDResp =
                &pQmiIndication->QmiInd.qaQmiCbkImsVoIPConfigInfo.SDTlv;

        /* The TLV was present in the Indication - hence process */
        if ( pSDResp->TlvPresent )
        {
            /* Extract the Parameters */
            VOIPConfigNotification.pSessionExpiryTimer = &pSDResp->sessExp;
        }
        else
        {
            VOIPConfigNotification.pSessionExpiryTimer = &CbkDefValWord;
        }

        struct MinSessExpTlv *pMSResp =
                &pQmiIndication->QmiInd.qaQmiCbkImsVoIPConfigInfo.MSETlv;

        /* The TLV was present in the Indication - hence process */
        if ( pMSResp->TlvPresent )
        {
            /* Extract the Parameters */
            VOIPConfigNotification.pMinSessionExpiryTimer =
                &pMSResp->minSessExp;
        }
        else
        {
            VOIPConfigNotification.pMinSessionExpiryTimer = &CbkDefValWord;
        }

        struct EnabAMRWBTlv *pEAResp =
                &pQmiIndication->QmiInd.qaQmiCbkImsVoIPConfigInfo.EAWTlv;

        /* The TLV was present in the Indication - hence process */
        if ( pEAResp->TlvPresent )
        {
            /* Extract the Parameters */
            VOIPConfigNotification.pAmrWbEnable = &pEAResp->amrWBEnable;
        }
        else
        {
            VOIPConfigNotification.pAmrWbEnable = &CbkDefValByte;
        }

        struct EnabSCRAMRTlv *pESResp =
                &pQmiIndication->QmiInd.qaQmiCbkImsVoIPConfigInfo.ESATlv;

        /* The TLV was present in the Indication - hence process */
        if ( pESResp->TlvPresent )
        {
            /* Extract the Parameters */
            VOIPConfigNotification.pScrAmrEnable = &pESResp->scrAmrEnable;
        }
        else
        {
            VOIPConfigNotification.pScrAmrEnable = &CbkDefValByte;
        }

        struct EnabSCRAMRWBTlv *pESWResp =
                &pQmiIndication->QmiInd.qaQmiCbkImsVoIPConfigInfo.ESAWTlv;

        /* The TLV was present in the Indication - hence process */
        if ( pESWResp->TlvPresent )
        {
            /* Extract the Parameters */
            VOIPConfigNotification.pScrAmrWbEnable = &pESWResp->scrAmrWBEnable;
        }
        else
        {
            VOIPConfigNotification.pScrAmrWbEnable = &CbkDefValByte;
        }

        struct AMRModeTlv *pAMResp =
                &pQmiIndication->QmiInd.qaQmiCbkImsVoIPConfigInfo.AMTlv;

        /* The TLV was present in the Indication - hence process */
        if ( pAMResp->TlvPresent )
        {
            /* Extract the Parameters */
            VOIPConfigNotification.pAmrMode = &pAMResp->amrMode;
        }
        else
        {
            VOIPConfigNotification.pAmrMode = &CbkDefValByte;
        }

        struct AMRWBModeTlv *pAWMResp =
                &pQmiIndication->QmiInd.qaQmiCbkImsVoIPConfigInfo.AWMTlv;

        /* The TLV was present in the Indication - hence process */
        if ( pAWMResp->TlvPresent )
        {
            /* Extract the Parameters */
            VOIPConfigNotification.pAmrWBMode = &pAWMResp->amrWBMode;
        }
        else
        {
            VOIPConfigNotification.pAmrWBMode = &CbkDefValWord;
        }

        struct AMROctAlgnTlv *pAOAResp =
                &pQmiIndication->QmiInd.qaQmiCbkImsVoIPConfigInfo.AOATlv;

        /* The TLV was present in the Indication - hence process */
        if ( pAOAResp->TlvPresent )
        {
            /* Extract the Parameters */
            VOIPConfigNotification.pAmrOctetAligned = &pAOAResp->amrOctAlgn;
        }
        else
        {
            VOIPConfigNotification.pAmrOctetAligned = &CbkDefValByte;
        }

        struct AMRWBOctAlgnTlv *pAWOAResp =
                &pQmiIndication->QmiInd.qaQmiCbkImsVoIPConfigInfo.AWOATlv;

        /* The TLV was present in the Indication - hence process */
        if ( pAWOAResp->TlvPresent )
        {
            /* Extract the Parameters */
            VOIPConfigNotification.pAmrWBOctetAligned =
                &pAWOAResp->amrWBOctAlgn;
        }
        else
        {
            VOIPConfigNotification.pAmrWBOctetAligned = &CbkDefValByte;
        }

        struct RngTmrTlv *pRTResp =
                &pQmiIndication->QmiInd.qaQmiCbkImsVoIPConfigInfo.RTTlv;

        /* The TLV was present in the Indication - hence process */
        if ( pRTResp->TlvPresent )
        {
            /* Extract the Parameters */
            VOIPConfigNotification.pRingingTimer = &pRTResp->RingTmr;
        }
        else
        {
            VOIPConfigNotification.pRingingTimer = &CbkDefValWord;
        }

        struct RngBkTmrTlv *pRBTResp =
                &pQmiIndication->QmiInd.qaQmiCbkImsVoIPConfigInfo.RBTTlv;

        /* The TLV was present in the Indication - hence process */
        if ( pRBTResp->TlvPresent )
        {
            /* Extract the Parameters */
            VOIPConfigNotification.pRingBackTimer = &pRBTResp->RingBkTmr;
        }
        else
        {
            VOIPConfigNotification.pRingBackTimer = &CbkDefValWord;
        }

        struct RTPRTCPInactTmrDurTlv *pRTIDResp =
                &pQmiIndication->QmiInd.qaQmiCbkImsVoIPConfigInfo.RTIDTlv;

        /* The TLV was present in the Indication - hence process */
        if ( pRTIDResp->TlvPresent )
        {
            /* Extract the Parameters */
            VOIPConfigNotification.pRTPRTCPInactTimer = &pRTIDResp->InactTmr;
        }
        else
        {
            VOIPConfigNotification.pRTPRTCPInactTimer = &CbkDefValWord;
        }
        /* Invoke the callback */
        ((tFNImsVoIPConfig)pCallback) ( &VOIPConfigNotification );
    }

}

/*************
 *
 * Name:   UpkQmiCbkIMSNotification
 *
 * Purpose: Unpacks the received Ims indication and invokes the approriate
 *          callback based on the QMI message type.
 *
 * Parms:   QmiMsgID        - QMI Message ID
 *          pMdmResp   [IN] - Pointer to packed response from the modem.
 *          pNotifResp [IN] - Notification Structure to fill in the parameters.
 *
 * Return:  eQCWWAN_ERR_NONE on success, eQCWWAN_ERR_XXX otherwise
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
package enum eQCWWANError UpkQmiCbkIMSNotification(
    USHORT                 QmiMsgID,
    BYTE                   *pMdmResp,
    struct QmiNotification *pNotifResp )
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    switch(QmiMsgID)
    {
        case eQMI_IMS_SIP_CONFIG_IND:
        {
            struct QmiCbkImsSIPConfigInd *pResp =
                &pNotifResp->QmiInd.qaQmiCbkImsSIPConfigInfo;

            /* Set all the TLVs to be in Not-Present State */
            pResp->SPTlv.TlvPresent  = FALSE;
            pResp->SRTlv.TlvPresent  = FALSE;
            pResp->STTlv.TlvPresent  = FALSE;
            pResp->TT1Tlv.TlvPresent = FALSE;
            pResp->TT2Tlv.TlvPresent = FALSE;
            pResp->TTfTlv.TlvPresent = FALSE;
            pResp->SCTlv.TlvPresent = FALSE;

            /* Unpack the SIP Config Info Indication */
            eRCode = UpkQmiCbkImsSIPCfgInd ( pMdmResp,
                                             pResp );

            /* Notify to the Callbacks associated */
            qaQmiIMSConfigNotify( pNotifResp );
            break;
        }
        case eQMI_IMS_REG_MGR_CONFIG_IND:
        {
            struct QmiCbkImsRegMgrConfigInd *pResp =
                &pNotifResp->QmiInd.qaQmiCbkImsRegMgrConfigInfo;

            /* Set all the TLVs to be in Not-Present State */
            pResp->PCTlv.TlvPresent  = FALSE;
            pResp->PNTlv.TlvPresent  = FALSE;
            pResp->TMTlv.TlvPresent  = FALSE;

            /* Unpack the Reg Mgr Config Info Indication */
            eRCode = UpkQmiCbkImsRegMgrCfgInd ( pMdmResp,
                                                pResp );

            /* Notify to the Callbacks associated */
            qaQmiIMSConfigNotify( pNotifResp );
            break;
        }
        case eQMI_IMS_SMS_CONFIG_IND:
        {
            struct QmiCbkImsSMSConfigInd *pResp =
                &pNotifResp->QmiInd.qaQmiCbkImsSMSConfigInfo;

            /* Set all the TLVs to be in Not-Present State */
            pResp->SFTlv.TlvPresent   = FALSE;
            pResp->SINTlv.TlvPresent  = FALSE;
            pResp->PCURTlv.TlvPresent = FALSE;

            /* Unpack the SMS Config Info Indication */
            eRCode = UpkQmiCbkImsSMSCfgInd ( pMdmResp,
                                             pResp );

            /* Notify to the Callbacks associated */
            qaQmiIMSConfigNotify( pNotifResp );
            break;
        }
        case eQMI_IMS_USER_CONFIG_IND:
        {
            struct QmiCbkImsUserConfigInd *pResp =
                &pNotifResp->QmiInd.qaQmiCbkImsUserConfigInfo;

            /* Set all the TLVs to be in Not-Present State */
            pResp->IDTlv.TlvPresent   = FALSE;

            /* Unpack the User Config Info Indication */
            eRCode = UpkQmiCbkImsUserCfgInd ( pMdmResp,
                                              pResp );

            /* Notify to the Callbacks associated */
            qaQmiIMSConfigNotify( pNotifResp );
            break;
        }

        case eQMI_IMS_VOIP_CONFIG_IND:
        {
            struct QmiCbkImsVoIPConfigInd *pResp =
                &pNotifResp->QmiInd.qaQmiCbkImsVoIPConfigInfo;

            /* Set all the TLVs to be in Not-Present State */
            pResp->SDTlv.TlvPresent   = FALSE;
            pResp->MSETlv.TlvPresent  = FALSE;
            pResp->EAWTlv.TlvPresent  = FALSE;
            pResp->ESATlv.TlvPresent  = FALSE;
            pResp->ESAWTlv.TlvPresent = FALSE;
            pResp->AMTlv.TlvPresent   = FALSE;
            pResp->AWMTlv.TlvPresent  = FALSE;
            pResp->AOATlv.TlvPresent  = FALSE;
            pResp->AWOATlv.TlvPresent = FALSE;
            pResp->RTTlv.TlvPresent   = FALSE;
            pResp->RBTTlv.TlvPresent  = FALSE;
            pResp->RTIDTlv.TlvPresent = FALSE;

            /* Unpack the VoIP Config Info Indication */
            eRCode = UpkQmiCbkImsVoIPCfgInd ( pMdmResp,
                                              pResp );

            /* Notify to the Callbacks associated */
            qaQmiIMSConfigNotify( pNotifResp );
            break;
        }
        default:
        {
            eRCode = eQCWWAN_ERR_INVALID_ARG;
            break;
        }
    }

    return eRCode;
}
