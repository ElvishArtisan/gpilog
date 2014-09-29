// gpilog.cpp
//
// A GPI Logger
//
//   (C) Copyright 2007,2014 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: gpilog.cpp,v 1.3 2014/03/27 21:18:47 cvs Exp $
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
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <qsignalmapper.h>
#include <qapplication.h>
#include <qdatetime.h>

#include <profile.h>
#include <gpilog.h>

#define BUFFER_SIZE 4096

//
// Global Variables
//
volatile bool restart=false;
volatile bool shutdown=false;


void SigHandler(int signo)
{
  pid_t pid;

  switch(signo) {
      case SIGTERM:
      case SIGINT:
	shutdown=true;
	break;

      case SIGHUP:
	restart=true;
	break;

      case SIGCHLD:
	pid=waitpid(-1,NULL,WNOHANG);
	while(pid>0) {
	  pid=waitpid(-1,NULL,WNOHANG);
	}
	signal(SIGCHLD,SigHandler);
	break;
  }
}


GpiLog::GpiLog(QObject *parent,const char *name)
  : QObject(this,name)
{
  initial_start=true;

  //
  // The Startup Timer
  //
  gpi_startup_timer=new QTimer(this,"gpi_startup_timer");
  connect(gpi_startup_timer,SIGNAL(timeout()),this,SLOT(startupTimeoutData()));

  //
  // The Watchdog Scan Timer
  //
  gpi_watchdog_scan_timer=new QTimer(this,"gpi_watchdog_scan_timer");
  connect(gpi_watchdog_scan_timer,SIGNAL(timeout()),
	  this,SLOT(watchdogScanData()));

  Init();

  //
  // The Restart Timer
  //
  QTimer *timer=new QTimer(this,"restart_timer");
  connect(timer,SIGNAL(timeout()),this,SLOT(restartData()));
  timer->start(1000);

  //
  // Detach
  //
  if((qApp->argc()<2)||(strcmp(qApp->argv()[1],"-d"))) {
    daemon(0,0);
  }

  //
  // Log the PID
  //
  ::signal(SIGINT,SigHandler);
  ::signal(SIGTERM,SigHandler);
  ::signal(SIGHUP,SigHandler);
  ::signal(SIGCHLD,SigHandler);
  FILE *file=fopen(GPILOG_PID_FILE,"w");
  if(file!=NULL) {
    fprintf(file,"%d",getpid());
    fclose(file);
  }
}


void GpiLog::startupTimeoutData()
{
  for(unsigned i=0;i<gpio_devices.size();i++) {
    connect(gpio_devices[i],SIGNAL(inputChanged(int,int,bool)),
	    this,SLOT(inputChangedData(int,int,bool)));
  }
}


void GpiLog::inputChangedData(int id,int line,bool state)
{
  //
  // Apply Inversion
  //
  if(gpi_config->inverted(id,line)) {
    state=!state;
  }

  //
  // Update Watchdog
  //
  int wd=gpi_config->watchdog(id,line);
  if(wd>=0) {
    if(gpi_config->watchdogNormalState(wd)==GPIConfig::StateTransition) {
      if(((unsigned)wd)<gpio_watchdogs.size()) {
	UpdateWatchdog(wd);
      }
    }
  }

  //
  // Generate Log Entry
  //
  switch(gpio_devices[id]->transition(line)) {
      case LocalGpio::TransBoth:
	if(state) {
	  LogGpiLine(QString().sprintf("%s ON",(const char *)
				    gpi_config->pinName(id,line)),id,line);
	}
	else {
	  LogGpiLine(QString().sprintf("%s OFF",(const char *)
				    gpi_config->pinName(id,line)),id,line);
	}
	break;

      case LocalGpio::TransRising:
	if(state) {
	  LogGpiLine(QString().sprintf("%s ACTIVATED",(const char *)
				    gpi_config->pinName(id,line)),id,line);
	}
	break;

      case LocalGpio::TransFalling:
	if(!state) {
	  LogGpiLine(QString().sprintf("%s ACTIVATED",(const char *)
				    gpi_config->pinName(id,line)),id,line);
	}
	break;
  }

  //
  // Run Command
  //
  if(state) {
    if(!gpi_config->onCommand(id,line).isEmpty()) {
      if(fork()==0) {
	system(gpi_config->onCommand(id,line));
	exit(0);
      }
    }
  }
  else {
    if(!gpi_config->offCommand(id,line).isEmpty()) {
      if(fork()==0) {
	system(gpi_config->offCommand(id,line));
	exit(0);
      }
    }
  }
}


void GpiLog::watchdogScanData()
{
  for(unsigned i=0;i<gpi_config->watchdogs();i++) {
    switch(gpi_config->watchdogNormalState(i)) {
      case GPIConfig::StateTransition:
	break;

      case GPIConfig::StateHigh:
	for(unsigned j=0;j<gpio_watchdogs[i]->monitors();j++) {
	  if(gpi_config->inverted(gpio_watchdogs[i]->monitoredDevice(j),
				  gpio_watchdogs[i]->monitoredPin(j))) {
	    if(!gpio_devices[gpio_watchdogs[i]->monitoredDevice(j)]->
	       inputState(gpio_watchdogs[i]->monitoredPin(j))) {
	      UpdateWatchdog(i);
	    }
	  }
	  else {
	    if(gpio_devices[gpio_watchdogs[i]->monitoredDevice(j)]->
	       inputState(gpio_watchdogs[i]->monitoredPin(j))) {
	      UpdateWatchdog(i);
	    }
	  }
	}
	break;

      case GPIConfig::StateLow:
	for(unsigned j=0;j<gpio_watchdogs[i]->monitors();j++) {
	  if(gpi_config->inverted(gpio_watchdogs[i]->monitoredDevice(j),
				  gpio_watchdogs[i]->monitoredPin(j))) {
	    if(gpio_devices[gpio_watchdogs[i]->monitoredDevice(j)]->
	       inputState(gpio_watchdogs[i]->monitoredPin(j))) {
	      UpdateWatchdog(i);
	    }
	  }
	  else {
	    if(!gpio_devices[gpio_watchdogs[i]->monitoredDevice(j)]->
	       inputState(gpio_watchdogs[i]->monitoredPin(j))) {
	      UpdateWatchdog(i);
	    }
	  }
	}
	break;
    }
  }
}


void GpiLog::watchdogTimeoutData(int n)
{
  if(!gpio_watchdogs[n]->slotActive(QTime::currentTime())) {
    return;
  }
  LocalGpio *dev=gpio_watchdogs[n]->timeoutDevice();
  int pin=gpio_watchdogs[n]->timeoutPin();
  QString cmd=gpio_watchdogs[n]->timeoutCommand();

  gpio_watchdogs[n]->setActive(true);
  if((pin>=0)&&(pin<dev->outputs())) {
    dev->gpoSet(pin,1000);
  }
  if(!cmd.isEmpty()) {
    if(fork()==0) {
      system(cmd);
      exit(0);
    }
  }
  LogGpiLine(gpi_config->watchdogName(n));
}


void GpiLog::watchdogRestartData(int n)
{
  gpio_watchdogs[n]->timeoutTimer()->stop();
  gpio_watchdogs[n]->timeoutTimer()->start(gpio_watchdogs[n]->
						timeout(),true);
  gpio_watchdogs[n]->setActive(false);
  gpio_watchdogs[n]->restartTimer()->
    start(gpio_watchdogs[n]->nextRestart(),true);
}


void GpiLog::monitorPacketData(int sock)
{
  char data[1500];
  int s;

  GPIMonitor *mon=NULL;
  for(unsigned i=0;i<gpio_monitors.size();i++) {
    if(gpio_monitors[i]->socketDevice()->socket()==sock) {
      mon=gpio_monitors[i];
      i=gpio_monitors.size();
    }
  }
  if(mon==NULL) {
    LogMonitor("attempting to read from unknown socket",mon);
    return;
  }

  mon->timeoutTimer()->stop();
  if(!mon->state()) {
    if(!mon->resetCommand().isEmpty()) {
      if(fork()==0) {
	system(mon->resetCommand());
	exit(0);
      }
    }
    mon->setState(true);
    if(mon->logTimeouts()) {
      LogMonitor(QString().sprintf("UDP monitor %s restored",
				   (const char *)mon->name()),mon);
    }
  }
  while((s=mon->socketDevice()->readBlock(data,1500))>0) {
    if(mon->logPackets()) {
      LogMonitor(QString().sprintf("%s received %d bytes: \"%s\"",
				   (const char *)mon->name(),s,
				   (const char *)MakePrintableString(data,s)),
		 mon);
    }
  }
  if(mon->timeout()>0) {
    mon->timeoutTimer()->start(mon->timeout(),true);
  }
}


void GpiLog::monitorTimeoutData(int n)
{
  printf("monitorTimeoutData()\n");
  GPIMonitor *mon=gpio_monitors[n];
  mon->setState(false);
  if(!mon->timeoutCommand().isEmpty()) {
    if(fork()==0) {
      system(mon->timeoutCommand());
      exit(0);
    }
  }
  if(mon->logTimeouts()) {
    LogMonitor(QString().sprintf("UDP monitor %s timed out",
				 (const char *)mon->name()),mon);
  }
}


void GpiLog::ipv4StateChangedData(unsigned n,bool state)
{
  //  printf("ipv4StateChangedData(%u,%d)\n",n,state);

  //
  // Log It
  //
  QString msg=tr("IPv4 monitor")+" \""+gpi_config->ipv4MonitorName(n)+"\" "+
    "["+gpi_config->ipv4MonitorAddress(n).toString()+"] ";
  if(state) {
    msg+=tr("been restored.");
  }
  else {
    msg+=tr("failed.");
  }
  msg+="\n";
  if(gpi_config->ipv4MonitorLogfile(n).isEmpty()) {
    LogLine(msg,gpi_config->logfile());
  }
  else {
    LogLine(msg,gpi_config->ipv4MonitorLogfile(n));
  }

  //
  // Run Script
  //
  if(state) {
    if(!gpi_config->ipv4MonitorResetCommand(n).isEmpty()) {
      if(fork()==0) {
	system(gpi_config->ipv4MonitorResetCommand(n));
	exit(0);
      }
    }
  }
  else {
    if(!gpi_config->ipv4MonitorTimeoutCommand(n).isEmpty()) {
      if(fork()==0) {
	system(gpi_config->ipv4MonitorTimeoutCommand(n));
	exit(0);
      }
    }
  }  
}


void GpiLog::restartData()
{
  if(shutdown) {
    LogGpiLine("--- gpilog exiting ---");
    for(unsigned i=0;i<gpio_monitors.size();i++) {
      if(gpio_monitors[i]->logfile()!=gpi_config->logfile()) {
	LogMonitor("--- gpilog exiting ---",gpio_monitors[i]);
      }
    }
    unlink(GPILOG_PID_FILE);
    exit(0);
  }
  if(restart) {
    Restart();
    Init();
    restart=false;
  }
}

/*
void GpiLog::LogGpiLine(const QString &str,int dev,int line) const
{
  FILE *file=NULL;
  QString ostr=QString().sprintf("%s | %s: %s\n",
	     (const char *)QDate::currentDate().toString("MM-dd-yyyy"),
	     (const char *)QTime::currentTime().toString("hh:mm:ss.zzz"),
	     (const char *)str);
  if(dev<0) {
    for(int i=0;i<gpi_config->uniqueLogfiles();i++) {
      file=fopen((const char *)gpi_config->uniqueLogfile(i),"a");
      if(file!=NULL) {
	fprintf(file,(const char *)ostr);
	fclose(file);
      }
    }
  }
  else {
    file=fopen((const char *)gpi_config->logfile(dev,line),"a");
    if(file!=NULL) {
      fprintf(file,(const char *)ostr);
      fclose(file);
    }
  }
}
*/
void GpiLog::LogGpiLine(const QString &str,int dev,int line) const
{
  QString ostr=QString().sprintf("%s | %s: %s\n",
	     (const char *)QDate::currentDate().toString("MM-dd-yyyy"),
	     (const char *)QTime::currentTime().toString("hh:mm:ss.zzz"),
	     (const char *)str);
  if(dev<0) {
    for(int i=0;i<gpi_config->uniqueLogfiles();i++) {
      LogLine(ostr,gpi_config->uniqueLogfile(i));
    }
  }
  else {
    LogLine(ostr,gpi_config->logfile(dev,line));
  }
}


void GpiLog::LogMonitor(const QString &str,GPIMonitor *mon) const
{
  QString ostr=QString().sprintf("%s | %s: %s\n",
	     (const char *)QDate::currentDate().toString("MM-dd-yyyy"),
	     (const char *)QTime::currentTime().toString("hh:mm:ss.zzz"),
	     (const char *)str);
  LogLine(ostr,mon->logfile());
}


void GpiLog::LogLine(const QString &str,const QString &logfile) const
{
  FILE *file=NULL;

  file=fopen((const char *)logfile,"a");
  if(file!=NULL) {
    fprintf(file,(const char *)str);
    fclose(file);
  }
}


void GpiLog::UpdateWatchdog(int wd)
{
  gpio_watchdogs[wd]->timeoutTimer()->stop();
  gpio_watchdogs[wd]->timeoutTimer()->start(gpio_watchdogs[wd]->
					    timeout(),true);
  if(gpio_watchdogs[wd]->active()) {
    gpio_watchdogs[wd]->setActive(false);
    LocalGpio *dev=gpio_watchdogs[wd]->resetDevice();
    int pin=gpio_watchdogs[wd]->resetPin();
    QString cmd=gpio_watchdogs[wd]->resetCommand();
    if((pin>=0)&&(pin<dev->outputs())) {
      dev->gpoSet(pin,1000);
    }
    if(!cmd.isEmpty()) {
      if(fork()==0) {
	system(cmd);
	exit(0);
      }
    }
    LogGpiLine(QString().sprintf("watchdog %d reset",wd));
  }
}


QString GpiLog::MakePrintableString(const char *data,int len) const
{
  QString ret;

  for(int i=0;i<len;i++) {
    if(isprint(data[i])) {
      ret+=data[i];
    }
    else {
      ret+=QString().sprintf("[%02X]",(int)data[i]);
    }
  }
  return ret;
}


void GpiLog::Init()
{
  //
  // Read Configuration
  //
  gpi_config=new GPIConfig(GPILOG_CONF_FILE);
  gpi_config->load();
  if(gpi_config->logfile().isEmpty()) {
    fprintf(stderr,"gpilog: no logfile specified\n");
    exit(1);
  }

  for(unsigned i=0;i<GPIO_MAX_DEVICES;i++) {
    //
    // Open Input Devices
    //
    if(!gpi_config->gpioDevice(i).isEmpty()) {
      gpio_devices.push_back(new LocalGpio(i,this));
      gpio_devices.back()->setDevice(gpi_config->gpioDevice(i));
      if(!gpio_devices.back()->open()) {
	if(initial_start) {
	  fprintf(stderr,"gpilog: unable to open gpio device %s\n",
		  (const char *)gpio_devices.back()->device());
	  exit(256);
	}
	else {
	  LogGpiLine(QString().
		  sprintf("unable to open gpio device %s",
			  (const char *)gpio_devices.back()->device()));
	}
      }
      for(int j=0;j<gpio_devices.back()->inputs();j++) {
	gpio_devices.back()->setTransition(j,gpi_config->transitionType(i,j));
      }

      //
      // Start Pulsers
      //
      for(int j=0;j<GPIO_MAX_LINES;j++) {
	if(gpi_config->pulsePeriod(i,j)>0) {
	  gpio_pulsers.
	    push_back(new GPIPulser(gpio_devices.back(),j,
				    gpi_config->pulsePeriod(i,j),this));
	}
      }
    }
  }

  
  //
  // Open Watchdog Devices
  //
  QSignalMapper *timeout_mapper=
    new QSignalMapper(this,"watchdog_timeout_mapper");
  connect(timeout_mapper,SIGNAL(mapped(int)),
	  this,SLOT(watchdogTimeoutData(int)));
  QSignalMapper *restart_mapper=
    new QSignalMapper(this,"watchdog_restart_mapper");
  connect(restart_mapper,SIGNAL(mapped(int)),
	  this,SLOT(watchdogRestartData(int)));
  for(unsigned i=0;i<gpi_config->watchdogs();i++) {
    gpio_watchdogs.push_back(new GPIWatchdog());
    for(unsigned j=0;j<gpi_config->watchdogTimeslots(i);j++) {
      gpio_watchdogs.back()->addSlot(gpi_config->watchdogStartTime(i,j),
				     gpi_config->watchdogEndTime(i,j));
    }
    gpio_watchdogs.back()->
      setTimeoutCommand(gpi_config->watchdogTimeoutCommand(i));
    gpio_watchdogs.back()->
      setResetCommand(gpi_config->watchdogResetCommand(i));
    gpio_watchdogs.back()->setTimeout(gpi_config->watchdogTimeout(i));
    gpio_watchdogs.back()->setTimeoutTimer(new QTimer(this));
    timeout_mapper->setMapping(gpio_watchdogs.back()->timeoutTimer(),i);
    connect(gpio_watchdogs.back()->timeoutTimer(),SIGNAL(timeout()),
	    timeout_mapper,SLOT(map()));
    gpio_watchdogs.back()->timeoutTimer()->
      start(gpio_watchdogs.back()->timeout(),true);
    gpio_watchdogs.back()->setRestartTimer(new QTimer(this));
    restart_mapper->setMapping(gpio_watchdogs.back()->restartTimer(),i);
    connect(gpio_watchdogs.back()->restartTimer(),SIGNAL(timeout()),
	    restart_mapper,SLOT(map()));
    gpio_watchdogs.back()->restartTimer()->
      start(gpio_watchdogs.back()->nextRestart(),true);
    for(unsigned j=0;j<gpio_devices.size();j++) {
      if(gpi_config->watchdogTimeoutDevice(i)==gpio_devices[j]->device()) {
	gpio_watchdogs.back()->setTimeoutDevice(gpio_devices[j]);
	gpio_watchdogs.back()->
	  setTimeoutPin(gpi_config->watchdogTimeoutPin(i));
      }
      if(gpi_config->watchdogResetDevice(i)==gpio_devices[j]->device()) {
	gpio_watchdogs.back()->setResetDevice(gpio_devices[j]);
	gpio_watchdogs.back()->setResetPin(gpi_config->watchdogResetPin(i));
      }
    }
  }

  //
  // Build Watchdogs-->Inputs Map
  //
  for(unsigned i=0;i<gpio_devices.size();i++) {
    for(int j=0;j<gpio_devices[i]->inputs();j++) {
      if(gpi_config->watchdog(i,j)>=0) {
	if(gpi_config->watchdog(i,j)<((int)gpio_watchdogs.size())) {
	  gpio_watchdogs[gpi_config->watchdog(i,j)]->addMonitor(i,j);
	}
	else {
	  LogGpiLine(QString().sprintf("configured watchdog %d doesn't exist",
				    gpi_config->watchdog(i,j)));
	}
      }
    }
  }
  gpi_watchdog_scan_timer->start(GPILOG_WATCHDOG_SCAN_INTERVAL);

  //
  // Load UDP Monitors
  //
  timeout_mapper=new QSignalMapper(this);
  connect(timeout_mapper,SIGNAL(mapped(int)),
	  this,SLOT(monitorTimeoutData(int)));
  for(unsigned i=0;i<gpi_config->monitors();i++) {
    gpio_monitors.push_back(new GPIMonitor());
    gpio_monitors.back()->setName(gpi_config->monitorName(i));
    gpio_monitors.back()->setTimeout(gpi_config->monitorTimeout(i));
    gpio_monitors.back()->
      setTimeoutCommand(gpi_config->monitorTimeoutCommand(i));
    gpio_monitors.back()->setResetCommand(gpi_config->monitorResetCommand(i));
    gpio_monitors.back()->setUdpPort(gpi_config->monitorUdpPort(i));
    gpio_monitors.back()->setLogfile(gpi_config->monitorLogfile(i));
    gpio_monitors.back()->setLogPackets(gpi_config->monitorLogPackets(i));
    gpio_monitors.back()->setLogTimeouts(gpi_config->monitorLogTimeouts(i));
    gpio_monitors.back()->setTimeoutTimer(new QTimer(this));
    timeout_mapper->setMapping(gpio_monitors.back()->timeoutTimer(),i);
    connect(gpio_monitors.back()->timeoutTimer(),SIGNAL(timeout()),
	    timeout_mapper,SLOT(map()));
    if(gpi_config->monitorTimeout(i)>0) {
      gpio_monitors.back()->
	timeoutTimer()->start(gpi_config->monitorTimeout(i),true);
    }
    gpio_monitors.back()->
      setSocketDevice(new QSocketDevice(QSocketDevice::Datagram));
    gpio_monitors.back()->socketDevice()->setBlocking(false);
    gpio_monitors.back()->socketDevice()->
      bind(QHostAddress(),gpi_config->monitorUdpPort(i));
    gpio_monitors.back()->
      setSocketNotifier(new QSocketNotifier(gpio_monitors.back()->
					    socketDevice()->socket(),
					    QSocketNotifier::Read));
    connect(gpio_monitors.back()->socketNotifier(),SIGNAL(activated(int)),
	    this,SLOT(monitorPacketData(int)));
  }

  //
  // Load IP Address Monitors
  //
  gpio_pinger=new IPv4Pinger(this);
  for(unsigned i=0;i<gpi_config->ipv4Monitors();i++) {
    gpio_ping_testers.push_back(new PingTester(gpi_config,i,gpio_pinger,this));
    connect(gpio_ping_testers.back(),SIGNAL(stateChanged(unsigned,bool)),
	    this,SLOT(ipv4StateChangedData(unsigned,bool)));
  }

  //
  // Log Startup
  //
  if(initial_start) {
    LogGpiLine("--- gpilog started ---");
    for(unsigned i=0;i<gpio_monitors.size();i++) {
      if(gpio_monitors[i]->logfile()!=gpi_config->logfile()) {
	LogMonitor("--- gpilog started ---",gpio_monitors[i]);
      }
    }
  }
  else {
    LogGpiLine("--- gpilog restarted ---");
    for(unsigned i=0;i<gpio_monitors.size();i++) {
      if(gpio_monitors[i]->logfile()!=gpi_config->logfile()) {
	LogMonitor("--- gpilog restarted ---",gpio_monitors[i]);
      }
    }
  }
  initial_start=false;

  //
  // Configure Signals
  //
  ::signal(SIGINT,SigHandler);
  ::signal(SIGTERM,SigHandler);
  ::signal(SIGHUP,SigHandler);

  gpi_startup_timer->start(1000,true);
}


void GpiLog::Restart()
{
  for(unsigned i=0;i<gpio_ping_testers.size();i++) {
    delete gpio_ping_testers[i];
  }
  gpio_ping_testers.clear();
  delete gpio_pinger;

  gpi_watchdog_scan_timer->stop();
  for(unsigned i=0;i<gpio_watchdogs.size();i++) {
    gpio_watchdogs[i]->timeoutTimer()->stop();
    delete gpio_watchdogs[i]->timeoutTimer();
    delete gpio_watchdogs[i]->restartTimer();
    delete gpio_watchdogs[i];
  }
  gpio_watchdogs.clear();
  for(unsigned i=0;i<gpio_pulsers.size();i++) {
    delete gpio_pulsers[i];
  }
  gpio_pulsers.clear();
  for(unsigned i=0;i<gpio_devices.size();i++) {
    gpio_devices[i]->close();
    delete gpio_devices[i];
  }
  gpio_devices.clear();
  for(unsigned i=0;i<gpio_monitors.size();i++) {
    delete gpio_monitors[i];
  }
  gpio_monitors.clear();
  delete gpi_config;
  gpi_config=NULL;
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new GpiLog(NULL,"GpiLog");
  return a.exec();
}
