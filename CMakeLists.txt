# Copyright (c) 2014-2018 by Ocelot Computer Services Inc. All rights reserved.
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

# Qt4 or Qt5 dev packages must be installed first. See 
# http://doc.qt.io/qt-5/linux.html
# ... but that is not complete for Ubuntu, you also need qtbase5-dev.
# If your OS comes with Qt5, you should download Qt5 dev packages.
# If your OS comes with Qt4, you should download Qt4 dev packages.
# If your os is unknown then try "pkg-config --modversion QtCore"
# or look in /usr/lib/pkgconfig files.
# For more instructions see "Qt for Linux/X11"
# http://doc.qt.io/qt-5/linux.html.
# If you have qt4, but findQt4() fails, a symbolic link might work, e.g.:
# sudo ln -s  /usr/lib64/qt4/bin/qmake /usr/bin/qmake-qt4

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

# USE_RPATH=TRUE|FALSE and CMAKE_INSTALL_RPATH
# By default we have rpath. But from debian/rules and ocelotgui.spec we pass -DUSE_RPATH=FALSE.
# For QMake, QMAKE_RPATHDIR is where ocelotgui will search for libmysqlclient.so at run time.
# CMAKE_INSTALL_RPATH is apparently the CMake translation of QMAKE_RPATHDIR,
# but first it will try several other ways.
# It does not replace our hard-coded defaults -- we append the hard-coded defaults to it.
# Start ocelotgui and say Help | libmysqlclient for details.
# In fact there may be an effect on RUNPATH, overriding RUNPATH.
# If you change this, test with "readelf -d ocelotgui | grep PATH".
# Remember that .deb and .rpm packagers suggest avoiding rpath.

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

# QT_VERSION=4 or QT_VERSION=5
# If -DQT_VERSION=4: Search only for Qt4, do not search for Qt5 first.
# If -DQT_VERSION=5: Search only for Qt5, do not search for Qt4 afterwards.
# If not specified or (not 4 and not 5): Search for Qt5 then search for Qt4.

# CPACK_GENERATOR="DEB" or CPACK_GENERATOR="RPM"
#   DEB:
#     cpack_package_file_name = package_version-release_arch.deb
#     Do not install COPYING file or LICENSE.GPL file.
#     Only copyright file is installed, which is legally the same thing.
#     This is to avoid lintian warning "extra-license-file".
#     Todo: let this also affect whether to use gzip for the man file.
#     Todo: let this also affect whether to strip
#   RPM:
#     cpack_package_file_name = package-version-release.arch.rpm
# iff CPACK_GENERATOR="DEB" is used, then after build say "cpack" to generate a .deb file
# iff CPACK_GENERATOR="RPM" is used, then after build say "cpack" to generate an .rpm file
# CPACK_GENERATOR is becoming obsolete. In ocelotgui 1.08 Ocelot's official method for .rpm
# is not wih cpack it is with rpm_build.sh. In ocelotgui 1.08 Ocelot's official method for .deb
# is not wth cpack it is with deb_build.sh and|or debian/rules -- debuild causes a pass of
# PACKAGE_TYPE="DEB" not CPACK_GENERATOR="DEB". So eventually we will remove this option.

# PACKAGE_TYPE="DEB" or PACKAGE_TYPE="RPM"
#   DEB:
#     This is the pass specified in debian/rules, we don't use it for cmake standalone.
#     cpack_package_file_name = package_version-release_arch.deb
#     Do not install COPYING file or LICENSE.GPL file.
#     This is to avoid lintian warning "extra-license-file".
#     Unlike CPACK_GENERATOR="DEB", we do not try to make .gz files, leave that to debuild.
#   RPM:
#     In ocelotgui.spec we say -DPACKAGE_TYPE="RPM" but we might not use it here.

# OCELOT_C_FLAGS and OCELOT_CXX_FLAGS and OCELOT_CPP_FLAGS
# These will override any environment variables or Qt defaults for C_FLAGS etc. if PACKAGE_TYPE="DEB".
# See debian/rules.
# Ordinarily the environment variables are blank and the Qt default is blank or -fPIE.
# Something can add -fPIC anyway.

# FILENAME_EXTRA
# Default is blank. Ocelot passes -DFILENAME_EXTRA="qt4" when building with qt4,
# so the final package name will look like ocelotgui_1.0.7qt4-1_amd64.deb or
# ocelotgui-1.0.7qt4-1.x86_64.rpm. Ordinarily it should not be specified.

# FILENAME_RELEASE
# Default is "1". This affects filenames generated by cpack. Ordinarily it doesn't need to be changed.

# e.g. to build and put results in ./bin, if CMAKE_PREFIX_PATH not needed, say
# cmake . -DCMAKE_PREFIX_PATH=/home/pgulutzan/Qt/5.4/gcc_64/lib/cmake/Qt5Widgets -DCMAKE_INSTALL_PREFIX=.
# make
# make install

# e.g. to generate .deb file and install with dpkg:
# cmake . -DCPACK_GENERATOR="DEB"
# make
# strip --remove-section=.comment ocelotgui
# cpack
# lintian ocelotgui_1.0.7-1_amd64.deb
# sudo dpkg -i ocelotgui_1.0.7-1_amd64.deb
# Todo: lintian will warn "new-package-should-close-itp-bug", which we ignore.
# Todo: lintian will warn about permissions if you are not root, which we work around by being root.
# but see also comments in rpm_build.sh and debian/README.Debian, we intend to stop using cpack

# e.g. to generate .rpm file and install with rpm:
# cmake . -DCPACK_GENERATOR="RPM"
# make
# cpack
# rpmlint ocelotgui-1.0.7-1.x86_64.rpm
# rpm -i ocelotgui-1.0.7-1.x86_64.rpm
# Todo: rpmlint will warn "standard-dir-owned-by-package" for /usr/share/man and /usr/share/man/man1, which we ignore.
# Todo: rpmlint will warn "no-signature", which we ignore.
# Todo: rpmlint will warn "no-packager-tag", wich we ignore.
# but see also comments in rpm_build.sh and debian/README.Debian, we intend to stop using cpack

message("-- CmakeLists.txt for ocelotgui 1.0.7")
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

cmake_minimum_required(VERSION 2.8.11)

if (CMAKE_MAJOR_VERSION EQUAL "3")
  if (CMAKE_MINOR_VERSION EQUAL "7")
    if (CPACK_GENERATOR MATCHES "DEB")
      message("-- Warning: CPack 3.7 bug:")
      message("     CPack version 3.7 may put md5sums files in the wrong directory.")
      message("     https://cmake.org/pipermail/cmake/2017-March/065110.html")
      message("     This may cause lintian to generate warnings.")
      message("     The .deb file is actually okay so this bug can be ignored.")
      message("     Or: use a different version of CMake/CPack.")
    endif (CPACK_GENERATOR MATCHES "DEB")
  endif (CMAKE_MINOR_VERSION EQUAL "7")
endif (CMAKE_MAJOR_VERSION EQUAL "3")

project(ocelotgui)

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
  message(FATAL ERROR "-- mysql.h not found in CMake default path or Ocelot hard-coded path")
endif (MYSQL_INCLUDE_DIR_RESULT)

# Make sure .h files in current directory are found e.g. ocelotgui.h or what uic produces
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#automoc requires "cmake_minimum_required(VERSION 2.8.6)" but that's okay.
set(CMAKE_AUTOMOC ON)

# Add MYSQL headers.
include_directories(${MYSQL_INCLUDE_DIR_RESULT})

#lintian complained about "unstripped-binary-or-object"
# set(CPACK_STRIP_FILES TRUE) was useless
# SET(CMAKE_BUILD_TYPE "Release" was useless
# We also should say, after "make" and before "cpack": strip --remove-section=.comment ocelotgui
# Todo: check: does this override a user who wants CMAKE_BUILD_TYPE debug?
if(CMAKE_COMPILER_IS_GNUCXX)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s")
endif(CMAKE_COMPILER_IS_GNUCXX)

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

if (NOT QT_VERSION EQUAL "4")
  if (QT_VERSION EQUAL "5")
    find_package(Qt5Widgets REQUIRED)
  else (QT_VERSION EQUAL "5")
    # QUIET suppresses the following warning because we'll look for Qt4 instead:
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
  endif (QT_VERSION EQUAL "5")
endif (NOT QT_VERSION EQUAL "4")

if (Qt5Widgets_FOUND)
  message("-- found Qt5")
  # -fPIE, but nobody has checked whether other flags are better
  # (some other flags depend on CMAKE_BUILD_TYPE)
  # (Ocelot developers use ocelotgui.pro which has -Wall -O2)
  set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
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

#MinGW ignores #pragmas in third_party.h
if (CMAKE_GENERATOR STREQUAL "MinGW Makefiles")
 set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -Wno-nonnull -Wno-pointer-arith -Wno-conversion")
endif (CMAKE_GENERATOR STREQUAL "MinGW Makefiles")

#If OCELOT_xxx_FLAGS is passed for a Debian package, then override environment variables and Qt5 setting.
#Maybe I could also check CMAKE_CXX_COMPILER should be gcc and CMAKE_CXXCOMPILER_VERSION should be >= something.
if (PACKAGE_TYPE STREQUAL "DEB")
if(CMAKE_COMPILER_IS_GNUCXX)
if (OCELOT_C_FLAGS)
  message("-- Setting CMAKE_C_FLAGS=OCELOT_C_FLAGS+OCELOT_CPP_FLAGS, CMAKE_CXX_FLAGS=OCELOT_CXX_FLAGS+OCELOt-CPP_FLAGS")
  set(CMAKE_C_FLAGS "${OCELOT_C_FLAGS} ${OCELOT_CPP_FLAGS}")
  set(CMAKE_CXX_FLAGS "${OCELOT_CXX_FLAGS} ${OCELOT_CPP_FLAGS}")
endif (OCELOT_C_FLAGS)
endif(CMAKE_COMPILER_IS_GNUCXX)
endif (PACKAGE_TYPE STREQUAL "DEB")

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

# See comments at the start of this file regarding USE_RPATH and CMAKE_INSTALL_RPATH.
if (DEFINED USE_RPATH)
  set(TMP_USE_RPATH ${USE_RPATH})
else (DEFINED USE_RPATH)
  set(TMP_USE_RPATH TRUE)
endif (DEFINED USE_RPATH)
if (TMP_USE_RPATH)
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
else (TMP_USE_RPATH)
  message("-- No rpath")
endif (TMP_USE_RPATH)

if (Qt5Widgets_FOUND)
  target_link_libraries(ocelotgui Qt5::Widgets)
else (Qt5Widgets_FOUND)
  target_link_libraries(ocelotgui Qt4::QtGui)
endif (Qt5Widgets_FOUND)

FILE(GLOB jpg_files "${CMAKE_CURRENT_SOURCE_DIR}/*.jpg")
FILE(GLOB htm_files "${CMAKE_CURRENT_SOURCE_DIR}/*.htm")
install(TARGETS ocelotgui RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${jpg_files} DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES ${htm_files} DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES debugger.png DESTINATION ${CMAKE_INSTALL_DOCDIR})
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
endif (NOT PACKAGE_TYPE MATCHES "RPM")
endif (NOT PACKAGE_TYPE MATCHES "DEB")
if (NOT CPACK_GENERATOR MATCHES "DEB")
if (NOT PACKAGE_TYPE MATCHES "DEB")
install(FILES LICENSE.GPL DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES COPYING DESTINATION ${CMAKE_INSTALL_DOCDIR})
endif (NOT PACKAGE_TYPE MATCHES "DEB")
endif (NOT CPACK_GENERATOR MATCHES "DEB")
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
if (CPACK_GENERATOR MATCHES "DEB")
  #lintian complains changelog-file-missing-in-native-package, this fixes it,
  #but lintian will still complain new-package-should-close-itp-bug
  #which we cannot fix until version >= 1.0.7 and someone posts an ITP bug
  execute_process(COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/changelog ${CMAKE_CURRENT_SOURCE_DIR}/changelog.bak)
  execute_process(COMMAND gzip -9 -f -n ${CMAKE_CURRENT_SOURCE_DIR}/changelog)
  execute_process(COMMAND mv ${CMAKE_CURRENT_SOURCE_DIR}/changelog.bak ${CMAKE_CURRENT_SOURCE_DIR}/changelog)
  install(FILES changelog.gz DESTINATION ${CMAKE_INSTALL_DOCDIR})
endif (CPACK_GENERATOR MATCHES "DEB")

if (NOT CPACK_GENERATOR)
if (NOT PACKAGE_TYPE MATCHES "DEB")
  execute_process(COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/ocelotgui.1 ${CMAKE_CURRENT_SOURCE_DIR}/ocelotgui.1.bak)
  execute_process(COMMAND gzip -9 -f -n ${CMAKE_CURRENT_SOURCE_DIR}/ocelotgui.1)
  execute_process(COMMAND mv ${CMAKE_CURRENT_SOURCE_DIR}/ocelotgui.1.bak ${CMAKE_CURRENT_SOURCE_DIR}/ocelotgui.1)
  install(FILES ocelotgui.1.gz DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
endif (NOT PACKAGE_TYPE MATCHES "DEB")
endif (NOT CPACK_GENERATOR)

if (CPACK_GENERATOR MATCHES "DEB")
  #lintian complains manpage-not-compressed, this fixes it.
  execute_process(COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/ocelotgui.1 ${CMAKE_CURRENT_SOURCE_DIR}/ocelotgui.1.bak)
  execute_process(COMMAND gzip -9 -f -n ${CMAKE_CURRENT_SOURCE_DIR}/ocelotgui.1)
  execute_process(COMMAND mv ${CMAKE_CURRENT_SOURCE_DIR}/ocelotgui.1.bak ${CMAKE_CURRENT_SOURCE_DIR}/ocelotgui.1)
  install(FILES ocelotgui.1.gz DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
endif (CPACK_GENERATOR MATCHES "DEB")

if (CPACK_GENERATOR MATCHES "RPM")
  #rpm -i will compress so if you plan to use cpack you only need to say:
  #install(FILES ocelotgui.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
  #but since we use rpm_build.sh we have the same as for "DEB"
  execute_process(COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/ocelotgui.1 ${CMAKE_CURRENT_SOURCE_DIR}/ocelotgui.1.bak)
  execute_process(COMMAND gzip -9 -f -n ${CMAKE_CURRENT_SOURCE_DIR}/ocelotgui.1)
  execute_process(COMMAND mv ${CMAKE_CURRENT_SOURCE_DIR}/ocelotgui.1.bak ${CMAKE_CURRENT_SOURCE_DIR}/ocelotgui.1)
  install(FILES ocelotgui.1.gz DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
endif (CPACK_GENERATOR MATCHES "RPM")

if (PACKAGE_TYPE MATCHES "DEB")
  install(FILES ocelotgui.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
endif (PACKAGE_TYPE MATCHES "DEB")

# version = 1.0.7
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "GUI client for MySQL or MariaDB")
set(CPACK_PACKAGE_VENDOR "Ocelot Computer Services Inc.")
set(CPACK_PACKAGE_CONTACT "Peter Gulutzan")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "7")
# Todo: set(CPACK_PACKAGE_DESCRIPTION_FILE first-lines-of-README)
# CPACK_PACKAGE_INSTALL_DIRECTORY = default
set(CPACK_PACKAGE_ICON ocelotgui-logo.png)
# CPACK_PROJECT_CONFIG_FILE = nonexistent

# I assume nobody wants package names like ocelotgui_1.0.7-Linux.deb
# but that's what I get if CMAKE_SYSTEM_NAME = "Linux".
# I'd prefer ocelotgui_1.0.7-1_amd64.deb or ocelotgui-1.0.7.x86_64.rpm
# If it's neither "DEB" nor "RPM", CPACK_PACKAGE_FILE_NAME = default.
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

if (CPACK_GENERATOR MATCHES "DEB")
  set(CPACK_PACKAGE_FILE_NAME ${CMAKE_PROJECT_NAME}_${FILENAME_VERSION}-${FILENAME_RELEASE}_${FILENAME_PLATFORM_FOR_DEB})
endif (CPACK_GENERATOR MATCHES "DEB")
if (CPACK_GENERATOR MATCHES "RPM" OR PACKAGE_TYPE MATCHES "RPM")
  set(CPACK_PACKAGE_FILE_NAME ${CMAKE_PROJECT_NAME}-${FILENAME_VERSION}-${FILENAME_RELEASE}.${FILENAME_PLATFORM_FOR_RPM})
endif (CPACK_GENERATOR MATCHES "RPM" OR PACKAGE_TYPE MATCHES "RPM")
if (PACKAGE_TYPE MATCHES "DEB")
  set(CPACK_PACKAGE_FILE_NAME ${CMAKE_PROJECT_NAME}_${FILENAME_VERSION}-${FILENAME_RELEASE}_${FILENAME_PLATFORM_FOR_DEB})
endif (PACKAGE_TYPE MATCHES "DEB")

# Todo: You should have both COPYING and COPYING.thirdparty, no?
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.htm")

if (CPACK_GENERATOR MATCHES "DEB")
  # CPACK_RESOURCE_FILE_WELCOME = nonexistent
  # CPACK_GENERATOR = we expect it to be specified if cpack will be used.
  # CPACK_DEBIAN_PACKAGE_NAME = default = CPACK_PACKAGE_NAME = project name
  # CPACK_DEBIAN_PACKAGE_VERSION = default = CPACK_PACKAGE_VERSION = 1.0.7
  # CPACK_DEBIAN_PACKAGE_DEPENDS = todo?
  set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Peter Gulutzan <pgulutzan@ocelot.ca>")
  set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Ocelot GUI (ocelotgui), a database client, allows users to connect to
 a MySQL or MariaDB DBMS server, enter SQL statements, and receive results.
 Some of its features are: syntax highlighting, user-settable colors
 and fonts for each part of the screen, result-set displays
 with multi-line rows and columns that can be resized, and a debugger.")
  set(CPACK_DEBIAN_PACKAGE_SECTION "database")
  set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
  set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "http://ocelot.ca")
  set(CPACK_DEBIAN_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
  #lintian complains "missing-depends-line" if this is not on
  set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
  # CPACK_DEBIAN_PACKAGE_RECOMMENDS = todo: should recommend libmysqlclient
endif (CPACK_GENERATOR MATCHES "DEB")

if (CPACK_GENERATOR MATCHES "RPM")
  # Todo: For other than Mageia prefer CPACK_RPM_PACKAGE_GROUP "Applications/Databases"
  #CPACK_RPM_PACKAGE_SUMMARY = default = CPACK_PACKAGE_DESCRIPTION_SUMMARY
  set(CPACK_RPM_PACKAGE_NAME "ocelotgui")
  #CPACK_RPM_PACKAGE_VERSION = DEFAULT = CPACK_PACKAGE_VERSION = 1.0.7
  set(CPACK_RPM_PACKAGE_RELEASE ${FILENAME_RELEASE})
  set(CPACK_RPM_PACKAGE_LICENSE GPLv2)
  set(CPACK_RPM_PACKAGE_GROUP "Databases")
  set(CPACK_RPM_PACKAGE_VENDOR "Ocelot Computer Services Inc.")
  set(CPACK_RPM_PACKAGE_URL "http://ocelot.ca")
  set(CPACK_RPM_PACKAGE_DESCRIPTION "Ocelot GUI (ocelotgui), a database client, allows users to connect to
 a MySQL or MariaDB DBMS server, enter SQL statements, and receive results.
 Some of its features are: syntax highlighting, user-settable colors
 and fonts for each part of the screen, result-set displays
 with rows that can have multiple lines and columns that can be dragged,
 and a debugger.")
  set(CPACK_RPM_CHANGELOG_FILE "${PROJECT_SOURCE_DIR}/rpmchangelog")
  set(CPACK_RPM_PRE_INSTALL_SCRIPT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/rpm_pre_install.sh")
  set(CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/rpm_post_install.sh")
  set(CPACK_RPM_PRE_UNINSTALL_SCRIPT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/rpm_pre_uninstall.sh")
  set(CPACK_RPM_POST_UNINSTALL_SCRIPT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/rpm_post_uninstall.sh")
endif (CPACK_GENERATOR MATCHES "RPM")

include(CPack)
