/**************
 *
 *  Filename:   qaGobiApiDcsTest.c
 *
 *  Purpose:    QMI DCS service test routines
 *
 * Copyright: © 2011 Sierra Wireless, Inc., all rights reserved
 *
 **************/

/* Linux definitions */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

/* include files */
#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "sludefs.h"

#include "qaGobiApiDcs.h"
#include "qatesthelper.h"
#include "qatestproto.h"

/*
 * Name:     doDCSGetConnecteDevID
 *
 * Purpose:  QCWWAN2kGetConnectedDeviceID API driver
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doDCSGetConnecteDevID( void )
{
    FILE  *fp = NULL;
    CHAR  devid[QMI_DEVICE_PATH_MAX_SIZE];
    ULONG devidsz = sizeof(devid);
    CHAR  devkey[QMI_MEID_BUF_SIZE + 1];
    ULONG devkeysz = sizeof(devkey);
    ULONG nRet;

    fp = tfopen("../../dcs/test/results/dcsgetconnectedevid.txt", "w");

    if(fp)
    {
        nRet = QCWWAN2kGetConnectedDeviceID( devidsz,
                                              devid,
                                              devkeysz,
                                              devkey );

        /* Display result code and text */
        doprintreason (fp, nRet);

        if ( !nRet )
        {
            fprintf(fp, "DCSGetConnectedDevID Successful\n");
            fprintf(fp, "device id:\t%s\ndevice key:\t%s\n", devid, devkey);
        }
        else
        {
            fprintf(fp, "DCSGetConnectedDevID Failed\n");
        }
    }
    if (fp)
        tfclose(fp);
}

/*
 * Name:     doSLQSGetUsbPortNames
 *
 * Purpose:  SLQSGetUsbPortNames API driver
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSLQSGetUsbPortNames( void )
{
    FILE *fp = NULL;
    ULONG nRet;
    struct DcsUsbPortNames UsbPortNames;

    slmemset ((char*)&UsbPortNames, EOS, sizeof (struct DcsUsbPortNames));

    fp = tfopen("../../dcs/test/results/slqsgetusbportnames.txt", "w");

    if(fp)
    {
        nRet = SLQSGetUsbPortNames( &UsbPortNames );

        /* Display result code and text */
        doprintreason (fp, nRet);

        if ( !nRet )
        {
            fprintf( fp, "SLQSGetUsbPortNames Successful\n");
            fprintf( fp, "AT Command Port: %s\n", UsbPortNames.AtCmdPort );
            fprintf( fp, "NMEA Port      : %s\n", UsbPortNames.NmeaPort );
            fprintf( fp, "DM Port        : %s\n", UsbPortNames.DmPort );
        }
        else
        {
            fprintf( fp, "SLQSGetUsbPortNames Failed\n" );
        }
    }
    if (fp)
        tfclose(fp);
}

/*
 * Name:     doSLQSGetDeviceMode
 *
 * Purpose:  SLQSGetDeviceMode API driver
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSLQSGetDeviceMode( void )
{
    FILE *fp = NULL;
    ULONG nRet;
    BYTE DeviceMode;


    fp = tfopen("../../dcs/test/results/slqsgetdevicemode.txt", "w");

    if(fp)
    {
        nRet = SLQSGetDeviceMode( &DeviceMode );

        /* Display result code and text */
        doprintreason (fp, nRet);

        if ( !nRet )
        {
            fprintf( fp, "SLQSGetDeviceMode Successful\n");
            fprintf( fp, "Device Mode: %d\n", DeviceMode );
        }
        else
        {
            fprintf( fp, "SLQSGetDeviceMode Failed\n" );
        }
    }
    if (fp)
        tfclose(fp);
}

/*
 * Name:    doSLQSKillSDKProcess
 *
 * Purpose: Perform the tests that call the SLQSKillSDKProcess() API
 *
 * Return:  none
 *
 * Notes:   none
 *
 */
void doSLQSKillSDKProcess( void )
{
    ULONG  nRet;
    FILE   *fp = NULL;

    fp = tfopen("../../dcs/test/results/slqskillsdkprocess.txt", "w");

    if (fp)
    {
        nRet = SLQSKillSDKProcess();

        doprintreason( fp, nRet );

        if ( !nRet )
        {
            fprintf (fp, "SLQSKillSDKProcess Successful\n");
        }
        else
        {
            fprintf( fp, "SLQSKillSDKProcess Failed\n" );
        }
    }

    if (fp)
        tfclose(fp);
}

typedef struct _StartSrvTestCase_t
{
    enum   eQCWWANError expectedReturnValue;
    ULONG               testCaseId;
    BYTE                action;
    BYTE                mask;
    CHAR                desc[100];
} StartSrvTestCase_t;

typedef struct
{
    enum   eQCWWANError expectedReturnValue;
    ULONG               testCaseId;
    BYTE                instance;
    BYTE                dscp;
    ULONG               qos_id;
    CHAR                desc[100];
} SLQSQosMapTestCase_t;

typedef struct
{
    enum   eQCWWANError expectedReturnValue;
    ULONG               testCaseId;
    BYTE                instance;
    CHAR                desc[100];
} SLQSQosClearMapTestCase_t;

typedef struct
{
    enum   eQCWWANError expectedReturnValue;
    ULONG               testCaseId;
    BYTE                instance;
    CHAR                desc[100];
} SLQSQosDumpMapTestCase_t;

/*
 * Name:    doSLQSStartSrv
 *
 * Purpose: Perform the tests that call the SLQSStartSrv() API
 *
 * Return:  none
 *
 * Notes:   none
 *
 */
void doSLQSStartSrv( void )
{
    ULONG  nRet;
    FILE   *fp = NULL;

    /* Define the test cases to be executed */
    StartSrvTestCase_t \
        StartSrvTestCases[] =
    {
        /* Disable service cases */
        {eQCWWAN_ERR_NONE, 1, 0x00, 0x0F, "Disable srv- BIT MASK " },
        {eQCWWAN_ERR_NONE, 2, 0x00, 0x07, "Disable srv- BIT MASK " },
        {eQCWWAN_ERR_NONE, 3, 0x00, 0x08, "Disable srv- BIT MASK " },
        {eQCWWAN_ERR_NONE, 4, 0x00, 0x09, "Disable srv- BIT MASK " },
        {eQCWWAN_ERR_NONE, 5, 0x00, 0x01, "Disable srv- BIT MASK " },
        {eQCWWAN_ERR_NONE, 6, 0x00, 0x03, "Disable srv- BIT MASK " },
        {eQCWWAN_ERR_NONE, 7, 0x00, 0x05, "Disable srv- BIT MASK " },

        /* Enable service cases */
        {eQCWWAN_ERR_NONE, 9,  0x01, 0x07, "Enable srv- BIT MASK " },
        {eQCWWAN_ERR_NONE, 10, 0x01, 0x08, "Enable srv- BIT MASK " },
        {eQCWWAN_ERR_NONE, 11, 0x01, 0x09, "Enable srv- BIT MASK " },
        {eQCWWAN_ERR_NONE, 12, 0x01, 0x01, "Enable srv- BIT MASK " },
        {eQCWWAN_ERR_NONE, 13, 0x01, 0x03, "Enable srv- BIT MASK " },
        {eQCWWAN_ERR_NONE, 14, 0x01, 0x05, "Enable srv- BIT MASK " },
    };

    fp = tfopen("../../dcs/test/results/slqsstartsrv.txt", "w");

    if (fp)
    {
        BYTE tc = 0;
        BYTE ncases = sizeof( StartSrvTestCases )/
                      sizeof( StartSrvTestCases[0] );

        while (tc < ncases )
        {
            StartSrvTestCase_t *pd = &StartSrvTestCases[tc++];
            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        tc,
                        ncases,
                        pd->desc);

            nRet = SLQSStartSrv( pd->action, pd->mask );

            doprintreason( fp, nRet );

            if ( !nRet )
            {
                fprintf (fp, "SLQSStartSrv Successful\n");
            }
            else
            {
                fprintf( fp, "SLQSStartSrv Failed\n" );
            }
        }
    }

    if (fp)
        tfclose(fp);
}

void doSLQSQosMap( void )
{
    ULONG  nRet;
    FILE   *fp = NULL;

    /* Define the test cases to be executed */
    SLQSQosMapTestCase_t tc[] =
    {
        //first QMI instance
        {eQCWWAN_ERR_NONE, 1, 0, 0x10, 0x1234, "Valid" },
        //second QMI instance
        {eQCWWAN_ERR_NONE, 1, 1, 0x20, 0x1234, "Valid" },
    };

    fp = tfopen("../../dcs/test/results/slqsQosMap.txt", "w");

    if (fp)
    {
        int i;
        BYTE ntc = sizeof( tc )/ sizeof( tc[0] );

        for(i=0;i<ntc;i++)
        {
            SLQSQosMapTestCase_t *pd = &tc[i];
            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        i,
                        ntc,
                        pd->desc);

            nRet = SLQSQosMap(pd->instance, pd->dscp, pd->qos_id );

            doprintreason( fp, nRet );

            if ( !nRet )
            {
                fprintf (fp, "SLQSQosMap Successful\n");
            }
            else
            {
                fprintf( fp, "SLQSQosMap Failed\n" );
            }
        }

        tfclose(fp);
    }

}

void doSLQSQosUnmap( void )
{
    ULONG  nRet;
    FILE   *fp = NULL;

    /* Define the test cases to be executed */
    SLQSQosMapTestCase_t tc[] =
    {
        //first QMI instance
        {eQCWWAN_ERR_NONE, 1, 0, 0x10, 0, "Valid" },
        //second QMI instance
        {eQCWWAN_ERR_NONE, 1, 1, 0x20, 0, "Valid" },
    };

    fp = tfopen("../../dcs/test/results/slqsQosUnmap.txt", "w");

    if (fp)
    {
        int i;
        BYTE ntc = sizeof( tc )/ sizeof( tc[0] );

        for(i=0;i<ntc;i++)
        {
            SLQSQosMapTestCase_t *pd = &tc[i];
            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        i,
                        ntc,
                        pd->desc);

            nRet = SLQSQosUnmap(pd->instance, pd->dscp );

            doprintreason( fp, nRet );

            if ( !nRet )
            {
                fprintf (fp, "SLQSQosUnmap Successful\n");
            }
            else
            {
                fprintf( fp, "SLQSQosUnmap Failed\n" );
            }
        }

        tfclose(fp);
    }

}

void doSLQSQosClearMap( void )
{
    ULONG  nRet;
    FILE   *fp = NULL;

    /* Define the test cases to be executed */
    SLQSQosClearMapTestCase_t tc[] =
    {
        //first QMI instance
        {eQCWWAN_ERR_NONE, 1, 0, "Valid" },
        //second QMI instance
        {eQCWWAN_ERR_NONE, 1, 1, "Valid" },
    };

    fp = tfopen("../../dcs/test/results/slqsQosClearMap.txt", "w");

    if (fp)
    {
        int i;
        BYTE ntc = sizeof( tc )/ sizeof( tc[0] );

        for(i=0;i<ntc;i++)
        {
            SLQSQosClearMapTestCase_t *pd = &tc[i];
            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        i,
                        ntc,
                        pd->desc);

            nRet = SLQSQosClearMap(pd->instance);

            doprintreason( fp, nRet );

            if ( !nRet )
            {
                fprintf (fp, "SLQSQosClearMap Successful\n");
            }
            else
            {
                fprintf( fp, "SLQSQosClearMap Failed\n" );
            }
        }

        tfclose(fp);
    }

}

void doSLQSQosDumpMap( void )
{
    ULONG  nRet;
    FILE   *fp = NULL;
    struct QosMap maps[MAX_QOS_MAP];
    BYTE   len;

    /* Define the test cases to be executed */
    SLQSQosDumpMapTestCase_t tc[] =
    {
        //first QMI instance
        {eQCWWAN_ERR_NONE, 1, 0, "Valid" },
        //second QMI instance
        {eQCWWAN_ERR_NONE, 1, 1, "Valid" },
    };

    fp = tfopen("../../dcs/test/results/slqsQosDumpMap.txt", "w");

    if (fp)
    {
        int i,j;
        BYTE ntc = sizeof( tc )/ sizeof( tc[0] );

        for(i=0;i<ntc;i++)
        {
            SLQSQosDumpMapTestCase_t *pd = &tc[i];
            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        i,
                        ntc,
                        pd->desc);

            nRet = SLQSQosDumpMap(pd->instance, &maps[0], &len);

            doprintreason( fp, nRet );

            if ( !nRet )
            {
                fprintf (fp, "SLQSQosDumpMap Successful\n");
                for(j=0;j<MAX_QOS_MAP;j++)
                {
                    fprintf (fp, "[%d]dscp 0x%x\n",j, maps[j].dscp);
                    fprintf (fp, "[%d]qos id 0x%lx\n",j, maps[j].qos_id);
                    fprintf (fp, "[%d]state 0x%x\n",j, maps[j].state);
                }
            }
            else
            {
                fprintf( fp, "SLQSQosDumpMap Failed\n" );
            }
        }

        tfclose(fp);
    }

}

void doSLQSQosEditMap( void )
{
    ULONG  nRet;
    FILE   *fp = NULL;

    /* Define the test cases to be executed */
    SLQSQosMapTestCase_t tc[] =
    {
        //first QMI instance
        {eQCWWAN_ERR_NONE, 1, 0, 0x10, 0x1234, "Valid" },
        //second QMI instance
        {eQCWWAN_ERR_NONE, 1, 1, 0x20, 0x1234, "Valid" },
    };

    fp = tfopen("../../dcs/test/results/slqsQosEditMap.txt", "w");

    if (fp)
    {
        int i;
        BYTE ntc = sizeof( tc )/ sizeof( tc[0] );

        for(i=0;i<ntc;i++)
        {
            SLQSQosMapTestCase_t *pd = &tc[i];
            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        i,
                        ntc,
                        pd->desc);

            nRet = SLQSQosEditMap(pd->instance, pd->dscp, pd->qos_id );

            doprintreason( fp, nRet );

            if ( !nRet )
            {
                fprintf (fp, "SLQSQosEditMap Successful\n");
            }
            else
            {
                fprintf( fp, "SLQSQosEditMap Failed\n" );
            }
        }

        tfclose(fp);
    }

}

void doSLQSQosReadMap( void )
{
    ULONG  qos_id;
    ULONG  nRet;
    FILE   *fp = NULL;

    /* Define the test cases to be executed */
    SLQSQosMapTestCase_t tc[] =
    {
        //first QMI instance
        {eQCWWAN_ERR_NONE, 1, 0, 0x10, 0x1234, "Valid" },
        //second QMI instance
        {eQCWWAN_ERR_NONE, 1, 1, 0x20, 0x1234, "Valid" },
    };

    fp = tfopen("../../dcs/test/results/slqsQosEditMap.txt", "w");

    if (fp)
    {
        int i;
        BYTE ntc = sizeof( tc )/ sizeof( tc[0] );

        for(i=0;i<ntc;i++)
        {
            SLQSQosMapTestCase_t *pd = &tc[i];
            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        i,
                        ntc,
                        pd->desc);

            nRet = SLQSQosReadMap(pd->instance, pd->dscp, &qos_id );

            doprintreason( fp, nRet );

            if ( !nRet )
            {
                fprintf (fp, "SLQSQosReadMap Successful, qos id 0x%lx\n", qos_id);
            }
            else
            {
                fprintf( fp, "SLQSQosReadMap Failed\n" );
            }
        }

        tfclose(fp);
    }

}
