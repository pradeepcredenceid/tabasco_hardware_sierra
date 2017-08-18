/*   ----------------------------------------------------------------------
     Copyright (C) 2014 Intel Mobile Communications GmbH

          Sec Class: Intel Confidential (IC)

     Copyright (C) 2009 - 2010 Infineon Technologies Denmark A/S (IFWD)
     ----------------------------------------------------------------------
     Revision Information:
       $$File name:  /msw_tools/FlashTool/libFlsTool/inc/ebl2_sec_errors.h $
       $$CC-Version: .../oint_tools_flashtoole2_01/1 $
       $$Date:       2014-03-13    15:39:31 UTC $
     ----------------------------------------------------------------------
     by CCCT (v0.4)
     ---------------------------------------------------------------------- */
#ifndef __EBL2_SEC_ERROR_H__
#define __EBL2_SEC_ERROR_H__

#ifdef FLASHTOOL_E2
  #define ERROR_DEF(eNUM,tEXT) tEXT,
  #define EblSecReturnCodesType
#else
  #define ERROR_DEF(eNUM,tEXT) eNUM,
  #define EblSecReturnCodesType EblSecReturnCodes
#endif


#ifdef FLASHTOOL_E2
  const char* const ebl2_sec_errors[] =
#else
  typedef enum
#endif
{
   ERROR_DEF(EBL_SEC_OK,"No error.")
   ERROR_DEF(EBL_SEC_FAIL,"Security failure.")
   ERROR_DEF(EBL_SEC_INVALID_TKT,"Invalid ticket.")
   ERROR_DEF(EBL_SEC_READ_TICKET_FAILED,"Ticket read failure.")
   ERROR_DEF(EBL_SEC_SHA1_HASHING_ERROR,"SHA1 hashing error.")
   ERROR_DEF(EBL_SEC_INVALID_HASH,"Invalid hash")
   ERROR_DEF(EBL_SEC_DOWNLOAD_TYPE_MISMATCH,"Download element type mishmatch.")
   ERROR_DEF(EBL_SEC_EXCEPTION_VER_CHECK_FAIL,"Exception log version check failed.")
   ERROR_DEF(EBL_SEC_INSTALLED_VER_CHECK_FAIL,"Installed version check failed.")
   ERROR_DEF(EBL_SEC_DOWNLOAD_TYPE_NOT_CODE,"Download element type not code as expected.")
   ERROR_DEF(EBL_SEC_FLASH_ERASE_NOT_IDLE,"Storage was not idle when starting erase.")
   ERROR_DEF(EBL_SEC_LOAD_MAP_NOT_USED,"Load map not used.")
   ERROR_DEF(EBL_SEC_LOAD_MAP_ADDR_ERROR,"Load map area address error.")
   ERROR_DEF(EBL_SEC_LOAD_MAP_LENGTH_ERROR,"Load map area length error.")
   ERROR_DEF(EBL_SEC_ERASE_ERROR,"Erase error")
   ERROR_DEF(EBL_SEC_INVALID_WRITE_ADDR,"Invalid write address.")
   ERROR_DEF(EBL_SEC_READ_ADDR_NOT_IN_RANGE,"Read address not in range.")
   ERROR_DEF(EBL_SEC_EXCEPTION_LOG_ERASE_FAILED,"Exception log erase failure.")
   ERROR_DEF(EBL_SEC_EXCEPTION_LOG_WRITE_FAILED,"Exception log write failure.")
   ERROR_DEF(EBL_SEC_ERASE_LOAD_MAP_OUT_OF_RANGE,"Erase load map out of range.")
   ERROR_DEF(EBL_SEC_WRITE_LOAD_MAP_OUT_OF_RANGE,"Write load map out of range.")
   ERROR_DEF(EBL_SEC_BAD_RSA_LEN,"Bad RSA length.")
   ERROR_DEF(EBL_SEC_BAD_RSA_BLOCK,"Bad RSA block.")
   ERROR_DEF(EBL_SEC_BAD_RSA_DATA,"Bad RSA data.")
   ERROR_DEF(EBL_SEC_RSA_DECRYPT_FAILED,"RSA decryption failed.")
   ERROR_DEF(EBL_SEC_ERASE_OUT_OF_BLOCK,"Erase out of block.")
   ERROR_DEF(EBL_SEC_GET_BLOCK_BOUNDARIES_FAILED,"Get block boundaries failed.")
   ERROR_DEF(EBL_SEC_VERION_MISMATCH,"Version mismatch.")
   ERROR_DEF(EBL_SEC_INVALID_DATA_LENGTH,"Invalid data length.")
   ERROR_DEF(EBL_SEC_WRITE_FAILED,"Write failed.")
   ERROR_DEF(EBL_SEC_READ_FAILED,"Read failed.")
   ERROR_DEF(EBL_SEC_INVALID_EBL_VERSION,"Invalid EBL version.")
   ERROR_DEF(EBL_SEC_INVALID_SECPACK,"Invalid sec-pack.")
   ERROR_DEF(EBL_SEC_INVALID_EPOCH,"HW EPOCH not valid.")
   ERROR_DEF(EBL_SEC_INVALID_INPUT,"Invalid input used in function.")
   ERROR_DEF(EBL_SEC_DECODING_FAILED,"Unable to decode security content.")
   ERROR_DEF(EBL_SEC_UPLOAD_NOT_ALLOWED,"Upload is not allowed.")
   ERROR_DEF(EBL_SEC_IN_SESSION,"Only allowed when not in session.")
   ERROR_DEF(EBL_SEC_ERASE_ALL_FLASH_NOT_ALLOWED,"Erase all flash is not allowed.")
   ERROR_DEF(EBL_SEC_MAX_SEC_PACKS_IN_AREA,"There is not room for all the sec packs in the reserved area.")
   ERROR_DEF(EBL_SEC_NUM_ERRORS, "Dummy")
} EblSecReturnCodesType;

#endif
