# deb_build.sh -- produce ocelotgui-1.0.7 .deb from ~/ocelotgui

# Ocelot uses this script to produce a .deb file from a source directory ~/ocelotgui on Ubuntu xenial.
# Only advanced users will want this, ordinary users will download the official-release .deb file or make with cmake.

# An easy way to get ~/ocelotgui is "cd ~" + "git clone https://github.com/ocelot-inc/ocelotgui ocelotgui".
# This script uses /tmp and cheerily deletes existing files in /tmp.
# The result will be /tmp/debian3/ocelotgui_1.0.7-1_amd64.deb or /tmp/debian3/ocelotgui_1.0.7-1_i386.deb.
# See also the comments in README.Debian.

sudo apt-get install debhelper dpkg-dev devscripts
sudo apt-get install gcc g++ libc6 cmake
sudo apt-get install qtbase5-dev
sudo apt-get install default-libmysqlclient-dev
sudo apt-get install piuparts

export VERSION=1.0.7
export SOURCE=$HOME/ocelotgui

cd /tmp
rm -f ocelotgui-$VERSION.tar.gz
rm -f ocelotgui_$VERSION.orig.tar.gz
rm -r -f ocelotgui-$VERSION
mkdir ocelotgui-$VERSION
mkdir ocelotgui-$VERSION/debian
mkdir ocelotgui-$VERSION/debian/patches
mkdir ocelotgui-$VERSION/debian/source
cp -p $SOURCE/README.md /tmp/ocelotgui-$VERSION/README.txt
cp -p $SOURCE/README.md /tmp/ocelotgui-$VERSION/README.htm
#cp -p $SOURCE/copyright /tmp/ocelotgui-$VERSION/debian/copyright
#cp -p $SOURCE/changelog /tmp/ocelotgui-$VERSION/debian/changelog
cp -p $SOURCE/ocelotgui.pro /tmp/ocelotgui-$VERSION/ocelotgui.pro
cp -p $SOURCE/ocelotgui.h /tmp/ocelotgui-$VERSION/ocelotgui.h
cp -p $SOURCE/ocelotgui.cpp /tmp/ocelotgui-$VERSION/ocelotgui.cpp
cp -p $SOURCE/install_sql.cpp /tmp/ocelotgui-$VERSION/install_sql.cpp
cp -p $SOURCE/codeeditor.h /tmp/ocelotgui-$VERSION/codeeditor.h
cp -p $SOURCE/hparse.h /tmp/ocelotgui-$VERSION/hparse.h
cp -p $SOURCE/ostrings.h /tmp/ocelotgui-$VERSION/ostrings.h
cp -p $SOURCE/ocelotgui.ui /tmp/ocelotgui-$VERSION/ocelotgui.ui
cp -p $SOURCE/LICENSE.GPL /tmp/ocelotgui-$VERSION/LICENSE.GPL
cp -p $SOURCE/manual.htm /tmp/ocelotgui-$VERSION/manual.htm
cp -p $SOURCE/README.txt /tmp/ocelotgui-$VERSION/README.txt
cp -p $SOURCE/README.md /tmp/ocelotgui-$VERSION/README.md
cp -p $SOURCE/README.htm /tmp/ocelotgui-$VERSION/README.htm
cp -p $SOURCE/debugger_reference.txt /tmp/ocelotgui-$VERSION/debugger_reference.txt
cp -p $SOURCE/windows.txt /tmp/ocelotgui-$VERSION/windows.txt
cp -p $SOURCE/example.cnf /tmp/ocelotgui-$VERSION/example.cnf
cp -p $SOURCE/readmylogin.c /tmp/ocelotgui-$VERSION/readmylogin.c
cp -p $SOURCE/options.txt /tmp/ocelotgui-$VERSION/options.txt
cp -p $SOURCE/starting-dialog.png /tmp/ocelotgui-$VERSION/starting-dialog.png
cp -p $SOURCE/starting.png /tmp/ocelotgui-$VERSION/starting.png
cp -p $SOURCE/statement-widget-example.png /tmp/ocelotgui-$VERSION/statement-widget-example.png
cp -p $SOURCE/result-widget-example.png /tmp/ocelotgui-$VERSION/result-widget-example.png
cp -p $SOURCE/menu-file.png /tmp/ocelotgui-$VERSION/menu-file.png
cp -p $SOURCE/menu-edit.png /tmp/ocelotgui-$VERSION/menu-edit.png
cp -p $SOURCE/menu-run.png /tmp/ocelotgui-$VERSION/menu-run.png
cp -p $SOURCE/menu-settings.png /tmp/ocelotgui-$VERSION/menu-settings.png
cp -p $SOURCE/menu-options.png /tmp/ocelotgui-$VERSION/menu-options.png
cp -p $SOURCE/menu-debug.png /tmp/ocelotgui-$VERSION/menu-debug.png
cp -p $SOURCE/menu-help.png /tmp/ocelotgui-$VERSION/menu-help.png
cp -p $SOURCE/debugger.png /tmp/ocelotgui-$VERSION/debugger.png
cp -p $SOURCE/special-vertical.png /tmp/ocelotgui-$VERSION/special-vertical.png
cp -p $SOURCE/special-images.png /tmp/ocelotgui-$VERSION/special-images.png
cp -p $SOURCE/special-settings.png /tmp/ocelotgui-$VERSION/special-settings.png
cp -p $SOURCE/special-detach.png /tmp/ocelotgui-$VERSION/special-detach.png
cp -p $SOURCE/shot1.jpg /tmp/ocelotgui-$VERSION/shot1.jpg
cp -p $SOURCE/shot2.jpg /tmp/ocelotgui-$VERSION/shot2.jpg
cp -p $SOURCE/shot3.png /tmp/ocelotgui-$VERSION/shot3.png
cp -p $SOURCE/shot4.jpg /tmp/ocelotgui-$VERSION/shot4.jpg
cp -p $SOURCE/shot5.jpg /tmp/ocelotgui-$VERSION/shot5.jpg
cp -p $SOURCE/shot6.jpg /tmp/ocelotgui-$VERSION/shot6.jpg
cp -p $SOURCE/shot7.jpg /tmp/ocelotgui-$VERSION/shot7.jpg
cp -p $SOURCE/shot8.jpg /tmp/ocelotgui-$VERSION/shot8.jpg
cp -p $SOURCE/shot9.jpg /tmp/ocelotgui-$VERSION/shot9.jpg
cp -p $SOURCE/shot10.jpg /tmp/ocelotgui-$VERSION/shot10.jpg
cp -p $SOURCE/shot11.png /tmp/ocelotgui-$VERSION/shot11.png
cp -p $SOURCE/ocelotgui-logo.png /tmp/ocelotgui-$VERSION/ocelotgui-logo.png
cp -p $SOURCE/ocelotgui_logo.png /tmp/ocelotgui-$VERSION/ocelotgui_logo.png
cp -p $SOURCE/CMakeLists.txt /tmp/ocelotgui-$VERSION/CMakeLists.txt
cp -p $SOURCE/COPYING /tmp/ocelotgui-$VERSION/COPYING
cp -p $SOURCE/COPYING.thirdparty /tmp/ocelotgui-$VERSION/COPYING.thirdparty
cp -p $SOURCE/ocelotgui.1 /tmp/ocelotgui-$VERSION/ocelotgui.1
cp -p $SOURCE/copyright /tmp/ocelotgui-$VERSION/copyright
cp -p $SOURCE/changelog /tmp/ocelotgui-$VERSION/changelog
cp -p $SOURCE/rpmchangelog /tmp/ocelotgui-$VERSION/rpmchangelog
cp -p $SOURCE/rpm_pre_install.sh /tmp/ocelotgui-$VERSION/rpm_pre_install.sh
cp -p $SOURCE/rpm_post_install.sh /tmp/ocelotgui-$VERSION/rpm_post_install.sh
cp -p $SOURCE/rpm_pre_uninstall.sh /tmp/ocelotgui-$VERSION/rpm_pre_uninstall.sh
cp -p $SOURCE/rpm_post_uninstall.sh /tmp/ocelotgui-$VERSION/rpm_post_uninstall.sh
cp -p $SOURCE/ocelotgui.desktop /tmp/ocelotgui-$VERSION/ocelotgui.desktop
cp -p $SOURCE/ocelotgui.spec /tmp/ocelotgui-$VERSION/ocelotgui.spec
cp -p $SOURCE/rpm_build.sh /tmp/ocelotgui-$VERSION/rpm_build.sh
cp -p $SOURCE/tarantool.txt /tmp/ocelotgui-$VERSION/tarantool.txt
cp -p $SOURCE/third_party.h /tmp/ocelotgui-$VERSION/third_party.h
cp -p $SOURCE/debian/README.Debian /tmp/ocelotgui-$VERSION/debian/README.Debian
cp -p $SOURCE/debian/changelog /tmp/ocelotgui-$VERSION/debian/changelog
cp -p $SOURCE/debian/compat /tmp/ocelotgui-$VERSION/debian/compat
cp -p $SOURCE/debian/control /tmp/ocelotgui-$VERSION/debian/control
cp -p $SOURCE/debian/copyright /tmp/ocelotgui-$VERSION/debian/copyright
cp -p $SOURCE/debian/rules /tmp/ocelotgui-$VERSION/debian/rules
cp -p $SOURCE/debian/watch /tmp/ocelotgui-$VERSION/debian/watch
cp -p $SOURCE/debian/patches/series /tmp/ocelotgui-$VERSION/debian/patches/series
cp -p $SOURCE/debian/source/format /tmp/ocelotgui-$VERSION/debian/source/format
cp -p $SOURCE/debian/source/local-options /tmp/ocelotgui-$VERSION/debian/source/local-options
cp -p $SOURCE/deb_build.sh /tmp/ocelotgui-$VERSION/deb_build.sh

tar -zcvf ocelotgui-$VERSION.tar.gz ocelotgui-$VERSION
rm -r -f /tmp/debian3
mkdir /tmp/debian3
cd /tmp/debian3
cp /tmp/ocelotgui-$VERSION.tar.gz ocelotgui_$VERSION.orig.tar.gz
tar -xf ocelotgui_$VERSION.orig.tar.gz
cd ocelotgui-$VERSION/debian
debuild

#Optional tests
# The result of debuild should be ocelotgui_$VERSION-1_amd64.deb or ocelotgui_$VERSION-1_i386.deb if 32-bit
# Assuming platform is Ubuntu, $CODENAME will be trusty/senial/bionic/etc. Remember, Ocelot builds on xenial.
# Final lines of output should show: piuparts says "PASS: All tests." and lintian -I says nothing.
export CODENAME=`lsb_release --codename --short`
cd /tmp/debian3
export DEBFILENAME=`ls *.deb`
sudo piuparts $DEBFILENAME -d $CODENAME
lintian -I $DEBFILENAME
lintian -I $DEBFILENAME

