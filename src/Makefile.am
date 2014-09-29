## automake.am
##
## Automake.am for the GPILog
##
## by Fred Gleason <fredg@paravelsystems.com>
##
##   (C) Copyright 2004, 2014 Fred Gleason <fredg@salemradiolabs.com>
##
##   This program is free software; you can redistribute it and/or modify
##   it under the terms of the GNU General Public License version 2 as
##   published by the Free Software Foundation.
##
##   This program is distributed in the hope that it will be useful,
##   but WITHOUT ANY WARRANTY; without even the implied warranty of
##   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##   GNU General Public License for more details.
##
##   You should have received a copy of the GNU General Public
##   License along with this program; if not, write to the Free Software
##   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
##
## Use automake to process this into a Makefile.in
##

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -DQTDIR=\"@QT_DIR@\" @QT_CXXFLAGS@ -I$(top_srcdir)/lib
LIBS = @QT_LIBS@ -L$(top_srcdir)/lib
MOC = @QT_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@

install-exec-local:	
	cp rc.gpilog /etc/init.d/gpilog

uninstall-local:
	rm -f /etc/init.d/gpilog

sbin_PROGRAMS = gpilog
dist_gpilog_SOURCES = gpi_config.cpp gpi_config.h\
                      gpi_monitor.cpp gpi_monitor.h\
                      gpi_pulser.cpp gpi_pulser.h\
                      gpi_watchdog.cpp gpi_watchdog.h\
                      gpilog.cpp gpilog.h\
                      ipv4_pinger.cpp ipv4_pinger.h\
                      local_gpio.cpp local_gpio.h\
                      ping_tester.cpp ping_tester.h\
                      profile.cpp profile.h\
                      profileline.cpp profileline.h\
                      profilesection.cpp profilesection.h
nodist_gpilog_SOURCES = moc_gpi_pulser.cpp\
                        moc_gpilog.cpp\
                        moc_ipv4_pinger.cpp\
                        moc_local_gpio.cpp\
                        moc_ping_tester.cpp

gpilog_LDADD = -lqui

rpm:	dist
	cp gpilog-$(VERSION).tar.gz $(RPM_ROOT)/SOURCES/
	$(RPMBUILD) -ba --target i586-$(VENDOR)-linux gpilog.spec
	mv $(RPM_ROOT)/RPMS/i586/gpilog-$(VERSION)-@RPM_RELEASE@.i586.rpm $(top_srcdir)/
	mv $(RPM_ROOT)/SRPMS/gpilog-$(VERSION)-@RPM_RELEASE@.src.rpm $(top_srcdir)/
	rm $(RPM_ROOT)/SOURCES/gpilog-$(VERSION).tar.gz
	rm -rf $(RPM_ROOT)/BUILD/gpilog-$(VERSION)

CLEANFILES = *~\
             *.rpm\
             moc_*
DISTCLEANFILES = depcomp\
                 install-sh\
                 missing mkinstalldirs\
                 gpilog.spec
MAINTAINERCLEANFILES = *~\
                       *.tar.gz\
                       configure aclocal.m4\
                       Makefile.in
