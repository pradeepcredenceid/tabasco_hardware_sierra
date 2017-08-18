/*
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 *
 */

#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "sludefs.h"
#include "qaCbkLocGetServerInd.h"

static    IPv4Address     addressIPV4;
static    IPv6Address     addressIPV6;
static    UrlAddress      addressURL;
package enum eQCWWANError UnpackCbkTlvGetServerStatus(
        BYTE *pTlvData,
        BYTE *pResp )
{
    ULONG *lResp =
        &(( QmiCbkLocGetServerInd *)pResp)->status;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Extract the Server status TLV */
    eRCode = GetLong (pTlvData, lResp);

    return eRCode;
}

package enum eQCWWANError UnpackCbkTlvGetServerType(
        BYTE *pTlvData,
        BYTE *pResp )
{
    ULONG *lResp =
        &(( QmiCbkLocGetServerInd *)pResp)->serverType;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Extract the Server Type TLV */
    eRCode = GetLong (pTlvData, lResp);

    return eRCode;
}

package enum eQCWWANError UnpackCbkTlvGetServerIPv4(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocGetServerInd *lResponse = ( QmiCbkLocGetServerInd *)pResp;

    /* Allocate memory to pointer */
    lResponse->pAddressIPV4 = &addressIPV4;
    IPv4Address *lResp = lResponse->pAddressIPV4;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Extract the IPV4 address */
    eRCode = GetLong (pTlvData, &(lResp->ipv4Addr));
    if( eRCode != eQCWWAN_ERR_NONE )
        return eRCode;

    /* Extract the IPV4 port */        
    eRCode = GetWord( pTlvData, &(lResp->port) );   
    return eRCode;
}

package enum eQCWWANError UnpackCbkTlvGetServerIPv6(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocGetServerInd *lResponse = ( QmiCbkLocGetServerInd *)pResp;

    /* Allocate memory to pointer */
    lResponse->pAddressIPV6 = &addressIPV6;
    IPv6Address *lResp = lResponse->pAddressIPV6;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    USHORT idx;
   
    /* Extract the IPV6 address */
    for (idx = 0; idx < 8; idx++)
    {
        eRCode = GetWordBe(pTlvData, &(lResp->ipv6Addr[idx]));
        if (eRCode != eQCWWAN_ERR_NONE)
            return eRCode;
    }
    /* Extract the IPV6 port */
    eRCode = GetLong (pTlvData, &(lResp->port));
  
    return eRCode;
}

package enum eQCWWANError UnpackCbkTlvGetServerURL(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocGetServerInd *lResponse = ( QmiCbkLocGetServerInd *)pResp;

    /* Allocate memory to pointer */
    lResponse->pAddressURL = &addressURL;
    UrlAddress *lResp = lResponse->pAddressURL;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    WORD lCount = 0;
   
    /* Extract the URL address */
    do
    {
        eRCode = GetByte(pTlvData, &(lResp->url[lCount]));
        if (eRCode != eQCWWAN_ERR_NONE)
            return eRCode;
        lCount++;    
    }while(lResp->url[lCount - 1] != EOS);

    return eRCode;
}

package enum eQCWWANError UpkQmiCbkLocGetServerInd(
        BYTE                    *pMdmResp,
        QmiCbkLocGetServerInd   *pApiResp)
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_GET_SERVER_STATUS,     &UnpackCbkTlvGetServerStatus },
        { eTLV_GET_SERVER_TYPE,       &UnpackCbkTlvGetServerType },
        { eTLV_GET_SERVER_IPV4,       &UnpackCbkTlvGetServerIPv4 },
        { eTLV_GET_SERVER_IPV6,       &UnpackCbkTlvGetServerIPv6 },
        { eTLV_GET_SERVER_URL,        &UnpackCbkTlvGetServerURL },
        { eTLV_TYPE_INVALID,                NULL } /* Important. Sentinel.
                                                    * Signifies last item in map.
                                                    */
    };

    eRCode = qmunpackresp( pMdmResp,
            (BYTE *)pApiResp,
            map,
            eQMI_LOC_GET_SERVER );
    return eRCode;
}
