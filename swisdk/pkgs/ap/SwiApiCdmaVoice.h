/**
 *    \if CvsId 
 *    $Id: SwiApiCdmaVoice.h,v 1.4 2010/06/10 00:13:14 epasheva Exp $
 *    \endif
 *    \ingroup cdmavoice
 *    \file SwiApiCdmaVoice.h
 * 
 *    \brief SWI CDMA specific Voice related APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */
#ifndef __SWIAPICDMAVOICE_H__
#define __SWIAPICDMAVOICE_H__

#ifdef __cplusplus
extern "C" {
#endif

/* include files */
#include "SwiDataTypes.h"
#include "SwiRcodes.h"

/**
 *
 * An enumeration of preference for data dedicated transmission mode.
 *
 */
typedef enum SWI_TYPE_DDTM_Pref
{
    /*! Prefer data dedicated mode off */
    SWI_HDR_DDTM_PREF_MODE_OFF          = 0x00000000,

    /*! Prefer data dedicated mode on */
    SWI_HDR_DDTM_PREF_MODE_ON           = 0x00000001,

    /*! No change in preference */
    SWI_HDR_DDTM_PREF_MODE_NO_CHANGE    = 0x00000002
}SWI_TYPE_DDTM_Pref;

/**
 *
 * Contains dedicated transmission mode data.
 *
 */
typedef struct SWI_STRUCT_DDTM_Pref
{
    swi_uint32 sizeStruct;      /*!< size of this structure */
    
    /*! preference for data dedicated transmission mode */
    swi_uint16 iDDTMPref;
}SWI_STRUCT_DDTM_Pref;

/* Function prototypes */

SWI_API SWI_RCODE SwiGetAnswerState(swi_uint16* AnswerMode,
                                    swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetAnswerState(swi_uint16 AnswerMode);

SWI_API SWI_RCODE SwiGetCallerID(swi_charp szCallerID, swi_uint32 iLength,
                                 swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetCallAnswerCmd();

SWI_API SWI_RCODE SwiSetCallFlashCmd(swi_uint16 SOValue,
                                     const swi_charp pszCallNumber,
                                     swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetHeadsetState(swi_uint16 *headsetState,
                                     swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetCdmaSpeakerVolume(swi_uint16* pSpeakerVolume,
                                          swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetCdmaSpeakerVolume(swi_uint16 SpeakerVolume,
                                          swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetSpeakerMuteStatus(swi_uint16* pSpeakerMuteStatus,
                                          swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetSpeakerMuteStatus(swi_uint16 SpeakerMuteStatus,
                                          swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetToneLevel(swi_uint16* pToneLevel, swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetToneLevel(swi_uint16 ToneLevel, swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetDTMFMuteStatus(swi_uint16* pDTMFMuteStatus,
                                       swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetDTMFMuteStatus(swi_uint16 DTMFMuteStatus,
                                       swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetMICMuteStatus(swi_uint16* pMICMuteStatus,
                                      swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetMICMuteStatus(swi_uint16 MICMuteStatus,
                                      swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetDTMFKey(swi_uint16 DTMFKey,
                                swi_uint32 timeout);
SWI_API SWI_RCODE SwiGetDTMFDuration(   swi_uint16* pDTMFKeyOn,
                                     swi_uint16* pDTMFBurstDigitOn,
                                     swi_uint16* pDTMFBurstInterdigitOff,
                                     swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetDTMFDuration(   swi_uint16 DTMFKeyOn,
                                     swi_uint16 DTMFBurstDigitOn,
                                     swi_uint16 DTMFBurstInterdigitOff,
                                     swi_uint32 timeout);

SWI_API SWI_RCODE SwiSetKeyPressed(swi_uint16 phoneKey);
SWI_API SWI_RCODE SwiSetKeyReleased();
SWI_API SWI_RCODE SwiPstGetMinuteCallBeep(swi_uint16* callBeep,
                                          swi_uint32 timeout);

SWI_API SWI_RCODE SwiPstSetMinuteCallBeep(swi_uint16 callBeep,
                                          swi_uint32 timeout);
SWI_API SWI_RCODE SwiSetDDTMPref(swi_uint16 iDDTMPref, swi_uint32 timeout);
SWI_API SWI_RCODE SwiClearVoicemail(swi_uint32 timeout);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* __SWIAPICDMAVOICE_H__ */

/* 
 * $Log: SwiApiCdmaVoice.h,v $
 */
 
