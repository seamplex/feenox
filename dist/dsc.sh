#!/bin/bash -ex

. versions.sh
echo $version
# sudo apt-get -y install debmake devscripts debhelper dpkg-dev pbuilder autopkgtest

if [ -e src/feenox-${version}.tar.gz ]; then
  cp src/feenox-${version}.tar.gz .
else
  wget https://seamplex.com/feenox/dist/src/feenox-${version}.tar.gz
  cp feenox-${version}.tar.gz src
fi

rm -rf feenox-${version}
tar -xvzf feenox-${version}.tar.gz

cd feenox-${version}
debmake
cp ../README.Debian ../control ../copyright ../rules ../watch debian/
cp ../metadata debian/upstream
m4 -Dfeenox_version=${version} ../changelog > debian/changelog
rm -f debian/source/control \
      debian/tests/control \
      debian/patches/series \
      debian/source/local-options \
      debian/source/options \
      debian/source/patch-header \
      debian/*.ex \
      debian/source/*.ex \
      debian/clean \
      debian/dirs \
      debian/gbp.conf \
      debian/install \
      debian/links \
      debian/README.source
rmdir debian/patches debian/tests
rm -f COPYING debian/feenox/usr/share/doc/feenox/COPYRIGHT.gz

# copy the debian directory to dist
rm -rf ../debian
cp -r debian ..
# exit

export DEBEMAIL="jeremy@seamplex.com"
export DEBFULLNAME="Jeremy Theler"
export DEBUILD_DPKG_BUILDPACKAGE_OPTS="-i -I -us -uc"
export DEBUILD_LINTIAN_OPTS="-i -I --show-overrides"
# export DEBSIGN_KEYID="Your_GPG_keyID
debuild
# # we could have also used
# pdebuild

cd ..
rm -f lintian.log
lintian -v -i -I -E --pedantic --profile debian feenox_${version}-1.dsc | tee -a lintian.log
lintian -v -i -I -E --pedantic --profile debian feenox_${version}-1.deb | tee -a lintian.log
lintian -v -i -I -E --pedantic --profile debian feenox_${version}-1_amd64.changes | tee -a lintian.log
lintian -v -i -I -E --pedantic --profile debian feenox_${version}-1_amd64.buildinfo | tee -a lintian.log


# # check
# # see https://wiki.ubuntu.com/PbuilderHowto
# sudo pbuilder create --debootstrapopts --variant=build
# sudo pbuilder update
# sudo pbuilder build feenox_${version}-1.dsc
