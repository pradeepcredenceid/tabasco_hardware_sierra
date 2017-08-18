/*
*       Copyright (C) 2011-2014 Intel Mobile Communications GmbH
*
*            Sec Class: Intel Confidential (IC)
*
*
* This document contains proprietary information belonging to IMC.
* Technologies Denmark A/S. Passing on and copying of this document, use and
* communication of its contents is not permitted without prior written
* authorisation.
*
* Description:
*   IFWD protocol handler - code
*     The protocol handler is responsible for the data transfer between the
*     upper layers and the target.It is responsible for framing and de-framing
*     the data packet during the transaction. The protocol used will be
*     transparent for the upper layers.
*
* Revision Information:
*  $File name  :  /msw_tools/FlashTool/libDownloadTool/src/Portable_code/protocol_handler/IFWD_protocol_handler.c $
*  $CC-Version: .../oint_tools_flashtoole2_01/16 $
*  $Date:       2014-03-13    15:41:07 UTC $
*   Responsible: kofod
*   Comment    :
*
*/

#define THIS_MODULE MODULE_MFH

/*****************************************************************************
*                           Include files                                   *
*****************************************************************************/

#include "OS.h"
#include "DebugModule.h"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "IFWD_timer.h"
#include "IFWD_mem.h"
#include "IFWD_protocol_handler.h"
#include "IFWD_boot.h"
#include "IFWD_comm.h"

#include "IFWD_global_func.h"
#include "IFWD_target_error_handler.h"
#include "IFWD_single_file_handler.h"

/******************************************************************************
*                      CONSTANT DEFINITIONS                                  *
******************************************************************************/
#define PROT_VERSION                      "2.0"

#define PROT_PACK_1_16_HEAD_LENGTH        ((uint32_t)6)
#define PROT_PACK_1_16_TAIL_LENGTH        ((uint32_t)4)
#define PROT_PACK_1_16_HEAD_TAIL_LENGTH   PROT_PACK_1_16_HEAD_LENGTH+PROT_PACK_1_16_TAIL_LENGTH

#define PROT_PACK_1_32_HEAD_LENGTH        ((uint32_t)12)
#define PROT_PACK_1_32_TAIL_LENGTH        ((uint32_t)4)
#define PROT_PACK_1_32_HEAD_TAIL_LENGTH   PROT_PACK_1_32_HEAD_LENGTH+PROT_PACK_1_32_TAIL_LENGTH

#define PROT_PACK_2_32_HEAD_LENGTH        ((uint32_t)8)
#define PROT_PACK_2_32_TAIL_LENGTH        ((uint32_t)0)
#define PROT_PACK_2_32_HEAD_TAIL_LENGTH   PROT_PACK_2_32_HEAD_LENGTH+PROT_PACK_2_32_TAIL_LENGTH

#define PROT_PACK_3_32_HEAD_LENGTH        ((uint32_t)8)
#define PROT_PACK_3_32_TAIL_LENGTH        ((uint32_t)0)
#define PROT_PACK_3_32_HEAD_TAIL_LENGTH   PROT_PACK_3_32_HEAD_LENGTH+PROT_PACK_3_32_TAIL_LENGTH

#define PROT_PACK_MAX_HEADER_SIZE   PROT_PACK_1_32_HEAD_TAIL_LENGTH;

#define SOT                               ((uint16_t)0x0002)
#define EOT                               ((uint8_t)0x03)

#define TR_PROT_1_16 0
#define TR_PROT_3_32 1
#define TR_PROT_1_32 2

/*****************************************************************************
*                         ENUMERATIONS                                      *
*****************************************************************************/

/* This Enum gives the possible management states of the handler */

typedef enum prot_manager_state_s
{
  prot_manager_idle = 0,
  prot_manager_instance_running,
  prot_manager_delete_channel,
  prot_manager_state_not_valid = 0xFFFF
} prot_manager_state_t;

/* This Enum gives the possible protocol states of the handler */

typedef enum prot_state_s
{
  prot_state_idle = 0,        /* Protocol is in idle state */
  prot_state_RX_TX_ready,     /* Protocol is ready to send/receive the data */
  prot_state_transmit_cmd,    /* Protocol is currently sending the packet */
  prot_state_rx_pack_header,  /* Protocol is receiving the packet header */
  prot_state_rx_pack_payload, /* Protocol is receiving  the packet payload */
  prot_state_error,           /* Protocol is in error state*/
  prot_state_not_valid = 0xFFFF
}prot_state_t;

/*Enums used in the Debug messages for interface identification */

typedef enum
{
  PROT_DB_init,
  PROT_DB_close,
  PROT_DB_clock,
  PROT_DB_add_channel,
  PROT_DB_delete_channel,
  PROT_DB_tx_pack,
  PROT_DB_rx_pack,
  PROT_DB_get_status,
  PROT_DB_set_status,
  PROT_DB_abort_process,
  PROT_DB_get_last_error,
  PROT_DB_set_last_error,
  PROT_DB_Rx_pack_notify,
  PROT_DB_Tx_pack_notify
} PROT_debug_t;



/******************************************************************************
*                   STRUCTURE  TYPE DEFINITIONS                              *
******************************************************************************/
/* Protocol pack type definitions */

typedef struct prot_pack_1_16_s
{
  uint16_t        sot;
  uint16_t        type;
  uint16_t        len;
  uint8_t         data[MAX_PACK_DATA_SIZE + TRA_COMPRESSION_HEADER_BYTES];  /* PACK_DATA_SIZE is 2 Kbytes + 4 byte compression header inserted by TRA */
  uint16_t        checksum;
  uint16_t        eot;
} prot_pack_1_16_t;

typedef struct prot_pack_1_32_s
{
  uint32_t        sot;
  uint32_t        type;
  uint32_t        len;
  uint8_t         data[MAX_PACK_DATA_SIZE + TRA_COMPRESSION_HEADER_BYTES];  /* PACK_DATA_SIZE is 2 Kbytes + 4 byte compression header inserted by TRA */
  uint16_t        checksum;
  uint16_t        eot;
} prot_pack_1_32_t;

typedef struct prot_pack_3_32_s
{
  uint16_t        checksum;
  package_type_t  type;
  uint32_t        len;
  uint8_t         data[MAX_PACK_DATA_SIZE + TRA_COMPRESSION_HEADER_BYTES];  /* PACK_DATA_SIZE is 2 Kbytes + 4 byte compression header inserted by TRA */
} prot_pack_3_32_t;

typedef struct g_prot_pack_s
{
  uint8_t          *start_addr;
  uint32_t          len;
  uint16_t          checksum;
  prot_pack_1_16_t *v_1_16;
  prot_pack_1_32_t *v_1_32;
  prot_pack_3_32_t *v_3_32;
} g_prot_pack_t;

typedef prot_payload_t  prot_pack_t;


/* This structure gives the debug information of the prototcol handler */

typedef struct prot_debug_info_s
{
  uint32_t         nr_of_abort_call;
  uint32_t         nr_of_overflow_bytes_tx;
  uint32_t         bytes_tx;
  uint32_t         nr_of_overflow_bytes_rx;
  uint32_t         bytes_rx;
  uint32_t         idle_time;
  uint32_t         busy_time;
  timer_chronom_t  chronom_idle;
  timer_chronom_t  chronom_busy;
  timer_chronom_t  chronom_tx;
  timer_chronom_t  chronom_rx;
  uint32_t         nr_of_mem_malloc_calls;
  uint32_t         nr_of_mem_free_calls;
} prot_debug_info_t;

/* This structure gives the details of an instance of the protocol handler */

typedef struct prot_instance_s
{
  uint8_t              channel_id;         /* Instance number */
  prot_state_t         state;              /* Current state of the instance */
  g_prot_pack_t        g_tx_prot_pack;     /* Transmitted packet */
  prot_header_t       *rx_hdr;             /* Packet header received */
  prot_payload_t      *rx_payload;         /* Packet payload received */
  g_prot_pack_t        g_rx_prot_pack;     /* Packet received */

  prot_fx_tx_t         fx_tx_pack;         /* Upper layer's TX Callback function */
  prot_fx_rx_t         fx_rx;              /* Upper layer's RX callback function */

  timer_event_timer_t  tx_pack_timer;      /* Timer for data transmission timeout */
  timer_event_timer_t  rx_timer;           /* Timer for data reception timeout */

  prot_status_t        last_status;        /* Handler's current status */
  prot_error_t         last_error;         /* Handler's current error state */

  uint16_t             debug_enabled;      /* Debug is enabled if value is 1*/
  uint16_t             rx_done;            /* If 1, reception is complete */
  uint16_t             instance_running;   /* ????????????? */
  uint32_t             UseProtocolVer;     /* Ex 0x00010010 (1.16) 0x00020020 (2.32)  */
  uint16_t             last_tx_pack_type;  /* Used for skip wr pack cnf */
  uint8_t              align_read_ahead;   /* the number of payload bytes read along with the header. For uint32_t alignment at HW level. */
  target_error_t       target_error;
} prot_instance_t;


/* This Structure contains infromation that the protocol manager maintains */

typedef struct prot_manager_s
{
  uint8_t               manager_id;              /* Manager identification number */
  uint8_t               nr_of_instances_running; /* count of running instances*/
  prot_manager_state_t  state;                   /* Current state of the of the manager */
  prot_status_t         last_status;             /* current status of the active instance */
  prot_error_t          last_error;              /* Handler's current error */
  prot_instance_t      *prot_handler[MAX_NUMBER_OF_CHANNELS + 1];/* Instances running*/
  uint8_t               active_channel;         /* channel number of the active instance */
  prot_instance_t      *active_prot_instance;   /*pointer to active instance*/
} prot_manager_t;


/*****************************************************************************
*                      LOCAL PROTOTYPES                                     *
*****************************************************************************/
const char * const pack_name[] = {
  "Errors",
  "RspChecksumError",
  "RspRxTimeoutError",
  "ErrorHandling",
  "IndErrorMsg",
  "IndWarningMsg",
  "IndInfoMsg",
  "IndDebugPackage",
  "MainStuff",
  "ReqRetransmission",
  "CnfBaudChange",
  "ErrWrongBaudrate",
  "ReqCfiInfo_1",
  "ReqCfiInfo_2",
  "ReqSetProtConf",
  "TestAndDebug",
  "RspRxLoopback",
  "ReqSetLed",
  "LowLevelService",
  "ReqSetRamWorkAddress",
  "ReqWriteToRam",
  "ReqExecuteFromRam",
  "ReqSecStart",
  "ReqSecEnd",
  "ReqCloseHandle",
  "ReqBootloaderVer",
  "ReqForceHwReset",
  "FlashFunctions",
  "ReqFlashId",
  "ReqFlashSetAddress",
  "ReqFlashReadBlock",
  "ReqFlashWriteBlock",
  "ReqFlashEraseStart",
  "ReqFlashEraseCheck",
  "ReqFlashReadChecksum",
  "ReqCompressedPackage",
  "Last_pack"
};


/*****************************************************************************
*                      LOCAL PROTOTYPES                                     *
*****************************************************************************/
static void State_machine(prot_instance_t *p_prot_ins);
static void Rx_pack_notify(uint8_t id, uint32_t length);
static void Tx_pack_notify(uint8_t id);
static uint16_t prot_pack_checksum(prot_instance_t *p_ins,
                              prot_header_t *p_hdr,
                              prot_payload_t *p_data);
static void Create_prot_package(prot_instance_t *p_ins,
                                g_prot_pack_t   *prot_pack,
                                prot_header_t   *p_hdr,
                                prot_payload_t  *p_data);
static uint16_t Calc_chksum_rx_pack(prot_header_t   *p_hdr,
                                uint8_t             *p_payload);
static void Create_rx_pack_hdr(prot_instance_t *p_ins,
                               g_prot_pack_t   *p_block,
                               prot_header_t   *p_hdr);
static uint32_t Prot_pack_header_size(uint32_t prot_version);
static uint32_t Prot_pack_payload_size(uint8_t channel,
                                  uint32_t prot_version,
                                  g_prot_pack_t   *p_block);
static uint8_t *Prot_pack_get_hdr_addr(uint32_t prot_version, prot_payload_t *p_payload);
static void Receive_payload(prot_instance_t *p_prot_ins);
static void Verify_rx_pack(prot_instance_t *p_prot_ins);
static void Trace_tx_data(uint8_t channel, uint32_t prot_version, uint8_t *p_data, uint32_t len);
static void Trace_rx_data(uint8_t channel, uint32_t prot_version, g_prot_pack_t *p_pack);

/*****************************************************************************
*                     PUBLIC PROTOTYPES                                     *
*****************************************************************************/
/* See IFWD_protocol_handler.h */

/******************************************************************************
*                         LOCAL VARIABLES                                    *
******************************************************************************/

static prot_manager_t   prot_manager;
static prot_manager_t  *p_prot_manager = NULL;
static uint8_t prot_activeList[MAX_NUMBER_OF_CHANNELS + 1];
static uint16_t prot_activeListNof = 0;



/******************************************************************************
*                         IMPLEMENTATION                                     *
******************************************************************************/

/************************** Public Functions ********************************/

/*******************************************************************************
* Function: void PROT_init(void);
*------------------------------------------------------------------------------
* Purpose:   This function is responsible for the initialization of the
*            structures used to manage the creation and disposal of protcol
*            and to trigger the state machine instances.
*
* Parameters:
* IN         None
*
* IN/OUT     None
*
* Returns:   Nothing
******************************************************************************/
void PROT_init(void)
{
  uint16_t i;
  p_prot_manager = &prot_manager;

  for (i = 0; i <= MAX_NUMBER_OF_CHANNELS; i++)
    p_prot_manager->prot_handler[i] = NULL;

  prot_activeListNof = 0; /* we have nothing in the active list */

  p_prot_manager->manager_id = 0xFF;
  p_prot_manager->state = prot_manager_idle;
  p_prot_manager->nr_of_instances_running = 0;
  p_prot_manager->last_status = prot_status_ok;
  p_prot_manager->last_error = prot_error_no_error;

  PROT_abort_process(0); // For coverage
  Calc_chksum_rx_pack(NULL, NULL); //
}

/*******************************************************************************
* Function:  void PROT_close(void);
*------------------------------------------------------------------------------
* Purpose:   This function is responsible for cleaning up the protocol
*            management state machine and instances, if any.
*
* Parameters:
* IN         None
*
* IN/OUT     None
*
* Returns:   Nothing
******************************************************************************/
void PROT_close(void)
{
  uint16_t i;
  for(i = 0; i < p_prot_manager->nr_of_instances_running; i++)
  {
    if(p_prot_manager->prot_handler[i])
    {
      PROT_delete_channel((uint8_t)i);
      MEM_free((uint8_t *)p_prot_manager->prot_handler[i]);
      p_prot_manager->prot_handler[i] = NULL;
    }
  }
  p_prot_manager->nr_of_instances_running = 0;
  p_prot_manager->state = prot_manager_idle;
  prot_activeListNof = 0; /* we have nothing in the active list */
}


/*******************************************************************************
* Function:  void PROT_clock(void);
*------------------------------------------------------------------------------
* Purpose:   This function is responsible to trigger the internal state machine
*            for each instance currently running.
*
* Parameters:
* IN         None
*
* IN/OUT     None
*
* Returns:   Nothing
******************************************************************************/
void PROT_clock(void)
{
  uint16_t i;
  uint8_t channel;
  if(p_prot_manager)
  {         /* The protocol manager process has been initialized */
    for(i = 0 ; i < prot_activeListNof ; i++)
    {
      channel = prot_activeList[i];
      if(p_prot_manager->prot_handler[channel]) /* The instance exists, run the internal state machine */
      {
        State_machine(p_prot_manager->prot_handler[channel]);
      }
    }
  }
}

/*******************************************************************************
* Function: prot_status_t PROT_add_channel(uint8_t channel);
*------------------------------------------------------------------------------
* Purpose:  This function is responsible for the creation of an instance and
*           the protocol state machine.
*
* Parameters:
* IN        channel - instance number
*
* IN/OUT    None
*
* Returns:  prot_status_t
*              The enum value indicating the current status of the handler
******************************************************************************/
prot_status_t   PROT_add_channel(uint8_t channel)
{
  prot_instance_t *p_prot_ins;
  prot_status_t    prot_status;

  if(channel > MAX_NUMBER_OF_CHANNELS || channel == 0)
  {      /* Invalid channel number */
    p_prot_manager->last_status = prot_status_invalid_channel_id;
    prot_status = p_prot_manager->last_status;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
    PROT_Err(channel, PROT_ERR_channel_is_outside_range);
#endif

  }
  else if(p_prot_manager->prot_handler[channel] )
  {      /* The instance for the requested channel is already created */
    p_prot_manager->last_status = prot_status_channel_already_added;
    prot_status = p_prot_manager->last_status;
  }
  else
  {      /* Create the channel */
    p_prot_ins =(prot_instance_t*)MEM_malloc(sizeof(prot_instance_t));
    if (p_prot_ins == NULL)
    {           /* Not enough memory to create an instance */
      p_prot_manager->last_status = prot_status_no_mem;
      prot_status = p_prot_manager->last_status;

#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
      PROT_Err(channel, PROT_ERR_malloc);
#endif

    }
    else
    {             /* Initialize the instance and start it. */
      memset((void *)p_prot_ins, 0x00, sizeof(prot_instance_t));
      p_prot_manager->prot_handler[channel] = p_prot_ins;
      p_prot_manager->nr_of_instances_running += 1;
      /* Instance for the channel can now be initialzed and started */
      p_prot_ins->channel_id = channel;
      p_prot_ins->debug_enabled = 0;
      p_prot_ins->rx_done = 0;
      p_prot_ins->instance_running = 1;

      p_prot_ins->last_error = prot_error_no_error;

      p_prot_ins->last_status = prot_status_ok;

      p_prot_ins->state = prot_state_RX_TX_ready;

//      p_prot_ins->g_tx_prot_pack= NULL;
      p_prot_ins->UseProtocolVer = PROT_1_16; /* For now hardcode to use 1.16 */
      p_prot_ins->last_tx_pack_type = packagedummy;
      p_prot_ins->align_read_ahead = 0;
      prot_status = p_prot_ins->last_status;
      prot_activeList[prot_activeListNof] = channel; /* store new channel in active list */
      prot_activeListNof++;                          /* remember that we have another one*/
    }
  }
  return prot_status;
}

/******************************************************************************
* Function:  prot_status_t PROT_delete_channel(uint8_t channel);
*------------------------------------------------------------------------------
* Purpose:   This function is responsible for disposing a running instance
*            corresponding to the channel id provided.
* Parameters:
* IN         channel  - Channel number identifying the instance
*
* IN/OUT     None
*
* Returns:   prot_status_t
*               The enum value indicating the current status of the handler
*
******************************************************************************/
prot_status_t   PROT_delete_channel(uint8_t channel)
{
  prot_status_t       prot_status;

  if(channel > MAX_NUMBER_OF_CHANNELS || channel == 0)
  {                  /* Invalid channel number */
    p_prot_manager->last_status = prot_status_invalid_channel_id;
    prot_status = p_prot_manager->last_status;

#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
    PROT_Err(channel, PROT_ERR_channel_is_outside_range);
#endif

  }
  else if(p_prot_manager->prot_handler[channel] == NULL)
  {              /* The instance for the requested channel does not exists */
    p_prot_manager->last_status = prot_status_channel_already_deleted;
    prot_status = p_prot_manager->last_status;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
    //    PROT_Err(channel, PROT_ERR_channel_does_not_exist);
#endif


  }
  else
  {               /* Now delete the instance */
    uint16_t n;

    MEM_free((uint8_t *)p_prot_manager->prot_handler[channel]);
    p_prot_manager->prot_handler[channel] = NULL;

    p_prot_manager->nr_of_instances_running -= 1;
    p_prot_manager->last_status = prot_status_ok;
    prot_status = p_prot_manager->last_status;

    /* just rebuild active list instead of adding code to memmove in the list: */
    prot_activeListNof = 0;
    for(n = 1 ; n <= MAX_NUMBER_OF_CHANNELS ; n++) /* channel 0 is not used */
    {
      if(p_prot_manager->prot_handler[n])
      {
        prot_activeList[prot_activeListNof] = (uint8_t)n;
        prot_activeListNof++;
      }
    }
  }
  return prot_status;
}
/*******************************************************************************
* Function: prot_status_t PROT_set_protocol(uint8_t channel, uint32_t prot_type);
*------------------------------------------------------------------------------
* Purpose:  Change the protocol 1.16, 2.32 ....
*
* Parameters:
* IN        channel   - Instance number
*           prot_type - PROT_1_16, PROT_2_32
*
* IN/OUT    None
*
* Returns:  prot_status_t
*              The enum value indicating the current status of the handler
******************************************************************************/
prot_status_t PROT_set_protocol(uint8_t channel, uint32_t prot_type)
{
  prot_error_t     prot_error  = prot_error_no_error;
  prot_status_t    prot_status = prot_status_ok;

  prot_instance_t *p_prot_ins;

  if(channel <= MAX_NUMBER_OF_CHANNELS)
  {
    p_prot_ins = p_prot_manager->prot_handler[channel];
    if(p_prot_ins)
    {
      p_prot_ins->UseProtocolVer = prot_type;
    }
    else
    {         /* Channel instance does not exists */
      p_prot_manager->last_status = prot_status_channel_instance_not_exist;

#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
      PROT_Err(channel, PROT_ERR_invalid_channel_instance);
#endif

      return  prot_status_channel_instance_not_exist;
    }
  }
  else
  {           /* Invalid channel number */
    p_prot_manager->last_status = prot_status_invalid_channel_id;

#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
    PROT_Err(channel, PROT_ERR_channel_is_outside_range);
#endif

    return prot_status_invalid_channel_id;
  }
  p_prot_ins->last_status = prot_status;
  p_prot_ins->last_error = prot_error;

  return prot_status;

}

/*******************************************************************************
* Function: uint16_t prot_pack_checksum(prot_header_t *p_hdr, prot_payload_t *p_data)
*------------------------------------------------------------------------------
* Purpose: Creates the package according to the protocol used
*
* Parameters:
* IN       p_hdr     - Protocol header
*          p_data    - Protocol pack
*
* Returns: Protocol pack XOR16 checksum
******************************************************************************/
/*lint -e438 value assigned to p is not used ... but it is. */
static uint16_t prot_pack_checksum(prot_instance_t *p_ins, prot_header_t *p_hdr, prot_payload_t *p_data)
{
  uint16_t checksum;
  volatile uint8_t  *p;
  int len;


  checksum = p_hdr->type;

  if (p_ins->UseProtocolVer == PROT_1_16)
  {
    len = (uint16_t)p_hdr->len;
    checksum += len;
  }
  else
  {
    len = p_hdr->len;
    checksum += (len >> 16) + (len & 0xFFFF);
  }

  p = p_data->data;
  #ifdef __BORLANDC__
    #pragma warn -8066
  #endif
/*lint -e646 case/default within do loop; may have been misplaced (duffs device) */
/*lint -e616 control flows into case/default (needed and used by duffs device) */
  switch (len & 7)         /* Use "duffs device" to unroll loop to make the operation ~3 times faster */
  {                        /* It looks ugly, but it is quite effective - google it */

    do
    {
              checksum += *p++;
      case 7: checksum += *p++;
      case 6: checksum += *p++;
      case 5: checksum += *p++;
      case 4: checksum += *p++;
      case 3: checksum += *p++;
      case 2: checksum += *p++;
      case 1: checksum += *p++;
      case 0:;
    } while ((len -= 8) >= 0);
  }
  #ifdef __BORLANDC__
    #pragma warn +8066
  #endif
/*lint +e646 case/default within do loop; may have been misplaced (duffs device) */
/*lint +e616 control flows into case/default (needed and used by duffs device) */

  return checksum;
}
/*line +e438 */

/*******************************************************************************
* Function: void Create_prot_package(prot_instance_t *p_ins,
*                                    g_prot_pack_t   *prot_pack,
*                                    prot_header_t   *p_hdr,
*                                    prot_payload_t  *p_data);
*------------------------------------------------------------------------------
* Purpose: Creates the package according to the protocol used
*
* Parameters:
* IN       p_inst    - Protocol instance.
*          p_hdr     - Protocol header
*          prot_pack - Protocol to create

*
* IN/OUT   p_data    - Protocol pack
*
* Returns: Nothing
******************************************************************************/
static void Create_prot_package(prot_instance_t *p_ins,
                                g_prot_pack_t   *prot_pack,
                                prot_header_t   *p_hdr,
                                prot_payload_t  *p_data)
{
  uint16_t *p_checksum;
  uintptr_t addr;

  switch(p_ins->UseProtocolVer)
  {
    case PROT_1_16:
      addr = (uintptr_t)(p_data->data) - PROT_PACK_1_16_HEAD_LENGTH;
      prot_pack->v_1_16 = (prot_pack_1_16_t *)(addr);
      prot_pack->start_addr = (uint8_t *)prot_pack->v_1_16;
      prot_pack->v_1_16->sot = SOT;
      prot_pack->v_1_16->type = p_hdr->type;
      prot_pack->v_1_16->len = (uint16_t)p_hdr->len;
      p_checksum = (uint16_t *)&p_data->data[p_hdr->len];
      *p_checksum = prot_pack_checksum(p_ins, p_hdr, p_data);
      p_data->data[(int)(p_hdr->len + 2)] = EOT;
      p_data->data[(int)(p_hdr->len + 3)] = 0;
      prot_pack->len = (uint32_t)(prot_pack->v_1_16->len + PROT_PACK_1_16_HEAD_TAIL_LENGTH);
      break;

    case PROT_1_32:
      addr = (uintptr_t)p_data->data - PROT_PACK_1_32_HEAD_LENGTH;
      prot_pack->v_1_32 = (prot_pack_1_32_t *)(addr);
      prot_pack->start_addr = (uint8_t *)prot_pack->v_1_32;
      prot_pack->v_1_32->sot = SOT;
      prot_pack->v_1_32->type = p_hdr->type;
      prot_pack->v_1_32->len = (uint32_t)p_hdr->len;
      p_checksum = (uint16_t *)&p_data->data[p_hdr->len];
      *p_checksum = prot_pack_checksum(p_ins, p_hdr, p_data);
      p_data->data[(int)(p_hdr->len + 2)] = EOT;
      p_data->data[(int)(p_hdr->len + 3)] = 0;
      prot_pack->len = (uint32_t)(prot_pack->v_1_32->len + PROT_PACK_1_32_HEAD_TAIL_LENGTH);
      break;

    case PROT_2_32:
    case PROT_3_32:
      addr = (uintptr_t)p_data->data - PROT_PACK_3_32_HEAD_LENGTH;
      prot_pack->v_3_32 = (prot_pack_3_32_t *)(addr);
      prot_pack->start_addr = (uint8_t *)prot_pack->v_3_32;
      prot_pack->v_3_32->type = p_hdr->type;
      prot_pack->v_3_32->len = p_hdr->len;
      prot_pack->v_3_32->checksum = prot_pack_checksum(p_ins, p_hdr, p_data);
      prot_pack->len = GLOBAL_use_pack_size[p_ins->channel_id] + PROT_PACK_3_32_HEAD_TAIL_LENGTH;
      break;

    default:
      break;
  }
}



/*******************************************************************************
 * Function: prot_status_t PROT_tx_pack(uint8_t channel,
 *                                      prot_header_t *p_hdr,
 *                                      prot_payload_t *p_data,
 *                                      prot_fx_tx_t fx_tx);
 *------------------------------------------------------------------------------
 * Purpose:  Transmit a protocol pack if the target is booted and instance is
 *           in ready state
 *
 * Parameters:
 * IN        channel   - Instance number
 *           p_hdr     - Pointer to protocol header
 *           p_data    - Pointer to protocol payload
 *           fx_tx     - Callback function called when transmition is complete
 *
 * IN/OUT    None
 *
 * Returns:  prot_status_t
 *              The enum value indicating the current status of the handler
 ******************************************************************************/
prot_status_t PROT_tx_pack(uint8_t channel,
                           prot_header_t *p_hdr,
                           prot_payload_t *p_data,
                           prot_fx_tx_t fx_tx)
{
  prot_error_t     prot_error  = prot_error_no_error;
  prot_status_t    prot_status;
  comm_status_t    comm_status;
  char            *p_temp_tx_buf_ptr;
  uint32_t         temp_tx_len;

  prot_instance_t *p_prot_ins;
  uint32_t         bytes_sent =0;

  if(channel <= MAX_NUMBER_OF_CHANNELS)
  {
    p_prot_ins = p_prot_manager->prot_handler[channel];
    if(p_prot_ins)
    {
      if(p_prot_ins->state == prot_state_RX_TX_ready)
      {
        /* Finally start the write operation */
        p_prot_ins->state = prot_state_transmit_cmd;
        prot_status = prot_status_tx_pack_ongoing;
        p_prot_ins->last_tx_pack_type = p_hdr->type;
        if ((p_prot_ins->last_tx_pack_type == ReqFlashWriteBlock) && target_capabilities[channel].skip_wr_pack_cnf)
        {
            p_prot_ins->last_tx_pack_type = p_hdr->type;
        }
        Create_prot_package(p_prot_ins, &p_prot_ins->g_tx_prot_pack, p_hdr, p_data);

        p_prot_manager->active_prot_instance = p_prot_ins;
        p_prot_manager->active_channel = channel;
        p_prot_ins->fx_tx_pack= fx_tx;

        TIMER_new_timer(&p_prot_ins->tx_pack_timer, TRA_GetCommTimeOut(channel));

        p_temp_tx_buf_ptr = ((char *)p_prot_ins->g_tx_prot_pack.start_addr);
        temp_tx_len = p_prot_ins->g_tx_prot_pack.len;

        if (COMM_TECH_IS_MIPI_ADD_HEADER(p_prot_ins->channel_id))
        {
          // The buffer is of type prot_payload_t which have a uint8_t dummy[128]
          // field in front of the payload data. So there is room for the uint32_t.
          p_temp_tx_buf_ptr -= sizeof(uint32_t);
          temp_tx_len += sizeof(uint32_t);

          *((uint32_t*)p_temp_tx_buf_ptr) = p_prot_ins->g_tx_prot_pack.len;
        }
        // Pad extra bytes if Protocol needs it
        temp_tx_len += temp_tx_len % GLOBAL_comm_align_bytes[p_prot_ins->channel_id];

        comm_status = COMM_send_buffer(channel,
                                        p_temp_tx_buf_ptr,
                                        temp_tx_len,
                                        &bytes_sent,
                                        Tx_pack_notify);

        switch(comm_status )
        {
        case comm_status_write_finished:
          prot_status = prot_status_tx_pack_done;
          p_prot_ins->state = prot_state_RX_TX_ready;

          Trace_tx_data(p_prot_ins->channel_id,
                        p_prot_ins->UseProtocolVer,
                        p_prot_ins->g_tx_prot_pack.start_addr,
                        p_prot_ins->g_tx_prot_pack.len);

          if(p_prot_ins->fx_tx_pack)
          {
            p_prot_ins->fx_tx_pack(channel,true);
            p_prot_ins->fx_tx_pack = NULL;
            if ((p_prot_ins->last_tx_pack_type == ReqFlashWriteBlock) &&
                (target_capabilities[channel].skip_wr_pack_cnf) &&
                ((p_prot_ins->UseProtocolVer == PROT_2_32) ||
                 (p_prot_ins->UseProtocolVer == PROT_3_32)))
            {
              /* Fake the response of the write block package */
              p_prot_ins->g_rx_prot_pack.start_addr = Prot_pack_get_hdr_addr(p_prot_ins->UseProtocolVer,
                                                                             p_prot_ins->rx_payload);
              p_prot_ins->rx_hdr->type = ReqFlashWriteBlock;
              p_prot_ins->rx_hdr->len = 2;
              p_prot_ins->rx_payload->data[0] = 0x00;
              p_prot_ins->rx_payload->data[1] = 0x00;
              if (p_prot_ins->fx_rx)
              {
                p_prot_ins->fx_rx(channel,true);
                p_prot_ins->fx_rx = NULL;
              }
              p_prot_ins->last_status = prot_status_rx_pack_ready;
              p_prot_ins->state = prot_state_RX_TX_ready;

            }

          }
          break;
        case comm_status_write_ongoing:
          prot_status = prot_status_tx_pack_ongoing;
          break;
        case comm_status_write_timeout:
          prot_status = prot_status_tx_pack_timeout;
          prot_error  = prot_error_tx_pack_timeout;
          p_prot_ins->state = prot_state_error;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
          PROT_Err(channel, PROT_ERR_tx_pack_timeout);
#endif

          break;
        case comm_status_write_error:
          prot_status = prot_status_error;
          prot_error = prot_error_tx_pack_error;
          p_prot_ins->state = prot_state_error;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
          PROT_Err(channel, PROT_ERR_tx_pack_error);
#endif
          break;

        default:
          break;
        }
      }
      else
      {
        switch(p_prot_ins->state)
        {
        case prot_state_rx_pack_header:
        case prot_state_rx_pack_payload:
          prot_error = prot_error_rx_ongoing;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
          PROT_Err(channel, PROT_ERR_rx_ongoing);
#endif
          break;
        case prot_state_transmit_cmd:
          prot_error = prot_error_tx_ongoing;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
          PROT_Err(channel, PROT_ERR_tx_ongoing);
#endif
          break;
        case prot_state_error:
          prot_error = prot_error_handler_in_error_state;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
          PROT_Err(channel, PROT_ERR_handler);
#endif

          break;
        default:
          prot_error = prot_error_unknown;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
          PROT_Err(channel, PROT_ERR_unknown);
#endif
          break;
        }
        prot_status = prot_status_error;
        p_prot_ins->state = prot_state_error;
      }
    }
    else
    {         /* Channel instance does not exists */
      p_prot_manager->last_status = prot_status_channel_instance_not_exist;

#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
      PROT_Err(channel, PROT_ERR_invalid_channel_instance);
#endif

      return  prot_status_channel_instance_not_exist;
    }
  }
  else
  {           /* Invalid channel number */
    p_prot_manager->last_status = prot_status_invalid_channel_id;

#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
    PROT_Err(channel, PROT_ERR_channel_is_outside_range);
#endif

    return prot_status_invalid_channel_id;
  }
  p_prot_ins->last_status = prot_status;
  p_prot_ins->last_error = prot_error;

  return prot_status;

}

/*******************************************************************************
* Function: uint16_t Calc_chksum_rx_pack(prot_header_t   *p_hdr
*                                   uint8_t              *p_payload)
*------------------------------------------------------------------------------
* Purpose: Checksum calculation
*
* Parameters:
* IN       p_hdr     - Protocol header
*          p_payload - Protocol payload
*
*
* Returns: XOR 16 checksum
******************************************************************************/
/*lint -e438 value assigned to p is not used ... but it is. */
static uint16_t Calc_chksum_rx_pack(prot_header_t   *p_hdr,
                                uint8_t             *p_payload)
{
  volatile unsigned char *p;
  int len;
  uint16_t checksum = 0;
  if ((p_hdr == NULL) || (p_payload == NULL))
	  return 0;

  if (dll_capabilities.protocol_crc  != CAP_PROT_NO_CRC)
  {
    checksum = p_hdr->type + p_hdr->len;
    p   = p_payload;
    len = p_hdr->len;

  #ifdef __BORLANDC__
    // Remove Borlands incorrect "Unreachable code warning" (needed and used by duffs device)
    #pragma warn -8066
  #endif
    /**
     * Use "duffs device" to unroll loop to make the operation ~3 times faster
     * It looks ugly, but it is quite effective - google it
     **/
    /*lint -e646 case/default within do loop; may have been misplaced */
    /*lint -e616 control flows into case/default */
    switch (len & 7)
    {
      do
      {
        checksum += *p++;
        case 7: checksum += *p++;
        case 6: checksum += *p++;
        case 5: checksum += *p++;
        case 4: checksum += *p++;
        case 3: checksum += *p++;
        case 2: checksum += *p++;
        case 1: checksum += *p++;
        case 0:;
      } while ((len -= 8) >= 0);
    }
  #ifdef __BORLANDC__
    #pragma warn +8066
  #endif
  }
  return checksum;
}
/*lint +e438 */
/*******************************************************************************
* Function: void Create_rx_prot_hdr(prot_instance_t *p_ins,
                                     g_prot_pack_t   *p_block,
*                                    prot_header_t   *p_hdr)
*------------------------------------------------------------------------------
* Purpose: Creates the package according to the protocol used
*
* Parameters:
* IN       p_inst    - Protocol instance.
*          p_block   - Memory block containing the data received
*
*
* OUT      p_hdr     - Protocol header
* Returns: Nothing
******************************************************************************/
static void Create_rx_pack_hdr(prot_instance_t *p_ins,
                               g_prot_pack_t   *p_block,
                               prot_header_t   *p_hdr)
{
  switch(p_ins->UseProtocolVer)
  {
    case PROT_1_16:
      p_block->v_1_16 = (prot_pack_1_16_t *)(p_block->start_addr);
      p_hdr->type = p_block->v_1_16->type;
      p_hdr->len = p_block->v_1_16->len;
      break;

    case PROT_1_32:
      p_block->v_1_32 = (prot_pack_1_32_t *)(p_block->start_addr);
      p_hdr->type = p_block->v_1_32->type;
      p_hdr->len = p_block->v_1_32->len;
      break;

    case PROT_2_32:
    case PROT_3_32:
      p_block->v_3_32 = (prot_pack_3_32_t *)(p_block->start_addr);
      p_hdr->type = p_block->v_3_32->type;
      p_hdr->len = p_block->v_3_32->len;
      break;

    default:
      break;
  }
}

/*******************************************************************************
 * Function: uint8_t *Prot_pack_get_hdr_addr(uint32_t prot_version,
 *                                      prot_payload_t *p_payload)
 *
  *------------------------------------------------------------------------------
 * Purpose: Retrieve the  start address of the protocol pack header*
 * Parameters:
 * IN       prot_version- Active protocol version
 *          p_payload   - Pointer to the memory buffer
 *
 *
 * Returns:
 *          Address of the memory block where the header will be stored
 *
 ******************************************************************************/
static uint8_t *Prot_pack_get_hdr_addr(uint32_t prot_version, prot_payload_t *p_payload)
{
  uint8_t *addr, offset;

  switch (prot_version)
  {
    case PROT_1_16:
      offset = PROT_PACK_1_16_HEAD_LENGTH;
      break;

    case PROT_1_32:
      offset = PROT_PACK_1_32_HEAD_LENGTH;
      break;

    case PROT_2_32:
    case PROT_3_32:
      offset = PROT_PACK_3_32_HEAD_LENGTH;
      break;

    default:
      offset = 0;
      break;
  }
  addr = (uint8_t *)((intptr_t)p_payload->data - offset);
  return addr;
}


/*******************************************************************************
 * Function: prot_status_t PROT_rx_pack(uint8_t channel,
 *                                      prot_header_t *p_hdr,
 *                                      prot_payload_t *p_data,
 *                                      prot_fx_rx_t fx_rx);
 *------------------------------------------------------------------------------
 * Purpose: upper layers can invoke this function to get the data buffer
 *          received through the communication channel
 *
 * Parameters:
 * IN       channel- Instance number
 *          fx_rx  - Callback function called when the data reception is
 *                   complete
 *
 * IN/OUT   p_hdr     - Pointer to protocol header
 *          p_data    - Pointer to protocol payload
 *                       written
 *
 * Returns: prot_status_t
 *              The enum value indicating the current status of the handler
 *
 ******************************************************************************/
prot_status_t PROT_rx_pack( uint8_t channel,
                            prot_header_t *p_hdr,
                            prot_payload_t *p_data,
                            prot_fx_rx_t fx_rx)
{
  prot_instance_t *p_prot_ins;
  comm_status_t    comm_status;
  uint32_t              bytes_read;
  uint8_t               *p_temp_rx_buf_ptr;
  uint32_t              temp_rx_len;

  if(channel <= MAX_NUMBER_OF_CHANNELS)
  {
    p_prot_ins = p_prot_manager->prot_handler[channel];
    if(p_prot_ins )
    {
      p_prot_ins->fx_rx = fx_rx;
      p_prot_ins->rx_hdr = p_hdr;
      p_prot_ins->rx_payload = p_data;

      if (p_prot_ins->last_tx_pack_type == ReqFlashWriteBlock
       && target_capabilities[channel].skip_wr_pack_cnf
       && p_prot_ins->UseProtocolVer >= PROT_2_32)
      {
        p_prot_ins->last_status = prot_status_rx_pack_ready;
        p_prot_ins->state = prot_state_RX_TX_ready;
      }
      else
      {
        if(p_prot_ins->state == prot_state_RX_TX_ready)
        {
          p_prot_ins->g_rx_prot_pack.len = Prot_pack_header_size(p_prot_ins->UseProtocolVer);
          p_prot_ins->g_rx_prot_pack.start_addr = Prot_pack_get_hdr_addr(p_prot_ins->UseProtocolVer,
                                                                         p_prot_ins->rx_payload);
          /* Finally start the read operation */
          p_prot_ins->state = prot_state_rx_pack_header;
          p_prot_ins->last_status = prot_status_rx_pack_ongoing;
          p_prot_manager->active_prot_instance = p_prot_ins;
          p_prot_ins->rx_done = 0;
          p_prot_manager->active_channel = channel;
          TIMER_new_timer(&p_prot_ins->rx_timer, TRA_GetCommTimeOut(channel));
          memset(p_prot_ins->rx_hdr, 0, sizeof(prot_header_t));
          memset(p_prot_ins->rx_payload->data, 0, 1024);

          // Use tmp pointer to receive data before the EBL package.
          p_temp_rx_buf_ptr = ((uint8_t *)p_prot_ins->g_rx_prot_pack.start_addr);
          temp_rx_len = p_prot_ins->g_rx_prot_pack.len;
          p_prot_ins->align_read_ahead = 0;

          if (COMM_TECH_IS_MIPI_ADD_HEADER(p_prot_ins->channel_id))
          {
            // When using the EXT_MIPI_USB box there is a uint32_t length word sent in
            // from of the EBL package.
            // The buffer is of type prot_payload_t which have uint8_t dummy[128]
            // in front of the payload data so there is room for the uint32_t word.
            p_temp_rx_buf_ptr -= sizeof(uint32_t);
            temp_rx_len += sizeof(uint32_t);
          }
          // Read into the payload data if the number of alignment bytes calls for it.
      	  p_prot_ins->align_read_ahead = temp_rx_len % GLOBAL_comm_align_bytes[p_prot_ins->channel_id];   /* remember the number of payload data read here */
      	  temp_rx_len += p_prot_ins->align_read_ahead;

      	  comm_status = COMM_receive_buffer(channel,
                                            (char*)p_temp_rx_buf_ptr,
                                            temp_rx_len,
                                            &bytes_read,
                                            Rx_pack_notify);

          switch(comm_status )
          {
          case comm_status_read_finished:
            p_prot_ins->last_status = prot_status_rx_pack_ongoing;
            p_prot_ins->state = prot_state_rx_pack_payload;
            Receive_payload(p_prot_ins);
            break;
          case comm_status_read_ongoing:
            p_prot_ins->last_status = prot_status_rx_pack_ongoing;
            break;
          default:
            p_prot_ins->last_status = prot_status_error;
            p_prot_ins->last_error  = prot_error_rx_pack_error;
            p_prot_ins->state = prot_state_error;
  #if defined (USE_TEXT_CLASS_IFWD_ERRORS)
            PROT_Err(channel, PROT_ERR_rx_pack_error);
  #endif
            break;
          }
        }
        else  /*  if state is not prot_state_RX_TX_ready */
        {
          switch(p_prot_ins->state)
          {
          case prot_state_rx_pack_header:
          case prot_state_rx_pack_payload:
            p_prot_ins->last_error = prot_error_rx_ongoing;
  #if defined (USE_TEXT_CLASS_IFWD_ERRORS)
            PROT_Err(channel, PROT_ERR_rx_ongoing);
  #endif
            break;
          case prot_state_transmit_cmd:
            p_prot_ins->last_error = prot_error_tx_ongoing;
  #if defined (USE_TEXT_CLASS_IFWD_ERRORS)
            PROT_Err(channel, PROT_ERR_tx_ongoing);
  #endif
            break;
          case prot_state_error:
            p_prot_ins->last_error = prot_error_handler_in_error_state;
  #if defined (USE_TEXT_CLASS_IFWD_ERRORS)
            PROT_Err(channel, PROT_ERR_handler);
  #endif

            break;
          default:
            p_prot_ins->last_error = prot_error_unknown;
  #if defined (USE_TEXT_CLASS_IFWD_ERRORS)
            PROT_Err(channel, PROT_ERR_unknown);
  #endif

          }
          p_prot_ins->last_status = prot_status_error;
          p_prot_ins->state = prot_state_error;
        }
      }
    }
    else
    {
      p_prot_manager->last_status = prot_status_channel_instance_not_exist;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
      PROT_Err(channel, PROT_ERR_invalid_channel_instance);
#endif
      return prot_status_channel_instance_not_exist;
    }
  }
  else
  {
    p_prot_manager->last_status = prot_status_invalid_channel_id;

#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
    PROT_Err(channel, PROT_ERR_channel_is_outside_range);
#endif

    return prot_status_invalid_channel_id;
  }

  return p_prot_ins->last_status;
}

#ifdef INCLUDE_UNUSED_FX

/*******************************************************************************
* Function: prot_status_t PROT_get_status(uint8_t channel, char *status)
*------------------------------------------------------------------------------
* Purpose:  This function returns the last status registred in the protocol
*           instance of the given channel.
*
* Parameters:
* IN        channel - Instance number.
*
* IN/OUT    status - status information  in the form of a string. The calling
*                    function is responsible for the memory allocation
*
* Returns:  prot_status_t
*              The enum value indicating the current status of the handler
*
******************************************************************************/
prot_status_t PROT_get_status(uint8_t channel, char *status)
{
  prot_status_t     prot_status = prot_status_ok;
  prot_instance_t  *p_prot_ins;
  PARAM_NOT_USED(status);

  if(channel <= MAX_NUMBER_OF_CHANNELS)
  {        /* channel number is valid */
    p_prot_ins = p_prot_manager->prot_handler[channel];
    if(p_prot_ins )
    {      /* channel instance exists */
      prot_status = p_prot_ins->last_status;
      p_prot_ins->last_status = prot_status;
    }
    else
    {
      p_prot_manager->last_status = prot_status_channel_instance_not_exist;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
      PROT_Err(channel, PROT_ERR_invalid_channel_instance);
#endif

      prot_status = prot_status_channel_instance_not_exist;
    }
  }
  else
  {        /* Invalid channel number */
    p_prot_manager->last_status = prot_status_invalid_channel_id;

#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
    PROT_Err(channel, PROT_ERR_channel_is_outside_range);
#endif

    prot_status = prot_status_invalid_channel_id;
  }
  return prot_status;
}

/*******************************************************************************
* Function: prot_status_t PROT_set_status(uint8_t channel)
*------------------------------------------------------------------------------
* Purpose: sets the last status registred in the instance to prot_status_ok
*
* Parameters:
* IN       channel - Instance number.
*
* IN/OUT   None
*
* Returns: prot_status_t
*              The enum value indicating the current status of the handler
******************************************************************************/
prot_status_t PROT_set_status(uint8_t channel)
{
  prot_status_t     prot_status = prot_status_ok;
  prot_instance_t  *p_prot_ins;

  if(channel <= MAX_NUMBER_OF_CHANNELS)
  {           /* channel number is valid */
    p_prot_ins = p_prot_manager->prot_handler[channel];
    if(p_prot_ins )
    {         /* channel instance exists */
      p_prot_ins->last_status = prot_status_ok;
    }
    else
    {          /* channel instance does not exists */
      p_prot_manager->last_status = prot_status_channel_instance_not_exist;
      prot_status = prot_status_channel_instance_not_exist;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
      PROT_Err(channel, PROT_ERR_invalid_channel_instance);
#endif

    }
  }
  else
  {            /* Invalid channel number */
    p_prot_manager->last_status = prot_status_invalid_channel_id;
    prot_status = prot_status_invalid_channel_id;

#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
    PROT_Err(channel, PROT_ERR_channel_is_outside_range);
#endif

  }
  return prot_status;
}
#endif

/*******************************************************************************
* Function: prot_status_t PROT_abort_process(uint8_t channel);
*------------------------------------------------------------------------------
* Purpose:  This function will terminate all oustanding transaction and close
*           the communication handler identified in the parameter channel. The
*           resources allocated by the instances are disposed
*
* Parameters:
* IN        channel - Instance number
*
* IN/OUT    None
*
* Returns:  prot_status_t
*              The enum value indicating the current status of the handler
******************************************************************************/
prot_status_t PROT_abort_process(uint8_t channel)
{
  prot_instance_t  *p_prot_ins;
  prot_status_t     prot_status;

  if(channel > MAX_NUMBER_OF_CHANNELS || channel == 0)
  {          /* Invalid channel number */
    p_prot_manager->last_status = prot_status_invalid_channel_id;
    prot_status = p_prot_manager->last_status;

#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
    PROT_Err(channel, PROT_ERR_channel_is_outside_range);
#endif

  }
  else
  {
    p_prot_ins = p_prot_manager->prot_handler[channel];
    if(p_prot_ins )
    {     /* The instance exists*/
      p_prot_manager->last_status = prot_status_ok;
      prot_status = p_prot_manager->last_status;
      p_prot_manager->last_error = prot_error_no_error;
      p_prot_ins->state = prot_state_RX_TX_ready;
      p_prot_ins->rx_done = 0;
    }
    else
    {
      p_prot_manager->last_status = prot_status_channel_instance_not_exist;
      prot_status = prot_status_channel_instance_not_exist;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
      PROT_Err(channel, PROT_ERR_invalid_channel_instance);
#endif

    }
  }
  return prot_status;
}

#ifdef INCLUDE_UNUSED_FX
/*******************************************************************************
* Function: prot_error_t PROT_get_last_error(uint8_t channel, char *error);
*------------------------------------------------------------------------------
* Purpose:  This function returns the last error registred in the protocol
*           instance of the given channel.
*
* Parameters:
* IN        channel - Instance number.
*
* IN/OUT
*           error - error value in the string format. The calling function is
*                   responsible for the memory allocation
*
* Returns:  prot_error_t
*              The enum value indicating the current error in the handler
*
******************************************************************************/
prot_error_t PROT_get_last_error(uint8_t channel, char *error)
{
  prot_error_t      prot_error = prot_error_no_error;
  prot_instance_t  *p_prot_ins;

  PARAM_NOT_USED(error);

  if(channel <= MAX_NUMBER_OF_CHANNELS)
  {
    p_prot_ins = p_prot_manager->prot_handler[channel];
    if(p_prot_ins )
    {
      prot_error = p_prot_ins->last_error;
    }
    else
    {
      p_prot_manager->last_status = prot_status_channel_instance_not_exist;
      prot_error = prot_error_channel_instance_not_exist;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
      PROT_Err(channel, PROT_ERR_invalid_channel_instance);
#endif

    }
  }
  else
  { /* Invalid channel number */
    p_prot_manager->last_status = prot_status_invalid_channel_id;
    prot_error = prot_error_invalid_channel_id;

#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
    PROT_Err(channel, PROT_ERR_channel_is_outside_range);
#endif

  }
  return prot_error;
}

/*******************************************************************************
* Function:  prot_status_t PROT_set_last_error(uint8_t channel);
*------------------------------------------------------------------------------
* Purpose:   This function initialize the error registred in the instance
*            to prot_error_no_error;
*
* Parameters:
* IN         channel:   Instance number.
*
* IN/OUT     None
*
* Returns:  prot_status_t
*              The enum value indicating the current status of the handler
*
******************************************************************************/
prot_status_t PROT_set_last_error(uint8_t channel)
{
  prot_status_t    prot_status = prot_status_ok;
  prot_instance_t  *p_prot_ins;

  if(channel <= MAX_NUMBER_OF_CHANNELS)
  {
    p_prot_ins = p_prot_manager->prot_handler[channel];
    if (p_prot_ins )
    {
      p_prot_ins->last_error = prot_error_no_error;
      p_prot_ins->last_status = prot_status_ok;
    }
    else
    {
      p_prot_manager->last_status = prot_status_channel_instance_not_exist;
      prot_status = prot_status_channel_instance_not_exist;
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
      PROT_Err(channel, PROT_ERR_invalid_channel_instance);
#endif

    }
  }
  else
  {                /* Invalid channel number */
    p_prot_manager->last_status = prot_status_invalid_channel_id;
    prot_status = prot_status_invalid_channel_id;

#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
    PROT_Err(channel, PROT_ERR_channel_is_outside_range);
#endif

  }
  return prot_status;
}
#endif

/******************************************************************************
* Function:... static void PROT_Err(uint8_t channel, uint32_t err)
*
* Parameters:. I :
* Returns:....
* Description: Simple wrapper for the SFH_set_error_string() function to
*              reduce code usage and increase readability
*
* Created:.... 05.04.06 by (Baskara AR Reddy, DWD / INF_BR)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
void PROT_Err(uint8_t channel, uint32_t err)
{
  if ((0 < channel) && (channel <= MAX_NUMBER_OF_CHANNELS))
    SFH_set_error_string(channel, GLOBAL_format_text(IFWD_DL_text_class_ifx_error_code, (uint16_t)err, channel));
}
#endif

/******************************************************************************
* Function:... static void PROT_info(uint8_t channel, uint32_t err)
*
* Parameters:. I :
* Returns:....
* Description: Simple wrapper for the GLOBAL_callback() function to
*              reduce code usage and increase readability
*
* Created:.... 05.04.06 by (Baskara AR Reddy, DWD / INF_BR)
* Modified:... DD.MM.YY by (Full name / initials)
*              Modifications note.
******************************************************************************/
#ifdef INCLUDE_UNUSED_FX
#if defined (USE_TEXT_CLASS_IFWD_INFO)
void PROT_info(uint8_t channel, uint32_t info)
{
  GLOBAL_callback(channel, IFWD_DL_ProcessDetailUpdated, GLOBAL_format_text(IFWD_DL_text_class_process_info, (uint16_t)info));
}
#endif
#endif


/**************************** Local Functions *********************************/

/*******************************************************************************
* Function: void State_machine(prot_instance_t *p_prot_ins);
*------------------------------------------------------------------------------
* Purpose: Internal clock of the handler to update the status of the TX or RX
*
* Parameters:
* IN        p_prot_ins  - current instance of the protocol
*
* IN/OUT    None
*
* Returns:  Nothing
******************************************************************************/
static void State_machine(prot_instance_t *p_prot_ins)
{
  static prot_state_t last_state;
  if (last_state != p_prot_ins->state)
  {
    Debug3("PROT state: %d -> %d\n", last_state, p_prot_ins->state);
    last_state = p_prot_ins->state;
  }

  switch(p_prot_ins->state)
  {
  case prot_state_idle:
    break;
  case prot_state_RX_TX_ready:
    break;
  case prot_state_transmit_cmd:
    if(TIMER_timer_expired(&p_prot_ins->tx_pack_timer))
    {
      p_prot_ins->state = prot_state_error;
      p_prot_ins->last_status = prot_status_tx_pack_timeout;
      p_prot_ins->last_error = prot_error_tx_pack_error;
      if(p_prot_ins->fx_tx_pack)
      {
        p_prot_ins->fx_tx_pack(p_prot_ins->channel_id,false);
        p_prot_ins->fx_tx_pack = NULL;
      }
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
      PROT_Err(p_prot_ins->channel_id, PROT_ERR_rx_pack_timeout);
#endif
    }
    break;
  case prot_state_rx_pack_header:
    if(TIMER_timer_expired(&p_prot_ins->rx_timer))
    {
      p_prot_ins->state = prot_state_error;
      p_prot_ins->last_status = prot_status_rx_pack_timeout;

      p_prot_ins->last_error = prot_error_rx_pack_error;
      if(p_prot_ins->fx_rx)
      {
        p_prot_ins->fx_rx(p_prot_ins->channel_id,false);
        p_prot_ins->fx_rx = NULL;
        p_prot_ins->rx_done = 0;
      }
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
      PROT_Err(p_prot_ins->channel_id, PROT_ERR_rx_pack_timeout);
#endif
    }
    break;
  case prot_state_rx_pack_payload:
    if(p_prot_ins->rx_done)
    {
      if(TIMER_timer_expired(&p_prot_ins->rx_timer))
      {
        p_prot_ins->state = prot_state_error;
        p_prot_ins->last_status = prot_status_rx_pack_timeout;

        p_prot_ins->last_error = prot_error_rx_pack_error;
        if(p_prot_ins->fx_rx)
        {
          p_prot_ins->fx_rx(p_prot_ins->channel_id,false);
          p_prot_ins->fx_rx = NULL;
          p_prot_ins->rx_done = 0;
        }
#if defined (USE_TEXT_CLASS_IFWD_ERRORS)
        PROT_Err(p_prot_ins->channel_id, PROT_ERR_rx_pack_timeout);
#endif
      }
    }
    else
    {
      Receive_payload(p_prot_ins);
    }
    break;
  case prot_state_error:
    if(p_prot_ins->fx_tx_pack)
    {
      p_prot_ins->fx_tx_pack(p_prot_ins->channel_id,false);
      p_prot_ins->fx_tx_pack = NULL;
    }
    else if(p_prot_ins->fx_rx)
    {
      p_prot_ins->fx_rx(p_prot_ins->channel_id,false);
      p_prot_ins->fx_rx = NULL;
      p_prot_ins->rx_done = 0;
    }
    break;
  default:
    break;
  }
}

/*******************************************************************************
* Function: void Rx_pack_notify(uint8_t id, uint32_t length);
*------------------------------------------------------------------------------
* Purpose:  Callback function called when the lower layer receives data
*
* Parameters:
* IN        id     - Instance number
*           length - Length of the data received
*
* IN/OUT    None
*
* Returns:  Nothing
******************************************************************************/
void Rx_pack_notify(uint8_t id, uint32_t length)
{
  prot_instance_t *p_prot_ins;
  comm_error_t     comm_error;
  p_prot_ins = p_prot_manager->prot_handler[id];

  if(TIMER_timer_expired(&p_prot_ins->rx_timer))
  {
    p_prot_ins->last_status = prot_status_rx_pack_timeout;

    p_prot_ins->last_error  = prot_error_rx_pack_timeout;
    p_prot_ins->state = prot_state_error;
    p_prot_ins->rx_done = 0;
  }
  else
  {
    if(length)
    {
      if(p_prot_ins->rx_done)
      {
        p_prot_ins->last_status = prot_status_rx_pack_ready;
        p_prot_ins->rx_done = 0;

        if (COMM_TECH_IS_MIPI_ADD_HEADER(p_prot_ins->channel_id)
           && p_prot_ins->UseProtocolVer != PROT_1_16)
        {
          memmove((void*) p_prot_ins->g_rx_prot_pack.v_3_32->data,
                  (const void*)(p_prot_ins->g_rx_prot_pack.v_3_32->data+4), length-4);
        }

        Verify_rx_pack(p_prot_ins);
      }
      else
      {
        Create_rx_pack_hdr(p_prot_ins,
                           &p_prot_ins->g_rx_prot_pack,
                           p_prot_ins->rx_hdr);

        p_prot_ins->last_status = prot_status_rx_pack_ongoing;
        p_prot_ins->state = prot_state_rx_pack_payload;
        /* Receive_payload(p_prot_ins); */
      }
    }
    else
    {  /* An error has occurred in the read transaction */
      p_prot_ins->last_status = prot_status_error;
      p_prot_ins->state = prot_state_error;
      p_prot_ins->rx_done = 0;
      comm_error = COMM_get_last_error(p_prot_ins->channel_id);
      switch (comm_error)
      {
      case comm_error_read_error:
      case comm_error_serial_not_open:
        p_prot_ins->last_error = prot_error_rx_pack_error;
        break;
      case comm_error_unkown:
        p_prot_ins->last_error = prot_error_unknown;
        break;
      default:
        break;
      }
      if(p_prot_ins->fx_rx)
      {
        p_prot_ins->fx_rx(id,false);
        p_prot_ins->fx_rx = NULL;
      }
    }
  }
}
/*******************************************************************************
* Function: void Tx_pack_notify(uint8_t channel);
*------------------------------------------------------------------------------
* Purpose:  Callback function called when the lower layer finishes transmitting
*           the packet
*
* Parameters:
* IN        channel - Instance number
*
* IN/OUT    None
*
* Returns:  Nothing
******************************************************************************/
void Tx_pack_notify(uint8_t channel)
{
  prot_instance_t *p_prot_ins;

  p_prot_ins = p_prot_manager->prot_handler[channel];

  if(TIMER_timer_expired(&p_prot_ins->tx_pack_timer))
  {
    p_prot_ins->state = prot_state_error;
    p_prot_ins->last_status = prot_status_tx_pack_timeout;
  }
  else
  {
    Trace_tx_data(channel,
                  p_prot_ins->UseProtocolVer,
                  p_prot_ins->g_tx_prot_pack.start_addr,
                  p_prot_ins->g_tx_prot_pack.len);

    p_prot_ins->state = prot_state_RX_TX_ready;
    if(p_prot_ins->fx_tx_pack)
    {
      p_prot_ins->fx_tx_pack(channel,true);
      p_prot_ins->fx_tx_pack = NULL;

      if (p_prot_ins->last_tx_pack_type == ReqFlashWriteBlock
       && target_capabilities[channel].skip_wr_pack_cnf
       && p_prot_ins->UseProtocolVer >= PROT_2_32)
      {
        /* Fake the response of the write block package */
        p_prot_ins->rx_hdr->type = ReqFlashWriteBlock;
        p_prot_ins->rx_hdr->len = 2;
        p_prot_ins->rx_payload->data[0] = 0x00;
        p_prot_ins->rx_payload->data[1] = 0x00;
        if (p_prot_ins->fx_rx)
        {
          p_prot_ins->fx_rx(channel,true);
          p_prot_ins->fx_rx = NULL;
        }
        p_prot_ins->last_status = prot_status_rx_pack_ready;
        p_prot_ins->state = prot_state_RX_TX_ready;

      }
    }
  }
}

/*******************************************************************************
* Function: uint32_t Prot_pack_header_size(uint32_t prot_version)
*------------------------------------------------------------------------------
* Purpose: This function return the size of the protocol pack payload
*
* Parameters:
* IN      prot_version  -  Protocol type
*
* OUT    None
*
* Returns:  Protocol pack payload size
******************************************************************************/
static uint32_t Prot_pack_header_size(uint32_t prot_version)
{
  uint32_t len;

  switch(prot_version)
  {
    case PROT_1_16:
      len = PROT_PACK_1_16_HEAD_LENGTH;
      break;

    case PROT_1_32:
      len = PROT_PACK_1_32_HEAD_LENGTH;
      break;

    case PROT_2_32:
    case PROT_3_32:
      len = PROT_PACK_3_32_HEAD_LENGTH;
      break;

    default:
      len = 0;
      break;
  }
  return len;

}


/*******************************************************************************
* Function: uint32_t Prot_pack_payload_size (uint8_t channel,
                                        uint32_t prot_version,
*                                       g_prot_pack_t   *p_block)
*------------------------------------------------------------------------------
* Purpose: This function return the size of the protocol pack payload
*
* Parameters:
* IN      prot_version  -  Protocol type
*         p_block       - memory block
*
* OUT    None
*
* Returns:  Protocol pack payload size
******************************************************************************/
static uint32_t Prot_pack_payload_size(uint8_t channel,
                                  uint32_t prot_version,
                                  g_prot_pack_t   *p_block)
{
  uint32_t len;

  switch(prot_version)
  {
    case PROT_1_16:
      p_block->v_1_16 = (prot_pack_1_16_t *)p_block->start_addr;
      len = (uint32_t)p_block->v_1_16->len + PROT_PACK_1_16_TAIL_LENGTH;
      break;

    case PROT_1_32:
      p_block->v_1_32 = (prot_pack_1_32_t *)p_block->start_addr;
      len = (uint32_t)p_block->v_1_32->len + PROT_PACK_1_32_TAIL_LENGTH;
      break;

    case PROT_2_32:
    case PROT_3_32:
      len = GLOBAL_use_pack_size[channel];
      break;

    default:
      len = 0;
      break;
  }

  return (uint32_t)len;

}


/*******************************************************************************
* Function: void Receive_payload(prot_instance_t *p_prot_ins);
*------------------------------------------------------------------------------
* Purpose: This function is called to receive the payload part of the package
*
* Parameters:
* IN        None
*
* IN/OUT    p_prot_ins - Channel Instance
*
* Returns:  Nothing
******************************************************************************/
static void Receive_payload(prot_instance_t *p_prot_ins)
{
  comm_status_t comm_status;
  uint8_t            *p_temp_rx_buf_ptr;
  uint32_t           temp_rx_len;

  uint32_t bytes_read = 0;

  p_prot_ins->state = prot_state_rx_pack_payload;
  p_prot_ins->last_status = prot_status_rx_pack_ongoing;

  p_prot_ins->rx_done = 1;

  TIMER_new_timer(&p_prot_ins->rx_timer, TRA_GetCommTimeOut(p_prot_ins->channel_id));

  p_prot_ins->g_rx_prot_pack.len = Prot_pack_payload_size(p_prot_ins->channel_id,
                                                           p_prot_ins->UseProtocolVer,
                                                          &p_prot_ins->g_rx_prot_pack);

  p_temp_rx_buf_ptr = p_prot_ins->rx_payload->data;
  temp_rx_len = p_prot_ins->g_rx_prot_pack.len;

  // Handle alignment if needed.
  p_temp_rx_buf_ptr += p_prot_ins->align_read_ahead; // Advance the pointer past the data read along with the header
  temp_rx_len       -= p_prot_ins->align_read_ahead; // Adjust the length accordingly.
  p_prot_ins->align_read_ahead = 0;
  temp_rx_len += temp_rx_len % GLOBAL_comm_align_bytes[p_prot_ins->channel_id]; // Align the length of the rest of the payload data if needed

  if (COMM_TECH_IS_MIPI_ADD_HEADER(p_prot_ins->channel_id)
     && p_prot_ins->UseProtocolVer != PROT_1_16)
  {
    Debug("Adding MIPI header to expected payload (%d+4) \n", temp_rx_len);
    temp_rx_len += sizeof(uint32_t);
  }

  comm_status = (comm_status_t)COMM_receive_buffer(p_prot_ins->channel_id,
                                                   (char*)p_temp_rx_buf_ptr,
                                                   temp_rx_len,
                                                   &bytes_read,
                                                   Rx_pack_notify);

  switch(comm_status)
  {
    case comm_status_read_finished:
      p_prot_ins->last_status = prot_status_rx_pack_ready;
      Verify_rx_pack(p_prot_ins);
      p_prot_ins->rx_done = 0;
      break;
    case comm_status_read_ongoing:
      p_prot_ins->last_status = prot_status_rx_pack_ongoing;
      break;
    default:
      p_prot_ins->last_status = prot_status_error;
      p_prot_ins->last_error  = prot_error_rx_pack_error;
      p_prot_ins->state       = prot_state_error;
      p_prot_ins->rx_done = 0;
      break;
    }
}

/*******************************************************************************
* Function: void Verify_rx_pack(prot_instance_t *p_prot_ins);
*------------------------------------------------------------------------------
* Purpose: Validates the received packet by calculating the checksum and
*          comparing it with the checksum value in the received packet
*
* Parameters:
* IN        None
*
* IN/OUT    p_prot_ins - Channel Instance
*
* Returns:  Nothing
******************************************************************************/
static void Verify_rx_pack(prot_instance_t *p_prot_ins)
{
  if(p_prot_ins->rx_hdr->type == IndErrorMsg)
  {
    /* Decode error pack */
    target_error_decode_pack_ebl2(p_prot_ins->rx_payload->data, p_prot_ins->channel_id);
    p_prot_ins->last_status = prot_status_error;

    p_prot_ins->last_error = prot_error_rx_pack_error;

    p_prot_ins->state = prot_state_error;

    /* Inform upper layers about the error state   */

    if(p_prot_ins->fx_rx )
    {
      p_prot_ins->fx_rx(p_prot_ins->channel_id,false);
      p_prot_ins->fx_rx = NULL;
    }
  }
  else
  if(p_prot_ins->rx_hdr->type == RspChecksumError)
  {
    p_prot_ins->last_status = prot_status_error;

    p_prot_ins->last_error = prot_error_rx_pack_error;

    p_prot_ins->state = prot_state_error;

    /* Inform upper layers about the error state   */

    if(p_prot_ins->fx_rx )
    {
      p_prot_ins->fx_rx(p_prot_ins->channel_id,false);
      p_prot_ins->fx_rx = NULL;
    }
  }
  else
  if(p_prot_ins->rx_hdr->type == RspRxTimeoutError)
  {
    p_prot_ins->last_status = prot_status_error;

    p_prot_ins->last_error = prot_error_rx_pack_error;

    p_prot_ins->state = prot_state_error;

    /* Inform upper layers about the error state   */

    if(p_prot_ins->fx_rx )
    {
      p_prot_ins->fx_rx(p_prot_ins->channel_id,false);
      p_prot_ins->fx_rx = NULL;
    }
  }
  else
  if(p_prot_ins->rx_hdr->type == ReqRetransmission)
  {
    p_prot_ins->last_status = prot_status_error;

    p_prot_ins->last_error = prot_error_rx_pack_error;

    p_prot_ins->state = prot_state_error;

    /* Inform upper layers about the error state   */
    /* Retransmit the last sent packet to the target */

    if(p_prot_ins->fx_rx )
    {
      p_prot_ins->fx_rx(p_prot_ins->channel_id,false);
      p_prot_ins->fx_rx = NULL;
    }
  }
  else
  {
    if (dll_capabilities.protocol_crc  != CAP_PROT_NO_CRC)
    {
      p_prot_ins->g_rx_prot_pack.checksum = Calc_chksum_rx_pack(p_prot_ins->rx_hdr,
                                                                p_prot_ins->rx_payload->data);
    }
    if((p_prot_ins->g_rx_prot_pack.checksum != p_prot_ins->rx_hdr->checksum) &&
        (dll_capabilities.protocol_crc  != CAP_PROT_NO_CRC))
    {
      p_prot_ins->last_status = prot_status_error;

      p_prot_ins->last_error = prot_error_rx_pack_error;

      p_prot_ins->state = prot_state_error;

      /* Inform upper layers about the error state   */

      if(p_prot_ins->fx_rx )
      {
        p_prot_ins->fx_rx(p_prot_ins->channel_id,false);
        p_prot_ins->fx_rx = NULL;
      }
    }
    else
    {
      p_prot_ins->last_status = prot_status_rx_pack_ready;
      p_prot_ins->state = prot_state_RX_TX_ready;

      /* Inform upper layers about the successful receipt of data */

      if(p_prot_ins->fx_rx)
      {
        p_prot_ins->fx_rx(p_prot_ins->channel_id,true);
        if ((p_prot_ins->last_tx_pack_type != ReqFlashWriteBlock) || !target_capabilities[p_prot_ins->channel_id].skip_wr_pack_cnf)
        {
          p_prot_ins->fx_rx = NULL;
        }
      }
    }
  }
  // Trace moved here from top.
  Trace_rx_data(p_prot_ins->channel_id,
                p_prot_ins->UseProtocolVer,
                &p_prot_ins->g_rx_prot_pack);
}

/*******************************************************************************
* Function: void Trace_tx_data(uint32_t prot_version)
*------------------------------------------------------------------------------
* Purpose:
*
* Parameters:
* IN        None
*
* IN/OUT    p_prot_ins - Channel Instance
*
* Returns:  Nothing
******************************************************************************/
static void Trace_tx_data(uint8_t channel, uint32_t prot_version, uint8_t *p_data, uint32_t len)
{
  if(!GLOBAL_trace_verbosity)
    return;

  switch (prot_version)
  {
    case PROT_1_16:
      TR_BEGIN(TR_Tx, 0, channel);
      TR_DATA(p_data, len);
      TR_END;

      break;

    case PROT_1_32:
//      TR_BEGIN(TR_Tx, TR_PROT_1_32, channel);
      TR_BEGIN(TR_Tx, 1, channel);
      TR_DATA(p_data, len);
      TR_END;

      break;


    case PROT_2_32:
    case PROT_3_32:
      TR_BEGIN(TR_Tx, 1, channel);
      TR_DATA(p_data, len);
      TR_END;

      break;

    default:
      break;
  }

}

/*******************************************************************************
* Function: void Trace_rx_data(uint32_t prot_version)
*------------------------------------------------------------------------------
* Purpose:
*
* Parameters:
* IN        None
*
* IN/OUT    p_prot_ins - Channel Instance
*
* Returns:  Nothing
******************************************************************************/
static void Trace_rx_data(uint8_t channel, uint32_t prot_version, g_prot_pack_t *p_pack)
{
  if(!GLOBAL_trace_verbosity)
    return;

  switch (prot_version)
  {
    case PROT_1_16:
      if (p_pack->v_1_16)
      {
        TR_BEGIN(TR_Rx, 0, channel);
        TR_DATA((uint8_t *)&p_pack->v_1_16->sot, p_pack->v_1_16->len + PROT_PACK_1_16_HEAD_TAIL_LENGTH);
        TR_END;
      } else {
        Debug("Trying to log Rx data, which is not initialized.\n");
      }
      break;

    case PROT_1_32:
      if (p_pack->v_1_32)
      {
        TR_BEGIN(TR_Rx, 1, channel);
        TR_DATA((uint8_t *)&p_pack->v_1_32->sot , p_pack->v_1_32->len + PROT_PACK_1_32_HEAD_TAIL_LENGTH);
        TR_END;
      } else {
        Debug("Trying to log Rx data, which is not initialized.\n");
      }

      break;

    case PROT_2_32:
    case PROT_3_32:
      if (p_pack->v_3_32)
      {
        TR_BEGIN(TR_Rx, 1, channel);
        TR_DATA((uint8_t *)&p_pack->v_3_32->checksum , p_pack->v_3_32->len + PROT_PACK_3_32_HEAD_TAIL_LENGTH);
        TR_END;
      } else {
        Debug("Trying to log Rx data, which is not initialized.\n");
      }
      break;

    default:
      break;
  }
}

