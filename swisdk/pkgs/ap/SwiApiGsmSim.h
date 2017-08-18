/**
 *    \if CvsId 
 *    $Id: SwiApiGsmSim.h,v 1.10 2010/06/10 00:19:58 epasheva Exp $
 *    \endif
 *    \ingroup gsmsim
 *    \file SwiApiGsmSim.h
 * 
 *    \brief This file contains definitions, enumerations, structures and
 *           forward declarations for SwiApiGsmSim.c file
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */
#ifndef __SWIAPI_GSM_SIM
#define __SWIAPI_GSM_SIM

#ifdef __cplusplus
extern "C" {
#endif

/* include files */
#include "SwiDataTypes.h"
#include "SwiRcodes.h"

/* constants */
#define LENGTH_MaxCHV               9
#define LENGTH_MEPCode              13
#define LENGTH_IccId                10
#define LENGTH_RandNum              16
#define LENGTH_Kc                   8

/* enum declarations */
/**
 *
 * An enumeration of SIM password types.
 *
 * \sa SWI_STRUCT_SIM_Verify
 * \sa SWI_STRUCT_SIM_Password
 *
 */
enum SWI_TYPE_SIM_Password
{
    /*! Unused password enumeration. */
    SWI_SIMPASSWORD_Reserved,
    
    /*! CHV1 (Card Holder Verification 1) is commonly referred to as PIN1.  
        This code cannot match or begin with an emergency number. */
    SWI_SIMPASSWORD_CHV1,

    /*! CHV2 (Card Holder Verification 2) is commonly referred to as PIN2. */
    SWI_SIMPASSWORD_CHV2,
    
    /*! The unblocking code for CHV1 is commonly referred to as PUK1. */
    SWI_SIMPASSWORD_UnblockingCHV1, 
    
    /*! The unblocking code for CHV2 is commonly referred to as PUK2. */
    SWI_SIMPASSWORD_UnblockingCHV2          
};
/**
 *
 * An enumeration of SIM status codes.
 *
 * \sa SWI_STRUCT_SIM_StatusExp
 *
 */
enum SWI_TYPE_SIM_StatusExp
{
    SWI_SIMSTATUSEXP_OK,                /*!< Success. */
    SWI_SIMSTATUSEXP_NotInserted,       /*!< SIM is not inserted. */
    SWI_SIMSTATUSEXP_Removed,           /*!< SIM has been removed. */
    SWI_SIMSTATUSEXP_InitializeFailure, /*!< SIM has failed to initialize*/
    SWI_SIMSTATUSEXP_GeneralFailure,    /*!< General SIM failure. */    
    SWI_SIMSTATUSEXP_Locked,            /*!< SIM is locked. */
    SWI_SIMSTATUSEXP_PIN1Blocked,       /*!< PIN1 is blocked. */
    SWI_SIMSTATUSEXP_PIN2Blocked,       /*!< PIN2 is blocked. */
    SWI_SIMSTATUSEXP_PIN1Rejected,      /*!< PIN1 is rejected. */
    SWI_SIMSTATUSEXP_PIN2Rejected,      /*!< PIN2 is rejected. */
    SWI_SIMSTATUSEXP_MEPLocked,         /*!< MEP check failed. */
    SWI_SIMSTATUSEXP_NetworkReject      /*!< SIM is rejected by the network. */
};
/**
 *
 * An enumeration of required SIM operations.
 *
 * \sa SWI_STRUCT_SIM_StatusExp
 *
 */
enum SWI_TYPE_SIM_RequiredOperation
{
    SWI_REQUIREDOPERATON_NoOperation, /*!< No operation required. */
    SWI_REQUIREDOPERATON_EnterPIN,    /*!< Enter PIN1. */
    SWI_REQUIREDOPERATON_EnterPIN2,   /*!< Enter PIN2. */
    SWI_REQUIREDOPERATON_EnterPUK,    /*!< Enter PIN1 unblocking code. */
    SWI_REQUIREDOPERATON_EnterPUK2,   /*!< Enter PIN2 unblocking code. */
    SWI_REQUIREDOPERATON_EnterMEP     /*!< Enter MEP unlocking code. */
};
/**
 *
 * An enumeration of previous SIM operations.
 *
 * \sa SWI_STRUCT_SIM_StatusExp
 *
 */
enum SWI_TYPE_SIM_PreviousOperation
{
    SWI_PREVIOUSOPERATION_None,       /*!< No previous operation. */
    SWI_PREVIOUSOPERATION_ChangePIN,  /*!< Change PIN1 operation. */
    SWI_PREVIOUSOPERATION_ChangePIN2, /*!< Change PIN2 operation. */
    /*! Enable/disable PIN1 verification */
    SWI_PREVIOUSOPERATION_EnableDisablePINVerification, 
    SWI_PREVIOUSOPERATION_VerifyPIN,  /*!< Verify PIN1 operation. */
    SWI_PREVIOUSOPERATION_VerifyPIN2, /*!< Verify PIN2 operation. */
    SWI_PREVIOUSOPERATION_VerifyPUK,  /*!< Verify PIN1 unblocking code. */
    SWI_PREVIOUSOPERATION_VerifyPUK2, /*!< Verify PIN2 unblocking code. */
    SWI_PREVIOUSOPERATION_MEPUnlock,  /*!< Verify MEP unlocking code. */
    SWI_PREVIOUSOPERATION_EnablePIN,  /*!< Enable PIN1. */
    SWI_PREVIOUSOPERATION_DisablePIN  /*!< Disable PIN1. */
};

/**
 *
 * An enumeration of SIM previous operation results.
 *
 * \sa SWI_STRUCT_SIM_StatusExp
 *
 */
enum SWI_TYPE_SIM_PreviousActionResult
{
    /*! Operation succeeded. */
    SWI_PREVIOUSACTIONRESULT_Succeeded,

    /*! Operation failed. */
    SWI_PREVIOUSACTIONRESULT_Failed,

    /*! Operation failed because PIN1 is not enabled. */
    SWI_PREVIOUSACTIONRESULT_PINVerifyDisabled,

    /*! Operation failed because the new PIN1 starts with an emergency 
        number. */
    SWI_PREVIOUSACTIONRESULT_PINStartsEmergency
};

/**
 *
 * An enumeration of SIM code retry information types.
 *
 * \sa SWI_STRUCT_SIM_StatusExp
 *
 */
enum SWI_TYPE_SIM_RetryInformation
{
    SWI_TYPE_RETRYINFORMATION_Pin,  /*!< PIN1 retry count. */
    SWI_TYPE_RETRYINFORMATION_Pin2, /*!< PIN2 retry count. */
    SWI_TYPE_RETRYINFORMATION_Puk,  /*!< PIN1 unblocking code retry count. */
    SWI_TYPE_RETRYINFORMATION_Puk2  /*!< PIN2 unblocking code retry count. */
};

/**
 *
 * An enumeration of PIN2 kick types.
 *
 * \sa SwiChv2StatusKick
 *
 */
enum SWI_TYPE_SIM_Chv2KickType
{
    SWI_TYPE_CHV2KICKTYPE_Verify,  /*!< Verifying PIN2. */
    SWI_TYPE_CHV2KICKTYPE_Change   /*!< Changing PIN2. */
};

/**
 *
 * This structure contains information used to unlock or unblock the SIM.
 *
 * \sa SWI_TYPE_SIM_Password
 * \sa SwiSetSimVerify
 *
 */
struct SWI_STRUCT_SIM_Verify
{
    /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32 sizeStruct;
    
    /*! Password type. */
    enum SWI_TYPE_SIM_Password eType;

    /*! NULL terminated password. */
    swi_char szPassword[LENGTH_MaxCHV];

    /*! NULL terminated unblocking password. */         
    swi_char szUnblockPassword[LENGTH_MaxCHV]; 
};

/**
 *
 * This structure contains information used to change SIM passwords.
 *
 * \sa SwiSetSimPassword
 *
 */
struct SWI_STRUCT_SIM_Password
{
    /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32 sizeStruct;
    
    /*! Password type. */                   
    enum SWI_TYPE_SIM_Password eType;

    /*! NULL terminated old password. */
    swi_char szOldPassword[LENGTH_MaxCHV]; 

    /*! NULL terminated new password. */    
    swi_char szNewPassword[LENGTH_MaxCHV];     
};  

/**
 *
 * This structure contains information required to enable PIN1.
 *
 * \sa SwiSetSimLock
 *
 */
struct SWI_STRUCT_SIM_EnableLock
{
    /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32 sizeStruct;
    
    /*! Set to true to enable PIN1, false to disable. */
    swi_bool bEnable;                           

    /*! NULL terminated password. */
    swi_char szPassword[LENGTH_MaxCHV];            
};

/**
 *
 * This structure contains PIN1 lock state information.
 *
 * \sa SwiGetSimLock
 *
 */
struct SWI_STRUCT_SIM_LockStatus
{
    /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32 sizeStruct;  
    
    /*! True is returned if PIN1 enabled, false otherwise. */
    swi_bool bEnabled;                          
};

/**
 *
 * This structure contains the status of the SIM and lists operations that may
 * be required.  If a previous SIM operation was performed, the status of that
 * operation is also presented. 
 *
 * \sa SWI_TYPE_SIM_StatusExp
 * \sa SWI_TYPE_SIM_RequiredOperation
 * \sa SWI_TYPE_SIM_PreviousOperation
 * \sa SWI_TYPE_SIM_PreviousActionResult
 * \sa SWI_TYPE_SIM_RetryInformation
 * \sa SWI_NOTIFY_SimStatusExp
 *
 */
struct SWI_STRUCT_SIM_StatusExp
{
    /*! The size, in bytes, of this structure.  */
    swi_uint32 sizeStruct;  

    /*! Status of the SIM. */
    enum SWI_TYPE_SIM_StatusExp eStatus;

    /*! Required SIM operation. */                  
    enum SWI_TYPE_SIM_RequiredOperation eRequiredOperation;

    /*! Previous SIM operation. */
    enum SWI_TYPE_SIM_PreviousOperation ePreviousOperation;

    /*! Previous SIM operation result. */
    enum SWI_TYPE_SIM_PreviousActionResult ePreviousActionResult;

    /*! Retry information present. */
    swi_bool bRetryInformationPresent;

    /*! Retry information type. */
    enum SWI_TYPE_SIM_RetryInformation eRetryInformation;

    /*! Retry count. */
    swi_uint32 nNumberRetries;          
};

/**
 *
 * This structure contains information required to perform an MEP unlock.
 *
 * \sa SwiSetMEPUnlock
 *
 */
struct SWI_STRUCT_MEP_Unlock
{
    /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32 sizeStruct;  

    /*! NULL terminated MEP unlock code. */
    swi_char szUnLockCode[LENGTH_MEPCode];     
};
/**
 *
 * This structure contains ICC ID of the SIM  
 *
 * \sa SwiGetIccId
 *
 */
struct SWI_STRUCT_IccId
{
    /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32 sizeStruct;
    
    /*! A binary number representing the ICC ID of the SIM. */
    swi_uint8 nIccId[LENGTH_IccId]; 
};

/**
 *
 *This structure contains information for SIM authentication.
 *
 * \sa SwiSetSimAuthentication
 *
 */
struct SWI_STRUCT_SimAuth 
{
    /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32 sizeStruct;
    
    /*! 16 bytes random number provided by the user*/
    swi_uint8 RandNum[LENGTH_RandNum];
    
    /*! 32 bit SIM response */
    swi_uint32 SimResponse;
    
    /*! 8 bit ciphering key returned from the SIM */
    swi_uint8 Kc[LENGTH_Kc];
};

/* function prototypes */
SWI_API SWI_RCODE SwiSetSimVerify (
    const struct SWI_STRUCT_SIM_Verify* pSim, 
    swi_uint32 timeout);
SWI_API SWI_RCODE SwiSetSimLock (
    const struct SWI_STRUCT_SIM_EnableLock*  pSim, 
    swi_uint32 timeout);
SWI_API SWI_RCODE SwiGetSimLock (
    struct SWI_STRUCT_SIM_LockStatus* pSim, 
    swi_uint32 timeout);
SWI_API SWI_RCODE SwiSetSimPassword (
    const struct SWI_STRUCT_SIM_Password* pSim, 
    swi_uint32 timeout);
SWI_API SWI_RCODE SwiChv2StatusKick(
    enum SWI_TYPE_SIM_Chv2KickType eKickType, 
    swi_uint32 timeout);
extern SWI_API SWI_RCODE SwiSetMEPUnlock (
    const struct SWI_STRUCT_MEP_Unlock* 
    pMEP, swi_uint32 timeout);
extern SWI_API SWI_RCODE SwiGetGsmIMSI(
    swi_charp szMSIN, 
    swi_uint32 sizeBuffer, 
    swi_uint32 *pMCC, 
    swi_uint32 *pMNC, 
    swi_uint32 timeout);
extern SWI_API SWI_RCODE SwiGetIccId(
    struct SWI_STRUCT_IccId *sIccId, 
    swi_uint32 timeout);
extern SWI_API SWI_RCODE SwiGetNetworkProviderID(
    swi_charp szName, 
    swi_uint32 sizeBuffer, 
    swi_uint32 timeout);
extern SWI_API SWI_RCODE SwiSetSimAuthentication(
    struct SWI_STRUCT_SimAuth  *pSimAuth, 
    swi_uint32 timeout);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* __SWIAPI_GSM_SIM */

/* 
 * $Log: SwiApiGsmSim.h,v $
 */
