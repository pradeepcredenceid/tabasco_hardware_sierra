/**
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 */ 

#include <syslog.h>
#include "SwiDataTypes.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaQmiBasic.h"
#include "qaQosGetNWProf.h"

//TODO provide common function for zero TLV pack case
enum eQCWWANError PkQmiQosGetNWProf (
    WORD    *pMlength,
    BYTE    *pParamField )
{
    /* There is no mandatory TLV and hence the map is empty */
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_TYPE_INVALID, NULL }  /* Important. Sentinel.
                                      * Signifies last item in map.
                                      */
    };

    enum eQCWWANError eRCode;

    eRCode = qmbuild( pParamField,
                      NULL,
                      map,
                      eQMI_QOS_GET_NW_PROFILES,
                      pMlength );

    return eRCode;
}

enum eQCWWANError UnpackTlvNWProf(
    BYTE* pTlvData,
    BYTE *pResp )
{
    BYTE i,j;
    struct QmiQosGetNWProfResp *lResp =
              (struct QmiQosGetNWProfResp *)pResp;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE nInstance;

    eRCode = GetByte( pTlvData, &nInstance);
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        *lResp->pSz = 0;
        return eRCode;
    }
    
    if ( nInstance > *lResp->pSz )
    {
        *lResp->pSz = nInstance;
        return eQCWWAN_ERR_INVALID_ARG;
    }
    *lResp->pSz = nInstance;

    for(i=0; i<nInstance; i++)
    {
        BYTE nProf;
        eRCode = GetWord( pTlvData, &lResp->pProfile[i].tech);
        if (eQCWWAN_ERR_NONE != eRCode)
        {
            lResp->pProfile[i].tech = ~0;
            return eRCode;
        }

        //TODO same error check as nInstance
        eRCode = GetByte( pTlvData, &nProf);
        if (eQCWWAN_ERR_NONE != eRCode)
        {
            *lResp->pProfile[i].pProfSz = 0;
            return eRCode;
        }

        if ( nProf > *lResp->pProfile[i].pProfSz )
        {
            *lResp->pProfile[i].pProfSz = nProf;
            return eQCWWAN_ERR_INVALID_ARG;
        }
        *lResp->pProfile[i].pProfSz = nProf;

        for(j=0; j<nProf; j++)
        {
            eRCode = GetWord( pTlvData, &lResp->pProfile[i].pProfValues[j]);
            if (eQCWWAN_ERR_NONE != eRCode)
            {
                lResp->pProfile[i].pProfValues[j] = ~0;
                return eRCode;
            }
        }
    }

    return eRCode;
}

enum eQCWWANError UnpackTlvExtErr(
    BYTE* pTlvData,
    BYTE *pResp )
{
    UNUSEDPARAM(pResp);
    WORD ExtErr;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    eRCode = GetWord( pTlvData, &ExtErr);
    syslog( LOG_DEBUG,
            "%s::%d: ExtErr 0x%x\n",
            __func__,
            __LINE__,
            ExtErr);

    return eRCode;
}

enum eQCWWANError UpkQmiQosGetNWProf(
    BYTE    *pMdmResp,
    struct  QmiQosGetNWProfResp *pApiResp )
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,             &qmUnpackTlvResultCode },
        { eTLV_NW_PROF,                 &UnpackTlvNWProf },
        { eTLV_EXT_ERR,                 &UnpackTlvExtErr },
        { eTLV_TYPE_INVALID,            NULL }  /* Important. Sentinel.
                                                 * Signifies last item in map.
                                                 */
    };

    eRCode = qmunpackresp( pMdmResp,
                          (BYTE*)pApiResp,
                          map,
                          eQMI_QOS_GET_NW_PROFILES );
    return eRCode;
}

