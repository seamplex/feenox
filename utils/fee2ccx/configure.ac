m4_include([../../version.m4])
AC_INIT([fee2ccx],feenoxversion)

AM_INIT_AUTOMAKE([foreign])
AC_PROG_CC

AC_CHECK_FILE([../../src/parser/parser.c],,AC_MSG_ERROR([dlfcn.h not found]))

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


AC_CONFIG_FILES([Makefile])
AC_OUTPUT
