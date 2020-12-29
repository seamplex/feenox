/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX common framework header
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

#ifndef FEENOX_H
#define FEENOX_H

#define HAVE_INLINE
#define GSL_RANGE_CHECK_OFF

// we need all the includes here so they all follow the inline directive above
#include <gsl/gsl_blas.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_deriv.h>
#include <gsl/gsl_heapsort.h>
#include <gsl/gsl_integration.h>
#include <gsl/gsl_interp.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_min.h>
#include <gsl/gsl_multifit_nlin.h>
#include <gsl/gsl_multimin.h>
#include <gsl/gsl_multiroots.h>
#include <gsl/gsl_qrng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_roots.h>
#include <gsl/gsl_spline.h>
#include <gsl/gsl_sf.h>
#include <gsl/gsl_statistics.h>
#include <gsl/gsl_sort_double.h>
#include <gsl/gsl_sort_vector_double.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_version.h>

#ifdef HAVE_IDA
 #include <ida/ida.h>
 #include <ida/ida_direct.h>
 #include <nvector/nvector_serial.h>
 #include <sundials/sundials_types.h>
 #include <sundials/sundials_math.h>
 #include <sunmatrix/sunmatrix_dense.h>
 #include <sunlinsol/sunlinsol_dense.h>
#endif


#include "contrib/uthash.h"
#include "contrib/utlist.h"


#define FEENOX_OK         0
#define FEENOX_ERROR      1

#define BUFFER_TOKEN_SIZE        255
#define BUFFER_LINE_SIZE        4095


// no son enums porque hacemos operaciones con las mascaras de abajo
#define EXPR_OPERATOR                             0
#define EXPR_CONSTANT                             1
#define EXPR_VARIABLE                             2
#define EXPR_VECTOR                               3
#define EXPR_MATRIX                               4
// old-school numbers
//#define EXPR_NUM                                  5
#define EXPR_BUILTIN_FUNCTION                     6
#define EXPR_BUILTIN_VECTORFUNCTION               7
#define EXPR_BUILTIN_FUNCTIONAL                   8
#define EXPR_FUNCTION                             9

#define EXPR_BASICTYPE_MASK                    1023
#define EXPR_CURRENT                              0
#define EXPR_INITIAL_TRANSIENT                 2048
#define EXPR_INITIAL_STATIC                    4096


enum version_type {
  version_compact,
  version_copyright,
  version_info
};

// macro to check error returns in function calls
#define feenox_call(function)   if ((function) != FEENOX_OK) return FEENOX_ERROR

// macro to access internal special variables
#define feenox_special_var(var) (feenox.special_vars.var)

// value of a variable
#define feenox_var_value(var)  (*(var->value))



// forward type definitions
typedef struct expr_t expr_t;
typedef struct expr_factor_t expr_factor_t;

typedef struct var_t var_t;
typedef struct vector_t vector_t;
typedef struct matrix_t matrix_t;
typedef struct function_t function_t;

typedef struct alias_t alias_t;

typedef struct instruction_t instruction_t;

typedef struct phase_object_t phase_object_t;
typedef struct dae_t dae_t;

typedef struct mesh_t mesh_t;


// individual factor of an algebraic expression
struct expr_factor_t {
  int n_chars;
  int type;        // defines #EXPR_ because we need to operate with masks
  
  int level;       // hierarchical level
  int tmp_level;   // for partial sums

  int oper;        // number of the operator if applicable
  double constant; // value of the numerical constant if applicable
  double value;    // current value
  
// vector with (optional) auxiliary stuff
// (last value, integral accumulator, rng, etc)
  double *aux;

/*
  builtin_function_t *builtin_function;
  builtin_vectorfunction_t *builtin_vectorfunction;
  builtin_functional_t *builtin_functional;
 */
  var_t *variable;
  vector_t *vector;
  matrix_t *matrix;
  function_t *function;

  vector_t **vector_arg;

  var_t *functional_var_arg;

  // algebraic expression of the arguments of the function
  expr_t *arg;

  expr_factor_t *next;
};


// algebraic expression
struct expr_t {
  expr_factor_t *factors;
  double value;
  
  char *string;     // just in case we keep the original string

  expr_t *next;
};



// variable
struct var_t {
  char *name;
  int initialized;
  int realloced;
  
  // these are pointers so the actual holder can be anything, from another
  // variable (i.e. an alias), a vector element (another alias) or even
  // more weird stufff (a fortran common?)
  double *value;
  double *initial_static;
  double *initial_transient;

  UT_hash_handle hh;
};


// vector
struct vector_t {
  char *name;
  int initialized;
  
//  expr_t *size_expr;
  int size;
  int constant;

  gsl_vector *value;
  gsl_vector *initial_transient;
  gsl_vector *initial_static;

  // flag to know if the pointer above is owned by us or by anybody else
  int reallocated;
  
  // pointer to a function where we should get the data from
  function_t *function;
  
  // linked list con las expresiones de datos
//  expr_t *datas;

  UT_hash_handle hh;
};


// matrix
struct matrix_t {
  char *name;
  int initialized;  
  expr_t *cols_expr;
  expr_t *rows_expr;
  
  int cols;
  int rows;
  int constant;
  
  gsl_matrix *value;
  gsl_matrix *initial_transient;
  gsl_matrix *initial_static;

  // flag para saber si el apuntador de arriba lo alocamos nosotros o alguien mas
  int realloced;
  
  expr_t *datas;
  
  UT_hash_handle hh;
};

// alias
struct alias_t {
  int initialized;
  
  var_t *new_variable;
  
  expr_t row;
  expr_t col;
  
  var_t *variable;
  vector_t *vector;
  matrix_t *matrix;
  
  alias_t *next;
};

// function
struct function_t {
  char *name;
  char *name_in_mesh;
  int initialized;
  
  // might be:
  //   - algebraic
  //   - pointwise-defined
  //       + given in the input
  //       + given in a file
  //       + given in wasora vectors
  //       + in a mesh
  //          * as another function defined over materials (physical groups)
  //          * data at nodes
  //          * data at cells
  //  - comptued in a user-provided routine
  //
  enum  {
    function_type_undefined,
    function_type_algebraic,
    function_type_pointwise_data,
    function_type_pointwise_file,
    function_type_pointwise_vector,
    function_type_pointwise_mesh_property,
    function_type_pointwise_mesh_node,
    function_type_pointwise_mesh_cell,
    function_type_routine,
    function_type_routine_internal,
  } type;
  
  // number of arguments the function takes
  int n_arguments;

  // array of pointers to already-existing variables for the arguments
  var_t **var_argument;
  int var_argument_allocated;


  // expression for algebraic functions
  expr_t algebraic_expression;

  // number of pairs of x-y data for pointwise functions
  size_t data_size;

  // arrays with the adata 
  double **data_argument;
  int data_argument_allocated;
  double *data_value;
  
  // this is in case there is a derivative of a mesh-based function and
  // we want to interoplate with the shape functions
  // warning! we need to put data on the original function and not on the derivatives
  function_t *spatial_derivative_of;
  int spatial_derivative_with_respect_to;

  // hints in case the grid is tensor-product regular
  int rectangular_mesh;
  expr_t expr_x_increases_first;
  int x_increases_first;
  expr_t *expr_rectangular_mesh_size;
  int *rectangular_mesh_size;
  double **rectangular_mesh_point;

  // file with the point-wise data
  char *data_file_path;

  // columns where the data is within the file
  // size = n_arguments+1 (n independent variables + 1 dependent variable) 
  int *column;

  // vectors with the point-wise data already read
  vector_t **vector_argument;
  vector_t *vector_value;
  
  // helpers to interpolate 1D with GSL
  gsl_interp *interp;
  gsl_interp_accel *interp_accel;
  gsl_interp_type interp_type;

  // multidimensional interpolation type
  enum {
    interp_nearest,
    interp_shepard,
    interp_shepard_kd,
    interp_bilinear
  } multidim_interp;

  
  expr_t expr_multidim_threshold;
  double multidim_threshold;
  expr_t expr_shepard_radius;
  double shepard_radius;
  expr_t expr_shepard_exponent;
  double shepard_exponent;

  // propiedad
  void *property;

  // malla no-estructurada sobre la que esta definida la funcion
  mesh_t *mesh;
  double mesh_time;
  
  // apuntador a un arbol k-dimensional para nearest neighbors 
  void *kd;
  
  // ----- ------- -----------   --        -       - 
  // funcion que hay que llamar para funciones tipo usercall 
  double (*routine)(const double *);
  
  // ----- ------- -----------   --        -       - 
  // funcion que hay que llamar para funciones internas
  double (*routine_internal)(const double *, function_t *);
  void *params;

  UT_hash_handle hh;
};



// instruction
struct instruction_t {
  int (*routine)(void *);
  void *argument;
  int argument_alloced;

  instruction_t *next;
};


struct phase_object_t {
  int offset;
  int size;
  int differential;
  char *name;

  var_t *variable;
  var_t *variable_dot;
  vector_t *vector;
  vector_t *vector_dot;
  matrix_t *matrix;
  matrix_t *matrix_dot;

  phase_object_t *next;
};

struct dae_t {
  expr_t residual;
  
  vector_t *vector;
  matrix_t *matrix;
  
  expr_t expr_i_min;
  expr_t expr_i_max;
  expr_t expr_j_min;
  expr_t expr_j_max;
  
  int i_min;
  int i_max;
  int j_min;
  int j_max;
  
  int equation_type;
  
  dae_t *next;
};

// global FeenoX singleton structure
struct {
  int argc;
  char **argv;
  int optind;

/*  
  int argc_orig;       // copy of the original arguments before calling getopt()
  char **argv_orig;

  int argc_unknown;
  char **argv_unknown;
*/
  char *main_input_filepath;
  char *main_input_dirname;
  
  enum {
    mode_normal,
    mode_ignore_debug,
    mode_debug,
    mode_single_step,
    mode_list_vars
  } mode;
  
  int rank;            // in serial or without petsc, this is always zero
  int nprocs;

  char **error;
  int error_level;
  
  expr_t *time_paths;
  expr_t *time_path_current;
  
  instruction_t *instructions;
//  instruction_t *last_defined_instruction;
  
  var_t *vars;
  vector_t *vectors;
  matrix_t *matrices;
  function_t *functions;
  
  struct {
    var_t *done;
    var_t *done_static;
    var_t *done_transient;
    var_t *done_outer;
  
    var_t *step_outer;
//    var_t *step_inner;
    var_t *step_static;
    var_t *step_transient;

    var_t *in_outer_initial;
    var_t *in_static;
    var_t *in_static_first;
    var_t *in_static_last;
    var_t *in_transient;
    var_t *in_transient_first;
    var_t *in_transient_last;

    var_t *static_steps;
    var_t *end_time;

    var_t *t;
    var_t *dt;

    var_t *rel_error;
    var_t *min_dt;
    var_t *max_dt;
  
    var_t *i;
    var_t *j;
    
    var_t *pi;
    var_t *zero;
    var_t *infinite;

//    var_t *ncores;
//    var_t *pid;
    
    var_t *on_nan;
    var_t *on_gsl_error;
    var_t *on_ida_error;
    var_t *realtime_scale;
  } special_vars;

/*	
  struct {
    vector_t *abs_error;
  } special_vectors;

  struct {
    file_t *stdin_;
    file_t *stdout_;
    file_t *stderr_;
  } special_files;  
*/  
  struct {
    int dimension;
    double **phase_value;
    double **phase_derivative;
    phase_object_t *phase_objects;
  
    dae_t *daes;
    void *system;

    enum {
      initial_conditions_as_provided,
      initial_conditions_from_variables,
      initial_conditions_from_derivatives,
    } initial_conditions_mode;

    instruction_t *instruction;

#if HAVE_IDA
    N_Vector x;
    N_Vector dxdt;
    N_Vector id;
    N_Vector abs_error;
    SUNMatrix A;
    SUNLinearSolver LS;
#endif
  } dae;
  
} feenox;


// function declarations

// init.c
extern int feenox_initialize(int argc, char **argv);
extern int feenox_init_special_objects(void);
extern int feenox_init_after_parser(void);


// version.c
extern void feenox_show_help(const char *progname);
extern void feenox_show_version(int version);
extern void feenox_copyright(void);
extern void feenox_shortversion(void);
extern void feenox_longversion(void);


// error.c
void feenox_push_error_message(const char *fmt, ...);
void feenox_pop_error_message(void);
void feenox_pop_errors(void);
void feenox_runtime_error(void);
void feenox_nan_error(void);
void feenox_gsl_handler (const char *reason, const char *file_ptr, int line, int gsl_errno);
void feenox_signal_handler(int sig_num);


// cleanup.c
void feenox_polite_exit(int error);
void feenox_finalize(void);


// parser.c
extern int feenox_parse_main_input_file(const char *filepath);
extern int feenox_parse_expression(const char *string, expr_t *expr);


// file.c
FILE *feenox_fopen(const char *filepath, const char *mode);


// abort.c
extern int feenox_instruction_abort(void *arg);


// algebra.c
extern double feenox_evaluate_expression_in_string(const char *string);


// dae.c
extern int feenox_add_time_path(const char *token);


// instruction.c
extern int feenox_add_instruction(int (*routine)(void *), void *argument);


// define.c
extern int feenox_check_name(const char *name);
extern var_t *feenox_define_variable(char *name);
  

// getptr.c
extern var_t *feenox_get_variable_ptr(const char *name);


#endif