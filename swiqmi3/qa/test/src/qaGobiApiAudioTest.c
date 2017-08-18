/**************
 *
 *  Filename: qaGobiApiAudioTest.c
 *
 *  Purpose:  Contains functions implementing specific tests for
 *            Audio Service called by the main() routine in qatest.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc., all rights reserved
 *
 **************/

/* Linux definitions */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* include files */
#include "SwiDataTypes.h"
#include "qmerrno.h"

#include "qaGobiApiAudio.h"
#include "qaGobiApiAudioTest.h"
#include "qatesthelper.h"
#include "qatestproto.h"

/*******************
    TEST DATA
 *******************/

/* Constants */
#define STAGESIZECOUNT 20

/* Global Variables */

/* Forward declaration */


/*******************
  TEST FUNCTIONS
 *******************/

/*
 * Name:     doSLQSGetAudioProfile
 *
 * Purpose:  Perform the tests that call the SLQSGetAudioProfile() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSLQSGetAudioProfile( void )
{
    BYTE audioGeneratorVoice   = 0x00;
    BYTE audioGeneratorKeyBeep = 0x01;
    BYTE audioGeneratorMIDI    = 0x02;

    GetAudioProfileResp tempResp;

    SLQSGetAudioProfileTestCase_t SLQSGetAudioProfileTestCases[] =
    {
        { eQCWWAN_ERR_INVALID_ARG, 1, &audioGeneratorVoice, NULL,
          "Invalid - Output parameters NULL"},

        { eQCWWAN_ERR_NONE, 2, &audioGeneratorVoice, &tempResp,
          "Valid - Voice Audio Profile"},

        { eQCWWAN_ERR_NONE, 3, &audioGeneratorKeyBeep, &tempResp,
          "Valid - Key Beep Audio Profile"},

        { eQCWWAN_ERR_NONE, 4, &audioGeneratorMIDI, &tempResp,
          "Valid - MIDI Audio Profile"},
    };

    ULONG nRet;
    GetAudioProfileReq  req;
    GetAudioProfileResp *pResp;
    FILE  *fp = NULL;

    fp = tfopen( "../../audio/test/results/slqsgetaudioprofile.txt", "w" );

    if (fp)
    {
        BYTE tc = 0;
        BYTE ncases = sizeof( SLQSGetAudioProfileTestCases )/
                      sizeof( SLQSGetAudioProfileTestCases[0] );

        while ( tc < ncases )
        {
            SLQSGetAudioProfileTestCase_t *pd =
                                     &SLQSGetAudioProfileTestCases[tc];
            req.Generator = *(pd->pGenerator);
            pResp = pd->pGetAudioProfileResp;

            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        (tc + 1),
                        ncases,
                        pd->desc);

            fprintf( fp, "Parameter Set:\n" );
            fprintf( fp, "Generator :0x%x\n", req.Generator );

            nRet = SLQSGetAudioProfile( &req, pResp );

            /* Display result code and text */
            doprintreason ( fp, nRet );

            if ( !nRet )
            {
                fprintf( fp, "Audio Profile successfully retrieved\n" );
                if( pd->pGetAudioProfileResp )
                {
                    fprintf( fp, "Profile          : %x\n", pResp->Profile );
                    fprintf( fp, "Ear Mute Setting : %x\n", pResp->EarMute );
                    fprintf( fp, "Mic Mute Setting : %x\n", pResp->MicMute );
                    fprintf( fp, "Volume Level     : %x\n", pResp->Volume );
                }
            }
            tc++;
        }
    }
    if ( fp )
        tfclose( fp );
}

/*
 * Name:     doSLQSSetAudioProfile
 *
 * Purpose:  Perform the tests that call the SLQSSetAudioProfile() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSLQSSetAudioProfile( void )
{
    BYTE audioProfileHS  = 0x00;
    BYTE audioProfileI2S = 0x09;
    BYTE audioProfileInv = 0x0B;

    BYTE earMute         = 0x01;
    BYTE micMute         = 0x01;
    BYTE earUnmute       = 0x00;
    BYTE micUnmute       = 0x00;
    BYTE earMuteInv      = 0x05;
    BYTE micMuteInv      = 0x05;

    BYTE audioGenVoice   = 0x00;
    BYTE audioGenKeyBeep = 0x01;
    BYTE audioVolume     = 0x04;

    SLQSSetAudioProfileTestCase_t SLQSSetAudioProfileTestCases[] =
    {
        { eQCWWAN_ERR_NONE, 1, &audioProfileHS, &earUnmute,
          &micUnmute, &audioGenVoice, &audioVolume,
          "All Valid - UnMuting Handset Voice"},

        { eQCWWAN_ERR_NONE, 2, &audioProfileHS, &earMute,
          &micMute, &audioGenVoice, &audioVolume,
          "All Valid - Muting Handset Voice"},

        { eQCWWAN_ERR_NONE, 3, &audioProfileI2S, &earUnmute,
          &micUnmute, &audioGenVoice, &audioVolume,
          "All Valid - UnMuting I2S Voice"},

        { eQCWWAN_ERR_NONE, 4, &audioProfileI2S, &earMute,
          &micMute, &audioGenVoice, &audioVolume,
          "All Valid - Muting I2S Voice"},

        { eQCWWAN_ERR_NONE, 5, &audioProfileI2S, &earUnmute,
          &micUnmute, &audioGenKeyBeep, &audioVolume,
          "All Valid - UnMuting I2S Key Beep"},

        { eQCWWAN_ERR_NONE, 6, &audioProfileI2S, &earMute,
          &micMute, &audioGenKeyBeep, &audioVolume,
          "All Valid - Muting I2S Key Beep"},

        { eQCWWAN_ERR_INVALID_ARG, 7, &audioProfileInv, &earUnmute,
          &micUnmute, &audioGenVoice, &audioVolume,
          "Invalid - Audio Profile Invalid"},

        { eQCWWAN_ERR_INVALID_ARG, 8, &audioProfileI2S, &earMuteInv,
          &micMuteInv, &audioGenVoice, &audioVolume,
          "Invalid - Ear mute and MIC mute"},

        { eQCWWAN_ERR_INVALID_ARG, 9, &audioProfileInv, &earMuteInv,
          &micMuteInv, &audioGenVoice, &audioVolume,
          "Invalid - Profile, Ear mute and MIC mute"},
    };

    ULONG nRet;
    SetAudioProfileReq  req;
    FILE  *fp = NULL;

    fp = tfopen( "../../audio/test/results/slqssetaudioprofile.txt", "w" );

    if (fp)
    {
        BYTE tc = 0;
        BYTE ncases = sizeof( SLQSSetAudioProfileTestCases )/
                      sizeof( SLQSSetAudioProfileTestCases[0] );

        while ( tc < ncases )
        {
            SLQSSetAudioProfileTestCase_t *pd =
                                     &SLQSSetAudioProfileTestCases[tc];
            req.Profile   = *(pd->pProfile);
            req.EarMute   = *(pd->pEarMute);
            req.MicMute   = *(pd->pMicMute);
            req.Generator = *(pd->pGenerator);
            req.Volume    = *(pd->pVolume);

            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        (tc + 1),
                        ncases,
                        pd->desc);

            fprintf( fp, "Parameter Set:\n" );
            fprintf( fp, "Profile      :0x%x\n", req.Profile );
            fprintf( fp, "Ear Mute     :0x%x\n", req.EarMute );
            fprintf( fp, "Mic Mute     :0x%x\n", req.MicMute );
            fprintf( fp, "Generator    :0x%x\n", req.Generator );
            fprintf( fp, "Volume Level :0x%x\n", req.Volume );

            nRet = SLQSSetAudioProfile( &req );

            /* Display result code and text */
            doprintreason ( fp, nRet );

            if ( !nRet )
            {
                fprintf( fp, "Audio Profile successfully Set\n" );
            }
            tc++;
        }
    }
    if ( fp )
        tfclose( fp );
}

/*
 * Name:     doSLQSGetAudioPathConfig
 *
 * Purpose:  Perform the tests that call the SLQSGetAudioPathConfig() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSLQSGetAudioPathConfig( void )
{
    BYTE audioProfileOne    = 0x04;
    BYTE audioProfileTwo    = 0x09;

    BYTE audioItemEC        = 0x00;
    BYTE audioItemNS        = 0x01;
    BYTE audioItemTXVol     = 0x02;
    BYTE audioItemDTMFTXG   = 0x03;
    BYTE audioItemCodecSTG  = 0x04;
    BYTE audioItemTXPCMFLTR = 0x05;
    BYTE audioItemRXPCMFLTR = 0x06;
    BYTE audioItemMICGain   = 0x07;
    BYTE audioItemRXAGC     = 0x08;
    BYTE audioItemTXAGC     = 0x09;
    BYTE audioItemRXAGCList = 0x0A;
    BYTE audioItemRXAVCList = 0x0B;
    BYTE audioItemTXAGCList = 0x0C;

    GetAudioPathConfigResp tempResp;

    SLQSGetAudioPathConfigTestCase_t SLQSGetAudioPathConfigTestCases[] =
    {
        { eQCWWAN_ERR_INVALID_ARG, 1, &audioProfileOne, &audioItemEC, NULL,
          "Invalid - Output parameters NULL"},

        { eQCWWAN_ERR_NONE, 2, &audioProfileOne, &audioItemEC, &tempResp,
          "Valid - Audio Profile: 4, Item: EC"},
        { eQCWWAN_ERR_NONE, 3, &audioProfileOne, &audioItemNS, &tempResp,
          "Valid - Audio Profile: 4, Item: NS"},
        { eQCWWAN_ERR_NONE, 4, &audioProfileOne, &audioItemTXVol, &tempResp,
          "Valid - Audio Profile: 4, Item: TX Volume"},
        { eQCWWAN_ERR_NONE, 5, &audioProfileOne, &audioItemDTMFTXG, &tempResp,
          "Valid - Audio Profile: 4, Item: DTMF TXG"},
        { eQCWWAN_ERR_NONE, 6, &audioProfileOne, &audioItemCodecSTG, &tempResp,
          "Valid - Audio Profile: 4, Item: CODECSTG"},
        { eQCWWAN_ERR_NONE, 7, &audioProfileOne, &audioItemTXPCMFLTR, &tempResp,
          "Valid - Audio Profile: 4, Item: TXPCMFLTR"},
        { eQCWWAN_ERR_NONE, 8, &audioProfileOne, &audioItemRXPCMFLTR, &tempResp,
          "Valid - Audio Profile: 4, Item: RXPCMFLTR"},
        { eQCWWAN_ERR_NONE, 9, &audioProfileOne, &audioItemMICGain, &tempResp,
          "Valid - Audio Profile: 4, Item: MICGain"},
        { eQCWWAN_ERR_NONE, 10, &audioProfileOne, &audioItemRXAGC, &tempResp,
          "Valid - Audio Profile: 4, Item: RXAGC"},
        { eQCWWAN_ERR_NONE, 11, &audioProfileOne, &audioItemTXAGC, &tempResp,
          "Valid - Audio Profile: 4, Item: TXAGC"},
        { eQCWWAN_ERR_NONE, 12, &audioProfileOne, &audioItemRXAGCList, &tempResp,
          "Valid - Audio Profile: 4, Item: RXAGCList"},
        { eQCWWAN_ERR_NONE, 13, &audioProfileOne, &audioItemRXAVCList, &tempResp,
          "Valid - Audio Profile: 4, Item: RXAVCList"},
        { eQCWWAN_ERR_NONE, 14, &audioProfileOne, &audioItemTXAGCList, &tempResp,
          "Valid - Audio Profile: 4, Item: TXAGCList"},

        { eQCWWAN_ERR_NONE, 15, &audioProfileTwo, &audioItemEC, &tempResp,
          "Valid - Audio Profile: 9, Item: EC"},
        { eQCWWAN_ERR_NONE, 16, &audioProfileTwo, &audioItemNS, &tempResp,
          "Valid - Audio Profile: 9, Item: NS"},
        { eQCWWAN_ERR_NONE, 17, &audioProfileTwo, &audioItemTXVol, &tempResp,
          "Valid - Audio Profile: 9, Item: TX Volume"},
        { eQCWWAN_ERR_NONE, 18, &audioProfileTwo, &audioItemDTMFTXG, &tempResp,
          "Valid - Audio Profile: 9, Item: DTMF TXG"},
        { eQCWWAN_ERR_NONE, 19, &audioProfileTwo, &audioItemCodecSTG, &tempResp,
          "Valid - Audio Profile: 9, Item: CODECSTG"},
        { eQCWWAN_ERR_NONE, 20, &audioProfileTwo, &audioItemTXPCMFLTR, &tempResp,
          "Valid - Audio Profile: 9, Item: TXPCMFLTR"},
        { eQCWWAN_ERR_NONE, 21, &audioProfileTwo, &audioItemRXPCMFLTR, &tempResp,
          "Valid - Audio Profile: 9, Item: RXPCMFLTR"},
        { eQCWWAN_ERR_NONE, 22, &audioProfileTwo, &audioItemMICGain, &tempResp,
          "Valid - Audio Profile: 9, Item: MICGain"},
        { eQCWWAN_ERR_NONE, 23, &audioProfileTwo, &audioItemRXAGC, &tempResp,
          "Valid - Audio Profile: 9, Item: RXAGC"},
        { eQCWWAN_ERR_NONE, 24, &audioProfileTwo, &audioItemTXAGC, &tempResp,
          "Valid - Audio Profile: 9, Item: TXAGC"},
        { eQCWWAN_ERR_NONE, 25, &audioProfileTwo, &audioItemRXAGCList, &tempResp,
          "Valid - Audio Profile: 9, Item: RXAGCList"},
        { eQCWWAN_ERR_NONE, 26, &audioProfileTwo, &audioItemRXAVCList, &tempResp,
          "Valid - Audio Profile: 9, Item: RXAVCList"},
        { eQCWWAN_ERR_NONE, 27, &audioProfileTwo, &audioItemTXAGCList, &tempResp,
          "Valid - Audio Profile: 9, Item: TXAGCList"},
    };

    ULONG nRet;
    GetAudioPathConfigReq  req;
    GetAudioPathConfigResp *pResp;

    /* Parameters for Response */
    BYTE ECMode, NSEnable, MICGainSelect, RXSwitch, TXSwitch;
    WORD TXGain, DTMFTXGain, CodecSTGain, TXPCMFlag, TXStageCnt, RXPCMFlag, RXStageCnt;
    WORD RXStatGain, RXAIG, RXExpThr, RXExpSlope, RXComprThr, RXComprSlope;
    WORD RXSens, RXHeadroom, TXStatGain, TXAIG, TXExpThr, TXExpSlope, TXComprThr, TXComprSlope;
    BYTE TXStage0[20], TXStage1[20], TXStage2[20], TXStage3[20], TXStage4[20];
    BYTE RXStage0[20], RXStage1[20], RXStage2[20], RXStage3[20], RXStage4[20];
    TXPCMIIRFltr tempTXPCMFLTR;
    RXPCMIIRFltr tempRXPCMFLTR;
    RXAGCList    tempRXAGCList;
    RXAVCList    tempRXAVCList;
    TXAGCList    tempTXAGCList;

    FILE  *fp = NULL;
    BYTE  lCtr = 0;

    fp = tfopen( "../../audio/test/results/slqsgetaudiopathConfig.txt", "w" );

    if (fp)
    {
        BYTE tc = 0;
        BYTE ncases = sizeof( SLQSGetAudioPathConfigTestCases )/
                      sizeof( SLQSGetAudioPathConfigTestCases[0] );

        while ( tc < ncases )
        {
            SLQSGetAudioPathConfigTestCase_t *pd =
                                     &SLQSGetAudioPathConfigTestCases[tc];

            /* Initialize request parameters */
            req.Profile = *(pd->pAudioProfile);
            req.Item    = *(pd->pItem);

            pResp = pd->pGetAudioPathConfigResp;

            /* Initialize response paramaters */
            if( NULL != pResp )
            {
                pResp->pECMode      = &ECMode;
                pResp->pNSEnable    = &NSEnable;
                pResp->pTXGain      = &TXGain;
                pResp->pDTMFTXGain  = &DTMFTXGain;
                pResp->pCodecSTGain = &CodecSTGain;

                tempTXPCMFLTR.pFlag      = &TXPCMFlag;
                tempTXPCMFLTR.pStageCnt  = &TXStageCnt;
                tempTXPCMFLTR.pStage0Val = TXStage0;
                tempTXPCMFLTR.pStage1Val = TXStage1;
                tempTXPCMFLTR.pStage2Val = TXStage2;
                tempTXPCMFLTR.pStage3Val = TXStage3;
                tempTXPCMFLTR.pStage4Val = TXStage4;
                pResp->pTXPCMIIRFltr     = &tempTXPCMFLTR;

                tempRXPCMFLTR.pFlag      = &RXPCMFlag;
                tempRXPCMFLTR.pStageCnt  = &RXStageCnt;
                tempRXPCMFLTR.pStage0Val = RXStage0;
                tempRXPCMFLTR.pStage1Val = RXStage1;
                tempRXPCMFLTR.pStage2Val = RXStage2;
                tempRXPCMFLTR.pStage3Val = RXStage3;
                tempRXPCMFLTR.pStage4Val = RXStage4;
                pResp->pRXPCMIIRFltr     = &tempRXPCMFLTR;

                pResp->pMICGainSelect  = &MICGainSelect;
                pResp->pRXAVCAGCSwitch = &RXSwitch;
                pResp->pTXAVCSwitch    = &TXSwitch;

                tempRXAGCList.pRXStaticGain = &RXStatGain;
                tempRXAGCList.pRXAIG        = &RXAIG;
                tempRXAGCList.pRXExpThres   = &RXExpThr;
                tempRXAGCList.pRXExpSlope   = &RXExpSlope;
                tempRXAGCList.pRXComprThres = &RXComprThr;
                tempRXAGCList.pRXComprSlope = &RXComprSlope;
                pResp->pRXAGCList           = &tempRXAGCList;

                tempRXAVCList.pAVRXAVCSens     = &RXSens;
                tempRXAVCList.pAVRXAVCHeadroom = &RXHeadroom;
                pResp->pRXAVCList              = &tempRXAVCList;

                tempTXAGCList.pTXStaticGain = &TXStatGain;
                tempTXAGCList.pTXAIG        = &TXAIG;
                tempTXAGCList.pTXExpThres   = &TXExpThr;
                tempTXAGCList.pTXExpSlope   = &TXExpSlope;
                tempTXAGCList.pTXComprThres = &TXComprThr;
                tempTXAGCList.pTXComprSlope = &TXComprSlope;
                pResp->pTXAGCList           = &tempTXAGCList;
            }
            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        (tc + 1),
                        ncases,
                        pd->desc);

            fprintf( fp, "Parameter Set:\n" );
            fprintf( fp, "Profile :0x%x\n", req.Profile );
            fprintf( fp, "Item :0x%x\n",    req.Item );

            nRet = SLQSGetAudioPathConfig( &req, pResp );

            /* Display result code and text */
            doprintreason ( fp, nRet );

            if ( !nRet )
            {
                fprintf( fp, "Audio Path successfully retrieved\n" );
                if( NULL != pResp )
                {
                    TXPCMIIRFltr *pTXPCMIIRFltr = pResp->pTXPCMIIRFltr;
                    RXPCMIIRFltr *pRXPCMIIRFltr = pResp->pRXPCMIIRFltr;
                    RXAGCList    *pRXAGCList    = pResp->pRXAGCList;
                    RXAVCList    *pRXAVCList    = pResp->pRXAVCList;
                    TXAGCList    *pTXAGCList    = pResp->pTXAGCList;

                    fprintf( fp, "EC Mode          : 0x%x\n", *(pResp->pECMode) );
                    fprintf( fp, "NS Enable        : 0x%x\n", *(pResp->pNSEnable) );
                    fprintf( fp, "TX Gain          : 0x%x\n", *(pResp->pTXGain) );
                    fprintf( fp, "DTMF TX Gain     : 0x%x\n", *(pResp->pDTMFTXGain) );
                    fprintf( fp, "Codec ST Gain    : 0x%x\n", *(pResp->pCodecSTGain) );

                    fprintf( fp, "TX PCM FLTR parameters: \n" );
                    fprintf( fp, "Flag             : 0x%x\n", *(pTXPCMIIRFltr->pFlag) );
                    fprintf( fp, "Stage Count      : 0x%x\n", *(pTXPCMIIRFltr->pStageCnt) );
                    fprintf( fp, "Stage 0 Value: " );
                    for( lCtr = 0; lCtr < STAGESIZECOUNT; lCtr++ )
                    {
                        fprintf( fp, "%X ", pTXPCMIIRFltr->pStage0Val[lCtr] );
                    }
                    fprintf( fp, "\n" );

                    fprintf( fp, "Stage 1 Value: " );
                    for( lCtr = 0; lCtr < STAGESIZECOUNT; lCtr++ )
                    {
                        fprintf( fp, "%X ", pTXPCMIIRFltr->pStage1Val[lCtr] );
                    }
                    fprintf( fp, "\n" );

                    fprintf( fp, "Stage 2 Value: " );
                    for( lCtr = 0; lCtr < STAGESIZECOUNT; lCtr++ )
                    {
                        fprintf( fp, "%X ", pTXPCMIIRFltr->pStage2Val[lCtr] );
                    }
                    fprintf( fp, "\n" );

                    fprintf( fp, "Stage 3 Value: " );
                    for( lCtr = 0; lCtr < STAGESIZECOUNT; lCtr++ )
                    {
                        fprintf( fp, "%X ", pTXPCMIIRFltr->pStage3Val[lCtr] );
                    }
                    fprintf( fp, "\n" );

                    fprintf( fp, "Stage 4 Value: " );
                    for( lCtr = 0; lCtr < STAGESIZECOUNT; lCtr++ )
                    {
                        fprintf( fp, "%X ", pTXPCMIIRFltr->pStage4Val[lCtr] );
                    }
                    fprintf( fp, "\n" );

                    fprintf( fp, "RX PCM FLTR parameters: \n" );
                    fprintf( fp, "Flag             : 0x%x\n", *(pRXPCMIIRFltr->pFlag) );
                    fprintf( fp, "Stage Count      : 0x%x\n", *(pRXPCMIIRFltr->pStageCnt) );
                    fprintf( fp, "Stage 0 Value: " );
                    for( lCtr = 0; lCtr < STAGESIZECOUNT; lCtr++ )
                    {
                        fprintf( fp, "%X ", pRXPCMIIRFltr->pStage0Val[lCtr] );
                    }
                    fprintf( fp, "\n" );

                    fprintf( fp, "Stage 1 Value: " );
                    for( lCtr = 0; lCtr < STAGESIZECOUNT; lCtr++ )
                    {
                        fprintf( fp, "%X ", pRXPCMIIRFltr->pStage1Val[lCtr] );
                    }
                    fprintf( fp, "\n" );

                    fprintf( fp, "Stage 2 Value: " );
                    for( lCtr = 0; lCtr < STAGESIZECOUNT; lCtr++ )
                    {
                        fprintf( fp, "%X ", pRXPCMIIRFltr->pStage2Val[lCtr] );
                    }
                    fprintf( fp, "\n" );

                    fprintf( fp, "Stage 3 Value: " );
                    for( lCtr = 0; lCtr < STAGESIZECOUNT; lCtr++ )
                    {
                        fprintf( fp, "%X ", pRXPCMIIRFltr->pStage3Val[lCtr] );
                    }
                    fprintf( fp, "\n" );

                    fprintf( fp, "Stage 4 Value: " );
                    for( lCtr = 0; lCtr < STAGESIZECOUNT; lCtr++ )
                    {
                        fprintf( fp, "%X ", pRXPCMIIRFltr->pStage4Val[lCtr] );
                    }
                    fprintf( fp, "\n" );

                    fprintf( fp, "MIC Gain              : 0x%x\n", *(pResp->pMICGainSelect) );
                    fprintf( fp, "RX AVC/AGC Switch     : 0x%x\n", *(pResp->pRXAVCAGCSwitch) );
                    fprintf( fp, "TX AVC Switch         : 0x%x\n", *(pResp->pTXAVCSwitch) );

                    fprintf( fp, "RX AGC List: \n" );
                    fprintf( fp, "Static Gain           : 0x%x\n", *(pRXAGCList->pRXStaticGain) );
                    fprintf( fp, "Gain Selection Flag   : 0x%x\n", *(pRXAGCList->pRXAIG) );
                    fprintf( fp, "Expansion Threshold   : 0x%x\n", *(pRXAGCList->pRXExpThres) );
                    fprintf( fp, "Expansion Slope       : 0x%x\n", *(pRXAGCList->pRXExpSlope) );
                    fprintf( fp, "Compression Threshold : 0x%x\n", *(pRXAGCList->pRXComprThres) );
                    fprintf( fp, "Compression Slope     : 0x%x\n", *(pRXAGCList->pRXComprSlope) );

                    fprintf( fp, "RX AVC List: \n" );
                    fprintf( fp, "AVC Sensitivity       : 0x%x\n", *(pRXAVCList->pAVRXAVCSens) );
                    fprintf( fp, "AVC Headroom          : 0x%x\n", *(pRXAVCList->pAVRXAVCHeadroom) );

                    fprintf( fp, "TX AGC List: \n" );
                    fprintf( fp, "Static Gain           : 0x%x\n", *(pTXAGCList->pTXStaticGain) );
                    fprintf( fp, "Gain Selection Flag   : 0x%x\n", *(pTXAGCList->pTXAIG) );
                    fprintf( fp, "Expansion Threshold   : 0x%x\n", *(pTXAGCList->pTXExpThres) );
                    fprintf( fp, "Expansion Slope       : 0x%x\n", *(pTXAGCList->pTXExpSlope) );
                    fprintf( fp, "Compression Threshold : 0x%x\n", *(pTXAGCList->pTXComprThres) );
                    fprintf( fp, "Compression Slope     : 0x%x\n", *(pTXAGCList->pTXComprSlope) );
                }
            }
            tc++;
        }
    }
    if ( fp )
        tfclose( fp );
}

/*
 * Name:     doSLQSSetAudioPathConfig
 *
 * Purpose:  Perform the tests that call the SLQSSetAudioPathConfig() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSLQSSetAudioPathConfig( void )
{
    BYTE audioProfileHS  = 0x00;
    BYTE audioProfileI2S = 0x09;
    BYTE audioProfileInv = 0x0B;

    BYTE ECMode          = 0x04;
    BYTE NSOff           = 0x00;
    BYTE NSOn            = 0x01;
    WORD TXVoiceGain     = 0xc000;
    WORD DTMFTXGain      = 0x1000;
    WORD CodecSTGain     = 0x400;

    TXPCMIIRFltr tempTXPCMIIRFLTR;
    WORD TXFlag          = 0x00;
    WORD TXStageCnt      = 0x00;
    BYTE TXStage0Val[20] = {
                             0x0, 0x0, 0x0, 0x0, 0x0,
                             0x0, 0x0, 0x0, 0x0, 0x0,
                             0x0, 0x0, 0x0, 0x0, 0x0,
                             0x0, 0x0, 0x0, 0x0, 0x0
                           };

    BYTE TXStage1Val[20] = {
                             0x0, 0x0, 0x0, 0x0, 0x0,
                             0x0, 0x0, 0x0, 0x0, 0x0,
                             0x0, 0x0, 0x0, 0x0, 0x0,
                             0x0, 0x0, 0x0, 0x0, 0x0
                           };

    BYTE TXStage2Val[20] = {
                             0x0, 0x0, 0x0, 0x0, 0x0,
                             0x0, 0x0, 0x0, 0x0, 0x0,
                             0x0, 0x0, 0x0, 0x0, 0x0,
                             0x0, 0x0, 0x0, 0x0, 0x0
                           };

    BYTE TXStage3Val[20] = {
                             0x0, 0x0, 0x0, 0x0, 0x0,
                             0x0, 0x0, 0x0, 0x0, 0x0,
                             0x0, 0x0, 0x0, 0x0, 0x0,
                             0x0, 0x0, 0x0, 0x0, 0x0
                           };

    BYTE TXStage4Val[20] = {
                             0x0, 0x0, 0x0, 0x0, 0x0,
                             0x0, 0x0, 0x0, 0x0, 0x0,
                             0x0, 0x0, 0x0, 0x0, 0x0,
                             0x0, 0x0, 0x0, 0x0, 0x0
                           };
    tempTXPCMIIRFLTR.pFlag      = &TXFlag;
    tempTXPCMIIRFLTR.pStageCnt  = &TXStageCnt;
    tempTXPCMIIRFLTR.pStage0Val = TXStage0Val;
    tempTXPCMIIRFLTR.pStage1Val = TXStage1Val;
    tempTXPCMIIRFLTR.pStage2Val = TXStage2Val;
    tempTXPCMIIRFLTR.pStage3Val = TXStage3Val;
    tempTXPCMIIRFLTR.pStage4Val = TXStage4Val;

    RXPCMIIRFltr tempRXPCMIIRFLTR;
    WORD RXFlag          = 0x00;
    WORD RXStageCnt      = 0x00;
    BYTE RXStage0Val[20] = {
                             0x0, 0x0, 0x0, 0x0, 0x0,
                             0x0, 0x0, 0x0, 0x0, 0x0,
                             0x0, 0x0, 0x0, 0x0, 0x0,
                             0x0, 0x0, 0x0, 0x0, 0x0
                           };

    BYTE RXStage1Val[20] = {
                             0x0, 0x0, 0x0, 0x0, 0x0,
                             0x0, 0x0, 0x0, 0x0, 0x0,
                             0x0, 0x0, 0x0, 0x0, 0x0,
                             0x0, 0x0, 0x0, 0x0, 0x0
                           };

    BYTE RXStage2Val[20] = {
                             0x0, 0x0, 0x0, 0x0, 0x0,
                             0x0, 0x0, 0x0, 0x0, 0x0,
                             0x0, 0x0, 0x0, 0x0, 0x0,
                             0x0, 0x0, 0x0, 0x0, 0x0
                           };

    BYTE RXStage3Val[20] = {
                             0x0, 0x0, 0x0, 0x0, 0x0,
                             0x0, 0x0, 0x0, 0x0, 0x0,
                             0x0, 0x0, 0x0, 0x0, 0x0,
                             0x0, 0x0, 0x0, 0x0, 0x0
                           };

    BYTE RXStage4Val[20] = {
                             0x0, 0x0, 0x0, 0x0, 0x0,
                             0x0, 0x0, 0x0, 0x0, 0x0,
                             0x0, 0x0, 0x0, 0x0, 0x0,
                             0x0, 0x0, 0x0, 0x0, 0x0
                           };
    tempRXPCMIIRFLTR.pFlag      = &RXFlag;
    tempRXPCMIIRFLTR.pStageCnt  = &RXStageCnt;
    tempRXPCMIIRFLTR.pStage0Val = RXStage0Val;
    tempRXPCMIIRFLTR.pStage1Val = RXStage1Val;
    tempRXPCMIIRFLTR.pStage2Val = RXStage2Val;
    tempRXPCMIIRFLTR.pStage3Val = RXStage3Val;
    tempRXPCMIIRFLTR.pStage4Val = RXStage4Val;

    BYTE RXAVCAGCSwitch  = 0x00;
    BYTE TXAVCSwitch     = 0x00;

    RXAGCList tempRXAGCList;
    WORD RXAGCStatGain   = 0x00;
    WORD RXGainSelFlag   = 0xFFFF;
    WORD RXExpThres      = 0x0780;
    WORD RXExpSlope      = 0xff9a;
    WORD RXComprThres    = 0x1b0c;
    WORD RXComprSlope    = 0xf333;
    tempRXAGCList.pRXStaticGain = &RXAGCStatGain;
    tempRXAGCList.pRXAIG        = &RXGainSelFlag;
    tempRXAGCList.pRXExpThres   = &RXExpThres;
    tempRXAGCList.pRXExpSlope   = &RXExpSlope;
    tempRXAGCList.pRXComprThres = &RXComprThres;
    tempRXAGCList.pRXComprSlope = &RXComprSlope;

    RXAVCList tempRXAVCList;
    WORD RXAVCSens       = 0xf200;
    WORD RXAVCHeadroom   = 0x0;
    tempRXAVCList.pAVRXAVCSens     = &RXAVCSens;
    tempRXAVCList.pAVRXAVCHeadroom = &RXAVCHeadroom;

    TXAGCList tempTXAGCList;
    WORD TXAGCStatGain   = 0x7f65;
    WORD TXGainSelFlag   = 0x0;
    WORD TXExpThres      = 0x780;
    WORD TXExpSlope      = 0xff9a;
    WORD TXComprThres    = 0x1b0c;
    WORD TXComprSlope    = 0xf333;
    tempTXAGCList.pTXStaticGain = &TXAGCStatGain;
    tempTXAGCList.pTXAIG        = &TXGainSelFlag;
    tempTXAGCList.pTXExpThres   = &TXExpThres;
    tempTXAGCList.pTXExpSlope   = &TXExpSlope;
    tempTXAGCList.pTXComprThres = &TXComprThres;
    tempTXAGCList.pTXComprSlope = &TXComprSlope;


    SLQSSetAudioPathConfigTestCase_t SLQSSetAudioPathConfigTestCases[] =
    {
        { eQCWWAN_ERR_NONE, 1, &audioProfileHS, NULL, NULL, NULL, NULL,
          NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
          "Valid Audio Profile - All optional params NULL"},

        { eQCWWAN_ERR_NONE, 2, &audioProfileHS, &ECMode, NULL, NULL, NULL,
          NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
          "Valid Audio Profile & EC Mode - Rest optional params NULL"},

        { eQCWWAN_ERR_NONE, 3, &audioProfileHS, NULL, &NSOff, NULL, NULL,
          NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
          "Valid Audio Profile & NS(Off) - Rest optional params NULL"},

        { eQCWWAN_ERR_NONE, 4, &audioProfileHS, NULL, &NSOn, NULL, NULL,
          NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
          "Valid Audio Profile & NS(On) - Rest optional params NULL"},

        { eQCWWAN_ERR_NONE, 5, &audioProfileHS, NULL, NULL, &TXVoiceGain, NULL,
          NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
          "Valid Audio Profile & TX voice gain - Rest optional params NULL"},

        { eQCWWAN_ERR_NONE, 6, &audioProfileHS, NULL, NULL, NULL, &DTMFTXGain,
          NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
          "Valid Audio Profile & DTMF TX voice gain - Rest optional params NULL"},

        { eQCWWAN_ERR_NONE, 7, &audioProfileHS, NULL, NULL, NULL, NULL,
          &CodecSTGain, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
          "Valid Audio Profile & Codec ST gain - Rest optional params NULL"},

        { eQCWWAN_ERR_NONE, 8, &audioProfileHS, NULL, NULL, NULL, NULL,
          NULL, &tempTXPCMIIRFLTR, NULL, NULL, NULL, NULL, NULL, NULL,
          "Valid Audio Profile & TX PCM filter - Rest optional params NULL"},

        { eQCWWAN_ERR_NONE, 9, &audioProfileHS, NULL, NULL, NULL, NULL,
          NULL, NULL, &tempRXPCMIIRFLTR, NULL, NULL, NULL, NULL, NULL,
          "Valid Audio Profile & RX PCM filter - Rest optional params NULL"},

        { eQCWWAN_ERR_NONE, 10, &audioProfileHS, NULL, NULL, NULL, NULL,
          NULL, NULL, NULL, &RXAVCAGCSwitch, NULL, NULL, NULL, NULL,
          "Valid Audio Profile & RX AVC/AGC Switch - Rest optional params NULL"},

        { eQCWWAN_ERR_NONE, 11, &audioProfileHS, NULL, NULL, NULL, NULL,
          NULL, NULL, NULL, NULL, &TXAVCSwitch, NULL, NULL, NULL,
          "Valid Audio Profile & TX AVC Switch - Rest optional params NULL"},

        { eQCWWAN_ERR_NONE, 12, &audioProfileHS, NULL, NULL, NULL, NULL,
          NULL, NULL, NULL, NULL, NULL, &tempRXAGCList, NULL, NULL,
          "Valid Audio Profile & RX AGC List - Rest optional params NULL"},

        { eQCWWAN_ERR_NONE, 13, &audioProfileHS, NULL, NULL, NULL, NULL,
          NULL, NULL, NULL, NULL, NULL, NULL, &tempRXAVCList, NULL,
          "Valid Audio Profile & RX AVC List - Rest optional params NULL"},

        { eQCWWAN_ERR_NONE, 14, &audioProfileHS, NULL, NULL, NULL, NULL,
          NULL, NULL, NULL, NULL, NULL, NULL, NULL, &tempTXAGCList,
          "Valid Audio Profile & TX AGC List - Rest optional params NULL"},

        { eQCWWAN_ERR_NONE, 15, &audioProfileHS, &ECMode, &NSOff, &TXVoiceGain,
          &DTMFTXGain, &CodecSTGain, &tempTXPCMIIRFLTR, &tempRXPCMIIRFLTR, &RXAVCAGCSwitch,
          &TXAVCSwitch, &tempRXAGCList, &tempRXAVCList, &tempTXAGCList,
          "All Valid - Audio Profile: 0"},

        { eQCWWAN_ERR_NONE, 15, &audioProfileI2S, &ECMode, &NSOff, &TXVoiceGain,
          &DTMFTXGain, &CodecSTGain, &tempTXPCMIIRFLTR, &tempRXPCMIIRFLTR, &RXAVCAGCSwitch,
          &TXAVCSwitch, &tempRXAGCList, &tempRXAVCList, &tempTXAGCList,
          "All Valid - Audio Profile: 9"},

        { eQCWWAN_ERR_INVALID_ARG, 15, &audioProfileInv, &ECMode, &NSOff, &TXVoiceGain,
          &DTMFTXGain, &CodecSTGain, &tempTXPCMIIRFLTR, &tempRXPCMIIRFLTR, &RXAVCAGCSwitch,
          &TXAVCSwitch, &tempRXAGCList, &tempRXAVCList, &tempTXAGCList,
          "InValid - Audio Profile: 11"},

    };

    ULONG nRet;
    SetAudioPathConfigReq  req;
    FILE  *fp = NULL;

    fp = tfopen( "../../audio/test/results/slqssetaudiopathconfig.txt", "w" );

    if (fp)
    {
        BYTE tc = 0;
        BYTE ncases = sizeof( SLQSSetAudioPathConfigTestCases )/
                      sizeof( SLQSSetAudioPathConfigTestCases[0] );

        while ( tc < ncases )
        {
            SLQSSetAudioPathConfigTestCase_t *pd =
                                     &SLQSSetAudioPathConfigTestCases[tc];

            /* Set request parameters */
            req.Profile         = *(pd->pProfile);
            req.pECMode         = pd->pECMode;
            req.pNSEnable       = pd->pNS;
            req.pTXGain         = pd->pTXVoiceGain;
            req.pDTMFTXGain     = pd->pDTMFTXGain;
            req.pCodecSTGain    = pd->pCODECSTGain;
            req.pTXPCMIIRFltr   = pd->pTXPCMIIRFLTR;
            req.pRXPCMIIRFltr   = pd->pRXPCMIIRFLTR;
            req.pRXAVCAGCSwitch = pd->pRXAVCAGCSwitch;
            req.pTXAVCSwitch    = pd->pTXAVCSwitch;
            req.pRXAGCList      = pd->pRXAGCList;
            req.pRXAVCList      = pd->pRXAVCList;
            req.pTXAGCList      = pd->pTXAGCList;

            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        (tc + 1),
                        ncases,
                        pd->desc);

            fprintf( fp, "Parameter Set:\n" );
            fprintf( fp, "Profile          : 0x%x\n", req.Profile );

            IFPRINTF( fp, "EC Value         : 0x%x\n", req.pECMode );
            IFPRINTF( fp, "NS Enable        : 0x%x\n", req.pNSEnable );
            IFPRINTF( fp, "TX Voice volume  : 0x%x\n", req.pTXGain );
            IFPRINTF( fp, "DTMF TX gain     : 0x%x\n", req.pDTMFTXGain );
            IFPRINTF( fp, "Codec ST gain    : 0x%x\n", req.pCodecSTGain );

            if( NULL != pd->pTXPCMIIRFLTR )
            {
                TXPCMIIRFltr *pTemp = pd->pTXPCMIIRFLTR;

                fprintf( fp, "\n TX PCM Filter parameters:\n" );

                IFPRINTF( fp, "Flag          : 0x%x\n", pTemp->pFlag );
                IFPRINTF( fp, "Stage Count   : 0x%x\n", pTemp->pStageCnt );
                IFPRINTF( fp, "Stage 0 Value : 0x%x\n", pTemp->pStage0Val );
                IFPRINTF( fp, "Stage 1 Value : 0x%x\n", pTemp->pStage1Val );
                IFPRINTF( fp, "Stage 2 Value : 0x%x\n", pTemp->pStage2Val );
                IFPRINTF( fp, "Stage 3 Value : 0x%x\n", pTemp->pStage3Val );
                IFPRINTF( fp, "Stage 4 Value : 0x%x\n", pTemp->pStage4Val );
            }

            if( NULL != pd->pRXPCMIIRFLTR )
            {
                RXPCMIIRFltr *pTemp = pd->pRXPCMIIRFLTR;

                fprintf( fp, "\n RX PCM Filter parameters:\n" );

                IFPRINTF( fp, "Flag          : 0x%x\n", pTemp->pFlag );
                IFPRINTF( fp, "Stage Count   : 0x%x\n", pTemp->pStageCnt );
                IFPRINTF( fp, "Stage 0 Value : 0x%x\n", pTemp->pStage0Val );
                IFPRINTF( fp, "Stage 1 Value : 0x%x\n", pTemp->pStage1Val );
                IFPRINTF( fp, "Stage 2 Value : 0x%x\n", pTemp->pStage2Val );
                IFPRINTF( fp, "Stage 3 Value : 0x%x\n", pTemp->pStage3Val );
                IFPRINTF( fp, "Stage 4 Value : 0x%x\n", pTemp->pStage4Val );
            }

            IFPRINTF( fp, "RX AVC/AGC Switch : 0x%x\n", req.pRXAVCAGCSwitch );
            IFPRINTF( fp, "TX AVC Switch     : 0x%x\n", req.pTXAVCSwitch );

            if( NULL != pd->pRXAGCList )
            {
                RXAGCList *pTemp = pd->pRXAGCList;

                fprintf( fp, "\n RX AGC List parameters:\n" );
                IFPRINTF( fp, "Static Gain              : 0x%x\n", pTemp->pRXStaticGain );
                IFPRINTF( fp, "Gain Selection Flag      : 0x%x\n", pTemp->pRXAIG );
                IFPRINTF( fp, "RX Expansion Threshold   : 0x%x\n", pTemp->pRXExpThres );
                IFPRINTF( fp, "RX Expansion Slope       : 0x%x\n", pTemp->pRXExpSlope );
                IFPRINTF( fp, "RX Compression Threshold : 0x%x\n", pTemp->pRXComprThres );
                IFPRINTF( fp, "RX Compression Slope     : 0x%x\n", pTemp->pRXComprSlope );
            }

            if( NULL != pd->pRXAVCList )
            {
                RXAVCList *pTemp = pd->pRXAVCList;

                fprintf( fp, "\n RX AVC List parameters:\n" );
                IFPRINTF( fp, "Static Gain              : 0x%x\n", pTemp->pAVRXAVCSens );
                IFPRINTF( fp, "Gain Selection Flag      : 0x%x\n", pTemp->pAVRXAVCHeadroom );
            }

            if( NULL != pd->pTXAGCList )
            {
                TXAGCList *pTemp = pd->pTXAGCList;

                fprintf( fp, "\n TX AGC List parameters:\n" );
                IFPRINTF( fp, "Static Gain              : 0x%x\n", pTemp->pTXStaticGain );
                IFPRINTF( fp, "Gain Selection Flag      : 0x%x\n", pTemp->pTXAIG );
                IFPRINTF( fp, "TX Expansion Threshold   : 0x%x\n", pTemp->pTXExpThres );
                IFPRINTF( fp, "TX Expansion Slope       : 0x%x\n", pTemp->pTXExpSlope );
                IFPRINTF( fp, "TX Compression Threshold : 0x%x\n", pTemp->pTXComprThres );
                IFPRINTF( fp, "TX Compression Slope     : 0x%x\n", pTemp->pTXComprSlope );
            }
            nRet = SLQSSetAudioPathConfig( &req );

            /* Display result code and text */
            doprintreason ( fp, nRet );

            if ( !nRet )
            {
                fprintf( fp, "Audio Path successfully Set\n" );
            }
            tc++;
        }
    }
    if ( fp )
        tfclose( fp );
}

/*
 * Name:     doSLQSGetAudioVolTLBConfig
 *
 * Purpose:  Perform the tests that call the SLQSGetAudioVolTLBConfig() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSLQSGetAudioVolTLBConfig( void )
{
    BYTE audioProfileHS  = 0x00;
    BYTE audioProfileI2S = 0x09;
    BYTE audioProfileInv = 0x0B;

    BYTE audioGeneratorVoice   = 0x00;
    BYTE audioGeneratorKeyBeep = 0x01;

    BYTE audioVolLev0 = 0x00;
    BYTE audioVolLev3 = 0x03;

    BYTE audioRXVol   = 0x0D;
    BYTE audioDTMFVol = 0x0E;
    BYTE audioPAD     = 0x0F;

    GetAudioVolTLBConfigResp tempResp;

    SLQSGetAudioVolTLBConfigTestCase_t SLQSGetAudioVolTLBConfigTestCases[] =
    {
        { eQCWWAN_ERR_INVALID_ARG, 1, &audioProfileHS, &audioGeneratorVoice,
          &audioVolLev0, &audioDTMFVol, NULL,
          "Invalid - Output parameters NULL"},

        { eQCWWAN_ERR_NONE, 2, &audioProfileInv, &audioGeneratorVoice,
          &audioVolLev3, &audioRXVol, &tempResp,
          "InValid - Audio Profile - 10"},

        { eQCWWAN_ERR_NONE, 3, &audioProfileHS, &audioGeneratorVoice,
          &audioVolLev3, &audioDTMFVol, &tempResp,
          "Valid - Profile: HS, Voice, 3, DTMF"},

        { eQCWWAN_ERR_NONE, 4, &audioProfileI2S, &audioGeneratorVoice,
          &audioVolLev0, &audioRXVol, &tempResp,
          "Valid - Profile: I2S, Voice, 0, RX"},

        { eQCWWAN_ERR_NONE, 5, &audioProfileI2S, &audioGeneratorVoice,
          &audioVolLev0, &audioDTMFVol, &tempResp,
          "Valid - Profile: I2S, Voice, 0, DTMF"},

        { eQCWWAN_ERR_NONE, 6, &audioProfileI2S, &audioGeneratorVoice,
          &audioVolLev0, &audioPAD, &tempResp,
          "Valid - Profile: I2S, Voice, 0, PAD"},

        { eQCWWAN_ERR_NONE, 7, &audioProfileI2S, &audioGeneratorKeyBeep,
          &audioVolLev3, &audioDTMFVol, &tempResp,
          "Valid - Profile: I2S, KeyBeep, 3, DTMF"},
    };

    ULONG nRet;
    GetAudioVolTLBConfigReq  req;
    GetAudioVolTLBConfigResp *pResp;
    FILE  *fp = NULL;

    fp = tfopen( "../../audio/test/results/slqsgetaudiovoltlbconfig.txt", "w" );

    if (fp)
    {
        BYTE tc = 0;
        BYTE ncases = sizeof( SLQSGetAudioVolTLBConfigTestCases )/
                      sizeof( SLQSGetAudioVolTLBConfigTestCases[0] );

        while ( tc < ncases )
        {
            SLQSGetAudioVolTLBConfigTestCase_t *pd =
                                     &SLQSGetAudioVolTLBConfigTestCases[tc];
            req.Profile   = *(pd->pAudioProfile);
            req.Generator = *(pd->pGenerator);
            req.Volume    = *(pd->pVolume);
            req.Item      = *(pd->pItem);
            pResp = pd->pGetAudioVolTLBConfigResp;

            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        (tc + 1),
                        ncases,
                        pd->desc);

            fprintf( fp, "Parameter Set:\n" );
            fprintf( fp, "Profile   :0x%x\n", req.Profile );
            fprintf( fp, "Generator :0x%x\n", req.Generator );
            fprintf( fp, "Volume    :0x%x\n", req.Volume );
            fprintf( fp, "Item      :0x%x\n", req.Item );

            nRet = SLQSGetAudioVolTLBConfig( &req, pResp );

            /* Display result code and text */
            doprintreason ( fp, nRet );

            if ( !nRet )
            {
                fprintf( fp, "Audio Vol TLB config successfully retrieved\n" );

                if( pd->pGetAudioVolTLBConfigResp )
                {
                    fprintf( fp, "Value          : %x\n", pResp->ResCode );
                }
            }
            tc++;
        }
    }
    if ( fp )
        tfclose( fp );
}

/*
 * Name:     doSLQSSetAudioVolTLBConfig
 *
 * Purpose:  Perform the tests that call the SLQSSetAudioVolTLBConfig() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSLQSSetAudioVolTLBConfig( void )
{
    BYTE audioProfileHS  = 0x00;
    BYTE audioProfileI2S = 0x09;
    BYTE audioProfileInv = 0x0B;

    BYTE audioGeneratorVoice   = 0x00;
    BYTE audioGeneratorKeyBeep = 0x01;

    BYTE audioVolLev0 = 0x00;
    BYTE audioVolLev3 = 0x03;

    BYTE audioRXVol   = 0x0D;
    BYTE audioDTMFVol = 0x0E;
    BYTE audioPAD     = 0x0F;

    WORD VolTableVal0 = 0x00;
    WORD VolTableVal1 = 0x01;

    SetAudioVolTLBConfigResp tempResp;

    SLQSSetAudioVolTLBConfigTestCase_t SLQSSetAudioVolTLBConfigTestCases[] =
    {
        { eQCWWAN_ERR_INVALID_ARG, 1, &audioProfileHS, &audioGeneratorVoice,
          &audioVolLev0, &audioDTMFVol, &VolTableVal0, NULL,
          "Invalid - Output parameters NULL"},

        { eQCWWAN_ERR_NONE, 2, &audioProfileInv, &audioGeneratorVoice,
          &audioVolLev3, &audioRXVol, &VolTableVal0, &tempResp,
          "InValid - Audio Profile - 10"},

        { eQCWWAN_ERR_NONE, 3, &audioProfileHS, &audioGeneratorVoice,
          &audioVolLev3, &audioDTMFVol, &VolTableVal0, &tempResp,
          "Valid - Profile: HS, Voice, 3, DTMF, 0"},

        { eQCWWAN_ERR_NONE, 4, &audioProfileI2S, &audioGeneratorVoice,
          &audioVolLev0, &audioRXVol, &VolTableVal1, &tempResp,
          "Valid - Profile: I2S, Voice, 0, RX, 1"},

        { eQCWWAN_ERR_NONE, 5, &audioProfileI2S, &audioGeneratorVoice,
          &audioVolLev0, &audioDTMFVol, &VolTableVal0, &tempResp,
          "Valid - Profile: I2S, Voice, 0, DTMF, 0"},

        { eQCWWAN_ERR_NONE, 6, &audioProfileI2S, &audioGeneratorVoice,
          &audioVolLev0, &audioPAD, &VolTableVal1, &tempResp,
          "Valid - Profile: I2S, Voice, 0, PAD, 1"},

        { eQCWWAN_ERR_NONE, 7, &audioProfileI2S, &audioGeneratorKeyBeep,
          &audioVolLev3, &audioDTMFVol, &VolTableVal1, &tempResp,
          "Valid - Profile: I2S, KeyBeep, 3, DTMF, 1"},
    };

    ULONG nRet;
    SetAudioVolTLBConfigReq  req;
    SetAudioVolTLBConfigResp *pResp;
    FILE  *fp = NULL;

    fp = tfopen( "../../audio/test/results/slqssetaudiovoltlbconfig.txt", "w" );

    if (fp)
    {
        BYTE tc = 0;
        BYTE ncases = sizeof( SLQSSetAudioVolTLBConfigTestCases )/
                      sizeof( SLQSSetAudioVolTLBConfigTestCases[0] );

        while ( tc < ncases )
        {
            SLQSSetAudioVolTLBConfigTestCase_t *pd =
                                     &SLQSSetAudioVolTLBConfigTestCases[tc];
            req.Profile   = *(pd->pAudioProfile);
            req.Generator = *(pd->pGenerator);
            req.Volume    = *(pd->pVolume);
            req.Item      = *(pd->pItem);
            req.VolValue  = *(pd->pVolTableVal);
            pResp = pd->pSetAudioVolTLBConfigResp;

            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        (tc + 1),
                        ncases,
                        pd->desc);

            fprintf( fp, "Parameter Set:\n" );
            fprintf( fp, "Profile            :0x%x\n", req.Profile );
            fprintf( fp, "Generator          :0x%x\n", req.Generator );
            fprintf( fp, "Volume             :0x%x\n", req.Volume );
            fprintf( fp, "Item               :0x%x\n", req.Item );
            fprintf( fp, "Volume Table Value :0x%x\n", req.VolValue );

            nRet = SLQSSetAudioVolTLBConfig( &req, pResp );

            /* Display result code and text */
            doprintreason ( fp, nRet );

            if ( !nRet )
            {
                fprintf( fp, "Audio Vol TLB config successfully set\n" );

                if( pd->pSetAudioVolTLBConfigResp )
                {
                    fprintf( fp, "Value          : %x\n", pResp->ResCode );
                }
            }
            tc++;
        }
    }
    if ( fp )
        tfclose( fp );
}
