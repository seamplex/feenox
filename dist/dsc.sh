#!/bin/bash -ex

. versions.sh
echo $version
# sudo apt-get -y install debmake devscripts debhelper dpkg-dev pbuilder autopkgtest

if [ -e src/feenox-${version}.tar.gz ]; then
  cp src/feenox-${version}.tar.gz .
else
  wget https://seamplex.com/feenox/dist/src/feenox-${version}.tar.gz
fi

rm -rf feenox-${version}
tar -xvzf feenox-${version}.tar.gz

cd feenox-${version}
debmake
cp ../README.Debian ../control ../copyright ../rules ../watch debian/
cp ../metadata debian/upstream
m4 -Dfeenox_version=${version} ../changelog > debian/changelog
rm -f debian/source/control debian/tests/control debian/patches/series debian/source/local-options debian/source/options debian/source/patch-header
rmdir debian/patches
rm COPYING
debuild

cd ..
lintian -v -i -I -E --pedantic --profile debian feenox_${version}-1.dsc 
