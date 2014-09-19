// gpi_monitor.h
//
// A container class for GpiLog UDP Monitor State
//
//   (C) Copyright 2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: gpi_monitor.h,v 1.1 2014/03/27 16:25:23 cvs Exp $
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

#ifndef GPI_MONITOR_H
#define GPI_MONITOR_H

#include <qstring.h>
#include <qtimer.h>
#include <qdatetime.h>
#include <qsocketdevice.h>
#include <qsocketnotifier.h>

class GPIMonitor
{
 public:
  GPIMonitor();
  ~GPIMonitor();
  bool state() const;
  void setState(bool state);
  QString name() const;
  void setName(const QString &str);
  int timeout() const;
  void setTimeout(int msecs);
  QString timeoutCommand() const;
  void setTimeoutCommand(const QString &cmd);
  QString resetCommand() const;
  void setResetCommand(const QString &cmd);
  Q_UINT16 udpPort() const;
  void setUdpPort(Q_UINT16 port);
  QString logfile() const;
  void setLogfile(const QString &str);
  bool logPackets() const;
  void setLogPackets(bool state);
  bool logTimeouts() const;
  void setLogTimeouts(bool state);
  QTimer *timeoutTimer() const;
  void setTimeoutTimer(QTimer *timer);
  QSocketDevice *socketDevice();
  void setSocketDevice(QSocketDevice *dev);
  QSocketNotifier *socketNotifier();
  void setSocketNotifier(QSocketNotifier *sn);
  void clear();

 private:
  bool mon_state;
  QString mon_name;
  int mon_timeout;
  QString mon_timeout_command;
  QString mon_reset_command;
  Q_UINT16 mon_udp_port;
  QString mon_logfile;
  bool mon_log_packets;
  bool mon_log_timeouts;
  QTimer *mon_timeout_timer;
  QSocketDevice *mon_socket_device;
  QSocketNotifier *mon_socket_notifier;
};


#endif  // GPI_MONITOR_H
