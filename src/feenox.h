/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX common framework header
 *
 *  Copyright (C) 2009--2025 Jeremy Theler
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// for POSIX in C99
#ifndef _GNU_SOURCE   
#define _GNU_SOURCE   
#endif

#if defined(HAVE___BUILTIN_EXPECT)
#  define feenox_unlikely(cond)   __builtin_expect(!!(cond),0)
#  define feenox_likely(cond)     __builtin_expect(!!(cond),1)
#else
#  define feenox_unlikely(cond)   (cond)
#  define feenox_likely(cond)     (cond)
#endif

// for inlining as much as possible GSL
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
#include <gsl/gsl_multifit_nlinear.h>
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

#ifdef HAVE_SUNDIALS
 #include <ida/ida.h>
 // #include <ida/ida_direct.h>
 #include <nvector/nvector_serial.h>
 #include <sundials/sundials_types.h>
 #include <sundials/sundials_math.h>
 #include <sunmatrix/sunmatrix_dense.h>
 #include <sunlinsol/sunlinsol_dense.h>
 
 #ifndef sunrealtype 
  #define sunrealty realtype
 #endif
#endif

// petsc complains if its headers are included from c++ within extern C
#ifdef HAVE_PETSC
#ifdef __cplusplus
extern "C++" {
#endif  // C++
 #include <petscsys.h>
 #include <petscpc.h>
 #include <petscsnes.h>
 #include <petscts.h>
 #include <petsctime.h>
 #ifdef HAVE_SLEPC
  #include <slepceps.h>
 #endif // HAVE_SLEPC
#ifdef __cplusplus
}
#endif  // C++

// PETSC_NULL is deprecated but it was not defined before 3.16
#if PETSC_VERSION_LT(3,16,0)
#define PETSC_NULLPTR  PETSC_NULL
#endif

#endif  // HAVE_PETSC


#include "contrib/uthash.h"
#include "contrib/utlist.h"
#include "contrib/kdtree.h"


#define FEENOX_OK         0
#define FEENOX_ERROR      1
#define FEENOX_UNHANDLED  2

#define FEENOX_VECTOR_INITIAL    0
#define FEENOX_VECTOR_NO_INITIAL 1

#define BUFFER_TOKEN_SIZE        256
#define BUFFER_LINE_SIZE        4096

// number of internal functions, functional and vector functions
#define N_BUILTIN_FUNCTIONS         70
#define N_BUILTIN_FUNCTIONALS       8
#define N_BUILTIN_VECTOR_FUNCTIONS  8

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

// reasonable defaults
#define DEFAULT_DT                         1.0/16.0
#define DEFAULT_DAE_RTOL                   1e-6
#define DEFAULT_RANDOM_METHOD              gsl_rng_mt19937

#define DEFAULT_PRINT_FORMAT               "%g"
#define DEFAULT_PRINT_SEPARATOR            "\t"

#define CHAR_PROGRESS_BUILD                "."
#define CHAR_PROGRESS_SOLVE                "-"
#define CHAR_PROGRESS_GRADIENT             "="


#define DEFAULT_ROOT_MAX_TER               1024
#define DEFAULT_ROOT_TOLERANCE             (9.765625e-4)         // (1/2)^-10

#define DEFAULT_INTEGRATION_INTERVALS      1024
#define DEFAULT_INTEGRATION_TOLERANCE      (9.765625e-4)         // (1/2)^-10
#define DEFAULT_INTEGRATION_KEY            GSL_INTEG_GAUSS31

#define DEFAULT_DERIVATIVE_STEP            (9.765625e-4)         // (1/2)^-10
#define DEFAULT_CONDITION_THRESHOLD        (9.765625e-4)         // (1/2)^-10

#define DEFAULT_FIT_MAX_ITER               100
#define DEFAULT_FIT_XTOL                   1e-8
#define DEFAULT_FIT_GTOL                   1e-8
#define DEFAULT_FIT_FTOL                   0.0

#define DEFAULT_SOLVE_METHOD               gsl_multiroot_fsolver_dnewton
#define DEFAULT_SOLVE_EPSREL               0   // zero means do not look for deltas in derivatives
#define DEFAULT_SOLVE_EPSABS               1e-6
#define DEFAULT_SOLVE_MAX_ITER             128


#define MINMAX_ARGS           10

// zero & infinite
#define ZERO          (8.881784197001252323389053344727e-16)  // (1/2)^-50
#define INFTY         (1125899906842624.0)                    // 2^50

#define MESH_INF 1e22
#define MESH_TOL 1e-6
#define MESH_FAILED_INTERPOLATION_FACTOR 1.5

#define DEFAULT_INTERPOLATION              (*gsl_interp_linear)
#define DEFAULT_MULTIDIM_INTERPOLATION_THRESHOLD   9.5367431640625e-07 // (1/2)^-20
#define DEFAULT_SHEPARD_RADIUS                     1.0
#define DEFAULT_SHEPARD_EXPONENT                   2


// these are Gmsh's nomenclature, we then convert to vtk/frd with "tables"
#define ELEMENT_TYPE_UNDEFINED      0
#define ELEMENT_TYPE_LINE2          1
#define ELEMENT_TYPE_TRIANGLE3      2
#define ELEMENT_TYPE_QUADRANGLE4    3
#define ELEMENT_TYPE_TETRAHEDRON4   4
#define ELEMENT_TYPE_HEXAHEDRON8    5
#define ELEMENT_TYPE_PRISM6         6
#define ELEMENT_TYPE_PYRAMID5       7
#define ELEMENT_TYPE_LINE3          8
#define ELEMENT_TYPE_TRIANGLE6      9
#define ELEMENT_TYPE_QUADRANGLE9    10
#define ELEMENT_TYPE_TETRAHEDRON10  11
#define ELEMENT_TYPE_HEXAHEDRON27   12
#define ELEMENT_TYPE_PRISM18        13
#define ELEMENT_TYPE_PYRAMID14      14
#define ELEMENT_TYPE_POINT1         15
#define ELEMENT_TYPE_QUADRANGLE8    16
#define ELEMENT_TYPE_HEXAHEDRON20   17
#define ELEMENT_TYPE_PRISM15        18
#define NUMBER_ELEMENT_TYPE         19

#define M_SQRT5 2.23606797749978969640917366873127623544061835961152572427089

#define FEENOX_SOLUTION_NOT_GRADIENT  0
#define FEENOX_SOLUTION_GRADIENT      1

#define feenox_distribution_define_mandatory(type, name, quoted_name, description) {\
  feenox_call(feenox_distribution_init(&(type.name), quoted_name)); \
  if (type.name.defined == 0) { feenox_push_error_message("undefined %s '%s'", description, quoted_name);  return FEENOX_ERROR; } \
  if (type.name.full == 0) { feenox_push_error_message("%s '%s' is not defined over all volumes", description, quoted_name); return FEENOX_ERROR; } }

enum version_type {
  version_compact,
  version_copyright,
  version_info,
  version_available_pdes,
  version_elements_info,
};

// macro to check error returns in function calls
#define feenox_call(function)           if (__builtin_expect((function), FEENOX_OK) != FEENOX_OK) return FEENOX_ERROR
#define feenox_call_null(function)      if (__builtin_expect((function), FEENOX_OK) != FEENOX_OK) return NULL
#define feenox_check_null(function)     if ((function) == NULL) return FEENOX_ERROR
#ifdef HAVE_SUNDIALS
 #define ida_call(function)             if ((err = __builtin_expect(!!(function), FEENOX_OK)) < 0) { feenox_push_error_message("IDA returned error %d", err); return FEENOX_ERROR; }
#endif

#ifdef HAVE_PETSC
 #define petsc_call(function)           if (__builtin_expect((function), 0)) { feenox_push_error_message("PETSc error"); CHKERRQ(1); }
 #define petsc_call_null(function)      if (__builtin_expect((function), 0)) { feenox_push_error_message("PETSc error"); return NULL; }

#endif

#define feenox_free(p)                  free(p); p = NULL;
 
// macro to check malloc() for NULL (ass means assignement, you ass!)
#define feenox_check_alloc(ass)         if ((ass) == NULL) { feenox_push_error_message("cannot allocate memory"); return FEENOX_ERROR; }
#define feenox_check_alloc_null(ass)    if ((ass) == NULL) { feenox_push_error_message("cannot allocate memory"); return NULL; }
#define feenox_check_minusone(ass)      if ((ass) == -1) { feenox_push_error_message("cannot allocate memory"); return FEENOX_ERROR; }
#define feenox_check_minusone_null(ass) if ((ass) == -1) { feenox_push_error_message("cannot allocate memory"); return NULL; }

// macro to access internal special variables
#define feenox_special_var(var) (feenox.special_vars.var)

// value of a variable
#define feenox_var_value(var)   (*(var->value))

// value of a special variable
#define feenox_special_var_value(var)  feenox_var_value(feenox_special_var(var))

// pointer to the content of an object
#define feenox_value_ptr(obj)  (obj->value)

// GSL's equivalent to PETSc's ADD_VALUES
#define gsl_vector_add_to_element(vector,i,x)    gsl_vector_set((vector),(i),gsl_vector_get((vector),(i))+(x))
#define gsl_matrix_add_to_element(matrix,i,j,x)  gsl_matrix_set((matrix),(i),(j),gsl_matrix_get((matrix),(i),(j))+(x))

#define is_element_local(msh, elm) \
((msh->mpi_matches_partitions == mpi_matches_partitions_serial) || \
 (msh->mpi_matches_partitions == mpi_matches_partitions_one_to_one && \
   (e->geometrical_entity != NULL && \
    e->geometrical_entity->partition != NULL && \
    (e->geometrical_entity->partition[0]-1) == feenox.mpi_rank)) || \
 (msh->mpi_matches_partitions == mpi_matches_partitions_one_to_many && \
   (e->geometrical_entity != NULL && \
    e->geometrical_entity->partition != NULL && \
    (((e->geometrical_entity->partition[0]-1)*feenox.mpi_size)/msh->n_partitions) == feenox.mpi_rank)) || \
 (msh->mpi_matches_partitions == mpi_matches_partitions_no && \
    (e->index >= msh->first_element && e->index < msh->last_element)))




// forward declarations
typedef struct feenox_t feenox_t;
typedef struct builtin_function_t builtin_function_t;
typedef struct builtin_functional_t builtin_functional_t;
typedef struct builtin_vectorfunction_t builtin_vectorfunction_t;

typedef struct expr_t expr_t;
typedef struct expr_item_t expr_item_t;

typedef struct var_t var_t;
typedef struct var_ll_t var_ll_t;
typedef struct vector_t vector_t;
typedef struct matrix_t matrix_t;
typedef struct function_t function_t;
typedef struct function_ll_t function_ll_t;

typedef struct alias_t alias_t;

typedef struct instruction_t instruction_t;
typedef struct conditional_block_t conditional_block_t;

typedef struct assignment_t assignment_t;

typedef struct file_t file_t;
typedef struct print_t print_t;
typedef struct print_token_t print_token_t;
typedef struct printf_t printf_t;
typedef struct print_function_t print_function_t;
typedef struct multidim_range_t multidim_range_t;
typedef struct print_vector_t print_vector_t;
typedef struct read_data_t read_data_t;


typedef struct sort_vector_t sort_vector_t;
typedef struct phase_object_t phase_object_t;
typedef struct dae_t dae_t;

typedef struct mesh_t mesh_t;
typedef struct mesh_write_t mesh_write_t;
typedef struct mesh_write_dist_t mesh_write_dist_t;
typedef struct mesh_integrate_t mesh_integrate_t;
typedef struct mesh_find_extrema_t mesh_find_extrema_t;

typedef struct fit_t fit_t;
typedef struct solve_t solve_t;
typedef struct dump_t dump_t;
typedef struct reaction_t reaction_t;

typedef struct physical_group_t physical_group_t;
typedef struct geometrical_entity_t geometrical_entity_t;
typedef struct physical_name_t physical_name_t;
typedef struct property_t property_t;
typedef struct property_data_t property_data_t;
typedef struct material_t material_t;
typedef struct material_ll_t material_ll_t;
typedef struct distribution_t distribution_t;

// nice but not needed
// typedef struct name_item_t name_item_t;
typedef struct bc_t bc_t;
typedef struct bc_data_t bc_data_t;

typedef struct node_t node_t;
typedef struct node_relative_t node_relative_t;
typedef struct element_t element_t;
typedef struct element_ll_t element_ll_t;
typedef struct element_type_t element_type_t;
typedef struct cell_t cell_t;
typedef struct neighbor_t neighbor_t;
typedef struct gauss_t gauss_t;

typedef struct elementary_entity_t elementary_entity_t;
typedef struct node_data_t node_data_t;


// individual item (factor) of an algebraic expression
struct expr_item_t {
  size_t n_chars;
  int type;           // defines #EXPR_ because we need to operate with masks
  
  size_t level;       // hierarchical level
  size_t tmp_level;   // for partial sums

  size_t oper;        // number of the operator if applicable
  double constant;    // value of the numerical constant if applicable
  double value;       // current value
  
// vector with (optional) auxiliary stuff (last value, integral accumulator, rng, etc)
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
  
  // lists of which variables and functions this item (and its daughters)
  var_ll_t *variables;
  function_ll_t *functions;
  

  expr_item_t *next;
};


// algebraic expression
struct expr_t {
  expr_item_t *items;
  double value;
  char *string;     // just in case we keep the original string
  
  // lists of which variables and functions this expression depends on
  var_ll_t *variables;
  function_ll_t *functions;
  
  expr_t *next;
};



// variable
struct var_t {
  char *name;
  int initialized;
  int reallocated;
  int used;
  int assigned_init;
  int assigned_zero;
  
  // these are pointers so the actual holder can be anything, from another
  // variable (i.e. an alias), a vector element (another alias) or even
  // more weird stufff (a fortran common?)
  double *value;
  double *initial_static;
  double *initial_transient;

  UT_hash_handle hh;
};

struct var_ll_t {
  var_t *var;
  var_ll_t *next;
};

struct function_ll_t {
  function_t *function;
  function_ll_t *next;
};

// vector
struct vector_t {
  char *name;
  int initialized;
  int reallocated;
  int used;
  int assigned_init;
  int assigned_zero;
  
  expr_t size_expr;
  int size;

  gsl_vector *value;
  gsl_vector *initial_transient;
  gsl_vector *initial_static;

  
  // linked list with the expressions of the initial elements
  expr_t *datas;

  UT_hash_handle hh;
};


// matrix
struct matrix_t {
  char *name;
  int initialized;  
  int reallocated;
  int used;
  int assigned_init;
  int assigned_zero;
  
  expr_t cols_expr;
  expr_t rows_expr;
  
  int cols;
  int rows;
  
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
  int used;
  
  // may be:
  //   - algebraic, either globally or on a per-material (physical groups) basis
  //   - pointwise-defined
  //       + given in the input with DATA
  //       + given in FeenoX vectors with VECTORS
  //       + given in a file with FILE
  //       + in a mesh
  //          * data at nodes
  //          * data at cells
  //  - computed in a user-provided routine (used to be supported, not anymore)
  //
  enum  {
    function_type_undefined,
    function_type_algebraic,
    function_type_pointwise_data,
    function_type_pointwise_file,
    function_type_pointwise_mesh_property,
    function_type_pointwise_mesh_node,
    function_type_pointwise_mesh_cell,
//    function_type_routine,
//    function_type_routine_internal,
  } type;
  
  // number of arguments the function takes
  int n_arguments;
  int n_arguments_given;  // check to see if the API gave us all the arguments the function needs

  // array of pointers to already-existing variables for the arguments
  var_t **var_argument;
  int var_argument_allocated;

  // expression for algebraic functions
  expr_t algebraic_expression;

  // number of tuples (independent, dependent) data for pointwise functions
  size_t data_size;

  // vectors with the data 
  vector_t **vector_argument;
  vector_t *vector_value;
  
  // this is in case there is a derivative of a mesh-based function and
  // we want to interpolate with the shape functions
  // warning! we need to put data on the original function and not on the derivatives
  function_t *spatial_derivative_of;
  int spatial_derivative_with_respect_to;

  // hints in case the grid is tensor-product regular
  int rectangular_mesh;
  expr_t expr_x_increases_first;
  int x_increases_first;
  expr_t *expr_rectangular_mesh_size;
  size_t *rectangular_mesh_size;
  double **rectangular_mesh_point;

  // helpers to interpolate 1D with GSL
  gsl_interp *interp;
  gsl_interp_accel *interp_accel;
  gsl_interp_type interp_type;

  // multidimensional interpolation type
  enum {
    interp_undefined,
    interp_nearest,
    interp_shepard,
    interp_shepard_kd,
    interp_bilinear
  } multidim_interp;

  
  expr_t expr_multidim_threshold;
  expr_t expr_shepard_radius;
  expr_t expr_shepard_exponent;

  double multidim_threshold;
  double shepard_radius;
  double shepard_exponent;

  // material property like E, nu, k, etc.
  property_t *property;

  // mesh over which the function is defined 
  mesh_t *mesh;
  double mesh_time;  // for time-dependent functions read from .msh
  
  // pointer to a k-dimensional tree to speed up stuff
  void *kd;
  
  // ----- ------- -----------   --        -       - 
  // funcion que hay que llamar para funciones tipo usercall 
//  double (*routine)(const double *);
  
  // ----- ------- -----------   --        -       - 
  // funcion que hay que llamar para funciones internas
//  double (*routine_internal)(const double *, function_t *);
//  void *params;
  
  // if this is true, we can hint feenox to do the implicit SOLVE_PROBLEM
  int is_solution;
  
  // if this is true and the parser sees this function,
  // then pde.compute_gradients is set to true
  int is_gradient;   
  int dummy_for_derivatives;
  double dummy_for_derivatives_value;

  UT_hash_handle hh;
};

struct feenox_expression_derivative_params {
  expr_t *expr;
  var_t *variable;
  function_t *function;
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


// igualacion algebraica 
struct  assignment_t {
  int initialized;
  
  var_t *variable;
  vector_t *vector;
  matrix_t *matrix;
  
  // expresiones que indican el rango de indices para asignaciones vectoriales/matriciales
  expr_t i_min;
  expr_t i_max;
  expr_t j_min;
  expr_t j_max;

  // expresiones que indican la fila y la columna en funcion de i y de j
  expr_t row;
  expr_t col;
  
  // to sum up:
  // a(row, col)<i_min,i_max;j_min,j_max>[t_min,tmax]
  
  // la expresion del miembro derecho
  expr_t rhs;
  
  // flag que indica si el assignment pide _init o _0
  int initial_static;
  int initial_transient;
  
  // si este es true entonces la asignacion es una sola (aun cuando el miembro
  // izquierdo sea un vector o matriz), i.e.
  // A(127,43)  es escalar
  // A(2*i+1,14) no es escalar
  int scalar;

  // si esto es true, entonces las asignaciones vector o matriz son plain, i.e.
  // A(i,j)
  int plain;
  
  // diferenciamos, en caso de las matrices, aquellas dependencias solo en i o en j
  // A(i,4) = 5 es un assignment que depende solo de i
  int expression_only_of_i;
  
  // A(8,j) = 1 es un assignment que depende solo de j
  int expression_only_of_j;
  
  
  assignment_t *next;

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

// printf-like instruction
struct printf_t {
  // pointer to the output file (if null, print to stdout)
  int all_ranks;
  file_t *file;
  char *format_string;
  int n_args;
  expr_t *expressions;
  
  printf_t *next;
};

struct read_data_t {
  file_t *file;
  // TODO: several things mixed together, this is where I miss C++'s STL
  var_t *variable;
  vector_t *vector;
  
  read_data_t *next;
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
  
  // mesh and physical group that tells where to print a function
  mesh_t *mesh;
  physical_group_t *physical_group;

  // flag to add a header explaining what the columns are
  int header;
  // formato de los numeritos "%e"
  char *format;
  // separador de cosas "\t"
  char *separator;

  print_function_t *next;

};

// instruccion imprimir uno o mas vectores 
struct print_vector_t {
  // apuntador al archivo de salida 
  file_t *file;

  // una linked list con las cosas a imprimir
  print_token_t *tokens;

  // apuntador a la primera funcion que aparezca (la que tiene la cantidad posta
  // de argumentos), no es igual a first_token->function porque esa puede ser NULL
  vector_t *first_vector;
  
  // stuff separator (default is tab)
  char *separator;
  
  int horizontal;
  expr_t elems_per_line;

  print_vector_t *next;
};

struct sort_vector_t {
  int descending;
  
  vector_t *v1;
  vector_t *v2;
};


struct phase_object_t {
  unsigned int offset;
  unsigned int size;
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



// mesh-related structs -----------------------------
// node
struct node_t {
  double x[3];              // spatial coordinates of the node
  size_t tag;               // number assigned by Gmsh
  size_t index_mesh;        // index within the node array
  size_t *index_dof;        // index within the solution vector for each DOF
  
  double *phi;              // values of the solution functions at the node
  gsl_matrix *dphidx;       // derivative of the m-th DOF with respect to coordinate g
                            // (this is a gsl_matrix to avoid having to do double mallocs and forgetting about row/col-major
//  gsl_matrix *delta_dphidx; // same as above but for the standard deviations of the derivatives
  double *flux;             // holder of arbitrary functions evaluated at the node (e.g. sigmas and taus)
  
  element_ll_t *element_list;
};


struct node_data_t {
  char *name_in_mesh;
  function_t *function;
  int found;
  
  node_data_t *next;
};

struct node_relative_t {
  size_t index;
  node_relative_t *next;
};


struct physical_group_t {
  char *name;
  // these are ints because that's what we read from a .msh
  int tag;
  int dimension;
  
  material_t *material;    // pointer to single material
  size_t n_bcs;
  bc_t **bc;              // array of pointers to bc_t
     
  // volume (or area or length depending on the dim, sometimes called mass)
  double volume;
  double cog[3];
  
  var_t *var_volume;
  vector_t *vector_cog;
  
  // a linked list seems to be too expensive
  size_t n_elements;
  size_t i_element;
  size_t *element;
  
  UT_hash_handle hh;
  UT_hash_handle hh_tag[4];
};


struct geometrical_entity_t {
  int dim;
  int tag;
  int parent_dim;
  int parent_tag;
  double boxMinX, boxMinY, boxMinZ, boxMaxX, boxMaxY, boxMaxZ;

  size_t num_partitions;
  int *partition;
  
  size_t num_physicals;
  int *physical;
  size_t num_bounding;
  int *bounding;

  UT_hash_handle hh[4];
};

struct elementary_entity_t {
  size_t id;
  elementary_entity_t *next;
};



struct gauss_t {
  unsigned int Q;               // number of points (q=1,2,...,Q )
  double *w;           // weights (w[q] is the weight of the v-th point)
  double **xi;         // coordinates (xi[q][d] is the coordinate of the q-th point in dimension d)
  
  // one matrix for each gauss point q
  gsl_matrix **H_c;    // H(1,j) = h_j(xi_q)
  gsl_matrix **B_c;    // B(d,j) = d(h_j)/d(xi_d) at xi_q
  gsl_matrix *extrap;  // matrix to extrapolate the values from the gauss points to the nodes
};


// constant structure with the types of elements
// this includes the pointers to the shape functions
// the numbering is Gmsh-based
struct element_type_t {
  unsigned int id;              // as of Gmsh
  unsigned int dim;
  unsigned int order;
  unsigned int nodes;           // total, i.e. 10 for tet10
  unsigned int vertices;        // the corner nodes, i.e 4 for tet10
  unsigned int faces;           // faces == number of neighbors
  unsigned int nodes_per_face;  // (max) number of nodes per face

  double *barycenter_coords;
  double **node_coords;
  node_relative_t **node_parents;
  
  // virtual (sic) methods!
  // shape functions and derivatives in the local coords
  double (*h)(int j, double *xi);
  double (*dhdxi)(int j, int d, double *xi);

  // G-aware canonical matrices
  gsl_matrix **H_Gc;
  gsl_matrix **B_Gc;
  
  // convenience methods
  int (*point_inside)(element_t *e, const double *x);
  double (*volume)(element_t *e);
  double (*area)(element_t *e);
  double (*size)(element_t *e);

  gauss_t gauss[2];    // sets of gauss points
                       // 0 - full integration
                       // 1 - reduced integration

  // for doc only
  // TODO: move somewhere else?
  char *name;
  char *desc;
  char *ascii_art;
  int doc_n_edges;
  int (*doc_edges)[2];
  int doc_n_faces;
  int (*doc_faces)[8];
  char *(*h_latex)(int j);

};


struct element_t {
  size_t index;
  size_t tag;
  
  double quality;
  double volume;
  double area;
  double size;
  double gradient_weight;   // this weight is used to average the contribution of this element to nodal gradients
  double *w;                // weights of the gauss points time determinant of the jacobian
  double **x;               // coordinates fo the gauss points 
  double *normal;           // outward normal direction (only for 2d elements)
  
  // matrix with the coordinates (to compute the jacobian)
  gsl_matrix *C;
  
  // these are pointers to arrays of matrices are evaluated at the gauss points
  gsl_matrix **B;
  gsl_matrix **B_G;
  gsl_matrix **J;    // d(x)/d(xi)
  gsl_matrix **invJ; // d(xi)/d(x)
  
  gsl_matrix **dphidx_gauss;  // spatial derivatives of the DOFs at the gauss points
  gsl_matrix **dphidx_node;   // spatial derivatives of the DOFs at the nodes (extrapoladed or evaluated)
  double **property_at_node;  // 2d array [j][N] of property N evaluated at node j

  element_type_t *type;                  // pointer to the element type
  physical_group_t *physical_group;      // pointer to the physical group this element belongs to
  geometrical_entity_t *geometrical_entity;
  node_t **node;                         // pointer to the nodes, node[j] points to the j-th local node
  cell_t *cell;                          // pointer to the associated cell (only for FVM)
};


struct element_ll_t {
  element_t *element;
  element_ll_t *next;
};

struct cell_t {
  size_t id;
  
  element_t *element;
  
  unsigned int n_neighbors;
  size_t *ineighbor;      // array with ids of neighboring elements
  size_t **ifaces;        // array of arrays of ids making up the faces
  
  neighbor_t *neighbor;   // array of neighbors

  double x[3];            // coordenadas espaciales del baricentro de la celda
  size_t *index;          // indice del vector incognita para cada uno de los grados de libertad

  double volume;

};

struct neighbor_t {
  cell_t *cell;
  element_t *element;
 
  double **face_coord;
  double x_ij[3];
  double n_ij[3];
  double S_ij;
};

struct material_t {
  char *name;
  mesh_t *mesh;
  property_data_t *property_datums;
  
  char *model;
  void *ctx;

  UT_hash_handle hh;
};

struct material_ll_t {
  material_t *material;
  material_ll_t *next;
};
  
struct property_t {
  char *name;
  property_data_t *property_datums;

  UT_hash_handle hh;
};

struct property_data_t {
  property_t *property;
  material_t *material;
  expr_t expr;

  UT_hash_handle hh;
};

struct distribution_t  {
  char *name;
  
  int defined;             // true or false if is defined at least in one group
  int *defined_per_group;  // true or false if is defined for each group
  int full;                // true if all the groups have the property
  
  property_t *property;
  function_t *function;
  var_t *variable;
  
  // caches
  material_t *last_material;
  property_data_t *last_property_data;
  expr_t *expr;
  double value;
  
  // dependencies
  var_ll_t *dependency_variables;
  function_ll_t *dependency_functions;
  
  // is this distribution uniform? (i.e. does not depend on space)
  int non_uniform;
  // is this distribution constant? (i.e. does not depend on time) 
  int constant;
  // does this distribution depend on the solution itself?
  int non_linear; 
  
  // virtual method to evaluate at a point
  double (*eval)(distribution_t *, const double *x, material_t *material);
};

// nice but not needed
/*
struct name_item_t {
  char *name;
  name_item_t *next;
};
*/

struct bc_t {
  char *name;
  mesh_t *mesh;
//  name_item_t *groups;
  bc_data_t *bc_datums;

  UT_hash_handle hh;  // for the hashed list mesh.bcs
};

struct bc_data_t {
  char *string;

  enum {
    bc_type_math_undefined,
    bc_type_math_dirichlet,
    bc_type_math_neumann,
    bc_type_math_robin,
    bc_type_math_multifreedom
  } type_math;
  
  int type_phys;     // problem-based flag that tells which type of BC this is
  
  // boolean flags
  int space_dependent;
  int nonlinear;
  int disabled;
  int fills_matrix;
  
  unsigned int dof;  // -1 means "all" dofs
  expr_t expr;
  expr_t condition;  // if it is not null the BC only applies if this evaluates to non-zero
  
  int (*set_essential)(bc_data_t *, element_t *, size_t j_global);
  int (*set_natural)(bc_data_t *, element_t *, unsigned int q);
  
  bc_data_t *prev, *next;   // doubly-linked list in ech bc_t
};


// unstructured mesh
struct mesh_t {
  file_t *file;
  unsigned int dim;
  unsigned int dim_topo;

  size_t n_nodes;
  size_t n_elements;
  size_t n_elements_per_dim[4];
  size_t n_cells; // a cell is an element with the topological dimension of the mesh

  int n_partitions;

  int degrees_of_freedom;        // per unknown
  unsigned int order;

  physical_group_t *physical_groups;              // global hash table
  physical_group_t *physical_groups_by_tag[4];    // 4 hash tables one per tag
  int physical_tag_max;                           // the higher tag of the entities
  int n_groups;
  
  // number of geometric entities of each dimension
  size_t points, curves, surfaces, volumes;
  geometrical_entity_t *geometrical_entities[4];     // 4 hash tables, one for each dimension
  
  // partition data
  size_t num_partitions;
  size_t num_ghost_entitites;
  size_t partitioned_points, partitioned_curves, partitioned_surfaces, partitioned_volumes;
  

  int sparse;            // flag that indicates if the nodes are sparse
  size_t *tag2index;     // array to map tags to indexes (we need a -1)
  // pointer to tag_min entries before so we can use the tag as an offset
  size_t *tag2index_from_tag_min; 
  
  enum {
    data_type_element,
    data_type_node,
  } data_type;
  
  enum {
    integration_full,
    integration_reduced
  } integration;
  
  enum {
    mpi_matches_partitions_no,
    mpi_matches_partitions_serial,
    mpi_matches_partitions_one_to_one,
    mpi_matches_partitions_one_to_many,
  } mpi_matches_partitions;

  // this is for mpi_matches_partitions_no
  size_t first_element;
  size_t last_element;
  
  
  int update_each_step;
  
  expr_t scale_factor;           // scale factor when reading the coordinates of the nodes
  expr_t offset_x;               // offset en nodos
  expr_t offset_y;               // offset en nodos
  expr_t offset_z;               // offset en nodos
    
  gsl_vector **nodes_argument;
  gsl_vector **cells_argument;

  node_data_t *node_datas;
  
  int n_physical_names;
  node_t *node;
  element_t *element;
  cell_t *cell;
  
  node_t bounding_box_max;
  node_t bounding_box_min;
  
  unsigned int max_nodes_per_element;
  unsigned int max_faces_per_element;
  unsigned int max_first_neighbor_nodes;  // to estimate matrix bandwidth

  // kd-trees to make efficient searches
  void *kd_nodes;
  void *kd_cells;

  element_t *last_chosen_element;     // interpolation cache

  // virtual method for format-dependet reader
  int (*reader)(mesh_t *);

  int initialized;

  UT_hash_handle hh;

};

typedef enum {
  field_location_default,
  field_location_nodes,
  field_location_cells,
  field_location_gauss
} field_location_t;

struct mesh_write_dist_t {
  char *name;
  unsigned int size;  // TODO: rename to num_of_components
  function_t **field;
  
  field_location_t field_location;
  char *printf_format;
  
  mesh_write_dist_t *next;
};

struct mesh_write_t {
  mesh_t *mesh;
  file_t *file;
  int no_mesh;
  
  enum  {
    post_format_fromextension,
    post_format_gmsh,
    post_format_vtu,
    post_format_vtk,
  } post_format;

  int no_physical_names;
  field_location_t field_location;
  char *printf_format;
  
  int (*write_header)(mesh_t *mesh, FILE *file);
  int (*write_mesh)(mesh_t *mesh, FILE *file, int no_physical_names);
  int (*write_data)(mesh_write_t *this, mesh_write_dist_t *dist);
  int (*write_footer)(mesh_write_t *this);
   
  // these two are to know if we have to change the type in VTK
  int point_init;
  int cell_init;
  
  // this is to know if we already wrote LOOKUP_TABLE
  int lookup_table_init;
  
  mesh_write_dist_t *mesh_write_dists;
  
  mesh_write_t *next;
};

struct mesh_integrate_t {
  mesh_t *mesh;
  function_t *function;
  expr_t expr;
  physical_group_t *physical_group;
  field_location_t field_location;
  unsigned int gauss_points;

  var_t *result;

  mesh_integrate_t *next;
};

struct mesh_find_extrema_t {
  mesh_t *mesh;
  physical_group_t *physical_group;
  function_t *function;
  expr_t expr;
  field_location_t field_location;
  
  var_t *min;
  var_t *i_min;
  var_t *x_min;
  var_t *y_min;
  var_t *z_min;

  var_t *max;
  var_t *i_max;
  var_t *x_max;
  var_t *y_max;
  var_t *z_max;
      
  mesh_find_extrema_t *next;
};


// fit an algebraic function to a pointwise-defined function
struct fit_t {
  
  unsigned int max_iter;
  int verbose;
  
  // number of parameters to fit (i.e. a & b -> 2)
  unsigned int n_via;
  // number of experimental data to fit (order 1,000)
  size_t n_data;
  
  // algebraic function whose parameters we have to fit 
  function_t *function;
  // point-wise function with the experimental data
  function_t *data;
  
  // pointers to the variables acting as parameters
  var_t **via;
  // and their statistical uncertainties
  var_t **sigma;

  // array of size n_params with the analytical derivatives of the function with respect to the params
  // if it is NULL we have to compute it numerically
  expr_t *gradient;  
  
  // same thing for the initial guess
  expr_t *guess;
  
  // range where to fit
  multidim_range_t range;
  
  //expr_t tol_abs;
  //expr_t tol_rel;
  
  // working pointers
  double *x;
  double *range_min;
  double *range_max;
  double *y_plus;
  double *y_minus;
  
  fit_t *next;
  
};

struct solve_t {
  size_t n_unknowns;
  var_t **unknown;
  expr_t *residual;
  
  expr_t *guess;
  
  expr_t epsabs;
  expr_t epsrel;
  int max_iter;
  int verbose;

  const gsl_multiroot_fsolver_type *type;
  
  solve_t *next;  
};

struct dump_t {
  
  enum {
    dump_format_default,
    dump_format_binary,
    dump_format_ascii,
    dump_format_octave,
  } format;  
  
  int K;
  int K_bc;
  int b;
  int b_bc;
  int M;
  int M_bc;
  
  dump_t *next;
};

struct reaction_t {
  physical_group_t *physical_group;
  var_t *scalar;
  vector_t *vector;
  int order;
  expr_t x0[3];
  
  reaction_t *next;
};

// global FeenoX singleton structure
struct feenox_t {
  int argc;
  char **argv;
  char **argv_orig;
  int optind;

  char *main_input_filepath;
  char *main_input_dirname;

#ifdef HAVE_CLOCK_GETTIME
  struct timespec tp0;  
#endif

  int check;
  // TODO
  // int debug;
  
  int mpi_rank;
  int mpi_size;

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
  
  assignment_t *assignments;
  file_t *files;
  print_t *prints;
  printf_t *printfs;
  print_function_t *print_functions;
  print_vector_t *print_vectors;
  fit_t *fits;
  solve_t *solves;
  
  struct {
    var_t *done;
    var_t *done_static;
    var_t *done_transient;
  
    var_t *step_static;
    var_t *step_transient;

    var_t *in_static;
    var_t *in_static_first;
    var_t *in_static_last;
    var_t *in_transient;
    var_t *in_transient_first;
    var_t *in_transient_last;
    var_t *in_time_path;

    var_t *static_steps;
    var_t *end_time;

    var_t *t;
    var_t *dt;

    var_t *dae_rtol;
    var_t *min_dt;
    var_t *max_dt;
  
    var_t *i;
    var_t *j;
    
    var_t *pi;
    var_t *zero;
    var_t *infinite;

    var_t *pid;
    var_t *mpi_size;
    var_t *mpi_rank;
    
    var_t *on_nan;
    var_t *on_gsl_error;
    var_t *on_ida_error;
//    var_t *realtime_scale;
  } special_vars;


  struct {
    vector_t *abs_error;
  } special_vectors;

  
  struct {
    file_t *_stdin;
    file_t *_stdout;
    file_t *_stderr;
  } special_files;  

  struct {

    int need_cells;

    mesh_t *meshes;
    mesh_t *mesh_main;
    
    material_t *materials;
    property_t *properties;
    bc_t *bcs;
    
    mesh_write_t *mesh_writes;
    mesh_integrate_t *integrates;
    mesh_find_extrema_t *find_extremas;
    
    field_location_t default_field_location;
    element_type_t *element_types;

    // estas tres variables estan reallocadas para apuntar a vec_x
    struct {
      var_t *x;
      var_t *y;
      var_t *z;
      var_t *arr_x[3];   // x, y y z en un array de tamanio 3
      vector_t *vec_x;

      var_t *nx;
      var_t *ny;
      var_t *nz;
      var_t *arr_n[3];
      vector_t *vec_n;

      var_t *eps;

      var_t *nodes;
      var_t *cells;
      var_t *elements;

      vector_t *bbox_min;
      vector_t *bbox_max;

      var_t *mesh_failed_interpolation_factor;
    } vars;

  } mesh;
  
  struct {
    unsigned int dimension;

    int reading_daes;
    double **phase_value;
    double **phase_derivative;
    phase_object_t *phase_objects;
    enum {
      initial_conditions_as_provided,
      initial_conditions_from_variables,
      initial_conditions_from_derivatives,
    } initial_conditions_mode;

#ifdef HAVE_SUNDIALS
    N_Vector x;
    N_Vector dxdt;
    N_Vector id;
    N_Vector abs_error;
    SUNMatrix A;
    SUNLinearSolver LS;
#if SUNDIALS_VERSION_MAJOR >= 6
    SUNContext ctx;
#endif
#endif

    dae_t *daes;
    void *system;
    instruction_t *instruction;
    
  } dae;
  
  struct {

    enum {
      symmetry_axis_none,
      symmetry_axis_x,
      symmetry_axis_y
    } symmetry_axis;
    
    enum  {
      hanging_nodes_nothing,
      hanging_nodes_detect,
      hanging_nodes_handle
    } hanging_nodes;
    enum  {
      unresolved_bcs_detect,
      unresolved_bcs_allow
    } unresolved_bcs;

    // these can be read from the input but also we can figure them out
    enum {
      transient_type_undefined,
      transient_type_transient,
      transient_type_quasistatic
    } transient_type;
    
    enum {
      math_type_automatic,
      math_type_linear,
      math_type_nonlinear,
      math_type_eigen,
    } math_type;

    enum {
      eigen_formulation_undefined,
      eigen_formulation_lambda,
      eigen_formulation_omega
    } eigen_formulation;
    
    enum {
      eigen_dirichlet_zero_M,
      eigen_dirichlet_zero_K
    } eigen_dirichlet_zero;
    

    unsigned int dim;              // spatial dimension of the problem (currently, equal to the topological dimension)
    unsigned int dofs;             // DoFs per node/cell
    size_t width;                  // number of expected non-zeros per row
    size_t spatial_unknowns;       // number of spatial unknowns (nodes in fem, cells in fvm)
    size_t size_global;            // total number of DoFs
    
    int do_not_solve;        // flag to skip solves in particular cases (e.g. convert to other format)
    int compute_gradients;   // do we need to compute gradients?
    int problem_solved;      // status to detect implicit SOLVE_PROBLEMs
    
    void *missed_dump;
//    gsl_matrix *m2;
    
    int rough;               // keep each element's contribution to the gradient?
    int roughish;            // average only on the same physical group?
  
    mesh_t *mesh;
    mesh_t *mesh_rough;      // in this mesh each elements has unique nodes (they are duplicated)
  
    // problem-specific virtual methods
    // parse
    int (*parse_problem)(const char *token);
    int (*parse_write_results)(mesh_write_t *mesh_write, const char *token);
    int (*parse_bc)(bc_data_t *bc_data, const char *lhs, char *rhs);
    
    // init
    int (*init_before_run)(void);
#ifdef HAVE_PETSC
    int (*setup_pc)(PC pc);
    int (*setup_ksp)(KSP ksp);
    int (*setup_snes)(SNES snes);
  #ifdef HAVE_SLEPC
    int (*setup_eps)(EPS eps);
  #endif
    int (*setup_ts)(TS ksp);
#endif

    // build
    int (*element_build_allocate_aux)(unsigned int J);
    int (*element_build_volumetric)(element_t *e);
    int (*element_build_volumetric_at_gauss)(element_t *e, unsigned int q);
    
    // solve
    int (*solve)(void);
    
    // post
    int (*solve_post)(void);
    int (*gradient_fill)(void);
    int (*gradient_nodal_properties)(element_t *e, mesh_t *mesh);
    int (*gradient_alloc_nodal_fluxes)(node_t *node);
    int (*gradient_add_elemental_contribution_to_node)(node_t *node, element_t *e, unsigned int j, double rel_weight);
    int (*gradient_fill_fluxes)(mesh_t *mesh, size_t j_global);


    // instruction pointer to know before/after transient PDE
    instruction_t *instruction;
    
    function_t *initial_guess;
    function_t *initial_condition;

    dump_t *dumps;
    reaction_t *reactions;
    
    struct {
      var_t *ksp_atol;
      var_t *ksp_rtol;
      var_t *ksp_divtol;
      var_t *ksp_max_it;

      var_t *snes_atol;
      var_t *snes_rtol;
      var_t *snes_stol;
      var_t *snes_max_it;
      
      var_t *ts_atol;
      var_t *ts_rtol;
      
      var_t *eps_tol;
      var_t *eps_max_it;
      var_t *eps_st_sigma;
      var_t *eps_st_nu;

      var_t *mumps_cntl_1;
      var_t *mumps_icntl_14;
      var_t *mumps_icntl_24;
      
      var_t *gamg_threshold;
      
      var_t *penalty_weight;
      var_t *nodes_rough;
      var_t *total_dofs;
    
      var_t *memory_available;
    
    } vars;

    // vectors
    struct {
      vector_t **phi;
    } vectors;

    char **unknown_name;          // one for each DOF
  
    // the functions with the solutions (one for each DOF)
    function_t **solution;
    // derivatives of solutions with respect to space (DOFs x dims)
    function_t ***gradient;
    // uncertainty (i.e standard deviation of the contribution of each element)
    function_t ***delta_gradient;  
    
    // solutions for eigenproblems
    function_t ***mode;
  
    enum {
      gradient_gauss_extrapolated,
      gradient_at_nodes,
      gradient_none
    } gradient_evaluation;

    enum {
      gradient_weight_volume,
      gradient_weight_quality,
      gradient_weight_volume_times_quality,
      gradient_weight_flat,
    } gradient_element_weight;

    enum {
      gradient_average,
      gradient_actual
    } gradient_highorder_nodes;
  
    int has_stiffness;
    int has_mass;
    int has_rhs;
    int has_internal_fluxes;
    int has_jacobian;
    int has_jacobian_K;
    int has_jacobian_M;
    int has_jacobian_b;
    int symmetric_K;
    int symmetric_M;

#ifdef HAVE_PETSC    
    char *petsc_options;
    PetscBool pre_allocate;        // preallocate? only works for petsc >= 3.19
    PetscBool allow_new_nonzeros;  // flag to set MAT_NEW_NONZERO_ALLOCATION_ERR to false, needed in some rare cases
    PetscBool petscinit_called;    // flag

    // stuff for mpi parallelization
    PetscInt nodes_local, size_local;
    PetscInt first_row, last_row;
    PetscInt first_node, last_node;
    
    // dirichlet BC scaling factor
    PetscScalar dirichlet_scale;
    PetscScalar dirichlet_scale_fraction;

    // global objects
    Vec phi;       // the unknown (solution) vector
    Vec phi_bc;    // the unknown (solution) vector with dirichlet BCs
    Vec b;         // the right-hand side vector without dirichlet BCs
    Vec b_bc;      // idem with dirichlet BCs (for KSP)
    Vec f;         // internal fluxes (forces) for non-linear problems
    Vec r;         // residual vector for SNES and TS
    
    Mat K;       // stiffness matrix without dirichlet BCs
    Mat K_bc;    // stiffness matrix with dirichlet BCs (for KSP)
    Mat M;       // mass matrix (rho for elastic, rho*cp for heat)
    Mat M_bc;    // mass matrix with dirichlet BCs (for EPS)
    Mat JK;      // jacobian for stiffness matrix = K'*phi
    Mat JM;      // jacobian for mass matrix = M'*phi_dot
    Mat Jb;      // jacobian for rhs vector = dq/dT for both volumetric and BCs
    Mat J_snes;  // jacobian for SNES
    Mat J_tran;  // jacobian for TS
    
    PetscScalar *eigenvalue;    // eigenvalue vector
    Vec *eigenvector;           // eivenvectors vector

    
    // internal storage of evaluated dirichlet conditions
    size_t          *dirichlet_nodes;
    unsigned int    *dirichlet_dofs;
    PetscInt        *dirichlet_indexes;
    PetscScalar     *dirichlet_values;
    PetscScalar     *dirichlet_derivatives;
    size_t          dirichlet_k;
    // reusable number of dirichlet rows to know how much memory to allocate
    size_t          dirichlet_rows;

    // internal storage of multi-freedom conditions
    PetscInt        **multifreedom_indexes;
    gsl_matrix      **multifreedom_coefficients;
    size_t          multifreedom_k;
    size_t          multifreedom_nodes;
    
    // PETSc's solvers
    TS ts;
    SNES snes;
    KSP ksp;

    // strings with types
    PCType pc_type;
    KSPType ksp_type;
    SNESType snes_type;
    SNESLineSearchType ls_type;
    TSType ts_type;
    TSAdaptType ts_adapt_type;

    PetscInt nev;                      // number of requested modes
#ifdef HAVE_SLEPC
    EPSType eps_type;
    STType st_type;
    EPS eps;
#endif

    PetscBool progress_ascii;
    PetscReal progress_r0;
    PetscReal progress_last;
    
    PetscLogStage stage_init;
    PetscLogStage stage_build;
    PetscLogStage stage_solve;
    PetscLogStage stage_post;
    

    expr_t eps_ncv;
    expr_t st_shift;
    expr_t st_anti_shift;  

#endif  // HAVE_PETSC    
    
  } pde;

  struct {
    // TODO: this is not thread safe! we stick to MPI anyway...
    size_t current_gauss_element_tag;
    element_type_t *current_gauss_type;

    unsigned int current_GJ;
    element_type_t *current_elemental_type;
    element_type_t *current_jacobian_type;
    
    size_t current_weight_element_tag;
    size_t current_jacobian_element_tag;
    unsigned int current_weight_gauss_point;
    unsigned int current_jacobian_gauss_point;
    
    double *w;
    double **x;
    gsl_matrix *C;
    gsl_matrix **Ji;
    gsl_matrix **invJi;
    gsl_matrix **Bi;
    gsl_matrix **B_Gi;
    
    // elemental (local) objects
    gsl_matrix *Ki;               // elementary stiffness matrix
    gsl_matrix *Mi;               // elementary mass matrix
    gsl_matrix *JKi;              // elementary jacobian for stiffness matrix
    gsl_matrix *Jbi;              // elementary jacobian for RHS vector
    gsl_vector *bi;               // elementary right-hand side vector
    gsl_vector *fi;               // elementary non-linear internal "force" vector
    gsl_vector *phii;             // elementary solution vector

    gsl_vector *vec_f;               // temporary vector for rhs things like H'*f

    int cache_J;
    int cache_B;
#ifdef HAVE_PETSC
    PetscInt *l;  // node-major-ordered vector with the global indexes of the DOFs in the element    
#endif    
  } fem;

  
};
extern feenox_t feenox;

// function declarations

// run.c
extern int feenox_run(void);
extern void feenox_limit_time_step(void);

// step.c
extern int feenox_step(instruction_t *first, instruction_t *last);

// init.c
extern int feenox_initialize(int argc, char **argv);
extern int feenox_init_special_objects(void);
extern int feenox_init_after_parser(void);

// mesh's init.c
extern int feenox_mesh_init_special_objects(void);

// version.c
extern void feenox_show_help(const char *progname, int extra);
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
extern char *feenox_ends_in_init(const char *name);
extern char *feenox_ends_in_zero(const char *name);
extern int feenox_count_arguments(char *string, size_t *);
extern int feenox_read_arguments(char *string, int n_arguments, char ***arg, size_t *n_chars);


// file.c
char *feenox_evaluate_string(const char *restrict format, int n_args, expr_t *arg);
extern int feenox_instruction_file(void *arg);
FILE *feenox_fopen(const char *filepath, const char *mode);
extern int feenox_instruction_file_open(void *arg);
extern int feenox_instruction_file_close(void *arg);

// read.c
extern int feenox_instruction_read_data(void *arg);


// abort.c
extern int feenox_instruction_abort(void *arg);

// expressions.c
extern int feenox_expression_parse(expr_t *, const char *string);
extern expr_item_t *feenox_expression_parse_item(const char *string);
extern int feenox_parse_range(char *string, const char left_delim, const char middle_delim, const char right_delim, expr_t *a, expr_t *b);

extern double feenox_expression_eval(expr_t *);
extern double feenox_expression_evaluate_in_string(const char *string);

extern int feenox_pull_dependencies_variables(var_ll_t **to, var_ll_t *from);
extern int feenox_pull_dependencies_functions(function_ll_t **to, function_ll_t *from);

double feenox_expression_derivative_wrt_function_gsl_function(double x, void *params);
double feenox_expression_derivative_wrt_function(expr_t *expr, function_t *function, double x);

double feenox_expression_derivative_wrt_variable_gsl_function(double x, void *params);
double feenox_expression_derivative_wrt_variable(expr_t *expr, var_t *variable, double x);

// dae.c
extern int feenox_add_time_path(const char *token);
extern int feenox_phase_space_add_object(const char *token, int differential);
extern char *feenox_find_first_dot(const char *s);
extern int feenox_add_dae(const char *lhs, const char *rhs);
extern int feenox_dae_init(void);
extern int feenox_dae_ic(void);
#ifdef HAVE_SUNDIALS
extern int feenox_ida_dae(sunrealtype t, N_Vector yy, N_Vector yp, N_Vector rr, void *params);
#else
extern int feenox_ida_dae(void);
#endif
extern int feenox_instruction_dae(void *arg);

// instruction.c
extern instruction_t *feenox_add_instruction_and_get_ptr(int (*routine)(void *), void *argument);
extern int feenox_add_instruction(int (*routine)(void *), void *argument);


// define.c
extern int feenox_check_name(const char *name);
extern int feenox_define_variable(const char *name);
extern int feenox_define_alias(const char *new_name, const char *existing_object, const char *row, const char *col);

extern int feenox_define_vector(const char *name, const char *size);
//extern int feenox_vector_attach_function(const char *name, const char *function_data);
extern int feenox_vector_attach_data(const char *name, expr_t *datas);

extern int feenox_define_matrix(const char *name, const char *rows, const char *cols);
extern int feenox_matrix_attach_data(const char *name, expr_t *datas);

extern int feenox_define_function(const char *name, unsigned int n_arguments);
extern function_t *feenox_define_function_get_ptr(const char *name, int n_arguments);

extern int feenox_define_file(const char *name, const char *format, int n_args, const char *mode);
extern int feenox_file_set_path_argument(const char *name, int i, const char *expression);
extern file_t *feenox_get_or_define_file_get_ptr(const char *name);

extern var_t *feenox_get_or_define_variable_get_ptr(const char *name);
extern var_t *feenox_define_variable_get_ptr(const char *name);

extern vector_t *feenox_define_vector_get_ptr(const char *name, size_t size);

extern int feenox_realloc_variable_ptr(var_t *, double *newptr, int copy_contents);
extern int feenox_realloc_vector_ptr(vector_t *, double *newptr, int copy_contents);

// getptr.c
extern var_t *feenox_get_variable_ptr(const char *name);
extern vector_t *feenox_get_vector_ptr(const char *name);
extern matrix_t *feenox_get_matrix_ptr(const char *name);
extern function_t *feenox_get_function_ptr(const char *name);
extern builtin_function_t *feenox_get_builtin_function_ptr(const char *name);
extern builtin_vectorfunction_t *feenox_get_builtin_vectorfunction_ptr(const char *name);
extern builtin_functional_t *feenox_get_builtin_functional_ptr(const char *name);

extern vector_t *feenox_get_first_vector(const char *s);

extern file_t *feenox_get_file_ptr(const char *name);
extern mesh_t *feenox_get_mesh_ptr(const char *name);
extern physical_group_t *feenox_get_physical_group_ptr(const char *name, mesh_t *mesh);


// assignment.c
extern int feenox_add_assignment(const char *left_hand, const char *right_hand);
extern int feenox_assign_single(assignment_t *assignment, unsigned int row, unsigned int col);
extern int feenox_instruction_assignment_scalar(void *arg);
extern int feenox_instruction_assignment_vector(void *arg);
extern int feenox_instruction_assignment_matrix(void *arg);
extern int feenox_get_assignment_rowcol(assignment_t *assignment, size_t i, size_t j, size_t *row, size_t *col);

// auxiliary.c
extern int feenox_strip_blanks(char *string);
extern int feenox_strip_comments(char *line);

// alias.c
extern int feenox_instruction_alias(void *arg);

// fit.c
extern int feenox_instruction_fit(void *arg);

// solve.c
extern int feenox_instruction_solve(void *arg);

// dump.c
extern int feenox_instruction_dump(void *arg);
#ifdef HAVE_PETSC
extern int feenox_dump_open_viewer(dump_t *, const char *name, PetscViewer *viewer);
#endif

// reaction.c
extern int feenox_instruction_reaction(void *arg);

// matrix.c
extern int feenox_matrix_init(matrix_t *);

// vector.c
extern int feenox_create_pointwise_function_vectors(function_t *function);
extern int feenox_vector_init(vector_t *, int no_initial);
extern int feenox_instruction_sort_vector(void *arg);
extern double feenox_vector_get(vector_t *, const size_t i);
extern double feenox_vector_get_initial_static(vector_t *, const size_t i);
extern double feenox_vector_get_initial_transient(vector_t *, const size_t i);
extern int feenox_vector_set(vector_t *, const size_t i, double value);
extern int feenox_vector_add(vector_t *, const size_t i, double value);
extern int feenox_vector_set_size(vector_t *, const size_t size);
extern size_t feenox_vector_get_size(vector_t *);

// function.c
extern int feenox_function_init(function_t *);
extern int feenox_function_set_args(function_t *, double *x);
extern int feenox_function_set_argument_variable(const char *name, unsigned int i, const char *variable_name);
extern int feenox_function_set_expression(const char *name, const char *expression);
extern int feenox_function_set_interpolation(const char *name, const char *type);
extern int feenox_function_set_file(const char *name, file_t *file, unsigned int *columns);
extern int function_set_buffered_data(function_t *function, double *buffer, size_t n_data, unsigned int n_columns, unsigned int *columns);
size_t feenox_structured_scalar_index(unsigned int n_dims, size_t *size, size_t *index, int x_increases_first);

extern double feenox_function_eval(function_t *, const double *x);
extern double feenox_factor_function_eval(expr_item_t *);

extern int feenox_function_is_structured_grid_2d(double *x, double *y, size_t n, size_t *nx, size_t *ny);
extern int feenox_function_is_structured_grid_3d(double *x, double *y, double *z, size_t n, size_t *nx, size_t *ny, size_t *nz);

// print.c
extern int feenox_instruction_print(void *arg);
extern int feenox_instruction_printf(void *arg);
extern int feenox_instruction_print_function(void *arg);
extern int feenox_instruction_print_vector(void *arg);
extern char *feenox_print_vector_current_format_reset(print_vector_t *);
extern int feenox_debug_print_gsl_vector(const gsl_vector *b, FILE *file);
extern int feenox_debug_print_gsl_matrix(const gsl_matrix *A, FILE *file);

// conditional.c
extern int feenox_instruction_if(void *arg);
extern int feenox_instruction_else(void *arg);
extern int feenox_instruction_endif(void *arg);

// functionals.c
extern double feenox_gsl_function(double x, void *params);

// mesh.c
extern int feenox_instruction_mesh_read(void *arg);
extern int feenox_mesh_create_nodes_argument(mesh_t *);
extern int feenox_mesh_free(mesh_t *);

extern int feenox_mesh_read_vtk(mesh_t *);
extern int feenox_mesh_read_frd(mesh_t *);

extern node_t *feenox_mesh_find_nearest_node(mesh_t *, const double *x);
extern element_t *feenox_mesh_find_element(mesh_t *, node_t *nearest_node, const double *x);

// interpolate.c
extern double feenox_function_property_eval(struct function_t *function, const double *x);
extern int feenox_mesh_interp_solve_for_r(element_t *, const double *x, double *r) ;
extern int feenox_mesh_interp_residual(const gsl_vector *test, void *params, gsl_vector *residual);
extern int feenox_mesh_interp_jacob(const gsl_vector *test, void *params, gsl_matrix *J);
extern int feenox_mesh_interp_residual_jacob(const gsl_vector *test, void *params, gsl_vector *residual, gsl_matrix * J);

extern double feenox_mesh_interpolate_function_node(struct function_t *function, const double *x);
extern int feenox_mesh_compute_r_tetrahedron(element_t *, const double *x, double *r);



// gmsh.c
extern int feenox_mesh_read_gmsh(mesh_t *mesh);
extern int feenox_mesh_tag2index_alloc(mesh_t *mesh, size_t, size_t );
extern int feenox_mesh_update_function_gmsh(function_t *function, double t, double dt);
extern int feenox_mesh_write_header_gmsh(mesh_t *mesh, FILE *file);
extern int feenox_mesh_write_mesh_gmsh(mesh_t *mesh, FILE *file, int no_physical_names);
extern int feenox_mesh_write_data_gmsh(mesh_write_t *, mesh_write_dist_t *dist);

extern int feenox_gmsh_read_data_int(mesh_t *, size_t n, int *data, int binary);
extern int feenox_gmsh_read_data_size_t(mesh_t *, size_t n, size_t *data, int binary);
extern int feenox_gmsh_read_data_double(mesh_t *, size_t n, double *data, int binary);

// write.c
extern int feenox_instruction_mesh_write(void *arg);

// integrate.c
extern int feenox_instruction_mesh_integrate(void *arg);
extern double feenox_mesh_integral_over_element(element_t *, mesh_t *mesh, function_t *function);
extern double feenox_mesh_integral_function_cell_cell(function_t *function, mesh_t *mesh, physical_group_t *physical_group);
extern double feenox_mesh_integral_function_general_cell(function_t *function, mesh_t *mesh, physical_group_t *physical_group);
extern double feenox_mesh_integral_function_node_gauss(function_t *function, mesh_t *mesh, physical_group_t *physical_group);
extern double feenox_mesh_integral_function_general_gauss(function_t *function, mesh_t *mesh, physical_group_t *physical_group);
extern double feenox_mesh_integral_expression_cell(expr_t *expr, mesh_t *mesh, physical_group_t *physical_group);
extern double feenox_mesh_integral_expression_gauss(expr_t *expr, mesh_t *mesh, physical_group_t *physical_group);

// extrema.c
extern int feenox_instruction_mesh_find_extrema(void *arg);

// physical_group.c
extern int feenox_define_physical_group(const char *name, const char *mesh_name, int dimension, int tag);
extern physical_group_t *feenox_define_physical_group_get_ptr(const char *name, mesh_t *mesh, int dimension, int tag);
extern physical_group_t *feenox_get_or_define_physical_group_get_ptr(const char *name, mesh_t *mesh, int dimension, int tag);
extern int feenox_physical_group_set_material(const char *group_name, const char *material_name, mesh_t *mesh);
extern int feenox_physical_group_compute_volume(physical_group_t *, const mesh_t *mesh);

// material.c
extern material_t *feenox_get_material_ptr(const char *name);

extern int feenox_define_material(const char *material_name, const char *mesh_name);
extern material_t *feenox_define_material_get_ptr(const char *name, mesh_t *mesh);

extern int feenox_define_property(const char *name, const char *mesh_name);
extern property_t *feenox_define_property_get_ptr(const char *name, mesh_t *mesh);

extern int feenox_define_property_data(const char *property_name, const char *material_name, const char *expr_string);
extern property_data_t *feenox_define_property_data_get_ptr(property_t *property, material_t *material, const char *expr_string);



// boundary_condition.c
extern bc_t *feenox_get_bc_ptr(const char *name);

extern int feenox_define_bc(const char *bc_name, const char *mesh_name);
extern bc_t *feenox_define_bc_get_ptr(const char *name, mesh_t *mesh);

extern int feenox_add_bc_data(const char *bc_name, const char *string);
extern bc_data_t *feenox_add_bc_data_get_ptr(bc_t *bc, const char *string);


// init.c
extern int feenox_mesh_element_types_init(void);
extern int feenox_mesh_elements_info(void);
extern int feenox_mesh_elements_info_geo(element_type_t *element_type);

// geom.c
extern void feenox_mesh_subtract(const double *a, const double *b, double *c);
extern void feenox_mesh_cross(const double *a, const double *b, double *c);
extern void feenox_mesh_normalized_cross(const double *a, const double *b, double *c);
extern double feenox_mesh_cross_dot(const double *a, const double *b, const double *c);
extern double feenox_mesh_subtract_cross_2d(const double *a, const double *b, const double *c);
extern double feenox_mesh_dot(const double *a, const double *b);
extern double feenox_mesh_subtract_dot(const double *b, const double *a, const double *c);
extern double feenox_mesh_subtract_module(const double *b, const double *a);
#define feenox_mesh_distance(a,b) feenox_mesh_subtract_module((a),(b))
extern double feenox_mesh_subtract_squared_module(const  double *b, const  double *a);
extern double feenox_mesh_subtract_squared_module2d(const  double *b, const  double *a);

extern int feenox_mesh_compute_outward_normal(element_t *element, double n[3]) __attribute__((noinline));

// element.c
extern int feenox_mesh_compute_normal_2d(element_t *e);
extern int feenox_mesh_add_element_to_list(element_ll_t **list, element_t *e);
extern int feenox_mesh_compute_element_barycenter(element_t *e, double barycenter[]);
extern int feenox_mesh_init_nodal_indexes(mesh_t *, int dofs);

// cell.c
extern int feenox_mesh_element2cell(mesh_t *);

// vtk.c
extern int feenox_mesh_read_vtk_field_node(mesh_t *mesh, FILE *fp, const char *name, unsigned int size);
extern int feenox_mesh_write_unstructured_mesh_vtk(mesh_t *mesh, FILE *file);
extern int feenox_mesh_write_header_vtk(mesh_t *mesh, FILE *file);
extern int feenox_mesh_write_vtk_cells(mesh_t *mesh, FILE * file, int with_size);
extern int feenox_mesh_write_vtk_types(mesh_t *mesh, FILE * file);
extern int feenox_mesh_write_mesh_vtk(mesh_t *mesh, FILE *file, int dummy);
extern int feenox_mesh_write_data_vtk(mesh_write_t *this, mesh_write_dist_t *dist);

// vtu.c
extern int feenox_mesh_write_header_vtu(mesh_t *mesh, FILE *file);
extern int feenox_mesh_write_mesh_vtu(mesh_t *, FILE *file, int dummy);
extern int feenox_mesh_write_data_vtu(mesh_write_t *this, mesh_write_dist_t *dist);
extern int feenox_mesh_write_footer_vtu(mesh_write_t *this);

// neighbors.c
extern element_t *feenox_mesh_find_element_volumetric_neighbor(element_t *e)  __attribute__((noinline));
extern int feenox_mesh_count_element_volumetric_neighbors(element_t *e)  __attribute__((noinline));


// init.c
extern int feenox_problem_parse_time_init(void);
extern int feenox_problem_init_runtime_general(void);
extern int feenox_problem_define_solutions(void);
extern int feenox_problem_define_solution_function(const char *name, function_t **function, int is_gradient);
extern int feenox_problem_fill_aux_solution(function_t *function);
#ifdef HAVE_PETSC
extern Mat feenox_problem_create_matrix(const char *name);
extern Vec feenox_problem_create_vector(const char *name); 
#endif
extern int feenox_compute_first_last_element(mesh_t *mesh);

// bulk.c
extern int feenox_problem_build(void);

// solve.c
extern int feenox_instruction_solve_problem(void *arg);

#ifdef HAVE_PETSC
// solve.c
extern int feenox_function_to_phi(function_t *function, Vec phi);
extern int feenox_problem_phi_to_solution(Vec phi, int gradients);

// petsc_ksp.c
extern int feenox_problem_solve_petsc_linear(void);
extern PetscErrorCode feenox_problem_ksp_monitor(KSP ksp, PetscInt n, PetscReal rnorm, void *dummy);
extern int feenox_problem_setup_ksp(KSP ksp);
extern int feenox_problem_setup_pc(PC pc);

// petsc_snes.c
extern int feenox_problem_solve_petsc_nonlinear(void);
extern int feenox_problem_setup_snes(SNES snes);
extern PetscErrorCode feenox_snes_residual(SNES snes, Vec phi, Vec r, void *ctx);
extern PetscErrorCode feenox_snes_jacobian(SNES snes,Vec phi, Mat J, Mat P, void *ctx);
extern PetscErrorCode feenox_snes_monitor(SNES snes, PetscInt n, PetscReal rnorm, void *dummy);

// petsc_ts.c
extern int feenox_problem_solve_petsc_transient(void);
extern int feenox_problem_setup_ts(TS ts);
extern PetscErrorCode feenox_ts_residual(TS ts, PetscReal t, Vec phi, Vec phi_dot, Vec r, void *ctx);
extern PetscErrorCode feenox_ts_jacobian(TS ts, PetscReal t, Vec T, Vec T_dot, PetscReal s, Mat J, Mat P,void *ctx);

// slepc_eps.c
extern int feenox_problem_solve_slepc_eigen(void);

// dirichlet.c
extern int feenox_problem_dirichlet_add(size_t j_global, unsigned int g, double value);
extern int feenox_problem_multifreedom_add(size_t index, double *coefficients);
extern int feenox_problem_mimicked_add(size_t index_guide, size_t index_follower, double coefficient_follower);
extern int feenox_problem_dirichlet_eval(void);
extern int feenox_problem_dirichlet_set_K(void);
extern int feenox_problem_dirichlet_set_M(void);
extern int feenox_problem_dirichlet_set_J(Mat J);
extern int feenox_problem_dirichlet_set_r(Vec r, Vec phi);
extern int feenox_problem_dirichlet_set_phi(Vec phi);
extern int feenox_problem_dirichlet_set_phi_dot(Vec phi_dot);
extern int feenox_problem_dirichlet_compute_scale(void);

#endif


// fem.c
extern int feenox_fem_elemental_caches_reset(void);
extern material_t *feenox_fem_get_material(element_t *e);
extern double *feenox_fem_compute_x_at_gauss(element_t *e, unsigned int q, int integration);
extern double *feenox_fem_compute_x_at_gauss_if_needed(element_t *e, unsigned int q, int integration,  int condition);
extern double *feenox_fem_compute_x_at_gauss_if_needed_and_update_var(element_t *e, unsigned int q, int integration, int condition);
extern double *feenox_fem_compute_x_at_gauss_and_update_var(element_t *e, unsigned int q, int integration);

extern double feenox_fem_determinant(const gsl_matrix *);
extern gsl_matrix *feenox_fem_matrix_invert(const gsl_matrix *direct, gsl_matrix *inverse);

extern double feenox_fem_compute_w_det_at_gauss_integration(element_t *e, unsigned int q, int integration);
#define feenox_fem_compute_w_det_at_gauss(e, q) feenox_fem_compute_w_det_at_gauss_integration((e), (q), feenox.pde.mesh->integration)

extern gsl_matrix *feenox_fem_compute_C(element_t *e);
extern gsl_matrix *feenox_fem_compute_H_c_at_gauss(element_t *e, unsigned int q, int integration);
extern gsl_matrix *feenox_fem_compute_H_Gc_at_gauss(element_t *e, unsigned int q, int integration);

extern gsl_matrix *feenox_fem_compute_invJ_at_gauss(element_t *e, unsigned int q, int integration);
extern gsl_matrix *feenox_fem_compute_J(element_t *e, double *xi);
extern gsl_matrix *feenox_fem_compute_J_at_gauss(element_t *e, unsigned int q, int integration);
extern gsl_matrix *feenox_fem_compute_J_at_gauss_1d(element_t *e, unsigned int q, int integration, gsl_matrix *J);
extern gsl_matrix *feenox_fem_compute_J_at_gauss_2d(element_t *e, unsigned int q, int integration, gsl_matrix *J);
extern gsl_matrix *feenox_fem_compute_J_square_at_gauss(element_t *e, unsigned int q, int integration, gsl_matrix *J);

extern gsl_matrix *feenox_fem_compute_B_c(element_t *e, double *xi);
extern gsl_matrix *feenox_fem_compute_B(element_t *e, double *xi);

extern gsl_matrix *feenox_fem_compute_B_at_gauss_integration(element_t *e, unsigned int q, int integration);
#define feenox_fem_compute_B_at_gauss(e, q) feenox_fem_compute_B_at_gauss_integration((e), (q), feenox.pde.mesh->integration)

extern gsl_matrix *feenox_fem_compute_B_Gc_at_gauss(element_t *e, unsigned int q, int integration);
extern gsl_matrix *feenox_fem_compute_B_G_at_gauss(element_t *e, unsigned int q, int integration);

#ifdef HAVE_PETSC
extern PetscInt *feenox_fem_compute_dof_indices(element_t *e, int G);
#endif

#define feenox_fem_update_coord_vars(val) {\
  feenox_var_value(feenox.mesh.vars.x) = val[0];\
  feenox_var_value(feenox.mesh.vars.y) = val[1];\
  feenox_var_value(feenox.mesh.vars.z) = val[2];\
}

// blas.c
extern int feenox_blas_Ab(const gsl_matrix *A, const gsl_vector *b, const double alpha, gsl_vector *c);
extern int feenox_blas_Atb(const gsl_matrix *A, const gsl_vector *b, const double alpha, gsl_vector *c);
extern int feenox_blas_Ab_accum(const gsl_matrix *A, const gsl_vector *b, const double alpha, gsl_vector *c);
extern int feenox_blas_Atb_accum(const gsl_matrix *A, const gsl_vector *b, const double alpha, gsl_vector *c);
extern int feenox_blas_BtB_accum(const gsl_matrix *B, double alpha, gsl_matrix *R);
extern int feenox_blas_BtB(const gsl_matrix *B, double alpha, gsl_matrix *R);
extern int feenox_blas_ABt(const gsl_matrix *A, const gsl_matrix *B, const double alpha, gsl_matrix *R);
extern int feenox_blas_ABt(const gsl_matrix *A, const gsl_matrix *B, const double alpha, gsl_matrix *R);
extern int feenox_blas_BtCB(const gsl_matrix *B, const gsl_matrix *C, gsl_matrix *CB, const double alpha, gsl_matrix *R);
extern int feenox_blas_BtCB_accum(const gsl_matrix *B, const gsl_matrix *C, gsl_matrix *CB, const double alpha, gsl_matrix *R);
extern int feenox_blas_BCBt(const gsl_matrix *B, const gsl_matrix *C, gsl_matrix *CB, const double alpha, gsl_matrix *R);
extern int feenox_blas_PtCB_accum(const gsl_matrix *P, const gsl_matrix *C, const gsl_matrix *B, gsl_matrix *CB, const double alpha, gsl_matrix *R);
extern int feenox_blas_PtCB(const gsl_matrix *P, const gsl_matrix *C, const gsl_matrix *B, gsl_matrix *CB, const double alpha, gsl_matrix *R);



// distribution.c
extern int feenox_distribution_init(distribution_t *, const char *name);
extern double feenox_distribution_eval(distribution_t *, const double *x, material_t *material);
extern double feenox_distribution_eval_property(distribution_t *, const double *x, material_t *material);
extern double feenox_distribution_eval_function_global(distribution_t *, const double *x, material_t *material);
extern double feenox_distribution_eval_function_local(distribution_t *, const double *x, material_t *material);
extern double feenox_distribution_eval_variable_global(distribution_t *, const double *x, material_t *material);
extern double feenox_distribution_eval_variable_local(distribution_t *, const double *x, material_t *material);

extern int feenox_pull_dependencies_variables_function(var_ll_t **to, function_t *function);
extern int feenox_pull_dependencies_functions_function(function_ll_t **to, function_t *function);

extern int feenox_depends_on_space(var_ll_t *variables);
extern int feenox_depends_on_normal(var_ll_t *variables);
extern int feenox_depends_on_time(var_ll_t *variables);
extern int feenox_depends_on_function(function_ll_t *functions, function_t *function);

extern int feenox_expression_depends_on_space(expr_t *expr);
extern int feenox_expression_depends_on_normal(expr_t *expr);


// build.c
extern int feenox_problem_build(void);
extern int feenox_problem_build_element_volumetric(element_t *);
extern int feenox_problem_build_element_natural_bc(element_t *, bc_data_t *bc_data);
extern int feenox_problem_build_allocate_elemental_objects(element_t *);
extern int feenox_problem_build_element_volumetric_gauss_point(element_t *, unsigned int q);
extern int feenox_problem_build_elemental_objects_allocate(element_t *);
extern int feenox_problem_build_elemental_objects_free(void);
extern int feenox_problem_build_assemble(void);
extern int feenox_problem_rhs_add(element_t *e, unsigned int q, double *value);
extern int feenox_problem_stiffness_add(element_t *e, unsigned int q, gsl_matrix *Kiq);

// gradient.c
extern int feenox_problem_gradient_compute(void);
extern int feenox_problem_gradient_compute_at_element(element_t *, mesh_t *mesh);
extern int feenox_problem_gradient_smooth_at_node(node_t *node);

// pdes/parse.c
extern int feenox_pde_parse_problem_type(const char *token);

// cleanup.c
extern void feenox_expression_destroy(expr_t *expr);

// include problem-dependent virtual methods
#include "pdes/methods.h"

#endif    /* FEENOX_H  */
