include variables.mak
undefine DESTDIR  # this variable is set by petsc somewhere and we need it empty to make install

AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = $(ACLOCAL_FLAGS)

bin_PROGRAMS = feenox

feenox_CFLAGS = $(SLEPC_CC_INCLUDES) $(PETSC_CC_INCLUDES) $(CC_INCLUDES) $(all_includes)
feenox_LDADD = $(SLEPC_LIB) $(PETSC_LIB) $(all_libraries)
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
./math/matrix.c \
./math/function.c \
./math/vector.c \
./mesh/init.c \
./mesh/geometry.c \
./mesh/neighbors.c \
./mesh/boundary_condition.c \
./mesh/physical_group.c \
./mesh/material.c \
./mesh/element.c \
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
./contrib/kdtree.c \
./contrib/kdtree.h \
./contrib/utlist.h \
./contrib/uthash.h

