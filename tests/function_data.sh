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

answer function_data1dlinear.fee "2.5"
exitifwrong $?

answer function_data1dlinearchanged.fee "1"
exitifwrong $?

answer1 function_data1d.fee linear "2"
exitifwrong $?

answer1 function_data1d.fee polynomial "2.27984"
exitifwrong $?

answer1 function_data1d.fee spline "2.07672"
exitifwrong $?

answer1 function_data1d.fee spline_periodic "2.09259"
exitifwrong $?

answer1 function_data1d.fee akima "2.09877"
exitifwrong $?

answer1 function_data1d.fee akima_periodic "2.09877"
exitifwrong $?

answer1 function_data1d.fee steffen "2.07407"
exitifwrong $?
