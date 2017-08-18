// BEGIN CCCT SECTION
/* -------------------------------------------------------------------------
 Copyright (C)  Intel Mobile Communications GmbH
 
      Sec Class: Intel Confidential (IC)
 ----------------------------------------------------------------------
 Revision Information:
    $File name:  /msw_tools/FlashTool/libFlsTool/src/errorcodes.h $
    $CC-Version: .../dhkristx_140314_v0/1 $
    $Date:       2014-04-16    12:19:11 UTC $
 ----------------------------------------------------------------------
 by CCCT (0.12l)
 ---------------------------------------------------------------------- */
// END CCCT SECTION

#ifndef __ERRORCODES_H__
#define __ERRORCODES_H__

typedef enum
{
  AS_SUCCESS,
  AS_FAILURE,
  PKCS_BAD_DATA_LEN,
  PKCS_BAD_HDR,
  PKCS_BAD_BLK_TYPE,
  PKCS_BAD_HDR_LEN,
  SHA1_HASHING_ERROR,
  RSA_ENCRYPTION_ERROR,
  RSA_DECRYPTION_ERROR,
  RSA_BAD_KEYMARKER,
  RSA_BAD_PRIVATE_SIGNATURE,
  MEM_ALLOC_FAILED,
  NUM_ERROR_CODES // This should be the last
} as_ret_type;

#endif
