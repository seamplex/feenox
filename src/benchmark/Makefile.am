include ../variables.mak

bin_PROGRAMS = principal_stress
principal_stress_CXXFLAGS = $(SLEPC_CC_INCLUDES) $(PETSC_CC_INCLUDES) $(CC_INCLUDES) $(DOWNLOADED_GSL_INCLUDES) $(all_includes)
principal_stress_LDFLAGS = -isystem /home/gtheler/codigos/benchmark/include -L/home/gtheler/codigos/benchmark/build/src
principal_stress_SOURCES = principal_stress.cpp petsc.h ../feenox.h 
principal_stress_LDADD = ../libfeenox.a $(SLEPC_LIB) $(PETSC_LIB) $(DOWNLOADED_GSL_LIBS) $(all_libraries) -lbenchmark
