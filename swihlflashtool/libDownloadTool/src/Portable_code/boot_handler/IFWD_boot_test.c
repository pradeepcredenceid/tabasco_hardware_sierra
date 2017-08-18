/**
 *       Copyright (C) 2011-2014 Intel Mobile Communications GmbH
 *
 *            Sec Class: Intel Confidential (IC)
 *
 *
 * This document contains proprietary information belonging to IMC.
 * Design A/S. Passing on and copying of this document, use and communication
 * of its contents is not permitted without prior written authorisation.
 *
 * Description:
 *   <hardcoded file description - the only part to be modified manually!>
 *
 * Revision Information:
 *  $File name:  /msw_tools/FlashTool/libDownloadTool/src/Portable_code/boot_handler/IFWD_boot_test.c $
 *  $CC-Version: .../dhkristx_140314_v0/1 $
 *  $Date:       2014-04-23    10:51:44 UTC $
 *   Responsible: bentzen
 *   Comment:
 *     SMS00847521: "Need to remove Lint warnings..."
 **/
#ifdef MSVC
  #include "StdAfx.h"
#else
  #include "OS.h"
  #include <stdio.h>
  #include <stdlib.h>
  #include <unistd.h>

  #include "IFWD_mem.h"
  #include "IFWD_timer.h"
  #include "IFWD_comm.h"
  #include "IFWD_boot.h"
  #include "IFWD_comport.h"
#endif



/*****************************************************************************/
/* Constant definitions */
#define BUFFER_LENGTH  4096

#if 0

/*****************************************************************************
 *                             LOCAL VARIABLES                               *
 *****************************************************************************/
static comm_cfg_t comm_cfg1;
static comm_cfg_t comm_cfg2;
static comm_status_t comm_status;
static boot_status_t boot_status;
static boot_ebl_version_t ebl_version;
static boot_ebl_version_t *p_ebl_version = &ebl_version;

static boot_dev_cfg_t dev_cfg;
static HardwareStructType hw_cfg;
static HardwareStructType *p_hw_cfg = &hw_cfg;


/***************************** IMPLEMENTATION ********************************/
void main(void)
{
  uint32_t i;

#if 1
  strcpy(comm_cfg1.comport_name,"\\\\.\\COM10");
#else
  strcpy(comm_cfg1.comport_name,"COM1:");
#endif
  comm_cfg1.baudrate = 115200;
  COMM_init();
  BOOT_init();
  COMPORT_init();
  COMM_open_port(1,&comm_cfg1);
  COMM_add_channel(1);
  BOOT_add_channel(1);

  dev_cfg.cfg = p_hw_cfg;
  dev_cfg.cfg->Platform = S_GOLD_CHIPSET_V2;
  boot_status = BOOT_start_boot(1, &dev_cfg, p_ebl_version, boot_not);


  while (1)
  {
    Sleep(10);
    BOOT_clock();
    COMM_clock();
  }
}
/*****************************************************************************
 *                            Private functions                              *
 *****************************************************************************/

/******************************************************************************
 * Function:... void boot_not(uint8_t channel, uint8_t ebl_run, boot_ebl_version_t *p_version)
 *
 * Parameters:. I :
 * Returns:....
 * Description:
 *
 * Created:.... 18.02.06 by (Alan Bugtrup, DWD / AB)
 * Modified:... DD.MM.YY by (Full name / initials)
 *              Modifications note.
 ******************************************************************************/
 static void boot_not(uint8_t channel, uint8_t ebl_run, boot_ebl_version_t *p_version)
 {

 }
#endif

