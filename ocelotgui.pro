#  Copyright (c) 2014 by Ocelot Computer Services Inc. All rights reserved.
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; version 2 of the License.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ocelotgui
TEMPLATE = app

CONFIG += warn_on
QMAKE_CFLAGS_WARN_ON    = -Wall -Wextra
QMAKE_CXXFLAGS_WARN_ON  = -Wall -Wextra

SOURCES += ocelotgui.cpp 
HEADERS += ocelotgui.h

FORMS    += ocelotgui.ui

# INCLUDEPATH is where the compiler will search for mysql.h at build time.
# Replace this with the actual location of the file as supplied by MySQL or MariaDB.
# If build fails because mysql.h is not found:
#   () If you installed MySQL or MariaDB from a distro repository,
#      check that you installed the "dev" package
#      it might be called libmysqlclient-dev
#   () If you can't remember whether it was installed, use
#      sudo find / -name "mysql.h"
#   When you find mysql.h, add its directory to this list.
INCLUDEPATH+= /usr/include/mysql
INCLUDEPATH+= /usr/local/include/mysql
INCLUDEPATH+= /usr/local/mysql/include
INCLUDEPATH+= /usr/local/mysql/include/mysql
INCLUDEPATH+= /opt/local/include/mysql
INCLUDEPATH+= /sw/include/mysql

# ocelotgui uses dlopen + dlsym. Some linkers might demand this.
LIBS += -ldl

# QMAKE_RPATHDIR is where ocelotgui will search for libmysqlclient.so at run time,
# but first it will try several other ways.
# Start ocelotgui and say Help | libmysqlclient for details.
QMAKE_RPATHDIR += /usr/mysql/lib
QMAKE_RPATHDIR += /usr/lib
QMAKE_RPATHDIR += /usr/lib/mysql
QMAKE_RPATHDIR += /usr/local
QMAKE_RPATHDIR += /usr/local/lib
QMAKE_RPATHDIR += /usr/local/lib/mysql
QMAKE_RPATHDIR += /usr/local/mysql
QMAKE_RPATHDIR += /usr/local/mysql/lib
QMAKE_RPATHDIR += /usr
contains(QMAKE_HOST.arch, x86_64) {
  QMAKE_RPATHDIR += /usr/lib/x86_64-linux-gnu
  QMAKE_RPATHDIR += /usr/lib64
  QMAKE_RPATHDIR += /usr/lib64/mysql
  } else {
  QMAKE_RPATHDIR += /usr/lib/i386-linux-gnu
  }
