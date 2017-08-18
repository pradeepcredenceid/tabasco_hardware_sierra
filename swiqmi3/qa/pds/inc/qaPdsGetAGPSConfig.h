/*
 * \ingroup pds
 *
 * \file    qaPdsGetAGPSConfig.h
 *
 * \brief   This file contains definitions, enumerations, structures and
 *          forward declarations for qaPdsGetAGPSConfig.c
 *
 * Copyright: © 2011-2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __PDS_GET_AGPS_CONFIG_H__
#define __PDS_GET_AGPS_CONFIG_H__

/*
 * An enumeration of eQMI_PDS_GET_AGPS_CONFIG request TLV IDs
 */
enum ePDS_GET_AGPS_CONFIG_REQ
{
    eTLV_GET_AGPS_NETWORK_MODE = 0x12
};

/*
 * An enumeration of eQMI_PDS_GET_AGPS_CONFIG response TLV IDs
 */
enum eQMI_PDS_GET_AGPS_CONFIG_RESP
{
    eTLV_GET_AGPS_LOCATION_SERVER     = 0x10,
    eTLV_GET_AGPS_LOCATION_SERVER_URL = 0x11
};

/*
 * An enumeration of eQMI_PDS_GET_AGPS_CONFIG response TLV Lengths
 */
enum eQMI_PDS_GET_AGPS_CONFIG_RESP_LENGTH
{
    eTLV_AGPS_LOCATION_SERVER_LENGTH = 0x08
};

/*
 * This structure contains the GetAGPSConfig request parameters.
 */
struct QmiPdsGetAGPSConfigReq
{
    BYTE  *pNetworkMode;
};

/*
 * This structure contains the GetAGPSConfig response parameters
 */
struct QmiPdsGetAGPSConfigResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;

    /* GetAGPSConfig response parameters */
    ULONG *pServerAddress;
    ULONG *pServerPort;
    BYTE  *pServerURL;
    BYTE  *pServerURLLength;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiPdsGetAGPSConfig(
    WORD *pMlength,
    BYTE *pBuffer,
    BYTE *pNetworkMode);

extern enum eQCWWANError UpkQmiPdsGetAGPSConfig(
    BYTE                           *pMdmResp,
    struct QmiPdsGetAGPSConfigResp *pApiResp );

#endif /* __PDS_GET_AGPS_CONFIG_H__ */
