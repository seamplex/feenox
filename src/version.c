/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX version and description routines
 *
 *  Copyright (C) 2009--2020 jeremy theler
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

#include <stdio.h>
#include <gsl/gsl_version.h>

#if HAVE_READLINE
#include <readline/readline.h>
#endif

#if HAVE_IDA
#include <sundials/sundials_config.h>
#endif

#if HAVE_PETSC
#include <petscsys.h>
#include <petscconfiginfo.h>
#endif
#ifdef HAVE_SLEPC
#include <slepcsys.h>
#endif
#include "feenox.h"
#include "version.h"

void feenox_show_help(char *progname) {
  // in parallel runs only print from first processor
  if (feenox.rank != 0) {
    return;
  }
  
  printf("usage: %s [options] inputfile [replacement arguments]\n", progname);
  
  printf("\n\
  -h, --help            display this help and exit\n\
  -i, --info            display detailed code information and exit\n\
  -v, --version         display version information and exit\n\n\
  -l, --list            list defined symbols and exit\n\
      --debug           start in debug mode\n\
      --no-debug        ignore standard input, avoid debug mode\n\
 instructions are read from standard input if \"-\" is passed as inputfile, i.e.\n\
   $ echo 2+2 | feenox -\n\
   4\n\
   $\n");
  
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
                             strcmp(FEENOX_GIT_BRANCH, "master")?FEENOX_GIT_BRANCH:"");
#else
  printf("%s\n", PACKAGE_VERSION);
#endif

  printf("a free no-fee no-X uniX-like finite-elementish computational engineering tool\n");

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
  printf("FeenoX is copyright (C) %d-%d %s\n\
an is licensed under GNU GPL version 3 or later.\n\
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
  printf("SUNDIALs version   : %s\n", SUNDIALS_VERSION);  
#endif
#if HAVE_READLINE
  printf("Readline version   : %s\n", rl_library_version);
#endif
    
#if HAVE_PETSC
  char petscversion[BUFFER_TOKEN_SIZE];
  char petscarch[BUFFER_TOKEN_SIZE];
  
  PetscGetVersion(petscversion, BUFFER_TOKEN_SIZE);
  PetscGetArchType(petscarch, BUFFER_TOKEN_SIZE);
  printf("PETSc version      : %s\n", petscversion);
  printf("PETSc arch         : %s\n", petscarch);
  printf("PETSc options      : %s\n", petscconfigureoptions);
#endif  
  
#ifdef HAVE_SLEPC
  char slepcversion[BUFFER_TOKEN_SIZE];
  SlepcGetVersion(slepcversion, BUFFER_TOKEN_SIZE);  
  printf("SLEPc version      : %s\n", slepcversion);
#endif  
  return;
}

