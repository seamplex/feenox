/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX initialization routines
 *
 *  Copyright (C) 2009--2024 Jeremy Theler
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

#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <libgen.h>

#ifdef HAVE_CLOCK_GETTIME
 #include <time.h>
#endif

#ifdef HAVE_READLINE
 #include <readline/readline.h>
#endif

int feenox_initialize(int argc, char **argv) {
  
  int optc = 0;
  int option_index = 0;
  int show_help = 0;
  int show_version = 0;

///help+usage+desc [options] inputfile [replacement arguments] [petsc options]  
  const struct option longopts[] = {
///op+help+option `-h`, `--help`
///op+help+desc display options and detailed explanations of command-line usage
    { "help",          no_argument,       NULL, 'h'},
///op+version+option `-v`, `--version`
///op+version+desc display brief version information and exit
    { "version",       no_argument,       NULL, 'v'},
///op+versions+option `-V`, `--versions`
///op+versions+desc display detailed version information
    { "versions",      no_argument,       NULL, 'V'},
    { "long-version",  no_argument,       NULL, 'V'},
    { "version-long",  no_argument,       NULL, 'V'},
///op+check+option `-c`, `--check`
///op+check+desc validates if the input file is sane or not
    { "check",         no_argument,       NULL, 'c'},
///op+pdes+option `--pdes`
///op+pdes+desc list the types of `PROBLEM`s that FeenoX can solve, one per line
    { "pdes",          no_argument,       NULL, 'p'},
///op+elements_info+option `--elements_info`
///op+elements_info+desc output a document with information about the supported element types
    { "elements_info", no_argument,       NULL, 'e'},
///op+ast+option `--ast`
///op+ast+desc dump an abstract syntax tree of the input
    { "ast",           no_argument,       NULL, 'a'},
    
///op+linear+option `--linear`
///op+linear+desc force FeenoX to solve the PDE problem as linear
    { "linear",        no_argument,       NULL, 'l'},
///op+non-linear+option `--non-linear`
///op+non-linear+desc force FeenoX to solve the PDE problem as non-linear
    { "nonlinear",     no_argument,       NULL, 'n'},
    { "non-linear",    no_argument,       NULL, 'n'},

//op+debug+option `-d`, `--debug`.
//op+debug+desc start in debug mode
//    { "debug",        no_argument,       NULL, 'd'},
//op+summarize+option `-s`, `--sumarize`
//op+summarize+desc list all symbols in the input file and exit
//    { "summarize",    no_argument,       NULL, 's'},
    { NULL, 0, NULL, 0 }
  };  
 
///help+extra+desc Instructions will be read from standard input if "-" is passed as `inputfile`, i.e.
///help+extra+desc @
///help+extra+desc ```terminal
///help+extra+desc $ echo 'PRINT 2+2' | feenox -
///help+extra+desc 4
///help+extra+desc ```
///help+extra+desc @
///help+extra+desc The optional `[replacement arguments]` part of the command line mean that each
///help+extra+desc argument after the input file that does not start with an hyphen will be expanded
///help+extra+desc verbatim in the input file in each occurrence of `$1`, `$2`, etc. For example
///help+extra+desc @
///help+extra+desc ```terminal
///help+extra+desc $ echo 'PRINT $1+$2' | feenox - 3 4
///help+extra+desc 7
///help+extra+desc ```
///help+extra+desc @
///help+extra+desc PETSc and SLEPc options can be passed in `[petsc options]` (or `[options]`) as well, with the
///help+extra+desc difference that two hyphens have to be used instead of only once. For example,
///help+extra+desc to pass the PETSc option `-ksp_view` the actual FeenoX invocation should be
///help+extra+desc @
///help+extra+desc ```terminal
///help+extra+desc $ feenox input.fee --ksp_view
///help+extra+desc ```
///help+extra+desc @
///help+extra+desc For PETSc options that take values, en equal sign has to be used:
///help+extra+desc @
///help+extra+desc ```terminal
///help+extra+desc $ feenox input.fee --mg_levels_pc_type=sor
///help+extra+desc ```
///help+extra+desc @
///help+extra+desc See <https://www.seamplex.com/feenox/examples> for annotated examples.
  
  // make a copy before calling getopt so we can re-use argv & argc for PETSc
  feenox_check_alloc(feenox.argv_orig = malloc((argc+1) * sizeof(char *)));
  int i = 0;
  for (i = 0; i < argc; i++) {
    feenox_check_alloc(feenox.argv_orig[i] = strdup(argv[i]));
  }
  feenox.argv_orig[i] = NULL;
  
  int dump_ast = 0;
  opterr = 0;   // don't complain about unknown options, they can be for PETSc/SLEPc
  while ((optc = getopt_long_only(argc, argv, "hvVc", longopts, &option_index)) != -1) {
    switch (optc) {
      case 'h':
        show_help = 1;
        break;
      case 'v':
        show_version = version_copyright;
        break;
      case 'V':
        show_version = version_info;
        break;
      case 'c':
        feenox.check = 1;
        break;
      case 'a':
        feenox.check = 1;
        dump_ast = 1;
        break;
      case 'p':
        show_version = version_available_pdes;
        break;
      case 'e':
        show_version = version_elements_info;
        break;
      case 'l':
        feenox.pde.math_type = math_type_linear;
        break;
      case 'n':
        feenox.pde.math_type = math_type_nonlinear;
        break;
//      case 'd':
//        feenox.debug = 1;
//        break;
//      case 's':
//        feenox.mode = mode_list_vars;
//        break;
      case '?':
        break;
      default:
        break;
    }
  }
  
  // we need to know this so we can handle expansion arguments
  feenox.optind = optind;
  
  if (show_help) {
    feenox_show_help(argv[0], 1);
    exit(EXIT_SUCCESS);
  } else if (show_version) {
    feenox_show_version(show_version);
    exit(EXIT_SUCCESS);
  } else if (feenox.optind == argc) {
    feenox_show_version(version_compact);
    printf("\n");
    feenox_show_help(argv[0], 0);
    exit(EXIT_SUCCESS);
  }
  

  // after getopt() the arguments are re-ordered as needed to further
  // process the input file and the replacement arguments
  feenox.argc = argc;
  feenox_check_alloc(feenox.argv = calloc(argc+1, sizeof(char *)));
  for (i = 0; i < argc; i++) {
    feenox_check_alloc(feenox.argv[i] = strdup(argv[i]));
  }
  feenox.argv[i] = NULL;
   
  // get a pointer to the main input file
  feenox.main_input_filepath = strdup(feenox.argv[feenox.optind]);
  
  // remember the base directory of the input file so we can try harder to find files
  feenox_check_alloc(feenox.main_input_dirname = strdup(dirname(argv[feenox.optind])));
  
  // remove the directory and trailing .fee extension so ${0} has the case name
  // first we have to make a copy of the current argv to pass it to basename
  // because basename might break the argument
  char *argv0 = NULL;
  feenox_check_alloc(argv0 = strdup(feenox.argv[feenox.optind]));
  // we should now free the original argv;
  feenox_free(feenox.argv[feenox.optind]);
  // get the basename (it is a pointer to statically allocated memory)
  char *argv0_basename = basename(argv0);
  char *fee = strstr(argv0_basename, ".fee");
  if (fee != NULL) {
    *fee = '\0';
  }
  // put the resulting argv0 in the actual argv0
  feenox_check_alloc(feenox.argv[feenox.optind] = strdup(argv0_basename));
  // free argv0
  feenox_free(argv0);
  
  // turn of GSL error handler
//  gsl_set_error_handler_off();
  // TODO: use our own
  // gsl_set_error_handler(feenox_gsl_handler);

  signal(SIGINT,  feenox_signal_handler);
  signal(SIGTERM, feenox_signal_handler);
  
  feenox_call(feenox_init_special_objects());
  feenox_call(feenox_parse_main_input_file(feenox.main_input_filepath));
  
  if (dump_ast) {
    printf("{\n");
    printf("  \"bcs\": [\n");
    // this is a loop over a hash, not over a linked list
    for (bc_t *bc = feenox.mesh.bcs; bc != NULL; bc = bc->hh.next) {
      printf("    { \"name\": \"%s\",\n", bc->name);
      printf("      \"groups\": [\n");
      
      int first = 1;
      physical_group_t *physical_group = NULL;
      for (physical_group = bc->mesh->physical_groups; physical_group != NULL; physical_group = physical_group->hh.next) {
        for (int i = 0; i < physical_group->n_bcs; i++) {
          if (physical_group->bc[i] == bc) {
            if (first == 0) {
              printf(",\n");
            } else {
              first = 0;
            }
            printf("        \"%s\"", physical_group->name);
          }
        }
      }
      
      printf("\n");
      printf("      ] }");
      if (bc->hh.next != NULL) {
        printf(",");
      }
      printf("\n");
    }
    printf("  ]\n");
    printf("}\n");
  }
  
  return FEENOX_OK;
}

int feenox_init_special_objects(void) {

#ifdef HAVE_CLOCK_GETTIME
  if (clock_gettime(CLOCK_MONOTONIC, &feenox.tp0) < 0) {
    feenox_runtime_error();
  }
#endif //  HAVE_CLOCK_GETTIME
  
  var_t *dummy = NULL;

///va+done_static+desc Flag that indicates whether the static calculation is over or not.
///va+done_static+detail It is set to true (i.e. $\neq 0$) by feenox if `step_static` $\ge$ `static_steps`.
///va+done_static+detail If the user sets it to true, the current step is marked as the last static step and
///va+done_static+detail the static calculation ends after finishing the step.
///va+done_static+detail It can be used in `IF` blocks to check if the static step is finished or not.
  feenox_special_var(done_static) = feenox_get_or_define_variable_get_ptr("done_static");
  

///va+done_transient+desc Flag that indicates whether the transient calculation is over or not.
///va+done_transient+detail It is set to true (i.e. $\neq 0$) by feenox if `t` $\ge$ `end_time`.
///va+done_transient+detail If the user sets it to true, the current step is marked as the last transient step and
///va+done_transient+detail the transient calculation ends after finishing the step.
///va+done_transient+detail It can be used in `IF` blocks to check if the transient steps are finished or not.
  feenox_special_var(done_transient) = feenox_get_or_define_variable_get_ptr("done_transient");


///va+done+desc Flag that indicates whether the overall calculation is over.
///va+done+detail This variable is set to true by FeenoX when the computation finished so it can
///va+done+detail be checked in an `IF` block to do something only in the last step.
///va+done+detail But this variable can also be set to true from the input file, indicating that
///va+done+detail the current step should also be the last one. For example, one can set `end_time = infinite`
///va+done+detail and then finish the computation at $t=10$ by setting `done = t > 10`.
///va+done+detail This `done` variable can also come from (and sent to) other sources, like
///va+done+detail a shared memory object for coupled calculations.
  feenox_special_var(done) = feenox_get_or_define_variable_get_ptr("done");

  

///va+step_static+desc Indicates the current step number of the iterative static calculation.
///va+step_static+detail This is a read-only variable that contains the current step of the static calculation.
  feenox_special_var(step_static) = feenox_get_or_define_variable_get_ptr("step_static");
  feenox_special_var_value(step_static) = 0;
  

///va+step_transient+desc Indicates the current step number of the transient static calculation.
///va+step_transient+detail This is a read-only variable that contains the current step of the transient calculation.
  feenox_special_var(step_transient) = feenox_get_or_define_variable_get_ptr("step_transient");
  feenox_special_var_value(step_transient) = 0;
  
///va+in_static+desc Flag that indicates if FeenoX is solving the iterative static calculation.
///va+in_static+detail This is a read-only variable that is non zero if the static calculation.
  feenox_special_var(in_static) = feenox_get_or_define_variable_get_ptr("in_static");


///va+in_static_first+desc Flag that indicates if feenox is in the first step of the iterative static calculation.
  feenox_special_var(in_static_first) = feenox_get_or_define_variable_get_ptr("in_static_first");


///va+in_static_last+desc Flag that indicates if feenox is in the last step of the iterative static calculation.
  feenox_special_var(in_static_last) = feenox_get_or_define_variable_get_ptr("in_static_last");


///va+in_transient+desc Flag that indicates if feenox is solving transient calculation.
  feenox_special_var(in_transient) = feenox_get_or_define_variable_get_ptr("in_transient");
  

///va+in_transient_first+desc Flag that indicates if feenox is in the first step of the transient calculation.
  feenox_special_var(in_transient_first) = feenox_get_or_define_variable_get_ptr("in_transient_first");
  

///va+in_transient_last+desc Flag that indicates if feenox is in the last step of the transient calculation.
  feenox_special_var(in_transient_last) = feenox_get_or_define_variable_get_ptr("in_transient_last");

///va+in_time_path+desc Flag that indicates if feenox is in a time belonging to a `TIME_PATH` keyword.
  feenox_special_var(in_time_path) = feenox_get_or_define_variable_get_ptr("in_time_path");
  

///va+static_steps+desc Number of steps that ought to be taken during the static calculation, to be set by the user. 
///va+static_steps+detail The default value is one, meaning only one static step. 
  feenox_special_var(static_steps) = feenox_get_or_define_variable_get_ptr("static_steps");
  feenox_special_var_value(static_steps) = 1;
  

///va+end_time+desc Final time of the transient calculation. 
///va+end_time+detail Set this variable to a positive value to ask for a transient calculation.
///va+end_time+detail For some PDEs, the default might be a quasi-static problem instead of an actual transient one.
///va+end_time+detail For instance, setting a non-zero `end_time` in `PROBLEM mechanical` starts a quasi-static computation
///va+end_time+detail but a real transient one for `PROBLEM thermal`.
///va+end_time+detail The default value is zero, meaning a static problem.
  feenox_special_var(end_time) = feenox_get_or_define_variable_get_ptr("end_time");
  

///va+t+desc Actual value of the time for transient or quasi-static calculations.
///va+t+detail This variable can be `PRINT`ed and used in expressions involving time and/or
///va+t+detail as arguments of time-dependent functions.
///va+t+detail It stars equal to zero and it is updated by FeenoX according to
///va+t+detail the time-stepping scheme. At the end, it is exactly equal to `end_time`.

  feenox_special_var(t) = feenox_get_or_define_variable_get_ptr("t");
  feenox_special_var_value(t) = 0;  
  

///va+dt+desc Actual value of the time step for transient or quasi-static calculations.
///va+dt+detail This variable can be `PRINT`ed and used in expressions involving time and/or
///va+dt+detail as arguments of time-dependent functions.
///va+dt+detail The initial time step can be set by assigning a non-zero value to `dt_0`.
///va+dt+detail Do not set `dt` explicitly in an assignment.
///va+dt+detail If you want to have a constant time step set both `min_dt` and `max_dt` to the same value.
///va+dt+detail Default initial time step is DEFAULT_DT, which is
///va+dt+detail a power of two and round-off errors are thus reduced.
  feenox_special_var(dt) = feenox_get_or_define_variable_get_ptr("dt");
  feenox_special_var_value(dt) = DEFAULT_DT;

///va_dae+dae_rtol+desc Maximum allowed relative error for the solution of DAE systems.
///va_dae+dae_rtol+detail Default value is
///va_dae+dae_rtol+detail is $1 \times 10^{-6}$. If a fine per-variable error control is needed, special vector
///va_dae+dae_rtol+detail `abs_error` should be used.
  feenox_special_var(dae_rtol) = feenox_get_or_define_variable_get_ptr("dae_rtol");
  feenox_special_var_value(dae_rtol) = DEFAULT_DAE_RTOL;
  
///va+min_dt+desc Minimum bound for the time step that FeenoX should take when solving transient problems.
  feenox_special_var(min_dt) = feenox_get_or_define_variable_get_ptr("min_dt");
  
///va+max_dt+desc Maximum bound for the time step that FeenoX should take when solving transient systems.
  feenox_special_var(max_dt) = feenox_get_or_define_variable_get_ptr("max_dt");
  
///va+i+desc Dummy index, used mainly in vector and matrix row subindex expressions.
  feenox_special_var(i) = feenox_get_or_define_variable_get_ptr("i");

///va+j+desc Dummy index, used mainly in matrix column subindex expressions.
  feenox_special_var(j) = feenox_get_or_define_variable_get_ptr("j");
  
///va+pi+desc A double-precision floating· point representation of the number $\pi$
///va+pi+detail It is equal to the `M_PI` constant in `math.h` .
  feenox_special_var(pi) = feenox_get_or_define_variable_get_ptr("pi");
  feenox_special_var_value(pi) = M_PI;

///va+zero+desc A very small positive number.
///va+zero+detail It is taken to avoid roundoff 
///va+zero+detail errors when comparing floating point numbers such as replacing $a \leq a_\text{max}$
///va+zero+detail with $a < a_\text{max} +$ `zero`.
///va+zero+detail  Default is $(1/2)^{-50} \approx 9\times 10^{-16}$ .
  feenox_special_var(zero) = feenox_get_or_define_variable_get_ptr("zero");
  feenox_special_var_value(zero) = ZERO;


///va+infinite+desc A very big positive number.
///va+infinite+detail It can be used as `end_time = infinite` or
///va+infinite+detail to define improper integrals with infinite limits.
///va+infinite+detail  Default is $2^{50} \approx 1 \times 10^{15}$.
  feenox_special_var(infinite) = feenox_get_or_define_variable_get_ptr("infinite");
  feenox_special_var_value(infinite) = INFTY;

///va+pid+desc The Unix process id of the FeenoX instance.
  feenox_special_var(pid) = feenox_get_or_define_variable_get_ptr("pid");
  feenox_special_var_value(pid) = (double)getpid();

///va+mpi_size+desc The number of total ranks in an MPI execution.
  feenox_special_var(mpi_size) = feenox_get_or_define_variable_get_ptr("mpi_size");
  feenox_special_var_value(mpi_size) = 0;

///va+mpi_rank+desc The current rank in an MPI execution. Mind the `PRINTF_ALL` instruction.
  feenox_special_var(mpi_rank) = feenox_get_or_define_variable_get_ptr("mpi_rank");
  feenox_special_var_value(mpi_rank) = 0;
  
///va+on_nan+desc This should be set to a mask that indicates how to proceed if Not-A-Number signal (such as a division by zero)
///va+on_nan+desc is generated when evaluating any expression within feenox.
  feenox_special_var(on_nan) = feenox_get_or_define_variable_get_ptr("on_nan");

  
///va+on_gsl_error+desc This should be set to a mask that indicates how to proceed if an error ir raised in any
///va+on_gsl_error+desc routine of the GNU Scientific Library. 
  feenox_special_var(on_gsl_error) = feenox_get_or_define_variable_get_ptr("on_gsl_error");


///va+on_ida_error+desc This should be set to a mask that indicates how to proceed if an error ir raised in any
///va+on_ida_error+desc routine of the SUNDIALS Library. 
  feenox_special_var(on_ida_error) = feenox_get_or_define_variable_get_ptr("on_sundials_error");

  dummy = feenox_get_or_define_variable_get_ptr("quit");
  feenox_var_value(dummy) = 0;
  
  dummy = feenox_get_or_define_variable_get_ptr("report");
  feenox_var_value(dummy) = 0;

  dummy = feenox_get_or_define_variable_get_ptr("dont_quit");
  feenox_var_value(dummy) = ON_ERROR_NO_QUIT;
  
  dummy = feenox_get_or_define_variable_get_ptr("dont_report");
  feenox_var_value(dummy) = ON_ERROR_NO_REPORT;

  
//va+realtime_scale+desc If this variable is not zero, then the transient problem is run trying to synchronize the
//va+realtime_scale+desc problem time with realtime, up to a scale given.
//va+realtime_scale+detail  For example, if the scale is set to one, then
//va+realtime_scale+detail FeenoX will advance the problem time at the same pace that the real wall time advances. If set to
//va+realtime_scale+detail two, FeenoX time will advance twice as fast as real time, and so on. If the calculation time is
//va+realtime_scale+detail slower than real time modified by the scale, this variable has no effect on the overall behavior
//va+realtime_scale+detail and execution will proceed as quick as possible with no delays.
//  feenox_special_var(realtime_scale) = feenox_get_or_define_variable_get_ptr("realtime_scale");

  // standard unix files 
  feenox_call(feenox_define_file("stdin", "stdin", 0, "r"));
  feenox.special_files._stdin = feenox_get_file_ptr("stdin");
  feenox.special_files._stdin->pointer = stdin;

  feenox_call(feenox_define_file("stdout", "stdout", 0, "w"));
  feenox.special_files._stdout = feenox_get_file_ptr("stdout");
  feenox.special_files._stdout->pointer = stdout;

  feenox_call(feenox_define_file("stderr", "stderr", 0, "w"));
  feenox.special_files._stderr = feenox_get_file_ptr("stderr");
  feenox.special_files._stderr->pointer = stderr;

  return FEENOX_OK;

}

// esta se llama despues del parser, hay que verificar que everything este manzana
// y pedir shared memory objecst y esas cosas
/*
int feenox_init_after_parser(void) {

  int i;
  
    
  
#ifdef HAVE_READLINE
  // autocompletion
  rl_attempted_completion_function = feenox_rl_completion;
#endif
  
  return WASORA_RUNTIME_OK;
}
*/
