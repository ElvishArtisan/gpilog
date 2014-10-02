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
             get_distro.sh\
             get_target.sh\
             gpilog.conf-sample\
             gpilog.spec.in\
             PACKAGE_VERSION\
             rc.gpilog.in

ACLOCAL_AMFLAGS = -I m4

maintainer-clean-local:	
	rm -rf m4

install-exec-local:	
	mkdir -p $(DESTDIR)/etc/init.d
	cp rc.gpilog $(DESTDIR)/etc/init.d/gpilog

uninstall-local:
	rm -f $(DESTDIR)/etc/init.d/gpilog

rpm:	dist
	cp gpilog-$(VERSION).tar.gz $(HOME)/rpmbuild/SOURCES/
	$(RPMBUILD) -ba --target $(ARCH)-$(VENDOR)-linux gpilog.spec
	mv $(HOME)/rpmbuild/RPMS/$(ARCH)/gpilog-*.rpm $(top_srcdir)/
	mv $(HOME)/rpmbuild/SRPMS/gpilog-*.src.rpm $(top_srcdir)/
	rm $(HOME)/rpmbuild/SOURCES/gpilog-$(VERSION).tar.gz
	rm -rf $(HOME)/rpmbuild/BUILD/gpilog-$(VERSION)

CLEANFILES = *~\
             *.rpm\
             moc_*
DISTCLEANFILES = depcomp\
                 install-sh\
                 missing mkinstalldirs\
                 gpilog.spec\
                 rc.gpilog
MAINTAINERCLEANFILES = *~\
                       *.tar.gz\
                       configure aclocal.m4\
                       Makefile.in
