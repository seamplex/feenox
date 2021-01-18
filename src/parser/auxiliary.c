/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox auxiliary parsing routines
 *
 *  Copyright (C) 2009--2021 jeremy theler
 *
 *  This file is part of feenox.
 *
 *  feenox is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  feenox is distributed in the hope that it will be useful,
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
#include "parser.h"
extern feenox_parser_t feenox_parser;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>



const char operators[]        = "&|=!<>+-*/^()";
const char factorseparators[] = "&|=!<>+-*/^(), \t\n";


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
          // no se puede meter en una linea porque me puede llegar a meter
          // el \0 en line[] antes de evaluar la condicion para salir del while
          feenox_parser.line[i++] = feenox.argv[feenox.optind+n][j++];
        }

      } else if (c == '\\') {

        switch (c = fgetc(file_ptr)) {
          case '"':
            // if there's an escaped quote, we take away the escape char and put
            // a magic marker 0x1e, afterwards in get_next_token() we change back
            // te 0x1e with the unescaped quote
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
      }
    }

    // escape from the "in comment" state if there's a newline
    if (c == '\n') {
      in_comment = 0;
    }

    // check if we need to reallocate the input buffer
    if (i >= feenox_parser.actual_buffer_size-16) {
      feenox_parser.actual_buffer_size += feenox_parser.page_size;
      if ((feenox_parser.line = realloc(feenox_parser.line, feenox_parser.actual_buffer_size)) == NULL) {
        feenox_push_error_message("out of memory");
        return FEENOX_ERROR;
      }
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
  
    feenox_call(feenox_expression_parse(expr[i], token));
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
  
  *result = feenox_evaluate_expression_in_string(token);

  return FEENOX_OK;
}


int feenox_parser_string(char **string) {
  
  char *token;
  
  if ((token = feenox_get_next_token(NULL)) == NULL) {
    feenox_push_error_message("expected string");
    return FEENOX_ERROR;
  }
  
  *string = strdup(token);
  
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
  *string = strdup(token);
      
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
    feenox_push_error_message("undefined file identifier '%s'", token);
    return FEENOX_ERROR;
  }
  
  // XXXXX
  
  return FEENOX_OK;
}

/*
int feenox_parser_file_path(file_t **file, char *mode) {
  
  char *token;
  if (feenox_parser_string(&token) != FEENOX_OK) {
    return FEENOX_ERROR;
  };
  
  if ((*file = feenox_define_file(token, token, 0, NULL, mode, 0)) == NULL) {
    return WASORA_RUNTIME_ERROR;
  }
  free(token);
  
  return FEENOX_OK;
}

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

  free(buff);

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
void feenox_strip_blanks(char *string) {
  int i = 0;
  int j = 0;
  char *buff;

  buff = strdup(string);

  for (i = 0; i < strlen(string); i++) {
    if (!isspace((int)buff[i])) {
      string[j++] = buff[i];
    }
  }

  string[j] = '\0';
  free(buff);

  return;

}


char *feenox_ends_in_zero(char *name) {

  char *dummy;

  if (((dummy = strstr(name, "_0")) != 0) && (*(dummy+2) == 0)) {
    return dummy;
  } else {
    return NULL;
  }
  
}

char *feenox_ends_in_init(char *name) {

  char *dummy;

  if (((dummy = strstr(name, "_init")) != 0) && (dummy[5] == '\0' || dummy[5] == '(')) {
    return dummy;
  } else {
    return NULL;
  }
}


char *feenox_ends_in_dot(char *name) {

  char *dummy;

  if (((dummy = strstr(name, "_dot")) != 0) && (dummy[4] == '\0' || dummy[4] == '(')) {
    return dummy;
  } else {
    return NULL;
  }
}

int feenox_count_arguments(char *string) {

  int n_arguments;
  int level;
  char *dummy;

  // arguments have to be inside parenthesis
  if (string[0] != '(') {
    feenox_push_error_message("expected arguments for function");
    return -1;
  }

  // count how many arguments are there (take into account nested parenthesis)
  dummy = string+1;
  level = 1;
  n_arguments = 1;
  while (level != 0) {
    if (*dummy == '(') {
      level++;
    } else if (*dummy == ')') {
      level--;
    } else if (*dummy == '\0') {
      feenox_push_error_message("argument list needs to be closed with ')'");
      return -1;
    }
    if (*dummy == ',' && level == 1) {
      n_arguments++;
    }
    dummy++;
  }
  *dummy = '\0';
  
  return n_arguments;
}

int feenox_read_arguments(char *string, int n_arguments, char ***arg_name) {
 
  char *dummy;
  char *dummy_openpar = NULL;
  char *dummy_closepar = NULL;
  char char_backup;
  
  if ((dummy_openpar = strchr(string, '(')) == NULL) {
    feenox_push_error_message("arguments must start with a parenthesis");
    return FEENOX_ERROR;
  }
  *dummy_openpar = '\0';

  if (((*arg_name) = calloc(n_arguments, sizeof(char *))) == NULL) {
    feenox_push_error_message("calloc() failed");
    return FEENOX_ERROR;
  }
  
  if (n_arguments == 1) {
    
    // if there's a single argument, that's easy
    *arg_name[0] = strdup(string+1);
    if ((dummy_closepar = strchr(*arg_name[0], ')')) == NULL) {
      feenox_push_error_message("expected ')' after function name (no spaces allowed)");
      return FEENOX_ERROR;
    }
    *dummy_closepar = '\0';

  } else {
    
    // otherwise one at a time
    int i;
    int level;
    
    for (i = 0; i < n_arguments; i++) {
      if (i != n_arguments-1) {

        // if we are not on the last argument, we need to put a ")\0" where it ends
        level = 1;
        dummy = string+1;
        while (1) {
          if (*dummy == ',' && level == 1) {
            break;
          }
          if (*dummy == '(') {
            level++;
          } else if (*dummy == ')') {
            level--;
          } else if (*dummy == '\0') {
            feenox_push_error_message("when parsing arguments");
            return FEENOX_ERROR;
          }
          dummy++;
        }

        // put a '\0' after dummy but make a backup of what there was there
        char_backup = *dummy;
        *dummy = '\0';
        
      } else {

        // strip the final ')'
        *(dummy+strlen(dummy)-1) = '\0';

      }


      // en dummy_argument+1 tenemos el argumento i-esimo
      (*arg_name)[i] = strdup(string+1);


      if (i != n_arguments-1) {
        *(dummy) = char_backup;
        string = dummy;
      }
    }
  }  
  
  return FEENOX_OK;
}

/*
int feenox_parse_assignment(char *line, assignment_t *assignment) {
  
  char *dummy;
  char *left_hand;
  char *right_hand;
  vector_t *vector = NULL;
  matrix_t *matrix = NULL;
  char *dummy_at;
  char *at_expression;
  int at_offset;
  char *dummy_time_range;
  char *dummy_index_range;
  char *dummy_par;
  char *dummy_init;
  char *dummy_0;
  
  // comenzamos suponiendo que es plain
  assignment->plain = 1;
  
  // left hand
  left_hand = strdup(line);
  dummy = strchr(left_hand, '=');
  *dummy = '\0';
  
  // primero hacemos que el right hand sea dummy+1
  right_hand = strdup(dummy+1);
  feenox_strip_blanks(right_hand);
  if (strlen(right_hand) == 0) {
    feenox_push_error_message("empty right-hand side");
    return FEENOX_ERROR;
  }
  
  // y despues sacamos blancos y otras porquerias que nos molestan
  feenox_strip_blanks(left_hand);
  
  // este es el orden inverso en el que se tienen que dar los subindices
  // por ejemplo un construcciones validas serian
  // matrix_0(i,j)<1:3;2:4> = algo
  // matrix(i,j)<1:3;2:4>[0.99:1.01] = algo
  
  // como ultima cosa, la arroba para hacer steps
  if ((dummy_at = strchr(left_hand, '@')) != NULL) {
    at_offset = dummy_at - left_hand;
    at_expression = strdup(dummy_at+1);
    left_hand = realloc(left_hand, strlen(left_hand) + 2*strlen(at_expression) + 32);
    sprintf(left_hand+at_offset, "[(%s)-0.1*dt:%s+(0.1)*dt]", at_expression, at_expression);
    free(at_expression);
  }
  
  // como anteultima cosa, el rango en tiempo
  if ((dummy_time_range = strchr(left_hand, '[')) != NULL) {
    *dummy_time_range = '\0';
  }
  
  // el rango en indices
  if ((dummy_index_range = strchr(left_hand, '<')) != NULL) {
    *dummy_index_range = '\0';
  }
  
  // despues los subindices entre parentesis si es un vector o matriz
  if ((dummy_par = strchr(left_hand, '(')) != NULL) {
    *dummy_par = '\0';
  }
  
  // despues _init o _0
  if ((dummy_init = feenox_ends_in_init(left_hand)) != NULL) {
    *dummy_init = '\0';
    assignment->initial_static = 1;
  }
  
  if ((dummy_0 = feenox_ends_in_zero(left_hand)) != NULL) {
    *dummy_0 = '\0';
    assignment->initial_transient = 1;
  }
  
  if ((matrix = feenox_get_matrix_ptr(left_hand)) != NULL) {
    assignment->matrix = matrix;
  } else if ((vector = feenox_get_vector_ptr(left_hand)) != NULL) {
    assignment->vector = vector;
  } else {
    // buscamos una variable, y si no existe vemos si tenemos que declararla implicitamente
    if ((assignment->variable = feenox_get_variable_ptr(left_hand)) == NULL) {
      if (feenox.implicit_none) {
        feenox_push_error_message("undefined symbol '%s' and disabled implicit definition", left_hand);
        return FEENOX_ERROR;
      } else {
        if ((assignment->variable = feenox_define_variable(left_hand)) == NULL) {
          return FEENOX_ERROR;
        } else {
          // las variables siempre son escalares
          assignment->plain = 0;
          assignment->scalar = 1;
        }
      }
    } else {
      // las variables siempre son escalares
      assignment->plain = 0;
      assignment->scalar = 1;
    }
  }
  
  // ahora vamos de izquierda a derecha
  // reconstruimos el string
  if (dummy_0 != NULL) {
    *dummy_0 = '_';
  }
  if (dummy_init != NULL) {
    *dummy_init = '_';
  }
  
  // si hay parentesis son subindices
  if (dummy_par != NULL) {
    if (matrix == NULL && vector == NULL) {
      if (feenox_get_function_ptr(left_hand) != NULL) {
        feenox_push_error_message("functions are defined using ':=' instead of '='", left_hand);  
      } else {
        feenox_push_error_message("'%s' is neither a vector nor a matrix", left_hand);
      }
      return FEENOX_ERROR;
    }
    *dummy_par = '(';
    
    if (matrix != NULL) {
      // si aparecen o la letra "i" o la letra "j" entonces no es escalar
      assignment->scalar = (strchr(dummy_par, 'i') == NULL) && (strchr(dummy_par, 'j') == NULL);
      // si aparece la letra "i" y no la letra "j" entonces se varia solo las filas
      assignment->expression_only_of_i = (strchr(dummy_par, 'j') == NULL) && (strchr(dummy_par, 'i') != NULL);
      // si no aparece la letra "i" pero si la letra "j" entonces se varia solo las columnas
      assignment->expression_only_of_j = (strchr(dummy_par, 'i') == NULL) && (strchr(dummy_par, 'j') != NULL);
      // si no es es "i,j" entonces no es plain
      if (strcmp("(i,j)", dummy_par) != 0) {
        assignment->plain = 0;
      }
      feenox_call(feenox_parse_range(dummy_par, '(', ',', ')', &assignment->row, &assignment->col));
    } else if (vector != NULL) {
      if ((dummy = strrchr(dummy_par, ')')) == NULL) {
        feenox_push_error_message("unmatched parenthesis for '%s'", left_hand);
        return FEENOX_ERROR;
      }
      *dummy = '\0';
      
      // si aparece la letra "i" entonces no es escalar
      assignment->scalar = strchr(dummy_par+1, 'i') == NULL;
      
      // si no es "i" entonces no es plain
      if (strcmp("i", dummy_par+1) != 0) {
        assignment->plain = 0;
      }
      feenox_call(feenox_parse_expression(dummy_par+1, &assignment->row));
      *dummy = ')';
    }
  }
  
  // si hay rango de indices
  if (dummy_index_range != NULL) {
    if (matrix == NULL && vector == NULL) {
      if (feenox_get_function_ptr(left_hand) != NULL) {
        feenox_push_error_message("functions are defined using ':=' instead of '='", left_hand);  
      } else {
        feenox_push_error_message("'%s' is neither a vector nor a matrix", left_hand);
      }
      return FEENOX_ERROR;
    }
    *dummy_index_range = '<';
    
    if (matrix != NULL) {
      feenox_call(feenox_parse_range(dummy_index_range, '<', ':', ';', &assignment->i_min, &assignment->i_max));
      feenox_call(feenox_parse_range(strchr(dummy_index_range, ';'), ';', ':', '>', &assignment->j_min, &assignment->j_max));
    } else if (vector != NULL) {
      feenox_call(feenox_parse_range(dummy_index_range, '<', ':', '>', &assignment->i_min, &assignment->i_max));
    }
  }
  
  // rango de tiempo
  if (dummy_time_range != NULL)  {
    *dummy_time_range = '[';
    feenox_call(feenox_parse_range(dummy_time_range, '[', ':', ']', &assignment->t_min, &assignment->t_max));
  }
  
  feenox_call(feenox_parse_expression(right_hand, &assignment->rhs));
  
  free(left_hand);
  free(right_hand);
  
  return WASORA_RUNTIME_OK;
}
*/
