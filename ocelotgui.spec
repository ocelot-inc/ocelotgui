# ocelotgui.spec file for version 1.0.7 supplied by Ocelot Computer Services Inc. as part of ocelotgui package

#How to Build an .rpm file
#-------------------------
# 1. Install necessary packages. These might already be installed. Some distros prefer dnf to install.
# sudo yum install qt5-qttools-devel
# sudo yum install mysql-devel
# sudo yum install gcc gcc-c++ make cmake git
# sudo yum install rpm rpm-build rpmlint
# 2. Copy the ocelotgui tar.gz file to your $HOME directory.
# It is available on github. You might have downloaded it already.
# For example you might say: wget https://github.com/ocelot-inc/ocelotgui/releases/download/1.0.7/ocelotgui-1.0.7.tar.gz
# (For this step, we assume you know where you downloaded to. See later explanation in section "Re: Source".)
# You must copy it to $HOME/ocelotgui-1.0.7.tar.gz -- this is hard coded.
# If CmakeLists.txt was updated since the release, which is true for 1.0.7, Step #2 will not work -- use Step #2a instead.
# 2a. Alternative to step 2: make ocelotgui-1.0.7.tar.gz from a clone of the latest ocelotgui source.
# This is approximately the way that Ocelot makes new tar.gz files when it makes new releases.
# To produce $HOME/ocelotgui-1.0.7.tar.gz from the latest github source, say:
# cd /tmp
# rm -r -f ocelotgui
# git clone https://github.com/ocelot-inc/ocelotgui ocelotgui
# rm -r -f ocelotgui/.git
# tar -zcvf $HOME/ocelotgui-1.0.7.tar.gz ocelotgui
# rm -r -f ocelotgui
# 3. Remake ocelotgui-1.0.,7.tar.gz so it will unpack to directory ocelotgui-1.0.7 rather than to ocelotgui.
# This step is necessary because ocelotgui.spec will look for files in ocelotgui-1.0.7.
# To change $HOME/ocelotgui-1.0.7.tar.gz to be ready for rpm, say:
# cd /tmp
# rm -r -f ocelotgui
# rm -r -f ocelotgui-1.0.7
# cp -p $HOME/ocelotgui-1.0.7.tar.gz ocelotgui-1.0.7.tar.gz
# tar -xf ocelotgui-1.0.7.tar.gz
# mv ocelotgui ocelotgui-1.0.7
# tar -zcvf $HOME/ocelotgui-1.0.7.tar.gz ocelotgui-1.0.7
# 4. Copy the ocelotgui.spec file to your $HOME directory.
# (For this step, we assume you know where the spec file is. After all, it is what you are reading now.)
# You must copy it to $HOME/ocelotgui.spec -- this is hard coded.
# In the followin gline we assume the spec file is on $HOME/ocelotgui, but change to wherever it really is.
# cp -p $HOME/ocelotgui/ocelotgui.spec $HOME/ocelotgui.spec
# 5. Clear $HOME/ocelotgui_rpm. This is an arbitrary directory name that we use in step 6. Change it if you wish.
# rm -r -f $HOME/ocelotgui_rpm
# 6. Run rpmbuild using the $HOME/ocelotgui_rpm directory. Notice that we don't bother with an .rpmmacros file.
# rpmbuild -ba $HOME/ocelotgui.spec --define "_topdir $HOME/ocelotgui_rpm/rp/rpmbuild" --define "_sourcedir $HOME"
# 7. Find the resulting rpm in the RPMS subdirectory and check it. Here we assume the platform is x86-64.
# rpmlint ~/ocelotgui_rpm//rp/rpmbuild/RPMS/x86_64/ocelotgui-1.0.7-1.x86_64.rpm
# If it says "0 errors, 0 warnings", you're done!
# You can copy the .rpm file to a permanent location and remove the ~/ocelotgui_rpm directory.
# 8. With the .rpm file you can say
# sudo rpm -i ~/ocelotgui_rpm//rp/rpmbuild/RPMS/x86_64/ocelotgui-1.0.7-1.x86_64.rpm
# Of course, the .rpm file name will be different on a 32-bit platform.

#Re Group:
#  Usually this is Group: Applications/Databases
#  On Mageia we change it to Group: Databases
#Re Source:
#  The URL here is in fact the source of the ocelotgui release.
#  However, we commented it out because setup doesn't download it
#  and in any case it would unpack to ocelotgui not ocelotgui-1.0.7.
#  The assumption is that Source0: is the file name and the directory
#  is $HOME and the file needs pre-processing as described above.
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
#    (an .so file) at runtime if there is an attempt to connect to a MySQL
#    or MariaDB server.
#Re Prefix:
#    The line "Prefix: /usr" has been commented out because rpmlint
#    complained about it. We think putting it back in is a good idea.
#Re build:
#  * On Fedora if we pass -DCMAKE_INSTALL_DOCDIR we get an error.
#    On SUSE if we don't pass -DCMAKE_INSTALL_DOCDIR we get an error.
#Re install:
#  * On Fedora we let the macros expand and all is well.
#    On SUSE if we let the macros expand then the "cd" would not be to /build.
#Re files:
#  * We hardcode /usr/share/applications/ocelotgui.desktop instead
#    of using the desktopdir macro. That way we don't get an error,
#    but alas, we also don't get an installation on the desktop.
#    That is why we have to use desktop-file-install.
#Re ocelotgui-logo.png:
#  * When we just said ocelotgui-logo without a fle name extension,
#    which is supposedly the recommended style, we ended up with
#    /usr/share/pixmaps/ocelotgui-logo and it wasn't seen, the generic
#    logo appeared instead. So we insist on using an extension.
#    However, in ocelotgui.desktop we do not need an extension so we
#    change Icon=ocelotgui-logo.png to Icon=ocelotgui-logo, with sed.
#  * "rpm -i ..." will put the icon file in the right place but will
#    not add to favorites. For that, the user has to click Activities,
#    click on the search bar (where it says "Type to search ..."), search
#    ocelotgui, right click on the ocelotgui logo, click "Add to favorites".
#Re if and endif for different Linux distros:
#  The mdvver test is for Mageia.
#  The suse_version test is for openSUSE.
#  The else path is for Fedora but other distros will go on the same path.
#TODO
#----
# * Copy or download the file mentioned in "Source:", as part of ocelotgui.spec rather than a prerequisite.
# * Look at the flags that get passed to cmake, maybe they must be used (currently we are ignoring them).
# * Test on a completely new machine, because BuildRequires: might not have a complete list.
# * Remove old files: rpm_post_install.sh  rpm_post_uninstall.sh  rpm_pre_install.sh  rpm_pre_uninstall.sh
# * Keep track of howtobuild.txt
# * CXXFLAGS is exported but we end up with CMAKE_CXX_FLAGS = (blank)
# * (Mageia warnings) no-signature, no-packager-tag, manpage-not-compressed
# * (SUSE warnings) invalid-license GPLv2, non-standard-group Unspecified, package-with-huge-docs, position-independent-executable suggested
# * (Fedora-26 warnings) non-standard-group Databases, rpm-buildroot-usage
# It would be great to have ifdef equivalents for sourcedir etc.


%global __spec_install_post %{nil}
%global debug_package %{nil}
%global __os_install_post %{_dbpath}/brp-compress
%global _hardened_build 1


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
%else
Group:          Databases
%endif
Vendor:         Ocelot Computer Services Inc.
Url:            http://ocelot.ca
#Source0:        ocelotgui-1.0.7.tar.gz
Source:         https://github.com/ocelot-inc/ocelotgui/releases/download/1.0.7/ocelotgui-1.0.7.tar.gz

%if %{defined suse_version}
BuildRequires:  libqt5-qttools-devel
%else
%if "%?mdvver" != ""
BuildRequires:  qt5-devel
%else
BuildRequires:  qt5-qttools-devel
%endif
%endif
BuildRequires:  mysql-devel
BuildRequires:  gcc >= 5.1
BuildRequires:  gcc-c++ >= 5.1
BuildRequires:  make
BuildRequires:  cmake >= 2.8.11
BuildRequires:  sed
BuildRequires:  rpm rpm-build rpmlint
BuildRequires:  desktop-file-utils
Suggests: tarantool

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
%%setup -q

sed -i 's|Icon=%{name}-logo.png|Icon=%{name}-logo|g' %{_builddir}/%{name}-%{version}/%{name}.desktop


%build
%if %{defined suse_version}
%cmake %{_builddir}/%{name}-%{version} -DPACKAGE_TYPE="RPM" -DUSE_RPATH=FALSE -DCMAKE_INSTALL_DOCDIR=%{_docdir}
%else
%if "%?mdvver" != ""
%cmake %{_builddir}/%{name}-%{version} -DPACKAGE_TYPE="RPM" -DUSE_RPATH=FALSE -DCMAKE_INSTALL_DOCDIR=%{_docdir}/%{name}
%else
%cmake . -DPACKAGE_TYPE="RPM" -DUSE_RPATH=FALSE
%endif
%endif
%make_build

%if %{defined suse_version}
cd %{_builddir}
cd %{name}-%{version}/build
make DESTDIR=%{buildroot} install
%else
%if "%?mdvver" != ""
%install
cd %{_builddir}
cd %{name}/%{version}/build
%make_install
%else
%install
%make_install
%endif
%endif

%files
%defattr(-,root,root,-)
%{_bindir}/ocelotgui
%{_mandir}/man1/ocelotgui.1*
%{_docdir}/ocelotgui/COPYING
%{_docdir}/ocelotgui/LICENSE.GPL
%{_docdir}/ocelotgui/README.htm
%{_docdir}/ocelotgui/copyright
%{_docdir}/ocelotgui/debugger.png
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
%{_datadir}/applications/ocelotgui.desktop
%{_datadir}/pixmaps/ocelotgui-logo.png


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
