" Vim syntax file
" Language: FeenoX input file
" Maintainer: jeremy Theler
" Latest Revision: 19 March 2022

if exists("b:current_syntax")
  finish
endif

syn keyword feeKeyword ABORT ALIAS ALLOW_NEW_NONZEROS ALLOW_UNRESOLVED_BCS APPEND AS ASCENDING ASCENDING_ORDER ASCII_FILE ASCII_FILE_PATH AXISYMMETRIC BC BINARY_FILE BINARY_FILE_PATH BOUNDARY_CONDITION CACHE_B CACHE_J CELL CELLS CLOSE COLS COLUMNS COMPUTE_REACTION DATA DEFAULT_ARGUMENT_VALUE DESCENDING DESCENDING_ORDER DETECT_HANGING_NODES DETECT_UNRESOLVED_BCS DIM DIMENSION DIMENSIONS DIRICHLET_SCALING DUMP EIGEN_DIRICHLET_ZERO EIGEN_FORMULATION EIGEN_SOLVER ELSE ENDIF EPS EPSABS EPSREL EPS_TYPE FILE FILE_FORMAT FILE_PATH FIND_EXTREMA FIT FOR FORMAT FROM FUNCTION FUNCTION_DATA GAUSS GRADIENT GROUP GROUPS HANDLE_HANGING_NODES HEADER HISTORY ID IF IGNORE_NULL I_MAX I_MIN IMPLICIT INCLUDE INITIAL_CONDITIONS INITIAL_CONDITIONS_MODE INPUT INPUT_FILE INTEGRATE INTEGRATION INTERPOLATION INTERPOLATION_THRESHOLD IS K K_bc KSP KSP_TYPE LABEL LINEAR LINEARIZE_STRESS LINEAR_SOLVER M MAIN MATERIAL MATRIX MAX MAX_ITER M_bc MESH METHOD MIN MODE MODES MOMENT N NAME NODE NODES NO_MESH NOMESH NONEWLINE NON_LINEAR NON_LINEAR_SOLVER NONLINEAR_SOLVER NO_PHYSICAL_NAMES NSTEPS OFFSET OPEN OUTPUT OUTPUT_FILE OVER PATH PC PC_TYPE PETSC_OPTIONS PHASE_SPACE PHYSICAL_ENTITY PHYSICAL_GROUP PREALLOCATE PRECONDITIONER PRINT PRINTF PRINTF_ALL PRINT_FUNCTION PRINT_VECTOR PROBLEM PROGRESS PROGRESS_ASCII QUASISTATIC RANGE_MAX RANGE_MIN REACTION READ READ_FIELD READ_FUNCTION READ_MESH READ_SYMMETRIC_TENSOR READ_VECTOR RE_READ RESULT ROWS SCALE SEM SEMAPHORE SEP SEPARATOR SHEPARD_EXPONENT SHEPARD_RADIUS SHM SHM_OBJECT SIZE SIZES SN SNES SNES_TYPE SOLVE SOLVE_PROBLEM SORT_VECTOR SPECTRAL_TRANSFORMATION ST STEP STRING ST_TYPE SYMMETRIC_TENSOR SYMM_TENSOR TEXT TIME_ADAPTATION TIME_PATH TO TOL_ABS TOL_REL TRANSIENT TRANSIENT_SOLVER TS TS_ADAPT TS_ADAPT_TYPE TS_TYPE UNKNOWNS UPDATE_EACH_STEP VAR VARIABLE VARIABLES VARS VECTOR VECTORS VECTOR_SORT VERBOSE VIA WRITE WRITE_MESH WRITE_RESULTS X0 X_INCREASES_FIRST X_MAX X_MIN Y0 Y_MAX Y_MIN Z0 Z_MAX Z_MIN ALLOWED AS_PROVIDED K M NONE POST SKIP_HEADER_STEP SKIP_STATIC_STEP SKIP_STEP SKIP_TIME WAIT
syn keyword feeVariable dae_rtol done done_static done_transient dont_quit dont_report dt end_time eps_max_it eps_st_nu eps_st_sigma eps_tol gamg_threshold i infinite in_static in_static_first in_static_last in_transient in_transient_first in_transient_last j keff ksp_atol ksp_divtol ksp_max_it ksp_rtol max_dt memory_available min_dt mpi_rank mpi_size mumps_icntl_14 nodes_rough on_gsl_error on_nan on_sundials_error penalty_weight pi pid quit realtime_scale report sn_alpha snes_atol snes_max_it snes_rtol snes_stol static_steps step_static step_transient t total_dofs ts_atol ts_rtol zero
syn keyword feeFunction abs acos asin atan atan2 ceil clock cos cosh cpu_time d_dt deadband derivative equal exp expint1 expint2 expint3 expintn floor func_min gammaf gauss_kronrod gauss_legendre heaviside if integral integral_dt integral_euler_dt is_even is_in_interval is_odd j0 lag lag_bilinear lag_euler last limit limit_dt log mark_max mark_min max memory min mod mpi_memory_global mpi_memory_local not prod quasi_random random random_gauss root round sawtooth_wave sgn sin sinh sqrt square_wave sum tan tanh threshold_max threshold_min triangular_wave vecdot vecmax vecmaxindex vecmin vecminindex vecnorm vecsize vecsum wall_time
" Numbers, allowing signs (both -, and +)
" Integer number.
syn match  feeNumber		display "[+-]\=\<\d\+\>"
" Floating point number.
syn match  feeFloat		display "[+-]\=\<\d\+\.\d+\>"
" Floating point number, starting with a dot.
syn match  feeFloat		display "[+-]\=\<.\d+\>"
syn case ignore
"floating point number, with dot, optional exponent
syn match  feeFloat	display "\<\d\+\.\d*\(e[-+]\=\d\+\)\=\>"
"floating point number, starting with a dot, optional exponent
syn match  feeFloat	display "\.\d\+\(e[-+]\=\d\+\)\=\>"
"floating point number, without dot, with exponent
syn match  feeFloat	display "\<\d\+e[-+]\=\d\+\>"
syn case match


syn match  feeComment     "#.*"

" Expression separators: ';' and ','
syn match  feeSemicolon   ";"
syn match  feeComma       ","
syn match  feeLineSkip    "\\$"

syn match  feeOperator     /+\||\|\.\.\|-\|(\|)\|{\|}\|\[\|\]\|=\|<\|>/



" Define the default highlighting.
hi def link feeKeyword     Statement
hi def link feeFunction    Function
hi def link feeVariable    Identifier
hi def link feeComment     Comment
hi def link feeSemicolon   Special
hi def link feeComma       Special
hi def link feeLineSkip    Special
hi def link feeOperator    Operator
hi def link feeNumber      Float
hi def link feeFloat       Float

let b:current_syntax = "fee"
