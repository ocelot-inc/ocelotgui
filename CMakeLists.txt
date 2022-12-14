# Copyright (c) 2022 by Peter Gulutzan. All rights reserved.
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
# Always say "make clean" and "rm CMakeCache.txt" before running cmake.

# This has been tested on Linux distros such as Ubuntu 16.04 and Mageia 5.
# On Ubuntu 16.04 the prerequisite steps were:
#  sudo apt-get install build-essential libgl1-mesa-dev qtbase5-dev
#  sudo apt-get install libmysqlclient-dev
# On PureOS 8 the prerequisite steps were:
# sudo apt-get install cmake g++ default-libmysqlclient-dev qtbase5-dev
# On Mageia 5 the prerequisite steps were:
#  sudo urpmi cmake make gcc gcc-c++
#  sudo urpmi libqt5-devel
#  sudo urpmi mysql-devel
# Then on both Ubuntu 16.04 and Mageia 5 the steps are:
# make clean
# rm CMakeCache.txt
# cmake .                       # but see below for description of options
# make
# make install                  # might require root privileges
# (Steps are different when generating .deb or .rpm, see comments later.)

# Qt4 or Qt5 or Qt6 dev packages must be installed first. See 
# http://doc.qt.io/qt-5/linux.html
# ... but that is not complete for Ubuntu, you also need qtbase5-dev.
# If your OS comes with Qt5, you should download Qt5 dev packages.
# If your OS comes with Qt4, you should download Qt4 dev packages.
# If your OS comes with Qt6, you should download Qt6 dev packages.
# If your os is unknown then try "pkg-config --modversion QtCore"
# or look in /usr/lib/pkgconfig files.
# For more instructions see "Qt for Linux/X11"
# http://doc.qt.io/qt-5/linux.html.
# For more instructions about Qt6 see "Get started with CMake"
# https://doc.qt.io/qt-6/cmake-get-started.html.
# If you have qt4, but findQt4() fails, a symbolic link might work, e.g.:
# sudo ln -s  /usr/lib64/qt4/bin/qmake /usr/bin/qmake-qt4
# If you have qt6, but find_package(qt6Widgets ...) fails, you might try:
# qmake-qt6 followed by make.

# CMAKE_PREFIX_PATH=<string>
# This must be the path where the Qt development files were installed.
# For Qt4, if it's in a default spot, findQt4() will find.
# For Qt5, it is not automatic, there is no findQt5().
# Therefore, if you have installed Qt5 development tools ...
#   (1) Consider using -DQT_VERSION=5 so that there's no attempt to find Qt4.
#   (2) The essential file is named Qt5WidgetsConfig.cmake, which is in a
#       directory named Qt5Widgets. If it's in a default spot, this works:
#       find /usr/lib -name "Qt5Widgets"
#       If it's in a non-default spot, this works although it's slow:
#       sudo find / -name "Qt5Widgets"
#       A probable default spot with Ubuntu is:
#       /usr/lib/x86_64-linux-gnu/cmake/Qt5Widgets
# For example, Suppose that Qt5WidgetsConfig.cmake is in this directory:
# /home/pgulutzan/Qt/5.4/gcc_64/lib/cmake/Qt5Widgets
# In that case instead of "cmake ." one would say
# cmake . -DCMAKE_PREFIX_PATH=/home/pgulutzan/Qt/5.4/gcc_64/lib/cmake/Qt5Widgets
# or wherever Qt5WidgetsConfig.cmake is to be found.

# CMAKE_INSTALL_PREFIX=<string>
# When "make install" happens, files go to the system bin directory
# (which requires special privileges), or relative to CMAKE_INSTALL_PREFIX/

# MYSQL_INCLUDE_DIR=<string>
# Directory to search first for mysql.h.
# In debian/rules this is calculated by looking in the installed MySQL/MariaDB package.

# CMAKE_SKIP_RPATH=TRUE|FALSE and CMAKE_INSTALL_RPATH
# By default we have rpath. But from debian/rules and ocelotgui.spec we pass -DCMAKE_SKIP_RPATH=TRUE.
# For QMake, QMAKE_RPATHDIR is where ocelotgui will search for libmysqlclient.so at run time.
# CMAKE_INSTALL_RPATH is apparently the CMake translation of QMAKE_RPATHDIR,
# but first it will try several other ways.
# It does not replace our hard-coded defaults -- we append the hard-coded defaults to it.
# Start ocelotgui and say Help | libmysqlclient for details.
# In fact there may be an effect on RUNPATH, overriding RUNPATH.
# If you change this, test with "readelf -d ocelotgui | grep PATH".
# Remember that .deb and .rpm packagers suggest avoiding rpath e.g.
# https://en.opensuse.org/openSUSE:Packaging_Guidelines#Beware_of_Rpath
# I guess rpath gets stripped anyway but I think I'm passing a flag

# CMAKE_BUILD_TYPE=<string>
# Typical alternatives: Debug, RelWithDebInfo, Release.
# If -DCMAKE_BUILD_TYPE=... is not specified, then probably it's blank, and in
# that case we set it to RelWithDebInfo, and the main effect of that is that
# CXXFLAGS will have "-O2" added to it. We do not do that for Windows though.
# For Windows, i.e. if MinGW, we say it's Debug no matter what the user passes,
# because otherwise ocelotgui will crash. Call it a kludge because we don't know why.

# OCELOT_THIRD_PARTY=n
# Say -DOCELOT_THIRD_PARTY=0 if you do not want to include third_party.h
# By default it is included.
# If it is not included, and ocelotgui is started with --ocelot_dbms=tarantool,
# then libtarantool.so will be sought.

# QT_VERSION=4 or QT_VERSION=5 or QT_VERSION=6
# If -DQT_VERSION=4: Search only for Qt4, do not search for Qt5 first.
# If -DQT_VERSION=5: Search only for Qt5, do not search for Qt4 afterwards.
# If -DQT_VERSION=6: Search only for Qt6, do not search for Qt5 or Qt4.
# If not specified or (not 4 and not 5 and not 6): Search for Qt5 first, then search for Qt4, then search for Qt6.

# PACKAGE_TYPE="DEB" or PACKAGE_TYPE="RPM"
#   DEB:
#     This is the pass specified in debian/rules, we don't use it for cmake standalone.
#     cpack_package_file_name = package_version-release_arch.deb
#     Do not install COPYING file or LICENSE.GPL file.
#     This is to avoid lintian warning "extra-license-file".
#     We do not try to make .gz files, leave that to debuild.
#   RPM:
#     In ocelotgui.spec we say -DPACKAGE_TYPE="RPM" but we might not use it here.

# OCELOT_C_FLAGS and OCELOT_CXX_FLAGS and OCELOT_CPP_FLAGS
# These will override any environment variables or Qt defaults for C_FLAGS etc. if PACKAGE_TYPE="DEB".
# See debian/rules.
# Ordinarily the environment variables are blank and the Qt default is blank or -fPIE.
# Something can add -fPIC anyway.

# OCELOT_LD_FLAGS
# This might contain "-pie".

# OCELOT_MYSQL_DEBUGGER
# By default this is 1. If it is 0, code related to the debugger is not included.

# OCELOT_MYSQL_INCLUDE
# By default this is 1. If it is 0, most code related to MySQL/MariaDB is not included.

# DEFAULT_OPTION=<string>
# The executable can read the <string> and set one default option before looking at the
# command line or the options file. There is no check that the <string> makes sense.
# A possibility is cmake . -DDEFAULT_OPTION="--ocelot_dbms=mariadb" which would
# mean the default default ocelot_dbms value is mariadb rather than hard-coded mysql.

# FILENAME_EXTRA
# Default is blank. ocelotgui passes -DFILENAME_EXTRA="qt4" when building with qt4,
# so the final package name will look like ocelotgui_1.8.0qt4-1_amd64.deb or
# ocelotgui-1.8.0qt4-1.x86_64.rpm. Ordinarily it should not be specified.

# FILENAME_RELEASE
# Default is "1". Ordinarily it doesn't need to be changed.

# e.g. to build and put results in ./bin, if CMAKE_PREFIX_PATH not needed, say
# cmake . -DCMAKE_PREFIX_PATH=/home/pgulutzan/Qt/5.4/gcc_64/lib/cmake/Qt5Widgets -DCMAKE_INSTALL_PREFIX=.
# make
# make install

# e.g. to generate .deb file and install with dpkg:
# deb_build.sh, which copies files and invokes debian/rules, which uses CMakeLists.txt
# sudo dpkg -i ocelotgui_1.8.0-1_amd64.deb
# but see also comments in deb_build.sh and debian/README.Debian

# e.g. to generate .rpm file and install with rpm:
# rpm_build.sh, which copies files and invokes ocelotgui.spec, which uses CMakeLists.txt
# rpmlint ocelotgui-1.8.0-1.x86_64.rpm
# rpm -i ocelotgui-1.8.0-1.x86_64.rpm
# but see also comments in rpm_build.sh

# Todo: We only check if CMAKE_SYSTEM_PROCESSOR = x86_64. On FreeBSD 12, uname -p says amd64.
# Todo: Maybe set(CMAKE_CXX_FLAGS "${Qt6Widgets_EXECUTABLE_COMPILE_FLAGS}") as we do for Qt5Widgets.
# Todo: We do not install README_tarantool.htm or the *_tarantool.png files, though we put them on github.
# Todo: We do not install ocelotgui-logo.png, we only install ocelotgui_logo.png.

message("-- CmakeLists.txt for ocelotgui 1.8.0")
message("-- see comments in CMakeLists.txt for special instructions")
message("-- see README.md for general instructions for building")

if (NOT CMAKE_GENERATOR STREQUAL "MinGW Makefiles")
if ("${CMAKE_BUILD_TYPE}" STREQUAL "")
  message("-- Setting CMAKE_BUILD_TYPE RelWithDebInfo")
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif ("${CMAKE_BUILD_TYPE}" STREQUAL "")
endif (NOT CMAKE_GENERATOR STREQUAL "MinGW Makefiles")

if (CMAKE_GENERATOR STREQUAL "MinGW Makefiles")
  message("-- Setting CMAKE_BUILD_TYPE Debug")
  set(CMAKE_BUILD_TYPE "Debug")
endif (CMAKE_GENERATOR STREQUAL "MinGW Makefiles")

cmake_minimum_required(VERSION 2.8.12)

project(ocelotgui)

if (DEFINED OCELOT_MYSQL_INCLUDE)
  set(TMP_OCELOT_MYSQL_INCLUDE ${OCELOT_MYSQL_INCLUDE})
else (DEFINED OCELOT_MYSQL_INCLUDE)
  set(TMP_OCELOT_MYSQL_INCLUDE TRUE)
endif (DEFINED OCELOT_MYSQL_INCLUDE)

if (TMP_OCELOT_MYSQL_INCLUDE)
# Find MYSQL include.
# The suggestion at https://cmake.org/Wiki/CMakeUserFindMySQL has too few choices.
# See comments in ocelotgui.pro for more explanation.
# First time, use MYSQL_INCLUDE_DIR in case user specified -DMYSQL_INCLUDE_DIR
# or it was already in cache (we don't assume CMakeCache.txt is removed).
# If that fails, rather than error, note the failure and try further ...
# Second time, use hard-coded list "/usr/include/mysql" etc.
# without specifying NO_DEFAULT_PATH so CMake searches its default
# directories first so quite often the hard-coded list doesn't matter.
# We don't need this if "DEB" because in debian/rules we find package's path.
unset(MYSQL_INCLUDE_DIR_RESULT)
unset(MYSQL_INCLUDE_DIR_SEARCH_PATH)
if (MYSQL_INCLUDE_DIR)
  find_path(MYSQL_INCLUDE_DIR_RESULT mysql.h PATHS ${MYSQL_INCLUDE_DIR} NO_DEFAULT_PATH)
  if (NOT MYSQL_INCLUDE_DIR_RESULT)
    message("-- mysql.h not found in MYSQL_INCLUDE_DIR ${MYSQL_INCLUDE_DIR}")
  endif (NOT MYSQL_INCLUDE_DIR_RESULT)
endif (MYSQL_INCLUDE_DIR)
if (NOT MYSQL_INCLUDE_DIR_RESULT)
  set(MYSQL_INCLUDE_DIR_SEARCH_PATH "/usr/include/mysql"
       "/usr/include/mysql"
       "/usr/local/include/mysql"
       "/usr/local/mysql/include"
       "/usr/local/mysql/include/mysql"
       "/opt/local/include/mysql"
       "/sw/include/mysql"
       "/usr/include/mariadb"
       "/usr/local/include/mariadb"
       "/usr/local/mariadb/include"
       "/usr/local/mariadb/include/mariadb"
       "/opt/local/include/mariadb"
       "/sw/include/mariadb"
  )
  find_path(MYSQL_INCLUDE_DIR_RESULT mysql.h PATHS ${MYSQL_INCLUDE_DIR_SEARCH_PATH})
endif (NOT MYSQL_INCLUDE_DIR_RESULT)
if (MYSQL_INCLUDE_DIR_RESULT)
  message("-- mysql.h found in ${MYSQL_INCLUDE_DIR_RESULT}")
else (MYSQL_INCLUDE_DIR_RESULT)
  message(FATAL ERROR "-- mysql.h not found in CMake default path or ocelotgui hard-coded path")
endif (MYSQL_INCLUDE_DIR_RESULT)
endif (TMP_OCELOT_MYSQL_INCLUDE)

# Make sure .h files in current directory are found e.g. ocelotgui.h or what uic produces
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#automoc requires "cmake_minimum_required(VERSION 2.8.6)" but that's okay.
set(CMAKE_AUTOMOC ON)

if (TMP_OCELOT_MYSQL_INCLUDE)
# Add MYSQL headers.
include_directories(${MYSQL_INCLUDE_DIR_RESULT})
endif (TMP_OCELOT_MYSQL_INCLUDE)

#lintian complained about "unstripped-binary-or-object"
# SET(CMAKE_BUILD_TYPE "Release" was useless
# We also should say, after "make": strip --remove-section=.comment ocelotgui
# Todo: check: does this override a user who wants CMAKE_BUILD_TYPE debug?
if(CMAKE_COMPILER_IS_GNUCXX)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s")
endif(CMAKE_COMPILER_IS_GNUCXX)

#rpmlint complained about position-independent-executable-suggested
#This only happened with SUSE which didn't say -pie for the linker.
#So in ocelotgui.spec we have SUSE-specific pass of -DOCELOT_LD_FLAGS:STRING="-pie"
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OCELOT_LD_FLAGS}")

#Typically this says [/usr/local] bin, share/doc/ocelotgui, man
#We commented out the messages because they're misleading,
#they say /usr/local etc. but if not local install they're /usr etc.
#(Actually they appear to be correct if one builds via debian/rules.)
include(GNUInstallDirs)
#message("-- bin will go to ${CMAKE_INSTALL_FULL_BINDIR}")
#message("-- doc will go to ${CMAKE_INSTALL_FULL_DOCDIR}")
#message("-- man will go to ${CMAKE_INSTALL_FULL_MANDIR}")

#Help|Manual will look at this path
add_definitions(-DOCELOTGUI_DOCDIR="${CMAKE_INSTALL_FULL_DOCDIR}")

#If cmake . -DOCELOT_THIRD_PARTY=0 then #include third_party.h won't happen
if (DEFINED OCELOT_THIRD_PARTY)
add_definitions(-DOCELOT_THIRD_PARTY=${OCELOT_THIRD_PARTY})
endif(DEFINED OCELOT_THIRD_PARTY)

#If cmake . -DOCELOT_MYSQL_DEBUGGER=0 then code related to the debugger will not be included
if (DEFINED OCELOT_MYSQL_DEBUGGER)
add_definitions(-DOCELOT_MYSQL_DEBUGGER=${OCELOT_MYSQL_DEBUGGER})
endif(DEFINED OCELOT_MYSQL_DEBUGGER)

#If cmake . -DOCELOT_MYSQL_INCLUDE=0 then most code related to MySQL/MariaDB will not be included
if (DEFINED OCELOT_MYSQL_INCLUDE)
add_definitions(-DOCELOT_MYSQL_INCLUDE=${OCELOT_MYSQL_INCLUDE})
endif(DEFINED OCELOT_MYSQL_INCLUDE)

#if cmake . -DOCELOT_OPTION=string then ocelotgui will treat string like a command-line option
if (DEFINED DEFAULT_OPTION)
add_definitions(-DDEFAULT_OPTION="${DEFAULT_OPTION}")
endif(DEFINED DEFAULT_OPTION)

if (DEFINED QT_VERSION)
  message("-- QT_VERSION is defined")
  if (QT_VERSION EQUAL "5")
    find_package(Qt5Widgets REQUIRED)
  elseif(QT_VERSION EQUAL "4")
    find_package(Qt4Widgets REQUIRED)
  elseif(QT_VERSION EQUAL "6")
    find_package(Qt6Widgets REQUIRED)
  else()
    message(FATAL_ERROR "QT_VERSION was specified but was not 5 or 4 or 6") 
  endif(QT_VERSION EQUAL "5")
else (DEFINED QT_VERSION)
  # QT_VERSION is not specified so we'll look for 5 then 6 then 4
  # QUIET suppresses the following warning because we'll look for another version instead:
  # CMake Warning at CMakeLists.txt:n (find_package):
  # By not providing "FindQt5Widget.cmake" in CMAKE_MODULE_PATH this project
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
  if (NOT Qt5Widgets_FOUND)
    find_package(Qt4Widgets QUIET)
    if (NOT Qt4Widgets_FOUND)
     find_package(Qt6Widgets QUIET)
     if (NOT Qt6Widgets_FOUND)
       message(FATAL_ERROR "QT_VERSION not specified, Qt5 not found, Qt4 not found, Qt6 not found")
     endif(NOT Qt6Widgets_FOUND)
    endif(NOT Qt4Widgets_FOUND)
  endif(NOT Qt5Widgets_FOUND)
endif(DEFINED QT_VERSION)

if (Qt5Widgets_FOUND)
  message("-- found Qt5")
  # -fPIE, but nobody has checked whether other flags are better
  # (some other flags depend on CMAKE_BUILD_TYPE)
  # (The ocelotgui developer uses ocelotgui.pro which has -Wall -O2)
  set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
  #Produce ui_ocelotgui.h from ocelotgui.ui
  qt5_wrap_ui(UI_OCELOTGUI ocelotgui.ui)
endif(Qt5Widgets_FOUND)
if (Qt4Widgets_FOUND)
  message("-- found Qt4")
  qt4_wrap_ui(UI_OCELOTGUI ocelotgui.ui)
endif(Qt4Widgets_FOUND)
if (Qt6Widgets_FOUND)
  message("-- found Qt6")
  qt6_wrap_ui(UI_OCELOTGUI ocelotgui.ui)
endif(Qt6Widgets_FOUND)

#MinGW ignores #pragmas in third_party.h
if (CMAKE_GENERATOR STREQUAL "MinGW Makefiles")
 set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -Wno-nonnull -Wno-pointer-arith -Wno-conversion")
endif (CMAKE_GENERATOR STREQUAL "MinGW Makefiles")

#If OCELOT_xxx_FLAGS is passed for a Debian package, then override environment variables and Qt5 setting.
#Maybe I could also check CMAKE_CXX_COMPILER should be gcc and CMAKE_CXXCOMPILER_VERSION should be >= something.
if (PACKAGE_TYPE STREQUAL "DEB" OR PACKAGE_TYPE STREQUAL "RPM")
if(CMAKE_COMPILER_IS_GNUCXX)
if (OCELOT_C_FLAGS)
  message("-- Setting CMAKE_C_FLAGS=OCELOT_C_FLAGS+OCELOT_CPP_FLAGS, CMAKE_CXX_FLAGS=OCELOT_CXX_FLAGS+OCELOt-CPP_FLAGS")
  set(CMAKE_C_FLAGS "${OCELOT_C_FLAGS} ${OCELOT_CPP_FLAGS}")
  set(CMAKE_CXX_FLAGS "${OCELOT_CXX_FLAGS} ${OCELOT_CPP_FLAGS}")
endif (OCELOT_C_FLAGS)
endif(CMAKE_COMPILER_IS_GNUCXX)
endif (PACKAGE_TYPE STREQUAL "DEB" OR PACKAGE_TYPE STREQUAL "RPM")

message("-- CMAKE_CXX_FLAGS  = ${CMAKE_CXX_FLAGS}")
message("-- CMAKE_C_FLAGS  = ${CMAKE_C_FLAGS}")

if (NOT UI_OCELOTGUI)
  message(FATAL_ERROR "uic did not produce ui_ocelotgui.h")
endif (NOT UI_OCELOTGUI)

add_executable(ocelotgui ${UI_OCELOTGUI} ocelotgui.cpp)

if (NOT CMAKE_GENERATOR STREQUAL "MinGW Makefiles")
# ocelotgui uses dlopen + dlsym. Some linkers might demand this.
target_link_libraries(ocelotgui dl)
endif (NOT CMAKE_GENERATOR STREQUAL "MinGW Makefiles")

# ocelotgui uses pthread. Some linkers might demand this.
target_link_libraries(ocelotgui pthread)

if (CMAKE_GENERATOR STREQUAL "MinGW Makefiles")
target_link_libraries(ocelotgui ws2_32)
endif (CMAKE_GENERATOR STREQUAL "MinGW Makefiles")

if (TMP_OCELOT_MYSQL_INCLUDE)
# See comments at the start of this file regarding CMAKE_SKIP_RPATH and CMAKE_INSTALL_RPATH.
if (DEFINED CMAKE_SKIP_RPATH)
  set(TMP_CMAKE_SKIP_RPATH ${CMAKE_SKIP_RPATH})
else (DEFINED CMAKE_SKIP_RPATH)
  set(TMP_CMAKE_SKIP_RPATH FALSE)
endif (DEFINED CMAKE_SKIP_RPATH)
if (NOT TMP_CMAKE_SKIP_RPATH)
  if (NOT CMAKE_INSTALL_RPATH)
    set(TMP_INSTALL_RPATH "")
  else (NOT CMAKE_INSTALL_RPATH)
    set(TMP_INSTALL_RPATH "${CMAKE_INSTALL_RPATH}")
  endif (NOT CMAKE_INSTALL_RPATH)
  if (TMP_INSTALL_RPATH STREQUAL "")
    #
  else (TMP_INSTALL_RPATH STREQUAL "")
    set(TMP_INSTALL_RPATH "${TMP_INSTALL_RPATH}:")
  endif (TMP_INSTALL_RPATH STREQUAL "")
  if (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
    message("-- 64-bit")
    set(TMP_INSTALL_RPATH "${TMP_INSTALL_RPATH}/usr/mysql/lib:/usr/lib:/usr/lib/mysql:/usr/local:/usr/local/lib:/usr/local/lib/mysql:/usr/local/mysql:/usr/local/mysql/lib:/usr:/usr/lib/x86_64-linux-gnu:/usr/lib64:/usr/lib64/mysql:/usr/mariadb/lib:/usr/lib/mariadb:/usr/local/lib/mariadb:/usr/local/mariadb:/usr/local/mariadb/lib:/usr/lib64/mariadb")
  else (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
    message("-- 32-bit")
    set(TMP_INSTALL_RPATH "${TMP_INSTALL_RPATH}/usr/mysql/lib:/usr/lib:/usr/lib/mysql:/usr/local:/usr/local/lib:/usr/local/lib/mysql:/usr/local/mysql:/usr/local/mysql/lib:/usr:/usr/lib/i386-linux-gnu:/usr/mariadb/lib:/usr/lib/mariadb:/usr/local/lib/mariadb:/usr/local/mariadb:/usr/local/mariadb/lib")            
  endif (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
  message("-- rpath = ${TMP_INSTALL_RPATH}")
  set_target_properties(ocelotgui PROPERTIES INSTALL_RPATH "${TMP_INSTALL_RPATH}")
  set_target_properties(ocelotgui PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
else (NOT TMP_CMAKE_SKIP_RPATH)
  message("-- No rpath")
endif (NOT TMP_CMAKE_SKIP_RPATH)
endif (TMP_OCELOT_MYSQL_INCLUDE)

if (Qt5Widgets_FOUND)
  target_link_libraries(ocelotgui Qt5::Widgets)
endif(Qt5Widgets_FOUND)
if (Qt4Widgets_FOUND)
  target_link_libraries(ocelotgui Qt4::QtGui)
endif(Qt4Widgets_FOUND)
if (Qt6Widgets_FOUND)
  target_link_libraries(ocelotgui Qt6::Widgets)
endif(Qt6Widgets_FOUND)

FILE(GLOB jpg_files "${CMAKE_CURRENT_SOURCE_DIR}/*.jpg")
FILE(GLOB htm_files "${CMAKE_CURRENT_SOURCE_DIR}/*.htm")
install(TARGETS ocelotgui RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${jpg_files} DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES ${htm_files} DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES debugger.png DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES completer_1.png DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES completer_2.png DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES completer_3.png DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES conditional.png DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES explorer1.png DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES explorer2.png DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES explorer3.png DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES explorer4.png DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES explorer5.png DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES explorer6.png DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES explorer7.png DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES explorer8.png DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES explorer9.png DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES menu-debug.png DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES menu-edit.png DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES menu-file.png DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES menu-help.png DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES menu-options.png DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES menu-run.png DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES menu-settings.png DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES ocelotgui_logo.png DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES result-widget-example.png DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES shot11.png DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES shot3.png DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES special-detach.png DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES special-images.png DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES special-settings.png DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES special-vertical.png DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES starting-dialog.png DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES starting.png DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES statement-widget-example.png DESTINATION ${CMAKE_INSTALL_DOCDIR})
# Debian packages would gzip .txt and .md files anyway.
if (NOT PACKAGE_TYPE MATCHES "DEB")
if (NOT PACKAGE_TYPE MATCHES "RPM")
install(FILES README.txt DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES README.md DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES options.txt DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES debugger_reference.txt DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES tarantool.txt DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES PKGBUILD DESTINATION ${CMAKE_INSTALL_DOCDIR})
endif (NOT PACKAGE_TYPE MATCHES "RPM")
endif (NOT PACKAGE_TYPE MATCHES "DEB")
if (NOT PACKAGE_TYPE MATCHES "DEB")
install(FILES LICENSE.GPL DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES COPYING DESTINATION ${CMAKE_INSTALL_DOCDIR})
endif (NOT PACKAGE_TYPE MATCHES "DEB")
install(FILES copyright DESTINATION ${CMAKE_INSTALL_DOCDIR})
if (PACKAGE_TYPE MATCHES "DEB")
install(FILES example.cnf DESTINATION ${CMAKE_INSTALL_DOCDIR}/examples)
else (PACKAGE_TYPE MATCHES "DEB")
install(FILES example.cnf DESTINATION ${CMAKE_INSTALL_DOCDIR})
endif (PACKAGE_TYPE MATCHES "DEB")
install(FILES ocelotgui.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
install(FILES ocelotgui-logo.png DESTINATION ${CMAKE_INSTALL_DATADIR}/pixmaps)
# So the only original files not copied are the source files:
# CMakeLists.txt ocelotgui.pro ocelotgui.cpp ocelotgui.h codeeditor.h hparse.h ostrings.h
# readmylogin.c ocelotgui.ui install_sql.cpp
# and maybe redundant doc files

if (NOT PACKAGE_TYPE MATCHES "DEB")
if (NOT PACKAGE_TYPE MATCHES "RPM")
  execute_process(COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/ocelotgui.1 ${CMAKE_CURRENT_SOURCE_DIR}/ocelotgui.1.bak)
  execute_process(COMMAND gzip -9 -f -n ${CMAKE_CURRENT_SOURCE_DIR}/ocelotgui.1)
  execute_process(COMMAND mv ${CMAKE_CURRENT_SOURCE_DIR}/ocelotgui.1.bak ${CMAKE_CURRENT_SOURCE_DIR}/ocelotgui.1)
  install(FILES ocelotgui.1.gz DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
endif (NOT PACKAGE_TYPE MATCHES "RPM")
endif (NOT PACKAGE_TYPE MATCHES "DEB")

if (PACKAGE_TYPE MATCHES "DEB")
  install(FILES ocelotgui.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
endif (PACKAGE_TYPE MATCHES "DEB")

if (PACKAGE_TYPE MATCHES "RPM")
  install(FILES ocelotgui.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
endif (PACKAGE_TYPE MATCHES "RPM")

# version = 1.8.0
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "8")
set(CPACK_PACKAGE_VERSION_PATCH "0")

# I assume nobody wants package names like ocelotgui_1.8.0-Linux.deb
# but that's what I get if CMAKE_SYSTEM_NAME = "Linux".
# I'd prefer ocelotgui_1.8.0-1_amd64.deb or ocelotgui-1.8.0.x86_64.rpm
# If it's neither "DEB" nor "RPM", = default.
set(FILENAME_PLATFORM_FOR_DEB "Linux")
set(FILENAME_PLATFORM_FOR_RPM "Linux")
string(TOLOWER "${CMAKE_SYSTEM_NAME}" cmake_system_name_lower)
if (cmake_system_name_lower MATCHES "linux")
  find_program(DPKG_CMD dpkg)
  if (DPKG_CMD)
    execute_process(COMMAND "${DPKG_CMD}" --print-architecture
      OUTPUT_VARIABLE FILENAME_PLATFORM_FOR_DEB
      OUTPUT_STRIP_TRAILING_WHITESPACE)
  endif (DPKG_CMD)
  find_program(UNAME_CMD uname)
  if (UNAME_CMD)
    execute_process(COMMAND "uname" --processor
      OUTPUT_VARIABLE FILENAME_PLATFORM_FOR_RPM
      OUTPUT_STRIP_TRAILING_WHITESPACE)
  endif(UNAME_CMD)
endif (cmake_system_name_lower MATCHES "linux")

set(FILENAME_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}${FILENAME_EXTRA}")
if (NOT FILENAME_RELEASE)
set(FILENAME_RELEASE "1")
endif (NOT FILENAME_RELEASE)
