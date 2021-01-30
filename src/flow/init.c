/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX initialization routines
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
 *  along with feenox.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include "feenox.h"
extern feenox_t feenox;

#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <libgen.h>

#ifdef HAVE_READLINE
#include <readline/readline.h>
#endif

int feenox_initialize(int argc, char **argv) {
  
  int i, optc;
  int option_index = 0;
  int show_help = 0;
  int show_version = 0;
  
  // assume the user wants a standard run
  feenox.run = feenox_run_standard;
  
///help+usage+desc usage: feenox [options] inputfile [replacement arguments]  
  
  const struct option longopts[] = {
///op+help+option `-h`, `--help`
///op+help+desc display usage and commmand-line help and exit
    { "help",         no_argument,       NULL, 'h'},
///op+version+option `-v`, `--version`
///op+version+desc display brief version information and exit
    { "version",      no_argument,       NULL, 'v'},
///op+versions+option `-V`, `--versions`
///op+versions+desc display detailed version information
    { "versions",     no_argument,       NULL, 'V'},
    { "long-version", no_argument,       NULL, 'V'},
    { "version-long", no_argument,       NULL, 'V'},
///op+debug+option `-d`, `--debug`.
///op+debug+desc start in debug mode
    { "debug",        no_argument,       NULL, 'd'},
///op+summarize+option `-s`, `--sumarize`
///op+summarize+desc list all symbols in the input file and exit
    { "summarize",    no_argument,       NULL, 's'},
    // --progress, --mumps, etc. are handled by the pde initialization routines    
    { NULL, 0, NULL, 0 }
  };  
 
///help+extra+desc Instructions will be read from standard input if "-" is passed as inputfile, i.e.
///help+extra+desc @
///help+extra+desc ```{bash}
///help+extra+desc $ echo "PRINT 2+2" | feenox -
///help+extra+desc 4
///help+extra+desc $
///help+extra+desc ```
  
  opterr = 0;   // don't complain about unknown options, they can be for PETSc/SLEPc
  while ((optc = getopt_long_only(argc, argv, "hvVs", longopts, &option_index)) != -1) {
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
      case 'd':
        feenox.debug = 1;
        break;
      case 's':
        feenox.mode = mode_list_vars;
        break;
      case '?':
        break;
      default:
        break;
    }
  }
  
  feenox.optind = optind;
  
  if (show_help) {
    feenox_show_help(argv[0]);
    exit(EXIT_SUCCESS);
  } else if (show_version) {
    feenox_show_version(show_version);
    exit(EXIT_SUCCESS);
  } else if (feenox.optind == argc) {
    feenox_show_version(version_compact);
    printf("\n");
    feenox_show_help(argv[0]);
    exit(EXIT_SUCCESS);
  }
  

  // after getopt() the arguments are re-ordered as needed to further
  // process the input file and the replacement arguments
  feenox.argc = argc;
  feenox.argv = malloc((argc+1) * sizeof(char *));
  for (i = 0; i < argc; i++) {
    feenox.argv[i] = strdup(argv[i]);
  }
  feenox.argv[i] = NULL;
  
  // get a pointer to the main input file
  feenox.main_input_filepath = feenox.argv[feenox.optind];
  
  // remember the base directory of the input file so we can try harder to find files
  char *argv_optind = strdup(argv[feenox.optind]);
  feenox.main_input_dirname = dirname(argv_optind);
  
  
  // turn of GSL error handler
  gsl_set_error_handler_off();
  // TODO: use our own
  // gsl_set_error_handler(feenox_gsl_handler);

  signal(SIGINT,  feenox_signal_handler);
  signal(SIGTERM, feenox_signal_handler);
  
  feenox_call(feenox_init_special_objects());
  feenox_call(feenox_parse_main_input_file(feenox.main_input_filepath));
  free(argv_optind);
  
  return FEENOX_OK;
}

int feenox_init_special_objects(void) {

  var_t *dummy;

///va+done_static+name done_static
///va+done_static+desc Flag that indicates whether the static calculation is over or not.
///va+done_static+desc It is set to true (i.e. $\neq 0$) by feenox if `step_static` $\ge$ `static_steps`.
///va+done_static+desc If the user sets it to true, the current step is marked as the last static step and
///va+done_static+desc the static calculation ends after finishing the step.
  feenox_special_var(done_static) = feenox_get_or_define_variable_ptr("done_static");
  

///va+done_transient+name done_transient
///va+done_transient+desc Flag that indicates whether the transient calculation is over or not.
///va+done_transient+desc It is set to true (i.e. $\neq 0$) by feenox if `t` $\ge$ `end_time`.
///va+done_transient+desc If the user sets it to true, the current step is marked as the last transient step and
///va+done_transient+desc the transient calculation ends after finishing the step.
  feenox_special_var(done_transient) = feenox_get_or_define_variable_ptr("done_transient");


///va+done_outer+name done_outer
///va+done_outer+desc Flag that indicates whether the parametric, optimization of fit calculation is over or not.
///va+done_outer+desc It is set to true (i.e. $\neq 0$) by feenox whenever the outer calculation is considered to be finished,
///va+done_outer+desc which can be that the parametric calculation swept the desired parameter space or that the
///va+done_outer+desc optimization algorithm reached the desired convergence criteria.
///va+done_outer+desc If the user sets it to true, the current step is marked as the last outer step and
///va+done_outer+desc the transient calculation ends after finishing the step.
  feenox_special_var(done_outer) = feenox_get_or_define_variable_ptr("done_outer");


///va+done+name done
///va+done+desc Flag that indicates whether the overall calculation is over.
  feenox_special_var(done) = feenox_get_or_define_variable_ptr("done");

  

///va+step_static+name step_static
///va+step_static+desc Indicates the current step number of the iterative static calculation.
  feenox_special_var(step_static) = feenox_get_or_define_variable_ptr("step_static");
  

///va+step_transient+name step_transient
///va+step_transient+desc Indicates the current step number of the transient static calculation.
  feenox_special_var(step_transient) = feenox_get_or_define_variable_ptr("step_transient");
  

///va+step_outer+name step_outer
///va+step_outer+desc Indicates the current step number of the iterative outer calculation (parametric, optimization or fit).
  feenox_special_var(step_outer) = feenox_get_or_define_variable_ptr("step_outer");

///va+in_outer_initial+name in_outer_initial
///va+in_outer_initial+desc Flag that indicates if the current step is the initial step of an optimization of fit run.
  feenox_special_var(in_outer_initial) = feenox_get_or_define_variable_ptr("in_outer_initial");
  

///va+in_static+name in_static
///va+in_static+desc Flag that indicates if feenox is solving the iterative static calculation.
  feenox_special_var(in_static) = feenox_get_or_define_variable_ptr("in_static");


///va+in_static+name in_static_first
///va+in_static+desc Flag that indicates if feenox is in the first step of the iterative static calculation.
  feenox_special_var(in_static_first) = feenox_get_or_define_variable_ptr("in_static_first");


///va+in_static+name in_static_last
///va+in_static+desc Flag that indicates if feenox is in the last step of the iterative static calculation.
  feenox_special_var(in_static_last) = feenox_get_or_define_variable_ptr("in_static_last");


///va+in_transient+name in_transient
///va+in_transient+desc Flag that indicates if feenox is solving transient calculation.
  feenox_special_var(in_transient) = feenox_get_or_define_variable_ptr("in_transient");
  

///va+in_transient_first+name in_transient_first
///va+in_transient_first+desc Flag that indicates if feenox is in the first step of the transient calculation.
  feenox_special_var(in_transient_first) = feenox_get_or_define_variable_ptr("in_transient_first");
  

///va+in_transient_last+name in_transient_last
///va+in_transient_last+desc Flag that indicates if feenox is in the last step of the transient calculation.
  feenox_special_var(in_transient_last) = feenox_get_or_define_variable_ptr("in_transient_last");


///va+static_steps+name static_steps
///va+static_steps+desc Number of steps that ought to be taken during the static calculation, to be set by the user. 
///va+static_steps+desc The default value is one, meaning only one static step. 
  feenox_special_var(static_steps) = feenox_get_or_define_variable_ptr("static_steps");
  

///va+end_time+name end_time
///va+end_time+desc Final time of the transient calculation, to be set by the user. 
///va+end_time+desc The default value is zero, meaning no transient calculation.
  feenox_special_var(end_time) = feenox_get_or_define_variable_ptr("end_time");
  

///va+t+name t
///va+t+desc Actual value of the time for transient calculations. This variable is set by
///va+t+desc feenox, but can be written by the user for example by importing it from another
///va+t+desc transient code by means of shared-memory objects. Care should be taken when
///va+t+desc solving DAE systems and overwriting `t`.
  feenox_special_var(t) = feenox_get_or_define_variable_ptr("t");
  

///va+dt+name dt
///va+dt+desc Actual value of the time step for transient calculations. When solving DAE systems,
///va+dt+desc this variable is set by feenox. It can be written by the user for example by importing it from another
///va+dt+desc transient code by means of shared-memory objects. Care should be taken when
///va+dt+desc solving DAE systems and overwriting `t`. Default value is DEFAULT_DT, which is
///va+dt+desc a power of two and roundoff errors are thus reduced.
  feenox_special_var(dt) = feenox_get_or_define_variable_ptr("dt");
  feenox_special_var_value(dt) = DEFAULT_DT;

///va+rel_error+name rel_error
///va+rel_error+desc Maximum allowed relative error for the solution of DAE systems. Default value is
///va+rel_error+desc is $1 \times 10^{-6}$. If a fine per-variable error control is needed, special vector
///va+rel_error+desc `abs_error` should be used.
  feenox_special_var(rel_error) = feenox_get_or_define_variable_ptr("rel_error");
  feenox_special_var_value(rel_error) = DEFAULT_REL_ERROR;
  
///va+min_dt+name min_dt
///va+min_dt+desc Minimum bound for the time step that feenox should take when solving DAE systems.
  feenox_special_var(min_dt) = feenox_get_or_define_variable_ptr("min_dt");
  
///va+max_dt+name max_dt
///va+max_dt+desc Maximum bound for the time step that feenox should take when solving DAE systems.
  feenox_special_var(max_dt) = feenox_get_or_define_variable_ptr("max_dt");
  
///va+i+name i
///va+i+desc Dummy index, used mainly in vector and matrix row subindex expressions.
  feenox_special_var(i) = feenox_get_or_define_variable_ptr("i");

///va+j+name j
///va+j+desc Dummy index, used mainly in matrix column subindex expressions.
  feenox_special_var(j) = feenox_get_or_define_variable_ptr("j");
  
///va+pi+name pi
///va+pi+desc A double-precision floating point representaion of the number $\pi$, equal to
///va+pi+desc `math.h` 's `M_PI` constant.
  feenox_special_var(pi) = feenox_get_or_define_variable_ptr("pi");
  feenox_special_var_value(pi) = M_PI;

///va+zero+name zero
///va+zero+desc A very small positive number, which is taken to avoid roundoff 
///va+zero+desc errors when comparing floating point numbers such as replacing $a \leq a_\text{max}$
///va+zero+desc with $a < a_\text{max} +$ `zero`. Default is $(1/2)^{-50} \approx 9\times 10^{-16}$ .
  feenox_special_var(zero) = feenox_get_or_define_variable_ptr("zero");
  feenox_special_var_value(zero) = ZERO;


///va+infinite+name infinite
///va+infinite+desc A very big positive number, which can be used as `end_time = infinite` or
///va+infinite+desc to define improper integrals with infinite limits. Default is $2^{50} \approx 1 \times 10^{15}$.
  feenox_special_var(infinite) = feenox_get_or_define_variable_ptr("infinite");
  feenox_special_var_value(infinite) = INFTY;

///va+ncores+name ncores
///va+ncores+desc The number of online available cores, as returned by `sysconf(_SC_NPROCESSORS_ONLN)`.
///va+ncores+desc This value can be used in the `MAX_DAUGHTERS` expression of the `PARAMETRIC` keyword
///va+ncores+desc (i.e `ncores/2`).
  feenox_special_var(ncores) = feenox_get_or_define_variable_ptr("ncores");
#ifdef HAVE_SYSCONF  
  feenox_special_var_value(ncores) = (double)sysconf(_SC_NPROCESSORS_ONLN);
#endif
  
///va+pid+name pid
///va+pid+desc The UNIX process id of feenox (or the plugin).
  feenox_special_var(pid) = feenox_get_or_define_variable_ptr("pid");
  feenox_special_var_value(pid) = (double)getpid();

  
///va+on_nan+name on_nan
///va+on_nan+desc This should be set to a mask that indicates how to proceed if Not-A-Number signal (such as a division by zero)
///va+on_nan+desc is generated when evaluating any expression within feenox.
  feenox_special_var(on_nan) = feenox_get_or_define_variable_ptr("on_nan");

  
///va+on_gsl_error+name on_gsl_error
///va+on_gsl_error+desc This should be set to a mask that indicates how to proceed if an error ir raised in any
///va+on_gsl_error+desc routine of the GNU Scientific Library. 
  feenox_special_var(on_gsl_error) = feenox_get_or_define_variable_ptr("on_gsl_error");


///va+on_ida_error+name on_ida_error
///va+on_ida_error+desc This should be set to a mask that indicates how to proceed if an error ir raised in any
///va+on_ida_error+desc routine of the SUNDIALS IDA Library. 
  feenox_special_var(on_ida_error) = feenox_get_or_define_variable_ptr("on_ida_error");

  dummy = feenox_get_or_define_variable_ptr("quit");
  feenox_var_value(dummy) = 0;
  
  dummy = feenox_get_or_define_variable_ptr("report");
  feenox_var_value(dummy) = 0;

  dummy = feenox_get_or_define_variable_ptr("dont_quit");
  feenox_var_value(dummy) = ON_ERROR_NO_QUIT;
  
  dummy = feenox_get_or_define_variable_ptr("dont_report");
  feenox_var_value(dummy) = ON_ERROR_NO_REPORT;

  
///va+realtime_scale+name realtime_scale
///va+realtime_scale+desc If this variable is not zero, then the transient problem is run trying to syncrhonize the
///va+realtime_scale+desc problem time with realtime, up to a scale given. For example, if the scale is set to one, then
///va+realtime_scale+desc feenox will advance the problem time at the same pace that the real wall time advances. If set to
///va+realtime_scale+desc two, feenox's time wil advance twice as fast as real time, and so on. If the calculation time is
///va+realtime_scale+desc slower than real time modified by the scale, this variable has no effect on the overall behavior
///va+realtime_scale+desc and execution will proceed as quick as possible with no delays.
  feenox_special_var(realtime_scale) = feenox_get_or_define_variable_ptr("realtime_scale");

  // files 
/*  
  feenox.special_files.stdin_ = feenox_define_file("stdin", "stdin", 0, NULL, "r", 0);
  feenox.special_files.stdin_->pointer = stdin;
*/
  feenox_call(feenox_define_file("stdout", "stdout", 0, "w"));
  feenox.special_files.stdout_ = feenox_get_file_ptr("stdout");
  feenox.special_files.stdout_->pointer = stdout;
/*
  feenox.special_files.stderr_ = feenox_define_file("stderr", "stderr", 0, NULL, "w", 0);
  feenox.special_files.stderr_->pointer = stderr;
*/
  // el framework de mallas
/*  
  if (!feenox_mesh.initialized) {
    feenox_call(feenox_mesh_init_before_parser());
  }
*/
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
  
  // inicializamos plugins
  for (i = 0; i < feenox.i_plugin; i++) {
    if (feenox.plugin[i].init_after_parser() != 0) {
      feenox_push_error_message("%s:", feenox.plugin[i].name_string);
      return WASORA_RUNTIME_ERROR;
    }
  }
  
  // algunas cosas (por ejempo inteprolacion de funciones) se inicializan bajo
  // demanda, i.e. la primera vez que se necesita evaluar una funcion interpolada
  // se hace el init en tiempo de ejecucion
  
  return WASORA_RUNTIME_OK;
}
*/
  
// before each run we need to reset time, clear histories, etc
int feenox_init_before_run(void) {
  int i;
  print_t *print;
//  history_t *history;
//  file_t *file;
  
  feenox_special_var_value(step_static) = 0;
  feenox_special_var_value(step_transient) = 0;
  feenox_special_var_value(static_steps) = 1;
  feenox_special_var_value(t) = 0;
  feenox_special_var_value(dt) = DEFAULT_DT;
  feenox_special_var_value(rel_error) = DEFAULT_REL_ERROR;
  
  
  feenox_special_var_value(done_static) = 0;
  feenox_special_var_value(done_transient) = 0;
  feenox_special_var_value(done) = 0;

  // does anybody dare to change pi?
  feenox_special_var_value(pi) = M_PI;
  feenox_special_var_value(zero) = ZERO;
  feenox_special_var_value(infinite) = INFTY;
#ifdef HAVE_SYSCONF
  feenox_special_var_value(ncores) = (double)sysconf(_SC_NPROCESSORS_ONLN);
#endif
  feenox_special_var_value(pid) = (double)getpid();
  
  feenox.time_path_current = feenox.time_paths;
  
  
#ifdef HAVE_IDA
  // el cuento es asi: cuando hacemos estudios parametricos, las condiciones
  // iniciales se vuelven a calcular porque ponemos t = 0
  // pero las derivadas valen lo ultimo que valian al terminar el paso anterior
  // asi que a menos que querramos hacer estudios adiabaticos (TODO!)
  // volvemos a poner las derivadas en cero
  // si el usuario da explicitamente los valores de las derivas, es lo mismo
  // que las condiciones iniciales asi que todo manzana
  if (feenox.dae.phase_derivative != NULL) {
    for (i = 0; i < feenox.dae.dimension; i++) {
      *(feenox.dae.phase_value[i]) = 0;
      *(feenox.dae.phase_derivative[i]) = 0;
    }
  }
  
  if (feenox.dae.system != NULL) {
    IDAFree(&feenox.dae.system);
    feenox.dae.system = NULL;
  }
    
  if (feenox.dae.x != NULL) {
    N_VDestroy_Serial(feenox.dae.x);
    feenox.dae.x = NULL;
  }
  if (feenox.dae.dxdt != NULL) {
    N_VDestroy_Serial(feenox.dae.dxdt);
    feenox.dae.dxdt = NULL;
  }
  if (feenox.dae.id != NULL) {
    N_VDestroy_Serial(feenox.dae.id);
    feenox.dae.id = NULL;
  }  
#endif  

/*  
  LL_FOREACH(feenox.histories, history) {
    history->function->data_size = 0;
    history->position = 0;

    if (history->function->data_value != NULL) {
      free(history->function->data_value);
    }
    if (history->function->data_argument != NULL) {
      free(history->function->data_argument[0]);
      free(history->function->data_argument);
    }
  }  
*/
  LL_FOREACH(feenox.prints, print) {
    print->last_static_step = 0;
    print->last_step = 0;
    print->last_time = 0;
    print->last_header_step = 0;
    print->header_already_printed = 0;
  }  

  // si no estamos en parametrico o en optimizacion cerramos los files
  // TODO: marcar los files como global o algo asi
/*  
  if (feenox.parametric.outer_steps == 0 && feenox.min.n == 0) {
    for (file = feenox.files; file != NULL; file = file->hh.next) {
      if (file->pointer != NULL && fileno(file->pointer) > 2) {
        feenox_instruction_close_file(file);
      }
    }
  }  
*/
  
  // TODO: init before run the PDEs

  return FEENOX_OK;
}


/*
// mira si los n puntos en x[] y en y[] forman una grilla estruturada
int feenox_is_structured_grid_2d(double *x, double *y, int n, int *nx, int *ny) {

  int i, j;
    
  // hacemos una primera pasada sobre x hasta encontrar de nuevo el primer valor
  i = 0;
  do {
    if (++i == n) {   
      // si recorrimos todo el set y nunca aparecio otra vez el x[0] a comerla
      return 0;
    }
  } while (gsl_fcmp(x[i], x[0], DEFAULT_MULTIDIM_INTERPOLATION_THRESHOLD) != 0);
  
  // si i es uno entonces a comerla
  if (i == 1) {
    return 0;
  }
    
  // asumimos que la cantidad de datos en x es i
  *nx = i;
  // y entonces ny tiene que ser la cantidad de datos totales dividido data_size
  if (n % (*nx) != 0) {
    // si esto no es entero, no puede ser rectangular
    return 0;
  }
  *ny = n/(*nx);

  // si x[j] != x[j-nx] entonces no es una grilla estructurada
  for (i = *nx; i < n; i++) {
    if (gsl_fcmp(x[i], x[i-(*nx)], DEFAULT_MULTIDIM_INTERPOLATION_THRESHOLD*fabs(x[(*nx)-1]-x[0])) != 0) {
      return 0;
    }
  }
  
  // ahora miramos el conjunto y, tienen que ser nx valores igualitos consecutivos
  for (i = 0; i < *ny; i++) {
    for (j = 0; j < (*nx)-1; j++) {
      if (gsl_fcmp(y[i*(*nx) + j], y[i*(*nx) + j + 1], DEFAULT_MULTIDIM_INTERPOLATION_THRESHOLD*fabs(y[n-1]-y[0])) != 0) {
        return 0;
      }
    }
  }
  

  // si llegamos hasta aca, es una grilla bidimensional estructurada!
  return 1;
    
}


// mira si los n puntos en x[], y[] y z[] forman una grilla estruturada
int feenox_is_structured_grid_3d(double *x, double *y, double *z, int n, int *nx, int *ny, int *nz) {

  int i;
  
  // hacemos una pasada sobre x hasta encontrar de nuevo el primer valor
  i = 0;
  do {
    i += 1;
    if (i == n) {   
      // si recorrimos todo el set y nunca aparecio otra vez el x[0] a comerla
      return 0;
    }
  } while (gsl_fcmp(x[i], x[0], DEFAULT_MULTIDIM_INTERPOLATION_THRESHOLD) != 0);

  // si i es uno entonces a comerla
  if (i == 1) {
    return 0;
  }
    
  // asumimos que la cantidad de datos en x es i
  *nx = i;

  // hacemos una pasada sobre y hasta encontrar de nuevo el primer valor
  // pero con step nx
  i = 0;
  do {
    i += *nx;
    if (i >= n) {   
      // si recorrimos todo el set y nunca aparecio otra vez el y[0] a comerla
      return 0;
    }
  } while (gsl_fcmp(y[i], y[0], DEFAULT_MULTIDIM_INTERPOLATION_THRESHOLD) != 0);
  
  // si i es uno entonces a comerla
  if (i == *nx || i % ((*nx)) != 0) {
    return 0;
  }

  // asumimos que la cantidad de datos en x es i
  *ny = i/(*nx);
  
  
  // y entonces nz tiene que ser la cantidad de datos totales dividido data_size
  if (n % ((*ny)*(*nx)) != 0) {
    // si esto no es entero, no puede ser rectangular
    return 0;
  }
  *nz = n/((*ny)*(*nx));


  for (i = *nx; i < n; i++) {
    if (gsl_fcmp(x[i], x[i-(*nx)], DEFAULT_MULTIDIM_INTERPOLATION_THRESHOLD*fabs(x[(*nx)-1]-x[0])) != 0) {
      return 0;
    }
  }
  for (i = (*nx)*(*ny); i < n; i++) {
    if (gsl_fcmp(y[i], y[i-(*nx)*(*ny)], DEFAULT_MULTIDIM_INTERPOLATION_THRESHOLD*fabs(x[(*nx)-1]-x[0])) != 0) {
      return 0;
    }
  }
  
  
  return 1;    
}
*/
