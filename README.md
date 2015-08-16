
Ocelot ocelotgui

Version 0.6.0

This is Ocelot GUI (ocelotgui), a GPL-license database client.

Copyright (c) 2014, Ocelot Computer Services Inc.
All rights reserved.

For the GPL license terms see https://github.com/ocelot-inc/ocelotgui/blob/master/LICENSE.GPL.

For instructions for end users see https://github.com/ocelot-inc/ocelotgui/blob/master/manual.htm.

For ocelotgui screenshots see http://ocelot.ca/screenshots.htm.

For ocelotgui/debugger screenshots see http://ocelot.ca/blog/the-ocelotgui-debugger.

This README file has instructions for getting started.
Ocelot does not supply the Qt libraries or the libmysqlclient
library. Therefore you will have to download them separately.

## Feeling Lucky?
-----------------

Often a machine already has Qt and libmysqlclient.
In that case, be up and running in 15 seconds with:
    mkdir ~/ocelotgui-test
    cd ~/ocelotgui-test
    wget http://github.com/ocelot-inc/ocelotgui/releases/download/0.6.0/ocelotgui.tar.gz
    tar -zxvf ocelotgui.tar.gz
    cd ocelotgui
    ./ocelotgui-qt4 #or ./ocelotgui-qt5

If the above instructions succeed, there is no need to read
the rest of this document. But usually there is more to do.

## Getting the Qt libraries
---------------------------

The Ocelot-supplied 64-bit executable "ocelotgui-qt4" will
try to load libQtGui.so.4, the Qt 4 library. The Ocelot-supplied
64-bit executable "ocelotgui-qt5" will try to load libQtGui.so.5, the
Qt 5 library. You may find that the Qt packages are already installed,
since other common packages depend on them. If not, your Linux
distro's repositories will provide either Qt 4 or Qt 5,
most likely Qt 4.

The installation examples that follow do not assume that the
Qt libraries are already installed.

## Getting the libmysqlclient.so library
----------------------------------------

The assumption is that you have already installed MySQL or MariaDB somewhere.
Therefore you may have a copy of libmysqlclient.so somewhere.
If it is not already on the default path, then an error or warning
will appear when you try to run ocelotgui. Find it, and say something like
export LD_RUN_PATH=[path to directory that contains libmysqlclient.so]
Several other directories are searched; for details start ocelotgui and 
choose Help | libmysqlclient.

The installation examples that follow do not assume that libmysqlclient.so
is already installed.

## Getting the ocelotgui source and executable files
----------------------------------------------------

The official location of the project is on github:
    https://github.com/ocelot-inc/ocelotgui
This is where the latest source files are. This is what can be "cloned".
Typically, to get it, one would install git, cd to a download directory, then
    git clone https://github.com/ocelot-inc/ocelotgui
A clone has the latest source, but not executables.

The releases for ocelot-inc/ocelotgui are also on github:
https://github.com/ocelot-inc/ocelotgui/releases
or https://github.com/pgulutzan/ocelotgui/releases.
A release includes the source files as of the release time,
and also two binary (executable) files.
Although the release does not have the "latest" source which is
in ocelot-inc/ocelotgui, the existence of the executables
might be convenient. A release file is highlighted in green
by github and is named ocelotgui.tar.gz. Thus release 0.6.0 is at
https://github.com/ocelotgui/releases/download/0.6.0/ocelotgui.tar.gz
or https://github.com/pgulutzan/ocelotgui/releases/download/0.6.0/ocelotgui.tar.gz.
Typically, to get it, one would cd to a download directory, then
wget https://github.com/ocelot-inc/ocelotgui/releases/download/0.6.0/ocelotgui.tar.gz
or use a browser to go to https://github.com/ocelot-inc/ocelotgui/releases
and click ocelotgui.tar.gz.

Most of the installation examples that follow assume that download is
of a release.


## An installation with Ubuntu 12.04, 64-bit
--------------------------------------------

These steps worked after a fresh installation of Ubuntu 12.04,
a GNOME-based Linux distro, after downloading ocelotgui.tar.gz to ~/Downloads.
    cd ~/Downloads
    #Unpack ocelotgui.tar.gz to a new subdirectory named ocelotgui
    tar -zxvf ocelotgui.tar.gz
    cd ocelotgui
    #Install a package containing libmysqlclient.so and/or libmysqlclient.so.18
    #Maybe mysql-common would have sufficed.
    sudo apt-get install mysql-client
    #Install a package containing libQtGui.so.4
    sudo apt-get install libqt4-core
    #Then, as an ordinary non-root user, say something like
    ./ocelotgui-qt4 --host=127.0.0.1

## An installation with Ubuntu 14.04, 64-bit
--------------------------------------------

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
    #This is unnecessary, apt-get install will say "already latest version"
    sudo apt-get install libqt5core5a
    #Then, as an ordinary non-root user, say something like
    ./ocelotgui-qt5 --host=127.0.0.1 --user=root
Warning: Some menu shortcut keys may not work properly with this distro.

## An installation with Mageia 4.1, 64-bit
------------------------------------------

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

## An installation with SUSE 13.1, 32-bit, from source
-------------------------------------------------------

These steps worked after a non-fresh installation of openSUSE 13.1,
a KDE-based Linux distro, after downloading ocelotgui.tar.gz to ~/Downloads.

    cd ~/Downloads
    #Unpack ocelotgui.tar.gz to a new subdirectory named ocelotgui
    tar -zxvf ocelotgui.tar.gz
    #Use "cnf qmake" to confirm that these package names are correct.
    sudo zypper install libqt4-devel
    sudo zypper install mariadb-client
    sudo zypper install libmysqlclient-devel
    cd [path to ocelotgui source files]
    #Edit ocelotgui.pro.
    #Make sure that any of the lines that begins with "QMAKE_RPATHDIR + ..."
    #points to where libmysqlclient.so really is now. Example:
    #"QMAKE_RPATHDIR += /usr/local/mysql/lib/mysql"
    #Make sure that any of the lines that begins with "INCLUDEPATH+ ..."
    #points to where mysql.h really is now. Example:
    #"INCLUDEPATH += /usr/local/mysql/include/mysql"
    make clean             #usually unnecessary; ignore any error message
    rm Makefile            #usually unnecessary; ignore any error message
    qmake -config release
    #In the following line, if /usr/local/mysql/lib/mysql is not the path
    #where libmysqlclient.so is, replace with the correct name.
    #(This is usually not necessary.)
    export LD_LIBRARY_PATH=/usr/local/mysql/lib/mysql
    make
    #Then, as an ordinary non-root user, say something like
    ./ocelotgui

## An installation with SUSE 42.1, from source in release tar
-------------------------------------------------------------
    #This builds using the source files in the 0.6.0 "release"
    #To produce an executable named ~/ocelotgui-test/ocelotgui/ocelotgui
    sudo zypper install libqt4-devel
    sudo zypper install libmysqlclient-devel
    sudo zypper install git
    sudo zypper install gcc
    sudo zypper install gcc-c++
    sudo zypper install make
    mkdir ~/ocelotgui-test
    cd ~/ocelotgui-test
    wget http://github.com/ocelot-inc/ocelotgui/releases/download/0.6.0/ocelotgui.tar.gz
    tar -zxvf ocelotgui.tar.gz
    cd ocelotgui
    qmake -config release
    make
    #To uninstall, say: rm -r ~/ocelotgui-test

## An installation with SUSE 42.1, from source in git clone
-----------------------------------------------------------
    #This builds using the source files in the very latest "post-release"
    #To produce an executable named ~/ocelotgui-test/ocelotgui/ocelotgui
    sudo zypper install libqt4-devel
    sudo zypper install libmysqlclient-devel
    sudo zypper install git
    sudo zypper install gcc
    sudo zypper install gcc-c++
    sudo zypper install make
    mkdir ~/ocelotgui-test
    cd ~/ocelotgui-test
    git clone https://github.com/ocelot-inc/ocelotgui
    cd ocelotgui
    qmake -config release
    make
    #To uninstall, say: rm -r ~/ocelotgui-test


## Installing by rebuilding source, on Ubuntu 12.04, without Qt Creator
-----------------------------------------------------------------------

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

## Installing by rebuilding source, on Ubuntu 15.04
---------------------------------------------------

    #These steps have been known to work for an experiment.

    #Get Qt libraries. The message "already installed" might appear.
    sudo apt-get install qt5-default qtbase5-dev qt5-qmake qtbase5-dev-tools
    #Get MySQL libraries. Either libmysqlclient-dev or libmariadbclient-dev.
    sudo apt-get install libmariadbclient-dev

    #Get latest source from github repository, non-release
    cd ~
    git clone https://github.com/ocelot-inc/ocelotgui
    #Build
    cd ~/ocelotgui
    #Following might have to be /usr/lib/x86_64-linux-gnu/qt5/bin/qmake
    qmake
    make
    #Start the program to make sure it starts (stop again with File|Exit or ^Q).
    ./ocelotgui


## Installing by rebuilding source, with Qt Creator
---------------------------------------------------

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

    #Edit [path]/ocelotgui.pro.
    #Make sure that any of the lines that begins with "QMAKE_RPATHDIR + ..."
    #points to where libmysqlclient.so really is now.
    #It comes with MySQL or MariaDB,
    #for example "sudo apt-get mysqlclient" will put it
    #in file/usr/lib/x86_64-linux-gnu/libmysqlclient.so.
    #Example:
    #"QMAKE_RPATHDIR += /usr/local/mysql/lib/mysql"
    #Make sure that any of the lines that begins with "INCLUDEPATH+ ..."
    #points to where mysql.h really is now. Example:
    #"INCLUDEPATH += /usr/local/mysql/include/mysql"
    #(mysql.h comes with MySQL or MariaDB developer packages.
    # For example "sudo apt-get libmysqlclient-dev" will
    # put it in directory /usr/nclude/mysql).

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
