// gpi_config.h
//
// A container class for a Gpi Log Configuration
//
//   (C) Copyright 2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: gpi_config.h,v 1.2 2014/03/27 21:12:26 cvs Exp $
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

#ifndef GPI_CONFIG_H
#define GPI_CONFIG_H

#include <vector>

#include <qstring.h>
#include <qdatetime.h>
#include <qhostaddress.h>

#include <local_gpio.h>

#define GPILOG_CONF_FILE "/etc/gpilog.conf"
#define GPILOG_DEFAULT_GPI_DEVICE "/dev/gpio0"
#define GPILOG_PID_FILE "/var/run/gpilog.pid"


class GPIConfig
{
 public:
  enum NormalState {StateTransition=0,StateHigh=1,StateLow=2};
  GPIConfig(QString filename);
  QString filename() const;
  void setFilename(QString filename);
  QString logfile() const;
  QString logfile(int dev,int line) const;
  int uniqueLogfiles() const;
  QString uniqueLogfile(int dev) const;
  QString gpioDevice(unsigned dev) const;
  LocalGpio::Transition transitionType(int dev,int line) const;
  QString pinName(int dev,int line) const;
  int watchdog(int dev,int line) const;
  QString onCommand(int dev,int line) const;
  QString offCommand(int dev,int line) const;
  bool inverted(int dev,int line) const;
  int pulsePeriod(int dev,int line) const;
  unsigned watchdogs() const;
  QString watchdogName(int wd) const;
  GPIConfig::NormalState watchdogNormalState(int wd) const;
  QString watchdogTimeoutDevice(int wd) const;
  int watchdogTimeoutPin(int wd) const;
  QString watchdogResetDevice(int wd) const;
  int watchdogResetPin(int wd) const;
  int watchdogTimeout(int wd) const;
  QString watchdogTimeoutCommand(int wd) const;
  QString watchdogResetCommand(int wd) const;
  unsigned watchdogTimeslots(int wd) const;
  QTime watchdogStartTime(int wd,unsigned slot);
  QTime watchdogEndTime(int wd,unsigned slot);
  unsigned monitors() const;
  QString monitorName(unsigned mon) const;
  int monitorTimeout(unsigned mon) const;
  QString monitorTimeoutCommand(unsigned mon) const;
  QString monitorResetCommand(unsigned mon) const;
  Q_UINT16 monitorUdpPort(unsigned mon) const;
  QString monitorLogfile(unsigned mon) const;
  bool monitorLogPackets(unsigned mon) const;
  bool monitorLogTimeouts(unsigned mon) const;

  unsigned ipv4Monitors() const;
  QString ipv4MonitorName(unsigned mon) const;
  QHostAddress ipv4MonitorAddress(unsigned mon) const;
  unsigned ipv4MonitorPings(unsigned mon) const;
  unsigned ipv4MonitorTimeout(unsigned mon) const;
  unsigned ipv4MonitorInterval(unsigned mon) const;
  QString ipv4MonitorTimeoutCommand(unsigned mon) const;
  QString ipv4MonitorResetCommand(unsigned mon) const;
  QString ipv4MonitorLogfile(unsigned mon) const;

  void load();
  void clear();

 private:
  QString conf_filename;
  QString conf_default_logfile;
  QString conf_gpio_devices[GPIO_MAX_DEVICES];
  QString conf_logfiles[GPIO_MAX_DEVICES][GPIO_MAX_LINES];
  std::vector<QString> conf_unique_logfiles;
  QString conf_pin_names[GPIO_MAX_DEVICES][GPIO_MAX_LINES];
  int conf_watchdogs[GPIO_MAX_DEVICES][GPIO_MAX_LINES];
  QString conf_on_commands[GPIO_MAX_DEVICES][GPIO_MAX_LINES];
  QString conf_off_commands[GPIO_MAX_DEVICES][GPIO_MAX_LINES];
  bool conf_inverted[GPIO_MAX_DEVICES][GPIO_MAX_LINES];
  int conf_pulse_periods[GPIO_MAX_DEVICES][GPIO_MAX_LINES];
  LocalGpio::Transition 
    conf_gpio_transitions[GPIO_MAX_DEVICES][GPIO_MAX_LINES];
  std::vector<QString> conf_watchdog_names;
  std::vector<GPIConfig::NormalState> conf_watchdog_normal_states;
  std::vector<QString> conf_watchdog_timeout_devices;
  std::vector<int> conf_watchdog_timeout_pins; 
  std::vector<QString> conf_watchdog_reset_devices;
  std::vector<int> conf_watchdog_reset_pins;
  std::vector<int> conf_watchdog_timeouts;
  std::vector<QString> conf_watchdog_timeout_commands;
  std::vector<QString> conf_watchdog_reset_commands;
  std::vector<std::vector<QTime> > conf_watchdog_starttimes;
  std::vector<std::vector<QTime> > conf_watchdog_endtimes;
  std::vector<QString> conf_monitor_names;
  std::vector<unsigned> conf_monitor_timeouts;
  std::vector<QString> conf_monitor_timeout_commands;
  std::vector<QString> conf_monitor_reset_commands;
  std::vector<Q_UINT16> conf_monitor_udp_ports;
  std::vector<QString> conf_monitor_logfiles;
  std::vector<bool> conf_monitor_log_packetss;
  std::vector<bool> conf_monitor_log_timeoutss;

  std::vector<QString> conf_ipv4_monitor_names;
  std::vector<QHostAddress> conf_ipv4_monitor_addresses;
  std::vector<unsigned> conf_ipv4_monitor_pings;
  std::vector<unsigned> conf_ipv4_monitor_timeouts;
  std::vector<unsigned> conf_ipv4_monitor_intervals;
  std::vector<QString> conf_ipv4_monitor_timeout_commands;
  std::vector<QString> conf_ipv4_monitor_reset_commands;
  std::vector<QString> conf_ipv4_monitor_logfiles;
};


#endif  // GPI_CONFIG_H
