// gpilog.h
//
// A GPI Logger
//
//   (C) Copyright 2004 Fred Gleason <fredg@salemradiolabs.com>
//
//      $Id: gpilog.h,v 1.2 2014/03/27 21:12:27 cvs Exp $
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

#ifndef GPILOG_H
#define GPILOG_H

#include <vector>

#include <qobject.h>
#include <qtimer.h>

#include <gpi_config.h>
#include <gpi_watchdog.h>
#include <gpi_monitor.h>
#include <gpi_pulser.h>
#include <local_gpio.h>

//
// Application Settings
//
#define GPILOG_WATCHDOG_SCAN_INTERVAL 1000

//
// Log Event Masks
//
#define GPILOG_RISING_EDGE 1
#define GPILOG_FALLING_EDGE 2

class GpiLog : public QObject
{
 Q_OBJECT
 public:
  GpiLog(QObject *parent=0,const char *name=0);

 private slots:
  void startupTimeoutData();
  void inputChangedData(int id,int line,bool state);
  void watchdogScanData();
  void watchdogTimeoutData(int n);
  void watchdogRestartData(int n);
  void monitorPacketData(int sock);
  void monitorTimeoutData(int n);
  void restartData();

 private:
  void UpdateWatchdog(int wd);
  QString MakePrintableString(const char *data,int len) const;
  void Init();
  void Restart();
  void LogGpiLine(const QString &str,int dev=-1,int line=-1) const;
  void LogMonitor(const QString &str,GPIMonitor *mon) const;
  void LogLine(const QString &str,const QString &logfile) const;
  std::vector<LocalGpio *> gpio_devices;
  std::vector<GPIWatchdog *> gpio_watchdogs;
  std::vector<GPIMonitor *> gpio_monitors;
  std::vector<GPIPulser *> gpio_pulsers;
  bool initial_start;
  GPIConfig *gpi_config;
  QTimer *gpi_startup_timer;
  QTimer *gpi_watchdog_scan_timer;
};


#endif  // GPILOG_H
