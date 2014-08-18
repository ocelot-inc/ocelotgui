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

# Todo: don't hard code these paths, find where mysql.h and libmysqlclient.so should be!
# Todo: getenv("LD_RUN_PATH") is in the code, but unused.
INCLUDEPATH += /home/pgulutzan/mariadb-10.0.12/include/mysql
LIBS += /home/pgulutzan/mariadb-10.0.12//lib/libmysqlclient.so
# INCLUDEPATH += /home/pgulutzan/mysql-5.5/include
# LIBS += /home/pgulutzan/mysql-5.5/lib/libmysqlclient.so




