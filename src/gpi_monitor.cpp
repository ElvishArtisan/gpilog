// gpi_monitor.cpp
//
// A container class for GpiLog UDP Monitor State
//
//   (C) Copyright 2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: gpi_monitor.cpp,v 1.1 2014/03/27 16:25:23 cvs Exp $
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

#include <gpi_monitor.h>

GPIMonitor::GPIMonitor()
{
  clear();
}


GPIMonitor::~GPIMonitor()
{
  if(mon_timeout_timer!=NULL) {
    delete mon_timeout_timer;
  }
  if(mon_socket_device!=NULL) {
    delete mon_socket_device;
  }
  if(mon_socket_notifier!=NULL) {
    delete mon_socket_notifier;
  }
}


bool GPIMonitor::state() const
{
  return mon_state;
}


void GPIMonitor::setState(bool state)
{
  mon_state=state;
}


QString GPIMonitor::name() const
{
  return mon_name;
}


void GPIMonitor::setName(const QString &str)
{
  mon_name=str;
}


int GPIMonitor::timeout() const
{
  return mon_timeout;
}


void GPIMonitor::setTimeout(int msecs)
{
  mon_timeout=msecs;
}


QString GPIMonitor::timeoutCommand() const
{
  return mon_timeout_command;
}


void GPIMonitor::setTimeoutCommand(const QString &cmd)
{
  mon_timeout_command=cmd;
}


QString GPIMonitor::resetCommand() const
{
  return mon_reset_command;
}


void GPIMonitor::setResetCommand(const QString &cmd)
{
  mon_reset_command=cmd;
}


Q_UINT16 GPIMonitor::udpPort() const
{
  return mon_udp_port;
}


void GPIMonitor::setUdpPort(Q_UINT16 port)
{
  mon_udp_port=port;
}


QString GPIMonitor::logfile() const
{
  return mon_logfile;
}


void GPIMonitor::setLogfile(const QString &str)
{
  mon_logfile=str;
}


bool GPIMonitor::logPackets() const
{
  return mon_log_packets;
}


void GPIMonitor::setLogPackets(bool state)
{
  mon_log_packets=state;
}


bool GPIMonitor::logTimeouts() const
{
  return mon_log_timeouts;
}


void GPIMonitor::setLogTimeouts(bool state)
{
  mon_log_timeouts=state;
}


QTimer *GPIMonitor::timeoutTimer() const
{
  return mon_timeout_timer;
}


void GPIMonitor::setTimeoutTimer(QTimer *timer)
{
  mon_timeout_timer=timer;
}


QSocketDevice *GPIMonitor::socketDevice()
{
  return mon_socket_device;
}


void GPIMonitor::setSocketDevice(QSocketDevice *dev)
{
  mon_socket_device=dev;
}


QSocketNotifier *GPIMonitor::socketNotifier()
{
  return mon_socket_notifier;
}


void GPIMonitor::setSocketNotifier(QSocketNotifier *sn)
{
  mon_socket_notifier=sn;
}


void GPIMonitor::clear()
{
  mon_state=true;
  mon_name="";
  mon_timeout=0;
  mon_timeout_command="";
  mon_reset_command="";
  mon_udp_port=0;
  mon_logfile="";
  mon_log_packets=false;
  mon_log_timeouts=false;
  mon_timeout_timer=NULL;
  mon_socket_device=NULL;
  mon_socket_notifier=NULL;
}
