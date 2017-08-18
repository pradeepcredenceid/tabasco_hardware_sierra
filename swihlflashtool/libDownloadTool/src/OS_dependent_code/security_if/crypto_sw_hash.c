/* -------------------------------------------------------------------------
   Copyright (C) 2013-2014 Intel Mobile Communications GmbH

        Sec Class: Intel Confidential (IC)

   All rights reserved.
   -------------------------------------------------------------------------
   Revision Information:
      $File name:  /msw_tools/FlashTool/libDownloadTool/src/OS_dependent_code/security_if/crypto_sw_hash.c $
      $CC-Version: .../oint_tools_flashtoole2_01/2 $
      $Date:       2014-03-13    15:42:12 UTC $
   ------------------------------------------------------------------------- */

/*****************************************************************************/
/* INCLUDES                                                                  */
/*****************************************************************************/

//#include "bastypes.h"
#include "crypto_sw_hash.h"
#include "string.h"

#include "crypto_sw_hash_core.h"


/*****************************************************************************/
/* DEFINES                                                                   */
/*****************************************************************************/
/** Hash defines */
#define SHA_BLOCK_SIZE 64
#define MD5_DIGEST_SIZE 16
#define SHA1_DIGEST_SIZE 20
#define SHA256_DIGEST_SIZE 32

/** Hmac defines */

#define IPAD 0x36
#define OPAD 0x5c

/*************************  LOCAL FUNCTIONS  ********************************/

/**
@addtogroup CRYPTO_SW_HASH
@{

*/

/**
 @brief      Local function used to hash data blocks.

 @param[in]  context Pointer to hashing context.

 @param[in] data Pionter to data.

 @param[in] blocks Number of blocks which should be hashed.

 @par        Description.
                The function dispatches the data to the correct hashing mode.

 @return     Returns CRYPTO_SW_RET_SUCCESS on success else errorcode.

*/

static T_CRYPTO_SW_RESULT process_blocks(T_SW_HASH_CONTEXT * context, uint8_t * data, uint32_t blocks)
{
    T_CRYPTO_SW_RESULT result=CRYPTO_SW_RET_FAIL;

    switch(context->m_hash_alg)
    {
        case CM_MD5:
            result=md5_sw_engine(context,(uint32_t *)(data),blocks);
            break;
        case CM_SHA1:
            result=sha1_sw_engine(context,(uint32_t *)(data),blocks);
            break;
        case CM_SHA2:
            result=sha256_sw_engine(context,(uint32_t *)(data),blocks);
            break;
        default:
            result = CRYPTO_SW_RET_UNKNOWN_ALGO_ERR;
            break;
    }
    return result;
}

/**
 @brief      Local function used to apply padding and to the last hashing block and process it.

 @param[in]  context Pointer to hashing context.

 @par        Description.
                The function applys padding and length to the residual buffer and processes it.

Returns CRYPTO_SW_RET_SUCCESS on success else errorcode.

*/
static T_CRYPTO_SW_RESULT process_last_message(T_SW_HASH_CONTEXT * context)
{
    uint32_t i;
    T_CRYPTO_SW_RESULT result;
    context->m_residue_buf[context->m_residue_len]=0x80; // apply 1 bit after message;

    for(i=(context->m_residue_len+1);i<SHA_BLOCK_SIZE;i++)
    {
      context->m_residue_buf[i]=0x00;
    }

    if(context->m_residue_len>=56)
    {
      if(CRYPTO_SW_RET_SUCCESS!=(result=process_blocks(context,context->m_residue_buf,1)))
      {
        return result;
      }
      memset(context->m_residue_buf,0x00,sizeof(context->m_residue_buf));
    }

    if(context->m_hash_alg==CM_MD5)
    {
      (((uint32_t* )context->m_residue_buf)[16-2])=(context->m_total_length<<3);
    }
    else
    {
      (((uint32_t* )context->m_residue_buf)[16-1])=LIT_TO_BIG(context->m_total_length<<3);
       //       (((uint32_t* )message)[16-2])=LIT_TO_BIG((length[1]<<3)|(length[0]>>(32-3)));
    }

    return process_blocks(context,context->m_residue_buf,1);
}
/** @} */

/*************************  EXTERNAL FUNCTIONS  *****************************/

/**************************************************************************
function: crypto_hash_get_size. See header for description.
***************************************************************************/

uint32_t crypto_sw_hash_get_size(T_CM_HASH_ALGO hash_alg)
{
  uint32_t length;

  switch(hash_alg)
  {
    case CM_MD5:
    {
      length = MD5_DIGEST_SIZE;
      break;
    }
    case CM_SHA1:
    {
      length = SHA1_DIGEST_SIZE;
      break;
    }
    case CM_SHA2:
    {
      length = SHA256_DIGEST_SIZE;
      break;
    }
    default:
    {
      length =0;
      break;
    }
  }
  return length;
}

/**************************************************************************
function: crypto_hash_init. See header for description.
***************************************************************************/

T_CRYPTO_SW_RESULT crypto_sw_hash_init(T_SW_HASH_CONTEXT *context, T_CM_HASH_ALGO hash_alg)
{
    T_CRYPTO_SW_RESULT result=CRYPTO_SW_RET_SUCCESS;

    if(NULL==context)
    {
      return CRYPTO_SW_RET_INPUT_PARAM_ERR;
    }

    context->m_total_length=0;
    context->m_residue_len=0;

    // reset context
    memset(context->m_residue_buf,0x00,sizeof(context->m_residue_buf));

    // set iv vectors
    switch(hash_alg)
    {
         case CM_MD5:
            ((uint32_t *)context->m_iv_buf)[0] = 0x67452301;
            ((uint32_t *)context->m_iv_buf)[1] = 0xEFCDAB89;
            ((uint32_t *)context->m_iv_buf)[2] = 0x98BADCFE;
            ((uint32_t *)context->m_iv_buf)[3] = 0x10325476;
            break;
        case CM_SHA1:
            ((uint32_t *)context->m_iv_buf)[0] = 0x67452301;
            ((uint32_t *)context->m_iv_buf)[1] = 0xEFCDAB89;
            ((uint32_t *)context->m_iv_buf)[2] = 0x98BADCFE;
            ((uint32_t *)context->m_iv_buf)[3] = 0x10325476;
            ((uint32_t *)context->m_iv_buf)[4] = 0xC3D2E1F0;
            break;
        case CM_SHA2:
            ((uint32_t *)context->m_iv_buf)[0] = 0x6a09e667;
            ((uint32_t *)context->m_iv_buf)[1] = 0xbb67ae85;
            ((uint32_t *)context->m_iv_buf)[2] = 0x3c6ef372;
            ((uint32_t *)context->m_iv_buf)[3] = 0xa54ff53a;
            ((uint32_t *)context->m_iv_buf)[4] = 0x510e527f;
            ((uint32_t *)context->m_iv_buf)[5] = 0x9b05688c;
            ((uint32_t *)context->m_iv_buf)[6] = 0x1f83d9ab;
            ((uint32_t *)context->m_iv_buf)[7] = 0x5be0cd19;
            break;
        default:
            result = CRYPTO_SW_RET_UNKNOWN_ALGO_ERR;
            break;
    }

    context->m_digest_len=crypto_sw_hash_get_size(hash_alg);
    context->m_hash_alg = hash_alg;

    return result;
}

/**************************************************************************
function: crypto_hash_update. See header for description.
**************************************************************************/
//lint -e426
T_CRYPTO_SW_RESULT crypto_sw_hash_update(T_SW_HASH_CONTEXT * context, uint8_t * data, uint32_t length)
{
    T_CRYPTO_SW_RESULT result=CRYPTO_SW_RET_SUCCESS;
    uint32_t remaining_bytes;
    uint8_t * pdata=data;

    if(0==length)
    {
      return CRYPTO_SW_RET_SUCCESS;
    }

    if(NULL==context||NULL==data)
    {
      return CRYPTO_SW_RET_INPUT_PARAM_ERR;
    }

    context->m_total_length+=length;
    remaining_bytes =length;

    if(context->m_residue_len)
    {
        if((context->m_residue_len+length)<SHA_BLOCK_SIZE)
        {
            memcpy(&context->m_residue_buf[context->m_residue_len],pdata,length);
            context->m_residue_len+=length;
            return CRYPTO_SW_RET_SUCCESS;
        }
        else
        {
            memcpy(&context->m_residue_buf[context->m_residue_len],data,SHA_BLOCK_SIZE-context->m_residue_len);
            remaining_bytes=remaining_bytes-(SHA_BLOCK_SIZE-context->m_residue_len);
            pdata=(uint8_t *)(pdata+(SHA_BLOCK_SIZE-context->m_residue_len));
            context->m_residue_len=0;
            if(CRYPTO_SW_RET_SUCCESS!=(result=process_blocks(context,context->m_residue_buf,1)))
            {
              return result;
            }
        }
    }

    if(remaining_bytes>=SHA_BLOCK_SIZE)
    {
        if(CRYPTO_SW_RET_SUCCESS!=(result=process_blocks(context,pdata,remaining_bytes/SHA_BLOCK_SIZE)))
        {
          return result;
        }
        pdata=(uint8_t *)(pdata + (uint32_t)((remaining_bytes/SHA_BLOCK_SIZE)*SHA_BLOCK_SIZE));
        remaining_bytes=remaining_bytes%SHA_BLOCK_SIZE;
    }

    if(remaining_bytes>0)
    {
    	// LI NT Warning 426: Call to function 'memcpy(void *, const void *, unsigned long)' violates semantic '(2P>=3n)'
    	// Cannot tell if pdata - the source - points to an area that is equal to or bigger that the number of bytes to be copied
        memcpy(context->m_residue_buf,pdata,remaining_bytes);
        context->m_residue_len=remaining_bytes;
    }

   return CRYPTO_SW_RET_SUCCESS;
}
//lint +e426

/**************************************************************************
function: crypto_hash_final. See header for description.
**************************************************************************/

T_CRYPTO_SW_RESULT crypto_sw_hash_final(T_SW_HASH_CONTEXT * context, uint8_t * hash)
{
    T_CRYPTO_SW_RESULT result;
    uint32_t i;

    if(NULL==context||NULL==hash)
    {
      return CRYPTO_SW_RET_INPUT_PARAM_ERR;
    }

    if(CRYPTO_SW_RET_SUCCESS!=(result=process_last_message(context)))
    {
      return result;
    }

    memcpy(hash,(uint8_t *)context->m_iv_buf,context->m_digest_len);
    if(context->m_hash_alg!=CM_MD5)
    {
      for(i=0;i<context->m_digest_len/4;i++)
      {
        ((uint32_t *)hash)[i]=LIT_TO_BIG(((uint32_t *)hash)[i]);
      }
    }

    return CRYPTO_SW_RET_SUCCESS;
}

/* -------------------------------------------------------------------------------------------------
                                                    CRYPTO hmac APIs
 -------------------------------------------------------------------------------------------------*/

/**************************************************************************
function: crypto_hmac_get_size. See header for description.
**************************************************************************/
/*
uint32_t crypto_sw_hmac_get_size(T_CM_HASH_ALGO      hash_alg)
{
  return crypto_sw_hash_get_size(hash_alg);
}
*/
/**************************************************************************
function: crypto_hmac_init. See header for description.
**************************************************************************/

T_CRYPTO_SW_RESULT crypto_sw_hmac_init(T_SW_HASH_CONTEXT *context,uint8_t* key, uint32_t key_length, T_CM_HASH_ALGO      hash_alg)
{
  T_CRYPTO_SW_RESULT result=CRYPTO_SW_RET_SUCCESS;
  uint8_t key_ipad[64];
  uint32_t i=0;

  if(NULL==context||NULL==key)
  {
    return CRYPTO_SW_RET_INPUT_PARAM_ERR;
  }

  for(i=0; i<SHA_BLOCK_SIZE;i++)
  {
    if(i<key_length)
    {
      context->m_opad_buf[i]=key[i]^OPAD;
      key_ipad[i]=key[i]^IPAD;
    }
    else
    {
      context->m_opad_buf[i]=OPAD;
      key_ipad[i]=IPAD;
    }
  }

  if(CRYPTO_SW_RET_SUCCESS!=(result=crypto_sw_hash_init(context, hash_alg)))
  {
    goto cleanup;
  }

  if(CRYPTO_SW_RET_SUCCESS!=(result=crypto_sw_hash_update(context,key_ipad,sizeof(key_ipad))))
  {
    goto cleanup;
  }

  cleanup:


  return result;
}

/**************************************************************************
function: crypto_hmac_update. See header for description.
**************************************************************************/

T_CRYPTO_SW_RESULT crypto_sw_hmac_update(T_SW_HASH_CONTEXT * context, uint8_t * data, uint32_t length)
{
  if(NULL==context||NULL==data)
  {
    return CRYPTO_SW_RET_INPUT_PARAM_ERR;
  }

  return crypto_sw_hash_update(context,data,length);
}

/**************************************************************************
function: crypto_hmac_final. See header for description.
**************************************************************************/

T_CRYPTO_SW_RESULT crypto_sw_hmac_final(T_SW_HASH_CONTEXT * context, uint8_t * hmac)
{
  uint8_t hash[32];
  T_CRYPTO_SW_RESULT result=CRYPTO_SW_RET_FAIL;

  if(NULL==context||NULL==hmac)
  {
    return CRYPTO_SW_RET_INPUT_PARAM_ERR;
  }

  if(CRYPTO_SW_RET_SUCCESS!=(result=crypto_sw_hash_final(context,hash)))
  {
    goto cleanup;
  }

  if(CRYPTO_SW_RET_SUCCESS!=(result=crypto_sw_hash_init(context,context->m_hash_alg)))
  {
    goto cleanup;
  }

  if(CRYPTO_SW_RET_SUCCESS!=(result=crypto_sw_hash_update(context,context->m_opad_buf,sizeof(context->m_opad_buf))))
  {
    goto cleanup;
  }

  if(CRYPTO_SW_RET_SUCCESS!=(result=crypto_sw_hash_update(context,hash,context->m_digest_len)))
  {
    goto cleanup;
  }

  if(CRYPTO_SW_RET_SUCCESS!=(result=crypto_sw_hash_final(context,hmac)))
  {
    goto cleanup;
  }

  cleanup:

  return result;
}


