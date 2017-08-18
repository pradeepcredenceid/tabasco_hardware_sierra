/*   ----------------------------------------------------------------------
     Copyright (C) 2011-2014 Intel Mobile Communications GmbH

          Sec Class: Intel Confidential (IC)

     ----------------------------------------------------------------------
     Revision Information:
       $$File name:  /msw_tools/FlashTool/DownloadTool/src/k8090.h $
       $$CC-Version: .../dhkristx_140314_v0/1 $
       $$Date:       2014-04-10    14:57:15 UTC $
     ----------------------------------------------------------------------
     by CCCT (v0.4)
     ---------------------------------------------------------------------- */

/*  -------------------------------------------------------------------------
	Description:
	   A k8090 Controller
    ------------------------------------------------------------------------- */

#ifndef _K8090_
#define _K8090_

#include "SerialPort.h"
#include "MemberCallback.h"

#include <memory>
#include <iostream>

#ifdef __linux__
  #define DEFAULT_RELAY_PORT "/dev/ttyACM0"
#elif __APPLE__
  #define DEFAULT_RELAY_PORT "/dev/cu.usbmodem"
#else
  #define DEFAULT_RELAY_PORT "\\\\.\\COM10"
#endif

// Timeouts between toggles in milliseconds
#define BOOT_TIME      3000       //< The time to wait until the device have booted
#define REBOOT_TIME    3000       //< The time a device should be turned off in order to discharge
#define RESET_TIME     200        //< The time the reset button should be held

#define STX                   0x04
#define ETX                   0x0F

#define PACKET_STX            0x01
#define PACKET_CMD            0x02
#define PACKET_MASK           0x03
#define PACKET_PARAM1         0x04
#define PACKET_PARAM2         0x05
#define PACKET_ETX            0x06

#define CMD_ON                0x11
#define CMD_OFF               0x12
#define CMD_TOGGLE            0x14

#define QUERY_STATUS          0x18
#define QUERY_TIMER_DELAY     0x44
#define QUERY_BUTTON_MODE     0x22

#define EVENT_BUTTON_STATUS   0x50
#define EVENT_RELAY_STATUS    0x51


#define CMD_SET_BUTTON_MODE   0x21  // Param1-EnableToggleMode, Param2-EnableTimedMode
#define CMD_START_TIMER       0x41  // Param1-HighByte, Param2-LowByte (Delay of start in seconds)
#define CMD_SET_TIMER         0x42  // Setting the Timer delay


// == K8090 Commands API ==
//typedef std::auto_ptr< Command > CommandPtr;

class Command
{
public:
  Command(char command, char mask, char param1 = 0, char param2 = 0)
  {
    char CRC = -(STX + command + mask + param1 +param2);
    cmd.push_back(STX);
    cmd.push_back(command);
    cmd.push_back(mask);
    cmd.push_back(param1);
    cmd.push_back(param2);
    cmd.push_back(CRC);
    cmd.push_back(ETX);
  }

  size_t size()       { return cmd.size(); }
  const char* data() { return &cmd[0]; }

private:
  std::vector<char> cmd;
};

class k8090 : public SerialPort
{
public:
  k8090(std::string dev = DEFAULT_RELAY_PORT);

  virtual ~k8090();

  void setVerbose(int verbose);

  /**
   * Connect to a given k8090 USB device
   *
   * @param dev = A string representing the device: e.g. /dev/ttyACM, COM19, /dev/cu.modem
   * @return True if the device was successfully opened
   */
  bool connect(std::string dev);
  void disconnect();

  /**
   * Three ways of manipulating the relays:
   *   toggle: Just a toggle of the relay, ending in its initial state
   *   Reset:  Turns the button On and then OFF
   *   Reboot: Turns the button OFF and then ON
   *
   * @param relay = The relay button to use
   */
  void reset(int relay, int Zzz = RESET_TIME);
  void reboot(int relay, int Zzz = REBOOT_TIME);

  void toggle(int relay);
  void setOff(int relay);
  void setOn(int relay);

private:
  //Command cmd;
  std::string dev;
  int m_verbose;

  void onRecv(const char *data, size_t len);
  bool relayState[8];
};


#endif // _K8090_

