# ocelotgui.spec file for version 2.4.0 supplied by Peter Gulutzan as part of ocelotgui package

#How to Build an .rpm file
#-------------------------
# (A version of this is also inside the rpm_build.sh script file.)
# 1. Install necessary packages. These might already be installed. Some distros prefer dnf to install.
# sudo yum install qt5-qttools-devel
# sudo yum install mysql-devel
# sudo yum install gcc gcc-c++ make cmake git
# sudo yum install rpm rpm-build rpmlint
# 2. Copy the ocelotgui tar.gz file to your $HOME directory.
# It is available on github. You might have downloaded it already.
# For example you might say: wget https://github.com/ocelot-inc/ocelotgui/releases/download/2.4.0/ocelotgui-2.4.0.tar.gz
# (For this step, we assume you know where you downloaded to. See later explanation in section "Re: Source".)
# You must copy it to $HOME/ocelotgui-2.4.0.tar.gz -- this is hard coded.
# If CmakeLists.txt was updated since the release, which is true for 2.4.0, Step #2 will not work -- use Step #2a instead.
# 2a. Alternative to step 2: make ocelotgui-2.4.0.tar.gz from a clone of the latest ocelotgui source.
# This is approximately the way that the ocelotgui developer makes new tar.gz files when making new releases.
# To produce $HOME/ocelotgui-2.4.0.tar.gz from the latest github source, say:
# cd /tmp
# rm -r -f ocelotgui
# git clone https://github.com/ocelot-inc/ocelotgui ocelotgui
# rm -r -f ocelotgui/.git
# tar -zcvf $HOME/ocelotgui-2.4.0.tar.gz ocelotgui
# rm -r -f ocelotgui
# 3. Remake ocelotgui-2.4.0.tar.gz so it will unpack to directory ocelotgui-2.4.0 rather than to ocelotgui.
# This step is necessary because ocelotgui.spec will look for files in ocelotgui-2.4.0.
# To change $HOME/ocelotgui-2.4.0.tar.gz to be ready for rpm, say:
# cd /tmp
# rm -r -f ocelotgui
# rm -r -f ocelotgui-2.4.0
# cp -p $HOME/ocelotgui-2.4.0.tar.gz ocelotgui-2.4.0.tar.gz
# tar -xf ocelotgui-2.4.0.tar.gz
# mv ocelotgui ocelotgui-2.4.0
# tar -zcvf $HOME/ocelotgui-2.4.0.tar.gz ocelotgui-2.4.0
# 4. Copy the ocelotgui.spec file to your $HOME directory.
# (For this step, we assume you know where the spec file is. After all, it is what you are reading now.)
# You must copy it to $HOME/ocelotgui.spec -- this is hard coded.
# In the following line we assume the spec file is on $HOME/ocelotgui, but change to wherever it really is.
# cp -p $HOME/ocelotgui/ocelotgui.spec $HOME/ocelotgui.spec
# 5. Clear $HOME/ocelotgui_rpm. This is an arbitrary directory name that we use in step 6. Change it if you wish.
# rm -r -f $HOME/ocelotgui_rpm
# 6. Run rpmbuild using the $HOME/ocelotgui_rpm directory. Notice that we don't bother with an .rpmmacros file.
# rpmbuild -ba $HOME/ocelotgui.spec --define "_topdir $HOME/ocelotgui_rpm/rp/rpmbuild" --define "_sourcedir $HOME"
# 7. Find the resulting rpm in the RPMS subdirectory and check it. Here we assume the platform is x86-64.
# rpmlint ~/ocelotgui_rpm//rp/rpmbuild/RPMS/x86_64/ocelotgui-2.4.0-1.x86_64.rpm
# If it says "0 errors, 0 warnings", you're done!
# You can copy the .rpm file to a permanent location and remove the ~/ocelotgui_rpm directory.
# 8. With the .rpm file you can say
# sudo rpm -i ~/ocelotgui_rpm//rp/rpmbuild/RPMS/x86_64/ocelotgui-2.4.0-1.x86_64.rpm
# Of course, the .rpm file name will be different on a 32-bit platform.
# Todo: nowadays yum install or dnf install is probably better, we'll say that in the main README.

#Re Group:
#  Usually this is Group: Applications/Databases
#  On Mageia: we change it to Group: Databases -- rpmlint doesn't seem to realize that this is
#  inside if/endif and complains anyway, but I can't think of anything to do about that except
#  have a separate .spec file for Mageia. See https://wiki.mageia.org/en/RPM_groups_policy
#  On Fedora: we could omit it nowadays. See https://fedoraproject.org/wiki/RPMGroups
#  On SUSE: Productivity/Databases/Clients|Tools. See https://en.opensuse.org/openSUSE:Package_group_guidelines
#Re Release:
#   Although 1[[percent-sign]{?{dist}, instead of 1, is the right thing for packaging
#   for a specific distro, it adds to the .rpm name, and we don't want that for a
#   package that has to work with more than one version.
#Re Source:
#  The URL here is in fact the source of the ocelotgui release.
#  However, we commented it out because setup doesn't download it
#  and in any case it would unpack to ocelotgui not ocelotgui-2.4.0.
#  The assumption is that Source0: is the file name and the directory
#  is $HOME and the file needs pre-processing as described above.
#  Todo: Consider that this is possible:
#        wget -P ~/ https://github.com/ocelot-inc/ocelotgui/archive/master.tar.gz
#Re Packager:
#  I used to say Packager: Peter Gulutzan but rpmlint warns hardcoded-packager-tag
#  Saying Packager:\n causes rpmlint to be quiet but you'll get an error if you try to use it.
#  So I remove Packager: altogether although rpmlint might warn no-packager-tag.
#Re Build-Requires:
#  * qt5-qttools-devel implies that we assume Qt version 5.
#    In fact Qt version 4 will work well. So will Qt version 6, see also "Re Qt6" below.
#  * mysql-devel implies that we assume MySQL.
#    In fact MariaDB will work well so mariadb-devel would be good too.
#    (or maybe mysql-compat-client?)
#    The requirement exists because our source has "#include mysql.h".
#  * All the other Build-Requires packages are common utilities
#    that are easily available on any rpm-based distro.
#    ocelotgui does not require a MySQL or MariaDB server to build.
#    ocelotgui will try to load a MySQL or MariaDB client library
#    (an .so file) at runtime if there is an attempt to connect to a MySQL
#    or MariaDB server.
#Re Requires:
#    This is left out because the required packages (glibc, libstdc, Qt)
#    are present in the core.
#Re Prefix:
#    The line "Prefix: /usr" has been commented out because rpmlint
#    complained about it. We think putting it back in is a good idea.
#Re build:
#  * On Fedora if we pass -DCMAKE_INSTALL_DOCDIR we get an error.
#    On SUSE or Mageia if we don't pass -DCMAKE_INSTALL_DOCDIR we get an error.
#Re install:
#  * On Fedora we let the macros expand and all is well.
#    On SUSE if we let the macros expand then the "cd" would not be to /build.
#  * On SUSE we have to tell CMakeLists.to add -pie for the linker.
#  * On SUSE and Mageia we need to cd to the build subdirectory.
#    On Fedora this is automatic.
#    So no problem -- but a mystery that it's not automatic for everybody.
#Re files:
#  * We hardcode /usr/share/applications/ocelotgui.desktop instead
#    of using the desktopdir macro. That way we don't get an error,
#    but alas, we also don't get an installation on the desktop.
#    Perhaps we should use desktop-file-install.
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
#  Convoluted "if" conditions containing "mdvver" are tests whether mdvver is defined as in Mageia.
#  Convoluted "if" conditions containing "suse_version" are tests whether suse_version is defined as in openSuSE.
#  The "defined" macro is not dependable.
#  The else path is for Fedora but other distros will go on the same path.
#Re: __cmake_in_source_build
#  Starting in Fedora 33 there was a huge change affecting existing behaviour
#  https://lists.fedoraproject.org/archives/list/devel@lists.fedoraproject.org/thread/P6FQAHOWSR5FTNJXEW6AWFAZQT7RROAS/
#  which this cancels, but it has to be regarded as temporary. 
#Re Qt6
#  If the intent is to link with Qt version 6 instead of 5 or 4, changes are small:
#   In rpm_build.sh, if you use it, you must first install qt6_qttools_devel
#   Here in ocelotgui.spec in BuildRequires you must change all occurrences of "qt5" to "qt6"
#   Here in ocelotgui.spec add -DQT_VERSION=6 for the cmake because cmake will look first for Qt5

#TODO
#----
# * Copy or download the file mentioned in "Source:", as part of ocelotgui.spec rather than a prerequisite.
# * Currently we pass suggested c_flags and cxx_flags but not ldflags.
# * Test on a completely new machine, because BuildRequires: might not have a complete list.
# * Keep track of howtobuild.txt
# * (Mageia warnings) no-signature (this can be ignored), manpage-not-compressed
# * (Fedora-26 warnings) non-standard-group Databases. See description of Group, above.
# * Instead of if/endif for 3 distros, make 3 directories = rpm_fedora | rpm_suse | rpm_mageia,
#   and each directory contains an ocelotgui.spec file that's only got the spec for that distro.
#   Maybe even more directories, for fedora35-with-qt6 etc.
# It would be great to have ifdef equivalents for sourcedir etc.
# (fixed?) rpmlint will warn "standard-dir-owned-by-package" for /usr/share/man and /usr/share/man/man1, which we ignore.
# (fixed?) rpmlint will warn "no-signature", which we ignore.
# (fixed?) rpmlint will warn "no-packager-tag", which we ignore.
# Change to out-of-source builds for CMake.

%if %{?suse_version:1}%{!?suse_version:0}
#
# spec file for package ocelotgui
#
# Copyright (c) 2024 by Peter Gulutzan
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# (The above text is copied for legal reasons from a specfile guideline:
#  https://en.opensuse.org/openSUSE:Specfile_guidelines)
%endif

%global debug_package %{nil}
%global _hardened_build 1
%global __cmake_in_source_build 1

# Restore old style debuginfo creation for rpm >= 4.14.
%undefine _debugsource_packages
%undefine _debuginfo_subpackages

Summary:        GUI client for MySQL or MariaDB
Name:           ocelotgui
Version:        2.4.0
Release:        1

%if %{?suse_version:1}%{!?suse_version:0}
License:        GPL-2.0-only
%else
License:        GPLv2
%endif

%if %{?mdvver:1}%{!?mdvver:0}
Group:          Applications/Databases
%else
%if %{?suse_version:1}%{!?suse_version:0}
Group:          Productivity/Databases/Clients
%else
Group:          Databases
%endif
%endif
Vendor:         Peter Gulutzan
Url:            http://ocelot.ca
#Source0:        ocelotgui-2.4.0.tar.gz
Source:         https://github.com/ocelot-inc/%name/releases/download/2.4.0/%name-%{version}.tar.gz

%if %{?suse_version:1}%{!?suse_version:0}
BuildRequires:  libqt5-qttools-devel
%else
%if %{?mdvver:1}%{!?mdvver:0}
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

#Prefix: /usr

%description
GUI client for MySQL or MariaDB or similar servers
 Users can connect to a DBMS server, enter SQL statements, and receive results.
 Some features are: syntax highlighting, user-settable colors
 and fonts for each part of the screen, result-set displays
 with rows that can have multiple lines and columns that can be dragged,
 and a debugger.

%prep
%%setup -q

sed -i 's|Icon=%{name}-logo.png|Icon=%{name}-logo|g' %{_builddir}/%{name}-%{version}/%{name}.desktop

%build
%if %{?suse_version:1}%{!?suse_version:0}
%cmake %{_builddir}/%{name}-%{version} -DPACKAGE_TYPE="RPM" -DCMAKE_SKIP_RPATH=TRUE -DCMAKE_INSTALL_DOCDIR=%{_docdir}/%{name} \
       -DOCELOT_C_FLAGS:STRING="%optflags" -DOCELOT_CXX_FLAGS:STRING="%optflags" -DOCELOT_LD_FLAGS:STRING="-pie"
%else
%if %{?mdvver:1}%{!?mdvver:0}
%cmake %{_builddir}/%{name}-%{version} -DPACKAGE_TYPE="RPM" -DCMAKE_SKIP_RPATH=TRUE -DCMAKE_INSTALL_DOCDIR=%{_docdir}/%{name} \
       -DOCELOT_C_FLAGS:STRING="%optflags" -DOCELOT_CXX_FLAGS:STRING="%optflags"
%else
%cmake . -DPACKAGE_TYPE="RPM" -DCMAKE_SKIP_RPATH=TRUE \
       -DOCELOT_C_FLAGS:STRING="%optflags" -DOCELOT_CXX_FLAGS:STRING="%optflags"
%endif
%endif
%make_build

%if %{?suse_version:1}%{!?suse_version:0}
%install
cd %{_builddir}/%{name}-%{version}/build
%make_install
%else
%if %{?mdvver:1}%{!?mdvver:0}
%install
cd %{_builddir}/%{name}-%{version}/build
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
%doc completer_1.png
%doc completer_2.png
%doc completer_3.png
%doc conditional.png
%doc COPYING
%doc LICENSE.GPL
%doc README.htm
%doc copyright
%doc debugger.png
%doc example.cnf
%doc manual.htm
%doc menu-debug.png
%doc menu-edit.png
%doc menu-file.png
%doc menu-help.png
%doc menu-options.png
%doc menu-run.png
%doc menu-settings.png
%doc ocelotgui_logo.png
%doc result-widget-example.png
%doc shot1.jpg
%doc shot2.jpg
%doc shot3.png
%doc shot4.jpg
%doc shot5.jpg
%doc shot6.jpg
%doc shot7.jpg
%doc shot8.jpg
%doc shot9.jpg
%doc shot10.jpg
%doc shot11.png
%doc explorer1.png
%doc explorer2.png
%doc explorer3.png
%doc explorer4.png
%doc explorer5.png
%doc explorer6.png
%doc explorer7.png
%doc explorer8.png
%doc explorer9.png
%doc special-detach.png
%doc special-images.png
%doc special-settings.png
%doc special-vertical.png
%doc starting-dialog.png
%doc starting.png
%doc menunew.png
%doc plugin.c
%doc statement-widget-example.png
%{_datadir}/applications/ocelotgui.desktop
%{_datadir}/pixmaps/ocelotgui-logo.png

%changelog
* Tue Jun 04 2024 Peter Gulutzan <pgulutzan at ocelot.ca> - 2.4.0-1
  Plugins.
* Mon Mar 18 2024 Peter Gulutzan <pgulutzan at ocelot.ca> - 2.3.0-1
  Make SHOW as good as SELECT.
* Thu Feb 15 2024 Peter Gulutzan <pgulutzan at ocelot.ca> - 2.2.0-1
  New server syntax.
* Fri Sep 29 2023 Peter Gulutzan <pgulutzan at ocelot.ca> - 2.1.0-1
  Improved Chart.
* Wed Jun 07 2023 Peter Gulutzan <pgulutzan at ocelot.ca> - 2.0.0-1
  Chart.
* Tue Mar 07 2023 Peter Gulutzan <pgulutzan at ocelot.ca> - 1.9.0-1
  ERDiagram.
* Wed Nov 16 2022 Peter Gulutzan <pgulutzan at ocelot.ca> - 1.8.0-1
  Explorer.
* Wed Jun 08 2022 Peter Gulutzan <pgulutzan at ocelot.ca> - 1.7.0-1
  Rewrite of detach code.
* Sun Jan 09 2022 Peter Gulutzan <pgulutzan at ocelot.ca> - 1.6.0-1
  Exports.
* Mon Jul 19 2021 Peter Gulutzan <pgulutzan at ocelot.ca> - 1.5.0-1
  Rewrite of result grid code.
* Fri Mar 19 2021 Peter Gulutzan <pgulutzan at ocelot.ca> - 1.4.0-1
  Better handling of history files.
* Tue Feb 09 2021 Peter Gulutzan <pgulutzan at ocelot.ca> - 1.3.0-1
  Conditional settings.
* Thu Oct 29 2020 Peter Gulutzan <pgulutzan at ocelot.ca> - 1.2.0-1
  Better autocomplete.
* Fri Jul 31 2020 Peter Gulutzan <pgulutzan at ocelot.ca> - 1.1.0-1
- Flexible grid.
* Sat Jun 29 2019 Peter Gulutzan <pgulutzan at ocelot.ca> - 1.0.9-1
- Some bug fixes.
* Tue Jan 08 2019 Peter Gulutzan <pgulutzan at ocelot.ca> - 1.0.8-1
- Better detached mode.
* Wed Aug 29 2018 Peter Gulutzan <pgulutzan at ocelot.ca> - 1.0.7-1
- Synch with newer versions of DBMS servers.
* Tue Dec 12 2017 Peter Gulutzan <pgulutzan at ocelot.ca> - 1.0.6-1
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
