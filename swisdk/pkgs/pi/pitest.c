/*
 *  $Id: pitest.c,v 1.3 2008/11/10 21:16:11 epasheva Exp $
 *
 *  pitest.c
 *
 * Purpose:   Contains functions to test the pi package on the 
 *            desktop
 *
 * Note:      None
 *
 */
#include <stdio.h>
#include "aa/aaglobal.h"
#include "pi/piudefs.h"

/*************
 *
 * Name:        piget64test
 *
 * Purpose:     test piget64() functionality 
 *             
 * Parms:       none
 *
 * Notes:       none
 *
 **************/
local swi_uint64 piget64test(void)
{
    swi_uint8 param[8];             /* 64-bit value holder */
    swi_uint8 *paramp = &param[0]; 
    swi_uint64 field;
    swi_uint32 i;
    
    /* Define the parameters for piget64() call */
    printf("64-bit value set:  ");    
    for (i=0; i< 8; i++)
    {
        param[i] = i;
        printf("(0x%x) ", param[i]);
    }
    printf("\n");
    
    /* Call piget64() with the defined parameters */
    field = piget64(&paramp);
    return (field);
}

/*************
 *
 * Name: main - main test funtion for the pi package
 *
 * Purpose:     This function writes a packet then read the created packet.
 *              The resulting buffer should be the same as the orginal buffer.
 *             
 * Members:     argc   - unused parameter
 *              argv   - unused parameter
 *
 * Notes:       none
 *
 **************/
int main(int argc, char* argv[])
{
    swi_uint8 buffer[16] = "ABCDEFGHIJKLMNO";    /* orginal buffer */
    swi_uint8 packet[16];                        /* created packet */
    swi_uint8 resultbuffer[16];                    /* result buffer */
    swi_uint8 *bufferp;                            /* moving buffer pointer */
    swi_uint8 *packetp;                            /* moving packet pointer */
    swi_uint32 *field32p;                        /* pointer to 32-bit field */
    swi_uint8 *field8p;                            /* pointer to 8-bit field */
    swi_uint16 *field16p;                        /* pointer to 16-bit field */
    swi_uint64 field64 = 0;                      /* 64-bit value holder */

    /* NULL terminate all buffers */
    buffer[15] = 0;            
    packet[15] = 0;
    resultbuffer[15] = 0;

    /* initialize buffer and packet pointers */
    bufferp = buffer;
    packetp = packet;

    /* write packet in network byte order */

    /* write a 32-bit field */
    field32p = (swi_uint32 *)bufferp;
    piput32(*field32p, &packetp);
    bufferp += sizeof(swi_uint32);

    /* write a 16-bit field */
    field16p = (swi_uint16 *)bufferp;
    piput16(*field16p, &packetp);
    bufferp += sizeof(swi_uint16);

    /* write three 8-bit fields */
    field8p = bufferp;
    piputmulti(field8p, &packetp, 3);
    bufferp += 3;

    /* write a 32-bit field */
    field32p = (swi_uint32 *)bufferp;
    piput32(*field32p, &packetp);
    bufferp += sizeof(swi_uint32);

    /* write a 16-bit field */
    field16p = (swi_uint16 *)bufferp;
    piput16(*field16p, &packetp);
    bufferp += sizeof(swi_uint16);

    /* read packet */

    /* reset buffer and packet pointers */
    packetp = packet;
    bufferp = resultbuffer;

    /* read a 32-bit field */
    field32p = (swi_uint32 *)bufferp;
    *field32p = piget32(&packetp);
    bufferp += sizeof(swi_uint32);

    /* read a 16-bit field */
    field16p = (swi_uint16 *)bufferp;
    *field16p = piget16(&packetp);
    bufferp += sizeof(swi_uint16);

    /* read three 8-bit fields */
    pigetmulti(&packetp, bufferp, 3);
    bufferp += 3;

    /* read a 32-bit field */
    field32p = (swi_uint32 *)bufferp;
    *field32p = piget32(&packetp);
    bufferp += sizeof(swi_uint32);

    /* read a 16-bit field */
    field16p = (swi_uint16 *)bufferp;
    *field16p = piget16(&packetp);
    bufferp += sizeof(swi_uint16);
    
    /* print test results */
    printf("orginal buffer  : %s\n", buffer);
    printf("packet          : %s\n", packet);
    printf("resulting buffer: %s\n", resultbuffer);
    
    /* testing piget64() */
    field64 = piget64test();
    printf("64-bit value read: (0x%016Lx)\n", field64);

    return 0;
}

/*
 * $Log: pitest.c,v $
 */
