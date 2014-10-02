// gpi_config.cpp
//
// A container class for a Gpi Log Configuration
//
//   (C) Copyright 2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: gpi_config.cpp,v 1.2 2014/03/27 21:12:26 cvs Exp $
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

#include <gpi_config.h>
#include <profile.h>


GPIConfig::GPIConfig(QString filename)
{
  clear();
  conf_filename=filename;
}


QString GPIConfig::filename() const
{
  return conf_filename;
}


void GPIConfig::setFilename(QString filename)
{
  conf_filename=filename;
}


QString GPIConfig::logfile() const
{
  return conf_default_logfile;
}


QString GPIConfig::logfile(int dev,int line) const
{
  return conf_logfiles[dev][line];
}


int GPIConfig::uniqueLogfiles() const
{
  return conf_unique_logfiles.size();
}


QString GPIConfig::uniqueLogfile(int dev) const
{
  return conf_unique_logfiles[dev];
}


QString GPIConfig::gpioDevice(unsigned dev) const
{
  return conf_gpio_devices[dev];
}


LocalGpio::Transition GPIConfig::transitionType(int dev,int line) const
{
  return conf_gpio_transitions[dev][line];
}


QString GPIConfig::pinName(int dev,int line) const
{
  return conf_pin_names[dev][line];
}


int GPIConfig::watchdog(int dev,int line) const
{
  return conf_watchdogs[dev][line];
}


QString GPIConfig::onCommand(int dev,int line) const
{
  return conf_on_commands[dev][line];
}


QString GPIConfig::offCommand(int dev,int line) const
{
  return conf_off_commands[dev][line];
}


bool GPIConfig::inverted(int dev,int line) const
{
  return conf_inverted[dev][line];
}


int GPIConfig::pulsePeriod(int dev,int line) const
{
  return conf_pulse_periods[dev][line];
}


unsigned GPIConfig::watchdogs() const
{
  return conf_watchdog_timeouts.size();
}


QString GPIConfig::watchdogName(int wd) const
{
  return conf_watchdog_names[wd];
}


GPIConfig::NormalState GPIConfig::watchdogNormalState(int wd) const
{
  return conf_watchdog_normal_states[wd];
}


QString GPIConfig::watchdogTimeoutDevice(int wd) const
{
  return conf_watchdog_timeout_devices[wd];
}


int GPIConfig::watchdogTimeoutPin(int wd) const
{
  return conf_watchdog_timeout_pins[wd];
}


QString GPIConfig::watchdogResetDevice(int wd) const
{
  return conf_watchdog_reset_devices[wd];
}


int GPIConfig::watchdogResetPin(int wd) const
{
  return conf_watchdog_reset_pins[wd];
}


int GPIConfig::watchdogTimeout(int wd) const
{
  return conf_watchdog_timeouts[wd];
}


QString GPIConfig::watchdogTimeoutCommand(int wd) const
{
  return conf_watchdog_timeout_commands[wd];
}


QString GPIConfig::watchdogResetCommand(int wd) const
{
  return conf_watchdog_reset_commands[wd];
}


unsigned GPIConfig::watchdogTimeslots(int wd) const
{
  return conf_watchdog_starttimes[wd].size();
}


QTime GPIConfig::watchdogStartTime(int wd,unsigned slot)
{
  return conf_watchdog_starttimes[wd][slot];
}


QTime GPIConfig::watchdogEndTime(int wd,unsigned slot)
{
  return conf_watchdog_endtimes[wd][slot];
}


unsigned GPIConfig::monitors() const
{
  return conf_monitor_names.size();
}


QString GPIConfig::monitorName(unsigned mon) const
{
  return conf_monitor_names[mon];
}


int GPIConfig::monitorTimeout(unsigned mon) const
{
  return conf_monitor_timeouts[mon];
}


QString GPIConfig::monitorTimeoutCommand(unsigned mon) const
{
  return conf_monitor_timeout_commands[mon];
}


QString GPIConfig::monitorResetCommand(unsigned mon) const
{
  return conf_monitor_reset_commands[mon];
}


Q_UINT16 GPIConfig::monitorUdpPort(unsigned mon) const
{
  return conf_monitor_udp_ports[mon];
}


QString GPIConfig::monitorLogfile(unsigned mon) const
{
  return conf_monitor_logfiles[mon];
}


bool GPIConfig::monitorLogPackets(unsigned mon) const
{
  return conf_monitor_log_packetss[mon];
}


bool GPIConfig::monitorLogTimeouts(unsigned mon) const
{
  return conf_monitor_log_timeoutss[mon];
}


unsigned GPIConfig::ipv4Monitors() const
{
  return conf_ipv4_monitor_names.size();
}


QString GPIConfig::ipv4MonitorName(unsigned mon) const
{
  return conf_ipv4_monitor_names[mon];
}


QHostAddress GPIConfig::ipv4MonitorAddress(unsigned mon) const
{
  return conf_ipv4_monitor_addresses[mon];
}


unsigned GPIConfig::ipv4MonitorPings(unsigned mon) const
{
  return conf_ipv4_monitor_pings[mon];
}


unsigned GPIConfig::ipv4MonitorValidPings(unsigned mon) const
{
  return conf_ipv4_monitor_valid_pings[mon];
}


unsigned GPIConfig::ipv4MonitorTimeout(unsigned mon) const
{
  return conf_ipv4_monitor_timeouts[mon];
}


unsigned GPIConfig::ipv4MonitorInterval(unsigned mon) const
{
  return conf_ipv4_monitor_intervals[mon];
}


QString GPIConfig::ipv4MonitorTimeoutCommand(unsigned mon) const
{
  return conf_ipv4_monitor_timeout_commands[mon];
}


QString GPIConfig::ipv4MonitorResetCommand(unsigned mon) const
{
  return conf_ipv4_monitor_reset_commands[mon];
}


QString GPIConfig::ipv4MonitorLogfile(unsigned mon) const
{
  return conf_ipv4_monitor_logfiles[mon];
}


void GPIConfig::load()
{
  QString dev;
  QString trans;
  QString default_trans;
  QString section;
  QString start_section;
  QString end_section;
  QString name;
  int dog=0;
  int mon=0;
  int slot=0;
  int pin=1;
  int timeout=-1;
  bool ok=false;
  Profile *p=new Profile();
  p->setSource(GPILOG_CONF_FILE);
  QTime start_time;
  QTime end_time;
  bool start_ok=false;
  bool end_ok=false;
  QString str;

  //
  // [Global] Section
  //
  conf_default_logfile=p->stringValue("Global","Logfile","");
  if(conf_default_logfile.isEmpty()) {
    fprintf(stderr,"gpilog: no logfile specified\n");
    exit(256);
  }
  for(unsigned i=0;i<GPIO_MAX_DEVICES;i++) {
    dev=p->stringValue("Global",QString().sprintf("GpioDevice%d",i),"",&ok);
    if(ok) {
      conf_gpio_devices[i]=dev;
      default_trans=
	p->stringValue("Global",QString().sprintf("LogEvents%d",i),"both");

      //
      // [Device<n>] Section
      //
      section=QString().sprintf("Device%d",i);
      for(unsigned j=0;j<GPIO_MAX_LINES;j++) {
	conf_logfiles[i][j]=
	  p->stringValue(section,QString().sprintf("Pin%dLogfile",j),
			 conf_default_logfile);
	trans=p->stringValue(section,QString().sprintf("Pin%dEvents",j),
			     default_trans);
	if(trans.lower()=="rising") {
	  conf_gpio_transitions[i][j]=LocalGpio::TransRising;
	}
	else {
	  if(trans.lower()=="falling") {
	    conf_gpio_transitions[i][j]=LocalGpio::TransFalling;
	  }
	  else {
	    conf_gpio_transitions[i][j]=LocalGpio::TransBoth;
	  }
	}
	conf_inverted[i][j]=
	  p->boolValue(section,QString().sprintf("Pin%dInverted",j),false);
	conf_pin_names[i][j]=
	  p->stringValue(section,QString().sprintf("Pin%dName",j),
			 QString().sprintf("Pin %d",pin++));
	conf_watchdogs[i][j]=
	  p->intValue(section,QString().sprintf("Pin%dWatchdog",j),-1);
	conf_on_commands[i][j]=
	  p->stringValue(section,QString().sprintf("Pin%dOnCommand",j));
	conf_off_commands[i][j]=
	  p->stringValue(section,QString().sprintf("Pin%dOffCommand",j));
	conf_pulse_periods[i][j]=
	  p->intValue(section,QString().sprintf("Pin%dPulsePeriod",j));
	ok=false;
	for(unsigned k=0;k<conf_unique_logfiles.size();k++) {
	  if(conf_unique_logfiles[k]==conf_logfiles[i][j]) {
	    ok=true;
	  }
	}
	if(!ok) {
	  conf_unique_logfiles.push_back(conf_logfiles[i][j]);
	}
      }
    }
  }

  //
  // [Watchdog<n>] Section
  //
  GPIConfig::NormalState nstate;
  section=QString().sprintf("Watchdog%d",dog);
  timeout=p->intValue(section,"Timeout",-1);
  while(timeout>0) {
    conf_watchdog_timeouts.push_back(timeout);
    conf_watchdog_names.
      push_back(p->stringValue(section,"Name",
			       QString().sprintf("watchdog %d activated",dog)));
    str=p->stringValue(section,"NormalState","Transition").lower();
    nstate=GPIConfig::StateTransition;
    if(str=="high") {
      nstate=GPIConfig::StateHigh;
    }
    if(str=="low") {
      nstate=GPIConfig::StateLow;
    }
    conf_watchdog_normal_states.push_back(nstate);
    conf_watchdog_timeout_devices.
      push_back(p->stringValue(section,"TimeoutDevice",""));
    conf_watchdog_timeout_pins.push_back(p->intValue(section,"TimeoutPin",0));
    conf_watchdog_reset_devices.
      push_back(p->stringValue(section,"ResetDevice",""));
    conf_watchdog_reset_pins.push_back(p->intValue(section,"ResetPin",0));
    conf_watchdog_timeout_commands.
      push_back(p->stringValue(section,"TimeoutCommand",""));
    conf_watchdog_reset_commands.
      push_back(p->stringValue(section,"ResetCommand",""));
    std::vector<QTime> *vec=new std::vector<QTime>;
    conf_watchdog_starttimes.push_back(*vec);
    vec=new std::vector<QTime>;
    conf_watchdog_endtimes.push_back(*vec);
    slot=0;
    start_section=QString().sprintf("StartTime%d",slot);
    end_section=QString().sprintf("EndTime%d",slot);
    start_time=p->timeValue(section,start_section,QTime(),&start_ok);
    end_time=p->timeValue(section,end_section,QTime(),&end_ok);
    while(start_ok&&end_ok) {
      conf_watchdog_starttimes[dog].push_back(start_time);
      conf_watchdog_endtimes[dog].push_back(end_time);
      slot++;
      start_section=QString().sprintf("StartTime%d",slot);
      end_section=QString().sprintf("EndTime%d",slot);
      start_time=p->timeValue(section,start_section,QTime(),&start_ok);
      end_time=p->timeValue(section,end_section,QTime(),&end_ok);
    }
    dog++;
    section=QString().sprintf("Watchdog%d",dog); 
    timeout=p->intValue(section,"Timeout",-1);
  }

  //
  // [UdpMonitor<n>] Section
  //
  section=QString().sprintf("UdpMonitor%d",mon);
  name=p->stringValue(section,"Name","",&ok);
  while(ok) {
    conf_monitor_names.push_back(name);
    conf_monitor_timeouts.push_back(p->intValue(section,"Timeout",60000));
    conf_monitor_timeout_commands.
      push_back(p->stringValue(section,"TimeoutCommand"));
    conf_monitor_reset_commands.
      push_back(p->stringValue(section,"ResetCommand"));
    conf_monitor_udp_ports.push_back(p->intValue(section,"UdpPort"));
    str=p->stringValue(section,"Logfile","",&ok);
    if(ok) {
      conf_monitor_logfiles.push_back(str);
    }
    else {
      conf_monitor_logfiles.push_back(conf_default_logfile);
    }
    conf_monitor_log_packetss.push_back(p->boolValue(section,"LogPackets"));
    conf_monitor_log_timeoutss.push_back(p->boolValue(section,"LogTimeouts"));

    section=QString().sprintf("UdpMonitor%d",++mon);
    name=p->stringValue(section,"Name","",&ok);
  }

  //
  // [IPv4AddressMonitor<n>] Section
  //
  mon=0;
  section=QString().sprintf("IPv4AddressMonitor%d",mon);
  name=p->stringValue(section,"Name","",&ok);
  while(ok) {
    conf_ipv4_monitor_names.push_back(name);
    conf_ipv4_monitor_addresses.
      push_back(QHostAddress(p->stringValue(section,"Address")));
    conf_ipv4_monitor_pings.push_back(p->intValue(section,"Pings",5));
    conf_ipv4_monitor_valid_pings.
      push_back(p->intValue(section,"ValidPings",
			    conf_ipv4_monitor_pings.back()));
    conf_ipv4_monitor_timeouts.push_back(p->intValue(section,"Timeout",5000));
    conf_ipv4_monitor_intervals.
      push_back(p->intValue(section,"Interval",60000));
    conf_ipv4_monitor_timeout_commands.
      push_back(p->stringValue(section,"TimeoutCommand"));
    conf_ipv4_monitor_reset_commands.
      push_back(p->stringValue(section,"ResetCommand"));
    conf_ipv4_monitor_logfiles.
      push_back(p->stringValue(section,"Logfile"));
    section=QString().sprintf("IPv4AddressMonitor%d",++mon);
    name=p->stringValue(section,"Name","",&ok);
  }
  delete p;
}


void GPIConfig::clear()
{
  conf_filename="";
  conf_default_logfile="";
  for(unsigned i=0;i<GPIO_MAX_DEVICES;i++) {
    conf_gpio_devices[i]="";
    for(unsigned j=0;j<GPIO_MAX_LINES;j++) {
      conf_gpio_transitions[i][i]=LocalGpio::TransRising;
      conf_inverted[i][j]=false;
      conf_logfiles[i][j]="";
      conf_pin_names[i][j]="";
      conf_watchdogs[i][j]=-1;
      conf_on_commands[i][j]="";
      conf_off_commands[i][j]="";
    }
  }
  for(unsigned i=0;i<conf_watchdog_starttimes.size();i++) {
    delete &(conf_watchdog_starttimes[i]);
  }
  conf_watchdog_starttimes.clear();
  for(unsigned i=0;i<conf_watchdog_endtimes.size();i++) {
    delete &(conf_watchdog_endtimes[i]);
  }
  conf_watchdog_endtimes.clear();
  conf_monitor_names.clear();
  conf_monitor_timeouts.clear();
  conf_monitor_timeout_commands.clear();
  conf_monitor_reset_commands.clear();
  conf_monitor_udp_ports.clear();
  conf_monitor_logfiles.clear();
  conf_monitor_log_packetss.clear();
  conf_monitor_log_timeoutss.clear();
}
