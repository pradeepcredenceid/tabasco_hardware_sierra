/*
 * \ingroup : loc
 *
 * \file    : qaSlqsLocInjectPosition.c
 *
 * \brief   : Contains Packing and UnPacking routines for the
 *            eQMI_LOC_INJECT_POSITION message.
 *
 * Copyright: © 2011-2016 Sierra Wireless, Inc. all rights reserved
 */

#include "SwiDataTypes.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaQmiBasic.h"

#include "qaSlqsLocInjectPosition.h"

/******************************************************************************
 * Request handling
 ******************************************************************************/
/*
 * This function packs the Latitude value field to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                 data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
 */
enum eQCWWANError BuildTlvInjectLatitude( BYTE *pBuf, BYTE *pParam )
{
    LocInjectPositionReq *pReq =
        (LocInjectPositionReq *)pParam;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    if ( pReq->pLatitude == NULL )
        return eQCWWAN_ERR_NONE;

    eRCode = Put64( pBuf, *pReq->pLatitude);
    return eRCode;
}

/*
 * This function packs the Longitude value field to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                 data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
 */
enum eQCWWANError BuildTlvInjectLongitude( BYTE *pBuf, BYTE *pParam )
{
    LocInjectPositionReq *pReq =
        (LocInjectPositionReq *)pParam;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    if ( pReq->pLongitude == NULL )
        return eQCWWAN_ERR_NONE;

    eRCode = Put64( pBuf, *pReq->pLongitude);
    return eRCode;
}

/*
 * This function packs the Horizontal Circular Uncertainty field to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                 data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
*/
enum eQCWWANError BuildTlvInjectCirHorUnc( BYTE *pBuf, BYTE *pParam )
{
    LocInjectPositionReq *pReq =
        (LocInjectPositionReq *)pParam;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    if ( pReq->pHorUncCircular == NULL )
        return eQCWWAN_ERR_NONE;

    eRCode = PutLong( pBuf, *pReq->pHorUncCircular);
    return eRCode;
}

/*
 * This function packs the Horizontal Confidence field to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                 data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
*/
enum eQCWWANError BuildTlvInjectHorConf( BYTE *pBuf, BYTE *pParam )
{
    LocInjectPositionReq *pReq =
        (LocInjectPositionReq *)pParam;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    if ( pReq->pHorConfidence == NULL )
        return eQCWWAN_ERR_NONE;

    eRCode = PutByte( pBuf, *pReq->pHorConfidence);
    return eRCode;
}

/*
 * This function packs the Horizontal Reliability field to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                 data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
*/
enum eQCWWANError BuildTlvInjectHorRel( BYTE *pBuf, BYTE *pParam )
{
    LocInjectPositionReq *pReq =
        (LocInjectPositionReq *)pParam;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    if ( pReq->pHorReliability == NULL )
        return eQCWWAN_ERR_NONE;

    eRCode = PutLong( pBuf, *pReq->pHorReliability);
    return eRCode;
}

/*
 * This function packs the Altitude wrt Ellipsoid field to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                 data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
*/
enum eQCWWANError BuildTlvInjectAltWrtEllips( BYTE *pBuf, BYTE *pParam )
{
    LocInjectPositionReq *pReq =
        (LocInjectPositionReq *)pParam;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    if ( pReq->pAltitudeWrtEllipsoid == NULL )
        return eQCWWAN_ERR_NONE;

    eRCode = PutLong( pBuf, *pReq->pAltitudeWrtEllipsoid);
    return eRCode;
}

/*
 * This function packs the Altitude wrt Mean Sea Level field to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                 data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
*/
enum eQCWWANError BuildTlvInjectAltWrtMeanSeaLvl( BYTE *pBuf, BYTE *pParam )
{
    LocInjectPositionReq *pReq =
        (LocInjectPositionReq *)pParam;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    if ( pReq->pAltitudeWrtMeanSeaLevel == NULL )
        return eQCWWAN_ERR_NONE;

    eRCode = PutLong( pBuf, *pReq->pAltitudeWrtMeanSeaLevel);
    return eRCode;
}

/*
 * This function packs the Vertical Uncertainty field to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                 data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
*/
enum eQCWWANError BuildInjectVertUnc( BYTE *pBuf, BYTE *pParam )
{
    LocInjectPositionReq *pReq =
        (LocInjectPositionReq *)pParam;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    if ( pReq->pVertUnc == NULL )
        return eQCWWAN_ERR_NONE;

    eRCode = PutLong( pBuf, *pReq->pVertUnc);
    return eRCode;
}

/*
 * This function packs the Vertical Confidence field to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                 data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
*/
enum eQCWWANError BuildTlvInjectVertConf( BYTE *pBuf, BYTE *pParam )
{
    LocInjectPositionReq *pReq =
        (LocInjectPositionReq *)pParam;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    if ( pReq->pVertConfidence == NULL )
        return eQCWWAN_ERR_NONE;

    eRCode = PutByte( pBuf, *pReq->pVertConfidence);
    return eRCode;
}

/*
 * This function packs the Vertical Reliability field to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                 data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
*/
enum eQCWWANError BuildTlvInjectVertRel( BYTE *pBuf, BYTE *pParam )
{
    LocInjectPositionReq *pReq =
        (LocInjectPositionReq *)pParam;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    if ( pReq->pVertReliability == NULL )
        return eQCWWAN_ERR_NONE;

    eRCode = PutLong( pBuf, *pReq->pVertReliability);
    return eRCode;
}

/*
 * This function packs the Altitude Source Info field to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                 data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
*/
enum eQCWWANError BuildTlvInjectAltSrcInfo( BYTE *pBuf, BYTE *pParam )
{
    LocInjectPositionReq *pReq =
        (LocInjectPositionReq *)pParam;

    altitudeSrcInfo *pRSReq = pReq->pAltitudeSrcInfo;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    if ( pRSReq == NULL )
        return eQCWWAN_ERR_NONE;

    eRCode = PutLong( pBuf, pRSReq->source);
    if( eQCWWAN_ERR_NONE != eRCode )
        return eRCode;

    eRCode = PutLong( pBuf, pRSReq->linkage);
    if( eQCWWAN_ERR_NONE != eRCode )
        return eRCode;

    eRCode = PutLong( pBuf, pRSReq->coverage);

    return eRCode;
}

/*
 * This function packs the UTC time stanp field to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                 data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
*/
enum eQCWWANError BuildTlvInjectUtcTimestamp( BYTE *pBuf, BYTE *pParam )
{
    LocInjectPositionReq *pReq =
        (LocInjectPositionReq *)pParam;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    if ( pReq->pTimestampUtc == NULL )
        return eQCWWAN_ERR_NONE;

    eRCode = Put64( pBuf, *pReq->pTimestampUtc);
    return eRCode;
}

/*
 * This function packs the Position Age field to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                 data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
*/
enum eQCWWANError BuildTlvInjectPositionAge( BYTE *pBuf, BYTE *pParam )
{
    LocInjectPositionReq *pReq =
        (LocInjectPositionReq *)pParam;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    if ( pReq->pTimestampAge == NULL )
        return eQCWWAN_ERR_NONE;

    eRCode = PutLong( pBuf, *pReq->pTimestampAge);
    return eRCode;
}

/*
 * This function packs the Position Source field to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                 data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
*/
enum eQCWWANError BuildTlvInjectPositionSrc( BYTE *pBuf, BYTE *pParam )
{
    LocInjectPositionReq *pReq =
        (LocInjectPositionReq *)pParam;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    if ( pReq->pPositionSrc == NULL )
        return eQCWWAN_ERR_NONE;

    eRCode = PutLong( pBuf, *pReq->pPositionSrc);
    return eRCode;
}

/*
 * This function packs the Raw Horizontal Uncertainty Circular Source field to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                 data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
*/
enum eQCWWANError BuildTlvInjectRawHorUncCir( BYTE *pBuf, BYTE *pParam )
{
    LocInjectPositionReq *pReq =
        (LocInjectPositionReq *)pParam;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    if ( pReq->pRawHorUncCircular == NULL )
        return eQCWWAN_ERR_NONE;

    eRCode = PutLong( pBuf, *pReq->pRawHorUncCircular);
    return eRCode;
}

/*
 * This function packs the Raw Horizontal Confidence Source field to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                 data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
*/
enum eQCWWANError BuildTlvInjectRawHorConf( BYTE *pBuf, BYTE *pParam )
{
    LocInjectPositionReq *pReq =
        (LocInjectPositionReq *)pParam;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    if ( pReq->pRawHorConfidence == NULL )
        return eQCWWAN_ERR_NONE;

    eRCode = PutByte( pBuf, *pReq->pRawHorConfidence);
    return eRCode;
}

/*
 * Packs the PkQmiLocInjectUTCTime parameters to the QMI message SDU
 *
 * \param  pParamField[OUT]          - Pointer to storage into which the packed
 *                                     data will be placed by this function.
 *
 * \param  pMlength[OUT]             - Total length of built message.
 *
 * \param  pLocInjectPositionReq[IN] - Inject Position Req
 *
 * \return  eQCWWAN_ERR_NONE on success, eQCWWAN_ERR_XXX on error
 *
 */

enum eQCWWANError PkQmiLocInjectPosition(
    WORD                    *pMlength,
    BYTE                    *pParamField,
    LocInjectPositionReq    *pLocInjectPositionReq )
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_INJECT_LATITUDE_DATA,               &BuildTlvInjectLatitude },
        { eTLV_INJECT_LONGITUDE_DATA,              &BuildTlvInjectLongitude },
        { eTLV_INJECT_CIR_HOR_UNC,                 &BuildTlvInjectCirHorUnc },
        { eTLV_INJECT_HOR_CONFIDENCE,              &BuildTlvInjectHorConf },
        { eTLV_INJECT_HOR_RELIABILITY,             &BuildTlvInjectHorRel },
        { eTLV_INJECT_ALTITUDE_WRT_ELLIPSOID,      &BuildTlvInjectAltWrtEllips },
        { eTLV_INJECT_ALTITUDE_WRT_MEAN_SEA_LVL,   &BuildTlvInjectAltWrtMeanSeaLvl },
        { eTLV_INJECT_VERT_UNCERTAINTY,            &BuildInjectVertUnc },
        { eTLV_INJECT_VERT_CONFIDENCE,             &BuildTlvInjectVertConf },
        { eTLV_INJECT_VERT_RELIABILITY,            &BuildTlvInjectVertRel },
        { eTLV_INJECT_ALTITUDE_SOURCE_INFO,        &BuildTlvInjectAltSrcInfo },
        { eTLV_INJECT_UTC_TIMESTAMP,               &BuildTlvInjectUtcTimestamp },
        { eTLV_INJECT_POSITION_AGE,                &BuildTlvInjectPositionAge },
        { eTLV_INJECT_POSITION_SOURCE,             &BuildTlvInjectPositionSrc },
        { eTLV_INJECT_RAW_HOR_UNC_CIRCULAR,        &BuildTlvInjectRawHorUncCir },
        { eTLV_INJECT_RAW_HOR_CONFIDENCE,          &BuildTlvInjectRawHorConf },
        { eTLV_TYPE_INVALID, NULL } /* Important. Sentinel.
                                     * Signifies last item in map
                                     */
    };

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    eRCode = qmbuild( pParamField,
                      (BYTE *)pLocInjectPositionReq,
                      map,
                      eQMI_LOC_INJECT_POSITION,
                      pMlength );
    return eRCode;
}

/******************************************************************************
 * Response handling
 ******************************************************************************

 * This function unpacks the eQMI_LOC_INJECT_POSITION response message
 * to a user-provided response structure.
 *
 * \param  pMdmResp  [IN]  - Pointer to packed response from the modem.
 *
 * \param  pApiResp  [OUT] - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE  - on success;  eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError UpkQmiLocInjectPosition(
                  BYTE                              *pMdmResp,
                  struct QmiLocInjectPositionResp   *pApiResp)
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,  &qmUnpackTlvResultCode },
        { eTLV_TYPE_INVALID, NULL } /* Important. Sentinel.
                                     * Signifies last item in map.
                                     */
    };

    eRCode = qmunpackresp( pMdmResp,
                           (BYTE *)pApiResp,
                           map,
                           eQMI_LOC_INJECT_POSITION);
    return eRCode;
}
