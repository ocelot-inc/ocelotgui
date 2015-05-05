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

SOURCES +=\
        ocelotgui.cpp 
HEADERS  += ocelotgui.h

FORMS    += ocelotgui.ui

# INCLUDEPATH is where the compiler will search for mysql.h at build time.
# Replace this with the actual location of the file as supplied by MySQL or MariaDB.
# Todo: don't hard code a single path, find where mysql.h should be!
INCLUDEPATH += /home/pgulutzan/mariadb-10.0.17/include/mysql

# QMAKE_RPATHDIR is where ocelotgui will search for libmysqlclient.so at run time,
# but first it will try several other ways.
# Start ocelotgui and say Help | libmysqlclient for details.
QMAKE_RPATHDIR += /usr/local/lib
QMAKE_RPATHDIR += /usr/mysql/lib
QMAKE_RPATHDIR += /usr/local/mysql/lib
QMAKE_RPATHDIR += /usr/lib
QMAKE_RPATHDIR += /usr/local/lib/mysql
QMAKE_RPATHDIR += /usr/lib/mysql
QMAKE_RPATHDIR += /usr/local
QMAKE_RPATHDIR += /usr/local/mysql
QMAKE_RPATHDIR += /usr/local
QMAKE_RPATHDIR += /usr







