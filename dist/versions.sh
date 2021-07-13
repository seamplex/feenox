petsc_ver=3.15.2
slepc_ver=3.15.1

name=$(uname)
if [ "x${name}" == "xLinux" ]; then
  PETSC_ARCH=arch-linux-serial-static
  target=linux-amd64
elif [ "x${name}" == "xCYGWIN_NT-10.0" ]; then
  PETSC_ARCH=arch-cygwin-serial-static
  target=windows-amd64
else
  echo "unknown uname '${name}'"
  exit 1
fi

