/*
 *
 *  Filename: qaGobiApiQosTest.c
 *
 *  Purpose:  Contains functions implementing specific tests for
 *            Qauality of Service (QOS)
 *            called by the main() routine in qatest.c
 *
 *  Copyright: © 2013 Sierra Wireless, Inc., all rights reserved
 *
 **************/

/* include files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* include files */
#include "SwiDataTypes.h"
#include "qmerrno.h"
#include "qaGobiApiQos.h"
#include "qaGobiApiQosTest.h"
#include "qatesthelper.h"
#include "qatestproto.h"
#include "sludefs.h"

static ULONG g_qos_id;
/*******************
    TEST DATA
 *******************/

/*******************
  TEST FUNCTIONS
 *******************/

/*
 * Name:     doSLQSQosReq
 *
 * Purpose:  Perform the tests that call the SLQSQosReq() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSLQSQosReq( void )
{
    BYTE      instance = 0;
    swiQosReq qosReq;
    swiQosIds qosIds;
    BYTE      tCaseNum = 0;
    ULONG     nRet;
    FILE      *fp = NULL;
    int       nTest;

    ULONG   latency = 150;
    ULONG   jitter = 50;
    BYTE    lteQci = 2;

    ULONG    ids[2];
    qosIds.pIds = ids;
    qosIds.sz = sizeof(ids) /sizeof(ids[0]);

    swiQosFlow txFlow;
    tokenBucket bucket;
    dataRate rate;

    memset(&qosReq, 0, sizeof(qosReq));
    memset(&txFlow, 0, sizeof(txFlow));
    qosReq.pTxFlow = &txFlow;

    txFlow.index = 100;
    txFlow.pLatency = &latency;
    txFlow.pJitter = &jitter;
    txFlow.pLteQci = &lteQci;

    bucket.peakRate = 100;
    bucket.tokenRate = 200;
    bucket.bucketSz = 300;
    txFlow.pTokenBucket = &bucket;

    rate.dataRateMax = 100;
    rate.guaranteedRate = 200;
    txFlow.pDataRate = &rate;

    fp = tfopen( "../../qos/test/results/slqsQosReq.txt", "w" );

    /* Define the test cases to be executed */
    const SLQSQosReqTestCase_t SLQSQosReqTestCase[] =
    {
        { eQCWWAN_ERR_INVALID_ARG,           1, NULL, NULL,
                                           "Null Request" },
        { eQCWWAN_ERR_NONE,                  2, &qosReq, &qosIds,
                                           "All Valid" },
    };

    nTest = sizeof(SLQSQosReqTestCase)/sizeof(SLQSQosReqTestCase_t);
    if (fp)
    {
        while( tCaseNum < nTest )
        {
            fprintf( fp, "\n\nTest Case %d / %d : %s\n",
                       (tCaseNum + 1),
                       nTest,
                       SLQSQosReqTestCase[tCaseNum].desc );

            /* Call the API */
            nRet = SLQSQosReq ( instance,
                    SLQSQosReqTestCase[tCaseNum].pReq,
                    SLQSQosReqTestCase[tCaseNum].pIds);

            if ( (nRet == eQCWWAN_ERR_NONE )
                    && ( SLQSQosReqTestCase[tCaseNum].pIds->sz > 0 ) )
            {
                g_qos_id = SLQSQosReqTestCase[tCaseNum].pIds->pIds[0];
            }

            /* Display result code and text */
            doprintreason ( fp, nRet );
            tCaseNum++;
        }
    }
    if (fp)
        tfclose(fp);
}

void doSLQSQosRel( void )
{
    BYTE   instance = 0;
    ULONG  nRet;
    FILE   *fp = NULL;
    int    nTest;

    swiQosIds qosIds;
    ULONG ids[1];
    ids[0] = g_qos_id;
    qosIds.sz = sizeof(ids)/sizeof(ids[0]);
    qosIds.pIds = ids;

    fp = tfopen( "../../qos/test/results/slqsQosRel.txt", "w" );

    /* Define the test cases to be executed */
    const SLQSQosRelTestCase_t tc[] =
    {
        { eQCWWAN_ERR_INVALID_ARG,           1, NULL,
                                           "Null Response" },
        { eQCWWAN_ERR_NONE,                  2, &qosIds,
                                           "All Valid" },
    };

    nTest = sizeof(tc)/sizeof(SLQSQosRelTestCase_t);
    if (fp)
    {
        int i;
        for(i=0;i<nTest;i++)
        {
            fprintf( fp, "\n\nTest Case %d / %d : %s\n",
                       (i + 1),
                       nTest,
                       tc[i].desc );

            /* Call the API */
            nRet = SLQSQosRel ( instance, tc[i].pIds);

            /* Display result code and text */
            doprintreason ( fp, nRet );
        }
        tfclose(fp);
    }
}

void doSLQSQosGetNetworkStatus( void )
{
    BYTE   instance = 0;
    ULONG  nRet;
    FILE   *fp = NULL;
    int    nTest;
    BYTE   status;

    fp = tfopen( "../../qos/test/results/slqsQosGetNetworkStatus.txt", "w" );

    /* Define the test cases to be executed */
    const SLQSQosGetNetworkStatusTestCase_t tc[] =
    {
        { eQCWWAN_ERR_INVALID_ARG,           1, NULL,
                                           "Null Response" },
        { eQCWWAN_ERR_NONE,                  2, &status,
                                           "All Valid" },
    };

    nTest = sizeof(tc)/sizeof(SLQSQosGetNetworkStatusTestCase_t);
    if (fp)
    {
        int i;
        for(i=0;i<nTest;i++)
        {
            fprintf( fp, "\n\nTest Case %d / %d : %s\n",
                       (i + 1),
                       nTest,
                       tc[i].desc );

            /* Call the API */
            nRet = SLQSQosGetNetworkStatus ( instance, tc[i].pStatus);

            /* Display result code and text */
            doprintreason ( fp, nRet );

            if ( eQCWWAN_ERR_NONE == nRet )
            {
                IFPRINTF( fp,"Qos Network support status: %d\n", tc[i].pStatus);
            }
        }
        tfclose(fp);
    }
}

void doSLQSQosGetFlowStatus( void )
{
    BYTE   instance = 0;
    ULONG  nRet;
    FILE   *fp = NULL;
    int    nTest;
    BYTE   status;

    fp = tfopen( "../../qos/test/results/slqsQosGetFlowStatus.txt", "w" );

    /* Define the test cases to be executed */
    const SLQSQosGetFlowStatusTestCase_t tc[] =
    {
        { eQCWWAN_ERR_INVALID_ARG,           1, NULL,
                                           "Null Response" },
        { eQCWWAN_ERR_NONE,                  2, &status,
                                           "All Valid" },
    };

    nTest = sizeof(tc)/sizeof(SLQSQosGetFlowStatusTestCase_t);
    if (fp)
    {
        int i;
        for(i=0;i<nTest;i++)
        {
            fprintf( fp, "\n\nTest Case %d / %d : %s\n",
                       (i + 1),
                       nTest,
                       tc[i].desc );

            /* Call the API */
            nRet = SLQSQosGetFlowStatus ( instance, g_qos_id, tc[i].pStatus);

            /* Display result code and text */
            doprintreason ( fp, nRet );

            if ( eQCWWAN_ERR_NONE == nRet )
            {
                IFPRINTF( fp,"Qos Flow status: %d\n", tc[i].pStatus);
            }
        }
        tfclose(fp);
    }
}

void doSLQSQosGetGranted( void )
{
    BYTE   instance = 0;
    ULONG  nRet;
    FILE   *fp = NULL;
    int    nTest;

    ULONG   latency, jitter;
    BYTE    lteQci;
    dataRate rate;
    swiQosGranted   granted;
    swiQosFlow      txFlow;
    memset(&granted, 0, sizeof(granted));
    memset(&txFlow, 0, sizeof(txFlow));
    txFlow.pLatency = &latency;
    txFlow.pJitter = &jitter;
    txFlow.pLteQci = &lteQci;
    txFlow.pDataRate = &rate;
    granted.pTxFlow = &txFlow;

    fp = tfopen( "../../qos/test/results/slqsQosGetGranted.txt", "w" );

    /* Define the test cases to be executed */
    const SLQSQosGetGrantedTestCase_t tc[] =
    {
        { eQCWWAN_ERR_INVALID_ARG,           1, NULL,
                                           "Null Response" },
        { eQCWWAN_ERR_NONE,                  2, &granted,
                                           "All Valid" },
    };

    nTest = sizeof(tc)/sizeof(SLQSQosGetGrantedTestCase_t);
    if (fp)
    {
        int i;
        for(i=0;i<nTest;i++)
        {
            fprintf( fp, "\n\nTest Case %d / %d : %s\n",
                       (i + 1),
                       nTest,
                       tc[i].desc );

            /* Call the API */
            nRet = SLQSQosGetGranted ( instance, g_qos_id, tc[i].pGranted);

            /* Display result code and text */
            doprintreason ( fp, nRet );

            if ( eQCWWAN_ERR_NONE == nRet )
            {
                fprintf(fp, "Qos Get Granted Tx flow index : %d\n",
                        tc[i].pGranted->pTxFlow->index);
                IFPRINTF(fp, "Qos Get Granted Tx flow latency : %lu\n",
                        tc[i].pGranted->pTxFlow->pLatency);
                IFPRINTF(fp, "Qos Get Granted Tx flow jitter : %lu\n",
                        tc[i].pGranted->pTxFlow->pJitter);
                IFPRINTF(fp, "Qos Get Granted Tx flow LTE QCI : %d\n",
                        tc[i].pGranted->pTxFlow->pLteQci);
                fprintf(fp, "Qos Get Granted Tx flow Max rate : %lu\n",
                        tc[i].pGranted->pTxFlow->pDataRate->dataRateMax);
                fprintf(fp, "Qos Get Granted Tx flow guaranteed rate : %lu\n",
                        tc[i].pGranted->pTxFlow->pDataRate->guaranteedRate);
            }
        }
        tfclose(fp);
    }
}

void doSLQSQosSuspend( void )
{
    BYTE      instance = 0;
    ULONG     nRet;
    FILE      *fp = NULL;
    int       nTest;
    swiQosIds qosIds;

    ULONG    ids[2] = {0x0001, 0x0002};
    qosIds.pIds = ids;
    qosIds.sz = sizeof(ids)/sizeof(ids[0]);

    fp = tfopen( "../../qos/test/results/slqsQosSuspend.txt", "w" );

    /* Define the test cases to be executed */
    const SLQSQosSuspendTestCase_t tc[] =
    {
        { eQCWWAN_ERR_INVALID_ARG, 1, NULL, "Invalid - Input parameters NULL" },
        { eQCWWAN_ERR_NONE, 2, &qosIds, "All Valid" },
    };

    nTest = sizeof(tc)/sizeof(SLQSQosSuspendTestCase_t);
    if (fp)
    {
        int i;
        for(i=0;i<nTest;i++)
        {
            fprintf( fp, "\n\nTest Case %d / %d : %s\n",
                       (i + 1),
                       nTest,
                       tc[i].desc );

            /* Call the API */
            nRet = SLQSQosSuspend( instance, tc[i].pQosIdList);
            /* Display result code and text */
            doprintreason ( fp, nRet );

            if ( eQCWWAN_ERR_NONE == nRet )
            {
                fprintf(fp, "Qos Suspend was sent OTA successfully\n");
            }
        }
        tfclose(fp);
    }
}

void doSLQSQosResume( void )
{
    BYTE      instance = 0;
    ULONG     nRet;
    FILE      *fp = NULL;
    int       nTest;
    swiQosIds qosIds;

    ULONG    ids[2] = {0x0001, 0x0002};
    qosIds.pIds = ids;
    qosIds.sz = sizeof(ids)/sizeof(ids[0]);

    fp = tfopen( "../../qos/test/results/slqsQosResume.txt", "w" );

    /* Define the test cases to be executed */
    const SLQSQosResumeTestCase_t tc[] =
    {
        { eQCWWAN_ERR_INVALID_ARG, 1, NULL, "Invalid - Input parameters NULL" },
        { eQCWWAN_ERR_NONE, 2, &qosIds, "All Valid" },
    };

    nTest = sizeof(tc)/sizeof(SLQSQosResumeTestCase_t);
    if (fp)
    {
        int i;
        for(i=0;i<nTest;i++)
        {
            fprintf( fp, "\n\nTest Case %d / %d : %s\n",
                       (i + 1),
                       nTest,
                       tc[i].desc );

            /* Call the API */
            nRet = SLQSQosResume( instance, tc[i].pQosIdList);
            /* Display result code and text */
            doprintreason ( fp, nRet );

            if ( eQCWWAN_ERR_NONE == nRet )
            {
                fprintf(fp, "Qos Resume was sent OTA successfully\n");
            }
        }
        tfclose(fp);
    }
}

void doSLQSQosReset( void )
{
    BYTE   instance = 0;
    ULONG  nRet;
    FILE   *fp = NULL;
    int    nTest;

    fp = tfopen( "../../qos/test/results/slqsQosReset.txt", "w" );

    /* Define the test cases to be executed */
    const SLQSQosResetTestCase_t tc[] =
    {
        { eQCWWAN_ERR_NONE, 1, "All Valid" }
    };

    nTest = sizeof(tc)/sizeof(SLQSQosResetTestCase_t);
    if (fp)
    {
        int i;
        for(i=0;i<nTest;i++)
        {
            fprintf( fp, "\n\nTest Case %d / %d : %s\n",
                       (i + 1),
                       nTest,
                       tc[i].desc );

            /* Call the API */
            nRet = SLQSQosReset ( instance );

            /* Display result code and text */
            doprintreason ( fp, nRet );

        }
        tfclose(fp);
    }
}

void doSLQSQosGetNWProf( void )
{
    BYTE   instance = 0;
    ULONG  nRet;
    FILE   *fp = NULL;
    int    nTest;
    BYTE   nProfile;
    NWProfile lNWProfile[2];
    WORD   ProfileList1[5];
    WORD   ProfileList2[5];
    BYTE   ProfSz1, ProfSz2;

    nProfile = sizeof(lNWProfile)/sizeof(NWProfile);
    ProfSz1 = sizeof(ProfileList1)/sizeof(WORD);
    lNWProfile[0].pProfSz = &ProfSz1;
    lNWProfile[0].pProfValues = ProfileList1;
    ProfSz2 = sizeof(ProfileList2)/sizeof(WORD);
    lNWProfile[1].pProfSz = &ProfSz2;
    lNWProfile[1].pProfValues = ProfileList2;

    fp = tfopen( "../../qos/test/results/slqsQosGetNWProf.txt", "w" );

    /* Define the test cases to be executed */
    const SLQSQosGetNWProfTestCase_t tc[] =
    {
        { eQCWWAN_ERR_INVALID_ARG,  1, "Null Response", NULL,       NULL },
        { eQCWWAN_ERR_NONE,         2, "All Valid",     &nProfile,  lNWProfile },
    };

    nTest = sizeof(tc)/sizeof(SLQSQosGetNWProfTestCase_t);
    if (fp)
    {
        BYTE i,j,k;
        for(i=0;i<nTest;i++)
        {
            fprintf( fp, "\n\nTest Case %d / %d : %s\n",
                       (i + 1),
                       nTest,
                       tc[i].desc );

            /* Call the API */
            nRet = SLQSQosGetNWProf ( instance, tc[i].pSz, tc[i].pNWProfile);

            /* Display result code and text */
            doprintreason ( fp, nRet );

            if ( eQCWWAN_ERR_NONE == nRet )
            {
                fprintf( fp,"Qos Network Profile:\n" );
                NWProfile *pProfile =  tc[i].pNWProfile;
                for(j=0; j < *tc[i].pSz; j++)
                {
                    fprintf( fp,"  [%d]: Technology: %d\n", j, pProfile[j].tech);
                    fprintf( fp,"  [%d]: Profiles:\n", j);
                    for(k=0; k < *pProfile[j].pProfSz; k++)
                    {
                        fprintf( fp,"    [%d]: Profile[%d] value: %d\n",
                                j, k,  pProfile[j].pProfValues[k]);
                    }
                }
            }
        }
        tfclose(fp);
    }
}
