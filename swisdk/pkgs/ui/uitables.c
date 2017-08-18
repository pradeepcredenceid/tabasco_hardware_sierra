/******************************************************************************
* $Id: uitables.c,v 1.1 2009/01/27 21:38:25 blee Exp $
*
* Filename - uitables.c
*
* Purpose  - ui command line menu tables and strings
*
* Copyright: C 2009 Sierra Wireless Inc.  All rights reserved
*
*******************************************************************************/

/* include files */
#include "ui/uiidefs.h"


/* command strings */

const local char uiDLStr[]      = "DL";
const local char uiDLOnStr[]    = "ON";
const local char uiDLOffStr[]   = "OFF";
const local char uiDLListStr[]  = "LIST";

/* help strings */
const local char uiNoHelp[] =         "";

const local char uiSubCmdHelp[] =     "[<subcmnd> [<params>]]";

const local char uiDLCtrlOnHelp[] =    "<package id> [<classmask (4-digit HEX)>]";
const local char uiDLCtrlOffHelp[] =   "<package id>";

/*
 *  entries in command tables must be in following format:
 *
 *  {ptr to cmd str, ptr to cmd fcn,  ptr to sub table, ptr to help str}
 *
 *  last entry in command table must be:
 *
 *    {NULL,           NULL,                 NULL,          NULL}
 *
 */

/* Debug Logger command table */
const local struct uiCmdTable uiDLTbl[] = {
    {uiDLOnStr,     uidlctrlon,         NULL,          uiDLCtrlOnHelp},
    {uiDLOffStr,    uidlctrloff,        NULL,          uiDLCtrlOffHelp},
    {uiDLListStr,   uidlctrllist,       NULL,          NULL},
    {NULL,          NULL,               NULL,          NULL}
};

/* main command table */
const local struct uiCmdTable uiMainTbl[] = {
    {uiDLStr,        NULL,                 uiDLTbl,       uiSubCmdHelp},
    {NULL,           NULL,                 NULL,          NULL}
};

/*************
*
* Name:      uiGetMainTblp - Get Main Table pointer
*
* Purpose:   return a pointer to the main command table
*             
* Parms:     none
*
* Return:    pointer to the main command table
*
* Abort:     none
*
* Notes:     none
*
**************/
package struct uiCmdTable const *uiGetMainTblp(
    void)
{
    return (uiMainTbl);
}

/*
* $Log: uitables.c,v $
*/
