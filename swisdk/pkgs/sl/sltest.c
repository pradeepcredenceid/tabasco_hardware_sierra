/*************
 *    
 * $Id: sltest.c,v 1.8 2010/06/10 00:00:00 epasheva Exp $
 *    
 * Filename: sltest.c - Test the string library
 *
 * Purpose:  This file has test routines for the string
 *           library (excl. tests for the ARM250 routines)
 *
 * Copyright: © 2000 Sierra Wireless Inc.,  all rights reserved
 *
 **************/

/* include files */
#include "aa/aaglobal.h"
#include <string.h> 
#include <stdlib.h>
#include <stdio.h>
#include "sludefs.h"

/*
 *  Local constants and enumerated types.
 */

/*
 * local structures
 */

/* 
 * functions    
 */

/**********
 * Name: slatoitest
 *
 * Purpose: test slatoi() functionality    
 *
 * Parms:   None
 *                                     
 * Returns: None
 *
 * Abort:   None
 *
 * Notes:    
 *
 **********/
local void slatoitest(void)
{
    /* tell user what function is being tested */
    printf("\nTesting slatoi()!\n");

    /* slatoi tests */
    printf("12345678 is %u\n", slatoi("12345678"));
    printf("9ABCDEF0 is %u\n", slatoi("9ABCDEF0"));
    printf("00000001 is %u\n", slatoi("00000001"));
    printf("1 is %u\n", slatoi("1"));
    printf("3498ab is %u\n", slatoi("3498ab"));
    printf("000000001 is %u\n", slatoi("00000001"));
    printf("0 is %u\n", slatoi("0"));
    printf("-1 is %u\n", slatoi("-1"));
    printf("100000001 is %u\n", slatoi("100000001"));
    printf("65536 is %u\n", slatoi("65536"));
    printf("1234 is %u\n", slatoi("1234"));
    printf("4294967295 is %u\n", slatoi("4294967295"));
}

/**********
 * Name: slatoltest
 *
 * Purpose: test slatol() functionality    
 *
 * Parms:   None
 *                                     
 * Returns: None
 *
 * Abort:   None
 *
 * Notes:    
 *
 **********/
local void slatoltest(void)
{
    /* tell user what function is being tested */
    printf("\nTesting slatol()!\n");

    /* slatol tests */
    printf("12345678 is %lu\n", slatol("12345678"));
    printf("1234 is %lu\n", slatol("1234"));
    printf("34th is %lu\n", slatol("34th"));
    printf("9ABCDEF0 is %lu\n", slatol("9ABCDEF0"));
    printf("00000001 is %lu\n", slatol("00000001"));
    printf("1 is %lu\n", slatol("1"));
    printf("4294967296 is %lu\n", slatol("4294967296"));
    printf("4294967299 is %lu\n", slatol("4294967299"));
    printf("4294967295 is %lu\n", slatol("4294967295"));
    printf("0000000 is %lu\n", slatol("0000000"));
    printf("-1 is %lu\n", slatol("-1"));
    printf("100000001 is %lu\n", slatol("100000001"));
    printf("ABCDEFG01 is %lu\n", slatol("ABCDEFG01"));

}

/**********
 * Name: slatol64test
 *
 * Purpose: test slatol64() functionality    
 *
 * Parms:   None
 *                                     
 * Returns: None
 *
 * Abort:   None
 *
 * Notes:    
 *
 **********/
local void slatol64test(void)
{
    /* tell user what function is being tested */
    printf("\nTesting slatol64()!\n");

    /* slatol tests */
    printf("1234567887654321 is %Lu\n", slatol64("1234567887654321"));
    
    printf("12341234 is %Lu\n", slatol64("12341234"));
    printf("34th34th is %Lu\n", slatol64("34th34th"));
    printf("9ABCDEF01ABCDEF0 is %Lu\n", slatol64("9ABCDEF01ABCDEF0"));
    printf("0000000000000001 is %Lu\n", slatol64("0000000000000001"));
    printf("1 is %Lu\n", slatol64("1"));
    /* testing for max value 0xffffffffffffffff which is 18446744073709551615 */
    printf("18446744073709551615 is %Lu\n", slatol64("18446744073709551615"));
    printf("18446744073709551619 is %Lu\n", slatol64("18446744073709551619"));
    printf("18446744073709551613 is %Lu\n", slatol64("18446744073709551613"));
    printf("000000000000000 is %Lu\n", slatol64("000000000000000"));
    printf("-1 is %Lu\n", slatol64("-1"));
    printf("10000000000000001 is %Lu\n", slatol64("10000000000000001"));
    printf("ABCDEFG01ABCDEFG01 is %Lu\n", slatol64("ABCDEFG01ABCDEFG01"));
   

}


/**********
 * Name: slahtoltest
 *
 * Purpose: test slahtol() functionality    
 *
 * Parms:   None
 *                                     
 * Returns: None
 *
 * Abort:   None
 *
 * Notes:    
 *
 **********/
local void slahtoltest(void)
{
    /* tell user what function is being tested */
    printf("\nTesting slahtol()!\n");

    /* slahtol tests */
    printf("0x12345678 is hex for %lx\n", slahtol("12345678"));
    printf("0x9ABCDEF0 is hex for %lx\n", slahtol("9ABCDEF0"));
    printf("0x00000001 is hex for %lx\n", slahtol("00000001"));
    printf("0x1 is hex for %lx\n", slahtol("1"));
    printf("0x00000001 is hex for %lx\n", slahtol("00000001"));
    printf("0x0000000 is hex for %lx\n", slahtol("0000000"));
    printf("-1 is hex for %lx\n", slahtol("-1"));
    printf("0x100000001 is hex for %lx\n", slahtol("100000001"));
    printf("0xABCDEFG is hex for %lx\n", slahtol("ABCDEFG"));
    printf("0xabcdefg is hex for %lx\n", slahtol("abcdefg"));
}

/**********
 * Name: slmemcpyprint
 *
 * Purpose: prints out result of calling slmemcpy()    
 *
 * Parms:   destp - pointer to destination object
 *          srcp  - pointer to source object
 *          n     - number of bytes to copy
 *                                     
 * Returns:    None
 *
 * Abort:      None
 *
 * Notes:    
 *
 **********/
local void slmemcpyprint(char *destp, char *srcp, int n)
{
    int i;    /* loop counter */
    
    printf(", %d bytes copied\n", n);

    /* print out source array */
    printf("source = ");
    for(i = 0; i < n; i++)
        printf("%d, ", srcp[i]);

    /* print out dest array before data is copied from source to dest */
    printf("\nBefore slmemcpy destination = ");
    for(i = 0; i < n; i++)
        printf("%d, ", destp[i]);

    slmemcpy(destp, srcp, n);

    /* print out dest array after data is copied from source to dest */
    printf("\nAfter slmemcpy destination = ");
    for(i = 0; i < n; i++)
        printf("%d, ", destp[i]);

    /* format purpose */
    printf("\n");

}

/**********
 * Name: slmemcpytest
 *
 * Purpose: test slmemcpy() functionality    
 *
 * Parms:   None
 *                                     
 * Returns: None
 *
 * Abort:   None
 *
 * Notes:    
 *
 **********/
local void slmemcpytest(void)
{
    swi_uint8 dest[40];
    swi_uint8 src[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};

    swi_uint8 src2[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    swi_uint8 dest2[40];

    swi_uint8 *srcp;
    swi_uint8 *destp;    

    int n;    /* number of object to copy */

    printf("\nslmemcpy test!\n");

    /* set all the bytes in dest and dest2 to 0 */
    slmemset((char *)dest, 0, 40);
    slmemset((char *)dest2, 0 , 40);

    /* copying non-overlapping memory */
    destp = &dest[0];
    srcp = &src[0];
    n = 17;
    
    printf("non-overlapping memory with src address at %d, and dest address at %d",
           (int)srcp, (int)destp);
    slmemcpyprint((char *)destp, (char *)srcp, (int)n);

    /* copying non-overlapping memory */
    n = 17;
    
    printf("\non-overlapping memory with src address at %d, and dest address at %d",
            (int)src2, (int)dest2);
    slmemcpyprint((char *)dest2, (char *)src2, n);

    /* test overlapping case 1: address of dest smaller than src */
    destp = &dest[0];
    srcp = &dest[4];
    n = 9;

    printf("\ntest overlapping case 1: address of dest smaller than src");
    slmemcpyprint((char *)destp, (char *)srcp, n);
    
    /* test overlapping case 2: address of dest is larger than src */
    destp = &destp[6];
    srcp = &dest[5];
    n = 11;

    printf("\ntest overlapping case 2: address of dest is larger than src");
    slmemcpyprint((char *)destp, (char *)srcp, n);

    /** test overlapping case 3: address of dest is equal to src **/
    destp = &dest[0];
    srcp = &dest[0];
    n = 13;

    printf("\ntest overlapping case 3: address of dest is equal to src");
    slmemcpyprint((char *)destp, (char *)srcp, n);
}

/**********
 * Name: slstrlen
 *
 * Purpose: test slstrlen functionality    
 *
 * Parms:   None
 *                                     
 * Returns: None
 *
 * Abort:   None
 *
 * Notes:    
 *
 **********/
local void slstrlentest(void)
{    
    char teststring[] = "0123456789";
    char null_string = EOS;
    char *null_stringp = &null_string;

    printf("\ntesting slstrlen() !\n");

    /* print out length of "teststring" */
    printf("length of %s is %d\n", teststring, (int)(slstrlen(teststring)));    
    printf("length of %s is %d\n", null_stringp, (int)(slstrlen(null_stringp)));    
}

/**********
 * Name: slstrncpyprint
 *
 * Purpose: prints out result of calling slstrncpy()    
 *
 * Parms:   destp - pointer to destination object
 *          srcp - pointer to source object
 *          n - number of chars to copy
 *                                     
 * Returns: None
 *
 * Abort:   None
 *
 * Notes:    
 *
 **********/
local void slstrncpyprint(char *destp, char *srcp, int n)
{
    printf(", %d bytes copied\n", n);

    /* print out source array */
    printf("source = %s\n", srcp);
    
    /* print out dest array before data is copied from source to dest */
    printf("Before slstrncpy destination = %s\n", destp);
   
    slstrncpy(destp, srcp, n);

    /* print out dest array after data is copied from source to dest */
    printf("After slstrncpy destination = %s\n", destp);
}

/**********
 * Name: slstrncpytest
 *
 * Purpose: test slstrncpy() functionality    
 *
 * Parms:   None
 *                                     
 * Returns: None
 *
 * Abort:   None
 *
 * Notes:    
 *
 **********/
local void slstrncpytest(void)
{
    char dest[200] = "..........................";        
    char src[200] = "abcdefghijklmnopqrstuvwxyz";
    char null_string = EOS;

    char *destp;    
    char *srcp;
    int n;    /* number of characters to copy */

    printf("\nslstrncpy test!\n");
    
    /* copying non-overlapping string */
    destp = &dest[0];
    srcp = &src[0];
    n = 26;
    
    printf("\nnon-overlapping strings");
    slstrncpyprint(destp, srcp, n);
    
    /* test overlapping case 1: address of dest smaller than src */
    destp = &dest[0];
    srcp = &dest[4];
    n = 4;

    printf("\ntest overlapping case 1: address of dest smaller than src");
    slstrncpyprint(destp, srcp, n);

    /* test overlapping case 2: address of dest is larger than src */
    destp = &destp[6];
    srcp = &dest[4];
    n = 6;

    printf("\ntest overlapping case 2: address of dest is larger than src");
    slstrncpyprint(destp, srcp, n);

    /* test overlapping case 3: address of dest is equal to src */
    destp = &dest[0];
    srcp = &dest[0];
    n = 116;

    printf("\ntest overlapping case 3: address of dest is equal to src");
    slstrncpyprint(destp, srcp, n);

    /* copying null_string */
    destp = &dest[0];
    srcp = &null_string;
    n = 1;
    
    printf("\nnull string test");
    slstrncpyprint(destp, srcp, n);

}

/**********
 * Name: slstrncmptest
 *
 * Purpose: test slstrncmp() functionality    
 *
 * Parms:   None
 *                                     
 * Returns: None
 *
 * Abort:   None
 *
 * Notes:    
 *
 **********/
local void slstrncmptest(void)
{    
    char str1[80] = "One two three four eight sixteen"; 
    char str2[80] = "One two THREE fOur five six seven eight";
    char str3[] = "One two";
    int n;    /* loop counter */
    int x;

    printf("\nslstrncmp() test\n");
    
    /* comparison of a shorter string 1 versus string 2 */
    printf("string 1 = %s\nstring 2 = %s\n", str1, str2);
    
    for(n = 0; n < 45; n++)
    {        
        x = slstrncmp(str1, str2, n);
        printf("Comparing %d characters, slstrncmp() returns %d.\n", n, x);
    }

    /* comparison of a longer string 1  versus string 2 */
    printf("string 1 = %s\nstring 2 = %s\n", str2, str1);
    
    for(n = 0; n < 45; n++)
    {        
        x = slstrncmp(str2, str1, n);
        printf("Comparing %d characters, slstrncmp() returns %d.\n", n, x);
    }

    /* comparison at a null character */
    printf("string 1 = %s\nstring 2 = %s\n", str1, str3);

    for(n = 0; n < 45; n++)
    {        
        x = slstrncmp(str1, str3, n);
        printf("Comparing %d characters, slstrncmp() returns %d.\n", n, x);
    }
}

/**********
 * Name: slmemcmptest
 *
 * Purpose: test slstrncmp() functionality    
 *
 * Parms:   None
 *                                     
 * Returns: None
 *
 * Abort:   None
 *
 * Notes:    
 *
 **********/
local void slmemcmptest(void)
{    
    char str1[70] = "One two three four eight sixteen"; 
    char str2[70] = "One two THREE fOur five six seven";
    swi_uint8 array1[20] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 20 }; 
    swi_uint8 array2[20] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 19 };
    
    int n;    /* loop counter */
    int x;    /* temp buffer */

    printf("\nslmemcmp() test\n");
    
    /* comparing char strings */
    printf("string 1 = %s\nstring 2 = %s\n", str1, str2);
    
    for(n = 0; n < 40; n++)
    {    
        x = slmemcmp(str1, str2, n);
        printf("\nComparing %d characters, slmemcmp() returns %d.\n", n, x);
    }

    /* comparing int arrays */
    printf("array 1 = ");
    for(n = 0; n < 10; n++)
        printf("%d, ", array1[n]);

    printf("array 2 = ");
    for(n = 0; n < 10; n++)
        printf("%d, ", array2[n]);
    
    for(n = 0; n < 12; n++)
    {    
        x = slmemcmp(array1, array2, n);
        printf("\nComparing %d bytes, slmemcmp() returns %d.\n", n, x);
    }


}

/**********
 * Name: slstrcmptest
 *
 * Purpose: test slstrncmp() functionality    
 *
 * Parms:   None
 *                                     
 * Returns: None
 *
 * Abort:   None
 *
 * Notes:    
 *
 **********/
local void slstrcmptest(void)
{    
    char str1[80] = " One two three four eight sixteen"; 
    char str2[80] = " One two THREE fOur five six seven";
    char str3[] = "ONE"; 
    char str4[] = "one";
    int x;    /* temp buffer */

    printf("\nslstrcmp() test\n");
    
    /* comparing str1 & str2 */
    printf("string 1 = %s, string 2 = %s", str1, str2);
    x = slstrcmp(str1, str2);
    printf("\nComparing characters, slstrncmp() returns %d.\n", x);
    
    /* comparing str3 & str4 */
    printf("string 1 = %s, string 2 = %s", str3, str4);
    x = slstrcmp(str3, str4);
    printf("\nComparing characters, slstrncmp() returns %d.\n", x);
}    

/**********
 * Name: slstrtoktest
 *
 * Purpose: test if slstrtok() works properly    
 *
 * Parms:   None
 *                                     
 * Returns: None
 *
 * Abort:   None
 *
 * Notes:    
 *
 **********/
local void slstrtoktest(void)
{    
    char search_string[]="One Two Three Four Five"; // string to be tokenized
    struct slstrtokblk test;            // string token control block
    struct slstrtokblk *testp = &test;  // points to string token control block
    char *array[50];                    // array containing pointers to tokens
    int loop;                           // loop counter
    
    /* tell user what function is being tested */
    printf("\nTesting slstrtok()!\n");

    /* get the pointer to the first token */
    array[0] = slstrtok(search_string," ", testp);
    
    /* make sure that search_string contains characters */
    if(array[0] == NULL)
    {
        printf("No test to search.\n");
    }
    else
    {
        /* search for more tokens */
        for(loop=1;loop<50;loop++)
        {
            array[loop] = slstrtok(NULL," ", testp);
            if(array[loop]==NULL)
                break;
        }

        /* print out the tokens */
        for(loop=0;loop<50;loop++)
        {
            if(array[loop]==NULL)
            break;
            printf("Item #%d is %s.\n",loop,array[loop]);
        }
    }
}

    
/*************
 *    
 * Name: main 
 *    
 * Purpose: This function is the main entry point for the
 *          string library testing program
 *             
 * Parms:   argc - quantity of command line parameters (not used)
 *          argv - command line parameters (not used)
 *
 * Return:  0 
 *
 * Abort:   None (No intentional fatal errors).
 *
 * Notes:   None. 
 *
 **************/
int main(int argc, char* argv[])
{
    char *bufp;        /* buffer pointer for string manipulation tests */
    int i;            /* index offset into the allocated buffer */
    swi_uint16 ui16;    /* converted string->uint16 value */
    swi_uint32 ui32;    /* converted string->uint32 value */
 
    /* tell user what function is being tested */
    printf("Testing slmemset()!\n");

    /* get a buffer to be used for conversion tests */
    bufp = malloc(200);

    /* fill the first 10 bytes of the buffer with 0x30 ('0' character) */
    slmemset(bufp, 0x30, 10);

    /* print out the first 10 bytes of the buffer */
    printf("\nHere is memory set to 000000000: ");
    for (i=0;i<10;i++)
        printf("%c",bufp[i]);

    printf("\n");  /* print a carriage return & go on to the next test */

    /* tell user what function is being tested */
    printf("Testing sluint16toasciihex()!\n");

    /* pick unusal uint16's (0xA5F0, 0x89AB, 0) 
     * to convert to strings and display them 
     */
    printf("0xA5F0 =%s\n",sluint16toasciihex(bufp,0xA5F0));
    ui16=0x89AB;
    printf("0x89AB =%s\n",sluint16toasciihex(bufp,ui16));
    ui16=0x0;
    printf("0x0 =%s\n",sluint16toasciihex(bufp,ui16));

    /* tell user what function is being tested */
    printf("\nTesting sluint32toasciihex()!\n");

    /* pick an unusal uint32's to convert to strings and display them */
    printf("0x1234FEDC =%s\n",sluint32toasciihex(bufp,0x1234FEDC));
    ui32=0xFF;
    printf("0xFF =%s\n",sluint32toasciihex(bufp,ui32));

    /* sltoupper tests */
    printf("a in upper case is %c\n", sltoupper('a'));
    printf("n in upper case is %c\n", sltoupper('n'));
    printf("z in upper case is %c\n", sltoupper('z'));
    printf("A in upper case is %c\n", sltoupper('A'));
    printf("N in upper case is %c\n", sltoupper('N'));
    printf("Z in upper case is %c\n", sltoupper('Z'));
    printf("1 in upper case is %c\n", sltoupper('1'));
    printf("$ in upper case is %c\n", sltoupper('$'));
    printf("- in upper case is %c\n", sltoupper('-'));
    
    /* slahtol test */
    slahtoltest();

    /* slatol test */
    slatoltest();
    
    /* slatol64 test */
    slatol64test();

    /* slatoi test */
    slatoitest();

    /* slstrtok() test */
    slstrtoktest();

    /* slstrncpy() test */
    slstrncpytest();


    /* slmemcpy() test */
    slmemcpytest();

    /* slstrlen() test */
    slstrlentest();

    /* slstrncmp() test */
    slstrncmptest();

    /* slstrcmp() test */
    slstrcmptest();

    /* slmemcmp() test */
    slmemcmptest();

    /* release the buffer and exit */
    free(bufp); 

    return 0;
}

/*
 * $Log: sltest.c,v $
 */
