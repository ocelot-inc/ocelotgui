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

# This has been tested on two Linux distros: Ubuntu 16.04 and Mageia 5.
# On Ubuntu 16.04 the prerequisite steps were:
#  sudo apt-get install build-essential libgl1-mesa-dev qtbase5-dev
#  sudo apt-get install libmysqlclient-dev
# On Mageia 5 the prerequisite steps were:
#  sudo urpmi cmake make gcc gcc-c++
#  sudo urpmi libqt5-devel
#  sudo urpmi mysql-devel
# Then on both Ubuntu 16.04 and Mageia 5 the steps are:
# cmake .
# make
# make install

# Qt4 or Qt5 dev packages must be installed first. See 
# http://doc.qt.io/qt-5/linux.html
# ... but that is not complete for Ubuntu, you also need qtbase5-dev.
# If your OS comes with Qt5, you must download Qt5 dev packages.
# If your OS comes with Qt4, you must download Qt4 dev packages.

# MySQL or MariaDB includes must be installed first.
# (Ubuntu) apt-get libmysqlclient-dev (Mageia) urpmi mysql-devel

# CMAKE_PREFIX_PATH
# This CmakeLists.txt file works if Qt + mysql/mariadb files are present.
# For Qt 5, it may require that cmake be started with -CMAKE_PREFIX_PATH.
# For example, 
# Suppose that Qt5WidgetsConfig.cmake is in this directory:
# /home/pgulutzan/Qt/5.4/gcc_64/lib/cmake/Qt5Widgets
# So do not say "cmake ." say
# cmake . -DCMAKE_PREFIX_PATH=/home/pgulutzan/Qt/5.4/gcc_64/lib/cmake/Qt5Widgets
# or wherever Qt5WidgetsConfig.cmake is to be found.
# If you have installed Qt5 development tools, you can always say
# sudo find / -name "Qt5Widgets"
# and within 10 minutes you'll find it.
# A probable default spot with Ubuntu is:
# /usr/lib/x86_64-linux-gnu/cmake/Qt5Widgets
# and sometimes, if it's in its default spot, FindQt5() will find it
# without any need for passing CMAKE_PREFIX_PATH.

# CMAKE_INSTALL_PREFIX
# When "make install" happens, files go to the system bin directory
# (which requires special privileges), or relative to CMAKE_INSTALL_PREFIX/

# e.g. to build and put results in ./bin, if CMAKE_PREFIX_PATH not needed, say
# cmake . -DCMAKE_PREFIX_PATH=/home/pgulutzan/Qt/5.4/gcc_64/lib/cmake/Qt5Widgets -DCMAKE_INSTALL_PREFIX=.
# make
# make install

# Todo: move above instructions + examples to the manual, not here
# Todo: allow user to pass -DWITH_QT4 or -DWITH_QT5 (but not both)
# Todo: allow user to pass -DMYSQL_INCLUDE_DIR
# Todo: allow user to pass -DCMAKE_INSTALL_RPATH
# Todo: try out a 32-bit platform

message("-- CmakeLists.txt for ocelotgui 0.9.0")
message("-- see comments in CMakeLists.txt for special instructions")
message("-- see README.md for the usual instructions for building")

cmake_minimum_required(VERSION 2.8.11)

project(ocelotgui)

# Find MYSQL include.
# Todo: consider using find_mysql_find stuff instead of this, which is our own coding.
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

# ocelotgui uses pthread. Some linkers might demand this.
target_link_libraries(ocelotgui pthread)

# QMAKE_RPATHDIR is where ocelotgui will search for libmysqlclient.so at run time,
# and CMAKE_INSTALL_RPATH is apparently the CMake translation of QMAKE_RPATHDIR,
# but first it will try several other ways.
# Start ocelotgui and say Help | libmysqlclient for details.
# This has never been tested.
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set_target_properties(ocelotgui PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)
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

FILE(GLOB png_files "${CMAKE_CURRENT_SOURCE_DIR}/*.png")
FILE(GLOB jpg_files "${CMAKE_CURRENT_SOURCE_DIR}/*.jpg")
FILE(GLOB htm_files "${CMAKE_CURRENT_SOURCE_DIR}/*.htm")
install(TARGETS ocelotgui RUNTIME DESTINATION bin)
install(FILES ${png_files} DESTINATION bin)
install(FILES ${jpg_files} DESTINATION bin)
install(FILES ${htm_files} DESTINATION bin)
install(FILES README.txt DESTINATION bin)
install(FILES README.md DESTINATION bin)
install(FILES options.txt DESTINATION bin)
install(FILES debugger_reference.txt DESTINATION bin)
install(FILES LICENSE.GPL DESTINATION bin)
install(FILES example.cnf DESTINATION bin)
# So the only original files not copied are the source files:
# CMakeLists.txt ocelotgui.pro ocelotgui.cpp ocelotgui.h
# readmylogin.c ocelotgui.ui install_sql.cpp

# To clean up:
# make clean
# rm CMakeCache.txt
# rm -r CMakeFiles
# rm cmake_install.cmake
