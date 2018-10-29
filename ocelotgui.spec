# ocelotgui.spec supplied by Ocelot Computer Services Inc. as part of ocelotgui package

#How to Build an .rpm file
#-------------------------
# 1. Install necessary packages. These might already be installed. On some distros prefer dnf rather than yum.
# sudo yum install qt5-qttools-devel
# sudo yum install mysql mysql-devel
# sudo yum install gcc gcc-c++ make cmake git
# sudo yum install rpm rpm-build rpmlint
# 2. Copy the ocelotgui tar.gz file to your $HOME directory.
# It is available on github. You might have downloaded it already.
# For example you might say: wget https://github.com/ocelot-inc/ocelotgui/releases/download/1.0.7/ocelotgui-1.0.7.tar.gz
# (For this step, we assume you know where you downloaded to. See later explanation in section "Re: Source".)
# You must copy it to $HOME/ocelotgui-1.0.7.tar.gz -- this is hard coded.
# 3. Copy the ocelotgui.spec file to your $HOME directory.
# (For this step, we assume you know where the spec file is. After all, it is what you are reading now.)
# You must copy it to $HOME/ocelotgui.spec -- this is hard coded.
# 4. Make sure you have nothing important in directory $HOME/ocelotgui_rpm, because it will be overwritten.
# 5. Run rpmbuild writing to the ~/ocelotgui_rpm and reading $HOME/ocelotgui.spec + $HOME/ocelotgui-1.0.7.tar.gz: 
# The command must look like this. We don't bother with an .rpmmacros file, therefore we must specify with --define.
# rpmbuild -ba $HOME/ocelotgui.spec --define "_topdir $HOME/ocelotgui_rpm/rp/rpmbuild"
# 7. Find the resulting rpm in the RPMS subdirectory and check it. Here we assume the platform is x86-64.
# rpmlint ~/ocelotgui_rpm//rp/rpmbuild/RPMS/x86_64/ocelotgui-1.0.7-1.x86_64.rpm
# If it says "0 errors, 0 warnings", you're done!
# 8. You can copy the .rpm file to a permanent location and remove the $HOME/ocelotgui_rpm directory.

#Re Group:
#  Usually this is Group: Applications/Databases
#  On Mageia we change it to Group: Databases
#Re Source0:
#  The URL here is in fact the source of the ocelotgui release.
#  However, setup does not work. That is one reason that we require putting the tar gz file in $HOME.
#  The other reason is that the regular tar directory is "ocelotgui", and we need it to be
#  "ocelotgui-1.0.7". Therefore, as you can see by looking at the prep section below, some
#  renaming takes place before the tar gz file is copied to $HOME/rp/rpmbuild/SOURCES.
#  Of course you can make your own tar gz file from the current sources,
#  i.e. git clone to directory ocelotgui and then tar -zcvf directory ocelotgui.
#Re Build-Requires:
#  * qt5-qttools-devel implies that we assume Qt version 5.
#    In fact Qt version 4 will work well.
#  * mysql-devel implies that we assume MySQL.
#    In fact MariaDB will work well.
#    The requirement exists because our source has "#include mysql.h".
#  * All the other Build-Requires packages are common utilities
#    that are easily available on any rpm-based distro.
#    ocelotgui does not require a MySQL or MariaDB server to build.
#    ocelotgui will try to load a MySQL or MariaDB client library
#    (an .so file) if there is an attempt to connect to a MySQL or
#    MariaDB server.
#Re Prefix:
#    The line "Prefix: /usr" has been commented out because rpmlint
#    complained about it. We think putting it back in is a good idea.
#Re build:
#  * The "rm" commands in this section are an attempted cleanup so
#    that there won't be error messages saying
#    "Installed (but unpackaged) file(s) found".
#    Probably there is a more standard way to do this.
#Re files:
#  * We hardcode /usr/share/applications/ocelotgui.desktop instead
#    of using the desktopdir macro. That way we don't get an error,
#    but alas, we also don't get an installation on the desktop.
#Re cleanup:
#  It's not automatic but it's simple: rm -r -f $HOME/ocelotgui_rpm.
#Re Qt4:
#  * I am assuming that users prefer Qt5.
#    Users who prefer Qt4 should:
#    Change the end of the cmake line to -DPACKAGE_TYPE="RPM" -DQT_VERSION=4
#    Change the BuildRequires "qt5-qttools-devel" to "libqt4-devel"

#TODO
#----
# * It would be possible to wget the file as the first step in the prep section.
# * Look at the flags that get passed to cmake, maybe they must be used (currently we are ignoring them).
# * Test on a completely new machine, because BuildRequires: might not have a complete list.
# * Remove old files: rpm_build.sh rpm_post_install.sh rpm_post_uninstall.sh rpm_pre_install.sh rpm_pre_uninstall.sh
# * Keep track of howtobuild.txt
# * Fix the desktop problem
# * End the hard coding
# * Add error checks and explanatory messages for the lines that were added in the prep section.
# * Find out why now you're saying qt5-qttools-devel when before it was just libqt5-devel.
# * Correct package name for package containing mysql.h might be lib64mariadb-devel or libmysqlclient-devel or mariadb-devel

%define __spec_install_post %{nil}
%define debug_package %{nil}
%define __os_install_post %{_dbpath}/brp-compress

# Restore old style debuginfo creation for rpm >= 4.14.
%undefine _debugsource_packages
%undefine _debuginfo_subpackages

Summary:        GUI client for MySQL or MariaDB
Name:           ocelotgui
Version:        1.0.7
Release:        1
License:        GPLv2
# This "if" should be true for Mageia
%if "%?mdvver" != ""
Group:          Applications/Databases
#else
Group:          Databases
%endif
Vendor:         Ocelot Computer Services Inc.
Url:            http://ocelot.ca
#Source0:        ocelotgui-1.0.7.tar.gz
Source:         https://github.com/ocelot-inc/ocelotgui/releases/download/1.0.7/ocelotgui-1.0.7.tar.gz
BuildRequires:  qt5-qttools-devel
BuildRequires:  mysql-devel
BuildRequires:  gcc gcc-c++ make cmake
BuildRequires:  rpm rpm-build rpmlint

#Prefix: /usr

Requires(post): info
Requires(preun): info

%description
Ocelot GUI (ocelotgui), a database client, allows users to connect to
 a MySQL or MariaDB DBMS server, enter SQL statements, and receive results.
 Some of its features are: syntax highlighting, user-settable colors
 and fonts for each part of the screen, result-set displays
 with rows that can have multiple lines and columns that can be dragged,
 and a debugger.
 
%prep
rm -r -f $HOME/ocelotgui_rpm
mkdir $HOME/ocelotgui_rpm $HOME/ocelotgui_rpm/rp $HOME/ocelotgui_rpm/rp/rpmbuild
mkdir -p --verbose $HOME/ocelotgui_rpm/rp/rpmbuild/{BUILD,RPMS,SOURCES,SPECS,SRPMS}
cp $HOME/ocelotgui-1.0.7.tar.gz $HOME/ocelotgui_rpm/ocelotgui-1.0.7.tar.gz
cd $HOME/ocelotgui_rpm
tar -xf ocelotgui-1.0.7.tar.gz
mv ocelotgui ocelotgui-1.0.7
tar -zcvf $HOME/ocelotgui_rpm/rp/rpmbuild/SOURCES/ocelotgui-1.0.7.tar.gz ocelotgui-1.0.7 
%%setup -q

%build
%cmake %{_builddir}/ocelotgui-1.0.7 -DPACKAGE_TYPE="RPM"
make
rm CMakeCache.txt
rm -r CMakeFiles
rm *.cmake
rm Makefile

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}
mkdir -p %{buildroot}/%{_docdir}
mkdir -p %{buildroot}/%{_docdir}/ocelotgui
cp %{_builddir}/ocelotgui-1.0.7/COPYING %{buildroot}/%{_docdir}/ocelotgui/COPYING
cp %{_builddir}/ocelotgui-1.0.7/LICENSE.GPL %{buildroot}/%{_docdir}/ocelotgui/LICENSE.GPL
cp %{_builddir}/ocelotgui-1.0.7/*.png %{buildroot}/%{_docdir}/ocelotgui
cp %{_builddir}/ocelotgui-1.0.7/*.jpg %{buildroot}/%{_docdir}/ocelotgui
cp %{_builddir}/ocelotgui-1.0.7/README.txt %{buildroot}/%{_docdir}/ocelotgui/README.txt
cp %{_builddir}/ocelotgui-1.0.7/debugger_reference.txt %{buildroot}/%{_docdir}/ocelotgui/debugger_reference.txt
cp %{_builddir}/ocelotgui-1.0.7/options.txt %{buildroot}/%{_docdir}/ocelotgui/options.txt
cp %{_builddir}/ocelotgui-1.0.7/tarantool.txt %{buildroot}/%{_docdir}/ocelotgui/tarantool.txt
cp %{_builddir}/ocelotgui-1.0.7/*.md %{buildroot}/%{_docdir}/ocelotgui
cp %{_builddir}/ocelotgui-1.0.7/*.htm %{buildroot}/%{_docdir}/ocelotgui
cp %{_builddir}/ocelotgui-1.0.7/copyright %{buildroot}/%{_docdir}/ocelotgui/copyright
cp %{_builddir}/ocelotgui-1.0.7/example.cnf %{buildroot}/%{_docdir}/ocelotgui/example.cnf
mkdir -p %{buildroot}/%{_bindir}
cp %{_builddir}/ocelotgui-1.0.7/ocelotgui %{buildroot}/%{_bindir}/ocelotgui
mkdir -p %{buildroot}/%{_mandir}
mkdir -p %{buildroot}/%{_mandir}/man1
cp %{_builddir}/ocelotgui-1.0.7/ocelotgui.1.gz %{buildroot}/%{_mandir}/man1/ocelotgui.1.gz
mkdir -p %{buildroot}/usr
mkdir -p %{buildroot}/usr/share
mkdir -p %{buildroot}/usr/share/applications
cp %{_builddir}/ocelotgui-1.0.7/ocelotgui.desktop %{buildroot}/usr/share/applications/ocelotgui.desktop
mkdir -p %{buildroot}/usr/share/pixmaps
cp %{_builddir}/ocelotgui-1.0.7/ocelotgui_logo.png %{buildroot}/usr/share/pixmaps/ocelotgui_logo
%clean

%files
%defattr(-,root,root,-)
%{_bindir}/ocelotgui
%{_mandir}/man1/ocelotgui.1.gz
/usr/share/applications/ocelotgui.desktop
%{_docdir}/ocelotgui/COPYING
%{_docdir}/ocelotgui/LICENSE.GPL
%{_docdir}/ocelotgui/README.htm
%{_docdir}/ocelotgui/README.md
%{_docdir}/ocelotgui/README.txt
%{_docdir}/ocelotgui/copyright
%{_docdir}/ocelotgui/debugger.png
%{_docdir}/ocelotgui/debugger_reference.txt
%{_docdir}/ocelotgui/example.cnf
%{_docdir}/ocelotgui/manual.htm
%{_docdir}/ocelotgui/menu-debug.png
%{_docdir}/ocelotgui/menu-edit.png
%{_docdir}/ocelotgui/menu-file.png
%{_docdir}/ocelotgui/menu-help.png
%{_docdir}/ocelotgui/menu-options.png
%{_docdir}/ocelotgui/menu-run.png
%{_docdir}/ocelotgui/menu-settings.png
%{_docdir}/ocelotgui/ocelotgui_logo.png
%{_docdir}/ocelotgui/options.txt
%{_docdir}/ocelotgui/result-widget-example.png
%{_docdir}/ocelotgui/shot1.jpg
%{_docdir}/ocelotgui/shot10.jpg
%{_docdir}/ocelotgui/shot11.png
%{_docdir}/ocelotgui/shot2.jpg
%{_docdir}/ocelotgui/shot3.png
%{_docdir}/ocelotgui/shot4.jpg
%{_docdir}/ocelotgui/shot5.jpg
%{_docdir}/ocelotgui/shot6.jpg
%{_docdir}/ocelotgui/shot7.jpg
%{_docdir}/ocelotgui/shot8.jpg
%{_docdir}/ocelotgui/shot9.jpg
%{_docdir}/ocelotgui/special-detach.png
%{_docdir}/ocelotgui/special-images.png
%{_docdir}/ocelotgui/special-settings.png
%{_docdir}/ocelotgui/special-vertical.png
%{_docdir}/ocelotgui/starting-dialog.png
%{_docdir}/ocelotgui/starting.png
%{_docdir}/ocelotgui/statement-widget-example.png
%{_docdir}/ocelotgui/tarantool.txt
/usr/share/pixmaps/ocelotgui_logo

%changelog
* Wed Aug 29 2018 Peter Gulutzan <pgulutzan at ocelot.ca> - 1.0.7-1
- Synch with newer versions of DBMS servers.
* Tue Dec 12 2017 Peter Gulutzan <pgulutzan at ocelot.ca> - 1.0.7-1
- Some bug fixes.
* Sun Jul 02 2017 Peter Gulutzan <pgulutzan at ocelot.ca> - 1.0.5-1
- Syntax checker understands more dialects.
* Mon Dec 12 2016 Peter Gulutzan <pgulutzan at ocelot.ca> - 1.0.4-1
- New options html, xml, ocelot_language.
* Tue Sep 27 2016 Peter Gulutzan <pgulutzan at ocelot.ca> - 1.0.3-1
- Better default font, new formatter and history-output options
* Mon Aug 15 2016 Peter Gulutzan <pgulutzan at ocelot.ca> - 1.0.2-1
- Some bug fixes.
* Fri Jul 01 2016 Peter Gulutzan <pgulutzan at ocelot.ca> - 1.0.1-1
- Some bug fixes.
* Fri May 27 2016 Peter Gulutzan <pgulutzan at ocelot.ca> - 1.0.0-1
- Initial release.
