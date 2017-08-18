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

#include "qaGobiApiSwiAudio.h"
#include "qaGobiApiSwiAudioTest.h"
#include "qatesthelper.h"
#include "qatestproto.h"

/*******************
    TEST DATA
 *******************/

/* Constants */

/* Global Variables */

/* Forward declaration */


/*******************
  TEST FUNCTIONS
 *******************/

/*
 * Name:     doSLQSGetM2MAudioProfile
 *
 * Purpose:  Perform the tests that call the SLQSGetM2MAudioProfile() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSLQSGetM2MAudioProfile( void )
{
    BYTE GenrtValid   = 0;
    BYTE GenrtInValid = 2;

    SLQSGetM2MAudioProfileTestCase_t SLQSGetM2MAudioProfileTestCases[] =
    {
        { eQCWWAN_ERR_INVALID_ARG, 1, &GenrtInValid, "Invalid - generator"},

        { eQCWWAN_ERR_NONE, 2, &GenrtValid, "Valid - Voice Generator"},

        { eQCWWAN_ERR_NONE, 3, NULL, "Valid - Optional parameter NULL"},
    };

    ULONG nRet;
    GetM2MAudioProfileReq  pReq;
    GetM2MAudioProfileResp pResp;
    FILE  *fp = NULL;

    fp = tfopen( "../../swiaudio/test/results/slqsgetm2maudioprofile.txt", "w" );

    if (fp)
    {
        BYTE tc = 0;
        BYTE ncases = sizeof( SLQSGetM2MAudioProfileTestCases )/
                      sizeof( SLQSGetM2MAudioProfileTestCases[0] );

        while ( tc < ncases )
        {
            SLQSGetM2MAudioProfileTestCase_t *pd =
                                     &SLQSGetM2MAudioProfileTestCases[tc];

            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        (tc + 1),
                        ncases,
                        pd->desc);

            pReq.pGenerator = pd->pGenerator;

            fprintf( fp, "\n\nParameters set \n" );
            IFPRINTF( fp, "Generator: 0x%x\n", pReq.pGenerator );

            nRet = SLQSGetM2MAudioProfile( &pReq, &pResp );

            /* Display result code and text */
            doprintreason ( fp, nRet );

            if ( !nRet )
            {
                fprintf( fp, "Audio Profile successfully retrieved\n" );
                fprintf( fp, "Profile   : %x\n", pResp.Profile );
                fprintf( fp, "Ear Mute  : %x\n", pResp.EarMute );
                fprintf( fp, "Mic Mute  : %x\n", pResp.MicMute );
                fprintf( fp, "Generator : %x\n", pResp.Generator );
                fprintf( fp, "Volume    : %x\n", pResp.Volume );
                fprintf( fp, "Cwt Mute  : %x\n", pResp.CwtMute );
            }
            tc++;
        }
    }
    if ( fp )
        tfclose( fp );
}

void doSLQSSetM2MAudioProfile( void )
{
    BYTE Profile    = 3;

    /* Invalid values */
    BYTE EarMuteInv = 2;
    BYTE MicMuteInv = 2;
    BYTE GenrtrInv  = 1;
    BYTE VolInv     = 6;
    BYTE CwtMuteInv = 3;

    /* Valid values */
    BYTE EarMute = 1;
    BYTE MicMute = 1;
    BYTE Genrtr  = 0;
    BYTE Volume  = 3;
    BYTE CwtMute = 1;

    SLQSSetM2MAudioProfileTestCase_t SLQSSetM2MAudioProfileTestCases[] =
    {
        { eQCWWAN_ERR_INVALID_ARG, 1, Profile, &EarMuteInv, &MicMuteInv, &GenrtrInv,
          &VolInv, &CwtMuteInv, "Invalid - invalid parameter values"},

        { eQCWWAN_ERR_NONE, 2, Profile, &EarMute, &MicMute, &Genrtr,
          &Volume, NULL, "Valid - Voice Audio Profile, optional params NULL"},

        { eQCWWAN_ERR_NONE, 3, Profile, &EarMute, &MicMute, &Genrtr, &Volume,
          &CwtMute, "Valid - All params"},
    };

    ULONG nRet;
    SetM2MAudioProfileReq pReq;
    FILE  *fp = NULL;

    fp = tfopen( "../../swiaudio/test/results/slqssetm2maudioprofile.txt", "w" );

    if (fp)
    {
        BYTE tc = 0;
        BYTE ncases = sizeof( SLQSSetM2MAudioProfileTestCases )/
                      sizeof( SLQSSetM2MAudioProfileTestCases[0] );

        while ( tc < ncases )
        {
            SLQSSetM2MAudioProfileTestCase_t *pd =
                                     &SLQSSetM2MAudioProfileTestCases[tc];

            /* Assign request parameters */
            pReq.Profile    = pd->Profile;
            pReq.pEarMute   = pd->pEarMute;
            pReq.pMicMute   = pd->pMicMute;
            pReq.pGenerator = pd->pGenerator;
            pReq.pVolume    = pd->pVolume;
            pReq.pCwtMute   = pd->pCwtMute;

            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        (tc + 1),
                        ncases,
                        pd->desc);

            fprintf( fp, "\n\nParameters set \n" );
            fprintf( fp, "Profile: 0x%x\n",    pReq.Profile );
            IFPRINTF( fp, "Ear Mute: 0x%x\n",  pReq.pEarMute );
            IFPRINTF( fp, "Mic Mute: 0x%x\n",  pReq.pMicMute );
            IFPRINTF( fp, "Generator: 0x%x\n", pReq.pGenerator );
            IFPRINTF( fp, "Volume: 0x%x\n",    pReq.pVolume );
            IFPRINTF( fp, "CWT Mute: 0x%x\n",  pReq.pCwtMute );

            nRet = SLQSSetM2MAudioProfile( &pReq );

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

void doSLQSGetM2MAudioVolume( void )
{
    BYTE Profile   = 3;
    BYTE Generator = 0;
    BYTE GenrtrInv = 2;

    SLQSGetM2MAudioVolumeTestCase_t SLQSGetM2MAudioVolumeTestCases[] =
    {
        { eQCWWAN_ERR_INVALID_ARG, 1, Profile, GenrtrInv, "Invalid - Invalid generator"},

        { eQCWWAN_ERR_NONE, 2, Profile, Generator, "Valid - Voice volume"},
    };

    ULONG nRet;
    GetM2MAudioVolumeReq  pReq;
    GetM2MAudioVolumeResp pResp;
    FILE  *fp = NULL;

    fp = tfopen( "../../swiaudio/test/results/slqsgetm2maudiovolume.txt", "w" );

    if (fp)
    {
        BYTE tc = 0;
        BYTE ncases = sizeof( SLQSGetM2MAudioVolumeTestCases )/
                      sizeof( SLQSGetM2MAudioVolumeTestCases[0] );

        while ( tc < ncases )
        {
            SLQSGetM2MAudioVolumeTestCase_t *pd =
                                     &SLQSGetM2MAudioVolumeTestCases[tc];

            pReq.Profile   = pd->Profile;
            pReq.Generator = pd->Generator;

            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        (tc + 1),
                        ncases,
                        pd->desc);

            fprintf( fp, "\n\nParameters set \n" );
            fprintf( fp, "Profile: 0x%x\n", pReq.Profile );
            fprintf( fp, "Generator: 0x%x\n", pReq.Generator );

            nRet = SLQSGetM2MAudioVolume( &pReq, &pResp);

            /* Display result code and text */
            doprintreason ( fp, nRet );

            if ( !nRet )
            {
                fprintf( fp, "Audio Volume successfully retrieved\n" );
                fprintf( fp, "Level : 0x%x\n", pResp.Level );
            }
            tc++;
        }
    }
    if ( fp )
        tfclose( fp );
}

void doSLQSSetM2MAudioVolume( void )
{
    BYTE ProfileInv = 6;
    BYTE GenrtrInv  = 2;
    BYTE LevelInv   = 8;

    BYTE Profile   = 3;
    BYTE Generator = 0;
    BYTE Level     = 3;

    SLQSSetM2MAudioVolumeTestCase_t SLQSSetM2MAudioVolumeTestCases[] =
    {
        { eQCWWAN_ERR_INVALID_ARG, 1, ProfileInv, GenrtrInv, LevelInv,
          "Invalid - Invalid parameters" },

        { eQCWWAN_ERR_NONE, 2, Profile, Generator, Level, "Valid - Voice Audio Profile" },
    };

    ULONG nRet;
    SetM2MAudioVolumeReq pReq;
    FILE  *fp = NULL;

    fp = tfopen( "../../swiaudio/test/results/slqssetm2maudiovolume.txt", "w" );

    if (fp)
    {
        BYTE tc = 0;
        BYTE ncases = sizeof( SLQSSetM2MAudioVolumeTestCases )/
                      sizeof( SLQSSetM2MAudioVolumeTestCases[0] );

        while ( tc < ncases )
        {
            SLQSSetM2MAudioVolumeTestCase_t *pd =
                                     &SLQSSetM2MAudioVolumeTestCases[tc];

                pReq.Profile   = pd->Profile;
                pReq.Generator = pd->Generator;
                pReq.Level     = pd->Level;

            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        (tc + 1),
                        ncases,
                        pd->desc);

            fprintf( fp, "\n\nParameters set \n" );
            fprintf( fp, "Profile: 0x%x\n", pReq.Profile );
            fprintf( fp, "Generator: 0x%x\n", pReq.Generator );
            fprintf( fp, "Level: 0x%x\n", pReq.Level );

            nRet = SLQSSetM2MAudioVolume( &pReq );

            /* Display result code and text */
            doprintreason ( fp, nRet );

            if ( !nRet )
            {
                fprintf( fp, "Volume successfully Set\n" );
            }
            tc++;
        }
    }
    if ( fp )
        tfclose( fp );
}

void doSLQSSetM2MAudioAVCFG( void )
{
    BYTE ProfInv = 6;
    BYTE DevInv  = 5;
    BYTE PIFaceIdInv = 4;

    BYTE Profile = 3;
    BYTE Device  = 3; /* 3-USB */
    BYTE PIFaceId = 3; /* 3-USB */

    SLQSSetM2MAudioAVCFGTestCase_t SLQSSetM2MAudioAVCFGTestCases[] =
    {
        { eQCWWAN_ERR_INVALID_ARG, 1, ProfInv, DevInv, PIFaceIdInv, NULL,
          "Invalid - Output parameters NULL"},

        { eQCWWAN_ERR_NONE, 2, Profile, Device, PIFaceId, NULL,
          "Valid - USB config set"},
    };

    ULONG nRet;
    SetM2MAudioAVCFGReq pReq;
    FILE  *fp = NULL;
    BYTE ctr = 0;

    fp = tfopen( "../../swiaudio/test/results/slqssetm2maudioAVCFG.txt", "w" );

    if (fp)
    {
        BYTE tc = 0;
        BYTE ncases = sizeof( SLQSSetM2MAudioAVCFGTestCases )/
                      sizeof( SLQSSetM2MAudioAVCFGTestCases[0] );

        while ( tc < ncases )
        {
            SLQSSetM2MAudioAVCFGTestCase_t *pd =
                                     &SLQSSetM2MAudioAVCFGTestCases[tc];

                pReq.Profile  = pd->Profile;
                pReq.Device   = pd->Device;
                pReq.PIFACEId = pd->PFACEId;
                pReq.pPCMParams = pd->pPCMParams;

            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        (tc + 1),
                        ncases,
                        pd->desc);

            fprintf( fp, "\n\nParameters set \n" );
            fprintf( fp, "Profile: 0x%x\n",  pReq.Profile );
            fprintf( fp, "Device: 0x%x\n",   pReq.Device );
            fprintf( fp, "PIFaceID: 0x%x\n", pReq.PIFACEId );
            if( NULL != pReq.pPCMParams )
            {
                fprintf( fp, "IFace Table length: 0x%x\n", pReq.pPCMParams->iFaceTabLen );
                fprintf( fp, "IFace Table: ");
                for( ctr = 0; ctr < pReq.pPCMParams->iFaceTabLen; ctr++ )
                {
                    fprintf( fp, "0x%x",  pReq.pPCMParams->iFaceTab[ctr] );
                }
                fprintf( fp, "\n" );
            }

            nRet = SLQSSetM2MAudioAVCFG( &pReq );

            /* Display result code and text */
            doprintreason ( fp, nRet );

            if ( !nRet )
            {
                fprintf( fp, "AVCFG successfully Set\n" );
            }
            tc++;
        }
    }
    if ( fp )
        tfclose( fp );
}

void doSLQSSetM2MAudioLPBK( void )
{
    BYTE Enable = 0;
    BYTE EnableInv = 5;

    SLQSSetM2MAudioLPBKTestCase_t SLQSSetM2MAudioLPBKTestCases[] =
    {
        { eQCWWAN_ERR_INVALID_ARG, 1, EnableInv, "Invalid - Invalid Enable parameter" },

        { eQCWWAN_ERR_NONE, 2, Enable, "Valid - Enable = 0"},
    };

    ULONG nRet;
    SetM2MAudioLPBKReq pReq;
    FILE  *fp = NULL;

    fp = tfopen( "../../swiaudio/test/results/slqssetm2maudioLPBK.txt", "w" );

    if (fp)
    {
        BYTE tc = 0;
        BYTE ncases = sizeof( SLQSSetM2MAudioLPBKTestCases )/
                      sizeof( SLQSSetM2MAudioLPBKTestCases[0] );

        while ( tc < ncases )
        {
            SLQSSetM2MAudioLPBKTestCase_t *pd =
                                     &SLQSSetM2MAudioLPBKTestCases[tc];

            pReq.Enable  = pd->Enable;

            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        (tc + 1),
                        ncases,
                        pd->desc);

            fprintf( fp, "\n\nParameters set \n" );
            fprintf( fp, "Enable: 0x%x\n",  pReq.Enable );

            nRet = SLQSSetM2MAudioLPBK( &pReq );

            /* Display result code and text */
            doprintreason ( fp, nRet );

            if ( !nRet )
            {
                fprintf( fp, "LPBK successfully Set\n" );
            }
            tc++;
        }
    }
    if ( fp )
        tfclose( fp );
}

void doSLQSSetM2MAudioNVDef( void )
{

    SLQSSetM2MAudioNVDefTestCase_t SLQSSetM2MAudioNVDefTestCases[] =
    {
        { eQCWWAN_ERR_INVALID_ARG, 1, "Valid"},
    };

    ULONG nRet;
    FILE  *fp = NULL;

    fp = tfopen( "../../swiaudio/test/results/slqssetm2maudioNVDef.txt", "w" );

    if (fp)
    {
        BYTE tc = 0;
        BYTE ncases = sizeof( SLQSSetM2MAudioNVDefTestCases )/
                      sizeof( SLQSSetM2MAudioNVDefTestCases[0] );

        while ( tc < ncases )
        {
            SLQSSetM2MAudioNVDefTestCase_t *pd =
                                     &SLQSSetM2MAudioNVDefTestCases[tc];
            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        (tc + 1),
                        ncases,
                        pd->desc);

            nRet = SLQSSetM2MAudioNVDef();

            /* Display result code and text */
            doprintreason ( fp, nRet );

            if ( !nRet )
            {
                fprintf( fp, "All Audio successfully set to default value\n" );
            }
            tc++;
        }
    }
    if ( fp )
        tfclose( fp );
}

void doSLQSGetM2MSpkrGain( void )
{
    BYTE Profile    = 3;
    BYTE InvProfile = 7;
    SLQSGetM2MSpkrGainTestCase_t SLQSGetM2MSpkrGainTestCases[] =
    {
        { eQCWWAN_ERR_INVALID_ARG, 1, InvProfile, "Invalid - Profile value invalid" },

        { eQCWWAN_ERR_NONE, 2, Profile, "Valid - Profile 3" },
    };

    ULONG nRet;
    GetM2MSpkrGainReq  pReq;
    GetM2MSpkrGainResp pResp;
    FILE  *fp = NULL;

    fp = tfopen( "../../swiaudio/test/results/slqsgetm2mSpkrGain.txt", "w" );

    if (fp)
    {
        BYTE tc = 0;
        BYTE ncases = sizeof( SLQSGetM2MSpkrGainTestCases )/
                      sizeof( SLQSGetM2MSpkrGainTestCases[0] );

        while ( tc < ncases )
        {
            SLQSGetM2MSpkrGainTestCase_t *pd =
                                     &SLQSGetM2MSpkrGainTestCases[tc];

            pReq.Profile = pd->Profile;

            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        (tc + 1),
                        ncases,
                        pd->desc);

            fprintf( fp, "\n\nParameters set \n" );
            fprintf( fp, "Profile: 0x%x\n",  pReq.Profile );

            nRet = SLQSGetM2MSpkrGain( &pReq, &pResp);

            /* Display result code and text */
            doprintreason ( fp, nRet );

            if ( !nRet )
            {
                fprintf( fp, "Speaker Gain Value Successfully Retrieved\n" );
                fprintf( fp, "Value   : %x\n", pResp.Value );
            }
            tc++;
        }
    }
    if ( fp )
        tfclose( fp );
}

void doSLQSSetM2MSpkrGain( void )
{
    BYTE Profile = 3;
    BYTE ProfInv = 6;
    WORD Value   = 0x01;

    SLQSSetM2MSpkrGainTestCase_t SLQSSetM2MSpkrGainTestCases[] =
    {
        { eQCWWAN_ERR_INVALID_ARG, 1, ProfInv, Value, "Invalid - Invalid profile" },
        { eQCWWAN_ERR_NONE, 2, Profile, Value, "Valid - Valid profile" },
    };

    ULONG nRet;
    SetM2MSpkrGainReq  pReq;
    FILE  *fp = NULL;

    fp = tfopen( "../../swiaudio/test/results/slqssetm2mSpkrGain.txt", "w" );

    if (fp)
    {
        BYTE tc = 0;
        BYTE ncases = sizeof( SLQSSetM2MSpkrGainTestCases )/
                      sizeof( SLQSSetM2MSpkrGainTestCases[0] );

        while ( tc < ncases )
        {
            SLQSSetM2MSpkrGainTestCase_t *pd =
                                     &SLQSSetM2MSpkrGainTestCases[tc];

            pReq.Profile = pd->Profile;
            pReq.Value   = pd->Value;

            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        (tc + 1),
                        ncases,
                        pd->desc);

            fprintf( fp, "\n\nParameters set \n" );
            fprintf( fp, "Profile: 0x%x\n",  pReq.Profile );
            fprintf( fp, "Value: 0x%x\n",    pReq.Value );

            nRet = SLQSSetM2MSpkrGain( &pReq );

            /* Display result code and text */
            doprintreason ( fp, nRet );

            if ( !nRet )
            {
                fprintf( fp, "Speaker Gain Value successfully Set\n" );
            }
            tc++;
        }
    }
    if ( fp )
        tfclose( fp );
}

void doSLQSGetM2MAVMute( void )
{
    BYTE Profile = 3;
    BYTE ProfileInv = 7;

    SLQSGetM2MAVMuteTestCase_t SLQSGetM2MAVMuteTestCases[] =
    {
        { eQCWWAN_ERR_INVALID_ARG, 1, ProfileInv, "Invalid - Profile value invalid"},
        { eQCWWAN_ERR_NONE, 2, Profile, "Valid - Profile = 3"},
    };

    ULONG nRet;
    GetM2MAVMuteReq  pReq;
    GetM2MAVMuteResp pResp;
    FILE  *fp = NULL;

    fp = tfopen( "../../swiaudio/test/results/slqsgetm2mAVMute.txt", "w" );

    if (fp)
    {
        BYTE tc = 0;
        BYTE ncases = sizeof( SLQSGetM2MAVMuteTestCases )/
                      sizeof( SLQSGetM2MAVMuteTestCases[0] );

        while ( tc < ncases )
        {
            SLQSGetM2MAVMuteTestCase_t *pd =
                                     &SLQSGetM2MAVMuteTestCases[tc];

            pReq.Profile = pd->Profile;

            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        (tc + 1),
                        ncases,
                        pd->desc);

            fprintf( fp, "\n\nParameters set \n" );
            fprintf( fp, "Profile: 0x%x\n",  pReq.Profile );

            nRet = SLQSGetM2MAVMute( &pReq ,&pResp );

            /* Display result code and text */
            doprintreason ( fp, nRet );

            if ( !nRet )
            {
                fprintf( fp, "AV Mute Value successfully Retrieved\n" );
                fprintf( fp, "EarMute   : %x\n", pResp.EarMute );
                fprintf( fp, "MicMute   : %x\n", pResp.MicMute );
                fprintf( fp, "CwtMute   : %x\n", pResp.CwtMute );
            }
            tc++;
        }
    }
    if ( fp )
        tfclose( fp );
}

void doSLQSSetM2MAVMute( void )
{
    BYTE Profile = 3;
    BYTE ProfileInv = 6;

    BYTE EarMuteInv = 2;
    BYTE MicMuteInv = 2;
    BYTE CwtMuteInv = 2;

    BYTE EarMute = 1;
    BYTE MicMute = 1;
    BYTE CwtMute = 1;

    SLQSSetM2MAVMuteTestCase_t SLQSSetM2MAVMuteTestCases[] =
    {
        { eQCWWAN_ERR_INVALID_ARG, 1, ProfileInv, EarMute, MicMute,
          &CwtMute, "Invalid - Profile invalid" },

        { eQCWWAN_ERR_INVALID_ARG, 2, Profile, EarMuteInv, MicMuteInv,
          &CwtMuteInv, "Invalid - Mute values invalid" },

        { eQCWWAN_ERR_NONE, 3, Profile, EarMute, MicMute, NULL,
          "Valid - Optional params NULL" },

        { eQCWWAN_ERR_NONE, 4, Profile, EarMute, MicMute, &CwtMute,
          "Valid - Optional params" },
    };

    ULONG nRet;
    SetM2MAVMuteReq  pReq;
    FILE  *fp = NULL;

    fp = tfopen( "../../swiaudio/test/results/slqssetm2mAVMute.txt", "w" );

    if (fp)
    {
        BYTE tc = 0;
        BYTE ncases = sizeof( SLQSSetM2MAVMuteTestCases )/
                      sizeof( SLQSSetM2MAVMuteTestCases[0] );

        while ( tc < ncases )
        {
            SLQSSetM2MAVMuteTestCase_t *pd =
                                     &SLQSSetM2MAVMuteTestCases[tc];

            pReq.Profile = pd->Profile;
            pReq.EarMute = pd->EarMute;
            pReq.MicMute = pd->MicMute;
            pReq.pCwtMute = pd->pCwtMute;

            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        (tc + 1),
                        ncases,
                        pd->desc);

            fprintf( fp, "\n\nParameters set \n" );
            fprintf( fp, "Profile: 0x%x\n", pReq.Profile );
            fprintf( fp, "Ear Mute: 0x%x\n", pReq.EarMute );
            fprintf( fp, "Mic Mute: 0x%x\n", pReq.MicMute );
            IFPRINTF( fp, "Cwt Mute: 0x%x\n", pReq.pCwtMute );

            nRet = SLQSSetM2MAVMute( &pReq );

            /* Display result code and text */
            doprintreason ( fp, nRet );

            if ( !nRet )
            {
                fprintf( fp, "AV Mute Value successfully Set\n" );
            }
            tc++;
        }
    }
    if ( fp )
        tfclose( fp );
}
