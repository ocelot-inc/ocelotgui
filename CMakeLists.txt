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

# CMakeLists.txt for ocelotgui
# This CmakeLists.txt file works if Qt + mysql/mariadb files are present.
# For Qt 5, it requires that cmake be started with -CMAKE_PREFIX_PATH.
# For example, 
# Suppose that Qt5WidgetsConfig.cmake is in this directory:
# /home/pgulutzan/Qt/5.4/gcc_64/lib/cmake/Qt5Widgets
# So do not say "cmake ." say
# cmake . -DCMAKE_PREFIX_PATH=/home/pgulutzan/Qt/5.4/gcc_64/lib/cmake/Qt5Widgets
# or wherever Qt5WidgetsConfig.cmake is to be found.
# Todo: consider using find_mysql_find stuff instead of this, which is our own coding.

message("-- CmakeLists.txt has not been tested on many platforms")
message("-- Ocelot recommends using qmake and ocelotgui.pro instead")
message("-- see README.md for the usual instructions for building")

cmake_minimum_required(VERSION 2.8.11)

project(ocelotgui)

# Find MYSQL include.
# See comments in ocelotgui.pro for more explanation.
find_path(MYSQL_INCLUDE_DIR
   mysql.h
   PATHS
       /usr/include/mysql
       /usr/local/include/mysql
       /usr/local/mysql/include
       /usr/local/mysql/include/mysql
       /opt/local/include/mysql
       /sw/include/mysql
)
if (MYSQL_INCLUDE_DIR)
   message("-- found mysql.h in ${MYSQL_INCLUDE_DIR}")
else (MYSQL_INCLUDE_DIR)
   message(FATAL_ERROR "-- mysql.h is not found")
endif (MYSQL_INCLUDE_DIR)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#automoc requires "cmake_minimum_required(VERSION 2.8.6)" but that's okay.
set(CMAKE_AUTOMOC ON)

# Add MYSQL headers.
include_directories(${MYSQL_INCLUDE_DIR})

# -fPIE, but nobody has checked whether other flags are better
set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

# QUIET suppresses the following warning because we'll look for Qt4 instead:
# CMake Warning at CMakeLists.txt:56 (find_package):
#   By not providing "FindQt5Widget.cmake" in CMAKE_MODULE_PATH this project
#   has asked CMake to find a package configuration file provided by
#   "Qt5Widget", but CMake did not find one.
#   Could not find a package configuration file provided by "Qt5Widget" with
#   any of the following names:
#     Qt5WidgetConfig.cmake
#     qt5widget-config.cmake
#   Add the installation prefix of "Qt5Widget" to CMAKE_PREFIX_PATH or set
#   "Qt5Widget_DIR" to a directory containing one of the above files.  If
#   "Qt5Widget" provides a separate development package or SDK, be sure it has
#   been installed.
find_package(Qt5Widgets QUIET)

if (Qt5Widgets_FOUND)
  message("-- found Qt5")
  #Produce ui_ocelotgui.h from ocelotgui.ui
  qt5_wrap_ui(UI_OCELOTGUI ocelotgui.ui)
else (Qt5Widgets_FOUND)
  find_package(Qt4 QUIET)
  if (Qt4_FOUND)
    message("-- found Qt4")
    qt4_wrap_ui(UI_OCELOTGUI ocelotgui.ui)
  else (Qt4_FOUND)
    message(FATAL_ERROR "Qt5 not found, Qt4 not found")
  endif (Qt4_FOUND)
endif (Qt5Widgets_FOUND)

if (NOT UI_OCELOTGUI)
  message(FATAL_ERROR "uic did not produce ui_ocelotgui.h")
endif (NOT UI_OCELOTGUI)

add_executable(ocelotgui ${UI_OCELOTGUI} ocelotgui.cpp)

# ocelotgui uses dlopen + dlsym. Some linkers might demand this.
target_link_libraries(ocelotgui dl)

# QMAKE_RPATHDIR is where ocelotgui will search for libmysqlclient.so at run time,
# and CMAKE_INSTALL_RPATH is apparently the CMake translation of QMAKE_RPATHDIR,
# but first it will try several other ways.
# Start ocelotgui and say Help | libmysqlclient for details.
# This has never been tested.
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
if (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
  message("-- 64-bit")
  set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH}:/usr/mysql/lib:/usr/lib:/usr/lib/mysql:/usr/local:/usr/local/lib:/usr/local/lib/mysql:/usr/local/mysql:/usr/local/mysql/lib:/usr:/usr/lib/x86_64-linux-gnu:/usr/lib64:/usr/lib64/mysql")
else (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
  message("-- 32-bit")
  set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH}:/usr/mysql/lib:/usr/lib:/usr/lib/mysql:/usr/local:/usr/local/lib:/usr/local/lib/mysql:/usr/local/mysql:/usr/local/mysql/lib:/usr:/usr/lib/i386-linux-gnu")            
endif (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")

if (Qt5Widgets_FOUND)
  target_link_libraries(ocelotgui Qt5::Widgets)
else (Qt5Widgets_FOUND)
  target_link_libraries(ocelotgui Qt4::QtGui)
endif (Qt5Widgets_FOUND)
