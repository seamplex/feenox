#!/bin/sh
for i in . tests; do
  if [ -e ${i}/functions.sh ]; then
    . ${i}/functions.sh 
  fi
done
if [ -z "${functions_found}" ]; then
  echo "could not find functions.sh"
   exit 1
fi

answer arguments_self.fee "arguments_self"
exitifwrong $?

answer2 arguments_quoted.fee hello world! "hello hello world!"
exitifwrong $?

answer1 arguments_bracketed.fee 2 "121212"
exitifwrong $?

answer default_argument_value.fee "hello world!"
exitifwrong $?

answer1 default_argument_value.fee good_morning "good_morning world!"
exitifwrong $?

answer2 default_argument_value.fee hola mundo! "hola mundo!"
exitifwrong $?

