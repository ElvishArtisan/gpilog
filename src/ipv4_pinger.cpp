// ipv4_pinger.cpp
//
// IPv4 Address Monitor
//
//   (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
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

#include <netinet/in.h>
#include <stdlib.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>

#include <qhostaddress.h>

#include "ipv4_pinger.h"

IPv4Pinger::IPv4Pinger(QObject *parent)
  : QObject(parent)
{
  //
  // ICMP Socket
  //
  if((ipv4_socket=socket(AF_INET,SOCK_RAW,IPPROTO_ICMP))<0) {
    syslog(LOG_ERR,"unable to create ICMP socket");
    exit(255);
  }
  ipv4_notifier=new QSocketNotifier(ipv4_socket,QSocketNotifier::Read,this);
  connect(ipv4_notifier,SIGNAL(activated(int)),this,SLOT(readyReadData(int)));
}


IPv4Pinger::~IPv4Pinger()
{

  delete ipv4_notifier;
}


void IPv4Pinger::sendPing(const QHostAddress &addr,uint16_t seqno,uint16_t id,
			  const QByteArray &payload)
{
  char packet[1500];
  int s=GeneratePacket(packet,seqno,id,payload);
  struct sockaddr_in sa;
  memset(&sa,0,sizeof(sa));
  sa.sin_addr.s_addr=htonl(addr.toIPv4Address());
  sendto(ipv4_socket,packet,s,MSG_DONTWAIT,(struct sockaddr *)(&sa),sizeof(sa));
}


void IPv4Pinger::readyReadData(int sock)
{
  int n;
  char data[1500];
  struct sockaddr_in sa;
  socklen_t sa_size;

  memset(&sa,0,sizeof(sa));
  sa_size=sizeof(sa);
  while((n=recvfrom(ipv4_socket,data,1500,MSG_DONTWAIT,
		    (struct sockaddr *)(&sa),&sa_size))>0) {
    if(CheckPacket(data+20,n-20)) {
      emit echoReceived(QHostAddress(ntohl(sa.sin_addr.s_addr)),
			((0xFF&data[26])<<8)+(0xFF&data[27]),
			((0xFF&data[24])<<8)+(0xFF&data[25]),data+36,n-36);
    }
    memset(&sa,0,sizeof(sa));
    sa_size=sizeof(sa);
  }
}


int IPv4Pinger::GeneratePacket(char *data,uint16_t seqno,uint16_t id,
			       const QByteArray &payload)
{
  time_t t=time(NULL);
  int payload_size=payload.size();

  //
  // Assemble Packet
  //
  data[0]=0x08;             // Type
  data[1]=0x00;             // Code
  data[2]=0x00;             // Checksum
  data[3]=0x00;
  data[4]=0xFF&(id>>8);     // Identifier
  data[5]=0xFF&id;
  data[6]=0xFF&(seqno>>8);  // Sequence Number
  data[7]=0xFF&seqno;
  data[8]=(t>>24)&0xFF;
  data[9]=(t>>16)&0xFF;
  data[10]=(t>>8)&0xFF;
  data[11]=t&0xFF;
  data[12]=0x00;
  data[13]=0x00;
  data[14]=0x00;
  data[15]=0x00;
  if(payload.isNull()) {
    payload_size=48;
  }
  else {
    memcpy(data+16,payload.data(),payload_size);
  }

  //
  // Apply Checksum
  //
  uint16_t sum=Checksum(data,payload_size+16);
  data[2]=0xFF&(sum>>8);
  data[3]=0xFF&sum;

  return payload_size+16;
}


uint16_t IPv4Pinger::Checksum(const char *data,int len)
{
  uint32_t sum=0;

  for(int i=0;i<len;i+=2) {
    sum+=((0xFF&data[i])<<8)+(0xFF&data[i+1]);
  }
  sum+=(0xFFFF&(sum>>16));

  return ~sum;
}


bool IPv4Pinger::CheckPacket(const char *data,int len)
{
  return (data[0]==0x00)&&(Checksum(data,len)==0);
}
