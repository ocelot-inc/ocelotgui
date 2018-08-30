# See comments in rpm_build.sh

%define __spec_install_post %{nil}
%define debug_package %{nil}
%define __os_install_post %{_dbpath}/brp-compress


# Restore old style debuginfo creation for rpm >= 4.14.
%undefine _debugsource_packages
%undefine _debuginfo_subpackages

#%define _rpmdir %_topdir/RPMS
#%define _srcrpmdir %_topdir/SRPMS
#%define _rpmfilename ocelotgui-1.0.7-1.x86_64.rpm
#%define _unpackaged_files_terminate_build 0

# BuildRoot:      %_topdir/ocelotgui-1.0.7-1.x86_64
Summary:        GUI client for MySQL or MariaDB
Name:           ocelotgui
Version:        1.0.7
Release:        1
License:        GPLv2
Group:          Applications/Databases
Vendor:         Ocelot Computer Services Inc.
Url:            http://ocelot.ca
Source0:        ocelotgui-1.0.7-for-rpm.tar.gz
Prefix: /usr


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
%setup -q

%build
%install
rm -rf %{buildroot}
mkdir -p %{buildroot}
cp -a * %{buildroot}
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
