/*
 *       Copyright (C) 2014 Intel Mobile Communications GmbH
 *
 *            Sec Class: Intel Confidential (IC)
 *
 *            All rights reserved.
 *
 *  -------------------------------------------------------------------------
 *   Revision Information:
 *      $File name:  /msw_tools/FlashTool/libFlsTool/src/sha256.cpp $
 *      $CC-Version: .../dhkristx_140314_v0/1 $
 *      $Date:       2014-04-16    12:20:19 UTC $
 *   ------------------------------------------------------------------------- */


/*****************************************************************************/
/* INCLUDES                                                                  */
/*****************************************************************************/

#include "sha256.h"
#include "string.h"

/*****************************************************************************/
/* DEFINES                                                                   */
/*****************************************************************************/
/** Hash defines */
#define SHA_BLOCK_SIZE 64
#define SHA256_DIGEST_SIZE 32


#define LIT_TO_BIG(x) ((((x)&0xff)<<24)|(((x)&0xff00)<<8)|(((x)&0xff0000)>>8)|(((x)&0xff000000)>>24))


#define SHA1_SCHEDULE_SIZE 80
#define SHA1_MESSAGE_SIZE 16
#define SHA1_HASH_SIZE 5

#define SHA256_SCHEDULE_SIZE 64
#define SHA256_MESSAGE_SIZE 16
#define SHA256_HASH_SIZE 8

#define SHA1_SCHEDULE_SIZE 80
#define SHA1_MESSAGE_SIZE 16
#define SHA1_HASH_SIZE 5

#define MD5_SCHEDULE_SIZE 16
#define MD5_MESSAGE_SIZE 16
#define MD5_HASH_SIZE 4

#define CH(x,y,z) ((x & y) ^ (( ~x) & z))
#define PARITY(x,y,z) (x ^ y ^ z)
#define MAJ(x,y,z) ((x & y) ^ (x & z) ^ (y & z))

#define F(x,y,z) ((x & y) | (( ~x) & z))
#define G(x,y,z) (((x) & (z)) | ((y) & ( ~(z))))
#define H(x,y,z) (x ^ y ^ z)
#define I(x,y,z) (y ^(x |( ~z)))


#define SIGMA_0_256(x) (rotate_right(x,2) ^ rotate_right(x,13) ^ rotate_right(x,22))
#define SIGMA_1_256(x) (rotate_right(x,6) ^ rotate_right(x,11) ^ rotate_right(x,25))

#define sigma_0_256(x) (rotate_right(x,7) ^ rotate_right(x,18) ^ (x>>3))
#define sigma_1_256(x) (rotate_right(x,17) ^ rotate_right(x,19) ^ (x>>10))



static uint32_t K_256[]={
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

uint32_t rotate_right(uint32_t byte, uint8_t bits)
{
    return (byte>>bits)|(byte<<(32-bits));
}

T_CRYPTO_SW_RESULT sha256_sw_engine(T_SW_HASH_CONTEXT * context, uint32_t * message, uint32_t blocks)
{
  uint32_t W[SHA256_SCHEDULE_SIZE],i,j;
  uint32_t a,b,c,d,e,f,g,h,T1,T2;

  for(j=0;j<blocks;j++)
  {
    /* swap to little endian */
    for(i=0;i<SHA256_MESSAGE_SIZE;i++)
    {
        W[i]=LIT_TO_BIG(message[i+SHA256_MESSAGE_SIZE*j]);
    }

    for(i=SHA256_MESSAGE_SIZE;i<SHA256_SCHEDULE_SIZE;i++)
    {
        W[i]=sigma_1_256(W[i-2])+W[i-7]+sigma_0_256(W[i-15])+W[i-16];
    }

    a = ((uint32_t*)context->m_iv_buf)[0];
    b = ((uint32_t*)context->m_iv_buf)[1];
    c = ((uint32_t*)context->m_iv_buf)[2];
    d = ((uint32_t*)context->m_iv_buf)[3];
    e = ((uint32_t*)context->m_iv_buf)[4];
    f = ((uint32_t*)context->m_iv_buf)[5];
    g = ((uint32_t*)context->m_iv_buf)[6];
    h = ((uint32_t*)context->m_iv_buf)[7];

    for(i=0;i<SHA256_SCHEDULE_SIZE;i++)
    {
      T1=h+SIGMA_1_256(e)+CH(e,f,g)+K_256[i]+W[i];
      T2=SIGMA_0_256(a)+MAJ(a,b,c);
      h=g;
      g=f;
      f=e;
      e=d+T1;
      d=c;
      c=b;
      b=a;
      a=T1+T2;
    }

    ((uint32_t*)context->m_iv_buf)[0] = a + ((uint32_t*)context->m_iv_buf)[0];
    ((uint32_t*)context->m_iv_buf)[1] = b + ((uint32_t*)context->m_iv_buf)[1];
    ((uint32_t*)context->m_iv_buf)[2] = c + ((uint32_t*)context->m_iv_buf)[2];
    ((uint32_t*)context->m_iv_buf)[3] = d + ((uint32_t*)context->m_iv_buf)[3];
    ((uint32_t*)context->m_iv_buf)[4] = e + ((uint32_t*)context->m_iv_buf)[4];
    ((uint32_t*)context->m_iv_buf)[5] = f + ((uint32_t*)context->m_iv_buf)[5];
    ((uint32_t*)context->m_iv_buf)[6] = g + ((uint32_t*)context->m_iv_buf)[6];
    ((uint32_t*)context->m_iv_buf)[7] = h + ((uint32_t*)context->m_iv_buf)[7];
  }
  return CRYPTO_SW_RET_SUCCESS;
}




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
    T_CRYPTO_SW_RESULT result;

    result=sha256_sw_engine(context,(uint32_t *)(data),blocks);
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

      (((uint32_t* )context->m_residue_buf)[16-1])=LIT_TO_BIG(context->m_total_length<<3);

    return process_blocks(context,context->m_residue_buf,1);
}
/** @} */

/*************************  EXTERNAL FUNCTIONS  *****************************/
/**************************************************************************
function: crypto_hash_init. See header for description.
***************************************************************************/

T_CRYPTO_SW_RESULT crypto_sw_sha256_init(T_SW_HASH_CONTEXT *context)
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
   ((uint32_t *)context->m_iv_buf)[0] = 0x6a09e667;
   ((uint32_t *)context->m_iv_buf)[1] = 0xbb67ae85;
   ((uint32_t *)context->m_iv_buf)[2] = 0x3c6ef372;
   ((uint32_t *)context->m_iv_buf)[3] = 0xa54ff53a;
   ((uint32_t *)context->m_iv_buf)[4] = 0x510e527f;
   ((uint32_t *)context->m_iv_buf)[5] = 0x9b05688c;
   ((uint32_t *)context->m_iv_buf)[6] = 0x1f83d9ab;
   ((uint32_t *)context->m_iv_buf)[7] = 0x5be0cd19;

    context->m_digest_len = SHA256_DIGEST_SIZE;

    return result;
}

/**************************************************************************
function: crypto_hash_update. See header for description.
**************************************************************************/

T_CRYPTO_SW_RESULT crypto_sw_sha256_update(T_SW_HASH_CONTEXT * context, uint8_t * data, uint32_t length)
{
    T_CRYPTO_SW_RESULT result;
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
        pdata=(uint8_t *)(((uintptr_t)pdata) + ((remaining_bytes/SHA_BLOCK_SIZE)*SHA_BLOCK_SIZE));
        remaining_bytes=remaining_bytes%SHA_BLOCK_SIZE;
    }

    if(remaining_bytes>0)
    {
        memcpy(context->m_residue_buf,pdata,remaining_bytes);
        context->m_residue_len=remaining_bytes;
    }

   return CRYPTO_SW_RET_SUCCESS;
}

/**************************************************************************
function: crypto_hash_final. See header for description.
**************************************************************************/

T_CRYPTO_SW_RESULT crypto_sw_sha256_final(T_SW_HASH_CONTEXT * context, uint8_t * hash)
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
      for(i=0;i<context->m_digest_len/4;i++)
      {
        ((uint32_t *)hash)[i]=LIT_TO_BIG(((uint32_t *)hash)[i]);
      }

    return CRYPTO_SW_RET_SUCCESS;
}


