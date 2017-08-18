/* -------------------------------------------------------------------------
 Copyright (C) 2014 Intel Mobile Communications GmbH

      Sec Class: Intel Confidential (IC)
 ----------------------------------------------------------------------
 Revision Information:
    $File name:  /msw_tools/FlashTool/libFlsTool/src/utilfunctions.c $
    $CC-Version: .../dhkristx_140314_v0/1 $
    $Date:       2014-04-16    12:20:52 UTC $
 ----------------------------------------------------------------------
 by CCCT (0.12l)
 ---------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include "utilfunctions.h"
#include <stdarg.h>

unsigned char as_errorcodes[NUM_ERROR_CODES][MAX_ERROR_CODE_LEN]=
{
	"AS_SUCCESS",
	"AS_FAILURE",
	"PKCS_BAD_DATA_LEN",
	"PKCS_BAD_HDR",
	"PKCS_BAD_BLK_TYPE",
	"PKCS_BAD_HDR_LEN",
	"SHA1_HASHING_ERROR",
	"RSA_ENCRYPTION_ERROR",
	"RSA_DECRYPTION_ERROR",
	"RSA_BAD_KEYMARKER",
	"RSA_BAD_PRIVATE_SIGNATURE",
	"MEM_ALLOC_FAILED"
};

/****************************************************************
*
*  FUNCTION:		AS_GetErrorString
*
*  PARAMETERS:        	String to which ERROR has to be copied,
*  			Error code
*
*  RETURNS:             void
*
*  DESCRIPTION:       	Copies the ERROR corrensponding to error_code to the string
*
*  CREATED:		01/11/2005 by Prasanth A G / wip_pag
*
*  MODIFIED:
*
*  REMARK:
*
****************************************************************/
void AS_GetErrorString(uint8_t *errorstring, uint8_t error_code)
{
  if(error_code < NUM_ERROR_CODES )
    strcpy((char*)errorstring,(char*)as_errorcodes[error_code]);
  else
    strcpy((char*)errorstring,UNKNOWN_ERROR);
}

/****************************************************************
*
*  FUNCTION:		copy_bytes
*
*  PARAMETERS:        	dest, src, length
*
*  RETURNS:             void
*
*  DESCRIPTION:       	copies 'len' num of bytes from src to dest
*
*  CREATED:		01/11/2005 by Prasanth A G / wip_pag
*
*  MODIFIED:
*
*  REMARK:
*
****************************************************************/
void copy_bytes(uint8_t* dest, uint8_t* src, int len)
{
	int i;
	for(i=0;i<len;i++)
  {
    dest[i] = src[i];
  }
}


/****************************************************************
*
*  FUNCTION:		printlong
*
*  PARAMETERS:        	name, var, length
*
*  RETURNS:             void
*
*  DESCRIPTION:       	prints 'len' number of 'unsigned long' from 'var'
*
*  CREATED:		01/11/2005 by Prasanth A G / wip_pag
*
*  MODIFIED:
*
*  REMARK:
*
****************************************************************/
void printlong(char *name, uint32_t *var, int len)
{
#ifdef DEBUG_PRINTS
	int i;
	AS_printf("%s: ",name);
	for(i=0;i<len;i++)
		AS_printf("%08x ",var[i]);
	AS_printf("\n");
#endif
}

/****************************************************************
*
*  FUNCTION:		printbytes
*
*  PARAMETERS:        	name, var, length
*
*  RETURNS:             void
*
*  DESCRIPTION:       	prints 'len' num of 'bytes' from 'var'
*
*  CREATED:		01/11/2005 by Prasanth A G / wip_pag
*
*  MODIFIED:
*
*  REMARK:
*
****************************************************************/
void printbytes(char *name, uint8_t* var, int len)
{
#ifdef DEBUG_PRINTS
	int i;

	AS_printf("%s: ",name);
	for(i=0;i<len;i++)
	{
		if(i%16 == 0) AS_printf("\n");
		AS_printf("%02x ",var[i]);
	}
	AS_printf("\n");
#endif
}

/****************************************************************
*
*  FUNCTION:		AS_printf
*
*  PARAMETERS:        	const char* format,
*
*  RETURNS:             void
*
*  DESCRIPTION:       	prints the char* to console
*
*  CREATED:		01/11/2005 by Prasanth A G / wip_pag
*
*  MODIFIED:
*
*  REMARK:
*
****************************************************************/
void AS_printf(const char* format, ...)
{
#ifdef DEBUG_PRINTS
	va_list ap;

	va_start(ap, format);

  	vprintf(format, ap);
	va_end(ap);
#endif
}

/****************************************************************
*
*  FUNCTION:		compare_long
*
*  PARAMETERS:        	A, B, length
*
*  RETURNS:             as_ret_type
*
*  DESCRIPTION:       	compares 'len' num of 'unsigned long' in A & B
*  			and returns AS_SUCCESS if they are same
*
*  CREATED:		01/11/2005 by Prasanth A G / wip_pag
*
*  MODIFIED:
*
*  REMARK:
*
****************************************************************/
as_ret_type compare_long(uint32_t *A, uint32_t *B, int len)
{
	int i;

	for(i=0;i<len;i++)
		if(A[i]!=B[i]) return AS_FAILURE;

	return AS_SUCCESS;
}
