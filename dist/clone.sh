
if [ ! -e ${package} ]; then
 git clone .. ${package}
else 
 cd ${package}
 git pull
 cd ..
fi

version=$(echo ${package}version | m4 ${package}/version.m4 -)
