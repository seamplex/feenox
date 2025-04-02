if INCLUDE_SLEPC
  include $(SLEPC_DIR)/lib/slepc/conf/slepc_variables
endif
if INCLUDE_PETSC
  include $(PETSC_DIR)/lib/petsc/conf/variables
endif

# include $(top_srcdir)/aminclude_static.am
AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = $(ACLOCAL_FLAGS)

bin_PROGRAMS = fee2ccx
fee2ccx_CFLAGS = -I../../src $(SLEPC_CC_INCLUDES) $(PETSC_CC_INCLUDES) $(CC_INCLUDES) $(DOWNLOADED_GSL_INCLUDES) $(all_includes)
fee2ccx_LDADD = $(SLEPC_LIB) $(PETSC_LIB) $(DOWNLOADED_GSL_LIBS) $(all_libraries)

fee2ccx_SOURCES = fee2ccx.c \
 ../../src/flow/init.c \
 ../../src/flow/cleanup.c \
 ../../src/flow/error.c \
 ../../src/flow/define.c \
 ../../src/flow/getptr.c \
 ../../src/flow/instruction.c \
 ../../src/flow/abort.c \
 ../../src/flow/conditional.c \
 ../../src/parser/parser.c \
 ../../src/parser/auxiliary.c \
 ../../src/math/alias.c \
 ../../src/math/expressions.c \
 ../../src/math/function.c \
 ../../src/math/vector.c \
 ../../src/math/matrix.c \
 ../../src/math/fit.c \
 ../../src/math/assignment.c \
 ../../src/math/solve.c \
 ../../src/io/file.c \
 ../../src/io/print.c \
 ../../src/io/printf.c \
 ../../src/version.c \
 ../../src/mesh/interpolate.c \
 ../../src/mesh/init.c \
 ../../src/mesh/calculix.c \
 ../../src/mesh/geometry.c \
 ../../src/mesh/mesh.c \
 ../../src/mesh/cell.c \
 ../../src/mesh/element.c \
 ../../src/mesh/neighbors.c \
 ../../src/mesh/gmsh.c \
 ../../src/mesh/write.c \
 ../../src/mesh/physical_group.c \
 ../../src/mesh/boundary_condition.c \
 ../../src/mesh/material.c \
 ../../src/mesh/extrema.c \
 ../../src/mesh/integrate.c \
 ../../src/mesh/elements/hexa20.c \
 ../../src/mesh/elements/hexa27.c \
 ../../src/mesh/elements/hexa8.c \
 ../../src/mesh/elements/line2.c \
 ../../src/mesh/elements/line3.c \
 ../../src/mesh/elements/point.c \
 ../../src/mesh/elements/prism15.c \
 ../../src/mesh/elements/prism6.c \
 ../../src/mesh/elements/quad4.c \
 ../../src/mesh/elements/quad8.c \
 ../../src/mesh/elements/quad9.c \
 ../../src/mesh/elements/tet10.c \
 ../../src/mesh/elements/tet4.c \
 ../../src/mesh/elements/triang3.c \
 ../../src/mesh/elements/triang6.c \
 ../../src/mesh/vtk.c \
 ../../src/mesh/vtu.c \
 ../../src/math/builtin_functions.c \
 ../../src/math/builtin_functionals.c \
 ../../src/math/builtin_vectorfunctions.c \
 ../../src/math/dae.c \
 ../../src/contrib/kdtree.c \
 ../../src/pdes/fem.c \
 ../../src/pdes/reaction.c \
 ../../src/pdes/dump.c \
 ../../src/pdes/init.c \
 ../../src/pdes/solve.c \
 ../../src/pdes/gradient.c \
 ../../src/pdes/dirichlet.c \
 ../../src/pdes/build.c \
 ../../src/pdes/blas.c \
 ../../src/pdes/petsc_ksp.c \
 ../../src/pdes/distribution.c \
 ../../src/pdes/mechanical/bc.c \
 ../../src/pdes/mechanical/bulk.c \
 ../../src/pdes/mechanical/elastic-isotropic.c \
 ../../src/pdes/mechanical/elastic-orthotropic.c \
 ../../src/pdes/mechanical/elastic-plane-strain.c \
 ../../src/pdes/mechanical/elastic-plane-stress.c \
 ../../src/pdes/mechanical/expansion-isotropic.c \
 ../../src/pdes/mechanical/expansion-orthotropic.c \
 ../../src/pdes/mechanical/init.c \
 ../../src/pdes/mechanical/linearize.c \
 ../../src/pdes/mechanical/parser.c \
 ../../src/pdes/mechanical/post.c \
 ../../src/pdes/mechanical/stress.c \
 ../../src/flow/step.c \
 ./parsepde-mechanical.c
