/* -------------------------------------------------------------------------
   Copyright (C) 2013-2014 Intel Mobile Communications GmbH

        Sec Class: Intel Confidential (IC)

   All rights reserved.
   -------------------------------------------------------------------------
   Revision Information:
      $File name:  /msw_tools/FlashTool/libDownloadTool/src/OS_dependent_code/security_if/crypto_sw_hash_core.h $
      $CC-Version: .../dhkristx_140314_v0/1 $
      $Date:       2014-04-23    10:54:33 UTC $
   ------------------------------------------------------------------------- */

#ifndef _CRYPTO_SW_HASH_CORE_H_
#define _CRYPTO_SW_HASH_CORE_H_

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


/**
    @addtogroup CM_LAYER
    @{
        @copydoc crypto_sw_hash.h
*/
//#include "bastypes.h"
#include "crypto_sw_hash.h"
//#include "tm_crypto.h"
#define LIT_TO_BIG(x) ((((x)&0xff)<<24)|(((x)&0xff00)<<8)|(((x)&0xff0000)>>8)|(((x)&0xff000000)>>24))

/**
 @brief      Service API hash blocks of data using the sha1 algorithm.

 @param[in]  context Pointer hash context.

 @param[in]  message Pointer to data, which shall be hashed.

 @param[in]  blocks Number of blocks to hash.

 @par        Description.
                The function hashes the message using the sha1 algorithm. The size hashed in bytes is 64*blocks.

 @return     Returns CRYPTO_SW_RET_SUCCESS on success else errorcode.

*/
T_CRYPTO_SW_RESULT sha1_sw_engine(T_SW_HASH_CONTEXT * context, uint32_t * message, uint32_t blocks);

/**
 @brief      Service API hash blocks of data using the sha256 algorithm.

 @param[in]  context Pointer hash context.

 @param[in]  message Pointer to data, which shall be hashed.

 @param[in]  blocks Number of blocks to hash.

 @par        Description.
                The function hashes the message using the sha256 algorithm. The size hashed in bytes is 64*blocks.

 @return     Returns CRYPTO_SW_RET_SUCCESS on success else errorcode.

*/
T_CRYPTO_SW_RESULT sha256_sw_engine(T_SW_HASH_CONTEXT * context, uint32_t * message, uint32_t blocks);

/**
 @brief      Service API hash blocks of data using the sha256 algorithm.

 @param[in]  context Pointer hash context.

 @param[in]  message Pointer to data, which shall be hashed.

 @param[in]  blocks Number of blocks to hash.

 @par        Description.
                The function hashes the message using the md5 algorithm. The size hashed in bytes is 64*blocks.

 @return     Returns CRYPTO_SW_RET_SUCCESS on success else errorcode.

*/
T_CRYPTO_SW_RESULT md5_sw_engine(T_SW_HASH_CONTEXT * context, uint32_t * message, uint32_t blocks);
/** @} */

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif //
