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

answerdiff qrng.fee
exitifwrong $?

answerdiff qrng_others.fee
exitifwrong $?

answerdiff qrng2d_sobol.fee
exitifwrong $?

answerdiff qrng2d_rhalton.fee
exitifwrong $?

echo -n "qrng2d_sobol_offset.fee ... "
feenox qrng2d_sobol_offset.fee 0 4 > qrng2d_sobol_offset.last
feenox qrng2d_sobol_offset.fee 4 4 >> qrng2d_sobol_offset.last
diff qrng2d_sobol_offset.last qrng2d_sobol.ref
exitifwrong $?
echo "ok"
