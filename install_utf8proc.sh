#!/bin/sh

PROJECT=utf8proc
VERSION=1.3.1
DIRECTORY="$PROJECT-$VERSION"
TAR_NAME="v${VERSION}.tar.gz"
TAR_URL="https://github.com/JuliaLang/$PROJECT/archive/$TAR_NAME"

set -ex

cd /tmp/
curl -OL $TAR_URL
tar xzf $TAR_NAME
cd $DIRECTORY/
make
sudo make install

if [ `uname -s` != Darwin ] ; then
    # Rebuild the shared object cache - needed to load the library
    # at runtime <http://linux.die.net/man/8/ldconfig>
    sudo ldconfig
fi
