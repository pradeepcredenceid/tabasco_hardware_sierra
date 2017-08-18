/**
 * \if CvsId
 * $Id: SwiApiGsmStk.h,v 1.2 2009/03/25 18:31:22 bsiu Exp $
 * \endif
 * \ingroup stk
 * \file SwiApiGsmStk.h
 *
 * \brief Header file for for GSM APIs related to the SIM Application Toolkit.
 *
 * Copyright: © 2009 Sierra Wireless, Inc. all rights reserved
 *
 */
#ifndef __SWIAPI_GSM_STK
#define __SWIAPI_GSM_STK

/* include files */
#include "SwiDataTypes.h"
#include "SwiRcodes.h"
#include "SwiApiGsmBasic.h"

/* CNS Objects packet lengths */
#define OBJ_CNS_STK_CMD_STRING_LENGTH       236
#define OBJ_CNS_STK_RESP_USER_ACK_LENGTH    5
#define OBJ_CNS_STK_RESP_GET_INPUT_LENGTH   245
#define OBJ_CNS_STK_RESP_GET_INPUT_STRING_LENGTH   238
#define OBJ_CNS_STK_RESP_USER_CNF_LENGTH    5
#define OBJ_CNS_STK_RESP_SELECT_ITEM_LENGTH 5

/*
 *
 * SIM Toolkit (STK)
 *
 */

/**
 * 
 * An enumeration of STK Text types. 
 *
 * \sa SwiGetSIMToolKitCommand
 * \sa SWI_STRUCT_STK_Notif_Command
 *
 */
enum SWI_TYPE_STK_TextType
{
    SWI_STKTEXTTTYPE_HelpData           = 0x01,
    SWI_STKTEXTTTYPE_DisplayText        = 0x02,
    SWI_STKTEXTTTYPE_DefaultDisplayText = 0x04,
    SWI_STKTEXTTTYPE_MenuTitle          = 0x08,
    SWI_STKTEXTTTYPE_MenuSelection      = 0x10
};

/**
 * 
 * An enumeration of STK Commands. 
 *
 * \sa SwiGetSIMToolKitCommand
 * \sa SWI_STRUCT_STK_Notif_Command
 *
 */
enum SWI_TYPE_STK_STKCommand
{
    SWI_STKCOMMANDTYPE_None,
    SWI_STKCOMMANDTYPE_MainMenu,
    SWI_STKCOMMANDTYPE_DisplayText,
    SWI_STKCOMMANDTYPE_GetInput,
    SWI_STKCOMMANDTYPE_GetInkey,
    SWI_STKCOMMANDTYPE_CallSetup,
    SWI_STKCOMMANDTYPE_SelectMenu
};

/**
 * 
 * An enumeration of STK Operations. 
 *
 * \sa SwiGetSIMToolKitCommand
 * \sa SWI_STRUCT_STK_Notif_Command
 *
 */
enum SWI_TYPE_STK_Operation
{
    SWI_STKOPERATIONTYPE_MainMenu,
    SWI_STKOPERATIONTYPE_Timeout,
    SWI_STKOPERATIONTYPE_UserAcknowledgement,
    SWI_STKOPERATIONTYPE_GetInput,
    SWI_STKOPERATIONTYPE_GetUserConfirmation,
    SWI_STKOPERATIONTYPE_MenuSelection
};

/**
 * 
 * An enumeration of STK User Responses. 
 *
 * \sa SwiSetSTKUserConfirmationResponse
 * \sa SWI_STRUCT_STK_RsepUserConfirm
 *
 */
enum SWI_TYPE_STK_UserResponse
{
    SWI_STKUSERRESPONSE_No,
    SWI_STKUSERRESPONSE_Yes
};

/**
 * 
 * An enumeration of STK Keys. 
 *
 * \sa SwiGetSIMToolKitCommand
 * \sa SWI_STRUCT_STK_Notif_Command
 *
 */
enum SWI_TYPE_STK_Key
{
    SWI_STYKEY_Digit,
    SWI_STYKEY_GSMAlphabet,
    SWI_STYKEY_Unicode
};

/**
 * 
 * An enumeration of STK Priorities. 
 *
 * \sa SwiGetSIMToolKitCommand
 * \sa SWI_STRUCT_STK_Notif_Command
 *
 */
enum SWI_TYPE_STKPriority
{
    SWI_STKPRIORITY_Normal,
    SWI_STKPRIORITY_High
};

/**
 * 
 * An enumeration of STK Return types. 
 *
 * \sa SwiSetSTKUserAcknowledge
 * \sa SWI_STRUCT_STK_RsepUserAck
 *
 */
enum SWI_TYPE_STKReturn
{
    SWI_STKRETURN_UserAck,
    SWI_STKRETURN_Back,
    SWI_STKRETURN_Abort,
    SWI_STKRETURN_MMIBusy
};

/**
 *
 * This structure contains the STK Command information 
 *
 * \sa SwiGetSIMToolKitCommand
 *
 */
struct SWI_STRUCT_STK_Command
{
    /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32                      sizeStruct;
    swi_uint32                      nSTK_Handle;
    enum SWI_TYPE_STK_TextType      eTextType;
    
    /*! Menu selection number - 0x01 - 0xFF. */    
    swi_uint32                      nMenuSelectionNumber;
    
    /*! false-no more data available true-more data available 
     * (use another set operation to get the remainder of the string */ 
    swi_bool                        bMore;
    
    /*! Number of UCS2 characters in the following string. */ 
    swi_uint32                      cntWCHAR;
    
    /*! variable length string (max length 236 bytes) always coded in UCS2) */ 
    swi_char                        wText [LENGTH_STKCommand_utf8];
};

/**
 *
 * This structure contains the STK Command Notification information 
 *
 * \sa SwiGetSIMToolKitCommand
 *
 */
struct SWI_STRUCT_STK_Notif_Command
{
    /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32                      sizeStruct;
    
    swi_uint32                      nSTK_Handle;
    swi_uint32                      nTimeout;
    enum SWI_TYPE_STK_STKCommand    eSTKCommand;
    enum SWI_TYPE_STK_Operation     eOperation;
    enum SWI_TYPE_STK_TextType      eTextType;
    swi_uint32                      nMinLengthOfInputExpected;
    swi_uint32                      nMaxLengthOfInputExpected;
    /*! false-Don't Echo to user (present '*' characters instead) 
     *  true-Echo characters to user */    
    swi_bool                        bEcho;
    enum SWI_TYPE_STK_Key           eKeyType;
    swi_uint32                      cntItemsInMenu;
    enum SWI_TYPE_STKPriority       ePriority;
};

/**
 *
 * This structure contains the STK User Acknowledge information 
 *
 * \sa SwiSetSTKUserAcknowledge
 *
 */
struct SWI_STRUCT_STK_RsepUserAck
{
   /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32              sizeStruct;
    swi_uint32              nSTK_Handle;
    enum SWI_TYPE_STKReturn eSTKReturn;
};
/* This to fix a typo in Windows' Version */ 
typedef struct SWI_STRUCT_STK_RsepUserAck SWI_STRUCT_STK_RespUserAck;

/**
 *
 * This structure contains the STK Command Notification information 
 *
 * \sa SwiSetSTKSetInput
 *
 */
struct SWI_STRUCT_STK_RsepGetInput
{
   /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32  sizeStruct;
    swi_uint32  nSTK_Handle;
    
   /*! true : more data to come */
    swi_bool    bMoreData;
    
   /*! Number of UCS2 characters in the following string */
    swi_uint32  cntWCHAR;    

   /*! variable length string (max length 10 chars) */
    swi_char    wText[LENGTH_STKRespInput];
};
/* This to fix a typo in Windows' Version */ 
typedef struct SWI_STRUCT_STK_RsepGetInput SWI_STRUCT_STK_RespGetInput;

/**
 *
 * This structure contains the STK User Confirm information 
 *
 * \sa SwiSetSTKUserConfirmationResponse
 *
 */
struct SWI_STRUCT_STK_RsepUserConfirm
{
   /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32                          sizeStruct;
    
    /*! */
    swi_uint32                          nSTK_Handle;
    
    /*!  */
    enum SWI_TYPE_STK_UserResponse      eUserResponse;
};

/**
 *
 * This structure contains the STK Select Item information 
 *
 * \sa SwiSetSTKUserSelectedMenuItem
 *
 */
struct SWI_STRUCT_STK_RsepSelectItem
{
   /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32 sizeStruct;
    swi_uint32 nSTK_Handle;

    /*! Menu item selected */
    swi_uint32 nMenuItem;
};

/* STK (SIM Toolkit) */

/* Return STK command */
extern SWI_API SWI_RCODE SwiGetSIMToolKitCommand (
    struct SWI_STRUCT_STK_Command *STKCommandp, 
    swi_uint32 timeout);

/* Set user ACK */
extern SWI_API SWI_RCODE SwiSetSTKUserAcknowledge (
    const struct SWI_STRUCT_STK_RsepUserAck *RespUserAckp, 
    swi_uint32 timeout);

/* Set input */
extern SWI_API SWI_RCODE SwiSetSTKSetInput (
    const struct SWI_STRUCT_STK_RsepGetInput *RespGetInputp, 
    swi_uint32 timeout);

/* Set user confirmation response */
extern SWI_API SWI_RCODE SwiSetSTKUserConfirmationResponse (
    const struct SWI_STRUCT_STK_RsepUserConfirm *RespUserConfirmp, 
    swi_uint32 timeout);

/* Set user selected menu item */
extern SWI_API SWI_RCODE SwiSetSTKUserSelectedMenuItem (
    const struct SWI_STRUCT_STK_RsepSelectItem *RespSelectItemp, 
    swi_bool *Resultp, 
    swi_uint32 timeout);

#endif /* __SWIAPI_GSM_STK */
/*
 * $Log: SwiApiGsmStk.h,v $
 */
