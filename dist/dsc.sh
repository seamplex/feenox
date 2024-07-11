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
cp ../README.Debian ../control ../copyright ../rules debian/
m4 -Dfeenox_version=${version} ../changelog > debian/changelog
rm debian/source/control debian/tests/control
debuild
