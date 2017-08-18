/*
 *       Copyright (C) 2014 Intel Mobile Communications GmbH
 *
 *            Sec Class: Intel Confidential (IC)
 *
 *            All rights reserved.
 *
 *  -------------------------------------------------------------------------
 *   Revision Information:
 *      $File name:  /msw_tools/FlashTool/libFlsTool/src/sha256.h $
 *      $CC-Version: .../dhkristx_140314_v0/1 $
 *      $Date:       2014-04-16    12:20:34 UTC $
 *   ------------------------------------------------------------------------- */


/*****************************************************************************/
/* INCLUDES                                                                  */
/*****************************************************************************/

#ifndef __CRYPTO_SW_HASH_H__
#define __CRYPTO_SW_HASH_H__

#include <stdint.h>

#define  CEU_DRV_HASH_BLOCK_LEN         64
#define  CEU_DRV_MAX_HASH_IV_LEN        32 //max IV length (in bytes)

typedef struct SW_HASH_CONTEXT
{
    uint8_t                          m_residue_buf [CEU_DRV_HASH_BLOCK_LEN];
    uint32_t                         m_residue_len;
    uint32_t                         m_total_length;   // contains length hashed so far
    uint8_t                          m_iv_buf [CEU_DRV_MAX_HASH_IV_LEN];
    //uint8_t                          m_opad_buf[CEU_DRV_HMAC_MAX_KEY_LEN];
    uint32_t                         m_digest_len; /*!< Len of hash digest(words) */
    //T_CM_HASH_ALGO              m_hash_alg;
}T_SW_HASH_CONTEXT;


/*****************************************************************************/
/* DEFINES                                                                   */
/*****************************************************************************/

/*****************************************************************************/
/* TYPE DEFINITIONS                                                          */
/*****************************************************************************/

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


/*****************************************************************************/
/* EXTERNAL FUNCTIONS                                                                 */
/*****************************************************************************/

/**
 @brief      Service API to initialte the hash operation.

 @param[in]  context Pointer to hash context variable, allocated by the caller.

 @param[in]  hash_alg Algorithms type used.

 @par        Description.
                The function inititiates the the hash operation and setup the context.


 @return     Returns CRYPTO_SW_RET_SUCCESS on success else errorcode.

*/
T_CRYPTO_SW_RESULT crypto_sw_sha256_init(T_SW_HASH_CONTEXT *context);

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
T_CRYPTO_SW_RESULT crypto_sw_sha256_update(T_SW_HASH_CONTEXT * context, uint8_t * data, uint32_t length);

/**
 @brief      Service API to finalise the hash operation.

 @param[in]  context Pointer to hash context variable, allocated by the caller.

 @param[out]  hash Pointer to hash output. Data is allocated by caller.

 @par        Description.
                The function finalises the hashing operation and returns hash calulated on success.

 @return     Returns CRYPTO_SW_RET_SUCCESS on success else errorcode.

*/
T_CRYPTO_SW_RESULT crypto_sw_sha256_final(T_SW_HASH_CONTEXT * context, uint8_t * hash);
/** @} */
#endif

