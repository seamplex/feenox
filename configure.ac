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
AC_SEARCH_LIBS([cos],[m],[],AC_MSG_ERROR([libm not found]))
AC_CHECK_HEADER([math.h],[],AC_MSG_ERROR([math.h not found]))

######################
# check for GSL (required)
AC_CHECK_HEADER([gsl/gsl_vector.h],    [], AC_MSG_ERROR[libgsl-dev not found])
AC_CHECK_LIB([gslcblas],[cblas_dgemm], [], AC_MSG_ERROR[libgsl-dev not found])
AC_CHECK_LIB([gsl],[gsl_blas_dgemm],   [], AC_MSG_ERROR[libgsl-dev not found])


######################
# check for Readline (optional)
# the default is read from the macro argument, but the help string
# does not expand variables so it always states that it is "check"
AC_ARG_WITH(
  [readline],
  AS_HELP_STRING([--with-readline], [support interactive debug mode @<:@default=check@:>@]),
  [],
  [with_readline=check]
)

have_readline="no"
AS_IF([test "x${with_readline}" != "xno"] , [
  AC_CHECK_HEADER([readline/readline.h],
    [],
    AS_IF([test "x${with_readline}" != "xcheck"], AC_MSG_FAILURE([--with-readline was given, but test for readline headers failed]))
  )
  AC_CHECK_LIB([readline], [readline],
    [],
    AS_IF([test "x${with_readline}" != "xcheck"],
      AC_MSG_FAILURE([--with-readline was given, but test for readline libray failed])
    )
  )  
])

# check if we have everything
AS_IF([test "x${ac_cv_lib_readline_readline}" = "xyes" -a \
            "x${ac_cv_header_readline_readline_h}" = "xyes" ],
  [
    AC_DEFINE(HAVE_READLINE)
    have_readline="yes"
  ],
)



######################
# check for SUNDIALS IDA (optional)
# the default is read from the macro argument, but the help string
# does not expand variables so it always states that it is "check"
AC_ARG_WITH(
  [ida],
  AS_HELP_STRING([--with-ida], [use SUNDIALS IDA to solve DAEs @<:@default=check@:>@]),
  [],
  [with_ida=check]
)

 
have_ida="no"
AS_IF([test "x${with_ida}" != "xno"] , [
  AC_CHECK_HEADERS([sundials/sundials_types.h ida/ida.h sundials/sundials_linearsolver.h],
    [],
    AS_IF([test "x${with_ida}" != "xcheck"], AC_MSG_FAILURE([--with-ida was given, but test for libsundials-dev headers failed]))
  )
  AC_CHECK_LIB([sundials_ida], [IDAInit],
    [],
    AS_IF([test "x${with_ida}" != "xcheck"],
      AC_MSG_FAILURE([--with-ida was given, but test for libsundials-dev libray failed])
    )
  )  
  AC_CHECK_HEADER([nvector/nvector_serial.h],
    [],
    AS_IF([test "x${with_ida}" != "xcheck"],
      AC_MSG_FAILURE([--with-ida was given, but test for libsundials-dev libray failed])
    )
  )
  AC_CHECK_LIB([sundials_nvecserial], [N_VNew_Serial],
    [],
    AS_IF([test "x${with_ida}" != "xcheck"],
      AC_MSG_FAILURE([--with-ida was given, but test for libsundials-dev libray failed])
    )  
  )
])

# check if we have everything
AS_IF([test "x${ac_cv_lib_sundials_ida_IDAInit}" = "xyes" -a \
            "x${ac_cv_header_sundials_sundials_types_h}" = "xyes" -a  \
            "x${ac_cv_lib_sundials_nvecserial_N_VNew_Serial}" = "xyes" -a \
            "x${ac_cv_header_nvector_nvector_serial_h}" = "xyes"],
  [
    AC_DEFINE(HAVE_IDA)
    have_ida="yes"
  ]
)
  
  


######################
# check for PETSc (optional)
AC_ARG_WITH([petsc],
  [AS_HELP_STRING([--with-petsc],
    [use PETSc to solve PDEs @<:@default=check@:>@])],
  [],
  [with_petsc=check]
)

have_petsc="no"
AS_IF([test "x${with_petsc}" != "xno"], [
  AC_ARG_VAR(PETSC_DIR, [location of PETSc installation])
  AC_ARG_VAR(PETSC_ARCH, [PETSc architecture])

  # asume we found it
  petsc_found=1

  AC_MSG_CHECKING([for PETSc dir])
  AS_IF([test -z "$PETSC_DIR"], [
    AC_MSG_RESULT([empty, trying /usr/lib/petsc])
    export PETSC_DIR=/usr/lib/petsc
  ])
  AS_IF([test ! -d "$PETSC_DIR"], [
    AC_MSG_RESULT([no])
    petsc_found=0
  ], [test ! -d "$PETSC_DIR/include"], [
    AC_MSG_RESULT([broken])
    petsc_found=0
  ], [test ! -f "$PETSC_DIR/include/petscversion.h"], [
    AC_MSG_RESULT([broken])
    petsc_found=0
  ],[
    AC_MSG_RESULT([$PETSC_DIR])
  ])

  AS_IF([test ${petsc_found} -eq 1], [
    AC_MSG_CHECKING([for PETSc arch])
    AS_IF([test ! -e "${PETSC_DIR}/${PETSC_ARCH}/lib/petsc/conf/petscvariables"], [
      AC_MSG_RESULT([broken])
      AC_MSG_ERROR([cannot find ${PETSC_DIR}/${PETSC_ARCH}/lib/petsc/conf/petscvariables.
Do you have a working PETSc > 3.6.0 installation?])
      petsc_found=0
    ],
      AC_MSG_RESULT([$PETSC_ARCH])
    ) 
  ])

  AS_IF([test ${petsc_found} -eq 1],
    [
      AC_DEFINE(HAVE_PETSC)
      have_petsc="yes"
    ]
  )  
])


######################
# check for SLEPc (optional)
AC_ARG_WITH([slepc],
  [AS_HELP_STRING([--with-slepc],
    [use PETSc to solve PDEs @<:@default=check@:>@])],
  [],
  [with_slepc=check]
)

have_slepc="no"
AS_IF([test "x${have_petsc}" != "xno" -a "x${with_slepc}" != "xno"], [
  AC_ARG_VAR(SLEPC_DIR, [location of SLEPc installation])

  # asume we found it
  slepc_found=1

  AC_MSG_CHECKING([for SLEPc dir])
  AS_IF([ test -z "$SLEPC_DIR" ], [
    AC_MSG_RESULT([empty, trying /usr/lib/slepc])
    export SLEPC_DIR=/usr/lib/slepc
  ])
  AS_IF([ test ! -d "$SLEPC_DIR" ], [
    AC_MSG_RESULT([no])
    slepc_found=0
  ], [test ! -d "$SLEPC_DIR/include"], [
    AC_MSG_RESULT([broken])
    slepc_found=0
  ], [test ! -f "$SLEPC_DIR/include/slepcversion.h"], [
    AC_MSG_RESULT([broken])
    slepc_found=0
  ], [test ! -f "$SLEPC_DIR/$PETSC_ARCH/lib/slepc/conf/slepcvariables"], [
    AC_MSG_RESULT([broken])
    slepc_found=0
  ],
    AC_MSG_RESULT([$SLEPC_DIR])
  )

  AS_IF([test ${slepc_found} -eq 1],
    [
      AC_DEFINE(HAVE_SLEPC)
      have_slepc="yes"
    ]
  )  
])


# if slepc is found we need to include its makefile,
# otherwise petsc and otherwise nothing
rm -f src/variables.mak
AS_IF([test "x${have_slepc}" = "xyes"],
  [echo "include \$(SLEPC_DIR)/lib/slepc/conf/slepc_variables" > src/variables.mak],
  AS_IF([test "x${have_petsc}" = "xyes"],
    [echo "include \$(PETSC_DIR)/lib/petsc/conf/variables" > src/variables.mak],
    [touch src/variables.mak]
  )
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
#define FEENOX_COMPILATION_DATE     "$(date +'%a %b %d %H:%M:%S %Y %z')"

#define FEENOX_COMPILATION_USERNAME "$(whoami | sed s/\\\\\\\\//)"
#define FEENOX_COMPILATION_HOSTNAME "$(hostname)"

#define FEENOX_COMPILER_ARCH     "${host_os} ${host_cpu}"
#define FEENOX_COMPILER_VERSION  "$($CC --version | head -n1)"
#define FEENOX_COMPILER_CFLAGS   "$CFLAGS"
EOF

  
  
AC_CONFIG_FILES([Makefile src/Makefile doc/Makefile]) 

AS_BOX([Summary of optional dependencies])
AS_ECHO(  ["  Readline       ${have_readline}"])
AS_ECHO(  ["  SUNDIALS IDA   ${have_ida}"])
AS_ECHO_N(["  PETSc          ${have_petsc}"])
AS_IF([test "x${have_petsc}" = "xyes"],
 AS_ECHO([" ${PETSC_DIR} ${PETSC_ARCH}"]),
 AS_ECHO
)
AS_ECHO_N(["  SLEPc          ${have_slepc}"])
AS_IF([test "x${have_slepc}" = "xyes"],
 AS_ECHO([" ${SLEPC_DIR}"]),
 AS_ECHO
)

AC_OUTPUT
