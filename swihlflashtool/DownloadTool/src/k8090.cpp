/*  -------------------------------------------------------------------------
    Copyright (C) 2011-2014 Intel Mobile Communications GmbH

         Sec Class: Intel Confidential (IC)

    All rights reserved.
    -------------------------------------------------------------------------
    This document contains proprietary information belonging to IMC.
    Passing on and copying of this document, use and communication of its
    contents is not permitted without prior written authorization.
    -------------------------------------------------------------------------
    Revision Information:
       $File name:  /msw_tools/FlashTool/DownloadTool/src/k8090.cpp $
       $CC-Version: .../dhkristx_140314_v0/1 $
       $Date:       2014-04-10    14:57:11 UTC $
    -------------------------------------------------------------------------
	Description:
       A k8090 Controller

    Packet Structure:
       (8 bits)  [ STX | CMD | MASK | PARAM1 | PARAM2 | CHK | ETX ]
       Checksum: -(STX + CMD + MASK + PARAM1 + PARAM2)
    ------------------------------------------------------------------------- */

#include "targetver.h"
#include "k8090.h"
#include "Utils.h"

#include <iostream>

using namespace std;


// == Recv Callback ==
void k8090::onRecv(const char *data, size_t len)
{
	if (len == 7 && data[0] == STX && data[6] == ETX)
	{
	   char CRC = -(data[0] + data[1] + data[2] + data[3] + data[4]);
	   if (CRC != data[5])
	     cerr << "Relay CRC Error!" << endl;

	   // Only react to status changes
	  if (data[1] != EVENT_RELAY_STATUS) return;

    char prevState = data[2];
    char curState = data[3];

    char on  = (prevState ^ curState) & curState;
    char off = (prevState ^ curState) & prevState;

    for(int i=0; i<8; ++i)
    {
      char mask = 0x01 << (i-1);

      if (on & mask)
      {
        if (m_verbose)
          cout << "Relay " << i << " On" << endl;
        relayState[i] = true;
      }
      else if (off & mask)
      {
        if (m_verbose)
          cout << "Relay " << i << " Off" << endl;
        relayState[i] = false;
      }
    }
	}
	else
	{
     if (len > 0 && m_verbose)
       cout << endl << "k8090> Hex(" << data2hex(data, len) << ")" << endl;
	}

#if 0
	 cout << "   [";
    for(unsigned int i=0;i<v.size();i++)
    {
        if(v[i]=='\n')
        {
            cout<<endl;
        } else {
            if(v[i]<32 || v[i]>=0x7f) cout.put(' ');//Remove non-ascii char
            else cout.put(v[i]);
        }
    }
 	 cout << "]" << endl;
	cout.flush();
#endif
}


// == The K8090 Class (19200) ==

k8090::k8090(string dev)
  : SerialPort(dev.c_str(),19200),
	  dev( dev ), m_verbose(false)
{
  setReadCallback( MemFun(&k8090::onRecv, this));
}

k8090::~k8090()
{
  //cout << "~k8090()" << endl;
}


void k8090::setVerbose(int verbose)
{
  if (verbose <= 0)
    m_verbose = 0;
  else
    m_verbose = verbose;
}

void k8090::toggle(int relay)
{
	char mask = 0x01 << (relay-1);

	Command cmd(CMD_TOGGLE, mask);
	write( cmd.data(), cmd.size() );
}

void k8090::setOn(int relay)
{
  char mask = 0x01 << (relay-1);

  Command cmd(CMD_ON, mask);
  write( cmd.data(), cmd.size() );
}

void k8090::setOff(int relay)
{
  char mask = 0x01 << (relay-1);

  Command cmd(CMD_OFF, mask);
  write( cmd.data(), cmd.size() );
}

void k8090::reset(int relay, int Zzz)
{
  setOn( relay );
  Sleep( Zzz );
  setOff( relay );
}

void k8090::reboot(int relay, int Zzz)
{
  setOff( relay );
  Sleep( Zzz );
  setOn( relay );
}
