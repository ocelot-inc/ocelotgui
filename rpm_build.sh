# WARNING: This destroys ~/.rpmmacros if such a file already exists.
cd ~/ocelotgui
tar -czvf ocelotgui-1.0.5.tar.gz ocelotgui-1.0.5/
rm -r rp
mkdir rp
cd rp
rm ~/.rpmmacros
cat <<EOF > ~/.rpmmacros
%_topdir $HOME/ocelotgui/rp/rpmbuild
%_ocelotguidir $HOME/ocelotgui
EOF
mkdir -p rpmbuild/{BUILD,BUILDROOT,RPMS,SOURCES,SPECS,SRPMS}
cp ~/ocelotgui/ocelotgui-1.0.5.tar.gz ocelotgui-1.0.5.tar.gz
cp ocelotgui-1.0.5.tar.gz rpmbuild/SOURCES/ocelotgui-1.0.5.tar.gz
cp ~/ocelotgui/ocelotgui.spec ocelotgui.spec
rpmbuild -ba ocelotgui.spec

