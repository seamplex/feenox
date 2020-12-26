include variables.mak
undefine DESTDIR  # this variable is set by petsc somewhere and we need it empty to make install

AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = $(ACLOCAL_FLAGS)

bin_PROGRAMS = feenox

feenox_CFLAGS = $(SLEPC_CC_INCLUDES) $(PETSC_CC_INCLUDES) $(CC_INCLUDES) $(all_includes)
feenox_LDADD = $(SLEPC_LIB) $(PETSC_LIB) $(all_libraries)
# fino_LDFLAGS = -rdynamic

feenox_SOURCES = \
./feenox.c \
./version.c \
./flow/init.c \
./flow/error.c \
./flow/cleanup.c \
./flow/abort.c \
./parser/parser.c \
./parser/auxiliary.c \
./io/file.c \
./math/algebra.c

