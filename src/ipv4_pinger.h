// ipv4_pinger.h
//
// IPv4 Address Monitor
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

#ifndef IPV4_PINGER_H
#define IPV4_PINGER_H

#include <stdint.h>
#include <time.h>

#include <qhostaddress.h>
#include <qobject.h>
#include <qsocketnotifier.h>

#include "gpi_config.h"

class IPv4Pinger : public QObject
{
  Q_OBJECT;
 public:
  IPv4Pinger(QObject *parent=0);
  ~IPv4Pinger();
  void sendPing(const QHostAddress &addr,uint16_t seqno,uint16_t id,
		const QByteArray &payload=QByteArray());

 signals:
  void echoReceived(const QHostAddress &addr,uint16_t seqno,uint16_t id,
		    const char *pay_load,int pay_len); 

 private slots:
  void readyReadData(int sock);

 private:
  int GeneratePacket(char *data,uint16_t seqno,uint16_t id,
		     const QByteArray &payload);
  uint16_t Checksum(const char *data,int len);
  bool CheckPacket(const char *data,int len);
  int ipv4_socket;
  QSocketNotifier *ipv4_notifier;
};


#endif  // IPV4_PINGER_H
