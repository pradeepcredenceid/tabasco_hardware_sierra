/**
 *
 * $Id: SwiIntUpkGsmPhonebook.c,v 1.2 2010/06/10 00:25:24 epasheva Exp $
 *
 * Filename: SwiIntUpkGsmPhonebook.c
 *
 * Purpose:  Contains unpacking routines for GSM Objects in the Phonebook
 *           Maintenance category.
 *
 * Copyright: © 2009 Sierra Wireless, Inc. all rights reserved
 *
 */
#include "apidefs.h"
#include "SwiApiGsmPhonebook.h"
/**
 *
 * This function unpacks the Phonebook entry from the CnS
 * object's parameter field
 *
 * \param  paramfieldp   - Pointer to the CnS parameter field of the 
 *                         CnS object containing the Phonebook entry
 * \param  sPhonebookp   - Pointer to storage into which the Phonebook entry
 *                         will be placed by this function
 *
 */
package void SwiUpkGsPhbkGetEntry(
    swi_uint8 *paramfieldp,
    struct SWI_STRUCT_PHONEBK_Entry* sPhonebookp )
{    
    int nUCS2char;
    
    /* Reference ProjectMemo036(GPRS Cns Object Definitions)
     * object: CNS_PHONE_NUM_READ_EXP  0x1045
     */
    
    /*! Phone book type. */   
    pigetmulti(&paramfieldp, (swi_uint8 *)&(sPhonebookp->ePhoneBookType), 1 ); 
    
    /* Index number */
    pigetmulti(&paramfieldp, (swi_uint8 *)&(sPhonebookp->nID), 1 ); 
    
    /* True indicates the phonebook data is valid, false otherwise. */
    pigetmulti(&paramfieldp, (swi_uint8 *)&(sPhonebookp->bValid), 1 ); 
    
    pigetmulti(&paramfieldp, (swi_uint8 *)&nUCS2char, 1 ); 

    /* unpack phonebook entry name string (convert from UCS2 to UTF8) */
    SwiIntUcs2Unpack(&paramfieldp, 
                     &sPhonebookp->szEntryName[0], 
                     LENGTH_EntryName, 
                     nUCS2char, 
                     OBJ_LENGTH_GSM_ENTRY_NAME);
          
    /* True indicates the phone number is international, false otherwise. */
    pigetmulti(&paramfieldp, (swi_uint8 *)&(sPhonebookp->bInternational), 1 );
     
    /*! Phone number length; minimum 1 digit, maximum 20 digits. */
    sPhonebookp->nPhoneNumberLength = *paramfieldp++;
    /* A binary number representing the phone number */
    pigetmulti(&paramfieldp, 
               (swi_uint8 *)&(sPhonebookp->byPhoneNumber[0]), 
               OBJ_LENGTH_GSM_PHONE_NUM); 

    /* True is returned if there are more entries, false otherwise.  This 
     field is only valid when reading from a phonebook. */
    pigetmulti(&paramfieldp, (swi_uint8 *)&(sPhonebookp->bMoreEntries), 1); 
}

/**
 *
 * This function unpacks the Phonebook Available info from the CnS
 * object's parameter field
 *
 * \param  paramfieldp   - Pointer to the CnS parameter field of the 
 *                         CnS object containing the Phonebook Available info
 * \param  sPhonebookp   - Pointer to storage into which the Phonebook Available
 *                         info will be placed by this function
 *
 */
package void SwiUpkGsPhbkAvail(
    swi_uint8 *paramfieldp, 
     struct SWI_STRUCT_PHONEBK_Available* sPhonebookp )
{
    /* Reference ProjectMemo036(GPRS Cns Object Definitions)
     * object: CNS_PHONE_BOOK_AVAIL  0x1049
     */
    
    /*! ADN phonebook available flag.  True if available, false otherwise. */           
    pigetmulti(&paramfieldp, (swi_uint8 *)&(sPhonebookp->bADN_Available), 1 ); 
    
    /*! FDN phonebook available flag.  True if available, false otherwise. */
    pigetmulti(&paramfieldp, (swi_uint8 *)&(sPhonebookp->bFDN_Available), 1 );
    
    /*! LND phonebook available flag.  True if available, false otherwise. */
    pigetmulti(&paramfieldp, (swi_uint8 *)&(sPhonebookp->bLND_Available), 1 );
    
    /*! MSISDN phonebook available flag.  True if available, false otherwise.*/
    pigetmulti(&paramfieldp, (swi_uint8 *)&(sPhonebookp->bMSISDN_Available), 1 );
    
    /*! LNM phonebook available flag.  True if available, false otherwise. */
    pigetmulti(&paramfieldp, (swi_uint8 *)&(sPhonebookp->bLNM_Available), 1 );
    
    /*! LNR phonebook available flag.  True if available, false otherwise. */
    pigetmulti(&paramfieldp, (swi_uint8 *)&(sPhonebookp->bLNR_Available), 1 );
    
    /*! SDN phonebook available flag.  True if available, false otherwise. */
    pigetmulti(&paramfieldp, (swi_uint8 *)&(sPhonebookp->bSDN_Available), 1 );
    
    /*! CPHS mailbox phonebook available flag.  True if available, false
        otherwise. */
    pigetmulti(&paramfieldp, (swi_uint8 *)&(sPhonebookp->bCPHSMN_Available), 1 );
}

/**
 *
 * This function unpacks the Phonebook Size info from the CnS
 * object's parameter field
 *
 * \param  paramfieldp   - Pointer to the CnS parameter field of the 
 *                         CnS object containing the Phonebook Size info
 * \param  sPhonebookp   - Pointer to storage into which the Phonebook Size
 *                         info will be placed by this function
 *
 */
package void SwiUpkGsPhbkSize(
    swi_uint8 *paramfieldp, 
     struct SWI_STRUCT_PHONEBK_Size* sPhonebookszp )
{
    /* Reference ProjectMemo017(UMTS Cns Object Definitions)
     * object: CNS_PHONE_BOOK_SIZE_EXP  0x104A - 20 bytes
     */
    
    /*! Number of free entries in the ADN phonebook. */                 
    /*! Total number of entries in the ADN phonebook. */               
    sPhonebookszp->nADNFreeEntries = *paramfieldp++;   
    sPhonebookszp->nADNTotalEntries = *paramfieldp++;
    
    /*! Number of free entries in the FDN phonebook. */         
    /*! Total number of entries in the FDN phonebook. */            
    sPhonebookszp->nFDNFreeEntries = *paramfieldp++;   
    sPhonebookszp->nFDNTotalEntries = *paramfieldp++;
    
    /*! Number of free entries in the LND phonebook. */         
    /*! Total number of entries in the LND phonebook. */            
    sPhonebookszp->nLNDFreeEntries = *paramfieldp++;   
    sPhonebookszp->nLNDTotalEntries = *paramfieldp++;
    
    /*! Number of free entries in the MSISDN phonebook. */          
    /*! Total number of entries in the MSISDN phonebook. */         
    sPhonebookszp->nMSISDNFreeEntries = *paramfieldp++;   
    sPhonebookszp->nMSISDNTotalEntries = *paramfieldp++;
    
    /*! Number of free entries in the LNM phonebook. */     
    /*! Total number of entries in the LNM phonebook. */        
    sPhonebookszp->nLNMFreeEntries = *paramfieldp++;   
    sPhonebookszp->nLNMTotalEntries = *paramfieldp++;
    
    /*! Number of free entries in the LNR phonebook. */         
    /*! Total number of entries in the LNR phonebook. */        
    sPhonebookszp->nLNRFreeEntries = *paramfieldp++;   
    sPhonebookszp->nLNRTotalEntries = *paramfieldp++;
    
    /*! Number of free entries in the SDN phonebook. */         
    /*! Total number of entries in the SDN phonebook. */            
    sPhonebookszp->nSDNFreeEntries = *paramfieldp++;   
    sPhonebookszp->nSDNTotalEntries = *paramfieldp++;
    
    /*! Number of free entries in the CPHS mailbox phonebook. */            
    /*! Total number of entries in the CPHS mailbox phonebook. */           
    sPhonebookszp->nCPHSMNFreeEntries = *paramfieldp++;   
    sPhonebookszp->nCPHSMNTotalEntries = *paramfieldp++;    
 };

/**
 *
 * This function unpacks the Phonebook Emergency number from the CnS
 * object's parameter field
 *
 * \param  paramfieldp   - Pointer to the CnS parameter field of the 
 *                         CnS object containing the Phonebook Emergency number
 * \param  sPhonebookp   - Pointer to storage into which the Phonebook Emergency
 *                         number will be placed by this function
 *
 */
package void SwiUpkGsPhbkGetEmerNum(
    swi_uint8 *paramfieldp, 
     struct SWI_STRUCT_Emergency_Number * sPhonebookp )
{
    /* Reference ProjectMemo017(UMTS Cns Object Definitions)
     * object: CNS_EMERG_NUM_EXP – 0x1072 - 8 bytes
     * Note CNS_EMERGENCY_NUM is obsolete.
     */
     
    /*! emergency number length - valid range: 1 to 4 digits */
    sPhonebookp->nPhoneNumberLength = *paramfieldp++; 

    /*! emergency phone number - 8-bit BCD */
    pigetmulti(&paramfieldp, 
               (swi_uint8 *)&(sPhonebookp->byPhoneNumber[0]), 
               OBJ_LENGTH_EmergencyPhoneNumber);

    /*! more entries flag indicates if there are more emergency
        numbers to be read */
    sPhonebookp->bMoreEntries = *paramfieldp++; 
}

/*
 * $Log: SwiIntUpkGsmPhonebook.c,v $
 */

