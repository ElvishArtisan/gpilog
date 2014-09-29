// ping_tester.h
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

#ifndef PING_TESTER_H
#define PING_TESTER_H

#include <stdint.h>

#include <vector>

#include <qobject.h>
#include <qtimer.h>

#include "gpi_config.h"
#include "ipv4_pinger.h"

class PingTester : public QObject
{
  Q_OBJECT;
 public:
  PingTester(GPIConfig *config,unsigned n,IPv4Pinger *pinger,QObject *parent=0);
  ~PingTester();
  bool state() const;

 signals:
  void stateChanged(unsigned n,bool state);

 private slots:
  void timeoutData();
  void echoReceivedData(const QHostAddress &addr,uint16_t seqno,uint16_t id,
			const char *pay_load,int pay_len); 

 private:
  GPIConfig *ping_config;
  int ping_number;
  IPv4Pinger *ping_pinger;
  bool ping_state;
  std::vector <bool> ping_replies;
  QTimer *ping_timer;
  unsigned ping_istate;
  uint16_t ping_id;
};


#endif  // PING_TESTER_H
