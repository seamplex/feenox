/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox error handling functions
 *
 *  Copyright (C) 2009--2013,2020 jeremy theler
 *
 *  This file is part of feenox.
 *
 *  feenox is free software: you can redistribute it and/or modify
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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/types.h>

#include <gsl/gsl_errno.h>

void feenox_push_error_message(const char *fmt, ...) {
  
  if (feenox.error_level > 100) {
    feenox_pop_errors();
    fprintf(stderr, "error: too many errors\n");
    exit(EXIT_FAILURE);
  }
  
  if ((feenox.error = realloc(feenox.error, (++feenox.error_level)*sizeof(char *))) == NULL) {
    fprintf(stderr, "cannot allocate memory to report error\n");
    exit(EXIT_FAILURE);
  }
  if ((feenox.error[feenox.error_level-1] = malloc(BUFFER_LINE_SIZE)) == NULL) {
    fprintf(stderr, "cannot allocate memory to report error\n");
    exit(EXIT_FAILURE);
  }
  
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(feenox.error[feenox.error_level-1], BUFFER_LINE_SIZE, fmt, ap);
  va_end(ap);
  
  return;
}

void feenox_pop_error_message(void) {
  
  if (feenox.error_level > 0) {
    feenox_free(feenox.error[feenox.error_level-1]);
    feenox.error_level--;
  }
  
  return;
}


void feenox_pop_errors(void) {

  if (feenox.mpi_size < 2) {
    fprintf(stderr, "error: ");
  } else {
    fprintf(stderr, "[%d] error: ", feenox.mpi_rank);
  }
  
  // TODO: write into a string and then print the whole string at once
  if (feenox.error_level == 0) {
    fprintf(stderr, "unspecified error\n");
  } else {
    while (feenox.error_level > 0) {
      fprintf(stderr, "%s%s", feenox.error[feenox.error_level-1], (feenox.error_level != 1)?" ":"\n");
      feenox_pop_error_message();
    }
  }

  return;
}


void feenox_runtime_error(void) {
  feenox_pop_errors();
  feenox_polite_exit(EXIT_FAILURE);
  return;
}

void feenox_nan_error(void) {

  // TODO
/*  
  int on_nan = (int)(feenox_value(feenox_special_var(on_nan)));

  if (!(on_nan & ON_ERROR_NO_REPORT)) {
    fprintf(stderr, "error: NaN found\n");
  }

  if (!(on_nan & ON_ERROR_NO_QUIT)) {
    feenox_runtime_error();
  }
*/
  return;
  
}


void feenox_gsl_handler(const char *reason, const char *file_ptr, int line, int gsl_errno) {

/*  
  int on_gsl_error = (int)(feenox_value(feenox_special_var(on_gsl_error)));

  if (!(on_gsl_error & ON_ERROR_NO_REPORT)) {
    feenox_push_error_message("gsl error #%d '%s' in %s", gsl_errno, reason, file_ptr);
    feenox_pop_errors();
  }

  if (!(on_gsl_error & ON_ERROR_NO_QUIT)) {
    feenox_polite_exit(WASORA_RUNTIME_ERROR);
  }
*/
  return;
}


void feenox_signal_handler(int sig_num) {

  if (feenox.mpi_size != 0) {
    fprintf(stderr, "pid %d: signal #%d caught, finnishing...\n", getpid(), sig_num);
  } else {
    fprintf(stderr, "[%d] pid %d: signal #%d caught, finnishing...\n", feenox.mpi_rank, getpid(), sig_num);
  }
  fflush(stderr);

  feenox_special_var_value(done) = (double)1.0;

  feenox_polite_exit(EXIT_SUCCESS);
  exit(1);

}
