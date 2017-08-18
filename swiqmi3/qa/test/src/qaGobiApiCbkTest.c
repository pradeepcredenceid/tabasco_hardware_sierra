/**************
 *
 *  Filename:   qaGobiApiCbkTest.c
 *
 *  Purpose:    Gobi Callback API driver.
 *
 * Copyright: © 2011-2013 Sierra Wireless, Inc., all rights reserved
 *
 **************/

/* Linux definitions */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* include files */
#include "SwiDataTypes.h"
#include "qmerrno.h"
#include "qmudefs.h"
#include "qaGobiApiCbk.h"
#include "qatesthelper.h"
#include "qaCbkCatEventReportInd.h"
#include "qaCbkSwiOmaDmEventReportInd.h"
#include "qaCbkVoiceUssdInd.h"
#include "qaCbkVoiceUssdNoWaitInd.h"
#include "qatestproto.h"

#define OPTION_LENG 0x04

/*******************
  TEST FUNCTIONS
 *******************/
/*
 * Name:     doprintsysInfoCommonCB
 *
 * Purpose:  This is a wrapper for printing System Information that is common
 *
 * Parms:    fp          - pointer to FILE
 *           pSys        - pointer to sysInfoCommon
 *
 * Return:   None
 *
 * Notes:    None.
 *
 */
void doprintsysInfoCommonCB( FILE *fp, sysInfoCommon *pSys )
{
    FILE *fpTmp;
    fpTmp = fp;
    fprintf( fpTmp,"Service Domain Valid   : %x\n",pSys->srvDomainValid );
    fprintf( fpTmp,"Service Domain         : %x\n",pSys->srvDomain );
    fprintf( fpTmp,"Service Capability Valid: %x\n",pSys->srvCapabilityValid );
    fprintf( fpTmp,"Service Capability     : %x\n",pSys->srvCapability );
    fprintf( fpTmp,"Roam Status Valid      : %x\n",pSys->roamStatusValid );
    fprintf( fpTmp,"Roam Status            : %x\n",pSys->roamStatus );
    fprintf( fpTmp,"Forbidden System Valid : %x\n",pSys->isSysForbiddenValid );
    fprintf( fpTmp,"Is Forbidden System    : %x\n",pSys->isSysForbiddenValid );
}

/*************
 *
 * Name:    cbkTestSetSessionStateCB
 *
 * Purpose: SLQSSetSessionStateCallback API callback function
 *
 * Parms:   pSessionStateInfo    - session state information
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:
 *
 **************/
local void cbkTestSetSessionStateCB(slqsSessionStateInfo *pSessionStateInfo)
{
    FILE *fp;

    fp = tfopen("../../cbk/test/results/setslqssessionstatecallback.txt", "a");
    if( fp == NULL )
    {
        perror("cbkTestSetSessionStateCB");
        return;
    }

    fprintf( fp,
             "\nInterface Instance ID: %d\tService %d\tv4 SID %lu\tv6 SID %lu\n",
             pSessionStateInfo->pQmiInterfaceInfo->qaQmiinstanceid,
             pSessionStateInfo->pQmiInterfaceInfo->qaQmisvctype,
             pSessionStateInfo->pQmiInterfaceInfo->v4sessionId,
             pSessionStateInfo->pQmiInterfaceInfo->v6sessionId );
    fprintf( fp,
             "state: %lu\treconfiguration_required: %lu\tsessionEndReason %lu\n",
             pSessionStateInfo->state,
             pSessionStateInfo->reconfiguration_required,
             pSessionStateInfo->sessionEndReason );

    tfclose(fp);
}

/*************
 *
 * Name:    doSetSessionStateCallback
 *
 * Purpose: SetSessionStateCallback API driver
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
package void doSetSessionStateCallback( void )
{
    ULONG rc;

    rc = SLQSSetSessionStateCallback(&cbkTestSetSessionStateCB);

    if( rc != eQCWWAN_ERR_NONE )
    {
        fprintf( stderr,
                 "doSetSessionStateCallback: failed to enable callback\n" );
    }
    else
    {
        printf( "doSetSessionStateCallback: enable callbackcbk "\
                "(cbkTestSetSessionStateCB)\n" );
    }
}

/*************
 *
 * Name:    doClearSessionStateCallback
 *
 * Purpose: SetSessionStateCallback API driver
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
package void doClearSessionStateCallback( void )
{
    ULONG rc;

    rc = SLQSSetSessionStateCallback(NULL);

    if( rc != eQCWWAN_ERR_NONE )
    {
        fprintf( stderr,
                 "doClearSessionStateCallback: failed to disable callback\n" );
    }
    else
    {
        printf( "doClearSessionStateCallback: disable callback (NULL)\n" );
    }
}

/*************
 *
 * Name:    cbkTestdoSLQSSetWdsEventCB
 *
 * Purpose: SLQSSetWdsEventCallback API callback function
 *
 * Parms:   pWdsEventInfo  - WDS Event info
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:
 *
 **************/
local void cbkTestdoSLQSSetWdsEventCB( slqsWdsEventInfo *pWdsEventInfo )
{
    FILE *fp;

    fp = tfopen("../../cbk/test/results/setwdseventcallback.txt", "a");

    if ( fp == NULL )
    {
        perror("cbkTestdoSLQSSetWdsEventCB");
        return;
    }

    fprintf( fp,
             "\nInterface Instance ID: %d\tService %d\tv4 SID %lu\tv6 SID %lu\n",
             pWdsEventInfo->pQmiInterfaceInfo->qaQmiinstanceid,
             pWdsEventInfo->pQmiInterfaceInfo->qaQmisvctype,
             pWdsEventInfo->pQmiInterfaceInfo->v4sessionId,
             pWdsEventInfo->pQmiInterfaceInfo->v6sessionId );

    if (pWdsEventInfo->pDataBearer)
        fprintf(fp, "dataBearer: %lu\n", *(pWdsEventInfo->pDataBearer) );

    if (pWdsEventInfo->pDormancyStatus)
        fprintf(fp, "Dormancy Status: %lu\n", *(pWdsEventInfo->pDormancyStatus));

    if (pWdsEventInfo->pPacketsCountTX)
        fprintf(fp, "Transmitted Packets: %lu\n", *(pWdsEventInfo->pPacketsCountTX));

    if (pWdsEventInfo->pPacketsCountRX)
        fprintf(fp, "Received Packets:    %lu\n", *(pWdsEventInfo->pPacketsCountRX) );

    if (pWdsEventInfo->pTotalBytesTX)
        fprintf(fp, "Transmitted Bytes: %llu\n", *(pWdsEventInfo->pTotalBytesTX));

    if (pWdsEventInfo->pTotalBytesRX)
        fprintf(fp, "Received Bytes:    %llu\n", *(pWdsEventInfo->pTotalBytesRX) );

    tfclose(fp);
}

local void cbkTestdoSLQSSetQosEventCB(
        BYTE    instance,
        QosFlowInfo *pFlowInfo
        )
{
    FILE *fp;
    fp = tfopen("../../cbk/test/results/setQosEventCallback.txt", "a");

    fprintf(fp, "\n%s: QMI %d\n", __func__, instance);

    if (pFlowInfo->pQFlowState)
    {
        fprintf(fp, "\t id %lx, isNew %d, event %d \n",
                pFlowInfo->pQFlowState->id,
                pFlowInfo->pQFlowState->isNewFlow,
                pFlowInfo->pQFlowState->state
               );
    }

    if (pFlowInfo->pTxQFlowGranted)
    {
        fprintf(fp, "\t tx flow index %d\n",
                pFlowInfo->pTxQFlowGranted->index);

        if (pFlowInfo->pTxQFlowGranted->pDataRate)
        {
            fprintf(fp, "\t max %ld\n",
                    pFlowInfo->pTxQFlowGranted->pDataRate->dataRateMax);
            fprintf(fp, "\t guaranteedRate %ld\n",
                    pFlowInfo->pTxQFlowGranted->pDataRate->guaranteedRate);
        }

        if (pFlowInfo->pTxQFlowGranted->pLatency)
        {
            fprintf(fp, "\t latency %ld\n",
                    *pFlowInfo->pTxQFlowGranted->pLatency);
        }

        if (pFlowInfo->pTxQFlowGranted->pJitter)
        {
            fprintf(fp, "\t jitter %ld\n",
                    *pFlowInfo->pTxQFlowGranted->pJitter);
        }

        if (pFlowInfo->pTxQFlowGranted->pLteQci)
        {
            fprintf(fp, "\t qci %d\n",
                    *pFlowInfo->pTxQFlowGranted->pLteQci);
        }
    }

    tfclose(fp);
}

local void iSLQSSetQosEventCallback(
    BYTE            instance,
    tFNSLQSQOSEvent pCallback)
{
    ULONG rc;
    rc = SLQSSetQosEventCallback(instance, pCallback );

    if ( rc != eQCWWAN_ERR_NONE )
    {
        fprintf( stderr,
                 "%s: failed to enable callback\n", __func__ );
    }
    else
    {
        fprintf(stderr,
                "%s: callback enabled\n", __func__ );
    }
}

package void doSLQSSetQosFirstInstEventCallback( void )
{
    iSLQSSetQosEventCallback(0, &cbkTestdoSLQSSetQosEventCB);
}

package void doSLQSClearQosFirstInstEventCallback( void )
{
    iSLQSSetQosEventCallback(0, NULL);
}

package void doSLQSSetQosSecondInstEventCallback( void )
{
    iSLQSSetQosEventCallback(1, &cbkTestdoSLQSSetQosEventCB);
}

package void doSLQSClearQosSecondInstEventCallback( void )
{
    iSLQSSetQosEventCallback(1, NULL);
}

package void doSLQSSetQosThirdInstEventCallback( void )
{
    iSLQSSetQosEventCallback(2, &cbkTestdoSLQSSetQosEventCB);
}

package void doSLQSClearQosThirdInstEventCallback( void )
{
    iSLQSSetQosEventCallback(2, NULL);
}

/*************
 *
 * Name:    doSLQSSetWdsSecondInstEventCallback
 *
 * Purpose: SLQSSetWdsEventCallback API driver function
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
package void doSLQSSetWdsEventCallback( void )
{
    ULONG rc;
    CHAR  *pEndOfLine;
    CHAR  selOption[OPTION_LENG];
    ULONG userOption;
    BYTE  len;
    BYTE  instance = 0;

    while(1)
    {
        fprintf( stderr,
                 "\nPlease enter InstanceID value (  0 to 2 )  \n"\
                 "Option: " );

        /* Receive the input from the user */
        fgets( selOption, ( OPTION_LENG ), stdin );

        /*If only <ENTER> is pressed quit authentication params */
        if( ENTER_KEY == selOption[0])
        {
            fprintf( stderr,"Default Instance ID - 0 chosen !!\n\n");
        }

        /* If '/n' character is not read, there are more characters in input
         * stream. Clear the input stream.
         */
        pEndOfLine = strchr( selOption, ENTER_KEY );

        if( NULL == pEndOfLine )
        {
            FlushStdinStream();
        }
        len = strlen( selOption );

        /* Convert the option added by user into integer */
        selOption[ len - 1 ] = '\0';
        userOption = atoi( selOption );

        /* If user has entered an invalid input, prompt again */
        if( 2 < strlen(selOption) )
        {
            fprintf(stderr,"Please choose valid option from menu \n");
            continue;
        }
        instance = userOption;
        switch( userOption )
        {
            case 0:
                break;
            case 1:
                break;
            case 2:
                break;
            default:
                fprintf(stderr,"\nPlesae enter valid InstanceID\n");
               continue;
        }
        break;
    }

    rc = SLQSSetWdsEventCallback( &cbkTestdoSLQSSetWdsEventCB,
                                  10,
                                  instance );
    if ( rc != eQCWWAN_ERR_NONE )
    {
        fprintf( stderr,
                 "doSLQSSetWdsEventCallback: failed to enable callback\n" );
    }
    else
    {
        fprintf(stderr,
                "doSLQSSetWdsEventCallback: enable callback (cbkTestdoSLQSSetWdsEventCB, 10)\n" );
    }
}


/*************
 *
 * Name:    doClearSLQSWdsEventCallback
 *
 * Purpose: SLQSSetWdsEventCallback driver
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
package void doClearSLQSWdsEventCallback( void )
{
    ULONG rc;
    CHAR  *pEndOfLine;
    CHAR  selOption[OPTION_LENG];
    ULONG userOption;
    BYTE  len;
    BYTE  instance = 0;

    while(1)
    {
        fprintf( stderr,
                 "\nPlease enter InstanceID value (  0 to 2 )  \n"\
                 "Option: " );

        /* Receive the input from the user */
        fgets( selOption, ( OPTION_LENG ), stdin );

        /*If only <ENTER> is pressed quit authentication params */
        if( ENTER_KEY == selOption[0])
        {
            fprintf( stderr,"Default Instance ID - 0 chosen !!\n\n");
        }

        /* If '/n' character is not read, there are more characters in input
         * stream. Clear the input stream.
         */
        pEndOfLine = strchr( selOption, ENTER_KEY );

        if( NULL == pEndOfLine )
        {
            FlushStdinStream();
        }
        len = strlen( selOption );

        /* Convert the option added by user into integer */
        selOption[ len - 1 ] = '\0';
        userOption = atoi( selOption );

        /* If user has entered an invalid input, prompt again */
        if( 2 < strlen(selOption) )
        {
            fprintf(stderr,"Please choose valid option from menu \n");
            continue;
        }
        instance = userOption;
        switch( userOption )
        {
            case 0:
                break;
            case 1:
                break;
            case 2:
                break;
            default:
                fprintf(stderr,"\nPlesae enter valid InstanceID\n");
               continue;
        }
        break;
    }

    rc = SLQSSetWdsEventCallback( NULL,
                                  10,
                                  instance );
    if ( rc != eQCWWAN_ERR_NONE )
    {
        fprintf( stderr,
                 "doSLQSSetWdsEventCallback: failed to Clear callback\n" );
    }
    else
    {
        fprintf(stderr,
                "doSLQSSetWdsEventCallback: cleared callback \n" );
    }
}

/*************
 *
 * Name:    cbkTestSetPowerCB
 *
 * Purpose: SetPowerCallback API callback function
 *
 * Parms:   operatingMode - Operating Mode
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:
 *
 **************/
local void cbkTestSetPowerCB(
    ULONG operatingMode )
{
    FILE *fp;

    fp = tfopen("../../cbk/test/results/setpowercallback.txt", "a");

    if ( fp == NULL )
    {
        perror("cbkTestSetPowerCB");
        return;
    }

    fprintf(fp, "Operating Mode: %lu\n", operatingMode );
    tfclose(fp);
}

/*************
 *
 * Name:    doSetPowerCallback
 *
 * Purpose: SetPowerCallback API driver function
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
package void doSetPowerCallback( void )
{
    ULONG rc;

    rc = SetPowerCallback(&cbkTestSetPowerCB);

    if ( rc != eQCWWAN_ERR_NONE )
    {
        fprintf( stderr,
                    "doSetPowerCallback: failed to enable callback\n" );
    }
    else
    {
        fprintf( stderr, "doSetPowerCallback: enable callback (cbkTestSetPowerCB)\n" );
    }
}

/*************
 *
 * Name:    doClearPowerCallback
 *
 * Purpose: SetPowerCallback API driver
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
package void doClearPowerCallback( void )
{
    ULONG rc;

    rc = SetPowerCallback(NULL);

    if ( rc != eQCWWAN_ERR_NONE )
    {
        fprintf( stderr,
                    "doClearPowerCallback: failed to disable callback\n" );
    }
    else
    {
        fprintf( stderr, "doClearPowerCallback: disable callback (NULL)\n" );

   }
}

/*************
 *
 * Name:    cbkTestSetActivationStatusCB
 *
 * Purpose: SetActivationStatusCallback API callback function
 *
 * Parms:   activationStatus  - Activation Status
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:
 *
 **************/
local void cbkTestSetActivationStatusCB(
    ULONG activationStatus )
{
    FILE *fp;

    fp = tfopen("../../cbk/test/results/setactivationstatuscallback.txt", "a");

    if ( fp == NULL )
    {
        perror("cbkTestSetActivationStatusCB");
        return;
    }

    fprintf(fp, "Activation Status: %lu\n", activationStatus );
    tfclose(fp);
}

/*************
 *
 * Name:    doSetActivationStatusCallback
 *
 * Purpose: SetActivationStatusCallback API driver function
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
package void doSetActivationStatusCallback( void )
{
    ULONG rc;

    rc = SetActivationStatusCallback(&cbkTestSetActivationStatusCB);

    if ( rc != eQCWWAN_ERR_NONE )
    {
        fprintf( stderr,
                    "doSetActivationStatusCallback: failed to enable callback\n" );
    }
    else
    {
        fprintf( stderr, "doSetActivationStatusCallback: enable callback (cbkTestSetActivationStatusCB)\n" );
    }
}

/*************
 *
 * Name:    doClearActivationStatusCallback
 *
 * Purpose: SetActivationStatusCallback API driver
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
package void doClearActivationStatusCallback( void )
{
    ULONG rc;

    rc = SetActivationStatusCallback(NULL);

    if ( rc != eQCWWAN_ERR_NONE )
    {
        fprintf( stderr,
                    "doClearActivationStatusCallback: failed to disable callback\n" );
    }
    else
    {
        fprintf( stderr, "doClearActivationStatusCallback: disable callback (NULL)\n" );
    }
}

/*************
 *
 * Name:    cbkTestSetMobileIPStatusCB
 *
 * Purpose: SetMobileIPStatusCallback API callback function
 *
 * Parms:   dormancyStatus  - Dormancy Status
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:
 *
 **************/
local void cbkTestSetMobileIPStatusCB(
    ULONG dormancyStatus )
{
    FILE *fp;

    fp = tfopen("../../cbk/test/results/setmobileipstatuscallback.txt", "a");

    if ( fp == NULL )
    {
        perror("cbkTestSetMobileIPStatusCB");
        return;
    }

    fprintf(fp, "Dormancy Status: %lu\n", dormancyStatus );
    tfclose(fp);
}

/*************
 *
 * Name:    doSetMobileIPStatusCallback
 *
 * Purpose: SetMobileIPStatusCallback API driver function
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
package void doSetMobileIPStatusCallback( void )
{
    ULONG rc;

    rc = SetMobileIPStatusCallback(&cbkTestSetMobileIPStatusCB);

    if ( rc != eQCWWAN_ERR_NONE )
    {
        fprintf( stderr,
                    "doSetMobileIPStatusCallback: failed to enable callback\n" );
    }
    else
    {
        fprintf( stderr, "doSetMobileIPStatusCallback: enable callback (cbkTestSetMobileIPStatusCB)\n" );
    }
}

/*************
 *
 * Name:    doClearMobileIPStatusCallback
 *
 * Purpose: SetMobileIPStatusCallback API driver
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
package void doClearMobileIPStatusCallback( void )
{
    ULONG rc;

    rc = SetMobileIPStatusCallback(NULL);

    if ( rc != eQCWWAN_ERR_NONE )
    {
        fprintf( stderr,
                    "doClearMobileIPStatusCallback: failed to disable callback\n" );
    }
    else
    {
        fprintf( stderr, "doClearMobileIPStatusCallback: disable callback (NULL)\n" );
    }
}

/*************
 *
 * Name:    cbkTestSetRoamingIndicatorCB
 *
 * Purpose: SetRoamingIndicatorCallback API callback function
 *
 * Parms:   roaming - Roaming indicator
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:
 *
 **************/
local void cbkTestSetRoamingIndicatorCB(
    ULONG roaming )
{
    FILE *fp;

    fp = tfopen("../../cbk/test/results/setroamingindicatorcallback.txt", "a");
    if( fp == NULL )
    {
        perror("cbkTestSetRoamingIndicatorCB");
        return;
    }

    fprintf( fp,
             "roaming indicator: %lu\n", roaming );

    tfclose(fp);
}

/*************
 *
 * Name:    doSetRoamingIndicatorCallback
 *
 * Purpose: SetRoamingIndicatorCallback API driver
 *
 * Parms:   none
 *
 * Return:  none
 *
 *
 * Abort:   none
 *
 * Notes:
 *
 **************/
package void doSetRoamingIndicatorCallback( void )
{
    ULONG rc;

    rc = SetRoamingIndicatorCallback(&cbkTestSetRoamingIndicatorCB);

    if( rc != eQCWWAN_ERR_NONE )
    {
        fprintf( stderr,
                 "doSetRoamingIndicatorCallback: failed to enable callback\n" );
    }
    else
    {
        fprintf( stderr, "doSetRoamingIndicatorCallback: enable callbackcbk "\
                "(cbkTestSetRoamingIndicatorCB)\n" );
    }
}

/*************
 *
 * Name:    doClearRoamingIndicatorCallback
 *
 * Purpose: SetRoamingIndicatorCallback API driver
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
package void doClearRoamingIndicatorCallback( void )
{
    ULONG rc;

    rc = SetRoamingIndicatorCallback(NULL);

    if( rc != eQCWWAN_ERR_NONE )
    {
        fprintf( stderr,
                 "doClearRoamingIndicatorCallback: failed to disable callback\n" );
    }
    else
    {
        fprintf( stderr, "doClearRoamingIndicatorCallback: disable callback (NULL)\n" );
    }
}

/*************
 *
 * Name:    cbkTestSetDataCapabilitiesCB
 *
 * Purpose: SetDataCapabilitiesCallback API callback function
 *
 * Parms:   datacapslen - Data Capabilities Length
 *          datacaps    - Data Capabilities
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:
 *
 **************/
local void cbkTestSetDataCapabilitiesCB(
    BYTE datacapslen,
    BYTE *datacaps )
{
    FILE *fp;
    BYTE count = 0;
    fp = tfopen("../../cbk/test/results/setdatacapabilitiescallback.txt", "a");
    if( fp == NULL )
    {
        perror("cbkTestSetDataCapabilitiesCB");
        return;
    }

    fprintf( fp, "data capabilities length: %d\n", datacapslen );
    for ( count = 0; count < datacapslen; count++ )
        fprintf( fp, "data capabilities[%d]: %d\n", count, datacaps[count] );

    tfclose(fp);
}

/*************
 *
 * Name:    doSetDataCapabilitiesCallback
 *
 * Purpose: SetDataCapabilitiesCallback API driver
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
package void doSetDataCapabilitiesCallback( void )
{
    ULONG rc;

    rc = SetDataCapabilitiesCallback(&cbkTestSetDataCapabilitiesCB);

    if( rc != eQCWWAN_ERR_NONE )
    {
        fprintf( stderr,
                 "doSetDataCapabilitiesCallback: failed to enable callback\n" );
    }
    else
    {
        fprintf( stderr, "doSetDataCapabilitiesCallback: enable callbackcbk "\
                "(cbkTestSetDataCapabilitiesCB)\n" );
    }
}

/*************
 *
 * Name:    doClearDataCapabilitiesCallback
 *
 * Purpose: SetDataCapabilitiesCallback API driver
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
package void doClearDataCapabilitiesCallback( void )
{
    ULONG rc;

    rc = SetDataCapabilitiesCallback(NULL);

    if( rc != eQCWWAN_ERR_NONE )
    {
        fprintf( stderr,
                 "doClearDataCapabilitiesCallback: failed to disable callback\n" );
    }
    else
    {
        fprintf( stderr, "doClearDataCapabilitiesCallback: disable callback (NULL)\n" );
    }
}

/*************
 *
 * Name:    cbkTestSetSignalStrengthCB
 *
 * Purpose: SetSignalStrengthCallback API callback function
 *
 * Parms:   activationStatus  - Activation Status
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:
 *
 **************/
local void cbkTestSetSignalStrengthCB(
    INT8  signalStrength,
    ULONG radioInterface )
{
    FILE *fp;

    fp = tfopen("../../cbk/test/results/setsignalstrengthcallback.txt", "a");

    if ( fp == NULL )
    {
        perror("cbkTestSetSignalStrengthCB");
        return;
    }
    fprintf(fp, "Signal Strength: %d\n", signalStrength );
    fprintf(fp, "Radio Interface: %lu\n", radioInterface );
    tfclose(fp);
}

/*************
 *
 * Name:    doSetSignalStrengthCallback
 *
 * Purpose: SetSignalStrengthCallback API driver function
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
package void doSetSignalStrengthCallback( void )
{
    ULONG rc;
    BYTE thresholdsSize = 5;
    INT8 thresholds[5]  = {-100,-90,-80,-70,-60};

    rc = SetSignalStrengthCallback(&cbkTestSetSignalStrengthCB,
                                           thresholdsSize,thresholds);

    if ( rc != eQCWWAN_ERR_NONE )
    {
        fprintf( stderr,
                "doSetSignalStrengthCallback: failed to enable callback\n" );
    }
    else
    {
        fprintf( stderr, "doSetSignalStrengthCallback:\
        enable callback(cbkTestSetSignalStrengthCB)\n" );
    }
}

/*************
 *
 * Name:    doClearSignalStrengthCallback
 *
 * Purpose: SetSignalStrengthCallback API driver
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
package void doClearSignalStrengthCallback( void )
{
    ULONG rc;
    BYTE thresholdsSize = 0;
    INT8 thresholds[1]  = {0};

    rc = SetSignalStrengthCallback(NULL,thresholdsSize,thresholds);

    if ( rc != eQCWWAN_ERR_NONE )
    {
        fprintf( stderr,
                    "doClearSignalStrengthCallback: failed to disable callback\n" );
    }
    else
    {
        fprintf( stderr, "doClearSignalStrengthCallback: disable callback (NULL)\n" );
    }
}

/*************
 *
 * Name:    cbkTestSetRFInfoCallbackCB
 *
 * Purpose: SetRFInfoCallback API callback function
 *
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:
 *
 **************/
local void cbkTestSetRFInfoCallbackCB(
    ULONG radioInterface,
    ULONG activeBandClass,
    ULONG activeChannel )
{
    FILE *fp;

    fp = tfopen("../../cbk/test/results/setrfinfocallback.txt", "a");

    if ( fp == NULL )
    {
        perror("cbkTestSetRFInfoCB");
        return;
    }

    fprintf(fp, "Radio Interface: %lx\n", radioInterface );
    fprintf(fp, "Active Band Class: %lx\n", activeBandClass );
    fprintf(fp, "Active Channel: %lx\n", activeChannel );
    tfclose(fp);
}

/*************
 *
 * Name:    doSetRFInfoCallback
 *
 * Purpose: SetRFInfoCallback API driver function
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
package void doSetRFInfoCallback( void )
{
    ULONG rc;

    rc = SetRFInfoCallback(&cbkTestSetRFInfoCallbackCB );

    if ( rc != eQCWWAN_ERR_NONE )
    {
        fprintf( stderr,
                "doSetRFInfoCallback: failed to enable callback\n" );
    }
    else
    {
        fprintf( stderr, "doSetRFInfoCallback:\
                          enable callback(cbkTestSetRFInfoCallbackCB)\n" );
    }
}

/*************
 *
 * Name:    doClearRFInfoCallback
 *
 * Purpose: SetRFInfoCallback API driver
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
package void doClearRFInfoCallback( void )
{
    ULONG rc;

    rc = SetRFInfoCallback( NULL );

    if ( rc != eQCWWAN_ERR_NONE )
    {
        fprintf( stderr,
                   "doClearSetRFInfoCallback: failed to disable callback\n" );
    }
    else
    {
        fprintf( stderr, "doClearSetRFInfoCallback: disable callback (NULL)\n" );
    }
}

/*************
 *
 * Name:    cbkTestSetLURejectCallbackCB
 *
 * Purpose: SetLURejectCallback API callback function
 *
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:
 *
 **************/
local void cbkTestSetLURejectCallbackCB()

{
    FILE *fp;

    fp = tfopen("../../cbk/test/results/setLUrejectcallback.txt", "a");

    if ( fp == NULL )
    {
        perror("cbkTestSetLUrejectCB");
        return;
    }
    tfclose(fp);
}

/*************
 *
 * Name:    doSetLURejectCallback
 *
 * Purpose: SetLURejectCallback API driver function
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
package void doSetLURejectCallback( void )
{
    ULONG rc;

    rc = SetLURejectCallback(&cbkTestSetLURejectCallbackCB );

    if ( rc != eQCWWAN_ERR_NONE )
    {
        fprintf( stderr,
                "doSetLURejectCallback: failed to enable callback\n" );
    }
    else
    {
        fprintf( stderr, "doSetLUrejectCallback:\
                          enable callback(cbkTestSetLURejectCallbackCB)\n" );
    }
}

/*************
 *
 * Name:    doClearLURejectCallback
 *
 * Purpose: SetLURejectCallback API driver
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
package void doClearLURejectCallback( void )
{
    ULONG rc;

    rc = SetLURejectCallback( NULL );

    if ( rc != eQCWWAN_ERR_NONE )
    {
        fprintf( stderr,
                   "doClearLURejectCallback: failed to disable callback\n" );
    }
    else
    {
        fprintf( stderr, "doClearLURejectCallback: disable callback (NULL)\n" );
    }
}

/*************
 *
 * Name:    cbkTestSetCatEventCB
 *
 * Purpose: SetCATEventCallback API callback function
 *
 **************/
local void cbkTestSetCatEventCB(
    ULONG EventID,
    ULONG EventLength,
    BYTE  *pEventData
)
{
    FILE *fp;
    ULONG index;

    fp = tfopen("../../cbk/test/results/cbkTestSetCatEventCB.txt", "a");
    if ( fp == NULL )
        perror("cbkTestSetCatEventCB");
    else
    {
        fprintf ( fp, "Event ID: %lx\r\n\n", EventID );
        fprintf ( fp, "Event Length is: %lx\r\n\n", EventLength );
        if( (EventID == eTLV_DISPLAY_TEXT)          ||
            (EventID == eTLV_GET_IN_KEY)            ||
            (EventID == eTLV_GET_INPUT)             ||
            (EventID == eTLV_SETUP_MENU)            ||
            (EventID == eTLV_SELECT_ITEM)           ||
            (EventID == eTLV_LANGUAGE_NOTIFICATION) ||
            (EventID == eTLV_SETUP_IDLE_MODE_TEXT) )
        {
            struct CatEventIDDataTlv *pData =
                    (struct CatEventIDDataTlv *)pEventData;
            fprintf ( fp, "Reference ID is: %lx\r\n\n", pData->ReferenceID );
            fprintf ( fp, "Data Length is: %d\r\n\n", pData->DataLength );
            fprintf ( fp, "Event Data is: \r\n" );
            for ( index = 0; index < pData->DataLength; index++ )
                fprintf ( fp, "%c", pData->Data[index] );

            fprintf ( fp, "\r\nEvent Data is (in HEX): \r\n" );
            for ( index = 0; index < pData->DataLength; index++ )
                fprintf ( fp, "%x ", pData->Data[index] );

            fprintf ( fp, "\r\n" );
        }
        tfclose(fp);
    }
}

/*************
 *
 * Name:    doSetCATEventCallback
 *
 * Purpose: SetCATEventCallback API test function
 *
 **************/
package void doSetCATEventCallback( void )
{
    ULONG rc;
    ULONG eventMask = 0xFFFF;
    ULONG pErrorMask;

    rc = SetCATEventCallback( &cbkTestSetCatEventCB, eventMask, &pErrorMask  );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doSetCATEventCallback: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "doSetCATEventCallback: Enabled callback\r\n" );
}

/*************
 *
 * Name:    doClearCATEventCallback
 *
 * Purpose: Clear the SetCATEventCallback API
 *
 **************/
package void doClearCATEventCallback( void )
{
    ULONG rc;
    ULONG eventMask = 0xFFFF;
    ULONG pErrorMask;

    rc = SetCATEventCallback( NULL, eventMask, &pErrorMask  );
    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearCATEventCallback: Failed : %lu\n", rc );
    else
        fprintf( stderr, "doClearCATEventCallback: Disabled\n" );
}

/*************
 *
 * Name:    cbkTestSetNMEACB
 *
 * Purpose: SetNMEACallback API callback function
 *
 **************/
local void cbkTestSetNMEACB(
const char *pNMEAData)

{
    FILE *fp;

    fp = tfopen("../../cbk/test/results/setPositionDataNMEAcallback.txt", "a");
    if ( fp == NULL )
        perror("cbkTestSetNMEACB");
    else
    {
        fprintf ( fp, "NMEA Data Follows:\r\n\n" );
        fprintf ( fp, "%s\r\n", pNMEAData );
        tfclose(fp);
    }
}

/*************
 *
 * Name:    doSetNMEACallback
 *
 * Purpose: SetNMEACallback API test function
 *
 **************/
package void doSetNMEACallback( void )
{
    ULONG rc;

    rc = SetNMEACallback(&cbkTestSetNMEACB );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doSetNMEACallback: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "doSetNMEACallback: Enabled callback\r\n" );
}

/*************
 *
 * Name:    doClearNMEACallback
 *
 * Purpose: Clear the SetNMEACallback API
 *
 **************/
package void doClearNMEACallback( void )
{
    ULONG rc;

    rc = SetNMEACallback( NULL );
    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearNMEACallback: Failed : %lu\n", rc );
    else
        fprintf( stderr, "doClearNMEACallback: Disabled\n" );
}

/*************
 *
 * Name:    cbkTestSetPdsStateCB
 *
 * Purpose: SetPdsStateCallback API callback function
 *
 **************/
local void cbkTestSetPdsStateCB(
    ULONG EnabledMode,
    ULONG TrackingState)
{
    FILE *fp;

    fp = tfopen("../../cbk/test/results/cbkTestSetPdsStateCbk.txt", "a");
    if ( fp == NULL )
        perror("cbkTestSetPdsStateCbk");
    else
    {
        fprintf ( fp, "Enabled Mode is: %lu\r\n\n", EnabledMode );
        fprintf ( fp, "cbkTestSetPdsStateCbk is: %lu\r\n\n", TrackingState );
        tfclose(fp);
    }
}

/*************
 *
 * Name:    doSetPdsStateCallback
 *
 * Purpose: SetPdsStateCallback API test function
 *
 **************/
package void doSetPdsStateCallback( void )
{
    ULONG rc;

    rc = SetPDSStateCallback( &cbkTestSetPdsStateCB );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doSetPdsStateCallback: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "doSetPdsStateCallback: Enabled callback\r\n" );
}

/*************
 *
 * Name:    doClearPdsStateCallback
 *
 * Purpose: Clear the SetPdsStateCallback API
 *
 **************/
package void doClearPdsStateCallback( void )
{
    ULONG rc;

    rc = SetPDSStateCallback( NULL );
    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearPdsStateCallback: Failed : %lu\n", rc );
    else
        fprintf( stderr, "doClearPdsStateCallback: Disabled\n" );
}

/*************
 *
 * Name:    cbkTestSetNewSMSCB
 *
 * Purpose: SetNewSMSCallback API callback function
 *
 **************/
local void cbkTestSetNewSMSCB(
    ULONG StorageType,
    ULONG MessageIndex
)
{
    FILE *fp;

    fp = tfopen("../../cbk/test/results/cbkTestSetNewSMSCB.txt", "a");
    if ( fp == NULL )
        perror("cbkTestSetNewSMSCB");
    else
    {
        fprintf ( fp, "SMS Storage Type is: %lu\r\n\n", StorageType );
        fprintf ( fp, "SMS Message Index is: %lu\r\n\n", MessageIndex );
        tfclose(fp);
    }
}

/*************
 *
 * Name:    doSetNewSMSCallback
 *
 * Purpose: SetNewSMSCallback API test function
 *
 **************/
package void doSetNewSMSCallback( void )
{
    ULONG rc;

    rc = SetNewSMSCallback( &cbkTestSetNewSMSCB );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doSetNewSMSCallback: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "doSetNewSMSCallback: Enabled callback\r\n" );
}

/*************
 *
 * Name:    doClearNewSMSCallback
 *
 * Purpose: Clear the SetNewSMSCallback API
 *
 **************/
package void doClearNewSMSCallback( void )
{
    ULONG rc;

    rc = SetNewSMSCallback( NULL );
    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearNewSMSCallback: Failed : %lu\n", rc );
    else
        fprintf( stderr, "doClearNewSMSCallback: Disabled\n" );
}

/*************
 *
 * Name:    cbkTestSMSEventCB
 *
 * Purpose: SLQSSetSMSEventCallback API callback function
 *
 **************/
local void cbkTestSMSEventCB( SMSEventInfo *pSMSEventInfo )
{
    FILE *fp;
    int lLength, lIndex = 0;

    fp = tfopen("../../cbk/test/results/cbkTestSMSEventCB.txt", "a");
    if ( fp == NULL )
        perror("cbkTestSMSEventCB");
    else
    {
        if ( pSMSEventInfo->smsEventType & SMS_EVENT_MT_MESSAGE )
        {
            fprintf ( fp, "Event : SMS_EVENT_MT_MESSAGE\r\n" );
            fprintf ( fp,
                      "SMS Storage Type is: %lu\r\n\n",
                      pSMSEventInfo->pMTMessageInfo->storageType );
            fprintf ( fp,
                      "SMS Message Index is: %lu\r\n\n",
                      pSMSEventInfo->pMTMessageInfo->messageIndex );
        }

        if ( pSMSEventInfo->smsEventType & SMS_EVENT_MESSAGE_MODE )
        {
            fprintf ( fp, "Event : SMS_EVENT_MESSAGE_MODE\r\n" );
            fprintf ( fp,
                      "SMS Mode is: %d\r\n\n",
                      pSMSEventInfo->pMessageModeInfo->messageMode );
        }

        if ( pSMSEventInfo->smsEventType & SMS_EVENT_TRANSFER_ROUTE_MT_MESSAGE )
        {
            fprintf ( fp, "Event : SMS_EVENT_TRANSFER_ROUTE_MT_MESSAGE\r\n" );
            fprintf ( fp,
                      "Ack Indicator is: %d\r\n\n",
                      pSMSEventInfo->pTransferRouteMTMessageInfo->ackIndicator );
            fprintf ( fp,
                      "Transaction ID is: %lu\r\n\n",
                      pSMSEventInfo->pTransferRouteMTMessageInfo->transactionID );
            fprintf ( fp,
                      "Format is: %d\r\n\n",
                      pSMSEventInfo->pTransferRouteMTMessageInfo->format );
            fprintf ( fp,
                      "length is: %d\r\n\n",
                      pSMSEventInfo->pTransferRouteMTMessageInfo->length );

            lLength = pSMSEventInfo->pTransferRouteMTMessageInfo->length;

            lIndex = 0;
            fprintf ( fp, "data is: ");
            while(lLength--)
                fprintf ( fp,
                          "%2xH",
                          pSMSEventInfo->pTransferRouteMTMessageInfo->data[lIndex++] );

            fprintf ( fp, "\r\n\n" );
        }

        if ( pSMSEventInfo->smsEventType & SMS_EVENT_ETWS )
        {
            fprintf ( fp, "Event : SMS_EVENT_ETWS\r\n" );
            fprintf ( fp,
                      "SMS Notification Type is: %d\r\n\n",
                      pSMSEventInfo->pEtwsMessageInfo->notificationType );
            fprintf ( fp,
                      "ETWS Message length is: %d\r\n\n",
                      pSMSEventInfo->pEtwsMessageInfo->length );

            lLength = pSMSEventInfo->pEtwsMessageInfo->length;
            lIndex = 0;
            fprintf ( fp, "data is: ");
            while(lLength--)
                fprintf ( fp,
                          "%2xH",
                          pSMSEventInfo->pEtwsMessageInfo->data[lIndex++] );

            fprintf ( fp, "\r\n\n" );

        }

        if ( pSMSEventInfo->smsEventType & SMS_EVENT_ETWS_PLMN )
        {
            fprintf ( fp, "Event : SMS_EVENT_ETWS_PLMN\r\n" );
            fprintf ( fp,
                      "MCC: %xH\r\n\n",
                      pSMSEventInfo->pEtwsPlmnInfo->mobileCountryCode );
            fprintf ( fp,
                      "MNC: %xH\r\n\n",
                      pSMSEventInfo->pEtwsPlmnInfo->mobileNetworkCode );
        }

        if ( pSMSEventInfo->smsEventType & SMS_EVENT_SMSC_ADDRESS )
        {
            fprintf ( fp, "Event : SMS_EVENT_SMSC_ADDRESS\r\n" );
            fprintf ( fp,
                      "SMSC length is: %d\r\n\n",
                      pSMSEventInfo->pSMSCAddressInfo->length );

            lLength = pSMSEventInfo->pSMSCAddressInfo->length;
            lIndex = 0;
            fprintf ( fp, "data is: ");
            while(lLength--)
                fprintf ( fp,
                          "%2x",
                          pSMSEventInfo->pSMSCAddressInfo->data[lIndex++] );

            fprintf ( fp, "\r\n\n" );
        }

        if ( pSMSEventInfo->smsEventType & SMS_EVENT_SMS_ON_IMS )
        {
            fprintf ( fp, "Event : SMS_EVENT_SMS_ON_IMS\r\n" );
            fprintf ( fp,
                      "SMS on IMS is: %x\r\n\n",
                      pSMSEventInfo->pSMSOnIMSInfo->smsOnIMS );
        }

        tfclose(fp);
    }
}

/*************
 *
 * Name:    doSLQSSetSMSEventCallback
 *
 * Purpose: SLQSSetSMSEventCallback API test function
 *
 **************/
package void doSLQSSetSMSEventCallback( void )
{
    ULONG rc;

    rc = SLQSSetSMSEventCallback( &cbkTestSMSEventCB );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doSLQSSetSMSEventCallback: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "doSLQSSetSMSEventCallback: Enabled callback\r\n" );
}

/*************
 *
 * Name:    doClearSLQSSetSMSEventCallback
 *
 * Purpose: Clear the SLQSSetSMSEventCallback API
 *
 **************/
package void doClearSLQSSetSMSEventCallback( void )
{
    ULONG rc;

    rc = SLQSSetSMSEventCallback( NULL );
    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearSLQSSetSMSEventCallback: Failed : %lu\n", rc );
    else
        fprintf( stderr, "doClearSLQSSetSMSEventCallback: Disabled\n" );
}

/*************
 *
 * Name:    cbkTestSetSLQSOMADMAlertCallback
 *
 * Purpose: SetSLQSOMADMAlertCallback API callback function
 *
 **************/
local void cbkTestSetSLQSOMADMAlertCallback(
   ULONG sessionType,
   BYTE* psessionTypeFields )
{
    FILE                  *fp;
    struct omaDmFotaTlv   *sessionInfoFota;
    struct omaDmConfigTlv *sessionInfoConfig;
    USHORT                lLength = 0;
    BYTE                  *pLByte;

    fp = tfopen("../../cbk/test/results/cbkTestSetSLQSOMADMAlertCallback.txt", "a");

    if ( fp == NULL )
        perror("cbkTestSetSLQSOMADMAlertCallback");
    else
    {
        fprintf ( fp, "Session Type is: %lu\r\n\n", sessionType );
        if ( sessionType == QMI_SWIOMA_DM_FOTA )
        {
            fprintf ( fp, "Session Information for FOTA is as follows:\n");
            if ( psessionTypeFields )
            {
                sessionInfoFota = (struct omaDmFotaTlv *)psessionTypeFields;
                fprintf( fp, "State: %x\n", sessionInfoFota->state );
                fprintf( fp, "User Input Req: %x\n",\
                    sessionInfoFota->userInputReq );
                fprintf( fp, "User Input Timeout: %d\n",\
                    sessionInfoFota->userInputTimeout );
                fprintf( fp, "Fwd Load Size: %lu\n",\
                    sessionInfoFota->fwdloadsize );
                fprintf( fp, "Fwd Load Complete: %lu\n",\
                    sessionInfoFota->fwloadComplete );
                fprintf( fp, "Update Complete Status: %d\n",\
                    sessionInfoFota->updateCompleteStatus );
                fprintf( fp, "Severity: %x\n",\
                    sessionInfoFota->severity );
                fprintf( fp, "Version Length: %d\n",\
                    sessionInfoFota->versionlength );
                fprintf( fp, "Version: " );
                lLength = sessionInfoFota->versionlength;
                pLByte  = sessionInfoFota->version;
                while ( lLength-- )
                    fprintf( fp, "%x", *(pLByte)++ );
                fprintf( fp, "\n" );
                fprintf( fp, "Name Length: %d\n",\
                    sessionInfoFota->namelength );
                fprintf( fp, "Package Name: " );
                lLength = sessionInfoFota->namelength;
                pLByte  = sessionInfoFota->package_name;
                while ( lLength-- )
                    fprintf( fp, "%x", *(pLByte)++ );
                fprintf( fp, "\n" );
                fprintf( fp, "Description Length: %d\n",\
                    sessionInfoFota->descriptionlength );
                fprintf( fp, "Description: " );
                lLength = sessionInfoFota->descriptionlength;
                pLByte  = sessionInfoFota->description;
                while ( lLength-- )
                    fprintf( fp, "%x", *(pLByte)++ );
                fprintf( fp, "\n" );
            }
        }
        else if( sessionType == QMI_SWIOMA_DM_CONFIG )
        {
            fprintf ( fp, "Session Information for Config is as follows:\n");
            if ( psessionTypeFields )
            {
                sessionInfoConfig = (struct omaDmConfigTlv *)psessionTypeFields;
                fprintf( fp, "State: %x\n", sessionInfoConfig->state );
                fprintf( fp, "User Input Req: %x\n",\
                    sessionInfoConfig->userInputReq );
                fprintf( fp, "User Input Timeout: %d\n",\
                    sessionInfoConfig->userInputTimeout );
                fprintf( fp, "Alert Message Length: %d\n",\
                    sessionInfoConfig->alertmsglength );
                fprintf( fp, "Alert Message: " );
                lLength = sessionInfoConfig->alertmsglength;
                pLByte  = sessionInfoConfig->alertmsg;
                while ( lLength-- )
                    fprintf( fp, "%x", *(pLByte)++ );
                fprintf( fp, "\n" );
            }
        }
        else
            fprintf ( fp, "Invalid Session Type Received\n");
        tfclose(fp);
    }
}

/*************
 *
 * Name:    doSetSLQSOMADMAlertCallback
 *
 * Purpose: SetSLQSOMADMAlertCallback API test function
 *
 **************/
package void doSetSLQSOMADMAlertCallback( void )
{
    ULONG rc;

    rc = SetSLQSOMADMAlertCallback( &cbkTestSetSLQSOMADMAlertCallback );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doSetSLQSOMADMAlertCallback: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "doSetSLQSOMADMAlertCallback: Enabled callback\r\n" );
}

/*************
 *
 * Name:    doClearSLQSOMADMAlertCallback
 *
 * Purpose: Clear the SetSLQSOMADMAlertCallback API
 *
 **************/
package void doClearSLQSOMADMAlertCallback( void )
{
    ULONG rc;

    rc = SetSLQSOMADMAlertCallback( NULL );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearSLQSOMADMAlertCallback: Failed : %lu\n", rc );
    else
        fprintf( stderr, "doClearSLQSOMADMAlertCallback: Disabled\n" );
}

/*************
 *
 * Name:    cbkTestSetDeviceStateChangeCallback
 *
 * Purpose: SetDeviceStateChangeCallback API callback function
 *
 **************/
local void cbkTestSetDeviceStateChangeCallback(
   eDevState   device_state )
{
    FILE *fp;

    fp = tfopen("../../cbk/test/results/cbkTestSetDeviceStateChangeCallback.txt", "a");

    if ( fp == NULL )
        perror("cbkTestSetDeviceStateChangeCallback");
    else
    {
        fprintf( fp, "Device State: %d\n", device_state );
        tfclose(fp);
    }
}

/*************
 *
 * Name:    doSetDeviceStateChangeCallback
 *
 * Purpose: SetDeviceStateChangeCallback API test function
 *
 **************/
package void doSetDeviceStateChangeCallback( void )
{
    ULONG rc;

    rc = SetDeviceStateChangeCbk( &cbkTestSetDeviceStateChangeCallback );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doSetDeviceStateChangeCallback: Failed : %lu\r\n",\
                 rc );
    else
        fprintf( stderr, "doSetDeviceStateChangeCallback: Enabled callback\r\n" );
}

/*************
 *
 * Name:    doClearDeviceStateChangeCallback
 *
 * Purpose: Clear the DeviceStateChangeCallback API
 *
 **************/
package void doClearDeviceStateChangeCallback( void )
{
    ULONG rc;

    rc = SetDeviceStateChangeCbk( NULL );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearDeviceStateChangeCallback: Failed : %lu\n",\
                 rc );
    else
        fprintf( stderr, "doClearDeviceStateChangeCallback: Disabled\n" );
}

/*************
 *
 * Name:    cbkTestSetFwDldCompletionCallback
 *
 * Purpose: SetFwDldCompletionCallback API callback function
 *
 **************/
local void cbkTestSetFwDldCompletionCallback(
    ULONG   fwdwl_com_status )
{
    FILE *fp;

    fp = tfopen("../../cbk/test/results/cbkTestSetFwDldCompletionCallback.txt", "a");

    if ( fp == NULL )
        perror("cbkTestSetFwDldCompletionCallback");
    else
    {
        fprintf( fp, "Firmware Download Completion Status: %lu\n",\
                 fwdwl_com_status );
        tfclose(fp);
    }
}

/*************
 *
 * Name:    doSetFwDldCompletionCallback
 *
 * Purpose: SetFwDldCompletionCallback API test function
 *
 **************/
package void doSetFwDldCompletionCallback( void )
{
    ULONG rc;

    rc = SetFwDldCompletionCbk( &cbkTestSetFwDldCompletionCallback );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doSetFwDldCompletionCallback: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "doSetFwDldCompletionCallback: Enabled callback\r\n" );
}

/*************
 *
 * Name:    doClearFwDldCompletionCallback
 *
 * Purpose: Clear the ClearFwDldCompletionCallback API
 *
 **************/
package void doClearFwDldCompletionCallback( void )
{
    ULONG rc;

    rc = SetFwDldCompletionCbk( NULL );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearFwDldCompletionCallback: Failed : %lu\n", rc );
    else
        fprintf( stderr, "doClearFwDldCompletionCallback: Disabled\n" );
}

/*************
 *
 * Name:    cbkSLQSServingSystemCallback
 *
 * Purpose: SLQSServingSystemCallback API callback function
 *
 **************/
local void cbkSLQSServingSystemCallback(
    struct ServingSystemInfo *pServingSystem )
{
    FILE *fp;

    fp = tfopen("../../cbk/test/results/cbkSLQSServingSystemCallback.txt", "a");

    if ( fp == NULL )
        perror("cbkSLQSServingSystemCallback");
    else
    {
        BYTE i;
        fprintf( fp, "SLQSServing System Parameters:\n" );
        fprintf( fp, "RegistrationState %d\n",
                 pServingSystem->registrationState );
        fprintf( fp, "CS AttachState:%d\n",
                 pServingSystem->csAttachState );
        fprintf( fp, "PS AttachState:%d\n",
                 pServingSystem->psAttachState );
        fprintf( fp, "SelectedNetwork:%d\n",
                 pServingSystem->selectedNetwork );
        fprintf( fp, "radioInterfaceNo:%d\n",
                 pServingSystem->radioInterfaceNo );
        fprintf( fp, "HDR Personality %d\n",
                 pServingSystem->hdrPersonality );
        for( i = 0; i < pServingSystem->radioInterfaceNo ; i++ )
            fprintf( fp, "RadioInterfaces:%d\n",
                     pServingSystem->radioInterfaceList[i] );
        tfclose(fp);
    }
}

/*************
 *
 * Name:    doSetSLQSServingSystemCallBack
 *
 * Purpose: SetSLQSServingSystemCallback API test function
 *
 **************/
package void doSetSLQSServingSystemCallback( void )
{
    ULONG rc;

    rc = SLQSSetServingSystemCallback( &cbkSLQSServingSystemCallback );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doSetSLQSServingSystemCallback: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "doSetSLQSServingSystemCallback: Enabled callback\r\n" );
}

/*************
 *
 * Name:    doClearSLQSServingSystemCallback
 *
 * Purpose: Clear the SLQSServingSystemCallback API
 *
 **************/
package void doClearSLQSServingSystemCallback( void )
{
    ULONG rc;

    rc = SLQSSetServingSystemCallback( NULL );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearSLQSServingSystemCallback: Failed : %lu\n", rc );
    else
        fprintf( stderr, "doClearSLQSServingSystemCallback: Disabled\n" );
}

/*************
 *
 * Name:    cbkTestSetOMADMStateCallback
 *
 * Purpose: SetOMADMStateCallback API callback function
 *
 **************/
local void cbkTestSetOMADMStateCallback(
    ULONG sessionState,
    ULONG failureReason )
{
    FILE *fp;

    fp = tfopen("../../cbk/test/results/cbkTestSetOMADMStateCallback.txt", "a");

    if ( fp == NULL )
        perror("cbkTestSetOMADMStateCallback");
    else
    {
        fprintf( fp, "OMA DM State: %lu :: Fail Reason: %lu\n",
                 sessionState,
                 failureReason );
        tfclose(fp);
    }
}

/*************
 *
 * Name:    doSetOMADMStateCallback
 *
 * Purpose: SetOMADMStateCallback API test function
 *
 **************/
package void doSetOMADMStateCallback( void )
{
    ULONG rc;

    rc = SetOMADMStateCallback( &cbkTestSetOMADMStateCallback );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doSetOMADMStateCallback: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "doSetOMADMStateCallback: Enabled callback\r\n" );
}

/*************
 *
 * Name:    doClearOMADMStateCallback
 *
 * Purpose: Clear the SetOMADMStateCallback API
 *
 **************/
package void doClearOMADMStateCallback( void )
{
    ULONG rc;

    rc = SetOMADMStateCallback( NULL );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearOMADMStateCallback: Failed : %lu\n", rc );
    else
        fprintf( stderr, "doClearOMADMStateCallback: Disabled\n" );
}
/*************
 *
 * Name:    cbkSLQSSetBandPreferenceCallback
 *
 * Purpose: SLQSSetBandPreferenceCallback API callback function
 *
 **************/
local void cbkSLQSSetBandPreferenceCallback(
    ULONGLONG band_pref )
{
    FILE *fp;

    fp = tfopen("../../cbk/test/results/cbkSLQSSetBandPreferenceCallback.txt", "a");

    if ( fp == NULL )
        perror("cbkSLQSSetBandPreferenceCallback");
    else
    {
        fprintf( fp, "SLQSSetBandPreferenceCallback Parameters:\n" );
        fprintf( fp, "Band Preference %llx\n", band_pref );
        tfclose(fp);
    }
}

/*************
 *
 * Name:    doSetSLQSSetBandPrefCallBack
 *
 * Purpose: SLQSSetBandPreferenceCallback API test function
 *
 **************/
package void doSetSLQSSetBandPrefCallBack( void )
{
    ULONG rc;

    rc = SLQSSetBandPreferenceCbk( &cbkSLQSSetBandPreferenceCallback );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doSetSLQSSetBandPrefCallBack: Failed :"
                 " %lu\r\n", rc );
    else
        fprintf( stderr, "doSetSLQSSetBandPrefCallBack: Enabled callback\r\n" );
}

/*************
 *
 * Name:    doClearSLQSSetBandPrefCallback
 *
 * Purpose: Clear the SLQSSetBandPreferenceCallback API
 *
 **************/
package void doClearSLQSSetBandPrefCallback( void )
{
    ULONG rc;

    rc = SLQSSetBandPreferenceCbk( NULL );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearSLQSSetBandPrefCallback: Failed : "
                 "%lu\n", rc );
    else
        fprintf( stderr, "doClearSLQSSetBandPrefCallback: Disabled\n" );
}

/*************
 *
 * Name:    cbkTestSetUSSDReleaseCallback
 *
 * Purpose: SetUSSDReleaseCallback API callback function
 *
 **************/
local void cbkTestSetUSSDReleaseCallback()
{
    FILE *fp;

    fp = tfopen("../../cbk/test/results/cbkTestSetUSSDReleaseCallback.txt", "a");
    if ( fp == NULL )
        perror("cbkTestSetUSSDReleaseCallback");

    fprintf( fp, "USSD release indication received\n" );

    tfclose(fp);
}

/*************
 *
 * Name:    doSetUSSDReleaseCallback
 *
 * Purpose: SetUSSDReleaseCallback API test function
 *
 **************/
package void doSetUSSDReleaseCallback( void )
{
    ULONG rc;

    rc = SetUSSDReleaseCallback( &cbkTestSetUSSDReleaseCallback );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doSetSetUSSDReleaseCallback: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "doSetUSSDReleaseCallback: Enabled callback\r\n" );
}

/*************
 *
 * Name:   doClearSetUSSDReleaseCallback
 *
 * Purpose: Clear the SetUSSDReleaseCallback
 *
 **************/
package void doClearSetUSSDReleaseCallback( void )
{
    ULONG rc;

    rc = SetUSSDReleaseCallback( NULL );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearSetUSSDReleaseCallback: Failed : %lu\n", rc );
    else
        fprintf( stderr, "doClearSetUSSDReleaseCallback: Disabled\n" );
}

/*************
 *
 * Name:    cbkTestSetUSSDNotificationCallback
 *
 * Purpose: SetUSSDNotificationCallback API callback function
 *
 **************/
local void cbkTestSetUSSDNotificationCallback(
    ULONG Notification_type,
    BYTE *pNetworkInfo )
{
    FILE *fp;

    struct USSInfo *pNwInfo;
    BYTE           lLength = 0;

    fp = tfopen("../../cbk/test/results/cbkTestSetUSSDNotificationCallback.txt", "a");

    if ( fp == NULL )
        perror("cbkTestSetUSSDNotificationCallback");
    else
    {
        fprintf( fp, "Voice Ussd Notification Type: %lu :: \n",
                 Notification_type );
        if( pNetworkInfo )
        {
           pNwInfo = (struct USSInfo *)pNetworkInfo;
           fprintf( fp, "USS DCS: %d\n", pNwInfo->ussDCS );
           fprintf( fp, "USS Len: %d\n", pNwInfo->ussLen );

           fprintf( fp, "USS Data: ");
           while ( lLength < pNwInfo->ussLen )
           {
                fprintf( fp, " 0x%x", pNwInfo->ussData[lLength]);
                lLength++;
           }
           fprintf( fp, "\n\n");
        }
        tfclose(fp);
    }
}

/*************
 *
 * Name:    doSetUSSDNotificationCallback
 *
 * Purpose: SetUSSDNotificationCallback API test function
 *
 **************/
package void doSetUSSDNotificationCallback( void )
{
    ULONG rc;
    rc = SetUSSDNotificationCallback(&cbkTestSetUSSDNotificationCallback);

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doSetUSSDNotificationCallback: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "SetUSSDNotificationCallback: Enabled callback\r\n" );
}

/*************
 *
 * Name:    doClearUSSDNotificationCallback
 *
 * Purpose: Clear the SetUSSDNotificationCallback API
 *
 **************/
package void doClearUSSDNotificationCallback( void )
{
    ULONG rc;

    rc = SetUSSDNotificationCallback( NULL );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearUSSDNotificationCallback: Failed : %lu\n", rc );
    else
        fprintf( stderr, "doClearUSSDNotificationCallback: Disabled\n" );
}

/*************
 *
 * Name:    cbkTestSetUSSDNoWaitIndicationCallback
 *
 * Purpose: SetUSSDNoWaitIndicationCallback API callback function
 *
 **************/
local void cbkTestSetUSSDNoWaitIndicationCallback(
        USSDNoWaitIndicationInfo *pNetworkInfo )
{
    FILE *fp;

    struct USSInfo *pNwInfo;
    BYTE           lLength = 0;

    fp = tfopen("../../cbk/test/results/cbkTestSetUSSDNoWaitIndicationCallback.txt", "a");

    if ( fp == NULL )
        perror("cbkTestSetUSSDNoWaitIndicationCallback");
    else
    {
        if( pNetworkInfo )
        {
           if( NULL != pNetworkInfo->pUSSDData )
           {
               pNwInfo = pNetworkInfo->pUSSDData;
               fprintf( fp, "USS DCS: %d\n", pNwInfo->ussDCS );
               fprintf( fp, "USS Len: %d\n", pNwInfo->ussLen );

               fprintf( fp, "USS Data: ");
               while ( lLength < pNwInfo->ussLen )
               {
                    fprintf( fp, "%c", pNwInfo->ussData[lLength]);
                    lLength++;
               }
           }
           fprintf( fp, "\n\n");
        }
        tfclose(fp);
    }
}

/*************
 *
 * Name:    doSetUSSDNoWaitIndicationCallback
 *
 * Purpose: SetUSSDNoWaitIndicationCallback API test function
 *
 **************/
package void doSetUSSDNoWaitIndicationCallback( void )
{
    ULONG rc;
    rc = SetUSSDNoWaitIndicationCallback(&cbkTestSetUSSDNoWaitIndicationCallback);

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doSetUSSDNoWaitIndicationCallback: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "doSetUSSDNoWaitIndicationCallback: Enabled callback\r\n" );
}

/*************
 *
 * Name:    doClearUSSDNoWaitIndicationCallback
 *
 * Purpose: Clear the SetUSSDNoWaitIndicationCallback API
 *
 **************/
package void doClearUSSDNoWaitIndicationCallback( void )
{
    ULONG rc;

    rc = SetUSSDNoWaitIndicationCallback( NULL );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearUSSDNotificationCallback: Failed : %lu\n", rc );
    else
        fprintf( stderr, "doClearUSSDNotificationCallback: Disabled\n" );
}

/*************
 *
 * Name:    cbkTestSLQSSetSignalStrengthsCallback
 *
 * Purpose: SLQSSetSignalStrengthsCallback API callback function
 *
 **************/
local void cbkTestSLQSSetSignalStrengthsCallback(
    struct SLQSSignalStrengthsInformation sSLQSSignalStrengthsInfo )
{
    FILE *fp;

    fp = tfopen( "../../cbk/test/results/cbkTestSLQSSetSignalStrengthsCallback.txt", "a" );

    if ( fp == NULL )
        perror( "cbkTestSLQSSetSignalStrengthsCallback" );
    else
    {
        fprintf( fp, "RSSI Information :\n" );
        fprintf( fp, "Received Signal Strength : %d\n",
                     sSLQSSignalStrengthsInfo.rxSignalStrengthInfo.
                     rxSignalStrength );
        fprintf( fp, "Radio IF : %d\n\n",
                      sSLQSSignalStrengthsInfo.rxSignalStrengthInfo.radioIf );

        fprintf( fp, "ECIO Information :\n" );
        fprintf( fp, "ECIO     : %d\n",
                      sSLQSSignalStrengthsInfo.ecioInfo.ecio );
        fprintf( fp, "Radio IF : %d\n\n",
                     sSLQSSignalStrengthsInfo.ecioInfo.radioIf );

        fprintf( fp, "IO   : %lu\n", sSLQSSignalStrengthsInfo.io );
        fprintf( fp, "SINR : %d\n\n", sSLQSSignalStrengthsInfo.sinr );

        fprintf( fp, "Error Rate Information :\n" );
        fprintf( fp, "Error Rate : %d\n",
                     sSLQSSignalStrengthsInfo.errorRateInfo.errorRate );
        fprintf( fp, "Radio IF   : %d\n\n",
                     sSLQSSignalStrengthsInfo.errorRateInfo.radioIf );

        fprintf( fp, "RSRQ Information :\n" );
        fprintf( fp, "RSRQ     : %d\n",
                     sSLQSSignalStrengthsInfo.rsrqInfo.rsrq );
        fprintf( fp, "Radio IF : %d\n\n",
                     sSLQSSignalStrengthsInfo.rsrqInfo.radioIf );

        fprintf( fp, "\n\n");
        tfclose(fp);
    }
}

/*************
 *
 * Name:    doSLQSSetSignalStrengthsCallback
 *
 * Purpose: SLQSSetSignalStrengthsCallback API test function
 *
 **************/
package void doSLQSSetSignalStrengthsCallback( void )
{
    struct SLQSSignalStrengthsIndReq sSLQSSignalStrengthsIndReq;
    ULONG                            rc;
    BYTE                             idx = 0;
    SHORT                            ecioBuf[] = { -10, -20, -30, -40, -50 };
    BYTE                             sinrBuf[] = { 10, 20, 30, 40, 50 };

    /* Intialize the input parameters */
    sSLQSSignalStrengthsIndReq.rxSignalStrengthDelta = 1;
    sSLQSSignalStrengthsIndReq.ecioDelta             = 1;
    sSLQSSignalStrengthsIndReq.ioDelta               = 1;
    sSLQSSignalStrengthsIndReq.sinrDelta             = 1;
    sSLQSSignalStrengthsIndReq.rsrqDelta             = 1;
    sSLQSSignalStrengthsIndReq.ecioThresholdListLen  = 5;
    sSLQSSignalStrengthsIndReq.sinrThresholdListLen  = 5;

    for( idx = 0;
         idx < sSLQSSignalStrengthsIndReq.ecioThresholdListLen;
         idx++ )
    {
        sSLQSSignalStrengthsIndReq.ecioThresholdList[idx] = ecioBuf[idx];
    }

    for( idx = 0;
         idx < sSLQSSignalStrengthsIndReq.sinrThresholdListLen;
         idx++ )
    {
        sSLQSSignalStrengthsIndReq.sinrThresholdList[idx] = sinrBuf[idx];
    }

    rc = SLQSSetSignalStrengthsCallback(&cbkTestSLQSSetSignalStrengthsCallback,
                                        &sSLQSSignalStrengthsIndReq );

    if ( rc != eQCWWAN_ERR_NONE )
    {
        fprintf( stderr, "doSLQSSetSignalStrengthsCallback: "\
                         "Failed : %lu\r\n", rc );
    }
    else
    {
        fprintf( stderr, "SLQSSetSignalStrengthsCallback: Enabled callback\r\n" );
    }
}

/*************
 *
 * Name:    doClearSLQSSetSignalStrengthsCallback
 *
 * Purpose: Clear the SLQSSetSignalStrengthsCallback API
 *
 **************/
package void doClearSLQSSetSignalStrengthsCallback( void )
{
    struct SLQSSignalStrengthsIndReq sSLQSSignalStrengthsIndReq;
    ULONG                            rc;

    rc = SLQSSetSignalStrengthsCallback( NULL,
                                         &sSLQSSignalStrengthsIndReq );
    if ( rc != eQCWWAN_ERR_NONE )
    {
        fprintf( stderr, "doClearSLQSSetSignalStrengthsCallback:"\
                         "Failed : %lu\n", rc );
    }
    else
    {
        fprintf( stderr, "doClearSLQSSetSignalStrengthsCallback: Disabled\n" );
    }
}

/*************
 *
 * Name:    cbkTestSLQSVoiceSetSUPSNotificationCallback
 *
 * Purpose: SLQSVoiceSetSUPSNotificationCallback API callback function
 *
 **************/
local void cbkTestSLQSVoiceSetSUPSNotificationCallback(
    voiceSUPSNotification *pInfo )
{
    FILE *fp;

    fp = tfopen("../../cbk/test/results/SLQSVoiceSetSUPSNotificationCallback.txt", "a");

    if ( fp == NULL )
        perror("cbkTestSLQSVoiceSetSUPSNotificationCallback");
    else
    {
        fprintf( fp, "Voice SUPS: Call ID: %x :: Notification Type : %x \n",
                 pInfo->callID, pInfo->notifType );

        if( pInfo->pCUGIndex )
        {
            fprintf( fp, "CUG Index: %d\n", *(pInfo->pCUGIndex) );
        }
        if( pInfo->pECTNum )
        {
            fprintf(fp, "ECT Information:\n" );
            fprintf(fp, "Call State: %x\n", pInfo->pECTNum->ECTCallState );
            fprintf(fp, "Presentation Ind: %x\n", pInfo->pECTNum->presentationInd);
            fprintf(fp, "CUG Index: %s\n", pInfo->pECTNum->number);
        }
        fprintf( fp, "\n\n");
    }
    tfclose(fp);
}

/*************
 *
 * Name:    doSLQSVoiceSetSUPSNotificationCallback
 *
 * Purpose: SLQSVoiceSetSUPSNotificationCallback test function
 *
 **************/
package void doSLQSVoiceSetSUPSNotificationCallback( void )
{
    ULONG rc;
    rc = SLQSVoiceSetSUPSNotificationCallback(&cbkTestSLQSVoiceSetSUPSNotificationCallback);

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "SLQSVoiceSetSUPSNotificationCallback: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "SLQSVoiceSetSUPSNotificationCallback: Enabled callback\r\n" );
}

/*************
 *
 * Name:    doClearSLQSVoiceSetSUPSNotificationCallback
 *
 * Purpose: Clear the doSLQSVoiceSetSUPSNotificationCallback API
 *
 **************/
package void doClearSLQSVoiceSetSUPSNotificationCallback( void )
{
    ULONG rc;

    rc = SLQSVoiceSetSUPSNotificationCallback( NULL );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearSLQSVoiceSetSUPSNotificationCallback: Failed : %lu\n", rc );
    else
        fprintf( stderr, "doClearSLQSVoiceSetSUPSNotificationCallback: Disabled\n" );
}

/*************
 *
 * Name:    cbkTestSetSLQSSDKTerminatedCallback
 *
 * Purpose: SetSLQSKillSDKProcessCallback API callback function
 *
 **************/
local void cbkTestSetSLQSSDKTerminatedCallback(
    BYTE *psReason)
{
    FILE *fp;

    fp = tfopen("../../cbk/test/results/cbkTestSetSLQSSDKTerminatedCallback.txt", "a");
    if ( fp == NULL )
        perror("cbkTestSetSLQSSDKTerminatedCallback");

    fprintf( fp, "\nSDK terminated indication:%s\n", psReason );

    tfclose(fp);
}

/*************
 *
 * Name:    doSetSLQSKillSDKProcessCallback
 *
 * Purpose: Set SLQSKillSDKProcessCallback API test function
 *
 **************/
package void doSetSLQSSDKTerminatedCallback( void )
{
    ULONG rc;
    rc = SLQSSetSDKTerminatedCallback (&cbkTestSetSLQSSDKTerminatedCallback);

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "SetSLQSSDKTerminatedCallback: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "SetSLQSSDKTerminatedCallback: Enabled callback\r\n" );
}

/*************
 *
 * Name:    doClearSLQSKillSDKProcessCallback
 *
 * Purpose: Clear the SLQSSetSignalStrengthsCallback API
 *
 **************/
package void doClearSLQSSDKTerminatedCallback( void )
{
    ULONG                            rc;

    rc = SLQSSetSDKTerminatedCallback( NULL ) ;
    if ( rc != eQCWWAN_ERR_NONE )
    {
        fprintf( stderr, "doClearSLQSSDKTerminatedCallback:"\
                         "Failed : %lu\n", rc );
    }
    else
    {
        fprintf( stderr, "ClearSLQSSDKTerminatedCallback: Disabled\n" );
    }
}

/*************
 *
 * Name:    cbkTestSLQSVoiceSetAllCallStatusCB
 *
 * Purpose: SLQSSetSMSEventCallback API callback function
 *
 **************/
local void cbkTestSLQSVoiceSetAllCallStatusCB(
    voiceSetAllCallStatusCbkInfo *pVoiceSetAllCallStatusCbkInfo )
{
    FILE *fp;
    int lCount  = 0;
    int lLcount = 0;
    voiceSetAllCallStatusCbkInfo *pLVACS = pVoiceSetAllCallStatusCbkInfo;

    fp = tfopen("../../cbk/test/results/cbkTestSLQSVoiceSetAllCallStatusCB.txt", "a");
    if ( fp == NULL )
        perror("cbkTestSLQSVoiceSetAllCallStatusCB");
    else
    {
        fprintf( fp, "\nCall Information:\n" );
        {
            arrCallInfo temp =
               (arrCallInfo )pLVACS->arrCallInfomation;

            fprintf( fp, "Num of instances:%d\n",
                              temp.numInstances );
            for(lCount = 0; lCount < temp.numInstances; lCount++)
            {
                fprintf( fp, "Call Identifier:%d\n",
                    temp.getAllCallInfo[lCount].Callinfo.callID);
                fprintf( fp, "Call State:%d\n",
                    temp.getAllCallInfo[lCount].Callinfo.callState);
                fprintf( fp, "Call Type:%d\n",
                    temp.getAllCallInfo[lCount].Callinfo.callType);
                fprintf( fp, "Direction:%d\n",
                    temp.getAllCallInfo[lCount].Callinfo.direction);
                fprintf( fp, "Mode:%d\n",
                    temp.getAllCallInfo[lCount].Callinfo.mode);
            }
        }

        if( pLVACS->pArrRemotePartyNum )
        {
             fprintf( fp, "\nRemote Party Number:\n" );
             {
                 arrRemotePartyNum *pTemp =
                    (arrRemotePartyNum * )pLVACS->pArrRemotePartyNum;
                 fprintf( fp, "Num Of Instances :%d\n",
                               pTemp->numInstances );

                 for(lCount = 0; lCount < pTemp->numInstances; lCount++)
                 {
                     fprintf( fp, "Call Identifier:%d\n",
                          pTemp->RmtPtyNum[lCount].callID);

                     fprintf( fp, "Presentation Indicator:%d\n",
                         pTemp->RmtPtyNum[lCount].RemotePartyNum.presentationInd);

                     fprintf( fp, "Number Length:%d\n",
                         pTemp->RmtPtyNum[lCount].RemotePartyNum.numLen);

                     fprintf( fp, "Number:" );
                     for( lLcount = 0; lLcount < pTemp->RmtPtyNum[lCount].
                                              RemotePartyNum.numLen; lLcount++ )
                     {
                         fprintf( fp, "%c",pTemp->RmtPtyNum[lCount].
                                      RemotePartyNum.remPartyNumber[lLcount]);
                     }
                 }
             }
             fprintf( fp, "\n");
        }

        if ( pLVACS->pArrRemotePartyName )
        {
            fprintf( fp, "\nRemote Party Name:\n" );
            {
                arrRemotePartyName *pTemp =
                   (arrRemotePartyName * )pLVACS->pArrRemotePartyName;
                fprintf( fp, "Num of instances :%d\n", pTemp->numInstances );

                for(lCount = 0; lCount < pTemp->numInstances; lCount++)
                {
                    fprintf( fp, "Call Identifier:%d\n",
                       pTemp->GetAllCallRmtPtyName[lCount].callID);

                    fprintf( fp, "Presentation Indicator :%d\n",
                                 pTemp->GetAllCallRmtPtyName[lCount].
                                        RemotePartyName.namePI);

                    fprintf( fp, "Coding Scheme:%d\n",
                       pTemp->GetAllCallRmtPtyName[lCount].
                              RemotePartyName.codingScheme);

                    fprintf( fp, "Number Length:%d\n",
                       pTemp->GetAllCallRmtPtyName[lCount].
                              RemotePartyName.nameLen);

                    fprintf( fp, "Caller Name:\n");
                    for( lLcount = 0; lLcount <
                                      pTemp->GetAllCallRmtPtyName[lCount].
                                      RemotePartyName.nameLen; lLcount++ )
                    {
                        fprintf( fp, "%c",
                                     pTemp->GetAllCallRmtPtyName[lCount].
                                     RemotePartyName.callerName[lLcount]);
                    }
                }
            }
            fprintf( fp, "\n");
        }

        if ( pLVACS->pArrAlertingType )
        {
            fprintf( fp, "\nAlerting Type:\n" );
            {
                arrAlertingType *pTemp =
                   (arrAlertingType* )pLVACS->pArrAlertingType;
                fprintf( fp, "Num of instances :%d\n", pTemp->numInstances );

                for(lCount = 0; lCount < pTemp->numInstances; lCount++)
                {
                    fprintf( fp, "Call Identifier:%d\n",
                         pTemp->callID[lCount]);

                    fprintf( fp, "Alerting type :%d\n",
                        pTemp->AlertingType[lCount]);

                }
            }
        }
        if ( pLVACS->pArrSvcOption )
        {
            fprintf( fp, "\nService Option:\n" );
            {
                arrSvcOption *pTemp =
                   (arrSvcOption* )pLVACS->pArrSvcOption;
                fprintf( fp, "Num of instances :%d\n", pTemp->numInstances );

                for(lCount = 0; lCount < pTemp->numInstances; lCount++)
                {
                    fprintf( fp, "Call Identifier:%d\n",
                         pTemp->callID[lCount]);

                    fprintf( fp, "Service Option:%d\n",
                        pTemp->srvOption[lCount]);
                }
            }
        }
        if ( pLVACS->pArrCallEndReason )
        {
            fprintf( fp, "\nCall End Reason:\n" );
            {
                arrCallEndReason *pTemp =
                   (arrCallEndReason* )pLVACS->pArrCallEndReason;
                fprintf( fp, "Num of instances :%d\n", pTemp->numInstances );

                for(lCount = 0; lCount < pTemp->numInstances; lCount++)
                {
                    fprintf( fp, "Call Identifier:%d\n",
                         pTemp->callID[lCount]);

                    fprintf( fp, "Service Option:%d\n",
                        pTemp->callEndReason[lCount]);
                }
            }
        }
        if ( pLVACS->pArrAlphaID )
        {
            fprintf( fp, "\nAlpha Identifier:\n" );
            {
                arrAlphaID *pTemp =
                   (arrAlphaID* )pLVACS->pArrAlphaID;
                fprintf( fp, "Num of instances :%d\n", pTemp->numInstances );

                for(lCount = 0; lCount < pTemp->numInstances; lCount++)
                {
                    fprintf( fp, "Call Identifier:%d\n",
                      pTemp->allCallsAlphaIDInfoArr[lCount].callID);

                    fprintf( fp, "Alpha DCS:%d\n",
                      pTemp->allCallsAlphaIDInfoArr[lCount].AlphaIDInfo.alphaDcs);

                    fprintf( fp, "Alpha Lenth:%d\n",
                      pTemp->allCallsAlphaIDInfoArr[lCount].AlphaIDInfo.alphaLen);

                    fprintf( fp, "Call Identifier:\n");
                    for(lLcount = 0; lLcount <
                                     pTemp->allCallsAlphaIDInfoArr[lCount].
                                     AlphaIDInfo.alphaLen; lLcount++ )
                    {
                        fprintf( fp, "%d",
                        pTemp->allCallsAlphaIDInfoArr[lCount].
                               AlphaIDInfo.alphaText[lLcount]);
                    }
                }
            }
            fprintf( fp, "\n");
        }
        if ( pLVACS->pArrConnectPartyNum )
        {
            fprintf( fp, "\nConnected Party Number:\n" );
            {
                arrConnectPartyNum *pTemp =
                   (arrConnectPartyNum* )pLVACS->pArrConnectPartyNum;
                fprintf( fp, "Num of instances :%d\n", pTemp->numInstances );

                for(lCount = 0; lCount < pTemp->numInstances; lCount++)
                {
                    fprintf( fp, "Call Identifier:%d\n",
                         pTemp->ConnectedPartyNum[lCount].callID);

                    fprintf( fp, "Presentation indicator:%d\n",
                        pTemp->ConnectedPartyNum[lCount].numPI);

                    fprintf( fp, "screening indicator:%d\n",
                        pTemp->ConnectedPartyNum[lCount].numSI);

                    fprintf( fp, "Connected number type:%d\n",
                        pTemp->ConnectedPartyNum[lCount].numType);

                    fprintf( fp, "Connected number plan:%d\n",
                        pTemp->ConnectedPartyNum[lCount].numPlan);

                    fprintf( fp, "Connected Length:%d\n",
                        pTemp->ConnectedPartyNum[lCount].numLen);

                    fprintf( fp, "Connected Number:");
                    for(lLcount = 0; lLcount < pTemp->ConnectedPartyNum[lCount]
                                               .numLen; lLcount++)
                    {
                        fprintf( fp, "%d",
                           pTemp->ConnectedPartyNum[lCount].number[lLcount]);
                    }
                }
            }
            fprintf( fp, "\n");
        }

        if ( pLVACS->pArrDiagInfo )
        {
            fprintf( fp, "\nDiagnostic Information:\n" );
            {
                arrDiagInfo *pTemp =
                   (arrDiagInfo* )pLVACS->pArrDiagInfo;
                fprintf( fp, "Num of instances :%d\n", pTemp->numInstances );

                for(lCount = 0; lCount < pTemp->numInstances; lCount++)
                {
                    fprintf( fp, "Call Identifier:%d\n",
                         pTemp->DiagInfo[lCount].callID);

                    fprintf( fp, "Diagnostic Info Length:%d\n",
                        pTemp->DiagInfo[lCount].DiagInfo.diagInfoLen);

                    fprintf( fp, "Diagnostic Information:");
                    for(lLcount = 0; lLcount < pTemp->DiagInfo[lCount].
                                               DiagInfo.diagInfoLen; lLcount++)
                    {
                        fprintf( fp, "%d",pTemp->DiagInfo[lCount].
                                     DiagInfo.diagnosticInfo[lLcount]);
                    }

                }
                fprintf( fp, "\n");
            }

            fprintf( fp, "\nCalled Party Number:\n" );
            {
                arrCalledPartyNum *pTemp =
                   (arrCalledPartyNum* )pLVACS->pArrCalledPartyNum;
                fprintf( fp, "Num of instances :%d\n", pTemp->numInstances );

                for(lCount = 0; lCount < pTemp->numInstances; lCount++)
                {
                    fprintf( fp, "Call Identifier:%d\n",
                         pTemp->CalledPartyNum[lCount].callID);

                    fprintf( fp, "Presentation indicator.:%d\n",
                         pTemp->CalledPartyNum[lCount].numPI);

                    fprintf( fp, "Number screening indicator:%d\n",
                         pTemp->CalledPartyNum[lCount].numSI);

                    fprintf( fp, "Number Type:%d\n",
                         pTemp->CalledPartyNum[lCount].numType);

                    fprintf( fp, "Number Plan:%d\n",
                         pTemp->CalledPartyNum[lCount].numPlan);

                    fprintf( fp, "Call Identifier:%d\n",
                         pTemp->CalledPartyNum[lCount].numLen);

                    fprintf( fp, "Connected Number:");
                    for(lLcount = 0; lLcount < pTemp->CalledPartyNum[lCount].
                                               numLen; lLcount++)
                    {
                        fprintf( fp, "%d",
                          pTemp->CalledPartyNum[lCount].number[lLcount]);
                    }

                }
            }
            fprintf( fp, "\n");
        }

        if ( pLVACS->pArrRedirPartyNum )
        {
            fprintf( fp, "\nRedirecting Party Number:\n" );
            {
                arrRedirPartyNum *pTemp =
                   (arrRedirPartyNum* )pLVACS->pArrRedirPartyNum;
                    fprintf( fp, "Num of instances :%d\n", pTemp->numInstances );

                for(lCount = 0; lCount < pTemp->numInstances; lCount++)
                {
                    fprintf( fp, "Call Identifier:%d\n",
                         pTemp->RedirPartyNum[lCount].callID);

                    fprintf( fp, "Presentation indicator.:%d\n",
                         pTemp->RedirPartyNum[lCount].numPI);

                    fprintf( fp, "Number screening indicator:%d\n",
                         pTemp->RedirPartyNum[lCount].numSI);

                    fprintf( fp, "Number type:%d\n",
                         pTemp->RedirPartyNum[lCount].numType);

                    fprintf( fp, "Number plan:%d\n",
                         pTemp->RedirPartyNum[lCount].numPlan);

                    fprintf( fp, "Call Identifier:%d\n",
                         pTemp->RedirPartyNum[lCount].numLen);

                    fprintf( fp, "Connected Number");
                    for(lLcount = 0; lLcount < pTemp->RedirPartyNum[lCount].
                                               numLen; lLcount++)
                    {
                        fprintf( fp, "%d",
                          pTemp->RedirPartyNum[lCount].number[lLcount]);
                    }
                }
            }
            fprintf( fp, "\n");
        }

        if ( pLVACS->pArrAlertingPattern )
        {

            fprintf( fp, "Alerting Pattern:\n" );
            {
                arrAlertingPattern *pTemp =
                   (arrAlertingPattern* )pLVACS->pArrAlertingPattern;
                fprintf( fp, "Num of instances :%d\n", pTemp->numInstances );

                for(lCount = 0; lCount < pTemp->numInstances; lCount++)
                {
                    fprintf( fp, "Call Identifier:%d\n",
                         pTemp->callID[lCount]);

                    fprintf( fp, "Service Option:%ld\n",
                        pTemp->alertingPattern[lCount]);
                }
            }
        }
    }
    tfclose(fp);
}

/*************
 *
 * Name:    doSLQSVoiceSetAllCallStatusCallback
 *
 * Purpose: SLQSVoiceSetAllCallStatusCallback API test function
 *
 **************/
package void doSetSLQSVoiceSetAllCallStatusCallback( void )
{
    ULONG rc;

    rc = SLQSVoiceSetAllCallStatusCallBack( &cbkTestSLQSVoiceSetAllCallStatusCB );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doSLQSVoiceSetAllCallStatusCallback: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "SLQSVoiceSetAllCallStatusCallback: Enabled callback\r\n" );
}

/*************
 *
 * Name:    doClearSLQSVoiceSetAllCallStatusCallback
 *
 * Purpose: Clear the SLQSVoiceSetAllCallStatusCallback API
 *
 **************/
package void doClearSLQSVoiceSetAllCallStatusCallback( void )
{
    ULONG rc;

    rc = SLQSVoiceSetAllCallStatusCallBack( NULL );
    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "SLQSVoiceSetAllCallStatusCallback: Failed : %lu\n", rc );
    else
        fprintf( stderr, "SLQSVoiceSetAllCallStatusCallback: Disabled\n" );
}

/*************
 *
 * Name:    cbkTestSLQSSetTransLayerInfoCallback
 *
 * Purpose: SLQSSetTransLayerInfoCallback API callback function
 *
 **************/
local void cbkTestSLQSSetTransLayerInfoCallback(
    transLayerNotification *pInfo )
{
    FILE *fp;

    fp = tfopen("../../cbk/test/results/SLQSSetTransLayerInfoCallback.txt", "a");

    if ( fp == NULL )
        perror("cbkTestSLQSSetTransLayerInfoCallback");
    else
    {
        fprintf( fp, "Registration Indication 0x%x \n",
                 pInfo->regInd );

        if( pInfo->pTransLayerInfo )
        {
            transLayerInfo Temp = *(pInfo->pTransLayerInfo);
            fprintf( fp, "Transport Layer Information:\n" );
            fprintf( fp, "Transport Type: 0x%x\n", Temp.TransType );
            fprintf( fp, "Transport Capabilities 0x%x\n", Temp.TransCap );
        }
        fprintf( fp, "\n\n" );
    }
    tfclose(fp);
}

/*************
 *
 * Name:    doSLQSSetTransLayerInfoCallback
 *
 * Purpose: SLQSSetTransLayerInfoCallback test function
 *
 **************/
package void doSLQSSetTransLayerInfoCallback( void )
{
    ULONG rc;
    rc = SLQSSetTransLayerInfoCallback( &cbkTestSLQSSetTransLayerInfoCallback );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "SLQSSetTransLayerInfoCallback: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "SLQSSetTransLayerInfoCallback: Enabled callback\r\n" );
}

/*************
 *
 * Name:    doClearSLQSSetTransLayerInfoCallback
 *
 * Purpose: Clear the doSLQSSetTransLayerInfoCallback API
 *
 **************/
package void doClearSLQSSetTransLayerInfoCallback( void )
{
    ULONG rc;

    rc = SLQSSetTransLayerInfoCallback( NULL );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearSLQSSetTransLayerInfoCallback: Failed : %lu\n", rc );
    else
        fprintf( stderr, "doClearSLQSSetTransLayerInfoCallback: Disabled\n" );
}

/*************
 *
 * Name:    cbkTestSLQSSetTransNWRegInfoCallback
 *
 * Purpose: SLQSSetTransNWRegInfoCallback API callback function
 *
 **************/
local void cbkTestSLQSSetTransNWRegInfoCallback(
    transNWRegInfoNotification *pInfo )
{
    FILE *fp;

    fp = tfopen("../../cbk/test/results/SLQSSetTransNWRegInfoCallback.txt", "a");

    if ( fp == NULL )
        perror("cbkTestSLQSSetTransNWRegInfoCallback");
    else
    {
        fprintf( fp, "Network Registration Status 0x%x \n",
                 pInfo->NWRegStat );

        fprintf( fp, "\n\n" );
    }
    tfclose(fp);
}

/*************
 *
 * Name:    doSLQSSetTransNWRegInfoCallback
 *
 * Purpose: SLQSSetTransNWRegInfoCallback test function
 *
 **************/
package void doSLQSSetTransNWRegInfoCallback( void )
{
    ULONG rc;
    rc = SLQSSetTransNWRegInfoCallback( &cbkTestSLQSSetTransNWRegInfoCallback );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "SLQSSetTransNWRegInfoCallback: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "SLQSSetTransNWRegInfoCallback: Enabled callback\r\n" );
}

/*************
 *
 * Name:    doClearSLQSSetTransNWRegInfoCallback
 *
 * Purpose: Clear the doSLQSSetTransNWRegInfoCallback API
 *
 **************/
package void doClearSLQSSetTransNWRegInfoCallback( void )
{
    ULONG rc;

    rc = SLQSSetTransNWRegInfoCallback( NULL );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearSLQSSetTransNWRegInfoCallback: Failed : %lu\n", rc );
    else
        fprintf( stderr, "doClearSLQSSetTransNWRegInfoCallback: Disabled\n" );
}

/*************
 *
 * Name:    cbkTestSLQSSetSysSelectionPrefCallBack
 *
 * Purpose: SLQSSetSysSelectionPrefCallBack API callback function
 *
 **************/
local void cbkTestSLQSSetSysSelectionPrefCallBack(
    sysSelectPrefInfo *pInfo )
{
    FILE *fp;

    fp = tfopen("../../cbk/test/results/SLQSSetSysSelectionPrefCallBack.txt", "a");

    if ( fp == NULL )
        perror("cbkTestSLQSSetSysSelectionPrefCallBack");
    else
    {
        fprintf( fp, "System Selection Preferences \n" );

        if( pInfo->pEmerMode )
        {
            fprintf( fp, "Emergency Mode: 0x%x\n", *(pInfo->pEmerMode) );
        }

        if( pInfo->pModePref)
        {
            fprintf( fp, "Mode Preference: 0x%x\n", *(pInfo->pModePref) );
        }

        if( pInfo->pBandPref)
        {
            fprintf( fp, "Band Preference: 0x%llx\n", *(pInfo->pBandPref) );
        }

        if( pInfo->pPRLPref)
        {
            fprintf( fp, "CDMA PRL Preference: 0x%x\n", *(pInfo->pPRLPref) );
        }

        if( pInfo->pRoamPref)
        {
            fprintf( fp, "Roaming Preference: 0x%x\n", *(pInfo->pRoamPref) );
        }

        if( pInfo->pLTEBandPref)
        {
            fprintf( fp, "LTE Band Preference: 0x%llx\n",
                    *(pInfo->pLTEBandPref) );
        }

        if( pInfo->pNetSelPref)
        {
            fprintf( fp, "Network Selection Preference: 0x%x\n",
                    *(pInfo->pNetSelPref) );
        }

        if( pInfo->pSrvDomainPref)
        {
            fprintf( fp, "Service Domain Preference: 0x%lx\n",
                    *(pInfo->pSrvDomainPref) );
        }

        if( pInfo->pGWAcqOrderPref)
        {
            fprintf( fp, "GSM/WCDMA Acquisition order Preference: 0x%lx\n",
                    *(pInfo->pGWAcqOrderPref) );
        }
        fprintf( fp, "\n\n");
    }
    tfclose(fp);
}

/*************
 *
 * Name:    doSLQSSetSysSelectionPrefCallBack
 *
 * Purpose: SLQSSetSysSelectionPrefCallBack test function
 *
 **************/
package void doSLQSSetSysSelectionPrefCallBack( void )
{
    ULONG rc;
    rc = SLQSSetSysSelectionPrefCallBack(&cbkTestSLQSSetSysSelectionPrefCallBack);

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "SLQSSetSysSelectionPrefCallBack: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "SLQSSetSysSelectionPrefCallBack: Enabled callback\r\n" );
}

/*************
 *
 * Name:    doClearSLQSSetSysSelectionPrefCallBack
 *
 * Purpose: Clear the doSLQSSetSysSelectionPrefCallBack API
 *
 **************/
package void doClearSLQSSetSysSelectionPrefCallBack( void )
{
    ULONG rc;

    rc = SLQSSetSysSelectionPrefCallBack( NULL );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearSLQSSetSysSelectionPrefCallBack: Failed : %lu\n", rc );
    else
        fprintf( stderr, "doClearSLQSSetSysSelectionPrefCallBack: Disabled\n" );
}

/*************
 *
 * Name:    cbkTestSLQSUIMSetRefreshCallBack
 *
 * Purpose: SLQSUIMSetRefreshCallBack API callback function
 *
 **************/
local void cbkTestSLQSUIMSetRefreshCallBack(
     UIMRefreshEvent *pUIMRefreshEvent )
{
    FILE *fp;
    BYTE lCount,lIcount;
    fp = tfopen("../../cbk/test/results/SLQSUIMSetRefreshCallBack.txt", "a");

    if ( fp == NULL )
        perror("cbkTestSLQSUIMSetRefreshCallBack");
    else
    {
        fprintf( fp, "\nUIM refresh event\n" );

        fprintf( fp, "Refresh Stage: %d\n", pUIMRefreshEvent->stage );
        fprintf( fp, "Refresh Mode : %d\n", pUIMRefreshEvent->mode );
        fprintf( fp, "Session Type : %d\n", pUIMRefreshEvent->sessionType );
        fprintf( fp, "aid Length   : %d\n", pUIMRefreshEvent->aidLength );

        fprintf( fp, "AID :" );
        for( lCount = 0; lCount < pUIMRefreshEvent->aidLength; lCount++ )
        {
            fprintf( fp, "%d", pUIMRefreshEvent->aid[lCount] );
        }
        fprintf( fp, "\n");

        fprintf( fp, "Number of files : %d\n", pUIMRefreshEvent->numOfFiles );

        for ( lCount = 0; lCount < pUIMRefreshEvent->numOfFiles; lCount++ )
        {
            fileInfo *pTmp = &pUIMRefreshEvent->arrfileInfo[lCount];
            fprintf( fp, "File ID : %x\n", pTmp->fileID );
            fprintf( fp, "pathLen : %d\n", pTmp->pathLen );

            fprintf( fp, "Path :" );
            for( lIcount = 0; lIcount < pTmp->pathLen; lIcount++ )
            {
                fprintf( fp, "%x", pTmp->path[lIcount] );
            }
            fprintf( fp, "\n");
        }
    }
    tfclose(fp);
}

/*************
 *
 * Name:    doSLQSUIMSetRefreshCallBack
 *
 * Purpose: SLQSUIMSetRefreshCallBack test function
 *
 **************/
package void doSLQSUIMSetRefreshCallBack( void )
{
    ULONG rc;
    rc = SLQSUIMSetRefreshCallBack(&cbkTestSLQSUIMSetRefreshCallBack);

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "SLQSUIMSetRefreshCallBack: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "SLQSUIMSetRefreshCallBack: Enabled callback\r\n" );
}

/*************
 *
 * Name:    doClearSLQSUIMSetRefreshCallBack
 *
 * Purpose: Clear the SLQSUIMSetRefreshCallBack API
 *
 **************/
package void doClearSLQSUIMSetRefreshCallBack( void )
{
    ULONG rc;

    rc = SLQSUIMSetRefreshCallBack( NULL );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearSLQSUIMSetRefreshCallBack: Failed : %lu\n", rc );
    else
        fprintf( stderr, "doClearSLQSUIMSetRefreshCallBack: Disabled\n" );
}

/*************
 *
 * Name:    cbkTestSLQSUIMSetStatusChangeCallBack
 *
 * Purpose: SLQSUIMSetStatusChangeCallBack API callback function
 *
 **************/
local void cbkTestSLQSUIMSetStatusChangeCallBack(
    UIMStatusChangeInfo *pUIMStatusChangeInfo )
{
    FILE *fp;
    WORD lcount,lIcount,lIcount1;
    fp = tfopen("../../cbk/test/results/SLQSUIMSetStatusChangeCallBack.txt", "a");

    if ( fp == NULL )
        perror("cbkTestSLQSUIMSetStatusChangeCallBack");
    else
    {
        cardStatus *lpCardStatus = &pUIMStatusChangeInfo->statusChange;
        fprintf(fp, "\nSLQSUIMSetStatusChangeCallBack Successful\n");
        fprintf(fp, "---------------------.----------------------\n");
        if ( NULL != lpCardStatus )
        {
            fprintf(fp, "Index of the primary GW   : %x\n", lpCardStatus->indexGwPri);
            fprintf(fp, "Index of the primary 1X   : %x\n", lpCardStatus->index1xPri);
            fprintf(fp, "Index of the secondary GW : %x\n", lpCardStatus->indexGwSec);
            fprintf(fp, "Index of the secondary 1X : %x\n", lpCardStatus->index1xSec);
            fprintf(fp, "Slots Available           : %x\n", lpCardStatus->numSlot);
            for ( lcount=0 ; lcount < lpCardStatus->numSlot; lcount++ )
            {
                slotInfo *temp = &lpCardStatus->SlotInfo[lcount];
                fprintf(fp, "\tInformation for SLOT%d\n ",lcount+1);
                fprintf(fp, "\tState of the Card         : %x\n", temp->cardState);
                fprintf(fp, "\tState of the UPIN         : %x\n", temp->upinState);
                fprintf(fp, "\tRetries Remaining(UPIN)   : %d\n", temp->upinRetries);
                fprintf(fp, "\tRetries Remaining(UPUK)   : %d\n", temp->upukRetries);
                fprintf(fp, "\tReason For Error          : %x\n", temp->errorState);
                fprintf(fp, "\tNo. of Apps Allowed       : %d\n", temp->numApp);
                for ( lIcount=0 ; lIcount < temp->numApp; lIcount++ )
                {
                    appStatus *lresp = &temp->AppStatus[lIcount];
                    fprintf(fp, "\t\tApplication Status Information for App%d\n ",lIcount+1);
                    fprintf(fp, "\t\tType of Application       : %x\n", lresp->appType);
                    fprintf(fp, "\t\tState of Application      : %x\n", lresp->appState);
                    fprintf(fp, "\t\tState of perso for App    : %x\n", lresp->persoState);
                    fprintf(fp, "\t\tIndicates perso feature   : %x\n", lresp->persoFeature);
                    fprintf(fp, "\t\tRetries Remaining(Perso BL): %d\n",
                                         lresp->persoRetries);
                    fprintf(fp, "\t\tRetries Remaining(Perso UB): %d\n",
                                         lresp->persoUnblockRetries);
                    fprintf(fp, "\t\tApplication Identifier Len: %d\n", lresp->aidLength);
                    fprintf(fp, "\t\tApplication Identifier Value : ");
                    for ( lIcount1=0 ; lIcount1 < lresp->aidLength; lIcount1++ )
                    {
                        fprintf(fp, "%x ", lresp->aidVal[lIcount1]);
                    }
                    fprintf(fp,"\n");
                    fprintf(fp, "\t\tIndication for UPIN       : %x\n", lresp->univPin);
                    fprintf(fp, "\t\tIndicates State of Pin1   : %x\n", lresp->pin1State);
                    fprintf(fp, "\t\tRetries Remaining(PIN1)   : %d\n", lresp->pin1Retries);
                    fprintf(fp, "\t\tRetries Remaining(PUK1)   : %d\n", lresp->puk1Retries);
                    fprintf(fp, "\t\tIndicates State of Pin2   : %x\n", lresp->pin2State);
                    fprintf(fp, "\t\tRetries Remaining(PIN2)   : %d\n", lresp->pin2Retries);
                    fprintf(fp, "\t\tRetries Remaining(PUK2)   : %d\n", lresp->puk2Retries);
                }
            }
        }
        fprintf(fp, "--------------------xxx---------------------\n");
    }
    tfclose(fp);
}

/*************
 *
 * Name:    doSLQSUIMSetStatusChangeCallBack
 *
 * Purpose: SLQSUIMSetStatusChangeCallBack test function
 *
 **************/
package void doSLQSUIMSetStatusChangeCallBack( void )
{
    ULONG rc;
    rc = SLQSUIMSetStatusChangeCallBack(&cbkTestSLQSUIMSetStatusChangeCallBack);

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "SLQSUIMSetStatusChangeCallBack: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "SLQSUIMSetStatusChangeCallBack: Enabled callback\r\n" );
}

/*************
 *
 * Name:    doClearSLQSUIMSetStatusChangeCallBack
 *
 * Purpose: Clear the SLQSUIMSetStatusChangeCallBack API
 *
 **************/
package void doClearSLQSUIMSetStatusChangeCallBack( void )
{
    ULONG rc;

    rc = SLQSUIMSetStatusChangeCallBack( NULL );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearSLQSUIMSetStatusChangeCallBack: Failed : %lu\n", rc );
    else
        fprintf( stderr, "doClearSLQSUIMSetStatusChangeCallBack: Disabled\n" );
}

/*************
 *
 * Name:    cbkTestSLQSVoiceSetPrivacyChangeCallback
 *
 * Purpose: SLQSVoiceSetPrivacyChangeCallback API callback function
 *
 **************/
local void cbkTestSLQSVoiceSetPrivacyChangeCallback(
    voicePrivacyInfo *pInfo )
{
    FILE *fp;

    fp = tfopen("../../cbk/test/results/SLQSVoiceSetPrivacyChangeCallback.txt", "a");

    if ( fp == NULL )
        perror("cbkTestSLQSVoiceSetPrivacyChangeCallback");
    else
    {
        fprintf( fp, "Privacy Change: Call ID: %x :: Voice Privacy : %x \n",
                 pInfo->callID, pInfo->voicePrivacy );

        fprintf( fp, "\n\n");
    }
    tfclose(fp);
}

/*************
 *
 * Name:    doSetSLQSVoiceSetPrivacyChangeCallback
 *
 * Purpose: SLQSVoiceSetPrivacyChangeCallback test function
 *
 **************/
package void doSetSLQSVoiceSetPrivacyChangeCallback( void )
{
    ULONG rc;
    rc = SLQSVoiceSetPrivacyChangeCallBack(&cbkTestSLQSVoiceSetPrivacyChangeCallback);

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "SLQSVoiceSetPrivacyChangeCallback: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "SLQSVoiceSetPrivacyChangeCallback: Enabled callback\r\n" );
}

/*************
 *
 * Name:    doClearSLQSVoiceSetPrivacyChangeCallback
 *
 * Purpose: Clear the doSLQSVoiceSetPrivacyChangeCallback API
 *
 **************/
package void doClearSLQSVoiceSetPrivacyChangeCallback( void )
{
    ULONG rc;

    rc = SLQSVoiceSetPrivacyChangeCallBack( NULL );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearSLQSVoiceSetPrivacyChangeCallback: Failed : %lu\n", rc );
    else
        fprintf( stderr, "doClearSLQSVoiceSetPrivacyChangeCallback: Disabled\n" );
}

/*************
 *
 * Name:    cbkTestSLQSVoiceSetDTMFEventCallback
 *
 * Purpose: SLQSVoiceSetDTMFEventCallback API callback function
 *
 **************/
local void cbkTestSLQSVoiceSetDTMFEventCallback(
    voiceDTMFEventInfo *pInfo )
{
    FILE *fp;
    WORD lcount;
    fp = tfopen("../../cbk/test/results/SLQSVoiceSetDTMFEventCallback.txt", "a");

    if ( fp == NULL )
        perror("cbkTestSLQSVoiceSetDTMFEventCallback");
    else
    {
        DTMFInfo *lTemp = &(pInfo->DTMFInformation);
        fprintf( fp, "Voice DTMF EVent Info ");
        fprintf( fp, "Call ID      : %x \n", lTemp->callID);
        fprintf( fp, "DTMF Event   : %x \n", lTemp->DTMFEvent);
        fprintf( fp, "No.of Digits : %d \n", lTemp->digitCnt);
        fprintf( fp, "Digit Values : ");
        for ( lcount = 0 ; lcount < lTemp->digitCnt ; lcount++ )
        {
            fprintf( fp, "%c", lTemp->digitBuff[lcount]);
           }
        fprintf( fp, "\n");
        IFPRINTF( fp, "On Length   : %d\n",pInfo->pOnLength);
        IFPRINTF( fp, "Off Length  : %d\n",pInfo->pOffLength);
        fprintf( fp, "\n\n");
    }
    tfclose(fp);
}

/*************
 *
 * Name:    doSetSLQSVoiceSetDTMFEventCallback
 *
 * Purpose: SLQSVoiceSetDTMFEventCallback test function
 *
 **************/
package void doSetSLQSVoiceSetDTMFEventCallback( void )
{
    ULONG rc;
    rc = SLQSVoiceSetDTMFEventCallBack(&cbkTestSLQSVoiceSetDTMFEventCallback);

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doSetSLQSVoiceSetDTMFEventCallback: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "doSetSLQSVoiceSetDTMFEventCallback: Enabled callback\r\n" );
}

/*************
 *
 * Name:    doClearSLQSVoiceSetDTMFEventCallback
 *
 * Purpose: Clear the doSLQSVoiceSetDTMFEventCallback API
 *
 **************/
package void doClearSLQSVoiceSetDTMFEventCallback( void )
{
    ULONG rc;

    rc = SLQSVoiceSetDTMFEventCallBack( NULL );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearSLQSVoiceSetDTMFEventCallback: Failed : %lu\n", rc );
    else
        fprintf( stderr, "doClearSLQSVoiceSetDTMFEventCallback: Disabled\n" );
}

/*************
 *
 * Name:    cbkTestSLQSVoiceSetSUPSCallback
 *
 * Purpose: SLQSVoiceSetSUPSCallback API callback function
 *
 **************/
local void cbkTestSLQSVoiceSetSUPSCallback(
    voiceSUPSInfo *pInfo )
{
    FILE *fp;
    WORD lcount,lIcount;
    fp = tfopen("../../cbk/test/results/SLQSVoiceSetSUPSCallback.txt", "a");

    if ( fp == NULL )
        perror("cbkTestSLQSVoiceSetSUPSCallback");
    else
    {
        SUPSInfo *lTemp = &(pInfo->SUPSInformation);
        fprintf( fp, "Voice SUPS Info From SLQSVoiceSetSUPSCallback\n");
        fprintf( fp, "Service Type   : %x \n", lTemp->svcType);
        fprintf( fp, "Modified by CC : %x \n", lTemp->isModByCC);

        IFPRINTF( fp, "Service Class  : %x\n",pInfo->pSvcClass);
        IFPRINTF( fp, "Reason         : %x\n",pInfo->pReason);
        if ( NULL != pInfo->pCallFWNum )
        {
            fprintf( fp, "Call FW Number : ");
            for ( lcount = 0 ; pInfo->pCallFWNum[lcount] != EOS ; lcount++ )
            {
                fprintf( fp, "%x ", pInfo->pCallFWNum[lcount]);
               }
            fprintf( fp, "\n");
        }
        IFPRINTF( fp, "Call FW No Reply Timer: %d\n",pInfo->pCallFWTimerVal);
        if ( NULL != pInfo->pUSSInfo )
        {
            fprintf( fp, "USS DCS        : %x \n", pInfo->pUSSInfo->ussDCS);
            fprintf( fp, "USS Length     : %d \n", pInfo->pUSSInfo->ussLen);
            fprintf( fp, "USS Data       : ");
            for ( lcount = 0 ; lcount < pInfo->pUSSInfo->ussLen ; lcount++ )
            {
                fprintf( fp, "%c", pInfo->pUSSInfo->ussData[lcount]);
            }
            fprintf( fp, "\n");
        }
        IFPRINTF( fp, "Call Id        : %x\n",pInfo->pCallID);
        if ( NULL != pInfo->pAlphaIDInfo )
        {
            alphaIDInfo *pAlphaIDInfo = pInfo->pAlphaIDInfo;
            fprintf( fp, "Alpha DCS       : %x \n", pAlphaIDInfo->alphaDcs);
            fprintf( fp, "Alpha Len       : %d \n", pAlphaIDInfo->alphaLen);
            fprintf( fp, "Alpha Text      : ");
            for (lcount = 0;lcount < pAlphaIDInfo->alphaLen;lcount++)
            {
                fprintf( fp, "%c", pAlphaIDInfo->alphaText[lcount]);
            }
            fprintf( fp,"\n");
        }
        if ( NULL != pInfo->pCallBarPasswd )
        {
            fprintf( fp, "Call Bar Password : ");
            for ( lcount = 0 ; lcount < PASSWORD_LENGTH ; lcount++ )
            {
                fprintf( fp, "%c", pInfo->pCallBarPasswd[lcount]);
               }
            fprintf( fp, "\n");
        }
        if ( NULL != pInfo->pNewPwdData )
        {
            newPwdData *pNewPwdData = pInfo->pNewPwdData;
            fprintf( fp, "New Password      : ");
            for ( lcount = 0 ; lcount < PASSWORD_LENGTH ; lcount++ )
            {
                fprintf( fp, "%c", pNewPwdData->newPwd[lcount]);
               }
            fprintf( fp, "\n");
            fprintf( fp, "New Password Again : ");
            for ( lcount = 0 ; lcount < PASSWORD_LENGTH ; lcount++ )
            {
                fprintf( fp, "%c", pNewPwdData->newPwdAgain[lcount]);
               }
            fprintf( fp, "\n");
        }
        IFPRINTF( fp, "Data Source      : %x\n",pInfo->pDataSrc);
        IFPRINTF( fp, "Failure Cause    : %x\n",pInfo->pFailCause);
        if ( NULL != pInfo->pCallFwdInfo )
        {
            fprintf(fp, "Instances of Call Forwarding Info : %x\n",
            pInfo->pCallFwdInfo->numInstances);
            for ( lcount = 0;lcount<pInfo->pCallFwdInfo->numInstances;lcount++)
            {
                callFWInfo *temp = &pInfo->pCallFwdInfo->CallFWInfo[lcount];
                fprintf(fp, "\tService Status   : %x \n",temp->SvcStatus);
                fprintf(fp, "\tService Class    : %x \n",temp->SvcClass);
                fprintf(fp, "\tNum Len          : %x \n",temp->numLen);
                fprintf(fp, "\tNumber           : ");
                for ( lIcount=0; lIcount<temp->numLen ;lIcount++ )
                {
                    fprintf(fp,"%c",temp->number[lIcount]);
                }
                fprintf(fp,"\n");
                fprintf(fp, "\tNo Reply Timer   : %x \n",temp->noReplyTimer);
            }
        }
        if ( NULL != pInfo->pCLIRstatus )
        {
            CLIRResp *pCLIRResp = pInfo->pCLIRstatus;
            fprintf( fp, "Activation Status : %x \n", pCLIRResp->ActiveStatus);
            fprintf( fp, "Provisioned Status: %x \n", pCLIRResp->ProvisionStatus);
        }
        if ( NULL != pInfo->pCLIPstatus )
        {
            CLIPResp *pCLIPResp = pInfo->pCLIPstatus;
            fprintf( fp, "Activation Status : %x \n", pCLIPResp->ActiveStatus);
            fprintf( fp, "Provisioned Status: %x \n", pCLIPResp->ProvisionStatus);
        }
        if ( NULL != pInfo->pCOLPstatus )
        {
            COLPResp *pCOLPResp = pInfo->pCOLPstatus;
            fprintf( fp, "Activation Status : %x \n", pCOLPResp->ActiveStatus);
            fprintf( fp, "Provisioned Status: %x \n", pCOLPResp->ProvisionStatus);
        }
        if ( NULL != pInfo->pCOLRstatus )
        {
            COLRResp *pCOLRResp = pInfo->pCOLRstatus;
            fprintf( fp, "Activation Status : %x \n", pCOLRResp->ActiveStatus);
            fprintf( fp, "Provisioned Status: %x \n", pCOLRResp->ProvisionStatus);
        }
        if ( NULL != pInfo->pCNAPstatus )
        {
            CNAPResp *pCNAPResp = pInfo->pCNAPstatus;
            fprintf( fp, "Activation Status : %x \n", pCNAPResp->ActiveStatus);
            fprintf( fp, "Provisioned Status: %x \n", pCNAPResp->ProvisionStatus);
        }
        fprintf( fp, "\n--------\n\n");
    }
    tfclose(fp);
}

/*************
 *
 * Name:    doSetSLQSVoiceSetSUPSCallback
 *
 * Purpose: SLQSVoiceSetSUPSCallback test function
 *
 **************/
package void doSetSLQSVoiceSetSUPSCallback( void )
{
    ULONG rc;
    rc = SLQSVoiceSetSUPSCallBack(&cbkTestSLQSVoiceSetSUPSCallback);

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doSetSLQSVoiceSetSUPSCallback: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "doSetSLQSVoiceSetSUPSCallback: Enabled callback\r\n" );
}

/*************
 *
 * Name:    doClearSLQSVoiceSetSUPSCallback
 *
 * Purpose: Clear the doSLQSVoiceSetSUPSCallback API
 *
 **************/
package void doClearSLQSVoiceSetSUPSCallback( void )
{
    ULONG rc;

    rc = SLQSVoiceSetSUPSCallBack( NULL );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearSLQSVoiceSetSUPSCallback: Failed : %lu\n", rc );
    else
        fprintf( stderr, "doClearSLQSVoiceSetSUPSCallback: Disabled\n" );
}

/*************
 *
 * Name:    cbkTestSLQSNasSysInfoCallBack
 *
 * Purpose: SLQSNasSysInfoCallBack API callback function
 *
 **************/
local void cbkTestSLQSNasSysInfoCallBack(
    nasSysInfo *pInfo )
{
    FILE *fp;
    WORD lC;
    fp = tfopen("../../cbk/test/results/SLQSNasSysInfoCallBack.txt", "a");

    if ( fp == NULL )
        perror("cbkTestSLQSNasSysInfoCallBack");
    else
    {
        fprintf( fp, "\n------------------------.------------------------\n\n");
        fprintf( fp, "NAS System Information From SLQSNasSysInfoCallBack\n");
        if ( pInfo->pCDMASrvStatusInfo )
        {
            fprintf( fp,"Service Status(CDMA) : %x\n",pInfo->pCDMASrvStatusInfo->srvStatus );
            fprintf( fp,"Is Preferred(CDMA)   : %x\n",pInfo->pCDMASrvStatusInfo->isPrefDataPath );
        }
        if ( pInfo->pHDRSrvStatusInfo )
        {
            fprintf( fp,"Service Status(HDR) : %x\n",pInfo->pHDRSrvStatusInfo->srvStatus );
            fprintf( fp,"Is Preferred(HDR)   : %x\n",pInfo->pHDRSrvStatusInfo->isPrefDataPath );
        }
        if ( pInfo->pGSMSrvStatusInfo )
        {
            fprintf( fp,"Service Status(GSM)      : %x\n",pInfo->pGSMSrvStatusInfo->srvStatus );
            fprintf( fp,"True Service Status(GSM) : %x\n",pInfo->pGSMSrvStatusInfo->trueSrvStatus );
            fprintf( fp,"Is Preferred(GSM)        : %x\n",pInfo->pGSMSrvStatusInfo->isPrefDataPath );
        }
        if ( pInfo->pWCDMASrvStatusInfo )
        {
            fprintf( fp,"Service Status(WCDMA)     : %x\n",pInfo->pWCDMASrvStatusInfo->srvStatus );
            fprintf( fp,"True Service Status(WCDMA): %x\n",pInfo->pWCDMASrvStatusInfo->trueSrvStatus );
            fprintf( fp,"Is Preferred(WCDMA)       : %x\n",pInfo->pWCDMASrvStatusInfo->isPrefDataPath );
        }
        if ( pInfo->pLTESrvStatusInfo )
        {
            fprintf( fp,"Service Status(LTE)      : %x\n",pInfo->pLTESrvStatusInfo->srvStatus );
            fprintf( fp,"True Service Status(LTE) : %x\n",pInfo->pLTESrvStatusInfo->trueSrvStatus );
            fprintf( fp,"Is Preferred(LTE)        : %x\n",pInfo->pLTESrvStatusInfo->isPrefDataPath );
        }
        if ( pInfo->pCDMASysInfo )
        {
            fprintf( fp,"\nCDMA SYSTEM INFORMATION ----\n");
            CDMASysInfo *t = pInfo->pCDMASysInfo;
            doprintsysInfoCommonCB( fp, &t->sysInfoCDMA );
            fprintf( fp,"System PRL Valid    : %x\n",t->isSysPrlMatchValid );
            fprintf( fp,"System PRL          : %x\n",t->isSysPrlMatch );
            fprintf( fp,"P_Rev Valid         : %x\n",t->pRevInUseValid );
            fprintf( fp,"P_Rev In Use        : %x\n",t->pRevInUse );
            fprintf( fp,"BS P_Rev Valid      : %x\n",t->bsPRevValid );
            fprintf( fp,"P_Rev In Use        : %x\n",t->bsPRev );
            fprintf( fp,"CCS_supp Valid      : %x\n",t->ccsSupportedValid );
            fprintf( fp,"CCS_supp            : %x\n",t->ccsSupported );
            fprintf( fp,"System Id           : %x\n",t->systemID );
            fprintf( fp,"Network Id          : %x\n",t->networkID );
            fprintf( fp,"BS Info Valid       : %x\n",t->bsInfoValid );
            fprintf( fp,"Base ID             : %x\n",t->baseId );
            fprintf( fp,"Base Latitude       : %lx\n",t->baseLat );
            fprintf( fp,"Base Longitude      : %lx\n",t->baseLong );
            fprintf( fp,"Packet Zone Valid   : %x\n",t->packetZoneValid );
            fprintf( fp,"Packet Zone         : %x\n",t->packetZone );
            fprintf( fp,"Network ID Valid    : %x\n",t->networkIdValid );
            fprintf( fp,"MCC info            : " );
            for ( lC = 0 ; lC < PLMN_LENGTH ; lC++ )
            {
                fprintf( fp,"%x ",t->MCC[lC] );
            }
            fprintf( fp,"\n" );
            fprintf( fp,"MNC info            : " );
            for ( lC = 0 ; lC < PLMN_LENGTH ; lC++ )
            {
                fprintf( fp,"%x ",t->MNC[lC] );
            }
            fprintf( fp,"\n" );
        }

        if ( pInfo->pHDRSysInfo )
        {
            fprintf( fp,"\nHDR SYSTEM INFORMATION ----\n");
            HDRSysInfo *t = pInfo->pHDRSysInfo;
            doprintsysInfoCommonCB( fp, &t->sysInfoHDR );
            fprintf( fp,"System PRL Valid    : %x\n",t->isSysPrlMatchValid );
            fprintf( fp,"System PRL          : %x\n",t->isSysPrlMatch );
            fprintf( fp,"Personality Valid   : %x\n",t->hdrPersonalityValid );
            fprintf( fp,"Personality         : %x\n",t->hdrPersonality );
            fprintf( fp,"Active Prot Valid   : %x\n",t->hdrActiveProtValid );
            fprintf( fp,"Active Protocol     : %x\n",t->hdrActiveProt );
            fprintf( fp,"IS-856 Sys Valid    : %x\n",t->is856SysIdValid );
            fprintf( fp,"IS-856 system ID    : " );
            for ( lC = 0 ; lC < SLQS_SYSTEM_ID_SIZE ; lC++ )
            {
                fprintf( fp,"%x ",t->is856SysId[lC] );
            }
            fprintf( fp,"\n" );
        }
        if ( pInfo->pGSMSysInfo )
        {
            fprintf( fp,"\nGSM SYSTEM INFORMATION ----\n");
            GSMSysInfo *t = pInfo->pGSMSysInfo;
            doprintsysInfoCommonCB( fp, &t->sysInfoGSM );
            fprintf( fp,"LAC Valid           : %x\n",t->lacValid );
            fprintf( fp,"LAC                 : %x\n",t->lac );
            fprintf( fp,"Cell ID Valid       : %x\n",t->cellIdValid );
            fprintf( fp,"Cell ID             : %lx\n",t->cellId );
            fprintf( fp,"Reg Rej Info Valid  : %x\n",t->regRejectInfoValid );
            fprintf( fp,"Reject Srvc Domain  : %x\n",t->rejectSrvDomain );
            fprintf( fp,"Reject Cause        : %x\n",t->rejCause );
            fprintf( fp,"Network Id Valid    : %x\n",t->networkIdValid );
            fprintf( fp,"MCC info            : " );
            for ( lC = 0 ; lC < PLMN_LENGTH ; lC++ )
            {
                fprintf( fp,"%x ",t->MCC[lC] );
            }
            fprintf( fp,"\n" );
            fprintf( fp,"MNC info            : " );
            for ( lC = 0 ; lC < PLMN_LENGTH ; lC++ )
            {
                fprintf( fp,"%x ",t->MNC[lC] );
            }
            fprintf( fp,"\n" );
            fprintf( fp,"EGPRS Support Valid : %x\n",t->egprsSuppValid );
            fprintf( fp,"EGPRS Support       : %x\n",t->egprsSupp );
            fprintf( fp,"DTM Support Valid   : %x\n",t->dtmSuppValid );
            fprintf( fp,"DTM Support         : %x\n",t->dtmSupp );
        }
        if ( pInfo->pWCDMASysInfo )
        {
            fprintf( fp,"\nWCDMA SYSTEM INFORMATION ----\n");
            WCDMASysInfo *t = pInfo->pWCDMASysInfo;
            doprintsysInfoCommonCB( fp, &t->sysInfoWCDMA );
            fprintf( fp,"LAC Valid           : %x\n",t->lacValid );
            fprintf( fp,"LAC                 : %x\n",t->lac );
            fprintf( fp,"Cell ID Valid       : %x\n",t->cellIdValid );
            fprintf( fp,"Cell ID             : %lx\n",t->cellId );
            fprintf( fp,"Reg Rej Info Valid  : %x\n",t->regRejectInfoValid );
            fprintf( fp,"Reject Srvc Domain  : %x\n",t->rejectSrvDomain );
            fprintf( fp,"Reject Cause        : %x\n",t->rejCause );
            fprintf( fp,"Network Id Valid    : %x\n",t->networkIdValid );
            fprintf( fp,"MCC info            : " );
            for ( lC = 0 ; lC < PLMN_LENGTH ; lC++ )
            {
                fprintf( fp,"%x ",t->MCC[lC] );
            }
            fprintf( fp,"\n" );
            fprintf( fp,"MNC info            : " );
            for ( lC = 0 ; lC < PLMN_LENGTH ; lC++ )
            {
                fprintf( fp,"%x ",t->MNC[lC] );
            }
            fprintf( fp,"\n" );
            fprintf( fp,"HS Call Status Valid: %x\n",t->hsCallStatusValid );
            fprintf( fp,"HS Call Status      : %x\n",t->hsCallStatus );
            fprintf( fp,"HS Ind Valid        : %x\n",t->hsIndValid );
            fprintf( fp,"HS Indication       : %x\n",t->hsInd );
            fprintf( fp,"PSC Valid           : %x\n",t->pscValid );
            fprintf( fp,"Primary Scrambling Code : %x\n",t->psc );
        }
        if ( pInfo->pLTESysInfo )
        {
            fprintf( fp,"\nLTE SYSTEM INFORMATION ----\n");
            LTESysInfo *t = pInfo->pLTESysInfo;
            doprintsysInfoCommonCB( fp, &t->sysInfoLTE );
            fprintf( fp,"LAC Valid           : %x\n",t->lacValid );
            fprintf( fp,"LAC                 : %x\n",t->lac );
            fprintf( fp,"Cell ID Valid       : %x\n",t->cellIdValid );
            fprintf( fp,"Cell ID             : %lx\n",t->cellId );
            fprintf( fp,"Reg Rej Info Valid  : %x\n",t->regRejectInfoValid );
            fprintf( fp,"Reject Srvc Domain  : %x\n",t->rejectSrvDomain );
            fprintf( fp,"Reject Cause        : %x\n",t->rejCause );
            fprintf( fp,"Network Id Valid    : %x\n",t->networkIdValid );
            fprintf( fp,"MCC info            : " );
            for ( lC = 0 ; lC < PLMN_LENGTH ; lC++ )
            {
                fprintf( fp,"%x ",t->MCC[lC] );
            }
            fprintf( fp,"\n" );
            fprintf( fp,"MNC info            : " );
            for ( lC = 0 ; lC < PLMN_LENGTH ; lC++ )
            {
                fprintf( fp,"%x ",t->MNC[lC] );
            }
            fprintf( fp,"\n" );
            fprintf( fp,"TAC Valid           : %x\n",t->tacValid );
            fprintf( fp,"Tracking Area Code  : %x\n",t->tac );
        }
        if ( pInfo->pAddCDMASysInfo )
        {
            fprintf( fp, "Geo Sys Idx(CDMA) : %x\n", pInfo->pAddCDMASysInfo->geoSysIdx );
            fprintf( fp, "Reg Period (CDMA) : %x\n", pInfo->pAddCDMASysInfo->regPrd );
        }
        IFPRINTF( fp, "Geo Sys Idx(HDR) : %x\n", pInfo->pAddHDRSysInfo );
        if ( pInfo->pAddGSMSysInfo )
        {
            fprintf( fp, "Geo Sys Idx(GSM) : %x\n", pInfo->pAddGSMSysInfo->geoSysIdx );
            fprintf( fp, "Cell Br Cap(GSM) : %lx\n", pInfo->pAddGSMSysInfo->cellBroadcastCap );
        }
        if ( pInfo->pAddWCDMASysInfo )
        {
            fprintf( fp, "Geo Sys Idx(WCDMA) : %x\n", pInfo->pAddWCDMASysInfo->geoSysIdx );
            fprintf( fp, "Cell Br Cap(WCDMA) : %lx\n", pInfo->pAddWCDMASysInfo->cellBroadcastCap );
        }
        IFPRINTF( fp, "Geo Sys Idx(LTE) : %x\n", pInfo->pAddLTESysInfo );
        if ( pInfo->pGSMCallBarringSysInfo )
        {
            fprintf( fp, "CS Bar Status(GSM): %lx\n", pInfo->pGSMCallBarringSysInfo->csBarStatus );
            fprintf( fp, "PS Bar Status(GSM): %lx\n", pInfo->pGSMCallBarringSysInfo->psBarStatus );
        }
        if ( pInfo->pWCDMACallBarringSysInfo )
        {
            fprintf( fp, "CS Bar Status(WCDMA): %lx\n", pInfo->pWCDMACallBarringSysInfo->csBarStatus );
            fprintf( fp, "PS Bar Status(WCDMA): %lx\n", pInfo->pWCDMACallBarringSysInfo->psBarStatus );
        }
        IFPRINTF( fp, "Voice Supp on LTE: %x\n", pInfo->pLTEVoiceSupportSysInfo );
        IFPRINTF( fp, "GSM Cipher Domain: %x\n", pInfo->pGSMCipherDomainSysInfo );
        IFPRINTF( fp, "WCDMA Cipher Domain: %x\n", pInfo->pWCDMACipherDomainSysInfo );
        IFPRINTF( fp, "System Info No Change Indication: %x\n", pInfo->pSysInfoNoChange );
        fprintf( fp, "\n-----------------------xxx-----------------------\n\n");
    }
    tfclose(fp);
}

/*************
 *
 * Name:    doSetSLQSNasSysInfoCallBack
 *
 * Purpose: SLQSNasSysInfoCallBack test function
 *
 **************/
package void doSetSLQSNasSysInfoCallBack( void )
{
    ULONG rc;
    rc = SLQSNasSysInfoCallBack(&cbkTestSLQSNasSysInfoCallBack);

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doSetSLQSNasSysInfoCallBack: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "doSetSLQSNasSysInfoCallBack: Enabled callback\r\n" );
}

/*************
 *
 * Name:    doClearSLQSNasSysInfoCallBack
 *
 * Purpose: Clear the doSLQSNasSysInfoCallBack API
 *
 **************/
package void doClearSLQSNasSysInfoCallBack( void )
{
    ULONG rc;

    rc = SLQSNasSysInfoCallBack( NULL );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearSLQSNasSysInfoCallBack: Failed : %lu\n", rc );
    else
        fprintf( stderr, "doClearSLQSNasSysInfoCallBack: Disabled\n" );
}

/*************
 *
 * Name:    cbkTestSLQSNasNetworkTimeCallBack
 *
 * Purpose: SLQSNasNetworkTimeCallBack API callback function
 *
 **************/
local void cbkTestSLQSNasNetworkTimeCallBack(
    nasNetworkTime *pInfo )
{
    FILE *fp;
    fp = tfopen("../../cbk/test/results/SLQSNasNetworkTimeCallBack.txt", "a");

    if ( fp == NULL )
        perror("cbkTestSLQSNasNetworkTimeCallBack");
    else
    {
        fprintf( fp, "\n------------------------.------------------------\n\n");
        fprintf( fp, "NAS Network Time From SLQSNasNetworkTimeCallBack\n");
        fprintf( fp, "Universal Time \n");
        fprintf( fp, "Year        : %d\n", pInfo->universalTime.year);
        fprintf( fp, "Month       : %d\n", pInfo->universalTime.month);
        fprintf( fp, "Day         : %d\n", pInfo->universalTime.day);
        fprintf( fp, "Hours       : %d\n", pInfo->universalTime.hour);
        fprintf( fp, "Minutes     : %d\n", pInfo->universalTime.minute);
        fprintf( fp, "Seconds     : %d\n", pInfo->universalTime.second);
        fprintf( fp, "Day of Week : %d\n", pInfo->universalTime.dayOfWeek);

        IFPRINTF( fp, "Time Zone          : %x\n", pInfo->pTimeZone );
        IFPRINTF( fp, "Daylight Saving Adj: %x\n", pInfo->pDayltSavAdj );

        fprintf( fp, "\n-----------------------xxx-----------------------\n\n");
    }
    tfclose(fp);
}

/*************
 *
 * Name:    doSetSLQSNasNetworkTimeCallBack
 *
 * Purpose: SLQSNasNetworkTimeCallBack test function
 *
 **************/
package void doSetSLQSNasNetworkTimeCallBack( void )
{
    ULONG rc;
    rc = SLQSNasNetworkTimeCallBack(&cbkTestSLQSNasNetworkTimeCallBack);

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doSetSLQSNasNetworkTimeCallBack: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "doSetSLQSNasNetworkTimeCallBack: Enabled callback\r\n" );
}

/*************
 *
 * Name:    doClearSLQSNasNetworkTimeCallBack
 *
 * Purpose: Clear the doSLQSNasNetworkTimeCallBack API
 *
 **************/
package void doClearSLQSNasNetworkTimeCallBack( void )
{
    ULONG rc;

    rc = SLQSNasNetworkTimeCallBack( NULL );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearSLQSNasNetworkTimeCallBack: Failed : %lu\n", rc );
    else
        fprintf( stderr, "doClearSLQSNasNetworkTimeCallBack: Disabled\n" );
}

/*************
 *
 * Name:    cbkTestSLQSSetMemeryFullInfoCallback
 *
 * Purpose: cbkTestSLQSSetMemeryFullInfoCallback API callback function
 *
 **************/
local void cbkTestSLQSSetMemeryFullInfoCallback(
    SMSMemoryInfo *pInfo )
{
    FILE *fp;

    fp = tfopen("../../cbk/test/results/SLQSSLQSWmsMemoryFullCallBack.txt", "a");

    if ( fp )
    {
        if( pInfo )
        {
            fprintf( fp, "\nMemory Full Information:\n" );
            fprintf( fp, "Storage Type: %x\n", pInfo->storageType );
            fprintf( fp, "Message Mode: %x\n", pInfo->messageMode );
        }
    }
    if( fp )
    tfclose(fp);
}

/*************
 *
 * Name:    doSetSLQSMemoryFullInfoCallback
 *
 * Purpose: SLQSWmsMemoryFullCallBack test function
 *
 **************/
package void doSetSLQSWmsMemoryFullCallBack( void )
{
    ULONG rc;
    rc = SLQSWmsMemoryFullCallBack( &cbkTestSLQSSetMemeryFullInfoCallback );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "SLQSSLQSWmsMemoryFullCallBack: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "SLQSSLQSWmsMemoryFullCallBack: Enabled callback\r\n" );
}

/*************
 *
 * Name:    doClearSLQSWmsMemoryFullCallBack
 *
 * Purpose: Clear the doSLQSWmsMemoryFullCallBack API
 *
 **************/
package void doClearSLQSWmsMemoryFullCallBack( void )
{
    ULONG rc;

    rc = SLQSWmsMemoryFullCallBack( NULL );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearSLQSWmsMemoryFullCallBack: Failed : %lu\n", rc );
    else
        fprintf( stderr, "doClearSLQSWmsMemoryFullCallBack: Disabled\n" );
}

/*************
 *
 * Name:    cbkTestSLQSVoiceInfoRecCallbackCB
 *
 * Purpose: SLQSSetSMSEventCallback API callback function
 *
 **************/
local void cbkTestSLQSVoiceInfoRecCallbackCB(
    voiceInfoRec *pVoiceInfoRec )
{
    FILE *fp;
    voiceInfoRec *pLVACS = pVoiceInfoRec;

    fp = tfopen("../../cbk/test/results/cbkTestSLQSVoiceInfoRecCallbackCB.txt", "a");
    if ( fp == NULL )
        perror("cbkTestSLQSVoiceInfoRecCallbackCB");
    else
    {
        fprintf( fp, "\nCall ID : %d", pLVACS->callID );

        if( pLVACS->pSignalInfo )
        {
            fprintf( fp, "\nSingnal Information:\n" );
            {
                signalInfo *pTemp = pLVACS->pSignalInfo;

                fprintf( fp, "Signal Type:%d\n", pTemp->signalType );
                fprintf( fp, "alertPitch :%d\n", pTemp->alertPitch );
                fprintf( fp, "signal     :%d\n", pTemp->signal );
            }
        }

        if ( pLVACS->pCallerIDInfo )
        {
            fprintf( fp, "Caller Id Information:\n" );
            {
                callerIDInfo *pTemp = pLVACS->pCallerIDInfo;
                fprintf( fp, "PI         :%d\n", pTemp->PI );
                fprintf(fp,"Caller ID :%s",pTemp->callerID);
            }
        }

        IFPRINTF(fp,"Display information     : %d\n",pLVACS->pDispInfo);
        IFPRINTF(fp,"Extended Display info   : %d\n",pLVACS->pExtDispInfo);
        IFPRINTF(fp,"Caller Name information : %d\n",pLVACS->pCallerNameInfo);
        IFPRINTF(fp,"Call wait Indicator     : %d\n",pLVACS->pCallWaitInd);

        if ( pLVACS->pConnectNumInfo )
        {
            fprintf( fp, "Connected Number Information:\n" );
            {
                connectNumInfo *pTemp = pLVACS->pConnectNumInfo;
                fprintf( fp, "PI          :%d\n", pTemp->numPresInd );
                fprintf( fp, "SI          :%d\n", pTemp->screeningInd );
                fprintf( fp, "numType     :%d\n", pTemp->numType );
                fprintf( fp, "numPlan     :%d\n", pTemp->numPlan );
                fprintf(fp,"Connected number Info :%s\n",pTemp->callerID);
            }
        }

        if ( pLVACS->pCallingPartyInfo )
        {
            fprintf( fp, "Calling Party Information:\n" );
            {
                /* Structure Element of connectNumInfo
                   are same as CallingPartyInfo */
                connectNumInfo *pTemp = pLVACS->pCallingPartyInfo;
                fprintf( fp, "PI          :%d\n", pTemp->numPresInd );
                fprintf( fp, "SI          :%d\n", pTemp->screeningInd );
                fprintf( fp, "numType     :%d\n", pTemp->numType );
                fprintf( fp, "numPlan     :%d\n", pTemp->numPlan );
                fprintf( fp, "callerIdlen :%d\n", pTemp->callerIDLen );

                fprintf(fp,"Calling Party Info :%s\n",pTemp->callerID);
            }
        }

        if ( pLVACS->pCalledPartyInfo )
        {
            fprintf( fp, "Called Party Information:\n" );
            {
                calledPartyInfo *pTemp = pLVACS->pCalledPartyInfo;
                fprintf( fp, "PI          :%d\n", pTemp->PI );
                fprintf( fp, "SI          :%d\n", pTemp->SI );
                fprintf( fp, "numType     :%d\n", pTemp->numType );
                fprintf( fp, "numPlan     :%d\n", pTemp->numPlan );
                fprintf( fp, "callerIdlen :%d\n", pTemp->numLen );

                fprintf(fp," Called Party Number :%s\n",pTemp->number);
            }
        }

        if ( pLVACS->pRedirNumInfo )
        {
            fprintf( fp, "Redirecting Number Information:\n" );
            {
                redirNumInfo *pTemp = pLVACS->pRedirNumInfo;
                fprintf( fp, "PI      :%d\n", pTemp->PI );
                fprintf( fp, "SI      :%d\n", pTemp->SI );
                fprintf( fp, "numType :%d\n", pTemp->numType );
                fprintf( fp, "numPlan :%d\n", pTemp->numPlan );
                fprintf( fp, "Reason  :%d\n", pTemp->reason );
                fprintf( fp, "numLen  :%d\n", pTemp->numLen );

                fprintf(fp,"Redirecting Number :%s\n",pTemp->number);
            }
        }
        fprintf( fp, "\n");
        IFPRINTF(fp,"CLIR Cause : %d\n",pLVACS->pCLIRCause);

        if ( pLVACS->pNSSAudioCtrl )
        {
            fprintf( fp, "NSS Audio Control:\n" );
            {
                NSSAudioCtrl *pTemp = pLVACS->pNSSAudioCtrl;
                fprintf( fp, "uplink   :%d\n", pTemp->upLink );
                fprintf( fp, "Downlink :%d\n", pTemp->downLink );
            }
        }
        fprintf( fp, "\n");

        IFPRINTF(fp,"CLIR Cause : %d\n",pLVACS->pNSSRelease);

        if ( pLVACS->pLineCtrlInfo )
        {
            fprintf( fp, "NSS Audio Control:\n" );
            {
                lineCtrlInfo *pTemp = pLVACS->pLineCtrlInfo;
                fprintf( fp, "polarityIncluded:%d\n", pTemp->polarityIncluded );
                fprintf( fp, "toggleMode      :%d\n", pTemp->toggleMode );
                fprintf( fp, "revPolarity     :%d\n", pTemp->revPolarity );
                fprintf( fp, "pwrDenialTime   :%d\n", pTemp->pwrDenialTime );
            }
        }
        fprintf( fp, "\n");

        if ( pLVACS->pExtDispRecInfo )
        {
            fprintf( fp, " Extended display Info record:\n" );
            {
                extDispRecInfo *pTemp = pLVACS->pExtDispRecInfo;
                fprintf( fp, "dispType       :%d\n", pTemp->dispType );
                fprintf( fp, "extDispInfoLen :%d\n", pTemp->extDispInfoLen );
                fprintf( fp, "Extended Disp Info :%s\n",pTemp->extDispInfo);
            }
        }
    }
    tfclose(fp);
}

/*************
 *
 * Name:    doSLQSVoiceInfoRecCallback
 *
 * Purpose: SLQSVoiceInfoRecCallback API test function
 *
 **************/
package void doSetSLQSVoiceInfoRecCallback( void )
{
    ULONG rc;

    rc = SLQSVoiceInfoRecCallback( &cbkTestSLQSVoiceInfoRecCallbackCB );

    if ( rc != eQCWWAN_ERR_NONE )

        fprintf( stderr, "doSLQSVoiceInfoRecCallback: Failed : %lu\r\n", rc );
    else
        printf( "SLQSVoiceInfoRecCallback: Enabled callback\r\n" );
}

/*************
 *
 * Name:    doClearSLQSVoiceInfoRecCallback
 *
 * Purpose: Clear the SLQSVoiceInfoRecCallback API
 *
 **************/

package void doClearSLQSVoiceInfoRecCallback( void )
{
    ULONG rc;
    rc = SLQSVoiceInfoRecCallback( NULL );

    if ( rc != eQCWWAN_ERR_NONE )

        fprintf( stderr, "SLQSVoiceInfoRecCallback: Failed : %lu\n", rc );
    else
        printf( "SLQSVoiceInfoRecCallback: Disabled\n" );
}

/*************
 *
 * Name:    cbkTestSLQSVoiceSetOTASPStatusCB
 *
 * Purpose: SLQSSetSMSEventCallback API callback function
 *
 **************/
local void cbkTestSLQSVoiceSetOTASPStatusCB(
   voiceOTASPStatusInfo *pVoiceOTASPStatusInfo )
{
    FILE *fp;
    voiceOTASPStatusInfo *pLOSI = pVoiceOTASPStatusInfo;

    fp = tfopen("../../cbk/test/results/cbkTestSLQSVoiceSetOTASPStatusCB.txt", "a");
    if ( fp == NULL )
        perror("cbkTestSLQSVoiceSetOTASPStatusCB");
    else
    {
        fprintf( fp, "\nOTASP Status Information :\n" );
        {
            fprintf( fp, "Call Identifier:%d\n", pLOSI->callID);
            fprintf( fp, "OTASP Status   :%d\n", pLOSI->OTASPStatus);
        }

    }
    tfclose(fp);
}

/*************
 *
 * Name:    doSLQSVoiceSetOTASPStatusCallBack
 *
 * Purpose: SLQSVoiceSetOTASPStatusCallBack test function
 *
 **************/
package void doSLQSVoiceSetOTASPStatusCallback( void )
{
    ULONG rc;
    rc = SLQSVoiceSetOTASPStatusCallBack(&cbkTestSLQSVoiceSetOTASPStatusCB);

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "SLQSVoiceSetOTASPStatusCallBack: Failed : %lu\r\n", rc );
    else
        printf( "SLQSVoiceSetOTASPStatusCallBack: Enabled callback\r\n" );
}
/*************
 *
 * Name:    doClearSLQSVoiceSetOTASPStatusCallback
 *
 * Purpose: Clear the SLQSVoiceSetOTASPStatusCallback API
 *
 **************/
package void doClearSLQSVoiceSetOTASPStatusCallback( void )
{
    ULONG rc;

    rc = SLQSVoiceSetOTASPStatusCallBack( NULL );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearSLQSVoiceSetOTASPStatusCallback: Failed : %lu\n", rc );
    else
        printf( "doClearSLQSVoiceSetOTASPStatusCallback: Disabled\n" );
}

local void cbkTestSLQSSetQosNWStatusCallback(
    BYTE status )
{
    FILE *fp;

    fp = tfopen("../../cbk/test/results/SLQSSetQosNWStatusCallback.txt", "a");

    if ( fp )
    {
        fprintf( fp, "\nNetwork Qos Support Status: %d\n", status );
    }
    if( fp )
    tfclose(fp);
}

package void doSetSLQSSetQosNWStatusCallback( void )
{
    ULONG rc;
    rc = SLQSSetQosNWStatusCallback( &cbkTestSLQSSetQosNWStatusCallback );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "SLQSSetQosNWStatusCallback : Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "SLQSSetQosNWStatusCallback : Enabled callback\r\n" );
}

package void doClearSLQSSetQosNWStatusCallback( void )
{
    ULONG rc;

    rc = SLQSSetQosNWStatusCallback( NULL );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearSLQSSetQosNWStatusCallback : Failed : %lu\n", rc );
    else
        fprintf( stderr, "doClearSLQSSetQosNWStatusCallback : Disabled\n" );
}

local void cbkTestSetNetChangeCbk(
    ULONG q_depth,
    BYTE isThrottle )
{
    FILE *fp;

    fp = tfopen("../../cbk/test/results/SetNetChangeCbk.txt", "a");

    if ( fp )
    {
        fprintf( fp, "\n: Queue depth,isThrottle: %lu, %u\n", q_depth, isThrottle );
        tfclose(fp);
    }
}

package void doSetNetChangeCbk( void )
{
    static ULONG period = 1;
    int i;
    ULONG rc;
    for(i=0;i<2;i++)
    {
        rc = SetNetChangeCbk(i, &cbkTestSetNetChangeCbk, 2, 0, period++ );

        if ( rc != eQCWWAN_ERR_NONE )
            fprintf( stderr, "SetNetChangeCbk : Failed : %lu\r\n", rc );
        else
            fprintf( stderr, "SetNetChangeCbk : Enabled callback\r\n" );
    }
}

package void doClearNetChangeCbk( void )
{
    ULONG rc;
    int i;

    for(i=0;i<2;i++)
    {
        rc = SetNetChangeCbk(i, NULL, 0, 0, 0 );

        if ( rc != eQCWWAN_ERR_NONE )
            fprintf( stderr, "%s : Failed : %lu\n", __func__, rc );
        else
            fprintf( stderr, "%s : Disabled\n", __func__ );
    }
}

/*************
 *
 * Name:    cbkTestSLQSSetMessageWaitingCallback
 *
 * Purpose: cbkTestSLQSSetMessageWaitingCallback API callback function
 *
 **************/
local void cbkTestSLQSSetMessageWaitingCallback(
    msgWaitingInfo *pInfo )
{
    FILE *fp;
    BYTE i;

    fp = tfopen("../../cbk/test/results/SLQSWmsMessageWaitingCallBack.txt", "a");

    if ( fp )
    {
        if( pInfo )
        {
            fprintf(fp, "\nreceive Message Waiting Info Indication\n");

            for ( i = 0; i < pInfo->numInstances; i++)
            {
                fprintf(fp, "message type: %d\n", pInfo->msgWaitInfo[i].msgType);
                fprintf(fp, "active indication: %d\n", pInfo->msgWaitInfo[i].activeInd);
                fprintf(fp, "message count: %d\n", pInfo->msgWaitInfo[i].msgCount);
                fprintf(fp, "\n");
            }
        }
    }
    if( fp )
    tfclose(fp);
}

/*************
 *
 * Name:    doSetSLQSWmsMessageWaitingCallBack
 *
 * Purpose: SLQSWmsMessageWaitingCallBack test function
 *
 **************/
package void doSetSLQSWmsMessageWaitingCallBack( void )
{
    ULONG rc;
    rc = SLQSWmsMessageWaitingCallBack( &cbkTestSLQSSetMessageWaitingCallback );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "SLQSWmsMessageWaitingCallBack: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "SLQSWmsMessageWaitingCallBack: Enabled callback\r\n" );
}

/*************
 *
 * Name:    doClearSLQSWmsMessageWaitingCallBack
 *
 * Purpose: Clear the doSetSLQSWmsMessageWaitingCallBack API
 *
 **************/
package void doClearSLQSWmsMessageWaitingCallBack( void )
{
    ULONG rc;

    rc = SLQSWmsMessageWaitingCallBack( NULL );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearSLQSWmsMessageWaitingCallBack: Failed : %lu\n", rc );
    else
        fprintf( stderr, "doClearSLQSWmsMessageWaitingCallBack: Disabled\n" );
}

/*************
 *
 * Name:    cbkTestSLQSNasSigInfoCallBack
 *
 * Purpose: SLQSNasSigInfoCallBack API callback function
 *
 **************/
local void cbkTestSLQSNasSigInfoCallBack(
    nasSigInfo *pInfo )
{
    FILE *fp;
    fp = tfopen("../../cbk/test/results/SLQSNasSigInfoCallBack.txt", "a");

    if ( fp == NULL )
        perror("cbkTestSLQSNasSigInfoCallBack");
    else
    {
        fprintf( fp, "\n------------------------.------------------------\n\n");
        fprintf( fp, "NAS Signal Strength Information From SLQSNasSigInfoCallBack\n");
        if ( pInfo->pCDMASigInfo )
        {
            fprintf( fp,"RSSI(CDMA) : %x\n",pInfo->pCDMASigInfo->rssi );
            fprintf( fp,"ECIO(CDMA) : %x\n",pInfo->pCDMASigInfo->ecio );
        }
        if ( pInfo->pHDRSigInfo )
        {
            fprintf( fp,"RSSI(HDR) : %x\n",pInfo->pHDRSigInfo->rssi );
            fprintf( fp,"ECIO(HDR) : %x\n",pInfo->pHDRSigInfo->ecio );
            fprintf( fp,"SINR(HDR) : %x\n",pInfo->pHDRSigInfo->sinr );
            fprintf( fp,"IO(HDR)   : %lu\n",pInfo->pHDRSigInfo->io );
        }
        if ( pInfo->pGSMSigInfo )
        {
            fprintf( fp,"GSM Signal Info(GSM)      : %d\n",*pInfo->pGSMSigInfo );
        }
        if ( pInfo->pWCDMASigInfo )
        {
            fprintf( fp,"RSSI(WCDMA)  : %x\n",pInfo->pWCDMASigInfo->rssi );
            fprintf( fp,"ECIO(WCDMA)  : %x\n",pInfo->pWCDMASigInfo->ecio );
        }
        if ( pInfo->pLTESigInfo )
        {
            fprintf( fp,"RSSI(LTE)  : %x\n",pInfo->pLTESigInfo->rssi );
            fprintf( fp,"RSRQ(LTE)  : %x\n",pInfo->pLTESigInfo->rsrq );
            fprintf( fp,"RSRP(LTE)  : %x\n",pInfo->pLTESigInfo->rsrp );
            fprintf( fp,"SNR(LTE)   : %x\n",pInfo->pLTESigInfo->snr );
        }
        if ( pInfo->pTDSCDMASigInfo )
        {
            fprintf( fp,"RSCP(TDSCDMA)  : %x\n",pInfo->pTDSCDMASigInfo->rscp );
        }
        if ( pInfo->pTDSCDMASigInfoExt )
        {
            fprintf( fp,"RSSI(TDSCDMAEXT)  : %lx\n",pInfo->pTDSCDMASigInfoExt->rssi );
            fprintf( fp,"RSCP(TDSCDMAEXT)  : %lx\n",pInfo->pTDSCDMASigInfoExt->rscp );
            fprintf( fp,"ECIO(TDSCDMAEXT)  : %lx\n",pInfo->pTDSCDMASigInfoExt->ecio );
            fprintf( fp,"SINR(TDSCDMAEXT)  : %lx\n",pInfo->pTDSCDMASigInfoExt->sinr );
        }
    }
    tfclose(fp);
}

/*************
 *
 * Name:    doSetSLQSNasSigInfoCallBack
 *
 * Purpose: SLQSNasSigInfoCallBack test function
 *
 **************/
package void doSetSLQSNasSigInfoCallBack( void )
{
    ULONG rc;
    rc = SLQSNasSigInfoCallBack(&cbkTestSLQSNasSigInfoCallBack);

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doSetSLQSNasSigInfoCallBack: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "doSetSLQSNasSigInfoCallBack: Enabled callback\r\n" );
}

/*************
 *
 * Name:    doClearSLQSNasSigInfoCallBack
 *
 * Purpose: Clear the doSLQSNasSigInfoCallBack API
 *
 **************/
package void doClearSLQSNasSigInfoCallBack( void )
{
    ULONG rc;

    rc = SLQSNasSigInfoCallBack( NULL );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearSLQSNasSigInfoCallBack: Failed : %lu\n", rc );
    else
        fprintf( stderr, "doClearSLQSNasSigInfoCallBack: Disabled\n" );
}

/*************
 *
 * Name:    cbkTestSLQSSetModemTempCallback
 *
 * Purpose: SLQSSetModemTempCallback API callback function
 *
 **************/
local void cbkTestSLQSSetModemTempCallback(
    modemTempNotification *pModemTempNotification )
{
    FILE *fp;

    fp = tfopen("TestReport/SLQSSetModemTempCallback.txt", "a");

    if ( fp == NULL )
        perror("cbkTestSLQSSetModemTempCallback");
    else
    {
        fprintf( fp, "Modem Temperature State 0x%x \n",
                 pModemTempNotification->ModemTempState );

        fprintf( fp, "Modem Temperature (in degree celsius) 0x%x \n",
                 pModemTempNotification->ModemTemperature );

        fprintf( fp, "\n\n" );
    }
    tfclose(fp);
}

/*************
 *
 * Name:    doSLQSSetModemTempCallback
 *
 * Purpose: SLQSSetModemTempCallback test function
 *
 **************/
package void doSLQSSetModemTempCallback( void )
{
    ULONG rc;
    rc = SLQSSetModemTempCallback( &cbkTestSLQSSetModemTempCallback );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "SLQSSetModemTempCallback: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "SLQSSetModemTempCallback: Enabled callback\r\n" );
}

/*************
 *
 * Name:    doClearSLQSSetModemTempCallback
 *
 * Purpose: Clear the doSLQSSetModemTempCallback API
 *
 **************/
package void doClearSLQSSetModemTempCallback( void )
{
    ULONG rc;

    rc = SLQSSetModemTempCallback( NULL );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearSLQSSetModemTempCallback: Failed : %lu\n", rc );
    else
        fprintf( stderr, "doClearSLQSSetModemTempCallback: Disabled\n" );
}

/*************
 *
 * Name:    cbkTestSLQSSetPacketSrvStatCallback
 *
 * Purpose: SLQSSetPacketSrvStatusCallback API callback function
 *
 * Parms:   pPacketSrvStatus - Packet Service Status
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:
 *
 **************/
local void cbkTestSLQSSetPacketSrvStatCallback(
    packetSrvStatus *pPacketSrvStatus )
{
    FILE *fp;

    fp = tfopen("TestReport/slqssetpacketsrvstatuscallback.txt", "a");
    if( fp == NULL )
    {
        perror("cbkTestSLQSSetPacketSrvStatusCallback");
        return;
    }

    fprintf( fp,
             "\nInterface Instance ID: %d\tService %d\tv4 SID %lu\tv6 SID %lu\n",
             pPacketSrvStatus->pQmiInterfaceInfo->qaQmiinstanceid,
             pPacketSrvStatus->pQmiInterfaceInfo->qaQmisvctype,
             pPacketSrvStatus->pQmiInterfaceInfo->v4sessionId,
             pPacketSrvStatus->pQmiInterfaceInfo->v6sessionId );

    fprintf( fp, "State: %d\n", pPacketSrvStatus->connStatus );
    fprintf( fp, "Reconfig. Required: %d\n", pPacketSrvStatus->reconfigReqd );
    fprintf( fp, "Session End Reason: %d\n",
             pPacketSrvStatus->sessionEndReason );
    fprintf( fp, "Verbose Session End Reason Type: %d\n",
             pPacketSrvStatus->verboseSessnEndReasonType );
    fprintf( fp, "Verbose Session End Reason: %d\n",
             pPacketSrvStatus->verboseSessnEndReason );
    fprintf( fp, "IP Family: %d\n", pPacketSrvStatus->ipFamily );
    fprintf( fp, "Tech. Name: %d\n", pPacketSrvStatus->techName );
    tfclose(fp);
}

/*************
 *
 * Name:    doSLQSSetPacketSrvStatusCallback
 *
 * Purpose: SLQSSetPacketSrvStatusCallback test function
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
package void doSLQSSetPacketSrvStatusCallback( void )
{
    ULONG rc;

    rc = SLQSSetPacketSrvStatusCallback(&cbkTestSLQSSetPacketSrvStatCallback);

    if( rc != eQCWWAN_ERR_NONE )
    {
        fprintf( stderr,
                 "doSLQSSetPacketSrvStatusCallback: failed to enable callback\n" );
    }
    else
    {
        fprintf( stderr, "doSLQSSetPacketSrvStatusCallback: enabled callback\n" );
    }
}

/*************
 *
 * Name:    doClearSLQSSetPacketSrvStatusCallback
 *
 * Purpose: clear the SLQSSetPacketSrvStatusCallback API
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
package void doClearSLQSSetPacketSrvStatusCallback( void )
{
    ULONG rc;

    rc = SLQSSetPacketSrvStatusCallback(NULL);

    if( rc != eQCWWAN_ERR_NONE )
    {
        fprintf( stderr,
                 "doClearSLQSSetPacketSrvStatusCallback: failed to disable callback\n" );
    }
    else
    {
        fprintf( stderr,
                 "doClearSLQSSetPacketSrvStatusCallback: disabled\n" );
    }
}

/*************
 *
 * Name:    cbkTestSLQSSetSwiHDRPersCallBack
 *
 * Purpose: SLQSNasHDRPersCallBack API callback function
 *
 **************/
local void cbkTestSLQSSetSwiHDRPersCallBack(
    HDRPersonalityInd *pInfo )
{
    FILE *fp;
    BYTE count;
    fp = tfopen("../../cbk/test/results/SLQSNasSwiHDRPersCallBack.txt", "a");

    if ( fp == NULL )
        perror("cbkTestSLQSSetSwiHDRPersCallBack");
    else
    {
        fprintf( fp, "\n------------------------.------------------------\n\n");
        fprintf( fp, "NAS HDR Personality From SLQSSetSwiHDRPersCallback\n");

        IFPRINTF( fp, "Current Personality  : 0x%x\n", pInfo->pCurrentPersonality );
        IFPRINTF( fp, "PersonalityListLength:0x%x\n", pInfo->pPersonalityListLength );

        if( NULL != pInfo->pPersonalityListLength &&
            NULL != pInfo->pProtocolSubtypeElement )
        {
            for( count = 0; count < *(pInfo->pPersonalityListLength); count++ )
            {
                fprintf( fp, "Accessmac      : %d\n", pInfo->pProtocolSubtypeElement[count].AccessMac );
                fprintf( fp, "AuthPort       : %d\n", pInfo->pProtocolSubtypeElement[count].AuthProt );
                fprintf( fp, "Controlmac     : %d\n", pInfo->pProtocolSubtypeElement[count].ControlMac );
                fprintf( fp, "EncryptProt    : %d\n", pInfo->pProtocolSubtypeElement[count].EncryptProt );
                fprintf( fp, "ForwardMac     : %d\n", pInfo->pProtocolSubtypeElement[count].ForwardMac );
                fprintf( fp, "Idle State     : %d\n", pInfo->pProtocolSubtypeElement[count].IdleState );
                fprintf( fp, "KeyExchange    : %d\n", pInfo->pProtocolSubtypeElement[count].KeyExchange );
                fprintf( fp, "MultDisc       : %d\n", pInfo->pProtocolSubtypeElement[count].MultDisc );
                fprintf( fp, "Physical Layer : %d\n", pInfo->pProtocolSubtypeElement[count].PhysicalLayer );
                fprintf( fp, "Reversemac     : %d\n", pInfo->pProtocolSubtypeElement[count].ReverseMac );
                fprintf( fp, "Secprot        : %d\n", pInfo->pProtocolSubtypeElement[count].SecProt );
                fprintf( fp, "VirtStream     : %d\n", pInfo->pProtocolSubtypeElement[count].VirtStream );
            }
        }

    }
    tfclose(fp);
}

/*************
 *
 * Name:    doSetSLQSSetSwiHDRPersCallBack
 *
 * Purpose: SLQSSetSwiHDRPersCallback test function
 *
 **************/
package void doSetSLQSSetSwiHDRPersCallBack( void )
{
    ULONG rc;
    rc = SLQSSetSwiHDRPersCallback(&cbkTestSLQSSetSwiHDRPersCallBack);

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doSetSLQSNasHDRPersCallBack: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "doSetSLQSNasHDRPersCallBack: Enabled callback\r\n" );
}

/*************
 *
 * Name:    doClearSLQSSetSwiHDRPersCallBack
 *
 * Purpose: Clear the SLQSSetSwiHDRPersCallback API
 *
 **************/
package void doClearSLQSSetSwiHDRPersCallBack( void )
{
    ULONG rc;

    rc = SLQSSetSwiHDRPersCallback( NULL );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearSLQSNasHDRPersCallBack: Failed : %lu\n", rc );
    else
        fprintf( stderr, "doClearSLQSNasHDRPersCallBack: Disabled\n" );
}


/*************
 *
 * Name:    cbkTestSLQSSetSIPConfigCallback
 *
 * Purpose: SLQSSetSIPConfigCallback API callback function
 *
 **************/
local void cbkTestSLQSSetSIPConfigCallback(
    imsSIPConfigInfo *pImsSIPConfigInfo )
{
    FILE *fp;

    fp = tfopen("TestReport/SLQSSetSIPConfigCallback.txt", "a");

    if ( fp == NULL )
        perror("cbkTestSLQSSetSIPConfigCallback");
    else
    {
        fprintf( fp, "SIP Port Number 0x%x \n",
                 *pImsSIPConfigInfo->pSIPLocalPort );

        fprintf( fp, "SIP Registration Timer 0x%lx \n",
                 *pImsSIPConfigInfo->pTimerSIPReg );

        fprintf( fp, "Subscribe Timer 0x%lx \n",
                 *pImsSIPConfigInfo->pSubscribeTimer );

        fprintf( fp, "Timer T1 0x%lx \n",
                 *pImsSIPConfigInfo->pTimerT1 );

        fprintf( fp, "Timer T2 0x%lx \n",
                 *pImsSIPConfigInfo->pTimerT2 );

        fprintf( fp, "Timer Tf 0x%lx \n",
                 *pImsSIPConfigInfo->pTimerTf );

        fprintf( fp, "SigComp Enabled 0x%x \n",
                 *pImsSIPConfigInfo->pSigCompEnabled );

        fprintf( fp, "\n\n" );
    }
    tfclose(fp);
}

/*************
 *
 * Name:    doSLQSSetSIPConfigCallback
 *
 * Purpose: SLQSSetSIPConfigCallback test function
 *
 **************/
package void doSLQSSetSIPConfigCallback( void )
{
    ULONG rc;
    rc = SLQSSetSIPConfigCallback( &cbkTestSLQSSetSIPConfigCallback );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "SLQSSetSIPConfigCallback: Failed : %lu\r\n", rc );
    else
        printf( "SLQSSetSIPConfigCallback: Enabled callback\r\n" );
}

/*************
 *
 * Name:    doClearSLQSSetSIPConfigCallback
 *
 * Purpose: Clear the doSLQSSetSIPConfigCallback API
 *
 **************/
package void doClearSLQSSetSIPConfigCallback( void )
{
    ULONG rc;

    rc = SLQSSetSIPConfigCallback( NULL );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearSLQSSetSIPConfigCallback: Failed : %lu\n", rc );
    else
        printf( "doClearSLQSSetSIPConfigCallback: Disabled\n" );
}

/*************
 *
 * Name:    cbkTestSLQSSetRegMgrConfigCallback
 *
 * Purpose: SLQSSetRegMgrConfigCallback API callback function
 *
 **************/
local void cbkTestSLQSSetRegMgrConfigCallback(
    imsRegMgrConfigInfo *pImsRegMgrConfigInfo )
{
    FILE *fp;

    fp = tfopen("TestReport/SLQSSetRegMgrConfigCallback.txt", "a");

    if ( fp == NULL )
        perror("cbkTestSLQSSetRegMgrConfigCallback");
    else
    {
        fprintf( fp, "Primary CSCF Port 0x%x \n",
                 *pImsRegMgrConfigInfo->pPriCSCFPort );

        fprintf( fp, "CSCF Port Name %s \n",
                 pImsRegMgrConfigInfo->pCSCFPortName );

        fprintf( fp, "IMS Test Mode 0x%x \n",
                 *pImsRegMgrConfigInfo->pIMSTestMode );

        fprintf( fp, "\n\n" );
    }
    tfclose(fp);
}

/*************
 *
 * Name:    doSLQSSetRegMgrConfigCallback
 *
 * Purpose: SLQSSetRegMgrConfigCallback test function
 *
 **************/
package void doSLQSSetRegMgrConfigCallback( void )
{
    ULONG rc;
    rc = SLQSSetRegMgrConfigCallback( &cbkTestSLQSSetRegMgrConfigCallback );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "SLQSSetRegMgrConfigCallback: Failed : %lu\r\n", rc );
    else
        printf( "SLQSSetRegMgrConfigCallback: Enabled callback\r\n" );
}

/*************
 *
 * Name:    doClearSLQSSetRegMgrConfigCallback
 *
 * Purpose: Clear the doSLQSSetRegMgrConfigCallback API
 *
 **************/
package void doClearSLQSSetRegMgrConfigCallback( void )
{
    ULONG rc;

    rc = SLQSSetRegMgrConfigCallback( NULL );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearSLQSSetRegMgrConfigCallback: Failed : %lu\n", rc );
    else
        printf( "doClearSLQSSetRegMgrConfigCallback: Disabled\n" );
}

/*************
 *
 * Name:    cbkTestSLQSSetSMSConfigCallback
 *
 * Purpose: SLQSSetSMSConfigCallback API callback function
 *
 **************/
local void cbkTestSLQSSetSMSConfigCallback(
    imsSMSConfigInfo *pImsSMSConfigInfo )
{
    FILE *fp;

    fp = tfopen("TestReport/SLQSSetSMSConfigCallback.txt", "a");

    if ( fp == NULL )
        perror("cbkTestSLQSSetSMSConfigCallback");
    else
    {
        fprintf( fp, "SMS Format 0x%x \n",
                 *pImsSMSConfigInfo->pSMSFormat );

        fprintf( fp, "SMS over IP Nw indication flag 0x%x \n",
                 *pImsSMSConfigInfo->pSMSOverIPNwInd );

        fprintf( fp, "Phone Context URI %s \n",
                 pImsSMSConfigInfo->pPhoneCtxtURI );

        fprintf( fp, "\n\n" );
    }
    tfclose(fp);
}

/*************
 *
 * Name:    doSLQSSetSMSConfigCallback
 *
 * Purpose: SLQSSetSMSConfigCallback test function
 *
 **************/
package void doSLQSSetSMSConfigCallback( void )
{
    ULONG rc;
    rc = SLQSSetIMSSMSConfigCallback( &cbkTestSLQSSetSMSConfigCallback );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "SLQSSetSMSConfigCallback: Failed : %lu\r\n", rc );
    else
        printf( "SLQSSetSMSConfigCallback: Enabled callback\r\n" );
}

/*************
 *
 * Name:    doClearSLQSSetSMSConfigCallback
 *
 * Purpose: Clear the doSLQSSetSMSConfigCallback API
 *
 **************/
package void doClearSLQSSetSMSConfigCallback( void )
{
    ULONG rc;

    rc = SLQSSetIMSSMSConfigCallback( NULL );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearSLQSSetSMSConfigCallback: Failed : %lu\n", rc );
    else
        printf( "doClearSLQSSetSMSConfigCallback: Disabled\n" );
}

/*************
 *
 * Name:    cbkTestSLQSSetUserConfigCallback
 *
 * Purpose: SLQSSetUserConfigCallback API callback function
 *
 **************/
local void cbkTestSLQSSetUserConfigCallback(
    imsUserConfigInfo *pImsUserConfigInfo )
{
    FILE *fp;

    fp = tfopen("TestReport/SLQSSetUserConfigCallback.txt", "a");

    if ( fp == NULL )
        perror("cbkTestSLQSSetUserConfigCallback");
    else
    {
        fprintf( fp, "IMS Domain Name %s \n",
                 pImsUserConfigInfo->pIMSDomain );

        fprintf( fp, "\n\n" );
    }
    tfclose(fp);
}

/*************
 *
 * Name:    doSLQSSetUserConfigCallback
 *
 * Purpose: SLQSSetUserConfigCallback test function
 *
 **************/
package void doSLQSSetUserConfigCallback( void )
{
    ULONG rc;
    rc = SLQSSetIMSUserConfigCallback( &cbkTestSLQSSetUserConfigCallback );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "SLQSSetUserConfigCallback: Failed : %lu\r\n", rc );
    else
        printf( "SLQSSetUserConfigCallback: Enabled callback\r\n" );
}

/*************
 *
 * Name:    doClearSLQSSetUserConfigCallback
 *
 * Purpose: Clear the doSLQSSetUserConfigCallback API
 *
 **************/
package void doClearSLQSSetUserConfigCallback( void )
{
    ULONG rc;

    rc = SLQSSetIMSUserConfigCallback( NULL );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearSLQSSetUserConfigCallback: Failed : %lu\n", rc );
    else
        printf( "doClearSLQSSetUserConfigCallback: Disabled\n" );
}

/*************
 *
 * Name:    cbkTestSLQSSetVoIPConfigCallback
 *
 * Purpose: SLQSSetVoIPConfigCallback API callback function
 *
 **************/
local void cbkTestSLQSSetVoIPConfigCallback(
    imsVoIPConfigInfo *pImsVoIPConfigInfo )
{
    FILE *fp;

    fp = tfopen("TestReport/SLQSSetVoIPConfigCallback.txt", "a");

    if ( fp == NULL )
        perror("cbkTestSLQSSetVoIPConfigCallback");
    else
    {
        fprintf( fp, "Session Expiry Timer: 0x%x \n",
                 *pImsVoIPConfigInfo->pSessionExpiryTimer );

        fprintf( fp, "Minimum Session Expiry Timer: 0x%x \n",
                 *pImsVoIPConfigInfo->pMinSessionExpiryTimer );

        fprintf( fp, "AMR WB Enable: 0x%x \n",
                 *pImsVoIPConfigInfo->pAmrWbEnable );

        fprintf( fp, "SCR AMR Enable: 0x%x \n",
                 *pImsVoIPConfigInfo->pScrAmrEnable );

        fprintf( fp, "SCR AMR WB Enable: 0x%x \n",
                 *pImsVoIPConfigInfo->pScrAmrWbEnable );

        fprintf( fp, "AMR Mode: 0x%x \n",
                 *pImsVoIPConfigInfo->pAmrMode );

        fprintf( fp, "AMR WB Mode: 0x%x \n",
                 *pImsVoIPConfigInfo->pAmrWBMode );

        fprintf( fp, "AMR Octet Aligned: 0x%x \n",
                 *pImsVoIPConfigInfo->pAmrOctetAligned );

        fprintf( fp, "AMR WB Octet Aligned: 0x%x \n",
                 *pImsVoIPConfigInfo->pAmrWBOctetAligned );

        fprintf( fp, "Ringing Timer: 0x%x \n",
                 *pImsVoIPConfigInfo->pRingingTimer );

        fprintf( fp, "Ringback Timer: 0x%x \n",
                 *pImsVoIPConfigInfo->pRingBackTimer );

        fprintf( fp, "RTP/RTCP Inactivity Timer: 0x%x \n",
                 *pImsVoIPConfigInfo->pRTPRTCPInactTimer );

        fprintf( fp, "\n\n" );
    }
    tfclose(fp);
}

/*************
 *
 * Name:    doSLQSSetVoIPConfigCallback
 *
 * Purpose: SLQSSetVoIPConfigCallback test function
 *
 **************/
package void doSLQSSetVoIPConfigCallback( void )
{
    ULONG rc;
    rc = SLQSSetIMSVoIPConfigCallback( &cbkTestSLQSSetVoIPConfigCallback );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "SLQSSetVoIPConfigCallback: Failed : %lu\r\n", rc );
    else
        printf( "SLQSSetVoIPConfigCallback: Enabled callback\r\n" );
}

/*************
 *
 * Name:    doClearSLQSSetVoIPConfigCallback
 *
 * Purpose: Clear the doSLQSSetVoIPConfigCallback API
 *
 **************/
package void doClearSLQSSetVoIPConfigCallback( void )
{
    ULONG rc;

    rc = SLQSSetIMSVoIPConfigCallback( NULL );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearSLQSSetVoIPConfigCallback: Failed : %lu\n", rc );
    else
        printf( "doClearSLQSSetVoIPConfigCallback: Disabled\n" );
}

local void cbkTestSLQSSetQosStatusCallback(
        BYTE    instance,
        ULONG   id,
        BYTE    status,
        BYTE    event,
        BYTE    reason
        )
{
    FILE *fp;

    fp = tfopen("../../cbk/test/results/SLQSSetQosStatusCallback.txt", "a");

    if ( fp )
    {
        fprintf( fp, "\nQos Flow Status QMI/id/status/event/reason: %d/0x%lx/%d/%d/0x%x\n",
                instance, id, status, event, reason);
    }
    if( fp ) tfclose(fp);
}

package void doSetSLQSSetQosStatusCallback( void )
{
    ULONG rc;
    BYTE instance = 0;

    rc = SLQSSetQosStatusCallback(instance, &cbkTestSLQSSetQosStatusCallback );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doSetSLQSSetQosStatusCallback : Failed : %lu\n", rc );
    else
        fprintf( stderr, "doSetSLQSSetQosStatusCallback : Enabled callback\n" );
}

package void doClearSLQSSetQosStatusCallback( void )
{
    ULONG rc;
    BYTE instance = 0;

    rc = SLQSSetQosStatusCallback(instance, NULL );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearSLQSSetQosStatusCallback : Failed : %lu\n", rc );
    else
        fprintf( stderr, "doClearSLQSSetQosStatusCallback : Disabled\n" );
}

local void cbkTestSLQSSetQosPriEventCallback( WORD event )
{
    FILE *fp;

    fp = tfopen("../../cbk/test/results/SLQSSetQosPriEventCallback.txt", "a");

    if ( fp )
    {
        fprintf( fp, "\nQos Primary Flow modify operation: %d\n", event);
    }
    if( fp ) tfclose(fp);
}

package void doSetSLQSSetQosPriEventCallback( void )
{
    ULONG rc;

    rc = SLQSSetQosPriEventCallback(&cbkTestSLQSSetQosPriEventCallback );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doSetSLQSSetQosPriEventCallback : Failed : %lu\n", rc );
    else
        fprintf( stderr, "doSetSLQSSetQosPriEventCallback : Enabled callback\n" );
}

package void doClearSLQSSetQosPriEventCallback( void )
{
    ULONG rc;

    rc = SLQSSetQosPriEventCallback( NULL );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearSLQSSetQosPriEventCallback : Failed : %lu\n", rc );
    else
        fprintf( stderr, "doClearSLQSSetQosPriEventCallback : Disabled\n" );
}

/*************
 *
 * Name:    cbkTestSLQSSetIMSARegStatusCallback
 *
 * Purpose: SLQSSetIMSARegStatusCallback API callback function
 *
 **************/
local void cbkTestSLQSSetIMSARegStatusCallback(
    imsaRegStatusInfo *pImsaRegStatusInfo )
{
    FILE *fp;

    fp = tfopen("TestReport/SLQSSetIMSARegStatusCallback.txt", "a");

    if ( fp == NULL )
        perror("cbkTestSLQSSetIMSARegStatusCallback");
    else
    {
        fprintf( fp, "IMS Registration Status 0x%x \n",
                 *pImsaRegStatusInfo->pbIMSRegistered );

        fprintf( fp, "IMS Registration Error Code 0x%x \n",
                 *pImsaRegStatusInfo->pRegStatusErrorCode );

        fprintf( fp, "New IMS Reg Status %ul \n",
                 *pImsaRegStatusInfo->pImsRegStatus );

        fprintf( fp, "\n\n" );
    }
    tfclose(fp);
}

/*************
 *
 * Name:    doSetSLQSSetIMSARegStatusCallback
 *
 * Purpose: SLQSSetIMSARegStatusCallback test function
 *
 **************/
package void doSetSLQSSetIMSARegStatusCallback( void )
{
    ULONG rc;
    rc = SLQSSetIMSARegStatusCallback( &cbkTestSLQSSetIMSARegStatusCallback );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "SLQSSetIMSARegStatusCallback: Failed : %lu\r\n", rc );
    else
        printf( "SSLQSSetIMSARegStatusCallback: Enabled callback\r\n" );
}

/*************
 *
 * Name:    doClearSLQSSetIMSARegStatusCallback
 *
 * Purpose: Clear the doSetSLQSSetIMSARegStatusCallback API
 *
 **************/
package void doClearSLQSSetIMSARegStatusCallback( void )
{
    ULONG rc;

    rc = SLQSSetIMSARegStatusCallback( NULL );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearSLQSSetIMSARegStatusCallback: Failed : %lu\n", rc );
    else
        printf( "doClearSLQSSetIMSARegStatusCallback: Disabled\n" );
}

/*************
 *
 * Name:    cbkTestSLQSSetIMSASvcStatusCallback
 *
 * Purpose: SLQSSetIMSASvcStatusCallback API callback function
 *
 **************/
local void cbkTestSLQSSetIMSASvcStatusCallback(
    imsaSvcStatusInfo *pImsaSvcStatusInfo )
{
    FILE *fp;

    fp = tfopen("TestReport/SLQSSetIMSASvcStatusCallback.txt", "a");

    if ( fp == NULL )
        perror("cbkTestSLQSSetIMSASvcStatusCallback");
    else
    {
        fprintf( fp, "SMS Service Status %lu \n",
                 *pImsaSvcStatusInfo->pSMSSvcStatus );

        fprintf( fp, "VOIP Service Status %lu \n",
                 *pImsaSvcStatusInfo->pVOIPSvcStatus );

        fprintf( fp, "VT Service Status %lu \n",
                 *pImsaSvcStatusInfo->pVTSvcStatus );

        fprintf( fp, "SMS Service RAT %lu \n",
                 *pImsaSvcStatusInfo->pSMSSvcRAT );

        fprintf( fp, "VOIP Service RAT %lu \n",
                 *pImsaSvcStatusInfo->pVOIPSvcRAT );

        fprintf( fp, "VT Service RAT %lu \n",
                 *pImsaSvcStatusInfo->pVTSvcRAT );

        fprintf( fp, "UT Service Status %lu \n",
                 *pImsaSvcStatusInfo->pUTSvcStatus );

        fprintf( fp, "UT Service RAT %lu \n",
                 *pImsaSvcStatusInfo->pUTSvcRAT );

        fprintf( fp, "\n\n" );
    }

    tfclose(fp);
}

/*************
 *
 * Name:    doSetSLQSSetIMSASvcStatusCallback
 *
 * Purpose: SLQSSetIMSASvcStatusCallback test function
 *
 **************/
package void doSetSLQSSetIMSASvcStatusCallback( void )
{
    ULONG rc;
    rc = SLQSSetIMSASvcStatusCallback( &cbkTestSLQSSetIMSASvcStatusCallback );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "SLQSSetIMSASvcStatusCallback: Failed : %lu\r\n", rc );
    else
        printf( "SLQSSetIMSASvcStatusCallback: Enabled callback\r\n" );
}

/*************
 *
 * Name:    doClearSLQSSetIMSASvcStatusCallback
 *
 * Purpose: Clear the doSetSLQSSetIMSASvcStatusCallback API
 *
 **************/
package void doClearSLQSSetIMSASvcStatusCallback( void )
{
    ULONG rc;

    rc = SLQSSetIMSASvcStatusCallback( NULL );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearSLQSSetIMSASvcStatusCallback: Failed : %lu\n", rc );
    else
        printf( "doClearSLQSSetIMSASvcStatusCallback: Disabled\n" );
}

/*************
 *
 * Name:    cbkTestSLQSSetIMSARatStatusCallback
 *
 * Purpose: SLQSSetIMSARatStatusCallback API callback function
 *
 **************/
local void cbkTestSLQSSetIMSARatStatusCallback(
    imsaRatStatusInfo *pImsaRatStatusInfo )
{
    FILE *fp;

    fp = tfopen("TestReport/SLQSSetIMSARatStatusCallback.txt", "a");

    if ( fp == NULL )
        perror("cbkTestSLQSSetIMSARatStatusCallback");
    else
    {
        fprintf( fp, "RAT Status %lu \n",
                 *pImsaRatStatusInfo->pRATStatus );

        fprintf( fp, "Source RAT %lu \n",
                 *pImsaRatStatusInfo->pSrcRAT );

        fprintf( fp, "Target RAT %lu \n",
                 *pImsaRatStatusInfo->pTgtRAT );

        fprintf( fp, "\n\n" );
    }
    tfclose(fp);
}

/*************
 *
 * Name:    doSetSLQSSetIMSARatStatusCallback
 *
 * Purpose: SLQSSetIMSARatStatusCallback test function
 *
 **************/
package void doSetSLQSSetIMSARatStatusCallback( void )
{
    ULONG rc;
    rc = SLQSSetIMSARatStatusCallback( &cbkTestSLQSSetIMSARatStatusCallback );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "SLQSSetIMSARatStatusCallback: Failed : %lu\r\n", rc );
    else
        printf( "SSLQSSetIMSARatStatusCallback: Enabled callback\r\n" );
}

/*************
 *
 * Name:    doClearSLQSSetIMSARatStatusCallback
 *
 * Purpose: Clear the doSetSLQSSetIMSARatStatusCallback API
 *
 **************/
package void doClearSLQSSetIMSARatStatusCallback( void )
{
    ULONG rc;

    rc = SLQSSetIMSARatStatusCallback( NULL );

    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearSLQSSetIMSARatStatusCallback: Failed : %lu\n", rc );
    else
        printf( "doClearSLQSSetIMSARatStatusCallback: Disabled\n" );
}

/*************
 *
 * Name:    cbkTestSetLocCradleMountCallback
 *
 * Purpose: SetLocCradleMountCallback API callback function
 *
 **************/
local void cbkTestSetLocCradleMountCallback( QmiCbkLocCradleMountInd *pMsg)
{
    FILE *fp;

    fp = tfopen("../../cbk/test/results/cbkTestSetLocCradleMountCallback.txt", "a");
    if ( fp == NULL )
        perror("cbkTestSetLocCradleMountCallback");
    else
    {
        if (pMsg != NULL)
        {
            fprintf( fp, "\nLOC Cradle Mount Status: %ld\n",
                         pMsg->cradleMountConfigStatus);
        }
        tfclose(fp);
    }
}

/*************
 *
 * Name:    doSetLocCradleMountCallback
 *
 * Purpose: Set the SetLocCradleMountCallback API
 *
 **************/
package void doSetLocCradleMountCallback(void)

{
    ULONG rc;
    rc = SetLocCradleMountCallback(  &cbkTestSetLocCradleMountCallback );
    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doSetLocCradleMountCallback: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "doSetLocCradleMountCallback: Enabled callback\r\n" );

}

/*************
 *
 * Name:    doClearLocCradleMountCallback
 *
 * Purpose: Clear the SetLocCradleMountCallback API
 *
 **************/
package void doClearLocCradleMountCallback(void)

{
    ULONG rc;
    rc = SetLocCradleMountCallback(  NULL );
    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearLocCradleMountCallback: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "doClearLocCradleMountCallback: Disabled callback\r\n" );

}

/*************
 *
 * Name:    cbkTestSetLocEventTimeSyncCallback
 *
 * Purpose: SetLocEventTimeSyncCallback API callback function
 *
 **************/
local void cbkTestSetLocEventTimeSyncCallback( QmiCbkLocEventTimeSyncInd *pMsg)
{
    FILE *fp;

    fp = tfopen("../../cbk/test/results/cbkTestSetLocEventTimeSyncCallback.txt", "a");
    if ( fp == NULL )
        perror("cbkTestSetLocEventTimeSyncCallback");
    else
    {
        if (pMsg != NULL)
        {
            fprintf( fp, "\nLOC Time Sync Ref Counter: %ld\n",
                         pMsg->timeSyncRefCounter);

        }

        tfclose(fp);
    }
}

/*************
 *
 * Name:    doSetLocEventTimeSyncCallback
 *
 * Purpose: Set the SetLocEventTimeSyncCallback API
 *
 **************/
package void doSetLocEventTimeSyncCallback(void)

{
    ULONG rc;
    rc = SetLocEventTimeSyncCallback( &cbkTestSetLocEventTimeSyncCallback );
    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doSetLocEventTimeSyncCallback: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "doSetLocEventTimeSyncCallback: Enabled callback\r\n" );

}

/*************
 *
 * Name:    doClearLocEventTimeSyncCallback
 *
 * Purpose: Clear the SetLocEventTimeSyncCallback API
 *
 **************/
package void doClearLocEventTimeSyncCallback(void)

{
    ULONG rc;
    rc = SetLocEventTimeSyncCallback(  NULL );
    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearLocEventTimeSyncCallback: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "doClearLocEventTimeSyncCallback: Disabled callback\r\n" );

}

/*************
 *
 * Name:    cbkTestSetLocInjectTimeCallback
 *
 * Purpose: SetLocInjectTimeCallback API callback function
 *
 **************/
local void cbkTestSetLocInjectTimeCallback( QmiCbkLocInjectTimeInd *pMsg)
{
    FILE *fp;

    fp = tfopen("../../cbk/test/results/cbkTestSetLocInjectTimeCallback.txt", "a");
    if ( fp == NULL )
        perror("cbkTestSetLocInjectTimeCallback");
    else
    {
        if (pMsg != NULL)
        {
            fprintf( fp, "\nLOC Inject Time Sync Status: %ld\n",
                         pMsg->injectTimeSyncStatus);

        }

        tfclose(fp);
    }
}

/*************
 *
 * Name:    doSetLocInjectTimeCallback
 *
 * Purpose: Set the SetLocInjectTimeCallback API
 *
 **************/
package void doSetLocInjectTimeCallback(void)

{
    ULONG rc;
    rc = SetLocInjectTimeCallback(  &cbkTestSetLocInjectTimeCallback );
    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doSetLocInjectTimeCallback: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "doSetLocInjectTimeCallback: Enabled callback\r\n" );

}

/*************
 *
 * Name:    doClearLocInjectTimeCallback
 *
 * Purpose: Clear the SetLocInjectTimeCallback API
 *
 **************/
package void doClearLocInjectTimeCallback(void)

{
    ULONG rc;
    rc = SetLocInjectTimeCallback(  NULL );
    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearLocInjectTimeCallback: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "doClearLocInjectTimeCallback: Disabled callback\r\n" );

}

/*************
 *
 * Name:    cbkTestSetLocSensorStreamCallback
 *
 * Purpose: SetLocSensorStreamingCallback API callback function
 *
 **************/
local void cbkTestSetLocSensorStreamCallback( QmiCbkLocSensorStreamingInd *pMsg)
{
    FILE *fp;

    fp = tfopen("../../cbk/test/results/cbkTestSetLocSensorStreamCallback.txt", "a");
    if ( fp == NULL )
        perror("cbkTestSetLocSensorStreamCallback");
    else
    {
        fprintf(fp, "\nSensor Streaming Ready Status\n");
        if (pMsg->pAccelAcceptReady)
        {
            fprintf( fp, "\nAccel Inject Enable: %d\n",
                         pMsg->pAccelAcceptReady->injectEnable);
            fprintf( fp, "\nAccel SamplesPerBatch: %d\n",
                         pMsg->pAccelAcceptReady->samplesPerBatch);
            fprintf( fp, "\nAccel BatchesPerSecond: %d\n",
                         pMsg->pAccelAcceptReady->batchPerSec);
        }
        if (pMsg->pGyroAcceptReady)
        {
            fprintf( fp, "\nGyro Inject Enable: %d\n",
                         pMsg->pGyroAcceptReady->injectEnable);
            fprintf( fp, "\nGyro SamplesPerBatch: %d\n",
                         pMsg->pGyroAcceptReady->samplesPerBatch);
            fprintf( fp, "\nGyro BatchesPerSecond: %d\n",
                        pMsg->pGyroAcceptReady->batchPerSec);
        }
        if (pMsg->pAccelTempAcceptReady)
        {
            fprintf( fp, "\nAccel Temp. Inject Enable: %d\n",
                         pMsg->pAccelTempAcceptReady->injectEnable);
            fprintf( fp, "\nAccel Temp. SamplesPerBatch: %d\n",
                         pMsg->pAccelTempAcceptReady->samplesPerBatch);
            fprintf( fp, "\nAccel Temp. BatchesPerSecond: %d\n",
                         pMsg->pAccelTempAcceptReady->batchPerSec);
        }
        if (pMsg->pGyroTempAcceptReady)
        {
            fprintf( fp, "\nGyro Temp. Inject Enable: %d\n",
                         pMsg->pGyroTempAcceptReady->injectEnable);
            fprintf( fp, "\nGyro Temp. SamplesPerBatch: %d\n",
                         pMsg->pGyroTempAcceptReady->samplesPerBatch);
            fprintf( fp, "\nGyro Temp. BatchesPerSecond: %d\n",
                         pMsg->pGyroTempAcceptReady->batchPerSec);
        }
        tfclose(fp);
    }
}

/*************
 *
 * Name:    doSetLocSensorStreamCallback
 *
 * Purpose: Set the SetLocSensorStreamingCallback API
 *
 **************/
package void doSetLocSensorStreamCallback(void)

{
    ULONG rc;
    rc = SetLocSensorStreamingCallback(  &cbkTestSetLocSensorStreamCallback );
    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doSetLocSensorStreamCallback: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "doSetLocSensorStreamCallback: Enabled callback\r\n" );

}

/*************
 *
 * Name:    doClearLocSensorStreamCallback
 *
 * Purpose: Clear the SetLocSensorStreamingCallback API
 *
 **************/
package void doClearLocSensorStreamCallback(void)

{
    ULONG rc;
    rc = SetLocSensorStreamingCallback(  NULL );
    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearLocSensorStreamCallback: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "doClearLocSensorStreamCallback: Disabled callback\r\n" );

}
/*************
 *
 * Name:    cbkTestSetLocInjectSensorDataCallback
 *
 * Purpose: SetLocInjectSensorDataCallback API callback function
 *
 **************/
local void cbkTestSetLocInjectSensorDataCallback( QmiCbkLocInjectSensorDataInd *pMsg)
{
    FILE *fp;

    fp = tfopen("../../cbk/test/results/cbkTestSetLocInjectSensorDataCallback.txt", "a");
    if ( fp == NULL )
        perror("cbkTestSetLocInjectSensorDataCallback");
    else
    {
        if (pMsg != NULL)
        {
             fprintf(fp, "\nInject Sensor Data\n");
             fprintf( fp, "\nInject Sensor Data Status: %ld\n",
                          pMsg->injectSensorDataStatus);
             IFPRINTF( fp, "\nOpaque Identifier: %ld\n",
                          pMsg->pOpaqueIdentifier);
             IFPRINTF( fp, "\nAccel Samples Accepted: %d\n",
                          pMsg->pAccelSamplesAccepted);
             IFPRINTF( fp, "\nGyro Samples Accepted: %d\n",
                          pMsg->pGyroSamplesAccepted);
             IFPRINTF( fp, "\nAccel Temp Samples Accepted: %d\n",
                          pMsg->pAccelTempSamplesAccepted);
             IFPRINTF( fp, "\nGyro Temp Samples Accepted: %d\n",
                          pMsg->pGyroTempSamplesAccepted);
        }
        tfclose(fp);
    }
}

/*************
 *
 * Name:    doSetLocInjectSensorDataCallback
 *
 * Purpose: Set the SetLocInjectSensorDataCallback API
 *
 **************/
package void doSetLocInjectSensorDataCallback(void)

{
    ULONG rc;
    rc = SetLocInjectSensorDataCallback(  &cbkTestSetLocInjectSensorDataCallback );
    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doSetLocInjectSensorDataCallback: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "doSetLocInjectSensorDataCallback: Enabled callback\r\n" );

}

/*************
 *
 * Name:    doClearLocInjectSensorDataCallback
 *
 * Purpose: Clear the SetLocInjectSensorDataCallback API
 *
 **************/
package void doClearLocInjectSensorDataCallback(void)

{
    ULONG rc;
    rc = SetLocInjectSensorDataCallback(  NULL );
    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearLocInjectSensorDataCallback: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "doClearLocInjectSensorDataCallback: Disabled callback\r\n" );

}

/*************
 *
 * Name:    cbkTestSetLocEventPositionCallback
 *
 * Purpose: SetLocEventPositionCallback API callback function
 *
 **************/
local void cbkTestSetLocEventPositionCallback( QmiCbkLocPositionReportInd *pMsg)
{
    FILE *fp;

    fp = tfopen("../../cbk/test/results/cbkTestSetLocEventPositionCallback.txt", "a");
    if ( fp == NULL )
        perror("cbkTestSetLocEventPositionCallback");
    else
    {
        if (pMsg != NULL)
        {
             fprintf(fp, "Event Position Report\n");
             fprintf( fp, "Session Status: %ld\n",
                          pMsg->sessionStatus);
             fprintf( fp, "Session ID: %d\n",
                          pMsg->sessionId);
             IFPRINTF( fp, "Latitude: %lf\n",
                          (double*)pMsg->pLatitude);
             IFPRINTF( fp, "Longitude: %lf\n",
                          (double*)pMsg->pLongitude);
             IFPRINTF( fp, "Horizontal position uncertainty: %f\n",
                          (float*)pMsg->pHorUncCircular);
             IFPRINTF( fp, "Semi-minor axis of horizontal elliptical uncertainty: %f\n",
                          (float*)pMsg->pHorUncEllipseSemiMinor);
             IFPRINTF( fp, "Semi-major axis of horizontal elliptical uncertainty: %f\n",
                          (float*)pMsg->pHorUncEllipseSemiMajor);
             IFPRINTF( fp, "Elliptical Horizontal Uncertainty Azimuth: %f\n",
                          (float*)pMsg->pHorUncEllipseOrientAzimuth);
             IFPRINTF( fp, "Horizontal Confidence: %d\n",
                           pMsg->pHorConfidence);
             IFPRINTF( fp, "Horizontal Reliability: %ld\n",
                           (pMsg->pHorReliability));
             IFPRINTF( fp, "Horizontal Speed: %f\n",
                           (float*)pMsg->pSpeedHorizontal);
             IFPRINTF( fp, "Speed Uncertainty: %f\n",
                           (float*)pMsg->pSpeedUnc);
             IFPRINTF( fp, "Altitude With Respect to Ellipsoid: %f\n",
                           (float*)pMsg->pAltitudeWrtEllipsoid);
             IFPRINTF( fp, "Altitude With Respect to Sea Level: %f\n",
                           (float*)pMsg->pAltitudeWrtMeanSeaLevel);
             IFPRINTF( fp, "Vertical Uncertainty: %f\n",
                           (float*)pMsg->pVertUnc);
             IFPRINTF( fp, "Vertical Confidence: %d\n",
                            pMsg->pVertConfidence);
             IFPRINTF( fp, "Vertical Reliability: %ld\n",
                            pMsg->pVertReliability);
             IFPRINTF( fp, "Vertical Speed: %f\n",
                           (float*)pMsg->pSpeedVertical);
             IFPRINTF( fp, "Heading: %f\n",
                           (float*)pMsg->pHeading);
             IFPRINTF( fp, "Heading Uncertainty: %f\n",
                           (float*)pMsg->pHeadingUnc);
             IFPRINTF( fp, "Magnetic Deviation: %f\n",
                           (float*)pMsg->pMagneticDeviation);
             IFPRINTF( fp, "Technology Mask: %lu\n",
                            pMsg->pTechnologyMask);
             if (pMsg->pPrecisionDilution != NULL)
             {
                 fprintf( fp, "Position dilution of precision: %f\n",
                              *((float*)&pMsg->pPrecisionDilution->PDOP));
                 fprintf( fp, "Horizontal dilution of precision: %f\n",
                              *((float*)&pMsg->pPrecisionDilution->HDOP));
                 fprintf( fp, "Vertical dilution of precision: %f\n",
                              *((float*)&pMsg->pPrecisionDilution->VDOP));
             }
             IFPRINTF( fp, "UTC Timestamp: %lf\n",
                           (double*)pMsg->pTimestampUtc);
             IFPRINTF( fp, "Leap Seconds: %d\n",
                            pMsg->pLeapSeconds);
             if (pMsg->pGpsTime != NULL)
             {
                 fprintf( fp, "GPS Week: %d\n",
                              (pMsg->pGpsTime->gpsWeek));
                 fprintf( fp, "Amount of time into the current GPS week: %ld\n",
                              (pMsg->pGpsTime->gpsTimeOfWeekMs));
             }
             IFPRINTF( fp, "Time Uncertainty: %f\n",
                          (float*)pMsg->pTimeUnc);
             IFPRINTF( fp, "Time Source: %ld\n",
                            pMsg->pTimeSrc);
             if (pMsg->pSensorDataUsage != NULL)
             {
                 fprintf( fp, "Usage Mask: %ld\n",
                              (pMsg->pSensorDataUsage->usageMask));
                 fprintf( fp, "Aided Indicator Mask: %ld\n",
                              (pMsg->pSensorDataUsage->aidingIndicatorMask));
             }
             IFPRINTF( fp, "Fix ID: %ld\n",
                            pMsg->pFixId);
             if (pMsg->pSvUsedforFix != NULL)
             {
                  BYTE i = 0;
                  fprintf( fp, "GNSS SV Used List Length: %d\n",
                              (pMsg->pSvUsedforFix->gnssSvUsedList_len));
                 for ( i = 0; i < pMsg->pSvUsedforFix->gnssSvUsedList_len; i++ )
                 {
                       fprintf( fp, "GNSS SVs Used List: %d\n",
                              pMsg->pSvUsedforFix->gnssSvUsedList[i]);
                 }
             }
             IFPRINTF( fp, "Altitude Assumed: %x\n",
                           pMsg->pAltitudeAssumed);
             }
        tfclose(fp);
    }
}

/*************
 *
 * Name:    doSetLocEventPositionCallback
 *
 * Purpose: Set the SetLocEventPositionCallback API
 *
 **************/
package void doSetLocEventPositionCallback(void)

{
    ULONG rc;
    rc = SetLocEventPositionCallback(  &cbkTestSetLocEventPositionCallback );
    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doSetLocEventPositionCallback: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "doSetLocEventPositionCallback: Enabled callback\r\n" );

}

/*************
 *
 * Name:    doClearLocEventPositionCallback
 *
 * Purpose: Clear the SetLocEventPositionCallback API
 *
 **************/
package void doClearLocEventPositionCallback(void)

{
    ULONG rc;
    rc = SetLocEventPositionCallback(  NULL );
    if ( rc != eQCWWAN_ERR_NONE )
        fprintf( stderr, "doClearLocEventPositionCallback: Failed : %lu\r\n", rc );
    else
        fprintf( stderr, "doClearLocEventPositionCallback: Disabled callback\r\n" );

}

