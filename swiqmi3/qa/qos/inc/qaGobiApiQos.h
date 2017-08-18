/**
 * \ingroup qos
 *
 * \file    qaGobiApiQos.h
 *
 * \brief   Quality of Service API function prototypes
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */
#ifndef __GOBI_API_QOS_H__
#define __GOBI_API_QOS_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  Reset the QoS service state variables of the requesting control point
 *
 *  \param  instance[IN]
 *          - QMI instance
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Technology Supported: CDMA, UMTS & LTE\n
 *          Device Supported: MC7750\n
 *          Timeout: 2 seconds
 *
 */
ULONG SLQSQosReset(
        BYTE instance
        );


/**
 * This structure contains the IP flow data rate min max
 *
 *  \param  dataRateMax
 *          Maximum required data rate (bits per second)
 *
 *  \param  guaranteedRate
 *          Minimum guaranteed data rate (bits per second)
 *
 */
typedef struct
{
    ULONG   dataRateMax;
    ULONG   guaranteedRate;
} dataRate;

/**
 * This structure contains the TP flow data rate token bucket
 *
 *  \param  peakRate
 *          Maximum rate at which data can be transmitted when the token bucket is full (bits per second)
 *  \param  tokenRate
 *          Rate at which tokens will be put in the token bucket (bits per second); a token is required to be present in the bucket to send a byte of data
 *
 *  \param  bucketSz
 *          Maximum number of tokens that can be accumulated at any instance (bytes); controls the size of the burst that is allowed at any given time
 *
 */
typedef struct
{
    ULONG   peakRate;
    ULONG   tokenRate;
    ULONG   bucketSz;
} tokenBucket;

/**
 * This structure contains the IP flow packet error rate
 *
 *  \param  multiplier
 *          Factor m in calculating packet error rate:
 *          E = m*10**(-p)
 *
 *  \param  exponent
 *          Factor p in calculating packet error rate (see above)
 *
 */
typedef struct
{
    WORD    multiplier;
    WORD    exponent;
} pktErrRate;

/**
 * This structure contains the QoS Flow Request
 *
 *  \param  index
 *          - IP flow index
 *          - Integer that uniquely identifies each flow instance
 *          - Unique index must be assigned by the control point to every flow_spec instance
 *
 *  \param  pProfileId3GPP2
 *          - IP flow 3GPP2 profile ID
 *          - A profile ID is shorthand for a defined set of QoS flow parameters specified by the network; to be present while requesting QoS for a CDMA device
 *          - NOT IMPLEMENTED
 *
 *  \param  p3GPP2Pri
 *          - IP flow 3GPP2 flow priority
 *          - Flow priority used by the network in case of contention between flows with same QoS; this parameter applies for CDMA devices
 *          - NOT IMPLEMENTED
 *
 *  \param  pTrafficClass
 *          - IP flow traffic class
 *          - Integer that designates the requested traffic class:
 *          - 0 – Conversational
 *          - 1 – Streaming
 *          - 2 – Interactive
 *          - 3 – Background
 *
 *  \param  pDataRate
 *          - IP flow data rate min max
 *          - See \ref dataRate for more information
 *
 *  \param  pTokenBucket
 *          - IP flow data rate token bucket
 *          - See \ref tokenBucket for more information
 *          - NOT IMPLEMENTED
 *
 *  \param  pLatency
 *          - IP flow latency
 *          - Maximum delay (in milliseconds) that can be tolerated by an IP packet during transfer through the wireless link
 *
 *  \param  pJitter
 *          - IP flow jitter
 *          - Difference between the maximum and minimum latency (in milliseconds) that can be tolerated by an IP packet during the transfer through the wireless link
 *
 *  \param  pPktErrRate
 *          - IP flow packet error rate
 *          - See \ref pktErrRate for more information
 *          - NOT IMPLEMENTED
 *
 *  \param  pMinPolicedPktSz
 *          - IP flow minimum policed packet size
 *          - Integer that defines the minimum packet size (in bytes) that will be policed for QoS guarantees; any IP packets that are smaller than the minimum specified policed size may not receive requested QoS
 *          - NOT IMPLEMENTED
 *
 *  \param  pMaxAllowedPktSz
 *          - IP flow maximum allowed packet size
 *          - Integer that defines the maximum packet size (in bytes) allowed in the IP flow; any IP packets greater in size than the maximum allowed packet size are not queued for transmission
 *          - NOT IMPLEMENTED
 *
 *  \param  p3GPPResResidualBER
 *          - IP flow 3GPP residual bit error rate
 *          - residual_bit_error_rate
 *          - 0 = 5*10-2 residual BER
 *          - 1 = 1*10-2 residual BER
 *          - 2 = 5*10-3 residual BER
 *          - 3 = 4*10-3 residual BER
 *          - 4 = 1*10-3 residual BER
 *          - 5 = 1*10-4 residual BER
 *          - 6 = 1*10-5 residual BER
 *          - 7 = 1*10-6 residual BER
 *          - 8 = 6*10-8 residual BER
 *          - Integer that indicates the undetected BER for each IP flow in the delivered packets; Tapplies only to 3GPP networks
 *          - NOT IMPLEMENTED
 *
 *  \param  p3GPPTraHdlPri
 *          - 3GPP traffic handling priority
 *          - 0 – Relative traffic handling priority 1
 *          - 1 – Relative traffic handling priority 2
 *          - 2 – Relative traffic handling priority 3
 *          - Defines the relative priority of the flow; applies only to 3GPP networks
 *          - NOT IMPLEMENTED
 *
 *  \param  p3GPPImCn
 *          - IP flow 3GPP IM CN flag
 *          - IM CN subsystem signaling flag:
 *          - 0x00 – FALSE
 *          - 0x01 – TRUE
 *          - This parameter applies only to 3GPP networks
 *          - NOT IMPLEMENTED
 *
 *  \param  p3GPPSigInd
 *          - IP flow 3GPP signaling indication
 *          - 0x00 – FALSE
 *          - 0x01 – TRUE
 *          - This parameter applies only to 3GPP networks
 *          - NOT IMPLEMENTED
 *
 *  \param  pLteQci
 *          - LTE QoS Class Identifier
 *          - QoS Class Identifier(QCI) is a required parameter to request QoS in LTE
 *          - QCI values:
 *            - QCI value 0 requests the network to assign the appropriate QCI value
 *            - QCI values 1-4 are associated with guaranteed bitrates
 *            - QCI values 5-9 are associated with nonguaranteed bitrates, so the values specified as guaranteed and maximum bitrates are ignored
 */
typedef struct
{
    BYTE        index;
    WORD        *pProfileId3GPP2;
    BYTE        *p3GPP2Pri;
    BYTE        *pTrafficClass;
    dataRate    *pDataRate;
    tokenBucket *pTokenBucket;
    ULONG       *pLatency;
    ULONG       *pJitter;
    pktErrRate  *pPktErrRate;
    ULONG       *pMinPolicedPktSz;
    ULONG       *pMaxAllowedPktSz;
    WORD        *p3GPPResResidualBER;
    WORD        *p3GPPTraHdlPri;
    BYTE        *p3GPPImCn;
    BYTE        *p3GPPSigInd;
    BYTE        *pLteQci;
} swiQosFlow;

/**
 * This structure contains the IPv4 filter address
 *
 *  \param  addr
 *          IPv4 address
 *
 *  \param  subnetMask
 *          A packet matches if:
 *          - (addr and subnetMask) == (IP pkt addr & subnetMask)
 *          Callers to set up a filter with a range of source addresses, if needed; subnet mask of all 1s (255.255.255.255) specifies a single address value
 *
 */
typedef struct
{
    ULONG   addr;
    ULONG   subnetMask;
} IPv4Addr;

/**
 * This structure contains the IPv4 filter type of service
 *
 *  \param  val
 *          Type of service value
 *
 *  \param  mask
 *          Packet matches the TOS filter if:
 *          (IPv4_filter_tos_val and IPv4_filter_tos_mask) == (TOS value in the IP packet & IPv4_filter_tos_mask)
 *          Example:
 *          - IPv4_filter_tos_val = 00101000
 *          - IPv4_filter_tos_mask = 11111100
 *          The filter will compare only the first 6 bits in the IPv4_filter_type_of_service with the first 6 bits in the TOS field of the IP packet. The first 6 bits in the TOS field of the IP packet must be 001010 to match the filter. The last 2 bits can be anything since they are ignored by filtering.
 *
 */
typedef struct
{
    ULONG   val;
    ULONG   mask;
} Tos;

/**
 * This structure contains the IPv6 filter address
 *
 *  \param  addr
 *          IPv6 address (in network byte order); this is a 16-byte byte array (in Big-endian format)
 *
 *  \param  prefixLen
 *          IPv6 filter prefix length; can take a value between 0 and 128
 *          Note: A packet matches if the IPv6 source address bytes until the prefix lengths are equal. Therefore prefix length can be used to set a filter with a range of source addresses. A prefix length of 128 specifies a single address value.
 *
 */
typedef struct
{
    BYTE    addr[16];
    BYTE    prefixLen;
} IPv6Addr;

/**
 * This structure contains the IPv6 filter traffic class
 *
 *  \param  val
 *          The traffic class value
 *
 *  \param  mask
 *          The packet matches the traffic class filter if:
 *          (IPv6_filter_traffic_class_val and IPv6_filter_traffic_class_mask) == (Traffic class value in the IP packet & IPv6_filter_traffic_class_mask)
 *          Example:
 *          - IPv6_filter_tos_val = 00101000
 *          - IPv6_filter_tos_mask = 11111100
 *          Filter will compare only the first 6 bits in IPv6_filter_traffic_class with the first 6 bits in the traffic class field of the IP packet; first 6 bits in the traffic class field of the IP packet must be 001010 to match filter; last 2 bits can be anything, since they are ignored by filtering
 *
 */
typedef struct
{
    ULONG   val;
    ULONG   mask;
} IPv6TrafCls;

/**
 * This structure contains the Port Filter
 *
 *  \param  port
 *          port value of the filter
 *
 *  \param  range
 *          range specifies the number of ports to be included in the filter starting from port; filter will match if port in the IP packet lies between port and (port + range )
 *          Range value of 0 implies that only one value of the port is valid, as specified by the port
 *
 */
typedef struct
{
    WORD   port;
    WORD   range;
} Port;

/**
 * This structure contains the QoS Filter Request
 *
 *  \param  index
 *          IP filter index
 *          Integer that uniquely identifies each filter instance
 *          This TLV must be present in the request
 *
 *  \param  version
 *          IP filter version
 *          Identifies whether the filter is associated with IPv4 or IPv6; value specified also implies that only TLVs defined for that IP version, i.e., TLVs with IPv4 or IPv6 in the name, can be specified
 *          - 0x04 – IPv4
 *          - 0x06 – Ipv6
 *
 *  \param  pIPv4SrcAddr
 *          IPv4 filter soruce address
 *          See \ref IPv4Addr for more information
 *
 *  \param  pIPv4DstAddr
 *          IPv4 filter destination address
 *          See \ref IPv4Addr for more information
 *
 *  \param  pNextHdrProto
 *          IP filter next header protocol
 *          This TLV must be present if any non-IP filter TLV(s) are provided
 *          If this field is specified, only IP packets belonging to specified higher layer protocol are considered when filtering
 *          The following protocols may be specified:
 *          - 0x06 = TCP
 *          - 0x11 = UDP
 *
 *  \param  pTos
 *          IPv4 filter type of service
 *          See \ref Tos for more information
 *
 *  \param  pIPv6SrcAddr
 *          IPv6 filter soruce address
 *          See \ref IPv6Addr for more information
 *
 *  \param  pIPv6DstAddr
 *          IPv6 filter destination address
 *          See \ref IPv6Addr for more information
 *
 *  \param  pIPv6TrafCls
 *          IPv6 filter traffic class
 *          See \ref IPv6TrafCls for more information
 *
 *  \param  pIPv6Label
 *          IPv6 flow label
 *          Packet matches the IPv6 flow label filter if:
 *          ( *pIPv6Label == flow label in the IPv6 header)
 *
 *  \param  pTCPSrcPort
 *          TCP filter source port filter
 *          See \ref Port for more information
 *
 *  \param  pTCPDstPort
 *          TCP filter destination port filter
 *          See \ref Port for more information
 *
 *  \param  pUDPSrcPort
 *          UDP filter source port filter
 *          See \ref Port for more information
 *
 *  \param  pUDPDstPort
 *          UDP filter destination port filter
 *          See \ref Port for more information
 *
 *  \param  pEspSpi
 *          ESP filter security policy index
 *          Security policy index to uniquely identify each IP flow for filtering encrypted packets for encapsulating security payload
 *
 *  \param  pPrecedence
 *          Filter Precedence
 *          Specifies the order in which filters are applied; lower numerical value has higher precedence
 *          Note: This TLV only applies to network-initiated QoS; QoS requests containing this TLV from control points will be ignored
 *          
 *  \param  pId
 *          Filter ID
 *          Unique identifier for each filter;filter ID is assigned by the modem
 *          Note: This TLV only applies to network-initiated QoS; QoS requests containing this TLV from control points will be ignored
 *
 *  \param  pTranSrcPort
 *          Transport protocolfilter source port
 *          See \ref Port for more information
 *
 *  \param  pUDPDstPort
 *          Transport protocol filter destination port
 *          See \ref Port for more information
 */
typedef struct
{
    BYTE        index;
    BYTE        version;
    IPv4Addr    *pIPv4SrcAddr;
    IPv4Addr    *pIPv4DstAddr;
    BYTE        *pNextHdrProto;
    Tos         *pTos;
    IPv6Addr    *pIPv6SrcAddr;
    IPv6Addr    *pIPv6DstAddr;
    IPv6TrafCls *pIPv6TrafCls;
    ULONG       *pIPv6Label;
    Port        *pTCPSrcPort;
    Port        *pTCPDstPort;
    Port        *pUDPSrcPort;
    Port        *pUDPDstPort;
    ULONG       *pEspSpi;
    BYTE        *pPrecedence;
    BYTE        *pId;
    Port        *pTranSrcPort;
    Port        *pTranDstPort;
} swiQosFilter;

/**
 * This structure contains the QoS Request parameters.
 *
 *  \param  index
 *          - An integer that uniquely identifies each QoS spec included in the QoS request message
 *
 *  \param  pTxFlow
 *          - See \ref swiQosFlow for more information
 *
 *  \param  pRxFlow
 *          - See \ref swiQosFlow for more information
 *
 *  \param  pTxFilter
 *          - See \ref swiQosFilter for more information
 *
 *  \param  pRxFilter
 *          - See \ref swiQosFilter for more information
 *
 */
typedef struct
{
    BYTE            index;
    swiQosFlow      *pTxFlow;
    swiQosFlow      *pRxFlow;
    swiQosFilter    *pTxFilter;
    swiQosFilter    *pRxFilter;
} swiQosReq;

/**
 * This structure contains the QoS Response parameters.
 *
 *  \param  sz
 *          Number of QoS identifiers
 *
 *  \param  pIds
 *          Identifier for the QoS flow requested; number of QoS identifiers present will be equal to number of QoS specs requested in the QoS Request message
 *
 */
typedef struct
{
    BYTE    sz;
    ULONG   *pIds;
} swiQosIds;

/**
 * Triggers QoS negotiation by providing QoS parameters
 *
 *  \param  instance[IN]
 *          - QMI instance
 *
 *  \param  pQoSReq[IN]
 *          - See \ref swiQosReq for more information
 *
 *  \param  pQosResp[OUT]
 *          - See \ref swiQosIds for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     see qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note   Technology Supported: UMTS/CDMA\n
 *          Device Supported: MC77XX\n
 *          Timeout: 2 seconds\n
 *
 *  \note   At least one pair of optional TLVs must be present; Tx QoS Flow Request and Tx QoS Filter Request TLVs must both be present if either one is present; Rx QoS Flow Request and Rx QoS Filter Request TLVs must both be present if either one is present
 */
ULONG SLQSQosReq(
    BYTE        instance,
    swiQosReq   *pQosReq,
    swiQosIds   *pQosResp );

/**
 *  Release one or more existing QoS flows
 *
 *  \param[in]  instance
 *              - QMI instance
 *
 *  \param  pQosIds
 *          - See \ref swiQosIds for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     see qmerrno.h for eQCWWAN_xxx error values
 *
 */
ULONG SLQSQosRel(
    BYTE        instance,
    swiQosIds   *pQosIds );

/**
 *  Suspend one or more existing QoS flows
 *
 *  \param[in]  instance
 *              - QMI instance
 *
 *  \param  pQosIds
 *          - See \ref swiQosIds for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     see qmerrno.h for eQCWWAN_xxx error values
 *
 */
ULONG SLQSQosSuspend(
    BYTE        instance,
    swiQosIds   *pQosIds );

/**
 *  Resume one or more existing QoS flows
 *
 *  \param[in]  instance
 *              - QMI instance
 *
 *  \param  pQosIds
 *          - See \ref swiQosIds for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     see qmerrno.h for eQCWWAN_xxx error values
 *
 */
ULONG SLQSQosResume(
    BYTE        instance,
    swiQosIds   *pQosIds );

/**
 * This structure contains the QoS Request parameters.
 *
 *  \param  id
 *          Identifier for the QoS flow/instance that has been negotiated and that needs to be modified
 *          The QoS_identifier is used to reference the actual flow/filter specifications that are in effect as a result of the negotiation triggered by QMI_QOS_REQUEST_QOS_REQ
 *
 *  \param  pTxFlow
 *          See \ref swiQosFlow for more information
 *
 *  \param  pRxFlow
 *          See \ref swiQosFlow for more information
 *
 *  \param  pTxFilter
 *          See \ref swiQosFilter for more information
 *
 *  \param  pRxFilter
 *          See \ref swiQosFilter for more information
 *
 */
typedef struct
{
    ULONG           id;
    swiQosFlow      *pTxFlow;
    swiQosFlow      *pRxFlow;
    swiQosFilter    *pTxFilter;
    swiQosFilter    *pRxFilter;
} swiQosModifyReq;

/**
 *  Resume one or more existing QoS flows
 *
 *  \warning NOT IMPLEMENTED
 *
 *  \param[in]  instance
 *              - QMI instance
 *
 *  \param  pReq
 *          - See \ref swiQosModifyReq for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     see qmerrno.h for eQCWWAN_xxx error values
 *
 */
ULONG SLQSQosModify(
    BYTE            instance,
    swiQosModifyReq *pReq );

/**
 * This structure contains the QoS granted flow
 *
 *  \param  pTxFlow
 *          See \ref swiQosFlow for more information
 *
 *  \param  pRxFlow
 *          See \ref swiQosFlow for more information
 *
 */
typedef struct
{
    swiQosFlow      *pTxFlow;
    swiQosFlow      *pRxFlow;
} swiQosGranted;

/**
 *  Retrieve the QoS parameters that are in effect for the specified QoS 16 flow as a result of network negotiation
 *
 *  \param[in]  instance
 *              - QMI instance
 *
 *  \param[in]  id
 *              - Qos identifier
 *              - Index identifying the QoS flow that has been negotiated
 *
 *  \param[in]  pGranted
 *              - Tx/Rx Qos granted flow
 *              - See \ref swiQosGranted for more information
 *
 *  \return     eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa         see qmerrno.h for eQCWWAN_xxx error values
 *
 */
ULONG SLQSQosGetGranted(
    BYTE            instance,
    ULONG           id,
    swiQosGranted   *pGranted );

/**
 *  Get the status of a QoS flow.
 *
 *  \param[in]  instance
 *              - QMI instance
 *
 *  \param[in]  id
 *              Qos identifier
 *              Index identifying the QoS flow that has been negotiated
 *
 *  \param[out] pStatus
 *              Qos status
 *              Current QoS instance status:
 *              - 0x01 – QMI_QOS_STATUS_ACTIVATED
 *              - 0x02 – QMI_QOS_STATUS_SUSPENDED
 *              - 0x03 – QMI_QOS_STATUS_GONE
 *
 *  \return     eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa         see qmerrno.h for eQCWWAN_xxx error values
 *
 */
ULONG SLQSQosGetFlowStatus(
    BYTE    instance,
    ULONG   id,
    BYTE    *pStatus);

/**
 *  Queries whether the device is currently on a network that supports QoS
 *
 *  \param  instance[IN]
 *          - QMI instance
 *
 *  \param[out] pStatus
 *              Network QoS support status
 *              - 0 – No QoS support in network
 *              - 1 – Network supports QoS
 *
 *  \return     eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa         see qmerrno.h for eQCWWAN_xxx error values
 *
 *  \note       Technology Supported: CDMA\n
 *
 */
ULONG SLQSQosGetNetworkStatus(
    BYTE    instance,
    BYTE    *pStatus);

/**
 * This structure contains Network supported QoS profile
 *
 *  \param  tech
 *          Technology on which the network supported QoS profiles are being returned:
 *          - CDMA – 0x8001
 *
 *  \param  exponent
 *
 */
typedef struct
{
    WORD    tech;
    BYTE    *pProfSz;
    WORD    *pProfValues;
} NWProfile;

/**
 *  Get network supported QoS profile information
 *
 *  \param[in]  instance
 *              - QMI instance
 *
 *  \param[in/out] pSz
 *              Number of network supported QoS profiles for one technology
 *
 *  \param[out] pProfile
 *              Network supported QoS profiles
 *
 *  \return     eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa         see qmerrno.h for eQCWWAN_xxx error values
 *
 */
ULONG SLQSQosGetNWProf(
    BYTE        instance,
    BYTE        *pSz,
    NWProfile   *pProfile);

/**
 * This structure contains the Data statistic per QoS flow
 *
 *  \param  qid
 *          - Qos flow ID
 *
 *  \param  tx_pkt
 *          - number of sent packets for the QoS flow ID
 *
 *  \param  tx_pkt_drp
 *          - number of dropped(TX) packets for the QoS flow ID
 *
 *  \param  rx_pkt
 *          - number of received packets for the QoS flow ID
 *
 *  \param  tx_bytes
 *          - number of sent bytes for the QoS flow ID
 *
 *  \param  tx_bytes_drp
 *          - number of dropped(TX) bytes for the QoS flow ID
 *
 *  \param  rx_bytes
 *          - number of received bytes for the QoS flow ID
 */
typedef struct
{
    ULONG   qid;
    ULONG   tx_pkt;
    ULONG   tx_pkt_drp;
    ULONG   rx_pkt;
    ULONG   tx_bytes;
    ULONG   tx_bytes_drp;
    ULONG   rx_bytes;
} QosFlowStat;

/**
 * This structure contains the Data statistic per QoS flow
 *
 *  \param  sid
 *          - session id
 *          - ID identifing the connected APN that the client would like to query the data statistic for
 *
 *  \param  total_tx_pkt
 *          - sum of all packets sent
 *
 *  \param  total_tx_pkt_drp
 *          - sum of all(TX) packets dropped
 *
 *  \param  total_rx_pkt
 *          - sum of all packets received
 *
 *  \param  total_tx_bytes
 *          - sum of all bytes sent
 *
 *  \param  toal_tx_bytes_drp
 *          - sum of all(TX) bytes dropped
 *
 *  \param  total_rx_bytes
 *          - number of received bytes for the QoS flow ID
 *
 *  \param  pNumQosFlow
 *          - pointer to number of QoS flow Stat
 *
 *  \param  pQosFlow
 *          - pointer to number of QoS flow Stat
 *
 *  \param  pQosFlow
 *          - Data statistic per QoS flow
 *          - See \ref QosFlowStat for more information
 */
typedef struct
{
    ULONG        sid;
    ULONG        total_tx_pkt;
    ULONG        total_tx_pkt_drp;
    ULONG        total_rx_pkt;
    ULONG        total_tx_bytes;
    ULONG        total_tx_bytes_drp;
    ULONG        total_rx_bytes;
    ULONG        *pNumQosFlow;
    QosFlowStat  *pQosFlow;
} QosStat;

/**
 *  Get the current number of packets and bytes sent, dropped and received for each UL, DL bearer and a sum of them for UL and DL direction in the modem.
 *
 *  \warning NOT IMPLEMENTED
 *
 *  \param[in]  sid
 *              -session id
 *
 *  \param[out] pQosStat
 *              See \ref QosFlow for more information
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
ULONG SLQSQosGetStat(
        ULONG   sid,
        QosStat *pQosStat
        );

/**
 *  Queries extra APN parameters that are not reported by existing QCT QMI service
 *
 *  \warning NOT IMPLEMENTED
 *
 *  \param[in]  sid
 *              - session id
 *
 *  \param[out] pAmbr
 *              - Aggregated Maximum Bit Rate
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 *  \sa     See qmerrno.h for eQCWWAN_xxx error values
 *
 */
ULONG SLQSQosGetAmbr(
        ULONG   sid,
        ULONG   *pAmbr
        );

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* __GOBI_API_QOS_H__ */
