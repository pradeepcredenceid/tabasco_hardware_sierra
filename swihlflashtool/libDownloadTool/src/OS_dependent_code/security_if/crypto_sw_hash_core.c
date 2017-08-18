/* -------------------------------------------------------------------------
   Copyright (C) 2013-2014 Intel Mobile Communications GmbH

        Sec Class: Intel Confidential (IC)

   All rights reserved.
   -------------------------------------------------------------------------
   Revision Information:
      $File name:  /msw_tools/FlashTool/libDownloadTool/src/OS_dependent_code/security_if/crypto_sw_hash_core.c $
      $CC-Version: .../oint_tools_flashtoole2_01/4 $
      $Date:       2014-03-13    15:42:06 UTC $
   ------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
//#include "bastypes.h"
#include "crypto_sw_hash_core.h"

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
//#define CH(x,y,z) (z ^ (x & (y ^ z)))
#define PARITY(x,y,z) (x ^ y ^ z)
#define MAJ(x,y,z) ((x & y) ^ (x & z) ^ (y & z))

#define F(x,y,z) ((x & y) | (( ~x) & z))
#define G(x,y,z) (((x) & (z)) | ((y) & ( ~(z))))
#define H(x,y,z) (x ^ y ^ z)
#define I(x,y,z) (y ^(x |( ~z)))

uint32_t rotate_left(uint32_t byte, uint8_t bits)
{
    return (byte<<bits)|(byte>>(32-bits));
}

uint32_t rotate_right(uint32_t byte, uint8_t bits)
{
    return (byte>>bits)|(byte<<(32-bits));
}

#define SIGMA_0_256(x) (rotate_right(x,2) ^ rotate_right(x,13) ^ rotate_right(x,22))
#define SIGMA_1_256(x) (rotate_right(x,6) ^ rotate_right(x,11) ^ rotate_right(x,25))

#define sigma_0_256(x) (rotate_right(x,7) ^ rotate_right(x,18) ^ (x>>3))
#define sigma_1_256(x) (rotate_right(x,17) ^ rotate_right(x,19) ^ (x>>10))



T_CRYPTO_SW_RESULT sha1_sw_engine(T_SW_HASH_CONTEXT * context, uint32_t * message, uint32_t blocks)
{
  uint32_t W[SHA1_SCHEDULE_SIZE],i,j;
  uint32_t a,b,c,d,e,T,K;

  for(j=0;j<blocks;j++)
  {
    a = ((uint32_t*)context->m_iv_buf)[0];
    b = ((uint32_t*)context->m_iv_buf)[1];
    c = ((uint32_t*)context->m_iv_buf)[2];
    d = ((uint32_t*)context->m_iv_buf)[3];
    e = ((uint32_t*)context->m_iv_buf)[4];

    /* swap to little endian */
    for(i=0;i<SHA1_MESSAGE_SIZE;i++)
    {
        W[i]=LIT_TO_BIG(message[(uint32_t)(i+j*SHA1_MESSAGE_SIZE)]);
    }

    for(i=SHA1_MESSAGE_SIZE;i<SHA1_SCHEDULE_SIZE;i++)
    {
        W[i]=rotate_left((W[i-3]^W[i-8]^W[i-14]^W[i-16]),1);
    }

    K = 0x5A827999;
    for(i=0;i<20;i++)
    {
      T = rotate_left(a,5) + (CH(b,c,d) + (e + (K + W[i])));
      e = d;
      d = c;
      c = rotate_left(b,30);
      b = a;
      a = T;
    }

    K = 0x6ED9EBA1;
    for(i=20;i<40;i++)
    {
      T = rotate_left(a,5) + (PARITY(b,c,d) + (e + (K + W[i])));
      e = d;
      d = c;
      c = rotate_left(b,30);
      b = a;
      a = T;
    }

    K = 0x8F1BBCDC;
    for(i=40;i<60;i++)
    {
      T = rotate_left(a,5) + MAJ(b,c,d) + e + K + W[i];
      e = d;
      d = c;
      c = rotate_left(b,30);
      b = a;
      a = T;
    }

    K = 0xCA62C1D6;
    for(i=60;i<80;i++)
    {
      T = rotate_left(a,5) + PARITY(b,c,d) + e + K + W[i];
      e = d;
      d = c;
      c = rotate_left(b,30);
      b = a;
      a = T;
    }

    ((uint32_t*)context->m_iv_buf)[0] = a + ((uint32_t*)context->m_iv_buf)[0];
    ((uint32_t*)context->m_iv_buf)[1] = b + ((uint32_t*)context->m_iv_buf)[1];
    ((uint32_t*)context->m_iv_buf)[2] = c + ((uint32_t*)context->m_iv_buf)[2];
    ((uint32_t*)context->m_iv_buf)[3] = d + ((uint32_t*)context->m_iv_buf)[3];
    ((uint32_t*)context->m_iv_buf)[4] = e + ((uint32_t*)context->m_iv_buf)[4];
  }

  return CRYPTO_SW_RET_SUCCESS;
}

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


T_CRYPTO_SW_RESULT sha256_sw_engine(T_SW_HASH_CONTEXT * context, uint32_t * message, uint32_t blocks)
{
  uint32_t W[SHA256_SCHEDULE_SIZE],i,j;
  uint32_t a,b,c,d,e,f,g,h,T1,T2;

  for(j=0;j<blocks;j++)
  {
    /* swap to little endian */
    for(i=0;i<SHA256_MESSAGE_SIZE;i++)
    {
        W[i]=LIT_TO_BIG(message[(uint32_t)(i+SHA256_MESSAGE_SIZE*j)]);
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

uint32_t T_MD5[] = {
  0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
  0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
  0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
  0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
  0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
  0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
  0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
  0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
  0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
  0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
  0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
  0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
  0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
  0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
  0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
  0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 };

uint32_t shifts_md5[] = {
  7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
  5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
  4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
  6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};

T_CRYPTO_SW_RESULT md5_sw_engine(T_SW_HASH_CONTEXT * context, uint32_t * message, uint32_t blocks)
{
  uint32_t X[MD5_SCHEDULE_SIZE],i,j;
  uint32_t a,b,c,d,tmp,f;

  for(j=0;j<blocks;j++)
  {

    a = ((uint32_t*)context->m_iv_buf)[0];
    b = ((uint32_t*)context->m_iv_buf)[1];
    c = ((uint32_t*)context->m_iv_buf)[2];
    d = ((uint32_t*)context->m_iv_buf)[3];

    /* swap to little endian */
    for(i=0;i<MD5_MESSAGE_SIZE;i++)
    {
      X[i]=message[(uint32_t)(i+j*MD5_MESSAGE_SIZE)];
    }

    for(i=0;i<16;i++)
    {
      f=F(b,c,d);
      tmp = d;
      d = c;
      c = b;
      b = b + rotate_left((a +f + X[i] + T_MD5[i]),shifts_md5[i]);
      a = tmp;
    }

    for(i=16;i<32;i++)
    {
      f=G(b,c,d);
      tmp = d;
      d = c;
      c = b;
      b = b + rotate_left((a + f+ X[(uint32_t)((5*i + 1)%16)] + T_MD5[i]),shifts_md5[i]);
      a = tmp;
    }

    for(i=32;i<48;i++)
    {
      f=H(b,c,d);
      tmp = d;
      d = c;
      c = b;
      b = b + rotate_left((a + f+ X[(uint32_t)((3*i + 5)%16)] + T_MD5[i]),shifts_md5[i]);
      a = tmp;
    }

    for(i=48;i<64;i++)
    {
      f=I(b,c,d);
      tmp = d;
      d = c;
      c = b;
      b = b + rotate_left((a + f + X[(uint32_t)((7*i)%16)] + T_MD5[i]),shifts_md5[i]);
      a = tmp;
    }

    ((uint32_t*)context->m_iv_buf)[0] = a + ((uint32_t*)context->m_iv_buf)[0];
    ((uint32_t*)context->m_iv_buf)[1] = b + ((uint32_t*)context->m_iv_buf)[1];
    ((uint32_t*)context->m_iv_buf)[2] = c + ((uint32_t*)context->m_iv_buf)[2];
    ((uint32_t*)context->m_iv_buf)[3] = d + ((uint32_t*)context->m_iv_buf)[3];

  }

  return CRYPTO_SW_RET_SUCCESS;
}

