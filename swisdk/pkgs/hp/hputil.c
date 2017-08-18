/*************
 *
 * $Id: hputil.c,v 1.5 2009/06/16 20:43:22 blee Exp $
 *
 * Filename: hputil.c
 *
 * Purpose:  This file contains procedures helpful to handling
 *           HIP packets.
 *
 * Copyright: © 2008 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* include files */
#include "hp/hpidefs.h"


/*************
*
* Name:     hpdesignator2msgid - convert msg designator to msg ids
*
* Purpose:  To convert the HIP message designator into the short message id
*           and extended message id.
*
* Parms:   (IN) msgdesignator - hp message designator
*          (OUT) shortmsgidp  - pointer to short message id
*          (OUT) extheaderp   - pointer to extended header flag
*          (OUT) extmsgidp    - pointer to extended message id
*
* Return:   None
*
* Abort:    None
*
* Notes:    None
*
**************/
package void hpdesignator2msgid(
    swi_uint32 msgdesignator, 
    swi_uint32 *shortmsgidp, 
    swi_bool *extheaderp, 
    swi_uint32 *extmsgidp)
{
  *shortmsgidp = (msgdesignator& HPMSGIDMSK);
  
  if((*shortmsgidp == HPMEXTENDED) || (*shortmsgidp == HPHEXTENDED))
  {
    *extheaderp= TRUE;
    *extmsgidp = (msgdesignator >> HPEXTMSGIDSHF) & HPEXTMSGIDMSK;
  }
  else
  {
    *extheaderp= FALSE;
    *extmsgidp = 0;
  }
}
/*************
*
* Name:     hpmsgid2designator - convert msg ids to a designator
*
* Purpose:  To convert the short message id and extended message id to the
*           HIP message designator.
*
* Parms:   (OUT) msgdesignatorip - pointer to the hp message designator
*          (IN) shortmsgid       - short message id
*          (IN) extmsgid         - extended message id
*
* Return:   None
*
* Abort:    None
*
* Notes:    None
*
**************/
package void hpmsgid2designator(
    swi_uint32 *msgdesignatorp, 
    swi_uint8 shortmsgid, 
    swi_uint16 extmsgid)
{
  if(shortmsgid == HPHEXTENDED) 
  {
    *msgdesignatorp= (extmsgid<<HPEXTMSGIDSHF)|shortmsgid;
  }
  else
  {
    *msgdesignatorp= shortmsgid;
  }
}

/*
 * $Log: hputil.c,v $
 */
