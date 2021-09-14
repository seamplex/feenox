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
AC_PROG_INSTALL
 
# TODO:investigate
#AX_CODE_COVERAGE

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

# check for sysconf() for page size
AC_CHECK_FUNCS([sysconf],[],[])

# check for clock_gettime() for unix time
AC_CHECK_FUNCS([clock_gettime],[],[])

# check for getrusage() for memory and cpu time
AC_CHECK_FUNCS([getrusage],[],[])

# check for asprintf
AC_CHECK_FUNCS([asprintf],[],[])



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
    #AS_IF([test "x${with_readline}" != "xcheck"], AC_MSG_FAILURE([--with-readline was given, but test for readline headers failed]))
  #)
  #AC_CHECK_LIB([readline], [readline],
    #[],
    #AS_IF([test "x${with_readline}" != "xcheck"],
      #AC_MSG_FAILURE([--with-readline was given, but test for readline libray failed])
    #)
  #)  
#])

# check if we have everything
AS_IF([test "x${ac_cv_lib_readline_readline}" = "xyes" -a \
            "x${ac_cv_header_readline_readline_h}" = "xyes" ],
  [
    AC_DEFINE(HAVE_READLINE)
    have_readline="yes"
  ],
)



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
  AC_CHECK_HEADERS([sundials/sundials_types.h ida/ida.h sundials/sundials_linearsolver.h],
    [],
    AS_IF([test "x${with_sundials}" != "xcheck"], AC_MSG_FAILURE([--with-sundials was given, but test for libsundials-dev headers failed]))
  )
  AC_CHECK_LIB([sundials_ida], [IDAInit],
    [],
    AS_IF([test "x${with_sundials}" != "xcheck"],
      AC_MSG_FAILURE([--with-sundials was given, but test for libsundials-dev libray failed])
    )
  )  
  AC_CHECK_HEADER([nvector/nvector_serial.h],
    [],
    AS_IF([test "x${with_sundials}" != "xcheck"],
      AC_MSG_FAILURE([--with-sundials was given, but test for libsundials-dev libray failed])
    )
  )
  AC_CHECK_LIB([sundials_nvecserial], [N_VNew_Serial],
    [],
    AS_IF([test "x${with_sundials}" != "xcheck"],
      AC_MSG_FAILURE([--with-sundials was given, but test for libsundials-dev libray failed])
    )  
  )
])

# check if we have everything
AS_IF([test "x${ac_cv_lib_sundials_ida_IDAInit}" = "xyes" -a \
            "x${ac_cv_header_sundials_sundials_types_h}" = "xyes" -a  \
            "x${ac_cv_lib_sundials_nvecserial_N_VNew_Serial}" = "xyes" -a \
            "x${ac_cv_header_nvector_nvector_serial_h}" = "xyes"],
  [
    AC_DEFINE(HAVE_SUNDIALS)
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

  # asume we found it
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
Do you have a working PETSc > 3.6.0 installation?])
      petsc_found=0
    ],
      AC_MSG_RESULT([${PETSC_ARCH}])
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
  AS_IF([ test -z "${SLEPC_DIR}" ], [
    AC_MSG_CHECKING([empty, trying /usr/lib/slepc])
    export SLEPC_DIR=/usr/lib/slepc
  ])
  AS_IF([ test ! -d "${SLEPC_DIR}" ], [
    AC_MSG_RESULT([no])
    slepc_found=0
  ], [test ! -d "${SLEPC_DIR}/include"], [
    AC_MSG_RESULT([broken1])
    slepc_found=0
  ], [test ! -f "${SLEPC_DIR}/include/slepcversion.h"], [
    AC_MSG_RESULT([broken2])
    slepc_found=0
  ], [test ! -f "${SLEPC_DIR}/${PETSC_ARCH}/lib/slepc/conf/slepcvariables"], [
    AC_MSG_RESULT(${SLEPC_DIR}/${PETSC_ARCH}/lib/slepc/conf/slepcvariables)
    AC_MSG_RESULT([broken3])
    slepc_found=0
  ],
    AC_MSG_RESULT([${SLEPC_DIR}])
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
# see if we have --enable-download-gsl
gslver=2.7
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




######################
# create version.h
AC_MSG_NOTICE([creating ${srcdir}/src/version.h])
rm -f ${srcdir}/src/version.h

feenox_version=feenoxversion
cat << EOF >> ${srcdir}/src/version.h
#define FEENOX_VERSION       "${feenox_version}"

EOF

AS_IF([test -e .git -a ! -z "$(which git)"],[
  cat << EOF >> ${srcdir}/src/version.h
#define FEENOX_GIT_VERSION   "$(git describe --tags | sed 's/-/./')"
#define FEENOX_GIT_BRANCH    "$(git symbolic-ref HEAD | sed -e 's,.*/\(.*\),\1,')"
#define FEENOX_GIT_DATE      "$(git log --pretty=format:"%ad" | head -n1)"
#define FEENOX_GIT_CLEAN     $(git status --porcelain | wc -l)

EOF
])

# give some information about the compiler in the summary
compiler_command="${CC}"
compiler_show="${compiler_command}"
AS_IF([test "x${have_petsc}" = "xyes"],[
  AS_IF([test ! -z "$(which mpicc)"],[
    compiler_command="mpicc"
    compiler_show="$(${compiler_command} -show)"
  ])
])


cat << EOF >> ${srcdir}/src/version.h
#define FEENOX_COMPILATION_DATE     "$(date +'%a %b %d %H:%M:%S %Y %z')"

#define FEENOX_COMPILATION_USERNAME "$(whoami | sed s/\\\\\\\\//)"
#define FEENOX_COMPILATION_HOSTNAME "$(hostname)"

#define FEENOX_COMPILER_COMMAND   "${compiler_command}"
#define FEENOX_COMPILER_ARCH     "${host_os} ${host_cpu}"
#define FEENOX_COMPILER_VERSION  "$(${compiler_command} --version | head -n1)"
#define FEENOX_COMPILER_CFLAGS   "${CFLAGS}"
#define FEENOX_COMPILER_SHOW     "${compiler_show}"
EOF


######################
# create some links to pass make distcheck
AC_CONFIG_LINKS([tests/abort.sh:tests/abort.sh])
AC_CONFIG_LINKS([tests/algebraic_expr.sh:tests/algebraic_expr.sh])
AC_CONFIG_LINKS([tests/builtin.sh:tests/builtin.sh])
AC_CONFIG_LINKS([tests/clean.sh:tests/clean.sh])
AC_CONFIG_LINKS([tests/default_argument_value.sh:tests/default_argument_value.sh])
AC_CONFIG_LINKS([tests/exp.sh:tests/exp.sh])
AC_CONFIG_LINKS([tests/expressions_constants.sh:tests/expressions_constants.sh])
AC_CONFIG_LINKS([tests/expressions_functions.sh:tests/expressions_functions.sh])
AC_CONFIG_LINKS([tests/expressions_variables.sh:tests/expressions_variables.sh])
AC_CONFIG_LINKS([tests/fit.sh:tests/fit.sh])
AC_CONFIG_LINKS([tests/function_algebraic.sh:tests/function_algebraic.sh])
AC_CONFIG_LINKS([tests/function_data.sh:tests/function_data.sh])
AC_CONFIG_LINKS([tests/function_file.sh:tests/function_file.sh])
AC_CONFIG_LINKS([tests/function_vectors.sh:tests/function_vectors.sh])
AC_CONFIG_LINKS([tests/functions.sh:tests/functions.sh])
AC_CONFIG_LINKS([tests/iaea-pwr.sh:tests/iaea-pwr.sh])
AC_CONFIG_LINKS([tests/integral.sh:tests/integral.sh])
AC_CONFIG_LINKS([tests/iterative.sh:tests/iterative.sh])
AC_CONFIG_LINKS([tests/laplace2d.sh:tests/laplace2d.sh])
AC_CONFIG_LINKS([tests/materials.sh:tests/materials.sh])
AC_CONFIG_LINKS([tests/mesh.sh:tests/mesh.sh])
AC_CONFIG_LINKS([tests/modal.sh:tests/modal.sh])
AC_CONFIG_LINKS([tests/nafems-lex.sh:tests/nafems-lex.sh])
AC_CONFIG_LINKS([tests/nafems-t4.sh:tests/nafems-t4.sh])
AC_CONFIG_LINKS([tests/nafems-tx.sh:tests/nafems-tx.sh])
AC_CONFIG_LINKS([tests/neutron_diffusion_keff.sh:tests/neutron_diffusion_keff.sh])
AC_CONFIG_LINKS([tests/neutron_diffusion_src.sh:tests/neutron_diffusion_src.sh])
AC_CONFIG_LINKS([tests/point-kinetics.sh:tests/point-kinetics.sh])
AC_CONFIG_LINKS([tests/thermal-1d.sh:tests/thermal-1d.sh])
AC_CONFIG_LINKS([tests/thermal-2d.sh:tests/thermal-2d.sh])
AC_CONFIG_LINKS([tests/trig.sh:tests/trig.sh])
AC_CONFIG_LINKS([tests/vector.sh:tests/vector.sh])
AC_CONFIG_LINKS([tests/2dpwr.fee:tests/2dpwr.fee])
AC_CONFIG_LINKS([tests/abort.fee:tests/abort.fee])
AC_CONFIG_LINKS([tests/algebraic_expr.fee:tests/algebraic_expr.fee])
AC_CONFIG_LINKS([tests/alias.fee:tests/alias.fee])
AC_CONFIG_LINKS([tests/assignment_scalar.fee:tests/assignment_scalar.fee])
AC_CONFIG_LINKS([tests/atan2.fee:tests/atan2.fee])
AC_CONFIG_LINKS([tests/beam-cantilever-modal-free-free.fee:tests/beam-cantilever-modal-free-free.fee])
AC_CONFIG_LINKS([tests/beam-cantilever-modal.fee:tests/beam-cantilever-modal.fee])
AC_CONFIG_LINKS([tests/cmp-float.fee:tests/cmp-float.fee])
AC_CONFIG_LINKS([tests/cmp-zero.fee:tests/cmp-zero.fee])
AC_CONFIG_LINKS([tests/default_argument_value.fee:tests/default_argument_value.fee])
AC_CONFIG_LINKS([tests/exp.fee:tests/exp.fee])
AC_CONFIG_LINKS([tests/expr.fee:tests/expr.fee])
AC_CONFIG_LINKS([tests/expressions_functions.fee:tests/expressions_functions.fee])
AC_CONFIG_LINKS([tests/expressions_variables.fee:tests/expressions_variables.fee])
AC_CONFIG_LINKS([tests/fibo_formula.fee:tests/fibo_formula.fee])
AC_CONFIG_LINKS([tests/fibo_iterative.fee:tests/fibo_iterative.fee])
AC_CONFIG_LINKS([tests/fibo_vector.fee:tests/fibo_vector.fee])
AC_CONFIG_LINKS([tests/file.fee:tests/file.fee])
AC_CONFIG_LINKS([tests/find_extrema2d.fee:tests/find_extrema2d.fee])
AC_CONFIG_LINKS([tests/fit1d.fee:tests/fit1d.fee])
AC_CONFIG_LINKS([tests/fit1d_gradient.fee:tests/fit1d_gradient.fee])
AC_CONFIG_LINKS([tests/fit1d_sigma.fee:tests/fit1d_sigma.fee])
AC_CONFIG_LINKS([tests/fit2d.fee:tests/fit2d.fee])
AC_CONFIG_LINKS([tests/function_algebraic.fee:tests/function_algebraic.fee])
AC_CONFIG_LINKS([tests/function_data1d.fee:tests/function_data1d.fee])
AC_CONFIG_LINKS([tests/function_data1dlinear.fee:tests/function_data1dlinear.fee])
AC_CONFIG_LINKS([tests/function_data1dlinearchanged.fee:tests/function_data1dlinearchanged.fee])
AC_CONFIG_LINKS([tests/function_data2d.fee:tests/function_data2d.fee])
AC_CONFIG_LINKS([tests/function_file1d.fee:tests/function_file1d.fee])
AC_CONFIG_LINKS([tests/function_file1d_columns.fee:tests/function_file1d_columns.fee])
AC_CONFIG_LINKS([tests/function_vectors1d.fee:tests/function_vectors1d.fee])
AC_CONFIG_LINKS([tests/hello.fee:tests/hello.fee])
AC_CONFIG_LINKS([tests/hoc.fee:tests/hoc.fee])
AC_CONFIG_LINKS([tests/if.fee:tests/if.fee])
AC_CONFIG_LINKS([tests/implicit.fee:tests/implicit.fee])
AC_CONFIG_LINKS([tests/include.fee:tests/include.fee])
AC_CONFIG_LINKS([tests/initial_conditions.fee:tests/initial_conditions.fee])
AC_CONFIG_LINKS([tests/integrate2d.fee:tests/integrate2d.fee])
AC_CONFIG_LINKS([tests/inverse-dae.fee:tests/inverse-dae.fee])
AC_CONFIG_LINKS([tests/inverse-integral.fee:tests/inverse-integral.fee])
AC_CONFIG_LINKS([tests/lag.fee:tests/lag.fee])
AC_CONFIG_LINKS([tests/lag_compact.fee:tests/lag_compact.fee])
AC_CONFIG_LINKS([tests/laplace-square.fee:tests/laplace-square.fee])
AC_CONFIG_LINKS([tests/logphi.fee:tests/logphi.fee])
AC_CONFIG_LINKS([tests/materials.fee:tests/materials.fee])
AC_CONFIG_LINKS([tests/matrix.fee:tests/matrix.fee])
AC_CONFIG_LINKS([tests/mesh3d.fee:tests/mesh3d.fee])
AC_CONFIG_LINKS([tests/modal-beam-cantilever.fee:tests/modal-beam-cantilever.fee])
AC_CONFIG_LINKS([tests/modal_rectangular_beam.fee:tests/modal_rectangular_beam.fee])
AC_CONFIG_LINKS([tests/nafems-le10.fee:tests/nafems-le10.fee])
AC_CONFIG_LINKS([tests/nafems-t2-1d.fee:tests/nafems-t2-1d.fee])
AC_CONFIG_LINKS([tests/nafems-t2-3d.fee:tests/nafems-t2-3d.fee])
AC_CONFIG_LINKS([tests/nafems-t3-1d.fee:tests/nafems-t3-1d.fee])
AC_CONFIG_LINKS([tests/nafems-t3-3d.fee:tests/nafems-t3-3d.fee])
AC_CONFIG_LINKS([tests/nafems-t4.fee:tests/nafems-t4.fee])
AC_CONFIG_LINKS([tests/open.fee:tests/open.fee])
AC_CONFIG_LINKS([tests/parameters.fee:tests/parameters.fee])
AC_CONFIG_LINKS([tests/peano.fee:tests/peano.fee])
AC_CONFIG_LINKS([tests/print.fee:tests/print.fee])
AC_CONFIG_LINKS([tests/print_function.fee:tests/print_function.fee])
AC_CONFIG_LINKS([tests/reactivity-from-table.fee:tests/reactivity-from-table.fee])
AC_CONFIG_LINKS([tests/read_mesh2d.fee:tests/read_mesh2d.fee])
AC_CONFIG_LINKS([tests/reflected-src.fee:tests/reflected-src.fee])
AC_CONFIG_LINKS([tests/reflected.fee:tests/reflected.fee])
AC_CONFIG_LINKS([tests/sort_vector.fee:tests/sort_vector.fee])
AC_CONFIG_LINKS([tests/sqrt.fee:tests/sqrt.fee])
AC_CONFIG_LINKS([tests/thermal-slab-convection-as-heat-nosource.fee:tests/thermal-slab-convection-as-heat-nosource.fee])
AC_CONFIG_LINKS([tests/thermal-slab-convection-nosource.fee:tests/thermal-slab-convection-nosource.fee])
AC_CONFIG_LINKS([tests/thermal-slab-heat-nosource.fee:tests/thermal-slab-heat-nosource.fee])
AC_CONFIG_LINKS([tests/thermal-slab-space-nosource.fee:tests/thermal-slab-space-nosource.fee])
AC_CONFIG_LINKS([tests/thermal-slab-temperature-nosource.fee:tests/thermal-slab-temperature-nosource.fee])
AC_CONFIG_LINKS([tests/thermal-slab-transient.fee:tests/thermal-slab-transient.fee])
AC_CONFIG_LINKS([tests/thermal-slab-uniform-nosource.fee:tests/thermal-slab-uniform-nosource.fee])
AC_CONFIG_LINKS([tests/thermal-slab-uniform-source.fee:tests/thermal-slab-uniform-source.fee])
AC_CONFIG_LINKS([tests/thermal-two-squares-material-explicit-space.fee:tests/thermal-two-squares-material-explicit-space.fee])
AC_CONFIG_LINKS([tests/thermal-two-squares-material-explicit-temperature.fee:tests/thermal-two-squares-material-explicit-temperature.fee])
AC_CONFIG_LINKS([tests/thermal-two-squares-material-explicit-uniform.fee:tests/thermal-two-squares-material-explicit-uniform.fee])
AC_CONFIG_LINKS([tests/thermal-two-squares-material-implicit-space.fee:tests/thermal-two-squares-material-implicit-space.fee])
AC_CONFIG_LINKS([tests/thermal-two-squares-material-implicit-temperature.fee:tests/thermal-two-squares-material-implicit-temperature.fee])
AC_CONFIG_LINKS([tests/thermal-two-squares-material-implicit-uniform.fee:tests/thermal-two-squares-material-implicit-uniform.fee])
AC_CONFIG_LINKS([tests/time_path.fee:tests/time_path.fee])
AC_CONFIG_LINKS([tests/ud20-1-0-sl-src-vacuum.fee:tests/ud20-1-0-sl-src-vacuum.fee])
AC_CONFIG_LINKS([tests/ud20-1-0-sl-src.fee:tests/ud20-1-0-sl-src.fee])
AC_CONFIG_LINKS([tests/ud20-1-0-sl-vacuum.fee:tests/ud20-1-0-sl-vacuum.fee])
AC_CONFIG_LINKS([tests/ud20-1-0-sl.fee:tests/ud20-1-0-sl.fee])
AC_CONFIG_LINKS([tests/var.fee:tests/var.fee])
AC_CONFIG_LINKS([tests/vector.fee:tests/vector.fee])
AC_CONFIG_LINKS([tests/write_mesh2d.fee:tests/write_mesh2d.fee])
AC_CONFIG_LINKS([tests/2dpwr-eighth.geo:tests/2dpwr-eighth.geo])
AC_CONFIG_LINKS([tests/2dpwr-quarter.geo:tests/2dpwr-quarter.geo])
AC_CONFIG_LINKS([tests/beam-cantilever-hex20.geo:tests/beam-cantilever-hex20.geo])
AC_CONFIG_LINKS([tests/beam-cantilever-hex27.geo:tests/beam-cantilever-hex27.geo])
AC_CONFIG_LINKS([tests/beam-cantilever-hex8.geo:tests/beam-cantilever-hex8.geo])
AC_CONFIG_LINKS([tests/beam-cantilever-tet10.geo:tests/beam-cantilever-tet10.geo])
AC_CONFIG_LINKS([tests/beam-cantilever-tet4.geo:tests/beam-cantilever-tet4.geo])
AC_CONFIG_LINKS([tests/beam-cantilever.geo:tests/beam-cantilever.geo])
AC_CONFIG_LINKS([tests/cylinder.geo:tests/cylinder.geo])
AC_CONFIG_LINKS([tests/nafems-le10.geo:tests/nafems-le10.geo])
AC_CONFIG_LINKS([tests/nafems-t4.geo:tests/nafems-t4.geo])
AC_CONFIG_LINKS([tests/rectangular_beam.geo:tests/rectangular_beam.geo])
AC_CONFIG_LINKS([tests/reflected.geo:tests/reflected.geo])
AC_CONFIG_LINKS([tests/slab-1d-0.1m.geo:tests/slab-1d-0.1m.geo])
AC_CONFIG_LINKS([tests/slab-3d-0.1m.geo:tests/slab-3d-0.1m.geo])
AC_CONFIG_LINKS([tests/slab.geo:tests/slab.geo])
AC_CONFIG_LINKS([tests/square-centered.geo:tests/square-centered.geo])
AC_CONFIG_LINKS([tests/square.geo:tests/square.geo])
AC_CONFIG_LINKS([tests/two-squares-quad.geo:tests/two-squares-quad.geo])
AC_CONFIG_LINKS([tests/two-squares.geo:tests/two-squares.geo])
AC_CONFIG_LINKS([tests/ud20-1-0-sl.geo:tests/ud20-1-0-sl.geo])
AC_CONFIG_LINKS([tests/beam-cantilever-hex20.msh:tests/beam-cantilever-hex20.msh])
AC_CONFIG_LINKS([tests/beam-cantilever-hex27.msh:tests/beam-cantilever-hex27.msh])
AC_CONFIG_LINKS([tests/beam-cantilever-hex8.msh:tests/beam-cantilever-hex8.msh])
AC_CONFIG_LINKS([tests/beam-cantilever-tet10.msh:tests/beam-cantilever-tet10.msh])
AC_CONFIG_LINKS([tests/beam-cantilever-tet4.msh:tests/beam-cantilever-tet4.msh])
AC_CONFIG_LINKS([tests/cylinder.msh:tests/cylinder.msh])
AC_CONFIG_LINKS([tests/mesh2d-bin.msh:tests/mesh2d-bin.msh])
AC_CONFIG_LINKS([tests/reflected.msh:tests/reflected.msh])
AC_CONFIG_LINKS([tests/slab-1d-0.1m.msh:tests/slab-1d-0.1m.msh])
AC_CONFIG_LINKS([tests/slab-3d-0.1m.msh:tests/slab-3d-0.1m.msh])
AC_CONFIG_LINKS([tests/slab.msh:tests/slab.msh])
AC_CONFIG_LINKS([tests/square.msh:tests/square.msh])
AC_CONFIG_LINKS([tests/square22.msh:tests/square22.msh])
AC_CONFIG_LINKS([tests/square40.msh:tests/square40.msh])
AC_CONFIG_LINKS([tests/square41.msh:tests/square41.msh])
AC_CONFIG_LINKS([tests/square41bin.msh:tests/square41bin.msh])
AC_CONFIG_LINKS([tests/two-squares-bin-gmsh.msh:tests/two-squares-bin-gmsh.msh])
AC_CONFIG_LINKS([tests/two-squares-bin-meshio.msh:tests/two-squares-bin-meshio.msh])
AC_CONFIG_LINKS([tests/two-squares.msh:tests/two-squares.msh])
AC_CONFIG_LINKS([tests/ud20-1-0-sl.msh:tests/ud20-1-0-sl.msh])
AC_CONFIG_LINKS([tests/data1d.dat:tests/data1d.dat])
AC_CONFIG_LINKS([tests/data1d_columns.dat:tests/data1d_columns.dat])
AC_CONFIG_LINKS([tests/data2d.dat:tests/data2d.dat])
AC_CONFIG_LINKS([tests/flux.dat:tests/flux.dat])
AC_CONFIG_LINKS([tests/algebraic_expr.ref:tests/algebraic_expr.ref])
AC_CONFIG_LINKS([tests/expressions_variables.ref:tests/expressions_variables.ref])
AC_CONFIG_LINKS([tests/fibo_formula.ref:tests/fibo_formula.ref])
AC_CONFIG_LINKS([tests/fibo_iterative.ref:tests/fibo_iterative.ref])
AC_CONFIG_LINKS([tests/fibo_vector.ref:tests/fibo_vector.ref])
AC_CONFIG_LINKS([tests/find_extrema2d.ref:tests/find_extrema2d.ref])
AC_CONFIG_LINKS([tests/fit1d_gradient.ref:tests/fit1d_gradient.ref])
AC_CONFIG_LINKS([tests/function_vectors1d.ref:tests/function_vectors1d.ref])
AC_CONFIG_LINKS([tests/nafems-le10.ref:tests/nafems-le10.ref])
AC_CONFIG_LINKS([tests/thermal-slab-uniform-source-octave.ref:tests/thermal-slab-uniform-source-octave.ref])



  
AC_CONFIG_FILES([Makefile src/Makefile doc/Makefile]) 

AS_BOX([Summary of dependencies])
AS_ECHO(  ["  GNU Scientific Library  ${gsl_version}"])
#AS_ECHO(  ["  Readline                ${have_readline}"])
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
AS_ECHO(  ["  Compiler                ${compiler_show}"])

AC_OUTPUT
