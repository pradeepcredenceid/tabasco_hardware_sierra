/**************
 *
 *  Filename: qaGobiApiQosTest.h
 *
 *  Purpose:  Contains prototypes for functions implementing specific
 *            tests for Quality of Service (QOS)
 *            called by the main() routine in qatest.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc., all rights reserved
 *
 **************/

#ifndef __GOBI_API_QOS_TEST_H__
#define __GOBI_API_QOS_TEST_H__

/* Include Files */

/* Defines */

/* Enums */

/* Structure for test cases step */
typedef struct
{
    enum eQCWWANError       expectedReturnValue;
    ULONG                   testCaseId;
    swiQosReq               *pReq;
    swiQosIds               *pIds;
    CHAR                    desc[100];
} SLQSQosReqTestCase_t;

typedef struct
{
    enum eQCWWANError       expectedReturnValue;
    ULONG                   testCaseId;
    swiQosIds               *pIds;
    CHAR                    desc[100];
} SLQSQosRelTestCase_t;

typedef struct
{
    enum eQCWWANError       expectedReturnValue;
    ULONG                   testCaseId;
    BYTE                    *pStatus;
    CHAR                    desc[100];
} SLQSQosGetNetworkStatusTestCase_t;

typedef struct
{
    enum eQCWWANError       expectedReturnValue;
    ULONG                   testCaseId;
    BYTE                    *pStatus;
    CHAR                    desc[100];
} SLQSQosGetFlowStatusTestCase_t;

typedef struct
{
    enum eQCWWANError       expectedReturnValue;
    ULONG                   testCaseId;
    swiQosGranted           *pGranted;
    CHAR                    desc[100];
} SLQSQosGetGrantedTestCase_t;

typedef struct
{
    enum eQCWWANError       expectedReturnValue;
    ULONG                   testCaseId;
    swiQosIds               *pQosIdList;
    CHAR                    desc[100];
} SLQSQosSuspendTestCase_t;

typedef struct
{
    enum eQCWWANError       expectedReturnValue;
    ULONG                   testCaseId;
    swiQosIds               *pQosIdList;
    CHAR                    desc[100];
} SLQSQosResumeTestCase_t;

typedef struct
{
    enum eQCWWANError       expectedReturnValue;
    ULONG                   testCaseId;
    CHAR                    desc[100];
} SLQSQosResetTestCase_t;

typedef struct
{
    enum eQCWWANError       expectedReturnValue;
    ULONG                   testCaseId;
    CHAR                    desc[100];
    BYTE                    *pSz;
    NWProfile               *pNWProfile;
} SLQSQosGetNWProfTestCase_t;

#endif /* __GOBI_API_QOS_TEST_H__ */
