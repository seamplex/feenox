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

#if HAVE_IDA
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
#define FEENOX_UNHANDLED  2

#define BUFFER_TOKEN_SIZE        255
#define BUFFER_LINE_SIZE        4095


// no son enums porque hacemos operaciones con las mascaras de abajo
#define EXPR_UNDEFINED                            0 
#define EXPR_OPERATOR                             1
#define EXPR_CONSTANT                             2
#define EXPR_VARIABLE                             3
#define EXPR_VECTOR                               4
#define EXPR_MATRIX                               5
#define EXPR_BUILTIN_FUNCTION                     6
#define EXPR_BUILTIN_VECTORFUNCTION               7
#define EXPR_BUILTIN_FUNCTIONAL                   8
#define EXPR_FUNCTION                             9

#define EXPR_BASICTYPE_MASK                    1023
#define EXPR_CURRENT                              0
#define EXPR_INITIAL_TRANSIENT                 2048
#define EXPR_INITIAL_STATIC                    4096

// constants for custom error handling
#define ON_ERROR_NO_QUIT            1
#define ON_ERROR_NO_REPORT          2


// type of phase space componentes (the values come from SUNDIALs, do not change!)
#define DAE_ALGEBRAIC                      0.0
#define DAE_DIFFERENTIAL                   1.0


#define STEP_ALL                           0
#define STEP_BEFORE_DAE                    1
#define STEP_AFTER_DAE                     3


// reasonable defaults
#define DEFAULT_DT                         1.0/16.0
#define DEFAULT_REL_ERROR                  1e-6
#define DEFAULT_RANDOM_METHOD              gsl_rng_mt19937

#define DEFAULT_PRINT_FORMAT               "%g"
#define DEFAULT_PRINT_SEPARATOR            "\t"

#define DEFAULT_ROOT_MAX_TER               1024
#define DEFAULT_ROOT_TOLERANCE             (9.765625e-4)         // (1/2)^-10

#define DEFAULT_INTEGRATION_INTERVALS      1024
#define DEFAULT_INTEGRATION_TOLERANCE      (9.765625e-4)         // (1/2)^-10
#define DEFAULT_INTEGRATION_KEY            GSL_INTEG_GAUSS31

#define DEFAULT_DERIVATIVE_STEP            (9.765625e-4)         // (1/2)^-10

#define MINMAX_ARGS       10


// zero & infinite
#define ZERO          (8.881784197001252323389053344727e-16)  // (1/2)^-50
#define INFTY         (1125899906842624.0)                    // 2^50

#define M_SQRT5 2.23606797749978969640917366873127623544061835961152572427089


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
#define feenox_var_value(var)   (*(var->value))

// value of a special variable
#define feenox_special_var_value(var)  feenox_var_value(feenox_special_var(var))

// pointer to the content of an object
#define feenox_value_ptr(obj)  (obj->value)

// check return value of alloc/calloc
#define feenox_check_alloc(a,b)  if ((a) == NULL) { feenox_push_error_message("memory allocation error"); return b; }



// number of internal functions, functional adn vector functions
#define N_BUILTIN_FUNCTIONS         53
#define N_BUILTIN_FUNCTIONALS       8
#define N_BUILTIN_VECTOR_FUNCTIONS  8



// forward declarations
typedef struct feenox_t feenox_t;
typedef struct builtin_function_t builtin_function_t;
typedef struct builtin_functional_t builtin_functional_t;
typedef struct builtin_vectorfunction_t builtin_vectorfunction_t;

typedef struct expr_t expr_t;
typedef struct expr_item_t expr_item_t;

typedef struct var_t var_t;
typedef struct vector_t vector_t;
typedef struct matrix_t matrix_t;
typedef struct function_t function_t;

typedef struct alias_t alias_t;

typedef struct instruction_t instruction_t;
typedef struct conditional_block_t conditional_block_t;

typedef struct file_t file_t;
typedef struct print_t print_t;
typedef struct print_token_t print_token_t;
typedef struct print_function_t print_function_t;
typedef struct multidim_range_t multidim_range_t;

typedef struct sort_vector_t sort_vector_t;
typedef struct phase_object_t phase_object_t;
typedef struct dae_t dae_t;

typedef struct mesh_t mesh_t;


// individual factor of an algebraic expression
struct expr_item_t {
  size_t n_chars;
  int type;        // defines #EXPR_ because we need to operate with masks
  
  size_t level;       // hierarchical level
  size_t tmp_level;   // for partial sums

  size_t oper;        // number of the operator if applicable
  double constant; // value of the numerical constant if applicable
  double value;    // current value
  
// vector with (optional) auxiliary stuff
// (last value, integral accumulator, rng, etc)
  double *aux;

  builtin_function_t *builtin_function;
  builtin_vectorfunction_t *builtin_vectorfunction;
  builtin_functional_t *builtin_functional;

  var_t *variable;
  vector_t *vector;
  matrix_t *matrix;
  function_t *function;

  vector_t **vector_arg;

  var_t *functional_var_arg;

  // algebraic expression of the arguments of the function
  expr_t *arg;

  expr_item_t *next;
};


// algebraic expression
struct expr_t {
  expr_item_t *items;
  double value;
  char *string;     // just in case we keep the original string
  expr_t *next;
};



// variable
struct var_t {
  char *name;
  int initialized;
  int reallocated;
  
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
  
  expr_t size_expr;
  int size;
  int constant;

  gsl_vector *value;
  gsl_vector *initial_transient;
  gsl_vector *initial_static;

  // flag to know if the pointer above is owned by us or by anybody else
  int reallocated;
  
  // pointer to a function where we should get the data from
  function_t *function;
  
  // linked list with the expressions of the initial elements
  expr_t *datas;

  UT_hash_handle hh;
};


// matrix
struct matrix_t {
  char *name;
  int initialized;  
  expr_t cols_expr;
  expr_t rows_expr;
  
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
  int n_arguments_given;  // check to see if the API gave us all the arguments the function needs

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


// internal function (this is initialized directly into the data space from builtin_functions.h)
struct builtin_function_t {
  char name[BUFFER_TOKEN_SIZE];
  int min_arguments;
  int max_arguments;
  double (*routine)(struct expr_item_t *);
};


// internal function over vectors
struct builtin_vectorfunction_t {
  char name[BUFFER_TOKEN_SIZE];
  int min_arguments;
  int max_arguments;
  double (*routine)(struct vector_t **);
};


// internal functional
// a functional is _like_ a regular function but its second argument is a variable
// (not an expression) that is used to evaluate the first argument (which is an expression
// of this variable) for different values and do something about it, for instance
// compute a derivative or an integral.
// There can also be more optional arguments (expressions) after this second special argument,
// i.e. integral(1+x,x,0,1) or derivative(1+x,x,0.5)
struct builtin_functional_t {
  char name[BUFFER_TOKEN_SIZE];
  int min_arguments; // contando la variable del segundo argumento
  int max_arguments;
  double (*routine)(struct expr_item_t *, struct var_t *);
};


// instruction
struct instruction_t {
  int (*routine)(void *);
  void *argument;
  int argument_alloced;

  instruction_t *next;
};


// conditional block
struct conditional_block_t {
  conditional_block_t *father;
  conditional_block_t *else_of;
  
  expr_t condition;
  
  instruction_t *first_true_instruction;
  instruction_t *first_false_instruction;
  
  int evaluated_to_true;
  int evaluated_to_false;
  
  conditional_block_t *next;
};


// a file, either input or output
struct file_t {
  char *name;
  int initialized;
  
  char *format;
  int n_format_args;
  int n_format_args_given;
  expr_t *arg;
  char *mode;

  char *path;
  FILE *pointer;

  UT_hash_handle hh;
};


// print a single line
struct print_t {
  // pointer to the output file (if null, print to stdout)
  file_t *file;

  // a linked list with the stuff to print
  print_token_t *tokens;
 
  // token separator, by default it is a tab "\t" 
  char *separator;

  // flag to indicate if we need to send the newline char "\n" at the end
  // it's called nonewline so the default is zero and the "\n" is sent
  int nonewline;

  // stuff to help with the SKIP_*
  int last_static_step;
  int last_step;
  double last_time;

  expr_t skip_static_step;
  expr_t skip_step;
  expr_t skip_time;

  int header;
  expr_t skip_header_step;
  int last_header_step;
  int header_already_printed;
  
  print_t *next;
};


// an individual token to be PRINTed
struct print_token_t {
  char *format;
  char *text;
  expr_t expression;
  
  vector_t *vector;
  matrix_t *matrix;
  function_t *function;
  
  print_token_t *next;
};

struct multidim_range_t {
  int dimensions;

  expr_t *min;
  expr_t *max;
  expr_t *step;
  expr_t *nsteps;
};


// print one or more functions
struct print_function_t {
  file_t *file;

  // linked list with the stuff to be printed
  print_token_t *tokens;

  // pointer to the first function (the one that defines the number of arguments
  // note that this might not be the same as as token->function, that can be NULL
  function_t *first_function;

  // explicit range to print the function
  multidim_range_t range;
  
  // mesh and physical entity that tells where to print a function
//  mesh_t *mesh;
//  physical_entity_t *physical_entity;

  // flag to add a header explaining what the columns are
  int header;
  // formato de los numeritos "%e"
  char *format;
  // separador de cosas "\t"
  char *separator;

  print_function_t *next;

};


struct sort_vector_t {
  int descending;
  
  vector_t *v1;
  vector_t *v2;
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
struct feenox_t {
  int argc;
  char **argv;
  int optind;

  char *main_input_filepath;
  char *main_input_dirname;
  
  enum {
    mode_normal,
    mode_parametric,
    mode_optimize,
    mode_fit,
    mode_list_vars
  } mode;
  
  // pointer to the run function (normal, parametric, optimize, fit, etc)
  int (*run)(void);
  
  int debug;
  
  int rank;            // in serial or without petsc, this is always zero
  int nprocs;

  char **error;
  int error_level;
  
  // if this is not null then a conditional is wanting us to branch
  instruction_t *next_instruction;
  
  expr_t *time_paths;
  expr_t *time_path_current;
  
  instruction_t *instructions;
  instruction_t *last_defined_instruction;  // TODO: see if we can put this somewhere else
  
  conditional_block_t *conditional_blocks;
  conditional_block_t *active_conditional_block;
  
  var_t *vars;
  vector_t *vectors;
  matrix_t *matrices;
  alias_t *aliases;
  function_t *functions;
  
  file_t *files;
  print_t *prints;
  print_function_t *print_functions;
  
  struct {
    var_t *done;
    var_t *done_static;
    var_t *done_transient;
    var_t *done_outer;
  
    var_t *step_outer;
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

    var_t *ncores;
    var_t *pid;
    
    var_t *on_nan;
    var_t *on_gsl_error;
    var_t *on_ida_error;
    var_t *realtime_scale;
  } special_vars;


  struct {
    vector_t *abs_error;
  } special_vectors;

  
  struct {
//    file_t *stdin_;
    file_t *stdout_;
//    file_t *stderr_;
  } special_files;  
  
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

};


// function declarations

// run_standard.c
int feenox_run_standard(void);

// step.c
int feenox_step(int whence);

// init.c
extern int feenox_initialize(int argc, char **argv);
extern int feenox_init_special_objects(void);
extern int feenox_init_after_parser(void);
extern int feenox_init_before_run(void);


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
extern char *feenox_ends_in_init(char *name);
extern char *feenox_ends_in_zero(char *name);
extern int feenox_count_arguments(char *string, size_t *);
extern int feenox_read_arguments(char *string, int n_arguments, char ***arg, size_t *n_chars);


// file.c
extern int feenox_instruction_file(void *arg);
FILE *feenox_fopen(const char *filepath, const char *mode);
extern int feenox_instruction_file_open(void *arg);
extern int feenox_instruction_file_close(void *arg);

// abort.c
extern int feenox_instruction_abort(void *arg);


// expressions.c
extern int feenox_expression_parse(expr_t *this, const char *string);
extern expr_item_t *feenox_expression_parse_item(const char *string);


extern double feenox_expression_eval(expr_t *this);
extern double feenox_evaluate_expression_in_string(const char *string);

// dae.c
extern int feenox_add_time_path(const char *token);
extern int feenox_dae_init(void);
extern int feenox_dae_ic(void);
#ifdef HAVE_IDA
extern int feenox_ida_dae(realtype t, N_Vector yy, N_Vector yp, N_Vector rr, void *params);
#else
extern int feenox_ida_dae(void);
#endif

// instruction.c
extern instruction_t *feenox_add_instruction_and_get_ptr(int (*routine)(void *), void *argument);
extern int feenox_add_instruction(int (*routine)(void *), void *argument);


// define.c
extern int feenox_check_name(const char *name);
extern int feenox_define_variable(const char *name);
extern int feenox_define_alias(const char *new_name, const char *existing_object, const char *row, const char *col);

extern int feenox_define_vector(const char *name, const char *size);
extern int feenox_vector_attach_function(const char *name, const char *function_data);
extern int feenox_vector_attach_data(const char *name, expr_t *datas);

extern int feenox_define_matrix(const char *name, const char *rows, const char *cols);
extern int feenox_matrix_attach_data(const char *name, expr_t *datas);

extern int feenox_define_function(const char *name, int n_arguments);
extern int feenox_function_set_argument_variable(const char *name, int i, const char *variable_name);
extern int feenox_function_set_expression(const char *name, const char *expression);

extern int feenox_define_file(const char *name, const char *format, int n_args, const char *mode);
extern int feenox_file_set_path_argument(const char *name, int i, const char *expression);

extern var_t *feenox_get_or_define_variable_ptr(const char *name);
extern var_t *feenox_define_variable_ptr(const char *name);

extern void feenox_realloc_variable_ptr(var_t *this, double *newptr, int copy_contents);
extern void feenox_realloc_vector_ptr(vector_t *this, double *newptr, int copy_contents);

// getptr.c
extern var_t *feenox_get_variable_ptr(const char *name);
extern vector_t *feenox_get_vector_ptr(const char *name);
extern matrix_t *feenox_get_matrix_ptr(const char *name);
extern function_t *feenox_get_function_ptr(const char *name);
extern builtin_function_t *feenox_get_builtin_function_ptr(const char *name);
extern builtin_vectorfunction_t *feenox_get_builtin_vectorfunction_ptr(const char *name);
extern builtin_functional_t *feenox_get_builtin_functional_ptr(const char *name);

extern file_t *feenox_get_file_ptr(const char *name);

// alias.c
extern int feenox_instruction_alias(void *arg);

// matrix.c
extern int feenox_matrix_init(matrix_t *this);

// vector.c
extern int feenox_vector_init(vector_t *this);
extern int feenox_instruction_sort_vector(void *arg);
extern double feenox_vector_get(vector_t *this, const size_t i);
extern double feenox_vector_get_initial_static(vector_t *this, const size_t i);
extern double feenox_vector_get_initial_transient(vector_t *this, const size_t i);


// function.c
extern int feenox_function_init(function_t *this);
extern void feenox_set_function_args(function_t *this, double *x);
extern double feenox_function_eval(function_t *this, const double *x);
extern double feenox_factor_function_eval(expr_item_t *this);

// print.c
extern int feenox_instruction_print(void *arg);
extern int feenox_instruction_print_function(void *arg);

// conditional.c
extern int feenox_instruction_if(void *arg);
extern int feenox_instruction_else(void *arg);
extern int feenox_instruction_endif(void *arg);

// functionals.c
extern double feenox_gsl_function(double x, void *params);

#endif    /* FEENOX_H  */
