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
./flow/abort.c \
./flow/init.c \
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
./math/algebra.c \
./math/dae.c \
./math/matrix.c \
./math/function.c \
./math/vector.c \
./contrib/kdtree.c \
./contrib/kdtree.h \
./contrib/utlist.h \
./contrib/uthash.h

