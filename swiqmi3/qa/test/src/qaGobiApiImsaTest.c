/**************
 *
 *  Filename: qaGobiApiImsaTest.c
 *
 *  Purpose:  Contains functions implementing specific tests for
 *            Ims Service called by the main() routine in qatest.c
 *
 * Copyright: © 2014 Sierra Wireless, Inc., all rights reserved
 *
 **************/

/* Linux definitions */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* include files */
#include "SwiDataTypes.h"
#include "qmerrno.h"

#include "qaGobiApiImsa.h"
#include "qaGobiApiImsaTest.h"
#include "qatesthelper.h"
#include "qatestproto.h"

/*******************
    TEST DATA
 *******************/

/* Constants */

/* Global Variables */

/* Indication Register Parameters */
local BYTE RegStatusInd = 0x01;
local BYTE SvcStatusInd = 0x01;
local BYTE RatStatusInd = 0x01;

/* Forward declaration */


/*******************
  TEST FUNCTIONS
 *******************/
/*
 * Name:    doSLQSRegisterIMSAIndication
 *
 * Purpose: Perform the tests that call the SLQSRegisterIMSAIndicationr() API
 *
 * Return:  none
 *
 * Notes:   none
 *
 */
void doSLQSRegisterIMSAIndication( void )
{
    /* Test Cases */
    SLQSIMSAIndRegisterTestCase_t SLQSIMSAIndRegisterTestCases[]=
    {
        { eQCWWAN_ERR_NONE, 1, NULL, NULL, NULL,
          "Error: At Least One Optional Parameter to be provided" },

        { eQCWWAN_ERR_NONE, 2, &RegStatusInd, NULL, NULL,
          "Set Reg Status Ind Event" },

        { eQCWWAN_ERR_NONE, 3, NULL, &SvcStatusInd, NULL, 
          "Set Svc Status Ind Event" },

        { eQCWWAN_ERR_NONE, 4, NULL, NULL, &RatStatusInd,
          "Set Rat Status Ind Event" },        

        { eQCWWAN_ERR_NONE, 5, &RegStatusInd, &SvcStatusInd, &RatStatusInd,
          "Set All" },
    };

    FILE  *fp = tfopen("../../imsa/test/results/slqsimsaindregister.txt", "w");

    IMSAIndRegisterInfo req;

    if (fp)
    {
        BYTE tc = 0;
        BYTE ncases = sizeof( SLQSIMSAIndRegisterTestCases )/
                      sizeof( SLQSIMSAIndRegisterTestCases[0] );
        while ( tc < ncases )
        {
            SLQSIMSAIndRegisterTestCase_t *pd =
                                    &SLQSIMSAIndRegisterTestCases[tc++];
            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        tc,
                        ncases,
                        pd->desc);

            req.pRegStatusConfig    = pd->pRegStatusConfig;
            req.pServiceStatusConfig = pd->pSvcStatusConfig;
            req.pRatHandoverStatusConfig    = pd->pRatStatusConfig;

            IFPRINTF(fp, "Reg Status Ind Event          : %x\n",
                                                 req.pRegStatusConfig);
            IFPRINTF(fp, "Svc Status Ind Event      : %x\n",
                                                 req.pServiceStatusConfig);
            IFPRINTF(fp, "Rat Status Ind Event          : %x\n",
                                                 req.pRatHandoverStatusConfig);

            ULONG nRet = SLQSRegisterIMSAIndication( &req );
            doprintreason( fp, nRet );

            if ( eQCWWAN_ERR_NONE == nRet )
            {
                fprintf(fp, "SLQSRegisterIMSAIndication Successful\n");
            }
        }
    }
    if (fp)
        tfclose(fp);
}

