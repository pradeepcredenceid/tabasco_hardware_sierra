/**
 *
 * $Id: SwiIntPkGsmPhonebook.c,v 1.3 2010/06/10 00:24:10 epasheva Exp $
 *
 * Filename: SwiIntPkGsmPhonebook.c
 *
 * Purpose:  Pack functions for outgoing request for GSM APIs related to 
 *           Phonebook Maintenance.  
 *
 * Copyright: © 2009 Sierra Wireless, Inc. all rights reserved
 *
 */
 
 /* include files */
#include "apidefs.h"
#include "SwiApiGsmPhonebook.h"
#include <string.h>
 
 /**
 *
 * This function packs the Gsm Phonebook Add Entry from the 
 * user structure to the CnS  object's parameter field
 *
 * \param sPhonebook  - user structure with unpacked Phonebook Add Entry
 * \param paramfieldp - Pointer to storage into which the packed  
 *                      data will be placed by this function
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint16 SwiPkGsPhbkAdd( 
    struct SWI_STRUCT_PHONEBK_Entry *sPhonebookp, 
    swi_uint8 *paramfieldp)
 {
    swi_uint8   *localfieldp;           /* Local copy of paramfieldp */
    swi_UCS2    tempucs2string[64];     /* temporary UCS2 string buffer */
    swi_uint16  numchars;
    swi_bool    bresult;
    swi_uint16  szlen;
     
    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    /* Reference ProjectMemo036(GPRS Cns Object Definitions)
     * object: CNS_PHONE_NUM_ADD_EXP  0x1046 - 104 bytes
     */
    *localfieldp++  = sPhonebookp->ePhoneBookType;
    szlen           = slstrlenutf8 (sPhonebookp->szEntryName);
    *localfieldp++  = szlen;

    /* Convert the name to UCS2 */
    slmemset ((char *)&tempucs2string[0] ,0, sizeof (tempucs2string));
    bresult = slUtf8ToUcs2((unsigned char *)&sPhonebookp->szEntryName[0], 
                            tempucs2string, 
                            OBJ_LENGTH_GSM_ENTRY_NAME/sizeof(swi_UCS2), 
                            &numchars);

    piput16s(tempucs2string, szlen, &localfieldp);

    localfieldp += (OBJ_LENGTH_GSM_ENTRY_NAME - szlen*sizeof (swi_UCS2));
    
    *localfieldp++ = sPhonebookp->bInternational;

    *localfieldp++ = sPhonebookp->nPhoneNumberLength;
    piputmulti(&sPhonebookp->byPhoneNumber[0], 
               &localfieldp, 
               OBJ_LENGTH_GSM_PHONE_NUM);
    
    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp ); 
 }
 
 /**
 *
 * This function packs the Gsm Phonebook Edit Entry from the
 * user structure to the CnS  object's parameter field
 *
 * \param sPhonebook  - user structure with unpacked Phonebook Edit Entry
 * \param paramfieldp - Pointer to storage into which the packed  
 *                      data will be placed by this function
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint16 SwiPkGsPhbkEdit( 
    struct SWI_STRUCT_PHONEBK_Entry *sPhonebookp,
    swi_uint8 *paramfieldp)
 {
    swi_uint8   *localfieldp;           /* Local copy of paramfieldp */
    swi_UCS2    tempucs2string[64];     /* temporary UCS2 string buffer */
    swi_uint16  numchars;
    swi_bool    bresult;
    
    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    /* Reference ProjectMemo036(GPRS Cns Object Definitions)
     * object: CNS_PHONE_NUM_EDIT_EXP  0x1047 - 105 bytes
     */
    *localfieldp++ = sPhonebookp->ePhoneBookType;
    *localfieldp++ = sPhonebookp->nID;
    *localfieldp++ = slstrlen (sPhonebookp->szEntryName);

    /* Convert the name to UCS2 */
    slmemset ((char *)&tempucs2string[0] ,0, sizeof (tempucs2string));
    bresult = slUtf8ToUcs2((unsigned char *)&sPhonebookp->szEntryName[0],
                            tempucs2string, 30, &numchars);

    piput16s(tempucs2string, strlen (sPhonebookp->szEntryName), &localfieldp);

    localfieldp += (60 -strlen (sPhonebookp->szEntryName)*sizeof (swi_UCS2));
   
    *localfieldp++ = sPhonebookp->bInternational;

    *localfieldp++ = sPhonebookp->nPhoneNumberLength;
    piputmulti(&sPhonebookp->byPhoneNumber[0], &localfieldp, 40);
    
    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp );
 }

/*
 * $Log: SwiIntPkGsmPhonebook.c,v $
 */

