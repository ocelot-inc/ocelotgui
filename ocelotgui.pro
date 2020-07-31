#  Copyright (c) 2014-2020 by Ocelot Computer Services Inc. All rights reserved.
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
HEADERS += codeeditor.h
HEADERS += install_sql.cpp
HEADERS += hparse.h
HEADERS += ostrings.h
HEADERS += third_party.h

FORMS    += ocelotgui.ui

# INCLUDEPATH is where the compiler will search for mysql.h at build time.
# Replace this with the actual location of the file as supplied by MySQL or MariaDB.
# If build fails because mysql.h is not found:
#   () If you installed MySQL or MariaDB from a distro repository,
#      check that you installed the "dev" package
#      it might be called libmysqlclient-dev or libmariadbclient-dev
#   () If you can't remember whether it was installed, use
#      sudo find / -name "mysql.h"
#   When you find mysql.h, add its directory to this list.
INCLUDEPATH+= /usr/include/mysql
INCLUDEPATH+= /usr/local/include/mysql
INCLUDEPATH+= /usr/local/mysql/include
INCLUDEPATH+= /usr/local/mysql/include/mysql
INCLUDEPATH+= /opt/local/include/mysql
INCLUDEPATH+= /sw/include/mysql
INCLUDEPATH+= /usr/include/mariadb
INCLUDEPATH+= /usr/local/include/mariadb
INCLUDEPATH+= /usr/local/mariadb/include
INCLUDEPATH+= /usr/local/mariadb/include/mariadb
INCLUDEPATH+= /opt/local/include/mariadb
INCLUDEPATH+= /sw/include/mariadb

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
QMAKE_RPATHDIR += /usr/mariadb/lib
QMAKE_RPATHDIR += /usr/lib/mariadb
QMAKE_RPATHDIR += /usr/local/lib/mariadb
QMAKE_RPATHDIR += /usr/local/mariadb
QMAKE_RPATHDIR += /usr/local/mariadb/lib
contains(QMAKE_HOST.arch, x86_64) {
  QMAKE_RPATHDIR += /usr/lib64/mariadb
  }

# Installing by rebuilding source, with Qt Creator
# Ocelot makes all packages with cmake as described in the README,
# and recommends that others do so.
# However, here are additional notes for users who are
# particularly interested in using Qt Creator instead.
# </P>
# <P>
# Important license notice: when downloading Qt Creator,
# you may be confronted with a "License Agreement" dialog box,
# remember that ocelotgui is licensed under GPL version 2,
# so we recommend picking LGPL and re-conveying as GPL.
# Do not accept any request to grant any additional rights
# to Qt's manufacturer.
# </P>
# <P>Make sure a C++ compiler such as g++ is installed first.</P>
# <P>Edit [path]/ocelotgui.pro: change INCLUDEPATH to a path
# that includes the file "mysql.h". It comes with MySQL or
# MariaDB developer packages, for example "sudo apt-get
# libmysqlclient-dev" will put it in directory /usr/include/mysql.<PRE>
#  #Edit [path]/ocelotgui.pro.
#  #Make sure that any of the lines that begins with "QMAKE_RPATHDIR + ..."
#  #points to where libmysqlclient.so really is now.
#  #It comes with MySQL or MariaDB,
#  #for example "sudo apt-get mysqlclient" will put it
#  #in file/usr/lib/x86_64-linux-gnu/libmysqlclient.so.
#  #Example:
#  #"QMAKE_RPATHDIR += /usr/local/mysql/lib/mysql"
#  #Make sure that any of the lines that begins with "INCLUDEPATH+ ..."
#  #points to where mysql.h really is now. Example:
#  #"INCLUDEPATH += /usr/local/mysql/include/mysql"
#  #(mysql.h comes with MySQL or MariaDB developer packages.
#  # For example "sudo apt-get libmysqlclient-dev" will
#  # put it in directory /usr/include/mysql).</PRE></P>
# <P>You will need to state a compiler for the kit.
# Steps for Qt Creator use are:<PRE>
#  Click File | Open File or Project ...
#  In "Open File" dialog box, in field "File name:", enter [path]/ocelotgui.pro
#  Click Next
#  Click Configure Project</PRE></P>
# <P>If you get a message "cannot find -lGL" when you
# try to run the project, try to install GL.
# With Ubuntu: sudo apt-get install libgl1-mesa-dev.
# With Fedora: sudo yum install mesa-libGL-devel.</P>
# 
