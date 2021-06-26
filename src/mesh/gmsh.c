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

  char *dummy = NULL;
  char *name = NULL;
  physical_group_t *physical_group = NULL;
  geometrical_entity_t *geometrical_entity = NULL;
  unsigned int version_maj = 0;
  unsigned int version_min = 0;
  size_t node, node_index;
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
 
      int i;
      for (i = 0; i < this->n_physical_names; i++) {

        int dimension;
        int tag;
        if (fscanf(this->file->pointer, "%d %d", &dimension, &tag) < 2) {
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
        
        // TODO: should we allow unquoted names?
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
       
        if ((physical_group = feenox_get_physical_group_ptr(name, this)) == NULL) {
          // create new physical group
          if ((physical_group = feenox_define_physical_group_get_ptr(name, this, dimension, tag)) == NULL) {
            return FEENOX_ERROR;
          }
        } else {
          // there already exists a physical group created with PHYSICAL_GROUP
          // check that either it does not have a tag or the tags coincide
          if (physical_group->tag == 0) {
            physical_group->tag = tag;
          } else if (physical_group->tag != tag) {
            feenox_push_error_message("physical group '%s' has tag %d in input and %d in mesh '%s'", name, physical_group->tag, tag, this->file->name);
            return FEENOX_ERROR;
          }
          // same for the dimension
          if (physical_group->dimension <= 0) {
            physical_group->dimension = dimension;
          } else if (physical_group->dimension != dimension) {
            feenox_push_error_message("physical group '%s' has dimension %d in input and %d in mesh '%s'", name, physical_group->dimension, dimension, this->file->name);
            return FEENOX_ERROR;
          }
          
        }

        // add the group to a local hash so we can solve the pointer
        // "more or less" easy
        HASH_ADD(hh_tag[dimension], this->physical_groups_by_tag[dimension], tag, sizeof(int), physical_group);

        // try to autoatically link materials and BCs
        // actually it should be either one (material) or the other (bc) or none
        
        // if the physical group does not already have a material, look for one with the same name
        if (physical_group->material == NULL) {
          // this returns NULL if not found, which is ok
          HASH_FIND_STR(feenox.mesh.materials, name, physical_group->material);
        }
        
        // same for BCs, although slightly different because there can be many
        bc_t *bc = NULL;
        HASH_FIND_STR(feenox.mesh.bcs, name, bc);
        if (bc != NULL) {
          LL_APPEND(physical_group->bcs, bc);
        }

        feenox_free(name);

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
 
      // number of entities per dimension
      if (fscanf(this->file->pointer, "%ld %ld %ld %ld", &this->points, &this->curves, &this->surfaces, &this->volumes) < 4) {
        return FEENOX_ERROR;
      }
      size_t i;
      for (i = 0; i < this->points+this->curves+this->surfaces+this->volumes; i++) {
        size_t dimension;
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
          if (fscanf(this->file->pointer, "%d %lf %lf %lf %ld",
                     &geometrical_entity->tag,
                     &geometrical_entity->boxMinX,
                     &geometrical_entity->boxMinY,
                     &geometrical_entity->boxMinZ,
                     &geometrical_entity->num_physicals) < 5) {
            feenox_push_error_message("not enough data in entities");
            return FEENOX_ERROR;
          }
        } else {
          if (fscanf(this->file->pointer, "%d %lf %lf %lf %lf %lf %lf %ld",
                     &geometrical_entity->tag,
                     &geometrical_entity->boxMinX,
                     &geometrical_entity->boxMinY,
                     &geometrical_entity->boxMinZ,
                     &geometrical_entity->boxMaxX,
                     &geometrical_entity->boxMaxY,
                     &geometrical_entity->boxMaxZ,
                     &geometrical_entity->num_physicals) < 8) {
            feenox_push_error_message("not enough data in entities");
            return FEENOX_ERROR;
          }
        }        
        
        if (geometrical_entity->num_physicals != 0) {
          feenox_check_alloc(geometrical_entity->physical = calloc(geometrical_entity->num_physicals, sizeof(int)));
          size_t j;
          for (j = 0; j < geometrical_entity->num_physicals; j++) {
            if (fscanf(this->file->pointer, "%d", &geometrical_entity->physical[j]) == 0) {
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
            feenox_check_alloc(geometrical_entity->bounding = calloc(geometrical_entity->num_bounding, sizeof(int)));
            for (size_t j = 0; j < geometrical_entity->num_bounding; j++) {
              // some groups can be negative because the tag is multiplied by the orientation
              if (fscanf(this->file->pointer, "%d", &geometrical_entity->bounding[j]) == 0) {
                return FEENOX_ERROR;
              }
            }
          }
        }
        
        // add the entity to the hashed map of its dimensions
        HASH_ADD(hh[dimension], this->geometrical_entities[dimension], tag, sizeof(int), geometrical_entity);
        
      }

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
    } else if (strncmp("$PartitionedEntities", buffer, 20) == 0) {
      
      // number of partitions and ghost entitites
      if (fscanf(this->file->pointer, "%ld %ld", &this->num_partitions, &this->num_ghost_entitites) < 2) {
        return FEENOX_ERROR;
      }
      
      if (this->num_ghost_entitites != 0) {
        feenox_push_error_message("ghost entitites not yet handled");
        return FEENOX_ERROR;
      }
      
      // number of entities per dimension
      if (fscanf(this->file->pointer, "%ld %ld %ld %ld", &this->partitioned_points, &this->partitioned_curves, &this->partitioned_surfaces, &this->partitioned_volumes) < 4) {
        return FEENOX_ERROR;
      }
      
      size_t i;
      for (i = 0; i < this->partitioned_points+this->partitioned_curves+this->partitioned_surfaces+this->partitioned_volumes; i++) {
        size_t dimension;
        if (i < this->partitioned_points) {
          dimension = 0;
        } else if (i < this->partitioned_points+this->partitioned_curves) {
          dimension = 1;          
        } else if (i < this->partitioned_points+this->partitioned_curves+this->partitioned_surfaces) {
          dimension = 2;          
        } else {
          dimension = 3;
        }
        
        feenox_check_alloc(geometrical_entity = calloc(1, sizeof(geometrical_entity_t)));
        if (fscanf(this->file->pointer, "%d %d %d %ld",
                   &geometrical_entity->tag,
                   &geometrical_entity->parent_dim,
                   &geometrical_entity->parent_tag,
                   &geometrical_entity->num_partitions) < 4) {
            feenox_push_error_message("not enough data in partitioned entities");
            return FEENOX_ERROR;
        }

        if (geometrical_entity->num_partitions != 0) {
          feenox_check_alloc(geometrical_entity->partition = calloc(geometrical_entity->num_partitions, sizeof(int)));
          size_t j;
          for (j = 0; j < geometrical_entity->num_partitions; j++) {
            if (fscanf(this->file->pointer, "%d", &geometrical_entity->partition[j]) == 0) {
              return FEENOX_ERROR;
            }
          }
        }  
        
        if (dimension == 0 && version_maj == 4 && version_min >= 1) {
          if (fscanf(this->file->pointer, "%lf %lf %lf %ld",
                     &geometrical_entity->boxMinX,
                     &geometrical_entity->boxMinY,
                     &geometrical_entity->boxMinZ,
                     &geometrical_entity->num_physicals) < 4) {
            feenox_push_error_message("not enough data in partitioned entities");
            return FEENOX_ERROR;
          }
        } else {
          if (fscanf(this->file->pointer, "%lf %lf %lf %lf %lf %lf %ld",
                     &geometrical_entity->boxMinX,
                     &geometrical_entity->boxMinY,
                     &geometrical_entity->boxMinZ,
                     &geometrical_entity->boxMaxX,
                     &geometrical_entity->boxMaxY,
                     &geometrical_entity->boxMaxZ,
                     &geometrical_entity->num_physicals) < 7) {
            feenox_push_error_message("not enough data in partitioned entities");
            return FEENOX_ERROR;
          }
        }

        if (geometrical_entity->num_physicals != 0) {
          feenox_check_alloc(geometrical_entity->physical = calloc(geometrical_entity->num_physicals, sizeof(int)));
          size_t j;
          for (j = 0; j < geometrical_entity->num_physicals; j++) {
            if (fscanf(this->file->pointer, "%d", &geometrical_entity->physical[j]) == 0) {
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
            feenox_check_alloc(geometrical_entity->bounding = calloc(geometrical_entity->num_bounding, sizeof(int)));
            for (size_t j = 0; j < geometrical_entity->num_bounding; j++) {
              // some groups can be negative because the tag is multiplied by the orientation
              if (fscanf(this->file->pointer, "%d", &geometrical_entity->bounding[j]) == 0) {
                return FEENOX_ERROR;
              }
            }
          }
        }
        
        // add the entity to the hashed map of its dimensions
        HASH_ADD(hh[dimension], this->geometrical_entities[dimension], tag, sizeof(int), geometrical_entity);
        
      }

      if (fgets(buffer, BUFFER_LINE_SIZE-1, this->file->pointer) == NULL) {
        feenox_push_error_message("corrupted mesh file '%s'", this->file->path);
        return -3;
      } 

      // the line $EndPartitionedEntities
      if (fgets(buffer, BUFFER_LINE_SIZE-1, this->file->pointer) == NULL) {
        feenox_push_error_message("corrupted mesh file '%s'", this->file->path);
        return -3;
      } 
      if (strncmp("$EndPartitionedEntities", buffer, 23) != 0) {
        feenox_push_error_message("$EndPartitionedEntities not found in mesh file '%s'", this->file->path);
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
          size_t tag;
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
          
        }
        
        // finished reading the node data, check if they are sparse
        // if they are, build tag2index
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
        // number of blocks and nodes
        size_t blocks;
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
          size_t geometrical;
          size_t dimension;
          size_t parametric;
          size_t num;
          // v4.0 and v4.1 have these two switched
          if (version_min == 0) {
            if (fscanf(this->file->pointer, "%ld %ld %ld %ld", &geometrical, &dimension, &parametric, &num) < 4) {
              feenox_push_error_message("not enough data in node block");
              return FEENOX_ERROR;
            }
          } else {
            if (fscanf(this->file->pointer, "%ld %ld %ld %ld", &dimension, &geometrical, &parametric, &num) < 4) {
              feenox_push_error_message("not enough data in node block");
              return FEENOX_ERROR;
            }
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
              
              // in msh4 the tags are the incides of the global mesh
              this->node[i].index_mesh = i;
              
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
        // number of elements
        if (fscanf(this->file->pointer, "%ld", &(this->n_elements)) == 0) {
          return FEENOX_ERROR;
        }
        if (this->n_elements == 0) {
          feenox_push_error_message("no elements found in mesh file '%s'", this->file->path);
          return -2;
        }
        this->element = calloc(this->n_elements, sizeof(element_t));

        for (size_t i = 0; i < this->n_elements; i++) {

          size_t tag;
          int ntags22;
          int type;
          if (fscanf(this->file->pointer, "%ld %d %d", &tag, &type, &ntags22) < 3) {
            return FEENOX_ERROR;
          }

          // in msh2 the tags are the indices
          if (i+1 != tag) {
            feenox_push_error_message("nodes in file '%s' are non-contiguous, which should not happen in v2.2", this->file->path);
            return FEENOX_ERROR;
          }
          this->element[i].tag = tag;
          this->element[i].index = i;

          if (type >= NUMBER_ELEMENT_TYPE) {
            feenox_push_error_message("elements of type '%d' are not supported in FeenoX", type);
            return FEENOX_ERROR;
          }
          if (feenox.mesh.element_types[type].nodes == 0) {
            feenox_push_error_message("elements of type '%s' are not supported in FeenoX", this->element[i].type->name);
            return FEENOX_ERROR;
          }
          this->element[i].type = &(feenox.mesh.element_types[type]);

          // format v2.2
          // each element has a tag which is an array of ints
          // first one is the ide of the physical entity
          // second one is the id of the geometrical entity (not interested)
          // then other optional stuff like partitions, domains, etc
          int *tags22;
          if (ntags22 > 0) {
            feenox_check_alloc(tags22 = malloc(ntags22 * sizeof(int)));
            for (size_t k = 0; k < ntags22; k++) {
              if (fscanf(this->file->pointer, "%d", &tags22[k]) == 0) {
                return FEENOX_ERROR;
              }
            }
            
            if (ntags22 > 1) {
              unsigned int dimension = this->element[i].type->dim;
              HASH_FIND(hh_tag[dimension], this->physical_groups_by_tag[dimension], &tags22[0], sizeof(int), physical_group);
              if ((this->element[i].physical_group = physical_group) == NULL) {
                // if we did not find anything, create one
                snprintf(buffer, BUFFER_LINE_SIZE-1, "%s_%d_%d", this->file->name, dimension, tags22[0]);
                if ((this->element[i].physical_group = feenox_define_physical_group_get_ptr(buffer, this, this->element[i].type->dim, tags22[0])) == NULL) {
                  return FEENOX_ERROR;
                }
                HASH_ADD(hh_tag[dimension], this->physical_groups_by_tag[dimension], tag, sizeof(int), this->element[i].physical_group);
              }
              this->element[i].physical_group->n_elements++;
            }
            feenox_free(tags22);
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
        size_t blocks;
        if (version_min == 0) {
          // in 4.0 there's no min/max (I asked for this)
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
          int geometrical;
          int dimension;
          int type;
          size_t num;
          if (version_min == 0) {
            if (fscanf(this->file->pointer, "%d %d %d %ld", &geometrical, &dimension, &type, &num) < 4) {
              return FEENOX_ERROR;
            }
          } else {
            if (fscanf(this->file->pointer, "%d %d %d %ld", &dimension, &geometrical, &type, &num) < 4) {
              return FEENOX_ERROR;
            }
          }  
          
          if (type >= NUMBER_ELEMENT_TYPE) {
            feenox_push_error_message("elements of type '%d' are not supported in this version", type);
            return FEENOX_ERROR;
          }
          if (feenox.mesh.element_types[type].nodes == 0) {
            feenox_push_error_message("elements of type '%s' are not supported in this version", feenox.mesh.element_types[type].name);
            return FEENOX_ERROR;
          }
          
          physical_group = NULL;
          if (geometrical != 0) {
            // the whole block has the same physical group, find it once and that's it
            HASH_FIND(hh[dimension], this->geometrical_entities[dimension], &geometrical, sizeof(int), geometrical_entity);
            if (geometrical_entity == NULL) {
              feenox_push_error_message("geometrical entity '%d' of dimension '%d' does not exist", geometrical, dimension);
              return FEENOX_ERROR;
            }
            if (geometrical_entity->num_physicals > 0) {
              // que hacemos si hay mas de una? la primera? la ultima?
              // TODO: todos! hacer un linked list
              int physical = geometrical_entity->physical[0];
              HASH_FIND(hh_tag[dimension], this->physical_groups_by_tag[dimension], &physical, sizeof(int), physical_group);
              if ((this->element[i].physical_group = physical_group) == NULL) {
                snprintf(buffer, BUFFER_LINE_SIZE-1, "%s_%d_%d", this->file->name, dimension, physical);
                if ((this->element[i].physical_group = feenox_define_physical_group_get_ptr(buffer, this, feenox.mesh.element_types[type].dim, physical)) == NULL) {
                  return FEENOX_ERROR;
                }
                this->element[i].physical_group->tag = physical;
                HASH_ADD(hh_tag[dimension], this->physical_groups_by_tag[dimension], tag, sizeof(int), this->element[i].physical_group);
              }
            }  
          }
                    
          for (size_t k = 0; k < num; k++) {
            if (fscanf(this->file->pointer, "%ld", &this->element[i].tag) == 0) {
              return FEENOX_ERROR;
            }
            
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
                feenox_push_error_message("node %d in element %d does not exist", node, this->element[i].tag);
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

      // string-tags
      int n_string_tags = 0;
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
      
      if ((dummy = strrchr(buffer, '\"')) != NULL) {
        *dummy = '\0';
      }
      char *string_tag = NULL;
      if ((dummy = strchr(buffer, '\"')) != NULL) {
        feenox_check_alloc(string_tag = strdup(dummy+1));
      } else {
        feenox_check_alloc(string_tag = strdup(dummy));
      }
      
      function_t *function = NULL;
      node_data_t *node_data = NULL;
      LL_FOREACH(this->node_datas, node_data) {
        if (strcmp(string_tag, node_data->name_in_mesh) == 0) {
          function = node_data->function;
          feenox_check_alloc(function->name_in_mesh = strdup(node_data->name_in_mesh));
        }
      }
      
      // if this function was not requested then we can ignore the block
      if (function == NULL) {
        continue;
      }
      
      // real-tags
      int n_real_tags = 0;
      if (fscanf(this->file->pointer, "%d", &n_real_tags) == 0) {
        feenox_push_error_message("error reading file");
        return FEENOX_ERROR;
      }
      if (n_real_tags != 1) {
        feenox_push_error_message("expected only one real tag for '%s'", string_tag);
        return FEENOX_ERROR;
      }
      double time = 0;
      if (fscanf(this->file->pointer, "%lf", &time) == 0) {
        feenox_push_error_message("error reading file");
        return FEENOX_ERROR;
      }
      // TODO: what to do here?
      // we read only data for t = 0
      if (time != 0) {
        continue;
      }
      
      // integer-tags
      int n_integer_tags = 0;
      if (fscanf(this->file->pointer, "%d", &n_integer_tags) == 0) {
        feenox_push_error_message("error reading file");
        return FEENOX_ERROR;
      }
      if (n_integer_tags != 3) {
        feenox_push_error_message("expected three integer tags for '%s'", string_tag);
        return FEENOX_ERROR;
      }
      int timestep = 0;
      if (fscanf(this->file->pointer, "%d", &timestep) == 0) {
        feenox_push_error_message("error reading file");
        return FEENOX_ERROR;
      }
      int dofs = 0;
      if (fscanf(this->file->pointer, "%d", &dofs) == 0) {
        feenox_push_error_message("error reading file");
        return FEENOX_ERROR;
      }
      int nodes = 0;
      if (fscanf(this->file->pointer, "%d", &nodes) == 0) {
        feenox_push_error_message("error reading file");
        return FEENOX_ERROR;
      }
      
      if (dofs != 1 || nodes != this->n_nodes) {
        continue;
      }
      
      // if we made it this far, we have a function!
      // TODO: use vectors?
      if (function->data_size != nodes) {
        function->type = function_type_pointwise_mesh_node;
        function->mesh = this;
        function->data_argument = this->nodes_argument;
        function->data_size = nodes;
        if (function->data_value != NULL) {
          feenox_free(function->data_value);
        }
        function->data_value = calloc(nodes, sizeof(double));
      }  
      
      size_t j = 0;
      size_t node = 0;
      double value = 0;
      for (j = 0; j < nodes; j++) {
        if (fscanf(this->file->pointer, "%ld %lf", &node, &value) == 0) {
          feenox_push_error_message("error reading file");
          return FEENOX_ERROR;
        }
        if ((node_index = (this->sparse==0)?node-1:this->tag2index[node]) < 0) {
          feenox_push_error_message("node %d does not exist", node);
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
    } else {
        
      do {
        if (fgets(buffer, BUFFER_LINE_SIZE-1, this->file->pointer) == NULL) {
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


int feenox_mesh_write_header_gmsh(FILE *file) {
  fprintf(file, "$MeshFormat\n");
  fprintf(file, "2.2 0 8\n");
  fprintf(file, "$EndMeshFormat\n");

  return FEENOX_OK;
}

int feenox_mesh_write_mesh_gmsh(mesh_t *this, FILE *file, int no_physical_names) {
  
  physical_group_t *physical_group;

  if (no_physical_names == 0) {
    unsigned int n = HASH_COUNT(this->physical_groups);
    if (n != 0) {
      fprintf(file, "$PhysicalNames\n");
      fprintf(file, "%d\n", n);
  
      // y despues barrerlas
      for (physical_group = this->physical_groups; physical_group != NULL; physical_group = physical_group->hh.next) {
        fprintf(file, "%d %d \"%s\"\n", physical_group->dimension, physical_group->tag, physical_group->name);
      }
      fprintf(file, "$EndPhysicalNames\n");
    }
  }
  
  
  fprintf(file, "$Nodes\n");
  fprintf(file, "%ld\n", this->n_nodes);
  size_t i = 0;
  for (i = 0; i < this->n_nodes; i++) {
    fprintf(file, "%ld %g %g %g\n", this->node[i].tag, this->node[i].x[0], this->node[i].x[1], this->node[i].x[2]);
  }
  fprintf(file, "$EndNodes\n");

  fprintf(file, "$Elements\n");
  fprintf(file, "%ld\n", this->n_elements);
  for (i = 0; i < this->n_elements; i++) {
    fprintf(file, "%ld ", this->element[i].tag);
    fprintf(file, "%d ", this->element[i].type->id);

    // in principle we should write the detailed information about groups and partitons
//    fprintf(file, "%d ", this->element[i].ntags);
    
    // but for now only two tags are enough:
    // the first one is the physical group and the second one ought to be the geometrical group
    // if there is no such information, then we just duplicate the physical group tag
    if (this->element[i].physical_group != NULL) {
      fprintf(file, "2 %d %d", this->element[i].physical_group->tag, this->element[i].physical_group->tag);
    } else {
      fprintf(file, "2 0 0");
    }
    // the nodes
    unsigned int j = 0;
    for (j = 0; j < this->element[i].type->nodes; j++) {
      fprintf(file, " %ld", this->element[i].node[j]->tag);
    }
    fprintf(file, "\n");
  }
  fprintf(file, "$EndElements\n");
  
  return FEENOX_OK;
  
}


int feenox_mesh_write_scalar_gmsh(mesh_write_t *mesh_write, function_t *function, field_location_t field_location, char *printf_format) {

  mesh_t *mesh = NULL;
  
  if (mesh_write->mesh != NULL) {
    mesh = mesh_write->mesh;
  } else if (function != NULL) {
    mesh = function->mesh;
  } else {
    feenox_push_error_message("do not know which mesh to apply to post-process function '%s'", function->name);
    return FEENOX_ERROR;
  }

  // TODO
  if (field_location == field_location_cells) {
    fprintf(mesh_write->file->pointer, "$ElementData\n");
    if (mesh->n_cells == 0) {
      feenox_call(feenox_mesh_element2cell(mesh));
    }
  } else {
    fprintf(mesh_write->file->pointer, "$NodeData\n");
  }

  // one string tag
  fprintf(mesh_write->file->pointer, "1\n");
  // the name of the vew
  fprintf(mesh_write->file->pointer, "\"%s\"\n", function->name);
  // the other one (optional) is the interpolation scheme
  
  // one real tag (only one)
  fprintf(mesh_write->file->pointer, "1\n");                          
  // time
  fprintf(mesh_write->file->pointer, "%g\n", feenox_special_var_value(t));

  // thre integer tags
  fprintf(mesh_write->file->pointer, "3\n");
  // timestep
  fprintf(mesh_write->file->pointer, "%d\n", (feenox_special_var_value(end_time) > 0) ? (unsigned int)(feenox_special_var_value(step_transient)) : (unsigned int)(feenox_special_var_value(step_static)));
  // number of data per node: one (scalar)
  fprintf(mesh_write->file->pointer, "%d\n", 1);

  // custom format
  char *format = NULL;
  if (printf_format == NULL) {
    feenox_check_alloc(format = strdup("%ld %g\n"));
  } else {
    if (printf_format[0] == '%') {
      feenox_check_minusone(asprintf(&format, "%%ld %s\n", printf_format));
    } else {
      feenox_check_minusone(asprintf(&format, "%%ld %%%s\n", printf_format));
    }  
  }
  
  size_t i = 0;
  if (field_location == field_location_cells) {
    // number of data
    fprintf(mesh_write->file->pointer, "%ld\n", mesh->n_cells);

    if (function->type == function_type_pointwise_mesh_cell && function->mesh == mesh) {
      for (i = 0; i < function->data_size; i++) {
        fprintf(mesh_write->file->pointer, format, mesh->cell[i].element->tag, function->data_value[i]);
      }
    } else {
      for (i = 0; i < mesh->n_cells; i++) {
        fprintf(mesh_write->file->pointer, format, mesh->cell[i].element->tag, feenox_function_eval(function, mesh->cell[i].x));
      }
    }
    fprintf(mesh_write->file->pointer, "$EndElementData\n");
    
  } else  {
    
    // number of data
    fprintf(mesh_write->file->pointer, "%ld\n", mesh->n_nodes);              
  
    if (function->type == function_type_pointwise_mesh_node && function->mesh == mesh) {
      for (i = 0; i < function->data_size; i++) {
        fprintf(mesh_write->file->pointer, format, mesh->node[i].tag, function->data_value[i]);
      }
    } else {
      for (i = 0; i < mesh->n_nodes; i++) {
        fprintf(mesh_write->file->pointer, format, mesh->node[i].tag, feenox_function_eval(function, mesh->node[i].x));
      }
    }
    fprintf(mesh_write->file->pointer, "$EndNodeData\n");    
  }
  
  fflush(mesh_write->file->pointer);
  feenox_free(format);

  return FEENOX_OK;

}

int feenox_mesh_write_vector_gmsh(mesh_write_t *mesh_write, function_t **function, field_location_t field_location, char *printf_format) {

  mesh_t *mesh = NULL;
  if (mesh_write->mesh != NULL) {
    mesh = mesh_write->mesh;
  } else if (function != NULL) {
    mesh = function[0]->mesh;
  } else {
    return FEENOX_ERROR;
  }

  if (field_location == field_location_cells) {
    fprintf(mesh_write->file->pointer, "$ElementData\n");
    if (mesh->n_cells == 0) {
      feenox_call(feenox_mesh_element2cell(mesh));
    }
  } else {
    fprintf(mesh_write->file->pointer, "$NodeData\n");
  }

  // only one string tag
  fprintf(mesh_write->file->pointer, "1\n");
  // view name
  fprintf(mesh_write->file->pointer, "\"%s_%s_%s\"\n", function[0]->name, function[1]->name, function[2]->name);
  // the other optional is the interpolation scheme
  
  // only one real tag
  fprintf(mesh_write->file->pointer, "1\n");                          
  // time
  fprintf(mesh_write->file->pointer, "%g\n", (feenox_special_var_value(end_time) > 0) ? feenox_special_var_value(t) : feenox_special_var_value(step_static));

  // three integer tags
  fprintf(mesh_write->file->pointer, "3\n");
  // timestep
  fprintf(mesh_write->file->pointer, "%d\n", (feenox_special_var_value(end_time) > 0) ? (unsigned int)(feenox_special_var_value(step_transient)) : (unsigned int)(feenox_special_var_value(step_static)));
  // number of values per point 3 (a vector)
  fprintf(mesh_write->file->pointer, "%d\n", 3);

  
  if (field_location == field_location_cells) {

    // number of point data
    fprintf(mesh_write->file->pointer, "%ld\n", mesh->n_cells);              

    // custom format
    char *format = NULL;
    if (printf_format == NULL) {
      feenox_check_alloc(format = strdup("%ld %g %g %g\n"));
    } else {
      if (printf_format[0] == '%') {
        feenox_check_minusone(asprintf(&format, "%%ld %s %s %s\n", printf_format, printf_format, printf_format));
      } else {
        feenox_check_minusone(asprintf(&format, "%%ld %%%s %%%s %%%s\n", printf_format, printf_format, printf_format));
      }  
    }
    
    size_t i = 0;
    for (i = 0; i < mesh->n_cells; i++) {
      fprintf(mesh_write->file->pointer, format, mesh->cell[i].element->tag,
                                                 feenox_function_eval(function[0], mesh->cell[i].x),
                                                 feenox_function_eval(function[1], mesh->cell[i].x),
                                                 feenox_function_eval(function[2], mesh->cell[i].x));
    }
    fprintf(mesh_write->file->pointer, "$EndElementData\n");
    feenox_free(format);
    
  } else {
    
    // number of point data
    fprintf(mesh_write->file->pointer, "%ld\n", mesh->n_nodes);              

    // custom format
    char *format = NULL;
    if (printf_format == NULL) {
      feenox_check_alloc(format = strdup("%g "));
    } else {
      if (printf_format[0] == '%') {
        feenox_check_minusone(asprintf(&format, "%s ", printf_format));
      } else {
        feenox_check_minusone(asprintf(&format, "%%%s ", printf_format));
      }  
    }
  
    size_t j = 0;
    for (j = 0; j < mesh->n_nodes; j++) {
      fprintf(mesh_write->file->pointer, "%ld ", mesh->node[j].tag);
      
      if (function[0]->type == function_type_pointwise_mesh_node && function[0]->data_size == mesh_write->mesh->n_nodes) {
        fprintf(mesh_write->file->pointer, format, function[0]->data_value[j]);
      } else {
        fprintf(mesh_write->file->pointer, format, feenox_function_eval(function[0], mesh->node[j].x));
      }

      if (function[1]->type == function_type_pointwise_mesh_node && function[1]->data_size == mesh_write->mesh->n_nodes) {
        fprintf(mesh_write->file->pointer, format, function[1]->data_value[j]);
      } else {
        fprintf(mesh_write->file->pointer, format, feenox_function_eval(function[1], mesh->node[j].x));
      }

      if (function[2]->type == function_type_pointwise_mesh_node && function[2]->data_size == mesh_write->mesh->n_nodes) {
        fprintf(mesh_write->file->pointer, format, function[2]->data_value[j]);
      } else {
        fprintf(mesh_write->file->pointer, format, feenox_function_eval(function[2], mesh->node[j].x));
      }
      fprintf(mesh_write->file->pointer, "\n");
    }
    fprintf(mesh_write->file->pointer, "$EndNodeData\n");
  }
 
  fflush(mesh_write->file->pointer);
  

  return FEENOX_OK;

}


// read the next available time step and interpolate
int feenox_mesh_update_function_gmsh(function_t *function, double t, double dt) {
 
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
    feenox_free(new_data);
  }
 
  return FEENOX_OK;
  
}
