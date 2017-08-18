/**
 *
 * @ingroup swiim
 *
 * @file
 * image select fuction of image management 
 *
 * @author
 * Copyright: © 2014 Sierra Wireless, Inc.
 *            All rights reserved
 *
 */

/* include files */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <dirent.h>
#include <stdbool.h>

#include "SwiDataTypes.h"
#define  LOG_TAG  "swi_imgmgr"
#include "swiril_log.h"
#include "swiim_img.h" 
#include "qmerrno.h"
#include "swiqmitool_common.h"

extern struct ImageList devImgList;
extern bool ComputeImageIndex ( BYTE *imageIdx );

/**
 * Name:     DeleteDeviceImage
 *
 * Purpose:  List all the images present on the device and then delete the
 *           image selected by the user from the device.
 *
 * Return:   none
 *
 * Notes:    none
 */
bool DeleteDeviceImage(BYTE imageIndex)
{
    struct ImageIdElement *pDeletingImg = NULL;
    struct ImageElement imageInfo;
    BYTE   idx = 0;
    ULONG  imgElemSize = 0, resultCode = 0;
    BYTE   priImageIdx = 0;

    /* If we fail to list the device images, return */
    if( !ListDeviceImages(false) )
    {
        return false;
    }

    while(1)
    {
        /* Calculate the Image index corresponding the actual PRI Image list */
        if (!ComputeImageIndex( &imageIndex )) {
            return false;
        }

        /* Get the PRI image index from the Image list */
        if( !GetPRIImageIdx( &priImageIdx ) )
        {
            fprintf(stderr,"\tFAIL SAFE Image Active!!\n\n" );
            return false;
        }

        /* Store the pointer to PRI image list */
        pDeletingImg = &( devImgList.imageIDEntries[priImageIdx].\
                          imageIDElement[imageIndex] );

        /* Get the image element size */
        imgElemSize = sizeof( struct ImageIdElement );

        /* Retrieve the information about the image to be deleted */
        imageInfo.imageType = IMG_TYPE_PRI;
        for ( idx = 0; idx < 16; idx++ )
        {
            imageInfo.imageId[idx] = pDeletingImg->imageID[idx];
        }
        imageInfo.buildIdLength = pDeletingImg->buildIDLength;
        strcpy( imageInfo.buildId, pDeletingImg->buildID );

        /* Display the information about the deleting image */
        fprintf( stderr,"\nDeleting Image Info:\n"\
                        "\tImage Id:");
        for ( idx = 0; idx < 16; idx++ )
        {
            fprintf( stderr,"%.2x ", imageInfo.imageId[idx]);
        }
        fprintf( stderr,"\n\tBuild Id Length: %d\n", imageInfo.buildIdLength );
        fprintf( stderr,"\tBuild Id : %s\n\n", imageInfo.buildId );

        /* Delete the image from the device */
        resultCode = DeleteStoredImage( imgElemSize,
                                        (BYTE *)&imageInfo );

        if( resultCode != 0 )
        {
            fprintf( stderr, "Failed to delete the image\n"\
                             "Failure Code : %lu\n", resultCode );
            continue;
        }

        fprintf( stderr, "Retrieved Image list from the device\n");

        /* If we fail to list the device images, return */
        if( !ListDeviceImages(false) )
        {
            return false;
        }
        return true;
    }
}

