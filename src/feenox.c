/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX main function
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
#include <getopt.h>
#include <string.h>

#include "feenox.h"


int main(int argc, char **argv) {

  int i, optc;
  int option_index = 0;
  int show_help = 0;
  int show_version = 0;
  
  const struct option longopts[] = {
    { "help",     no_argument,       NULL, 'h'},
    { "version",  no_argument,       NULL, 'v'},
    { "info",     no_argument,       NULL, 'i'},
    { "no-debug", no_argument,       NULL, 'n'},
    { "debug",    no_argument,       NULL, 'd'},
    { "list",     no_argument,       NULL, 'l'},
    { NULL, 0, NULL, 0 }
  };  
 
  // before getop breaks them we make a copy of the arguments
  feenox.argc_orig = argc;
  feenox.argv_orig = malloc((argc+1) * sizeof(char *));
  for (i = 0; i < argc; i++) {
    feenox.argv_orig[i] = strdup(argv[i]);
  }
  feenox.argv_orig[i] = NULL;
  
  
  // don't complain about unknown options, they can be for PETSc/SLEPc
  opterr = 0;
  while ((optc = getopt_long_only(argc, argv, "hvil", longopts, &option_index)) != -1) {
    switch (optc) {
      case 'h':
        show_help = 1;
        break;
      case 'v':
        show_version = version_copyright;
        break;
      case 'i':
        show_version = version_info;
        break;
      case 'n':
        feenox.mode = mode_ignore_debug;
        break;
      case 'd':
        feenox.mode = mode_debug;
        break;
      case 'l':
        feenox.mode = mode_list_vars;
        break;
      case '?':
        break;
      default:
        break;
    }
  }
  
  if (show_help) {
    feenox_show_help(argv[0]);
    exit(EXIT_SUCCESS);
  } else if (show_version && optind == argc) {
    feenox_show_version(show_version);
    exit(EXIT_SUCCESS);
  } else if (feenox.mode && optind == argc) {
/*    
    feenox_call(feenox_init_before_parser());
    feenox_list_symbols();
    feenox_finalize();
 */
    exit(EXIT_SUCCESS);
  } else if (optind == argc) {
    feenox_show_version(0);
//    feenox_finalize();
    exit(EXIT_SUCCESS);
  }
  
  
   printf("FeenoX\n");
   return 0;
}
