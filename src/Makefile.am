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
./contrib/kdtree.c \
./contrib/kdtree.h \
./contrib/utlist.h \
./contrib/uthash.h \
./flow/init.c \
./flow/abort.c \
./flow/conditional.c \
./flow/error.c \
./flow/define.c \
./flow/getptr.c \
./flow/cleanup.c \
./flow/instruction.c \
./flow/step.c \
./flow/run.c \
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
./pdes/build.c \
./pdes/dirichlet.c \
./pdes/distribution.c \
./pdes/dump.c \
./pdes/init.c \
./pdes/gradient.c \
./pdes/neumann.c \
./pdes/petsc_ksp.c \
./pdes/petsc_snes.c \
./pdes/petsc_ts.c \
./pdes/slepc_eps.c \
./pdes/solve.c \
./pdes/laplace/laplace.h \
./pdes/laplace/methods.h \
./pdes/laplace/init.c \
./pdes/laplace/bulk.c \
./pdes/laplace/bc.c \
./pdes/thermal/thermal.h \
./pdes/thermal/methods.h \
./pdes/thermal/init.c \
./pdes/thermal/bulk.c \
./pdes/thermal/bc.c \
./pdes/thermal/heatflux.c \
./pdes/thermal/post.c \
./pdes/mechanical/mechanical.h \
./pdes/mechanical/methods.h \
./pdes/mechanical/init.c \
./pdes/mechanical/bulk.c \
./pdes/mechanical/bc.c \
./pdes/mechanical/stress.c \
./pdes/mechanical/post.c \
./pdes/modal/modal.h \
./pdes/modal/methods.h \
./pdes/modal/init.c \
./pdes/modal/bulk.c \
./pdes/modal/bc.c \
./pdes/modal/post.c \
./pdes/neutron_diffusion/neutron_diffusion.h \
./pdes/neutron_diffusion/methods.h \
./pdes/neutron_diffusion/init.c \
./pdes/neutron_diffusion/bulk.c \
./pdes/neutron_diffusion/bc.c \
./pdes/neutron_diffusion/post.c \
./pdes/neutron_diffusion/parser.c

