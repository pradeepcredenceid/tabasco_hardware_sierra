/*
 * \ingroup qos
 *
 * \file qaQosResume.c
 *
 * \brief  Contains Packing and UnPacking routines for the
 *   QMI_QOS_RESUME_QOS message.
 *
 * Copyright: © 2011-2013 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */

#include "SwiDataTypes.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaQmiBasic.h"

#include "qaQosCommon.h"
#include "qaQosResume.h"

/*****************************************************************************
 * Local Variables
 ******************************************************************************/

/*****************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the QoS identifier lists parameters
 * to the QMI message SDU
 *
 * \param    pParamField [OUT] - Pointer to storage into which the packed
 *                                    data will be placed by this function.
 *
 * \param    pMlength    [OUT] - Total length of built message.
 *
 * \return   eQCWWAN_ERR_NONE     - Increment successful
 * \return   eQCWWAN_ERR_MEMORY    - Increment beyond allowed size attempted
 *
 */
enum eQCWWANError PkQmiQosResume (
    WORD    *pMlength,
    BYTE    *pParamField,
    swiQosIds   *pQosIds)
{
    /* There is no mandatory TLV and hence the map is empty */
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_QOS_RESUME_QOS, &mBuildTlvQosIds},
        { eTLV_TYPE_INVALID, NULL }  /* Important. Sentinel.
                                      * Signifies last item in map.
                                      */
    };

    enum eQCWWANError eRCode;

    eRCode = qmbuild( pParamField,
                      (BYTE*)pQosIds,
                      map,
                      eQMI_QOS_RESUME,
                      pMlength );

    return eRCode;
}

/*****************************************************************************
 * Response handling
 ******************************************************************************/

/*
 * This function unpacks the Get Model ID response message to to a
 * user-provided response structure.
 *
 * \param   pMdmResp - Pointer to packed response from the modem.
 *
 * \param   pApiResp - Pointer to storage to unpack into.
 *
 * \return  eQCWWAN_ERR_NONE, on success
 */
enum eQCWWANError UpkQmiQosResume(
    BYTE    *pMdmResp,
    struct  QmiQosResumeResp *pApiResp )
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,             &qmUnpackTlvResultCode },
        { eTLV_TYPE_INVALID,            NULL }  /* Important. Sentinel.
                                                 * Signifies last item in map.
                                                 */
    };

    eRCode = qmunpackresp( pMdmResp,
                          (BYTE*)pApiResp,
                          map,
                          eQMI_QOS_RESUME );
    return eRCode;
}

