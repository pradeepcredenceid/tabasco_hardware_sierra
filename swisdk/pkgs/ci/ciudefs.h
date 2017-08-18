/*
 *  $Id: ciudefs.h,v 1.4 2010/06/09 23:34:14 epasheva Exp $
 *
 *  Filename: ciudefs.h
 *
 *  Purpose:  User definitions for the CI package
 *
 *  Copyright: © 2009 Sierra Wireless Inc., all rights reserved
 *
 */

#ifndef ciudefs_h
#define ciudefs_h

#include "ic/icudefs.h"
#include "us/usudefs.h"

/* Used by packages required to notify all connected applications */
#define CICNNOTIFYALL      0xFF

/* Maximum number of channels supported  */
#define CIMAXIPCCHAN       14
#define CIINVIPCCHAN       CIMAXIPCCHAN           /* used for error checking 
                                                   * This value cannot be used 
                                                   * for indexing our channel 
                                                   * array as it is out 
                                                   * of bounds.
                                                   */

/* Maximum number of CNRR & Notification channel-pairs supported  */
#define CIMAXCNRRPAIR      5

/* Maximum number of DR services supported  */
#define CIMAXDRSVC         3

/* IPC channels names */
#define CIIPCCNRR1      "CNRR1"
#define CIIPCCNRR2      "CNRR2"
#define CIIPCCNRR3      "CNRR3"
#define CIIPCCNRR4      "CNRR4"
#define CIIPCCNRR5      "CNRR5"
#define CIIPCNOTIF1     "NOTIF1"
#define CIIPCNOTIF2     "NOTIF2"
#define CIIPCNOTIF3     "NOTIF3"
#define CIIPCNOTIF4     "NOTIF4"
#define CIIPCNOTIF5     "NOTIF5"
#define CIIPCUI         "UIIPC"
#define CIIPCDIAG       "DIAG"
#define CIIPCUDIAG      "UDIAG"
#define CIIPCNMEA       "NMEA"
#define CIIPCUNMEA      "UNMEA"
#define CIIPCAT         "AT"
#define CIIPCUAT        "UAT"
/*
 * Name:    ciipcifusetyp
 *
 * Purpose: High-level usage indicator 
 *
 * Members: CIRR       - Request/Response channel
 *          CINOTIF    - Notification channel
 *          CIDRSVC    - Data Relay service channel
 *          CIUI       - User Interface channel
 *
 * Note:    
 */
enum ciipcifusetyp
{
    CIRR,
    CINOTIF,
    CIDRSVC,
    CIUI,
    CIUSETYPEMAX
};

/**
 * Name:   ciifacetype
 *
 * Purpose: Enumerated list of CI device interface types
 *
 * Members: CIIPC      - IPC channel
 *          CIUIF      - USB Interface
 *          CIIFINVALID - Invalid interface type
 *
 * Note:    
 */
enum ciifacetype
{
    CIIPC,
    CIUIF,
    CIIFINVALID
};

/*************
 *
 * Name:    ciipcchanfixed
 *
 * Purpose: Structure that implements a table of fixed channel information. 
 *             
 * Members: cichannamep     - Pointer to name of channel
 *          cipeerchannamep - Pointer to name of peer channel, eg, notification
 *          ciiftype        - Interface type - IPC or USB (currently)
 *          ciipcifuse      - Usage of IPC channel, ie, type of service
 *          ciuifuse        - Usage of USB channel, ie, type of service
 * 
 * Notes:   
 *
 **************/
struct ciipcchanfixed
{
    char           *cichannamep;
    char           *cipeerchannamep;
    enum ciifacetype    ciiftype;
    enum ciipcifusetyp  ciipcifuse;
    enum useptype       ciuifuse;
};

/*************
 *
 * Name:    ciipcchan
 *
 * Purpose: Structure that implements a table of all channel information. 
 *             
 * Members: cichanfixedinfop  - Pointer to fixed info portion
 *          cichannel         - Channel number 
 *          ciipccb           - Storage for IPC control block
 * 
 * Notes:   
 *
 **************/
struct ciipcchan
{
    struct ciipcchanfixed *cichanfixedinfop;
    swi_uint16 cichannel;
    struct icipccb ciipccb;
};

/* User prototypes */
#include "ci/ciuproto.h"

#endif

/*
 * $Log: ciudefs.h,v $
 */
