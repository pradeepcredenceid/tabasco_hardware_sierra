/*
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
 *  $File name:  /msw_tools/FlashTool/libDownloadTool/src/OS_dependent_code/comm_if/IFWD_comport.c $
 *  $CC-Version: .../dhkristx_140314_v0/2 $
 *  $Date:       2014-04-23    11:01:34 UTC $
 *   Responsible: bentzen (windows), hilmar (*nix)
 *   Comment:
 *     Fixed suppert for Quantum USB port names and Certificate upload
 */

#define THIS_MODULE MODULE_COMM

// Always enable emulation functions
#define EMULATION_FUNCTIONS

/*---------------------------------------------*/
/* Include files.                              */
/*---------------------------------------------*/
#include "OS.h"
#include "DebugModule.h"

#include "devices.h"
#include "GenericUSB.h"

#include "IFWD_comport.h"
#include "IFWD_emuport.h"

#ifdef _WIN32
  #include <windows.h>
  #include <winioctl.h>
#else
  #include <string.h>
  #include <unistd.h>
  #include <fcntl.h>
  #include <errno.h>
  #include <termios.h>
  #include <sys/ioctl.h>

  #define INVALID_HANDLE_VALUE (-1)

  #ifdef __APPLE__
    //#include <IOKit/serial/ioss.h>
    #define B921600 921600
    #define B460800 460800
  #endif

  // Defines for the external MIPI (USP-HSI) box
  #define EXT_MIPI_IOC_CODE                   0x99
  #define EXT_MIPI_IOC_SEQ_START              0x10
  #define EXT_MIPI_IOC_SET_CONF               _IO(EXT_MIPI_IOC_CODE, EXT_MIPI_IOC_SEQ_START + 0)
  #define EXT_MIPI_IOC_GET_CONF               _IO(EXT_MIPI_IOC_CODE, EXT_MIPI_IOC_SEQ_START + 1)
  #define EXT_MIPI_BUS_ENABLE_GET             0x9100
  #define EXT_MIPI_RX_ERROR_FLAGS_GET         0x8000
  #define EXT_MIPI_TX_ERROR_FLAGS_GET         0x8100
  #define EXT_MIPI_DEEP_CLEAN                 0x2000
  #define EXT_MIPI_RX_DATA_RATE_SET(value)    (0x0000 + (value & 0xFF))
  #define EXT_MIPI_TX_DATA_RATE_SET(value)    (0x0100 + (value & 0xFF))
  #define EXT_MIPI_RX_TRANS_MODE_FRAMED_SET   0x0601
  #define EXT_MIPI_TX_TRANS_MODE_FRAMED_SET   0x0701
  #define EXT_MIPI_RX_CH_FIELD_W_SET(value)   (0x0200 + (value & 0xFF))
  #define EXT_MIPI_TX_CH_FIELD_W_SET(value)   (0x0300 + (value & 0x0F))
  #define EXT_MIPI_RX_DATA_RATE_GET           0x8200
  #define EXT_MIPI_TX_DATA_RATE_GET           0x8300
#endif


/*---------------------------------------------*/
/* Constant definitions                        */
/*---------------------------------------------*/
#define COMPORT_CAPABILITIES             (SP_BAUD | SP_DATABITS | SP_HANDSHAKING | SP_PARITY | SP_PARITY_CHECK | SP_STOPBITS )
#define COMPORT_VERSION                  "2.0"
#define MAX_COMPORT_INSTANCES              32

#define OUT_BUFFER_SIZE                  (16384 * 2)
#define IN_BUFFER_SIZE                   (16384 * 2)

#define READ_INTERVAL_TIMEOUT_CONST   MAXDWORD
//#define MAX_READ_BYTES                    1000
//#define MAX_WRITE_BYTES                   1000

#define SIG_COM_READER      1
#define SIG_COM_WRITER      2
#define SIG_COM_READY       3
#define SIG_COM_DATA_IN     4   /* for byte by byte operation */
#define SIG_COM_CLOSE       5   /* for byte by byte operation */
#define TSERIAL_SIGNAL_NBR  6   /* number of events in the thread */


/*---------------------------------------------*/
/* Type definitions                            */
/*---------------------------------------------*/

typedef struct comport_dev_s
{
  /* Public interface */
  comport_handler_t              port;
  /* Private interface */
  uint32_t                       usb_port;
  uint32_t                       dwSettableParams;
  uint32_t                       thread_id;
  uint8_t                        rts_enabled;
  uint8_t                        dtr_enabled;
  uint32_t                       reset_gpio_num;
  uint8_t                        reset_comm_port_close_locally;
  COMM_HANDLE                    h_comm;
  COMM_HANDLE                    h_reset_comm_port; // To be used for the prolific handling
#ifdef _WIN32
  COMSTAT                        stat;
  DCB                            dcb;
  DCB                            org_dcb;
  HANDLE                         serialEvents[TSERIAL_SIGNAL_NBR]; /* events to wait on */
  OVERLAPPED                     ov_reader;       /* Overlapped structure for ReadFile */
  OVERLAPPED                     ov_writer;       /* Overlapped structure for WriteFile */
#endif
  uint8_t                       *buf;
  uint32_t                       len;
  uint32_t                       pos;

  uint32_t                      *read_bytes;
  uint32_t                       write_bytes;
  uint32_t                       rx_buffer_size;
  uint32_t                       tx_buffer_size;

  uint16_t                       tx_in_progress;   /* uint8_t indicating if a WriteFile is in progress */
  uint16_t                       rx_in_progress;   /* uint8_t indicating if a ReadFile is in progress */
  uint16_t                       data_to_send;
  uint32_t                       data_send;
  uint16_t                       ready;
  comport_cfg_t                  comport_cfg;
  comport_cfg_t                  org_comport_cfg;
  uint32_t                       org_rx_buffer_size;
  uint32_t                       org_tx_buffer_size;
  uint16_t                      *buf_to_send;
  uint8_t                        comport_enabled;
  DWORD                          last_error;
  comport_status_t               current_status;
  comport_status_t               last_status;
  uint16_t                       debug_enabled;
  uint8_t                        done;
  uint8_t                        process_unning;
  comm_error_report_t            error_report;
  uint8_t                        comm_channel_id;
} comport_dev_t;


typedef struct emulation_dev_s
{
  comport_handler_t              port;
  uint32_t                       h_comm;
  comport_rts_control_t          hw_flow_control;
  uint32_t                       rts;
  uint32_t                       dtr;
  uint32_t                       baudrate;
  uint32_t                      *read_bytes;
  uint32_t                      *bytes_sent;
  uint32_t                       temp_bytes_sent;
  uint32_t                       rx_buffer_size;
  uint32_t                       tx_buffer_size;
  uint16_t                       tx_in_progress;   /* uint8_t indicating if a WriteFile is in progress */
  uint16_t                       rx_in_progress;   /* uint8_t indicating if a ReadFile is in progress */
  comport_cfg_t                  comport_cfg;
  uint8_t                        comm_channel_id;

  DWORD                          last_error;
  comm_error_report_t            error_report;
  comport_status_t               current_status;
  comport_status_t               last_status;
} emulation_dev_t;

typedef struct comport_dev_table_s
{
  comport_dev_t       *comport_dev[MAX_COMPORT_INSTANCES+1];
  uint8_t              instance_nr;
} comport_dev_table_t;

typedef struct comport_emu_dev_table_s
{
  emulation_dev_t *dev[MAX_COMPORT_INSTANCES+1];
  uint8_t          instance_nr;
} comport_emu_dev_table_t;


/*****************************************************************************
*                             LOCAL PROTOTYPES                              *
*****************************************************************************/

static uint8_t  COMPORT_set_baudrate_I(uint8_t id, uint32_t baudrate);
static uint8_t  COMPORT_get_modem_status_I(uint8_t id,
                                    comport_modem_status_line_t *modem_status_line);
static uint8_t  COMPORT_close_port_I(uint8_t id);
static uint8_t  COMPORT_set_buffer_size_I(uint8_t id, uint32_t rx_buf_size, uint32_t tx_buf_size);
static uint32_t COMPORT_get_rx_used_buffer_size_I(uint8_t id);
static uint8_t  COMPORT_rx_I(uint8_t id, uint8_t **block, uint32_t bytes_to_read,
                      uint32_t *bytes_read, comport_fx_rx_t fx);
static uint8_t  COMPORT_tx_I(uint8_t id, uint8_t *block, uint32_t bytes_to_send,
                      uint32_t *bytes_send, comport_fx_tx_t fx);
static uint8_t  COMPORT_set_rts_I(uint8_t id, uint8_t new_rts);
static uint8_t  COMPORT_set_dtr_I(uint8_t id, uint8_t new_dtr);
static uint8_t  COMPORT_flush_rx_buffer_I(uint8_t id);
static uint8_t  COMPORT_flush_tx_buffer_I(uint8_t id);
static comport_error_t  COMPORT_get_last_error_I(uint8_t id);
static comport_status_t COMPORT_get_status_I(uint8_t id);
static void COMPORT_process_communication_I(uint8_t id);
static uint8_t Set_stop_bits_I(uint8_t id, comport_stop_bits_t StopBits);
static uint8_t Set_data_bits_I(uint8_t id);
static uint8_t Set_parity_I(uint8_t id, comport_parity_t parity);
static uint8_t disable_hw_flow_ctrl_I(uint8_t id);

#ifndef _WIN32
  static uint8_t COMPORT_special_Linux_settings( uint8_t handle, uint8_t technology );
#else
  static uint8_t Set_comm_timeouts_I(uint8_t comm_handler, LPCOMMPROP const lpCommProp);
  static void COMPORT_close_event_handler_I(comport_dev_t *comport_dev_temp);
#endif


/* Emulation functions used during testing: */
#ifdef EMULATION_FUNCTIONS
  static uint8_t  COMPORT_set_baudrate_E(uint8_t id, uint32_t baudrate);
  static uint8_t  COMPORT_get_modem_status_E(uint8_t id,
                                      comport_modem_status_line_t *modem_status_line);
  static uint8_t  COMPORT_close_port_E(uint8_t id);
  static uint8_t  COMPORT_set_buffer_size_E(uint8_t id, uint32_t rx_buf_size, uint32_t tx_buf_size);
  static uint32_t COMPORT_get_rx_used_buffer_size_E(uint8_t id);
  static uint8_t  COMPORT_rx_E(uint8_t id, uint8_t **block, uint32_t bytes_to_read,
                        uint32_t *bytes_read, comport_fx_rx_t fx);
  static uint8_t  COMPORT_tx_E(uint8_t id, uint8_t *block, uint32_t bytes_to_send,
                        uint32_t *bytes_send, comport_fx_tx_t fx);
  static uint8_t  COMPORT_set_rts_E(uint8_t id, uint8_t new_rts);
  static uint8_t  COMPORT_set_dtr_E(uint8_t id, uint8_t new_dtr);
  static uint8_t  COMPORT_flush_rx_buffer_E(uint8_t id);
  static uint8_t  COMPORT_flush_tx_buffer_E(uint8_t id);
  static comport_error_t  COMPORT_get_last_error_E(uint8_t id);
  static comport_status_t COMPORT_get_status_E(uint8_t id);
  static void COMPORT_process_communication_E(uint8_t id);
  //static uint8_t disable_hw_flow_ctrl_E(uint8_t id);
  static comport_handler_t *COMPORT_open_E(uint8_t comm_channel_id, comport_cfg_t *comport_cfg,
                                         comm_error_report_t error_report);
  //static uint8_t COMPORT_close_E(void);
#endif

static comport_handler_t *COMPORT_open_I(uint8_t comm_channel_id, comport_cfg_t *comport_cfg,
                                       comm_error_report_t error_report);

/*****************************************************************************
*                            PUBLIC PROTOTYPES                              *
*****************************************************************************/
uint8_t COMPORT_init(void);



/*****************************************************************************/
/* Local variables declaration */
//static comport_dev_table_t *p_comport_dev_table = NULL;
static comport_dev_table_t comport_dev_table;
static comport_emu_dev_table_t  comport_emu_dev_table;

/*****************************************************************************
*                            IMPLEMENTATION                                 *
*****************************************************************************/

/******************************************************************************
* Function:... uint8_t COMPORT_init(void)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 23.01.05 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
uint8_t COMPORT_init(void)
{
  uint16_t i;
  unsigned char *buf = NULL;

  for (i = 0; i <= MAX_COMPORT_INSTANCES; i++)
  {
    comport_dev_table.comport_dev[i] = NULL;
    comport_emu_dev_table.dev[i] =  NULL;
  }
  comport_dev_table.instance_nr = 0;
  comport_emu_dev_table.instance_nr = 0;

  // for coverage
  DEVICE_get_imc_name(NO_ID,NO_ID,NO_ID,NO_ID);
  COMPORT_get_handle_I(0);
  COMPORT_rx_I(0, &buf, 0, NULL, (comport_fx_rx_t) 0);
  COMPORT_get_last_error_I(0);

  return 0xFF;
}


/******************************************************************************
* Function:... COMPORT_open(uint8_t comm_channel_id, comport_cfg_t *comport_cfg,
*                           comm_error_report_t error_report)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 23.01.05 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
comport_handler_t *COMPORT_open(uint8_t id, comport_cfg_t *comport_cfg,
                                comm_error_report_t error_report)
{
  comport_handler_t *dev;
  char *comport_name = (char*)comport_cfg->comport_name;
  if (!comport_name)
    return NULL;

  // If a number is passed down we assume
  if (isdigit(comport_name[0])
      || strncmp(USB_DEV_DIR, comport_name, 12) == 0)
  {
    // if sysname provided, map the device to channel 1
    if (strlen(comport_name) > 2 && comport_name[1] == '-')
    {
      GenericUSB_map(1, comport_name);
      comport_name = "1";
    }

    GLOBAL_usb_device = true;
    GLOBAL_use_GenericUSB = true;
  }
  else
  {
    /* Determine communication type (USIF,USB,HSI) */
    GLOBAL_use_GenericUSB = false;
    #ifdef _WIN32
      //if (strstr(comport_name,"COM")) technology = COMPORT_TECH_USB;
      GLOBAL_usb_device = !strstr(comport_name,"COM");
    #elif defined(__APPLE__)
      GLOBAL_usb_device = !strstr(comport_name,"usbserial");
    #elif defined(ANDROID)
      GLOBAL_usb_device = !strstr(comport_name,"MFD");

      if (strstr(comport_name,"IFX"))
      {
        GLOBAL_hsi_link = true;
        GLOBAL_usb_device = false;
      }
    #else
      GLOBAL_usb_device = !(strstr(comport_name,"ttyUSB") || strstr(comport_name,"ttyS"));
    #endif
  }

  if(GLOBAL_use_hw_ch_change || strstr(comport_name,"hsi"))
  {
    GLOBAL_hsi_link = true;
    GLOBAL_usb_device = false;
    GLOBAL_comm_align_bytes[id] = 4; // When using MIPI the protocol must ensure read/write is done with 4 bytes alignment.
    if (GLOBAL_use_hw_ch_change) // Connection needs special protocol handling
    {
      Debug(" *** Special MIPI treatment ***\n");
      GLOBAL_comm_technology[id] = COMM_TECH_MIPI_EXT_BOX_PRE_OPEN;
    }
    else
    {
      Debug(" *** MIPI pre-open ***\n");
      GLOBAL_comm_technology[id] = COMM_TECH_MIPI_PRE_OPEN;
    }
  }

  // Opening the Communication Port
  if(GLOBAL_UseCOMPortEmulation)
  {
    #ifdef EMULATION_FUNCTIONS
      dev = COMPORT_open_E(id, comport_cfg, error_report);
    #endif
  }
  else if(GLOBAL_use_GenericUSB)
  {
    dev = GenericUSB_open(id, comport_cfg, error_report);
  }
  else
  {
    dev = COMPORT_open_I(id, comport_cfg, error_report);
  }
  return dev;
}




/******************************************************************************
* Function: COMPORT_get_handle_I(uint8_t id)
******************************************************************************/
COMM_HANDLE COMPORT_get_handle_I(uint8_t id)
{
  comport_dev_t *comport_dev = comport_dev_table.comport_dev[id];

  if (!comport_dev || comport_dev->h_comm == INVALID_HANDLE_VALUE)
    return 0;

  return (COMM_HANDLE)comport_dev->h_comm;
}



/**
 *  Windows API is defined first, then *nix.
 *  The two API's are too different to merge
 **/

 #ifdef _WIN32



/******************************************************************************
* Function:... COMPORT_open_I(uint8_t comm_channel_id, comport_cfg_t *comport_cfg,
*                           comm_error_report_t error_report)
* Parameters:. I :
* Description:
*
* Created:.... 23.01.05 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
comport_handler_t *COMPORT_open_I(uint8_t comm_channel_id, comport_cfg_t *comport_cfg,
                                  comm_error_report_t error_report)
{
  comport_dev_t    *comport_dev_temp;
  int res;
  DWORD dwSize;
  LPCOMMPROP lpCommProp;
  int i;

  if (comport_cfg && (0 < comport_cfg->id) && (comport_cfg->id <= MAX_COMPORT_INSTANCES))
  {
  comport_dev_temp = comport_dev_table.comport_dev[comport_cfg->id];
  if (comport_dev_temp != NULL)
  {
    /* Comport is already created
    Check if Channel id are different from already created file
    */
    if ( comm_channel_id != comport_dev_temp->comm_channel_id )
    {
    comport_dev_temp->last_error = comport_error_open_failed;
    comport_dev_temp->error_report(comport_dev_temp->comm_channel_id, comport_dev_temp->last_error ,__LINE__);

    CloseHandle(comport_dev_temp->h_comm);
    free(comport_dev_temp);
    comport_dev_table.comport_dev[comport_cfg->id] = NULL;
    return NULL;
    }
  }
  else
  {
      comport_dev_table.comport_dev[comport_cfg->id] = (comport_dev_t *) malloc (sizeof(comport_dev_t));
      memset(comport_dev_table.comport_dev[comport_cfg->id], 0, sizeof(comport_dev_t));
      comport_dev_table.instance_nr = comport_cfg->id;

      comport_dev_temp = comport_dev_table.comport_dev[comport_cfg->id];

      comport_dev_temp->port.id = comport_cfg->id;
      comport_dev_temp->h_comm = INVALID_HANDLE_VALUE;
      memcpy((void*)&comport_dev_temp->comport_cfg, (void*)comport_cfg, sizeof(comport_cfg_t));
    if (((comport_dev_temp->comport_cfg.comport_name[0] == (char)'U') &&
         (comport_dev_temp->comport_cfg.comport_name[1] == (char)'S') &&
         (comport_dev_temp->comport_cfg.comport_name[2] == (char)'B')) ||
        ((comport_dev_temp->comport_cfg.comport_name[4] == (char)'U') &&
         (comport_dev_temp->comport_cfg.comport_name[5] == (char)'S') &&
         (comport_dev_temp->comport_cfg.comport_name[6] == (char)'B')) ||
        ((comport_dev_temp->comport_cfg.comport_name[0] == (char)'\\') &&
         (comport_dev_temp->comport_cfg.comport_name[1] == (char)'\\') &&
         (comport_dev_temp->comport_cfg.comport_name[2] == (char)'?')) )
    {
      comport_dev_temp->usb_port = 1;
    }
      comport_dev_temp->last_error = 0;
      comport_dev_temp->process_unning = 0;
      comport_dev_temp->thread_id = 0;
      comport_dev_temp->rx_in_progress = 0;
      comport_dev_temp->tx_in_progress = 0;
      comport_dev_temp->comport_enabled = 0;
      comport_dev_temp->debug_enabled = 0;
      comport_dev_temp->data_to_send = 0;
      comport_dev_temp->data_send = 0;
      comport_dev_temp->done = 0;
      comport_dev_temp->ready = 0;
      comport_dev_temp->error_report = error_report;
      comport_dev_temp->comm_channel_id = comm_channel_id;
      comport_dev_temp->reset_comm_port_close_locally = false;


      ZeroMemory((void*)&comport_dev_temp->ov_reader,sizeof(comport_dev_temp->ov_reader));      /* clearing the overlapped */
      ZeroMemory((void*)&comport_dev_temp->ov_writer,sizeof(comport_dev_temp->ov_writer));

      comport_dev_temp->h_comm=CreateFile((char *)comport_dev_temp->comport_cfg.comport_name,
        GENERIC_READ | GENERIC_WRITE, 0,
        NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

      if (comport_dev_temp->h_comm==INVALID_HANDLE_VALUE)
      { /* A error has occurred */
        comport_dev_temp->last_error = GetLastError();
#if 0
        comport_dev_temp->error_report(comport_dev_temp->comm_channel_id, comport_dev_temp->last_error ,__LINE__);
#endif

        free(comport_dev_temp);
        comport_dev_table.comport_dev[comport_cfg->id] = NULL;
        return NULL;
      }

    }


    /* Clean up the RX/TX buffers and RX/TX flags */
  if (!comport_dev_temp->usb_port)
  {
    if (!PurgeComm(comport_dev_temp->h_comm, PURGE_TXABORT | PURGE_RXABORT |
                   PURGE_TXCLEAR | PURGE_RXCLEAR))
    { /* A error has occurred */
      CloseHandle(comport_dev_temp->h_comm);
      comport_dev_temp->last_error = GetLastError();
      comport_dev_temp->error_report(comport_dev_temp->comm_channel_id, comport_dev_temp->last_error ,__LINE__);
      free(comport_dev_temp);
      comport_dev_table.comport_dev[comport_cfg->id] = NULL;
      return NULL;
    }
  }

    /* Get the propierties of the comport before its initialisation */
    lpCommProp = (LPCOMMPROP) malloc(sizeof(COMMPROP));
    if(!lpCommProp) /* just to keep lint happy - if we can't get memory we are in trouble anyway */
    {
      CloseHandle(comport_dev_temp->h_comm);
      return NULL;
    }
    GetCommProperties(comport_dev_temp->h_comm, lpCommProp);
    comport_dev_temp->dwSettableParams = lpCommProp->dwSettableParams;
    dwSize = lpCommProp->wPacketLength;
    free(lpCommProp);
    lpCommProp = NULL; /* mark it as free'ed */
    if (dwSize > 0)
    {
      LPCOMMPROP templpCommProp;

      if(dwSize < sizeof(COMMPROP))
        dwSize = sizeof(COMMPROP);
      lpCommProp = (LPCOMMPROP) malloc(dwSize);
      if(!lpCommProp) /* just to keep lint happy - if we can't get memory we are in trouble anyway */
      {
        CloseHandle(comport_dev_temp->h_comm);
        return NULL;
      }
      GetCommProperties(comport_dev_temp->h_comm, lpCommProp);

      /* Store the original configuration */
      comport_dev_temp->org_rx_buffer_size = lpCommProp->dwMaxRxQueue;
      comport_dev_temp->org_tx_buffer_size = lpCommProp->dwMaxTxQueue;
      /* Setup device buffers */
      if ((lpCommProp->dwMaxRxQueue == 0) || (lpCommProp->dwMaxRxQueue > IN_BUFFER_SIZE))
        comport_dev_temp->rx_buffer_size = IN_BUFFER_SIZE;
      else
        comport_dev_temp->rx_buffer_size = lpCommProp->dwMaxRxQueue;
      if ((lpCommProp->dwMaxTxQueue == 0) || (lpCommProp->dwMaxTxQueue > OUT_BUFFER_SIZE))
        comport_dev_temp->tx_buffer_size = OUT_BUFFER_SIZE;
      else
        comport_dev_temp->tx_buffer_size = lpCommProp->dwMaxTxQueue;

      /* Set the comport RX/TX buffer size */
      if (!SetupComm(comport_dev_temp->h_comm, comport_dev_temp->rx_buffer_size, comport_dev_temp->tx_buffer_size))
      { /* An Error has occurred */
        CloseHandle(comport_dev_temp->h_comm);
        comport_dev_temp->last_error = GetLastError();
        comport_dev_temp->error_report(comport_dev_temp->comm_channel_id, comport_dev_temp->last_error ,__LINE__);
        if(lpCommProp)
          free(lpCommProp);
        free(comport_dev_temp);
        comport_dev_table.comport_dev[comport_cfg->id] = NULL;
        return NULL;
      }

      templpCommProp = (LPCOMMPROP)lpCommProp; /* use a temp pointer so Lint doesn't give "warning 429: Custodial pointer" */
      if (Set_comm_timeouts_I(comport_dev_temp->port.id, templpCommProp) == 0)
      {
        CloseHandle(comport_dev_temp->h_comm); // Make sure the port is closed
        if(lpCommProp)
          free(lpCommProp);
        free(comport_dev_temp);
        comport_dev_table.comport_dev[comport_cfg->id] = NULL;
        return NULL;
      }
    }
    else
    {
      comport_dev_temp->rx_buffer_size = IN_BUFFER_SIZE;
      comport_dev_temp->tx_buffer_size = OUT_BUFFER_SIZE;
    }

    /* Release memory allocated to lpCommProp */
    if(lpCommProp)
    {
      free(lpCommProp);
 //     lpCommProp = NULL; /* mark as deallocated */
    }

    if ((comport_dev_temp->dwSettableParams & COMPORT_CAPABILITIES)== COMPORT_CAPABILITIES)
    {
      res = GetCommState(comport_dev_temp->h_comm, &comport_dev_temp->dcb);
      if (res == 0)
      {
        CloseHandle(comport_dev_temp->h_comm);
        comport_dev_temp->last_error = GetLastError();
        comport_dev_temp->error_report(comport_dev_temp->comm_channel_id, comport_dev_temp->last_error ,__LINE__);
        free(comport_dev_temp);
        comport_dev_table.comport_dev[comport_cfg->id] = NULL;
        return NULL;
      }

      /* Store original configuration */
      comport_dev_temp->org_comport_cfg.baudrate = comport_dev_temp->dcb.BaudRate;
      comport_dev_temp->org_comport_cfg.databits = comport_dev_temp->dcb.ByteSize;

      switch (comport_dev_temp->dcb.StopBits)
      {
      case 1:
        comport_dev_temp->org_comport_cfg.stop_bits = comport_one_5_stop_bit;
        break;
      case 2:
        comport_dev_temp->org_comport_cfg.stop_bits = comport_two_stop_bits;
        break;
      case 0:
      default:
        comport_dev_temp->org_comport_cfg.stop_bits = comport_one_stop_bit;
        break;
      }

      switch (comport_dev_temp->dcb.Parity)
      {
      case 1:
        comport_dev_temp->org_comport_cfg.parity = comport_odd_parity;
        break;
      case 2:
        comport_dev_temp->org_comport_cfg.parity = comport_even_parity;
        break;
      case 3:
        comport_dev_temp->org_comport_cfg.parity = comport_mark_parity;
        break;
      case 4:
        comport_dev_temp->org_comport_cfg.parity = comport_space_parity;
        break;
      default:
      case 0:
        comport_dev_temp->org_comport_cfg.parity = comport_no_parity;
        break;
      }

      switch (comport_dev_temp->dcb.fRtsControl)
      {
      case RTS_CONTROL_ENABLE:
        comport_dev_temp->org_comport_cfg.rts_control = comport_rts_control_enable;
        break;

      case RTS_CONTROL_HANDSHAKE:
        comport_dev_temp->org_comport_cfg.rts_control = comport_rts_control_handshake;
        break;

      case RTS_CONTROL_TOGGLE:
        comport_dev_temp->org_comport_cfg.rts_control = comport_rts_control_toggle;
        break;

      case RTS_CONTROL_DISABLE:
      default:
        comport_dev_temp->org_comport_cfg.rts_control = comport_rts_control_disable;
        break;
      }

      memcpy((void*)&comport_dev_temp->org_dcb, (void*)&comport_dev_temp->dcb, sizeof(DCB));
      /*  Set Communications timeouts */

      /* Configure baudrate, databits, stopbits, parity, RTS and DTR */
      int id = comport_dev_temp->port.id;
      int done = false;
      do {
        if (!COMPORT_set_baudrate_I(id, comport_cfg->baudrate)) break;
        if (!Set_data_bits_I(id)) break;
        if (!Set_parity_I(id, comport_cfg->parity)) break;
        if (!Set_stop_bits_I(id, comport_cfg->stop_bits)) break;
        if (!disable_hw_flow_ctrl_I(id)) break;
        if (!COMPORT_set_rts_I(id, comport_dev_temp->rts_enabled)) break;
        if (!COMPORT_set_dtr_I(id, comport_dev_temp->dtr_enabled)) break;
        done = true;
      } while (0);

      if (!done)
      {
        PrintError("Error> USIF setup failed %d: %s\n", errno, strerror(errno));
        CloseHandle(comport_dev_temp->h_comm);
        free(comport_dev_temp);
        comport_dev_table.comport_dev[comport_cfg->id] = NULL;
        return NULL;
      }

    }

    /* Clean up the RX/TX buffers and RX/TX flags */
    if (!comport_dev_temp->usb_port)
    {
      if (!PurgeComm(comport_dev_temp->h_comm, PURGE_TXABORT | PURGE_RXABORT |
                     PURGE_TXCLEAR | PURGE_RXCLEAR))
      {
        CloseHandle(comport_dev_temp->h_comm);  // Make sure the port is closed
        comport_dev_temp->last_error = GetLastError();
        comport_dev_temp->error_report(comport_dev_temp->comm_channel_id, comport_dev_temp->last_error ,__LINE__);
        free(comport_dev_temp);
        comport_dev_table.comport_dev[comport_cfg->id] = NULL;
        return NULL;
      }
    }

    comport_dev_temp->ready = 0;
    comport_dev_temp->write_bytes = 0;
    comport_dev_temp->port.close = COMPORT_close_port_I;
    comport_dev_temp->port.process_comm = COMPORT_process_communication_I;
    comport_dev_temp->port.rx = COMPORT_rx_I;
    comport_dev_temp->port.flush_rx_buffer = COMPORT_flush_rx_buffer_I;
    comport_dev_temp->port.flush_tx_buffer = COMPORT_flush_tx_buffer_I;
    comport_dev_temp->port.get_modem_status = COMPORT_get_modem_status_I;
    comport_dev_temp->port.get_last_error = COMPORT_get_last_error_I;
    comport_dev_temp->port.get_rx_used_buffer_size = COMPORT_get_rx_used_buffer_size_I;
    comport_dev_temp->port.get_status = COMPORT_get_status_I;
    comport_dev_temp->port.set_baud = COMPORT_set_baudrate_I;
    comport_dev_temp->port.set_buffer_size = COMPORT_set_buffer_size_I;
    comport_dev_temp->port.set_dtr = COMPORT_set_dtr_I;
    comport_dev_temp->port.set_rts = COMPORT_set_rts_I;
    comport_dev_temp->port.tx = COMPORT_tx_I;

    /*creating Events for the different sources */
    for (i=0; i<TSERIAL_SIGNAL_NBR; i++)
    {
      if ((i == SIG_COM_READER) || (i == SIG_COM_WRITER))
        comport_dev_temp->serialEvents[i] = CreateEvent(NULL, true, false, NULL);  /* Manual Reset */
      else
        comport_dev_temp->serialEvents[i] = CreateEvent(NULL, false, false, NULL); /* Auto reset */

      if (comport_dev_temp->serialEvents[i] == NULL)
      {
        comport_dev_temp->last_error = GetLastError();
        comport_dev_temp->error_report(comport_dev_temp->comm_channel_id, comport_dev_temp->last_error ,__LINE__);
        COMPORT_close_event_handler_I(comport_dev_temp);
        CloseHandle(comport_dev_temp->h_comm); // Make sure the port is closed
        free(comport_dev_temp);
        comport_dev_table.comport_dev[comport_cfg->id] = NULL;
        return NULL;
      }
    }

    comport_dev_temp->ov_reader.hEvent = comport_dev_temp->serialEvents[SIG_COM_READER];
    comport_dev_temp->ov_writer.hEvent = comport_dev_temp->serialEvents[SIG_COM_WRITER];

    return (&comport_dev_temp->port);
  }
  else
  { /* Invalid id. Must be between 1 and MAX_COMPORT_INSTANCES+1 */
    return NULL;
  }
}

/******************************************************************************
* Function:... uint8_t  COMPORT_close_port(uint8_t id)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 23.01.05 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static uint8_t  COMPORT_close_port_I(uint8_t id)
{
  uint8_t res;
  comport_dev_t *comport_dev = comport_dev_table.comport_dev[id];

  if(comport_dev && comport_dev->h_comm != INVALID_HANDLE_VALUE)
  {
    COMPORT_close_event_handler_I(comport_dev);
    res = CloseHandle(comport_dev->h_comm) != 0;
    if (res == false)
    {
      comport_dev->last_error = GetLastError();
      comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
      comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
    }
    free(comport_dev);
    comport_dev_table.comport_dev[id] = NULL;
  }
  else
    res = true;
  return res;
}

/******************************************************************************
* Function:... COMPORT_close(void)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 23.01.05 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
/*uint8_t COMPORT_close_I(void)
{
  uint16_t i;
  comport_dev_t    *comport_dev;

  //  if (p_comport_dev_table != NULL)
  //  {
  for (i = 0; i <= MAX_COMPORT_INSTANCES; i++)
  {
    if (comport_dev_table.comport_dev[i] != NULL)
    {
      comport_dev = comport_dev_table.comport_dev[i];
      if(comport_dev)
      {
        if (comport_dev->h_comm != INVALID_HANDLE_VALUE)
          CloseHandle(comport_dev->h_comm);
        free(comport_dev);
      }
      comport_dev_table.comport_dev[i] = NULL;
    }
  }
  //  }
  //  else
  //    return 0;

  return 0xFF;
}
*/

/******************************************************************************
* Function:... uint8_t COMPORT_rx(uint8_t  id, uint8_t *block,
uint32_t bytes_to_read, uint32_t *bytes_read, fx_t fx)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 23.01.05 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static uint8_t COMPORT_rx_I(uint8_t id, uint8_t **block, uint32_t bytes_to_read,
                     uint32_t *bytes_read, comport_fx_rx_t fx)
{
  char          success;
  comport_dev_t *comport_dev = comport_dev_table.comport_dev[id];
  DWORD          readError = 0;

  if(!comport_dev)
    return 0;
  if (comport_dev->rx_in_progress)
  {
    return ((uint8_t)comport_dev->rx_in_progress);
  }
  comport_dev->port.on_buffer_arrival_manager = fx;
  if (!ClearCommError(comport_dev->h_comm,&readError,&comport_dev->stat))
  {
    comport_dev->last_error = GetLastError();
    comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
    return 0;
  }
  if (readError & CE_BREAK)
  {
    ClearCommBreak(comport_dev->h_comm);
    return 0;
  }

  if (readError)
  {
    comport_dev->last_error = GetLastError();
    comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
    return 0;
  }

  comport_dev->read_bytes = bytes_read;
  success = (char) ReadFile(comport_dev->h_comm, *block, (unsigned long)bytes_to_read,
    (unsigned long *)comport_dev->read_bytes, &comport_dev->ov_reader);
  if (success)
  {
    /* immediate return => data processing */
    ResetEvent(comport_dev->serialEvents[SIG_COM_READER]);
    comport_dev->rx_in_progress = 0;
    comport_dev->last_status = comport_dev->current_status;
    comport_dev->current_status = comport_read_operation_finished;
    if (comport_dev->port.on_buffer_arrival_manager != NULL)
      comport_dev->port.on_buffer_arrival_manager(comport_dev->port.id, *comport_dev->read_bytes);
  }
  else
  {
    /* GetOverlapped didn't succeed ! */
    if(GetLastError() == ERROR_IO_PENDING )
    {
      comport_dev->rx_in_progress = 1; /*wait for completion read not ended */
      comport_dev->last_status = comport_dev->current_status;
      comport_dev->current_status = comport_read_operation_ongoing;
    }
    else
    {
      comport_dev->last_status = comport_dev->current_status;
      comport_dev->current_status = comport_read_operation_finished_with_error;
      comport_dev->last_error = GetLastError();
      comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
      return 0;
    }
  }
  return ((uint8_t)comport_dev->rx_in_progress);
}


/******************************************************************************
* Function:... uint8_t COMPORT_tx(uint8_t id, uint8_t *block,
*                         uint32_t bytes_to_send, uint32_t *bytes_send, fx_t fx)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 25.01.05 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static uint8_t  COMPORT_tx_I(uint8_t id, uint8_t *block, uint32_t bytes_to_send,
                      uint32_t *bytes_send, comport_fx_tx_t fx)
{
  char          success;
  unsigned long temp;
  comport_dev_t *comport_dev = comport_dev_table.comport_dev[id];

  if(!comport_dev)
    return 0;
  comport_dev->port.on_buffer_sent_manager = fx;
  comport_dev->write_bytes += bytes_to_send;

  success = (char) WriteFile(comport_dev->h_comm, block, (unsigned long)bytes_to_send,
    &temp, &comport_dev->ov_writer);
  /* sending data on the port */
  if (success)
  {
    /* as long as the write is returning immediately,be ready to send
    *  additonnal data */
    comport_dev->tx_in_progress = 0;
    /* calling the frame to indicate that the transmission is over and
    * we're ready to receive a new data */
    comport_dev->last_status = comport_dev->current_status;
    comport_dev->current_status = comport_write_operation_finished;
    *bytes_send = temp;
  }
  else
  {
    *bytes_send = 0;
    if(GetLastError() == ERROR_IO_PENDING )
    {
      comport_dev->tx_in_progress = 1;       /* write is pending */
      comport_dev->last_status = comport_dev->current_status;
      comport_dev->current_status = comport_write_operation_ongoing;
    }
    else
    { /* failure */
      comport_dev->tx_in_progress = 2;
      comport_dev->last_status = comport_dev->current_status;
      comport_dev->current_status = comport_write_operation_finished_with_error;
    }
  }

  return ((uint8_t)(comport_dev->tx_in_progress));
}


/******************************************************************************
* Function:... uint8_t COMPORT_set_rts(uint8_t id, uint8_t new_rts)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 26.01.05 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static uint8_t COMPORT_set_rts_I(uint8_t id, uint8_t new_rts)
{
  uint8_t res;
  comport_dev_t *comport_dev = comport_dev_table.comport_dev[id];

  if (!comport_dev || comport_dev->h_comm == INVALID_HANDLE_VALUE)
    return 0;

  if (comport_dev->dwSettableParams & SP_HANDSHAKING)
  {
    GetCommState(comport_dev->h_comm, &comport_dev->dcb);
    if (new_rts)
    {
      res = (uint8_t)EscapeCommFunction(comport_dev->h_comm, SETRTS);
      comport_dev->dcb.fRtsControl = RTS_CONTROL_ENABLE;
    }
    else
    {
      res = (uint8_t)EscapeCommFunction(comport_dev->h_comm, CLRRTS);
      comport_dev->dcb.fRtsControl = RTS_CONTROL_DISABLE;
    }
    SetCommState(comport_dev->h_comm, &comport_dev->dcb);

    return ((uint8_t)res);
  }
  else
    return (0);
}

/******************************************************************************
* Function:... uint8_t COMPORT_set_dtr(uint8_t id, char new_dtr)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 24.11.01 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static uint8_t COMPORT_set_dtr_I(uint8_t id, uint8_t new_dtr)
{
  uint8_t res;
  comport_dev_t *comport_dev = comport_dev_table.comport_dev[id];

  if (!comport_dev || comport_dev->h_comm == INVALID_HANDLE_VALUE)
    return 0;

  if (comport_dev->dwSettableParams & SP_HANDSHAKING)
  {
    GetCommState(comport_dev->h_comm, &comport_dev->dcb);
    if (new_dtr)
    {
      res = EscapeCommFunction(comport_dev->h_comm, SETDTR);
      comport_dev->dcb.fDtrControl = DTR_CONTROL_ENABLE;
    }
    else
    {
      res = EscapeCommFunction(comport_dev->h_comm, CLRDTR);
      comport_dev->dcb.fDtrControl = DTR_CONTROL_DISABLE;
    }
    SetCommState(comport_dev->h_comm, &comport_dev->dcb);
  }
  else
    res = 0;
  return ((uint8_t)res);
}

/******************************************************************************
* Function:... COMPORT_set_baudrate(uint8_t id,
*                                   uint32_t Baud)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 24.11.01 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static uint8_t COMPORT_set_baudrate_I(uint8_t id, uint32_t baudrate)
{
  DWORD dwSize;
  comport_dev_t *comport_dev = comport_dev_table.comport_dev[id];
  int res;
  LPCOMMPROP lpCommProp;
  DCB        *pDcb;
  COMMTIMEOUTS cto;

  if(!comport_dev)
    return 0;
  if (comport_dev->h_comm == INVALID_HANDLE_VALUE)
  {
    comport_dev->last_error = comport_error_invalid_handle;
    return 0;
  }

  if (comport_dev->dwSettableParams & SP_BAUD)
  {
    /* Get the propierties of the comport before its initialisation */
    lpCommProp = (LPCOMMPROP) malloc(sizeof(COMMPROP));
    if(!lpCommProp)
      return 0;
    GetCommProperties(comport_dev->h_comm, lpCommProp);

    dwSize = lpCommProp->wPacketLength;
    free(lpCommProp);
    lpCommProp = (LPCOMMPROP) malloc(dwSize);
    if(!lpCommProp)
      return 0;
    GetCommProperties(comport_dev->h_comm, lpCommProp);

    res = GetCommState(comport_dev->h_comm, &comport_dev->dcb);
    pDcb = &comport_dev->dcb;
    if (res == 0)
    {
      comport_dev->last_error = GetLastError();
      comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
      free(lpCommProp);
      return 0;
    }

    pDcb->BaudRate = baudrate;
    comport_dev->comport_cfg.baudrate = pDcb->BaudRate;

    // Force the value of these to a known state as they may have been set by other programs.
    comport_dev->dcb.fOutxCtsFlow = 0;
    comport_dev->dcb.fTXContinueOnXoff = 0;
    comport_dev->dcb.fAbortOnError = 0;
    comport_dev->dcb.fBinary = 1;
    comport_dev->dcb.fOutX = 0;
    comport_dev->dcb.fInX = 0;
    comport_dev->dcb.fErrorChar = 0;
    comport_dev->dcb.fNull = 0;
    comport_dev->dcb.XonLim = 0x4000;
    comport_dev->dcb.XoffLim = 0x1000;
    comport_dev->dcb.XonChar = 0x11;
    comport_dev->dcb.XoffChar = 0x13;
    comport_dev->dcb.ErrorChar = 0;
    comport_dev->dcb.EofChar = 0;
    comport_dev->dcb.EvtChar = 0;

    res = SetCommState(comport_dev->h_comm, &comport_dev->dcb);
    if (res == 0)
    {
      comport_dev->last_error = GetLastError();
      comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
      free(lpCommProp);
      return 0;
    }
    /* Set read/write timeouts */
    if ((lpCommProp->dwProvCapabilities & PCF_INTTIMEOUTS) &&
      (lpCommProp->dwProvCapabilities & PCF_TOTALTIMEOUTS))
    {  /* Interval Timeout and total timeout supported */
      if (!GetCommTimeouts(comport_dev->h_comm, &cto))
      {
        comport_dev->last_error = GetLastError();
        comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
        free(lpCommProp);
        return 0;
      }

      cto.ReadIntervalTimeout = READ_INTERVAL_TIMEOUT_CONST;
      cto.ReadTotalTimeoutMultiplier = 0;
      cto.ReadTotalTimeoutConstant = 0;
      cto.WriteTotalTimeoutMultiplier = 0;
      cto.WriteTotalTimeoutConstant = 0/*int (byteTimeout * MAX_WRITE_BYTES)*/;
      if (!SetCommTimeouts(comport_dev->h_comm, &cto))
      {
        comport_dev->last_error = GetLastError();
        free(lpCommProp);
        return 0;
      }
    }
    else
    {
      comport_dev->last_error = GetLastError();
      comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
      free(lpCommProp);
      return 0;
    }
    /* Release memory allocated to lpCommProp */
    free(lpCommProp);

    return 0xFF;
  }
  else
  {
#if 0
    comport_dev->last_error = comport_error_baudrate_not_supported;
    comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
#endif
    return 0xFF;
  }
}

/******************************************************************************
* Function:... uint8_t Set_Stop_bits(uint8_t id, stop_bits_t stop_bits)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 26.01.05 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static uint8_t Set_stop_bits_I(uint8_t id, comport_stop_bits_t StopBits)
{
  comport_dev_t *comport_dev = comport_dev_table.comport_dev[id];
  DWORD dw_size, error;
  int res;
  LPCOMMPROP lpCommProp;
  DCB        *pDcb;

  if (!comport_dev || comport_dev->h_comm == INVALID_HANDLE_VALUE)
    return 0;
  /* Get the propierties of the comport before its initialisation */
  lpCommProp = (LPCOMMPROP) malloc(sizeof(COMMPROP));
  if(!lpCommProp)
    return 0;
  GetCommProperties(comport_dev->h_comm, lpCommProp);
  if (!ClearCommError(comport_dev->h_comm,&error,&comport_dev->stat))
  {
    comport_dev->last_error = GetLastError();
    comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
    free(lpCommProp);
    return 0;
  }

  dw_size = lpCommProp->wPacketLength;
  free(lpCommProp);
  lpCommProp = (LPCOMMPROP) malloc(dw_size);
  if(!lpCommProp)
    return 0;
  GetCommProperties(comport_dev->h_comm, lpCommProp);
  if (!ClearCommError(comport_dev->h_comm,&error,&comport_dev->stat))
  {
    comport_dev->last_error = GetLastError();
    comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
    free(lpCommProp);
    return 0;
  }

  res = GetCommState(comport_dev->h_comm, &comport_dev->dcb);
  pDcb = &comport_dev->dcb;
  if (res == 0)
  {
    comport_dev->last_error = GetLastError();
    comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
    free(lpCommProp);
    return 0;
  }

  if (!ClearCommError(comport_dev->h_comm,&error,&comport_dev->stat))
  {
    comport_dev->last_error = GetLastError();
    comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
    free(lpCommProp);
    return 0;
  }

  res = 0xFF;
  switch(StopBits)
  {
  case comport_one_stop_bit:
    if (lpCommProp->wSettableStopParity & STOPBITS_10)
      pDcb->StopBits =  ONESTOPBIT;
    else
      res = 0;
    break;
  case comport_two_stop_bits:
    if (lpCommProp->wSettableStopParity & STOPBITS_20)
      pDcb->StopBits =  TWOSTOPBITS;
    else
      res = 0;
    break;
  default:
    if (lpCommProp->wSettableStopParity & STOPBITS_10)
      pDcb->StopBits =  ONESTOPBIT;
    else
      res = 0;
  }
  if(res != 0)
  {
    res = SetCommState( comport_dev->h_comm, &comport_dev->dcb);
    if(res == 0)
    {
      comport_dev->last_error = GetLastError();
      comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
    }
    else
      res = 0xFF; /* Mark as success */
  }

  /* Release memory allocated to lpCommProp */
  free(lpCommProp);
  return res;
}

/******************************************************************************
* Function:... char Set_data_bits(uint8_t id)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 26.01.05 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static uint8_t Set_data_bits_I(uint8_t id)
{
  comport_dev_t *comport_dev = comport_dev_table.comport_dev[id];
  DWORD dwSize, error, res;
  LPCOMMPROP lpCommProp;
  DCB        *pDcb;

  if (!comport_dev || comport_dev->h_comm == INVALID_HANDLE_VALUE)
    return 0;
  /* Get the propierties of the comport before its initialisation */
  lpCommProp = (LPCOMMPROP) malloc(sizeof(COMMPROP));
  if(!lpCommProp)
    return 0;
  GetCommProperties(comport_dev->h_comm, lpCommProp);

  if (!ClearCommError(comport_dev->h_comm,&error,&comport_dev->stat))
  {
    comport_dev->last_error = GetLastError();
    comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
    free(lpCommProp);
    return 0;
  }

  dwSize = lpCommProp->wPacketLength;
  free(lpCommProp);
  lpCommProp = (LPCOMMPROP) malloc(dwSize);
  if(!lpCommProp)
    return 0;
  GetCommProperties(comport_dev->h_comm, lpCommProp);

  res = GetCommState(comport_dev->h_comm, &comport_dev->dcb);
  pDcb = &comport_dev->dcb;

  if (res == 0)
  {
    comport_dev->last_error = GetLastError();
    comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
    if (comport_dev->h_comm == INVALID_HANDLE_VALUE)
    {
      free(lpCommProp);
      return 0;
    }
  }

  if (lpCommProp->wSettableData & DATABITS_8)
    pDcb->ByteSize = 8;
  else
  {
    free(lpCommProp);
    return 0;
  }
  res = SetCommState(comport_dev->h_comm, &comport_dev->dcb);
  if (res == 0)
  {
    comport_dev->last_error = GetLastError();
    comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
    free(lpCommProp);
    return 0;
  }

  if (!ClearCommError(comport_dev->h_comm,&error,&comport_dev->stat))
  {
    comport_dev->last_error = GetLastError();
    comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
    free(lpCommProp);
    return 0;
  }
  /* Release memory allocated to lpCommProp */
  free(lpCommProp);
  return 0xFF;
}

/******************************************************************************
* Function:... char SetParity(uint8_t id, comport_parity_t parity)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 24.11.01 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static uint8_t Set_parity_I(uint8_t id, comport_parity_t parity)
{
  comport_dev_t *comport_dev = comport_dev_table.comport_dev[id];
  DWORD dwSize, error;
  int res;
  LPCOMMPROP lpCommProp;
  DCB        *pDcb;

  if (!comport_dev || comport_dev->h_comm == INVALID_HANDLE_VALUE)
    return 0;

  /* Get the propierties of the comport before its initialisation */
  lpCommProp = (LPCOMMPROP) malloc(sizeof(COMMPROP));
  if(!lpCommProp)
    return 0;

  GetCommProperties(comport_dev->h_comm, lpCommProp);
  if (!ClearCommError(comport_dev->h_comm,&error,&comport_dev->stat))
  {
    comport_dev->last_error = GetLastError();
    comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
    free(lpCommProp);
    return 0;
  }

  dwSize = lpCommProp->wPacketLength;
  free(lpCommProp);
  lpCommProp = (LPCOMMPROP) malloc(dwSize);
  if(!lpCommProp)
    return 0;
  GetCommProperties(comport_dev->h_comm, lpCommProp);
  if (!ClearCommError(comport_dev->h_comm,&error,&comport_dev->stat))
  {
    comport_dev->last_error = GetLastError();
    comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
    free(lpCommProp);
    return 0;
  }

  res = GetCommState(comport_dev->h_comm, &comport_dev->dcb);
  pDcb = &comport_dev->dcb;
  if (res == 0)
  {
    comport_dev->last_error = GetLastError();
    comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
    free(lpCommProp);
    return 0;
  }
  if (!ClearCommError(comport_dev->h_comm,&error,&comport_dev->stat))
  {
    comport_dev->last_error = GetLastError();
    comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
    free(lpCommProp);
    return 0;
  }

  /* Set parity bit */
  switch (parity)
  {
    case comport_no_parity:
      if (lpCommProp->wSettableStopParity & PARITY_NONE)
      {
        pDcb->fParity = 0;
        pDcb->Parity = NOPARITY;
      }
      break;
    case comport_odd_parity:
      if (lpCommProp->wSettableStopParity & PARITY_ODD)
      {
        pDcb->fParity = 1;
        pDcb->Parity = ODDPARITY;
      }
      break;
    case comport_even_parity:
      if (lpCommProp->wSettableStopParity & PARITY_EVEN)
      {
        pDcb->fParity = 1;
        pDcb->Parity = EVENPARITY;
      }
      break;
    case comport_mark_parity:
      if (lpCommProp->wSettableStopParity & PARITY_MARK)
      {
        pDcb->fParity = 1;
        pDcb->Parity = MARKPARITY;
      }
      break;
    case comport_space_parity:
      if (lpCommProp->wSettableStopParity & PARITY_SPACE)
      {
        pDcb->fParity = 1;
        pDcb->Parity = SPACEPARITY;
      }
      break;
    default:
      if (lpCommProp->wSettableStopParity & PARITY_NONE)
      {
        pDcb->fParity = 0;
        pDcb->Parity = NOPARITY;
      }
      else
      {
        free(lpCommProp);
        return 0;
      }
  }
  res = SetCommState(comport_dev->h_comm, &comport_dev->dcb);
  if (res == 0)
  {
    comport_dev->last_error = GetLastError();
    comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
    free(lpCommProp);
    return 0;
  }
  if (!ClearCommError(comport_dev->h_comm,&error,&comport_dev->stat))
  {
    comport_dev->last_error = GetLastError();
    comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
    free(lpCommProp);
    return 0;
  }
  /* Release memory allocated to lpCommProp */
  free(lpCommProp);
  return 0xFF;
}

/******************************************************************************
* Function:... uint8_t  COMPORT_set_buffer_size(uint8_t id, uint32_t rx_buf_size,
*                                               uint32_t tx_buf_size)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 24.11.01 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static uint8_t COMPORT_set_buffer_size_I(uint8_t id, uint32_t rx_buf_size, uint32_t tx_buf_size)
{
  LPCOMMPROP lpCommProp;
  comport_dev_t *comport_dev = comport_dev_table.comport_dev[id];

  if(!comport_dev)
    return 0;
  /* Get the propierties of the comport before its initialisation */
  lpCommProp = (LPCOMMPROP) malloc(sizeof(COMMPROP));
  if(!lpCommProp)
    return 0;
  GetCommProperties(comport_dev->h_comm, lpCommProp);

  if (tx_buf_size > lpCommProp->dwMaxTxQueue)
    tx_buf_size = lpCommProp->dwMaxTxQueue;

  if (rx_buf_size > lpCommProp->dwMaxRxQueue)
    rx_buf_size = lpCommProp->dwMaxRxQueue;

  if (!SetupComm(comport_dev->h_comm, rx_buf_size , tx_buf_size))
  {
    comport_dev->last_error = GetLastError();
    comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
    free(lpCommProp);
    return 0;
  }

  /* Release memory allocated to lpCommProp */
  free(lpCommProp);

  return 0xFF;
}

/******************************************************************************
* Function:... uint32_t COMPORT_get_rx_used_buffer_size(uint8_t id)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 25.01.05 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static uint32_t COMPORT_get_rx_used_buffer_size_I(uint8_t id)
{
  DWORD      error;
  comport_dev_t *comport_dev = comport_dev_table.comport_dev[id];

  if(!comport_dev)
    return 0;
  if (!ClearCommError(comport_dev->h_comm, &error, &comport_dev->stat))
  {
    comport_dev->last_error = GetLastError();
    comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
    return 0;
  }
  if (error & CE_BREAK)
    ClearCommBreak(comport_dev->h_comm);

  return (uint32_t) comport_dev->stat.cbInQue;
}

/******************************************************************************
* Function:... uint8_t  COMPORT_flush_rx_buffer(uint8_t id)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 30.01.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static uint8_t  COMPORT_flush_rx_buffer_I(uint8_t id)
{
  comport_dev_t *comport_dev = comport_dev_table.comport_dev[id];

  if(!comport_dev)
    return 0;

  if (!comport_dev->usb_port)
  {
    if (!PurgeComm(comport_dev->h_comm, PURGE_RXABORT | PURGE_RXCLEAR))
    {
      comport_dev->last_error = GetLastError();
      comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
      return 0;
    }
  }
  comport_dev->rx_in_progress = 0;
  return 0xFF;
}

/******************************************************************************
* Function:... uint8_t  COMPORT_flush_tx_buffer(uint8_t id)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 30.01.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static uint8_t  COMPORT_flush_tx_buffer_I(uint8_t id)
{
  comport_dev_t *comport_dev = comport_dev_table.comport_dev[id];

  if(!comport_dev)
    return 0;

  if (!comport_dev->usb_port)
  {
    if (!PurgeComm(comport_dev->h_comm, PURGE_TXABORT | PURGE_TXCLEAR))
    {
      comport_dev->last_error = GetLastError();
      comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
      return 0;
    }
  }
  return 0xFF;

}

/******************************************************************************
* Function:... static uint8_t Set_comm_timeouts(uint8_t id, LPCOMMPROP lpCommProp)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 26.01.05 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static uint8_t Set_comm_timeouts_I(uint8_t id,  LPCOMMPROP const lpCommProp)
{
  comport_dev_t *comport_dev = comport_dev_table.comport_dev[id];
  DWORD        error;
  COMMTIMEOUTS cto;

  if (!comport_dev || comport_dev->h_comm == INVALID_HANDLE_VALUE)
    return 0;
  if(lpCommProp == 0)
    return 0;
  if ((lpCommProp->dwProvCapabilities & PCF_INTTIMEOUTS) &&
    (lpCommProp->dwProvCapabilities & PCF_TOTALTIMEOUTS))
  {  /* Interval Timeout and total timeout supported */
    if (!GetCommTimeouts(comport_dev->h_comm, &cto))
    {
      comport_dev->last_error = GetLastError();
      comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
      return 0;
    }
    if (!ClearCommError(comport_dev->h_comm,&error,&comport_dev->stat))
    {
      comport_dev->last_error = GetLastError();
      comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
      return 0;
    }

    cto.ReadIntervalTimeout = READ_INTERVAL_TIMEOUT_CONST;
    cto.ReadTotalTimeoutMultiplier = 0;
    cto.ReadTotalTimeoutConstant = 0;
    cto.WriteTotalTimeoutMultiplier = 0;
    cto.WriteTotalTimeoutConstant = 0;
    if (!SetCommTimeouts(comport_dev->h_comm, &cto))
    {
      comport_dev->last_error = GetLastError();
      comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
      return 0;
    }
    if (!ClearCommError(comport_dev->h_comm, &error, &comport_dev->stat))
    {
      comport_dev->last_error = GetLastError();
      comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
      return 0;
    }

  }
  else
  {
    comport_dev->last_error = GetLastError();
    comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
    return 0;
  }
  return 0xFF;
}

/******************************************************************************
* Function:... uint8_t disable_hw_flow_ctrl(uint8_t id)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 24.01.05 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static uint8_t disable_hw_flow_ctrl_I(uint8_t id)
{
  int res;
  DCB        *pDcb;
  DWORD       error;
  comport_dev_t *comport_dev = comport_dev_table.comport_dev[id];

  if (!comport_dev || comport_dev->h_comm == INVALID_HANDLE_VALUE)
    return 0;


  res = GetCommState(comport_dev->h_comm, &comport_dev->dcb);
  pDcb = &comport_dev->dcb;
  if (res == 0)
  {
    comport_dev->last_error = GetLastError();
    comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
    return 0;
  }


  if (!ClearCommError(comport_dev->h_comm,&error,&comport_dev->stat))
  {
    comport_dev->last_error = GetLastError();
    comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
    return 0;
  }

  /* Disable HW flow control and DTR Control */
  pDcb->fDtrControl = DTR_CONTROL_DISABLE;
  pDcb->fOutxCtsFlow = false;
  pDcb->fOutxDsrFlow = false;
  pDcb->fRtsControl = RTS_CONTROL_DISABLE;
  pDcb->fOutxCtsFlow = false;
  pDcb->fOutxDsrFlow = false;
  pDcb->fDsrSensitivity = false;

  #if 0
    // Enable HW Flow Control
    pDcb->fOutxCtsFlow = true;
    pDcb->fRtsControl = RTS_CONTROL_HANDSHAKE;
  #endif

  if (res == 0)
  {
    comport_dev->last_error = GetLastError();
    comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
    return 0;
  }

  if (!ClearCommError(comport_dev->h_comm,&error,&comport_dev->stat))
  {
    comport_dev->last_error = GetLastError();
    comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
    return 0;
  }
  return 0xFF;
}

/******************************************************************************
* Function:... uint8_t  COMPORT_get_modem_status(uint8_t id,
comm_modem_status_line_t *modem_status_line);
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 05.09.04 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static uint8_t  COMPORT_get_modem_status_I(uint8_t id,
                                    comport_modem_status_line_t *modem_status_line)
{
  DWORD modemLineStatus;
  DCB        *pDcb;
  comport_modem_status_line_t temp_status;
  int res;
  comport_dev_t *comport_dev = comport_dev_table.comport_dev[id];

  if(!comport_dev)
    return 0;
  temp_status.comport_ms_cts_on = 0;
  temp_status.comport_ms_dsr_on = 0;
  temp_status.comport_ms_dtr_on = 0;
  temp_status.comport_ms_ring_on = 0;
  temp_status.comport_ms_rlsd_on = 0;
  temp_status.comport_ms_rts_on = 0;

  res = GetCommModemStatus(comport_dev->h_comm,&modemLineStatus);
  if (res == 0)
  {
    comport_dev->last_error = GetLastError();
    comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
    return 0;
  }

  if ((modemLineStatus & MS_CTS_ON) == MS_CTS_ON)
    temp_status.comport_ms_cts_on = 0xFF;
  if ((modemLineStatus & MS_DSR_ON) == MS_DSR_ON)
    temp_status.comport_ms_dsr_on = 0xFF;
  if ((modemLineStatus & MS_RING_ON) == MS_RING_ON)
    temp_status.comport_ms_ring_on = 0xFF;
  if ((modemLineStatus & MS_RLSD_ON) == MS_RLSD_ON)
    temp_status.comport_ms_rlsd_on = 0xFF;

  /* Get the state of rts, dtr and dsr */
  res = GetCommState(comport_dev->h_comm, &comport_dev->dcb);
  pDcb = &comport_dev->dcb;
  if (res == 0)
  {
    comport_dev->last_error = GetLastError();
    comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
    return 0;
  }
  else
  {
    if ((pDcb->fRtsControl & RTS_CONTROL_ENABLE) == RTS_CONTROL_ENABLE)
      temp_status.comport_ms_rts_on = 0xFF;
    if ((pDcb->fDtrControl & DTR_CONTROL_ENABLE) == DTR_CONTROL_ENABLE)
      temp_status.comport_ms_dtr_on = 0xFF;
  }

  *modem_status_line = temp_status;
  return 0xFF;

}

/******************************************************************************
* Function:... void COMPORT_process_communication(uint8_t id)
* Parameters: None
* Returns:.... Nothing
* Description:
* Created:.... 30.01.06 by (AlanBugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Name, DWD / Initials)
*
******************************************************************************/
void COMPORT_process_communication_I(uint8_t id)
{
  long            status;
  unsigned long   result;
  int             i;
  comport_dev_t *comport_dev = comport_dev_table.comport_dev[id];

  if (comport_dev && !comport_dev->done)
  {
    /********************************************************/
    /*            Waiting  for signals                      */
    /********************************************************/
    /* Main wait function. Waiting for something to happen. */
    /* This may be either the completion of a Read or a Write or */
    /* the reception of Power On, Power Down, new Tx */

    status = WaitForMultipleObjects(TSERIAL_SIGNAL_NBR, comport_dev->serialEvents,
      false, 0);

    /* processing answer to filter other failures */
    status = status - WAIT_OBJECT_0;
    if (((status<0) || (status>=TSERIAL_SIGNAL_NBR)) && (status != WAIT_TIMEOUT))
    {
      comport_dev->done=1;
    }
    else
    {
      /********************** EVENT DISPATCHER ********************/
      switch(status)
      {
      case WAIT_TIMEOUT:
        /* Operation isn't complete yet. fWaitingOnRead flag isn't */
        /* changed since I'll loop back around, and I don't want   */
        /* to issue another read until the first one finishes.     */
        /*                                                         */
        /* This is a good time to do some background work.         */

        break;
        /************************************************************/
      case SIG_COM_READY:

        comport_dev->ready = 1;
        break;
        /************************************************************/
        /************************************************************/
      case SIG_COM_READER:

        /* reading the result of the terminated read */
        if (GetOverlappedResult(comport_dev->h_comm, &comport_dev->ov_reader,
          (unsigned long*)comport_dev->read_bytes, false))
        {
          /* no error => OK */
          comport_dev->last_status = comport_dev->current_status;
          comport_dev->current_status = comport_read_operation_finished;
          /* Read operation completed successfully */
          ResetEvent(comport_dev->serialEvents[SIG_COM_READER]);
          /* Write operation completed successfully */
          if (comport_dev->port.on_buffer_arrival_manager != NULL)
            comport_dev->port.on_buffer_arrival_manager(comport_dev->port.id, *comport_dev->read_bytes);
          /* if incoming data, I process them */
          comport_dev->rx_in_progress = 0; /* read has ended */
        }
        else
        {
          /* GetOverlapped didn't succeed ! */
          if(GetLastError() == ERROR_IO_PENDING )

            comport_dev->rx_in_progress = 1; /* wait for completion read not ended */
          else
          {
            comport_dev->last_error = GetLastError();
            comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
            return;
          }
        }
        break;
        /************************************************************/
      case SIG_COM_WRITER:
        /* WriteFile has terminated. checkin the result of the operation */
        if (GetOverlappedResult(comport_dev->h_comm, &comport_dev->ov_writer, &result, false))
        { /* Write operation completed successfully */
          comport_dev->last_status = comport_dev->current_status;
          comport_dev->current_status = comport_write_operation_finished;
          ResetEvent(comport_dev->serialEvents[SIG_COM_WRITER]);
          comport_dev->tx_in_progress = 0; /* write has ended */
          if (comport_dev->port.on_buffer_sent_manager!= NULL)
            comport_dev->port.on_buffer_sent_manager(comport_dev->port.id, result);
        }
        else
        { /* GetOverlapped didn't succeed ! */
          if(GetLastError() == ERROR_IO_PENDING )
            comport_dev->tx_in_progress = 1; /* write not ended, wait for completion */
          else
          {
            comport_dev->last_error = GetLastError();
            comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
            ResetEvent(comport_dev->serialEvents[SIG_COM_WRITER]);
            comport_dev->tx_in_progress = 0; /* write has ended */
            if (comport_dev->port.on_buffer_sent_manager!= NULL)
            {
              comport_dev->port.on_buffer_sent_manager(comport_dev->port.id, 0); // Send length 0 to signal error.
            }
            return;
          }
        }
        break;

        /************************************************************/
      case SIG_COM_CLOSE:
        comport_dev->done = 1;

        for (i=0; i<TSERIAL_SIGNAL_NBR; i++)
        {
          if (comport_dev->serialEvents[i]!=INVALID_HANDLE_VALUE)
          {
            if(!CloseHandle(comport_dev->serialEvents[i]))
            {
              comport_dev->last_error = GetLastError();
              return;
            }
            else
            {
              comport_dev->serialEvents[i] = INVALID_HANDLE_VALUE; /* JMB: marked as invalid after closing it - needed? */
              comport_dev->last_error = (intptr_t)INVALID_HANDLE_VALUE;
              comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
              return;
            }
          }
        }
        break;
      }

    }
  }
  /* deleting the events */

}

/******************************************************************************
* Function:... comport_error_t  COMPORT_get_last_error(uint8_t id)
* Parameters:.
* Returns:....
* Description:
* Created:.... 30.01.06 by (AlanBugtrup, DWD / AB)
* Modified:...
*
******************************************************************************/
static comport_status_t COMPORT_get_status_I(uint8_t id)
{
  comport_dev_t *comport_dev = comport_dev_table.comport_dev[id];
  if(!comport_dev)
    return comport_status_no_event;
  return comport_dev->current_status;
}

/******************************************************************************
* Function:... comport_error_t  COMPORT_get_last_error(uint8_t id)
* Parameters:.
* Returns:....
* Description:
* Created:.... 30.01.06 by (AlanBugtrup, DWD / AB)
* Modified:...
*
******************************************************************************/
static comport_error_t  COMPORT_get_last_error_I(uint8_t id)
{
  comport_dev_t *comport_dev = comport_dev_table.comport_dev[id];
  comport_error_t error;

  if(!comport_dev)
    return comport_error_access_denied;
  switch(comport_dev->last_error)
  {
  case ERROR_INVALID_HANDLE:
    error = comport_error_invalid_handle;
    break;
  case ERROR_ACCESS_DENIED:
    error = comport_error_access_denied;
    break;
  case ERROR_NOT_READY:
    error = comport_error_device_not_ready;
    break;
  case ERROR_ALREADY_ASSIGNED:
    error = comport_error_already_assigned;
    break;
  case ERROR_OPEN_FAILED:
    error = comport_error_open_failed;
    break;
  case ERROR_BUSY:
    error = comport_error_busy;
    break;
  case ERROR_ALREADY_EXISTS:
    error = comport_error_already_exist;
    break;
  case ERROR_IO_DEVICE:
    error = comport_error_io_device;
    break;
  case ERROR_SERIAL_NO_DEVICE:
    error = comport_error_serial_no_device;
    break;
  case ERROR_MORE_WRITES:;
    error = comport_error_more_writes;
    break;
  case ERROR_COUNTER_TIMEOUT:
    error = comport_error_counter_timeout;
    break;
  case ERROR_NO_MORE_DEVICES:
    error = comport_error_no_more_devices;
    break;
  case CE_BREAK:
    error = comport_ce_break;
    break;
  case CE_FRAME:
    error = comport_ce_frame;
    break;
  case CE_IOE:
    error = comport_ce_ioe;
    break;
  case CE_MODE:
    error = comport_ce_mode;
    break;
  case CE_OVERRUN:
    error = comport_ce_overrun;
    break;
  case CE_RXOVER:
    error = comport_ce_rxover;
    break;
  case CE_RXPARITY:
    error = comport_ce_rxparity;
    break;
  case CE_TXFULL:
    error = comport_ce_txfull;
    break;

  default:
    error = comport_error_unkown;
    break;
  }
  return error;
}

/******************************************************************************
* Function:... void COMPORT_close_event_handler(comport_dev_t *comport_dev_temp)
* Parameters:.
* Returns:....
* Description:
* Created:.... 30.01.06 by (AlanBugtrup, DWD / AB)
* Modified:...
*
******************************************************************************/
static void COMPORT_close_event_handler_I(comport_dev_t *comport_dev_temp)
{
  int i;
  if (comport_dev_temp != NULL)
  {
    /*creating Events for the different sources */
    for (i=0; i<TSERIAL_SIGNAL_NBR; i++)
    {
      if ((comport_dev_temp->serialEvents[i] != INVALID_HANDLE_VALUE) &&
        (comport_dev_temp->serialEvents[i] != NULL))
        CloseHandle(comport_dev_temp->serialEvents[i]);
    }
  }
}


/*******************************************************************************
  Functions to use the GPIO pins of a Prolific PL2303 chip
*******************************************************************************/

typedef struct
{
  HANDLE    port_handle;
  uint8_t   close_local;
  uint32_t  gpio_mask;
} prolific_handling_type;

prolific_handling_type prolific_handling[MAX_COMPORT_INSTANCES+1] = {{NULL, false, 0},{NULL, false, 0},{NULL, false, 0},{NULL, false, 0},{NULL, false, 0},{NULL, false, 0},{NULL, false, 0},{NULL, false, 0},{NULL, false, 0},{NULL, false, 0},{NULL, false, 0},{NULL, false, 0},{NULL, false, 0},{NULL, false, 0},{NULL, false, 0},{NULL, false, 0},{NULL, false, 0},{NULL, false, 0},{NULL, false, 0},{NULL, false, 0},{NULL, false, 0},{NULL, false, 0},{NULL, false, 0},{NULL, false, 0},{NULL, false, 0},{NULL, false, 0},{NULL, false, 0},{NULL, false, 0},{NULL, false, 0},{NULL, false, 0},{NULL, false, 0},{NULL, false, 0},{NULL, false, 0}};

#define USB2SER_IOCTL_INDEX  0x0800

#define GP0_OUTPUT_ENABLE CTL_CODE(FILE_DEVICE_UNKNOWN, USB2SER_IOCTL_INDEX+20, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define GP1_OUTPUT_ENABLE CTL_CODE(FILE_DEVICE_UNKNOWN, USB2SER_IOCTL_INDEX+21, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define GP0_SET_VALUE CTL_CODE(FILE_DEVICE_UNKNOWN, USB2SER_IOCTL_INDEX+22, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define GP1_SET_VALUE CTL_CODE(FILE_DEVICE_UNKNOWN, USB2SER_IOCTL_INDEX+23, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define GP0_GET_VALUE CTL_CODE(FILE_DEVICE_UNKNOWN, USB2SER_IOCTL_INDEX+24, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define GP1_GET_VALUE CTL_CODE(FILE_DEVICE_UNKNOWN, USB2SER_IOCTL_INDEX+25, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define GP23_GET_DATA CTL_CODE(FILE_DEVICE_UNKNOWN, USB2SER_IOCTL_INDEX+38, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define SEND_ENCAP CTL_CODE(FILE_DEVICE_UNKNOWN, USB2SER_IOCTL_INDEX+3, METHOD_BUFFERED, FILE_ANY_ACCESS)




//---------------------------------------------------------------------------
DWORD COMPORT_prolific_open_port(const char *port_name, HANDLE *port_handle)
{
  DWORD result = 0; // Assume success

  *port_handle = CreateFile(port_name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

  if (*port_handle == INVALID_HANDLE_VALUE)
     result = GetLastError();

  return result;
}

DWORD COMPORT_prolific_close_port(HANDLE *port_handle)
{
   DWORD result = 0; // Assume success

   if (!CloseHandle(*port_handle))
     result = GetLastError();
   else
     *port_handle = NULL;

  return result;
}


DWORD COMPORT_prolific_set_gpio(HANDLE port_handle, int gpio_num, int gpio_level)
{
  DWORD result = 0;      // Assume success
  DWORD written;         // Dummy var to read the bytes written by the IoControl ... not used
  uint8_t do_enable = true; // So the IoControl can point to it.

  switch(gpio_num)
  {
     case 0:
     {
        if (DeviceIoControl(port_handle, GP0_OUTPUT_ENABLE, &do_enable, sizeof(BYTE), NULL, 0, &written, NULL))
          if (DeviceIoControl(port_handle, GP0_SET_VALUE, &gpio_level, sizeof(BYTE), NULL, 0, &written, NULL))
            break; // Good case exit
        result = GetLastError(); // Could not set the GPIO.
        break; // Error case exit
     }
     case 1:
     {
        if (DeviceIoControl(port_handle, GP1_OUTPUT_ENABLE, &do_enable, sizeof(BYTE), NULL, 0, &written, NULL))
          if (DeviceIoControl(port_handle, GP1_SET_VALUE, &gpio_level, sizeof(BYTE), NULL, 0, &written, NULL))
            break; // Good case exit
        result = GetLastError(); // Could not set the GPIO.
        break; // Error case exit
     }
     case 2:
     case 3:
     {
        WORD value[2];

        value[0] = 141; // cmd
        value[1] = 0;
        if (!DeviceIoControl(port_handle, GP23_GET_DATA, value, sizeof(value), value, sizeof(value), &written, NULL))
        {
          result = GetLastError(); // Could not get the GPIO values.
          break; // Error case exit
        }

        if (gpio_level)
          value[1] = value[0] |  ((BYTE)(gpio_num-1));
        else
          value[1] = value[0] & ~((BYTE)(gpio_num-1));

        value[0] = 13;  // cmd

        if (!DeviceIoControl(port_handle, SEND_ENCAP, value, sizeof(value), NULL, 0, &written, NULL))
          result = GetLastError(); // Could not get the GPIO values.
          // Error case exit
        break;
     }

     default:
     {
        result = ERROR_NOT_SUPPORTED; // GPIO number is not supported.
     }
  }

  return result;
}

uint8_t COMPORT_set_gp0(uint8_t channel, uint8_t value)
{
  comport_dev_t *comport_dev;
  uint8_t result = false;

  if(channel > 0 && channel <= MAX_COMPORT_INSTANCES)
  {
    comport_dev = comport_dev_table.comport_dev[channel];
    if(comport_dev && comport_dev->h_comm != INVALID_HANDLE_VALUE)
    {
      COMPORT_prolific_set_gpio(comport_dev->h_comm,0,(int)value);
    }
  }
  return result;
}

uint8_t COMPORT_stop_prolific_board_reset(uint8_t channel)
{
  if(channel > 0 && channel <= MAX_COMPORT_INSTANCES)
  {
    if (prolific_handling[0].port_handle)
    {
      prolific_handling[channel].close_local = prolific_handling[0].close_local;
      prolific_handling[channel].gpio_mask   = prolific_handling[0].gpio_mask;
      prolific_handling[channel].port_handle = prolific_handling[0].port_handle;
      prolific_handling[0].port_handle = NULL;
    }
    if (prolific_handling[channel].close_local)
    {
      Debug("Close prolific port \n");
      COMPORT_prolific_close_port(&prolific_handling[channel].port_handle);
      prolific_handling[channel].close_local = false;
      prolific_handling[channel].gpio_mask   = 0;
      prolific_handling[channel].port_handle = NULL;
    }
  }
  return true;
}

uint8_t COMPORT_init_prolific_board_reset(uint8_t channel, char *reset_port_name, uint32_t gpio_mask)
{
  comport_dev_t *comport_dev;
  int i;
  uint8_t result = true; // Assume success

  if(channel > 0 && channel <= MAX_COMPORT_INSTANCES)
  {
    comport_dev = comport_dev_table.comport_dev[channel];
    if(!comport_dev)
    {
      return false;
    }
    if(comport_dev->h_comm == INVALID_HANDLE_VALUE)
    {
      return false;
    }
    Debug("Open prolific port %s\n",reset_port_name);
    if (reset_port_name)
    {
      // got a port name so we must open the port
      if (COMPORT_prolific_open_port(reset_port_name,&prolific_handling[channel].port_handle))
      {
        return false;
      }
      prolific_handling[channel].close_local = true;
    }
    else
    {
      // no port name so use the existing port
      prolific_handling[channel].port_handle = comport_dev->h_comm;
      prolific_handling[channel].close_local = false;
    }
    prolific_handling[channel].gpio_mask = gpio_mask;

    // finally set the reset port(s) high
    for (i=0; i<4; i++)
    {
      if (prolific_handling[channel].gpio_mask & (1<<i))
        COMPORT_prolific_set_gpio(prolific_handling[channel].port_handle, i, 1); // 1 = set port high
    }
  }
  else if (channel == 0)
  {
    // Not a valid channel so use channel 0
    Debug("COMM open channel 0\n");
    if (COMPORT_prolific_open_port(reset_port_name,&prolific_handling[0].port_handle))
    {
      return false;
    }
    prolific_handling[0].close_local = true;
    prolific_handling[0].gpio_mask = gpio_mask;

    // finally set the reset port(s) high
    for (i=0; i<4; i++)
    {
      if (prolific_handling[channel].gpio_mask & (1<<i))
        COMPORT_prolific_set_gpio(prolific_handling[0].port_handle, i, 1); // 1 = set port high
    }

  }
  return result;
}

void COMPORT_prolific_port_reset_cb(uint8_t channel)
{
  int i;

  if(channel > 0 && channel <= MAX_COMPORT_INSTANCES)
  {
    if (prolific_handling[0].port_handle)
    {
      prolific_handling[channel].close_local = prolific_handling[0].close_local;
      prolific_handling[channel].gpio_mask   = prolific_handling[0].gpio_mask;
      prolific_handling[channel].port_handle = prolific_handling[0].port_handle;
      prolific_handling[0].port_handle = NULL;
    }

    Debug("Prolific callback\n");
    for (i=0; i<4; i++)
    {
      if (prolific_handling[channel].gpio_mask & (1<<i))
      {
        Debug("Set gpio %d\n",i);
        COMPORT_prolific_set_gpio(prolific_handling[channel].port_handle, i, 0); // 0 = set port low
      }
    }
  }
}

/*****************************************************************************
   EMULATION FUNCTIONS:
*****************************************************************************/
static uint8_t COMPORT_set_baudrate_E(uint8_t id, uint32_t baudrate)
{
  emulation_dev_t  *dev = comport_emu_dev_table.dev[id];

  if (!dev || dev->h_comm == 0)
    return 0;

  dev->baudrate = baudrate;

  return 0xFF;
}
//------------------------------------------------------------------------------
static uint8_t  COMPORT_get_modem_status_E(uint8_t id, comport_modem_status_line_t *modem_status_line)
{
//  DWORD modemLineStatus;
  comport_modem_status_line_t temp_status;
//  int res;
  emulation_dev_t  *dev = comport_emu_dev_table.dev[id];

  if (!dev || dev->h_comm == 0)
    return 0;

  temp_status.comport_ms_cts_on = 0;
  temp_status.comport_ms_dsr_on = 0;
  temp_status.comport_ms_dtr_on = 0;
  temp_status.comport_ms_ring_on = 0;
  temp_status.comport_ms_rlsd_on = 0;
  temp_status.comport_ms_rts_on = 0;

  if (dev->rts == 0xFF )
  {
    temp_status.comport_ms_rts_on = 0xFF;
  }
  if (dev->dtr == 0xFF)
  {
    temp_status.comport_ms_dtr_on = 0xFF;
  }

  *modem_status_line = temp_status;
  return 0xFF;
}
//------------------------------------------------------------------------------
static uint8_t  COMPORT_close_port_E(uint8_t id)
{
  emulation_dev_t  *dev = comport_emu_dev_table.dev[id];

  if (!dev || dev->h_comm == 0)
    return 0;
  dev->h_comm = 0;

  return 0xFF;
}
//------------------------------------------------------------------------------
static uint8_t  COMPORT_set_buffer_size_E(uint8_t id, uint32_t rx_buf_size, uint32_t tx_buf_size)
{
  emulation_dev_t  *dev = comport_emu_dev_table.dev[id];

  if (!dev || dev->h_comm == 0)
    return 0;

  if (rx_buf_size < IN_BUFFER_SIZE)
  {
    dev->rx_buffer_size = rx_buf_size;
  }
  else
  {
    dev->rx_buffer_size = IN_BUFFER_SIZE;
  }

  if (tx_buf_size < OUT_BUFFER_SIZE)
  {
    dev->tx_buffer_size = tx_buf_size;
  }
  else
  {
    dev->tx_buffer_size = OUT_BUFFER_SIZE;
  }

  return 0xFF;
}

//------------------------------------------------------------------------------
static uint32_t COMPORT_get_rx_used_buffer_size_E(uint8_t id)
{
  uint32_t result = 0;
  emulation_dev_t  *dev = comport_emu_dev_table.dev[id];

  if(dev && dev->h_comm)
  {
  //  return IN_BUFFER_SIZE;
    if(GLOBAL_EmulateRxAvail)
      result = GLOBAL_EmulateRxAvail(id);
//    if(result)
//    {
//      Sleep(55); // don't know why this is needed! but if I don't I will get get "Device Syncronized" during test script emulation
//    }
  }
  return result;
}

//------------------------------------------------------------------------------
static uint8_t COMPORT_rx_E(uint8_t id, uint8_t **block, uint32_t bytes_to_read, uint32_t *bytes_read, comport_fx_rx_t fx)
{
  emulation_dev_t  *dev = comport_emu_dev_table.dev[id];
  char             success;

  if (!dev || dev->h_comm == 0)
    return 0;

  if (dev->rx_in_progress)
  {
    return ((uint8_t)dev->rx_in_progress);
  }

  dev->port.on_buffer_arrival_manager = fx;
  dev->read_bytes = bytes_read;
  success = (char) EMU_ReadFile(id, *block, bytes_to_read, dev->read_bytes);
  if (success)
  {
    /* immediate return => data processing */
    dev->rx_in_progress = 0;
    dev->last_status = dev->current_status;
    dev->current_status = comport_read_operation_finished;
    if (dev->port.on_buffer_arrival_manager != NULL)
      dev->port.on_buffer_arrival_manager(dev->port.id, *dev->read_bytes);
  }
  else
  {
    dev->rx_in_progress = 1; /*wait for completion read not ended */
    dev->last_status = dev->current_status;
    dev->current_status = comport_read_operation_ongoing;
  }
  return ((uint8_t)dev->rx_in_progress);
}

//------------------------------------------------------------------------------
static uint8_t  COMPORT_tx_E(uint8_t id, uint8_t *block, uint32_t bytes_to_send,
                      uint32_t *bytes_send, comport_fx_tx_t fx)
{
  char          success;
//  unsigned long temp;
  emulation_dev_t  *dev = comport_emu_dev_table.dev[id];

  if (!dev || dev->h_comm == 0)
    return 0;
  dev->port.on_buffer_sent_manager = fx;

  dev->bytes_sent = bytes_send;
  dev->temp_bytes_sent = 0;

  success = (char) EMU_WriteFile(id, block, (unsigned long)bytes_to_send,
                                 dev->bytes_sent);
  /* sending data on the port */
  if (success)
  {
    *bytes_send = *dev->bytes_sent;
    /* as long as the write is returning immediately,be ready to send
    *  additonnal data */
    dev->tx_in_progress = 0;
    /* calling the frame to indicate that the transmission is over and
    * we're ready to receive a new data */
    dev->last_status = dev->current_status;
    dev->current_status = comport_write_operation_finished;
    *bytes_send = *dev->bytes_sent;
  }
  else
  {
    dev->temp_bytes_sent = *bytes_send;
    *bytes_send = 0;
    dev->tx_in_progress = 1;       /* write is pending */
    dev->last_status = dev->current_status;
    dev->current_status = comport_write_operation_ongoing;
  }

  return ((uint8_t)(dev->tx_in_progress));
}

//------------------------------------------------------------------------------
static uint8_t  COMPORT_set_rts_E(uint8_t id, uint8_t new_rts)
{
  emulation_dev_t  *dev = comport_emu_dev_table.dev[id];

  if (!dev || dev->h_comm == 0)
    return 0;

  if (new_rts)
  {
    dev->rts = 0xFF;
  }
  else
  {
    dev->rts = 0;
  }
  return 0xFF;
}
//------------------------------------------------------------------------------
static uint8_t  COMPORT_set_dtr_E(uint8_t id, uint8_t new_dtr)
{
  emulation_dev_t  *dev = comport_emu_dev_table.dev[id];

  if (!dev || dev->h_comm == 0)
    return 0;

  if (new_dtr)
  {
    dev->dtr = 0xFF;
  }
  else
  {
    dev->dtr = 0;
  }
  return 0xFF;
}
//------------------------------------------------------------------------------
static uint8_t  COMPORT_flush_rx_buffer_E(uint8_t id)
{
  emulation_dev_t  *dev = comport_emu_dev_table.dev[id];

  if (!dev || dev->h_comm == 0)
    return 0;

  dev->rx_in_progress = 0;
  dev->rx_buffer_size = IN_BUFFER_SIZE;

  return 0xFF;
}
//------------------------------------------------------------------------------
static uint8_t  COMPORT_flush_tx_buffer_E(uint8_t id)
{
  emulation_dev_t  *dev = comport_emu_dev_table.dev[id];

  if (!dev || dev->h_comm == 0)
    return 0;

  dev->tx_in_progress = 0;
  dev->tx_buffer_size = OUT_BUFFER_SIZE;
  return 0xFF;
}
//------------------------------------------------------------------------------
static comport_error_t  COMPORT_get_last_error_E(uint8_t id)
{
  emulation_dev_t  *dev = comport_emu_dev_table.dev[id];
  comport_error_t error = comport_error_unkown;

  if(!dev)
    return comport_error_access_denied;

  error = comport_error_unkown;

  return error;
}
//------------------------------------------------------------------------------
static comport_status_t COMPORT_get_status_E(uint8_t id)
{
  emulation_dev_t  *dev = comport_emu_dev_table.dev[id];

  if(!dev)
    return comport_status_no_event;
  return dev->current_status;
}
//------------------------------------------------------------------------------
static void COMPORT_process_communication_E(uint8_t id)
{
  emulation_dev_t  *dev = comport_emu_dev_table.dev[id];

  if (!dev || dev->h_comm == 0)
    return;

  if (dev->rx_in_progress)
  {

    /* no error => OK */
    dev->last_status = dev->current_status;
    dev->current_status = comport_read_operation_finished;
    /* Read operation completed successfully */
    dev->rx_in_progress = 0; /* read has ended */
    if (dev->port.on_buffer_arrival_manager != NULL)
    {
      dev->port.on_buffer_arrival_manager(dev->port.id, *dev->read_bytes);
    }
   }
   if (dev->tx_in_progress)
   { /* Write operation completed successfully */
     dev->last_status = dev->current_status;
     dev->current_status = comport_write_operation_finished;
     dev->tx_in_progress = 0; /* write has ended */
     *dev->bytes_sent = dev->temp_bytes_sent;
     if (dev->port.on_buffer_sent_manager!= NULL)
     {
       dev->port.on_buffer_sent_manager(dev->port.id, *dev->bytes_sent);
     }
   }
}
//------------------------------------------------------------------------------
/*static uint8_t disable_hw_flow_ctrl_E(uint8_t id)
{
  emulation_dev_t  *dev = comport_emu_dev_table.dev[id];

  if (!dev || dev->h_comm == 0)
    return 0;

  dev->hw_flow_control = comport_rts_control_disable;
  return 0xFF;

} */
//------------------------------------------------------------------------------

static comport_handler_t *COMPORT_open_E(uint8_t comm_channel_id, comport_cfg_t *comport_cfg,
                                         comm_error_report_t error_report)
{

  emulation_dev_t  *dev;
//  int res;
//  DWORD dwSize;
//  int i;

  if (comport_cfg && (0 < comport_cfg->id) && (comport_cfg->id <= MAX_COMPORT_INSTANCES))
  {
    dev = comport_emu_dev_table.dev[comport_cfg->id];
    if (dev != NULL)
    {
      /* Comport is already created
      Check if Channel id are different from already created file
      */
      if ( comm_channel_id != dev->comm_channel_id )
      {
      dev->last_error = comport_error_open_failed;
      dev->error_report(dev->comm_channel_id, dev->last_error ,__LINE__);

      dev->h_comm = 0;
      free(dev);
      comport_emu_dev_table.dev[comport_cfg->id] = NULL;
      return NULL;
      }
    }
    comport_emu_dev_table.dev[comport_cfg->id] = (emulation_dev_t *) malloc (sizeof(comport_dev_t));
    comport_emu_dev_table.instance_nr = comport_cfg->id;

    dev = comport_emu_dev_table.dev[comport_cfg->id];

    dev->port.id = comport_cfg->id;
    dev->h_comm= (intptr_t)dev;
    dev->comm_channel_id = comm_channel_id;

    memcpy((void*)&dev->comport_cfg, (void*)comport_cfg, sizeof(comport_cfg_t));
    dev->last_error = 0;
    dev->rx_in_progress = 0;
    dev->tx_in_progress = 0;
    dev->error_report = error_report;



    dev->rx_buffer_size = IN_BUFFER_SIZE;
    dev->tx_buffer_size = OUT_BUFFER_SIZE;
    dev->hw_flow_control = comport_rts_control_disable;
    dev->rts = 0;
    dev->dtr = 0;

    dev->port.close = COMPORT_close_port_E;
    dev->port.process_comm = COMPORT_process_communication_E;
    dev->port.rx = COMPORT_rx_E;
    dev->port.flush_rx_buffer = COMPORT_flush_rx_buffer_E;
    dev->port.flush_tx_buffer = COMPORT_flush_tx_buffer_E;
    dev->port.get_modem_status = COMPORT_get_modem_status_E;
    dev->port.get_last_error = COMPORT_get_last_error_E;
    dev->port.get_rx_used_buffer_size = COMPORT_get_rx_used_buffer_size_E;
    dev->port.get_status = COMPORT_get_status_E;
    dev->port.set_baud = COMPORT_set_baudrate_E;
    dev->port.set_buffer_size = COMPORT_set_buffer_size_E;
    dev->port.set_dtr = COMPORT_set_dtr_E;
    dev->port.set_rts = COMPORT_set_rts_E;
    dev->port.tx = COMPORT_tx_E;

    return (&dev->port);
  }
  else
  { /* Invalid id. Must be between 1 and MAX_INSTANCES+1 */
    return NULL;
  }
}
//------------------------------------------------------------------------------



#else // if __linux__


/******************************************************************************
* Function:... COMPORT_open_I(uint8_t comm_channel_id, comport_cfg_t *comport_cfg,
*                             comm_error_report_t error_report)
* Parameters:. I :
* Description:
*
* Created:.... 23.01.05 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
comport_handler_t *COMPORT_open_I(uint8_t comm_channel_id, comport_cfg_t *comport_cfg,
                                  comm_error_report_t error_report)
{
  if (!comport_cfg)
    return NULL;

  comport_dev_t  *comport_temp;
  int id = comport_cfg->id;

  /* Invalid id. Must be between 1 and MAX_COMPORT_INSTANCES */
  if (id < 1 || id > MAX_COMPORT_INSTANCES)
    return NULL;

  char *comport_name = (char*)comport_cfg->comport_name;

  //Debug("Opening %s...\n", comport_name);
  int handle = open(comport_name, O_RDWR | O_NOCTTY | O_NONBLOCK );
  if (handle==INVALID_HANDLE_VALUE)
    return NULL;

  Debug("Comport open: %s (HW: %d, USB: %d ,HSI: %d, Comm: %d ,fd:%d)\n",
      comport_name,
      GLOBAL_use_hw_ch_change, GLOBAL_usb_device,
      GLOBAL_hsi_link, GLOBAL_comm_technology[id],
      handle);

  /* Allocating the device */
  comport_temp = comport_dev_table.comport_dev[id] = (comport_dev_t *) malloc (sizeof(comport_dev_t));
  if (comport_temp == 0) {
    close(handle);
    return NULL;
  }

  comport_dev_table.instance_nr = id;
  memset((char*)comport_temp, '\0', sizeof(comport_dev_t));
  memcpy((char*)&comport_temp->comport_cfg, (const char*)comport_cfg, sizeof(comport_cfg_t));

  comport_temp->port.id = id;
  comport_temp->h_comm = handle;

  comport_temp->rx_buffer_size = IN_BUFFER_SIZE;
  comport_temp->tx_buffer_size = OUT_BUFFER_SIZE;

  comport_temp->error_report = error_report;
  comport_temp->comm_channel_id = comm_channel_id;

  comport_temp->port.close = COMPORT_close_port_I;
  comport_temp->port.process_comm = COMPORT_process_communication_I;
  comport_temp->port.rx = COMPORT_rx_I;
  comport_temp->port.flush_rx_buffer = COMPORT_flush_rx_buffer_I;
  comport_temp->port.flush_tx_buffer = COMPORT_flush_tx_buffer_I;
  comport_temp->port.get_modem_status = COMPORT_get_modem_status_I;
  comport_temp->port.get_last_error = COMPORT_get_last_error_I;
  comport_temp->port.get_rx_used_buffer_size = COMPORT_get_rx_used_buffer_size_I;
  comport_temp->port.get_status = COMPORT_get_status_I;
  comport_temp->port.set_baud = COMPORT_set_baudrate_I;
  comport_temp->port.set_buffer_size = COMPORT_set_buffer_size_I;
  comport_temp->port.set_dtr = COMPORT_set_dtr_I;
  comport_temp->port.set_rts = COMPORT_set_rts_I;
  comport_temp->port.tx = COMPORT_tx_I;

  /* Setting up the device */
  if(!GLOBAL_use_hw_ch_change || strstr(comport_name,"hsi"))
  {
    /* Configure baudrate, databits, stopbits, parity, RTS and DTR */
    Debug("Setting up %s (Tech: %d)\n", comport_name, GLOBAL_comm_technology[id]);
    if (!COMPORT_special_Linux_settings(handle, GLOBAL_comm_technology[id]))
      PrintError("COMPORT_special_Linux_settings failed\n");
  }

  if (!GLOBAL_hsi_link && !GLOBAL_usb_device && !GLOBAL_comm_technology[id])
  {
    int done = false;
    Debug("Setting up USIF %s\n", comport_name);

    do {
      if (!COMPORT_set_baudrate_I(id, comport_cfg->baudrate)) break;
      if (!Set_data_bits_I(id)) break;
      if (!Set_parity_I(id, comport_cfg->parity)) break;
      if (!Set_stop_bits_I(id, comport_cfg->stop_bits)) break;
      if (!disable_hw_flow_ctrl_I(id)) break;
      if (!COMPORT_set_rts_I(id, 0)) break;
      if (!COMPORT_set_dtr_I(id, 0)) break;
      done = true;
    } while (0);

    if (!done)
    {
      PrintError("Error> USIF setup failed %d: %s\n", errno, strerror(errno));
      free(comport_temp);
      close(handle);
      return 0;
    }
  }

  return ((comport_handler_t *)comport_temp);
}

/******************************************************************************
* Function:... uint8_t  COMPORT_close_port(uint8_t id)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 23.01.05 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static uint8_t  COMPORT_close_port_I(uint8_t id)
{
  comport_dev_t *comport_dev = comport_dev_table.comport_dev[id];

  if(comport_dev && comport_dev->h_comm != INVALID_HANDLE_VALUE)
  {
    if( close(comport_dev->h_comm) < 0 )
    {
#ifdef _WIN32
       comport_dev->last_error = errno;
       comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
#endif
       free(comport_dev);
       comport_dev_table.comport_dev[id] = NULL;
       return false;
    }
  }
  return true; /* If closing is OK return true. If the channel is not defined, assume closing like OK */
}

/******************************************************************************
* Function: COMPORT_ioctl_I(uint8_t id, unsigned int req, ...)
******************************************************************************/
int COMPORT_ioctl_I(uint8_t id, unsigned int req, ...)
{
  void* arg;
  va_list ap;
  va_start(ap, req);
  arg = va_arg(ap,void*);
  va_end(ap);

  comport_dev_t *comport_dev = comport_dev_table.comport_dev[id];

  if (!comport_dev || comport_dev->h_comm == INVALID_HANDLE_VALUE)
    return 0;

  return ioctl( comport_dev->h_comm, req, arg);
}

/******************************************************************************
* Function:... void COMPORT_process_communication(uint8_t id)
* Parameters: None
* Returns:.... Nothing
* Description:
* Created:.... 30.01.06 by (AlanBugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Name, DWD / Initials)
*
******************************************************************************/
void COMPORT_process_communication_I(uint8_t id)
{
  Debug3(" Begin %s()\n", __func__);

  comport_dev_t *comport_dev = comport_dev_table.comport_dev[id];
  if (comport_dev
      && !comport_dev->done
      && comport_dev->current_status == comport_read_operation_ongoing)
  {
    int rc;
    unsigned int fd = comport_dev->h_comm;
    fd_set readfs;
    struct timeval timeout;

    timeout.tv_usec = 200;
    timeout.tv_sec = 0;
    FD_ZERO(&readfs);
    FD_SET(fd, &readfs);
    rc = select((fd+1), &readfs, NULL, NULL, &timeout);

    if (rc > 0) /* byte incoming */
    {
      int len   = 0;
      uint8_t *buf   = comport_dev->buf;
      uint32_t pos   = comport_dev->pos;
      uint32_t total = comport_dev->len;
      // comport_fx_rx_t fx = comport_dev->port.on_buffer_arrival_manager;

      //fcntl(comport_dev->h_comm, F_SETFL, FNDELAY); //not needed anymore with select function()
      if( (len = read( comport_dev->h_comm, buf+pos, total-pos )) >= 0 )
      {
        comport_dev->pos += len;
        if (len)
          Debug2("  Read %d/%d bytes [ %s ]\n",
                 len+pos, total, getHex(buf, len+pos>25 ? 25 : len+pos));

        if ((pos+len) == total)
        {
          Debug("  Read %d bytes [ %s ]\n", total, getHex(buf, len+pos>25 ? 25 : len+pos));
          (*comport_dev->read_bytes) = total;
          comport_dev->last_status = comport_dev->current_status;
          comport_dev->current_status = comport_read_operation_finished;
          //if (fx)
          //  fx(id, total);
        }
      }
      else
      {
         if (errno != EAGAIN)
         {
           Debug("Read error %d: %s\n", errno, strerror(errno));

           comport_dev->last_status = comport_dev->current_status;
           comport_dev->current_status = comport_read_operation_finished_with_error;
           comport_dev->last_error = errno;
           comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
         }
      }
    }
    else if (rc == 0) /* timeout occurred */
    {
      //maxvo: nothing to do...try again later
      //Debug("read TIMEOUT!\n");
    }
    else /* (rc < 0) error */
    {
      if (errno == EINTR) return;
      PrintError("select Error (%d) %d: %s\n", rc, errno, strerror(errno));

      comport_dev->last_status = comport_dev->current_status;
      comport_dev->current_status = comport_read_operation_finished_with_error;
      comport_dev->last_error = errno;
      comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
    }
  }

  Debug3(" End %s()\n", __func__);
}



void COMPORT_rx_dump(uint8_t id)
{
  Debug("COMPORT> Dumping from id %d\n", id);

  comport_dev_t *comport_dev = comport_dev_table.comport_dev[id];

  int fd = comport_dev->h_comm;

//if (GLOBAL_HSI)
  //{
    // Read the EXT_MIPI_BUS_ENABLE_GET flag
    int reg = ioctl(fd, EXT_MIPI_IOC_GET_CONF, EXT_MIPI_BUS_ENABLE_GET);
    if (reg == -1) {
      Debug("Error> IOCTL failed during EXT_MIPI_BUS_ENABLE_GET (errno: %s)\n", strerror(errno));
    } else {
      Debug("EXT_MIPI_BUS_ENABLE_GET> 0x%X\n", reg);
    }

    // Read the EXT_MIPI_RX_ERROR_FLAGS_GET flag
    reg = ioctl(fd, EXT_MIPI_IOC_GET_CONF, EXT_MIPI_RX_ERROR_FLAGS_GET);
    if (reg == -1) {
      Debug("Error> IOCTL failed during EXT_MIPI_RX_ERROR_FLAGS_GET (errno: %s)\n", strerror(errno));
    } else {
      Debug("EXT_MIPI_RX_ERROR_FLAGS_GET> 0x%X\n", reg);
    }

    // Read the EXT_MIPI_TX_ERROR_FLAGS_GET flag
    reg = ioctl(fd, EXT_MIPI_IOC_GET_CONF, EXT_MIPI_TX_ERROR_FLAGS_GET);
    if (reg == -1) {
      Debug("Error> IOCTL failed during EXT_MIPI_TX_ERROR_FLAGS_GET (errno: %s)\n", strerror(errno));
    } else {
      Debug("EXT_MIPI_TX_ERROR_FLAGS_GET> 0x%X\n", reg);
    }
//}

  int len = 0;
  if (comport_dev->current_status == comport_read_operation_ongoing)
  {
    uint8_t *buf   = comport_dev->buf;
    uint32_t pos   = comport_dev->pos;
    uint32_t total = comport_dev->len;

    Debug("  RxDump> %d bytes [ %s ] %d/%d",
          pos, getHex(buf, pos), pos, total);
  }
  else
  {
    uint32_t bytes_ready = COMPORT_get_rx_used_buffer_size_I(id);
    if (bytes_ready)
    {
      comport_dev->current_status = comport_read_operation_ongoing;
      uint8_t *data = (uint8_t*)MEM_malloc( bytes_ready );
      if( (len = read( fd, data, bytes_ready )) >= 0 )
      {
        Debug("  RxDump> %d bytes [ %s ] %d/%d",
              len, getHex(data, len), len, bytes_ready);
      }
      MEM_free(data);
    } else {
      Debug("  RxDump> Buffer is empty.\n");
    }
  }
}


/******************************************************************************
* Function:... uint8_t COMPORT_rx(uint8_t  id, uint8_t *block,
uint32_t bytes_to_read, uint32_t *bytes_read, fx_t fx)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 23.01.05 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static uint8_t COMPORT_rx_I(uint8_t id, uint8_t **block, uint32_t bytes_to_read,
                            uint32_t *bytes_read, comport_fx_rx_t fx)
{
  comport_dev_t *comport_dev = comport_dev_table.comport_dev[id];
  if(!comport_dev)
  {
    return 0;
  }
  if (comport_dev->current_status == comport_read_operation_ongoing)
  {
    return 1;
  }
  comport_dev->current_status = comport_read_operation_ongoing;

  // Setting up the read operation
  comport_dev->port.on_buffer_arrival_manager = fx;
  comport_dev->read_bytes = bytes_read;
  comport_dev->pos = 0;
  comport_dev->len = bytes_to_read;
  comport_dev->buf = *block;

  Debug2("Setting Rx to read %d bytes...", bytes_to_read);
  //comport_dev->port.process_comm(id);

  return (comport_dev->current_status == comport_read_operation_ongoing);
}


/******************************************************************************
* Function:... uint8_t COMPORT_tx(uint8_t id, uint8_t *block,
*                                 uint32_t bytes_to_send, uint32_t *bytes_send, fx_t fx)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 25.01.05 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static uint8_t  COMPORT_tx_I(uint8_t id, uint8_t *block, uint32_t bytes_to_send,
                             uint32_t *bytes_send, comport_fx_tx_t fx)
{
  Debug3(" Begin %s()", __func__);

  comport_dev_t *comport_dev = comport_dev_table.comport_dev[id];
  uint32_t total=0;
  int nbytes=0;
  unsigned int fd, rc;
//  int maxfd;
  fd_set writefs;
  struct timeval timeout;
  uint8_t tx_in_progress = true;

  if(!comport_dev)
    return 0;
  fd = comport_dev->h_comm;
  comport_dev->port.on_buffer_sent_manager = fx;
  comport_dev->write_bytes += bytes_to_send;

  timeout.tv_usec = 2000;
  timeout.tv_sec  = 0;
  FD_ZERO(&writefs);

  #if 0 //def __APPLE__
    // Make the mark in the IFX driver
    int mics = bytes_to_send;
    if (-1 == ioctl(comport_dev->h_comm, IOSSDATALAT_32, &mics))
      fprintf(stderr, "ioctl2(IOSSDATALAT) returned error - %s(%d)\n", strerror(errno), errno);
  #endif

  /* sending data on the port */
  while (tx_in_progress)
  {
    // TODO: Create an I/O thread actually making use of select
    #ifdef ANDROID
      rc = 1;
    #else
      FD_SET(fd, &writefs);
      rc = select((fd+1), NULL, &writefs, NULL, &timeout);
    #endif
    if(rc > 0)
    {
      nbytes = write( fd, (block+total), (bytes_to_send-total) );
      if(nbytes < 0)
      {
        if (errno == EAGAIN) {
          Sleep(100);
          continue;
        }
        comport_dev->tx_in_progress = comport_write_operation_finished_with_error;
        Debug("Error writing %d: %s", errno, strerror(errno));
        comport_dev->last_error = errno;
        tx_in_progress = false;
        break; /* exit write with error*/
      }

      total += nbytes;

      if (nbytes == 0 && errno)
        Debug("Nothing was sent. (errno: %s)\n", strerror(errno));
      else
        Debug2("Wrote %d/%d bytes [ %s ]\n",
            total, bytes_to_send, getHex(block, total>25 ? 25 : total));

      /* write complete */
      if( total == bytes_to_send )
      {
        Debug("Wrote %d/%d bytes [ %s ]\n",
            total, bytes_to_send, getHex(block, total>25 ? 25 : total));

        tx_in_progress = false;
        *bytes_send = bytes_to_send;
        comport_dev->tx_in_progress = 0;
        comport_dev->last_status = comport_dev->current_status;
        comport_dev->current_status = comport_write_operation_finished;
      }
    }
    else if (rc == 0) /* timeout occurred */
    {
      //try again...
    }
    else /* (rc < 0) */
    {
      Debug("Select error (errno: %s)\n", strerror(errno));
      comport_dev->tx_in_progress = comport_write_operation_finished_with_error;
      tx_in_progress = false;
    }
  }

  Debug3(" End %s()\n", __func__);
  return 0;
}


/******************************************************************************
* Function:... uint8_t COMPORT_set_rts(uint8_t id, uint8_t new_rts)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 26.01.05 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static uint8_t COMPORT_set_rts_I(uint8_t id, uint8_t new_rts)
{
  int mdmbits;
  comport_dev_t *comport_dev = comport_dev_table.comport_dev[id];

  if (!comport_dev || comport_dev->h_comm == INVALID_HANDLE_VALUE)
    return 0;

  if (GLOBAL_hsi_link || GLOBAL_usb_device || GLOBAL_use_GenericUSB)
    return 0xFF;

  ioctl( comport_dev->h_comm, TIOCMGET, &mdmbits);
  if (new_rts)
  {
     mdmbits |= TIOCM_RTS;
  }
  else
  {
     mdmbits &= ~TIOCM_RTS;
  }
  if( ioctl( comport_dev->h_comm, TIOCMSET, &mdmbits ) < 0)
    return 0;
  else
    return 0xFF;
}

/******************************************************************************
* Function:... uint8_t COMPORT_set_dtr(uint8_t id, char new_dtr)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 24.11.01 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static uint8_t COMPORT_set_dtr_I(uint8_t id, uint8_t new_dtr)
{
  int mdmbits;
  comport_dev_t *comport_dev = comport_dev_table.comport_dev[id];

  if (!comport_dev || comport_dev->h_comm == INVALID_HANDLE_VALUE)
    return 0;

  if (GLOBAL_hsi_link || GLOBAL_usb_device || GLOBAL_use_GenericUSB)
    return 0xFF;

  ioctl( comport_dev->h_comm, TIOCMGET, &mdmbits );
  if (new_dtr)
  {
     mdmbits |= TIOCM_DTR;
  }
  else
  {
     mdmbits &= ~TIOCM_DTR;
  }
  if( ioctl( comport_dev->h_comm, TIOCMSET, &mdmbits ) < 0 )
    return 0;
  else
    return 0xFF;
}

/******************************************************************************
* Function:... COMPORT_special_Linux_settings(uint8_t id)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 28.05.10
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static uint8_t COMPORT_special_Linux_settings( uint8_t handle, uint8_t technology )
{
  struct termios config;
  int res;

  /* HSI port: set the transfer size. */
  if (technology)
  {
    uint8_t done = false;
    do {
      printf("Setting MIPI config...\n");
      if (ioctl(handle, EXT_MIPI_IOC_SET_CONF, EXT_MIPI_DEEP_CLEAN) == -1) break;

      Debug("Setting Tx rate to 192 Mbps (0x%X).\n", EXT_MIPI_TX_DATA_RATE_SET(0));
      if (ioctl(handle, EXT_MIPI_IOC_SET_CONF, EXT_MIPI_TX_DATA_RATE_SET(0)) == -1) break;

      Debug("Setting I/O trans mode to 0x%X,0x%X.\n", EXT_MIPI_RX_TRANS_MODE_FRAMED_SET, EXT_MIPI_TX_TRANS_MODE_FRAMED_SET);
      if (ioctl(handle, EXT_MIPI_IOC_SET_CONF, EXT_MIPI_RX_TRANS_MODE_FRAMED_SET) == -1) break;
      if (ioctl(handle, EXT_MIPI_IOC_SET_CONF, EXT_MIPI_TX_TRANS_MODE_FRAMED_SET) == -1) break;

      int n_channel_bits = 0;
      Debug("Setting I/O channel bits to %d.\n", n_channel_bits);
      if (ioctl(handle, EXT_MIPI_IOC_SET_CONF, EXT_MIPI_RX_CH_FIELD_W_SET(n_channel_bits)) == -1) break;
      if (ioctl(handle, EXT_MIPI_IOC_SET_CONF, EXT_MIPI_TX_CH_FIELD_W_SET(n_channel_bits)) == -1) break;

      Debug("Bit Rate = %3.2fMbps\n", 192/((float)1));
      if (ioctl(handle, EXT_MIPI_IOC_GET_CONF, EXT_MIPI_RX_DATA_RATE_GET) == -1) break;
      if (ioctl(handle, EXT_MIPI_IOC_GET_CONF, EXT_MIPI_TX_DATA_RATE_GET) == -1) break;
      done = true;
    } while (0);

    if (!done)
    {
      Debug("Error> IOCTL failed: %s\n", GetErrorString(true));
      return 0;
    }

    return 0xFF;
  }


  if (GLOBAL_hsi_link)
  {
    Debug("Setting up HSI driver...\n");

    int err = tcgetattr(handle, &config);
    if (-1 != err) {
      cfmakeraw(&config);
      tcsetattr(handle, TCSANOW, &config);
    }

    if (fcntl(handle, F_SETFL, O_NONBLOCK) < 0)
    {
      Debug("Error> fcntl failed: %s\n", GetErrorString(true));
      return 0;
    }

    memset((void*)&config, 0, sizeof(config));
    config.c_cflag = B115200;

    config.c_cflag |= CS8 | CLOCAL | CREAD;
    config.c_iflag &= ~(INPCK | IGNPAR | PARMRK | ISTRIP | IXANY | ICRNL);
    config.c_oflag &= ~OPOST;
    config.c_cc[VMIN] = 1;
    config.c_cc[VTIME] = 10;

    tcflush(handle, TCIFLUSH);
    cfsetispeed(&config, 115200);
    cfsetospeed(&config, 115200);

    if (!tcsetattr(handle, TCSANOW, &config)) {
      PrintError("tcsetattr failed: %s\n", GetErrorString(true));
      return 0;
    }

    res = tcgetattr( handle, &config );
    if (res == -1)
      Debug(" ---> HSI Driver setup, but tcsetattr still fails: %s\n", GetErrorString(true));

    return 0xFF;
  }

  res = tcgetattr( handle, &config );
  if (res < 0)
  {
    Debug("tcgetattr failed: %s\n", GetErrorString(true));
    close( handle );
    return 0;
  }

  config.c_cflag |= (CLOCAL | CREAD | CS8 | HUPCL);
  config.c_cflag &= ~(PARENB | CSIZE | CSTOPB);

  config.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

  config.c_iflag &= ~(INLCR | ICRNL | IGNCR | IXON | IXOFF);
  config.c_iflag |= IGNPAR;

  // INFO: Removed 'OLCUC'
  config.c_oflag &= ~(OPOST | ONLRET | ONOCR | OCRNL | ONLCR);

  res = tcsetattr( handle, TCSANOW, &config );
  if (res < 0)
  {
    Debug("tcsetattr failed\n");
    close( handle );
    //comport_dev->last_reported_error = comport_dev->last_error;
    //comport_dev->last_error = errno;
    //comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
    return 0;
  }
  return 0xFF;
}

#if 0
/* OS X*/
int cfsetispeed_osx ( struct termios *termios_p, speed_t speed)
{
    if ((speed & ~CBAUD) != 0 && (speed < B57600 || speed > B460800))
    {
      __set_errno(EINVAL);
      return -1;
    }

    if (speed == 0)
      termios_p->c_iflag |= IBAUD0;
    else
    {
      termios_p->c_iflag &= ~IBAUD0;
      termios_p->c_cflag &= ~(CBAUD | CBAUDEX);
      termios_p->c_cflag |= speed;
    }

    return 0;
}
#endif

/******************************************************************************
* Function:... COMPORT_set_baudrate(uint8_t id, uint32_t Baud)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 24.11.01 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static uint8_t COMPORT_set_baudrate_I(uint8_t id, uint32_t baudrate)
{
  int speed, res;
  struct termios config;
  comport_dev_t *comport_dev = comport_dev_table.comport_dev[id];

  if(!comport_dev)
    return 0;
  if (comport_dev->h_comm == INVALID_HANDLE_VALUE)
  {
    comport_dev->last_error = comport_error_invalid_handle;
    return 0;
  }

  /**
   * TODO: Try out this code, if more baudrates are requested (Manually setting speeds)
   *
   * struct serial_struct ss;
   * ioctl(comport_dev->h_comm, TIOCGSERIAL, &ss);
   * ss.flags = (ss.flags & ~ASYNC_SPD_MASK) | ASYNC_SPD_CUST;
   * ss.custom_divisor = (ss.baud_base + (baudrate/2)) / baudrate;
   * uint32_t closest_baudrate = ss.baud_base / ss.custom_divisor;
   * if (closest_baudrate < baudrate *98/100 || closest_baudrate > baudrate *102/100)
   *  Debug("Cannot set serial port speed to %d. Closest is %d\n", baudrate, closest_baudrate);
   * ioctl(comport_dev->h_comm, TIOCSSERIAL, &ss);
   */
  if (GLOBAL_usb_device || GLOBAL_comm_technology[id])
  {
   Debug("Ignoring COMPORT_set_baudrate (id=%d, GLOBAL_usb_device=%d, GLOBAL_comm_technology=%d)\n", id, GLOBAL_usb_device, GLOBAL_comm_technology[id]);
   return 0xFF;
  }

  Debug("COMPORT_set_baudrate(%d)\n", baudrate);

  res = tcgetattr( comport_dev->h_comm, &config );
  if (res < 0)
  {
    Debug("tcgetattr failed: %s\n", strerror(errno));
    comport_dev->last_error = errno;//GetLastError();
    comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
    return 0;
  }

  switch (baudrate)
  {
  case 1200:    speed = B1200;    break;
  case 1800:    speed = B1800;    break;
  case 2400:    speed = B2400;    break;
  case 4800:    speed = B4800;    break;
  case 9600:    speed = B9600;    break;
  case 19200:   speed = B19200;   break;
  case 38400:   speed = B38400;   break;
  case 57600:   speed = B57600;   break;
  case 230400:  speed = B230400;  break;
  case 460800:  speed = B460800;  break;
  case 921600:  speed = B921600;  break;
#ifdef __linux__
  case 500000:  speed = B500000;  break;
  case 576000:  speed = B576000;  break;
  case 1000000: speed = B1000000; break;
  case 1152000: speed = B1152000; break;
  case 1500000: speed = B1500000; break;
  case 2500000: speed = B2500000; break;
  case 3000000: speed = B3000000; break;
  case 4000000: speed = B4000000; break;
#endif
  case 115200:  speed = B115200;  break;
  default:
    speed = B115200;
    PrintError("Baud rate %d not supported... Falling back to default baud rate (115200)\n", baudrate);
  }

  cfsetispeed(&config, speed);
  cfsetospeed(&config, speed);

  res = tcsetattr( comport_dev->h_comm, TCSANOW, &config );
  if (res < 0)
  {
    Debug("tcsetattr failed: %s\n", strerror(errno));
    comport_dev->last_error = errno;//GetLastError();
    comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
    return 0;
  }
  comport_dev->comport_cfg.baudrate = baudrate;
  return 0xFF;
}

/******************************************************************************
* Function:... uint8_t Set_Stop_bits(uint8_t id, stop_bits_t     stop_bits)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 26.01.05 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static uint8_t Set_stop_bits_I(uint8_t id, comport_stop_bits_t StopBits)
{
  comport_dev_t *comport_dev = comport_dev_table.comport_dev[id];
  struct termios config;
  int res;

  if (!comport_dev || comport_dev->h_comm == INVALID_HANDLE_VALUE)
    return 0;
  /* Get the propierties of the comport before its initialisation */
  tcgetattr( comport_dev->h_comm, &config );

  switch (StopBits)
  {
    case comport_one_stop_bit:
      config.c_cflag &= ~CSTOPB; /* Linux one stop bit */
      break;

    case comport_one_5_stop_bit:
      return 0; /* not supported by Linux */

    case comport_two_stop_bits:
      config.c_cflag |= CSTOPB;
      break;

    default:
      config.c_cflag &= ~CSTOPB; /* Linux one stop bit */
      break;
  }
  res = tcsetattr( comport_dev->h_comm, TCSANOW, &config );
  if (res < 0)
  {
    comport_dev->last_error = errno;
    comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
    return 0;
  }
  return 0xFF;
}

/******************************************************************************
* Function:... char Set_data_bits(uint8_t id)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 26.01.05 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static uint8_t Set_data_bits_I(uint8_t id)
{
  comport_dev_t *comport_dev = comport_dev_table.comport_dev[id];
  struct termios config;
  int res;

  if (!comport_dev || comport_dev->h_comm == INVALID_HANDLE_VALUE)
    return 0;
  tcgetattr( comport_dev->h_comm, &config );

  config.c_cflag |= CS8;
  res = tcsetattr( comport_dev->h_comm, TCSANOW, &config );
  if (res < 0)
  {
    Debug("tcsetattr failed: %s\n", strerror(errno));
    comport_dev->last_error = errno;
    comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
    return 0;
  }
  return 0xFF;
}

/******************************************************************************
* Function:... char SetParity(uint8_t id, comport_parity_t parity)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 24.11.01 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static uint8_t Set_parity_I(uint8_t id, comport_parity_t parity)
{
  comport_dev_t *comport_dev = comport_dev_table.comport_dev[id];
  struct termios config;
  int res;

  if (!comport_dev || comport_dev->h_comm == INVALID_HANDLE_VALUE)
    return 0;
  tcgetattr( comport_dev->h_comm, &config );

  /* Set parity bit */
  switch (parity)
  {
    case comport_no_parity:
      config.c_cflag &= ~PARENB;
      break;

    case comport_odd_parity:
      config.c_cflag |= PARENB;
      config.c_cflag |= PARODD;
      break;

    case comport_even_parity:
      config.c_cflag |= PARENB;
      config.c_cflag &= ~PARODD;
      break;

    case comport_mark_parity: /* Not allowed */
      return 0;

    case comport_space_parity:  /* Not allowed */
      return 0;

    default:
       config.c_cflag &= ~PARENB;
       return 0;
  }
  res = tcsetattr( comport_dev->h_comm, TCSANOW, &config );
  if (res < 0)
  {
    Debug("tcsetattr failed: %s\n", strerror(errno));
    comport_dev->last_error = errno;
    comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
    return 0;
  }
  return 0xFF;
}

/******************************************************************************
* Function:... uint8_t  COMPORT_set_buffer_size(uint8_t id, uint32_t rx_buf_size,
*                                               uint32_t tx_buf_size)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 24.11.01 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static uint8_t COMPORT_set_buffer_size_I(uint8_t id, uint32_t rx_buf_size, uint32_t tx_buf_size)
{
  /* In Linux serial buffer size cannot be altered, skip with an OK dummy response */
  return 0xFF;
}

/******************************************************************************
* Function:... uint32_t COMPORT_get_rx_used_buffer_size(uint8_t id)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 25.01.05 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static uint32_t COMPORT_get_rx_used_buffer_size_I(uint8_t id)
{
  comport_dev_t *comport_dev = comport_dev_table.comport_dev[id];
  int bytes_available = 0;

  if( ioctl( comport_dev->h_comm, FIONREAD, &bytes_available ) < 0 )
  {
    Debug("Error> IOCTL failed during FIONREAD (errno: %s)\n", strerror(errno));
    return 0;
  }
  else
  {
    //Debug("Bytes available: %d", bytes_available);
    return (uint32_t)bytes_available;
  }
}

/******************************************************************************
* Function:... uint8_t  COMPORT_flush_rx_buffer(uint8_t id)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 30.01.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static uint8_t  COMPORT_flush_rx_buffer_I(uint8_t id)
{
  comport_dev_t *comport_dev = comport_dev_table.comport_dev[id];

  if(!comport_dev)
    return 0;

  if (GLOBAL_usb_device || GLOBAL_comm_technology[id])
    return 0xFF;

  if( tcflush(comport_dev->h_comm, TCIFLUSH) < 0 )
  {
    Debug("tcflush failed: %s\n", strerror(errno));
    comport_dev->last_error = errno;
    comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
    return 0;
  }
  return 0xFF;
}

/******************************************************************************
* Function:... uint8_t  COMPORT_flush_tx_buffer(uint8_t id)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 30.01.06 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static uint8_t  COMPORT_flush_tx_buffer_I(uint8_t id)
{
  comport_dev_t *comport_dev = comport_dev_table.comport_dev[id];

  if(!comport_dev)
    return 0;

  if (GLOBAL_usb_device || GLOBAL_comm_technology[id])
    return 0xFF;

  if( tcflush(comport_dev->h_comm, TCOFLUSH) < 0 )
  {
    Debug("tcflush failed: %s\n", strerror(errno));
    comport_dev->last_error = errno;
    comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
    return 0;
  }
  return 0xFF;
}

/******************************************************************************
* Function:... uint8_t disable_hw_flow_ctrl(uint8_t id)
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 24.01.05 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static uint8_t disable_hw_flow_ctrl_I(uint8_t id)
{
  comport_dev_t *comport_dev = comport_dev_table.comport_dev[id];
  struct termios config;
  int res;

  if (!comport_dev || comport_dev->h_comm == INVALID_HANDLE_VALUE)
    return 0;
  /* Get the propierties of the comport before its initialisation */
  tcgetattr( comport_dev->h_comm, &config );
  config.c_cflag &= ~CRTSCTS;
  res = tcsetattr( comport_dev->h_comm, TCSANOW, &config );
  if (res < 0)
  {
    Debug("tcsetattr failed: %s\n", strerror(errno));
    comport_dev->last_error = errno;
    comport_dev->error_report(comport_dev->comm_channel_id, comport_dev->last_error ,__LINE__);
    return 0;
  }
  return 0xFF;
}

/******************************************************************************
* Function:... uint8_t  COMPORT_get_modem_status(uint8_t id,
comm_modem_status_line_t *modem_status_line);
* Parameters:. I :
* Returns:....
* Description:
*
* Created:.... 05.09.04 by (Alan Bugtrup, DWD / AB)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
static uint8_t  COMPORT_get_modem_status_I(uint8_t id,
                                    comport_modem_status_line_t *modem_status_line)
{
  comport_modem_status_line_t temp_status;
  int modemLineStatus;
  comport_dev_t *comport_dev = comport_dev_table.comport_dev[id];

  if(!comport_dev)
    return 0;
  temp_status.comport_ms_cts_on = 0;
  temp_status.comport_ms_dsr_on = 0;
  temp_status.comport_ms_dtr_on = 0;
  temp_status.comport_ms_ring_on = 0;
  temp_status.comport_ms_rlsd_on = 0;
  temp_status.comport_ms_rts_on = 0;

  ioctl( comport_dev->h_comm, TIOCMGET, &modemLineStatus );

  if( (modemLineStatus & TIOCM_CTS) == TIOCM_CTS )
    temp_status.comport_ms_cts_on = 0xFF;
  if( (modemLineStatus & TIOCM_LE) == TIOCM_LE ) /* DSR */
    temp_status.comport_ms_dsr_on = 0xFF;
  if( (modemLineStatus & TIOCM_RNG) == TIOCM_RNG )
    temp_status.comport_ms_ring_on = 0xFF;
  if( (modemLineStatus & TIOCM_CAR) == TIOCM_CAR ) /* DCD */
    temp_status.comport_ms_rlsd_on = 0xFF; /* DCD */
  if( (modemLineStatus & TIOCM_RTS) == TIOCM_RTS )
      temp_status.comport_ms_rts_on = 0xFF;
  if( (modemLineStatus & TIOCM_DTR) == TIOCM_DTR )
      temp_status.comport_ms_dtr_on = 0xFF;
  *modem_status_line = temp_status;
  return 0xFF;
}

/******************************************************************************
* Function:... comport_error_t  COMPORT_get_last_error(uint8_t id)
* Parameters:.
* Returns:....
* Description:
* Created:.... 30.01.06 by (AlanBugtrup, DWD / AB)
* Modified:...
*
******************************************************************************/
static comport_status_t COMPORT_get_status_I(uint8_t id)
{
  comport_dev_t *comport_dev = comport_dev_table.comport_dev[id];
  if(!comport_dev)
    return comport_status_no_event;
  return comport_dev->current_status;
}

/******************************************************************************
* Function:... comport_error_t  COMPORT_get_last_error(uint8_t id)
* Parameters:.
* Returns:....
* Description:
* Created:.... 30.01.06 by (AlanBugtrup, DWD / AB)
* Modified:...
*
******************************************************************************/
static comport_error_t  COMPORT_get_last_error_I(uint8_t id)
{
  comport_dev_t *comport_dev = comport_dev_table.comport_dev[id];
  comport_error_t error;

  if(!comport_dev)
    return comport_error_access_denied;

  switch(comport_dev->last_error)
  {
      /* Linux error causes are ISO standard i.e. there is a reduced set since they are not specific like the Windows com port ones */

     case EBADF:
       error = comport_error_invalid_handle;
       break;

     case EBUSY:
       error = comport_error_busy;
       break;

     default:
        error = comport_error_unkown;
         break;
  }

  return error;
}

/******************************************************************************
* Function:... void COMPORT_close_event_handler(comport_dev_t *comport_dev_temp)
* Parameters:.
* Returns:....
* Description:
* Created:.... 30.01.06 by (AlanBugtrup, DWD / AB)
* Modified:...
*
******************************************************************************/
/*
static void COMPORT_close_event_handler_I(comport_dev_t *comport_dev_temp)
{
   // Linux code is implemented with NON overlapped I/O i.e. no serial events
}
*/

/*******************************************************************************
  Function to set the General Purpose Pin of the Prolific PL2303 usb-to-serial chip
*******************************************************************************/
#define USB2SER_IOCTL_INDEX  0x0800

// For GP0
#define GP0_OUTPUT_ENABLE  CTL_CODE(FILE_DEVICE_UNKNOWN,\
                 USB2SER_IOCTL_INDEX+20,\
                 METHOD_BUFFERED, \
                 FILE_ANY_ACCESS)

#define GP0_SET_VALUE    CTL_CODE(FILE_DEVICE_UNKNOWN,\
                 USB2SER_IOCTL_INDEX+22,\
                 METHOD_BUFFERED,\
                 FILE_ANY_ACCESS)

uint8_t COMPORT_set_gp0(uint8_t channel, uint8_t value)
{
  /* N/A in Linux */
  return false;
}
/*****************************************************************************
   EMULATION FUNCTIONS:
*****************************************************************************/

#ifdef EMULATION_FUNCTIONS

static uint8_t COMPORT_set_baudrate_E(uint8_t id, uint32_t baudrate)
{
  emulation_dev_t  *dev = comport_emu_dev_table.dev[id];

  if (!dev || dev->h_comm == 0)
   return 0;

  dev->baudrate = baudrate;

  return 0xFF;
}
//------------------------------------------------------------------------------
static uint8_t  COMPORT_get_modem_status_E(uint8_t id, comport_modem_status_line_t *modem_status_line)
{
//  DWORD modemLineStatus;
  comport_modem_status_line_t temp_status;
//  int res;
  emulation_dev_t  *dev = comport_emu_dev_table.dev[id];

  if (!dev || dev->h_comm == 0)
   return 0;

  temp_status.comport_ms_cts_on = 0;
  temp_status.comport_ms_dsr_on = 0;
  temp_status.comport_ms_dtr_on = 0;
  temp_status.comport_ms_ring_on = 0;
  temp_status.comport_ms_rlsd_on = 0;
  temp_status.comport_ms_rts_on = 0;

  if (dev->rts == 0xFF )
  {
    temp_status.comport_ms_rts_on = 0xFF;
  }
  if (dev->dtr == 0xFF)
  {
    temp_status.comport_ms_dtr_on = 0xFF;
  }

  *modem_status_line = temp_status;
  return 0xFF;
}
//------------------------------------------------------------------------------
static uint8_t  COMPORT_close_port_E(uint8_t id)
{
  emulation_dev_t  *dev = comport_emu_dev_table.dev[id];

  if (!dev || dev->h_comm == 0)
    return 0;
  dev->h_comm = 0;

  return 0xFF;
}
//------------------------------------------------------------------------------
static uint8_t  COMPORT_set_buffer_size_E(uint8_t id, uint32_t rx_buf_size, uint32_t tx_buf_size)
{
  emulation_dev_t  *dev = comport_emu_dev_table.dev[id];

  if (!dev || dev->h_comm == 0)
    return 0;

  if (rx_buf_size < IN_BUFFER_SIZE)
  {
    dev->rx_buffer_size = rx_buf_size;
  }
  else
  {
    dev->rx_buffer_size = IN_BUFFER_SIZE;
  }

  if (tx_buf_size < OUT_BUFFER_SIZE)
  {
    dev->tx_buffer_size = tx_buf_size;
  }
  else
  {
    dev->tx_buffer_size = OUT_BUFFER_SIZE;
  }

  return 0xFF;
}
//------------------------------------------------------------------------------
#ifdef UNUSED_FUNCTION
static uint32_t COMPORT_get_rx_buffer_size_E(uint8_t id)
{
  emulation_dev_t  *dev = comport_emu_dev_table.dev[id];

  if (!dev || dev->h_comm == 0)
    return 0;

  return dev->rx_buffer_size;
}
#endif
//------------------------------------------------------------------------------
static uint32_t COMPORT_get_rx_used_buffer_size_E(uint8_t id)
{
  emulation_dev_t  *dev = comport_emu_dev_table.dev[id];

  if (!dev || dev->h_comm == 0)
    return 0;

//  return IN_BUFFER_SIZE;
  return GLOBAL_EmulateRxAvail(id);
}
//------------------------------------------------------------------------------
#ifdef UNUSED_FUNCTION
static uint32_t COMPORT_get_rx_free_buffer_size_E(uint8_t comm_handler)
{
  emulation_dev_t  *dev = comport_emu_dev_table.dev[comm_handler];

  if (!dev || dev->h_comm == 0)
    return 0;

//  return dev->rx_buffer_size;
return IN_BUFFER_SIZE - GLOBAL_EmulateRxAvail(comm_handler);
}
#endif

//------------------------------------------------------------------------------
static uint8_t COMPORT_rx_E(uint8_t id, uint8_t **block, uint32_t bytes_to_read, uint32_t *bytes_read, comport_fx_rx_t fx)
{
  emulation_dev_t  *dev = comport_emu_dev_table.dev[id];
  char             success;

  if (!dev || dev->h_comm == 0)
    return 0;

  if (dev->rx_in_progress)
  {
    return ((uint8_t)dev->rx_in_progress);
  }

  dev->port.on_buffer_arrival_manager = fx;
  dev->read_bytes = bytes_read;
  success = (char) EMU_ReadFile(id, *block, bytes_to_read, dev->read_bytes);
  if (success)
  {
    /* immediate return => data processing */
    dev->rx_in_progress = 0;
    dev->last_status = dev->current_status;
    dev->current_status = comport_read_operation_finished;
    if (dev->port.on_buffer_arrival_manager != NULL)
      dev->port.on_buffer_arrival_manager(dev->port.id, *dev->read_bytes);
  }
  else
  {
    dev->rx_in_progress = 1; /*wait for completion read not ended */
    dev->last_status = dev->current_status;
    dev->current_status = comport_read_operation_ongoing;
  }
  return ((uint8_t)dev->rx_in_progress);
}
//------------------------------------------------------------------------------

#ifdef UNUSED_FUNCTION
static uint32_t COMPORT_get_tx_buffer_size_E(uint8_t id)
{
  emulation_dev_t  *dev = comport_emu_dev_table.dev[id];

  if (!dev || dev->h_comm == 0)
    return 0;

  return dev->tx_buffer_size;
}
//------------------------------------------------------------------------------
static uint32_t COMPORT_get_tx_used_buffer_size_E(uint8_t id)
{
  emulation_dev_t  *dev = comport_emu_dev_table.dev[id];

  if (!dev || dev->h_comm == 0)
    return 0;

  return 0;
}
//------------------------------------------------------------------------------
static uint32_t COMPORT_get_tx_free_buffer_size_E(uint8_t id)
{
  emulation_dev_t  *dev = comport_emu_dev_table.dev[id];

  if (!dev || dev->h_comm == 0)
    return 0;

  return dev->tx_buffer_size;
}
#endif

//------------------------------------------------------------------------------
static uint8_t  COMPORT_tx_E(uint8_t id, uint8_t *block, uint32_t bytes_to_send,
                             uint32_t *bytes_send, comport_fx_tx_t fx)
{
  char          success;
//  unsigned long temp;
  emulation_dev_t  *dev = comport_emu_dev_table.dev[id];

  if (!dev || dev->h_comm == 0)
    return 0;
  dev->port.on_buffer_sent_manager = fx;

  dev->bytes_sent = bytes_send;
  dev->temp_bytes_sent = 0;

  success = (char) EMU_WriteFile(id, block, (unsigned long)bytes_to_send,
                                 dev->bytes_sent);
  /* sending data on the port */
  if (success)
  {
    *bytes_send = *dev->bytes_sent;
    /* as long as the write is returning immediately,be ready to send
    *  additonnal data */
    dev->tx_in_progress = 0;
    /* calling the frame to indicate that the transmission is over and
    * we're ready to receive a new data */
    dev->last_status = dev->current_status;
    dev->current_status = comport_write_operation_finished;
    *bytes_send = *dev->bytes_sent;
  }
  else
  {
    dev->temp_bytes_sent = *bytes_send;
    *bytes_send = 0;
    dev->tx_in_progress = 1;       /* write is pending */
    dev->last_status = dev->current_status;
    dev->current_status = comport_write_operation_ongoing;
  }

  return ((uint8_t)(dev->tx_in_progress));
}

//------------------------------------------------------------------------------
#ifdef UNUSED_FUNCTION
static uint8_t  COMPORT_set_flowctrl_E(uint8_t comm_handler, comport_rts_control_t rts_control)
{
  emulation_dev_t  *dev = comport_emu_dev_table.dev[comm_handler];

  if (!dev || dev->h_comm == 0)
    return 0;

  dev->hw_flow_control = rts_control;
  return 0xFF;

}
//------------------------------------------------------------------------------
static uint8_t  COMPORT_get_flowctrl_E(uint8_t id, comport_rts_control_t *rts_control)
{
  emulation_dev_t  *dev = comport_emu_dev_table.dev[id];

  if (!dev || dev->h_comm == 0)
    return 0;

  *rts_control = dev->hw_flow_control;
  return 0xFF;
}
#endif
//------------------------------------------------------------------------------
static uint8_t  COMPORT_set_rts_E(uint8_t id, uint8_t new_rts)
{
  emulation_dev_t  *dev = comport_emu_dev_table.dev[id];

  if (!dev || dev->h_comm == 0)
    return 0;

  if (new_rts)
  {
    dev->rts = 0xFF;
  }
  else
  {
    dev->rts = 0;
  }
  return 0xFF;
}
//------------------------------------------------------------------------------
static uint8_t  COMPORT_set_dtr_E(uint8_t id, uint8_t new_dtr)
{
  emulation_dev_t  *dev = comport_emu_dev_table.dev[id];

  if (!dev || dev->h_comm == 0)
    return 0;

  if (new_dtr)
  {
    dev->dtr = 0xFF;
  }
  else
  {
    dev->dtr = 0;
  }
  return 0xFF;
}
//------------------------------------------------------------------------------
static uint8_t  COMPORT_flush_rx_buffer_E(uint8_t id)
{
  emulation_dev_t  *dev = comport_emu_dev_table.dev[id];

  if (!dev || dev->h_comm == 0)
    return 0;

  dev->rx_in_progress = 0;
  dev->rx_buffer_size = IN_BUFFER_SIZE;

  return 0xFF;
}
//------------------------------------------------------------------------------
static uint8_t  COMPORT_flush_tx_buffer_E(uint8_t id)
{
  emulation_dev_t  *dev = comport_emu_dev_table.dev[id];

  if (!dev || dev->h_comm == 0)
    return 0;

  dev->tx_in_progress = 0;
  dev->tx_buffer_size = OUT_BUFFER_SIZE;
  return 0xFF;
}
//------------------------------------------------------------------------------
static comport_error_t  COMPORT_get_last_error_E(uint8_t id)
{
  emulation_dev_t  *dev = comport_emu_dev_table.dev[id];
  comport_error_t error = comport_error_unkown;

  if(!dev)
    return comport_error_access_denied;

  error = comport_error_unkown;

  return error;
}
//------------------------------------------------------------------------------
#ifdef UNUSED_FUNCTION
static void COMPORT_set_last_error_E(uint8_t id)
{
  emulation_dev_t  *dev = comport_emu_dev_table.dev[id];

  if(dev)
  {
    dev->last_error = 0;
  }
}
#endif
//------------------------------------------------------------------------------
static comport_status_t COMPORT_get_status_E(uint8_t id)
{
  emulation_dev_t  *dev = comport_emu_dev_table.dev[id];

  if(!dev)
    return comport_status_no_event;
  return dev->current_status;
}
//------------------------------------------------------------------------------
static void COMPORT_process_communication_E(uint8_t id)
{
  emulation_dev_t  *dev = comport_emu_dev_table.dev[id];

  if (!dev || dev->h_comm == 0)
    return;

  if (dev->rx_in_progress)
  {

    /* no error => OK */
    dev->last_status = dev->current_status;
    dev->current_status = comport_read_operation_finished;
    /* Read operation completed successfully */
    dev->rx_in_progress = 0; /* read has ended */
    if (dev->port.on_buffer_arrival_manager != NULL)
    {
      dev->port.on_buffer_arrival_manager(dev->port.id, *dev->read_bytes);
    }
   }
   if (dev->tx_in_progress)
   { /* Write operation completed successfully */
     dev->last_status = dev->current_status;
     dev->current_status = comport_write_operation_finished;
     dev->tx_in_progress = 0; /* write has ended */
     *dev->bytes_sent = dev->temp_bytes_sent;
     if (dev->port.on_buffer_sent_manager!= NULL)
     {
       dev->port.on_buffer_sent_manager(dev->port.id, *dev->bytes_sent);
     }
   }
}
//------------------------------------------------------------------------------
/*static uint8_t disable_hw_flow_ctrl_E(uint8_t id)
{
  emulation_dev_t  *dev = comport_emu_dev_table.dev[id];

  if (!dev || dev->h_comm == 0)
    return 0;

  dev->hw_flow_control = comport_rts_control_disable;
  return 0xFF;

} */
//------------------------------------------------------------------------------

static comport_handler_t *COMPORT_open_E(uint8_t comm_channel_id, comport_cfg_t *comport_cfg,
                                         comm_error_report_t error_report)
{

  emulation_dev_t  *dev;
//  int res;
//  DWORD dwSize;
//  int i;

  if (comport_cfg && (0 < comport_cfg->id) && (comport_cfg->id <= MAX_COMPORT_INSTANCES))
  {
    dev = comport_emu_dev_table.dev[comport_cfg->id];
    if (dev != NULL)
    {
      /* Comport is already created
      Check if Channel id are different from already created file
      */
      if ( comm_channel_id != dev->comm_channel_id )
      {
      dev->last_error = comport_error_open_failed;
      dev->error_report(dev->comm_channel_id, dev->last_error ,__LINE__);

      dev->h_comm = 0;
      free(dev);
      comport_emu_dev_table.dev[comport_cfg->id] = NULL;
      return NULL;
      }
    }
    comport_emu_dev_table.dev[comport_cfg->id] = (emulation_dev_t *) malloc (sizeof(comport_dev_t));
    comport_emu_dev_table.instance_nr = comport_cfg->id;

    dev = comport_emu_dev_table.dev[comport_cfg->id];

    dev->port.id = comport_cfg->id;
    dev->h_comm= (intptr_t)dev;
    dev->comm_channel_id = comm_channel_id;

    memcpy((void*)&dev->comport_cfg, (void*)comport_cfg, sizeof(comport_cfg_t));
    dev->last_error = 0;
    dev->rx_in_progress = 0;
    dev->tx_in_progress = 0;
    dev->error_report = error_report;

    dev->rx_buffer_size = IN_BUFFER_SIZE;
    dev->tx_buffer_size = OUT_BUFFER_SIZE;
    dev->hw_flow_control = comport_rts_control_disable;
    dev->rts = 0;
    dev->dtr = 0;

    dev->port.close = COMPORT_close_port_E;
    dev->port.process_comm = COMPORT_process_communication_E;
    dev->port.rx = COMPORT_rx_E;
    dev->port.flush_rx_buffer = COMPORT_flush_rx_buffer_E;
    dev->port.flush_tx_buffer = COMPORT_flush_tx_buffer_E;
    dev->port.get_modem_status = COMPORT_get_modem_status_E;
    dev->port.get_last_error = COMPORT_get_last_error_E;
    dev->port.get_rx_used_buffer_size = COMPORT_get_rx_used_buffer_size_E;
    dev->port.get_status = COMPORT_get_status_E;
    dev->port.set_baud = COMPORT_set_baudrate_E;
    dev->port.set_buffer_size = COMPORT_set_buffer_size_E;
    dev->port.set_dtr = COMPORT_set_dtr_E;
    dev->port.set_rts = COMPORT_set_rts_E;
    dev->port.tx = COMPORT_tx_E;

    return (&dev->port);
  }
  else
  { /* Invalid id. Must be between 1 and MAX_INSTANCES+1 */
    return NULL;
  }
}
//------------------------------------------------------------------------------
#endif

int COMPORT_prolific_open_port(const char *port_name, int *port_handle)
{
  return 1;
}

DWORD COMPORT_prolific_close_port(int *port_handle)
{
  return 1;
}

int COMPORT_prolific_set_gpio(int port_handle, int gpio_num, int gpio_level)
{
  return 1;
}

uint8_t COMPORT_stop_prolific_board_reset(uint8_t channel)
{
  return false;
}

uint8_t COMPORT_init_prolific_board_reset(uint8_t channel, char *reset_port_name, uint32_t gpio_mask)
{
  return false;
}

 void COMPORT_prolific_port_reset_cb(uint8_t channel)
{
  //Debug("Linux Prolific GPIO Reset\n");
}

#endif // __linux__

