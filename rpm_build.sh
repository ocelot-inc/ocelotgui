# rpmbuild.sh -- produce ocelotgui-1.0.7 .rpm from ~/ocelotgui

# Ocelot uses this script to produce a .rpm file from a source directory ~/ocelotgui on Fedora 28.
# Only advanced users will want this, ordinary users will download the official-release .rpm file or make with cmake.

# An easy way to get ~/ocelotgui is "cd ~" + "git clone https://github.com/ocelot-inc/ocelotgui ocelotgui".
# This script uses /tmp and cheerily deletes existing files in /tmp.
# This script uses $HOME and cheerily overwrites $HOME/ocelotgui-$VERSION.tar.gz ocelotgui-$VERSION + $HOME/ocelotgui-$VERSION + $HOME/ocelotgui.spec.
# This script uses $HOME/rpm_build and cheerily deletes existing files in $HOME/rpm_build.
# The result will be / ~/ocelotgui_rpm//rp/rpmbuild/RPMS/x86_64/ocelotgui-1.0.7-1.x86_64.rpm or  ~/ocelotgui_rpm//rp/rpmbuild/RPMS/i686/ocelotgui-1.0.7-1.i686.rpm
# See also the comments in ocelotgui.spec.

#For SUSE, replace following line with sudo yum install libqt5-qttools-devel
sudo yum install qt5-qttools-devel
#Optionally, replace following line with sudo yum install mariadb-devel 
sudo yum install mysql-devel
sudo yum install gcc gcc-c++ make cmake
sudo yum install sed rpm rpm-build rpmlint desktop-file-utils

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

rm -r -f $HOME/ocelotgui-$VERSION.tar.gz
rm -r -f $HOME/ocelotgui-$VERSION
cd /tmp
tar -zcvf $HOME/ocelotgui-$VERSION.tar.gz ocelotgui-$VERSION
cd ~
tar -xf ocelotgui-$VERSION.tar.gz

rm -r -f $HOME/ocelotgui_rpm
cp ~/ocelotgui-1.0.7/ocelotgui.spec $HOME/ocelotgui.spec

rpmbuild -ba $HOME/ocelotgui.spec --define "_topdir $HOME/ocelotgui_rpm/rp/rpmbuild" --define "_sourcedir $HOME"

#Optional tests
# The result of rpmbuild should be ocelotgui_$VERSION-1.x86_64.rpm or ocelotgui_$VERSION-1_i686.deb if 32-bit
# Final lines of output should show: rpmlint says 0 errors, 0 warnings.
rpmlint ~/ocelotgui.spec
rpmlint ~/ocelotgui_rpm//rp/rpmbuild/RPMS/x86_64/ocelotgui-1.0.7-1.x86_64.rpm




# rpm_build.sh -- used by Ocelot to produce .rpm file for release 1.0.7

# This utility is deprecated and will be removed in a later version.
# For the newer way to produce an .rpm file, see the comments in ocelotgui.spec.

# rpm prerequissites are:
# dnf install gcc rpm-build rpm-devel rpmlint make python bash coreutils diffutils patch rpmdevtools

# We produced ocelotgui-1.0.7.x86_64.rpm with Fedora Release 26 64-bit thus:
# su root
# yum install qt5-qttools-devel
# yum install mysql mysql-devel
# yum install gcc gcc-c++ make cmake git
# yum install rpm rpm-build rpmlint
# exit
# cd ~
# git clone https://github.com/ocelot-inc/ocelotgui
# cd ocelotgui
# make clean
# rm -r -f _CPack*
# rm -f CMakeCache.txt
# rm -r -f ocelotgui-1.0.7/usr
# rm -r -f ocelotgui_autogen*
# cmake . -DQT_VERSION=5 -DCPACK_GENERATOR="RPM" -DCMAKE_INSTALL_PREFIX="ocelotgui-1.0.7/usr"
# make
# make install
# chmod +x rpm_build.sh
# ./rpm_build.sh
# rpmlint ./rp/rpmbuild/RPMS/ocelotgui-1.0.7-1.x86_64.rpm

# We produced ocelotgui-1.0.7qt4-1.x86_64.rpm the same way, except:
# yum install qt4-devel
# cmake . -DQT_VERSION=4 -DCPACK_GENERATOR="RPM" -DCMAKE_INSTALL_PREFIX="ocelotgui-1.0.7/usr"
# cmake . -DQT_VERSION=4 -DCPACK_GENERATOR="RPM" -DFILENAME_EXTRA="qt4" -DCMAKE_INSTALL_PREFIX="ocelotgui-1.0.7/usr"
# (filename has to be changed manually, dunno why -DFILENAME_EXTRA didn't work)
# rpmlint ./rp/rpmbuild/RPMS/ocelotgui-1.0.7qt4-1.x86_64.rpm

# Instead of running rpm_build.sh we could produce an .rpm file by
# using the default CMAKE_INSTALL_PREFIX and running cpack,
# (cmake + make produce a different .spec file),
# but rpmlint would complain about it, so we have these extra steps.

# Requires ocelotgui.spec, which is supplied by Ocelot.

# Warning: This destroys ~/ocelotgui subdirectories if such subdirectories already exist.
# Warning: This destroys ~/.rpmmacros if such a file already exists.
# Warning: we ignored a warning about "cmp0071".
# Warning: there is no cleanup.
# Tip: on Mageia, change ocelotgui.spec "Group:" to "Group: Databases".

#cd ~/ocelotgui
#tar -czvf ocelotgui-1.0.7-for-rpm.tar.gz ocelotgui-1.0.7/
#rm -r rp
#mkdir rp
#cd rp
#rm ~/.rpmmacros
#cat <<EOF > ~/.rpmmacros
#%_topdir $HOME/ocelotgui/rp/rpmbuild
#%_ocelotguidir $HOME/ocelotgui
#EOF
#mkdir -p rpmbuild/{BUILD,BUILDROOT,RPMS,SOURCES,SPECS,SRPMS}
#cp ~/ocelotgui/ocelotgui-1.0.7-for-rpm.tar.gz ocelotgui-1.0.7-for-rpm.tar.gz
#cp ocelotgui-1.0.7-for-rpm.tar.gz rpmbuild/SOURCES/ocelotgui-1.0.7-for-rpm.tar.gz
#cp ~/ocelotgui/ocelotgui.spec ocelotgui.spec
#rpmbuild -ba ocelotgui.spec

