# rpmbuild.sh -- produce ocelotgui-2.0.0 .rpm from ~/ocelotgui

# Peter Gulutzan uses this script to produce a .rpm file from a source directory ~/ocelotgui on Fedora 28.
# Only advanced users will want this, ordinary users will download the official-release .rpm file or make with cmake.

# For 2.0.0 32-bit ocelotgui was on Fedora 26 from archives.fedoraproject.org/pub/archive/fedora-secondary/releases/26, with pae.

# An easy way to get ~/ocelotgui is "cd ~" + "git clone https://github.com/ocelot-inc/ocelotgui ocelotgui".
# This script uses /tmp and cheerily deletes existing files in /tmp.
# This script uses $HOME and cheerily overwrites $HOME/ocelotgui-$VERSION.tar.gz ocelotgui-$VERSION + $HOME/ocelotgui-$VERSION + $HOME/ocelotgui.spec.
# This script uses $HOME/rpm_build and cheerily deletes existing files in $HOME/rpm_build.
# The result will be / ~/ocelotgui_rpm//rp/rpmbuild/RPMS/x86_64/ocelotgui-2.0.0-1.x86_64.rpm or  ~/ocelotgui_rpm//rp/rpmbuild/RPMS/i686/ocelotgui-2.0.0-1.i686.rpm
# See also the comments in ocelotgui.spec.

#For SUSE, replace following line with sudo yum install libqt5-qttools-devel
#For Mageia, the name is libqt5-devel and dnf works with Mageia 7
sudo yum install qt5-qttools-devel
#Optionally, replace following line with sudo yum install mariadb-devel 
sudo yum install mysql-devel
sudo yum install gcc gcc-c++ make cmake
sudo yum install sed rpm rpm-build rpmlint desktop-file-utils
sudo yum install python

export VERSION=2.0.0
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
cp -p $SOURCE/completer_1.png /tmp/ocelotgui-$VERSION/completer_1.png
cp -p $SOURCE/completer_2.png /tmp/ocelotgui-$VERSION/completer_2.png
cp -p $SOURCE/completer_3.png /tmp/ocelotgui-$VERSION/completer_3.png
cp -p $SOURCE/conditional.png /tmp/ocelotgui-$VERSION/conditional.png
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
cp -p $SOURCE/explorer1.png /tmp/ocelotgui-$VERSION/explorer1.png
cp -p $SOURCE/explorer2.png /tmp/ocelotgui-$VERSION/explorer2.png
cp -p $SOURCE/explorer3.png /tmp/ocelotgui-$VERSION/explorer3.png
cp -p $SOURCE/explorer4.png /tmp/ocelotgui-$VERSION/explorer4.png
cp -p $SOURCE/explorer5.png /tmp/ocelotgui-$VERSION/explorer5.png
cp -p $SOURCE/explorer6.png /tmp/ocelotgui-$VERSION/explorer6.png
cp -p $SOURCE/explorer7.png /tmp/ocelotgui-$VERSION/explorer7.png
cp -p $SOURCE/explorer8.png /tmp/ocelotgui-$VERSION/explorer8.png
cp -p $SOURCE/explorer9.png /tmp/ocelotgui-$VERSION/explorer9.png
cp -p $SOURCE/ocelotgui-logo.png /tmp/ocelotgui-$VERSION/ocelotgui-logo.png
cp -p $SOURCE/ocelotgui_logo.png /tmp/ocelotgui-$VERSION/ocelotgui_logo.png
cp -p $SOURCE/CMakeLists.txt /tmp/ocelotgui-$VERSION/CMakeLists.txt
cp -p $SOURCE/COPYING /tmp/ocelotgui-$VERSION/COPYING
cp -p $SOURCE/COPYING.thirdparty /tmp/ocelotgui-$VERSION/COPYING.thirdparty
cp -p $SOURCE/ocelotgui.1 /tmp/ocelotgui-$VERSION/ocelotgui.1
cp -p $SOURCE/copyright /tmp/ocelotgui-$VERSION/copyright
cp -p $SOURCE/changelog /tmp/ocelotgui-$VERSION/changelog
cp -p $SOURCE/rpmchangelog /tmp/ocelotgui-$VERSION/rpmchangelog
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
cp -p $SOURCE/deb_build.sh /tmp/ocelotgui-$VERSION/deb_build.sh

cd /tmp

rm -r -f $HOME/ocelotgui-$VERSION.tar.gz
rm -r -f $HOME/ocelotgui-$VERSION
cd /tmp
tar -zcvf $HOME/ocelotgui-$VERSION.tar.gz ocelotgui-$VERSION
cd ~
tar -xf ocelotgui-$VERSION.tar.gz

rm -r -f $HOME/ocelotgui_rpm
cp ~/ocelotgui-2.0.0/ocelotgui.spec $HOME/ocelotgui.spec

rpmbuild -ba $HOME/ocelotgui.spec --define "_topdir $HOME/ocelotgui_rpm/rp/rpmbuild" --define "_sourcedir $HOME"

#Optional tests
# The result of rpmbuild should be ocelotgui_$VERSION-1.x86_64.rpm or ocelotgui_$VERSION-1_i686.rpm if 32-bit
# Final lines of output should show: rpmlint says 0 errors, 0 warnings.
rpmlint ~/ocelotgui.spec
rpmlint ~/ocelotgui_rpm//rp/rpmbuild/RPMS/x86_64/ocelotgui-2.0.0-1.x86_64.rpm
