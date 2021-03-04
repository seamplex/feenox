/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX parser header
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
 *  along with FeenoX.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */

#ifndef FEENOX_PARSER_H
#define FEENOX_PARSER_H

// token delimiters
#define UNQUOTED_DELIM    " \t\n"
#define QUOTED_DELIM      "\""

extern const char operators[];
extern const char factorseparators[];

typedef struct feenox_parser_t feenox_parser_t;

struct feenox_parser_t {
  size_t page_size;
  size_t actual_buffer_size; 
  
  int implicit_none;
  int reading_daes;
  
  char *line;
  char *full_line; // strtok breaks line so we keep a copy with the full one
  
// this is a pointer to an internal buffer to have "memory" for strtoken
// so we can parse stuff between quotation marks as a single token
  char *strtok_internal;

  conditional_block_t *active_conditional_block;  
};

// parser.c
extern int feenox_parse_input_file(const char *filepath, int from, int to);
extern int feenox_parse_line(void);

// auxiliary.c
extern int feenox_read_line(FILE *file_ptr);
extern char *feenox_get_next_token(char *line);

extern int feenox_parser_string(char **string);
extern int feenox_parser_expression(expr_t *expr);
extern int feenox_parser_expressions(expr_t *expr[], size_t n);
extern int feenox_parser_expression_in_string(double *result);
extern int feenox_parser_string_format(char **string, int *n_args);

extern int feenox_add_function_from_string(const char *string, char **name);

extern int feenox_parser_file(file_t **file);

extern int feenox_parser_vector(vector_t **vector);
extern int feenox_parser_variable(var_t **var);
extern int feenox_parser_keywords_ints(char *keyword[], int *value, int *option);
extern int feenox_parser_match_keyword_expression(char *token, char *keyword[], expr_t *expr[], size_t n);

extern int feenox_parse_include(void);
extern int feenox_parse_default_argument_value(void);
extern int feenox_parse_abort(void);
extern int feenox_parse_implicit(void);
extern int feenox_parse_time_path(void);
extern int feenox_parse_initial_conditions(void);
extern int feenox_parse_variables(void);
extern int feenox_parse_alias(void);
extern int feenox_parse_vector(void);
extern int feenox_parse_matrix(void);
extern int feenox_parse_function(void);

extern int feenox_parse_sort_vector(void);

extern int feenox_parse_file(char *mode);
extern int feenox_parse_open_close(const char *what);

extern int feenox_parse_print(void);
extern int feenox_parse_print_function(void);

extern int feenox_parse_if(void);
extern int feenox_parse_else(void);
extern int feenox_parse_endif(void);

extern int feenox_parse_phase_space(void);

extern int feenox_parse_read_mesh(void);
extern int feenox_parse_write_mesh(void);
extern int feenox_parse_material(void);
extern int feenox_parse_physical_group(void);

#endif  
