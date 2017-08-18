/*
 *  $Id: aptestgsmphonebook.c,v 1.2 2010/06/10 00:27:06 epasheva Exp $
 *
 *  Filename: aptestgsmphonebook.c
 *
 *  Purpose:  Contains functions for testing Common Basic APIs
 *
 * Copyright: © 2009 Sierra Wireless, Inc., all rights reserved
 *
 */

/* Linux definitions */
#include <stdio.h>
#include <unistd.h>

/* include files */
#include "SwiDataTypes.h"
#include "sl/sludefs.h"
#include "SwiApiGsmBasic.h"
#include "SwiApiGsmPhonebook.h"
#include "apudefs.h"
#include "aptestdefs.h"

/**************
 *
 * Name:     doSwiGetPhonebookEntryExp
 *
 * Purpose:  Exercises the SwiGetPhonebookEntryExp API - calls the API
 *           and prints information related to the API
 *
 * Parms:    timeout   - timeout value for stop and wait functions as
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiGetPhonebookEntryExp(unsigned int timeout)
{
    SWI_RCODE retcode;
    struct SWI_STRUCT_PHONEBK_Entry sPhonebook;
    int i;

    /* set up for notification */
    retcode = SwiNotify(SWI_NOTIFY_PhonebookReady, timeout);
    retcode = SwiNotify(SWI_NOTIFY_PhonebookEntry, timeout);

    slmemset ((char *)&sPhonebook, 0, sizeof (struct SWI_STRUCT_PHONEBK_Entry));   
  
    sPhonebook.sizeStruct = sizeof (struct SWI_STRUCT_PHONEBK_Entry);
    sPhonebook.ePhoneBookType = SWI_PHONEBK_Adn;
    sPhonebook.bReadFromStart = TRUE;
    sPhonebook.bMoreEntries = TRUE;
    
    printf("SwiGetPhonebookEntryExp Info:\n");
    while (sPhonebook.bMoreEntries)
    {
        retcode = SwiGetPhonebookEntryExp(&sPhonebook, timeout);

        if (retcode == SWI_RCODE_OK)
        {
            if (sPhonebook.nID != 0)
            {
                printf ("\n  Index: %d \n", (int)sPhonebook.nID);
                printf("    Name: %s\n", sPhonebook.szEntryName);
                printf("    Number: ");
                for (i = 0; i < sPhonebook.nPhoneNumberLength; i++)
                {
                    printf( "%d", sPhonebook.byPhoneNumber[i]);
                }
                printf ("\n");
            }
            else
                printf ("\n  No entries in Phone book\n");
            
            /* Subsequent reads */
            sPhonebook.bReadFromStart = FALSE;
        }
        else
        {
            printf("SwiGetPhonebookEntryExp returned error: %d\n", retcode ); 
            break;       
        }
    }
}

/**************
 *
 * Name:     doSwiAddPhonebookEntryExp
 *
 * Purpose:  Exercises the SwiAddPhonebookEntryExp API - calls the API
 *           and prints information related to the API
 *
 * Parms:    timeout   - timeout value for stop and wait functions as
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiAddPhonebookEntryExp(unsigned int timeout)
{
    SWI_RCODE retcode;
    struct SWI_STRUCT_PHONEBK_Entry sPhonebook;
    int i;
  
    retcode = SwiNotify(SWI_NOTIFY_PhonebookReady, timeout);

    slmemset ((char *)&sPhonebook, 0, sizeof (struct SWI_STRUCT_PHONEBK_Entry));   
  
    sPhonebook.sizeStruct = sizeof (struct SWI_STRUCT_PHONEBK_Entry);;
    sPhonebook.ePhoneBookType = SWI_PHONEBK_Adn;
    
    slmemcpy (&(sPhonebook.szEntryName[0]), "Sierra Joe", 10);
    /* This will show the joined 'ae' Unicode character */
    sPhonebook.szEntryName[10] = 0xC7;
    sPhonebook.szEntryName[11] = 0xBD;
    
    sPhonebook.nPhoneNumberLength = 10;
    sPhonebook.byPhoneNumber[0] = 6;
    sPhonebook.byPhoneNumber[1] = 0;
    sPhonebook.byPhoneNumber[2] = 4;
    sPhonebook.byPhoneNumber[3] = 2;
    sPhonebook.byPhoneNumber[4] = 3;
    sPhonebook.byPhoneNumber[5] = 3;
    sPhonebook.byPhoneNumber[6] = 7;
    sPhonebook.byPhoneNumber[7] = 8;
    sPhonebook.byPhoneNumber[8] = 1;
    sPhonebook.byPhoneNumber[9] = 6;
    
    retcode = SwiAddPhonebookEntryExp(&sPhonebook, timeout);
    if (retcode == SWI_RCODE_OK)
    {
        printf("SwiAddPhonebookEntryExp added:\n");
        printf("  Name: %s\n", sPhonebook.szEntryName);
        printf("  Number: ");
        for (i = 0; i < sPhonebook.nPhoneNumberLength; i++)
        {
            printf( "%d", sPhonebook.byPhoneNumber[i]);
        }
        printf ("\n  Index is %d\n", (int)sPhonebook.nID);
    }
    else
    {
        printf("SwiAddPhonebookEntryExp returned error: %d\n", retcode );    
    }
}

/**************
 *
 * Name:     doSwiDeletePhonebookEntry
 *
 * Purpose:  Exercises the SwiDeletePhonebookEntry API - calls the API
 *           and prints information related to the API
 *
 * Parms:    timeout   - timeout value for stop and wait functions as
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiDeletePhonebookEntry(unsigned int timeout)
{
    SWI_RCODE retcode;
    struct SWI_STRUCT_PHONEBK_DeleteEntry sPhonebook;
  
    retcode = SwiNotify(SWI_NOTIFY_PhonebookReady, timeout);
    slmemset ((char *)&sPhonebook, 0, 
                sizeof (struct SWI_STRUCT_PHONEBK_DeleteEntry));   
  
    sPhonebook.sizeStruct = sizeof (struct SWI_STRUCT_PHONEBK_DeleteEntry);;
    sPhonebook.ePhoneBookType = SWI_PHONEBK_Adn;
    
    sPhonebook.nID = 1;
    
    retcode = SwiDeletePhonebookEntry(&sPhonebook, timeout);
    if (retcode == SWI_RCODE_OK)
    {
        printf("SwiDeletePhonebookEntry deleted:\n");

        printf ("\n  Index is %d\n", (int)sPhonebook.nID);
    }
    else
    {
        printf("SwiDeletePhonebookEntry returned error: %d, %d\n", 
                retcode,
                sPhonebook.eResultCode);
        if (sPhonebook.eResultCode == 0x04)
        {
            printf("Error code: %d -  Entry does not exist\n",
            sPhonebook.eResultCode);
        }    
    }
}


/**************
 *
 * Name:     doSwiDeletePhonebookLastNum
 *
 * Purpose:  Exercises the SwiDeletePhonebookLastNum API - calls the API
 *           and prints information related to the API
 *
 * Parms:    timeout   - timeout value for stop and wait functions as
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiDeletePhonebookLastNum(unsigned int timeout)
{
    SWI_RCODE retcode;

    retcode = SwiNotify(SWI_NOTIFY_PhonebookReady, timeout);

    /* Note: Voice must be enabled using AT!CUSTOM="ISVOICEN",1 */
    /* 2 = last number dialed*/
    /* 4 = last number missed */
    /* 5 = last number received */
    retcode = SwiDeletePhonebookLastNum(2, timeout);
    if (retcode == SWI_RCODE_OK)
    {
        printf("SwiDeletePhonebookLastNum LAST NUMBER DIALED deleted:\n");
    }
    else
    {
        printf("SwiDeletePhonebookLastNum LND returned error: %d\n", retcode );
    }

    retcode = SwiDeletePhonebookLastNum(4, timeout);
    if (retcode == SWI_RCODE_OK)
    {
        printf("SwiDeletePhonebookLastNum LAST NUM MISSED deleted:\n");
    }
    else
    {
        printf("SwiDeletePhonebookLastNum LNM returned error: %d\n", retcode );
    }

    retcode = SwiDeletePhonebookLastNum(5, timeout);
    if (retcode == SWI_RCODE_OK)
    {
        printf("SwiDeletePhonebookLastNum LAST NUM RECVD deleted:\n");
    }
    else
    {
        printf("SwiDeletePhonebookLastNum LNR returned error: %d\n", retcode );
    }
}

/**************
 *
 * Name:     doSwiEditPhonebookEntryExp
 *
 * Purpose:  Exercises the SwiEditPhonebookEntryExp API - calls the API
 *           and prints information related to the API
 *
 * Parms:    timeout   - timeout value for stop and wait functions as
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiEditPhonebookEntryExp(unsigned int timeout)
{
    SWI_RCODE retcode;
    struct SWI_STRUCT_PHONEBK_Entry sPhonebook;
    int i;
  
    retcode = SwiNotify(SWI_NOTIFY_PhonebookReady, timeout);

    slmemset ((char *)&sPhonebook, 0, sizeof (struct SWI_STRUCT_PHONEBK_Entry));   
  
    sPhonebook.sizeStruct = sizeof (struct SWI_STRUCT_PHONEBK_Entry);;
    sPhonebook.ePhoneBookType = SWI_PHONEBK_Adn;
    
    slmemcpy (&(sPhonebook.szEntryName[0]),"Donkey Kong",sizeof("Donkey Kong"));
    
    sPhonebook.nPhoneNumberLength = 10;
    sPhonebook.byPhoneNumber[0] = 6;
    sPhonebook.byPhoneNumber[1] = 0;
    sPhonebook.byPhoneNumber[2] = 4;
    sPhonebook.byPhoneNumber[3] = 2;
    sPhonebook.byPhoneNumber[4] = 8;
    sPhonebook.byPhoneNumber[5] = 8;
    sPhonebook.byPhoneNumber[6] = 9;
    sPhonebook.byPhoneNumber[7] = 1;
    sPhonebook.byPhoneNumber[8] = 4;
    sPhonebook.byPhoneNumber[9] = 7;
    
    sPhonebook.nID = 1;
    
    retcode = SwiEditPhonebookEntryExp(&sPhonebook, timeout);
    if (retcode == SWI_RCODE_OK)
    {
        printf("SwiEditPhonebookEntryExp done:\n");
        printf("  Name: %s\n", sPhonebook.szEntryName);
        printf("  Number: ");
        for (i = 0; i < sPhonebook.nPhoneNumberLength; i++)
        {
            printf( "%d", sPhonebook.byPhoneNumber[i]);
        }
        printf ("\n  Index is %d\n", (int)sPhonebook.nID);
    }
    else
    {
        printf("SwiEditPhonebookEntryExp returned error: %d\n", retcode );    
    }
}
 
/**************
 *
 * Name:     doSwiGetPhonebookAvailable
 *
 * Purpose:  Exercises the SwiGetPhonebookAvailable API - calls the API
 *           and prints information related to the API
 *
 * Parms:    timeout   - timeout value for stop and wait functions as
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiGetPhonebookAvailable(unsigned int timeout)
{
    SWI_RCODE retcode;
    struct SWI_STRUCT_PHONEBK_Available sPhonebook;
  
     retcode = SwiNotify(SWI_NOTIFY_PhonebookReady, timeout);
 
    slmemset ((char *)&sPhonebook, 0, sizeof (struct SWI_STRUCT_PHONEBK_Available));   

    sPhonebook.sizeStruct = sizeof (struct SWI_STRUCT_PHONEBK_Available);;        
    retcode = SwiGetPhonebookAvailable(&sPhonebook, timeout);

    if (retcode == SWI_RCODE_OK)
    {
        printf("SwiGetPhonebookAvailable results:\n");
        printf(" ADN    is %s\n",
                (sPhonebook.bADN_Available) ? "available" : "unavailable");
        printf(" FDN    is %s\n",
                (sPhonebook.bFDN_Available) ? "available" : "unavailable");
        printf(" LND    is %s\n",
                (sPhonebook.bLND_Available) ? "available" : "unavailable");
        printf(" MSISDN is %s\n",
                (sPhonebook.bMSISDN_Available) ? "available" : "unavailable");
        printf(" LNM    is %s\n",
                (sPhonebook.bLNM_Available) ? "available" : "unavailable");
        printf(" LNR    is %s\n",
                (sPhonebook.bLNR_Available) ? "available" : "unavailable");
        printf(" SDN    is %s\n",
                (sPhonebook.bSDN_Available) ? "available" : "unavailable");
        printf(" CPHSMN is %s\n",
                (sPhonebook.bCPHSMN_Available) ? "available" : "unavailable");
    }
    else
    {
        printf("SwiGetPhonebookAvailable returned error: %d\n", retcode );     
    }
}

/**************
 *
 * Name:     doSwiGetPhonebookSize
 *
 * Purpose:  Exercises the SwiGetPhonebookSize API - calls the API
 *           and prints information related to the API
 *
 * Parms:    timeout   - timeout value for stop and wait functions as
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiGetPhonebookSize(unsigned int timeout)
{
    SWI_RCODE retcode;
    struct SWI_STRUCT_PHONEBK_Size sPhonebook;
  
    retcode = SwiNotify(SWI_NOTIFY_PhonebookReady, timeout);

    slmemset((char *)&sPhonebook,0,sizeof(struct SWI_STRUCT_PHONEBK_Size));   
    sPhonebook.sizeStruct = sizeof (struct SWI_STRUCT_PHONEBK_Size);;        
       
    retcode = SwiGetPhonebookSize(&sPhonebook, timeout);

    if (retcode == SWI_RCODE_OK)
    {
        printf("SwiGetPhonebookSize results:\n");
        printf(" ADN    free : %d\n", (int)sPhonebook.nADNFreeEntries);
        printf(" ADN    total: %d\n", (int)sPhonebook.nADNTotalEntries);
        printf(" FDN    free : %d\n", (int)sPhonebook.nFDNFreeEntries);
        printf(" FDN    total: %d\n", (int)sPhonebook.nFDNTotalEntries);
        printf(" LND    free : %d\n", (int)sPhonebook.nLNDFreeEntries);
        printf(" LND    total: %d\n", (int)sPhonebook.nLNDTotalEntries);
        printf(" MSISDN free : %d\n", (int)sPhonebook.nMSISDNFreeEntries);
        printf(" MSISDN total: %d\n", (int)sPhonebook.nMSISDNTotalEntries);
        printf(" LNM    free : %d\n", (int)sPhonebook.nLNMFreeEntries);
        printf(" LNM    total: %d\n", (int)sPhonebook.nLNMTotalEntries);
        printf(" LNR    free : %d\n", (int)sPhonebook.nLNRFreeEntries);
        printf(" LNR    total: %d\n", (int)sPhonebook.nLNRTotalEntries);
        printf(" SDN    free : %d\n", (int)sPhonebook.nSDNFreeEntries);
        printf(" SDN    total: %d\n", (int)sPhonebook.nSDNTotalEntries);
        printf(" CPHSMN free : %d\n", (int)sPhonebook.nCPHSMNFreeEntries);
        printf(" CPHSMN total: %d\n", (int)sPhonebook.nCPHSMNTotalEntries);
    }
    else
    {
        printf("SwiGetPhonebookSize returned error: %d\n", (int)retcode );     
    }
}

/**************
 *
 * Name:     doSwiGetEmergencyEntry
 *
 * Purpose:  Exercises the SwiGetEmergencyEntry API - calls the API
 *           and prints information related to the API
 *
 * Parms:    timeout   - timeout value for stop and wait functions as
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiGetEmergencyEntry(unsigned int timeout)
{
    SWI_RCODE retcode;
    struct SWI_STRUCT_Emergency_Number sPhonebook;
    swi_bool bFirst = FALSE;
    int i;
 
    retcode = SwiNotify(SWI_NOTIFY_PhonebookReady, timeout);
 
    slmemset((char *)&sPhonebook,0,sizeof(struct SWI_STRUCT_Emergency_Number));   
    sPhonebook.sizeStruct = sizeof (struct SWI_STRUCT_Emergency_Number);;        

    sPhonebook.bMoreEntries = TRUE;
    
    while (sPhonebook.bMoreEntries)
    {
        retcode = SwiGetEmergencyEntry(&sPhonebook, bFirst, timeout);
        /* Subsequent reads */
        bFirst = FALSE;
       
        if (retcode == SWI_RCODE_OK)
        {
            if (bFirst) 
                printf("SwiGetEmergencyEntry Info:\n");

            printf(" Number: ");
            for (i = 0; i < sPhonebook.nPhoneNumberLength; i++)
            {
                printf( "%d", sPhonebook.byPhoneNumber[i]);
            }
            printf ("\n");
        }
        else
        {
            printf("SwiGetEmergencyEntry returned error: %d\n", retcode ); 
            break;       
        }
    }
}

/**************
 *
 * Name:     doSwiGetFdnMode
 *
 * Purpose:  Exercises the SwiGetFdnMode API - calls the API
 *           and prints information related to the API
 *
 * Parms:    timeout   - timeout value for stop and wait functions as
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiGetFdnMode(unsigned int timeout)
{
    SWI_RCODE retcode;
    swi_bool bFDNmode = FALSE;
  
    retcode = SwiNotify(SWI_NOTIFY_PhonebookReady, timeout);
    
    retcode = SwiGetFdnMode(&bFDNmode, timeout);

    if (retcode == SWI_RCODE_OK)
    {
        printf("SwiGetFdnMode results:\n");
        printf(" FDN mode is %s\n", bFDNmode ? "enabled" : "disabled");
    }
    else
    {
        printf("SwiGetFdnMode returned error: %d\n", retcode );     
    }
}

/**************
 *
 * Name:     doSwiSetFdnModeEnable
 *
 * Purpose:  Exercises the SwiSetFdnMode API - calls the API
 *           and prints information related to the API
 *
 * Parms:    timeout   - timeout value for stop and wait functions as
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiSetFdnModeEnable(unsigned int timeout)
{
    SWI_RCODE retcode;
    struct SWI_STRUCT_FdnMode Fdn;
    struct SWI_STRUCT_SIM_Verify Sim;

    /* set up for notification */
    retcode = SwiNotify(SWI_NOTIFY_SimStatusExp, timeout);
    retcode = SwiNotify(SWI_NOTIFY_PhonebookReady, timeout);

    /* Need CHV2 for this operation */
    /* Following is for Axalto SIMs - white top, purple bottom */
    slmemset ((char *)&Sim, 0, sizeof(struct SWI_STRUCT_SIM_Verify));
    Sim.eType = SWI_SIMPASSWORD_CHV2;
    Sim.sizeStruct = sizeof(struct SWI_STRUCT_SIM_Verify);
    slstrncpy(Sim.szPassword, "2222", sizeof("2222"));
    slstrncpy(Sim.szUnblockPassword,"22222222", sizeof("22222222"));

    retcode = SwiSetSimVerify (&Sim, timeout);

    sleep (3);
    Fdn.sizeStruct = sizeof (struct SWI_STRUCT_FdnMode);;        
    Fdn.bEnabled = TRUE;

    retcode = SwiSetFdnMode(&Fdn, timeout);

    if (retcode == SWI_RCODE_OK)
    {
        printf("SwiSetFdnMode results:\n");
        printf(" FDN mode is set to %s\n",
                Fdn.bEnabled ? "enabled" : "disabled");
    }
    else
    {
        printf("SwiSetFdnMode returned error: %d, %d\n",
                retcode, Fdn.eResultCode );
    }
}
/**************
 *
 * Name:     doSwiSetFdnModeDisable
 *
 * Purpose:  Exercises the SwiSetFdnMode API - calls the API
 *           and prints information related to the API
 *
 * Parms:    timeout   - timeout value for stop and wait functions as
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiSetFdnModeDisable(unsigned int timeout)
{
    SWI_RCODE retcode;
    struct SWI_STRUCT_FdnMode Fdn;
    struct SWI_STRUCT_SIM_Verify Sim;

    /* set up for notification */
    retcode = SwiNotify(SWI_NOTIFY_PhonebookReady, timeout);
    retcode = SwiNotify(SWI_NOTIFY_SimStatusExp, timeout);

    /* Need CHV2 for this operation */
    /* Following is for Axalto SIMs - white top, purple bottom */
    slmemset ((char *)&Sim, 0, sizeof(struct SWI_STRUCT_SIM_Verify));
    Sim.eType = SWI_SIMPASSWORD_CHV2;
    Sim.sizeStruct = sizeof(struct SWI_STRUCT_SIM_Verify);
    slstrncpy(Sim.szPassword, "2222", sizeof("2222"));
    slstrncpy(Sim.szUnblockPassword,"22222222", sizeof("22222222"));

    retcode = SwiSetSimVerify (&Sim, timeout);

    sleep (3);
    Fdn.sizeStruct = sizeof (struct SWI_STRUCT_FdnMode);;        
    Fdn.bEnabled = FALSE;
    retcode = SwiSetFdnMode(&Fdn, timeout);

    if (retcode == SWI_RCODE_OK)
    {
        printf("SwiSetFdnMode results:\n");
        printf(" FDN mode is set to %s\n",
                Fdn.bEnabled ? "enabled" : "disabled");
    }
    else
    {
        printf("SwiSetFdnMode returned error: %d, %d\n",
                retcode, Fdn.eResultCode);
    }
}
 /*
  * $Log: aptestgsmphonebook.c,v $
  */

