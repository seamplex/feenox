
if [ ! -e ${package} ]; then
 git clone .. ${package}
 cd ${package}
 ./autogen.sh
 cd ..
else 
 cd ${package}
 git pull
 ./autogen.sh
 cd ..
fi

version=$(echo ${package}version | m4 ${package}/version.m4 -)
