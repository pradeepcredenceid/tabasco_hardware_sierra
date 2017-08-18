/**************
 *
 *  Filename: qaGobiApiImsaTest.h
 *
 *  Purpose:  Contains prototypes for functions implementing specific
 *            tests for Imsa Service
 *            called by the main() routine in qatest.c
 *
 * Copyright: © 2014 Sierra Wireless, Inc., all rights reserved
 *
 **************/

#ifndef __GOBI_API_IMSA_TEST_H__
#define __GOBI_API_IMSA_TEST_H__

typedef struct _SLQSIMSAIndRegisterTestCase_t
{
    enum eQCWWANError expectedReturnValue;
    ULONG             testCaseId;
    BYTE              *pRegStatusConfig;
    BYTE              *pSvcStatusConfig;
    BYTE              *pRatStatusConfig;
    CHAR              desc[100];
} SLQSIMSAIndRegisterTestCase_t;

#endif /* __GOBI_API_IMSA_TEST_H__ */
