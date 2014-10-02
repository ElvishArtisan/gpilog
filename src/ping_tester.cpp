// ping_tester.cpp
//
// Monitor an IP Address
//
//   (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License version 2 as
//   published by the Free Software Foundation.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public
//   License along with this program; if not, write to the Free Software
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#include <stdlib.h>

#include "ping_tester.h"

PingTester::PingTester(GPIConfig *config,unsigned n,IPv4Pinger *pinger,
		       QObject *parent)
  : QObject(parent)
{
  ping_config=config;
  ping_number=n;

  //
  // Pinger
  //
  ping_pinger=pinger;
  connect(pinger,
	  SIGNAL(echoReceived(const QHostAddress &,uint16_t,uint16_t,
			      const char *,int)),
	  this,
	  SLOT(echoReceivedData(const QHostAddress &,uint16_t,uint16_t,
				const char *,int))); 
  ping_state=true;

  //
  // Reply Slots
  //
  for(unsigned i=0;i<config->ipv4MonitorPings(n);i++) {
    ping_replies.push_back(true);
  }

  //
  // Timer
  //
  ping_timer=new QTimer(this);
  connect(ping_timer,SIGNAL(timeout()),this,SLOT(timeoutData()));
  ping_istate=ping_replies.size();
  ping_timer->start(ping_config->ipv4MonitorInterval(n),true);
}


PingTester::~PingTester()
{
  delete ping_timer;
}


bool PingTester::state() const
{
  return ping_state;
}


void PingTester::timeoutData()
{
  unsigned valid_pings=0;

  if(ping_istate==ping_replies.size()) {
    //
    // Check Reply Slots
    //
    for(unsigned i=0;i<ping_replies.size();i++) {
      if(ping_replies[i]) {
	valid_pings++;
      }
    }
    if(valid_pings<ping_config->ipv4MonitorValidPings(ping_number)) {
      if(ping_state) {
	ping_state=false;
	emit stateChanged(ping_number,ping_state);
      }
    }
    else {
      if(!ping_state) {
	ping_state=true;
	emit stateChanged(ping_number,ping_state);
      }
    }

    //
    // Sleep
    //
    ping_istate++;
    ping_timer->start(ping_config->ipv4MonitorInterval(ping_number),true);
  }
  else {
    if(ping_istate>ping_replies.size()) {
      //
      // Start New Run
      //
      for(unsigned i=0;i<ping_replies.size();i++) {
	ping_replies[i]=false;
      }
      ping_id=0xFFFF&rand();
      ping_istate=0;
    }
    ping_pinger->sendPing(ping_config->ipv4MonitorAddress(ping_number),
			  ping_istate,ping_id);
    ping_istate++;
    ping_timer->start(1000,true);
  }
}


void PingTester::echoReceivedData(const QHostAddress &addr,uint16_t seqno,
				  uint16_t id,const char *pay_load,int pay_len)
{
  if((addr==ping_config->ipv4MonitorAddress(ping_number))&&
     (id==ping_id)&&(seqno<(uint16_t)ping_replies.size())) {
    ping_replies[seqno]=true;
  }
} 
