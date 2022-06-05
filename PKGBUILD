# Maintainer: Peter Gulutzan <pgulutzan at ocelot.ca>
# This file may be useful for creating an ocelotgui package for pacman on Arch Linux derivatives.
# Instructions: As tested with Manjaro-21 Live CD:
#               Get this file, PKGBUILD, from http://github.com/ocelot-inc/ocelotgui
#               cd to the directory containing this file
#               sudo pacman -S base-devel libmariadbclient cmake
#               makepkg
#               sudo pacman -U ./ocelotgui-1.6.0-1-x86_64.pkg.tar.zst
#               Now ocelotgui is ready to run. It might be necessary to run it twice.
# For Qt 6:     Change depends=('qt5-base' ...) to depends=('qt6-base' ...)
#               Add at end of cmake line: -DQT_VERSION=6
#               (Yes we could make two packages with the same PKGBUILD file,
#               ocelotgui-qt5 and ocelotgui-qt6, but that requires they both be present.)
# For MySQL:    Change depends=(... 'libmariadbclient') to depends=(... 'libmysqlclient')
#               (This is unnecessary since libmariadbclient has the same functionality,
#               but perhaps some people already have libmysqlclient installed.)
#               (We don't need libmariadbclient or equivalent when running unless we connect to a
#               MySQL or MariaDB server, but that would be normal since it is the most important purpose.)
#Re cleanup:    There is no advance cleanup so this might be a good idea if makepkg has been done before:
#               cd to directory containing PKGBUILD
#               rm -r -f ocelotgui (to remove what "git clone" produced)
#               rm -r -f build (to remove what "cmake" produced)
#Re cmake:      This is an out-of-source build and produces a RelWithDebInfo executable with an -O2 flag.
#               For other options read CMakeLists.txt.
pkgname=ocelotgui
pkgver=1.6.0
pkgrel=1
pkgdesc="GUI client for MySQL or MariaDB or Tarantool DBMS"
arch=("i686" "x86_64")
url="http://ocelot.ca"
license=('GPL2')
depends=('qt5-base' 'libmariadbclient')
makedepends=('cmake' 'git')
optdepends=('tarantool: for connecting to Tarantool server')
source=("git+https://github.com/ocelot-inc/ocelotgui.git")
md5sums=('SKIP')
build() {
  cd $pkgname
  cmake -B build -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_SKIP_RPATH=TRUE
  cmake --build build
}
package() {
  cd $pkgname/build
  make DESTDIR="$pkgdir/" install
}
