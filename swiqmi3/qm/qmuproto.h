/*************
 *
 * Filename: qmuproto.h
 *
 * Purpose:  External prototypes for qm package
 *
 * Copyright: © 2011 Sierra Wireless Inc., all rights reserved
 *
 **************/

#ifndef QMUPROTO_H
#define QMUPROTO_H

/*-------------
  include files
 --------------*/
#include "qm/qmdcs.h"
#include "qm/qmqmisvc.h"

/* qm_api.c */
extern swi_bool   qminitapi(void);
extern swi_uint8* qmgetreqbkp(void);
extern swi_uint32 qmgetreqbksz(void);
extern swi_uint8* qmgetrespbkp(void);
extern swi_uint32 qmgetrespbksz(void);
extern swi_uint32 qmgetappparm(void);
extern void       qmrelreqbkp( void );

/* qmcommon.c */
extern enum eQCWWANError PokeByte(swi_uint8 *bufp, swi_uint16 index, swi_uint8 value);
extern enum eQCWWANError PokeWord(swi_uint8 *bufp, swi_uint16 index, swi_uint16 value);
extern enum eQCWWANError IndexU(swi_uint8 *bufp, swi_uint16 index);
extern enum eQCWWANError PutByte(swi_uint8 *bufp, swi_uint8 value);
extern enum eQCWWANError PutWord(swi_uint8 *bufp, swi_uint16 value);
extern enum eQCWWANError PutLong(swi_uint8 *bufp, swi_uint32 value);
extern enum eQCWWANError PutByteBe(swi_uint8 *bufp, swi_uint8 value);
extern enum eQCWWANError PutWordBe(swi_uint8 *bufp, swi_uint16 value);
extern enum eQCWWANError PutLongBe(swi_uint8 *bufp, swi_uint32 value);
extern enum eQCWWANError Put64(swi_uint8 *bufp, swi_uint64 value);
extern enum eQCWWANError PutStream(swi_uint8 *bufp, swi_uint8 *value, swi_uint32 streamlen);
extern enum eQCWWANError GetStream(swi_uint8 *bufp, swi_uint8 *value, swi_uint32 streamlen);
extern enum eQCWWANError GetByte(swi_uint8 *bufp, swi_uint8 *valuep);
extern enum eQCWWANError GetWord(swi_uint8 *bufp, swi_uint16 *valuep);
extern enum eQCWWANError GetLong(swi_uint8 *bufp, swi_uint32 *valuep);
extern enum eQCWWANError GetByteBe(swi_uint8 *bufp, swi_uint8 *valuep);
extern enum eQCWWANError GetWordBe(swi_uint8 *bufp, swi_uint16 *valuep);
extern enum eQCWWANError GetLongBe(swi_uint8 *bufp, swi_uint32 *valuep);
extern enum eQCWWANError GetLongLong(swi_uint8 *bufp, swi_uint64 *valuep);
extern enum eQCWWANError GetStringLen(swi_uint8 *bufp, swi_uint16 *valuep);
extern enum eQCWWANError GetTlvType( swi_uint8 *bufp, swi_uint8 *valuep );

extern enum eQCWWANError qmQmiVerifyResponseTLVlength(
    struct qmTBuffer *bufp,
    swi_uint16 TlvLength);

extern swi_uint16 qmQmiGetResponseTLVlength(
    struct qmTBuffer *bufp );

extern enum eQCWWANError qmQmiExtractString(
    swi_uint8 *pTlvData,
    swi_char  *pInBuffer,
    swi_uint32 lengthBuffer );

extern enum eQCWWANError qmbuild(
    swi_uint8 *bufp,
    swi_uint8 *reqp,
    struct qmTlvBuilderItem *itemp,
    const swi_uint16 messageId,
    swi_uint16 *mLengthp);

extern enum eQCWWANError qmunpackresp(
    swi_uint8 *bufp,
    swi_uint8 *resp,
    struct qmTlvUnpackerItem *itemp,
    const swi_uint16 messageId);

enum eQCWWANError tlv_unpack(
    swi_uint8 *bufp,
    swi_uint16 buflen,
    swi_uint8 *resp,
    struct qmTlvUnpackerItem *pItem);

extern enum eQCWWANError qmUnpackTlvResultCode(
    swi_uint8 *TlvDatap,
    swi_uint8 *resp);

extern enum eQCWWANError qm_result_code_tlv_unpack(
    swi_uint8 *psrc,
    swi_uint8 *pdest );

typedef enum eQCWWANError (*qmTlvBuilder)(swi_uint8*, swi_uint8*);
typedef enum eQCWWANError (*qmTlvUnpacker)(swi_uint8*, swi_uint8*);

extern void
qm_dcs_event_notification_send(
    struct qmTlvBuilderItem *pmap,
    struct qm_qmi_response_tlvs_values *prsptlvs );

void qm_dcs_net_notify( swi_uint32 q_depth, swi_uint8 isThrottle );

/* qmdcscommon.c */
extern enum eQCWWANError
qm_dcs_enumerate_devices_response_unpack(
    swi_uint8 *prsp,
    struct    qm_qmi_response_tlvs_values *prsptlvs );

global enum eQCWWANError
qm_dcs_connect_response_unpack(
    swi_uint8 *prsp,
    struct qm_qmi_response_tlvs_values *prsptlvs );

extern enum eQCWWANError
qm_dcs_connect_request_pack(
    swi_char  *pdevnode,
    swi_char  *pdevkey,
    swi_uint8 *preqbuf,
    swi_uint16 *plen );

extern enum eQCWWANError
qm_dcs_get_connected_device_id_response_unpack(
    swi_uint8 *prsp,
    struct    qm_qmi_response_tlvs_values *prsptlvs );

extern enum eQCWWANError
qm_dcs_get_usb_port_names_response_unpack(
    swi_uint8 *prsp,
    struct qm_qmi_response_tlvs_values *prsptlvs );

extern enum eQCWWANError
qm_dcs_get_device_mode_response_unpack(
    swi_uint8 *prsp,
    struct qm_qmi_response_tlvs_values *prsptlvs );

extern global enum eQCWWANError
qm_dcs_start_srv_response_unpack(
    swi_uint8 *prsp,
    struct qm_qmi_response_tlvs_values *prsptlvs );

global enum eQCWWANError
qm_dcs_qos_map_response_unpack(
    swi_uint8 *prsp,
    struct qm_qmi_response_tlvs_values *prsptlvs );

global enum eQCWWANError
qm_dcs_qos_unmap_response_unpack(
    swi_uint8 *prsp,
    struct qm_qmi_response_tlvs_values *prsptlvs );

global enum eQCWWANError
qm_dcs_qos_clear_map_response_unpack(
    swi_uint8 *prsp,
    struct qm_qmi_response_tlvs_values *prsptlvs );

global enum eQCWWANError
qm_dcs_qos_edit_map_response_unpack(
    swi_uint8 *prsp,
    struct qm_qmi_response_tlvs_values *prsptlvs );

global enum eQCWWANError
qm_dcs_qos_read_map_response_unpack(
    swi_uint8 *prsp,
    struct qm_qmi_response_tlvs_values *prsptlvs );

global enum eQCWWANError
qm_dcs_qos_dump_map_response_unpack(
    swi_uint8 *prsp,
    struct qm_qmi_response_tlvs_values *prsptlvs );

/* qmfmscommon.c */
extern enum eQCWWANError
qm_fms_set_firmware_preference_request_pack(
    swi_uint8   *preq,
    swi_uint16  *plen );

extern enum eQCWWANError
qm_fms_enable_force_download_request_pack(
    swi_uint8   *preq,
    swi_uint16  *plen );

extern enum eQCWWANError
qm_fms_set_image_path_request_pack(
    swi_uint16  imgMask,
    swi_uint32  isMDM9x15,
    swi_uint16  mdmProtocol,
    swi_char    *path,
    swi_uint8   *preq,
    swi_uint16  *plen );

extern enum eQCWWANError
qm_fms_get_cwe_spkgs_info_request_pack(
    swi_uint8       imgType,
    const swi_char  *path,
    swi_uint8       *preq,
    swi_uint16      *plen );

extern enum eQCWWANError
qm_fms_get_cwe_spkgs_info_response_unpack(
    swi_uint8 *prsp,
    struct qm_qmi_response_tlvs_values *prsptlvs );

extern enum eQCWWANError
qm_fms_get_mbn_info_request_pack(
    const swi_char  *path,
    const swi_uint8 imgtype,
    swi_uint8       *preq,
    swi_uint16      *plen );

extern enum eQCWWANError
qm_fms_get_mbn_info_response_unpack(
    swi_uint8 *prsp,
    struct qm_qmi_response_tlvs_values *prsptlvs );

/* qmcommon.c */
extern enum eQCWWANError
qm_result_code_tlv_unpack(
    swi_uint8 *psource,
    swi_uint8 *pdest );

enum eQCWWANError
qm_dcs_start_srv_pack(
    swi_char   action,
    swi_char   mask,
    swi_uint8  *preqbuf,
    swi_uint16 *plen );

enum eQCWWANError
qm_dcs_qos_map_pack(
    swi_char   dscp,
    swi_uint32 qos_id,
    swi_uint8  *preqbuf,
    swi_uint16 *plen );

enum eQCWWANError
qm_dcs_qos_unmap_pack(
    swi_char   dscp,
    swi_uint8  *preqbuf,
    swi_uint16 *plen );

enum eQCWWANError
qm_dcs_qos_clear_map_pack(
    swi_uint8  *preqbuf,
    swi_uint16 *plen );

enum eQCWWANError
qm_dcs_qos_edit_map_pack(
    swi_char   dscp,
    swi_uint32 qos_id,
    swi_uint8  *preqbuf,
    swi_uint16 *plen );

enum eQCWWANError
qm_dcs_qos_read_map_pack(
    swi_char   dscp,
    swi_uint8  *preqbuf,
    swi_uint16 *plen );

enum eQCWWANError
qm_dcs_qos_dump_map_pack(
    swi_uint8  *preqbuf,
    swi_uint16 *plen );

extern enum fw_image_type_e qmGetFileType(
    const swi_char *path );

/* qmtask_sdk.c */
extern void qminit(void);
extern void qmtaskinit(void);

extern void qmsendrr(
    swi_uint8 *qmimsgp,
    swi_uint8 *memfreep,
    swi_uint8 ipcchannel );

void qm_ds_net_notification(swi_uint32 q_depth, swi_uint8 isThrottle);

enum eQCWWANError tlvbuild(
    swi_uint8 *bufp,
    swi_uint8 *reqp,
    struct qmTlvBuilderItem *itemp,
    swi_uint16 *mLengthp);

#endif /* QMUPROTO_H */
