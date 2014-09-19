## automake.am
##
## Automake.am for the GPILog
##
## by Fred Gleason <fredg@salemradiolabs.com>
##
##   (C) Copyright 2004 Fred Gleason <fredg@salemradiolabs.com>
##
##      $Id: Makefile.am,v 1.12 2014/03/27 21:34:15 cvs Exp $
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

SUBDIRS = src

EXTRA_DIST = autogen.sh\
             gpilog.conf.example\
             gpilog.spec.in\
             PACKAGE_VERSION\
             rc.gpilog

ACLOCAL_AMFLAGS = -I m4

maintainer-clean-local:	
	rm -rf m4

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
