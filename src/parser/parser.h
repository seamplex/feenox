/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX parser header
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

#ifndef FEENOX_PARSER_H
#define FEENOX_PARSER_H

// token delimiters
#define UNQUOTED_DELIM    " \t\n"
#define QUOTED_DELIM      "\""

extern const char operators[];
extern const char factorseparators[];

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


struct {
  size_t page_size;
  size_t actual_buffer_size; 
  
  int implicit_none;
  
  char *line;
  
// this is a pointer to an internal buffer to have "memory" for strtoken
// so we can parse stuff between quotation marks as a single token
  char *strtok_internal;

  
} feenox_parser;


// parser.c
extern int feenox_parse_input_file(const char *filepath, int from, int to);
extern int feenox_parse_line(void);
extern int feenox_parse_include(void);
extern int feenox_parse_default_argument_value(void);
extern int feenox_parse_implicit(void);


// parseraux.c
extern int feenox_read_line(FILE *file_ptr);
extern char *feenox_get_next_token(char *line);
extern void feenox_strip_blanks(char *string);
extern void feenox_add_leading_zeros(char **string);

extern int feenox_parse_expression(const char *string, expr_t *expr);
extern int feenox_parser_expression_in_string(double *result);
extern int feenox_parse_madeup_expression(char *string, expr_factor_t *factor);
extern int feenox_parse_factor(char *string, expr_factor_t *factor);

extern char *feenox_ends_in_init(char *name);
extern char *feenox_ends_in_zero(char *name);

extern int feenox_parser_keywords_ints(char *keyword[], int *value, int *option);

#endif  