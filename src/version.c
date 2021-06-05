/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX version and description routines
 *
 *  Copyright (C) 2009--2021 jeremy theler
 *
 *  This file is part of FeenoX.
 *
 *  FeenoX is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  FeenoX is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with FeenoX.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include "feenox.h"
extern feenox_t feenox;

#include "version.h"

// TODO: put in a hader and generate it from doc
#define FEENOX_ONE_LINER "a free no-fee no-X uniX-like finite-element(ish) computational engineering tool"
#define FEENOX_HELP "\
  -h, --help         display usage and commmand-line help and exit\n\
  -v, --version      display brief version information and exit\n\
  -V, --versions     display detailed version information\n\
  -s, --sumarize     list all symbols in the input file and exit\n\
\n\
Instructions will be read from standard input if “-” is passed as\n\
inputfile, i.e.\n\
\n\
    $ echo \"PRINT 2+2\" | feenox -\n\
    4\n\
"

#include <stdio.h>
#include <gsl/gsl_version.h>

#if HAVE_READLINE
#include <readline/readline.h>
#endif

#if HAVE_IDA
#include <sundials/sundials_version.h>
#endif

#if HAVE_PETSC
#include <petscsys.h>
#include <petscconfiginfo.h>
#endif
#if HAVE_SLEPC
#include <slepcsys.h>
#endif

void feenox_show_help(const char *progname) {
  // in parallel runs only print from first processor
  if (feenox.rank != 0) {
    return;
  }
  
  printf("usage: %s [options] inputfile [replacement arguments]\n\n", progname);
  
  printf("%s\n", FEENOX_HELP);
  
  printf("Report bugs at https://github.com/seamplex/feenox or to jeremy@seamplex.com\n");
  printf("Feenox home page: https://www.seamplex.com/feenox/\n");
  
  return;
}


void feenox_show_version(int version) {
  
  // in parallel runs only print from first processor
  if (feenox.rank != 0) {
    return;
  }
  
  feenox_shortversion();

  if (version) {
    printf("\n");
  }
     
  switch (version) {
    case version_copyright:
      feenox_copyright();
    break;
    case version_info:
      feenox_longversion();
    break;
  }
    

  fflush(stdout);

  return;
}

void feenox_shortversion(void) {

  // in parallel runs only print from first processor
  if (feenox.rank != 0) {
    return;
  }
  
  printf("FeenoX ");
#ifdef FEENOX_GIT_BRANCH
  printf("%s%s %s\n", FEENOX_GIT_VERSION,
                             (FEENOX_GIT_CLEAN==0)?"":"-dirty",
                             strcmp(FEENOX_GIT_BRANCH, "main")?FEENOX_GIT_BRANCH:"");
#else
  printf("%s\n", PACKAGE_VERSION);
#endif

  printf("%s\n", FEENOX_ONE_LINER);

  return;
}


void feenox_copyright(void) {

  // in parallel runs only print from first processor
  if (feenox.rank != 0) {
    return;
  }

  /* It is important to separate the year from the rest of the message,
     as done here, to avoid having to retranslate the message when a new
     year comes around.  */  
  printf("Copyright (C) %d--%d %s\n\
GNU General Public License v3+, https://www.gnu.org/licenses/gpl.html. \n\
FeenoX is free software: you are free to change and redistribute it.\n\
There is NO WARRANTY, to the extent permitted by law.\n", 2009, 2021, "jeremy theler"); 
}

void feenox_longversion(void) {
  
  // in parallel runs only print from first processor
  if (feenox.rank != 0) {
    return;
  }

#ifdef FEENOX_GIT_DATE
  printf("Last commit date   : %s\n", FEENOX_GIT_DATE);
#endif
#ifdef FEENOX_COMPILATION_DATE
  printf("Build date         : %s\n", FEENOX_COMPILATION_DATE);
#endif
#ifdef FEENOX_COMPILER_ARCH
  printf("Build architecture : %s\n", FEENOX_COMPILER_ARCH);
#endif
#ifdef FEENOX_COMPILER_VERSION
  printf("Compiler           : %s\n", FEENOX_COMPILER_VERSION);
#endif
#ifdef FEENOX_COMPILER_CFLAGS
  printf("Compiler flags     : %s\n", FEENOX_COMPILER_CFLAGS);
#endif
#ifdef FEENOX_COMPILATION_USERNAME
  printf("Builder            : %s@%s\n", FEENOX_COMPILATION_USERNAME, FEENOX_COMPILATION_HOSTNAME);
#endif  
  
  printf("GSL version        : %s\n", gsl_version);
  
#if HAVE_IDA
  char *sundials_version = malloc(BUFFER_TOKEN_SIZE);
  SUNDIALSGetVersion(sundials_version, BUFFER_TOKEN_SIZE);
#else
  char *sundials_version = "N/A";
#endif
  printf("SUNDIALs version   : %s\n", sundials_version);
  
  printf("Readline version   : %s\n", 
#if HAVE_READLINE
         rl_library_version
#else
         "N/A"
#endif
  );


#if HAVE_PETSC
  char petscversion[BUFFER_TOKEN_SIZE];
  char petscarch[BUFFER_TOKEN_SIZE];
  
  PetscGetVersion(petscversion, BUFFER_TOKEN_SIZE);
  PetscGetArchType(petscarch, BUFFER_TOKEN_SIZE);
  printf("PETSc version      : %s\n", petscversion);
  printf("PETSc arch         : %s\n", petscarch);
  printf("PETSc options      : %s\n", petscconfigureoptions);
#else
  printf("PETSc version      : N/A\n");
#endif      
  
#ifdef HAVE_SLEPC
  char slepcversion[BUFFER_TOKEN_SIZE];
  SlepcGetVersion(slepcversion, BUFFER_TOKEN_SIZE);  
#else
  char *slepcversion = "N/A";
#endif
  printf("SLEPc version      : %s\n", slepcversion);

  return;
}

