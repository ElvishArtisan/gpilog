// profile.cpp
//
// A class to read an ini formatted configuration file.
//
// (C) Copyright 2002-2003 Fred Gleason <fredg@salemradiolabs.com>
//
//    $Id: profile.cpp,v 1.1 2014/03/27 16:25:24 cvs Exp $
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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
//

#include <qfile.h>
#include <qtextstream.h>

#include <profile.h>


Profile::Profile()
{
}


QString Profile::source() const
{
  return profile_source;
}


bool Profile::setSource(QString filename)
{
  QString section;
  int offset;

  profile_source=filename;
  profile_section.resize(0);
  profile_section.push_back(ProfileSection());
  profile_section.back().setName("");
  QFile *file=new QFile(filename);
  if(!file->open(IO_ReadOnly)) {
    delete file;
    return false;
  }
  QTextStream *text=new QTextStream(file);
  QString line=text->readLine().stripWhiteSpace();
  while(!line.isNull()) {
    if((line.left(1)!=";")&&(line.left(1)!="#")) {
      if((line.left(1)=="[")&&(line.right(1)=="]")) {
	section=line.mid(1,line.length()-2);
	profile_section.push_back(ProfileSection());
	profile_section.back().setName(section);
      }
      else if(((offset=line.find('='))!=-1)) {
//      else if(((offset=line.find('='))!=-1)&&(!section.isEmpty())) {
	profile_section.back().
	  addValue(line.left(offset),
		   line.right(line.length()-offset-1).stripWhiteSpace());
      }
    }
    line=text->readLine().stripWhiteSpace();
  }
  delete text;
  delete file;
  return true;
}


QTime Profile::timeValue(QString section,QString tag,QTime default_value,
			 bool *ok) const
{
  bool local_ok=false;
  bool global_ok=true;
  int hour=0;
  int minute=0;
  int second=0;
  int n;
  QString str=stringValue(section,tag,"",&local_ok);
  if((!local_ok)||(str.contains(":")!=2)) {
    if(ok!=NULL) {
      *ok=false;
    }
    return default_value;
  }
  n=str.find(":");
  hour=str.left(n).toInt(&local_ok);
  global_ok&=local_ok;
  str=str.right(str.length()-n-1);
  n=str.find(":");
  minute=str.left(n).toInt(&local_ok);
  global_ok&=local_ok;
  second=str.right(str.length()-n-1).toInt(&local_ok);
  global_ok&=local_ok;
  if(!global_ok) {
    if(ok!=NULL) {
      *ok=false;
    }
    return default_value;
  }
  if(ok!=NULL) {
    *ok=true;
  }
  return QTime(hour,minute,second);
}


QString Profile::stringValue(QString section,QString tag,
			      QString default_str,bool *ok) const
{
  QString result;

  for(unsigned i=0;i<profile_section.size();i++) {
    if(profile_section[i].name()==section) {
      if(profile_section[i].getValue(tag,&result)) {
	if(ok!=NULL) {
	  *ok=true;
	}
	return result;
      }
      if(ok!=NULL) {
	*ok=false;
      }
      return default_str;
    }
  }
  if(ok!=NULL) {
    *ok=false;
  }
  return default_str;
}


int Profile::intValue(QString section,QString tag,
		       int default_value,bool *ok) const
{
  bool valid;

  int result=stringValue(section,tag).toInt(&valid,10);
  if(!valid) {
    if(ok!=NULL) {
      *ok=false;
    }
    return default_value;
  }
  if(ok!=NULL) {
    *ok=true;
  }
  return result;
}


int Profile::hexValue(QString section,QString tag,
		       int default_value,bool *ok) const
{
  bool valid;

  int result=stringValue(section,tag).toInt(&valid,16);
  if(!valid) {
    if(ok!=NULL) {
      *ok=false;
    }
    return default_value;
  }
  if(ok!=NULL) {
    *ok=true;
  }
  return result;
}


float Profile::floatValue(QString section,QString tag,
			   float default_value,bool *ok) const
{
  bool valid;

  float result=stringValue(section,tag).toDouble(&valid);
  if(!valid) {
    if(ok!=NULL) {
      *ok=false;
    }
    return default_value;
  }
  if(ok!=NULL) {
    *ok=true;
  }
  return result;
}


double Profile::doubleValue(QString section,QString tag,
			    double default_value,bool *ok) const
{
  bool valid;

  double result=stringValue(section,tag).toDouble(&valid);
  if(!valid) {
    if(ok!=NULL) {
      *ok=false;
    }
    return default_value;
  }
  if(ok!=NULL) {
    *ok=true;
  }
  return result;
}


bool Profile::boolValue(QString section,QString tag,
			 bool default_value,bool *ok) const
{
  bool valid;

  QString str=stringValue(section,tag,"",&valid).lower();
  if(!valid) {
    if(ok!=NULL) {
      *ok=false;
    }
    return default_value;
  }
  if((str=="yes")||(str=="true")||(str=="on")) {
    if(ok!=NULL) {
      *ok=true;
    }
    return true;
  }
  if((str=="no")||(str=="false")||(str=="off")) {
    if(ok!=NULL) {
      *ok=true;
    }
    return false;
  }
  if(ok!=NULL) {
    *ok=false;
  }
  return default_value;
}


void Profile::clear()
{
  profile_source="";
  profile_section.resize(0);
}
