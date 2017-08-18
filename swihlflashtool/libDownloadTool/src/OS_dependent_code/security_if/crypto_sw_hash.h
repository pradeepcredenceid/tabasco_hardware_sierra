/* -------------------------------------------------------------------------
   Copyright (C) 2013-2014 Intel Mobile Communications GmbH

        Sec Class: Intel Confidential (IC)

   All rights reserved.
   -------------------------------------------------------------------------
   Revision Information:
      $File name:  /msw_tools/FlashTool/libDownloadTool/src/OS_dependent_code/security_if/crypto_sw_hash.h $
      $CC-Version: .../dhkristx_140314_v0/1 $
      $Date:       2014-04-23    10:54:29 UTC $
   ------------------------------------------------------------------------- */

/*****************************************************************************/
/* INCLUDES                                                                  */
/*****************************************************************************/

#ifndef __CRYPTO_SW_HASH_H__
#define __CRYPTO_SW_HASH_H__

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#include "IFWD_sec_channel.h"

//#include "bastypes.h"
//#include "ceu_drv.h"
//#include "tm_crypto.h"
//#include "cm_sw_crypto.h"
/**
    @addtogroup CM_LAYER
    @{
        @copydoc crypto_sw_hash.h
*/

/*****************************************************************************/
/* DEFINES                                                                   */
/*****************************************************************************/

/*****************************************************************************/
/* TYPE DEFINITIONS                                                          */
/*****************************************************************************/

#define  CEU_DRV_HASH_BLOCK_LEN         64
#define  CEU_DRV_MAX_HASH_IV_LEN        32 //max IV length (in bytes)
/* Max Key Length supported in CEU HMAC */
#define  CEU_DRV_HMAC_MAX_KEY_LEN       64

/** Crypto SW driver return types */
typedef enum{
  CRYPTO_SW_RET_SUCCESS=0,                 /*!< Successful operation. */
  CRYPTO_SW_RET_FAIL,                            /*!< Default error. */
  CRYPTO_SW_RET_INPUT_PARAM_ERR,     /*!< Input arguments incorrect. */
  CRYPTO_SW_RET_UNKNOWN_ALGO_ERR, /*!< Unknown algorithms type. See \ref t_algo_type. */
  CRYPTO_SW_RET_KEY_ERR,                      /*!< Faulty key setup. */
  CRYPTO_SW_RET_KEY_IDENTIFIER_ERR  /*!< Key identifier unknown. */
}T_CRYPTO_SW_RESULT_ENUM;
typedef uint32_t T_CRYPTO_SW_RESULT;

/** Hash Algorithm's */
typedef enum
{
    CM_MD5  = 0,          /*!< Hash Algorithm is MD5 */
    CM_SHA1 = 1,          /*!< Hash Algorithm is SHA1 */
    CM_SHA2 = 2           /*!< Hash Algorithm is SHA2 */
} T_CM_HASH_ALGO ;

typedef struct SW_HASH_CONTEXT
{
    uint8_t                          m_residue_buf [CEU_DRV_HASH_BLOCK_LEN];
    uint32_t                         m_residue_len;
    uint32_t                         m_total_length;   // contains length hashed so far
    uint8_t                          m_iv_buf [CEU_DRV_MAX_HASH_IV_LEN];
    uint8_t                          m_opad_buf[CEU_DRV_HMAC_MAX_KEY_LEN];
    uint32_t                         m_digest_len; /*!< Len of hash digest(words) */
    T_CM_HASH_ALGO   m_hash_alg;
}T_SW_HASH_CONTEXT;

/** Algorithms types */
typedef enum
{
 algo_sha1, /*!< Sha1 hash algorithm. */
 algo_sha256 /*!< Sha256 hash algorithm. */
}t_algo_type;

/** Hashing context */
typedef struct
{
 uint32_t iv[8];
 uint32_t iv_init;
 uint8_t buffer[128];
 uint32_t used_buffer;
 uint32_t total_length[2];
 t_algo_type algo;
 uint32_t force_use_sw;
 uint32_t digest_size;
}t_hash_context;

/** Hmacing context */
typedef struct
{
 uint8_t key_opad[64];
 t_hash_context hash_context;
}t_hmac_context;

/*****************************************************************************/
/* LOCAL  FUNCTIONS                                                                 */
/*****************************************************************************/


/*****************************************************************************/
/* EXTERNAL FUNCTIONS                                                                 */
/*****************************************************************************/
/**
 @brief      Service API to get hash digest size in bytes.

 @param[in]  hash_alg Algorithms type used.

 @par        Description.
                The function returns the digest length in bytes based on the algorithm type.

 @return     Returns digest length on success else 0.

*/
uint32_t crypto_sw_hash_get_size(T_CM_HASH_ALGO hash_alg);

/**
 @brief      Service API to initialte the hash operation.

 @param[in]  context Pointer to hash context variable, allocated by the caller.

 @param[in]  hash_alg Algorithms type used.

 @par        Description.
                The function inititiates the the hash operation and setup the context.


 @return     Returns CRYPTO_SW_RET_SUCCESS on success else errorcode.

*/
T_CRYPTO_SW_RESULT crypto_sw_hash_init(T_SW_HASH_CONTEXT *context, T_CM_HASH_ALGO hash_alg);

/**
 @brief      Service API to partial hash data.

 @param[in]  context Pointer to hash context variable, allocated by the caller.

 @param[in]  data Pointer to data, which shall be hashed.

 @param[in]  length Size of data in bytes

 @par        Description.
                The function is capable of hashing data seperated into smaller blocks. To improve hashing performace, use as large
                blocks as possible and make sure data is 4 byte aligned. No max data size nor alignement requirement.

 @return     Returns CRYPTO_SW_RET_SUCCESS on success else errorcode.

*/
T_CRYPTO_SW_RESULT crypto_sw_hash_update(T_SW_HASH_CONTEXT * context, uint8_t * data, uint32_t length);

/**
 @brief      Service API to finalise the hash operation.

 @param[in]  context Pointer to hash context variable, allocated by the caller.

 @param[out]  hash Pointer to hash output. Data is allocated by caller.

 @par        Description.
                The function finalises the hashing operation and returns hash calulated on success.

 @return     Returns CRYPTO_SW_RET_SUCCESS on success else errorcode.

*/
T_CRYPTO_SW_RESULT crypto_sw_hash_final(T_SW_HASH_CONTEXT * context, uint8_t * hash);

/*
 @brief      Service API to get hmac digest size in bytes.

 @param[in]  type Algorithms type used.

 @par        Description.
                The function returns the digest length in bytes based on the algorithm type.

 @return     Returns digest length on success else 0.

*/
//uint32_t crypto_sw_hmac_get_size(T_CM_HASH_ALGO hash_alg);

/*
 @brief      Service API to initialte the hmac operation.

 @param[in]  context Pointer to hmac context variable, allocated by the caller.

 @param[in]  key Pointer to key used in hmac calculation.

 @param[in]  key_length Size of key in bytes.

 @param[in]  type Algorithms type used.

 @par        Description.
                The function inititiates the the hmac operation and setup the context.


 @return     Returns CRYPTO_SW_RET_SUCCESS on success else errorcode.

*/
T_CRYPTO_SW_RESULT crypto_sw_hmac_init(T_SW_HASH_CONTEXT *context,uint8_t* key, uint32_t key_length, T_CM_HASH_ALGO hash_alg);

/**
 @brief      Service API to partial hmac data.

 @param[in]  context Pointer to hmac context variable, allocated by the caller.

 @param[in]  data Pointer to data, which shall be hashed.

 @param[in]  length Size of data in bytes

 @par        Description.
                The function is capable of hmacing data seperated into smaller blocks. To improve hmacing performace, use as large
                blocks as possible and make sure data is 4 byte aligned. No max data size nor alignement requirement.

 @return     Returns CRYPTO_SW_RET_SUCCESS on success else errorcode.

*/
T_CRYPTO_SW_RESULT crypto_sw_hmac_update(T_SW_HASH_CONTEXT * context, uint8_t * data, uint32_t length);

/**
 @brief      Service API to finalise the hmac operation.

 @param[in]  context Pointer to hash context variable, allocated by the caller.

 @param[out]  hmac Pointer to hmac output. Data is allocated by caller.

 @par        Description.
                The function finalises the hmacing operation and returns hmac calulated on success.

 @return     Returns CRYPTO_SW_RET_SUCCESS on success else errorcode.

*/
T_CRYPTO_SW_RESULT crypto_sw_hmac_final(T_SW_HASH_CONTEXT * context, uint8_t * hmac);

/**
 @brief      Service API to perform RSA public key operation.

 @param[in]  context Pointer to RSA context variable, allocated by the caller.

 @param[in] data_in  Pointer to input data. Data_in must have the same size as the key length.

 @param[out] data_out  Pointer to output data. Data_out must have the same size as the key length, and be allocated by the caller.

 @param[in] key  Pointer to the public key.

 @par        Description.
                The function performs a public key operation returning the result in data_out.

 @return     Returns CRYPTO_SW_RET_SUCCESS on success else errorcode.

*/
/** @} */

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif

