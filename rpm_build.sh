# rpm_build.sh -- used by Ocelot to produce .rpm file for release 1.0.7

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

cd ~/ocelotgui
tar -czvf ocelotgui-1.0.7-for-rpm.tar.gz ocelotgui-1.0.7/
rm -r rp
mkdir rp
cd rp
rm ~/.rpmmacros
cat <<EOF > ~/.rpmmacros
%_topdir $HOME/ocelotgui/rp/rpmbuild
%_ocelotguidir $HOME/ocelotgui
EOF
mkdir -p rpmbuild/{BUILD,BUILDROOT,RPMS,SOURCES,SPECS,SRPMS}
cp ~/ocelotgui/ocelotgui-1.0.7-for-rpm.tar.gz ocelotgui-1.0.7-for-rpm.tar.gz
cp ocelotgui-1.0.7-for-rpm.tar.gz rpmbuild/SOURCES/ocelotgui-1.0.7-for-rpm.tar.gz
cp ~/ocelotgui/ocelotgui.spec ocelotgui.spec
rpmbuild -ba ocelotgui.spec

