/* -------------------------------------------------------------------------
   Copyright (C) 2013-2014 Intel Mobile Communications GmbH

        Sec Class: Intel Confidential (IC)

   All rights reserved.
   -------------------------------------------------------------------------
   Revision Information:
      $File name:  /msw_tools/FlashTool/libDownloadTool/src/OS_dependent_code/security_if/sign_layer1_tool.c $
      $CC-Version: .../oint_tools_flashtoole2_01/4 $
      $Date:       2014-03-13    15:41:59 UTC $
   ------------------------------------------------------------------------- */
#define THIS_MODULE MODULE_SEC  //< Runtime verbosity evaluation

/****************************************************************************/
/* INCLUDES                                                                 */
/****************************************************************************/
#include "IFWD_sec_channel.h"
#include "DebugModule.h"

/*==========================================================================*/
/*===========================                    ===========================*/
/*======================       HELPER FUNCTIONS       ======================*/
/*===========================                    ===========================*/
/*==========================================================================*/

// Writes out function in log level 3.
// Newlines will cause prefix to be inserted.
void GLOB_log_information(T_LOG_CONTEXT *log, uint8_t loglvl, char *prefix, char* format, ...)
{
  char buffer[2048];
  char *curr=0,*next=0,orig=0;
  va_list ap;

  if(NULL == log)
  {
    return;
  }

  if(!strlen(format))
  {
    return;
  }
  va_start(ap, format);
  vsprintf(buffer,format, ap);

  curr = next = &buffer[0];
  do
  {
    // Try finding first newline char (or null terminator).
    while(*next != '\0' && *next != '\n')
    {
      next++;
    }

    // Insert null terminator char.
    orig = *next;
    *next = '\0';

    // ----- WRITE OUT DATA --------------------------------------------------

    switch(loglvl)
    {
      case 2: // Mark with error - log in low and high.
        /* fall-through */
      case 1: // Normal - log in low and high.
        if(log->log_highlevel)
        {
          Debug("%s%s%s\n",prefix,curr,(2==loglvl)?" ### ERROR ###":"");
        }
        /* fall-through */
      case 0: // Lowlevel - log only in low level.
        if(log->log_lowlevel)
        {
          Debug("%s%s%s\n",prefix,curr,(2==loglvl)?" ### ERROR ###":"");
        }
        /* fall-through */
      default:
        break;
    }

    // ----- END WRITE OUT ---------------------------------------------------

    // If line ended with newline, there is a next string.
    if(orig == '\n')
    {
      next++;
    }

    // Move "curr" to next newline.
    curr = next;
  }
  while(*next != '\0');
}


uint8_t SIGN_sm_error_check(T_SIGN_CONTEXT *sign, T_SM_RESULT sm_result, char *func_str,char *sub_str)
{
  // Check in the sm_result.
  if(SM_RES_SUCCESS != sm_result)
  {
    GLOB_log_information(sign->log,2,"","- %s(%s) returned %08lX.",func_str,sub_str?sub_str:"",sm_result);
    return false;
  }
  return true;
}

/*==========================================================================*/
/*===========================                    ===========================*/
/*======================    INIT,CREATE/DELETE CTX    ======================*/
/*===========================                    ===========================*/
/*==========================================================================*/
uint8_t SIGN_create_context(T_SIGN_CONTEXT *sign, T_HW_DETAILS *hw_details, T_PRIVATE_KEY *private_key)
{
  uint8_t result = true;
  T_SM_RESULT sm_result;

  if(NULL == sign)
  {
    return false;
  }

  GLOB_log_information(sign->log,1,"","SIGN_setup_context");

  // Validate input parameters.
  if(NULL == hw_details || NULL == private_key)
  {
    GLOB_log_information(sign->log,2,"","- Invalid input parameters.");
    return false;
  }

  /************* Create the context *****************************************/

  // Create a signing module context.
  sm_result = SM_ContextCreate(&sign->sm_context);
  result &= SIGN_sm_error_check(sign,sm_result,"SM_ContextCreate",0);

  // Add HW details to context.
  sm_result = SM_ElementSet(sign->sm_context,SM_ELM_HW_UNIQUE_ID,sizeof(hw_details->hwid_baseband)+sizeof(hw_details->hwid_flashchip),hw_details);
  result &= SIGN_sm_error_check(sign,sm_result,"SM_ElementSet","SM_ELM_HW_UNIQUE_ID");

  // Add private key to context.
  sm_result = SM_ElementSet(sign->sm_context,SM_ELM_PRIVATE_KEY,sizeof(T_PRIVATE_KEY),private_key);
  result &= SIGN_sm_error_check(sign,sm_result,"SM_ElementSet","SM_ELM_PRIVATE_KEY");

  return result;
}

uint8_t SIGN_delete_context(T_SIGN_CONTEXT *sign)
{
  uint8_t result = true;
  T_SM_RESULT sm_result;

  if(NULL == sign)
  {
    return false;
  }

  GLOB_log_information(sign->log,1,"","SIGN_delete_context");

  /************* Delete the context *****************************************/

  sm_result = SM_ContextDelete(sign->sm_context);
  result &= SIGN_sm_error_check(sign,sm_result,"SM_ContextCreate",0);

  return result;
}

/*==========================================================================*/
/*===========================                    ===========================*/
/*======================       MAKE KEY CERT DATA     ======================*/
/*===========================                    ===========================*/
/*==========================================================================*/
uint32_t SIGN_make_key_certificate(T_SIGN_CONTEXT *sign, uint8_t bind_to_hwid,
        T_PRIVATE_KEY *cprk, uint32_t cprk_size,
        T_PUBLIC_KEY  *cpuk, uint32_t cpuk_size,
        uint32_t *keyswap_list, uint32_t keyswap_list_size,
        uint8_t *out_data, uint32_t *out_size, uint32_t max_size)
{
  uint8_t result = true;
  uint32_t block_type=0;
  T_SM_RESULT sm_result;
  uint8_t cfg_emsa_pkcs = true;
  uint8_t cfg_hash_sha256 = SHA256; // should be an in put parameter

  if(NULL == sign)
  {
    return false;
  }

  GLOB_log_information(sign->log,1,"","SIGN_make_key_certificate");

  // Input data validation.
  if(NULL == cpuk || 0 == cpuk_size)
  {
    GLOB_log_information(sign->log,2,"","- Invalid input parameters.");
    return false;
  }

  // Set block type.
  block_type = SM_BLK_KEY_CERT_DATA;

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  //~~~~~~~~~~~~~~  Add data and config elements
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Add CPRK.
  if(cprk)
  {
    sm_result=SM_ElementSet(sign->sm_context,SM_ELM_KEYCERT_CPRK,cprk_size,cprk);
    result &= SIGN_sm_error_check(sign,sm_result,"SM_ElementSet","SM_ELM_KEYCERT_CPRK");
  }

  // Add CPUK.
  if(cpuk)
  {
    sm_result=SM_ElementSet(sign->sm_context,SM_ELM_KEYCERT_CPUK,cpuk_size,cpuk);
    result &= SIGN_sm_error_check(sign,sm_result,"SM_ElementSet","SM_ELM_KEYCERT_CPUK");
  }

  // Configure option to use HW details.
  if(bind_to_hwid)
  {
    sm_result=SM_ConfigSet(sign->sm_context,SM_CFG_BIND_TO_HWID,sizeof(bind_to_hwid),&bind_to_hwid);
    result &= SIGN_sm_error_check(sign,sm_result,"SM_ConfigSet","SM_CFG_BIND_TO_HWID");
  }

  // Add keyswap list.
  if(keyswap_list)
  {
    sm_result=SM_ElementSet(sign->sm_context,SM_ELM_KEYCERT_LIST,keyswap_list_size,keyswap_list);
    result &= SIGN_sm_error_check(sign,sm_result,"SM_ElementSet","SM_ELM_KEYCERT_LIST");
  }

  // Use RSA PKCS#1 encryption standard
  {
    sm_result=SM_ConfigSet(sign->sm_context,SM_CFG_EMSA_PKCS_SIGN,sizeof(cfg_emsa_pkcs),&cfg_emsa_pkcs);
    result &= SIGN_sm_error_check(sign,sm_result,"SM_ConfigSet","SM_CFG_EMSA_PKCS_SIGN");
  }

  // Use SHA-256 hashing
  {
    sm_result=SM_ConfigSet(sign->sm_context,SM_CFG_HASH,sizeof(cfg_hash_sha256),&cfg_hash_sha256);
    result &= SIGN_sm_error_check(sign,sm_result,"SM_ConfigSet","SM_CFG_HASH");
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  //~~~~~~~~~~~~~~  Verify and generate data
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  // Read out the block size.
  sm_result = SM_BlockSize(sign->sm_context,block_type,out_size);
  result &= SIGN_sm_error_check(sign,sm_result,"SM_BlockSize",0);

  // Only generate block if output buffer is large enough.
  if(*out_size <= max_size)
  {
    // Generate data block.
    sm_result = SM_BlockMake(sign->sm_context,block_type,out_data);
    result &= SIGN_sm_error_check(sign,sm_result,"SM_BlockMake",0);
  }
  else
  {
    GLOB_log_information(sign->log,2,"","- Output buffer to small (%d) to contain data from SM (%d).",max_size,*out_size);
  }

  /************* Cleanup the context ****************************************/

  // Cleanup context.
  sm_result = SM_ElementDel(sign->sm_context,SM_ELM_KEYCERT_CPRK);
  result &= SIGN_sm_error_check(sign,sm_result,"SM_ElementDel","SM_ELM_KEYCERT_CPRK");
  sm_result = SM_ElementDel(sign->sm_context,SM_ELM_KEYCERT_CPUK);
  result &= SIGN_sm_error_check(sign,sm_result,"SM_ElementDel","SM_ELM_KEYCERT_CPUK");
  sm_result = SM_ConfigDel(sign->sm_context,SM_CFG_BIND_TO_HWID);
  result &= SIGN_sm_error_check(sign,sm_result,"SM_ElementDel","SM_CFG_BIND_TO_HWID");
  sm_result = SM_ElementDel(sign->sm_context,SM_ELM_KEYCERT_LIST);
  result &= SIGN_sm_error_check(sign,sm_result,"SM_ElementDel","SM_ELM_KEYCERT_LIST");
  sm_result = SM_ConfigDel(sign->sm_context,SM_CFG_EMSA_PKCS_SIGN);
  result &= SIGN_sm_error_check(sign,sm_result,"SM_ElementDel","SM_CFG_EMSA_PKCS_SIGN");
  sm_result = SM_ConfigDel(sign->sm_context,SM_CFG_HASH);
  result &= SIGN_sm_error_check(sign,sm_result,"SM_ElementDel","SM_CFG_HASH");

  // Return block id if successful - else 0.
  return result?block_type:0;
}

/*==========================================================================*/
/*===========================                    ===========================*/
/*======================    MAKE GENERIC SIGNATURE    ======================*/
/*===========================                    ===========================*/
/*==========================================================================*/
uint32_t SIGN_make_generic_signature(T_SIGN_CONTEXT *sign, uint8_t pkcs_block_type, uint8_t emsa_pkcs_type, uint8_t hash_type,
                                        uint8_t *in_data, uint32_t in_size, uint8_t *out_data, uint32_t *out_size, uint32_t max_size)
{
  uint8_t result = true;
  uint32_t block_type=0;
  T_SM_RESULT sm_result;

  if(NULL == sign)
  {
    return false;
  }

  GLOB_log_information(sign->log,1,"","SIGN_make_generic_signature");

  // Input data validation.
  if(NULL == in_data || 0 == in_size || NULL == out_data)
  {
    GLOB_log_information(sign->log,2,"","- Invalid input parameters.");
    return false;
  }

  // Set block type.
  block_type = SM_BLK_SIGNATURE;

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  //~~~~~~~~~~~~~~  Add data and config elements
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  // Add input data
  sm_result = SM_ElementSet(sign->sm_context,SM_ELM_SIGNING_INP_DATA,in_size,in_data);
  result &= SIGN_sm_error_check(sign,sm_result,"SM_ElementSet","SM_ELM_SIGNING_INP_DATA");

  // Configure for DER formatted signature
  sm_result = SM_ConfigSet(sign->sm_context,SM_CFG_EMSA_PKCS_SIGN,1,&emsa_pkcs_type);
  result &= SIGN_sm_error_check(sign,sm_result,"SM_ConfigSet","SM_CFG_EMSA_PKCS_SIGN");

  // Configure for using SHA-256 hash
  sm_result = SM_ConfigSet(sign->sm_context,SM_CFG_HASH,1,&hash_type);
  result &= SIGN_sm_error_check(sign,sm_result,"SM_ConfigSet","SM_CFG_HASH");

  // Add PKCS block type.
  sm_result = SM_ElementSet(sign->sm_context,SM_ELM_SIGNING_BLK_TYPE,1,&pkcs_block_type);
  result &= SIGN_sm_error_check(sign,sm_result,"SM_ElementSet","SM_ELM_SIGNING_BLK_TYPE");

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  //~~~~~~~~~~~~~~  Verify and generate data
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  // Read out the block size.
  sm_result = SM_BlockSize(sign->sm_context,block_type,out_size);
  result &= SIGN_sm_error_check(sign,sm_result,"SM_BlockSize",0);

  // Only generate block if output buffer is large enough.
  if(*out_size <= max_size)
  {
    // Generate data block.
    sm_result = SM_BlockMake(sign->sm_context,block_type,out_data);
    result &= SIGN_sm_error_check(sign,sm_result,"SM_BlockMake",0);
  }
  else
  {
    GLOB_log_information(sign->log,2,"","- Output buffer to small (%d) to contain data from SM (%d).",max_size,*out_size);
  }

  /************* Cleanup the context ****************************************/

  // Cleanup context.
  sm_result = SM_ElementDel(sign->sm_context,SM_ELM_KEYCERT_CPRK);
  result &= SIGN_sm_error_check(sign,sm_result,"SM_ElementDel","SM_ELM_KEYCERT_CPRK");
  sm_result = SM_ElementDel(sign->sm_context,SM_ELM_KEYCERT_CPUK);
  result &= SIGN_sm_error_check(sign,sm_result,"SM_ElementDel","SM_ELM_KEYCERT_CPUK");
  sm_result = SM_ConfigDel(sign->sm_context,SM_CFG_BIND_TO_HWID);
  result &= SIGN_sm_error_check(sign,sm_result,"SM_ElementDel","SM_CFG_BIND_TO_HWID");
  sm_result = SM_ElementDel(sign->sm_context,SM_ELM_KEYCERT_LIST);
  result &= SIGN_sm_error_check(sign,sm_result,"SM_ElementDel","SM_ELM_KEYCERT_LIST");

  // Return block id if successful - else 0.
  return result?block_type:0;
}

//---------------------------------------------------------------------------

