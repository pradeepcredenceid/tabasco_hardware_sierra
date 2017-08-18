/* -------------------------------------------------------------------------
 Copyright (C) 2014 Intel Mobile Communications GmbH
 
      Sec Class: Intel Confidential (IC)
 ----------------------------------------------------------------------
 Revision Information:
    $File name:  /msw_tools/FlashTool/libFlsTool/src/rsa_tool.c $
    $CC-Version: .../dhkristx_140314_v0/2 $
    $Date:       2014-04-23    11:04:05 UTC $
 ----------------------------------------------------------------------
 by CCCT (0.12l)
 ---------------------------------------------------------------------- */

/* ====================================================================
 * Copyright (c) 1998-2011 The OpenSSL Project.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit. (http://www.openssl.org/)"
 *
 * 4. The names "OpenSSL Toolkit" and "OpenSSL Project" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For written permission, please contact
 *    openssl-core@openssl.org.
 *
 * 5. Products derived from this software may not be called "OpenSSL"
 *    nor may "OpenSSL" appear in their names without prior written
 *    permission of the OpenSSL Project.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit (http://www.openssl.org/)"
 *
 * THIS SOFTWARE IS PROVIDED BY THE OpenSSL PROJECT ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE OpenSSL PROJECT OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 *
 * This product includes cryptographic software written by Eric Young
 * (eay@cryptsoft.com).  This product includes software written by Tim
 * Hudson (tjh@cryptsoft.com).
 *
 */


/* Copyright (C) 1995-1998 Eric Young (eay@cryptsoft.com)
 * All rights reserved.
 *
 * This package is an SSL implementation written
 * by Eric Young (eay@cryptsoft.com).
 * The implementation was written so as to conform with Netscapes SSL.
 *
 * This library is free for commercial and non-commercial use as long as
 * the following conditions are aheared to.  The following conditions
 * apply to all code found in this distribution, be it the RC4, RSA,
 * lhash, DES, etc., code; not just the SSL code.  The SSL documentation
 * included with this distribution is covered by the same copyright terms
 * except that the holder is Tim Hudson (tjh@cryptsoft.com).
 *
 * Copyright remains Eric Young's, and as such any Copyright notices in
 * the code are not to be removed.
 * If this package is used in a product, Eric Young should be given attribution
 * as the author of the parts of the library used.
 * This can be in the form of a textual message at program startup or
 * in documentation (online or textual) provided with the package.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    "This product includes cryptographic software written by
 *     Eric Young (eay@cryptsoft.com)"
 *    The word 'cryptographic' can be left out if the rouines from the library
 *    being used are not cryptographic related :-).
 * 4. If you include any Windows specific code (or a derivative thereof) from
 *    the apps directory (application code) you must include an acknowledgement:
 *    "This product includes software written by Tim Hudson (tjh@cryptsoft.com)"
 *
 * THIS SOFTWARE IS PROVIDED BY ERIC YOUNG ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * The licence and distribution terms for any publically available version or
 * derivative of this code cannot be changed.  i.e. this code cannot simply be
 * copied and put under another distribution licence
 * [including the GNU Public Licence.]
 */

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <limits.h>
#include  "rsa_tool.h"

#ifdef __cplusplus
  #include <assert.h>
#else
  #define assert(X)
#endif

#ifdef max
  #undef max
#endif
#ifdef min
  #undef min
#endif

/******************************************************************************
*     GLOBAL SECTION                                                          *
******************************************************************************/

unsigned char cleanse_ctr = 0;

/******************************************************************************
*     FUNCTION SECTION                                                        *
******************************************************************************/

static void *(*malloc_func)(size_t)         = malloc;
static void *default_malloc_ex(size_t num, const char *file, int line){ return malloc_func(num); }
static void *(*malloc_ex_func)(size_t, const char *file, int line) = default_malloc_ex;
static void (*malloc_debug_func)(void *,int,const char *,int,int) = NULL;
static void (*free_debug_func)(void *,int) = NULL;
static void (*free_func)(void *)            = free;
static void (*free_locked_func)(void *)     = free;

void CRYPTO_free_locked(void *str)
{
	if (free_debug_func != NULL)
		free_debug_func(str, 0);
#ifdef LEVITTE_DEBUG_MEM
	fprintf(stderr, "LEVITTE_DEBUG_MEM:         < 0x%p\n", str);
#endif
	free_locked_func(str);
	if (free_debug_func != NULL)
		free_debug_func(NULL, 1);
}
void CRYPTO_free(void *str)
{
	if (free_debug_func != NULL)
		free_debug_func(str, 0);
#ifdef LEVITTE_DEBUG_MEM
	fprintf(stderr, "LEVITTE_DEBUG_MEM:         < 0x%p\n", str);
#endif
	free_func(str);
	if (free_debug_func != NULL)
		free_debug_func(NULL, 1);
}
void *CRYPTO_malloc(int num, const char *file, int line)
{
	void *ret;
	extern unsigned char cleanse_ctr;

	if (num <= 0) return NULL;

	if (malloc_debug_func != NULL)
	{
		malloc_debug_func(NULL, num, file, line, 0);
	}
	ret = malloc_ex_func(num,file,line);
#ifdef LEVITTE_DEBUG_MEM
	fprintf(stderr, "LEVITTE_DEBUG_MEM:         > 0x%p (%d)\n", ret, num);
#endif
	if (malloc_debug_func != NULL)
		malloc_debug_func(ret, num, file, line, 1);

        /* Create a dependency on the value of 'cleanse_ctr' so our memory
		* sanitisation function can't be optimised out. NB: We only do
    	* this for >2Kb so the overhead doesn't bother us. */
	if(ret && (num > 2048))
		((unsigned char *)ret)[0] = cleanse_ctr;

	return ret;
}

void OPENSSL_cleanse(void *ptr, size_t len)
{
  unsigned char *p = (unsigned char *)ptr;
  size_t loop = len;
  while(loop--)
  {
    *(p++) = cleanse_ctr;
    cleanse_ctr += (17 + (unsigned char)((uintptr_t)p & 0xF));
  }
  if(memchr(ptr, cleanse_ctr, len))
    cleanse_ctr += 63;
}

RSA *RSA_new(void)
{
	RSA *ret=(RSA *)OPENSSL_malloc(sizeof(RSA));
    ret->pad=0;
	ret->version=0;
	ret->n=NULL;
	ret->e=NULL;
	ret->d=NULL;
	ret->p=NULL;
	ret->q=NULL;
	ret->dmp1=NULL;
	ret->dmq1=NULL;
	ret->iqmp=NULL;
	ret->references=1;
	ret->_method_mod_n=NULL;
	ret->_method_mod_p=NULL;
	ret->_method_mod_q=NULL;
	ret->blinding=NULL;
	ret->bignum_data=NULL;
	ret->flags=6;
	return ret;
}

void BN_clear_free(BIGNUM *a)
{
	int i;
	if (a == NULL) return;
	if (a->d != NULL)
	{
		OPENSSL_cleanse(a->d,a->dmax*sizeof(a->d[0]));
		if (!(BN_get_flags(a,BN_FLG_STATIC_DATA)))
			OPENSSL_free(a->d);
	}
	i=BN_get_flags(a,BN_FLG_MALLOCED);
	OPENSSL_cleanse(a,sizeof(BIGNUM));
	if (i)
		OPENSSL_free(a);
}

void BN_free(BIGNUM *a)
{
	if (a == NULL) return;
	if ((a->d != NULL) && !(BN_get_flags(a,BN_FLG_STATIC_DATA)))
		OPENSSL_free(a->d);
	a->flags|=BN_FLG_FREE; /* REMOVE? */
	if (a->flags & BN_FLG_MALLOCED)
		OPENSSL_free(a);
}


void BN_BLINDING_free(BN_BLINDING *r)
{
	if(r == NULL)
		return;

	if (r->A  != NULL) BN_free(r->A );
	if (r->Ai != NULL) BN_free(r->Ai);
	OPENSSL_free(r);
}

void RSA_free(RSA *r)    //modified
{
	if (r == NULL) return;
	if (r->n != NULL) BN_clear_free(r->n);
	if (r->e != NULL) BN_clear_free(r->e);
	if (r->d != NULL) BN_clear_free(r->d);
	if (r->p != NULL) BN_clear_free(r->p);
	if (r->q != NULL) BN_clear_free(r->q);
	if (r->dmp1 != NULL) BN_clear_free(r->dmp1);
	if (r->dmq1 != NULL) BN_clear_free(r->dmq1);
	if (r->iqmp != NULL) BN_clear_free(r->iqmp);
	if (r->blinding != NULL) BN_BLINDING_free(r->blinding);
	if (r->bignum_data != NULL) OPENSSL_free_locked(r->bignum_data);
	OPENSSL_free(r);
}

BIGNUM *BN_new(void)
{
	BIGNUM *ret;

	if ((ret=(BIGNUM *)OPENSSL_malloc(sizeof(BIGNUM))) == NULL)
	{
		AS_printf("Error in BN_new\n");
		return(NULL);
	}
	ret->flags=BN_FLG_MALLOCED;
	ret->top=0;
	ret->neg=0;
	ret->dmax=0;
	ret->d=NULL;
	return(ret);
}

static BN_ULONG *bn_expand_internal(const BIGNUM *b, int words)
{
	BN_ULONG *A,*a;
	const BN_ULONG *B;
	int i;

	if (words > (INT_MAX/(4*BN_BITS2)))
	{
		AS_printf("Error in words size\n");
		return NULL;
	}

	bn_check_top(b);	
	if (BN_get_flags(b,BN_FLG_STATIC_DATA))
	{
		AS_printf("Error in getting flag\n");
		return(NULL);
	}
	a=A=(BN_ULONG *)OPENSSL_malloc((sizeof(BN_ULONG)*(words+1)));
	if (A == NULL)
	{
		AS_printf("Error in mallocing A\n");
		return(NULL);
	}
#if 1
	B=b->d;
	/* Check if the previous number needs to be copied */
	if (B != NULL)
	{
		for (i=b->top>>2; i>0; i--,A+=4,B+=4)
		{
		/*
		* The fact that the loop is unrolled
		* 4-wise is a tribute to Intel. It's
		* the one that doesn't have enough
		* registers to accomodate more data.
		* I'd unroll it 8-wise otherwise:-)
		*
		*		<appro@fy.chalmers.se>
			*/
			BN_ULONG a0,a1,a2,a3;
			a0=B[0]; a1=B[1]; a2=B[2]; a3=B[3];
			A[0]=a0; A[1]=a1; A[2]=a2; A[3]=a3;
		}
		switch (b->top&3)
		{
		case 3:	A[2]=B[2];
     /* fall through */
		case 2:	A[1]=B[1];
     /* fall through */
		case 1:	A[0]=B[0];
     /* fall through */
		case 0: /* workaround for ultrix cc: without 'case 0', the optimizer does
				* the switch table by doing a=top&3; a--; goto jump_table[a];
			* which fails for top== 0 */
			;
		}
	}

	/* Now need to zero any data between b->top and b->max */
	/* XXX Why? */

	A= &(a[b->top]);
	for (i=(words - b->top)>>3; i>0; i--,A+=8)
	{
		A[0]=0; A[1]=0; A[2]=0; A[3]=0;
		A[4]=0; A[5]=0; A[6]=0; A[7]=0;
	}
	for (i=(words - b->top)&7; i>0; i--,A++)
		A[0]=0;
#else
	memset(A,0,sizeof(BN_ULONG)*(words+1));
	memcpy(A,b->d,sizeof(b->d[0])*b->top);
#endif

	return(a);
}


BIGNUM *bn_expand2(BIGNUM *b, int words)
{
	if (words > b->dmax)
	{
		BN_ULONG *a = bn_expand_internal(b, words);
		
		if (a)
		{
			if (b->d)
				OPENSSL_free(b->d);
			b->d=a;
			b->dmax=words;
		}
		else
			b = NULL;
	}
	return b;
}

BIGNUM *BN_bin2bn(const unsigned char *s, int len, BIGNUM *ret)
{
	unsigned int i,m;
	unsigned int n;
	BN_ULONG l;

	if (ret == NULL) ret=BN_new();
	if (ret == NULL) return(NULL);
	l=0;
	n=len;
	if (n == 0)
	{
		ret->top=0;
		return(ret);
	}
	if (bn_expand(ret,((int)(n+2)*8)) == NULL)
		return(NULL);
	i=((n-1)/BN_BYTES)+1;
	m=((n-1)%(BN_BYTES));
	ret->top=i;
	ret->neg=0;
	while (n-- > 0)
	{
		l=(l<<8L)| *(s++);
		if (m-- == 0)
		{
			ret->d[--i]=l;
			l=0;
			m=BN_BYTES-1;
		}
	}
	/* need to call this due to clear byte at top if avoiding
	* having the top bit set (-ve number) */
	bn_fix_top(ret);
	return(ret);
}
#define THIRTY_TWO_BIT
int BN_num_bits_word(BN_ULONG l)
{
	static const char bits[256]={
		0,1,2,2,3,3,3,3,4,4,4,4,4,4,4,4,
			5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
			6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
			6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
			7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
			7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
			7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
			7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
			8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
			8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
			8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
			8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
			8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
			8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
			8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
			8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
	};

#if defined(SIXTY_FOUR_BIT_LONG)
	if (l & 0xffffffff00000000L)
	{
		if (l & 0xffff000000000000L)
		{
			if (l & 0xff00000000000000L)
			{
				return(bits[(int)(l>>56)]+56);
			}
			else	return(bits[(int)(l>>48)]+48);
		}
		else
		{
			if (l & 0x0000ff0000000000L)
			{
				return(bits[(int)(l>>40)]+40);
			}
			else	return(bits[(int)(l>>32)]+32);
		}
	}
	else
#else
#ifdef SIXTY_FOUR_BIT
		if (l & 0xffffffff00000000LL)
		{
			if (l & 0xffff000000000000LL)
			{
				if (l & 0xff00000000000000LL)
				{
					return(bits[(int)(l>>56)]+56);
				}
				else	return(bits[(int)(l>>48)]+48);
			}
			else
			{
				if (l & 0x0000ff0000000000LL)
				{
					return(bits[(int)(l>>40)]+40);
				}
				else	return(bits[(int)(l>>32)]+32);
			}
		}
		else
#endif
#endif
		{
#if defined(THIRTY_TWO_BIT) || defined(SIXTY_FOUR_BIT) || defined(SIXTY_FOUR_BIT_LONG)
			if (l & 0xffff0000L)
			{
				if (l & 0xff000000L)
					return(bits[(int)(l>>24L)]+24);
				else	return(bits[(int)(l>>16L)]+16);
			}
			else
#endif
			{
#if defined(SIXTEEN_BIT) || defined(THIRTY_TWO_BIT) || defined(SIXTY_FOUR_BIT) || defined(SIXTY_FOUR_BIT_LONG)
				if (l & 0xff00L)
					return(bits[(int)(l>>8)]+8);
				else	
#endif
					return(bits[(int)(l   )]  );
			}
		}
}

int BN_num_bits(const BIGNUM *a)
{
	BN_ULONG l;
	int i;

	bn_check_top(a);

	if (a->top == 0) return(0);
	l=a->d[a->top-1];
	assert(l != 0);
	i=(a->top-1)*BN_BITS2;
	return(i+BN_num_bits_word(l));
}

int BN_bn2bin(const BIGNUM *a, unsigned char *to)
{
	int n,i;
	BN_ULONG l;

	n=i=BN_num_bytes(a);
	while (i-- > 0)
	{
		l=a->d[i/BN_BYTES];
		*(to++)=(unsigned char)(l>>(8*(i%BN_BYTES)))&0xff;
	}
	return(n);
}

int BN_mask_bits(BIGNUM *a, int n)
{
	int b,w;

	w=n/BN_BITS2;
	b=n%BN_BITS2;
	if (w >= a->top) return(0);
	if (b == 0)
		a->top=w;
	else
	{
		a->top=w+1;
		a->d[w]&= ~(BN_MASK2<<b);
	}
	bn_fix_top(a);
	return(1);
}
void BN_CTX_start(BN_CTX *ctx)
{
	if (ctx->depth < BN_CTX_NUM_POS)
		ctx->pos[ctx->depth] = ctx->tos;
	ctx->depth++;
}


BIGNUM *BN_CTX_get(BN_CTX *ctx)
{
/* Note: If BN_CTX_get is ever changed to allocate BIGNUMs dynamically,
* make sure that if BN_CTX_get fails once it will return NULL again
* until BN_CTX_end is called.  (This is so that callers have to check
* only the last return value.)
	*/
	if (ctx->depth > BN_CTX_NUM_POS || ctx->tos >= BN_CTX_NUM)
	{
		if (!ctx->too_many)
		{
			AS_printf("BN_CTX_GET_TOO_MANY_TEMPORARY_VARIABLES\n");
			/* disable error code until BN_CTX_end is called: */
			ctx->too_many = 1;
		}
		return NULL;
	}
	return (&(ctx->bn[ctx->tos++]));
}

int bn_cmp_words(const BN_ULONG *a, const BN_ULONG *b, int n)
{
	int i;
	BN_ULONG aa,bb;

	aa=a[n-1];
	bb=b[n-1];
	if (aa != bb) return((aa > bb)?1:-1);
	for (i=n-2; i>=0; i--)
	{
		aa=a[i];
		bb=b[i];
		if (aa != bb) return((aa > bb)?1:-1);
	}
	return(0);
}
BN_ULONG bn_add_words(BN_ULONG *r, const BN_ULONG *a, const BN_ULONG *b, int n)
{
	BN_ULONG c,l,t;
	/*lint -e514 */
	assert(n >= 0);
	if (n <= 0) return((BN_ULONG)0);

	c=0;
	for (;;)
	{
		t=a[0];
		t=(t+c)&BN_MASK2;
		c=(t < c);
		l=(t+b[0])&BN_MASK2;
		c+=(l < t);
		r[0]=l;
		if (--n <= 0) break;

		t=a[1];
		t=(t+c)&BN_MASK2;
		c=(t < c);
		l=(t+b[1])&BN_MASK2;
		c+=(l < t);
		r[1]=l;
		if (--n <= 0) break;

		t=a[2];
		t=(t+c)&BN_MASK2;
		c=(t < c);
		l=(t+b[2])&BN_MASK2;
		c+=(l < t);
		r[2]=l;
		if (--n <= 0) break;

		t=a[3];
		t=(t+c)&BN_MASK2;
		c=(t < c);
		l=(t+b[3])&BN_MASK2;
		c+=(l < t);
		r[3]=l;
		if (--n <= 0) break;

		a+=4;
		b+=4;
		r+=4;
	}
	return((BN_ULONG)c);
	/*lint +e514 */
  
}


BN_ULONG bn_sub_words(BN_ULONG *r, const BN_ULONG *a, const BN_ULONG *b, int n)
{
	BN_ULONG t1,t2;
	int c=0;

	assert(n >= 0);
	if (n <= 0) return((BN_ULONG)0);
	
	for (;;)
	{
		t1=a[0]; t2=b[0];
		r[0]=(t1-t2-c)&BN_MASK2;
		if (t1 != t2) c=(t1 < t2);
		if (--n <= 0) break;

		t1=a[1]; t2=b[1];
		r[1]=(t1-t2-c)&BN_MASK2;
		if (t1 != t2) c=(t1 < t2);
		if (--n <= 0) break;

		t1=a[2]; t2=b[2];
		r[2]=(t1-t2-c)&BN_MASK2;
		if (t1 != t2) c=(t1 < t2);
		if (--n <= 0) break;

		t1=a[3]; t2=b[3];
		r[3]=(t1-t2-c)&BN_MASK2;
		if (t1 != t2) c=(t1 < t2);
		if (--n <= 0) break;

		a+=4;
		b+=4;
		r+=4;
	}
	return(c);
}
BN_ULONG bn_mul_add_words(BN_ULONG *rp, BN_ULONG *ap, int num, BN_ULONG w)
{
	BN_ULONG c1=0;

	if (num <= 0) return(c1);

	while (num&~3)
	{
		mul_add(rp[0],ap[0],w,c1);
		mul_add(rp[1],ap[1],w,c1);
		mul_add(rp[2],ap[2],w,c1);
		mul_add(rp[3],ap[3],w,c1);
		ap+=4; rp+=4; num-=4;
	}
	if (num)
	{
		mul_add(rp[0],ap[0],w,c1); if (--num==0) return c1;
		mul_add(rp[1],ap[1],w,c1); if (--num==0) return c1;
		mul_add(rp[2],ap[2],w,c1); return c1;
	}

	return(c1);
} 
BN_ULONG bn_mul_words(BN_ULONG *rp, const BN_ULONG *ap, int num, BN_ULONG w)
{
	BN_ULONG c1=0;

	assert(num >= 0);
	if (num <= 0) return(c1);

	while (num&~3)
	{
		mul(rp[0],ap[0],w,c1);
		mul(rp[1],ap[1],w,c1);
		mul(rp[2],ap[2],w,c1);
		mul(rp[3],ap[3],w,c1);
		ap+=4; rp+=4; num-=4;
	}
	if (num)
	{
		mul(rp[0],ap[0],w,c1); if (--num == 0) return c1;
		mul(rp[1],ap[1],w,c1); if (--num == 0) return c1;
		mul(rp[2],ap[2],w,c1);
	}
	return(c1);
} 

void bn_sqr_words(BN_ULONG *r, const BN_ULONG *a, int n)
{
	assert(n >= 0);
	if (n <= 0) return;
	while (n&~3)
	{
		sqr(r[0],r[1],a[0]);
		sqr(r[2],r[3],a[1]);
		sqr(r[4],r[5],a[2]);
		sqr(r[6],r[7],a[3]);
		a+=4; r+=8; n-=4;
	}
	if (n)
	{
		sqr(r[0],r[1],a[0]); if (--n == 0) return;
		sqr(r[2],r[3],a[1]); if (--n == 0) return;
		sqr(r[4],r[5],a[2]);
	}
}

void bn_mul_normal(BN_ULONG *r, BN_ULONG *a, int na, BN_ULONG *b, int nb)
{
	BN_ULONG *rr;

#ifdef BN_COUNT
	AS_printf(" bn_mul_normal %d * %d\n",na,nb);
#endif

	if (na < nb)
	{
		int itmp;
		BN_ULONG *ltmp;

		itmp=na; na=nb; nb=itmp;
		ltmp=a;   a=b;   b=ltmp;

	}
	rr= &(r[na]);
	rr[0]=bn_mul_words(r,a,na,b[0]);

	for (;;)
	{
		if (--nb <= 0) return;
		rr[1]=bn_mul_add_words(&(r[1]),a,na,b[1]);
		if (--nb <= 0) return;
		rr[2]=bn_mul_add_words(&(r[2]),a,na,b[2]);
		if (--nb <= 0) return;
		rr[3]=bn_mul_add_words(&(r[3]),a,na,b[3]);
		if (--nb <= 0) return;
		rr[4]=bn_mul_add_words(&(r[4]),a,na,b[4]);
		rr+=4;
		r+=4;
		b+=4;
	}
}

void bn_mul_recursive(BN_ULONG *r, BN_ULONG *a, BN_ULONG *b, int n2,
					  BN_ULONG *t)
{
	int n=n2/2,c1,c2;
	unsigned int neg,zero;
	BN_ULONG ln,lo,*p;

# ifdef BN_COUNT
	AS_printf(" bn_mul_recursive %d * %d\n",n2,n2);
# endif
# ifdef BN_MUL_COMBA
#  if 0
	if (n2 == 4)
	{
		bn_mul_comba4(r,a,b);
		return;
	}
#  endif
	if (n2 == 8)
	{
		bn_mul_comba8(r,a,b);
		return; 
	}
# endif /* BN_MUL_COMBA */
	if (n2 < BN_MUL_RECURSIVE_SIZE_NORMAL)
	{
		/* This should not happen */
		bn_mul_normal(r,a,n2,b,n2);
		return;
	}
	/* r=(a[0]-a[1])*(b[1]-b[0]) */
	c1=bn_cmp_words(a,&(a[n]),n);
	c2=bn_cmp_words(&(b[n]),b,n);
	zero=neg=0;
	switch (c1*3+c2)
	{
	case -4:
		bn_sub_words(t,      &(a[n]),a,      n); /* - */
		bn_sub_words(&(t[n]),b,      &(b[n]),n); /* - */
		break;
	case -3:
		zero=1;
		break;
	case -2:
		bn_sub_words(t,      &(a[n]),a,      n); /* - */
		bn_sub_words(&(t[n]),&(b[n]),b,      n); /* + */
		neg=1;
		break;
	case -1:
	case 0:
	case 1:
		zero=1;
		break;
	case 2:
		bn_sub_words(t,      a,      &(a[n]),n); /* + */
		bn_sub_words(&(t[n]),b,      &(b[n]),n); /* - */
		neg=1;
		break;
	case 3:
		zero=1;
		break;
	case 4:
		bn_sub_words(t,      a,      &(a[n]),n);
		bn_sub_words(&(t[n]),&(b[n]),b,      n);
		break;
	}

# ifdef BN_MUL_COMBA
	if (n == 4)
	{
		if (!zero)
			bn_mul_comba4(&(t[n2]),t,&(t[n]));
		else
			memset(&(t[n2]),0,8*sizeof(BN_ULONG));

		bn_mul_comba4(r,a,b);
		bn_mul_comba4(&(r[n2]),&(a[n]),&(b[n]));
	}
	else if (n == 8)
	{
		if (!zero)
			bn_mul_comba8(&(t[n2]),t,&(t[n]));
		else
			memset(&(t[n2]),0,16*sizeof(BN_ULONG));

		bn_mul_comba8(r,a,b);
		bn_mul_comba8(&(r[n2]),&(a[n]),&(b[n]));
	}
	else
# endif /* BN_MUL_COMBA */
	{
		p= &(t[n2*2]);
		if (!zero)
			bn_mul_recursive(&(t[n2]),t,&(t[n]),n,p);
		else
			memset(&(t[n2]),0,n2*sizeof(BN_ULONG));
		bn_mul_recursive(r,a,b,n,p);
		bn_mul_recursive(&(r[n2]),&(a[n]),&(b[n]),n,p);
	}

	/* t[32] holds (a[0]-a[1])*(b[1]-b[0]), c1 is the sign
	* r[10] holds (a[0]*b[0])
	* r[32] holds (b[1]*b[1])
	*/

	c1=(int)(bn_add_words(t,r,&(r[n2]),n2));

	if (neg) /* if t[32] is negative */
	{
		c1-=(int)(bn_sub_words(&(t[n2]),t,&(t[n2]),n2));
	}
	else
	{
		/* Might have a carry */
		c1+=(int)(bn_add_words(&(t[n2]),&(t[n2]),t,n2));
	}

	/* t[32] holds (a[0]-a[1])*(b[1]-b[0])+(a[0]*b[0])+(a[1]*b[1])
	* r[10] holds (a[0]*b[0])
	* r[32] holds (b[1]*b[1])
	* c1 holds the carry bits
	*/
	c1+=(int)(bn_add_words(&(r[n]),&(r[n]),&(t[n2]),n2));
	if (c1)
	{
		p= &(r[n+n2]);
		lo= *p;
		ln=(lo+c1)&BN_MASK2;
		*p=ln;

		/* The overflow will stop before we over write
		* words we should not overwrite */
		if (ln < (BN_ULONG)c1)
		{
			do	{
				p++;
				lo= *p;
				ln=(lo+1)&BN_MASK2;
				*p=ln;
			} while (ln == 0);
		}
	}
	}

	int BN_mul(BIGNUM *r, const BIGNUM *a, const BIGNUM *b, BN_CTX *ctx)
	{
		int top,al,bl;
		BIGNUM *rr;
		int ret = 0;
#if defined(BN_MUL_COMBA) || defined(BN_RECURSION)
		int i;
#endif
#ifdef BN_RECURSION
		BIGNUM *t;
		int j,k;
#endif

#ifdef BN_COUNT
		AS_printf("BN_mul %d * %d\n",a->top,b->top);
#endif

		bn_check_top(a);
		bn_check_top(b);
		bn_check_top(r);

		al=a->top;
		bl=b->top;

		if ((al == 0) || (bl == 0))
		{
			if (!BN_zero(r)) goto err;
			return(1);
		}
		top=al+bl;

		BN_CTX_start(ctx);
		if ((r == a) || (r == b))
		{
			if ((rr = BN_CTX_get(ctx)) == NULL) goto err;
		}
		else
			rr = r;
		rr->neg=a->neg^b->neg;

#if defined(BN_MUL_COMBA) || defined(BN_RECURSION)
		i = al-bl;
#endif
#ifdef BN_MUL_COMBA
		if (i == 0)
		{
# if 0
			if (al == 4)
			{
				if (bn_wexpand(rr,8) == NULL) goto err;
				rr->top=8;
				bn_mul_comba4(rr->d,a->d,b->d);
				goto end;
			}
# endif
			if (al == 8)
			{
				if (bn_wexpand(rr,16) == NULL) goto err;
				rr->top=16;
				bn_mul_comba8(rr->d,a->d,b->d);
				goto end;
			}
		}
#endif /* BN_MUL_COMBA */
#ifdef BN_RECURSION
		if ((al >= BN_MULL_SIZE_NORMAL) && (bl >= BN_MULL_SIZE_NORMAL))
		{
			if (i == 1 && !BN_get_flags(b,BN_FLG_STATIC_DATA) && bl<b->dmax)
			{
#if 0	/* tribute to const-ification, bl<b->dmax above covers for this */
				if (bn_wexpand(b,al) == NULL) goto err;
#endif
				b->d[bl]=0;
				bl++;
				i--;
			}
			else if (i == -1 && !BN_get_flags(a,BN_FLG_STATIC_DATA) && al<a->dmax)
			{
#if 0	/* tribute to const-ification, al<a->dmax above covers for this */
				if (bn_wexpand(a,bl) == NULL) goto err;
#endif
				a->d[al]=0;
				al++;
				i++;
			}
			if (i == 0)
			{
				/* symmetric and > 4 */
				/* 16 or larger */
				j=BN_num_bits_word((BN_ULONG)al);
				j=1<<(j-1);
				k=j+j;
				t = BN_CTX_get(ctx);
        if (!t) goto end;
				if (al == j) /* exact multiple */
				{
					if (bn_wexpand(t,k*2) == NULL) goto err;
					if (bn_wexpand(rr,k*2) == NULL) goto err;
					bn_mul_recursive(rr->d,a->d,b->d,al,t->d);
					rr->top=top;
					goto end;
				}
#if 0	/* tribute to const-ification, rsa/dsa performance is not affected */
				else
				{
					if (bn_wexpand(a,k) == NULL ) goto err;
					if (bn_wexpand(b,k) == NULL ) goto err;
					if (bn_wexpand(t,k*4) == NULL ) goto err;
					if (bn_wexpand(rr,k*4) == NULL ) goto err;
					for (i=a->top; i<k; i++)
						a->d[i]=0;
					for (i=b->top; i<k; i++)
						b->d[i]=0;
					bn_mul_part_recursive(rr->d,a->d,b->d,al-j,j,t->d);
				}
				rr->top=top;
				goto end;
#endif
			}
		}
#endif /* BN_RECURSION */
		if (bn_wexpand(rr,top) == NULL) goto err;
		rr->top=top;
		bn_mul_normal(rr->d,a->d,al,b->d,bl);

#if defined(BN_MUL_COMBA) || defined(BN_RECURSION)
end:
#endif
		bn_fix_top(rr);
		if (r != rr) BN_copy(r,rr);
		ret=1;
err:
		BN_CTX_end(ctx);
		return(ret);
	}

	void bn_sqr_normal(BN_ULONG *r, const BN_ULONG *a, int n, BN_ULONG *tmp)
	{
		int i,j,max;
		const BN_ULONG *ap;
		BN_ULONG *rp;

		max=n*2;
		ap=a;
		rp=r;
		rp[0]=rp[max-1]=0;
		rp++;
		j=n;

		if (--j > 0)
		{
			ap++;
			rp[j]=bn_mul_words(rp,ap,j,ap[-1]);
			rp+=2;
		}

		for (i=n-2; i>0; i--)
		{
			j--;
			ap++;
			rp[j]=bn_mul_add_words(rp,(BN_ULONG*)ap,j,ap[-1]);
			rp+=2;
		}

		bn_add_words(r,r,r,max);

		/* There will not be a carry */

		bn_sqr_words(tmp,a,n);

		bn_add_words(r,r,tmp,max);
	}
	void bn_sqr_recursive(BN_ULONG *r, const BN_ULONG *a, int n2, BN_ULONG *t)
	{
		int n=n2/2;
		int zero,c1;
		BN_ULONG ln,lo,*p;

#ifdef BN_COUNT
		fprintf(stderr," bn_sqr_recursive %d * %d\n",n2,n2);
#endif
		if (n2 == 4)
		{
#ifndef BN_SQR_COMBA
			bn_sqr_normal(r,a,4,t);
#else
			bn_sqr_comba4(r,(BN_ULONG*)a);
#endif
			return;
		}
		else if (n2 == 8)
		{
#ifndef BN_SQR_COMBA
			bn_sqr_normal(r,a,8,t);
#else
			bn_sqr_comba8(r,(BN_ULONG*)a);
#endif
			return;
		}
		if (n2 < BN_SQR_RECURSIVE_SIZE_NORMAL)
		{
			bn_sqr_normal(r,a,n2,t);
			return;
		}
		/* r=(a[0]-a[1])*(a[1]-a[0]) */
		c1=bn_cmp_words(a,&(a[n]),n);
		zero=0;
		if (c1 > 0)
			bn_sub_words(t,a,&(a[n]),n);
		else if (c1 < 0)
			bn_sub_words(t,&(a[n]),a,n);
		else
			zero=1;

		/* The result will always be negative unless it is zero */
		p= &(t[n2*2]);

		if (!zero)
			bn_sqr_recursive(&(t[n2]),t,n,p);
		else
			memset(&(t[n2]),0,n2*sizeof(BN_ULONG));
		bn_sqr_recursive(r,a,n,p);
		bn_sqr_recursive(&(r[n2]),&(a[n]),n,p);

		/* t[32] holds (a[0]-a[1])*(a[1]-a[0]), it is negative or zero
		* r[10] holds (a[0]*b[0])
		* r[32] holds (b[1]*b[1])
		*/

		c1=(int)(bn_add_words(t,r,&(r[n2]),n2));

		/* t[32] is negative */
		c1-=(int)(bn_sub_words(&(t[n2]),t,&(t[n2]),n2));

		/* t[32] holds (a[0]-a[1])*(a[1]-a[0])+(a[0]*a[0])+(a[1]*a[1])
		* r[10] holds (a[0]*a[0])
		* r[32] holds (a[1]*a[1])
		* c1 holds the carry bits
		*/
		c1+=(int)(bn_add_words(&(r[n]),&(r[n]),&(t[n2]),n2));
		if (c1)
		{
			p= &(r[n+n2]);
			lo= *p;
			ln=(lo+c1)&BN_MASK2;
			*p=ln;

			/* The overflow will stop before we over write
			* words we should not overwrite */
			if (ln < (BN_ULONG)c1)
			{
				do	{
					p++;
					lo= *p;
					ln=(lo+1)&BN_MASK2;
					*p=ln;
				} while (ln == 0);
			}
		}
	}

	int BN_sqr(BIGNUM *r, const BIGNUM *a, BN_CTX *ctx)
	{
		int max,al;
		int ret = 0;
		BIGNUM *tmp,*rr;

#ifdef BN_COUNT
		fprintf(stderr,"BN_sqr %d * %d\n",a->top,a->top);
#endif
		bn_check_top(a);

		al=a->top;
		if (al <= 0)
		{
			r->top=0;
			return(1);
		}

		BN_CTX_start(ctx);
		rr=(a != r) ? r : BN_CTX_get(ctx);
		if (rr == NULL) goto err;
		tmp=BN_CTX_get(ctx);
		if (tmp == NULL) goto err;

		max=(al+al);
		if (bn_wexpand(rr,max+1) == NULL) goto err;

		if (al == 4)
		{
#ifndef BN_SQR_COMBA
			BN_ULONG t[8];
			bn_sqr_normal(rr->d,a->d,4,t);
#else
			bn_sqr_comba4(rr->d,a->d);
#endif
		}
		else if (al == 8)
		{
#ifndef BN_SQR_COMBA
			BN_ULONG t[16];
			bn_sqr_normal(rr->d,a->d,8,t);
#else
			bn_sqr_comba8(rr->d,a->d);
#endif
		}
		else 
		{
#if defined(BN_RECURSION)
			if (al < BN_SQR_RECURSIVE_SIZE_NORMAL)
			{
				BN_ULONG t[BN_SQR_RECURSIVE_SIZE_NORMAL*2];
				bn_sqr_normal(rr->d,a->d,al,t);
			}
			else
			{
				int j,k;
				
				j=BN_num_bits_word((BN_ULONG)al);
				j=1<<(j-1);
				k=j+j;
				if (al == j)
				{
					if (bn_wexpand(tmp,k*2) == NULL) goto err;
					bn_sqr_recursive(rr->d,a->d,al,tmp->d);
				}
				else
				{
					if (bn_wexpand(tmp,max) == NULL) goto err;
					bn_sqr_normal(rr->d,a->d,al,tmp->d);
				}
			}
#else
			if (bn_wexpand(tmp,max) == NULL) goto err;
			bn_sqr_normal(rr->d,a->d,al,tmp->d);
#endif
		}

		rr->top=max;
		rr->neg=0;
		if ((max > 0) && (rr->d[max-1] == 0)) rr->top--;
		if (rr != r) BN_copy(r,rr);
		ret = 1;
err:
		BN_CTX_end(ctx);
		return(ret);
	}

	int BN_set_word(BIGNUM *a, BN_ULONG w)
	{
		int i,n;
		if (bn_expand(a,(int)(sizeof(BN_ULONG)*8)) == NULL) return(0);

		n=sizeof(BN_ULONG)/BN_BYTES;
		a->neg=0;
		a->top=0;
		a->d[0]=(BN_ULONG)w&BN_MASK2;
		if (a->d[0] != 0) a->top=1;
     /*lint -e681 loop not entered!! */
		for (i=1; i<n; i++)
		{
		/* the following is done instead of
		* w>>=BN_BITS2 so compilers don't complain
			* on builds where sizeof(long) == BN_TYPES */
#ifndef SIXTY_FOUR_BIT /* the data item > uint32 */
			w>>=BN_BITS4;
			w>>=BN_BITS4;
#else
			w=0;
#endif
			a->d[i]=(BN_ULONG)w&BN_MASK2;
			if (a->d[i] != 0) a->top=i+1;
		}
     /*lint +e681 loop not entered!! */
		return(1);                        
	}


	int BN_ucmp(const BIGNUM *a, const BIGNUM *b)
	{
		int i;
		BN_ULONG t1,t2,*ap,*bp;

		bn_check_top(a);
		bn_check_top(b);

		i=a->top-b->top;
		if (i != 0) return(i);
		ap=a->d;
		bp=b->d;
		for (i=a->top-1; i>=0; i--)
		{
			t1= ap[i];
			t2= bp[i];
			if (t1 != t2)
				return(t1 > t2?1:-1);
		}
		return(0);
	}

	void BN_init(BIGNUM *a)
	{
		memset(a,0,sizeof(BIGNUM));
	}
	void BN_CTX_init(BN_CTX *ctx)
	{
#if 0 /* explicit version */
		int i;
		ctx->tos = 0;
		ctx->flags = 0;
		ctx->depth = 0;
		ctx->too_many = 0;
		for (i = 0; i < BN_CTX_NUM; i++)
			BN_init(&(ctx->bn[i]));
#else
		memset(ctx, 0, sizeof *ctx);
#endif
	}
	BN_CTX *BN_CTX_new(void)
	{
		BN_CTX *ret;
		
		ret=(BN_CTX *)OPENSSL_malloc(sizeof(BN_CTX));
		if (ret == NULL)
		{
			AS_printf("BN_CTX_NEW_MALLOC_FAILURE\n");
			return(NULL);
		}
		
		BN_CTX_init(ret);
		ret->flags=BN_FLG_MALLOCED;
		return(ret);
	}
	void BN_MONT_CTX_init(BN_MONT_CTX *ctx)
	{
		ctx->ri=0;
		BN_init(&(ctx->RR));
		BN_init(&(ctx->N));
		BN_init(&(ctx->Ni));
		ctx->flags=0;
	}
	BN_MONT_CTX *BN_MONT_CTX_new(void)
	{
		BN_MONT_CTX *ret;
		
		if ((ret=(BN_MONT_CTX *)OPENSSL_malloc(sizeof(BN_MONT_CTX))) == NULL)
			return(NULL);
		
		BN_MONT_CTX_init(ret);
		ret->flags=BN_FLG_MALLOCED;
		return(ret);
	}

	int BN_set_bit(BIGNUM *a, int n)
	{
		int i,j,k;
		
		i=n/BN_BITS2;
		j=n%BN_BITS2;
		if (a->top <= i)
		{
			if (bn_wexpand(a,i+1) == NULL) return(0);
			for(k=a->top; k<i+1; k++)
				a->d[k]=0;
			a->top=i+1;
		}

		a->d[i]|=(((BN_ULONG)1)<<j);
		return(1);
	}
	int BN_lshift(BIGNUM *r, const BIGNUM *a, int n)
	{
		int i,nw,lb,rb;
		BN_ULONG *t,*f;
		BN_ULONG l;
		
		r->neg=a->neg;
		nw=n/BN_BITS2;
		if (bn_wexpand(r,a->top+nw+1) == NULL) return(0);
		lb=n%BN_BITS2;
		rb=BN_BITS2-lb;
		f=a->d;
		t=r->d;
		t[a->top+nw]=0;
		if (lb == 0)
			for (i=a->top-1; i>=0; i--)
				t[nw+i]=f[i];
			else
				for (i=a->top-1; i>=0; i--)
				{
					l=f[i];
					t[nw+i+1]|=(l>>rb)&BN_MASK2;
					t[nw+i]=(l<<lb)&BN_MASK2;
				}
				memset(t,0,nw*sizeof(t[0]));
				/*	for (i=0; i<nw; i++)
				t[i]=0;*/
				r->top=a->top+nw+1;
				bn_fix_top(r);
				return(1);
	}

	int BN_add_word(BIGNUM *a, BN_ULONG w)
	{
		BN_ULONG l;
		int i;
		
		if (a->neg)
		{
			a->neg=0;
			i=BN_sub_word(a,w);
			if (!BN_is_zero(a))
				a->neg=!(a->neg);
			return(i);
		}
		w&=BN_MASK2;
		if (bn_wexpand(a,a->top+1) == NULL) return(0);
		i=0;
		for (;;)
		{
			if (i >= a->top)
				l=w;
			else
				l=(a->d[i]+(BN_ULONG)w)&BN_MASK2;
			a->d[i]=l;
			if (w > l)
				w=1;
			else
				break;
			i++;
		}
		if (i >= a->top)
			a->top++;
		return(1);
	}


	int BN_sub_word(BIGNUM *a, BN_ULONG w)
	{
		int i;
		
		if (BN_is_zero(a) || a->neg)
		{
			a->neg=0;
			i=BN_add_word(a,w);
			a->neg=1;
			return(i);
		}

		w&=BN_MASK2;
		if ((a->top == 1) && (a->d[0] < w))
		{
			a->d[0]=w-a->d[0];
			a->neg=1;
			return(1);
		}
		i=0;
		for (;;)
		{
			if (a->d[i] >= w)
			{
				a->d[i]-=w;
				break;
			}
			else
			{
				a->d[i]=(a->d[i]-w)&BN_MASK2;
				i++;
				w=1;
			}
		}
		if ((a->d[i] == 0) && (i == (a->top-1)))
			a->top--;
		return(1);
	}


	/* solves ax == 1 (mod n) */
  /*lint -e506 constant boolean*/
	BIGNUM *BN_mod_inverse(BIGNUM *in,
		const BIGNUM *a, const BIGNUM *n, BN_CTX *ctx)
	{
		BIGNUM *A,*B,*X,*Y,*M,*D,*T,*R=NULL;
		BIGNUM *ret=NULL;
		int sign;

		bn_check_top(a);
		bn_check_top(n);

		BN_CTX_start(ctx);
		A = BN_CTX_get(ctx);
		B = BN_CTX_get(ctx);
		X = BN_CTX_get(ctx);
		D = BN_CTX_get(ctx);
		M = BN_CTX_get(ctx);
		Y = BN_CTX_get(ctx);
		T = BN_CTX_get(ctx);
		if (!T || !Y || !M || !D || !X || !B || !A) goto err;

		if (in == NULL)
			R=BN_new();
		else
			R=in;
		if (R == NULL) goto err;

		BN_one(X);
		BN_zero(Y);
		if (BN_copy(B,a) == NULL) goto err;
		if (BN_copy(A,n) == NULL) goto err;
		A->neg = 0;
		if (B->neg || (BN_ucmp(B, A) >= 0))
		{
			if (!BN_nnmod(B, B, A, ctx)) goto err;
		}
		sign = -1;
		/* From  B = a mod |n|,  A = |n|  it follows that
		*
		*      0 <= B < A,
		*     -sign*X*a  ==  B   (mod |n|),
		*      sign*Y*a  ==  A   (mod |n|).
		*/

		if (BN_is_odd(n) && (BN_num_bits(n) <= (BN_BITS <= 32 ? 450 : 2048)))
		{
		/* Binary inversion algorithm; requires odd modulus.
		* This is faster than the general algorithm if the modulus
		* is sufficiently small (about 400 .. 500 bits on 32-bit
			* sytems, but much more on 64-bit systems) */
			int shift;

			while (!BN_is_zero(B))
			{
			/*
			*      0 < B < |n|,
			*      0 < A <= |n|,
			* (1) -sign*X*a  ==  B   (mod |n|),
			* (2)  sign*Y*a  ==  A   (mod |n|)
				*/

				/* Now divide  B  by the maximum possible power of two in the integers,
				* and divide  X  by the same value mod |n|.
				* When we're done, (1) still holds. */
				shift = 0;
				while (!BN_is_bit_set(B, shift)) /* note that 0 < B */
				{
					shift++;
					
					if (BN_is_odd(X))
					{
						if (!BN_uadd(X, X, n)) goto err;
					}
					/* now X is even, so we can easily divide it by two */
					if (!BN_rshift1(X, X)) goto err;
				}
				if (shift > 0)
				{
					if (!BN_rshift(B, B, shift)) goto err;
				}

				/* Same for  A  and  Y.  Afterwards, (2) still holds. */
				shift = 0;
				while (!BN_is_bit_set(A, shift)) /* note that 0 < A */
				{
					shift++;
					
					if (BN_is_odd(Y))
					{
						if (!BN_uadd(Y, Y, n)) goto err;
					}
					/* now Y is even */
					if (!BN_rshift1(Y, Y)) goto err;
				}
				if (shift > 0)
				{
					if (!BN_rshift(A, A, shift)) goto err;
				}

				/* We still have (1) and (2).
				* Both  A  and  B  are odd.
				* The following computations ensure that
				*
				*     0 <= B < |n|,
				*      0 < A < |n|,
				* (1) -sign*X*a  ==  B   (mod |n|),
				* (2)  sign*Y*a  ==  A   (mod |n|),
				*
				* and that either  A  or  B  is even in the next iteration.
				*/
				if (BN_ucmp(B, A) >= 0)
				{
					/* -sign*(X + Y)*a == B - A  (mod |n|) */
					if (!BN_uadd(X, X, Y)) goto err;
					/* NB: we could use BN_mod_add_quick(X, X, Y, n), but that
					* actually makes the algorithm slower */
					if (!BN_usub(B, B, A)) goto err;
				}
				else
				{
					/*  sign*(X + Y)*a == A - B  (mod |n|) */
					if (!BN_uadd(Y, Y, X)) goto err;
					/* as above, BN_mod_add_quick(Y, Y, X, n) would slow things down */
					if (!BN_usub(A, A, B)) goto err;
				}
			}
		}
		else
		{
			/* general inversion algorithm */
			while (!BN_is_zero(B))
			{
				BIGNUM *tmp;

				/*
				*      0 < B < A,
				* (*) -sign*X*a  ==  B   (mod |n|),
				*      sign*Y*a  ==  A   (mod |n|)
				*/

				/* (D, M) := (A/B, A%B) ... */
				if (BN_num_bits(A) == BN_num_bits(B))
				{
					if (!BN_one(D)) goto err;
					if (!BN_sub(M,A,B)) goto err;
				}
				else if (BN_num_bits(A) == BN_num_bits(B) + 1)
				{
					/* A/B is 1, 2, or 3 */
					if (!BN_lshift1(T,B)) goto err;
					if (BN_ucmp(A,T) < 0)
					{
						/* A < 2*B, so D=1 */
						if (!BN_one(D)) goto err;
						if (!BN_sub(M,A,B)) goto err;
					}
					else
					{
						/* A >= 2*B, so D=2 or D=3 */
						if (!BN_sub(M,A,T)) goto err;
						if (!BN_add(D,T,B)) goto err; /* use D (:= 3*B) as temp */
						if (BN_ucmp(A,D) < 0)
						{
							/* A < 3*B, so D=2 */
							if (!BN_set_word(D,2)) goto err;
							/* M (= A - 2*B) already has the correct value */
						}
						else
						{
							/* only D=3 remains */
							if (!BN_set_word(D,3)) goto err;
							/* currently  M = A - 2*B,  but we need  M = A - 3*B */
							if (!BN_sub(M,M,B)) goto err;
						}
					}
				}
				else
				{
					if (!BN_div(D,M,A,B,ctx)) goto err;
				}

				/* Now
				*      A = D*B + M;
				* thus we have
				* (**)  sign*Y*a  ==  D*B + M   (mod |n|).
				*/

				tmp=A; /* keep the BIGNUM object, the value does not matter */

				/* (A, B) := (B, A mod B) ... */
				A=B;
				B=M;
				/* ... so we have  0 <= B < A  again */

				/* Since the former  M  is now  B  and the former  B  is now  A,
				* (**) translates into
				*       sign*Y*a  ==  D*A + B    (mod |n|),
				* i.e.
				*       sign*Y*a - D*A  ==  B    (mod |n|).
				* Similarly, (*) translates into
				*      -sign*X*a  ==  A          (mod |n|).
				*
				* Thus,
				*   sign*Y*a + D*sign*X*a  ==  B  (mod |n|),
				* i.e.
				*        sign*(Y + D*X)*a  ==  B  (mod |n|).
				*
				* So if we set  (X, Y, sign) := (Y + D*X, X, -sign),  we arrive back at
				*      -sign*X*a  ==  B   (mod |n|),
				*       sign*Y*a  ==  A   (mod |n|).
				* Note that  X  and  Y  stay non-negative all the time.
				*/

				/* most of the time D is very small, so we can optimize tmp := D*X+Y */
				if (BN_is_one(D))
				{
					if (!BN_add(tmp,X,Y)) goto err;
				}
				else
				{
					if (BN_is_word(D,2))
					{
						if (!BN_lshift1(tmp,X)) goto err;
					}
					else if (BN_is_word(D,4))
					{
						if (!BN_lshift(tmp,X,2)) goto err;
					}
					else if (D->top == 1)
					{
						if (!BN_copy(tmp,X)) goto err;
						if (!BN_mul_word(tmp,D->d[0])) goto err;
					}
					else
					{
						if (!BN_mul(tmp,D,X,ctx)) goto err;
					}
					if (!BN_add(tmp,tmp,Y)) goto err;
				}

				M=Y; /* keep the BIGNUM object, the value does not matter */
				Y=X;
				X=tmp;
				sign = -sign;
			}
		}

		/*
		* The while loop (Euclid's algorithm) ends when
		*      A == gcd(a,n);
		* we have
		*       sign*Y*a  ==  A  (mod |n|),
		* where  Y  is non-negative.
		*/

		if (sign < 0)
		{
			if (!BN_sub(Y,n,Y)) goto err;
		}
		/* Now  Y*a  ==  A  (mod |n|).  */

		if (BN_is_one(A))
		{
			/* Y*a == 1  (mod |n|) */
			if (!Y->neg && BN_ucmp(Y,n) < 0)
			{
				if (!BN_copy(R,Y)) goto err;
			}
			else
			{
				if (!BN_nnmod(R,Y,n,ctx)) goto err;
			}
		}
		else
		{
			AS_printf("BN_NO_INVERSE\n");
			goto err;
		}
		ret=R;
err:
		if ((ret == NULL) && (in == NULL)) BN_free(R);
		BN_CTX_end(ctx);
		return(ret);
	}
/*lint +e506 constant boolean*/

	int BN_MONT_CTX_set(BN_MONT_CTX *mont, const BIGNUM *mod, BN_CTX *ctx)
	{
		BIGNUM Ri,*R;
		//  unsigned char p_buffer[256];
		//	int count;
		BN_init(&Ri);
		R= &(mont->RR);					/* grab RR as a temp */
		BN_copy(&(mont->N),mod);			/* Set N */
		mont->N.neg = 0;

#ifdef MONT_WORD
		{
			BIGNUM tmod;
			BN_ULONG buf[2];

			mont->ri=(BN_num_bits(mod)+(BN_BITS2-1))/BN_BITS2*BN_BITS2;
			if (!(BN_zero(R))) goto err;
			if (!(BN_set_bit(R,BN_BITS2))) goto err;	/* R */

			buf[0]=mod->d[0]; /* tmod = N mod word size */
			buf[1]=0;
			tmod.d=buf;
			tmod.top=1;
			tmod.dmax=2;
			tmod.neg=0;
			/* Ri = R^-1 mod N*/
			if ((BN_mod_inverse(&Ri,R,&tmod,ctx)) == NULL)
				goto err;
				/* BN_bn2bin(&Ri,p_buffer);
				AS_printf("Ri -> ");
				for (count=0;count<128;count++ )
				AS_printf("%02x",p_buffer[count]);
				AS_printf("\n");
			*/	if (!BN_lshift(&Ri,&Ri,BN_BITS2)) goto err; /* R*Ri */
			if (!BN_is_zero(&Ri))
			{
				if (!BN_sub_word(&Ri,1)) goto err;
			}
			else /* if N mod word size == 1 */
			{
				if (!BN_set_word(&Ri,BN_MASK2)) goto err;  /* Ri-- (mod word size) */
			}
			if (!BN_div(&Ri,NULL,&Ri,&tmod,ctx)) goto err;
			/* Ni = (R*Ri-1)/N,
			* keep only least significant word: */
			mont->n0 = (Ri.top > 0) ? Ri.d[0] : 0;
			// BN_free(&Ri);  //< Removed because of GCC warning
		}
#else /* !MONT_WORD */
		{ /* bignum version */
			mont->ri=BN_num_bits(&mont->N);
			if (!BN_zero(R)) goto err;
			if (!BN_set_bit(R,mont->ri)) goto err;  /* R = 2^ri */
			/* Ri = R^-1 mod N*/
			if ((BN_mod_inverse(&Ri,R,&mont->N,ctx)) == NULL)
				goto err;
			if (!BN_lshift(&Ri,&Ri,mont->ri)) goto err; /* R*Ri */
			if (!BN_sub_word(&Ri,1)) goto err;
			/* Ni = (R*Ri-1) / N */
			if (!BN_div(&(mont->Ni),NULL,&Ri,&mont->N,ctx)) goto err;
			BN_free(&Ri);
		}
#endif

		/* setup RR for conversions */
		if (!BN_zero(&(mont->RR))) goto err;
		if (!BN_set_bit(&(mont->RR),mont->ri*2)) goto err;
		if (!BN_mod(&(mont->RR),&(mont->RR),&(mont->N),ctx)) goto err;

		return(1);
err:
		return(0);
	}

	void BN_MONT_CTX_free(BN_MONT_CTX *mont)
	{
		if(mont == NULL)
			return;
		
		BN_free(&(mont->RR));
		BN_free(&(mont->N));
		BN_free(&(mont->Ni));
		if (mont->flags & BN_FLG_MALLOCED)
			OPENSSL_free(mont);
	}

	int BN_sub(BIGNUM *r, const BIGNUM *a, const BIGNUM *b)
	{
		int max;
		int add=0,neg=0;
		const BIGNUM *tmp;

		bn_check_top(a);
		bn_check_top(b);

		/*  a -  b	a-b
		*  a - -b	a+b
		* -a -  b	-(a+b)
		* -a - -b	b-a
		*/
		if (a->neg)
		{
			if (b->neg)
			{ tmp=a; a=b; b=tmp; }
			else
			{ add=1; neg=1; }
		}
		else
		{
			if (b->neg) { add=1; neg=0; }
		}
		
		if (add)
		{
			if (!BN_uadd(r,a,b)) return(0);
			r->neg=neg;
			return(1);
		}

		/* We are actually doing a - b :-) */

		max=(a->top > b->top)?a->top:b->top;
		if (bn_wexpand(r,max) == NULL) return(0);
		if (BN_ucmp(a,b) < 0)
		{
			if (!BN_usub(r,b,a)) return(0);
			r->neg=1;
		}
		else
		{
			if (!BN_usub(r,a,b)) return(0);
			r->neg=0;
		}
		return(1);
	}

	int BN_add(BIGNUM *r, const BIGNUM *a, const BIGNUM *b)
	{
		const BIGNUM *tmp;
		int a_neg = a->neg;

		bn_check_top(a);
		bn_check_top(b);

		/*  a +  b	a+b
		*  a + -b	a-b
		* -a +  b	b-a
		* -a + -b	-(a+b)
		*/
		if (a_neg ^ b->neg)
		{
			/* only one is negative */
			if (a_neg)
			{ tmp=a; a=b; b=tmp; }
			
			/* we are now a - b */
			
			if (BN_ucmp(a,b) < 0)
			{
				if (!BN_usub(r,b,a)) return(0);
				r->neg=1;
			}
			else
			{
				if (!BN_usub(r,a,b)) return(0);
				r->neg=0;
			}
			return(1);
		}

		if (!BN_uadd(r,a,b)) return(0);
		if (a_neg) /* both are neg */
			r->neg=1;
		else
			r->neg=0;
		return(1);
	}

	int BN_nnmod(BIGNUM *r, const BIGNUM *m, const BIGNUM *d, BN_CTX *ctx)
	{
	/* like BN_mod, but returns non-negative remainder
		* (i.e.,  0 <= r < |d|  always holds) */

		if (!(BN_mod(r,m,d,ctx)))
			return 0;
		if (!r->neg)
			return 1;
		/* now   -|d| < r < 0,  so we have to set  r := r + |d| */
		return (d->neg ? BN_sub : BN_add)(r, r, d);
	}

	void BN_CTX_end(BN_CTX *ctx)
	{
		if (ctx == NULL) return;
		assert(ctx->depth > 0);
		if (ctx->depth == 0)
		/* should never happen, but we can tolerate it if not in
		* debug mode (could be a 'goto err' in the calling function
		* before BN_CTX_start was reached) */
		BN_CTX_start(ctx);

		ctx->too_many = 0;
		ctx->depth--;
		if (ctx->depth < BN_CTX_NUM_POS)
			ctx->tos = ctx->pos[ctx->depth];
	}

	void BN_CTX_free(BN_CTX *ctx)
	{
		int i;
		
		if (ctx == NULL) return;
		assert(ctx->depth == 0);
		
		for (i=0; i < BN_CTX_NUM; i++)
			BN_clear_free(&(ctx->bn[i]));
		if (ctx->flags & BN_FLG_MALLOCED)
			OPENSSL_free(ctx);
	}

	int BN_is_bit_set(const BIGNUM *a, int n)
	{
		int i,j;

		if (n < 0) return(0);
		i=n/BN_BITS2;
		j=n%BN_BITS2;
		if (a->top <= i) return(0);
		return((a->d[i]&(((BN_ULONG)1)<<j))?1:0);
	}
	BIGNUM *BN_copy(BIGNUM *a, const BIGNUM *b)
	{
		int i;
		BN_ULONG *A;
		const BN_ULONG *B;

		bn_check_top(b);

		if (a == b) return(a);
		if (bn_wexpand(a,b->top) == NULL) return(NULL);

#if 1
		A=a->d;
		B=b->d;
		for (i=b->top>>2; i>0; i--,A+=4,B+=4)
		{
			BN_ULONG a0,a1,a2,a3;
			a0=B[0]; a1=B[1]; a2=B[2]; a3=B[3];
			A[0]=a0; A[1]=a1; A[2]=a2; A[3]=a3;
		}
		switch (b->top&3)
		{
		case 3: A[2]=B[2];
     /* fall through */
		case 2: A[1]=B[1];
     /* fall through */
		case 1: A[0]=B[0];
     /* fall through */
	  	case 0: ; /* ultrix cc workaround, see comments in bn_expand_internal */
		}
#else
		memcpy(a->d,b->d,sizeof(b->d[0])*b->top);
#endif

		/*	memset(&(a->d[b->top]),0,sizeof(a->d[0])*(a->max-b->top));*/
		a->top=b->top;
		if ((a->top == 0) && (a->d != NULL))
			a->d[0]=0;
		a->neg=b->neg;
		return(a);
	}

	int BN_from_montgomery(BIGNUM *ret, const BIGNUM *a, BN_MONT_CTX *mont,
		BN_CTX *ctx)
	{
		int retn=0;

#ifdef MONT_WORD
		BIGNUM *n,*r;
		BN_ULONG *ap,*np,*rp,n0,v,*nrp;
		int al,nl,max,i,x,ri;

		BN_CTX_start(ctx);
		if ((r = BN_CTX_get(ctx)) == NULL) goto err;

		if (!BN_copy(r,a)) goto err;
		n= &(mont->N);

		//ap=a->d; remove warning about assigning and never using!
		/* mont->ri is the size of mont->N in bits (rounded up
		to the word size) */
		al=ri=mont->ri/BN_BITS2;

		nl=n->top;
		if ((al == 0) || (nl == 0)) { r->top=0; return(1); }

		max=(nl+al+1); /* allow for overflow (no?) XXX */
		if (bn_wexpand(r,max) == NULL) goto err;
		if (bn_wexpand(ret,max) == NULL) goto err;

		r->neg=a->neg^n->neg;
		np=n->d;
		rp=r->d;
		nrp= &(r->d[nl]);

		/* clear the top words of T */
#if 1
		for (i=r->top; i<max; i++) /* memset? XXX */
			r->d[i]=0;
#else
		memset(&(r->d[r->top]),0,(max-r->top)*sizeof(BN_ULONG)); 
#endif

		r->top=max;
		n0=mont->n0;

#ifdef BN_COUNT
		fprintf(stderr,"word BN_from_montgomery %d * %d\n",nl,nl);
#endif
		for (i=0; i<nl; i++)
		{
#ifdef __TANDEM
			{
				long long t1;
				long long t2;
				long long t3;
				t1 = rp[0] * (n0 & 0177777);
				t2 = 037777600000l;
				t2 = n0 & t2;
				t3 = rp[0] & 0177777;
				t2 = (t3 * t2) & BN_MASK2;
				t1 = t1 + t2;
				v=bn_mul_add_words(rp,np,nl,(BN_ULONG) t1);
			}
#else
			v=bn_mul_add_words(rp,np,nl,(rp[0]*n0)&BN_MASK2);
#endif
			nrp++;
			rp++;
			if (((nrp[-1]+=v)&BN_MASK2) >= v)
				continue;
			else
			{
				if (((++nrp[0])&BN_MASK2) != 0) continue;
				if (((++nrp[1])&BN_MASK2) != 0) continue;
				for (x=2; (((++nrp[x])&BN_MASK2) == 0); x++) ;
			}
		}
		bn_fix_top(r);

		/* mont->ri will be a multiple of the word size */
#if 0
		BN_rshift(ret,r,mont->ri);
#else
		ret->neg = r->neg;
		x=ri;
		rp=ret->d;
		ap= &(r->d[x]);
		if (r->top < x)
			al=0;
		else
			al=r->top-x;
		ret->top=al;
		al-=4;
		for (i=0; i<al; i+=4)
		{
			BN_ULONG t1,t2,t3,t4;
			
			t1=ap[i+0];
			t2=ap[i+1];
			t3=ap[i+2];
			t4=ap[i+3];
			rp[i+0]=t1;
			rp[i+1]=t2;
			rp[i+2]=t3;
			rp[i+3]=t4;
		}
		al+=4;
		for (; i<al; i++)
			rp[i]=ap[i];
#endif
#else /* !MONT_WORD */ 
		BIGNUM *t1,*t2;

		BN_CTX_start(ctx);
		t1 = BN_CTX_get(ctx);
		t2 = BN_CTX_get(ctx);
		if (t1 == NULL || t2 == NULL) goto err;

		if (!BN_copy(t1,a)) goto err;
		BN_mask_bits(t1,mont->ri);

		if (!BN_mul(t2,t1,&mont->Ni,ctx)) goto err;
		BN_mask_bits(t2,mont->ri);

		if (!BN_mul(t1,t2,&mont->N,ctx)) goto err;
		if (!BN_add(t2,a,t1)) goto err;
		if (!BN_rshift(ret,t2,mont->ri)) goto err;
#endif /* MONT_WORD */

		if (BN_ucmp(ret, &(mont->N)) >= 0)
		{
			if (!BN_usub(ret,ret,&(mont->N))) goto err;
		}
		retn=1;
err:
		BN_CTX_end(ctx);
		return(retn);
	}

	int BN_mod_mul_montgomery(BIGNUM *r, const BIGNUM *a, const BIGNUM *b,
		BN_MONT_CTX *mont, BN_CTX *ctx)
	{
		BIGNUM *tmp;
		int ret=0;

		BN_CTX_start(ctx);
		tmp = BN_CTX_get(ctx);
		if (tmp == NULL) goto err;

		bn_check_top(tmp);
		if (a == b)
		{
			if (!BN_sqr(tmp,a,ctx)) goto err;
		}
		else
		{
			if (!BN_mul(tmp,a,b,ctx)) goto err;
		}
		/* reduce from aRR to aR */
		if (!BN_from_montgomery(r,tmp,mont,ctx)) goto err;
		ret=1;
err:
		BN_CTX_end(ctx);
		return(ret);
	}

	const BIGNUM *BN_value_one(void)
	{
		static BN_ULONG data_one=1L;
		static BIGNUM const_one={&data_one,1,1,0,0};

		return(&const_one);
	}

	int BN_mod_exp_mont(BIGNUM *rr, const BIGNUM *a, const BIGNUM *p,
		const BIGNUM *m, BN_CTX *ctx, BN_MONT_CTX *in_mont)
	{
		int i,j,bits,ret=0,wstart,wend,window,wvalue;
		int start,ts=0;
		BIGNUM *d,*r;
		const BIGNUM *aa;
		BIGNUM val[TABLE_SIZE];
		BN_MONT_CTX *mont=NULL;

		bn_check_top(a);
		bn_check_top(p);
		bn_check_top(m);

		if (!(m->d[0] & 1))
		{
			AS_printf("BN_BN_MOD_EXP_MONT_CALLED_WITH_EVEN_MODULUS\n");
			return(0);
		}
		bits=BN_num_bits(p);
		if (bits == 0)
		{
			ret = BN_one(rr);
			return ret;
		}

		BN_CTX_start(ctx);
		d = BN_CTX_get(ctx);
		r = BN_CTX_get(ctx);
		if (d == NULL || r == NULL) goto err;

		/* If this is not done, things will break in the montgomery
		* part */

		if (in_mont != NULL)
			mont=in_mont;
		else
		{
			if ((mont=BN_MONT_CTX_new()) == NULL) goto err;
			if (!BN_MONT_CTX_set(mont,m,ctx)) goto err;
		}

		BN_init(&val[0]);
		ts=1;
		if (a->neg || BN_ucmp(a,m) >= 0)
		{
			if (!BN_nnmod(&(val[0]),a,m,ctx))
				goto err;
			aa= &(val[0]);
		}
		else
			aa=a;
		if (BN_is_zero(aa))
		{
			ret = BN_zero(rr);
			goto err;
		}
		if (!BN_to_montgomery(&(val[0]),aa,mont,ctx)) goto err; /* 1 */

		window = BN_window_bits_for_exponent_size(bits);
		if (window > 1)
		{
			if (!BN_mod_mul_montgomery(d,&(val[0]),&(val[0]),mont,ctx)) goto err; /* 2 */
			j=1<<(window-1);
			for (i=1; i<j; i++)
			{
				BN_init(&(val[i]));
				if (!BN_mod_mul_montgomery(&(val[i]),&(val[i-1]),d,mont,ctx))
					goto err;
			}
			ts=i;
		}

		start=1;	/* This is used to avoid multiplication etc
					* when there is only the value '1' in the
		* buffer. */
     //remove warning about assigning and never using!
		//wvalue=0;	/* The 'value' of the window */
		wstart=bits-1;	/* The top bit of the window */
     //remove warning about assigning and never using!
		//wend=0;		/* The bottom bit of the window */

		if (!BN_to_montgomery(r,BN_value_one(),mont,ctx)) goto err;
		for (;;)
		{
			if (BN_is_bit_set(p,wstart) == 0)
			{
				if (!start)
				{
					if (!BN_mod_mul_montgomery(r,r,r,mont,ctx))
						goto err;
				}
				if (wstart == 0) break;
				wstart--;
				continue;
			}
			/* We now have wstart on a 'set' bit, we now need to work out
			* how bit a window to do.  To do this we need to scan
			* forward until the last set bit before the end of the
			* window */
			//j=wstart;  remove warning about assigning and never using!
			wvalue=1;
			wend=0;
			for (i=1; i<window; i++)
			{
				if (wstart-i < 0) break;
				if (BN_is_bit_set(p,wstart-i))
				{
					wvalue<<=(i-wend);
					wvalue|=1;
					wend=i;
				}
			}

			/* wend is the size of the current window */
			j=wend+1;
			/* add the 'bytes above' */
			if (!start)
				for (i=0; i<j; i++)
				{
					if (!BN_mod_mul_montgomery(r,r,r,mont,ctx))
						goto err;
				}

				/* wvalue will be an odd number < 2^window */
				if (!BN_mod_mul_montgomery(r,r,&(val[wvalue>>1]),mont,ctx))
					goto err;

				/* move the 'window' down further */
				wstart-=wend+1;
				//wvalue=0; remove warning about assigning and never using!
				start=0;
				if (wstart < 0) break;
		}
		if (!BN_from_montgomery(rr,r,mont,ctx)) goto err;
		ret=1;
err:
		if ((in_mont == NULL) && (mont != NULL)) BN_MONT_CTX_free(mont);
		BN_CTX_end(ctx);
		for (i=0; i<ts; i++)
			BN_clear_free(&(val[i]));
		return(ret);
	}

	int RSA_padding_check_PKCS1_type_1(unsigned char *to, int tlen,
		const unsigned char *from, int flen, int num)
	{
		int i,j;
		const unsigned char *p;

		p=from;
		if ((num != (flen+1)) || (*(p++) != 01))
		{
			AS_printf("RSA_PADDING_CHECK_PKCS1_TYPE_1_BLOCK_TYPE_IS_NOT_01\n");
			return(-1);
		}

		/* scan over padding data */
		j=flen-1; /* one for type. */
		for (i=0; i<j; i++)
		{
			if (*p != 0xff) /* should decrypt to 0xff */
			{
				if (*p == 0)
				{ p++; break; }
				else	{
					AS_printf("RSA_PADDING_CHECK_PKCS1_TYPE_1_BAD_FIXED_HEADER_DECRYPT\n");
					return(-1);
				}
			}
			p++;
		}

		if (i == j)
		{
			AS_printf("RSA_PADDING_CHECK_PKCS1_TYPE_1_NULL_BEFORE_BLOCK_MISSING\n");
			return(-1);
		}

		if (i < 8)
		{
			AS_printf("RSA_RSA_PADDING_CHECK_PKCS1_TYPE_1_BYTE_COUNT\n");
			return(-1);
		}
		i++; /* Skip over the '\0' */
		j-=i;
		if (j > tlen)
		{
			AS_printf("RSA_PADDING_CHECK_PKCS1_TYPE_1_DATA_TOO_LARGE\n");
			return(-1);
		}
		memcpy(to,p,(unsigned int)j);

		return(j);
	}

	int RSA_padding_add_none(unsigned char *to, int tlen,
		const unsigned char *from, int flen)
	{
		if (flen > tlen)
		{
			AS_printf("RSA_PADDING_ADD_NONE_DATA_TOO_LARGE_FOR_KEY_SIZE\n");
			return(0);
		}

		if (flen < tlen)
		{
			AS_printf("RSA_RSA_PADDING_ADD_NONE_DATA_TOO_SMALL_FOR_KEY_SIZE\n");
			return(0);
		}

		memcpy(to,from,(unsigned int)flen);
		return(1);
	}

	int RSA_padding_check_none(unsigned char *to, int tlen,
		const unsigned char *from, int flen, int num)
	{

		if (flen > tlen)
		{
			AS_printf("RSA_PADDING_CHECK_NONE_DATA_TOO_LARGE\n");
			return(-1);
		}

		memset(to,0,tlen-flen);
		memcpy(to+tlen-flen,from,flen);
		return(tlen);
	}

	void bn_sqr_comba4(BN_ULONG *r, BN_ULONG *a)
	{
		BN_ULONG t[8];
		bn_sqr_normal(r,a,4,t);
	}
	void bn_sqr_comba8(BN_ULONG *r, BN_ULONG *a)
	{
		BN_ULONG t[16];
		bn_sqr_normal(r,a,8,t);
	}
	void bn_mul_comba4(BN_ULONG *r, BN_ULONG *a, BN_ULONG *b)
	{
		r[4]=bn_mul_words(    &(r[0]),a,4,b[0]);
		r[5]=bn_mul_add_words(&(r[1]),a,4,b[1]);
		r[6]=bn_mul_add_words(&(r[2]),a,4,b[2]);
		r[7]=bn_mul_add_words(&(r[3]),a,4,b[3]);
	}
	void bn_mul_comba8(BN_ULONG *r, BN_ULONG *a, BN_ULONG *b)
	{
		r[ 8]=bn_mul_words(    &(r[0]),a,8,b[0]);
		r[ 9]=bn_mul_add_words(&(r[1]),a,8,b[1]);
		r[10]=bn_mul_add_words(&(r[2]),a,8,b[2]);
		r[11]=bn_mul_add_words(&(r[3]),a,8,b[3]);
		r[12]=bn_mul_add_words(&(r[4]),a,8,b[4]);
		r[13]=bn_mul_add_words(&(r[5]),a,8,b[5]);
		r[14]=bn_mul_add_words(&(r[6]),a,8,b[6]);
		r[15]=bn_mul_add_words(&(r[7]),a,8,b[7]);
	}
	int BN_usub(BIGNUM *r, const BIGNUM *a, const BIGNUM *b)
	{
		int max,min;
		register BN_ULONG t1,t2,*ap,*bp,*rp;
		int i,carry;
#if defined(IRIX_CC_BUG) && !defined(LINT)
		int dummy;
#endif

		bn_check_top(a);
		bn_check_top(b);

		if (a->top < b->top) /* hmm... should not be happening */
		{
			AS_printf("BN_USUB_ARG2_LT_ARG3\n");
			return(0);
		}

		max=a->top;
		min=b->top;
		if (bn_wexpand(r,max) == NULL) return(0);

		ap=a->d;
		bp=b->d;
		rp=r->d;

#if 1
		carry=0;
		for (i=0; i<min; i++)
		{
			t1= *(ap++);
			t2= *(bp++);
			if (carry)
			{
				carry=(t1 <= t2);
				t1=(t1-t2-1)&BN_MASK2;
			}
			else
			{
				carry=(t1 < t2);
				t1=(t1-t2)&BN_MASK2;
			}
#if defined(IRIX_CC_BUG) && !defined(LINT)
			dummy=t1;
#endif
			*(rp++)=t1&BN_MASK2;
		}
#else
		carry=bn_sub_words(rp,ap,bp,min);
		ap+=min;
		bp+=min;
		rp+=min;
		i=min;
#endif
		if (carry) /* subtracted */
		{
			while (i < max)
			{
				i++;
				t1= *(ap++);
				t2=(t1-1)&BN_MASK2;
				*(rp++)=t2;
				if (t1 > t2) break;
			}
		}
#if 0
		memcpy(rp,ap,sizeof(*rp)*(max-i));
#else
		if (rp != ap)
		{
			for (;;)
			{
				if (i++ >= max) break;
				rp[0]=ap[0];
				if (i++ >= max) break;
				rp[1]=ap[1];
				if (i++ >= max) break;
				rp[2]=ap[2];
				if (i++ >= max) break;
				rp[3]=ap[3];
				rp+=4;
				ap+=4;
			}
		}
#endif

		r->top=max;
		r->neg=0;
		bn_fix_top(r);
		return(1);
	}
	/* unsigned add of b to a, r must be large enough */
	int BN_uadd(BIGNUM *r, const BIGNUM *a, const BIGNUM *b)
	{
		register int i;
		int max,min;
		BN_ULONG *ap,*bp,*rp,carry,t1;
		const BIGNUM *tmp;

		bn_check_top(a);
		bn_check_top(b);

		if (a->top < b->top)
		{ tmp=a; a=b; b=tmp; }
		max=a->top;
		min=b->top;

		if (bn_wexpand(r,max+1) == NULL)
			return(0);

		r->top=max;

		ap=a->d;
		bp=b->d;
		rp=r->d;
		//carry=0; remove warning about assigning and never using!

		carry=bn_add_words(rp,ap,bp,min);
		rp+=min;
		ap+=min;
		//bp+=min;    remove warning about assigning and never using!
		i=min;

		if (carry)
		{
			while (i < max)
			{
				i++;
				t1= *(ap++);
				if ((*(rp++)=(t1+1)&BN_MASK2) >= t1)
				{
					carry=0;
					break;
				}
			}
			if ((i >= max) && carry)
			{
				*(rp++)=1;
				r->top++;
			}
		}
		if (rp != ap)
		{
			for (; i<max; i++)
				*(rp++)= *(ap++);
		}
		/* memcpy(rp,ap,sizeof(*ap)*(max-i));*/
		r->neg = 0;
		return(1);
	}

	int BN_rshift(BIGNUM *r, const BIGNUM *a, int n)
	{
		int i,j,nw,lb,rb;
		BN_ULONG *t,*f;
		BN_ULONG l,tmp;

		nw=n/BN_BITS2;
		rb=n%BN_BITS2;
		lb=BN_BITS2-rb;
		if (nw > a->top || a->top == 0)
		{
			BN_zero(r);
			return(1);
		}
		if (r != a)
		{
			r->neg=a->neg;
			if (bn_wexpand(r,a->top-nw+1) == NULL) return(0);
		}
		else
		{
			if (n == 0)
				return 1; /* or the copying loop will go berserk */
		}

		f= &(a->d[nw]);
		t=r->d;
		j=a->top-nw;
		r->top=j;

		if (rb == 0)
		{
			for (i=j+1; i > 0; i--)
				*(t++)= *(f++);
		}
		else
		{
			l= *(f++);
			for (i=1; i<j; i++)
			{
				tmp =(l>>rb)&BN_MASK2;
				l= *(f++);
				*(t++) =(tmp|(l<<lb))&BN_MASK2;
			}
			*(t++) =(l>>rb)&BN_MASK2;
		}
		*t=0;
		bn_fix_top(r);
		return(1);
	}
	int BN_div(BIGNUM *dv, BIGNUM *rem, const BIGNUM *m, const BIGNUM *d,
		BN_CTX *ctx)
	{
		int i,nm,nd;
		int ret = 0;
		BIGNUM *D;

		bn_check_top(m);
		bn_check_top(d);
		if (BN_is_zero(d))
		{
			AS_printf("BN_DIV_BY_ZERO\n");
			return(0);
		}

		if (BN_ucmp(m,d) < 0)
		{
			if (rem != NULL)
			{ if (BN_copy(rem,m) == NULL) return(0); }
			if (dv != NULL) BN_zero(dv);
			return(1);
		}

		BN_CTX_start(ctx);
		D = BN_CTX_get(ctx);
		if (dv == NULL) dv = BN_CTX_get(ctx);
		if (rem == NULL) rem = BN_CTX_get(ctx);
		if (D == NULL || dv == NULL || rem == NULL)
			goto end;

		nd=BN_num_bits(d);
		nm=BN_num_bits(m);
		if (BN_copy(D,d) == NULL) goto end;
		if (BN_copy(rem,m) == NULL) goto end;

		/* The next 2 are needed so we can do a dv->d[0]|=1 later
		* since BN_lshift1 will only work once there is a value :-) */
		BN_zero(dv);
		bn_wexpand(dv,1);
		dv->top=1;

		if (!BN_lshift(D,D,nm-nd)) goto end;
		for (i=nm-nd; i>=0; i--)
		{
			if (!BN_lshift1(dv,dv)) goto end;
			if (BN_ucmp(rem,D) >= 0)
			{
				dv->d[0]|=1;
				if (!BN_usub(rem,rem,D)) goto end;
			}
			/* CAN IMPROVE (and have now :=) */
			if (!BN_rshift1(D,D)) goto end;
		}
		rem->neg=BN_is_zero(rem)?0:m->neg;
		dv->neg=m->neg^d->neg;
		ret = 1;
end:
		BN_CTX_end(ctx);
		return(ret);
	}

	int BN_lshift1(BIGNUM *r, const BIGNUM *a)
	{
		register BN_ULONG *ap,*rp,t,c;
		int i;

		if (r != a)
		{
			r->neg=a->neg;
			if (bn_wexpand(r,a->top+1) == NULL) return(0);
			r->top=a->top;
		}
		else
		{
			if (bn_wexpand(r,a->top+1) == NULL) return(0);
		}
		ap=a->d;
		rp=r->d;
		c=0;
		for (i=0; i<a->top; i++)
		{
			t= *(ap++);
			*(rp++)=((t<<1)|c)&BN_MASK2;
			c=(t & BN_TBIT)?1:0;
		}
		if (c)
		{
			*rp=1;
			r->top++;
		}
		return(1);
	}

	int BN_rshift1(BIGNUM *r, const BIGNUM *a)
	{
		BN_ULONG *ap,*rp,t,c;
		int i;

		if (BN_is_zero(a))
		{
			BN_zero(r);
			return(1);
		}
		if (a != r)
		{
			if (bn_wexpand(r,a->top) == NULL) return(0);
			r->top=a->top;
			r->neg=a->neg;
		}
		ap=a->d;
		rp=r->d;
		c=0;
		for (i=a->top-1; i>=0; i--)
		{
			t=ap[i];
			rp[i]=((t>>1)&BN_MASK2)|c;
			c=(t&1)?BN_TBIT:0;
		}
		bn_fix_top(r);
		return(1);
	}

	int BN_mul_word(BIGNUM *a, BN_ULONG w)
	{
		BN_ULONG ll;

		w&=BN_MASK2;
		if (a->top)
		{
			if (w == 0)
				BN_zero(a);
			else
			{
				ll=bn_mul_words(a->d,a->d,a->top,w);
				if (ll)
				{
					if (bn_wexpand(a,a->top+1) == NULL) return(0);
					a->d[a->top++]=ll;
				}
			}
		}
		return(1);
	}

	/* signing */
	int RSA_eay_private_encrypt(int flen, const unsigned char *from,
		unsigned char *to, RSA *rsa, int padding)
	{
		BIGNUM f,ret;
		int i,j,k,num=0,r= -1;
		unsigned char *buf=NULL;
		BN_CTX *ctx;
		int local_blinding = 0;
		BN_BLINDING *blinding = NULL;

		BN_init(&f);
		BN_init(&ret);

		if ((ctx=BN_CTX_new()) == NULL) goto err;
		num=BN_num_bytes(rsa->n);
		if ((buf=(unsigned char *)OPENSSL_malloc(num)) == NULL)
		{
			AS_printf("RSA_EAY_PRIVATE_ENCRYPT_MALLOC_FAILURE\n");
			goto err;
		}

		switch (padding)
		{
		case RSA_NO_PADDING:
			i=RSA_padding_add_none(buf,num,from,flen);
			break;
		default:
			AS_printf("RSA_EAY_PRIVATE_ENCRYPT_PADDING_NOT_SUPPORTED\n");
			goto err;
		}
		if (i <= 0) goto err;

		if (BN_bin2bn(buf,num,&f) == NULL) goto err;

		if (BN_ucmp(&f, rsa->n) >= 0)
		{	
			/* usually the padding functions would catch this */
			AS_printf("RSA__EAY_PRIVATE_ENCRYPT_DATA_TOO_LARGE_FOR_MODULUS");
			goto err;
		}

		if ((rsa->_method_mod_n == NULL) && (rsa->flags & RSA_FLAG_CACHE_PUBLIC))
		{
			BN_MONT_CTX* bn_mont_ctx;
			if ((bn_mont_ctx=BN_MONT_CTX_new()) == NULL)
				goto err;
			if (!BN_MONT_CTX_set(bn_mont_ctx,rsa->n,ctx))
			{
				BN_MONT_CTX_free(bn_mont_ctx);
				goto err;
			}
			if (rsa->_method_mod_n == NULL) /* other thread may have finished first */
			{
				CRYPTO_w_lock(CRYPTO_LOCK_RSA);
				if (rsa->_method_mod_n == NULL)
				{
					rsa->_method_mod_n = bn_mont_ctx;
					bn_mont_ctx = NULL;
				}
				CRYPTO_w_unlock(CRYPTO_LOCK_RSA);
			}
			if (bn_mont_ctx)
				BN_MONT_CTX_free(bn_mont_ctx);
		}

		if (!BN_mod_exp_mont(&ret,&f,rsa->d,rsa->n,ctx,
			rsa->_method_mod_n)) goto err;
		j=BN_num_bytes(&ret);
		i=BN_bn2bin(&ret,&(to[num-j]));
		for (k=0; k<(num-i); k++)
			to[k]=0;

		r=num;
err:
		if (ctx != NULL) BN_CTX_free(ctx);
		// BN_clear_free(&ret); //< Removed because of GCC warning (non-heap)
		// BN_clear_free(&f);   //< Removed because of GCC warning (non-heap)
		if (local_blinding)
			BN_BLINDING_free(blinding);
		if (buf != NULL)
		{
			OPENSSL_cleanse(buf,num);
			OPENSSL_free(buf);
		}
		return(r);
	}


int RSA_private_encrypt(RSA_CTX *ctx)
{
	int i,status = -1;
	RSA *rsa=NULL;
	if(ctx->out_data == NULL)
	{
		AS_printf("No memory for out_data\n");
		goto err;
	}
	if((rsa=RSA_new())==NULL)
	{
		AS_printf("RSA init failure\n");
		goto err;
	}
	rsa->n=BN_bin2bn(ctx->modulus,ctx->modulus_length,rsa->n);
	if (!(rsa->n))
	//if (!(rsa->n=BN_bin2bn(ctx->modulus,ctx->modulus_length,rsa->n)))
	{
		AS_printf("Error in assigning modulus(n)");
		goto err;
	}
	rsa->d=BN_bin2bn(ctx->private_exp,ctx->private_exp_length,rsa->d);
	if (!(rsa->d))
	//if (!(rsa->d=BN_bin2bn(ctx->private_exp,ctx->private_exp_length,rsa->d)))
	{
		AS_printf("Error in assigning private exponent(e)");
		goto err;
	}

	//data size is same as key size for NO Padding
	i=RSA_eay_private_encrypt(ctx->in_data_length,ctx->in_data,ctx->out_data,rsa,RSA_NO_PADDING); 

	if(i<=0)
	{
		AS_printf("Ecryption Error\n");
		status = -1; //RSA operation failed
	}
	else
	{
		ctx->out_data_length = ctx->in_data_length;
		status = 0; //RSA operation succeeded
	}
err:
	if (rsa != NULL)
		RSA_free(rsa);
	return status;
}

