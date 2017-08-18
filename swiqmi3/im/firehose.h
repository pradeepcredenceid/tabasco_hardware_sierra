/*************
 *
 * Filename:    firehose.h
 *
 * Purpose:     firehose private constants and structures
 *
 * Copyright:  2017 Sierra Wireless Inc., all rights reserved
 *
 **************/

#ifndef FIREHOSE_H
#define FIREHOSE_H

/* Includes */
#include "aa/aaglobal.h"
#include "qm/qmerrno.h"
#include "dl/dludefs.h"
#include "im/imidefs.h"


#define SAHARA_HELLO_REQ_PKT_LENGTH               0x00000030
#define SAHARA_COMMAND_READY_PKT_LENGTH           0x00000008
#define SAHARA_COMMAND_EXECUTE_RSP_PKT_LENGTH     0x00000010


#define XML_HEADER_LENGTH 41    // 40 does NOT include null
#define XML_TAIL_LENGTH   9     // 9 DOES include null
#define FIREHOSE_TX_BUFFER_SIZE 131072 //128kB


/*************
 *
 * Name:    fheventtypes
 *
 * Purpose: Enumerated list of event block types. These are the types
 *          that get written into the QMI Event Block whenever an
 *          inter-task message is to be sent to the IM Task.
 *
 * Members:
 *
 * Notes:   Mapped to SSDP responses where relevant
 *
 **************/
enum fheventtype {
    e_MIN,
    e_SAHARA_RESP,
    e_FIREHOSE_RESP,
    e_TMRTICK,
    e_TERMINATE,
    e_MAX
};

/*************
 *
 * Name:    fhsteventtype
 *
 * Purpose: Enumerated list of event block types. These are the types
 *          that get written into the QMI Event Block whenever an
 *          inter-task message is to be sent to the FHTask.
 *
 * Members:
 *
 * Notes:   Mapped to Sahara commands and Firehose commands
 *
 **************/
enum fhsteventtype {
    eSA_MIN,
    eSA_HELLO = eSA_MIN,
    eSA_COMMAND_READY,
    eSA_COMMAND_EXE_RSP,
    eSA_SWITCH_CONFIRMED_RSP,
    eFH_INFO_RSP,
    eFH_CONFIGURE_RSP,
    eFH_PROGRAM_RSP,
    eFH_PROCESS_RSP,
    eFH_RESET_RSP,
    eFH_ERROR_RSP,
    eFH_MAX_EVENT
};


/*************
 *
 * Name:     fhstate_t
 *
 * Purpose:  Enumerated states, managed by state machine
 *
 * Members:  stZERO     -
 *           stONE      -
 *           stTWO      -
 *           stMAX      - Number of states
 *
 * Notes:    STATES MUST BE ENUMERATED FROM 0 (zero)
 *
 **************/
enum fhstate_t
{
    stSA_IDLE,
    stSA_HELLO_RSP,
    stSA_COM_EXE,
    stSA_COM_EXE_DATA,
    stFH_INFO,
    stFH_CONFIGURE,
    stFH_PROGRAM,
    stFH_RAW_DATA_TRF,
    stFH_RESET,
    stFH_DONE_FINAL,
    stFH_NUM_ACTIVE_STATES, /* used for table sizing */
    stFH_IGNORE,
    stFH_INVAL,
    stFH_MAX = stFH_INVAL
};

/*---------------
  ENUM
 ---------------*/
 
typedef enum
{
    HELLO_REQUEST = 0x01,
    HELLO_RESPONSE,
    READ_DATA,
    END_OF_IMAGE_TRASFER,
    DONE,
    DONE_RESPONSE,
    SAHARA_RESET,
    RESET_RESPONSE,
    MEMORY_DEBUG,
    MEMORY_READ,
    COMMAND_READY,
    COMMAND_SWITCH_MODE,
    COMMAND_EXECUTE,
    COMMAND_EXECUTE_RESPONSE,
    COMMAND_EXECUTE_DATA
} e_sahara_commands;

// Status codes for Sahara
typedef enum
{
    // Success
    SAHARA_STATUS_SUCCESS =                     0x00,

    // Invalid command received in current state
    SAHARA_NAK_INVALID_CMD =                    0x01,

    // Protocol mismatch between host and target
    SAHARA_NAK_PROTOCOL_MISMATCH =              0x02,

    // Invalid target protocol version
    SAHARA_NAK_INVALID_TARGET_PROTOCOL =        0x03,

    // Invalid host protocol version
    SAHARA_NAK_INVALID_HOST_PROTOCOL =          0x04,

    // Invalid packet size received
    SAHARA_NAK_INVALID_PACKET_SIZE =            0x05,

    // Unexpected image ID received
    SAHARA_NAK_UNEXPECTED_IMAGE_ID =            0x06,

    // Invalid image header size received
    SAHARA_NAK_INVALID_HEADER_SIZE =            0x07,

    // Invalid image data size received
    SAHARA_NAK_INVALID_DATA_SIZE =              0x08,

    // Invalid image type received
    SAHARA_NAK_INVALID_IMAGE_TYPE =             0x09,

    // Invalid tranmission length
    SAHARA_NAK_INVALID_TX_LENGTH =              0x0A,

    // Invalid reception length
    SAHARA_NAK_INVALID_RX_LENGTH =              0x0B,

    // General transmission or reception error
    SAHARA_NAK_GENERAL_TX_RX_ERROR =            0x0C,

    // Error while transmitting READ_DATA packet
    SAHARA_NAK_READ_DATA_ERROR =                0x0D,

    // Cannot receive specified number of program headers
    SAHARA_NAK_UNSUPPORTED_NUM_PHDRS =          0x0E,

    // Invalid data length received for program headers
    SAHARA_NAK_INVALID_PDHR_SIZE =              0x0F,

    // Multiple shared segments found in ELF image
    SAHARA_NAK_MULTIPLE_SHARED_SEG =            0x10,

    // Uninitialized program header location
    SAHARA_NAK_UNINIT_PHDR_LOC =                0x11,

    // Invalid destination address
    SAHARA_NAK_INVALID_DEST_ADDR =              0x12,

    // Invalid data size receieved in image header
    SAHARA_NAK_INVALID_IMG_HDR_DATA_SIZE =      0x13,

    // Invalid ELF header received
    SAHARA_NAK_INVALID_ELF_HDR =                0x14,

    // Unknown host error received in HELLO_RESP
    SAHARA_NAK_UNKNOWN_HOST_ERROR =             0x15,

    // Timeout while receiving data
    SAHARA_NAK_TIMEOUT_RX =                     0x16,

    // Timeout while transmitting data
    SAHARA_NAK_TIMEOUT_TX =                     0x17,

    // Invalid mode received from host
    SAHARA_NAK_INVALID_HOST_MODE =              0x18,

    // Invalid memory read access
    SAHARA_NAK_INVALID_MEMORY_READ =            0x19,

    // Host cannot handle read data size requested
    SAHARA_NAK_INVALID_DATA_SIZE_REQUEST =      0x1A,

    // Memory debug not supported
    SAHARA_NAK_MEMORY_DEBUG_NOT_SUPPORTED =     0x1B,

    // Invalid mode switch
    SAHARA_NAK_INVALID_MODE_SWITCH =            0x1C,

    // Failed to execute command
    SAHARA_NAK_CMD_EXEC_FAILURE =               0x1D,

    // Invalid parameter passed to command execution
    SAHARA_NAK_EXEC_CMD_INVALID_PARAM =         0x1E,

    // Unsupported client command received
    SAHARA_NAK_EXEC_CMD_UNSUPPORTED =           0x1F,

    // Invalid client command received for data response
    SAHARA_NAK_EXEC_DATA_INVALID_CLIENT_CMD =   0x20,

    // Failed to authenticate hash table
    SAHARA_NAK_HASH_TABLE_AUTH_FAILURE =        0x21,

    // Failed to verify hash for a given segment of ELF image
    SAHARA_NAK_HASH_VERIFICATION_FAILURE =      0x22,

    // Failed to find hash table in ELF image
    SAHARA_NAK_HASH_TABLE_NOT_FOUND =           0x23,

    // Place all new error codes above this
    SAHARA_NAK_LAST_CODE,

    SAHARA_NAK_MAX_CODE = 0x7FFFFFFF // To ensure 32-bits wide
} e_boot_sahara_status;


#endif /* FIREHOSE_H */

