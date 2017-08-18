/*
 * \ingroup swiaudio
 *
 * \file   qaAudioSLQSSetM2MAudioNVDef.c
 *
 * \brief  Contains Packing and UnPacking routines for the
 *         QMI_M2M_AUDIO_SET_NV_DEF_MSG message.
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */
#include "SwiDataTypes.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaQmiBasic.h"
#include "qaGobiApiSwiAudio.h"
#include "qaAudioSLQSSetM2MAudioNVDef.h"

/*****************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the SLQSSetM2MAudioNVDef parameters to the QMI message
 * SDU
 *
 * \param  pParamField [OUT] - Pointer to storage into which the packed
 *                             data will be placed by this function.
 *
 * \param  pMlength    [OUT] - Total length of built message.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError PkQmiAudioSLQSSetM2MAudioNVDef(
    WORD               *pMlength,
    BYTE               *pParamField )
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_TYPE_INVALID, NULL } /* Important. Sentinel.
                                     * Signifies last item in map
                                     */
    };

    enum   eQCWWANError                      eRCode;
    eRCode = qmbuild( pParamField,
                      NULL,
                      map,
                      eQMI_SWIAUDIO_SET_NV_DEF,
                      pMlength );
    return eRCode;
}

/*****************************************************************************
 * Response handling
 ******************************************************************************/

/*
 * This function unpacks the SLQSSetM2MAudioNVDef response message to a
 * user-provided response structure.
 *
 * \param     MdmResp   [IN]  - Pointer to packed response from the modem.
 *
 * \param     pApiResp  [OUT] - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UpkQmiAudioSLQSSetM2MAudioNVDef(
    BYTE                                   *pMdmResp,
    struct QmiAudioSLQSSetM2MAudioNVDefResp *pApiResp)
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_SET_M2M_AUDIO_NVDEF_EC, &qmUnpackTlvResultCode },
        { eTLV_TYPE_INVALID,           NULL } /* Important. Sentinel.
                                               * Signifies last item in map.
                                               */
    };
    eRCode = qmunpackresp( pMdmResp,
                           (BYTE*)pApiResp,
                           map,
                           eQMI_SWIAUDIO_SET_NV_DEF );
    return eRCode;
}
