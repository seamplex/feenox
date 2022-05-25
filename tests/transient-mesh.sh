#!/bin/sh
for i in . tests; do
  if [ -e ${i}/functions.sh ]; then
    . ${i}/functions.sh 
  fi
done
if [ -z "${functions_found}" ]; then
  echo "could not find functions.sh"
  exit 1;
fi

answerdiff transient-to-mesh.fee
exitifwrong $?

answerdiff transient-from-mesh-same-dt.fee
exitifwrong $?

answerdiff transient-from-mesh-different-dt.fee
exitifwrong $?
