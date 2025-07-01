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
mv feenox-${version}.tar.gz feenox_${version}.orig.tar.gz

# if [ ! -d feenox-salsa ]; then
#   git clone https://salsa.debian.org/jtheler/feenox.git feenox-salsa
# else
#   cd feenox-salsa
#   git pull
#   cd ..
# fi

cd feenox-${version}
rm -rf debian
cp -r ../feenox-salsa/debian .

cat << EOF > tmp
feenox (${version}-1) unstable; urgency=medium

  * New upstream version.

 -- Jeremy Theler <jeremy@seamplex.com>  $(date -R)
 
EOF
cat debian/changelog >> tmp
mv tmp debian/changelog

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
