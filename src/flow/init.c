/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX initialization routines
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
 *  along with wasora.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <libgen.h>

#ifdef HAVE_READLINE
#include <readline/readline.h>
#endif

#include "feenox.h"
//#include "thirdparty/kdtree.h"


int feenox_initialize(int argc, char **argv) {
  
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
 
  // before getop() breaks them we make a copy of the arguments
  // TODO: de we need this?
/*  
  feenox.argc_orig = argc;
  feenox.argv_orig = malloc((argc+1) * sizeof(char *));
  for (i = 0; i < argc; i++) {
    feenox.argv_orig[i] = strdup(argv[i]);
  }
  feenox.argv_orig[i] = NULL;
*/  
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
  
  feenox.optind = optind;
  
  if (show_help) {
    feenox_show_help(argv[0]);
    exit(EXIT_SUCCESS);
  } else if (show_version && feenox.optind == argc) {
    feenox_show_version(show_version);
    exit(EXIT_SUCCESS);
  } else if (feenox.mode && feenox.optind == argc) {
    // TODO
/*    
    feenox_call(feenox_init_before_parser());
    feenox_list_symbols();
    feenox_finalize();
 */
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
  feenox.main_input_dirname = dirname(strdup(argv[feenox.optind]));
  
  
  // turn of GSL error handler
  gsl_set_error_handler_off();
  // TODO: use our own
  // gsl_set_error_handler(feenox_gsl_handler);

  signal(SIGINT,  feenox_signal_handler);
  signal(SIGTERM, feenox_signal_handler);
  
  feenox_call(feenox_init_special_objects());
  feenox_call(feenox_parse_main_input_file(feenox.main_input_filepath));
  
  return FEENOX_OK;
}

// esta se llama despues de haber alocado los objetos
// la idea es definir variables, funciones, archivos, etc. internos
int feenox_init_special_objects(void) {

/*  
  int i;
  int error;
  var_t *dummy;

///va+done_static+name done_static
///va+done_static+desc Flag that indicates whether the static calculation is over or not.
///va+done_static+desc It is set to true (i.e. $\neq 0$) by wasora if `step_static` $\ge$ `static_steps`.
///va+done_static+desc If the user sets it to true, the current step is marked as the last static step and
///va+done_static+desc the static calculation ends after finishing the step.
  wasora_special_var(done_static) = wasora_define_variable("done_static");
  

///va+done_transient+name done_transient
///va+done_transient+desc Flag that indicates whether the transient calculation is over or not.
///va+done_transient+desc It is set to true (i.e. $\neq 0$) by wasora if `t` $\ge$ `end_time`.
///va+done_transient+desc If the user sets it to true, the current step is marked as the last transient step and
///va+done_transient+desc the transient calculation ends after finishing the step.
  wasora_special_var(done_transient) = wasora_define_variable("done_transient");


///va+done_outer+name done_outer
///va+done_outer+desc Flag that indicates whether the parametric, optimization of fit calculation is over or not.
///va+done_outer+desc It is set to true (i.e. $\neq 0$) by wasora whenever the outer calculation is considered to be finished,
///va+done_outer+desc which can be that the parametric calculation swept the desired parameter space or that the
///va+done_outer+desc optimization algorithm reached the desired convergence criteria.
///va+done_outer+desc If the user sets it to true, the current step is marked as the last outer step and
///va+done_outer+desc the transient calculation ends after finishing the step.
  wasora_special_var(done_outer) = wasora_define_variable("done_outer");


///va+done+name done
///va+done+desc Flag that indicates whether the overall calculation is over.
  wasora_special_var(done) = wasora_define_variable("done");

  

///va+step_static+name step_static
///va+step_static+desc Indicates the current step number of the iterative static calculation.
  wasora_special_var(step_static) = wasora_define_variable("step_static");
  

///va+step_transient+name step_transient
///va+step_transient+desc Indicates the current step number of the transient static calculation.
  wasora_special_var(step_transient) = wasora_define_variable("step_transient");
  

///va+step_outer+name step_outer
///va+step_outer+desc Indicates the current step number of the iterative outer calculation (parametric, optimization or fit).
  wasora_special_var(step_outer) = wasora_define_variable("step_outer");

///va+step_outer+name step_outer
///va+step_outer+desc Indicates the current step number of the iterative inner calculation (optimization or fit).
  wasora_special_var(step_inner) = wasora_define_variable("step_inner");

///va+in_outer_initial+name in_outer_initial
///va+in_outer_initial+desc Flag that indicates if the current step is the initial step of an optimization of fit run.
  wasora_special_var(in_outer_initial) = wasora_define_variable("in_outer_initial");
  

///va+in_static+name in_static
///va+in_static+desc Flag that indicates if wasora is solving the iterative static calculation.
  wasora_special_var(in_static) = wasora_define_variable("in_static");


///va+in_static+name in_static_first
///va+in_static+desc Flag that indicates if wasora is in the first step of the iterative static calculation.
  wasora_special_var(in_static_first) = wasora_define_variable("in_static_first");


///va+in_static+name in_static_last
///va+in_static+desc Flag that indicates if wasora is in the last step of the iterative static calculation.
  wasora_special_var(in_static_last) = wasora_define_variable("in_static_last");


///va+in_transient+name in_transient
///va+in_transient+desc Flag that indicates if wasora is solving transient calculation.
  wasora_special_var(in_transient) = wasora_define_variable("in_transient");
  

///va+in_transient_first+name in_transient_first
///va+in_transient_first+desc Flag that indicates if wasora is in the first step of the transient calculation.
  wasora_special_var(in_transient_first) = wasora_define_variable("in_transient_first");
  

///va+in_transient_last+name in_transient_last
///va+in_transient_last+desc Flag that indicates if wasora is in the last step of the transient calculation.
  wasora_special_var(in_transient_last) = wasora_define_variable("in_transient_last");


///va+static_steps+name static_steps
///va+static_steps+desc Number of steps that ought to be taken during the static calculation, to be set by the user. 
///va+static_steps+desc The default value is one, meaning only one static step. 
  wasora_special_var(static_steps) = wasora_define_variable("static_steps");
  wasora_var(wasora_special_var(static_steps)) = 1;
  

///va+end_time+name end_time
///va+end_time+desc Final time of the transient calculation, to be set by the user. 
///va+end_time+desc The default value is zero, meaning no transient calculation.
  wasora_special_var(end_time) = wasora_define_variable("end_time");
  

///va+t+name t
///va+t+desc Actual value of the time for transient calculations. This variable is set by
///va+t+desc wasora, but can be written by the user for example by importing it from another
///va+t+desc transient code by means of shared-memory objects. Care should be taken when
///va+t+desc solving DAE systems and overwriting `t`.
  wasora_special_var(t) = wasora_define_variable("t");
  wasora_special_var(time) = wasora_special_var(t);
  

///va+dt+name dt
///va+dt+desc Actual value of the time step for transient calculations. When solving DAE systems,
///va+dt+desc this variable is set by wasora. It can be written by the user for example by importing it from another
///va+dt+desc transient code by means of shared-memory objects. Care should be taken when
///va+dt+desc solving DAE systems and overwriting `t`. Default value is 1/16, which is
///va+dt+desc a power of two and roundoff errors are thus reduced.
  wasora_special_var(dt) = wasora_define_variable("dt");
  wasora_var(wasora_special_var(dt)) = 1.0/16.0;

///va+rel_error+name rel_error
///va+rel_error+desc Maximum allowed relative error for the solution of DAE systems. Default value is
///va+rel_error+desc is $1 \times 10^{-6}$. If a fine per-variable error control is needed, special vector
///va+rel_error+desc `abs_error` should be used.
  wasora_special_var(rel_error) = wasora_define_variable("rel_error");
  wasora_var(wasora_special_var(rel_error)) = 1e-6;
  

///va+min_dt+name min_dt
///va+min_dt+desc Minimum bound for the time step that wasora should take when solving DAE systems.
  wasora_special_var(min_dt) = wasora_define_variable("min_dt");
  

///va+max_dt+name max_dt
///va+max_dt+desc Maximum bound for the time step that wasora should take when solving DAE systems.
  wasora_special_var(max_dt) = wasora_define_variable("max_dt");
  

///va+i+name i
///va+i+desc Dummy index, used mainly in vector and matrix row subindex expressions.
  wasora_special_var(i) = wasora_define_variable("i");


///va+j+name j
///va+j+desc Dummy index, used mainly in matrix column subindex expressions.
  wasora_special_var(j) = wasora_define_variable("j");
  

///va+pi+name pi
///va+pi+desc A double-precision floating point representaion of the number $\pi$, equal to
///va+pi+desc `math.h` 's `M_PI` constant.
  wasora_special_var(pi) = wasora_define_variable("pi");
  wasora_var(wasora_special_var(pi)) = M_PI;
  

///va+zero+name zero
///va+zero+desc A very small positive number, which is taken to avoid roundoff 
///va+zero+desc errors when comparing floating point numbers such as replacing $a \leq a_\text{max}$
///va+zero+desc with $a < a_\text{max} +$ `zero`. Default is $(1/2)^{-50} \approx 9\times 10^{-16}$ .
  wasora_special_var(zero) = wasora_define_variable("zero");
  wasora_var(wasora_special_var(zero)) = ZERO;


///va+infinite+name infinite
///va+infinite+desc A very big positive number, which can be used as `end_time = infinite` or
///va+infinite+desc to define improper integrals with infinite limits. Default is $2^{50} \approx 1 \times 10^{15}$.
  wasora_special_var(infinite) = wasora_define_variable("infinite");
  wasora_var(wasora_special_var(infinite)) = INFTY;

///va+ncores+name ncores
///va+ncores+desc The number of online available cores, as returned by `sysconf(_SC_NPROCESSORS_ONLN)`.
///va+ncores+desc This value can be used in the `MAX_DAUGHTERS` expression of the `PARAMETRIC` keyword
///va+ncores+desc (i.e `ncores/2`).
  wasora_special_var(ncores) = wasora_define_variable("ncores");
  wasora_var(wasora_special_var(ncores)) = (double)sysconf(_SC_NPROCESSORS_ONLN);
  
///va+pid+name pid
///va+pid+desc The UNIX process id of wasora (or the plugin).
  wasora_special_var(pid) = wasora_define_variable("pid");
  wasora_var(wasora_special_var(pid)) = (double)getpid();

  
///va+on_nan+name on_nan
///va+on_nan+desc This should be set to a mask that indicates how to proceed if Not-A-Number signal (such as a division by zero)
///va+on_nan+desc is generated when evaluating any expression within wasora.
  wasora_special_var(on_nan) = wasora_define_variable("on_nan");

  
///va+on_gsl_error+name on_gsl_error
///va+on_gsl_error+desc This should be set to a mask that indicates how to proceed if an error ir raised in any
///va+on_gsl_error+desc routine of the GNU Scientific Library. 
  wasora_special_var(on_gsl_error) = wasora_define_variable("on_gsl_error");


///va+on_ida_error+name on_ida_error
///va+on_ida_error+desc This should be set to a mask that indicates how to proceed if an error ir raised in any
///va+on_ida_error+desc routine of the SUNDIALS IDA Library. 
  wasora_special_var(on_ida_error) = wasora_define_variable("on_ida_error");

  dummy = wasora_define_variable("quit");
  wasora_var_value(dummy) = 0;
  
  dummy = wasora_define_variable("report");
  wasora_var_value(dummy) = 0;

  dummy = wasora_define_variable("dont_quit");
  wasora_var_value(dummy) = ON_ERROR_NO_QUIT;
  
  dummy = wasora_define_variable("dont_report");
  wasora_var_value(dummy) = ON_ERROR_NO_REPORT;

  
///va+realtime_scale+name realtime_scale
///va+realtime_scale+desc If this variable is not zero, then the transient problem is run trying to syncrhonize the
///va+realtime_scale+desc problem time with realtime, up to a scale given. For example, if the scale is set to one, then
///va+realtime_scale+desc wasora will advance the problem time at the same pace that the real wall time advances. If set to
///va+realtime_scale+desc two, wasora's time wil advance twice as fast as real time, and so on. If the calculation time is
///va+realtime_scale+desc slower than real time modified by the scale, this variable has no effect on the overall behavior
///va+realtime_scale+desc and execution will proceed as quick as possible with no delays.
  wasora_special_var(realtime_scale) = wasora_define_variable("realtime_scale");

  // files 
  wasora.special_files.stdin_ = wasora_define_file("stdin", "stdin", 0, NULL, "r", 0);
  wasora.special_files.stdin_->pointer = stdin;

  wasora.special_files.stdout_ = wasora_define_file("stdout", "stdout", 0, NULL, "w", 0);
  wasora.special_files.stdout_->pointer = stdout;

  wasora.special_files.stderr_ = wasora_define_file("stderr", "stderr", 0, NULL, "w", 0);
  wasora.special_files.stderr_->pointer = stderr;

  // el framework de mallas
  if (!wasora_mesh.initialized) {
    wasora_call(wasora_mesh_init_before_parser());
  }
*/
  return FEENOX_OK;

}

// esta se llama despues del parser, hay que verificar que everything este manzana
// y pedir shared memory objecst y esas cosas
int feenox_init_after_parser(void) {

/*  
  int i;
  
  if (wasora.active_conditional_block != NULL) {
    wasora_push_error_message("conditional block not closed");
    return WASORA_RUNTIME_ERROR;
  }
    
  
#ifdef HAVE_READLINE
  // autocompletion
  rl_attempted_completion_function = wasora_rl_completion;
#endif
  
  // inicializamos plugins
  for (i = 0; i < wasora.i_plugin; i++) {
    if (wasora.plugin[i].init_after_parser() != 0) {
      wasora_push_error_message("%s:", wasora.plugin[i].name_string);
      return WASORA_RUNTIME_ERROR;
    }
  }
  
  // algunas cosas (por ejempo inteprolacion de funciones) se inicializan bajo
  // demanda, i.e. la primera vez que se necesita evaluar una funcion interpolada
  // se hace el init en tiempo de ejecucion
  
  return WASORA_RUNTIME_OK;
}

// esta se ejecuta antes de cada corrida, hay que resetear el tiempo, borrar historias, etc
int wasora_init_before_run(void) {
  int i;
  history_t *history;
  print_t *print;
  file_t *file;
  
  wasora_var(wasora_special_var(step_static)) = 0;
  wasora_var(wasora_special_var(step_transient)) = 0;
  wasora_var(wasora_special_var(time)) = 0;
  wasora_var(wasora_special_var(dt)) = 1.0/16.0;
  wasora.current_time_path = &wasora.time_path[0];
  
  wasora_var(wasora_special_var(done_static)) = 0;
  wasora_var(wasora_special_var(done_transient)) = 0;
  wasora_var(wasora_special_var(done)) = 0;
  
#ifdef HAVE_IDA
  // el cuento es asi: cuando hacemos estudios parametricos, las condiciones
  // iniciales se vuelven a calcular porque ponemos t = 0
  // pero las derivadas valen lo ultimo que valian al terminar el paso anterior
  // asi que a menos que querramos hacer estudios adiabaticos (TODO!)
  // volvemos a poner las derivadas en cero
  // si el usuario da explicitamente los valores de las derivas, es lo mismo
  // que las condiciones iniciales asi que todo manzana
  if (wasora_dae.phase_derivative != NULL) {
    for (i = 0; i < wasora_dae.dimension; i++) {
      *(wasora_dae.phase_value[i]) = 0;
      *(wasora_dae.phase_derivative[i]) = 0;
    }
  }
  
  if (wasora_dae.system != NULL) {
    IDAFree(&wasora_dae.system);
    wasora_dae.system = NULL;
  }
    
  if (wasora_dae.x != NULL) {
    N_VDestroy_Serial(wasora_dae.x);
    wasora_dae.x = NULL;
  }
  if (wasora_dae.dxdt != NULL) {
    N_VDestroy_Serial(wasora_dae.dxdt);
    wasora_dae.dxdt = NULL;
  }
  if (wasora_dae.id != NULL) {
    N_VDestroy_Serial(wasora_dae.id);
    wasora_dae.id = NULL;
  }  
#endif  
  
  LL_FOREACH(wasora.histories, history) {
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

  LL_FOREACH(wasora.prints, print) {
    print->last_static_step = 0;
    print->last_step = 0;
    print->last_time = 0;
    print->last_header_step = 0;
    print->header_already_printed = 0;
  }  

  // si no estamos en parametrico o en optimizacion cerramos los files
  // TODO: marcar los files como global o algo asi
  if (wasora.parametric.outer_steps == 0 && wasora.min.n == 0) {
    for (file = wasora.files; file != NULL; file = file->hh.next) {
      if (file->pointer != NULL && fileno(file->pointer) > 2) {
        wasora_instruction_close_file(file);
      }
    }
  }  

  for (i = 0; i < wasora.i_plugin; i++) {
    wasora_call(wasora.plugin[i].init_before_run());
  }
*/
  return FEENOX_OK;
}


/*
// mira si los n puntos en x[] y en y[] forman una grilla estruturada
int wasora_is_structured_grid_2d(double *x, double *y, int n, int *nx, int *ny) {

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
int wasora_is_structured_grid_3d(double *x, double *y, double *z, int n, int *nx, int *ny, int *nz) {

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