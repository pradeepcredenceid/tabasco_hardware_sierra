/*
 *       Copyright (C) 2013-2014 Intel Mobile Communications GmbH
 *
 *            Sec Class: Intel Confidential (IC)
 *
 *
 * This document contains proprietary information belonging to IMC.
 * Design A/S. Passing on and copying of this document, use and communication
 * of its contents is not permitted without prior written authorisation.
 *
 * Description:
 *   <hardcoded file description - the only part to be modified manually!>
 *
 * Revision Information:
 *  $File name:  /msw_tools/FlashTool/libDownloadTool/src/OS_dependent_code/security_if/IFWD_sec_channel.h $
 *  $CC-Version: .../dhkristx_140314_v0/1 $
 *  $Date:       2014-04-23    10:54:26 UTC $
 *   Comment:
 *     SMS00847521: "Need to remove Lint warnings..."
 */

#ifndef IFWD_sec_channelH
#define IFWD_sec_channelH

/****************************************************************************/
/* INCLUDES                                                                 */
/****************************************************************************/
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "signing_module.h"
//#include "sm_bastypes.h"

#include "uta_secure_channel_tags.h"
#include "crypto_sw_hash.h"
#include "IFWD_sec_channel.h"

/****************************************************************************/
/* DEFINES                                                                  */
/****************************************************************************/
#define UCHR(x) ((unsigned char)(x))
#define UI16(x) ((uint16_t)(x))
#define UI32(x) ((uint32_t)(x))

#define BIG_UINT16_TO_UCHARS(A,B) {(A)[0]=(B)>>8&0xFF;(A)[1]=(B)&0xFF;}
#define BIG_UCHARS_TO_UINT16(A) (UI16((A)[0])<<8|UI16((A)[1]))
#define BIG_UINT32_TO_UCHARS(A,B) {(A)[0] =(B)>>24&0xFF;(A)[1]=(B)>>16&0xFF;(A)[2]=(B)>>8&0xFF;(A)[3]=(B)&0xFF;}
#define BIG_UCHARS_TO_UINT32(A) (UI32((A)[0])<<24|UI32((A)[1])<<16|UI32((A)[2])<<8|UI32((A)[3]))

#define LIT_UINT16_TO_UCHARS(A,B) {(A)[1]=((B)>>8)&0xFF;(A)[0]=(B)&0xFF;}
#define LIT_UCHARS_TO_UINT16(A) (UI16((A)[1])<<8|UI16((A)[0]))
#define LIT_UINT32_TO_UCHARS(A,B) {(A)[3]=(B)>>24&0xFF;(A)[2]=(B)>>16&0xFF;(A)[1]=(B)>>8&0xFF;(A)[0]=(B)&0xFF;}
#define LIT_UCHARS_TO_UINT32(A) (UI32((A)[3])<<24|UI32((A)[2])<<16|UI32((A)[1])<<8|UI32((A)[0]))


// For certificate TLV
#define FLASH_BLOCK_GROUP 0xAAAAAAAA
#define CERT_BLOCK_GROUP  0x80000000

#define RND_DATA          0x00000004
#define MID_DATA          0x00000008
#define KEY_SEC_CH_DATA   0x00000010

#define KEY_CERT_ROOT     0x82000000
#define MID_GRP_TYPE      0x9999
#define RND_GRP_TYPE      0x8888

#define SEC_HWID_SIZE 16
#define SEC_MAX_SIZE_KEY_CERT 0x560
#define SEC_MAX_SIZE_SIGNATURE 0x100

// Definition of data types.
#define SECURE_CH_DATA_FREE_RETURN_DATA 0x00000000
#define SECURE_CH_DATA_SNUM 			0x00000001
#define SECURE_CH_DATA_REQ 				0x00000002
#define SECURE_CH_DATA_REPLY 			0x00000003
// Allow the customer to add types
#define SECURE_CH_CB_CUST_TYPE_01 		0x00000800
#define SECURE_CH_CB_CUST_TYPE_02 		0x00000801
#define SECURE_CH_CB_CUST_TYPE_03 		0x00000802
#define SECURE_CH_CB_CUST_TYPE_04 		0x00000803
#define SECURE_CH_CB_CUST_TYPE_05 		0x00000804
#define SECURE_CH_CB_CUST_TYPE_06 		0x00000805
#define SECURE_CH_CB_CUST_TYPE_07 		0x00000806
#define SECURE_CH_CB_CUST_TYPE_08 		0x00000807
#define SECURE_CH_CB_CUST_TYPE_09 		0x00000808
#define SECURE_CH_CB_CUST_TYPE_10 		0x00000809
#define SECURE_CH_CB_CUST_TYPE_11 		0x0000080A
#define SECURE_CH_CB_CUST_TYPE_12 		0x0000080B
#define SECURE_CH_CB_CUST_TYPE_13 		0x0000080C
#define SECURE_CH_CB_CUST_TYPE_14 		0x0000080D
#define SECURE_CH_CB_CUST_TYPE_15 		0x0000080E
#define SECURE_CH_CB_CUST_TYPE_16 		0x0000080F


// To allow the init function to report errors ... add if more specific errors are needed.
#define SECURE_CH_RES_SUCCESS           0x00000000
#define SECURE_CH_RES_ERROR             0x00000001
#define SECURE_CH_RES_INVALID_PARAM     0x00000002
#define SECURE_CH_RES_MALLOC_FAILED     0x00000003
#define SECURE_CH_RES_SIGNMOD_ERROR     0x00000004
#define SECURE_CH_RES_FILE_ERROR        0x00000005
#define SECURE_CH_RES_INVALID_REQ_ID    0x00000006

#define SEC_CHANNEL_HEADER_SIZE 4



/****************************************************************************/
/* TYPE DEFINITIONS                                                         */
/****************************************************************************/

typedef struct
{
  FILE *log_lowlevel; // Detailed logging.
  FILE *log_highlevel; // Highlevel logging.
} T_LOG_CONTEXT;

// Defining the IFX standard private key format.

typedef struct
{
  uint32_t KeyLength;      // The Length of RSA Exponent & Modulus in bits.
  uint8_t  Exponent[256];  // The Exponent for RSA Encryption/Decryption.
  uint8_t  Modulus[256];   // The Modulus for RSA Encryption/Decryption.
} T_BASIC_KEY;

typedef struct
{
  uint32_t PrivateSignature[5];    // SHA-1 of private key structure. i.e. Hash of ( KeyMarker + Key + PublicSignature)
  uint32_t KeyType;                // The Key Type (indicates whether key used during production, development etc.)
  uint32_t KeyMarker;              // Magic number for quick detection of possible key presence.
  T_BASIC_KEY Key;            // Points to structure that contains Exponent and Modulus.
  uint32_t PublicSignature[5];     // SHA-1 of public key structure.
} T_PRIVATE_KEY;

typedef struct{
   uint32_t KeyType;
   uint32_t KeyLength;
   uint32_t Exponent;
   uint8_t Modulus[256];
   uint8_t Montgom[256];
}T_PUBLIC_KEY;

// Defining the structure of the HW details.
typedef struct
{
  uint8_t hwid_baseband[16];
  uint8_t hwid_flashchip[16];
  uint8_t hash_gpuk[20];
  uint8_t hash_mpuk[20];
  uint8_t hash_spuk[20];
} T_HW_DETAILS;

typedef struct
{
  T_LOG_CONTEXT *log;
  T_SM_CONTEXT sm_context;
} T_SIGN_CONTEXT;

typedef enum
{
  T_TLV_16_BIT = 2, /**< Type field is 16 bit. Length field is 16 bit.*/
  T_TLV_32_BIT = 4 /**< Type field is 32 bit. Length field is 32 bit.*/
} T_TLV_SIZE_MODE;

typedef struct {
  T_TLV_SIZE_MODE root_size;
  uint32_t root_id;
  uint16_t root_offset;
} T_SECCH_CERT_TLV_INFO;

typedef struct {
  uint32_t flash_tlv_id;
  uint32_t sm_block_type;
  T_SECCH_CERT_TLV_INFO cert_tlv;
} T_SECCH_CERT_BLK_INFO;

typedef struct {
  uint32_t flash_tlv_id;
  uint8_t *data;
  uint32_t length;
} T_SECCH_CERT_ELM;

typedef struct T_SECCH_LIST_ELM{
  T_SECCH_CERT_ELM certificate;
  struct T_SECCH_LIST_ELM *next_elem;
} T_SECCH_LIST_ELM;

/* supported secure channel request types */
typedef enum UtaSecChannelReqIdEnum{
  UTA_SEC_SIM_VALIDATE=1,
  UTA_SEC_VERSION_VALIDATE
}UtaSecChannelReqIdEnum;

typedef uint32_t UtaSecChannelReqId;

typedef enum T_SEC_CERT_BLOCK{
  SEC_CERT_INJECT=1,
  SEC_CERT_AREA_READ,
  SEC_CERT_AREA_FREE
}T_SEC_CERT_BLOCK;

/****************************************************************************/
/* EXPORTED FUNCTIONS                                                       */
/****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

extern void GLOB_log_information(T_LOG_CONTEXT *ctx, uint8_t loglvl, char *prefix, char* format, ...);
extern uint8_t SIGN_create_context(T_SIGN_CONTEXT *sign, T_HW_DETAILS *hw_details, T_PRIVATE_KEY *private_key);
extern uint8_t SIGN_delete_context(T_SIGN_CONTEXT *sign);
extern uint32_t SIGN_make_key_certificate(T_SIGN_CONTEXT *sign,uint8_t bind_to_hwid,T_PRIVATE_KEY *cprk,uint32_t cprk_size,T_PUBLIC_KEY *cpuk,uint32_t cpuk_size,uint32_t *keyswap_list,uint32_t keyswap_list_size,uint8_t *out_data,uint32_t *out_size,uint32_t max_size);
extern uint32_t SIGN_make_generic_signature(T_SIGN_CONTEXT *sign, uint8_t pkcs_block_type, uint8_t emsa_pkcs_type, uint8_t hash_type, uint8_t *in_data, uint32_t in_size, uint8_t *out_data, uint32_t *out_size, uint32_t max_size);

extern uint32_t sec_ch_setup(uint32_t options, char ** cert_files);
extern uint32_t sec_ch_callback(uint32_t *data_type, size_t *data_length, uint8_t **data_ptr_ptr);

extern uint32_t sec_generate_cert_block(T_SEC_CERT_BLOCK type_id, size_t *data_length, uint8_t **data_ptr_ptr);

#ifdef __cplusplus
}
#endif

#endif  /* from #ifndef IFWD_sec_channelSH */
