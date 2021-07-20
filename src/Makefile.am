include variables.mak
undefine DESTDIR  # this variable is set by petsc somewhere and we need it empty to make install

# include $(top_srcdir)/aminclude_static.am
AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = $(ACLOCAL_FLAGS)

bin_PROGRAMS = feenox

feenox_CFLAGS = $(SLEPC_CC_INCLUDES) $(PETSC_CC_INCLUDES) $(CC_INCLUDES) $(DOWNLOADED_GSL_INCLUDES) $(all_includes)
feenox_LDADD = $(SLEPC_LIB) $(PETSC_LIB) $(DOWNLOADED_GSL_LIBS) $(all_libraries)
#feenox_LIBS = $(CODE_COVERAGE_LIBS)
# feenox_LDFLAGS = -rdynamic

feenox_SOURCES = \
./feenox.c \
./feenox.h \
./version.c \
./version.h \
./flow/init.c \
./flow/abort.c \
./flow/conditional.c \
./flow/error.c \
./flow/define.c \
./flow/getptr.c \
./flow/cleanup.c \
./flow/instruction.c \
./flow/step.c \
./flow/run_standard.c \
./parser/parser.c \
./parser/parser.h \
./parser/auxiliary.c \
./io/file.c \
./io/print.c \
./math/alias.c \
./math/assignment.c \
./math/builtin_functions.c \
./math/builtin_functionals.c \
./math/builtin_vectorfunctions.c \
./math/dae.c \
./math/expressions.c \
./math/fit.c \
./math/matrix.c \
./math/function.c \
./math/vector.c \
./mesh/init.c \
./mesh/geometry.c \
./mesh/neighbors.c \
./mesh/boundary_condition.c \
./mesh/physical_group.c \
./mesh/interpolate.c \
./mesh/material.c \
./mesh/element.c \
./mesh/element.h \
./mesh/cell.c \
./mesh/fem.c \
./mesh/write.c \
./mesh/integrate.c \
./mesh/extrema.c \
./mesh/line2.c \
./mesh/line3.c \
./mesh/triang3.c \
./mesh/triang6.c \
./mesh/quad4.c \
./mesh/quad8.c \
./mesh/quad9.c \
./mesh/tet4.c \
./mesh/tet10.c \
./mesh/hexa8.c \
./mesh/hexa20.c \
./mesh/hexa27.c \
./mesh/prism6.c \
./mesh/prism15.c \
./mesh/point.c \
./mesh/mesh.c \
./mesh/gmsh.c \
./mesh/calculix.c \
./mesh/vtk.c \
./problems/build.c \
./problems/dirichlet.c \
./problems/distribution.c \
./problems/dump.c \
./problems/init.c \
./problems/neumann.c \
./problems/petsc_ksp.c \
./problems/petsc_snes.c \
./problems/petsc_ts.c \
./problems/slepc_eps.c \
./problems/solve.c \
./problems/thermal/thermal.h \
./problems/thermal/methods.h \
./problems/thermal/init.c \
./problems/thermal/bulk.c \
./problems/thermal/bc.c \
./problems/thermal/post.c \
./problems/mechanical/mechanical.h \
./problems/mechanical/methods.h \
./problems/mechanical/init.c \
./problems/mechanical/bulk.c \
./problems/mechanical/bc.c \
./problems/mechanical/post.c \
./problems/modal/modal.h \
./problems/modal/methods.h \
./problems/modal/init.c \
./problems/modal/bulk.c \
./problems/modal/bc.c \
./problems/modal/post.c \
./contrib/kdtree.c \
./contrib/kdtree.h \
./contrib/utlist.h \
./contrib/uthash.h

