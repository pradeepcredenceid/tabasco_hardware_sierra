/**
 *
 * $Id: SwiIntPkGsmStk.c,v 1.2 2010/06/10 00:24:10 epasheva Exp $
 *
 * Filename: SwiIntPkGsmPhonebook.c
 *
 * Purpose:  Pack functions for outgoing requests for GSM APIs related to 
 *           SIM Toolkit.  
 *
 * Copyright: © 2009 Sierra Wireless, Inc. all rights reserved
 *
 */
 
/* include files */
#include "apidefs.h"
#include "SwiApiGsmStk.h"
 
 /**
 *
 * This function packs the Gsm SIM Toolkit Command from the 
 * user structure to the CnS  object's parameter field
 *
 * \param STKCommandp - user structure with unpacked SIM Toolkit command
 * \param paramfieldp - Pointer to storage into which the packed  
 *                      data will be placed by this function
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint16 SwiPkGsStkCmd( 
     struct SWI_STRUCT_STK_Command *STKCommandp, 
     swi_uint8 *paramfieldp)
 {
    swi_uint8   *localfieldp;           /* Local copy of paramfieldp */
    swi_uint8   texttype, selnum;
      
    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    /* Reference ProjectMemo036(GPRS Cns Object Definitions)
     * object: CNS_STK_CMD_REQ  0x1073 - 6 bytes
     */
    
    piput32(STKCommandp->nSTK_Handle, &localfieldp);
    
    texttype = (swi_uint8)STKCommandp->eTextType;
    piputmulti (&texttype, &localfieldp, 1);
    
    selnum = (swi_uint8)STKCommandp->nMenuSelectionNumber;
    piputmulti (&selnum,  &localfieldp, 1);
        
    /* Report how many bytes we've just written */
    return( localfieldp - paramfieldp ); 
 }
 
 /**
 *
 * This function packs the Gsm STK Command Response Get Input data from the 
 * user structure to the CnS  object's parameter field
 *
 * \param RespGetInputp - user structure with unpacked STK Command Response Get
 *                        Input data
 * \param paramfieldp - Pointer to storage into which the packed  
 *                      data will be placed by this function
 *
 * \return  The number of bytes written into the memory pointed to by
 *          paramfieldp
 *
 */
package swi_uint16 SwiPkGsStkRespGetInput( 
     const struct SWI_STRUCT_STK_RsepGetInput *RespGetInputp, 
     swi_uint8 *paramfieldp)
 {
    swi_uint8   *localfieldp;           /* Local copy of paramfieldp */
    swi_uint8   moredata, nUCS2char;
    swi_UCS2    tempucs2string[LENGTH_STKCommand]; /* temporary UCS2 string
                                                    * buffer
                                                    */
    swi_uint16  numconv;
    swi_bool    bresult;
    
    /* Make a local copy of the parameter field pointer */
    localfieldp = paramfieldp;

    /* Reference ProjectMemo036(GPRS Cns Object Definitions)
     * object: CNS_STK_RESP_GET_INPUT  0x1075 - 245 bytes
     */
    slmemset ((char *)localfieldp, 0, OBJ_CNS_STK_RESP_GET_INPUT_LENGTH);
    
    piput32(RespGetInputp->nSTK_Handle, &localfieldp);
    
    moredata = (swi_uint8)RespGetInputp->bMoreData;
    piputmulti (&moredata,  &localfieldp, 1);
    
    nUCS2char = (swi_uint8)RespGetInputp->cntWCHAR;
    piputmulti (&nUCS2char,  &localfieldp, 1);
    
    /* Convert the name to UCS2 and copy over */
    slmemset ((char *)&tempucs2string[0] ,0, sizeof (tempucs2string));
    bresult = slUtf8ToUcs2((unsigned char *)&(RespGetInputp->wText[0]), 
                            tempucs2string, 
                            LENGTH_STKCommand, 
                            &numconv);

    piput16s(tempucs2string, nUCS2char, &localfieldp);

    /* Adjust pointer to end of packet */
    localfieldp += (OBJ_CNS_STK_RESP_GET_INPUT_STRING_LENGTH - 
                    nUCS2char*sizeof (swi_UCS2));
    
    /* Report how many bytes we've written,compensate for NULL termination */
    return (localfieldp - paramfieldp + 1); 

 }

 
/*
 * $Log: SwiIntPkGsmStk.c,v $
 */

