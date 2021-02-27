/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's mesh-related gmsh routines
 *
 *  Copyright (C) 2014--2021 jeremy theler
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
#include "../feenox.h"
extern feenox_t feenox;


int feenox_mesh_read_gmsh(mesh_t *this) {

  char buffer[BUFFER_LINE_SIZE];
  size_t *tags = NULL;

  char *dummy = NULL;
  char *name = NULL;
  physical_group_t *physical_group = NULL;
  geometrical_entity_t *geometrical_entity = NULL;
  unsigned int version_maj = 0;
  unsigned int version_min = 0;
  size_t blocks, geometrical, tag, dimension, parametric, num;
  size_t first, second; // this are buffers because 4.0 and 4.1 swapped tag,dim to dim,tag
  size_t type, physical;
  size_t node, node_index;
  size_t ntags;
  size_t tag_min = 0;
  size_t tag_max = 0;

  if (this->file->pointer == NULL) {
    feenox_call(feenox_instruction_file_open(this->file));
  }
  
  while (fgets(buffer, BUFFER_LINE_SIZE-1, this->file->pointer) != NULL) {

    if (strncmp("\n", buffer, 1) == 0) {
      ;
    
    // ------------------------------------------------------  
    } else if (strncmp("$MeshFormat", buffer, 11) == 0) {
  
      // version
      if (fscanf(this->file->pointer, "%s", buffer) == 0) {
        return FEENOX_ERROR;
      }
      if (strcmp("2.2", buffer) == 0) {
        version_maj = 2;
        version_min = 2;
      } else if (strcmp("4", buffer) == 0) {
        version_maj = 4;
        version_min = 0;       
      } else if (strcmp("4.1", buffer) == 0) {
        version_maj = 4;
        version_min = 1;       
      } else {
        feenox_push_error_message("mesh '%s' has an incompatible version '%s', only versions 2.2, 4.0 and 4.1 are supported", this->file->path, buffer);
        return FEENOX_ERROR;
      }
  
      // type (ASCII int; 0 for ASCII mode, 1 for binary mode)
      if (fscanf(this->file->pointer, "%s", buffer) == 0) {
        return FEENOX_ERROR;
      }
      if (strcmp("0", buffer) != 0) {
        feenox_push_error_message("mesh '%s' is binary, only ASCII files are supported", this->file->path);
        return FEENOX_ERROR;
      }
  
      // the size of the floating-point numbers are irrelevant in ASCII mode
      // (ASCII int; sizeof(size_t))
      if (fscanf(this->file->pointer, "%s", buffer) == 0) {
        return FEENOX_ERROR;
      }
      
      // newline
      if (fgets(buffer, BUFFER_LINE_SIZE-1, this->file->pointer) == NULL) {
        feenox_push_error_message("corrupted mesh '%s'", this->file->path);
        return FEENOX_ERROR;
      } 
      
      // line $EndMeshFormat
      if (fgets(buffer, BUFFER_LINE_SIZE-1, this->file->pointer) == NULL) {
        feenox_push_error_message("corrupted mesh '%s'", this->file->path);
        return FEENOX_ERROR;
      } 
      if (strncmp("$EndMeshFormat", buffer, 14) != 0) {
        feenox_push_error_message("$EndMeshFormat not found in mesh '%s'", this->file->path);
        return FEENOX_ERROR;
      }

    // ------------------------------------------------------      
    } else if (strncmp("$PhysicalNames", buffer, 14) == 0) {

      // if there are physical names then we define implicitly each group
      // if they already exist we check the ids

      // number of names
      if (fscanf(this->file->pointer, "%d", &this->n_physical_names) == 0) {
        return FEENOX_ERROR;
      }
 
      unsigned int i;
      for (i = 0; i < this->n_physical_names; i++) {

        if (fscanf(this->file->pointer, "%ld %ld", &dimension, &tag) < 2) {
          return FEENOX_ERROR;
        }
        if (dimension < 0 || dimension > 4) {
          feenox_push_error_message("invalid dimension %d for physical group %d in mesh file '%s'", dimension, tag, this->file->path);
          return FEENOX_ERROR;
        }
        
        if (fgets(buffer, BUFFER_LINE_SIZE-1, this->file->pointer) == NULL) {
          feenox_push_error_message("corrupted mesh file '%s'", this->file->path);
          return FEENOX_ERROR;
        }
        if ((dummy = strrchr(buffer, '\"')) == NULL) {
          feenox_push_error_message("end quote not found in physical name %d in '%s'", tag, this->file->path);
          return FEENOX_ERROR;
        }
        *dummy = '\0';
        if ((dummy = strchr(buffer, '\"')) == NULL) {
          feenox_push_error_message("beginning quote not found in physical name %d in '%s'", tag, this->file->path);
          return FEENOX_ERROR;
        }
        feenox_check_alloc(name = strdup(dummy+1));
       
        if ((physical_group = feenox_get_physical_group_ptr(this, name)) == NULL) {
          // create new physical group
          if ((physical_group = feenox_define_physical_group(this, name, dimension)) == NULL) {
            return FEENOX_ERROR;
          }
          // TODO: api, set tag
          physical_group->tag = tag;
        } else {
          // there already exists a physical group created with PHYSICAL_GROUP
          // check that either it does not have a tag or the tags coincide
          if (physical_group->tag == 0) {
            physical_group->tag = tag;
          } else if (physical_group->tag != tag) {
            feenox_push_error_message("physical group '%s' has tag %d in input and %d in mesh '%s'", name, physical_group->tag, tag, this->name);
            return FEENOX_ERROR;
          }
          // same for the dimension
          if (physical_group->dimension <= 0) {
            // si tiene 0 le ponemos la que acabamos de leer
            physical_group->dimension = dimension;
          } else if (physical_group->dimension != dimension) {
            // si no coincide nos quejamos
            feenox_push_error_message("physical group '%s' has dimension %d in input and %d in mesh '%s'", name, physical_group->dimension, dimension, this->name);
            return FEENOX_ERROR;
          }
          
        }

        // add the group to a local hash so we can solve the pointer
        // "more or less" easy
        HASH_ADD(hh_tag[dimension], this->physical_groups_by_tag[dimension], tag, sizeof(int), physical_group);

        // if the physical group does not have a material, look for one with the same name
        if (physical_group->material == NULL) {
          HASH_FIND_STR(feenox.mesh.materials, name, physical_group->material);
        }
        // TODO: same for BCs

        free(name);

      }
      

      // the line $EndPhysicalNames
      if (fgets(buffer, BUFFER_LINE_SIZE-1, this->file->pointer) == NULL) {
        feenox_push_error_message("corrupted mesh file '%s'", this->file->path);
        return -3;
      } 
      if (strncmp("$EndPhysicalNames", buffer, 17) != 0) {
        feenox_push_error_message("$EndPhysicalNames not found in mesh file '%s'", this->file->path);
        return -2;
      }
      
    // ------------------------------------------------------      
    } else if (strncmp("$Entities", buffer, 9) == 0) {
 
      // la cantidad de cosas
      if (fscanf(this->file->pointer, "%d %d %d %d", &this->points, &this->curves, &this->surfaces, &this->volumes) < 4) {
        return FEENOX_ERROR;
      }

      size_t i;
      for (i = 0; i < this->points+this->curves+this->surfaces+this->volumes; i++) {
        if (i < this->points) {
          dimension = 0;
        } else if (i < this->points+this->curves) {
          dimension = 1;          
        } else if (i < this->points+this->curves+this->surfaces) {
          dimension = 2;          
        } else {
          dimension = 3;
        }
        
        feenox_check_alloc(geometrical_entity = calloc(1, sizeof(geometrical_entity_t)));
        // a partir de 4.1 los puntos tienen solo 3 valores, no 6 de bounding box
        if (dimension == 0 && version_maj == 4 && version_min >= 1) {
          if (fscanf(this->file->pointer, "%ld %lf %lf %lf %ld",
                     &geometrical_entity->tag,
                     &geometrical_entity->boxMinX,
                     &geometrical_entity->boxMinY,
                     &geometrical_entity->boxMinZ,
                     &geometrical_entity->num_physicals) < 5) {
            feenox_push_error_message("not enough data in physical groups");
            return FEENOX_ERROR;
          }
        } else {
          if (fscanf(this->file->pointer, "%ld %lf %lf %lf %lf %lf %lf %ld",
                     &geometrical_entity->tag,
                     &geometrical_entity->boxMinX,
                     &geometrical_entity->boxMinY,
                     &geometrical_entity->boxMinZ,
                     &geometrical_entity->boxMaxX,
                     &geometrical_entity->boxMaxY,
                     &geometrical_entity->boxMaxZ,
                     &geometrical_entity->num_physicals) < 8) {
            feenox_push_error_message("not enough data in physical groups");
            return FEENOX_ERROR;
          }
        }        
        
        if (geometrical_entity->num_physicals != 0) {
          feenox_check_alloc(geometrical_entity->physical = calloc(geometrical_entity->num_physicals, sizeof(size_t)));
          size_t j;
          for (j = 0; j < geometrical_entity->num_physicals; j++) {
            if (fscanf(this->file->pointer, "%ld", &geometrical_entity->physical[j]) == 0) {
              return FEENOX_ERROR;
            }
          }
        }
        
        // points do not have bounding groups
        if (dimension != 0) {
          if (fscanf(this->file->pointer, "%ld", &geometrical_entity->num_bounding) < 1) {
            return FEENOX_ERROR;
          }          
          if (geometrical_entity->num_bounding != 0) {
            feenox_check_alloc(geometrical_entity->bounding = calloc(geometrical_entity->num_bounding, sizeof(size_t)));
            for (size_t j = 0; j < geometrical_entity->num_bounding; j++) {
              // some groups can be negative because the tag is multiplied by the orientation
              if (fscanf(this->file->pointer, "%ld", &geometrical_entity->bounding[j]) == 0) {
                return FEENOX_ERROR;
              }
            }
          }
        }
        
        // agregamos la group al hash de la dimension que corresponda
        HASH_ADD(hh[dimension], this->geometrical_entities[dimension], tag, sizeof(int), geometrical_entity);
        
      }

      // queda un blanco y un newline
      if (fgets(buffer, BUFFER_LINE_SIZE-1, this->file->pointer) == NULL) {
        feenox_push_error_message("corrupted mesh file '%s'", this->file->path);
        return -3;
      } 

      // the line $EndEntities
      if (fgets(buffer, BUFFER_LINE_SIZE-1, this->file->pointer) == NULL) {
        feenox_push_error_message("corrupted mesh file '%s'", this->file->path);
        return -3;
      } 
      if (strncmp("$EndEntities", buffer, 12) != 0) {
        feenox_push_error_message("$EndEntities not found in mesh file '%s'", this->file->path);
        return -2;
      }
      
    // ------------------------------------------------------      
    } else if (strncmp("$Nodes", buffer, 6) == 0) {

      if (version_maj == 2) {
        // number of nodes
        if (fscanf(this->file->pointer, "%ld", &(this->n_nodes)) == 0) {
          return FEENOX_ERROR;
        }
        if (this->n_nodes == 0) {
          feenox_push_error_message("no nodes found in mesh '%s'", this->file->path);
          return FEENOX_ERROR;
        }

        feenox_check_alloc(this->node = calloc(this->n_nodes, sizeof(node_t)));

        for (size_t j = 0; j < this->n_nodes; j++) {
          if (fscanf(this->file->pointer, "%ld %lf %lf %lf",
                  &tag,
                  &this->node[j].x[0],
                  &this->node[j].x[1],
                  &this->node[j].x[2]) < 4) {
            return FEENOX_ERROR;
          }
          
          // en msh2 si no es sparse, los tags son indices pero si es sparse es otro cantar
          if (j+1 != tag) {
            this->sparse = 1;
          }
          
          if ((this->node[j].tag = tag) > tag_max) {
            tag_max = this->node[j].tag;
          }
          this->node[j].index_mesh = j;
          
          // si nos dieron degrees of freedom entonces tenemos que allocar
          // lugar para la solucion phi de alguna PDE
/*          
          if (this->degrees_of_freedom != 0) {
            feenox_check_alloc(this->node[j].phi = calloc(this->degrees_of_freedom, sizeof(double)));
          }
 */
        }
        
        // terminamos de leer los nodos, si los nodos son sparse tenemos que hacer el tag2index
        if (this->sparse) {
          if (tag_max < this->n_nodes) {
            feenox_push_error_message("maximum node tag %d is less that number of nodes %d", tag_max, this->n_nodes);
          }
          feenox_check_alloc(this->tag2index = malloc((tag_max+1) * sizeof(size_t)));
          for (size_t k = 0; k <= tag_max; k++) {
            this->tag2index[k] = SIZE_MAX;
          }
          for (size_t i = 0; i < this->n_nodes; i++) {
            this->tag2index[this->node[i].tag] = i;
          }
        }

        
      } else if (version_maj == 4) {
        // la cantidad de bloques y de nodos
        if (version_min == 0) {
          // en 4.0 no tenemos min y max
          if (fscanf(this->file->pointer, "%ld %ld", &blocks, &this->n_nodes) < 2) {
            feenox_push_error_message("error reading node blocks");
            return FEENOX_ERROR;
          }
        } else {
          if (fscanf(this->file->pointer, "%ld %ld %ld %ld", &blocks, &this->n_nodes, &tag_min, &tag_max) < 4) {
            feenox_push_error_message("error reading node blocks");
            return FEENOX_ERROR;
          }
        }
        if (this->n_nodes == 0) {
          feenox_push_error_message("no nodes found in mesh '%s'", this->file->path);
          return FEENOX_ERROR;
        }

        feenox_check_alloc(this->node = calloc(this->n_nodes, sizeof(node_t)));

        if (tag_max != 0) {
          // we can do this as a one single pass because we have tag_max (I asked for this in v4.1)
          // TODO: offset with tag_min?
          if (tag_max < this->n_nodes) {
            feenox_push_error_message("maximum node tag %d is less that number of nodes %d", tag_max, this->n_nodes);
          }
          feenox_check_alloc(this->tag2index = malloc((tag_max+1) * sizeof(size_t)));
          for (size_t k = 0; k <= tag_max; k++) {
            this->tag2index[k] = SIZE_MAX;
          }
        }
        
        size_t i = 0;
        for (size_t l = 0; l < blocks; l++) {
          if (fscanf(this->file->pointer, "%ld %ld %ld %ld", &first, &second, &parametric, &num) < 4) {
            feenox_push_error_message("not enough data in node block");
            return FEENOX_ERROR;
          }
          // v4.0 and v4.1 have these two switched
          if (version_min == 0) {
            geometrical = first;
            dimension = second;
          } else {
            dimension = first;
            geometrical = second;
          }

          if (parametric) {
            feenox_push_error_message("mesh '%s' contains parametric data, which is unsupported yet", this->file->path);
            return FEENOX_ERROR;
          }
          
          if (version_min == 0) {
            // here we have tag and coordinate in the same line
            for (size_t k = 0; k < num; k++) {
              if (fscanf(this->file->pointer, "%ld %lf %lf %lf",
                         &this->node[i].tag,
                         &this->node[i].x[0],
                         &this->node[i].x[1],
                         &this->node[i].x[2]) < 4) {
                feenox_push_error_message("reading node data");
                return FEENOX_ERROR;
              }
              
              if (this->node[i].tag > tag_max) {
                tag_max = this->node[i].tag;
              }
              
              // en msh4 los tags son los indices de la malla global
              this->node[i].index_mesh = i;
              
              // si nos dieron degrees of freedom entonces tenemos que allocar
              // lugar para la solucion phi de alguna PDE
/*              
              if (this->degrees_of_freedom != 0) {
                this->node[i].phi = calloc(this->degrees_of_freedom, sizeof(double));
              }
 */
              
              i++;
            }
          } else {

            // here we have first all the tags and then all the coordinates
            // (this is to allow for block-reading binary files, which we do not care about)
            for (size_t k = 0; k < num; k++) {
              if (fscanf(this->file->pointer, "%ld", &this->node[i+k].tag) < 1) {
                feenox_push_error_message("reading node tag");
                return FEENOX_ERROR;
              }
            }
            for (size_t k = 0; k < num; k++) {
              if (fscanf(this->file->pointer, "%lf %lf %lf",
                         &this->node[i].x[0],
                         &this->node[i].x[1],
                         &this->node[i].x[2]) < 3) {
                feenox_push_error_message("reading node coordinates");
                return FEENOX_ERROR;
              }
              
              this->node[i].index_mesh = i;
              this->tag2index[this->node[i].tag] = i;
              
              // si nos dieron degrees of freedom entonces tenemos que allocar
              // lugar para la solucion phi de alguna PDE
/*              
              if (this->degrees_of_freedom != 0) {
                this->node[i].phi = calloc(this->degrees_of_freedom, sizeof(double));
              }
*/
              
              i++;
            }
          }
          
        }
        
        if (version_min == 0) {
          // for v4.0 we need an extra loop because we did not have the actual tag_max
          feenox_check_alloc(this->tag2index = malloc((tag_max+1) * sizeof(size_t)));
          for (size_t k = 0; k <= tag_max; k++) {
            this->tag2index[k] = SIZE_MAX;
          }
          for (size_t i = 0; i < this->n_nodes; i++) {
            this->tag2index[this->node[i].tag] = i;
          }
        }
        
      }
           
      // the newline
      if (fgets(buffer, BUFFER_LINE_SIZE-1, this->file->pointer) == NULL) {
        feenox_push_error_message("corrupted mesh file '%s'", this->file->path);
        return -3;
      } 
      
      // the line $EndNodes
      if (fgets(buffer, BUFFER_LINE_SIZE-1, this->file->pointer) == NULL) {
        feenox_push_error_message("corrupted mesh file '%s'", this->file->path);
        return -3;
      } 
      if (strncmp("$EndNodes", buffer, 9) != 0) {
        feenox_push_error_message("$EndNodes not found in mesh file '%s'", this->file->path);
        return -2;
      }

    // ------------------------------------------------------      
    } else if (strncmp("$Elements", buffer, 9) == 0) {              
      
      if (version_maj == 2) {
        // la cantidad de elementos
        if (fscanf(this->file->pointer, "%ld", &(this->n_elements)) == 0) {
          return FEENOX_ERROR;
        }
        if (this->n_elements == 0) {
          feenox_push_error_message("no elements found in mesh file '%s'", this->file->path);
          return -2;
        }
        this->element = calloc(this->n_elements, sizeof(element_t));

        for (size_t i = 0; i < this->n_elements; i++) {

          if (fscanf(this->file->pointer, "%ld %ld %ld", &tag, &type, &ntags) < 3) {
            return FEENOX_ERROR;
          }

          // en msh2 los tags son indices
          if (i+1 != tag) {
            feenox_push_error_message("nodes in file '%s' are sparse", this->file->path);
            return FEENOX_ERROR;
          }
          this->element[i].tag = tag;
          this->element[i].index = i;

          // tipo de elemento
          if (type >= NUMBER_ELEMENT_TYPE) {
            feenox_push_error_message("elements of type '%d' are not supported in this version :-(", type);
            return FEENOX_ERROR;
          }
          if (feenox.mesh.element_types[type].nodes == 0) {
            feenox_push_error_message("elements of type '%s' are not supported in this version :-(", this->element[i].type->name);
            return FEENOX_ERROR;
          }
          this->element[i].type = &(feenox.mesh.element_types[type]);

          // format v2.2
          // cada elemento tiene un tag que es un array de enteros
          // el primero es el id de la entidad fisica
          // el segundo es el id de la entidad geometrica (no nos interesa)
          // despues siguen cosas opcionales como particiones, padres, dominios, etc
          if (ntags > 0) {
            feenox_check_alloc(tags = malloc(ntags * sizeof(size_t)));
            for (size_t k = 0; k < ntags; k++) {
              if (fscanf(this->file->pointer, "%ld", &tags[k]) == 0) {
                return FEENOX_ERROR;
              }
            }
            
            
            if (ntags > 1) {
              dimension = this->element[i].type->dim;
              HASH_FIND(hh_tag[dimension], this->physical_groups_by_tag[dimension], &tags[0], sizeof(int), physical_group);
              if ((this->element[i].physical_group = physical_group) == NULL) {
                // si no encontramos ninguna, hay que crear una
                snprintf(buffer, BUFFER_LINE_SIZE-1, "%s_%ld_%ld", this->name, dimension, tags[0]);
                if ((this->element[i].physical_group = feenox_define_physical_group(this, buffer, this->element[i].type->dim)) == NULL) {
                  return FEENOX_ERROR;
                }
                this->element[i].physical_group->tag = tags[0];
                HASH_ADD(hh_tag[dimension], this->physical_groups_by_tag[dimension], tag, sizeof(int), this->element[i].physical_group);
              }
              this->element[i].physical_group->n_elements++;
            }
            free(tags);
          }
          
          this->element[i].node = calloc(this->element[i].type->nodes, sizeof(node_t *));
          for (size_t j = 0; j < this->element[i].type->nodes; j++) {
            if (fscanf(this->file->pointer, "%ld", &node) < 1) {
              return FEENOX_ERROR;
            }
            if (this->sparse == 0 && (node < 1 || node > this->n_nodes)) {
              feenox_push_error_message("node %d in element %d does not exist", node, tag);
              return FEENOX_ERROR;
            }

            if ((node_index = (this->sparse==0)?node-1:this->tag2index[node]) < 0) {
              feenox_push_error_message("node %d in element %d does not exist", node, tag);
              return FEENOX_ERROR;
            }
            this->element[i].node[j] = &this->node[node_index];
            feenox_mesh_add_element_to_list(&this->element[i].node[j]->associated_elements, &this->element[i]);
          }
        }
        
      } else if (version_maj == 4) {
        // la cantidad de bloques y de elementos
        if (version_min == 0) {
          // en 4.0 no tenemos min y max
          tag_min = 0;
          tag_max = 0;
          if (fscanf(this->file->pointer, "%ld %ld", &blocks, &this->n_elements) < 2) {
            feenox_push_error_message("error reading element blocks");
            return FEENOX_ERROR;
          }
        } else {
          if (fscanf(this->file->pointer, "%ld %ld %ld %ld", &blocks, &this->n_elements, &tag_min, &tag_max) < 4) {
            feenox_push_error_message("error reading node blocks");
            return FEENOX_ERROR;
          }
        }
        
        if (this->n_elements == 0) {
          feenox_push_error_message("no elements found in mesh file '%s'", this->file->path);
          return FEENOX_ERROR;
        }
        this->element = calloc(this->n_elements, sizeof(element_t));

        size_t i = 0;
        for (size_t l = 0; l < blocks; l++) {
          if (fscanf(this->file->pointer, "%ld %ld %ld %ld", &first, &second, &type, &num) < 4) {
            return FEENOX_ERROR;
          }
          if (version_min == 0) {
            geometrical = first;
            dimension = second;
          } else {
            dimension = first;
            geometrical = second;
          }
          
          if (type >= NUMBER_ELEMENT_TYPE) {
            feenox_push_error_message("elements of type '%d' are not supported in this version", type);
            return FEENOX_ERROR;
          }
          if (feenox.mesh.element_types[type].nodes == 0) {
            feenox_push_error_message("elements of type '%s' are not supported in this version", feenox.mesh.element_types[type].name);
            return FEENOX_ERROR;
          }
          
          // todo el bloque tiene la misma entidad fisica, la encontramos una vez y ya
          HASH_FIND(hh[dimension], this->geometrical_entities[dimension], &geometrical, sizeof(int), geometrical_entity);
          if (geometrical_entity->num_physicals > 0) {
            // que hacemos si hay mas de una? la primera? la ultima?
            physical = geometrical_entity->physical[0];
            HASH_FIND(hh_tag[dimension], this->physical_groups_by_tag[dimension], &physical, sizeof(int), physical_group);
            if ((this->element[i].physical_group = physical_group) == NULL) {
              snprintf(buffer, BUFFER_LINE_SIZE-1, "%s_%ld_%ld", this->name, dimension, physical);
              if ((this->element[i].physical_group = feenox_define_physical_group(this, buffer, feenox.mesh.element_types[type].dim)) == NULL) {
                return FEENOX_ERROR;
              }
              this->element[i].physical_group->tag = physical;
              HASH_ADD(hh_tag[dimension], this->physical_groups_by_tag[dimension], tag, sizeof(int), this->element[i].physical_group);
            }
          } else {
            physical_group = NULL;
          }
                    
          for (size_t k = 0; k < num; k++) {
            if (fscanf(this->file->pointer, "%ld", &tag) == 0) {
              return FEENOX_ERROR;
            }
            
            this->element[i].tag = tag;
            this->element[i].index = i;
            this->element[i].type = &(feenox.mesh.element_types[type]);
            
            if ((this->element[i].physical_group = physical_group) != NULL) {
              this->element[i].physical_group->n_elements++;
            }
            
            this->element[i].node = calloc(this->element[i].type->nodes, sizeof(node_t *));
            for (size_t j = 0; j < this->element[i].type->nodes; j++) {
              if (fscanf(this->file->pointer, "%ld", &node) == 0) {
                return FEENOX_ERROR;
              }
              // ojo al piojo en msh4, hay que usar el maneje del tag2index
              if ((this->element[i].node[j] = &this->node[this->tag2index[node]]) == 0) {
                feenox_push_error_message("node %d in element %d does not exist", node, tag);
                return FEENOX_ERROR;
              }
              feenox_mesh_add_element_to_list(&this->element[i].node[j]->associated_elements, &this->element[i]);
            }
            i++;
          }
        }
      }

      // the newline
      if (fgets(buffer, BUFFER_LINE_SIZE-1, this->file->pointer) == NULL) {
        feenox_push_error_message("corrupted mesh file '%s'", this->file->path);
        return -3;
      } 

      // the line $EndElements
      if (fgets(buffer, BUFFER_LINE_SIZE-1, this->file->pointer) == NULL) {
        feenox_push_error_message("corrupted mesh file '%s'", this->file->path);
        return -3;
      } 
      if (strncmp("$EndElements", buffer, 12) != 0) {
        feenox_push_error_message("$EndElements not found in mesh file '%s'", this->file->path);
        return -2;
      }
            
    // ------------------------------------------------------      
    } else if (strncmp("$ElementData", buffer, 12) == 0) {
      
      // TODO!

    // ------------------------------------------------------      
    } else if (strncmp("$ElementNodeData", buffer, 16) == 0) {
      
      // TODO!

    // ------------------------------------------------------      
    } else if (strncmp("$NodeData", buffer, 9) == 0) {
      
      node_data_t *node_data;
      function_t *function = NULL;
      double time, value;
      int j, timestep, dofs, nodes;
      int n_string_tags, n_real_tags, n_integer_tags;
      char *string_tag = NULL;
      
      // string-tags
      if (fscanf(this->file->pointer, "%d", &n_string_tags) == 0) {
        feenox_push_error_message("error reading file");
        return FEENOX_ERROR;
      }
      if (n_string_tags != 1) {
        continue;
      }
      // el \n
      if (fgets(buffer, BUFFER_LINE_SIZE-1, this->file->pointer) == NULL) {
        feenox_push_error_message("error reading file");
        return FEENOX_ERROR;
      }
      if (fgets(buffer, BUFFER_LINE_SIZE-1, this->file->pointer) == NULL) {
        feenox_push_error_message("error reading file");
        return FEENOX_ERROR;
      }
      string_tag = strtok(buffer, "\"");
      
      LL_FOREACH(this->node_datas, node_data) {
        if (strcmp(string_tag, node_data->name_in_mesh) == 0) {
          function = node_data->function;
          feenox_check_alloc(function->name_in_mesh = strdup(node_data->name_in_mesh));
        }
      }
      
      // si no tenemos funcion seguimos de largo e inogramos todo el bloque
      if (function == NULL) {
        continue;
      }
      
      // real-tags
      if (fscanf(this->file->pointer, "%d", &n_real_tags) == 0) {
        feenox_push_error_message("error reading file");
        return FEENOX_ERROR;
      }
      if (n_real_tags != 1) {
        continue;
      }
      if (fscanf(this->file->pointer, "%lf", &time) == 0) {
        feenox_push_error_message("error reading file");
        return FEENOX_ERROR;
      }
      // we read only data for t = 0
      if (time != 0) {
        continue;
      }
      
      // integer-tags
      if (fscanf(this->file->pointer, "%d", &n_integer_tags) == 0) {
        feenox_push_error_message("error reading file");
        return FEENOX_ERROR;
      }
      if (n_integer_tags != 3) {
        continue;
      }
      if (fscanf(this->file->pointer, "%d", &timestep) == 0) {
        feenox_push_error_message("error reading file");
        return FEENOX_ERROR;
      }
      if (fscanf(this->file->pointer, "%d", &dofs) == 0) {
        feenox_push_error_message("error reading file");
        return FEENOX_ERROR;
      }
      if (fscanf(this->file->pointer, "%d", &nodes) == 0) {
        feenox_push_error_message("error reading file");
        return FEENOX_ERROR;
      }
      
      if (dofs != 1 || nodes != this->n_nodes) {
        continue;
      }
      
      // if we made it this far, we have a function!
      if (function->data_size != nodes) {
        function->type = function_type_pointwise_mesh_node;
        function->mesh = this;
        function->data_argument = this->nodes_argument;
        function->data_size = nodes;
        if (function->data_value != NULL) {
          free(function->data_value);
        }
        function->data_value = calloc(nodes, sizeof(double));
      }  
      
      for (j = 0; j < nodes; j++) {
        if (fscanf(this->file->pointer, "%ld %lf", &node, &value) == 0) {
          feenox_push_error_message("error reading file");
          return FEENOX_ERROR;
        }
        if ((node_index = (this->sparse==0)?node-1:this->tag2index[node]) < 0) {
          feenox_push_error_message("node %d in element %d does not exist", node, tag);
          return FEENOX_ERROR;
        }
        function->data_value[node_index] = value;
      }

      // the newline
      if (fgets(buffer, BUFFER_LINE_SIZE-1, this->file->pointer) == NULL) {
        feenox_push_error_message("corrupted mesh file '%s'", this->file->path);
        return -3;
      } 

      // the line $ElementNodeData
      if (fgets(buffer, BUFFER_LINE_SIZE-1, this->file->pointer) == NULL) {
        feenox_push_error_message("corrupted mesh file '%s'", this->file->path);
        return -3;
      } 
      if (strncmp("$EndNodeData", buffer, 12) != 0 && strncmp("$EndElementData", buffer, 15) != 0) {
        feenox_push_error_message("$EndNodeData not found in mesh file '%s'", this->file->path);
        return -2;
      }

  
    // ------------------------------------------------------      
    // extension nuestra!
/*      
    } else if (strncmp("$Neighbors", buffer, 10) == 0 || strncmp("$Neighbours", buffer, 11) == 0) {

      int element_id;
      
      // la cantidad de celdas
      if (fscanf(this->file->pointer, "%ld", &(this->n_cells)) == 0) {
        return FEENOX_ERROR;
      }
      if (this->n_cells == 0) {
        feenox_push_error_message("no cells found in mesh file '%s'", this->file->path);
        return -2;
      }
      this->cell = calloc(this->n_cells, sizeof(cell_t));

      for (i = 0; i < this->n_cells; i++) {
    
        if (fscanf(this->file->pointer, "%d", &cell_id) == 0) {
          return FEENOX_ERROR;
        }
        if (cell_id != i+1) {
          feenox_push_error_message("cells in mesh file '%s' are not sorted", this->file->path);
          return -2;
        }
        this->cell[i].id = cell_id;
        
        if (fscanf(this->file->pointer, "%d", &element_id) == 0) {
          return FEENOX_ERROR;
        }
        this->cell[i].element = &this->element[element_id - 1];
        
        if (fscanf(this->file->pointer, "%d", &this->cell[i].n_neighbors) == 0) {
          return FEENOX_ERROR;
        }
        if (fscanf(this->file->pointer, "%d", &j) == 0) {
          return FEENOX_ERROR;
        }
        if (j != this->cell[i].element->type->nodes_per_face) {
          feenox_push_error_message("mesh file '%s' has inconsistencies in the neighbors section", this->file->path);
          return -2;
        }

        this->cell[i].ineighbor = malloc(this->cell[i].n_neighbors * sizeof(int));
        this->cell[i].ifaces = malloc(this->cell[i].n_neighbors * sizeof(int *));
        for (j = 0; j < this->cell[i].n_neighbors; j++) {
          if (fscanf(this->file->pointer, "%d", &this->cell[i].ineighbor[j]) == 0) {
            return FEENOX_ERROR;
          }
          this->cell[i].ifaces[j] = malloc(this->cell[i].element->type->nodes_per_face);
          for (k = 0; k < this->cell[i].element->type->nodes_per_face; k++) {
            if (fscanf(this->file->pointer, "%d", &this->cell[i].ifaces[j][k]) == 0) {
              return FEENOX_ERROR;
            }
          }
        }
      }
        
      
      // the newline
      if (fgets(buffer, BUFFER_LINE_SIZE-1, this->file->pointer) == NULL) {
        feenox_push_error_message("corrupted mesh file '%s'", this->file->path);
        return -3;
      } 

      // the line $EndNeighbors
      if (fgets(buffer, BUFFER_LINE_SIZE-1, this->file->pointer) == NULL) {
        feenox_push_error_message("corrupted mesh file '%s'", this->file->path);
        return -3;
      } 
      if (strncmp("$EndNeighbors", buffer, 13) != 0 && strncmp("$EndNeighbours", buffer, 14) != 0) {
        feenox_push_error_message("$EndNeighbors not found in mesh file '%s'", this->file->path);
        return -2;
      }
      
*/
    // ------------------------------------------------------      
    } else {
        
      do {
        if (fgets(buffer, BUFFER_LINE_SIZE-1, this->file->pointer) == NULL || buffer == NULL) {
          feenox_push_error_message("corrupted mesh file '%s'", this->file->path);
          return -3;
        }
      } while(strncmp("$End", buffer, 4) != 0);
        
    }
  }

  // close the mesh file
  fclose(this->file->pointer);
  this->file->pointer = NULL;
  
  // limpiar hashes
  
  return FEENOX_OK;
}


int mesh_gmsh_write_header(FILE *file) {
  fprintf(file, "$MeshFormat\n");
  fprintf(file, "2.2 0 8\n");
  fprintf(file, "$EndMeshFormat\n");

  return FEENOX_OK;
}

int mesh_gmsh_write_mesh(mesh_t *this, int no_physical_names, FILE *file) {
  
  int i, j, n;
  physical_group_t *physical_group;

  if (no_physical_names == 0) {
    n = HASH_COUNT(this->physical_groups);
    if (n != 0) {
      fprintf(file, "$PhysicalNames\n");
      fprintf(file, "%d\n", n);
  
      // y despues barrerlas
      for (physical_group = this->physical_groups; physical_group != NULL; physical_group = physical_group->hh.next) {
        fprintf(file, "%d %ld \"%s\"\n", physical_group->dimension, physical_group->tag, physical_group->name);
      }
      fprintf(file, "$EndPhysicalNames\n");
    }
  }
  
  
  fprintf(file, "$Nodes\n");
  fprintf(file, "%ld\n", this->n_nodes);
  for (i = 0; i < this->n_nodes; i++) {
    fprintf(file, "%ld %g %g %g\n", this->node[i].tag, this->node[i].x[0], this->node[i].x[1], this->node[i].x[2]);
  }
  fprintf(file, "$EndNodes\n");

  fprintf(file, "$Elements\n");
  fprintf(file, "%ld\n", this->n_elements);
  for (i = 0; i < this->n_elements; i++) {
    fprintf(file, "%ld ", this->element[i].tag);
    fprintf(file, "%ld ", this->element[i].type->id);

    // in principle we shuold write the detailed information about groups and parititons
//    fprintf(file, "%d ", this->element[i].ntags);
    
    // but for now only two tags are enough:
    // the first one is the physical group and the second one ought to be the geometrical group
    // if there is no such information, then we just duplicate the physical group tag
    if (this->element[i].physical_group != NULL) {
      fprintf(file, "2 %ld %ld", this->element[i].physical_group->tag, this->element[i].physical_group->tag);
    } else {
      fprintf(file, "2 0 0");
    }
    // los nodos
    for (j = 0; j < this->element[i].type->nodes; j++) {
      fprintf(file, " %ld", this->element[i].node[j]->tag);
    }
    fprintf(file, "\n");
  }
  fprintf(file, "$EndElements\n");
  
  return FEENOX_OK;
  
}

/*
int mesh_gmsh_write_scalar(mesh_post_t *mesh_post, function_t *function, centering_t centering) {

  int i;
  mesh_t *mesh;
  
  if (mesh_post->mesh != NULL) {
    mesh = mesh_post->mesh;
  } else if (function != NULL) {
    mesh = function->mesh;
  } else {
    feenox_push_error_message("do not know which mesh to apply to post-process function '%s'", function->name);
    return FEENOX_ERROR;
  }

  if (centering == centering_cells) {
    fprintf(mesh_post->file->pointer, "$ElementData\n");
    if (this->n_cells == 0) {
      feenox_call(mesh_element2cell(mesh));
    }
  } else {
    fprintf(mesh_post->file->pointer, "$NodeData\n");
  }

  // one string tag
  fprintf(mesh_post->file->pointer, "1\n");
  // the name of the vew
  fprintf(mesh_post->file->pointer, "\"%s\"\n", function->name);
  // the other one (optional) is the interpolation scheme
  
  // one real tag (only one)
  fprintf(mesh_post->file->pointer, "1\n");                          
  // time
  fprintf(mesh_post->file->pointer, "%g\n", feenox_value(feenox_special_var(time)));

  // thre integer tags
  fprintf(mesh_post->file->pointer, "3\n");
  // timestep
  fprintf(mesh_post->file->pointer, "%d\n", (int)((feenox_var(feenox_special_var(end_time)) != 0) ? feenox_var(feenox_special_var(step_transient)) : feenox_value(feenox_special_var(step_static))));
  // number of data per node: uno (scalar)
  fprintf(mesh_post->file->pointer, "%d\n", 1);

  if (centering == centering_cells) {
    // number of data
    fprintf(mesh_post->file->pointer, "%d\n", this->n_cells);

    if (function->type == type_pointwise_mesh_cell && function->mesh == mesh) {
      for (i = 0; i < function->data_size; i++) {
        fprintf(mesh_post->file->pointer, "%d %g\n", this->cell[i].element->tag, function->data_value[i]);
      }
    } else {
      for (i = 0; i < this->n_cells; i++) {
        fprintf(mesh_post->file->pointer, "%d %g\n", this->cell[i].element->tag, feenox_evaluate_function(function, this->cell[i].x));
      }
    }
    fprintf(mesh_post->file->pointer, "$EndElementData\n");
    
  } else  {
    
    // number of data
    fprintf(mesh_post->file->pointer, "%d\n", this->n_nodes);              
  
    if (function->type == type_pointwise_mesh_node && function->mesh == mesh) {
      for (i = 0; i < function->data_size; i++) {
        fprintf(mesh_post->file->pointer, "%d %g\n", this->node[i].tag, function->data_value[i]);
      }
    } else {
      for (i = 0; i < this->n_nodes; i++) {
        fprintf(mesh_post->file->pointer, "%d %g\n", this->node[i].tag, feenox_evaluate_function(function, this->node[i].x));
      }
    }
    fprintf(mesh_post->file->pointer, "$EndNodeData\n");    
  }
  
  fflush(mesh_post->file->pointer);
  

  return FEENOX_OK;

}

int mesh_gmsh_write_vector(mesh_post_t *mesh_post, function_t **function, centering_t centering) {

  int i, j;
  mesh_t *mesh;
  
  if (mesh_post->mesh != NULL) {
    mesh = mesh_post->mesh;
  } else if (function != NULL) {
    mesh = function[0]->mesh;
  } else {
    return FEENOX_ERROR;
  }

  if (centering == centering_cells) {
    fprintf(mesh_post->file->pointer, "$ElementData\n");
    if (this->n_cells == 0) {
      feenox_call(mesh_element2cell(mesh));
    }
  } else {
    fprintf(mesh_post->file->pointer, "$NodeData\n");
  }

  // un tag de string  
  fprintf(mesh_post->file->pointer, "1\n");
  // nombre de la vista
  fprintf(mesh_post->file->pointer, "\"%s_%s_%s\"\n", function[0]->name, function[1]->name, function[2]->name);
  // la otra (opcional) es el esquema de interpolacion
  
  // un tag real (el unico)  
  fprintf(mesh_post->file->pointer, "1\n");                          
  // tiempo
  fprintf(mesh_post->file->pointer, "%g\n", (feenox_var(feenox_special_var(end_time)) != 0) ? feenox_value(feenox_special_var(time)) : feenox_value(feenox_special_var(step_static)));

  // tres tags enteros
  fprintf(mesh_post->file->pointer, "3\n");
  // timestep
  fprintf(mesh_post->file->pointer, "%d\n", (int)((feenox_var(feenox_special_var(end_time)) != 0) ? feenox_var(feenox_special_var(step_transient)) : feenox_value(feenox_special_var(step_static))));
  // cantidad de datos por punto: 3 (un vector)
  fprintf(mesh_post->file->pointer, "%d\n", 3);

  if (centering == centering_cells) {
    for (i = 0; i < this->n_cells; i++) {
      fprintf(mesh_post->file->pointer, "%d %g %g %g\n", this->cell[i].element->index,
                                                         feenox_evaluate_function(function[0], this->cell[i].x),
                                                         feenox_evaluate_function(function[1], this->cell[i].x),
                                                         feenox_evaluate_function(function[2], this->cell[i].x));
    }
    fprintf(mesh_post->file->pointer, "$EndElementData\n");
  } else {
    // numero de datos
    fprintf(mesh_post->file->pointer, "%d\n", this->n_nodes);              
  
    for (j = 0; j < this->n_nodes; j++) {
      fprintf(mesh_post->file->pointer, "%d ", this->node[j].tag);
      
      if (function[0]->type == type_pointwise_mesh_node && function[0]->data_size == mesh_post->this->n_nodes) {
        fprintf(mesh_post->file->pointer, "%g ", function[0]->data_value[j]);
      } else {
        fprintf(mesh_post->file->pointer, "%g ", feenox_evaluate_function(function[0], this->node[j].x));
      }

      if (function[1]->type == type_pointwise_mesh_node && function[1]->data_size == mesh_post->this->n_nodes) {
        fprintf(mesh_post->file->pointer, "%g ", function[1]->data_value[j]);
      } else {
        fprintf(mesh_post->file->pointer, "%g ", feenox_evaluate_function(function[1], this->node[j].x));
      }

      if (function[2]->type == type_pointwise_mesh_node && function[2]->data_size == mesh_post->this->n_nodes) {
        fprintf(mesh_post->file->pointer, "%g\n", function[2]->data_value[j]);
      } else {
        fprintf(mesh_post->file->pointer, "%g\n", feenox_evaluate_function(function[2], this->node[j].x));
      }
    }
    fprintf(mesh_post->file->pointer, "$EndNodeData\n");
  }
 
  fflush(mesh_post->file->pointer);
  

  return FEENOX_OK;

}
*/

// read the next available time step and interpolate
int mesh_gmsh_update_function(function_t *function, double t, double dt) {
 
  char buffer[BUFFER_LINE_SIZE];
  double time, alpha;
  mesh_t *mesh = function->mesh;
  double *new_data = NULL; 
  int j, node, node_index;
  int done = 0;

  if (mesh->file->pointer == NULL) {
    feenox_call(feenox_instruction_file_open(mesh->file));
  }

   while (done == 0 && fgets(buffer, BUFFER_LINE_SIZE-1, mesh->file->pointer) != NULL) {

    if (strncmp("\n", buffer, 1) == 0) {
      ;
    
    // ------------------------------------------------------  
    } else if (strncmp("$NodeData", buffer, 9) == 0) {
      
      double value;
      int j, timestep, dofs, nodes;
      int n_string_tags, n_real_tags, n_integer_tags;
      char *string_tag = NULL;
      
      // string-tags
      if (fscanf(mesh->file->pointer, "%d", &n_string_tags) == 0) {
        feenox_push_error_message("error reading file");
        return FEENOX_ERROR;
      }
      if (n_string_tags != 1) {
        continue;
      }
      // el \n
      if (fgets(buffer, BUFFER_LINE_SIZE-1, mesh->file->pointer) == NULL) {
        feenox_push_error_message("error reading file");
        return FEENOX_ERROR;
      }
      if (fgets(buffer, BUFFER_LINE_SIZE-1, mesh->file->pointer) == NULL) {
        feenox_push_error_message("error reading file");
        return FEENOX_ERROR;
      }
      string_tag = strtok(buffer, "\"");
      
      if (strcmp(string_tag, function->name_in_mesh) != 0) {
        continue;
      }
      
      // real-tags
      if (fscanf(mesh->file->pointer, "%d", &n_real_tags) == 0) {
        feenox_push_error_message("error reading file");
        return FEENOX_ERROR;
      }
      if (n_real_tags != 1) {
        continue;
      }
      if (fscanf(mesh->file->pointer, "%lf", &time) == 0) {
        feenox_push_error_message("error reading file");
        return FEENOX_ERROR;
      }
      // read only data for the next time
      if (time < t-1e-5) {
        continue;
      }
      
      // integer-tags
      if (fscanf(mesh->file->pointer, "%d", &n_integer_tags) == 0) {
        feenox_push_error_message("error reading file");
        return FEENOX_ERROR;
      }
      if (n_integer_tags != 3) {
        continue;
      }
      if (fscanf(mesh->file->pointer, "%d", &timestep) == 0) {
        feenox_push_error_message("error reading file");
        return FEENOX_ERROR;
      }
      if (fscanf(mesh->file->pointer, "%d", &dofs) == 0) {
        feenox_push_error_message("error reading file");
        return FEENOX_ERROR;
      }
      if (fscanf(mesh->file->pointer, "%d", &nodes) == 0) {
        feenox_push_error_message("error reading file");
        return FEENOX_ERROR;
      }
      
      if (dofs != 1) {
        feenox_push_error_message("expected only one DOF");
        return FEENOX_ERROR;
      }
      if (nodes != mesh->n_nodes) {
        feenox_push_error_message("expected %d nodes, not %d", mesh->n_nodes, nodes);
        return FEENOX_ERROR;
      }
      
      new_data = calloc(nodes, sizeof(double));
      
      for (j = 0; j < nodes; j++) {
        if (fscanf(mesh->file->pointer, "%d %lf", &node, &value) == 0) {
          feenox_push_error_message("error reading file");
          return FEENOX_ERROR;
        }
        if ((node_index = (mesh->sparse==0) ? node-1 : mesh->tag2index[node]) < 0) {
          feenox_push_error_message("node %d does not exist", node);
          return FEENOX_ERROR;
        }
        new_data[node_index] = value;
      }
      
      // done!
      done = 1;

      // the newline
      if (fgets(buffer, BUFFER_LINE_SIZE-1, mesh->file->pointer) == NULL) {
        feenox_push_error_message("corrupted mesh file '%s'", mesh->file->path);
        return -3;
      } 

      // the line $ElementNodeData
      if (fgets(buffer, BUFFER_LINE_SIZE-1, mesh->file->pointer) == NULL) {
        feenox_push_error_message("corrupted mesh file '%s'", mesh->file->path);
        return -3;
      } 
      if (strncmp("$EndNodeData", buffer, 12) != 0) {
        feenox_push_error_message("$EndElementNodeData not found in mesh file '%s'", mesh->file->path);
        return -2;
      }
    }  
  }
  
  if (new_data != NULL) {
    if ((alpha = (t-function->mesh_time)/(time-function->mesh_time)) < 1) {
      for (j = 0; j < function->data_size; j++) {
        function->data_value[j] += alpha * (new_data[j] - function->data_value[j]);
      }
    }
    free(new_data);
  }
 
  return FEENOX_OK;
  
}
