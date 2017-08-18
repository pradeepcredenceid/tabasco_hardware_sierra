/* -------------------------------------------------------------------------
   Copyright (C) 2013-2014 Intel Mobile Communications GmbH

        Sec Class: Intel Confidential (IC)

   All rights reserved.
   -------------------------------------------------------------------------
   Revision Information:
      $File name:  /msw_tools/FlashTool/libDownloadTool/src/OS_dependent_code/security_if/signing_module.h $
      $CC-Version: .../dhkristx_140314_v0/1 $
      $Date:       2014-04-23    10:54:36 UTC $
   ------------------------------------------------------------------------- */

#if !defined(SM_INTERFACE_H)
#define SM_INTERFACE_H


/*****************************************************************************/
/* INCLUDES                                                                  */
/*****************************************************************************/

#include <stdio.h>
#include <stdint.h>

/*****************************************************************************/
/* DEFINES                                                                   */
/*****************************************************************************/

#if defined(INCLUDE_DEPRECATED_IF)
  #define FLAG_DPIF " (DPIF)"
#else
  #define FLAG_DPIF
#endif

#if defined(DEBUG)
  #define FLAG_DEBUG " (DEBUG)"
#else
  #define FLAG_DEBUG
#endif

/* ========================================= SIGNING MODULE VERSION ======== */
#define SM_VERSION     "6.0.0.7" FLAG_DPIF FLAG_DEBUG
#define SM_WINVERS      6,0,0,7
/* ========================================================================= */


#if defined(WINDOWS) && defined(BUILD_DLL)
  #define DLL_EXPORT __declspec(dllexport)
#else
  #define DLL_EXPORT
#endif

#ifdef PREPEND_US
  #define SM_BlockMake _SM_BlockMake
  #define SM_BlockSize _SM_BlockSize
  #define SM_ConfigDel _SM_ConfigDel
  #define SM_ConfigGet _SM_ConfigGet
  #define SM_ConfigSet _SM_ConfigSet
  #define SM_ContextCreate _SM_ContextCreate
  #define SM_ContextDelete _SM_ContextDelete
  #define SM_ElementDel _SM_ElementDel
  #define SM_ElementGet _SM_ElementGet
  #define SM_ElementSet _SM_ElementSet
  #define SM_GetVersion _SM_GetVersion
  #define SM_FormatOutput _SM_FormatOutput
#endif // PREPEND_US

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t T_SM_RESULT_TYPE;

/*! @addtogroup sm_result_causes ID List: Result Causes
The result causes defines the result values for the signing module.
In case of an error, the result cause will help to determine the nature of the error situation.
@{
*/

// All SM Result Types must be in format 0x8nnnnnnnL
#define SM_RES_SUCCESS                  0x80000000 /*!< \brief No error, successfull operation. */
#define SM_RES_FAILURE                  0x80000001 /*!< \brief Unspecified error. */
#define SM_RES_INTERNAL                 0x80000002 /*!< \brief Internal error. */
#define SM_RES_UNSUPPORTED              0x80000003 /*!< \brief Data input is unsupported. */
#define SM_RES_MALLOC_ERROR             0x80000004 /*!< \brief Memory allocation failed. */
#define SM_RES_INVALID_LENGTH           0x80000005 /*!< \brief Data length is invalid. */
#define SM_RES_INVALID_FORMAT           0x80000006 /*!< \brief Found corrupt or malformated data. */
#define SM_RES_INVALID_INPUT            0x80000007 /*!< \brief Input parameter is incorrect. */
#define SM_RES_INVALID_HASH             0x80000008 /*!< \brief Data hash inconsistency. */
#define SM_RES_INVALID_TYPE             0x80000009 /*!< \brief Data hash inconsistancy. */
#define SM_RES_INVALID_BOOL             0x8000000A /*!< \brief Data is not a boolean value (0 or 1). */
#define SM_RES_NOT_FOUND                0x8000000B /*!< \brief Requsted data was not found. */
#define SM_RES_DATA_EXISTS              0x8000000C /*!< \brief Data element exists already. */
#define SM_RES_TLV_FAILURE              0x80000200 /*!< \brief Unknown TLV error occured. */
#define SM_RES_TLV_SETUP_FAILED         0x80000201 /*!< \brief Error setting up TLV iterator on TLV data. */
#define SM_RES_TLV_INVALID_PATH         0x80000202 /*!< \brief The TLV path was not found. */
#define SM_RES_TLV_BAD_OBJ_SIZE         0x80000203 /*!< \brief Unexpected size of object. */

// Crypto algorithm related result codes.
#define SM_RES_CBC_BAD_INPUT_LEN        0x80080001 /*!< \brief CBC bad input length. */
#define SM_RES_RSA_ENCRYPTION_ERROR     0x80080101 /*!< \brief RSA encryption error. */
#define SM_RES_RSA_DECRYPTION_ERROR     0x80080102 /*!< \brief RSA decryption error. */
#define SM_RES_RSA_BAD_KEYMARKER        0x80080103 /*!< \brief RSA bad key marker. */
#define SM_RES_RSA_BAD_KEYLENGTH        0x80080104 /*!< \brief RSA bad key length. */
#define SM_RES_RSA_BAD_PRIV_SIGNATURE   0x80080105 /*!< \brief RSA bad private signature. */
#define SM_RES_PKCS_BAD_DATA_LEN        0x80080201 /*!< \brief PKCS bad data length. */
#define SM_RES_PKCS_BAD_HDR             0x80080202 /*!< \brief PKCS bad header. */
#define SM_RES_PKCS_BAD_BLK_TYPE        0x80080203 /*!< \brief PKCS bad block type. */
#define SM_RES_PKCS_BAD_HDR_LEN         0x80080204 /*!< \brief PKCS bad header length. */
#define SM_RES_SHA1_HASHING_ERROR       0x80080301 /*!< \brief SHA1 hashing error. */
/* @} */

/*! @addtogroup sm_block_types ID List: Data Block Types
The data block types are the block which the signing module can generate.
@{
*/
// ----- All SM BLock Types must be in format 0x0nnnnnnnL ------------------
#define SM_BLK_IMEI_DATA_STD            0x00010001L /*!< \brief IMEI standard single IMEI format. */
#define SM_BLK_IMEI_DATA_MID            0x00010002L /*!< \brief IMEI in MID certificate format (dual and single IMEI). */
#define SM_BLK_RND_CERT_DATA            0x00010801L /*!< \brief RND certificate. */
#define SM_BLK_TICK_SYSTEM_VALID        0x00020000L /*!< \brief Valid system ticket. */
#define SM_BLK_TICK_SYSTEM_TI           0x00020001L /*!< \brief Test interface ticket definintion. */
#define SM_BLK_TICK_SYSTEM_BC           0x00020002L /*!< \brief Boot core interface ticket definition. */
#define SM_BLK_TICK_SYSTEM_SM           0x00020003L /*!< \brief Sec module interface ticket definition. */
#define SM_BLK_TICK_SIMLOCK_NO          0x00020010L /*!< \brief Simlock NO ticket. */
#define SM_BLK_TICK_SIMLOCK_NS          0x00020011L /*!< \brief Simlock NS ticket. */
#define SM_BLK_TICK_SIMLOCK_SP          0x00020012L /*!< \brief Simlock SP ticket. */
#define SM_BLK_TICK_SIMLOCK_CP          0x00020013L /*!< \brief Simlock CP ticket. */
#define SM_BLK_TICK_SIMLOCK_SM          0x00020014L /*!< \brief Simlock SM ticket. */
#define SM_BLK_SIMLOCK_DATA             0x00030001L /*!< \brief Simlock SLF data.  */
#define SM_BLK_SIGNATURE                0x00080001L /*!< \brief Generic signature. */
#define SM_BLK_KEY_CERT_DATA            0x00080002L /*!< \brief Key Certificate    */
#define SM_BLK_SECURE_CHANNEL_DATA      0x00080003L /*!< \brief Secure channel data*/


/* @} */

/*! @addtogroup sm_config_types ID List: Config Block Types
The config block types are used to control how the data blocks are generated.
@{
*/
// ----- All Config Types must be in format 0x1nnnnnnL ----------------------
#define SM_CFG_HW_CONFIG                0x10000001L /*!< \brief RFU (reserved for future usage). */
#define SM_CFG_TLV_REPLACE_HWID         0x10000002L /*!< \brief Replace HW details in TLV data. */
#define SM_CFG_BIND_TO_HWID             0x10000003L /*!< \brief Replace HW details in TLV data. */
#define SM_CFG_SM_SUPPORT_BLOCKS        0x10001001L /*!< \brief RFU (reserved for future usage). */
#define SM_CFG_HW_SUPPORT_BLOCKS        0x10001002L /*!< \brief RFU (reserved for future usage). */
#define SM_CFG_EMSA_PKCS_SIGN           0x10002001L /*!< \brief Generates standard RSA EMSA-PKCS V1.5 signature. */
#define SM_CFG_HASH                     0x10002002L /*!< \brief Uses the SHA256 hashing algorithm when genrating the signature. */

typedef enum T_SM_CFG_HASH_ENUM{
  MD5=0,
  SHA1,
  SHA256
}T_SM_CFG_HASH_ENUM;

typedef uint8_t T_SM_CFG_HASH;


/* @} */

/*! @addtogroup sm_element_types ID List: Element Types
The element blocks are used to define input data, needed for block data generation.
@{
*/
// ----- All SM Element Types must be in format 0x2nnnnnnnL -----------------

// Element: Common.
#define SM_ELM_HW_UNIQUE_ID             0x20000001L /*!< \brief HW unique id from phone, 16 byte baseband chip id + 16 byte flashchip id. */
#define SM_ELM_PRIVATE_KEY              0x20000002L /*!< \brief Private key to use for data block generation. */

// Element: IMEI & RnD Data.
#define SM_ELM_IMEI_STD                 0x20001001L /*!< \brief Null-terminated 15 digit ASCII represented IMEI number. */
#define SM_ELM_IMEI_MID                 0x20001002L /*!< \brief Single or dual IMEI MID certificate (TLV structure). */
#define SM_ELM_RND_CERT                 0x20001801L /*!< \brief RnD certificate data block (TLV structure). */

// Element: Tickets
#define SM_ELM_TICK_UNLOCK_CODE         0x20002001L /*!< \brief Ticket unlock code, 6-32 bytes in binary or ASCII format. */
#define SM_ELM_TICK_VALID_TYPE          0x20002002L /*!< \brief One byte indicating ticket ID. */
#define SM_ELM_TICK_GOLD_FLAG           0x20002003L /*!< \brief One byte (boolean) indicating if ticket must contain gold flag (needed for system ticket, to allow boot core ticket validation). */
#define SM_ELM_TICK_MINUTES             0x20002004L /*!< \brief One byte indicating number of minutes before a valid ticket is autodeleted (system ticket). Set zero for no auto-deletion. */
#define SM_ELM_TICK_COUNTER             0x20002005L /*!< \brief One byte indicating number of powerons before a valid ticket is autodeleted (system ticket). Set zero for no auto-deletion. */

// Element: Sim Lock Data
#define SM_ELM_SIMLOCK_DATA_STD         0x20003001L /*!< \brief Simlock TLV data definition block, generated using Phone Tool. */
#define SM_ELM_SIGNING_INP_DATA         0x20008001L /*!< \brief Data block one which to generate signature. */
#define SM_ELM_SIGNING_BLK_TYPE         0x20008002L /*!< \brief One byte indicating signature block type. */

// Element: Key Cert Data
#define SM_ELM_KEYCERT_CPRK             0x2000A001L /*!< \brief fff. */
#define SM_ELM_KEYCERT_CPUK             0x2000A002L /*!< \brief fff. */
#define SM_ELM_KEYCERT_LIST             0x2000A004L /*!< \brief fff. */


/* @} */

/*! @addtogroup sm_interface Interface Functions
@{
*/

/*****************************************************************************/
/* TYPE DEFINITIONS                                                          */
/*****************************************************************************/

typedef void* T_SM_CONTEXT; /*!< \brief Reference to the internal context type, used by signing module interface. */
typedef uint32_t T_SM_RESULT; /*!< \brief Type definition for use with @ref sm_result_causes. */

/*****************************************************************************/
/* FUNCTIONS                                                                 */
/*****************************************************************************/

/**
Create a new context.

Function allocates and prepares the internal context structure.
@param context Pointer to context variable pointer, to which newly created context is assigned.
@return Result cause according to @ref sm_result_causes.
*/
extern T_SM_RESULT DLL_EXPORT SM_ContextCreate(T_SM_CONTEXT *context);

/**
Delete a context.

Function deletes all data elements and memory allocated for the context.
@param context Pointer to context variable.
@return Result cause according to @ref sm_result_causes.
*/
extern T_SM_RESULT DLL_EXPORT SM_ContextDelete(T_SM_CONTEXT context);

/**
Set a config element.

Function copies input data into context memory.
@param context Pointer to context variable.
@param cfg_id Unique config id for data according to @ref sm_config_types.
@param length Length of input data.
@param data Pointer to input data.
@return Result cause according to @ref sm_result_causes.
*/
extern T_SM_RESULT DLL_EXPORT SM_ConfigSet(T_SM_CONTEXT context, uint32_t cfg_id, uint32_t length, void* data);

/**
Get a config element.

Function returns length and pointer of existing data element in context.
@param context Pointer to context variable.
@param cfg_id Unique config id for data according to @ref sm_config_types.
@param length Length of output data.
@param data Pointer to output data pointer.
@return Result cause according to @ref sm_result_causes.
*/
extern T_SM_RESULT DLL_EXPORT SM_ConfigGet(T_SM_CONTEXT context, uint32_t cfg_id, uint32_t *length, void **data);

/**
Delete a config element.

Function deletes existing data element in context.
@param context Pointer to context variable.
@param cfg_id Unique config id for data according to @ref sm_config_types.
@return Result cause according to @ref sm_result_causes.
*/
extern T_SM_RESULT DLL_EXPORT SM_ConfigDel(T_SM_CONTEXT context, uint32_t cfg_id);

/**
Set a data element.

Function copies input data into context memory.
@param context Pointer to context variable.
@param elm_id Unique element id for data according to @ref sm_element_types.
@param length Length of input data.
@param data Pointer to input data.
@return Result cause according to @ref sm_result_causes.
*/
extern T_SM_RESULT DLL_EXPORT SM_ElementSet(T_SM_CONTEXT context, uint32_t elm_id, uint32_t length, void* data);

/**
Get a data element.

Function returns length and pointer of existing data element in context.
@param context Pointer to context variable.
@param elm_id Unique element id for data according to @ref sm_element_types.
@param length Length of output data.
@param data Pointer to output data pointer.
@return Result cause according to @ref sm_result_causes.
*/
extern T_SM_RESULT DLL_EXPORT SM_ElementGet(T_SM_CONTEXT context, uint32_t elm_id, uint32_t *length, void **data);

/**
Delete a data element.

Function deletes existing data element in context.
@param context Pointer to context variable.
@param elm_id Unique element id for data according to @ref sm_element_types.
@return Result cause according to @ref sm_result_causes.
*/
extern T_SM_RESULT DLL_EXPORT SM_ElementDel(T_SM_CONTEXT context, uint32_t elm_id);

/**
Query size of data block.

Function analyses the data size requirement for the block generation.
@param context Pointer to context variable.
@param blk_id Unique block id for data to be generated according to @ref sm_block_types.
@param out_size Pointer to 32 bit length variable, in which to store length.
@return Result cause according to @ref sm_result_causes.
*/
extern T_SM_RESULT DLL_EXPORT SM_BlockSize(T_SM_CONTEXT context, uint32_t blk_id, uint32_t *out_size);

/**
Generate data block.

Function generates the data block based on relevant config and element blocks stored in context.
@param context Pointer to context variable.
@param blk_id Unique block id for data to be generated according to @ref sm_block_types.
@param out_data Pointer to output buffer in which to store generated data.
@return Result cause according to @ref sm_result_causes.
*/
extern T_SM_RESULT DLL_EXPORT SM_BlockMake(T_SM_CONTEXT context, uint32_t blk_id, void *out_data);

/**
Set log file.

Set file pointer to be used for debug/info logging in the signing module.
@param context Pointer to context variable.
@param fp Pointer to FILE stream.
@return Result cause according to @ref sm_result_causes.
*/
extern T_SM_RESULT DLL_EXPORT SM_LogFile(T_SM_CONTEXT context, FILE *fp);
/* @} */

/**
Get the signing module version.

Function returns the signing module version as string.
@param version Pointer in which the version string will be stored.
*/

extern void SM_GetVersion(uint8_t *version);
/* @} */


#ifdef __cplusplus
}
#endif

#endif /* SM_INTERFACE_H */
