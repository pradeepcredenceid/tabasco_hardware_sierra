/*************
 *
 * $Id: uidl.c,v 1.3 2010/06/09 23:54:02 epasheva Exp $
 *
 * Filename: uidl.c
 *
 * Purpose:  UI package DL associated functions
 *
 * Copyright: © 2009 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* include files */
#include "ui/uiidefs.h"

/* buffer to output messages */
swi_uint8    prtmsg[UIBUFSZ];


/**********
* Name:      uiprocuidlreq
*
* Purpose:   Dispatches the given UI command using the table-driven processor.
*             
* Params:    cmdp - pointer to the command string - must be NULL terminated
*
* Return:    TRUE - if command parsed successfully
*            FALSE - otherwise
*
* Notes:     The command line is converted to upper case before processing
*
* Abort:     None
*
***********/
package void uiprocuidlreq(swi_uint8 *cmdp)
{
    swi_uint8 *lcmdp = cmdp; /* local copy of pointer to command */
    swi_uint8 temp;
    
    /* convert command line to upper case - this may need to be re-visitef
     * for AT commands if lower case is a requirement for AT commands 
     */
    while (*lcmdp)
    {
        temp = sltoupper(*lcmdp);
       *lcmdp++ = temp;
    }

    /* table-driven, no retcodes to check, routines should output 
     * directly to the caller via IPC messages
     */
    uiProcCmd(uiGetMainTblp(), (char *)(cmdp));
}

/**********
* Name:      uidlctrlon
*
* Purpose:   Processes the UI command that configures debug logging for a
*            specified package.  User has chosen to turn logging on and  
*            optionally can define the classmask for that package.  If 
*            the classmask is not specified, then the classmask will 
*            default to log all debug messages for that package.
*             
* Params:    cmdp - pointer to the command string
*
* Return:    TRUE - if command parsed successfully
*            FALSE - otherwise
*
* Notes:     
*
* Abort:     None
*
***********/
package swi_bool uidlctrlon(char *cmdp)
{
    char delimit[] = " ";    /* delimitor */
    char *tokenp;
    struct dlusercb uidlusercb;
    swi_bool keepmask = FALSE;
    swi_uint8    pkgstr[10];
    struct uitaskcb   *uicbkp   = uigetcbp();
   
    /* a particular instance of the string token control block */
    struct slstrtokblk strtokblk;    

    /* Since this UI command is turning logging on, set the masterenable */
    uidlusercb.dlmasterenable = TRUE;

    /* Get the first token from command string */
    tokenp = uistrtok(cmdp, delimit, &strtokblk);

    if (!tokenp)
    {
        /* First token is NULL.  The user did not specify a package ID. */
        return (FALSE);
    }

    /* First token is the package ID */
    /* Get the package ID enumeration based on the string */
    uidlusercb.dlpkgid = dlpackidget(tokenp);

    if (uidlusercb.dlpkgid >= DLPID_MAX)
    {
        /* packid is DLPID_LAST and therefore invalid. */
        uiOutput("Error: package is not valid or unregistered for logging");
        return (FALSE);
    }
 
    slmemset ((char *)&pkgstr[0], 0, 10);
    slmemcpy (&pkgstr[0], tokenp, strlen(tokenp));
    
    /* Get the second token (classmask) from command string */
    tokenp = uistrtok(NULL, delimit, &strtokblk);

    if (tokenp == NULL)
    {
        /* The second token is NULL, i.e. the user did not specify the 
         * classmask.  Keep the present classmask. 
         */
        keepmask = TRUE;
        uidlusercb.dlclassmask = 0xFFFF;
    }
    else if (slstrlen(tokenp)>4)
    {
        /* classmask is more than a 4 digit hex */
        uiOutput("Error: Classmask is maximum 4 hex digits (no leading '0x' \
                  required)\r\n");
        return (FALSE);
    }
    else
    {
        /* Set classmask to this token value */
        uidlusercb.dlclassmask = (swi_uint16) slahtol(tokenp);
    }

    /* Setup the package's user block */

    if (!(dlusercbconfig(&uidlusercb, keepmask)))
    {
        /* Package is not registered with DL */
        dlLog( &uicbkp->uidlcb, UILOG_CLASSA,
               "Error: %s is not registered for logging!",
               (swi_uint32)&pkgstr[0],
               (swi_uint32)NULL,
               (swi_uint32)NULL,
               (swi_uint32)NULL);
        snprintf((char *)prtmsg, sizeof(prtmsg),
                "Error: %s is not registered for logging!", &pkgstr[0]);
        uiOutput ((char *)prtmsg);
        return (FALSE);
    }
    dlLog(  &uicbkp->uidlcb, UILOG_CLASSA,
            "%s logging turned ON, mask=0x%04x",
            (swi_uint32)&pkgstr[0],
            (swi_uint32)uidlusercb.dlclassmask,
            (swi_uint32)NULL,
            (swi_uint32)NULL);

    return (TRUE);
}

/**********
* Name:      uidlctrloff
*
* Purpose:   Processes the UI command that turns off debug logging for a
*            specified package.  The classmask will not be affected.
*             
* Params:    cmdp - pointer to the command string
*
* Return:    TRUE - if command parsed successfully
*            FALSE - otherwise
*
* Notes:     
*
* Abort:     None
*
***********/
package swi_bool uidlctrloff(char *cmdp)
{
    char delimit[] = " ";    /* delimitor */
    char *tokenp;
    struct dlusercb uidlusercb;
    /* Keep the current value of the classmask */
    swi_bool keepmask = TRUE;
    swi_uint8    pkgstr[10];
    struct uitaskcb   *uicbkp   = uigetcbp();

    /* a particular instance of the string token control block */
    struct slstrtokblk strtokblk;    

    /* Since this UI command is turning logging off, clear the masterenable */
    uidlusercb.dlmasterenable = FALSE;

    /* Get the first token from command string */
    tokenp = uistrtok(cmdp, delimit, &strtokblk);

    if (!tokenp)
    {
        /* First token is NULL.  The user did not specify a package ID. */
        return (FALSE);
    }

    /* First token is the package ID */
    /* Get the package ID enumeration based on the string */
    uidlusercb.dlpkgid = dlpackidget(tokenp);

    slmemset ((char *)&pkgstr[0], 0, 10);
    slmemcpy (&pkgstr[0], tokenp, strlen(tokenp));

    if (uidlusercb.dlpkgid >= DLPID_MAX)
    {
        /* packid is DLPID_LAST and therefore invalid. */
        uiOutput("Error: package ID is not valid.");
        return (FALSE);
    }
 
    /* Setup the package's user block */
    if (!(dlusercbconfig(&uidlusercb, keepmask)))
    {
        /* classmask is not 4 digit hex */
        /* Package is not registered with DL */
        dlLog(  &uicbkp->uidlcb, UILOG_CLASSA, 
                "Error: %s is not registered for logging!",
                (swi_uint32)&pkgstr[0],
                (swi_uint32)NULL,
                (swi_uint32)NULL,
                (swi_uint32)NULL);
        snprintf((char *)prtmsg, sizeof(prtmsg),
                "Error: %s is not registered for logging!", &pkgstr[0]);
        uiOutput ((char *)prtmsg);
        return (FALSE);
    }
    
    dlLog(  &uicbkp->uidlcb, UILOG_CLASSA,"%s logging turned OFF",
            (swi_uint32)&pkgstr[0],
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL);

    return (TRUE);
}

/**********
* Name:      uidlctrllist
*
* Purpose:   Output messages containing the available package name strings 
*            to the application.
*             
* Params:    cmdp - pointer to the command string
*
* Return:    TRUE - if command parsed successfully
*            FALSE - otherwise
*
* Notes:     
*
* Abort:     None
*
***********/
package swi_bool uidlctrllist(char *cmdp)
{
    char * pkgstrp;
    int i, pstrlen;
    
    uiOutput (" Available packages:\r\n");
    for (i = 0; i < DLPID_MAX; i++)
    {
        pkgstrp = (char *)dlpackidstrget(i);
        if (pkgstrp)
        {
            pstrlen = slvsprintflenc((char *)prtmsg, 
                                    "%s", 
                                    (char *)dlpackidstrget(i), 
                                    DLPKGNAMELEN_MAX, 1);
            uiOutput ((char *)prtmsg); 
        }
    }
    return TRUE;
}
/*
 * $Log: uidl.c,v $
 */

