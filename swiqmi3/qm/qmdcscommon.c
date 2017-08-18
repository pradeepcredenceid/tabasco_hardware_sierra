/*************
 *
 * Filename: qmdcscommon.c
 *
 * Purpose:  QMI Device Connectivity Service utilities common to application and SDK
 *
 * Copyright: © 2011 Sierra Wireless Inc., all rights reserved
 *
 **************/

/*-------------
  include files
 --------------*/
#include "aa/aaglobal.h"
#include "qm/qmidefs.h"
#include "qm/qmerrno.h"
#include "sl/sludefs.h"
#include "qm/qmqmisvc.h"

/*---------
  Functions
 ----------*/

/*************
 *
 * Name:    qm_dcs_enumerate_device_key_unpack_tlv
 *
 * Purpose: Unpack device key TLV Value
 *
 * Parms:   (IN)    psource - source data
 *          (OUT)   pdest   - destination buffer
 *
 * Return:  eQCWWAN_ERR_NONE on success
 *
 * Abort:   none
 *
 * Notes:
 *
 **************/
local enum eQCWWANError
qm_dcs_enumerate_device_key_unpack_tlv(
    swi_uint8 *psource,
    swi_uint8 *pdest )
{
    struct qm_qmi_response_tlvs_values *pin;
    struct qm_dcs_enumerate_devices_response_tlv_values *pval;

    pin = (struct qm_qmi_response_tlvs_values *)pdest;
    pval = (struct qm_dcs_enumerate_devices_response_tlv_values *)&pin->tlvvalues.qmdcstlvs.enumerate;

    pval->dev.devkeystrsize = sizeof(pval->dev.devkey);
    slmemset( (char *)pval->dev.devkey,0, sizeof(pval->dev.devkey) );
    return GetStream( psource,
                      pval->dev.devkey,
                      sizeof(pval->dev.devkey) - 1 );
}

/*************
 *
 * Name:    qm_dcs_enumerate_device_node_unpack_tlv
 *
 * Purpose: Unpack device node TLV Value
 *
 * Parms:   (IN)    psource - source data
 *          (OUT)   pdest   - destination storage
 *
 * Return:  eQCWWAN_ERR_NONE on success
 *
 * Abort:   none
 *
 * Notes:
 *
 **************/
local enum eQCWWANError
qm_dcs_enumerate_device_node_unpack_tlv(
    swi_uint8 *psource,
    swi_uint8 *pdest )
{
    struct qm_qmi_response_tlvs_values *pin;
    struct qm_dcs_enumerate_devices_response_tlv_values *pval;

    pin = (struct qm_qmi_response_tlvs_values *)pdest;
    pval = (struct qm_dcs_enumerate_devices_response_tlv_values *)&pin->tlvvalues.qmdcstlvs.enumerate;

    enum eQCWWANError rc;
    rc = qmQmiExtractString( psource,
                             (swi_char *)pval->dev.devnode,
                             sizeof(pval->dev.devnode) );

    pval->dev.devnodestrsize =
    rc == eQCWWAN_ERR_NONE
    ? slstrlen((char *)pval->dev.devnode) + 1
    : 0;

    return rc;
}

/*************
 *
 * Name:    qm_dcs_enumerate_devices_response_unpack
 *
 * Purpose: Unpack DCS enumerate response
 *
 * Parms:   (IN)    prsp        - response data
 *          (OUT)   prsptlvs    - destination response parameters structure
 *
 * Return:  eQCWWAN_ERR_NONE on success
 *
 * Abort:   none
 *
 * Notes:
 *
 **************/
global enum eQCWWANError
qm_dcs_enumerate_devices_response_unpack(
    swi_uint8 *prsp,
    struct qm_qmi_response_tlvs_values *prsptlvs )
{
    struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,
          qm_result_code_tlv_unpack },

        { eTLV_DCS_ENUMERATE_DEVICE_NODE,
          qm_dcs_enumerate_device_node_unpack_tlv },

        { eTLV_DCS_ENUMERATE_DEVICE_KEY,
          qm_dcs_enumerate_device_key_unpack_tlv },

        { eTLV_TYPE_INVALID, NULL }
    };

    return qmunpack( prsp,
                     (swi_uint8 *)prsptlvs,
                     map,
                     eQMI_DCS_ENUMERATE_DEVICES );
}

/*************
 *
 * Name:    qm_dcs_connect_device_response_unpack
 *
 * Purpose: Unpack DCS connect response
 *
 * Parms:   (IN)    prsp        - response data
 *          (OUT)   prsptlvs    - destination response parameters structure
 *
 * Return:  eQCWWAN_ERR_NONE on success
 *
 * Abort:   none
 *
 * Notes:
 *
 **************/
global enum eQCWWANError
qm_dcs_connect_response_unpack(
    swi_uint8 *prsp,
    struct qm_qmi_response_tlvs_values *prsptlvs )
{
    struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,
          qm_result_code_tlv_unpack },

        { eTLV_TYPE_INVALID, NULL }
    };

    return qmunpack( prsp,
                     (swi_uint8 *)prsptlvs,
                     map,
                     eQMI_DCS_CONNECT );
}

/*************
 *
 * Name:    qm_dcs_connect_device_key_pack_tlv
 *
 * Purpose: Pack device key TLV Value
 *
 * Parms:   (OUT)   pdest   - destination buffer
 *          (IN)    pparm   - source data
 *
 * Return:  eQCWWAN_ERR_NONE
 *
 * Abort:   none
 *
 * Notes:
 *
 **************/
local enum eQCWWANError
qm_dcs_connect_device_key_pack_tlv(
    swi_uint8 *pdest,
    swi_uint8 *psource )
{
    struct qm_dcs_connect_request_args *pin;
    pin = (struct qm_dcs_connect_request_args *)psource;
    return PutStream( pdest,
                      (swi_uint8 *)pin->pdevkey,
                      QMI_MEID_BUF_SIZE - 1 );
}

/*************
 *
 * Name:    qm_dcs_connect_device_node_pack_tlv
 *
 * Purpose: Pack device node TLV Value
 *
 * Parms:   (OUT)   pdest   - destination buffer
 *          (IN)    psrc    - source data
 *
 * Return:  eQCWWAN_ERR_NONE
 *
 * Abort:   none
 *
 * Notes:
 *
 **************/
local enum eQCWWANError
qm_dcs_connect_device_node_pack_tlv(
    swi_uint8 *pdest,
    swi_uint8 *psource )
{
    struct qm_dcs_connect_request_args *pin;
    pin = (struct qm_dcs_connect_request_args *)psource;
    return PutStream( pdest,
                      (swi_uint8 *)pin->pdevnode,
                      slstrlen(pin->pdevnode) + 1 );
}

/*************
 *
 * Name:    qm_dcs_start_srv_pack_tlv
 *
 * Purpose: Pack device node TLV Value
 *
 * Parms:   (OUT)   pdest   - destination buffer
 *          (IN)    psrc    - source data
 *
 * Return:  eQCWWAN_ERR_NONE
 *
 * Abort:   none
 *
 * Notes:
 *
 **************/
local enum eQCWWANError
qm_dcs_start_srv_pack_tlv(
    swi_uint8 *pdest,
    swi_uint8 *psource )
{
    struct qm_dcs_start_srv_request_args *pin;
    pin = (struct qm_dcs_start_srv_request_args *)psource;
    return PutByte( pdest,
                      pin->action );
}

/*************
 *
 * Name:    qm_dcs_mask_pack_tlv
 *
 * Purpose: Pack device node TLV Value
 *
 * Parms:   (OUT)   pdest   - destination buffer
 *          (IN)    psrc    - source data
 *
 * Return:  eQCWWAN_ERR_NONE
 *
 * Abort:   none
 *
 * Notes:
 *
 **************/
local enum eQCWWANError
qm_dcs_mask_pack_tlv(
    swi_uint8 *pdest,
    swi_uint8 *psource )
{
    struct qm_dcs_start_srv_request_args *pin;
    pin = (struct qm_dcs_start_srv_request_args *)psource;
    return PutByte( pdest, pin->mask );
}

local enum eQCWWANError
qm_dcs_qos_map_dscp_pack_tlv(
    swi_uint8 *pdest,
    swi_uint8 *psource )
{
    struct qm_dcs_qos_map_args *pin;
    pin = (struct qm_dcs_qos_map_args *)psource;
    return PutByte( pdest,
                      pin->dscp );
}

local enum eQCWWANError
qm_dcs_qos_map_qos_id_pack_tlv(
    swi_uint8 *pdest,
    swi_uint8 *psource )
{
    struct qm_dcs_qos_map_args *pin;
    pin = (struct qm_dcs_qos_map_args *)psource;
    return PutLong( pdest,
                      pin->qos_id );
}

local enum eQCWWANError
qm_dcs_qos_unmap_dscp_pack_tlv(
    swi_uint8 *pdest,
    swi_uint8 *psource )
{
    return PutByte( pdest, *psource );
}

/*************
 *
 * Name:    qm_dcs_connect_request_pack
 *
 * Purpose: Pack  DCS Connect request
 *
 * Parms:   (IN)    parg      - source device path buffer
 *          (IN)    pdevkey   - source device key buffer
 *          (OUT)   preq      - outgoing request buffer
 *          (OUT)   plen      - length of packed request
 *
 * Return:  eQCWWAN_ERR_NONE on success
 *
 * Abort:   none
 *
 * Notes:
 *
 **************/
global enum eQCWWANError
qm_dcs_connect_request_pack(
    swi_char *pdevnode,
    swi_char *pdevkey,
    swi_uint8 *preqbuf,
    swi_uint16 *plen )
{
    struct qmTlvBuilderItem map[] =
    {
        { eTLV_DCS_CONNECT_DEVICE_NODE,
          qm_dcs_connect_device_node_pack_tlv },

        { eTLV_DCS_CONNECT_DEVICE_KEY,
          qm_dcs_connect_device_key_pack_tlv },

        { eTLV_TYPE_INVALID, NULL }
    };

    /* QMI request packing input arguments */
    struct qm_dcs_connect_request_args arg;
    arg.pdevnode = pdevnode;
    arg.pdevkey = pdevkey;

    /* pack QMI request message */
    enum eQCWWANError rc;
    rc = qmbuild( preqbuf,
                  (swi_uint8 *)&arg,
                  map,
                  eQMI_DCS_CONNECT,
                  plen );

    return rc;
}

/*************
 *
 * Name:    qm_dcs_start_srv_pack
 *
 * Purpose: Pack DCS Start Service request
 *
 * Parms:   (IN)    parg      - source device path buffer
 *          (IN)    pdevkey   - source device key buffer
 *          (OUT)   preq      - outgoing request buffer
 *          (OUT)   plen      - length of packed request
 *
 * Return:  eQCWWAN_ERR_NONE on success
 *
 * Abort:   none
 *
 * Notes:
 *
 **************/
global enum eQCWWANError
qm_dcs_start_srv_pack(
    swi_char action,
    swi_char mask,
    swi_uint8 *preqbuf,
    swi_uint16 *plen )
{
    struct qmTlvBuilderItem map[] =
    {
        { eTLV_ACTION,
          qm_dcs_start_srv_pack_tlv },

        { eTLV_MASK,
          qm_dcs_mask_pack_tlv },

        { eTLV_TYPE_INVALID, NULL }
    };

    /* QMI request packing input arguments */
    struct qm_dcs_start_srv_request_args arg;
    arg.action = action;
    arg.mask   = mask;

    /* pack QMI request message */
    enum eQCWWANError rc;
    rc = qmbuild( preqbuf,
                  (swi_uint8 *)&arg,
                  map,
                  eQMI_DCS_START_SRV,
                  plen );

    return rc;
}

/*************
 *
 * Name:    qm_dcs_qos_map_pack
 *
 * Purpose: Pack DCS Qos Map request
 *
 * Parms:   (IN)    parg      - source device path buffer
 *          (IN)    pdevkey   - source device key buffer
 *          (OUT)   preq      - outgoing request buffer
 *          (OUT)   plen      - length of packed request
 *
 * Return:  eQCWWAN_ERR_NONE on success
 *
 * Abort:   none
 *
 * Notes:
 *
 **************/
global enum eQCWWANError
qm_dcs_qos_map_pack(
    swi_char dscp,
    swi_uint32 qos_id,
    swi_uint8 *preqbuf,
    swi_uint16 *plen )
{
    struct qmTlvBuilderItem map[] =
    {
        { eTLV_DSCP,
            qm_dcs_qos_map_dscp_pack_tlv },

        { eTLV_QOS_ID,
            qm_dcs_qos_map_qos_id_pack_tlv },

        { eTLV_TYPE_INVALID, NULL }
    };

    /* QMI request packing input arguments */
    struct qm_dcs_qos_map_args arg;
    arg.dscp = dscp;
    arg.qos_id = qos_id;

    /* pack QMI request message */
    enum eQCWWANError rc;
    rc = qmbuild( preqbuf,
                  (swi_uint8 *)&arg,
                  map,
                  eQMI_DCS_QOS_MAP,
                  plen );

    return rc;
}

global enum eQCWWANError
qm_dcs_qos_read_map_pack(
    swi_char dscp,
    swi_uint8 *preqbuf,
    swi_uint16 *plen )
{
    struct qmTlvBuilderItem map[] =
    {
        { eTLV_DSCP,
            qm_dcs_qos_map_dscp_pack_tlv },

        { eTLV_TYPE_INVALID, NULL }
    };

    /* QMI request packing input arguments */
    struct qm_dcs_qos_map_args arg;
    arg.dscp = dscp;

    /* pack QMI request message */
    enum eQCWWANError rc;
    rc = qmbuild( preqbuf,
                  (swi_uint8 *)&arg,
                  map,
                  eQMI_DCS_QOS_READ_MAP,
                  plen );

    return rc;
}

global enum eQCWWANError
qm_dcs_qos_dump_map_pack(
    swi_uint8 *preqbuf,
    swi_uint16 *plen )
{
    struct qmTlvBuilderItem map[] =
    {
        { eTLV_TYPE_INVALID, NULL }
    };

    /* pack QMI request message */
    enum eQCWWANError rc;
    rc = qmbuild( preqbuf,
                  NULL,
                  map,
                  eQMI_DCS_QOS_DUMP_MAP,
                  plen );

    return rc;
}

global enum eQCWWANError
qm_dcs_qos_edit_map_pack(
    swi_char dscp,
    swi_uint32 qos_id,
    swi_uint8 *preqbuf,
    swi_uint16 *plen )
{
    struct qmTlvBuilderItem map[] =
    {
        { eTLV_DSCP,
            qm_dcs_qos_map_dscp_pack_tlv },

        { eTLV_QOS_ID,
            qm_dcs_qos_map_qos_id_pack_tlv },

        { eTLV_TYPE_INVALID, NULL }
    };

    /* QMI request packing input arguments */
    struct qm_dcs_qos_map_args arg;
    arg.dscp = dscp;
    arg.qos_id = qos_id;

    /* pack QMI request message */
    enum eQCWWANError rc;
    rc = qmbuild( preqbuf,
                  (swi_uint8 *)&arg,
                  map,
                  eQMI_DCS_QOS_EDIT_MAP,
                  plen );

    return rc;
}

global enum eQCWWANError
qm_dcs_qos_unmap_pack(
    swi_char dscp,
    swi_uint8 *preqbuf,
    swi_uint16 *plen )
{
    struct qmTlvBuilderItem map[] =
    {
        { eTLV_DSCP,
            qm_dcs_qos_unmap_dscp_pack_tlv },

        { eTLV_TYPE_INVALID, NULL }
    };

    /* pack QMI request message */
    enum eQCWWANError rc;
    rc = qmbuild( preqbuf,
                  (swi_uint8 *)&dscp,
                  map,
                  eQMI_DCS_QOS_UNMAP,
                  plen );

    return rc;
}

global enum eQCWWANError
qm_dcs_qos_clear_map_pack(
    swi_uint8 *preqbuf,
    swi_uint16 *plen )
{
    struct qmTlvBuilderItem map[] =
    {
        { eTLV_TYPE_INVALID, NULL }
    };

    /* pack QMI request message */
    enum eQCWWANError rc;
    rc = qmbuild( preqbuf,
                  NULL,
                  map,
                  eQMI_DCS_QOS_CLEAR_MAP,
                  plen );

    return rc;
}

/*************
 *
 * Name:    qm_dcs_get_connected_device_id_response_unpack
 *
 * Purpose: Unpack DCS connected device id response
 *
 * Parms:   (IN)    prsp        - source response data
 *          (OUT)   prsptlvs    - structure into which the response TLV data is
 *                                unpacked.
 *
 * Return:  eQCWWAN_ERR_NONE on success
 *
 * Abort:   none
 *
 * Notes:
 *
 **************/
global enum eQCWWANError
qm_dcs_get_connected_device_id_response_unpack(
    swi_uint8 *prsp,
    struct qm_qmi_response_tlvs_values *prsptlvs )
{
    struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,
          qm_result_code_tlv_unpack },

        { eTLV_DCS_CONNECTED_DEVICEID_DEVICE_NODE,
          qm_dcs_enumerate_device_node_unpack_tlv },

        { eTLV_DCS_CONNECT_DEVICE_KEY,
          qm_dcs_enumerate_device_key_unpack_tlv },

        { eTLV_TYPE_INVALID, NULL }
    };

    return qmunpack( prsp,
                     (swi_uint8 *)prsptlvs,
                     map,
                     eQMI_DCS_GET_CONNECTED_DEVICE_ID );
}

/*************
 *
 * Name:    qm_dcs_get_usb_port_names_atcmdport_unpack_tlv
 *
 * Purpose: Unpack AT command port name TLV Value
 *
 * Parms:   (IN)    psource - source data
 *          (OUT)   pdest   - destination storage
 *
 * Return:  eQCWWAN_ERR_NONE on success
 *
 * Abort:   none
 *
 * Notes:
 *
 **************/
local enum eQCWWANError
qm_dcs_get_usb_port_names_atcmdport_unpack_tlv(
    swi_uint8 *psource,
    swi_uint8 *pdest )
{
    struct qm_qmi_response_tlvs_values *pin;
    struct qm_dcs_usb_port_names_response_tlv_values *pval;

    pin = (struct qm_qmi_response_tlvs_values *)pdest;
    pval = (struct qm_dcs_usb_port_names_response_tlv_values *)&pin->tlvvalues.qmdcstlvs.portnames;

    enum eQCWWANError rc;
    rc = qmQmiExtractString( psource,
                             (swi_char *)pval->usbportnames.AtCmdPort,
                             sizeof(pval->usbportnames.AtCmdPort) );

    return rc;
}
/*************
 *
 * Name:    qm_dcs_get_usb_port_names_nmeaport_unpack_tlv
 *
 * Purpose: Unpack NMEA port name TLV Value
 *
 * Parms:   (IN)    psource - source data
 *          (OUT)   pdest   - destination storage
 *
 * Return:  eQCWWAN_ERR_NONE on success
 *
 * Abort:   none
 *
 * Notes:
 *
 **************/
local enum eQCWWANError
qm_dcs_get_usb_port_names_nmeaport_unpack_tlv(
    swi_uint8 *psource,
    swi_uint8 *pdest )
{
    struct qm_qmi_response_tlvs_values *pin;
    struct qm_dcs_usb_port_names_response_tlv_values *pval;

    pin = (struct qm_qmi_response_tlvs_values *)pdest;
    pval = (struct qm_dcs_usb_port_names_response_tlv_values *)&pin->tlvvalues.qmdcstlvs.portnames;

    enum eQCWWANError rc;
    rc = qmQmiExtractString( psource,
                             (swi_char *)pval->usbportnames.NmeaPort,
                             sizeof(pval->usbportnames.NmeaPort) );

    return rc;
}
/*************
 *
 * Name:    qm_dcs_get_usb_port_names_dmport_unpack_tlv
 *
 * Purpose: Unpack DM port name TLV Value
 *
 * Parms:   (IN)    psource - source data
 *          (OUT)   pdest   - destination storage
 *
 * Return:  eQCWWAN_ERR_NONE on success
 *
 * Abort:   none
 *
 * Notes:
 *
 **************/
local enum eQCWWANError
qm_dcs_get_usb_port_names_dmport_unpack_tlv(
    swi_uint8 *psource,
    swi_uint8 *pdest )
{
    struct qm_qmi_response_tlvs_values *pin;
    struct qm_dcs_usb_port_names_response_tlv_values *pval;

    pin = (struct qm_qmi_response_tlvs_values *)pdest;
    pval = (struct qm_dcs_usb_port_names_response_tlv_values *)&pin->tlvvalues.qmdcstlvs.portnames;

    enum eQCWWANError rc;
    rc = qmQmiExtractString( psource,
                             (swi_char *)pval->usbportnames.DmPort,
                             sizeof(pval->usbportnames.DmPort) );

    return rc;
}

/*************
 *
 * Name:    qm_dcs_get_usb_port_names_response_unpack
 *
 * Purpose: Unpack DCS SLQSGetUSBPortNames response
 *
 * Parms:   (IN)    prsp        - response data
 *          (OUT)   prsptlvs    - destination response parameters structure
 *
 * Return:  eQCWWAN_ERR_NONE on success
 *
 * Abort:   none
 *
 * Notes:
 *
 **************/
global enum eQCWWANError
qm_dcs_get_usb_port_names_response_unpack(
    swi_uint8 *prsp,
    struct qm_qmi_response_tlvs_values *prsptlvs )
{
    struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,
          qm_result_code_tlv_unpack },

        { eTLV_DCS_GET_USB_PORT_NAME_ATCMDPORT,
          qm_dcs_get_usb_port_names_atcmdport_unpack_tlv
        },

        { eTLV_DCS_GET_USB_PORT_NAME_NMEAPORT,
          qm_dcs_get_usb_port_names_nmeaport_unpack_tlv
        },

        { eTLV_DCS_GET_USB_PORT_NAME_DMPORT,
          qm_dcs_get_usb_port_names_dmport_unpack_tlv
        },

        { eTLV_TYPE_INVALID, NULL }
    };

    return qmunpack( prsp,
                     (swi_uint8 *)prsptlvs,
                     map,
                     eQMI_DCS_GET_USB_PORT_NAMES );
}

/*************
 *
 * Name:    qm_dcs_get_device_mode_unpack_tlv
 *
 * Purpose: Unpack Device Mode TLV Value
 *
 * Parms:   (IN)    psource - source data
 *          (OUT)   pdest   - destination storage
 *
 * Return:  eQCWWAN_ERR_NONE on success
 *
 * Abort:   none
 *
 * Notes:
 *
 **************/
local enum eQCWWANError
qm_dcs_get_device_mode_unpack_tlv(
    swi_uint8 *psource,
    swi_uint8 *pdest )
{
    struct qm_qmi_response_tlvs_values *pin;
    struct qm_dcs_device_state_change_tlv_values *pval;

    pin = (struct qm_qmi_response_tlvs_values *)pdest;
    pval = (struct qm_dcs_device_state_change_tlv_values *)&pin->tlvvalues.qmdcstlvs.devicestate;

    enum eQCWWANError rc;
    rc = GetByte( psource, &(pval->devstate));

    return rc;
}

local enum eQCWWANError
qm_dcs_read_map_unpack_tlv(
    swi_uint8 *psource,
    swi_uint8 *pdest )
{
    struct qm_qmi_response_tlvs_values *pin;
    struct qm_dcs_qos_map_tlv_values *pval;

    pin = (struct qm_qmi_response_tlvs_values *)pdest;
    pval = (struct qm_dcs_qos_map_tlv_values *)&pin->tlvvalues.qmdcstlvs.qosMap;

    enum eQCWWANError rc;
    rc = GetLong( psource, &(pval->qos_id));

    return rc;
}

local enum eQCWWANError
qm_dcs_dump_map_unpack_tlv(
    swi_uint8 *psource,
    swi_uint8 *pdest )
{
    int i;
    struct qm_qmi_response_tlvs_values *pin;
    struct qm_dcs_qos_map_tlv_values *pval;

    pin = (struct qm_qmi_response_tlvs_values *)pdest;
    pval = (struct qm_dcs_qos_map_tlv_values *)&pin->tlvvalues.qmdcstlvs.qosMaps[0];

    enum eQCWWANError rc;

    for(i=0;i<MAX_QOS_MAP;i++)
    {
        rc = GetByte( psource, &(pval[i].dscp));
        if (rc != eQCWWAN_ERR_NONE)
            break;
        rc = GetLong( psource, &(pval[i].qos_id));
        if (rc != eQCWWAN_ERR_NONE)
            break;
        rc = GetByte( psource, &(pval[i].state));
        if (rc != eQCWWAN_ERR_NONE)
            break;
    }

    return rc;
}

/*************
 *
 * Name:    qm_dcs_Start_Srv_unpack_tlv
 *
 * Purpose: Unpack Device Mode TLV Value
 *
 * Parms:   (IN)    psource - source data
 *          (OUT)   pdest   - destination storage
 *
 * Return:  eQCWWAN_ERR_NONE on success
 *
 * Abort:   none
 *
 * Notes:
 *
 **************/
local enum eQCWWANError
qm_dcs_Start_Srv_unpack_tlv(
    swi_uint8 *psource,
    swi_uint8 *pdest )
{
    struct qm_qmi_response_tlvs_values *pin;
    struct qm_dcs_start_srv_tlv_values *pval;

    pin = (struct qm_qmi_response_tlvs_values *)pdest;
    pval = &pin->tlvvalues.qmdcstlvs.StartSrv;

    enum eQCWWANError rc;
    rc = GetByte( psource, &(pval->srvStatus));

    return rc;
}

/*************
 *
 * Name:    qm_dcs_get_device_mode_response_unpack
 *
 * Purpose: Unpack DCS SLQSGetDeviceMode response
 *
 * Parms:   (IN)    prsp        - response data
 *          (OUT)   prsptlvs    - destination response parameters structure
 *
 * Return:  eQCWWAN_ERR_NONE on success
 *
 * Abort:   none
 *
 * Notes:
 *
 **************/
global enum eQCWWANError
qm_dcs_get_device_mode_response_unpack(
    swi_uint8 *prsp,
    struct qm_qmi_response_tlvs_values *prsptlvs )
{
    struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,
          qm_result_code_tlv_unpack },

        { eTLV_DCS_GET_DEVICE_MODE,
          qm_dcs_get_device_mode_unpack_tlv
        },

        { eTLV_TYPE_INVALID, NULL }
    };

    return qmunpack( prsp,
                     (swi_uint8 *)prsptlvs,
                     map,
                     eQMI_DCS_GET_DEVICE_MODE);
}

/*************
 *
 * Name:    qm_dcs_Start_Srv_response_unpack
 *
 * Purpose: Unpack DCS SLQSGetDeviceMode response
 *
 * Parms:   (IN)    prsp        - response data
 *          (OUT)   prsptlvs    - destination response parameters structure
 *
 * Return:  eQCWWAN_ERR_NONE on success
 *
 * Abort:   none
 *
 * Notes:
 *
 **************/
global enum eQCWWANError
qm_dcs_start_srv_response_unpack(
    swi_uint8 *prsp,
    struct qm_qmi_response_tlvs_values *prsptlvs )
{
    struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,
          qm_result_code_tlv_unpack },

        { eTLV_DCS_START_SRV,
          qm_dcs_Start_Srv_unpack_tlv
        },

        { eTLV_TYPE_INVALID, NULL }
    };

    return qmunpack( prsp,
                     (swi_uint8 *)prsptlvs,
                     map,
                     eQMI_DCS_GET_DEVICE_MODE);
}

global enum eQCWWANError
qm_dcs_qos_map_response_unpack(
    swi_uint8 *prsp,
    struct qm_qmi_response_tlvs_values *prsptlvs )
{
    struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,
          qm_result_code_tlv_unpack },

        { eTLV_TYPE_INVALID, NULL }
    };

    return qmunpack( prsp,
                     (swi_uint8 *)prsptlvs,
                     map,
                     eQMI_DCS_QOS_MAP);
}

//DRY
global enum eQCWWANError
qm_dcs_qos_edit_map_response_unpack(
    swi_uint8 *prsp,
    struct qm_qmi_response_tlvs_values *prsptlvs )
{
    struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,
          qm_result_code_tlv_unpack },

        { eTLV_TYPE_INVALID, NULL }
    };

    return qmunpack( prsp,
                     (swi_uint8 *)prsptlvs,
                     map,
                     eQMI_DCS_QOS_EDIT_MAP);
}

global enum eQCWWANError
qm_dcs_qos_read_map_response_unpack(
    swi_uint8 *prsp,
    struct qm_qmi_response_tlvs_values *prsptlvs )
{
    struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,
          qm_result_code_tlv_unpack },

        { eTLV_DCS_READ_MAP,
          qm_dcs_read_map_unpack_tlv
        },

        { eTLV_TYPE_INVALID, NULL }
    };

    return qmunpack( prsp,
                     (swi_uint8 *)prsptlvs,
                     map,
                     eQMI_DCS_QOS_READ_MAP);
}

global enum eQCWWANError
qm_dcs_qos_dump_map_response_unpack(
    swi_uint8 *prsp,
    struct qm_qmi_response_tlvs_values *prsptlvs )
{
    struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,
          qm_result_code_tlv_unpack },

        { eTLV_DCS_DUMP_MAP,
          qm_dcs_dump_map_unpack_tlv
        },

        { eTLV_TYPE_INVALID, NULL }
    };

    return qmunpack( prsp,
                     (swi_uint8 *)prsptlvs,
                     map,
                     eQMI_DCS_QOS_DUMP_MAP);
}

global enum eQCWWANError
qm_dcs_qos_unmap_response_unpack(
    swi_uint8 *prsp,
    struct qm_qmi_response_tlvs_values *prsptlvs )
{
    struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,
          qm_result_code_tlv_unpack },

        { eTLV_TYPE_INVALID, NULL }
    };

    return qmunpack( prsp,
                     (swi_uint8 *)prsptlvs,
                     map,
                     eQMI_DCS_QOS_UNMAP);
}

global enum eQCWWANError
qm_dcs_qos_clear_map_response_unpack(
    swi_uint8 *prsp,
    struct qm_qmi_response_tlvs_values *prsptlvs )
{
    struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,
          qm_result_code_tlv_unpack },

        { eTLV_TYPE_INVALID, NULL }
    };

    return qmunpack( prsp,
                     (swi_uint8 *)prsptlvs,
                     map,
                     eQMI_DCS_QOS_CLEAR_MAP);
}

