/*
 * \ingroup cbk
 *
 * \file qaCbkLocPositionRptInd.c
 *
 * \brief Contains UnPacking routines for the
 *        QMI_LOC_EVENT_POSITION_REPORT_IND message.
 *
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */

#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "sludefs.h"
#include "qaCbkLocPositionRptInd.h"


static    ULONGLONG               latitude;
static    ULONGLONG               longitude;
static    ULONG                   horUncCircular;
static    ULONG                   horUncEllipseSemiMinor;
static    ULONG                   horUncEllipseSemiMajor;
static    ULONG                   horUncEllipseOrientAzimuth;
static    BYTE                    horConfidence;
static    ULONG                   horReliability;
static    ULONG                   speedHorizontal;
static    ULONG                   speedUnc;
static    ULONG                   altitudeWrtEllipsoid;
static    ULONG                   altitudeWrtMeanSeaLevel;
static    ULONG                   vertUnc;
static    BYTE                    vertConfidence;
static    ULONG                   vertReliability;
static    ULONG                   speedVertical;
static    ULONG                   heading;
static    ULONG                   headingUnc;
static    ULONG                   magneticDeviation;
static    ULONG                   technologyMask;
static    precisionDilution       sPrecisionDilution;
static    ULONGLONG               timestampUtc;
static    BYTE                    leapSeconds;
static    gpsTime                 sGpsTime;
static    ULONG                   timeUnc;
static    ULONG                   timeSrc;
static    sensorDataUsage         sSensorDataUsage;
static    ULONG                   fixId;
static    svUsedforFix            sSvUsedforFix;
static    BYTE                    altitudeAssumed;

/*
 * This function unpacks the Status TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvSessionStatus(
    BYTE *pTlvData,
    BYTE *pResp )
{
      QmiCbkLocPositionReportInd *lResp =
            ( QmiCbkLocPositionReportInd *)pResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Position Report TLV */
    eRCode = GetLong (pTlvData, &lResp->sessionStatus);

    return eRCode;
}
/*
 * This function unpacks the Status TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvSessionId(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocPositionReportInd *lResp =
          ( QmiCbkLocPositionReportInd *)pResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Position Report TLV */
    eRCode = GetByte (pTlvData, &lResp->sessionId);

    return eRCode;
}
/*
 * This function unpacks the Status TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvLatitude(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocPositionReportInd *lResp =
        ( QmiCbkLocPositionReportInd *)pResp;

    /* Allocate memory to pointer */
    lResp->pLatitude = &latitude;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Position Report TLV */
    eRCode = GetLongLong (pTlvData, lResp->pLatitude);

    return eRCode;
}
/*
 * This function unpacks the Status TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvLongitude(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocPositionReportInd *lResp =
        ( QmiCbkLocPositionReportInd *)pResp;

    /* Allocate memory to pointer */
    lResp->pLongitude = &longitude;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Position Report TLV */
    eRCode = GetLongLong (pTlvData, lResp->pLongitude);

    return eRCode;
}
/*
 * This function unpacks the Status TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvCirHorPosUnc(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocPositionReportInd *lResp =
        ( QmiCbkLocPositionReportInd *)pResp;

    /* Allocate memory to pointer */
    lResp->pHorUncCircular = &horUncCircular;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Position Report TLV */
    eRCode = GetLong (pTlvData, lResp->pHorUncCircular);

    return eRCode;
}
/*
 * This function unpacks the Status TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvHorEllipseUncMin(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocPositionReportInd *lResp =
        ( QmiCbkLocPositionReportInd *)pResp;

    /* Allocate memory to pointer */
    lResp->pHorUncEllipseSemiMinor = &horUncEllipseSemiMinor;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Position Report TLV */
    eRCode = GetLong (pTlvData, lResp->pHorUncEllipseSemiMinor);

    return eRCode;
}
/*
 * This function unpacks the Status TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvHorEllipseUncMaj(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocPositionReportInd *lResp =
        ( QmiCbkLocPositionReportInd *)pResp;

    /* Allocate memory to pointer */
    lResp->pHorUncEllipseSemiMajor = &horUncEllipseSemiMajor;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Position Report TLV */
    eRCode = GetLong (pTlvData, lResp->pHorUncEllipseSemiMajor);

    return eRCode;
}
/*
 * This function unpacks the Status TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvEllipseUncAzi(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocPositionReportInd *lResp =
        ( QmiCbkLocPositionReportInd *)pResp;

    /* Allocate memory to pointer */
    lResp->pHorUncEllipseOrientAzimuth = &horUncEllipseOrientAzimuth;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Position Report TLV */
    eRCode = GetLong (pTlvData, lResp->pHorUncEllipseOrientAzimuth);

    return eRCode;
}
/*
 * This function unpacks the Status TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvHorConfidence(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocPositionReportInd *lResp =
        ( QmiCbkLocPositionReportInd *)pResp;

    /* Allocate memory to pointer */
    lResp->pHorConfidence = &horConfidence;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Position Report TLV */
    eRCode = GetByte (pTlvData, lResp->pHorConfidence);

    return eRCode;
}
/*
 * This function unpacks the Status TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvHorReliability(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocPositionReportInd *lResp =
        ( QmiCbkLocPositionReportInd *)pResp;

    /* Allocate memory to pointer */
    lResp->pHorReliability = &horReliability;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Position Report TLV */
    eRCode = GetLong (pTlvData, lResp->pHorReliability);

    return eRCode;
}
/*
 * This function unpacks the Status TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvHorSpeed(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocPositionReportInd *lResp =
        ( QmiCbkLocPositionReportInd *)pResp;

    /* Allocate memory to pointer */
    lResp->pSpeedHorizontal = &speedHorizontal;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Position Report TLV */
    eRCode = GetLong (pTlvData, lResp->pSpeedHorizontal);

    return eRCode;
}
/*
 * This function unpacks the Status TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvSpeedUnc(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocPositionReportInd *lResp =
        ( QmiCbkLocPositionReportInd *)pResp;

    /* Allocate memory to pointer */
    lResp->pSpeedUnc = &speedUnc;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Position Report TLV */
    eRCode = GetLong (pTlvData, lResp->pSpeedUnc);

    return eRCode;
}
/*
 * This function unpacks the Status TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvAltitudeWrtEllipse(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocPositionReportInd *lResp =
        ( QmiCbkLocPositionReportInd *)pResp;

    /* Allocate memory to pointer */
    lResp->pAltitudeWrtEllipsoid = &altitudeWrtEllipsoid;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Position Report TLV */
    eRCode = GetLong (pTlvData, lResp->pAltitudeWrtEllipsoid);

    return eRCode;
}
/*
 * This function unpacks the Status TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvAltitudeWrtSeaLvl(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocPositionReportInd *lResp =
        ( QmiCbkLocPositionReportInd *)pResp;

    /* Allocate memory to pointer */
    lResp->pAltitudeWrtMeanSeaLevel = &altitudeWrtMeanSeaLevel;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Position Report TLV */
    eRCode = GetLong (pTlvData, lResp->pAltitudeWrtMeanSeaLevel);

    return eRCode;
}
/*
 * This function unpacks the Status TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvVertUnc(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocPositionReportInd *lResp =
        ( QmiCbkLocPositionReportInd *)pResp;

    /* Allocate memory to pointer */
    lResp->pVertUnc = &vertUnc;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Position Report TLV */
    eRCode = GetLong (pTlvData, lResp->pVertUnc);

    return eRCode;
}
/*
 * This function unpacks the Status TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvVertConfidence(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocPositionReportInd *lResp =
        ( QmiCbkLocPositionReportInd *)pResp;

    /* Allocate memory to pointer */
    lResp->pVertConfidence = &vertConfidence;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Position Report TLV */
    eRCode = GetByte (pTlvData, lResp->pVertConfidence);

    return eRCode;
}
/*
 * This function unpacks the Status TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvVertReliability(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocPositionReportInd *lResp =
        ( QmiCbkLocPositionReportInd *)pResp;

    /* Allocate memory to pointer */
    lResp->pVertReliability = &vertReliability;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Position Report TLV */
    eRCode = GetLong (pTlvData, lResp->pVertReliability);

    return eRCode;
}
/*
 * This function unpacks the Status TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvVertSpeed(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocPositionReportInd *lResp =
        ( QmiCbkLocPositionReportInd *)pResp;

    /* Allocate memory to pointer */
    lResp->pSpeedVertical = &speedVertical;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Position Report TLV */
    eRCode = GetLong (pTlvData, lResp->pSpeedVertical);

    return eRCode;
}
/*
 * This function unpacks the Status TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvHeading(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocPositionReportInd *lResp =
        ( QmiCbkLocPositionReportInd *)pResp;

    /* Allocate memory to pointer */
    lResp->pHeading = &heading;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Position Report TLV */
    eRCode = GetLong (pTlvData, lResp->pHeading);

    return eRCode;
}
/*
 * This function unpacks the Status TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvHeadingUnc(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocPositionReportInd *lResp =
        ( QmiCbkLocPositionReportInd *)pResp;

    /* Allocate memory to pointer */
    lResp->pHeadingUnc = &headingUnc;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Position Report TLV */
    eRCode = GetLong (pTlvData, lResp->pHeadingUnc);

    return eRCode;
}
/*
 * This function unpacks the Status TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvMagneticDeviation(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocPositionReportInd *lResp =
        ( QmiCbkLocPositionReportInd *)pResp;

    /* Allocate memory to pointer */
    lResp->pMagneticDeviation = &magneticDeviation;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Position Report TLV */
    eRCode = GetLong (pTlvData, lResp->pMagneticDeviation);

    return eRCode;
}
/*
 * This function unpacks the Status TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvTechMask(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocPositionReportInd *lResp =
        ( QmiCbkLocPositionReportInd *)pResp;

    /* Allocate memory to pointer */
    lResp->pTechnologyMask = &technologyMask;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Position Report TLV */
    eRCode = GetLong (pTlvData, lResp->pTechnologyMask);

    return eRCode;
}
/*
 * This function unpacks the Precision Dilution TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvDilutionPercision(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocPositionReportInd *lResp =
        ( QmiCbkLocPositionReportInd *)pResp;

    /* Allocate memory to pointer */
    lResp->pPrecisionDilution = &sPrecisionDilution;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Position Report TLV */
        eRCode = GetLong( pTlvData, &lResp->pPrecisionDilution->PDOP );
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
    /* Get the parameter from Position Report TLV */
        eRCode = GetLong( pTlvData, &lResp->pPrecisionDilution->HDOP );
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
             return eRCode;
        }
    /* Get the parameter from Position Report TLV */
        eRCode = GetLong( pTlvData, &lResp->pPrecisionDilution->VDOP );

        return eRCode;
}
/*
 * This function unpacks the Status TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvTimeStamp(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocPositionReportInd *lResp =
        ( QmiCbkLocPositionReportInd *)pResp;

    /* Allocate memory to pointer */
    lResp->pTimestampUtc = &timestampUtc;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Position Report TLV */
    eRCode = GetLongLong (pTlvData, lResp->pTimestampUtc);

    return eRCode;
}
/*
 * This function unpacks the Status TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvLeapSeconds(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocPositionReportInd *lResp =
        ( QmiCbkLocPositionReportInd *)pResp;

    /* Allocate memory to pointer */
    lResp->pLeapSeconds = &leapSeconds;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Position Report TLV */
    eRCode = GetByte (pTlvData, lResp->pLeapSeconds);

    return eRCode;
}
/*
 * This function unpacks the GPS Time  TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvGpsTime(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocPositionReportInd *lResp =
        ( QmiCbkLocPositionReportInd *)pResp;

    /* Allocate memory to pointer */
    lResp->pGpsTime = &sGpsTime;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Position Report TLV */
        eRCode = GetWord( pTlvData, &lResp->pGpsTime->gpsWeek );
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
    /* Get the parameter from Position Report TLV */
        eRCode = GetLong( pTlvData, &lResp->pGpsTime->gpsTimeOfWeekMs );

        return eRCode;
}
/*
 * This function unpacks the Status TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvTimeUnc(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocPositionReportInd *lResp =
        ( QmiCbkLocPositionReportInd *)pResp;

    /* Allocate memory to pointer */
    lResp->pTimeUnc = &timeUnc;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Position Report TLV */
    eRCode = GetLong (pTlvData, lResp->pTimeUnc);

    return eRCode;
}
/*
 * This function unpacks the Status TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvTimeSrc(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocPositionReportInd *lResp =
        ( QmiCbkLocPositionReportInd *)pResp;

    /* Allocate memory to pointer */
    lResp->pTimeSrc = &timeSrc;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Position Report TLV */
    eRCode = GetLong (pTlvData, lResp->pTimeSrc);

    return eRCode;
}
/*
 * This function unpacks the Sensor Data Usage  TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvSensorDataUsage(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocPositionReportInd *lResp =
        ( QmiCbkLocPositionReportInd *)pResp;

    /* Allocate memory to pointer */
    lResp->pSensorDataUsage = &sSensorDataUsage;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Position Report TLV */
        eRCode = GetLong( pTlvData, &lResp->pSensorDataUsage->usageMask );
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
    /* Get the parameter from Position Report TLV */
        eRCode = GetLong( pTlvData, &lResp->pSensorDataUsage->aidingIndicatorMask );

        return eRCode;
}
/*
 * This function unpacks the Status TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvFixId(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocPositionReportInd *lResp =
        ( QmiCbkLocPositionReportInd *)pResp;

    /* Allocate memory to pointer */
    lResp->pFixId = &fixId;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Position Report TLV */
    eRCode = GetLong (pTlvData, lResp->pFixId);

    return eRCode;
}
/*
 * This function unpacks the SVs List  TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvSvList(
    BYTE *pTlvData,
    BYTE *pResp )
{
    BYTE lCount = 0;
    QmiCbkLocPositionReportInd *lResp =
        ( QmiCbkLocPositionReportInd *)pResp;

    /* Allocate memory to pointer */
    lResp->pSvUsedforFix = &sSvUsedforFix;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Position Report TLV */
        eRCode = GetByte( pTlvData, &lResp->pSvUsedforFix->gnssSvUsedList_len );
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
        while( lCount < lResp->pSvUsedforFix->gnssSvUsedList_len )
        {
             /* Get the parameter from Position Report TLV */
             eRCode = GetWord( pTlvData, &lResp->pSvUsedforFix->gnssSvUsedList[lCount] );
             if( eQCWWAN_ERR_NONE != eRCode )
             {
                  return eRCode;
              }
              lCount++;
         }
return eRCode;
}
/*
 * This function unpacks the Status TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvAltitudeAssumed(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocPositionReportInd *lResp =
        ( QmiCbkLocPositionReportInd *)pResp;

    /* Allocate memory to pointer */
    lResp->pAltitudeAssumed = &altitudeAssumed;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Position Report TLV */
    eRCode = GetByte (pTlvData, lResp->pAltitudeAssumed);

    return eRCode;
}
/*
 * This function unpacks the Position Report Indication message to a
 * user-provided response structure.
 *
 * \param  pMdmResp - Pointer to packed response from the modem.
 *
 * \param  pApiResp - Pointer to storage to unpack into.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UpkQmiCbkLocPositionRptInd(
        BYTE                              *pMdmResp,
        QmiCbkLocPositionReportInd        *pApiResp )
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
            { eTLV_IND_SESSION_STATUS,            &UnpackCbkTlvSessionStatus },
            { eTLV_IND_SESSION_ID,                &UnpackCbkTlvSessionId },
            { eTLV_IND_LATITUDE,                  &UnpackCbkTlvLatitude },
            { eTLV_IND_LONGITUDE,                 &UnpackCbkTlvLongitude },
            { eTLV_IND_CIR_HOR_POS_UNC,           &UnpackCbkTlvCirHorPosUnc },
            { eTLV_IND_HOR_ELLIPSE_UNC_MIN,       &UnpackCbkTlvHorEllipseUncMin },
            { eTLV_IND_HOR_ELLIPSE_UNC_MAJ,       &UnpackCbkTlvHorEllipseUncMaj },
            { eTLV_IND_HOR_ELLIPSE_UNC_AZI,       &UnpackCbkTlvEllipseUncAzi },
            { eTLV_IND_HOR_CONFIDENCE,            &UnpackCbkTlvHorConfidence },
            { eTLV_IND_HOR_RELIABILITY,           &UnpackCbkTlvHorReliability },
            { eTLV_IND_HOR_SPEED,                 &UnpackCbkTlvHorSpeed },
            { eTLV_IND_SPEED_UNC,                 &UnpackCbkTlvSpeedUnc },
            { eTLV_IND_ALTITUDE_WRT_ELLIPSE,      &UnpackCbkTlvAltitudeWrtEllipse },
            { eTLV_IND_ALTITUDE_WRT_SEA_LVL,      &UnpackCbkTlvAltitudeWrtSeaLvl },
            { eTLV_IND_VERT_UNC,                  &UnpackCbkTlvVertUnc },
            { eTLV_IND_VERT_CONFIDENCE,           &UnpackCbkTlvVertConfidence },
            { eTLV_IND_VERT_RELIABILITY,          &UnpackCbkTlvVertReliability },
            { eTLV_IND_VERT_SPEED,                &UnpackCbkTlvVertSpeed },
            { eTLV_IND_HEADING,                   &UnpackCbkTlvHeading },
            { eTLV_IND_HEADING_UNC,               &UnpackCbkTlvHeadingUnc },
            { eTLV_IND_MAGNETIC_DEVIATION,        &UnpackCbkTlvMagneticDeviation },
            { eTLV_IND_TECH_MASK,                 &UnpackCbkTlvTechMask },
            { eTLV_IND_DILUTION_PERCISION,        &UnpackCbkTlvDilutionPercision },
            { eTLV_IND_UTC_TIME_STAMP,            &UnpackCbkTlvTimeStamp },
            { eTLV_IND_LEAP_SECONDS,              &UnpackCbkTlvLeapSeconds },
            { eTLV_IND_GPS_TIME,                  &UnpackCbkTlvGpsTime },
            { eTLV_IND_TIME_UNC,                  &UnpackCbkTlvTimeUnc },
            { eTLV_IND_TIME_SRC,                  &UnpackCbkTlvTimeSrc },
            { eTLV_IND_SENSOR_DATA_USAGE,         &UnpackCbkTlvSensorDataUsage },
            { eTLV_IND_FIX_ID,                    &UnpackCbkTlvFixId },
            { eTLV_IND_SV_LIST,                   &UnpackCbkTlvSvList },
            { eTLV_IND_ALTITUDE_ASSUMED,          &UnpackCbkTlvAltitudeAssumed },
            { eTLV_TYPE_INVALID,                  NULL } /* Important. Sentinel.
                                                      * Signifies last item in map.
                                                      */
    };

    eRCode = qmunpackresp( pMdmResp,
                           (BYTE *)pApiResp,
                           map,
                           eQMI_LOC_EVENT_POSITION_REPORT_IND );
    return eRCode;
}
