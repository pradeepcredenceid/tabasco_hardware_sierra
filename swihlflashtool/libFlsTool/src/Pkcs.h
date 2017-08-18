// BEGIN CCCT SECTION
/* -------------------------------------------------------------------------
 Copyright (C)  Intel Mobile Communications GmbH
 
      Sec Class: Intel Confidential (IC)
 ----------------------------------------------------------------------
 Revision Information:
    $File name:  /msw_tools/FlashTool/libFlsTool/src/Pkcs.h $
    $CC-Version: .../dhkristx_140314_v0/1 $
    $Date:       2014-04-16    12:18:53 UTC $
 ----------------------------------------------------------------------
 by CCCT (0.12l)
 ---------------------------------------------------------------------- */
// END CCCT SECTION

#ifndef __PKCS__
#define __PKCS__

#include <stdint.h>
#include "KeyFormat.h"

#define SHA_DIGEST_LONGS      5
#define SHA2_DIGEST_LONGS      8
#define CHIP_ID_DATA_LENGTH   16

// block types for decrypting with the master key:
#define SECURE_BLOCK_TYPE  1
#define AREA_BLOCK_TYPE    2
#define IMAGE_BLOCK_TYPE   3

// block types for decrypting with the SEC key:
#define TICKET_BLOCK_TYPE  0x01
#define IMEI_BLOCK_TYPE    0x02
#define SIMLOCK_BLOCK_TYPE 0x03

//Exception log check return codes
#define EXCEPTION_LOG_NOT_INVALID 0
#define SEC_TYPE_NOT_LOG_TYPE 1
#define SEC_TYPE_EQUALS_LOG_TYPE 2
#define EXCEPTION_LOG_FFS_STA_TYPE 3
#define EXCEPTION_LOG_FFS_DYN_TYPE 4

// preparation for sha2:
typedef union {
        uint32_t Sha2Signature[SHA2_DIGEST_LONGS];
        uint32_t Sha1Signature[SHA_DIGEST_LONGS]; /* SHA-1 of FLS data*/
}SignatureStructType;

#if defined(UNIX_BUILD)
#pragma pack(1)  // Forcing following structures packing to be 1 byte aligned
#endif
typedef struct {
        uint32_t StartAddr;
        uint32_t Length;
        uint8_t  DER[15];
        uint8_t Signature[SHA_DIGEST_LONGS*4]; /* SHA-1 of FLS data*/
}PsiSha1AreaSignatureStructType;

typedef struct {
        uint32_t StartAddr;
        uint32_t Length;
        uint8_t  DER[19];
        uint8_t Signature[SHA2_DIGEST_LONGS*4]; /* SHA-2 of FLS data*/
}PsiSha2AreaSignatureStructType;

typedef struct {
        uint8_t  DER[15];
        uint32_t Signature[SHA_DIGEST_LONGS]; /* SHA-1 of FLS data*/
}SecPackSha1SignatureStructType;

typedef struct {
        uint8_t  DER[15];
        uint32_t Signature[SHA2_DIGEST_LONGS]; /* SHA-1 of FLS data*/
}SecPackSha2SignatureStructType;

#if defined(UNIX_BUILD)
#pragma pack() // set the packing to be original
#endif

typedef struct {
        uint32_t StartAddr;
        uint32_t Length;
        uint32_t Signature[SHA_DIGEST_LONGS]; /* SHA-1 of FLS data*/
}AreaSignatureStructType;

typedef struct {
        uint32_t StartAddr;
        uint32_t UsedLength;
}HashRegionStructType;

typedef struct {
        uint32_t HashFunction;
        HashRegionStructType Regions[8];
        SignatureStructType Signature;
}ImageSignatureStructType;


typedef struct {
        uint32_t FlsSignature[SHA_DIGEST_LONGS]; /* SHA-1 of FLS data*/
        uint32_t DataSignature[SHA_DIGEST_LONGS]; /* SHA-1 of FLS data*/
        uint32_t ChipId[SHA_DIGEST_LONGS];
        uint32_t FlsSignature2[SHA_DIGEST_LONGS]; /* SHA-1 of FLS data*/
}DownloadCryptoBlockStructType;

typedef struct {
     uint32_t TicketType;
     uint32_t SigOfHWID[SHA_DIGEST_LONGS]; /* SHA-1 of Hardware ID*/
}TicketStructType;

#ifdef __cplusplus
extern "C" {
#endif

int PKCS_Decrypt_WS (unsigned char *WS,
                  unsigned char *Output,
                  unsigned int *OutputLen,
                  unsigned char *Input,
                  unsigned int InputLen,
                  NewPublicKeyStructType *Key,
                  unsigned char BlockType);


extern int PKCS_Encrypt(unsigned char *Output,
                 unsigned int *OutputLen,
                 unsigned char *Input,
                 unsigned int InputLen,
                 PrivateKeyStructType *Key,
                 unsigned char BlockType,
                 unsigned char UseSalt,
                 unsigned char NoRsa);
#ifdef __cplusplus
}
#endif

#endif
