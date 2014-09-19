#!/bin/sh
##
##    (C) Copyright 2002-2004  Fred Gleason <fredg@salemradiolabs.com>
##
##    Adapted from './autogen.sh' in the Jack Audio Connection Kit.
##    (C) Copyright 2001-2003 Paul Davis, et al.
##
##       $Id: autogen.sh,v 1.2 2006/10/20 02:26:23 fredg Exp $
##
##    This program is free software; you can redistribute it and/or modify
##    it under the terms of version 2 of the GNU General Public License as
##    published by the Free Software Foundation;
##
##    This program is distributed in the hope that it will be useful,
##    but WITHOUT ANY WARRANTY; without even the implied warranty of
##    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##    GNU General Public License for more details.
##
##    You should have received a copy of the GNU General Public License
##    along with this program; if not, write to the Free Software
##    Foundation, Inc., 59 Temple Place, Suite 330, 
##    Boston, MA  02111-1307  USA
##

aclocal $ACLOCAL_FLAGS || {
    echo "aclocal \$ACLOCAL_FLAGS where \$ACLOCAL_FLAGS= failed, exiting..."
    exit 1
}

automake --add-missing --foreign || {
    echo "automake --add-missing --foreign failed, exiting..."
    exit 1
}

autoconf || {
    echo "autoconf failed, exiting..."
    exit 1
}
