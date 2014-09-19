// gpi_pulse.h
//
// Pulse a GPO Line
//
//   (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: gpi_pulser.h,v 1.1 2014/03/27 21:12:27 cvs Exp $
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

#ifndef GPI_PULSER_H
#define GPI_PULSER_H

#include <qobject.h>
#include <qtimer.h>

#include <local_gpio.h>

class GPIPulser : public QObject
{
  Q_OBJECT;
 public:
  GPIPulser(LocalGpio *gpio,int line,int period,QObject *parent);
  ~GPIPulser();

 private slots:
  void timeoutData();

 private:
  LocalGpio *pulse_gpio;
  int pulse_line;
  int pulse_period;
  bool pulse_state;
  QTimer *pulse_timer;
};


#endif  // GPI_PULSER_H
