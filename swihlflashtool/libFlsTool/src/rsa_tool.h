/* -------------------------------------------------------------------------
 Copyright (C) 2014 Intel Mobile Communications GmbH
 
      Sec Class: Intel Confidential (IC)
 ----------------------------------------------------------------------
 Revision Information:
    $File name:  /msw_tools/FlashTool/libFlsTool/src/rsa_tool.h $
    $CC-Version: .../dhkristx_140314_v0/2 $
    $Date:       2014-04-23    11:04:11 UTC $
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

#ifndef RSA_H
#define RSA_H
#include <string.h>
#include "as_defs.h"
#include "utilfunctions.h"


/******************************************************************************
*     DEFINE SECTION                                                          *
******************************************************************************/

#define RSA_NO_PADDING		3
#define RSA_FLAG_EXT_PKEY		0x20
#define BN_CTX_NUM	32
#define BN_CTX_NUM_POS	12
//#define BN_ULLONG	unsigned _int64

#define BN_ULLONG	uint64_t
#define BN_ULONG	uint32_t
#define BN_LONG		long
#define BN_BITS		64
#define BN_BYTES	4
#define BN_BITS2	32
#define BN_BITS4	16
#define BN_MASK		(0xffffffffffffffffL)
#define BN_MASK2	(0xffffffffL)
#define BN_MASK2l	(0xffff)
#define BN_MASK2h1	(0xffff8000L)
#define BN_MASK2h	(0xffff0000L)
#define BN_TBIT		(0x80000000L)
#define BN_DEC_CONV	(1000000000L)
#define BN_DEC_FMT1	"%lu"
#define BN_DEC_FMT2	"%09lu"
#define BN_DEC_NUM	9

#define BN_FLG_FREE		0x8000	/* used for debuging */
#define BN_FLG_MALLOCED		0x01
#define BN_FLG_STATIC_DATA  	0x02
#define RSA_FLAG_CACHE_PUBLIC		0x02
#define MONT_WORD /* use the faster word-based algorithm */
#define CRYPTO_w_lock(a)
#define CRYPTO_w_unlock(a)
#define CRYPTO_LOCK_RSA			9
#define TABLE_SIZE	32
#define BN_MUL_COMBA
#define BN_SQR_COMBA
#define BN_RECURSION
#define BN_MUL_RECURSIVE_SIZE_NORMAL		(16) /* 32 less than */
#define BN_SQR_RECURSIVE_SIZE_NORMAL		(16) /* 32 */
#define BN_MULL_SIZE_NORMAL			(16) /* 32 */

/******************************************************************************
*     MACRO SECTION                                                           *
******************************************************************************/
#define BN_get_flags(b,n)	((b)->flags&(n))
#define OPENSSL_malloc(num)	CRYPTO_malloc((int)num,__FILE__,__LINE__)
#define OPENSSL_free(addr)	CRYPTO_free(addr)
#define OPENSSL_free_locked(addr) CRYPTO_free_locked(addr)
#define bn_expand(a,bits) ((((((bits+BN_BITS2-1))/BN_BITS2)) <= (a)->dmax)?\
                                      (a):bn_expand2((a),(bits)/BN_BITS2+1))
#define bn_wexpand(a,words) (((words) <= (a)->dmax)?(a):bn_expand2((a),(words)))

#define bn_check_top(a) assert ((a)->top >= 0 && (a)->top <= (a)->dmax);
#define bn_fix_top(a) \
        { \
        BN_ULONG *ftl; \
		if ((a)->top > 0) \
		{ \
		for (ftl= &((a)->d[(a)->top-1]); (a)->top > 0; (a)->top--) \
		if (*(ftl--)) break; \
		} \
	}
#define BN_num_bytes(a)	((BN_num_bits(a)+7)/8)
#define BN_one(a)	(BN_set_word((a),1))
#define BN_zero(a)	(BN_set_word((a),0))
#define BN_abs_is_word(a,w) (((a)->top == 1) && ((a)->d[0] == (BN_ULONG)(w)))
#define BN_is_zero(a)       (((a)->top == 0) || BN_abs_is_word(a,0))
#define BN_to_montgomery(r,a,mont,ctx)	BN_mod_mul_montgomery(\
                                          (r),(a),&((mont)->RR),(mont),(ctx))
#define BN_window_bits_for_exponent_size(b) \
	((b) > 671 ? 6 : \
	(b) > 239 ? 5 : \
	(b) >  79 ? 4 : \
    (b) >  23 ? 3 : 1)

#define Lw(t)    (((BN_ULONG)(t))&BN_MASK2)
#define Hw(t)    (((BN_ULONG)((t)>>BN_BITS2))&BN_MASK2)

#define mul_add(r,a,w,c) { \
	BN_ULLONG t; \
	t=(BN_ULLONG)w * (a) + (r) + (c); \
	(r)= Lw(t); \
	(c)= Hw(t); \
	}
#define mul(r,a,w,c) { \
	BN_ULLONG t; \
	t=(BN_ULLONG)w * (a) + (c); \
	(r)= Lw(t); \
	(c)= Hw(t); \
	}
#define sqr(r0,r1,a) { \
	BN_ULLONG t; \
	t=(BN_ULLONG)(a)*(a); \
	(r0)=Lw(t); \
	(r1)=Hw(t); \
	}

#define mul_add_c(a,b,c0,c1,c2) \
	t=(BN_ULLONG)a*b; \
	t1=(BN_ULONG)Lw(t); \
	t2=(BN_ULONG)Hw(t); \
	c0=(c0+t1)&BN_MASK2; if ((c0) < t1) t2++; \
    c1=(c1+t2)&BN_MASK2; if ((c1) < t2) c2++;
#define mul_add_c2(a,b,c0,c1,c2) \
	t=(BN_ULLONG)a*b; \
	tt=(t+t)&BN_MASK; \
	if (tt < t) c2++; \
	t1=(BN_ULONG)Lw(tt); \
	t2=(BN_ULONG)Hw(tt); \
	c0=(c0+t1)&BN_MASK2;  \
	if ((c0 < t1) && (((++t2)&BN_MASK2) == 0)) c2++; \
           c1=(c1+t2)&BN_MASK2; if ((c1) < t2) c2++;

#define sqr_add_c(a,i,c0,c1,c2) \
	t=(BN_ULLONG)a[i]*a[i]; \
	t1=(BN_ULONG)Lw(t); \
	t2=(BN_ULONG)Hw(t); \
	c0=(c0+t1)&BN_MASK2; if ((c0) < t1) t2++; \
    c1=(c1+t2)&BN_MASK2; if ((c1) < t2) c2++;
#define sqr_add_c2(a,i,j,c0,c1,c2) \
        mul_add_c2((a)[i],(a)[j],c0,c1,c2)

#define BN_mod(rem,m,d,ctx) BN_div(NULL,(rem),(m),(d),(ctx))
#define REMAINDER_IS_ALREADY_CALCULATED
#define BN_is_odd(a)	    (((a)->top > 0) && ((a)->d[0] & 1))
#define BN_TBIT		(0x80000000L)
#define BN_is_one(a)        (BN_abs_is_word((a),1) && !(a)->neg)
#define BN_is_word(a,w)     ((w) ? BN_abs_is_word((a),(w)) && !(a)->neg : \
                                                           BN_is_zero((a)))


/******************************************************************************
*     DECLARATION SECTION                                                     *
******************************************************************************/

typedef struct rsa_st RSA;
typedef struct bignum_ctx BN_CTX;
typedef struct engine_st ENGINE;
typedef struct bignum_st
{
	BN_ULONG *d;	/* Pointer to an array of 'BN_BITS2' bit chunks. */
	int top;	/* Index of last used d +1. */
	/* The next are internal book keeping for bn_expand. */
	int dmax;	/* Size of the d array. */
	int neg;	/* one if the number is negative */
	int flags;
} BIGNUM;
struct bignum_ctx
{
	int tos;
	BIGNUM bn[BN_CTX_NUM];
	int flags;
	int depth;
	int pos[BN_CTX_NUM_POS];
	int too_many;
} /* BN_CTX */;
typedef struct bn_blinding_st
{
	int init;
	BIGNUM *A;
	BIGNUM *Ai;
	BIGNUM *mod; /* just a reference */
	uint32_t thread_id; /* added in OpenSSL 0.9.6j and 0.9.7b;
				 * used only by crypto/rsa/rsa_eay.c, rsa_lib.c */
} BN_BLINDING;

/* Used for montgomery multiplication */
typedef struct bn_mont_ctx_st
{
	int ri;        /* number of bits in R */
	BIGNUM RR;     /* used to convert to montgomery form */
	BIGNUM N;      /* The modulus */
	BIGNUM Ni;     /* R*(1/R mod N) - N*Ni = 1
	     		   * (Ni is only stored for bignum algorithm) */
	BN_ULONG n0;   /* least significant word of Ni */
	int flags;
} BN_MONT_CTX;

typedef struct stack_st
{
	int num;
	char **data;
	int sorted;

	int num_alloc;
	int (*comp)(const char * const *, const char * const *);
} STACK;
typedef struct crypto_ex_data_st
{
	STACK *sk;
	int dummy; /* gcc is screwing up this data structure :-( */
} CRYPTO_EX_DATA;
typedef struct rsa_meth_st
{
	const char *name;
	int (*rsa_pub_enc)(int flen,const unsigned char *from,
	unsigned char *to,
	RSA *rsa,int padding);
	int (*rsa_pub_dec)(int flen,const unsigned char *from,
	unsigned char *to,
	RSA *rsa,int padding);
	int (*rsa_priv_enc)(int flen,const unsigned char *from,
	unsigned char *to,
	RSA *rsa,int padding);
	int (*rsa_priv_dec)(int flen,const unsigned char *from,
	unsigned char *to,
	RSA *rsa,int padding);
	int (*rsa_mod_exp)(BIGNUM *r0,const BIGNUM *I,RSA *rsa); /* Can be null */
	int (*bn_mod_exp)(BIGNUM *r, const BIGNUM *a, const BIGNUM *p,
	const BIGNUM *m, BN_CTX *ctx,
	BN_MONT_CTX *m_ctx); /* Can be null */
	int (*init)(RSA *rsa);		/* called at new */
	int (*finish)(RSA *rsa);	/* called at free */
	int flags;			/* RSA_METHOD_FLAG_* things */
	char *app_data;			/* may be needed! */
							/* New sign and verify functions: some libraries don't allow arbitrary data
							* to be signed/verified: this allows them to be used. Note: for this to work
							* the RSA_public_decrypt() and RSA_private_encrypt() should *NOT* be used
							* RSA_sign(), RSA_verify() should be used instead. Note: for backwards
							* compatibility this functionality is only enabled if the RSA_FLAG_SIGN_VER
							* option is set in 'flags'.
	                        */
	int (*rsa_sign)(int type,
		const unsigned char *m, unsigned int m_length,
		unsigned char *sigret, unsigned int *siglen, const RSA *rsa);
	int (*rsa_verify)(int dtype,
		const unsigned char *m, unsigned int m_length,
		unsigned char *sigbuf, unsigned int siglen, const RSA *rsa);

} RSA_METHOD;
struct rsa_st
{
/* The first parameter is used to pickup errors where
	* this is passed instead of aEVP_PKEY, it is set to 0 */
	int pad;
	long version;
	const RSA_METHOD *meth;
	/* functional reference if 'meth' is ENGINE-provided */
	ENGINE *engine;
	BIGNUM *n;
	BIGNUM *e;
	BIGNUM *d;
	BIGNUM *p;
	BIGNUM *q;
	BIGNUM *dmp1;
	BIGNUM *dmq1;
	BIGNUM *iqmp;
	/* be careful using this if the RSA structure is shared */
	CRYPTO_EX_DATA ex_data;
	int references;
	int flags;

	/* Used to cache montgomery values */
	BN_MONT_CTX *_method_mod_n;
	BN_MONT_CTX *_method_mod_p;
	BN_MONT_CTX *_method_mod_q;

	/* all BIGNUM values are actually in the following data, if it is not
	* NULL */
	char *bignum_data;
	BN_BLINDING *blinding;
} ;

typedef struct RSA_CTX_st
{
	unsigned char *modulus;
	uint16_t modulus_length;
	unsigned char *private_exp;
	uint16_t private_exp_length;
	unsigned char *in_data;
	uint16_t in_data_length;
	unsigned char *out_data;
	uint16_t out_data_length;

} RSA_CTX;

/******************************************************************************
*     PROTOTYPE SECTION                                                       *
******************************************************************************/

RSA *RSA_new(void);
void RSA_free(RSA *r);
void bn_mul_comba8(BN_ULONG *r, BN_ULONG *a, BN_ULONG *b);
void bn_mul_comba4(BN_ULONG *r, BN_ULONG *a, BN_ULONG *b);
void bn_mul_normal(BN_ULONG *r, BN_ULONG *a, int na, BN_ULONG *b, int nb);
void bn_sqr_comba8(BN_ULONG *r, BN_ULONG *a);
void bn_sqr_comba4(BN_ULONG *r, BN_ULONG *a);
int BN_div(BIGNUM *dv, BIGNUM *rm, const BIGNUM *num, const BIGNUM *divisor,
		   BN_CTX *ctx);
void BN_CTX_end(BN_CTX *ctx);
BIGNUM *BN_copy(BIGNUM *a, const BIGNUM *b);
BIGNUM *BN_bin2bn(const unsigned char *s, int len, BIGNUM *ret);
int BN_set_word(BIGNUM *a, BN_ULONG w);
int BN_sub_word(BIGNUM *a, BN_ULONG w);
int BN_nnmod(BIGNUM *r, const BIGNUM *m, const BIGNUM *d, BN_CTX *ctx);
int BN_is_bit_set(const BIGNUM *a, int n);
int BN_mul_word(BIGNUM *a, BN_ULONG w);
int BN_add(BIGNUM *r, const BIGNUM *a, const BIGNUM *b);
int BN_lshift1(BIGNUM *r, const BIGNUM *a);
int BN_sub(BIGNUM *r, const BIGNUM *a, const BIGNUM *b);
int BN_usub(BIGNUM *r, const BIGNUM *a, const BIGNUM *b);
int BN_rshift(BIGNUM *r, const BIGNUM *a, int n);
int BN_uadd(BIGNUM *r, const BIGNUM *a, const BIGNUM *b);
int BN_rshift1(BIGNUM *r, const BIGNUM *a);
int RSA_eay_private_encrypt(int flen, const unsigned char *from,
							unsigned char *to, RSA *rsa, int padding);
int RSA_private_encrypt(RSA_CTX *ctx);

#endif // RSA_H ///:~

