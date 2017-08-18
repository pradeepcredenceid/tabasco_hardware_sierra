/**
 * \if CvsId
 * $Id: SwiApiGsmPhonebook.h,v 1.4 2010/06/10 00:19:10 epasheva Exp $
 * \endif
 * \ingroup gsmvoice
 * \file SwiApiGsmPhonebook.h
 *
 * \brief Header file for for GSM APIs related to the PhoneBook.
 *
 * Copyright: © 2009 Sierra Wireless, Inc. all rights reserved
 *
 */
#ifndef __SWIAPI_GSM_PHONEBK
#define __SWIAPI_GSM_PHONEBK

#ifdef __cplusplus
extern "C" {
#endif

/* include files */
#include "SwiDataTypes.h"
#include "SwiRcodes.h"
#include "SwiApiGsmBasic.h"

/* object 0x1045 */
#define OBJ_LENGTH_GSM_ENTRY_NAME       60
#define OBJ_LENGTH_GSM_PHONE_NUM        40
#define OBJ_LENGTH_EmergencyPhoneNumber 6

/**
 *
 * An enumeration of GSM phonebooks. 
 *
 * \sa SWI_NOTIFY_PhonebookEntry
 * \sa SwiDeletePhonebookLastNum
 *
 */
enum SWI_TYPE_Phonebook
{
    SWI_PHONEBK_Adn,    /*!< Abbreviated dialing numbers */
    SWI_PHONEBK_Fdn,    /*!< Fixed dialing numbers */
    SWI_PHONEBK_Lnd,    /*!< Last numbers dialed */
    SWI_PHONEBK_Msisdn, /*!< Mobile subscriber international subscriber
                             identity numbers */
    SWI_PHONEBK_Lnm,    /*!< Last numbers missed */
    SWI_PHONEBK_Lnr,    /*!< Last numbers received */
    SWI_PHONEBK_Sdn,    /*!< Service dialing numbers */
    SWI_PHONEBK_Cphsmn /*!< CPHS mailing numbers */
};

/**
 * 
 * An enumeration of phonebook operation results. 
 *
 * \sa SWI_STRUCT_PHONEBK_Entry
 * \sa SWI_STRUCT_PHONEBK_DeleteEntry
 * \sa SWI_STRUCT_FdnMode
 *
 */
enum SWI_TYPE_PHONEBOOK_Result
{
    /*! The phonebook operation was successful. */
    SWI_PHONEBOOKRESULT_Success,
    
    /*! Extended number field not available.*/
    SWI_PHONEBOOKRESULT_Ext1NotAvail,
    
    /*! Too many phonebook entries greater than 20 digits. */   
    SWI_PHONEBOOKRESULT_TooManyEntries, 
    
    /*! CHV2 validation error. */
    SWI_PHONEBOOKRESULT_CHV2NotValid,
    
    /*! Phonebook entry does not exist. */
    SWI_PHONEBOOKRESULT_NotExist,
    
    /*! General operation failure. */       
    SWI_PHONEBOOKRESULT_Failed          
};

/**
 *
 * This structure contains phonebook size information and is used as output 
 * for the SwiGetPhonbookSize function.
 *
 * \sa SwiGetPhonebookSize
 *
 */
struct SWI_STRUCT_PHONEBK_Size
{
    /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32 sizeStruct;
    
    /*! Number of free entries in the ADN phonebook. */                 
    swi_uint32 nADNFreeEntries;
    
    /*! Total number of entries in the ADN phonebook. */            
    swi_uint32 nADNTotalEntries;
    
    /*! Number of free entries in the FDN phonebook. */         
    swi_uint32 nFDNFreeEntries;
    
    /*! Total number of entries in the FDN phonebook. */            
    swi_uint32 nFDNTotalEntries;
    
    /*! Number of free entries in the LND phonebook. */         
    swi_uint32 nLNDFreeEntries;
    
    /*! Total number of entries in the LND phonebook. */            
    swi_uint32 nLNDTotalEntries;
    
    /*! Number of free entries in the MSISDN phonebook. */          
    swi_uint32 nMSISDNFreeEntries;
    
    /*! Total number of entries in the MSISDN phonebook. */         
    swi_uint32 nMSISDNTotalEntries;
    
    /*! Number of free entries in the LNM phonebook. */     
    swi_uint32 nLNMFreeEntries;
    
    /*! Total number of entries in the LNM phonebook. */        
    swi_uint32 nLNMTotalEntries;
    
    /*! Number of free entries in the LNR phonebook. */         
    swi_uint32 nLNRFreeEntries;
    
    /*! Total number of entries in the LNR phonebook. */        
    swi_uint32 nLNRTotalEntries;
    
    /*! Number of free entries in the SDN phonebook. */         
    swi_uint32 nSDNFreeEntries;
    
    /*! Total number of entries in the SDN phonebook. */            
    swi_uint32 nSDNTotalEntries;
    
    /*! Number of free entries in the CPHS mailbox phonebook. */            
    swi_uint32 nCPHSMNFreeEntries;
    
    /*! Total number of entries in the CPHS mailbox phonebook. */           
    swi_uint32 nCPHSMNTotalEntries;        
};

/**
 *
 * This structure contains phone book entry information. 
 *
 * \sa SWI_TYPE_Phonebook
 * \sa SWI_TYPE_PHONEBOOK_Result
 * \sa SwiGetPhonebookEntry
 * \sa SwiAddPhonebookEntry
 * \sa SwiEditPhonebookEntry
 *
 */
struct SWI_STRUCT_PHONEBK_Entry
{
    /*! The size, in bytes, of this structure. */
    swi_uint32 sizeStruct; 
    
    /*! Entry ID. */                        
    swi_uint32 nID;
    
    /*! Phone book type. */                            
    enum SWI_TYPE_Phonebook ePhoneBookType;
    
    /*! A NULL terminated string representing the phonebook entry name. */                      
    swi_char szEntryName[LENGTH_EntryName];
    
    /*! Phone number length; minimum 1 digit, maximum 20 digits. */
    swi_uint32 nPhoneNumberLength;
    
    /*! A binary number representing the phone number. */
    swi_uint8 byPhoneNumber[LENGTH_GSM_PhoneNumber];
    
    /*! True indicates the phonebook data is valid, false otherwise. */ 
    swi_bool bValid;    
    
    /*! True indicates the phone number is international, false otherwise. */                          
    swi_bool bInternational;
    
    /*! True is passed to the modem to read from the start of the phonebook.  
        False is passed to read from the current position. */                       
    swi_bool bReadFromStart;
    
    /*! True is returned if there are more entries, false otherwise.  This 
        field is only valid when reading from a phonebook. */                   
    swi_bool bMoreEntries;
    
    /*! Result of the phonebook operation. */               
    enum SWI_TYPE_PHONEBOOK_Result eResultCode;                  
};

/**
 *
 * This structure contains information used to delete a phonebook entry.  
 *
 * \sa SWI_TYPE_PHONEBOOK_Result
 * \sa SWI_TYPE_Phonebook
 * \sa SwiDeletePhonebookEntry
 *
 */
struct SWI_STRUCT_PHONEBK_DeleteEntry
{
    /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32 sizeStruct; 
    
    /*! Phonebook to delete from. */            
    enum SWI_TYPE_Phonebook ePhoneBookType;  
    
    /*! Entry ID to be deleted. */      
    swi_uint32 nID;
    
    /*! Result of delete operation.  This value is returned after calling 
        SwiDeletePhonebookEntry. */
    enum SWI_TYPE_PHONEBOOK_Result eResultCode;          
};

/**
 *
 * This structure is used to indicate when phonebook operations are 
 * permitted.
 *
 * \sa SWI_NOTIFY_PhonebookReady
 *
 */
struct SWI_STRUCT_PhonebookReady
{
    /*! The size, in bytes, of this structure. */
    swi_uint32 sizeStruct;
    
    /*! True if phonebook operations are permitted, false otherwise. */          
    swi_bool bReady;              
};

/**
 *
 * This structure contains phonebook availability information.
 *
 * \sa SwiGetPhonebookAvailable
 *
 */
struct SWI_STRUCT_PHONEBK_Available
{
    /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32 sizeStruct;
    
    /*! ADN phonebook available flag.  True if available, false otherwise. */           
    swi_bool bADN_Available;
    
    /*! FDN phonebook available flag.  True if available, false otherwise. */
    swi_bool bFDN_Available;
    
    /*! LND phonebook available flag.  True if available, false otherwise. */
    swi_bool bLND_Available;
    
    /*! MSISDN phonebook available flag.  True if available, false otherwise.*/
    swi_bool bMSISDN_Available;
    
    /*! LNM phonebook available flag.  True if available, false otherwise. */
    swi_bool bLNM_Available;
    
    /*! LNR phonebook available flag.  True if available, false otherwise. */
    swi_bool bLNR_Available;
    
    /*! SDN phonebook available flag.  True if available, false otherwise. */
    swi_bool bSDN_Available;
    
    /*! CPHS mailbox phonebook available flag.  True if available, false
        otherwise. */
    swi_bool bCPHSMN_Available;
};


/**
 *
 * This structure is used as output of the SwiGetEmergencyEntry function.   
 *
 * \sa SwiGetEmergencyEntry 
 *
 */
struct SWI_STRUCT_Emergency_Number
{
    /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32 sizeStruct;

    /*! emergency number length
        - valid range: 1 to 4 digits */
    swi_uint32 nPhoneNumberLength;

    /*! emergency phone number
        - 8-bit BCD */
    swi_uint8 byPhoneNumber[LENGTH_EmergencyPhoneNumber];  

    /*! more entries flag indicates if there are more emergency
        numbers to be read */
    swi_bool bMoreEntries;                                  
};

/** 
 *
 * This structure contains information used to configure the FDN mode.
 *
 * \sa SWI_TYPE_PHONEBOOK_Result
 * \sa SetFdnMode
 *
 */
struct SWI_STRUCT_FdnMode
{
    /*! The size, in bytes, of this structure.  This value must always be 
        populated by the calling application. */
    swi_uint32 sizeStruct;
    
    /*! FDN enable flag.  Set to true to enable, false to disable. */
    swi_bool bEnabled;
    
    /*! Result of FDN mode change request. */           
    enum SWI_TYPE_PHONEBOOK_Result eResultCode;      
};

/* Phonebook Maintenance prototypes */
extern SWI_API SWI_RCODE SwiGetPhonebookSize(
    struct SWI_STRUCT_PHONEBK_Size*  pPhonebook, 
    swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiGetPhonebookEntryExp(
    struct SWI_STRUCT_PHONEBK_Entry* pPhonebook,
    swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiAddPhonebookEntryExp(
    struct SWI_STRUCT_PHONEBK_Entry* pPhonebook,
    swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiEditPhonebookEntryExp(
    struct SWI_STRUCT_PHONEBK_Entry* pPonebook, 
    swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiDeletePhonebookEntry(
    struct SWI_STRUCT_PHONEBK_DeleteEntry* pPhonebook, 
    swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiDeletePhonebookLastNum(
    const enum SWI_TYPE_Phonebook ePhoneBookType, 
    swi_uint32 timeout);
                    
extern SWI_API SWI_RCODE SwiGetPhonebookAvailable(
    struct SWI_STRUCT_PHONEBK_Available* pPhonebook, 
    swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiGetFdnMode(
    swi_bool* pFdnEnabled,
    swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiGetEmergencyEntry(
    struct SWI_STRUCT_Emergency_Number *pEmergencyNumber, 
    swi_bool bFirst, 
    swi_uint32 timeout);

extern SWI_API SWI_RCODE SwiSetFdnMode(
    struct SWI_STRUCT_FdnMode *Fdnp,
    swi_uint32 timeout);

#if 0
/**
 *
 * This function is used to set the fixed number dialing (FDN) mode
 * on the modem.
 * 
 * Before calling this function, CHV2 must be verified.  CHV2 is prompted for
 * by calling SwiChv2StatusKick.  
 *         
 * \param  [IN/OUT] pFdn:  
 *         Set bEnabled to true to enable FDN, false otherwise.  eResultCode
 *         contains the result of the request.
 *
 * \param  [IN] timeout:
 *         Response timeout value in milliseconds.
 *
 * \return SWI_RCODE_OK:
 *         The request has been processed and values returned are valid.
 *         See eResultCode within the SWI_STRUCT_FdnMode structure for
 *         the result of the operation.
 *
 * \return See SWI_RCODE for other error return codes.
 *
 * \sa SWI_STRUCT_FdnMode
 * \sa SwiChv2StatusKick
 * \sa SwiGetFdnMode
 *
 */
SWI_API SWI_RCODE SwiSetFdnMode(SWI_STRUCT_FdnMode* pFdn, swi_uint32 timeout);

/** 
 *
 * This function is used to get CHV2 status from the modem.  
 *         
 * SIM API functions can not be called until a SWI_NOTIFY_SimsStatusExp
 * notification is received.  This indicates that the SIM is ready to 
 * process user requests.
 * 
 * The kick type determines if the user is prompted to enter the CHV2.  For
 * example, if the card is kicked for CHV2 verify, then 
 * SWI_NOTIFY_SimsStatusExp will contain a user prompt to enter CHV2.  If the
 * card is kicked for CHV2 change, SWI_NOTIFY_SimsStatusExp will not contain
 * user prompts.
 *
 * \param  [IN] eKickType:
 *         CHV2 kick type
 *
 * \param  [IN] timeout:
 *         Response timeout value in milliseconds
 *
 * \return SWI_RCODE_OK:
 *         The request has been processed.  The SWI_NOTIFY_SimStatusExp 
 *         notification indicates the result of the operation and any
 *         additional actions required by the user. 
 *
 * \return See SWI_RCODE for other error return codes.
 *
 * \sa SWI_TYPE_SIM_Chv2KickType
 * \sa SWI_NOTIFY_SimStatusExp
 *
 */
SWI_API SWI_RCODE SwiChv2StatusKick(SWI_TYPE_SIM_Chv2KickType eKickType, 
                                    U32 timeout);
#endif

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* __SWIAPI_GSM_PHONEBK */
/*
 * $Log: SwiApiGsmPhonebook.h,v $
 */
