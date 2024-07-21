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
rm -f COPYING debian/feenox/usr/share/doc/feenox/COPYRIGHT.gz
debuild
# # we could have also used
# pdebuild

cd ..
lintian -v -i -I -E --pedantic --profile debian feenox_${version}-1.dsc 
lintian -v -i -I -E --pedantic --profile debian feenox_${version}-1.deb
lintian -v -i -I -E --pedantic --profile debian feenox_${version}-1_amd64.changes
lintian -v -i -I -E --pedantic --profile debian feenox_${version}-1_amd64.buildinfo


feenox_1.0.66-1_amd64.changes

# # check
# sudo pbuilder create --debootstrapopts --variant=buildd
# sudo pbuilder update
# sudo pbuilder build feenox_${version}-1.dsc
