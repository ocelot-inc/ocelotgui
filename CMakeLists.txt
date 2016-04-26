# CMakeLists.txt for ocelotgui
# This CmakeLists.txt file works if Qt 5 + mysql/mariadb files are present.
# It does not work with Qt 4.
# It requires that cmake be started with -CMAKE_PREFIX_PATH.
# For example, 
# Suppose that Qt5WidgetsConfig.cmake is in this directory:
# /home/pgulutzan/Qt/5.4/gcc_64/lib/cmake/Qt5Widgets
# So do not say "cmake ." say
# cmake . -DCMAKE_PREFIX_PATH=/home/pgulutzan/Qt/5.4/gcc_64/lib/cmake/Qt5Widgets
# or wherever Qt5WidgetsConfig.cmake is to be found.
# Todo: try to figure out how to fall back to Qt4 if Qt5 isn't around.
# Todo: list library paths as you do in ocelotgui.pro.

message("-- CmakeLists.txt has not been tested on many platforms")
message("-- Ocelot recommends using qmake and ocelotgui.pro instead")
message("-- see README.md for the usual instructions for building")

cmake_minimum_required(VERSION 2.8.11)

project(ocelotgui)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#automoc requires "cmake_minimum_required(VERSION 2.8.6)" but that's okay.
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets)

#If I use autouic, I have to say "cmake_minimum_required(VERSION 3.0.2)"
#I don't expect to have it, so will use what seems to be the older method.
#set(CMAKE_AUTOUIC ON)
qt5_wrap_ui(UI_OCELOTGUI ocelotgui.ui)

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
   message("-- mysql.h is in ${MYSQL_INCLUDE_DIR}")
else (MYSQL_INCLUDE_DIR)
   message(FATAL_ERROR "-- mysql.h is not found")
endif (MYSQL_INCLUDE_DIR)

# Add MYSQL headers.
include_directories(${MYSQL_INCLUDE_DIR})
# Add MYSQL lib to the list of libraries to use.
set(LIBS ${LIBS} ${MYSQL_LIB_DIR})

# There is only one executable: ocelotgui
add_executable(ocelotgui ${UI_OCELOTGUI} ocelotgui.cpp)

target_link_libraries(ocelotgui Qt5::Widgets)

target_link_libraries(ocelotgui mysqlclient)
