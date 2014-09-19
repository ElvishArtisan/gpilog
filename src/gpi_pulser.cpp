// gpi_pulse.cpp
//
// Pulse a GPO Line
//
//   (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: gpi_pulser.cpp,v 1.1 2014/03/27 21:12:26 cvs Exp $
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

#include "gpi_pulser.h"

GPIPulser::GPIPulser(LocalGpio *gpio,int line,int period,QObject *parent)
  : QObject(parent)
{
  pulse_gpio=gpio;
  pulse_line=line;
  pulse_period=period;
  pulse_state=false;

  pulse_timer=new QTimer(this);
  connect(pulse_timer,SIGNAL(timeout()),this,SLOT(timeoutData()));
  pulse_timer->start(pulse_period);
}


GPIPulser::~GPIPulser()
{
  delete pulse_timer;
}


void GPIPulser::timeoutData()
{
  if(pulse_state) {
    pulse_gpio->gpoSet(pulse_line,0);
    pulse_state=false;
  }
  else {
    pulse_gpio->gpoReset(pulse_line,0);
    pulse_state=true;
  }
}
