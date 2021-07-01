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

#define feenox_mesh_gmsh_readnewline()  \
  if (fgets(buffer, BUFFER_LINE_SIZE-1, fp) == NULL) { \
    feenox_push_error_message("corrupted mesh '%s'", this->file->path); \
    return FEENOX_ERROR; \
  }

int feenox_mesh_read_gmsh(mesh_t *this) {

  if (this->file->pointer == NULL) {
    feenox_call(feenox_instruction_file_open(this->file));
  }
  FILE *fp = this->file->pointer;

  char *dummy = NULL;
  char *name = NULL;
  physical_group_t *physical_group = NULL;
  geometrical_entity_t *geometrical_entity = NULL;
  size_t node = 0;
  size_t node_index = 0;
  unsigned int version_maj = 0;
  unsigned int version_min = 0;
  int binary = 0;
  
  
  char buffer[BUFFER_LINE_SIZE];
  while (fgets(buffer, BUFFER_LINE_SIZE-1, fp) != NULL) {

    if (strncmp("\n", buffer, 1) == 0) {
      ;
    
    // ------------------------------------------------------  
    } else if (strncmp("$MeshFormat", buffer, 11) == 0) {
  
      // version
      if (fscanf(fp, "%s", buffer) == 0) {
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
  
      // file-type (ASCII int; 0 for ASCII mode, 1 for binary mode)
      if (fscanf(fp, "%d", &binary) == 0) {
        return FEENOX_ERROR;
      }
      
      if (binary && (version_maj == 2 || version_min == 0))  {
        feenox_push_error_message("binary gmsh reader works only for version 4.1");
        return FEENOX_ERROR;
      }
  
      // data-size (ASCII int; the size of size_t)
      size_t size_of_size_t = 0;
      feenox_call(feenox_gmsh_read_data_size_t(this, 1, &size_of_size_t, 0));
      
      if (size_of_size_t != sizeof(size_t)) {
        feenox_push_error_message("size of size_t in mesh '%s' is %d and in the system is %d", this->file->name, size_of_size_t, sizeof(size_t));
        return FEENOX_ERROR;
      }

      feenox_mesh_gmsh_readnewline();
      
      // for binary, we have to read one to check endianness
      if (binary) {
        int one = 0;
        feenox_call(feenox_gmsh_read_data_int(this, 1, &one, 1));
        if (one != 1) {
          feenox_push_error_message("incompatible binary endianness in mesh '%s'", this->file->name);
          return FEENOX_ERROR;
        }
        feenox_mesh_gmsh_readnewline();
      }
      
      // line $EndMeshFormat
      feenox_mesh_gmsh_readnewline();
      if (strncmp("$EndMeshFormat", buffer, 14) != 0) {
        feenox_push_error_message("$EndMeshFormat not found in mesh '%s'", this->file->path);
        return FEENOX_ERROR;
      }

    // ------------------------------------------------------      
    } else if (strncmp("$PhysicalNames", buffer, 14) == 0) {

      // if there are physical names then we define implicitly each group
      // if they already exist we check the ids

      // number of names
      if (fscanf(fp, "%d", &this->n_physical_names) == 0) {
        return FEENOX_ERROR;
      }
 
      unsigned int i = 0;
      for (i = 0; i < this->n_physical_names; i++) {

        int dimension = 0;
        int tag = 0;
        if (fscanf(fp, "%d %d", &dimension, &tag) < 2) {
          return FEENOX_ERROR;
        }
        if (dimension < 0 || dimension > 4) {
          feenox_push_error_message("invalid dimension %d for physical group %d in mesh file '%s'", dimension, tag, this->file->path);
          return FEENOX_ERROR;
        }
        
        feenox_mesh_gmsh_readnewline();
        
        if ((dummy = strrchr(buffer, '\"')) != NULL) {
          *dummy = '\0';
        }
        if ((dummy = strchr(buffer, '\"')) != NULL) {
          feenox_check_alloc(name = strdup(dummy+1));
        } else {
          feenox_check_alloc(name = strdup(buffer));
        }
        
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
      feenox_mesh_gmsh_readnewline();
      if (strncmp("$EndPhysicalNames", buffer, 17) != 0) {
        feenox_push_error_message("$EndPhysicalNames not found in mesh file '%s'", this->file->path);
        return -2;
      }
      
    // ------------------------------------------------------      
    } else if (strncmp("$Entities", buffer, 9) == 0) {
 
      // number of entities per dimension
      size_t num_entities[4] = {0, 0, 0, 0};
      feenox_call(feenox_gmsh_read_data_size_t(this, 4, num_entities, binary));
      this->points = num_entities[0];
      this->curves = num_entities[1];
      this->surfaces = num_entities[2];
      this->volumes = num_entities[3];
      
      size_t i = 0;
      for (i = 0; i < this->points+this->curves+this->surfaces+this->volumes; i++) {
        unsigned int dimension = 0;
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
        feenox_call(feenox_gmsh_read_data_int(this, 1, &geometrical_entity->tag, binary));
        
        double bbox[6] = {0,0,0,0,0,0};
        // since version 4.1 points have only 3 numbers, not 6 for bounding box
        if (dimension == 0 && version_maj == 4 && version_min >= 1) {
          feenox_call(feenox_gmsh_read_data_double(this, 3, bbox, binary));
        } else {
          feenox_call(feenox_gmsh_read_data_double(this, 6, bbox, binary));
        }
        
        geometrical_entity->boxMinX = bbox[0];
        geometrical_entity->boxMinY = bbox[1];
        geometrical_entity->boxMinZ = bbox[2];
        geometrical_entity->boxMaxX = bbox[3];
        geometrical_entity->boxMaxY = bbox[4];
        geometrical_entity->boxMaxZ = bbox[5];
        
        feenox_call(feenox_gmsh_read_data_size_t(this, 1, &geometrical_entity->num_physicals, binary));
        if (geometrical_entity->num_physicals != 0) {
          feenox_check_alloc(geometrical_entity->physical = calloc(geometrical_entity->num_physicals, sizeof(int)));
          feenox_call(feenox_gmsh_read_data_int(this, geometrical_entity->num_physicals, geometrical_entity->physical, binary));
        }
        
        // points do not have bounding groups
        if (dimension != 0) {
          feenox_call(feenox_gmsh_read_data_size_t(this, 1, &geometrical_entity->num_bounding, binary));
          if (geometrical_entity->num_bounding != 0) {
            feenox_check_alloc(geometrical_entity->bounding = calloc(geometrical_entity->num_bounding, sizeof(int)));
            feenox_call(feenox_gmsh_read_data_int(this, geometrical_entity->num_bounding, geometrical_entity->bounding, binary));
          }
        }
        
        // add the entity to the hashed map of its dimensions
        HASH_ADD(hh[dimension], this->geometrical_entities[dimension], tag, sizeof(int), geometrical_entity);
        
      }

      feenox_mesh_gmsh_readnewline();
      
      // the line $EndEntities
      feenox_mesh_gmsh_readnewline();
      if (strncmp("$EndEntities", buffer, 12) != 0) {
        feenox_push_error_message("$EndEntities not found in mesh file '%s'", this->file->path);
        return -2;
      }
      
    // ------------------------------------------------------      
    } else if (strncmp("$PartitionedEntities", buffer, 20) == 0) {
      
      // number of partitions and ghost entitites
      size_t numbers[2] = {0,0};
      feenox_call(feenox_gmsh_read_data_size_t(this, 2, numbers, binary));
      this->num_partitions = numbers[0];
      this->num_ghost_entitites = numbers[1];
      
      if (this->num_ghost_entitites != 0) {
        feenox_push_error_message("ghost entitites not yet handled");
        return FEENOX_ERROR;
      }
      
      // number of entities per dimension
      size_t num_entities[4] = {0, 0, 0, 0};
      feenox_call(feenox_gmsh_read_data_size_t(this, 4, num_entities, binary));
      this->points = num_entities[0];
      this->curves = num_entities[1];
      this->surfaces = num_entities[2];
      this->volumes = num_entities[3];
      
      size_t i = 0;
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
        
        int tag_dim_tag[3] = {0,0,0};
        feenox_call(feenox_gmsh_read_data_int(this, 3, tag_dim_tag, binary));
        geometrical_entity->tag = tag_dim_tag[0];
        geometrical_entity->parent_dim = tag_dim_tag[1];
        geometrical_entity->parent_tag = tag_dim_tag[2];
        
        feenox_call(feenox_gmsh_read_data_size_t(this, 1, &geometrical_entity->num_partitions, binary));
        if (geometrical_entity->num_partitions != 0) {
          feenox_check_alloc(geometrical_entity->partition = calloc(geometrical_entity->num_partitions, sizeof(int)));
          feenox_call(feenox_gmsh_read_data_int(this, geometrical_entity->num_partitions, geometrical_entity->partition, binary));
        }
        
        double bbox[6] = {0,0,0,0,0,0};
        // since version 4.1 points have only 3 numbers, not 6 for bounding box
        if (dimension == 0 && version_maj == 4 && version_min >= 1) {
          feenox_call(feenox_gmsh_read_data_double(this, 3, bbox, binary));
        } else {
          feenox_call(feenox_gmsh_read_data_double(this, 6, bbox, binary));
        }
        
        geometrical_entity->boxMinX = bbox[0];
        geometrical_entity->boxMinY = bbox[1];
        geometrical_entity->boxMinZ = bbox[2];
        geometrical_entity->boxMaxX = bbox[3];
        geometrical_entity->boxMaxY = bbox[4];
        geometrical_entity->boxMaxZ = bbox[5];
        
        feenox_call(feenox_gmsh_read_data_size_t(this, 1, &geometrical_entity->num_physicals, binary));
        if (geometrical_entity->num_physicals != 0) {
          feenox_check_alloc(geometrical_entity->physical = calloc(geometrical_entity->num_physicals, sizeof(int)));
          feenox_call(feenox_gmsh_read_data_int(this, geometrical_entity->num_physicals, geometrical_entity->physical, binary));
        }
        
        // points do not have bounding groups
        if (dimension != 0) {
          feenox_call(feenox_gmsh_read_data_size_t(this, 1, &geometrical_entity->num_bounding, binary));
          if (geometrical_entity->num_bounding != 0) {
            feenox_check_alloc(geometrical_entity->bounding = calloc(geometrical_entity->num_bounding, sizeof(int)));
            feenox_call(feenox_gmsh_read_data_int(this, geometrical_entity->num_bounding, geometrical_entity->bounding, binary));
          }
        }
        
        // add the entity to the hashed map of its dimensions
        HASH_ADD(hh[dimension], this->geometrical_entities[dimension], tag, sizeof(int), geometrical_entity);
        
      }

      feenox_mesh_gmsh_readnewline();

      // the line $EndPartitionedEntities
      feenox_mesh_gmsh_readnewline();
      if (strncmp("$EndPartitionedEntities", buffer, 23) != 0) {
        feenox_push_error_message("$EndPartitionedEntities not found in mesh file '%s'", this->file->path);
        return -2;
      }
      
    // ------------------------------------------------------      
    } else if (strncmp("$Nodes", buffer, 6) == 0) {

      size_t tag_min = SIZE_MAX;
      size_t tag_max = 0;
      
      if (version_maj == 2) {
        // number of nodes
        if (fscanf(fp, "%ld", &(this->n_nodes)) == 0) {
          return FEENOX_ERROR;
        }
        if (this->n_nodes == 0) {
          feenox_push_error_message("no nodes found in mesh '%s'", this->file->path);
          return FEENOX_ERROR;
        }

        feenox_check_alloc(this->node = calloc(this->n_nodes, sizeof(node_t)));

        for (size_t j = 0; j < this->n_nodes; j++) {
          size_t tag;
          if (fscanf(fp, "%ld %lf %lf %lf",
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
          
          this->node[j].tag = tag;
          if (tag < tag_min) {
            tag_min = tag;
          }
          if (tag > tag_max) {
            tag_max = tag;
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
        size_t num_blocks;
        if (version_min == 0) {
          // en 4.0 no tenemos min y max
          if (fscanf(fp, "%ld %ld", &num_blocks, &this->n_nodes) < 2) {
            feenox_push_error_message("error reading node blocks");
            return FEENOX_ERROR;
          }
        } else {
          size_t data[4] = {0,0,0,0};
          feenox_call(feenox_gmsh_read_data_size_t(this, 4, data, binary));
          num_blocks = data[0];
          this->n_nodes = data[1];
          tag_min = data[2];
          tag_max = data[3];
        }
        
        if (this->n_nodes == 0) {
          feenox_push_error_message("no nodes found in mesh '%s'", this->file->path);
          return FEENOX_ERROR;
        }

        feenox_check_alloc(this->node = calloc(this->n_nodes, sizeof(node_t)));

        // tag_min?
        if (tag_max != 0) {
          // we can do this as a one single pass because we have tag_max (I asked for this in v4.1)
          // TODO: offset with tag_min?
          if (tag_max < this->n_nodes) {
            feenox_push_error_message("maximum node tag %d is less that number of nodes %d", tag_max, this->n_nodes);
          }
          feenox_check_alloc(this->tag2index = calloc((tag_max+1), sizeof(size_t)));
          for (size_t k = 0; k <= tag_max; k++) {
            this->tag2index[k] = SIZE_MAX;
          }
        }
        
        size_t j = 0;
        for (size_t l = 0; l < num_blocks; l++) {
          int geometrical;
          int dimension;
          int parametric;
          size_t num_nodes;
          // v4.0 and v4.1 have geometrical and dimension switched
          if (version_min == 0) {
            if (fscanf(fp, "%d %d %d %ld", &geometrical, &dimension, &parametric, &num_nodes) < 4) {
              feenox_push_error_message("not enough data in node block");
              return FEENOX_ERROR;
            }
          } else {
            int data[3] = {0,0,0};
            feenox_call(feenox_gmsh_read_data_int(this, 3, data, binary));
            dimension = data[0];
            geometrical = data[1];
            parametric = data[2];
            feenox_call(feenox_gmsh_read_data_size_t(this, 1, &num_nodes, binary));
          }

          if (parametric) {
            feenox_push_error_message("mesh '%s' contains parametric data, which is unsupported yet", this->file->path);
            return FEENOX_ERROR;
          }
          
          if (version_min == 0) {
            // here we have tag and coordinate in the same line
            for (size_t k = 0; k < num_nodes; k++) {
              if (fscanf(fp, "%ld %lf %lf %lf",
                         &this->node[j].tag,
                         &this->node[j].x[0],
                         &this->node[j].x[1],
                         &this->node[j].x[2]) < 4) {
                feenox_push_error_message("reading node data");
                return FEENOX_ERROR;
              }
              
              if (this->node[j].tag > tag_max) {
                tag_max = this->node[j].tag;
              }
              
              // in msh4 the tags are the incides of the global mesh
              this->node[j].index_mesh = j;
              
              j++;
            }
          } else {

            // here we have first all the tags and then all the coordinates
            size_t *node_tags = NULL;
            feenox_check_alloc(node_tags = calloc(num_nodes, sizeof(size_t)));
            feenox_call(feenox_gmsh_read_data_size_t(this, num_nodes, node_tags, binary));
            size_t k = 0;
            // i has the last fully-read node index
            for (k = 0; k < num_nodes; k++) {
              this->node[j+k].tag = node_tags[k];
            }
            feenox_free(node_tags);
            
            double *node_coords = NULL;
            feenox_check_alloc(node_coords = calloc(3*num_nodes, sizeof(size_t)));
            feenox_call(feenox_gmsh_read_data_double(this, 3*num_nodes, node_coords, binary));
            for (k = 0; k < num_nodes; k++) {
              this->node[j].x[0] = node_coords[3*k+0];
              this->node[j].x[1] = node_coords[3*k+1];
              this->node[j].x[2] = node_coords[3*k+2];
              this->node[j].index_mesh = j;
              this->tag2index[this->node[j].tag] = j;
              j++;
            }
            feenox_free(node_coords);
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
           
      feenox_mesh_gmsh_readnewline();
      
      // the line $EndNodes
      feenox_mesh_gmsh_readnewline();
      if (strncmp("$EndNodes", buffer, 9) != 0) {
        feenox_push_error_message("$EndNodes not found in mesh file '%s'", this->file->path);
        return -2;
      }

    // ------------------------------------------------------      
    } else if (strncmp("$Elements", buffer, 9) == 0) {              

      size_t tag_min = SIZE_MAX;
      size_t tag_max = 0;
      
      if (version_maj == 2) {
        // number of elements
        if (fscanf(fp, "%ld", &(this->n_elements)) == 0) {
          return FEENOX_ERROR;
        }
        if (this->n_elements == 0) {
          feenox_push_error_message("no elements found in mesh file '%s'", this->file->path);
          return -2;
        }
        feenox_check_alloc(this->element = calloc(this->n_elements, sizeof(element_t)));

        size_t i = 0;
        for (i = 0; i < this->n_elements; i++) {

          size_t tag = 0;
          int ntags22 = 0;
          int type = 0;
          if (fscanf(fp, "%ld %d %d", &tag, &type, &ntags22) < 3) {
            return FEENOX_ERROR;
          }

          // in msh2 the tags are the indices
          if (i+1 != tag) {
            feenox_push_error_message("nodes in file '%s' are non-contiguous, which should not happen in v2.2", this->file->path);
            return FEENOX_ERROR;
          }

          this->element[i].index = i;
          this->element[i].tag = tag;
          if (tag < tag_min) {
            tag_min = tag;
          }
          if (tag > tag_max) {
            tag_max = tag;
          }
          

          if (type >= NUMBER_ELEMENT_TYPE) {
            feenox_push_error_message("elements of type '%d' are not supported in FeenoX", type);
            return FEENOX_ERROR;
          }
          if (feenox.mesh.element_types[type].nodes == 0) {
            feenox_push_error_message("elements of type '%s' are not supported in FeenoX", this->element[i].type->name);
            return FEENOX_ERROR;
          }
          this->element[i].type = &(feenox.mesh.element_types[type]);
          this->n_elements_per_dim[this->element[i].type->dim]++;

          // format v2.2
          // each element has a tag which is an array of ints
          // first one is the ide of the physical entity
          // second one is the id of the geometrical entity (not interested)
          // then other optional stuff like partitions, domains, etc
          int *tags22 = NULL;
          if (ntags22 > 0) {
            feenox_check_alloc(tags22 = calloc(ntags22, sizeof(int)));
            for (size_t k = 0; k < ntags22; k++) {
              if (fscanf(fp, "%d", &tags22[k]) == 0) {
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
          
          feenox_check_alloc(this->element[i].node = calloc(this->element[i].type->nodes, sizeof(node_t *)));
          size_t j = 0;
          for (j = 0; j < this->element[i].type->nodes; j++) {
            if (fscanf(fp, "%ld", &node) < 1) {
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
        size_t num_blocks;
        if (version_min == 0) {
          // in 4.0 there's no min/max (I asked for this)
          tag_min = 0;
          tag_max = 0;
          if (fscanf(fp, "%ld %ld", &num_blocks, &this->n_elements) < 2) {
            feenox_push_error_message("error reading element blocks");
            return FEENOX_ERROR;
          }
        } else {
          size_t data[4] = {0,0,0,0};
          feenox_call(feenox_gmsh_read_data_size_t(this, 4, data, binary));
          num_blocks = data[0];
          this->n_elements = data[1];
          tag_min = data[2];
          tag_max = data[3];
        }
        
        if (this->n_elements == 0) {
          feenox_push_error_message("no elements found in mesh file '%s'", this->file->path);
          return FEENOX_ERROR;
        }
        feenox_check_alloc(this->element = calloc(this->n_elements, sizeof(element_t)));

        size_t i = 0;
        for (size_t l = 0; l < num_blocks; l++) {
          int geometrical = 0;
          int dimension = 0;
          int type = 0;
          size_t num_elements = 0;
          if (version_min == 0) {
            if (fscanf(fp, "%d %d %d %ld", &geometrical, &dimension, &type, &num_elements) < 4) {
              return FEENOX_ERROR;
            }
          } else {
            int data[3] = {0,0,0};
            feenox_call(feenox_gmsh_read_data_int(this, 3, data, binary));
            dimension = data[0];
            geometrical = data[1];
            type = data[2];
            feenox_call(feenox_gmsh_read_data_size_t(this, 1, &num_elements, binary));
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
              // what to do if there is more than one? the first? the last one?
              // TODO: all of them! make a linked list
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
          
          size_t *element_data = NULL;
          size_t size_block = 1+feenox.mesh.element_types[type].nodes;
          feenox_check_alloc(element_data = calloc(num_elements*size_block, sizeof(size_t)));
          feenox_call(feenox_gmsh_read_data_size_t(this, num_elements*size_block, element_data, binary));
          
          size_t k = 0;
          // i has the last fully-read node index
          for (k = 0; k < num_elements; k++) {
            this->element[i].tag = element_data[size_block * k + 0];
            this->element[i].index = i;
            this->element[i].type = &(feenox.mesh.element_types[type]);
            this->n_elements_per_dim[this->element[i].type->dim]++;
            
            if ((this->element[i].physical_group = physical_group) != NULL) {
              this->element[i].physical_group->n_elements++;
            }
            
            feenox_check_alloc(this->element[i].node = calloc(this->element[i].type->nodes, sizeof(node_t *)));
            for (size_t j = 0; j < this->element[i].type->nodes; j++) {
              node = element_data[size_block * k + 1 + j];
              // for msh4 we need to use tag2index
              if ((this->element[i].node[j] = &this->node[this->tag2index[node]]) == 0) {
                feenox_push_error_message("node %d in element %d does not exist", node, this->element[i].tag);
                return FEENOX_ERROR;
              }
              feenox_mesh_add_element_to_list(&this->element[i].node[j]->associated_elements, &this->element[i]);
            }
            i++;
          }
          feenox_free(element_data);
        }
      }

      feenox_mesh_gmsh_readnewline();

      // the line $EndElements
      feenox_mesh_gmsh_readnewline();
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

      // number of string tags (ASCII int)
      int n_string_tags = 0;
      feenox_call(feenox_gmsh_read_data_int(this, 1, &n_string_tags, 0));
      if (n_string_tags != 1) {
        feenox_push_error_message("do not know hwo to handle numStringTags != 1");
        return FEENOX_ERROR;
      }
      
      feenox_mesh_gmsh_readnewline();
      
      // actual string tag (string)
      feenox_mesh_gmsh_readnewline();

      char *string_tag = NULL;
      if ((dummy = strrchr(buffer, '\"')) != NULL) {
        *dummy = '\0';
      }
      if ((dummy = strchr(buffer, '\"')) != NULL) {
        feenox_check_alloc(string_tag = strdup(dummy+1));
      } else {
        feenox_check_alloc(string_tag = strdup(buffer));
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
      
      // real-tags (all ASCII stuff)
      int n_real_tags = 0;
      feenox_call(feenox_gmsh_read_data_int(this, 1, &n_real_tags, 0));
      if (n_real_tags != 1) {
        feenox_push_error_message("expected only one real tag for '%s'", string_tag);
        return FEENOX_ERROR;
      }
      
      double time = 0;
      feenox_call(feenox_gmsh_read_data_double(this, 1, &time, 0));
      
      // TODO: what to do here?
      // we read only data for t = 0
      if (time != 0) {
        continue;
      }
      
      // integer-tags
      int n_integer_tags = 0;
      feenox_call(feenox_gmsh_read_data_int(this, 1, &n_integer_tags, 0));
      if (n_integer_tags != 3) {
        feenox_push_error_message("expected three integer tags for '%s'", string_tag);
        return FEENOX_ERROR;
      }
      
      int data[3] = {0,0,0};
      feenox_call(feenox_gmsh_read_data_int(this, 3, data, 0));
//      int timestep = data[0];
      int dofs = data[1];
      int num_nodes = data[2];
      
      if (dofs != 1) {
        feenox_push_error_message("cannot handle non-scalar field '%s'", string_tag);
        return FEENOX_ERROR;
      }
      if (num_nodes != this->n_nodes) {
        feenox_push_error_message("field '%s' has %d nodes and the mesh has %ld nodes", string_tag, num_nodes, this->n_nodes);
        return FEENOX_ERROR;
      }
      
      // if we made it this far, we have a function!
      // TODO: use vectors instead of argument_data
      if (function->data_size != num_nodes) {
        function->type = function_type_pointwise_mesh_node;
        function->mesh = this;
        function->data_argument = this->nodes_argument;
        function->data_size = num_nodes;
        if (function->data_value != NULL) {
          feenox_free(function->data_value);
        }
        feenox_check_alloc(function->data_value = calloc(num_nodes, sizeof(double)));
      }  

      feenox_mesh_gmsh_readnewline();
      
      size_t j = 0;
      int node = 0;
      double value = 0;
      for (j = 0; j < num_nodes; j++) {
        feenox_call(feenox_gmsh_read_data_int(this, 1, &node, binary));
        feenox_call(feenox_gmsh_read_data_double(this, 1, &value, binary));
        
        if ((node_index = (this->sparse==0) ? node-1 : this->tag2index[node]) < 0) {
          feenox_push_error_message("node %d does not exist", node);
          return FEENOX_ERROR;
        }
        function->data_value[node_index] = value;
      }

      // the newline
      feenox_mesh_gmsh_readnewline();

      // the line $ElementNodeData
      feenox_mesh_gmsh_readnewline();
      if (strncmp("$EndNodeData", buffer, 12) != 0 && strncmp("$EndElementData", buffer, 15) != 0) {
        feenox_push_error_message("$EndNodeData not found in mesh file '%s'", this->file->path);
        return -2;
      }
      
    // ------------------------------------------------------      
    } else {
        
      do {
        if (fgets(buffer, BUFFER_LINE_SIZE-1, fp) == NULL) {
          feenox_push_error_message("corrupted mesh file '%s'", this->file->path);
          return -3;
        }
      } while(strncmp("$End", buffer, 4) != 0);
        
    }
  }

  // close the mesh file
  fclose(fp);
  fp = NULL;
  
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


int feenox_gmsh_read_data_int(mesh_t *this, size_t n, int *data, int binary) {

  FILE *fp = this->file->pointer;

  if (binary) {
    if (fread(data, sizeof(int), n, fp) != n) {
      feenox_push_error_message("not enough data in mesh '%s'", this->file->name);
      return FEENOX_ERROR;
    }
  } else {  
    size_t i = 0;
    for (i = 0; i < n; i++) {
      if (fscanf(fp, "%d", &data[i]) != 1) {
        feenox_push_error_message("not enough data in mesh '%s'", this->file->name);
        return FEENOX_ERROR;
      }
    }
  }

  return FEENOX_OK;
}


int feenox_gmsh_read_data_size_t(mesh_t *this, size_t n, size_t *data, int binary) {

  FILE *fp = this->file->pointer;

  if (binary) {
    if (fread(data, sizeof(size_t), n, fp) != n) {
      feenox_push_error_message("not enough data in mesh '%s'", this->file->name);
      return FEENOX_ERROR;
    }
  } else {  
    size_t i = 0;
    for (i = 0; i < n; i++) {
      if (fscanf(fp, "%lu", &data[i]) != 1) {
        feenox_push_error_message("not enough data in mesh '%s'", this->file->name);
        return FEENOX_ERROR;
      }
    }
  }

  return FEENOX_OK;
}

int feenox_gmsh_read_data_double(mesh_t *this, size_t n, double *data, int binary) {

  FILE *fp = this->file->pointer;

  if (binary) {
    if (fread(data, sizeof(double), n, fp) != n) {
      feenox_push_error_message("not enough data in mesh '%s'", this->file->name);
      return FEENOX_ERROR;
    }
  } else {  
    size_t i = 0;
    for (i = 0; i < n; i++) {
      if (fscanf(fp, "%lf", &data[i]) != 1) {
        feenox_push_error_message("not enough data in mesh '%s'", this->file->name);
        return FEENOX_ERROR;
      }
    }
  }

  return FEENOX_OK;
}