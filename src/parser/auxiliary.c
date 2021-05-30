/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX auxiliary parsing routines
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
#include "feenox.h"
extern feenox_t feenox;
#include "parser.h"
extern feenox_parser_t feenox_parser;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


// reads a line from the input file and returns it "nicely-formatted" 
int feenox_read_line(FILE *file_ptr) {

  int i;
  int c;
  int in_comment = 0;
  int in_brackets = 0;
  int lines = 0;

  // ignore trailing whitespace
  do {
    c = fgetc(file_ptr);
    // if we find a newline, then there's nothing for us
    if (c == '\n') {
      feenox_parser.line[0] = '\0';
      return 1;
    }
  } while (isspace(c));

  i = 0;
  while ( !((c == EOF) || (in_brackets == 0 && c == '\n')) ) {
    if (in_comment == 0) {
      if (c == '#' || c == ';') {
        in_comment = 1;
      } else if (c == '{') {
        in_brackets = 1;
      } else if (c == '}') {
        in_brackets = 0;
      } else if (c == '$') {
        // commandline arguments
        int j, n;
        if (fscanf(file_ptr, "%d", &n) != 1) {
          feenox_push_error_message("failed to match $n");
          return (lines==0 && i!=0)?-1:-lines;
        }
        if (feenox.optind+n >= feenox.argc) {
          // call recursively so we finish reading the line
          feenox_read_line(file_ptr);
          // make sure that line[0] is not '\0'
          if (feenox_parser.line[0] == '\0') {
            feenox_parser.line[0] = ' ';
            feenox_parser.line[1] = '\0';
           }
          return (lines==0 && i!=0)?-1:-lines;
        }

        j = 0;
        if (feenox.argv[feenox.optind+n] == NULL) {
          return FEENOX_ERROR;
        }
        while (feenox.argv[feenox.optind+n][j] != 0) {
          // watch out!
          // this cannot be put in a single line because the '\0' might come
          // in line[] before evaluating the condition to get out of the while
          feenox_parser.line[i++] = feenox.argv[feenox.optind+n][j++];
        }

      } else if (c == '\\') {

        switch (c = fgetc(file_ptr)) {
          case '"':
            // if there's an escaped quote, we take away the escape char and put
            // a magic marker 0x1e, afterwards in get_next_token() we change back
            // the 0x1e with the unescaped quote
            feenox_parser.line[i++] = 0x1e;
          break;
            // escape sequences
          case 'a':
            feenox_parser.line[i++] = '\a';
          break;
          case 'b':
            feenox_parser.line[i++] = '\b';
          break;
          case 'n':
            feenox_parser.line[i++] = '\n';
          break;
          case 'r':
            feenox_parser.line[i++] = '\r';
          break;
          case 't':
            feenox_parser.line[i++] = '\t';
          break;
          case 'v':
            feenox_parser.line[i++] = '\v';
          break;
          case '\n':
            // escaped newlines are taken as continuation lines
            lines++; 
          break;
          case '\\':
            // this is taken into account int he default, but just in case
            feenox_parser.line[i++] = '\\';
          break;
          // TODO: hex representation
          default:
            feenox_parser.line[i++] = c;
          break;
        }
      } else if (!in_comment && c != '\n' && c != '\r' && c != EOF) {
        feenox_parser.line[i++] = c;
      } else if (in_brackets && c == '\n') {
        feenox_parser.line[i++] = ' ';
      }
    }

    // escape from the "in comment" state if there's a newline
    if (c == '\n') {
      in_comment = 0;
    }

    // check if we need to reallocate the input buffer
    if (i >= feenox_parser.actual_buffer_size-16) {
      feenox_parser.actual_buffer_size += feenox_parser.page_size;
      feenox_check_alloc(feenox_parser.line = realloc(feenox_parser.line, feenox_parser.actual_buffer_size));
    }

    // pedimos lo que viene en futbol de primera
    // contamos las lineas aca porque si nos toca \n nos vamos sin comerla ni beberla
    if ((c = fgetc(file_ptr)) == '\n') {
      lines++;
    }

  }
  
  feenox_parser.line[i] = '\0';
  
  // if lines is zero but we did read something, we need to return one
  // otherwise the final lines of input files not ending in \n are ignored
  return (lines == 0 && i != 0) ? 1 : lines;

}


int feenox_parser_expression(expr_t *expr) {
  
  char *token;
  
  if ((token = feenox_get_next_token(NULL)) == NULL) {
    feenox_push_error_message("expected expression");
    return FEENOX_ERROR;
  }
  
  if (feenox_expression_parse(expr, token) != FEENOX_OK) {
    return FEENOX_ERROR;
  }
  
  return FEENOX_OK;
}


int feenox_parser_expressions(expr_t *expr[], size_t n) {
  
  char *token;
  int i;
  
  *expr = calloc(n, sizeof(expr_t));
  
  for (i = 0; i < n; i++) {
    if ((token = feenox_get_next_token(NULL)) == NULL) {
      feenox_push_error_message("expected expression");
      return FEENOX_ERROR;
    }
  
    feenox_call(feenox_expression_parse(&((*expr)[i]), token));
  }
  
  return FEENOX_OK;
}

int feenox_parser_match_keyword_expression(char *token, char *keyword[], expr_t *expr[], size_t n) {

  int i;
  int found = 0;
  
  for (i = 0; i < n; i++) {
    if (strcasecmp(token, keyword[i]) == 0) {

      found = 1;
      
      if ((token = feenox_get_next_token(NULL)) == NULL) {
        feenox_push_error_message("expected expression");
        return FEENOX_ERROR;
      }
              
      feenox_call(feenox_expression_parse(expr[i], token));
    }
  }
          
  if (found == 0) {
    return FEENOX_UNHANDLED;
  }
  
  return FEENOX_OK;
}


int feenox_parser_expression_in_string(double *result) {
  
  char *token;
  
  if ((token = feenox_get_next_token(NULL)) == NULL) {
    feenox_push_error_message("expected expression");
    return FEENOX_ERROR;
  }
  
  *result = feenox_expression_evaluate_in_string(token);

  return FEENOX_OK;
}


int feenox_parser_string(char **string) {
  
  char *token;
  
  if ((token = feenox_get_next_token(NULL)) == NULL) {
    feenox_push_error_message("expected string");
    return FEENOX_ERROR;
  }
  
  feenox_check_alloc(*string = strdup(token));
  
  return FEENOX_OK;
}

int feenox_parser_string_format(char **string, int *n_args) {

  char *token;
  char *dummy;
  
  *n_args = 0;
  
  if ((token = feenox_get_next_token(NULL)) == NULL) {
    feenox_push_error_message("expected a string with optional printf format data");
    return FEENOX_ERROR;
  }
  feenox_check_alloc(*string = strdup(token));
      
  dummy = *string;
  while (*dummy != '\0') {
    if (*dummy == '\\') {
      dummy++;
    } else if (*dummy == '%' && *(dummy+1) == '%') {
      dummy++;
    } else if (*dummy == '%') {
      if (dummy[1] == 'd') {
        dummy[1] = 'g';
      }
      (*n_args)++;
    }
    dummy++;
  }
  
  return FEENOX_OK;
  
}


int feenox_parser_file(file_t **file) {
  
  char *token;
  
  if ((token = feenox_get_next_token(NULL)) == NULL) {
    feenox_push_error_message("expected file identifier");
    return FEENOX_ERROR;
  }

  if ((*file = feenox_get_file_ptr(token)) == NULL) {
    feenox_call(feenox_define_file(token, token, 0, NULL));
    if ((*file = feenox_get_file_ptr(token)) == NULL) {
      return FEENOX_ERROR;
    }
  }
  return FEENOX_OK;
}

/*
int feenox_parser_function(function_t **function) {
  
  char *token;
  
  if ((token = feenox_get_next_token(NULL)) == NULL) {
    feenox_push_error_message("expected function name");
    return FEENOX_ERROR;
  }

  if ((*function = feenox_get_function_ptr(token)) == NULL) {
    feenox_push_error_message("undefined function identifier '%s' (remember that only the function name is needed, not the arguments)", token);
    return FEENOX_ERROR;
  }
  
  return FEENOX_OK;
}
*/
int feenox_parser_vector(vector_t **vector) {
  
  char *token;
  
  if ((token = feenox_get_next_token(NULL)) == NULL) {
    feenox_push_error_message("expected vector name");
    return FEENOX_ERROR;
  }

  if ((*vector = feenox_get_vector_ptr(token)) == NULL) {
    feenox_push_error_message("undefined vector identifier '%s'", token);
    return FEENOX_ERROR;
  }
  
  return FEENOX_OK;
}

int feenox_parser_variable(var_t **var) {
  
  char *token;
  
  if ((token = feenox_get_next_token(NULL)) == NULL) {
    feenox_push_error_message("expected variable name");
    return FEENOX_ERROR;
  }

  if ((*var = feenox_get_variable_ptr(token)) == NULL) {
    feenox_push_error_message("undefined variable identifier '%s'", token);
    return FEENOX_ERROR;
  }
  
  return FEENOX_OK;
}


int feenox_parser_keywords_ints(char *keyword[], int *value, int *option) {

  char *token;
  int i = 0;
  
  if ((token = feenox_get_next_token(NULL)) == NULL) {
    while (keyword[i][0] != '\0') {
      feenox_push_error_message("%s", keyword[i++]);
    }
    feenox_push_error_message("expected one of");
    return FEENOX_ERROR;
  }
  
  while (keyword[i][0] != '\0') {
    if (strcasecmp(token, keyword[i]) == 0) {
      *option = value[i];
      return FEENOX_OK;
    }
    i++;
  }
  
  i = 0;
  while (keyword[i][0] != '\0') {
    feenox_push_error_message("%s", keyword[i++]);
  }
  feenox_push_error_message("unknown keyword '%s', expected one of", token);
  
  return FEENOX_ERROR;
  
}

/*
int feenox_parser_read_keywords_voids(char *keyword[], void *value[], void **option) {

  char *token;
  int i = 0;
  
  if ((token = feenox_get_next_token(NULL)) == NULL) {
    while (keyword[i][0] != '\0') {
      feenox_push_error_message("%s", keyword[i++]);
    }
    feenox_push_error_message("expected one of");
    return FEENOX_ERROR;
  }
  
  while (keyword[i][0] != '\0') {
    if (strcasecmp(token, keyword[i]) == 0) {
      *option = value[i];
      return FEENOX_OK;
    }
    i++;
  }
  
  i = 0;
  while (keyword[i][0] != '\0') {
    feenox_push_error_message("%s", keyword[i++]);
  }
  feenox_push_error_message("unknown keyword '%s', expected one of", token);
  
  return FEENOX_ERROR;
  
}



*/


// slightly-modified strtok() that can take into account if the next token
// starts with a quote (or not) and can read stuff between them
char *feenox_get_next_token(char *line) {

  int i;
  int n;
  char *token;

  // MAMA! lo que renegue para sacar la lectura invalida de este rutina que
  // acusaba el valgrind! el chiste es asi: cuando se termina de parsear una
  // linea vieja, el internal apunta a cualquier fruta (diferente de NULL)
  // entonces no vale mirar que tiene. La solucion que se me ocurrio es que si
  // line es diferente de NULL estamos empezando una linea nueva, eso quiere
  // decir que internal apunta a fruta entonces lo hacemos igual a NULL y usamos
  // los delimitadores sin comillas, porque total es una keyword
  if (line != NULL) {
    feenox_parser.strtok_internal = NULL;
  }


  if (feenox_parser.strtok_internal == NULL) {
    token = strtok_r(line, UNQUOTED_DELIM, &(feenox_parser.strtok_internal));
  } else {
    if (*feenox_parser.strtok_internal == '"') {
      token = strtok_r(line, QUOTED_DELIM, &(feenox_parser.strtok_internal));

      // si es quoted, barremos token y reemplazamos el caracter 0x1E por un quote
      n = strlen(token);
      for (i = 0; i < n; i++) {
        if (token[i] == -1 || token[i] == 0x1e) {
          token[i] = '"';
        }
      }
    } else {
      token = strtok_r(line, UNQUOTED_DELIM, &(feenox_parser.strtok_internal));
    }
  }

  // avanzamos el apuntador strtok_internal hasta el siguiente caracter no-blanco
  // asi si la cadena q sigue empieza con comillas, la ve bien el if de arriba
  // sino si hay muchos espacios en blanco el tipo piensa que el token esta unquoted
  // el if de que no sea null saltaba solamente en windoze, ye ne se pa
  if (feenox_parser.strtok_internal != NULL) {
    while (*feenox_parser.strtok_internal == ' ' || *feenox_parser.strtok_internal == '\t') {
      feenox_parser.strtok_internal++;
    }
  }

  // esto casi nunca pasa, pero si nos quedamos sin tokens entonces otra vez
  // internal apunta a cualquier lado 
  // OJO! a partir de algun update de la libc si dejamos esto tenemos segfault cuando
  // algun primary keyword consume todas las keywords de la linea como PHYSICAL_ENTITY BC strings

//  if (token == NULL) {
//    feenox_parser.strtok_internal = NULL;
//  }

  return token;

}


/*




// parsea el rango de indices 
int feenox_parse_range(char *string, const char left_delim, const char middle_delim, const char right_delim, expr_t *a, expr_t *b) {
  char *first_bracket;
  char *second_bracket;
  char *colon;

  if ((first_bracket = strchr(string, left_delim)) == NULL) {
    feenox_push_error_message("range '%s' does not start with '%c'", string, left_delim);
    return FEENOX_ERROR;
  }

  if ((second_bracket = strrchr(string, right_delim)) == NULL) {
    feenox_push_error_message("unmatched '%c' for range in '%s'", left_delim, string);
    return FEENOX_ERROR;
  }

  *second_bracket = '\0';

  if ((colon = strchr(string, middle_delim)) == NULL) {
    feenox_push_error_message("delimiter '%c' not found when giving range", middle_delim);
    return FEENOX_ERROR;
  }
  *colon = '\0';

  if (feenox_parse_expression(first_bracket+1, a) != 0) {
    feenox_push_error_message("in min range expression");
    return FEENOX_ERROR;
  }
  if (feenox_parse_expression(colon+1, b) != 0) {
    feenox_push_error_message("in max range expression");
    return FEENOX_ERROR;
  }
  
  *second_bracket = right_delim;;
  *colon = middle_delim;
          
  return FEENOX_OK;
}


// saca los comentarios y los espacios en blanco iniciales de una linea
void feenox_strip_comments(char *line) {
  int i = 0;
  int j = 0;
  char *buff = strdup(line);

  // saca los espacios y tabs iniciales
  while (isspace((int)buff[i])) {
    i++;
  }

//  while ((buff[i] != '#' && buff[i] != '\r' && buff[i] != '\n' && buff[i] != '\0') || (i > 0 && buff[i] == '#' && buff[i-1] == '\\')) {
  while ((buff[i] != '#' && buff[i] != '\0') || (i > 0 && buff[i] == '#' && buff[i-1] == '\\')) {  
    if (i > 0 && buff[i] == '#' && buff[i-1] == '\\') {
      j--;
    }
    line[j++] = buff[i++];
  }

  line[j] = '\0';

  feenox_free(buff);

  return;
}



// lee una linea y eventualmente procesa los {}
int feenox_read_data_line(FILE *file_ptr, char *buffer) {

  int l;
  int lines = 1;

  if (fgets(buffer, BUFFER_SIZE*BUFFER_SIZE, file_ptr) == NULL) {
    return 0;
  }
  
  feenox_strip_comments(buffer); // limpiamos la linea

  // TODO: explicar esto!
  // si no tiene "{" entonces se curte
  if (strchr(buffer, '{') == NULL) {
    return 1;
  }

  do {
    l = strlen(buffer);
    if (fgets(buffer + l + 1, (BUFFER_SIZE*BUFFER_SIZE)-1, file_ptr) == 0) {
      return FEENOX_ERROR;
    }
    buffer[l] = ' ';
    feenox_strip_comments(buffer);  // limpiamos la linea
    lines++;
  } while (strchr(buffer, '}') == NULL);

//  feenox_strip_brackets(buffer);

  return lines;

}
*/

// strips the blanks of a string (it modifies the argument string)
int feenox_strip_blanks(char *string) {
  int i = 0;
  int j = 0;
  char *buff;

  feenox_check_alloc(buff = strdup(string));

  for (i = 0; i < strlen(string); i++) {
    if (!isspace((int)buff[i])) {
      string[j++] = buff[i];
    }
  }

  string[j] = '\0';
  feenox_free(buff);

  return FEENOX_OK;

}

// this function parses a string like "f(x,y,z)"
// it creates the function and sets the arguments
// it returns the name of the function in name which should be freed
int feenox_add_function_from_string(const char *string, char **name) {
    
  char *dummy_openpar = strchr(string, '(');
  if (dummy_openpar == NULL) {
    feenox_push_error_message("expecting open parenthesis in '%s'", string);
    return FEENOX_ERROR;
  }
  *dummy_openpar = '\0';
  feenox_check_alloc(*name = strdup(string));
  *dummy_openpar = '(';
  feenox_call(feenox_strip_blanks(*name));

  char *arguments;
  feenox_check_alloc(arguments = strdup(dummy_openpar));
  feenox_call(feenox_strip_blanks(arguments));

  size_t n_arguments;
  if ((n_arguments = feenox_count_arguments(arguments, NULL)) <= 0) {
    return FEENOX_ERROR;
  }
  char **arg_name = NULL;
  feenox_call(feenox_read_arguments(arguments, n_arguments, &arg_name, NULL));
  feenox_free(arguments);

  *dummy_openpar = '\0';
  feenox_call(feenox_define_function(*name, n_arguments));
  int i;
  for (i = 0; i < n_arguments; i++) {
    feenox_call(feenox_function_set_argument_variable(*name, i, arg_name[i]));
  }

  // clean up this (partial) mess
  if (arg_name != NULL) {
    for (i = 0; i < n_arguments; i++) {
      feenox_free(arg_name[i]);
    }
    feenox_free(arg_name);
  }
  
  return FEENOX_OK;
}
