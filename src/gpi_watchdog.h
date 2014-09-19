// gpi_watchdog.h
//
// A container class for GpiLog Watchdog State
//
//   (C) Copyright 2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: gpi_watchdog.h,v 1.1 2014/03/27 16:25:23 cvs Exp $
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

#ifndef GPI_WATCHDOG_H
#define GPI_WATCHDOG_H

#include <vector>

#include <qdatetime.h>

#include <local_gpio.h>

class GPIWatchdog
{
 public:
  GPIWatchdog();
  int timeout() const;
  void setTimeout(int msecs);
  LocalGpio *timeoutDevice() const;
  void setTimeoutDevice(LocalGpio *dev);
  unsigned monitors() const;
  int monitoredDevice(unsigned n) const;
  int monitoredPin(unsigned n) const;
  void addMonitor(int dev,int pin);
  int timeoutPin() const;
  void setTimeoutPin(int line);
  LocalGpio *resetDevice() const;
  void setResetDevice(LocalGpio *dev);
  int resetPin() const;
  void setResetPin(int line);
  QString timeoutCommand() const;
  void setTimeoutCommand(const QString &cmd);
  QString resetCommand() const;
  void setResetCommand(const QString &cmd);
  QTimer *timeoutTimer() const;
  void setTimeoutTimer(QTimer *timer);
  QTimer *restartTimer() const;
  void setRestartTimer(QTimer *timer);
  bool uniqueInstance() const;
  void setUniqueInstance(bool state);
  bool active() const;
  void setActive(bool state);
  void addSlot(const QTime &start_time,const QTime &end_time);
  bool slotActive(const QTime &time);
  int nextRestart();
  void clear();

 private:
  int dog_timeout;
  LocalGpio *dog_timeout_device;
  int dog_timeout_pin;
  LocalGpio *dog_reset_device;
  int dog_reset_pin;
  QString dog_timeout_command;
  QString dog_reset_command;
  QTimer *dog_timeout_timer;
  QTimer *dog_restart_timer;
  bool dog_unique_instance;
  bool dog_active;
  std::vector<int> dog_monitored_devices;
  std::vector<int> dog_monitored_pins;
  std::vector<QTime> dog_start_times;
  std::vector<QTime> dog_end_times;
};


#endif  // GPI_WATCHDOG_H
