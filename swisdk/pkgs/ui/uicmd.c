/*************
 *
 * $Id: uicmd.c,v 1.2 2010/06/09 23:53:49 epasheva Exp $
 *
 * Filename: uicmd.c
 *
 * Purpose:  UI command line processing functions
 *
 * Copyright: © 2009 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* include files */
#include "ui/uiidefs.h"

/* Defines */
/*
 local static variables
*/
local char delimit[] = " ";  /* delimiters */

/*************
*
* Name:      uiProcCmdHelp
*
* Purpose:   Command Help processing function. 
*             
* Parms:     cTablep - pionter to the command table
*
* Return:    None
*
* Abort:     None
*
* Notes:     
*
**************/
local void uiProcCmdHelp(
    struct uiCmdTable const *cTablep)
{
    swi_uint32 i;
    swi_uint8 prtstr[128];
    
    /* Display all the help strings */
    uiOutput("Commands:\r\n");

    for(i=0; cTablep[i].cmdStrp != NULL; i++)
    {
        snprintf((char *)prtstr, sizeof(prtstr),
        "%s %s", cTablep[i].cmdStrp, 
        cTablep[i].cmdHelp);
        uiOutput ((char *)prtstr);
    }
    uiOutput("OK"); /* signal the application we're done outputting */
}

/*************
*
* Name:     uiProcCmd
*
* Purpose:  Table driven command processing function. 
*             
* Parms:    cTablep - pointer to the command table
*           cmdp    - pointer to command string
*
* Return:   None
*
* Abort:    None
*
* Notes:    The function will match on a partial command so long as
*           that command is uniquely identified     
*
*           This function is recursive for each nested level
*           of menus (can be changed to non-recursive if required)
*
**************/
package void uiProcCmd(struct uiCmdTable const *cTablep, char *cmdp)
{
    char *tokenp;
    char *restCmdTokenp;
    swi_uint32 cmdIndex;
    swi_uint32 cmdCount;
    swi_uint32 len;
    swi_uint32 i;
    struct slstrtokblk strtokblk; /* an instance of the string token control block */
    swi_uint8 prtstr[UIBUFSZ];

    /* if command string is a null character */
    if (cmdp[0] == EOS)
    {
        uiProcCmdHelp(cTablep);
        return;
    }

    // get the first token from msg. The command string should be the first token.
    tokenp = slstrtok(cmdp, delimit, &strtokblk);
    /* if the first character is a space then tokenp will be NULL */
    if (tokenp == NULL)
        return;

    len = slstrlen(tokenp);

    // get the rest of the cmd string
    restCmdTokenp = tokenp+len+1;

    // reset count and index
    cmdIndex=0;
    cmdCount=0;

    // loop through all commands
    for(i=0; cTablep[i].cmdStrp != NULL; i++)
    {
        // check if token matches a command
        if (slstrncmp(cTablep[i].cmdStrp, tokenp, len)==0)
        {
            // increment the match count
            cmdCount++;
            cmdIndex = i;
        }
    }

    /* if command uniquely identified */
    if (cmdCount == 1)
    {
        /* check if command has sub commands */
        if(cTablep[cmdIndex].SubTblp != NULL)
        {
            /* process the sub commands */
            uiProcCmd(cTablep[cmdIndex].SubTblp, restCmdTokenp);
        }
        else
        {
            /* call the command processing function */
            if( (cTablep[cmdIndex].cmdFp)(restCmdTokenp) )
            {
                uiOutput("OK"); /* signal we're done */
            }
            else
            {
                /* if command processing failed return help string for command*/
                uiOutput("Command syntax:\r\n");
                snprintf((char *)prtstr, sizeof(prtstr),
                         "%s %s\r\n", 
                         cTablep[cmdIndex].cmdStrp, cTablep[cmdIndex].cmdHelp);
                uiOutput ((char *)prtstr);
                uiOutput("OK"); /* signal done */
            }
        }
    }
    else
    {
        /* if no match or multiple matches print help */
        uiProcCmdHelp(cTablep);
    }
    return ;
}

/*************
*
* Name:      uistrtok - String token function
*
* Purpose:   Tokenize the string
*             
* Parms:     strTokenp      - pointer to string
*            strDelimitp    - pointer to string delimiter
*            tokblkp        - points to user defined storage for a particular 
*                               uistrtok() instance 
*
* Return:    Tokened string
*
* Abort:     If validation tests fail.
*
* Notes:     If tokp == NULL, uistrtok() continues tokenizing the string.
*
**************/
package char *uistrtok( 
    char *strTokenp, 
    const char *strDelimitp, 
    struct slstrtokblk *strtokblkp)
{
    /* use slstrtok function in String Library (SL) package */
    return(slstrtok(strTokenp, strDelimitp, strtokblkp));
}

/*
 * $Log: uicmd.c,v $
 */

