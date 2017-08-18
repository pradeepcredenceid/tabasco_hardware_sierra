// BEGIN CCCT SECTION
/* -------------------------------------------------------------------------
 Copyright (C)  Intel Mobile Communications GmbH
 
      Sec Class: Intel Confidential (IC)
 ----------------------------------------------------------------------
 Revision Information:
    $File name:  /msw_tools/FlashTool/libFlsTool/src/KeyFormat.h $
    $CC-Version: .../dhkristx_140314_v0/1 $
    $Date:       2014-04-16    12:18:37 UTC $
 ----------------------------------------------------------------------
 by CCCT (0.12l)
 ---------------------------------------------------------------------- */
// END CCCT SECTION

#ifndef KEYFORMAT
#define KEYFORMAT

#include <stdint.h>

#define MAX_MODULUS_LEN (2048/8)

#define KEY_MARKER 0xAABBCCDD

#define PROD_KEY     0
#define DEV_KEY      1
#define GOLD_KEY     2

typedef struct{
        uint32_t KeyLength;/* length of modulus */
        uint8_t Exponent[MAX_MODULUS_LEN];
        uint8_t Modulus[MAX_MODULUS_LEN];
}BasicKeyStructType;

typedef struct {
        uint32_t PublicSignature[5]; /* SHA-1 of pub key struct*/
        uint32_t KeyType;
        uint32_t KeyMarker;/* magic number for quick detection of possible key presence*/
        BasicKeyStructType Key;
}PublicKeyStructType;

typedef struct {
        uint32_t PrivateSignature[5];/* SHA-1 of priv key struct*/
        uint32_t KeyType;
        uint32_t KeyMarker;/*magic number for quick detection of possible key presence*/
        BasicKeyStructType Key;
        uint32_t PublicSignature[5]; /* SHA-1 of pub key struct*/
}PrivateKeyStructType;

//New Public Key structure:
typedef struct{
   uint32_t KeyType;
   uint32_t KeyLength;
   uint32_t Exponent;
   uint8_t Modulus[256];
   uint8_t Montgom[256];
}NewPublicKeyStructType;


#endif
