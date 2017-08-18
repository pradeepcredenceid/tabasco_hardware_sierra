/*************
 *
 * Filename: firehose.c
 *
 * Purpose: firmware download task for fire hose protocol
 *
 * Copyright: 2017 Sierra Wireless Inc., all rights reserved
 *
 **************/

/*---------------
  include files
 ---------------*/
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>

#include "qa/inc/SwiDataTypes.h"

#include "sm/smudefs.h"
#include "ci/ciudefs.h"
#include "sl/sludefs.h"
#include "hd/hdudefs.h"
#include "pi/piudefs.h"
#include "mm/mmudefs.h"
#include "os/swi_ossdk.h"
#include "imcommon.h"
#include "imapi.h"
#include "imudefs.h"
#include "imidefs.h"
#include "qm/qmiproto.h"
#include "firehose.h"


#define DEBUG_ON 1
#define IMG_MASK_MDM (1<<0)
#define IMG_MASK_PRI (1<<1)
#define CHUNK_SIZE   (131072)

/* Image Management State machines */
local sm_t imst;

/* IM task control block */
local struct imtcb fhimtaskblk;
local swi_uint8    *fhimtaskevtmemfreep = NULL;

static const char xml_header[XML_HEADER_LENGTH] = "<\?xml version=\"1.0\" encoding=\"UTF-8\" \?>\n";
static const char xml_resp_ack[XML_HEADER_LENGTH] = "<response value=\"ACK\"";
static const char xml_resp_nak[XML_HEADER_LENGTH] = "<response value=\"NAK\"";


static char tx_buffer[FIREHOSE_TX_BUFFER_SIZE] = {0};
static char temp_buffer[512] = {0};
static swi_uint8 fw_file_buffer[FIREHOSE_TX_BUFFER_SIZE] = {0};
static swi_uint32 maxPayloadSizeToTargetInBytes = 16384; //16kB
local int g_sector_size = 2048;

/*************
 *
 * Name:    fhimgetcbp
 *
 * Purpose: Return a pointer to the Image Management task control block.
 *
 * Parms:   none
 *
 * Return:  pointer to task control block
 *
 * Abort:   none
 *
 * Notes:   None
 *
 **************/
package struct imtcb *
fhimgetcbp(void)
{
    return &fhimtaskblk;
}

/***********************
 *
 * Routines for firehose data dump for debugging
 *
 * 
 ***********************/


static void firehose_dump(swi_uint8 *imbuf,swi_uint32 len)
{
    struct imtcb *fhimtcbp = fhimgetcbp();
    char copybuf[1024];
    char printbuf[52];
    unsigned long i;
    unsigned long j=0;

    if (len > 1023) {
        len = 1023;
        dlLog0( &fhimtcbp->imdlcb,
               IMLOG_CLASSC,
               "data length truncated to 1023 bytes\n");
    }

    if (len > 0) {
        memcpy(copybuf,imbuf,len);
        copybuf[len]='\0';
    }
    else {
         dlLog0( &fhimtcbp->imdlcb,
               IMLOG_CLASSC,
               "data length is zero\n");
         return;
    }

    /* replace any \r, \n or \t */
    for (i=0; i<len; i++) {
        if (isspace(copybuf[i]))
            copybuf[i] = ' ';
    }

    for (i=0; i<len; i++) {
        printbuf[j] = copybuf[i];
        if (((i > 0) && (i % 50 == 0)) ||
            (i == len-1)) {
            j++;
            printbuf[j] ='\0';
            dlLog1( &fhimtcbp->imdlcb,
                    IMLOG_CLASSC,
                    "%s",
                    (swi_uint32)printbuf );
            j = 0;
        }
        else
            j++;
    }
}

static void print_saharapayload_hex(
   void *pBuffer,
   unsigned long bufSize )
{
    struct imtcb *fhimtcbp = fhimgetcbp();
    char *pPrintBuf;
    unsigned long pos;
    int status;
    char buf[52];
    unsigned long i;
    unsigned long j=0;

    if(bufSize == 0)
    {
        dlLog0( &fhimtcbp->imdlcb,
               IMLOG_CLASSC,
               "Sahara payload length is zero\n");
        return;
    }

    if(bufSize > 256)
    {
        bufSize = 256;
        dlLog0( &fhimtcbp->imdlcb,
               IMLOG_CLASSC,
               "Sahara payload length truncated to 256 bytes\n");
    }

    pPrintBuf = malloc( bufSize * 3 + 1);
    if (pPrintBuf == NULL)
    {
        dlLog0( &fhimtcbp->imdlcb,
               IMLOG_CLASSC,
               "Unable to allocate buffer for Sahara payload dump\n");
        return;
    }
    memset( pPrintBuf, 0 , bufSize * 3 + 1 );

    for (pos = 0; pos < bufSize; pos++)
    {
        status = snprintf( (pPrintBuf + (pos * 3)),
                         4,
                         "%02X ",
                         *(unsigned char *)(pBuffer + pos) );
        if (status != 3)
        {
            dlLog0( &fhimtcbp->imdlcb,
                   IMLOG_CLASSC,
                   "snprintf error\n");
            free( pPrintBuf );
            return;
        }
    }

    for (i=0; i<(bufSize * 3); i++) {
        buf[j] = pPrintBuf[i];
        if (((i > 0) && (i % 50 == 0)) ||
            (i == (bufSize * 3)-1)) {
            j++;
            buf[j] ='\0';
            dlLog1( &fhimtcbp->imdlcb,
                    IMLOG_CLASSC,
                    "%s",
                    (swi_uint32)buf );
            j = 0;
        }
        else
            j++;
    }

   free( pPrintBuf );
   pPrintBuf = NULL;
   return;
}

/*************
 *
 * Name:   fh iminit
 *
 * Purpose: Package initialization routine
 *
 * Parms:   none
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   Given a) the download task is terminated once a download completes
 *                   or fails and
 *                b) there is no mechanism for freeing the
 *                   memory reserved for a memory pool, the memory pool for the
 *                   IM task is created here to avoid a memory leak
 *                   for a session whereby multiple downloads occur during the
 *                   lifetime of a SLQS process
 *
 **************/
global void fhiminit(void)
{
    struct imtcb *fhimtcbp = fhimgetcbp(); /* Get the task control block ptr */

    /* Ensure the modem wait timer is disabled initially */
    fhimtcbp->immdmwaittmr = 0;

    /* invalidate firmware image file descriptor */
    fhimtcbp->imqdl.fp.osfiledescrip = -1;

    /* Create the Event Pool for receiving IM responses */
    mmpcreate( &fhimtcbp->imevtpool,
               "IMEVTPOOL",
               IMEVTPOOLSZ,
               sizeof(struct imevtblock) );

    /* create a memory pool for download buffer allocation */
    mmpcreate( &fhimtcbp->imdlpool,
               "IMDLPOOL",
               IMDLPOOLSZ,
               IMDLBUFSIZE + 64 ); /* we need extra 13B for ssdp_unframed_req_s header when
                                      block size equals IMDLBUFSIZE */

    /* create a memory pool for transmit buffer allocation */
    mmpcreate( &fhimtcbp->imtxpool,
               "IMTXPOOL",
               IMTXPOOLSZ,
               IMTXBUFSIZE );

    /* create a memory pool for receive buffer allocation */
    mmpcreate( &fhimtcbp->imrxpool,
               "IMRXPOOL",
               IMRXPOOLSZ,
               IMRXBUFSIZE );
}

/*************
 *
 * Name:    fhimrelevtbk
 *
 * Purpose: To release the resources associated with the IM event block
 *
 * Parms:   eventbkp - Pointer to the IM Event Block structure,
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   This function releases the resources associated with the
 *          IM event block as follows: first the memory buffer containing
 *          the newly received IM packet is released. Next the event buffer
 *          itself is released.
 *
 **************/
local void
fhimrelevtbk(
    struct imevtblock *eventbkp )
{
    /* If the caller's memory can be freed... */
    if( eventbkp->imevtmemfreep != NULL )
    {
        /* Free the IM message buffer first */
        mmbufrel( eventbkp->imevtmemfreep );
    }

    /* Free the event block itself */
    mmbufrel( (swi_uint8 *)eventbkp );
}

/*************
 *
 * Name:    fhtaskshutdown
 *
 * Purpose: Shuts down firehose task
 *
 * Parms:   imrxdatap - not used
 *
 * Return:  Does not return, the thread terminates
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
local void fhtaskshutdown( void *imrxdatap )
{
    struct imtcb *fhimtcbp = fhimgetcbp(); /* Get the task control block ptr */

    UNUSEDPARAM( imrxdatap );
    /* close the file */
    swi_ossdkfileclose(&fhimtcbp->imqdl.fp);

    /* Release the last consumed event block */
    if( NULL != fhimtaskevtmemfreep )
    {
        fhimrelevtbk( ( struct imevtblock * )fhimtaskevtmemfreep );
        fhimtaskevtmemfreep = NULL;
    }

    /* clear firmware download path */
    slmemset( fhimtcbp->imqdl.filepath,
              0,
              sizeof(fhimtcbp->imqdl.filepath) );

    dlLog0( &fhimtcbp->imdlcb,
            IMLOG_CLASSC,
           "Task terminated\n" );

    swi_ossdkthreadterminate();
}

local void initBufferWithXMLHeader (char *pMyBuffer, swi_uint32 Length)
{
    memset (pMyBuffer, 0, Length);
    memcpy (pMyBuffer, xml_header, XML_HEADER_LENGTH);
}

local void appendToBuffer (char *pMyBuffer, char *buf, swi_uint32 maxBufferSize)
{
    swi_uint16 spaceLeft, charsToAdd = 0;
    swi_uint16 Length;

    Length       = strlen ( (const char *) pMyBuffer);
    charsToAdd   = strlen ( (const char *) buf); // size of string to append

    spaceLeft = maxBufferSize - Length - XML_TAIL_LENGTH;      // tx_buffer is transmit_buffer of size FIREHOSE_TX_BUFFER_SIZE

    if (charsToAdd > spaceLeft)
    {
        return;  // too big to add this
    }

    slmemcpy ( (pMyBuffer + Length), buf, charsToAdd); // memcpy
    pMyBuffer[Length + charsToAdd] = '\0';
}

local void fhSetFile(
        struct imtcb* fhimtcbp,
        struct dirent *ep
        )
{
    slmemset ( fhimtcbp->imqdl.filename,
            0,
            sizeof (fhimtcbp->imqdl.filename) );
    slmemcpy ( fhimtcbp->imqdl.filename,
            ep->d_name,
            sizeof (ep->d_name) );
    slmemset ( fhimtcbp->imqdl.fullpathfilename,
            0,
            sizeof (fhimtcbp->imqdl.fullpathfilename) );
    strcat ( fhimtcbp->imqdl.fullpathfilename,
            fhimtcbp->imqdl.filepath );
    strcat ( fhimtcbp->imqdl.fullpathfilename,
            "/" );
    strcat ( fhimtcbp->imqdl.fullpathfilename,
            fhimtcbp->imqdl.filename );
}

/*************
 *
 * Name:    imgetcwefname
 *
 * Purpose: Looks for a CWE file
 *
 * Parms:   pathp   - base path to search in
 *
 * Return:  fw_image_type_e
 *
 * Abort:   none
 *
 * Notes:   full file name is set in task control block field if found
 *
 **************/
local  enum fw_image_type_e  getCweFname (char * pathp)
{
    DIR *dp;
    enum fw_image_type_e eftype = FW_IMG_TYPE_INVLD;
    struct imtcb      *fhimtcbp = fhimgetcbp(); /* Get the task control block ptr */

    dp = opendir (pathp);
    if (dp != NULL)
    {
        struct dirent *ep;
        while ((ep = readdir (dp))) /* assignment, not comparison */
        {
            eftype = imgetfiletype (ep->d_name);
            if (eftype == FW_IMG_TYPE_CWE)
            {
                /* For MDM 9x15, also check if cwe image was requested
                 * by device. i.e. bit 1 of bitmask should be set */
                if( fhimtcbp->imqdl.isMDM9x15 == TRUE )
                {
                    if( 0 != (fhimtcbp->imqdl.imgMask & IMG_MASK_MDM) )
                    {
                        /* Clear the bit mask for modem image( cwe file ) */
                        fhimtcbp->imqdl.imgMask &= ~IMG_MASK_MDM;
                    }
                    else
                    {
                        /* Found a cwe(modem image), but it wasn't requested */
                        continue;
                    }
                }
                /* For now, find the first CWE file and run with it */
                fhSetFile(fhimtcbp, ep);
                break;
           }
           else if (eftype == FW_IMG_TYPE_SPK )
           {
                /* for 9x15 spk case, always clear the bit mask as only one file needs to be downloaded */
                if( fhimtcbp->imqdl.isMDM9x15 == TRUE )
                {
                        fhimtcbp->imqdl.imgMask = 0x00;
                }
                /* treat it same as CWE file, find the first CWE file and run with it */
                fhSetFile(fhimtcbp, ep);
                break;
           }
           else if( eftype == FW_IMG_TYPE_NVU )
           {
               /* Check if it is a MDM 9x15 device */
               if( fhimtcbp->imqdl.isMDM9x15 == TRUE )
               {
                   if( 0 != (fhimtcbp->imqdl.imgMask & IMG_MASK_PRI) )
                   {
                       /* Clear the bit mask for PRI image( nvu files ) */
                       fhimtcbp->imqdl.imgMask &= ~IMG_MASK_PRI;
                       /* For now, find the first CWE file and run with it */
                       fhSetFile(fhimtcbp, ep);
                       break;

                   }
                   else
                   {
                       /* Not requested, so ignore */
                       continue;
                   }
               }
               else
               {
                   fhSetFile(fhimtcbp, ep);
                   break;

               }
           }
        }
        (void) closedir (dp);
    }
    else
    {   /* Special Handle With File Name */
        swi_char    fullpathfilename[IM_MAX_BUF_SIZE]={0};
        FILE*       rfp=NULL;
        memset(&fullpathfilename,0,IM_MAX_BUF_SIZE);
        snprintf(fullpathfilename,IM_MAX_BUF_SIZE-1,"%s",pathp);
        dlLog1( &fhimtcbp->imdlcb,
                IMLOG_CLASSC,
                "%s" ,(swi_uint32)fullpathfilename);
        rfp = fopen(fullpathfilename, "rb");
        if(rfp)
        {
            fclose(rfp);
            eftype = imgetfiletype (fullpathfilename);
            if (eftype == FW_IMG_TYPE_SPK )
            {
                /* for 9x15 spk case, always clear the bit mask as only one file needs to be downloaded */
                if( fhimtcbp->imqdl.isMDM9x15 == TRUE )
                {
                        fhimtcbp->imqdl.imgMask = 0x00;
                }
                strcpy(fhimtcbp->imqdl.filename,"");
                strcpy(fhimtcbp->imqdl.fullpathfilename,fullpathfilename);
            }
            else if (eftype == FW_IMG_TYPE_CWE )
            {
                /* for 9x15 spk case, always clear the bit mask as only one file needs to be downloaded */
                if( fhimtcbp->imqdl.isMDM9x15 == TRUE )
                {
                        fhimtcbp->imqdl.imgMask = 0x00;
                }
                /* treat it same as CWE file, find the first CWE file and run with it */
                strcpy(fhimtcbp->imqdl.fullpathfilename,fullpathfilename);
                strcpy(fhimtcbp->imqdl.filename,"");
           }
           else if( eftype == FW_IMG_TYPE_NVU )
           {
               /* Check if it is a MDM 9x15 device */
               if( fhimtcbp->imqdl.isMDM9x15 == TRUE )
               {
                   if( 0 != (fhimtcbp->imqdl.imgMask & IMG_MASK_PRI) )
                   {
                       /* Clear the bit mask for PRI image( nvu files ) */
                       fhimtcbp->imqdl.imgMask &= ~IMG_MASK_PRI;
                       /* For now, find the first CWE file and run with it */
                       strcpy(fhimtcbp->imqdl.fullpathfilename,fullpathfilename);
                       strcpy(fhimtcbp->imqdl.filename,"");
                   }
                   else
                   {
                       /* Not requested, so ignore */
                   }
               }
               else
               {
                   strcpy(fhimtcbp->imqdl.fullpathfilename,fullpathfilename);

               }
           }
        }
        else // Not a file
        {
            dlLog0( &fhimtcbp->imdlcb,
                IMLOG_CLASSC,
                "Couldn't open the directory" );
        }
    }

    return eftype;
}

/*************
 *
 * Name:    prepareFile
 *
 * Purpose: Get size info from download file
 *
 * Parms:   none
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:
 *
 **************/
local void prepareFile()
{
    struct imtcb *fhimtcbp = fhimgetcbp(); /* Get the task control block ptr */

    if( FW_IMG_TYPE_INVLD == getCweFname (fhimtcbp->imqdl.filepath) )
    {
        dlLog0( &fhimtcbp->imdlcb,
                IMLOG_CLASSC,
                "Invalid Image Type, shutting down FH task" );

        fhtaskshutdown(NULL);
    }

}

/*************
 *
 * Name:    sasendDummyPkt
 *
 * Purpose: Send a dummy packet to receive hello packet from modem .
 *
 * Parms:   none
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
local void sasendDummyPkt(void )
{
    struct imtcb      *fhimtcbp = fhimgetcbp(); /* Get the task control block ptr */

    swi_uint8 pDummyPkt[]= {0x55,0x54,0x55,0x56,
                            0x55,0x54,0x55,0x56,
                            0x55,0x54,0x55,0x56,
                            0x55,0x54,0x55,0x56,
                            0x55,0x54,0x55,0x56,
                            0x55,0x54,0x55,0x56
                            };

    imdssend(pDummyPkt, sizeof(pDummyPkt));

    dlLog1( &fhimtcbp->imdlcb,
           IMLOG_CLASSC,
           "Send Dummy Pkt, st:%i\n",
           (swi_uint32)fhimtcbp->imsmp->state);
    return;
}


/*************
 *
 * Name:    sasendHelloRsp
 *
 * Purpose: Send hello response to modem .
 *
 * Parms:   imrxdatap - pointer to response message from modem
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
local void sasendHelloRsp( void *imrxdatap )
{
    UNUSEDPARAM(imrxdatap);

    struct imtcb      *fhimtcbp = fhimgetcbp(); /* Get the task control block ptr */
    swi_uint8 pSaharaRsp[]={0x02,0x00,0x00,0x00,
                                                   0x30,0x00,0x00,0x00,
                                                   0x02,0x00,0x00,0x00,
                                                   0x02,0x00,0x00,0x00,
                                                   0x00,0x00,0x00,0x00,
                                                   0x03,0x00,0x00,0x00,
                                                   0x01,0x00,0x00,0x00,
                                                   0x02,0x00,0x00,0x00,
                                                   0x03,0x00,0x00,0x00,
                                                   0x04,0x00,0x00,0x00,
                                                   0x05,0x00,0x00,0x00,
                                                   0x06,0x00,0x00,0x00};

    imdssend(pSaharaRsp, sizeof(pSaharaRsp));

    dlLog1( &fhimtcbp->imdlcb,
           IMLOG_CLASSC,
           "Send Hello Response, st:%i\n",
           (swi_uint32)fhimtcbp->imsmp->state);

    /* We handle the command ready in the next state */
    return;
}


/*************
 *
 * Name:    sasendComExe
 *
 * Purpose: Send Command Execute command to modem to switch mode .
 *
 * Parms:   imrxdatap - pointer to response message from modem
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
local void sasendComExe( void *imrxdatap )
{
    UNUSEDPARAM(imrxdatap);

    struct imtcb      *fhimtcbp = fhimgetcbp(); /* Get the task control block ptr */
    swi_uint8 pCommandExe[]={0x0d,0x00,0x00,0x00,
                                                          0x0c,0x00,0x00,0x00,
                                                          0x00,0xff,0x00,0x00};


    imdssend(pCommandExe, sizeof(pCommandExe));

    dlLog1( &fhimtcbp->imdlcb,
           IMLOG_CLASSC,
           "Send Command Execute, st:%i\n",
           (swi_uint32)fhimtcbp->imsmp->state);

    return;
}

/*************
 *
 * Name:    sasendComExeData
 *
 * Purpose: Send Command Execute Data command to modem
 *
 * Parms:   imrxdatap - pointer to response message from modem
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
local void sasendComExeData( void *imrxdatap )
{
    UNUSEDPARAM(imrxdatap);

    struct imtcb      *fhimtcbp = fhimgetcbp(); /* Get the task control block ptr */
    swi_uint8 pCommandExeData[]={
                              0x0f,0x00,0x00,0x00,
                              0x0c,0x00,0x00,0x00,
                              0x00,0xff,0x00,0x00};

    imdssend(pCommandExeData, sizeof(pCommandExeData));

    dlLog1( &fhimtcbp->imdlcb,
           IMLOG_CLASSC,
           "Send Command Execute Data, st:%i\n",
           (swi_uint32)fhimtcbp->imsmp->state);

    return;
}

/*************
 *
 * Name:    fhsendConfig
 *
 * Purpose: Send Firehose Configure command to modem
 *
 * Parms:   imrxdatap - pointer to response message from modem
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
local void fhsendConfig( void *imrxdatap )
{
    UNUSEDPARAM(imrxdatap);
    struct imtcb      *fhimtcbp = fhimgetcbp(); /* Get the task control block ptr */
    swi_uint32 buf_length = 0;

    initBufferWithXMLHeader(tx_buffer, sizeof(tx_buffer));
    appendToBuffer (tx_buffer, "<data>\n", FIREHOSE_TX_BUFFER_SIZE);

    /* initialize temp_buffer */
    slmemset(temp_buffer, 0, sizeof(temp_buffer));
    /* construct the configure XML message */
    snprintf(temp_buffer, 512, "<configure MemoryName=\"eMMC\" Verbose=\"0\" AlwaysValidate=\"0\" MaxDigestTableSizeInBytes=\"8192\" \
 MaxPayloadSizeToTargetInBytes=\"%d\" ", maxPayloadSizeToTargetInBytes  );

    appendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);
    appendToBuffer (tx_buffer, "ZlpAwareHost=\"0\" SkipStorageInit=\"0\" TargetName=\"8960\" ", FIREHOSE_TX_BUFFER_SIZE);
    appendToBuffer (tx_buffer, "/>\n</data>\n", FIREHOSE_TX_BUFFER_SIZE); // HACK, the \n here is to match Zeno

    /* no XML file is ever an exact multiple of 512 bytes */
    buf_length = strlen(tx_buffer);
    if ( ((buf_length % 512) == 0) && (buf_length < FIREHOSE_TX_BUFFER_SIZE))
    {
        tx_buffer[buf_length+1] = '\n';
    }

    imdssend((swi_uint8*)tx_buffer, strlen(tx_buffer));

    dlLog1( &fhimtcbp->imdlcb,
           IMLOG_CLASSC,
           "Send configure XML, st:%i\n",
           (swi_uint32)fhimtcbp->imsmp->state);

    #ifdef DEBUG_ON
    dlLog1( &fhimtcbp->imdlcb,
           IMLOG_CLASSC,
           "Send Configure XML length[%lu], payload is:\n",
           (swi_uint32)strlen(tx_buffer));
    firehose_dump(tx_buffer,strlen(tx_buffer));
    #endif

    return;
}

/*************
 *
 * Name:    fhsendProgram
 *
 * Purpose: Send Firehose Program command to modem
 *
 * Parms:   imrxdatap - pointer to response message from modem
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
local void fhsendProgram( void *imrxdatap )
{
    UNUSEDPARAM(imrxdatap);
    struct imtcb      *fhimtcbp = fhimgetcbp(); /* Get the task control block ptr */
    swi_uint32 buf_length = 0;
    FILE *pFile;
    long lSize;
    swi_uint32 num_part_sectors = 0;

    /* obtain the firmware file from the provided path */
    prepareFile();

    pFile = fopen(fhimtcbp->imqdl.fullpathfilename, "rb");
    if ( pFile == NULL )
    {
         dlLog1( &fhimtcbp->imdlcb,
                 IMLOG_CLASSC,
                 "open firmware file failed!path:%s\n",
                 (swi_uint32)fhimtcbp->imqdl.fullpathfilename);
         return;
    }
    else
    {
        /* obtain file size */
        fseek(pFile, 0, SEEK_END);
        lSize = ftell(pFile);
        rewind(pFile);
        num_part_sectors = (lSize + g_sector_size -1) /g_sector_size;
        fclose(pFile);
    }

    initBufferWithXMLHeader(tx_buffer, sizeof(tx_buffer));
    appendToBuffer (tx_buffer, "<data>\n", FIREHOSE_TX_BUFFER_SIZE);

    /* initialize temp_buffer */
    slmemset(temp_buffer, 0, sizeof(temp_buffer));
    /* construct the program XML message */
    snprintf(temp_buffer, 512, "<program PAGES_PER_BLOCK=\"64\" SECTOR_SIZE_IN_BYTES=\"%d\" \
num_partition_sectors=\"%d\" filename=\"spkg.cwe\" physical_partition_number=\"0\" start_sector=\"-1\" ",
        g_sector_size, num_part_sectors);

    appendToBuffer(tx_buffer, temp_buffer, FIREHOSE_TX_BUFFER_SIZE);
    appendToBuffer (tx_buffer, "/>\n</data>\n", FIREHOSE_TX_BUFFER_SIZE); // HACK, the \n here is to match Zeno

    /* no XML file is ever an exact multiple of 512 bytes */
    buf_length = strlen(tx_buffer);
    if ( ((buf_length % 512) == 0) && (buf_length < FIREHOSE_TX_BUFFER_SIZE))
    {
        tx_buffer[buf_length+1] = '\n';
    }

    imdssend((swi_uint8*)tx_buffer, strlen(tx_buffer));

    dlLog1( &fhimtcbp->imdlcb,
           IMLOG_CLASSC,
           "Send program XML, st:%i\n",
           (swi_uint32)fhimtcbp->imsmp->state);

    #ifdef DEBUG_ON
    dlLog1( &fhimtcbp->imdlcb,
           IMLOG_CLASSC,
           "Send program XML length[%lu], payload is:\n",
           (swi_uint32)strlen(tx_buffer));
    firehose_dump(tx_buffer,strlen(tx_buffer));
    #endif

    return;
}

local void fhsendFile(void *imrxdatap )
{
    UNUSEDPARAM(imrxdatap);
    struct imtcb      *fhimtcbp = fhimgetcbp(); /* Get the task control block ptr */
    FILE *pFile;
    long lSize;
    swi_uint32 writeSize = 0;
    swi_uint32 result;

    pFile = fopen(fhimtcbp->imqdl.fullpathfilename, "rb");
    if ( pFile == NULL)
    {
        dlLog1( &fhimtcbp->imdlcb,
                 IMLOG_CLASSC,
                 "open firmware file failed!path:%s\n",
                 (swi_uint32)fhimtcbp->imqdl.fullpathfilename);
        return;
    }
    else
    {
        /* obtain file size */
        fseek(pFile, 0, SEEK_END);
        lSize = ftell(pFile);
        rewind(pFile);

        while ( (lSize - writeSize) > 0)
        {
            /* reset the write buffer */
            slmemset((char*)fw_file_buffer, 0, sizeof(fw_file_buffer));
            result = fread(fw_file_buffer, 1, CHUNK_SIZE, pFile);

            /* send the last trunk of data */
            if (result < CHUNK_SIZE)
            {
                imdssend(fw_file_buffer, CHUNK_SIZE);
#ifdef DEBUG_ON
                            dlLog0( &fhimtcbp->imdlcb,
                                    IMLOG_CLASSC,
                                    "last write");
#endif
                break;
            }
            else
            {
                imdssend(fw_file_buffer, result);
                writeSize+= result;
#ifdef DEBUG_ON
                dlLog2( &fhimtcbp->imdlcb,
                        IMLOG_CLASSC,
                        "numwrites: %ld, remainder: %ld",
                        writeSize, 
                        (lSize - writeSize));
#endif
             }
        }
        fclose(pFile);
    }

    return;
}

/*************
 *
 * Name:    fhsendReset
 *
 * Purpose: Send Firehose Reset command to modem
 *
 * Parms:   imrxdatap - pointer to response message from modem
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
local void fhsendReset( void *imrxdatap )
{
    UNUSEDPARAM(imrxdatap);
    struct imtcb      *fhimtcbp = fhimgetcbp(); /* Get the task control block ptr */
    swi_uint32 buf_length = 0;

    initBufferWithXMLHeader(tx_buffer, sizeof(tx_buffer));
    appendToBuffer (tx_buffer, "<data>\n", FIREHOSE_TX_BUFFER_SIZE);

    /* construct the RESET XML message */
    appendToBuffer (tx_buffer, "<power DelayInSeconds=\"0\" value=\"reset\" ", FIREHOSE_TX_BUFFER_SIZE);
    appendToBuffer (tx_buffer, "/>\n</data>\n", FIREHOSE_TX_BUFFER_SIZE);

    /* no XML file is ever an exact multiple of 512 bytes */
    buf_length = strlen(tx_buffer);
    if ( ((buf_length % 512) == 0) && (buf_length < FIREHOSE_TX_BUFFER_SIZE))
    {
        tx_buffer[buf_length+1] = '\n';
    }

    imdssend((swi_uint8*)tx_buffer, strlen(tx_buffer));

    dlLog1( &fhimtcbp->imdlcb,
           IMLOG_CLASSC,
           "Send power XML, st:%i\n",
           (swi_uint32)fhimtcbp->imsmp->state);

    /* set firmware download status to complete */
    qmSetFwDownloadStatus(eQCWWAN_ERR_SWIIM_FW_FLASH_COMPLETE);

    return;
}

//retrieve sector size via getStorageInfo
local void fhinfo( void *imrxdatap )
{
    UNUSEDPARAM(imrxdatap);
    struct imtcb      *fhimtcbp = fhimgetcbp(); /* Get the task control block ptr */
    strcpy(tx_buffer,
            "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\
            <data>\
            <getStorageInfo physical_partition_number=\"0\" />\
            </data>");
    imdssend((swi_uint8*)tx_buffer, strlen(tx_buffer));
    dlLog1( &fhimtcbp->imdlcb,
           IMLOG_CLASSC,
           "Send storage info XML, st:%i\n",
           (swi_uint32)fhimtcbp->imsmp->state);
    return;
}
local void fhdoneFinal( void *imrxdatap )
{
    UNUSEDPARAM( imrxdatap );
    struct imtcb      *fhimtcbp = fhimgetcbp(); /* Get the task control block ptr */

    dlLog1( &fhimtcbp->imdlcb,
           IMLOG_CLASSC,
           "done Final, st:%i\n",
           (swi_uint32)fhimtcbp->imsmp->state);

    /* terminate fhtask */
    fhtaskshutdown(NULL);

    return;
}


/*************
 *
 * Name:    fh_error
 *
 * Purpose: Does nothing, just log error.
 *
 * Parms:   none
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
local void fh_error( void *imrxdatap )
{
    struct imtcb *fhimtcbp = fhimgetcbp(); /* Get the task control block ptr */

    swi_uint32 err;
    swi_uint8 *errp = (swi_uint8 *)imrxdatap+1;

    err = piget32(&errp);

    dlLog3( &fhimtcbp->imdlcb,
           IMLOG_CLASSC,
           "error, st:%d, error:0x%08x\n",
           (swi_uint32)fhimtcbp->imsmp->state,
           (swi_uint32)err,
           (swi_uint32)NULL );

    fhtaskshutdown( NULL );

    return;
}

/* These tables located here to avoid forward declarations of above functions */
/*************
 *
 * Name:     im_stt
 *
 * Purpose:  Describes state transitions used by the im state machine
 *
 * Notes:
 *
  0stSA_IDLE
  1stSA_HELLO_RSP,
  2stSA_COM_EXE,
  3stSA_COM_EXE_DATA,
  4stFH_CONFIGURE,
  5stFH_PROGRAM,
  6stFH_RAW_DATA_TRF,
  7stFH_RESET,
    stFH_IGNORE,
    stFH_INVAL,
    stFH_MAX = stFH_INVAL
 *
 **************/
local swi_uint8 fh_stt[stFH_NUM_ACTIVE_STATES][eFH_MAX_EVENT] =
{
              /* evHello         evCommandRdy  evComExeRsp        evSwitchConf      evConfigRsp  evProgramRsp      evProcessRsp evResetRsp   error */
/* state 0 */   {stSA_HELLO_RSP, stFH_IGNORE,  stFH_IGNORE,       stFH_IGNORE,      stFH_IGNORE,      stFH_IGNORE, stFH_IGNORE,      stFH_IGNORE, stFH_IGNORE, stFH_INVAL},
/* state 1 */   {stFH_IGNORE,    stSA_COM_EXE, stFH_IGNORE,       stFH_IGNORE,      stFH_IGNORE,      stFH_IGNORE, stFH_IGNORE,      stFH_IGNORE, stFH_IGNORE, stFH_INVAL},
/* state 2 */   {stFH_IGNORE,    stFH_IGNORE,  stSA_COM_EXE_DATA, stFH_IGNORE,      stFH_IGNORE,      stFH_IGNORE, stFH_IGNORE,      stFH_IGNORE, stFH_IGNORE, stFH_INVAL},
/* state 3 */   {stFH_IGNORE,    stFH_IGNORE,  stFH_IGNORE,       stFH_INFO,        stFH_IGNORE,      stFH_IGNORE, stFH_IGNORE,      stFH_IGNORE, stFH_IGNORE, stFH_INVAL},
/* state 3 */   {stFH_IGNORE,    stFH_IGNORE,  stFH_IGNORE,       stFH_IGNORE,      stFH_CONFIGURE,   stFH_IGNORE, stFH_IGNORE,      stFH_IGNORE, stFH_IGNORE, stFH_INVAL},
/* state 4 */   {stFH_IGNORE,    stFH_IGNORE,  stFH_IGNORE,       stFH_IGNORE,      stFH_IGNORE,      stFH_PROGRAM,stFH_IGNORE,      stFH_IGNORE, stFH_IGNORE, stFH_INVAL},
/* state 5 */   {stFH_IGNORE,    stFH_IGNORE,  stFH_IGNORE,       stFH_IGNORE,      stFH_IGNORE,      stFH_PROGRAM,stFH_RAW_DATA_TRF,stFH_IGNORE, stFH_IGNORE, stFH_INVAL},
/* state 6 */   {stFH_IGNORE,    stFH_IGNORE,  stFH_IGNORE,       stFH_IGNORE,      stFH_IGNORE,      stFH_IGNORE, stFH_IGNORE,      stFH_RESET,  stFH_IGNORE, stFH_INVAL},
/* state 7 */   {stFH_IGNORE,    stFH_IGNORE,  stFH_IGNORE,       stFH_IGNORE,      stFH_IGNORE,      stFH_IGNORE, stFH_IGNORE,      stFH_IGNORE, stFH_DONE_FINAL, stFH_INVAL}
};

/*************
 *
 * Name:    im_sat
 *
 * Purpose:  set of functions executed upon entering a state
 *
 * Members:  one for each state
 *
 * Notes:
 *
 **************/
local void(*fh_sat[stFH_NUM_ACTIVE_STATES][eFH_MAX_EVENT])(void *) =
{
              /* evHello         evCommandRdy  evComExeRsp      evSwitchConf  evConfigRsp   evProgramRsp evProcessRsp evResetRsp error */
/* state 0 */   {sasendHelloRsp, fh_error,     fh_error,         fh_error,     fh_error,     fh_error,     fh_error,    fh_error,    fh_error, fh_error},
/* state 1 */   {fh_error,       sasendComExe, fh_error,         fh_error,    fh_error,     fh_error,     fh_error,    fh_error,    fh_error, fh_error},
/* state 2 */   {fh_error,       fh_error,     sasendComExeData, fh_error,    fh_error,     fh_error,     fh_error,    fh_error,    fh_error, fh_error},
/* state 3 */   {fh_error,       fh_error,     fh_error,         fhinfo,      fh_error,     fh_error,     fh_error,    fh_error,    fh_error, fh_error},
/* state 4 */   {fh_error,       fh_error,     fh_error,         fh_error,    fhsendConfig, fh_error,     fh_error,    fh_error,    fh_error, fh_error},
/* state 5 */   {fh_error,       fh_error,     fh_error,         fh_error,    fh_error,     fhsendProgram,fh_error,    fh_error,    fh_error, fh_error},
/* state 6 */   {fh_error,       fh_error,     fh_error,         fh_error,    fh_error,     fh_error,     fhsendFile,  fh_error,    fh_error, fh_error},
/* state 7 */   {fh_error,       fh_error,     fh_error,         fh_error,    fh_error,     fh_error,     fh_error,    fhsendReset, fh_error, fh_error},
/* state 8 */   {fh_error,       fh_error,     fh_error,         fh_error,    fh_error,     fh_error,     fh_error,    fh_error,    fhdoneFinal, fh_error},
/* state 9 */   {fh_error,       fh_error,     fh_error,         fh_error,    fh_error,     fh_error,     fh_error,    fh_error,    fh_error, fh_error}
};

local swi_uint8 parseSaharaCommandResp(swi_uint8 *pResp)
{
    enum fhsteventtype event = eSA_MIN;

    /* the first character of the message is the Command ID */
    switch (pResp[0])
    {
        case HELLO_REQUEST:
            event = eSA_HELLO;
            break;

        case COMMAND_READY:
            event = eSA_COMMAND_READY;
            break;

        case COMMAND_EXECUTE_RESPONSE:
            event = eSA_COMMAND_EXE_RSP;
            break;

        default:
            break;
    }

    return event;
}

local swi_uint8 parseFirehoseCommandResp(char *pResp)
{
    static enum fhsteventtype event = eSA_MIN;
    struct imtcb *fhimtcbp = fhimgetcbp();
    char *pch, *endptr;
    long ret = 0;

    /* Fire hose protocol uses XML message  */
    if ( strstr(pResp, "SWI supported functions: CWE"))
        event = eSA_SWITCH_CONFIRMED_RSP;
    else if ( strstr(pResp, xml_header))
    {
        if (strstr(pResp, "SWI CWE image processed OK"))
        {
            event = eFH_PROCESS_RSP;
        }
        //reply of getStorageInfo
        else if (strstr(pResp, "\"SECTOR_SIZE_IN_BYTES"))
        {
            event = eFH_INFO_RSP;
            pch = strstr(pResp, "SECTOR_SIZE_IN_BYTES");
            if (pch)
            {
                pch = strtok(pch, "=");
                pch = strtok(NULL, "=");
                if (pch)
                {
                    g_sector_size = strtol(pch, &endptr, 10);
                }
            }
        }
        else if (strstr(pResp, " Requested POWER_RESET"))
        {
            event = eFH_RESET_RSP;
            fhimtcbp->imsmp->state = stFH_RESET;
        }

        /* WP7603 replied this ERROR message after host transfered the whole firmware file,
          reset the state so that the Fire hose RESET command can be resent */
        else if (strstr(pResp, "ERROR: XML not formed correctly"))
        {
            event = eFH_PROCESS_RSP;
            fhimtcbp->imsmp->state = stFH_RAW_DATA_TRF;
        }
        else if (strstr(pResp, xml_resp_ack))
        {
            if ( strstr(pResp, "MaxPayloadSizeToTargetInBytes"))
                event = eFH_CONFIGURE_RSP;
            else if ( strstr(pResp, "rawmode=\"true\""))
                event = eFH_PROGRAM_RSP;
        }
        else if ( strstr(pResp, xml_resp_nak))
        {
            /* extract the max playload size the target supported */
            pch = strstr(pResp, "MaxPayloadSizeToTargetInBytes");

            if (pch)
            {
                /* locate the value after MaxPayloadSizeToTargetInBytes = " */
                pch = strtok(pch, "\"");
                /*chop the value string */
                pch = strtok(NULL, "\"");
                if (pch)
                {
                    /* convert the string to a long integer */
                    ret = strtol(pch, &endptr, 10);
                    maxPayloadSizeToTargetInBytes = ret;

                    /* reset the event and state so that the host can send the configure XML again */
                    event = eSA_SWITCH_CONFIRMED_RSP;
                    fhimtcbp->imsmp->state = stSA_COM_EXE_DATA;
                }
            }
        }
        else if ( strstr(pResp, " HANDLE_PROGRAM_FAILURE"))
        {
            dlLog0( &fhimtcbp->imdlcb,
                    IMLOG_CLASSC,
                    "HANDLE_PROGRAM_FAILURE\n");
            event = eFH_ERROR_RSP;
        }
        else if ( strstr(pResp, "SWI Error Code"))
        {
            dlLog0( &fhimtcbp->imdlcb,
                    IMLOG_CLASSC,
                    "SWI Error Code Detected\n");
            event = eFH_ERROR_RSP;
        }
    }

    return event;
}

/*************
 *
 * Name:    fhimmdmtimeout
 *
 * Purpose: Called if the immdmtmrwait timer expires, meaning that the
 *          modem has not responded to an outstanding QMI request
 *          in the time specified by the caller of the API. Send back
 *          a timeout indication to the API side
 *
 * Parms:   ipcchannel  - IPC channel to send response to
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
local void
fhimmdmtimeout(
    swi_uint32 ipcchannel )
{
    UNUSEDPARAM( ipcchannel );

    return;
}

/*************
 *
 * Name: fhimtick
 *
 * Purpose: Called once each time a timer tick message is received
 *          by the IM task. This function takes care of managing
 *          internal timers, decrementing them for each call. When
 *          the timer expires (reaches zero) the expiry function
 *          is called
 *
 * Parms:   none
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
local void
fhimtick(void)
{
    struct imtcb      *fhimtcbp = fhimgetcbp(); /* Get the task control block ptr */

    /* Decrement the timer and if it reaches zero as a result,
     * invoke the timeout function
     */
    if( fhimtcbp->immdmwaittmr )
    {
        if( --(fhimtcbp->immdmwaittmr) ==  0 )
        {
            /* Invoke the timer expiry function */
            fhimmdmtimeout(0);
        }
    }
}

/*************
 *
 * Name:    fhimqueuevt
 *
 * Purpose: Send a new IM Request message to the IM task
 *
 * Parms:   qevt        - event type
 *          imimsgp     - Pointer to the IM Request message
 *          memfreep    - Pointer to the memory release address for the inbound
 *                        message. IM task will manage the release of this
 *                        packet when done processing it.
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:
 *
 **************/
global void
fhimqueuevt(
    enum imeventtype qevt,
    swi_uint8 *immsgp,
    swi_uint32 immsglen,
    swi_uint8 *memfreep
)
{
    struct imevtblock *eventbkp;
    struct imtcb      *fhimtcbp = fhimgetcbp(); /* Get the task control block ptr */

    UNUSEDPARAM( memfreep );

    /* Allocate an event block */
    eventbkp = (struct imevtblock *) mmbufget( &fhimtcbp->imevtpool );

    if( eventbkp)
    {
        /* Stuff the fields with the required information */
        eventbkp->imeventtype = qevt;

        /* The IM information comes next */
        eventbkp->imevtdatap = immsgp;
        eventbkp->imevtdatalen = immsglen;

        /* Save the memory release pointer */
        eventbkp->imevtmemfreep = immsgp;

        /* Send the message on its way */
        icsmt_send( &fhimtcbp->imicmsgque,
                    (swi_uint32) eventbkp );
    }
    else
    {
        char errmsg[100];
        snprintf(errmsg, sizeof(errmsg),
                 "%s:%d no event blocks",
                 (char *)__func__, __LINE__);
        erAbort(errmsg, 0 );
    }
}

/*************
 *
 * Name:    fhtask
 *
 * Purpose: The Image Management task. This task coordinates the downloading of
 *          Firmware to the modem
 *
 * Parms:   paramp - pointer to data that may be passed in when this task
 *                   is started.
 *
 * Return:  exits when download is completed or errored out.
 *
 * Abort:   none
 *
 * Notes:
 *
 **************/
local void
fhtask(
    void *paramp)
{
    swi_bool          releaseevt; /* Some event blocks should not be released */
    swi_uint32         rmsg;       /* ptr to incoming IM response from modem */
    struct imevtblock *eventbkp;
    struct imtcb      *fhimtcbp = fhimgetcbp(); /* Get the task control block ptr */
    BYTE              event;
    BYTE              previous_event = 0;
    static int i;

    UNUSEDPARAM( paramp );
    i = 0;
    dlLog(  &fhimtcbp->imdlcb,
            IMLOG_CLASSC,
            "Task started\n",
           (swi_uint32)NULL,
           (swi_uint32)NULL,
           (swi_uint32)NULL,
           (swi_uint32)NULL );

    /* Initialize state tables */
    fhimtcbp->imsmp                = &imst;
    fhimtcbp->imsmp->sat           = *fh_sat;
    fhimtcbp->imsmp->stt           = *fh_stt;
    fhimtcbp->imsmp->evnum         = eFH_MAX_EVENT;
    fhimtcbp->imsmp->state         = stSA_IDLE;
    fhimtcbp->imsmp->stIgnore      = stFH_IGNORE;
    fhimtcbp->imsmp->stInvld       = stFH_INVAL;
    fhimtcbp->imqdl.remainder      = 0;
    fhimtcbp->imqdl.numwrites      = 0;;
    fhimtcbp->imqdl.sequenceNumber = 0;
    fhimtcbp->imqdl.multiImgProg   = FALSE;
    slmemset( fhimtcbp->imqdl.filename,
              0,
              sizeof(fhimtcbp->imqdl.filename) );
    slmemset( fhimtcbp->imqdl.fullpathfilename,
              0,
              sizeof(fhimtcbp->imqdl.fullpathfilename) );

    smvalidate( fhimtcbp->imsmp );

    /* 
     * Workaround as SDK misses first "hello" pkt from modem
     * Modem will be triggered to send duplicate "hello" pkt
     * by sending a dummy packet
     */
    sasendDummyPkt();

    /* Main line of processing begins next */
    for(;;)
    {
        /* Reset the release event block flag */
        releaseevt = TRUE;

        /* Wait for an incoming IPC message then dispatch it.
         * Timeout value is NULL to wait forever. We get our
         * ticks from the IC timer task.
         */
        icsmt_rcv( &fhimtcbp->imicmsgque,
                   IMNOTIMEOUT,
                   &rmsg );

        if(rmsg)
        {
            /* Cast the data to an event block pointer */
            eventbkp = (struct imevtblock *)rmsg;

            /* Dispatch to the handler for this type of message */
            switch( (enum fheventtype) eventbkp->imeventtype )
            {
                case e_SAHARA_RESP:
                    fhimtaskevtmemfreep = (swi_uint8 *)eventbkp;
                    fhimtcbp->imsmp->udata = eventbkp->imevtdatap;
                    event = parseSaharaCommandResp((swi_uint8*)fhimtcbp->imsmp->udata);

                    dlLog( &fhimtcbp->imdlcb,
                           IMLOG_CLASSC,
                           "Sahara Resp: st:%i, ev:%d\n",
                           (swi_uint32)fhimtcbp->imsmp->state,
                           (swi_uint32)event,
                           (swi_uint32)NULL,
                           (swi_uint32)NULL );

                    smrun ( fhimtcbp->imsmp, event );
                    break;

                case e_FIREHOSE_RESP:
                    fhimtaskevtmemfreep = (swi_uint8 *)eventbkp;
                    fhimtcbp->imsmp->udata = eventbkp->imevtdatap;
                    event = parseFirehoseCommandResp((char*)fhimtcbp->imsmp->udata);
                    if ( event != previous_event )
                    {
                        dlLog( &fhimtcbp->imdlcb,
                               IMLOG_CLASSC,
                               "Firehose Resp: st:%i, ev:%d\n",
                               (swi_uint32)fhimtcbp->imsmp->state,
                               (swi_uint32)event,
                               (swi_uint32)NULL,
                               (swi_uint32)NULL );

                        smrun ( fhimtcbp->imsmp, event );
                    }
                    /* modem will report so many error events "XML not formed correctly" after processing the cwe image,
                       this will cause that host keeps sending the power reset command which will cause SDK abortion on Marvell
                       hence, only send the power reset command from time to time */
                    else if ( strstr( 
                                (char*)fhimtcbp->imsmp->udata,
                                "XML not formed correctly"))
                    {
                        i++;
                        if ( i == 20 || i == 40 )
                        {
                            dlLog0( &fhimtcbp->imdlcb,
                                    IMLOG_CLASSC,
                                    "try again!\n");
                            smrun ( fhimtcbp->imsmp, event );
                        }
                    }
                    previous_event = event;
                    break;

                /* Terminate FH task */
                case e_TERMINATE:
                    fhtaskshutdown(NULL);
                    break;

                /* Periodic timer tick message */
                case e_TMRTICK:
                    fhimtick();
                    break;

                default:
                    {
                        char errmsg[100];
                        snprintf(errmsg, sizeof(errmsg),
                                 "%s:%d Invalid Event message",
                                 (char *)__func__, __LINE__);
                        erAbort(errmsg,
                                rmsg);
                    }
                    break;
            }

            /* Message has been handled, release the associated resources */
            if( releaseevt )
            {
                fhimrelevtbk( eventbkp );
                fhimtaskevtmemfreep = NULL;
            }
        }
        /* Do nothing if the message queue is empty or on timeout */
    }
}

/*************
 *
 * Name:    fhimrxpkt
 *
 * Purpose: Entry point in IM package for receiving a packet from the modem.
 *
 * Parms:   imbufp      - pointer to beginning of received message
 *          immsglen    - length of received message
 *          memrelp     - pointer to memory that needs to be released
 *          client      - not used
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   Runs in the context of a DS shell
 *
 **************/
local void fhimrxpkt(
    swi_uint8  *imbufp,
    swi_uint32 immsglen,
    swi_uint8  *memrelp,
    swi_uint16 client )
{
    struct imtcb      *fhimtcbp = fhimgetcbp(); /* Get the task control block ptr */
    swi_uint8* rxbufp;

    UNUSEDPARAM( client );


    /* store ds buffer pointer to release when finished download to prevent
     * mempool exhaustion for successive downloads
     */
    fhimtcbp->imdsmemrelp = memrelp;

    /* Copy to our own buffer since DS re-uses it's buffer */
    rxbufp = (swi_uint8 *) mmbufget( &fhimtcbp->imrxpool );
    
    dlLog1( &fhimtcbp->imdlcb,
           IMLOG_CLASSC,
           "Receive payload length[%lu]\n",
           (swi_uint32)immsglen );

    if (rxbufp == NULL)
    {
        dlLog0( &fhimtcbp->imdlcb,
               IMLOG_CLASSC,
               "Out of receive buffers\n");

        return;
    }
    slmemset( (char *)rxbufp, 0, IMRXBUFSIZE );
    slmemcpy( (char *)rxbufp, imbufp, immsglen );

    /* check if it is an XML response  or Sahara command response,
       the XML packet started with character '<' */
    if ( rxbufp[0] == '<')
    {
        #if DEBUG_ON
        dlLog0( &fhimtcbp->imdlcb,
           IMLOG_CLASSC,
           "firehose XML payload is:\n");

        firehose_dump(imbufp,immsglen);
        #endif
        
        /* Simply queue data on our own queue to run in our own context */
        fhimqueuevt( e_FIREHOSE_RESP,
                   rxbufp,
                   immsglen,
                   NULL);
    }
    else
    {
        #if DEBUG_ON
        dlLog0( &fhimtcbp->imdlcb,
           IMLOG_CLASSC,
           "sahara payload in hex:\n");

        print_saharapayload_hex(imbufp,immsglen);
        #endif

        /* Simply queue data on our own queue to run in our own context */
        fhimqueuevt( e_SAHARA_RESP,
                   rxbufp,
                   immsglen,
                   NULL);
    }
}

/*************
 *
 * Name:    imdssinitcb
 *
 * Purpose: IM task DS shell ready callback
 *
 * Parms:   none
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:
 *
 **************/
local void fhdssinitcb(swi_bool dsshellready)
{
    struct imtcb *fhimtcbp = fhimgetcbp();

    if( !dsshellready )
    {
        dlLog0( &fhimtcbp->imdlcb,
                IMLOG_CLASSC,
                "IM task DS shell failed to start" );

        fhtaskshutdown(NULL);
    }

    /* Create the IM task */
    swi_ossdkthreadcreate( fhtask,
                           NULL );
}

/*************
 *
 * Name:    fhdsstart
 *
 * Purpose: Instantiate DS interface for Firehose protocol download.
 *
 * Parms:   none
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:
 *
 **************/
local void
fhdsstart()
{
    struct imtcb *fhimtcbp = fhimgetcbp(); /* Get the task control block ptr */

    /* Initialize the SMT Message queue structure */
    icsmt_create( &fhimtcbp->imicmsgque, "FHIMMESSAGEQ", IMMSGDEPTH );

    /* Initialize  DS shell parameters */
    fhimtcbp->imdsshell.dsipcshellnamep = (swi_uint8 *)CIIPCUIMRR1;
    fhimtcbp->imdsshell.dsipccbfcnp     = fhimrxpkt;
    fhimtcbp->imdsshell.dssinitcbfp     = fhdssinitcb;

    /* Activate DS shell for sending SDP requests and reading responses */
    swi_ossdkthreadcreate( dslaunchshell,
                           &fhimtaskblk.imdsshell);
}

global void fhtaskinit(const char *pFwpath,
                       swi_uint32 isMDM9x15,
                       swi_uint16 imgMask )
{
    swi_uint32 vid,pid;

    /* do nothing if path is empty */
    if( 0 == slstrlen( pFwpath) )
    {
        return;
    }

    if( usvidpid(&vid, &pid) )
    {
        struct imtcb *fhimtcbp = fhimgetcbp();

        /* save firmware download path */
        slmemset( fhimtcbp->imqdl.filepath,
                  0,
                  sizeof(fhimtcbp->imqdl.filepath) );

        if( 0 != slstrlen(pFwpath) )
        {
            slmemcpy ( fhimtcbp->imqdl.filepath,
                       (swi_uint8 *)pFwpath,
                       slstrlen(pFwpath) );
        }

        fhimtcbp->imqdl.isMDM9x15 = isMDM9x15;
        fhimtcbp->imqdl.imgMask   = imgMask;

        switch(pid)
        {
            case SWI_QMIPID_WP76:
                /* Prepare for logging */
                if( !fhimtcbp->imdlcb.registered )
                {
                    dlregister( "FHIM", &fhimtcbp->imdlcb, TRUE );
                }

                /* create Firehose DS shell prior to starting the download*/
                fhdsstart();
                break;

            default:
                {
                    char errmsg[100];
                    snprintf(errmsg, sizeof(errmsg),
                             "%s:%d Invalid Product ID",
                             (char *)__func__, __LINE__);
                    erAbort(errmsg, pid);
                }
                break;
        }
    }

}
