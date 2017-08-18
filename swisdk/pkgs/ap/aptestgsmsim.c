/*
 *  $Id: aptestgsmsim.c,v 1.11 2010/06/10 00:27:06 epasheva Exp $
 *
 *  Filename: aptestgsmsim.c
 *
 *  Purpose:  Contains functions for testing GSM SIM APIs.
 *
 * Copyright: © 2008 Sierra Wireless, Inc., all rights reserved
 *
 */

/* Linux definitions */
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* include files */
#include "SwiDataTypes.h"
#include "sl/sludefs.h"
#include "apudefs.h"
#include "aptestdefs.h"

/* Variables */
int status;
static swi_bool waiting = TRUE;

/* Functions */
/**************
 *
 * Name:     doSwiSetMEPUnlock
 *
 * Purpose:  Exercises the SwiSetMEPUnlock API - calls the API and prints
 *           information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiSetMEPUnlock(unsigned int timeout)
{
    SWI_RCODE retcode;
    static struct SWI_STRUCT_MEP_Unlock MEP;
    struct SWI_STRUCT_SIM_Verify simVerify;
    swi_char lasterror[80];
    
    /* set up for notification */
    retcode = SwiNotify(SWI_NOTIFY_SimStatusExp, timeout);
    
    slmemset((char *)&simVerify, 0, sizeof(struct SWI_STRUCT_SIM_Verify));
    simVerify.sizeStruct = sizeof(struct SWI_STRUCT_SIM_Verify);
    simVerify.eType = SWI_SIMPASSWORD_CHV1;
    slstrncpy(simVerify.szPassword, "1234", sizeof("1234"));
    slstrncpy(simVerify.szUnblockPassword,"11111111", sizeof("11111111"));

 
    /* Fill up our data  */
    retcode = SwiSetSimVerify( &simVerify, timeout );
    
    /* Fill up our data  */

    /* Note: The modem must have been locked with the 
     * AT+CLCK="PN",1,"10519602" first. 
     *
     *  The password is arbitrarily hard-coded here.
     *       After that, a SIM card must be inserted 
     *       In the test, AT!CUSTOM="MEPCODE",1 and
     *                    AT!CUSTOM="MEPLOCK",1 were set
     * To check the lock status, use AT+CLCK="PN",2 
     * where in the output, 0 = unlocked, and 1 = locked 
     */
    printf("\nTEST A: SENDING MEP code\n");
    MEP.sizeStruct = 13;
    slmemset (&(MEP.szUnLockCode[0]), 0, MEP.sizeStruct);   
    MEP.szUnLockCode[0] = 8;
    slmemcpy (&(MEP.szUnLockCode[1]), "10519602", 8);
    
    retcode = SwiSetMEPUnlock( &MEP, timeout );

    if( retcode == SWI_RCODE_OK )
    {
        printf ("\nMEP unlock is being processed\n");
        printf("\nPlease use CTRL+C to exit this test.\n" );
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
        printf ("MEP unlock command timed out\n");
    else
    {
        printf ("MEP unlock command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    while(1)
    {
        sleep(10);
    }
}

/**************
 *
 * Name:     doSwiSetSimVerify
 *
 * Purpose:  Exercises the SwiSetSimVerify API - calls the API and prints
 *           information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    The call to SwiSetSimVerify() is purposefully commented out
 *           since sending this request unknowningly from the test application
 *           could result in a blocked or rejected SIM. 
 *
 **************/
package void doSwiSetSimVerify(unsigned int timeout)
{
    SWI_RCODE retcode = 0xFFFF;
    struct SWI_STRUCT_SIM_Verify simVerify;
    swi_char  lasterror[80];
    
    /* set up for notification */
    retcode = SwiNotify(SWI_NOTIFY_SimStatusExp, timeout);

    printf("\nTEST A: SUCCESSFUL verify PUK\n");
    slmemset((char *)&simVerify, 0, sizeof(struct SWI_STRUCT_SIM_Verify));
    simVerify.sizeStruct = sizeof(struct SWI_STRUCT_SIM_Verify);
    simVerify.eType = SWI_SIMPASSWORD_UnblockingCHV1;
    slstrncpy(simVerify.szPassword, "1234", sizeof("1234"));
    slstrncpy(simVerify.szUnblockPassword,"11111111", sizeof("11111111"));

 
    /* Fill up our data  */
/*    retcode = SwiSetSimVerify( &simVerify, timeout ); */

    if( retcode == SWI_RCODE_OK )
        printf ("SIM Verify command is being processed\n");
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
        printf ("SIM Verify command timed out\n");
    else if (retcode == 0xFFFF)
    {
        printf("SIM Verify command has been commented out\n");
        return;
    }
    else
    {
        printf ("SIM Verify command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }

    
    printf("\nTEST B: SUCCESSFUL verify PIN\n");
    slmemset((char *)&simVerify, 0, sizeof(struct SWI_STRUCT_SIM_Verify));
    simVerify.sizeStruct = sizeof(struct SWI_STRUCT_SIM_Verify);
    simVerify.eType = SWI_SIMPASSWORD_CHV1;
    slstrncpy(simVerify.szPassword, "1234", sizeof("1234"));
 
    /* Fill up our data  */
    retcode = SwiSetSimVerify( &simVerify, timeout );
    
    if( retcode == SWI_RCODE_OK )
        printf ("SIM Verify command is being processed\n");
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
        printf ("SIM Verify command timed out\n");
    else if (retcode == 0xFFFF)
    {
        printf("SIM Verify command has been commented out\n");
        return;
    }
    else
    {
        printf ("SIM Verify command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }

    printf("\nTEST C: Attempt to Verify PUK with missing PIN\n");
    printf("   Should receive an invalid parameter error from the modem\n");
    slmemset((char *)&simVerify, 0, sizeof(struct SWI_STRUCT_SIM_Verify));
    simVerify.sizeStruct = sizeof(struct SWI_STRUCT_SIM_Verify);
    simVerify.eType = SWI_SIMPASSWORD_UnblockingCHV1;
    slstrncpy(simVerify.szUnblockPassword,"11111111", sizeof("11111111"));
 
    /* Fill up our data  */
    retcode = SwiSetSimVerify( &simVerify, timeout );

    if( retcode == SWI_RCODE_OK )
        printf ("SIM Verify command is being processed\n");
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
        printf ("SIM Verify command timed out\n");
    else if (retcode == 0xFFFF)
    {
        printf("SIM Verify command has been commented out\n");
        return;
    }
    else
    {
        printf ("SIM Verify command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    
    printf("\nTEST D: Attempt to Verify PUK with missing PUK\n");
    printf("   Should receive an invalid parameter error from the modem\n");
    slmemset((char *)&simVerify, 0, sizeof(struct SWI_STRUCT_SIM_Verify));
    simVerify.sizeStruct = sizeof(struct SWI_STRUCT_SIM_Verify);
    simVerify.eType = SWI_SIMPASSWORD_UnblockingCHV1;
    slstrncpy(simVerify.szPassword, "1234", sizeof("1234"));
 
    /* Fill up our data  */
    retcode = SwiSetSimVerify( &simVerify, timeout );

    if( retcode == SWI_RCODE_OK )
        printf ("SIM Verify command is being processed\n");
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
        printf ("SIM Verify command timed out\n");
    else if (retcode == 0xFFFF)
    {
        printf("SIM Verify command has been commented out\n");
        return;
    }
    else
    {
        printf ("SIM Verify command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    
    printf("\nTEST E: Attempt to Verify PIN with missing PIN\n");
    printf("   Should receive an invalid parameter error from the modem\n");
    slmemset((char *)&simVerify, 0, sizeof(struct SWI_STRUCT_SIM_Verify));
    simVerify.sizeStruct = sizeof(struct SWI_STRUCT_SIM_Verify);
    simVerify.eType = SWI_SIMPASSWORD_CHV1;
 
    /* Fill up our data  */
    retcode = SwiSetSimVerify( &simVerify, timeout );

    if( retcode == SWI_RCODE_OK )
        printf ("SIM Verify command is being processed\n");
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
        printf ("SIM Verify command timed out\n");
    else if (retcode == 0xFFFF)
    {
        printf("SIM Verify command has been commented out\n");
        return;
    }
    else
    {
        printf ("SIM Verify command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }

}
/**************
 *
 * Name:     doSwiSetSimLock
 *
 * Purpose:  Exercises the SwiSetSimLock API - calls the API and prints
 *           information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    The call to SwiSetSimLock() is purposefully commented out
 *           since sending this request unknowningly from the test application
 *           could result in a blocked or rejected SIM. 
 *
 **************/
package void doSwiSetSimLock(unsigned int timeout)
{
    SWI_RCODE retcode = 0xFFFF;
    struct SWI_STRUCT_SIM_EnableLock simLock;
    swi_char lasterror[80];
   
    /* set up for notification */
    retcode = SwiNotify(SWI_NOTIFY_SimStatusExp, timeout);
    
    printf("\nTEST A: SUCCESSFUL enabling\n");
    slmemset((char *)&simLock, 0, sizeof(struct SWI_STRUCT_SIM_EnableLock));

    simLock.sizeStruct = sizeof(struct SWI_STRUCT_SIM_EnableLock);
    simLock.bEnable = TRUE;
    slstrncpy(simLock.szPassword, "1234", sizeof("1234"));
 
    /* Fill up our data  */
/*    retcode = SwiSetSimLock( &simLock, timeout ); */

    if( retcode == SWI_RCODE_OK )
        printf ("SIM Enable command is being processed\n");
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
        printf ("SIM Enable command timed out\n");
    else if (retcode == 0xFFFF)
    {
        printf("SIM Enable command has been commented out\n");
        return;
    }
    else
    {
        printf ("SIM Enable command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\nTEST B: SUCCESSFUL disabling\n");
    slmemset((char *)&simLock, 0, sizeof(struct SWI_STRUCT_SIM_EnableLock));
    simLock.sizeStruct = sizeof(struct SWI_STRUCT_SIM_EnableLock);
    simLock.bEnable = FALSE;
    slstrncpy(simLock.szPassword, "1234", sizeof("1234"));
 
    /* Fill up our data  */
    retcode = SwiSetSimLock( &simLock, timeout );

    if( retcode == SWI_RCODE_OK )
        printf ("SIM Disable command is being processed\n");
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
        printf ("SIM Disable command timed out\n");
    else if (retcode == 0xFFFF)
    {
        printf("SIM Disable command has been commented out\n");
        return;
    }
    else
    {
        printf ("SIM Disable command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    printf("\nTEST C: Attempt without PIN\n");
    printf("   Should yield an invalid parameter error from the modem\n");
    slmemset((char *)&simLock, 0, sizeof(struct SWI_STRUCT_SIM_EnableLock));
    simLock.sizeStruct = sizeof(struct SWI_STRUCT_SIM_EnableLock);
    simLock.bEnable = TRUE;
 
    /* Fill up our data  */
    retcode = SwiSetSimLock( &simLock, timeout );

    if( retcode == SWI_RCODE_OK )
        printf ("SIM Enable command is being processed\n");
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
        printf ("SIM Enable command timed out\n");
    else if (retcode == 0xFFFF)
    {
        printf("SIM Enable command has been commented out\n");
        return;
    }
    else
    {
        printf ("SIM Enable command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
}
/**************
 *
 * Name:     doSwiGetSimLock
 *
 * Purpose:  Exercises the SwiGetSimLock API - calls the API and prints
 *           information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiGetSimLock(unsigned int timeout)
{
    SWI_RCODE retcode;
    struct SWI_STRUCT_SIM_LockStatus simLock;
    struct SWI_STRUCT_SIM_EnableLock simEnable;
    swi_char lasterror[80];
    
    /* First enable the lock */
    slmemset((char *)&simEnable, 0, sizeof(struct SWI_STRUCT_SIM_EnableLock));
    simEnable.sizeStruct = sizeof(struct SWI_STRUCT_SIM_EnableLock);
    simEnable.bEnable = TRUE;
    slstrncpy(simEnable.szPassword, "1234", sizeof("1234"));
    retcode = SwiSetSimLock( &simEnable, timeout );
    
    /* Get the SIM lock status from the device */
    simLock.sizeStruct = sizeof( struct SWI_STRUCT_SIM_LockStatus ) ;
    retcode = SwiGetSimLock( &simLock, timeout );

    printf ("\nTEST A: Successfully fetch the SIM lock state of an enabled SIM\n");

    if( retcode == SWI_RCODE_OK )
    {
        if(simLock.bEnabled)
        {
            printf("SIM Lock is enabled\n");
        }
        else
        {
            printf("SIM lock is disabled\n");
        }
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
        printf ("SIM Enable/Disable status request timed out\n");
    else
    {
        printf ("SIM Enable/Disable status request is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    
    /* Now disable the lock */
    slmemset((char *)&simEnable, 0, sizeof(struct SWI_STRUCT_SIM_EnableLock));
    simEnable.sizeStruct = sizeof(struct SWI_STRUCT_SIM_EnableLock);
    simEnable.bEnable = FALSE;
    slstrncpy(simEnable.szPassword, "1234", sizeof("1234"));
    retcode = SwiSetSimLock( &simEnable, timeout );
    
    /* Get the SIM Lock status from the device */
    simLock.sizeStruct = sizeof( struct SWI_STRUCT_SIM_LockStatus ) ;
    retcode = SwiGetSimLock( &simLock, timeout );

    printf ("\nTEST A: Successfully fetch the SIM lock state of an disabled SIM\n");

    if( retcode == SWI_RCODE_OK )
    {
        if(simLock.bEnabled)
        {
            printf("SIM Lock is enabled\n");
        }
        else
        {
            printf("SIM lock is disabled\n");
        }
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
        printf ("SIM Enable/Disable status request timed out\n");
    else
    {
        printf ("SIM Enable/Disable status request is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
}
/**************
 *
 * Name:     doSwiSetSimPassword
 *
 * Purpose:  Exercises the SwiSetSimPassword API - calls the API 
 *           and prints information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    The call to SwiSetSimPassword() is purposefully commented out
 *           since sending this request unknowningly from the test application
 *           could result in a blocked or rejected SIM. 
 *
 **************/
package void doSwiSetSimPassword(unsigned int timeout)
{
    SWI_RCODE retcode = 0xFFFF;
    struct SWI_STRUCT_SIM_Password simChange;
    struct SWI_STRUCT_SIM_EnableLock simEnable;
    swi_char lasterror[80];
    
    /* set up for notification */
    retcode = SwiNotify(SWI_NOTIFY_SimStatusExp, timeout);
    
    /* First enable the lock */
    slmemset((char *)&simEnable, 0, sizeof(struct SWI_STRUCT_SIM_EnableLock));
    simEnable.sizeStruct = sizeof(struct SWI_STRUCT_SIM_EnableLock);
    simEnable.bEnable = TRUE;
    slstrncpy(simEnable.szPassword, "1234", sizeof("1234"));
/*    retcode = SwiSetSimLock( &simEnable, timeout ); */
    
    
    printf ("\nTEST A: Successfully change PIN\n");
    
    slmemset((char *)&simChange, 0, sizeof(struct SWI_STRUCT_SIM_Password));
    simChange.sizeStruct = sizeof(struct SWI_STRUCT_SIM_Password);
    simChange.eType = SWI_SIMPASSWORD_CHV1;
    slstrncpy(simChange.szOldPassword, "1234", sizeof("1234"));
    slstrncpy(simChange.szNewPassword,"1234", sizeof("1234"));
 
    /* Fill up our data  */
    retcode = SwiSetSimPassword (&simChange, timeout);\

    if( retcode == SWI_RCODE_OK )
        printf ("SIM Change command is being processed\n");
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
        printf ("SIM Change command status request timed out\n");
    else if (retcode == 0xFFFF)
    {
        printf("SIM Change command has been commented out\n");
    }
    else 
    {
        printf ("SIM Change command is rejected  retcode[%d]\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    
    printf ("\nTEST B: Attempt to change PIN with a NULL old PIN buffer\n");
    
    slmemset((char *)&simChange, 0, sizeof(struct SWI_STRUCT_SIM_Password));
    simChange.sizeStruct = sizeof(struct SWI_STRUCT_SIM_Password);
    simChange.eType = SWI_SIMPASSWORD_CHV1;
    slstrncpy(simChange.szNewPassword,"1234", sizeof("1234"));
 
    /* Fill up our data  */
    retcode = SwiSetSimPassword (&simChange, timeout);\

    if( retcode == SWI_RCODE_OK )
        printf ("SIM Change command is being processed\n");
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
        printf ("SIM Change command status request timed out\n");
    else if (retcode == 0xFFFF)
    {
        printf("SIM Change command has been commented out\n");
    }
    else 
    {
        printf ("SIM Change command is rejected  retcode[%d]\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    
    printf ("\nTEST C: Attempt to change PIN with a NULL old PIN buffer\n");
    
    slmemset((char *)&simChange, 0, sizeof(struct SWI_STRUCT_SIM_Password));
    simChange.sizeStruct = sizeof(struct SWI_STRUCT_SIM_Password);
    simChange.eType = SWI_SIMPASSWORD_CHV1;
    slstrncpy(simChange.szOldPassword,"1234", sizeof("1234"));
 
    /* Fill up our data  */
    retcode = SwiSetSimPassword (&simChange, timeout);\

    if( retcode == SWI_RCODE_OK )
        printf ("SIM Change command is being processed\n");
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
        printf ("SIM Change command status request timed out\n");
    else if (retcode == 0xFFFF)
    {
        printf("SIM Change command has been commented out\n");
    }
    else 
    {
        printf ("SIM Change command is rejected  retcode[%d]\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
}
/**************
 *
 * Name:     doSwiChv2StatusKick
 *
 * Purpose:  Exercises the SwiChv2StatusKick API - calls the API 
 *           and prints information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiChv2StatusKick(unsigned int timeout)
{
    SWI_RCODE retcode;
    swi_char lasterror[80];
    
    /* set up for notification */
    retcode = SwiNotify(SWI_NOTIFY_SimStatusExp, timeout);

    /* Fill up our data  */
    retcode = SwiChv2StatusKick(SWI_TYPE_CHV2KICKTYPE_Change, timeout);

    printf ("\n");

    if( retcode == SWI_RCODE_OK )
        printf ("CHV2 kick request sent\n");
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
        printf ("CHV2 kick request timed out\n");
    else 
    {
        printf ("CHV2 kick request command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
}
/**************
 *
 * Name:     doSwiGetGsmIMSI
 *
 * Purpose:  Exercises the SwiGetGsmIMSI API - calls the API 
 *           and prints information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiGetGsmIMSI(unsigned int timeout)
{
    SWI_RCODE retcode;
    swi_char szMSIN[11]; 
    swi_uint32 MCC; 
    swi_uint32 MNC; 
    swi_char lasterror[80];

    /* Fill up our data  */
    printf("\nTEST A: Fetch IMSI\n");
    slmemset((char *)&szMSIN, 0, 11);
    retcode = SwiGetGsmIMSI(&szMSIN[0],11, &MCC, &MNC, timeout);

    if( retcode == SWI_RCODE_OK )
    {
        printf ("IMSI info\n");
        printf ("  MSIN: %s\n", (char *)&szMSIN[0]);
        printf ("  MCC: %x\n", (unsigned int)MCC);
        printf ("  MNC: %x\n", (unsigned int)MNC);
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
        printf ("IMSI request timed out\n");
    else if (retcode == SWI_RCODE_SMALL_BUF)
        printf("Buffer too small!\n");
    else 
    {
        printf ("IMSI kick request command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    
    printf("\nTEST B: Attempt to fetch IMSI with no buffer\n");
    slmemset((char *)&szMSIN, 0, 11);
    retcode = SwiGetGsmIMSI(NULL, 0, &MCC, &MNC, timeout);

    if( retcode == SWI_RCODE_OK )
    {
        printf ("IMSI info\n");
        printf ("  MSIN: %s\n", (char *)&szMSIN[0]);
        printf ("  MCC: %x\n", (unsigned int)MCC);
        printf ("  MNC: %x\n", (unsigned int)MNC);
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
        printf ("IMSI request timed out\n");
    else if (retcode == SWI_RCODE_SMALL_BUF)
        printf("Buffer too small!\n");
    else 
    {
        printf ("IMSI kick request command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    
    printf("\nTEST C: Attempt to fetch IMSI with no buffer\n");
    slmemset((char *)&szMSIN, 0, 11);
    retcode = SwiGetGsmIMSI(szMSIN, 4, &MCC, &MNC, timeout);

    if( retcode == SWI_RCODE_OK )
    {
        printf ("IMSI info\n");
        printf ("  MSIN: %s\n", (char *)&szMSIN[0]);
        printf ("  MCC: %x\n", (unsigned int)MCC);
        printf ("  MNC: %x\n", (unsigned int)MNC);
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
        printf ("IMSI request timed out\n");
    else if (retcode == SWI_RCODE_SMALL_BUF)
        printf("Buffer too small!\n");
    else 
    {
        printf ("IMSI kick request command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
}

/**************
 *
 * Name:     apSimCallback
 *
 * Purpose:  This function checks if a SIM card has been inserted into
 * 			 the modem
 *           
 * Parms:    sVariantp - pointer to the received notification info
 *
 * Return:   None
 *
 * Notes:    None
 * 
 **************/
package void apSimCallback( struct SwiNotifyVariant *sVariantp)
{
    printf("\nReceived notification:\n");
    status = sVariantp->_SimStatusExp.eStatus;
    waiting = FALSE;
}

/**************
 *
 * Name:     doSwiGetIccId
 *
 * Purpose:  Exercises the SwiGetIccId API - calls the API 
 *           and prints information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiGetIccId(unsigned int timeout)
{
    SWI_RCODE retcode;
    struct SWI_STRUCT_IccId IccId;
    char  iccid[21];
    swi_char lasterror[80];
    int i;

    apaddcbkfp((void *)apSimCallback);

    retcode = SwiNotify(SWI_NOTIFY_SimStatusExp, timeout);

    while(waiting) 
    {
        sleep(1);
    }

    switch( status )
    {
        case SWI_SIMSTATUSEXP_NotInserted:
        case SWI_SIMSTATUSEXP_Removed:
        case SWI_SIMSTATUSEXP_InitializeFailure:
        case SWI_SIMSTATUSEXP_GeneralFailure:
            printf("\nSIM Errror: %s", apsimstatusmap[status]);
            printf("\nPlease insert a Sim card and try again.");
            return;
            break;
    }
    /* Fill up our data  */
    retcode = SwiGetIccId(&IccId, timeout);

    printf ("\n");

    if( retcode == SWI_RCODE_OK )
    {
        for(i = 0; i< LENGTH_IccId; i++)
        {
            sluint8toasciihex(&iccid[2*i], IccId.nIccId[i]);
        }
        iccid[20] = 0;
        printf("ICC ID:\n");
        printf("  %s\n", &iccid[0]);
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
        printf ("ICCID request timed out\n");
    else 
    {
        printf ("ICCID kick request command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
}
/**************
 *
 * Name:     doSwiGetNetworkProviderID
 *
 * Purpose:  Exercises the SwiGetNetworkProvider API - calls the API 
 *           and prints information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiGetNetworkProviderID(unsigned int timeout)
{
    SWI_RCODE retcode;
    swi_char spn[17];
    swi_char lasterror[80];

    printf("\nTEST A: Fetch SPN from SIM\n");
    slmemset((char *)&spn, 0, 17);
    retcode = SwiGetNetworkProviderID(&spn[0], 17, timeout);
    if( retcode == SWI_RCODE_OK )
    {
        printf("SPN ID:\n");
        if(!slstrlen(&spn[0]))
            printf("  no SPN is present\n");
        else
            printf("  %s\n", &spn[0]);
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
        printf ("SPN request timed out\n");
    else if (retcode == SWI_RCODE_SMALL_BUF)
        printf("Buffer too small!\n");
    else 
    {
        printf ("SPN request command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    
    printf("\nTEST B: attempt to fetch SPN from SIM but provide no buffer\n");
    retcode = SwiGetNetworkProviderID(NULL, 0, timeout);
    slmemset((char *)&spn, 0, 17);
    if( retcode == SWI_RCODE_OK )
    {
        printf("SwiGetNetworkProviderID() returned successfully\n");
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
        printf ("SPN request timed out\n");
    else if (retcode == SWI_RCODE_SMALL_BUF)
        printf("Buffer too small!\n");
    else 
    {
        printf ("SPN request command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
    
    printf("\nTEST C: Fetch SPN from SIM with a small buffer\n");
    slmemset((char *)&spn, 0, 17);
    retcode = SwiGetNetworkProviderID(&spn[0], 4, timeout);
    if( retcode == SWI_RCODE_OK )
    {
        printf("SPN ID:\n");
        if(!slstrlen(&spn[0]))
            printf("  no SPN is present\n");
        else
            printf("  %s\n", &spn[0]);
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
        printf ("SPN request timed out\n");
    else 
    {
        printf ("SPN request command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
}

/**************
 *
 * Name:     doSwiSetSimAuthentication
 *
 * Purpose:  Exercises the SwiSetSimAuthentication API - calls the API
 *           and prints information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    
 *
 **************/
package void doSwiSetSimAuthentication(unsigned int timeout)
{
    SWI_RCODE retcode = 0xFFFF;
    struct SWI_STRUCT_SimAuth  SimAuth;
    swi_char lasterror[80];

    slmemset ((char *)(&SimAuth), 0, sizeof(struct SWI_STRUCT_SimAuth));  
    SimAuth.sizeStruct = sizeof(struct SWI_STRUCT_SimAuth);
    slstrncpy((char *)(SimAuth.RandNum), "123456789066666",
               sizeof("123456789066666"));

     /* Fill up our data  */
    retcode = SwiSetSimAuthentication( &SimAuth, timeout );

    printf ("\n");

    if( retcode == SWI_RCODE_OK )
    {
        printf ("SwiSetSimAuthentication command\n");
        printf ("   SIM response: %d\n", (int)SimAuth.SimResponse);
        printf ("   KC[0]: %d\n", SimAuth.Kc[0]);
        printf ("   KC[1]: %d\n", SimAuth.Kc[1]);
        printf ("   KC[2]: %d\n", SimAuth.Kc[2]);
        printf ("   KC[3]: %d\n", SimAuth.Kc[3]);
        printf ("   KC[4]: %d\n", SimAuth.Kc[4]);
        printf ("   KC[5]: %d\n", SimAuth.Kc[5]);
        printf ("   KC[6]: %d\n", SimAuth.Kc[6]);
        printf ("   KC[7]: %d\n", SimAuth.Kc[7]);
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
    {
        printf ("SwiSetSimAuthentication command timed out\n");
    }
    else if (retcode == 0xFFFF)
    {
        printf("SwiSetSimAuthentication command has been commented out\n");
    }
    else
    {
        printf ("SwiSetSimAuthentication command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }

}

/*
 * $Log: aptestgsmsim.c,v $
 */
