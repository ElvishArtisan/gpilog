// gpi_watchdog.cpp
//
// A container class for GpiLog Watchdog State
//
//   (C) Copyright 2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: gpi_watchdog.cpp,v 1.1 2014/03/27 16:25:23 cvs Exp $
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

#include <gpi_watchdog.h>


GPIWatchdog::GPIWatchdog()
{
  clear();
}


int GPIWatchdog::timeout() const
{
  return dog_timeout;
}


void GPIWatchdog::setTimeout(int msecs)
{
  dog_timeout=msecs;
}


LocalGpio *GPIWatchdog::timeoutDevice() const
{
  return dog_timeout_device;
}


void GPIWatchdog::setTimeoutDevice(LocalGpio *dev)
{
  dog_timeout_device=dev;
}


unsigned GPIWatchdog::monitors() const
{
  return dog_monitored_devices.size();
}


int GPIWatchdog::monitoredDevice(unsigned n) const
{
  return dog_monitored_devices[n];
}


int GPIWatchdog::monitoredPin(unsigned n) const
{
  return dog_monitored_pins[n];
}


void GPIWatchdog::addMonitor(int dev,int pin)
{
  dog_monitored_devices.push_back(dev);
  dog_monitored_pins.push_back(pin);
}


int GPIWatchdog::timeoutPin() const
{
  return dog_timeout_pin;
}


void GPIWatchdog::setTimeoutPin(int line)
{
  dog_timeout_pin=line;
}


LocalGpio *GPIWatchdog::resetDevice() const
{
  return dog_reset_device;
}


void GPIWatchdog::setResetDevice(LocalGpio *dev)
{
  dog_reset_device=dev;
}


int GPIWatchdog::resetPin() const
{
  return dog_reset_pin;
}


void GPIWatchdog::setResetPin(int line)
{
  dog_reset_pin=line;
}


QString GPIWatchdog::timeoutCommand() const
{
  return dog_timeout_command;
}


void GPIWatchdog::setTimeoutCommand(const QString &cmd)
{
  dog_timeout_command=cmd;
}


QString GPIWatchdog::resetCommand() const
{
  return dog_reset_command;
}


void GPIWatchdog::setResetCommand(const QString &cmd)
{
  dog_reset_command=cmd;
}


QTimer *GPIWatchdog::timeoutTimer() const
{
  return dog_timeout_timer;
}


void GPIWatchdog::setTimeoutTimer(QTimer *timer)
{
  dog_timeout_timer=timer;
}


QTimer *GPIWatchdog::restartTimer() const
{
  return dog_restart_timer;
}


void GPIWatchdog::setRestartTimer(QTimer *timer)
{
  dog_restart_timer=timer;
}


bool GPIWatchdog::uniqueInstance() const
{
  return dog_unique_instance;
}


void GPIWatchdog::setUniqueInstance(bool state)
{
  dog_unique_instance=state;
}


bool GPIWatchdog::active() const
{
  return dog_active;
}


void GPIWatchdog::setActive(bool state)
{
  dog_active=state;
}


void GPIWatchdog::addSlot(const QTime &start_time,const QTime &end_time)
{
  dog_start_times.push_back(start_time);
  dog_end_times.push_back(end_time);
}


bool GPIWatchdog::slotActive(const QTime &time)
{
  for(unsigned i=0;i<dog_start_times.size();i++) {
    if((dog_start_times[i]<time)&&(dog_end_times[i]>time)) {
      return true;
    }
  }
  return false;
}


int GPIWatchdog::nextRestart()
{
  QTime time=QTime::currentTime();
  QTime start_time;
  int ret=-1;
  for(unsigned i=0;i<dog_start_times.size();i++) {
    if((time<dog_start_times[i])&&(time>start_time)) {
      start_time=dog_start_times[i];
    }
  }
  if(start_time>time) {
    ret=time.msecsTo(start_time);
  }
  else {
    ret=time.msecsTo(QTime(23,59,59))+1000+QTime().msecsTo(start_time);
  }
  return ret;
}


void GPIWatchdog::clear()
{
  dog_timeout=0;
  dog_timeout_device=NULL;
  dog_timeout_pin=-1;
  dog_reset_device=NULL;
  dog_reset_pin=-1;
  dog_timeout_command="";
  dog_reset_command="";
  dog_timeout_timer=NULL;
  dog_restart_timer=NULL;
  dog_unique_instance=false;
  dog_active=false;
}
