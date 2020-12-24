dnl Process this file with autoconf to produce a configure script.
dnl
dnl This file is free software; as a special exception the author gives
dnl unlimited permission to copy and/or distribute it, with or without
dnl modifications, as long as this notice is preserved.
dnl
dnl This program is distributed in the hope that it will be useful, but
dnl WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
dnl implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

m4_include([version.m4])
AC_INIT([feenox],feenoxversion,[jeremy@seamplex.com])

AC_CONFIG_SRCDIR([src/feenox.c])
AC_CANONICAL_HOST

AM_INIT_AUTOMAKE
AC_PROG_CC


######################
# optimization flags
AS_IF([test "$CFLAGS" = "-g -O2"], [CFLAGS="-O3"])

######################
# libraries
#AC_CHECK_LIB([rt],[shm_open])
#AC_CHECK_LIB([pthread],[pthread_create])

#AC_SEARCH_LIBS([dlopen],[dl dld],[])
#AC_CHECK_HEADER([dlfcn.h],[],AC_MSG_ERROR([dlfcn.h not found]))

# check for math
#AC_SEARCH_LIBS([cos],[m],[],AC_MSG_ERROR([libm not found]))
#AC_CHECK_HEADER([math.h],[],AC_MSG_ERROR([math.h not found]))

######################
# check for GSL (required)
AC_CHECK_HEADER([gsl/gsl_vector.h],[],AC_MSG_ERROR[libgsl-dev not found])
AC_CHECK_LIB([gslcblas],[cblas_dgemm],[],AC_MSG_ERROR[libgsl-dev not found])
AC_CHECK_LIB([gsl],[gsl_blas_dgemm],[],AC_MSG_ERROR[libgsl-dev not found])


######################
# check for SUNDIALS IDA (optional)
# the default is read from the macro argument, but the help string
# does not expand variables so it always states that it is "check"
ida_default=m4_default([$1],[check])
ida_version=0

AC_ARG_WITH([ida],
  [AS_HELP_STRING([--with-ida],
    [support differential-algebraic equations @<:@default=check@:>@])],
  [],
  [with_ida=${ida_default}])

AS_IF([test "x$with_ida" != xno],[
  AC_CHECK_HEADERS([sundials/sundials_types.h ida/ida.h],
  [
    AC_CHECK_HEADERS([sundials/sundials_linearsolver.h])
  ],
  [
    AS_IF([test "x$with_ida" != xcheck],
      [AC_MSG_FAILURE([--with-ida was given, but test for ida headers failed])],
      [AC_MSG_WARN([sundials ida headers not found])
    ])
  ])
  AC_CHECK_LIB([sundials_ida], [IDAInit],,
    [AS_IF([test "x$with_ida" != xcheck],
      [AC_MSG_FAILURE([--with-ida was given, but test for ida libray failed])],
      [AC_MSG_WARN([sundials ida library (libsundials-ida) not found])])
    ])
  AC_CHECK_HEADER([nvector/nvector_serial.h], [],
    [AS_IF([test "x$with_ida" != xcheck],
      [AC_MSG_FAILURE([--with-ida was given, but test for sundials nvecserial headers failed])],
      [AC_MSG_WARN([sundials ida headers not found])
    ])
  ])
  AC_CHECK_LIB([sundials_nvecserial], [N_VNew_Serial],,
    [AS_IF([test "x$with_ida" != xcheck],
      [AC_MSG_FAILURE([--with-ida was given, but test for sundials nvecserial libray failed])],
      [AC_MSG_WARN([sundials nvecserial library (libsundials-nvecserial) not found])
    ])
  ])
])

# check if we have everything
AS_IF([test "x$ac_cv_lib_sundials_ida_IDAInit" = xyes -a "x$ac_cv_header_sundials_sundials_types_h" = xyes -a "x$ac_cv_lib_sundials_nvecserial_N_VNew_Serial" = xyes -a "x$ac_cv_header_nvector_nvector_serial_h" = xyes ],
  [AC_DEFINE(HAVE_IDA)]
)
  


######################
# check for Readline (optional)
# the default is read from the macro argument, but the help string
# does not expand variables so it always states that it is "check"
readline_default=m4_default([$1],[check])

AC_ARG_WITH([readline],
  [AS_HELP_STRING([--with-readline],
    [support interactive debug mode @<:@default=check@:>@])],
  [],
  [with_readline=${readline_default}])

AS_IF([test "x$with_readline" != xno],[
   AC_CHECK_HEADER([readline/readline.h], [],
    [AS_IF([test "x$with_readline" != xcheck],
       [AC_MSG_FAILURE([--with-readline was given, but test for readline headers failed])],
       [AC_MSG_WARN([GNU readline headers (libreadline-dev) not found.])])
    ])
   AC_CHECK_LIB([readline], [readline],,
    [AS_IF([test "x$with_readline" != xcheck],
       [AC_MSG_FAILURE([--with-readline was given, but test for readline libray failed])],
       [AC_MSG_WARN([GNU readline library (libreadline) not found.])])
    ])
  ])

# check if we have everything
AS_IF([test "x$ac_cv_lib_readline_readline" = xyes -a "x$ac_cv_header_readline_readline_h" = xyes ],
  [AC_DEFINE(HAVE_READLINE)]
)


######################
# create version.h
AC_MSG_NOTICE([creating version.h])
rm -f src/version.h

feenox_version=feenoxversion
cat << EOF >> src/version.h
#define FEENOX_VERSION       "${feenox_version}"

EOF

AS_IF([test -e .git -a ! -z "$(which git)"],[
  cat << EOF >> src/version.h
#define FEENOX_GIT_VERSION   "$(git describe --tags | sed 's/-/./')"
#define FEENOX_GIT_BRANCH    "$(git symbolic-ref HEAD | sed -e 's,.*/\(.*\),\1,')"
#define FEENOX_GIT_DATE      "$(git log --pretty=format:"%ad" | head -n1)"
#define FEENOX_GIT_CLEAN     $(git status --porcelain | wc -l)

EOF
])

cat << EOF >> src/version.h
#define FEENOX_COMPILER_ARCH     "${host_os} ${host_cpu}"
#define FEENOX_COMPILER_VERSION  "$($CC --version | head -n1)"
#define FEENOX_COMPILER_CFLAGS   "$CFLAGS"
EOF

  
  
AC_CONFIG_FILES([Makefile src/Makefile doc/Makefile]) 

echo ${gsl_version}

AC_OUTPUT
