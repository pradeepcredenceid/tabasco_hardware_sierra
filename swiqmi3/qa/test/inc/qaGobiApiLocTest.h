/**************
 *
 *  Filename: qaGobiApiLocTest.h
 *
 *  Purpose:  Contains prototypes for functions implementing specific tests
 *            for LOC called by the main() routine in qatest.c
 *
 * Copyright: © 2015 Sierra Wireless, Inc., all rights reserved
 *
 **************/

#ifndef __GOBI_API_LOC_TEST_H__
#define __GOBI_API_LOC_TEST_H__

typedef struct _SLQSLocEventRegisterTestCase_t
{
    enum eQCWWANError expectedReturnValue;
    ULONG             testCaseId;
    ULONGLONG         eventRegister;
    CHAR              desc[100];
} SLQSLocEventRegisterTestCase_t;

typedef struct _SLQSLocSetExtPowerTestCase_t
{
    enum eQCWWANError expectedReturnValue;
    ULONG             testCaseId;
    ULONG             extPowerState;
    CHAR              desc[100];
} SLQSLocSetExtPowerTestCase_t;

typedef struct _SLQSLocStartTestCase_t
{
    enum eQCWWANError expectedReturnValue;
    ULONG                     testCaseId;
    BYTE                      sessionId;
    ULONG                     *pRecurrenceType;
    ULONG                     *pHorizontalAccuracyLvl;
    ULONG                     *pIntermediateReportState;
    ULONG                     *pMinIntervalTime;
    struct LocApplicationInfo *pApplicationInfo;
    ULONG                     *pConfigAltitudeAssumed;
    CHAR                      desc[100];
} SLQSLocStartTestCase_t;

typedef struct _SLQSLocStopTestCase_t
{
    enum eQCWWANError expectedReturnValue;
    ULONG             testCaseId;
    BYTE              sessionId;
    CHAR              desc[100];
} SLQSLocStopTestCase_t;

typedef struct
{
    enum eQCWWANError expectedReturnValue;
    ULONG             mode;
    CHAR              desc[100];
} SLQSLocSetOpModeTestCase_t;

typedef struct
{
    enum eQCWWANError expectedReturnValue;
    ULONG             testCaseId;
    SVInfo            *pSVInfo;
    GnssData          *pGnssData;
    CellDb            *pCellDb;
    ClkInfo           *pClkInfo;
    BdsSVInfo         *pBdsSVInfo;
    CHAR              desc[100];
} SLQSLocDelAssDataTestCase_t;

typedef struct
{
    enum eQCWWANError expectedReturnValue;
    ULONG             testCaseId;
    CHAR              desc[100];
} SLQSLocInjectUTCTimeTestCase_t;

typedef struct
{
    enum eQCWWANError expectedReturnValue;
    ULONG             testCaseId;
    ULONG             serverType;
    BYTE              *pAddressType;
    CHAR              desc[100];
} SLQSLOCGetServerTestCase_t;

typedef struct
{
    enum eQCWWANError expectedReturnValue;
    ULONG             testCaseId;
    ULONG             serverType;
    IPv4Address       *pIPv4;
    IPv6Address       *pIPv6;
    UrlAddress        *pURL;
    CHAR              desc[100];
} SLQSLOCSetServerTestCase_t;

typedef struct
{
    enum eQCWWANError expectedReturnValue;
    ULONG             testCaseId;
    CHAR              desc[100];
} SLQSLOCGetOpModeTestCase_t;

typedef struct
{
    enum eQCWWANError expectedReturnValue;
    ULONG             testCaseId;
    CHAR              desc[100];
} SLQSLOCGetFixCriteriaTestCase_t;


typedef struct
{
    enum eQCWWANError expectedReturnValue;
    ULONG             testCaseId;
    ULONGLONG         *pLatitude;
    ULONGLONG         *pLongitude;
    ULONG             *pHorUncCircular;
    BYTE              *pHorConfidence;
    ULONG             *pHorReliability;
    ULONG             *pAltitudeWrtEllipsoid;
    ULONG             *pAltitudeWrtMeanSeaLevel;
    ULONG             *pVertUnc;
    BYTE              *pVertConfidence;
    ULONG             *pVertReliability;
    altitudeSrcInfo   *pAltitudeSrcInfo;
    ULONGLONG         *pTimestampUtc;
    ULONG             *pTimestampAge;
    ULONG             *pPositionSrc;
    ULONG             *pRawHorUncCircular;
    BYTE              *pRawHorConfidence;
    CHAR              desc[100];
} SLQSLocInjectPositionTestCase_t;

typedef struct _SLQSLOCSetCradleMountConfigTestCase_t
{
    enum eQCWWANError expectedReturnValue;
    ULONG             testCaseId;
    ULONG             state;
    BYTE              *pConfidence;
    CHAR              desc[100];
} SLQSLOCSetCradleMountConfigTestCase_t;

typedef struct _SLQSLOCInjectSensorDataTestCase_t
{
    enum eQCWWANError expectedReturnValue;
    ULONG             testCaseId;
    ULONG             *pOpaqueIdentifier;
    sensorData        *pAcceleroData;
    sensorData        *pGyroData;
    ULONG             *pAcceleroTimeSrc;
    ULONG             *pGyroTimeSrc;
    tempratureData    *pAcceleroTempData;
    tempratureData    *pGyroTempData;
    CHAR              desc[100];
} SLQSLOCInjectSensorDataTestCase_t;


#endif /* __GOBI_API_LOC_TEST_H__ */
