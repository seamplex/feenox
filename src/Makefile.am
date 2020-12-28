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
./flow/error.c \
./flow/cleanup.c \
./flow/instruction.c \
./flow/abort.c \
./parser/parser.c \
./parser/parser.h \
./parser/auxiliary.c \
./io/file.c \
./math/algebra.c \
./math/dae.c \
./contrib/kdtree.c \
./contrib/kdtree.h \
./contrib/utlist.h \
./contrib/uthash.h

