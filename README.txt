
Ocelot ocelotgui

Version 0.5.0

This is Ocelot GUI (ocelotgui), a GPL-license database client.

Copyright (c) 2014, Ocelot Computer Services Inc.
All rights reserved.
Read file LICENSE.GPL to see the terms of the license.

The file manual.htm has instructions for end users.

This README file has instructions for getting started.
Ocelot does not supply the Qt libraries or the libmysqlclient
library. Therefore you will have to download them separately.

Getting the Qt libraries
------------------------

The Ocelot-supplied 64-bit executable "ocelotgui-qt4" will
try to load libQtGui.so.4, the Qt 4 library. The Ocelot-supplied
64-bit executable "ocelotgui-qt5" will try to load libQtGui.so.5, the
Qt 5 library. You may find that the Qt packages are already installed,
since other common packages depend on them. If not, your Linux
distro's repositories will provide either Qt 4 or Qt 5,
most likely Qt 4.

The installation examples that follow do not assume that the
Qt libraries are already installed.

Getting the libmysqlclient.so library
-------------------------------------

The assumption is that you have already installed MySQL or MariaDB somewhere.
Therefore you may have a copy of libmysqlclient.so somewhere.
If it is not already on the default path, then an error or warning
will appear when you try to run ocelotgui. Find it, and say something like
export LD_RUN_PATH=[path to directory that contains libmysqlclient.so]
Several other directories are searched; for details start ocelotgui and 
choose Help | libmysqlclient.

The installation examples that follow do not assume that libmysqlclient.so
is already installed.

Getting the ocelotgui source and executable files
-------------------------------------------------

The official location of the project is on github:
https://github.com/ocelot-inc/ocelotgui
This is where the latest source files are. This is what can be "cloned".

The releases for ocelot-inc/ocelotgui are elsewhere on github:
https://github.com/pgulutzan/ocelotgui/releases
A release includes the source files as of the release time,
and also two binary (executable) files.
Although the release does not have the "latest" source which is
in ocelot-inc/ocelotgui, the existence of the executables
might be convenient. A release file is highlighted in green
by github and is named ocelotgui.tar.gz. Thus release 0.5.0 is at
https://github.com/pgulutzan/ocelotgui/releases/download/0.5.0/ocelotgui.tar.gz

The installation examples that follow assume that download is
of a release.


An installation with Ubuntu 12.04, 64-bit
-----------------------------------------

These steps worked after a fresh installation of Ubuntu 12.04,
a GNOME-based Linux distro, after downloading ocelotgui.tar.gz to ~/Downloads.
cd ~/Downloads
#Unpack ocelotgui.tar.gz to a new subdirectory named ocelotgui
tar -zxvf ocelotgui.tar.gz
cd ocelotgui
#Install a package containing libmysqlclient.so
#Maybe mysql-common would have sufficed.
sudo apt-get install mysql-client
#Install a package containing libQtGui.so.4
sudo apt-get install libqt4-core
#Then, as an ordinary non-root user, say something like
./ocelotgui-qt4 --host=127.0.0.1

An installation with Ubuntu 14.04, 64-bit
-----------------------------------------

These steps worked after a fresh installation of Ubuntu 14.04,
a GNOME-based Linux distro, after downloading ocelotgui.tar.gz to ~/Downloads.
cd ~/Downloads
#Unpack ocelotgui.tar.gz to a new subdirectory named ocelotgui
tar -zxvf ocelotgui.tar.gz
cd ocelotgui
#Install a package containing libmysqlclient.so
#Maybe mysql-common would have sufficed.
sudo apt-get install mysql-client
#Install a package containing libQtGui.so.5
#This is unnecessary, apt-get install will say "already installed"
sudo apt-get install libqt5core5a
#Then, as an ordinary non-root user, say something like
./ocelotgui-qt5 --host=127.0.0.1 --user=root
Warning: Some menu shortcut keys do not work properly with this distro.

An installation with Mageia 4.1, 64-bit
---------------------------------------

These steps worked after a fresh installation of Mageia 4.1,
a KDE-based Linux distro, after downloading ocelotgui.tar.gz to !Downloads.
cd ~/Downloads
#Unpack ocelotgui.tar.gz to a new subdirectory named ocelotgui
tar -zxvf ocelotgui.tar.gz
cd ocelotgui
#Install a package containing libmysqlclient.so
#You can use "urpmf <library name>" to find
#what packages contain libmysqlclient.so.
#If the answer is lib64mariadb18, you can say as root:
sudo urpmi lib64mariadb18
#You will probably find that libQtGui.so.4 is already
#present, so there is no need to install Qt libraries.
#Use "whereis libQtGui.so.4" to make sure.
#Then, as an ordinary non-root user, say something like
./ocelotgui-qt4 --host=127.0.0.1

An installation with Fedora 20, 64-bit
--------------------------------------

These steps worked after a fresh installation of Fedora 20, a
GNOME-based Linux distro, after downloading ocelotgui.tar.gz to ~/Downloads.
cd ~/Downloads
#Unpack ocelotgui.tar.gz to a new subdirectory named ocelotgui
tar -zxvf ocelotgui.tar.gz
cd ocelotgui
#Install a package containing libmysqlclient.so
#This actually brings in the package supplied by MariaDB
sudo yum install mysql
#Find out where library libQtGui.so.4 is
yum provides 'libQtGui.so.4'
#Suppose "yum provides" says that you need qt-x11-4.8.6-10.fc20.i686
#That's nice, but i686 is a 32-bit version, so change i686 to x86_64
sudo yum install qt-x11-4.8.6-10.fc20.x86_64
#Then, as an ordinary non-root user, say something like
./ocelotgui-qt4 --host=127.0.0.1

An installation with SUSE 13.1, 32-bit, from source
---------------------------------------------------

These steps worked after a non-fresh installation of openSUSE 13.1,
a KDE-based Linux distro, after downloading ocelotgui.tar.gz to ~/Downloads.

cd ~/Downloads
#Unpack ocelotgui.tar.gz to a new subdirectory named ocelotgui
tar -zxvf ocelotgui.tar.gz

#Use "cnf qmake" to confirm that these package names are correct.
sudo zypper install libqt4-devel
sudo zypper install mariadb-client
cd [path to ocelotgui source files]
#Edit ocelotgui.pro.
#Change line that begins with "LIBS + ..."
#so that it points to where libmysqlclient.so really is now. Example:
#"LIBS += /usr/local/mysql/lib/mysql/libmysqlclient.so"
#Change line that begins with "INCLUDEPATH ..."
#so that it points to where mysql.h really is now. Example:
#"INCLUDEPATH += /usr/local/mysql/include/mysql"
make clean             #usually unnecessary; ignore any error message
rm Makefile            #usually unnecessary; ignore any error message
qmake -config release
#In the following line, if /usr/local/mysql/lib/mysql is not the path
#where libmysqlclient.so is, replace with the correct name.
export LD_LIBRARY_PATH=/usr/local/mysql/lib/mysql
make
#Then, as an ordinary non-root user, say something like
./ocelotgui

Installing by rebuilding source, on Ubuntu 12.04, without Qt Creator
--------------------------------------------------------------------

All Ocelot source files are supplied in subdirectory ocelotgui.

Examples here use the common Ubuntu install method, "apt-get".
Other Linux distros will have different methods, such as "yum" or "yast2",
and will have different names for the Qt packages.

If the intent is to rebuild for Qt 4 from source:
sudo apt-get install qt4-qmake
sudo apt-get install libqt4-dev
cd [path to ocelotgui source files]
make clean
/usr/bin/qmake-qt4 -config release
make

If the intent is to rebuild for Qt 5 from source:
sudo apt-get install libqt5-default
sudo apt-get install libqt5-qmake
(These are just guesses. We used Qt Creator.)
cd [path to ocelotgui source files]
make clean
[path-to-qt5-qmake]/qmake -config release
make
#Then, as an ordinary non-root user, say something like
./ocelotgui --protocol=tcp


Installing by rebuilding source, with Qt Creator
------------------------------------------------

For any version or common distro, one can download Qt Creator
from the Qt download site http://qt-project.org/downloads.
When confronted with the "License Agreement" dialog box,
remember that ocelotgui is licensed under GPL version 2,
so we recommend picking LGPL and re-conveying as GPL.

Make sure a C++ compiler such as g++ is installed first.

Edit [path]/ocelotgui.pro: change INCLUDEPATH to a path
that includes the file "mysql.h". It comes with MySQL or
MariaDB developer packages, for example "sudo apt-get
libmysqlclient-dev" will put it in directory /usr/include/mysql.

Edit [path]/ocelotgui.pro: change LIBS to the file
"libmysqlclient.so". It comes with MySQL or MariaDB,
for example "sudo apt-get mysqlclient" will put it
in file /usr/lib/x86_64-linux-gnu/libmysqlclient.so.

You will need to state a compiler for the kit.
Steps for Qt Creator use are:
  Click File | Open File or Project ...
  In "Open File" dialog box, in field "File name:", enter [path]/ocelotgui.pro
  Click Next
  Click Configure Project

If you get a message "cannot find -lGL" when you
try to run the project, try to install GL.
With Ubuntu: sudo apt-get install libgl1-mesa-dev.
With Fedora: sudo yum install mesa-libGL-devel.

Using Qt Creator is the method that Ocelot itself uses regularly,
and it is simple to install for most Linux distros. However,
it requires a large amount of disk space.

For more commentary about compiling and building,
read an ocelotgui.cpp comment that begins with the words
"General comments".
