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
m4_include([ax_gcc_builtin.m4])
AC_INIT([feenox],feenoxversion,[jeremy@seamplex.com])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([${srcdir}/src/feenox.c])
AC_CANONICAL_HOST

AM_INIT_AUTOMAKE
AC_PROG_CC
AC_PROG_INSTALL

# TODO: investigate
#AX_CODE_COVERAGE

# Support both --with-debug and --enable-debug options
AC_ARG_WITH([debug],
  [AS_HELP_STRING([--with-debug], [Compile with debugging symbols (legacy option)])],
  [with_debug=$withval], [with_debug=no])

AC_ARG_ENABLE([debug],
  [AS_HELP_STRING([--enable-debug], [Compile with debugging symbols])],
  [enable_debug=$enableval], [enable_debug=no])

AC_ARG_ENABLE([fee2ccx],
  [AS_HELP_STRING([--enable-fee2ccx], [Compile the tool fee2ccx @<:@default=no@:>@])],
  [enable_fee2ccx=$enableval], [enable_fee2ccx=no])


######################
# default optimization flags
AS_IF([test "x$CFLAGS" = "x-g -O2"],
  AS_IF([test "x$with_debug" = "xyes" -o "x$enable_debug" = "xyes"] , [
    CFLAGS="-g -O0"
    AC_DEFINE([DEBUG], [1], [Define to 1 if debugging is enabled])
  ],[
    CFLAGS="-O3 -flto=auto -no-pie"
  ])
)



######################
# libraries

# check for math
AC_SEARCH_LIBS([cos],[m],[],AC_MSG_ERROR([libm not found]))
AC_CHECK_HEADER([math.h],[],AC_MSG_ERROR([math.h not found]))

# check for sysconf() for page size
AC_CHECK_FUNCS([sysconf],[],[])

# check for clock_gettime() for unix time
AC_CHECK_FUNCS([clock_gettime],[],[])

# check for getrusage() for memory and cpu time
AC_CHECK_FUNCS([getrusage],[],[])

# check for asprintf
AC_CHECK_FUNCS([asprintf],[],[])

# check for feenox_likely
AX_GCC_BUILTIN(__builtin_expect)


# some other stuff that should be pretty standard
# but we comment out to have a smaller compilation string

#AC_CHECK_FUNCS([floor memset pow rint sqrt strcasecmp strchr strdup strerror strncasecmp strpbrk strrchr strstr], [], [])
#AC_CHECK_HEADERS(fcntl.h limits.h malloc.h stddef.h, [], [])
#AC_CHECK_HEADER_STDBOOL
#AC_CHECK_TYPES([ptrdiff_t])
#AC_C_INLINE
#AC_FUNC_ALLOCA
#AC_FUNC_MALLOC
#AC_FUNC_REALLOC
#AC_TYPE_SIZE_T
#AC_TYPE_UINT16_T
#AC_TYPE_UINT32_T
#AC_TYPE_UINT8_T


######################
# check for Readline (optional)
# the default is read from the macro argument, but the help string
# does not expand variables so it always states that it is "check"

#AC_ARG_WITH(
  #[readline],
  #AS_HELP_STRING([--with-readline], [support interactive debug mode @<:@default=check@:>@]),
  #[],
  #[with_readline=check]
#)

#have_readline="no"
#AS_IF([test "x${with_readline}" != "xno"] , [
  #AC_CHECK_HEADER([readline/readline.h],
    #[],
    #AS_IF([test "x${with_readline}" != "xcheck"], AC_MSG_FAILURE([--with-readline was given but test for readline headers failed]))
  #)
  #AC_CHECK_LIB([readline], [readline],
    #[],
    #AS_IF([test "x${with_readline}" != "xcheck"],
      #AC_MSG_FAILURE([--with-readline was given but test for readline library failed])
    #)
  #)  
#])

# check if we have everything
#AS_IF([test "x${ac_cv_lib_readline_readline}" = "xyes" -a \
            #"x${ac_cv_header_readline_readline_h}" = "xyes" ],
  #[
    #AC_DEFINE(HAVE_READLINE)
    #have_readline="yes"
  #],
#)



######################
# check for SUNDIALS (optional)
# the default is read from the macro argument, but the help string
# does not expand variables so it always states that it is "check"
AC_ARG_WITH(
  [sundials],
  AS_HELP_STRING([--with-sundials], [use SUNDIALS to solve DAEs @<:@default=check@:>@]),
  [],
  [with_sundials=check]
)

 
have_sundials="no"
AS_IF([test "x${with_sundials}" != "xno"] , [
  CC_OLD=$CC
  CC=mpicc
  AC_CHECK_HEADERS([sundials/sundials_types.h ida/ida.h sundials/sundials_linearsolver.h],
    [],
    AS_IF([test "x${with_sundials}" != "xcheck"], AC_MSG_FAILURE([--with-sundials was given but test for libsundials-dev headers failed]))
  )
  AC_CHECK_LIB([sundials_ida], [IDAInit],
    [],
    AS_IF([test "x${with_sundials}" != "xcheck"],
      AC_MSG_FAILURE([--with-sundials was given but test for libsundials-dev library failed])
    )
  )  
  AC_CHECK_LIB([sundials_core], [SUNContext_Create],
    [],
    AS_IF([test "x${with_sundials}" != "xcheck"],
      AC_MSG_FAILURE([--with-sundials was given but test for libsundials-dev library failed])
    )
  )  
  AC_CHECK_HEADER([nvector/nvector_serial.h],
    [],
    AS_IF([test "x${with_sundials}" != "xcheck"],
      AC_MSG_FAILURE([--with-sundials was given but test for libsundials-dev library failed])
    )
  )
  AC_CHECK_LIB([sundials_nvecserial], [N_VNew_Serial],
    [],
    AS_IF([test "x${with_sundials}" != "xcheck"],
      AC_MSG_FAILURE([--with-sundials was given but test for libsundials-dev library failed])
    )  
  )
  CC=$CC_OLD
])

# check if we have everything
AS_IF([test "x${ac_cv_lib_sundials_ida_IDAInit}" = "xyes" -a \
            "x${ac_cv_header_sundials_sundials_types_h}" = "xyes" -a  \
            "x${ac_cv_lib_sundials_nvecserial_N_VNew_Serial}" = "xyes" -a \
            "x${ac_cv_header_nvector_nvector_serial_h}" = "xyes"],
  [
    AC_DEFINE([HAVE_SUNDIALS], [], [SUNDIALS is available])
    have_sundials="yes"
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

  # assume we found it
  petsc_found=1

  AC_MSG_CHECKING([for PETSc dir])
  AS_IF([test -z "${PETSC_DIR}"], [
    AC_MSG_CHECKING([empty, trying /usr/lib/petsc])
    export PETSC_DIR=/usr/lib/petsc
  ])
  AS_IF([test ! -d "${PETSC_DIR}"], [
    AC_MSG_RESULT([no])
    petsc_found=0
  ], [test ! -d "${PETSC_DIR}/include"], [
    AC_MSG_RESULT([broken])
    petsc_found=0
  ], [test ! -f "${PETSC_DIR}/include/petscversion.h"], [
    AC_MSG_RESULT([broken])
    petsc_found=0
  ],[
    AC_MSG_RESULT([${PETSC_DIR}])
  ])

  AS_IF([test ${petsc_found} -eq 1], [
    AC_MSG_CHECKING([for PETSc arch])
    AS_IF([test ! -e "${PETSC_DIR}/${PETSC_ARCH}/lib/petsc/conf/petscvariables"], [
      AC_MSG_RESULT([broken])
      AC_MSG_ERROR([cannot find ${PETSC_DIR}/${PETSC_ARCH}/lib/petsc/conf/petscvariables.
Do you have a working PETSc installation? See this page for help:
https://seamplex.com/feenox/doc/compilation.html])
      petsc_found=0
    ],
      AC_MSG_RESULT([${PETSC_ARCH}])
    ) 
  ])

  AS_IF([test ${petsc_found} -eq 1],
    [
      AC_DEFINE([HAVE_PETSC], [1], [PETSc is available])
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

  # assume we found it
  slepc_found=1

  AC_MSG_CHECKING([for SLEPc dir])
  
  AS_IF([test -z "${SLEPC_DIR}"], [
    # first try to see if PETSc was configured with --download-slepc
    export SLEPC_DIR=${PETSC_DIR}/${PETSC_ARCH}/externalpackages/git.slepc
    AS_IF([test ! -d ${SLEPC_DIR}],[
      # otherwise see if it is in the standard location as installed with apt
      # this works in Debian, not sure about other (it does not on Fedora)
      export SLEPC_DIR=/usr/lib/slepc
    ])
  AC_MSG_CHECKING([empty, trying ${SLEPC_DIR}])      
  ])
     
  AS_IF([ test -z "${SLEPC_DIR}" ], [
    AC_MSG_CHECKING([empty, trying /usr/lib/slepc])
    export SLEPC_DIR=/usr/lib/slepc
  ])
  AS_IF([ test ! -d "${SLEPC_DIR}" ], [
    AC_MSG_RESULT([no])
    slepc_found=0
  ], [test ! -d "${SLEPC_DIR}/include"], [
    AC_MSG_RESULT([broken no include])
    slepc_found=0
  ], [test ! -f "${SLEPC_DIR}/include/slepcversion.h"], [
    AC_MSG_RESULT([broken no version])
    slepc_found=0
  ], [test ! -f "${SLEPC_DIR}/${PETSC_ARCH}/lib/slepc/conf/slepcvariables"], [
    AC_MSG_RESULT([broken no variables])
    slepc_found=0
  ],
    AC_MSG_RESULT([${SLEPC_DIR}])
  )

  AS_IF([test ${slepc_found} -eq 1],
    [
      AC_DEFINE([HAVE_SLEPC], [1], [SLEPc is available])
      have_slepc="yes"
    ]
  )  
])


# if slepc is found we need to include its makefile,
# otherwise petsc and otherwise nothing
AM_CONDITIONAL([INCLUDE_PETSC], [test "x${have_petsc}" = "xyes" -a "x${have_slepc}" = "xno"])
AM_CONDITIONAL([INCLUDE_SLEPC], [test "x${have_slepc}" = "xyes"])


######################
# see if we have --enable-download-gsl
gslver=2.8
gsldist=gsl-${gslver}
gslmirror=http://ftpmirror.gnu.org/gsl/${gsldist}.tar.gz

AC_ARG_ENABLE([download-gsl],
 [AS_HELP_STRING([--enable-download-gsl], [try to automatically download and compile GSL @<:@default=no@:>@])],
 [download_gsl=yes],
 [download_gsl=no])

# if gsl directory does not exist, see if we have to uncompress and/or download
AS_IF([test ! -e ${gsldist}],[
 AS_IF([test ! -e ${gsldist}.tar.gz],[
  AS_IF([test "x$download_gsl" = "xyes"],[
   AS_IF([test "x$(which wget)" != "x"],[
    AC_MSG_NOTICE([downloading ${gslmirror}])
    wget -c ${gslmirror}
   ],[
    AC_MSG_ERROR([file ${gsldist}.tar.gz not found and wget not installed])
   ])
   AS_IF([test ! -e ${gsldist}.tar.gz],[
    AC_MSG_ERROR([file ${gsldist}.tar.gz could not be downloaded, copy it manually and re-try.])
   ])
  ])
 ])
 
 AS_IF([test -e ${gsldist}.tar.gz],[
  AC_MSG_NOTICE([uncompressing ${gsldist}.tar.gz])
  tar xzf ${gsldist}.tar.gz
 ])
])

# if gsl directory exists, see if we have to compile it
AS_IF([test -e ${gsldist}],[
 AS_IF([test -e ${gsldist}/.libs/libgsl.a],[
  AC_MSG_NOTICE([using already-compiled GSL library ${gsldist}/.libs/libgsl.a])
 ],[
  AC_MSG_NOTICE([configuring ${gsldist}])
  cd ${gsldist} 
  ./configure --prefix=${prefix} --host=${host}
  AC_MSG_NOTICE([compiling ${gsldist}])
  make
  cd ..
 ])
 
 AC_SUBST([DOWNLOADED_GSL_LIBS], ["../${gsldist}/.libs/libgsl.a ../${gsldist}/cblas/.libs/libgslcblas.a"])
 AC_SUBST([DOWNLOADED_GSL_INCLUDES], ["-I ../${gsldist} -I ../../${gsldist}"])
 gsl_version="${gslver} (downloaded and statically linked)"
],[ 
 # traditional test for GSL
 # check for GSL & CBLAS (required)
 AC_CHECK_HEADER([gsl/gsl_vector.h],    [], AC_MSG_ERROR([GNU Scientific library headers libgsl-dev not found.
Either install them with your package manager or configure with --enable-download-gsl]))

 # TODO: the original idea is that
 # if we found PETSc, we use whatever BLAS it has, otherwise we use GSL's CBLAS
 # but this does not work in Fedora since even though the library flexiblas
 # that is used by PETSc does contain cblas_dgemm, it is not found by the linker
 #AS_IF([test "x${have_petsc}" != "xyes"],
  AC_CHECK_LIB([gslcblas],[cblas_dgemm], [], AC_MSG_ERROR([GNU Scientific library CBLAS libgsl-dev not found]))
 #)
 AC_CHECK_LIB([gsl],[gsl_blas_dgemm],   [], AC_MSG_ERROR([GNU Scientific library libgsl-dev not found]))
 gsl_version="from system"
])



# give some information about the compiler in the summary
compiler_command="${CC}"
compiler_show="${compiler_command}"
AS_IF([test "x${have_petsc}" = "xyes"],[
  AS_IF([test ! -z "$(which mpicc)"],[
    compiler_command="mpicc"
    compiler_show="$(${compiler_command} -show | sed 's/"/\\"/g')"
  ])
])
compiler_version=$(${compiler_command} --version | head -n1 | tr -d \\n])

######################
# create version.h
AC_MSG_NOTICE([gathering versions])
AC_DEFINE([FEENOX_VERSION], "feenoxversion", [FeenoX version string])

# these guys create problems with reprotest
# AC_DEFINE([FEENOX_COMPILATION_DATE],      ["m4_esyscmd([date +'%a %b %d %H:%M:%S %Y %z' | tr -d \\n])"], [FeenoX compilation date])
# AC_DEFINE([FEENOX_COMPILATION_USERNAME],  ["m4_esyscmd([whoami | sed s/\\\\\\\\// | tr -d \\n])"], [FeenoX compilation username])
# AC_DEFINE([FEENOX_COMPILATION_HOSTNAME],  ["m4_esyscmd([hostname | tr -d \\n])"], [FeenoX compilation hostname])

m4_include(versiongit.m4)

AC_DEFINE_UNQUOTED([FEENOX_COMPILER_COMMAND], ["$compiler_command"],  [FeenoX compiler command])
AC_DEFINE_UNQUOTED([FEENOX_COMPILER_ARCH],    ["$host_os $host_cpu"], [FeenoX compiler architecture])
AC_DEFINE_UNQUOTED([FEENOX_COMPILER_VERSION], ["$compiler_version"],  [Feenox compiler version])
AC_DEFINE_UNQUOTED([FEENOX_COMPILER_SHOW],    ["${compiler_show}"],   [FeenoX actual compiler command])
AC_DEFINE_UNQUOTED([FEENOX_COMPILER_CFLAGS],  ["${CFLAGS}"],  [CFLAGS])
AC_DEFINE_UNQUOTED([FEENOX_COMPILER_LDFLAGS], ["${LDFLAGS}"], [LDFLAGS])


######################
# create some links to pass make distcheck
m4_include(auto_links.m4)

AC_CONFIG_FILES([Makefile src/Makefile])
# AC_CONFIG_FILES([src/benchmark/Makefile]) 
# AC_CONFIG_FILES([doc/Makefile])
AS_IF([test "x${enable_fee2ccx}" = "xyes"] , [
  AC_CONFIG_FILES([utils/fee2ccx/Makefile])
])


AS_BOX([Summary of dependencies])
AS_ECHO(  ["  GNU Scientific Library  ${gsl_version}"])
# AS_ECHO(  ["  Readline                ${have_readline}"])
AS_ECHO(  ["  SUNDIALS                ${have_sundials}"])
AS_ECHO_N(["  PETSc                   ${have_petsc}"])
AS_IF([test "x${have_petsc}" = "xyes"],
 AS_ECHO([" ${PETSC_DIR} ${PETSC_ARCH}"]),
 AS_ECHO
)
AS_ECHO_N(["  SLEPc                   ${have_slepc}"])
AS_IF([test "x${have_slepc}" = "xyes"],
 AS_ECHO([" ${SLEPC_DIR}"]),
 AS_ECHO
)
AS_ECHO(  ["  Compile fee2ccx         ${enable_fee2ccx}"])
AS_ECHO(  ["  Compiler                ${compiler_show}"])
AS_ECHO(  ["  Compiler flags          ${CFLAGS}"])
# AS_ECHO(  ["  Linker flags            ${LDFLAGS}"])

AC_OUTPUT
