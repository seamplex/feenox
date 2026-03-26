/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's mesh-related gmsh routines
 *
 *  Copyright (C) 2014--2025 Jeremy Theler
 *
 *  This file is part of feenox.
 *
 *  feenox is free software: you can redistribute it and/or modify
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
 *  along with feenox.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include "../feenox.h"

#define feenox_mesh_gmsh_readnewline()  \
  if (fgets(buffer, BUFFER_LINE_SIZE-1, fp) == NULL) { \
    feenox_push_error_message("corrupted mesh '%s'", mesh->file->path); \
    return FEENOX_ERROR; \
  }

int feenox_mesh_read_gmsh(mesh_t *mesh) {

  if (mesh->file->pointer == NULL) {
    feenox_call(feenox_instruction_file_open(mesh->file));
  }
  FILE *fp = mesh->file->pointer;

  char *dummy = NULL;
  char *name = NULL;
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
        feenox_push_error_message("mesh '%s' has an incompatible version '%s', only versions 2.2, 4.0 and 4.1 are supported", mesh->file->path, buffer);
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
      feenox_call(feenox_gmsh_read_data_size_t(mesh, 1, &size_of_size_t, 0));
      
      if (size_of_size_t != sizeof(size_t)) {
        feenox_push_error_message("size of size_t in mesh '%s' is %d and in the system is %d", mesh->file->name, size_of_size_t, sizeof(size_t));
        return FEENOX_ERROR;
      }

      feenox_mesh_gmsh_readnewline();
      
      // for binary, we have to read one to check endianness
      if (binary) {
        int one = 0;
        feenox_call(feenox_gmsh_read_data_int(mesh, 1, &one, 1));
        if (one != 1) {
          feenox_push_error_message("incompatible binary endianness in mesh '%s'", mesh->file->name);
          return FEENOX_ERROR;
        }
        feenox_mesh_gmsh_readnewline();
      }
      
      // line $EndMeshFormat
      feenox_mesh_gmsh_readnewline();
      if (strncmp("$EndMeshFormat", buffer, 14) != 0) {
        feenox_push_error_message("$EndMeshFormat not found in mesh '%s'", mesh->file->path);
        return FEENOX_ERROR;
      }

    // ------------------------------------------------------      
    } else if (strncmp("$PhysicalNames", buffer, 14) == 0) {

      // if there are physical names then we define implicitly each group
      // if they already exist we check the ids

      // number of names
      if (fscanf(fp, "%d", &mesh->n_physical_names) == 0) {
        return FEENOX_ERROR;
      }
 
      for (unsigned int i = 0; i < mesh->n_physical_names; i++) {

        int dimension = 0;
        int tag = 0;
        if (fscanf(fp, "%d %d", &dimension, &tag) < 2) {
          return FEENOX_ERROR;
        }
        if (dimension < 0 || dimension > 4) {
          feenox_push_error_message("invalid dimension %d for physical group %d in mesh file '%s'", dimension, tag, mesh->file->path);
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
        
        physical_group_t *physical_group = NULL;
        if ((physical_group = feenox_get_physical_group_ptr(name, mesh)) == NULL) {
          // create new physical group
          if ((physical_group = feenox_define_physical_group_get_ptr(name, mesh, dimension, tag)) == NULL) {
            return FEENOX_ERROR;
          }
        } else {
          // there already exists a physical group created with PHYSICAL_GROUP
          // check that either it does not have a tag or the tags coincide
          if (physical_group->tag == 0) {
            physical_group->tag = tag;
          } else if (physical_group->tag != tag) {
            feenox_push_error_message("physical group '%s' has tag %d in input and %d in mesh '%s'", name, physical_group->tag, tag, mesh->file->name);
            return FEENOX_ERROR;
          }
          // same for the dimension
          if (physical_group->dimension <= 0) {
            physical_group->dimension = dimension;
          } else if (physical_group->dimension != dimension) {
            feenox_push_error_message("physical group '%s' has dimension %d in input and %d in mesh '%s'", name, physical_group->dimension, dimension, mesh->file->name);
            return FEENOX_ERROR;
          }
          
        }

        // add the group to a local hash so we can solve the pointer
        // "more or less" easy
        HASH_ADD(hh_tag[dimension], mesh->physical_groups_by_tag[dimension], tag, sizeof(int), physical_group);

        // try to automatically link materials and BCs
        // actually it should be either one (material) or the other (bc) or none
        
        // if the physical group does not already have a material, look for one with the same name
        if (physical_group->material == NULL) {
          // this returns NULL if not found, which is ok
          HASH_FIND_STR(feenox.mesh.materials, name, physical_group->material);
        }
        
        feenox_free(name);

      }
      

      // the line $EndPhysicalNames
      feenox_mesh_gmsh_readnewline();
      if (strncmp("$EndPhysicalNames", buffer, 17) != 0) {
        feenox_push_error_message("$EndPhysicalNames not found in mesh file '%s'", mesh->file->path);
        return -2;
      }
      
    // ------------------------------------------------------      
    } else if (strncmp("$Entities", buffer, 9) == 0) {
 
      // number of entities per dimension
      size_t num_entities[4] = {0, 0, 0, 0};
      feenox_call(feenox_gmsh_read_data_size_t(mesh, 4, num_entities, binary));
      mesh->points = num_entities[0];
      mesh->curves = num_entities[1];
      mesh->surfaces = num_entities[2];
      mesh->volumes = num_entities[3];
      
      for (size_t i  = 0; i < mesh->points+mesh->curves+mesh->surfaces+mesh->volumes; i++) {
        unsigned int dimension = 0;
        if (i < mesh->points) {
          dimension = 0;
        } else if (i < mesh->points+mesh->curves) {
          dimension = 1;          
        } else if (i < mesh->points+mesh->curves+mesh->surfaces) {
          dimension = 2;          
        } else {
          dimension = 3;
        }
        
        geometrical_entity_t *geometrical_entity = NULL;
        feenox_check_alloc(geometrical_entity = calloc(1, sizeof(geometrical_entity_t)));
        geometrical_entity->dim = dimension;
        feenox_call(feenox_gmsh_read_data_int(mesh, 1, &geometrical_entity->tag, binary));
        
        double bbox[6] = {0,0,0,0,0,0};
        // since version 4.1 points have only 3 numbers, not 6 for bounding box
        if (dimension == 0 && version_maj == 4 && version_min >= 1) {
          feenox_call(feenox_gmsh_read_data_double(mesh, 3, bbox, binary));
        } else {
          feenox_call(feenox_gmsh_read_data_double(mesh, 6, bbox, binary));
        }
        
        geometrical_entity->boxMinX = bbox[0];
        geometrical_entity->boxMinY = bbox[1];
        geometrical_entity->boxMinZ = bbox[2];
        geometrical_entity->boxMaxX = bbox[3];
        geometrical_entity->boxMaxY = bbox[4];
        geometrical_entity->boxMaxZ = bbox[5];
        
        feenox_call(feenox_gmsh_read_data_size_t(mesh, 1, &geometrical_entity->num_physicals, binary));
        if (geometrical_entity->num_physicals != 0) {
          feenox_check_alloc(geometrical_entity->physical = calloc(geometrical_entity->num_physicals, sizeof(int)));
          feenox_call(feenox_gmsh_read_data_int(mesh, geometrical_entity->num_physicals, geometrical_entity->physical, binary));
          // gmsh can give a negative tag for the physical entity, depending on the orientation of the geometrical entity
          // we do not care about that (should we?) so we take the absolute value
          for (int k = 0; k < geometrical_entity->num_physicals; k++) {
            geometrical_entity->physical[k] = abs(geometrical_entity->physical[k]);
          }
        }
        
        // points do not have bounding groups
        if (dimension != 0) {
          feenox_call(feenox_gmsh_read_data_size_t(mesh, 1, &geometrical_entity->num_bounding, binary));
          if (geometrical_entity->num_bounding != 0) {
            feenox_check_alloc(geometrical_entity->bounding = calloc(geometrical_entity->num_bounding, sizeof(int)));
            feenox_call(feenox_gmsh_read_data_int(mesh, geometrical_entity->num_bounding, geometrical_entity->bounding, binary));
          }
        }
        
        // add the entity to the hashed map of its dimensions
        HASH_ADD(hh[dimension], mesh->geometrical_entities[dimension], tag, sizeof(int), geometrical_entity);
        
      }

      feenox_mesh_gmsh_readnewline();
      
      // the line $EndEntities
      feenox_mesh_gmsh_readnewline();
      if (strncmp("$EndEntities", buffer, 12) != 0) {
        feenox_push_error_message("$EndEntities not found in mesh file '%s'", mesh->file->path);
        return -2;
      }
      
    // ------------------------------------------------------      
    } else if (strncmp("$PartitionedEntities", buffer, 20) == 0) {
      
      // number of partitions and ghost entities
      size_t numbers[2] = {0,0};
      feenox_call(feenox_gmsh_read_data_size_t(mesh, 2, numbers, binary));
      mesh->num_partitions = numbers[0];
      mesh->num_ghost_entitites = numbers[1];
      
      if (mesh->num_ghost_entitites != 0) {
        feenox_push_error_message("ghost entities not yet handled");
        return FEENOX_ERROR;
      }
      
      // number of entities per dimension
      size_t num_entities[4] = {0, 0, 0, 0};
      feenox_call(feenox_gmsh_read_data_size_t(mesh, 4, num_entities, binary));
      mesh->partitioned_points = num_entities[0];
      mesh->partitioned_curves = num_entities[1];
      mesh->partitioned_surfaces = num_entities[2];
      mesh->partitioned_volumes = num_entities[3];
      
      for (size_t i = 0; i < mesh->partitioned_points+mesh->partitioned_curves+mesh->partitioned_surfaces+mesh->partitioned_volumes; i++) {
        unsigned int dimension = 0;
        if (i < mesh->partitioned_points) {
          dimension = 0;
        } else if (i < mesh->partitioned_points+mesh->partitioned_curves) {
          dimension = 1;          
        } else if (i < mesh->partitioned_points+mesh->partitioned_curves+mesh->partitioned_surfaces) {
          dimension = 2;          
        } else {
          dimension = 3;
        }
        
        geometrical_entity_t *geometrical_entity = NULL;
        feenox_check_alloc(geometrical_entity = calloc(1, sizeof(geometrical_entity_t)));
        
        int tag_dim_tag[3] = {0,0,0};
        feenox_call(feenox_gmsh_read_data_int(mesh, 3, tag_dim_tag, binary));
        geometrical_entity->dim = dimension;
        geometrical_entity->tag = tag_dim_tag[0];
        geometrical_entity->parent_dim = tag_dim_tag[1];
        geometrical_entity->parent_tag = tag_dim_tag[2];
        
        feenox_call(feenox_gmsh_read_data_size_t(mesh, 1, &geometrical_entity->num_partitions, binary));
        if (geometrical_entity->num_partitions != 0) {
          feenox_check_alloc(geometrical_entity->partition = calloc(geometrical_entity->num_partitions, sizeof(int)));
          feenox_call(feenox_gmsh_read_data_int(mesh, geometrical_entity->num_partitions, geometrical_entity->partition, binary));
          // loop over partitions and get the largest number
          for (unsigned int p = 0; p < geometrical_entity->num_partitions; p++) {
            if (geometrical_entity->partition[p] > mesh->n_partitions) {
              mesh->n_partitions = geometrical_entity->partition[p];
            }
          }
        }

        double bbox[6] = {0,0,0,0,0,0};
        // since version 4.1 points have only 3 numbers, not 6 for bounding box
        if (dimension == 0 && version_maj == 4 && version_min >= 1) {
          feenox_call(feenox_gmsh_read_data_double(mesh, 3, bbox, binary));
        } else {
          feenox_call(feenox_gmsh_read_data_double(mesh, 6, bbox, binary));
        }
        
        geometrical_entity->boxMinX = bbox[0];
        geometrical_entity->boxMinY = bbox[1];
        geometrical_entity->boxMinZ = bbox[2];
        geometrical_entity->boxMaxX = bbox[3];
        geometrical_entity->boxMaxY = bbox[4];
        geometrical_entity->boxMaxZ = bbox[5];
        
        feenox_call(feenox_gmsh_read_data_size_t(mesh, 1, &geometrical_entity->num_physicals, binary));
        if (geometrical_entity->num_physicals != 0) {
          feenox_check_alloc(geometrical_entity->physical = calloc(geometrical_entity->num_physicals, sizeof(int)));
          feenox_call(feenox_gmsh_read_data_int(mesh, geometrical_entity->num_physicals, geometrical_entity->physical, binary));
        }
        
        // points do not have bounding groups
        if (dimension != 0) {
          feenox_call(feenox_gmsh_read_data_size_t(mesh, 1, &geometrical_entity->num_bounding, binary));
          if (geometrical_entity->num_bounding != 0) {
            feenox_check_alloc(geometrical_entity->bounding = calloc(geometrical_entity->num_bounding, sizeof(int)));
            feenox_call(feenox_gmsh_read_data_int(mesh, geometrical_entity->num_bounding, geometrical_entity->bounding, binary));
          }
        }
        
        // add the entity to the hashed map of its dimensions
        HASH_ADD(hh[dimension], mesh->geometrical_entities[dimension], tag, sizeof(int), geometrical_entity);
        
      }

      feenox_mesh_gmsh_readnewline();

      // the line $EndPartitionedEntities
      feenox_mesh_gmsh_readnewline();
      if (strncmp("$EndPartitionedEntities", buffer, 23) != 0) {
        feenox_push_error_message("$EndPartitionedEntities not found in mesh file '%s'", mesh->file->path);
        return -2;
      }
      
    // ------------------------------------------------------      
    } else if (strncmp("$Nodes", buffer, 6) == 0) {

      size_t tag_min = SIZE_MAX;
      size_t tag_max = 0;
      
      if (version_maj == 2) {
        // number of nodes
        if (fscanf(fp, "%lu", &(mesh->n_nodes)) == 0) {
          return FEENOX_ERROR;
        }
        if (mesh->n_nodes == 0) {
          feenox_push_error_message("no nodes found in mesh '%s'", mesh->file->path);
          return FEENOX_ERROR;
        }

        feenox_check_alloc(mesh->node = calloc(mesh->n_nodes, sizeof(node_t)));

        for (size_t j = 0; j < mesh->n_nodes; j++) {
          size_t tag;
          if (fscanf(fp, "%lu %lf %lf %lf",
                  &tag,
                  &mesh->node[j].x[0],
                  &mesh->node[j].x[1],
                  &mesh->node[j].x[2]) < 4) {
            return FEENOX_ERROR;
          }
          
          // en msh2 si no es sparse, los tags son indices pero si es sparse es otro cantar
          mesh->sparse |= (j+1 != tag);
          
          mesh->node[j].tag = tag;
          if (tag < tag_min) {
            tag_min = tag;
          }
          if (tag > tag_max) {
            tag_max = tag;
          }
          mesh->node[j].index_mesh = j;
          
        }
        
        // finished reading the node data, check if they are sparse
        // if they are, build tag2index
        // TODO: use the vtk hashmap
        if (mesh->sparse) {
          if (tag_max < mesh->n_nodes) {
            feenox_push_error_message("maximum node tag %d is less that number of nodes %d", tag_max, mesh->n_nodes);
          }
          feenox_call(feenox_mesh_tag2index_alloc(mesh, tag_min, tag_max));
          for (size_t j = 0; j < mesh->n_nodes; j++) {
            if (feenox_unlikely(mesh->tag2index_from_tag_min[mesh->node[j].tag] != SIZE_MAX)) {
              feenox_push_error_message("duplicate node tag %ld in mesh '%s'", mesh->node[j].tag, mesh->file->name);
              return FEENOX_ERROR;
            }
            mesh->tag2index_from_tag_min[mesh->node[j].tag] = j;
          }
        }

        
      } else if (version_maj == 4) {
        // number of blocks and nodes
        size_t num_blocks = 0;
        if (version_min == 0) {
          // in 4.0 there's no min/max, I asked for this in
          // https://onelab.info/pipermail/gmsh/2018/012432.html
          //
          // > I would need to know the biggest tag of the nodes or elements before
          // > readuing them, not just the number of non-zero elements. Otherwise I
          // > need to either double-parse the file or reallocate the array each time.
          //  
          // we could indeed add min/max vertex/element tags in the section header
          // in a future revision of the format
          //
          // see also https://gitlab.onelab.info/gmsh/gmsh/-/issues/444
          size_t data[2] = {0,0};
          feenox_call(feenox_gmsh_read_data_size_t(mesh, 2, data, binary));
          num_blocks = data[0];
          mesh->n_nodes = data[1];
        } else {
          size_t data[4] = {0,0,0,0};
          feenox_call(feenox_gmsh_read_data_size_t(mesh, 4, data, binary));
          num_blocks = data[0];
          mesh->n_nodes = data[1];
          tag_min = data[2];
          tag_max = data[3];
        }
        
        if (mesh->n_nodes == 0) {
          feenox_push_error_message("no nodes found in mesh '%s'", mesh->file->path);
          return FEENOX_ERROR;
        }

        feenox_check_alloc(mesh->node = calloc(mesh->n_nodes, sizeof(node_t)));

        if (tag_max != 0) {
          // we can do this as a one single pass because we have tag_max
          // (I asked for this in v4.1, see links above)
          if (tag_max < mesh->n_nodes) {
            feenox_push_error_message("maximum node tag %d is less that number of nodes %d", tag_max, mesh->n_nodes);
            return FEENOX_ERROR;
          }
          if (tag_min <= 0) {
            feenox_push_error_message("minimum node tag %d has to be positive", tag_min);
            return FEENOX_ERROR;
          }
          feenox_call(feenox_mesh_tag2index_alloc(mesh, tag_min, tag_max));
        }
        
        size_t node_index = 0;
        for (size_t l = 0; l < num_blocks; l++) {
          size_t num_nodes = 0;

          int data[3] = {0,0,0};
          feenox_call(feenox_gmsh_read_data_int(mesh, 3, data, binary));
          
          // v4.0 and v4.1 have geometrical and dimension switched
          // the way we read nodes, we do not need them
//          int dimension = (version_min == 0) ? data[1] : data[0];
//          int geometrical = (version_min == 0) ? data[0] : data[1];
          int parametric = data[2];
          if (parametric) {
            feenox_push_error_message("mesh '%s' contains parametric data, which is unsupported yet", mesh->file->path);
            return FEENOX_ERROR;
          }
          
          feenox_call(feenox_gmsh_read_data_size_t(mesh, 1, &num_nodes, binary));

          
          if (version_min == 0) {
            // here we have tag and coordinate in the same line
            for (size_t k = 0; k < num_nodes; k++) {
              if (fscanf(fp, "%lu %lf %lf %lf",
                         &mesh->node[node_index].tag,
                         &mesh->node[node_index].x[0],
                         &mesh->node[node_index].x[1],
                         &mesh->node[node_index].x[2]) < 4) {
                feenox_push_error_message("reading node data");
                return FEENOX_ERROR;
              }
              
              if (mesh->node[node_index].tag < tag_min) {
                tag_min = mesh->node[node_index].tag;
              }
              if (mesh->node[node_index].tag > tag_max) {
                tag_max = mesh->node[node_index].tag;
              }
              mesh->node[node_index].index_mesh = node_index;
              // we don't have tag2index yet, but we can check if it is sparse
              mesh->sparse |= (mesh->node[node_index].tag != node_index+1);
              node_index++;
            }
          } else {

            // here we have first all the tags and then all the coordinates
            size_t *node_tags = NULL;
            feenox_check_alloc(node_tags = calloc(num_nodes, sizeof(size_t)));
            feenox_call(feenox_gmsh_read_data_size_t(mesh, num_nodes, node_tags, binary));
            // node_index has the last fully-read node index
            for (size_t k = 0; k < num_nodes; k++) {
              mesh->node[node_index+k].tag = node_tags[k];
            }
            feenox_free(node_tags);
            
            double *node_coords = NULL;
            feenox_check_alloc(node_coords = calloc(3*num_nodes, sizeof(size_t)));
            feenox_call(feenox_gmsh_read_data_double(mesh, 3*num_nodes, node_coords, binary));
            for (size_t k = 0; k < num_nodes; k++) {
              mesh->node[node_index].x[0] = node_coords[3*k+0];
              mesh->node[node_index].x[1] = node_coords[3*k+1];
              mesh->node[node_index].x[2] = node_coords[3*k+2];
              mesh->node[node_index].index_mesh = node_index;

              if (feenox_unlikely(mesh->tag2index_from_tag_min[mesh->node[node_index].tag] != SIZE_MAX)) {
                feenox_push_error_message("duplicate node tag %ld in mesh '%s'", mesh->node[node_index].tag, mesh->file->name);
                return FEENOX_ERROR;
              }
              mesh->tag2index_from_tag_min[mesh->node[node_index].tag] = node_index;
              mesh->sparse |= (mesh->node[node_index].tag != node_index+1);
              node_index++;
            }
            feenox_free(node_coords);
          }
        }
        
        mesh->node_tag_min = tag_min;
        mesh->node_tag_max = tag_max;
        if (version_min == 0) {
          // for v4.0 we need an extra loop because we did not have
          // the actual tag_min/tag_max before reading
          feenox_mesh_tag2index_alloc(mesh, tag_min, tag_max);
          
          for (size_t j = 0; j < mesh->n_nodes; j++) {
            if (feenox_unlikely(mesh->tag2index_from_tag_min[mesh->node[j].tag] != SIZE_MAX)) {
              feenox_push_error_message("duplicate node tag %ld in mesh '%s'", mesh->node[j].tag, mesh->file->name);
              return FEENOX_ERROR;
            }
            mesh->tag2index_from_tag_min[mesh->node[j].tag] = j;
          }
        }
      }
           
      feenox_mesh_gmsh_readnewline();
      
      // the line $EndNodes
      feenox_mesh_gmsh_readnewline();
      if (strncmp("$EndNodes", buffer, 9) != 0) {
        feenox_push_error_message("$EndNodes not found in mesh file '%s'", mesh->file->path);
        return -2;
      }

    // ------------------------------------------------------      
    } else if (strncmp("$Elements", buffer, 9) == 0) {              

      size_t tag_min = SIZE_MAX;
      size_t tag_max = 0;
      
      if (version_maj == 2) {
        // number of elements
        if (fscanf(fp, "%lu", &(mesh->n_elements)) == 0) {
          return FEENOX_ERROR;
        }
        if (mesh->n_elements == 0) {
          feenox_push_error_message("no elements found in mesh file '%s'", mesh->file->path);
          return -2;
        }
        feenox_check_alloc(mesh->element = calloc(mesh->n_elements, sizeof(element_t)));

        size_t i = 0;
        for (i = 0; i < mesh->n_elements; i++) {

          size_t tag = 0;
          int ntags22 = 0;
          int type = 0;
          if (fscanf(fp, "%lu %d %d", &tag, &type, &ntags22) < 3) {
            return FEENOX_ERROR;
          }

          // in msh2 the tags are the indices
          if (i+1 != tag) {
            feenox_push_error_message("nodes in file '%s' are non-contiguous, which should not happen in v2.2", mesh->file->path);
            return FEENOX_ERROR;
          }

          mesh->element[i].index = i;
          mesh->element[i].tag = tag;
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
            feenox_push_error_message("elements of type '%s' are not supported in FeenoX", mesh->element[i].type->name);
            return FEENOX_ERROR;
          }
          mesh->element[i].type = &(feenox.mesh.element_types[type]);
          mesh->n_elements_per_dim[mesh->element[i].type->dim]++;

          // format v2.2
          // each element has a tag which is an array of ints
          // first one is the ide of the physical entity
          // second one is the id of the geometrical entity (interesting only because it has the partitioning info)
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
              physical_group_t *physical_group = NULL;
              unsigned int dimension = mesh->element[i].type->dim;
              HASH_FIND(hh_tag[dimension], mesh->physical_groups_by_tag[dimension], &tags22[0], sizeof(int), physical_group);
              if ((mesh->element[i].physical_group = physical_group) == NULL) {
                // if we did not find anything, create one
                snprintf(buffer, BUFFER_LINE_SIZE-1, "%s_%d_%d", mesh->file->name, dimension, tags22[0]);
                if ((mesh->element[i].physical_group = feenox_define_physical_group_get_ptr(buffer, mesh, mesh->element[i].type->dim, tags22[0])) == NULL) {
                  return FEENOX_ERROR;
                }
                HASH_ADD(hh_tag[dimension], mesh->physical_groups_by_tag[dimension], tag, sizeof(int), mesh->element[i].physical_group);
              }
              mesh->element[i].physical_group->n_elements++;
            }
            feenox_free(tags22);
          }
          
          feenox_check_alloc(mesh->element[i].node = calloc(mesh->element[i].type->nodes, sizeof(node_t *)));
          size_t j = 0;
          for (j = 0; j < mesh->element[i].type->nodes; j++) {
            if (fscanf(fp, "%lu", &node) < 1) {
              return FEENOX_ERROR;
            }
            if (mesh->sparse == 0 && (node < 1 || node > mesh->n_nodes)) {
              feenox_push_error_message("node %d in element %d does not exist", node, tag);
              return FEENOX_ERROR;
            }

            if ((node_index = (mesh->sparse==0) ? (node-1) : (mesh->tag2index[node - tag_min])) == SIZE_MAX) {
              feenox_push_error_message("node %d in element %d does not exist", node, tag);
              return FEENOX_ERROR;
            }
            mesh->element[i].node[j] = &mesh->node[node_index];
            feenox_mesh_add_element_to_list(&mesh->element[i].node[j]->element_list, &mesh->element[i]);
          }
        }
        
      } else if (version_maj == 4) {
        size_t num_blocks;
        if (version_min == 0) {
          tag_min = 0;
          tag_max = 0;
          if (fscanf(fp, "%lu %lu", &num_blocks, &mesh->n_elements) < 2) {
            feenox_push_error_message("error reading element blocks");
            return FEENOX_ERROR;
          }
        } else {
          size_t data[4] = {0,0,0,0};
          feenox_call(feenox_gmsh_read_data_size_t(mesh, 4, data, binary));
          num_blocks = data[0];
          mesh->n_elements = data[1];
          tag_min = data[2];
          tag_max = data[3];
        }
        
        if (mesh->n_elements == 0) {
          feenox_push_error_message("no elements found in mesh file '%s'", mesh->file->path);
          return FEENOX_ERROR;
        }
        feenox_check_alloc(mesh->element = calloc(mesh->n_elements, sizeof(element_t)));

        size_t index = 0;
        for (size_t l = 0; l < num_blocks; l++) {
          geometrical_entity_t *geometrical_entity = NULL;          
          int geometrical = 0;
          int dimension = 0;
          int type = 0;
          size_t num_elements = 0;
          if (version_min == 0) {
            if (fscanf(fp, "%d %d %d %lu", &geometrical, &dimension, &type, &num_elements) < 4) {
              return FEENOX_ERROR;
            }
          } else {
            int data[3] = {0,0,0};
            if (feenox_gmsh_read_data_int(mesh, 3, data, binary) != FEENOX_OK) {
              feenox_push_error_message("reading element block %ld out of %ld,", l+1, num_blocks);
              return FEENOX_ERROR;
            }
            dimension = data[0];
            geometrical = data[1];
            type = data[2];
            feenox_call(feenox_gmsh_read_data_size_t(mesh, 1, &num_elements, binary));
          }  
          
          if (type >= NUMBER_ELEMENT_TYPE) {
            feenox_push_error_message("elements of type '%d' are not supported in this version", type);
            return FEENOX_ERROR;
          }
          if (feenox.mesh.element_types[type].nodes == 0) {
            feenox_push_error_message("elements of type '%s' are not supported in this version", feenox.mesh.element_types[type].name);
            return FEENOX_ERROR;
          }

          physical_group_t *physical_group = NULL;
          if (geometrical != 0) {
            // the whole block has the same physical group, find it once and that's it
            HASH_FIND(hh[dimension], mesh->geometrical_entities[dimension], &geometrical, sizeof(int), geometrical_entity);
            if (geometrical_entity == NULL) {
              feenox_push_error_message("geometrical entity '%d' of dimension '%d' does not exist", geometrical, dimension);
              return FEENOX_ERROR;
            }
            
            if (geometrical_entity->num_physicals > 0) {
              // what to do if there is more than one? the first? the last one?
              // TODO: all of them! make a linked list
              int physical = geometrical_entity->physical[0];
              HASH_FIND(hh_tag[dimension], mesh->physical_groups_by_tag[dimension], &physical, sizeof(int), physical_group);
              if ((mesh->element[index].physical_group = physical_group) == NULL) {
                snprintf(buffer, BUFFER_LINE_SIZE-1, "%s_%d_%d", mesh->file->name, dimension, physical);
                if ((mesh->element[index].physical_group = feenox_define_physical_group_get_ptr(buffer, mesh, feenox.mesh.element_types[type].dim, physical)) == NULL) {
                  return FEENOX_ERROR;
                }
                mesh->element[index].physical_group->tag = physical;
                HASH_ADD(hh_tag[dimension], mesh->physical_groups_by_tag[dimension], tag, sizeof(int), mesh->element[index].physical_group);
              }
            }  
          }
          
          size_t *element_data = NULL;
          size_t size_block = 1+feenox.mesh.element_types[type].nodes;
          feenox_check_alloc(element_data = calloc(num_elements*size_block, sizeof(size_t)));
          feenox_call(feenox_gmsh_read_data_size_t(mesh, num_elements*size_block, element_data, binary));
          
          // index has the last fully-read element index
          for (size_t k = 0; k < num_elements; k++) {
            mesh->element[index].tag = element_data[size_block * k + 0];
            mesh->element[index].index = index;
            mesh->element[index].type = &(feenox.mesh.element_types[type]);
            mesh->n_elements_per_dim[mesh->element[index].type->dim]++;
            
            if ((mesh->element[index].physical_group = physical_group) != NULL) {
              mesh->element[index].physical_group->n_elements++;
            }
            mesh->element[index].geometrical_entity = geometrical_entity;
            
            feenox_check_alloc(mesh->element[index].node = calloc(mesh->element[index].type->nodes, sizeof(node_t *)));
            for (size_t j = 0; j < mesh->element[index].type->nodes; j++) {
              // for msh4 we need to use tag2index
              node_index = mesh->tag2index_from_tag_min[element_data[size_block * k + 1 + j]];
              if (feenox_unlikely(node_index) == SIZE_MAX) {
                feenox_push_error_message("node %d in element %d does not exist", node, mesh->element[index].tag);
                return FEENOX_ERROR;
              }
              mesh->element[index].node[j] = &mesh->node[node_index];
              feenox_mesh_add_element_to_list(&mesh->element[index].node[j]->element_list, &mesh->element[index]);
            }
            index++;
          }
          feenox_free(element_data);
        }
      }

      feenox_mesh_gmsh_readnewline();

      // the line $EndElements
      feenox_mesh_gmsh_readnewline();
      if (strncmp("$EndElements", buffer, 12) != 0) {
        feenox_push_error_message("$EndElements not found in mesh file '%s'", mesh->file->path);
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
      feenox_call(feenox_gmsh_read_data_int(mesh, 1, &n_string_tags, 0));
      if (n_string_tags > 2) {
        feenox_push_error_message("number of string tags cannot be larger than two");
        return FEENOX_ERROR;
      }
      
      feenox_mesh_gmsh_readnewline();
      
      // the first actual string tag (string) is the post-processing view name
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
      LL_FOREACH(mesh->node_datas, node_data) {
        if (strcmp(string_tag, node_data->name_in_mesh) == 0) {
          function = node_data->function;
          node_data->found = 1;
          feenox_check_alloc(function->name_in_mesh = strdup(node_data->name_in_mesh));
        }
      }
      
      // if this function was not requested then we can ignore the block
      if (function == NULL) {
        continue;
      }
      
      // the second (optional) string tag is the interpolation scheme which we ignore for now
      if (n_string_tags > 1) {
        feenox_mesh_gmsh_readnewline();
      }
      
      // real-tags (all ASCII stuff)
      int n_real_tags = 0;
      feenox_call(feenox_gmsh_read_data_int(mesh, 1, &n_real_tags, 0));
      if (n_real_tags != 1) {
        feenox_push_error_message("expected only one real tag for '%s'", string_tag);
        return FEENOX_ERROR;
      }
      
      double time = 0;
      feenox_call(feenox_gmsh_read_data_double(mesh, 1, &time, 0));
      
      // TODO: what to do here?
      // we read only data for t = 0
      if (time != 0) {
        continue;
      }
      
      // integer-tags
      int n_integer_tags = 0;
      feenox_call(feenox_gmsh_read_data_int(mesh, 1, &n_integer_tags, 0));
      if (n_integer_tags > 4) {
        feenox_push_error_message("number of integer tags cannot be larger than four for '%s'", string_tag);
        return FEENOX_ERROR;
      }
      
      int data[4] = {0,0,0,0};
      feenox_call(feenox_gmsh_read_data_int(mesh, n_integer_tags, data, 0));
//      int timestep = data[0];
      int dofs = data[1];
      int num_nodes = data[2];
//      int partition_index = data[3];

      if (num_nodes != mesh->n_nodes) {
        feenox_push_error_message("field '%s' has %d nodes and the mesh has %ld nodes", string_tag, num_nodes, mesh->n_nodes);
        return FEENOX_ERROR;
      }

      // to handle vector/tensor functions we need more
      function_t **functions = NULL;
      feenox_check_alloc(functions = calloc(dofs, sizeof(function_t *)));
      
      unsigned int g = 0;
      if (dofs == 1) {
        functions[0] = function;
      } else if (dofs == 3 || dofs == 6) {
        
        char *components[6] = {"x", "y", "z", "xy", "yz", "zx"};
        for (g = 0; g < dofs; g++) {
          char *function_name = NULL;
          feenox_check_minusone(asprintf(&function_name, "%s_%s", function->name, components[g]));
          // these functions have already been defined in the parser
          if ((functions[g] = feenox_get_function_ptr(function_name)) == NULL) {
            feenox_push_error_message("internal mismatch, cannot find function '%s'", function_name);
            return FEENOX_ERROR;
          }
          feenox_free(function_name);
        }
        
      } else {
        feenox_push_error_message("cannot handle field '%s' with %d components", string_tag, dofs);
        return FEENOX_ERROR;
      }
      
      
      if (mesh->nodes_argument == NULL) {
        feenox_call(feenox_mesh_create_nodes_argument(mesh));
      }
      for (unsigned int g = 0; g < dofs; g++) {
        functions[g]->type = function_type_pointwise_mesh_node;
        functions[g]->data_size = num_nodes;
        functions[g]->mesh = mesh;
        functions[g]->vector_value->size = num_nodes;
        feenox_call(feenox_vector_init(functions[g]->vector_value, 1));
      }  

      feenox_mesh_gmsh_readnewline();
      
      int node = 0;
      double value = 0;
      for (size_t j = 0; j < num_nodes; j++) {
        feenox_call(feenox_gmsh_read_data_int(mesh, 1, &node, binary));
        for (unsigned int g = 0; g < dofs; g++) {
          feenox_call(feenox_gmsh_read_data_double(mesh, 1, &value, binary));
          if ((node_index = (mesh->sparse==0) ? node-1 : mesh->tag2index_from_tag_min[node]) == SIZE_MAX) {
            feenox_push_error_message("node %d does not exist", node);
            return FEENOX_ERROR;
          }
          feenox_vector_set(functions[g]->vector_value, node_index, value);
        }  
      }

      // the newline
      feenox_mesh_gmsh_readnewline();

      // the line $ElementNodeData
      feenox_mesh_gmsh_readnewline();
      if (strncmp("$EndNodeData", buffer, 12) != 0 && strncmp("$EndElementData", buffer, 15) != 0) {
        feenox_push_error_message("$EndNodeData not found in mesh file '%s'", mesh->file->path);
        return -2;
      }
      
    // ------------------------------------------------------      
    } else {
        
      do {
        if (fgets(buffer, BUFFER_LINE_SIZE-1, fp) == NULL) {
          feenox_push_error_message("corrupted mesh file '%s'", mesh->file->path);
          return -3;
        }
      } while(strncmp("$End", buffer, 4) != 0);
        
    }
  }

  // close the mesh file
  fclose(fp);
  mesh->file->pointer = NULL;
  
  return FEENOX_OK;
}

int feenox_mesh_tag2index_alloc(mesh_t *mesh, size_t tag_min, size_t tag_max) {
  size_t tag2index_size = tag_max - tag_min + 1;
  feenox_check_alloc(mesh->tag2index = malloc(tag2index_size * sizeof(size_t)));
  for (size_t k = 0; k < tag2index_size; k++) {
    mesh->tag2index[k] = SIZE_MAX;
  }
  mesh->tag2index_from_tag_min = mesh->tag2index - tag_min;

  return FEENOX_OK;
}


int feenox_mesh_write_header_gmsh(mesh_t *mesh, FILE *file) {
  fprintf(file, "$MeshFormat\n");
  fprintf(file, "2.2 0 8\n");
  fprintf(file, "$EndMeshFormat\n");

  return FEENOX_OK;
}

int feenox_mesh_write_mesh_gmsh(mesh_t *mesh, FILE *file, int no_physical_names) {
  
  physical_group_t *physical_group;

  if (no_physical_names == 0) {
    unsigned int n = HASH_COUNT(mesh->physical_groups);
    if (n != 0) {
      fprintf(file, "$PhysicalNames\n");
      fprintf(file, "%d\n", n);
  
      for (physical_group = mesh->physical_groups; physical_group != NULL; physical_group = physical_group->hh.next) {
        fprintf(file, "%d %d \"%s\"\n", physical_group->dimension, physical_group->tag, physical_group->name);
      }
      fprintf(file, "$EndPhysicalNames\n");
    }
  }
  
  
  fprintf(file, "$Nodes\n");
  fprintf(file, "%ld\n", mesh->n_nodes);
  for (size_t j = 0; j < mesh->n_nodes; j++) {
    fprintf(file, "%ld %g %g %g\n", mesh->node[j].tag, mesh->node[j].x[0], mesh->node[j].x[1], mesh->node[j].x[2]);
  }
  fprintf(file, "$EndNodes\n");

  fprintf(file, "$Elements\n");
  fprintf(file, "%ld\n", mesh->n_elements);
  for (size_t i = 0; i < mesh->n_elements; i++) {
    fprintf(file, "%ld ", mesh->element[i].tag);
    fprintf(file, "%d ", mesh->element[i].type->id);

    // in principle we should write the detailed information about groups and partitions
//    fprintf(file, "%d ", this->element[i].ntags);
    
    // but for now only two tags are enough:
    // the first one is the physical group and the second one ought to be the geometrical group
    // if there is no such information, then we just duplicate the physical group tag
    if (mesh->element[i].physical_group != NULL) {
      fprintf(file, "2 %d %d", mesh->element[i].physical_group->tag, mesh->element[i].physical_group->tag);
    } else {
      fprintf(file, "2 0 0");
    }
    // the nodes
    for (unsigned int j = 0; j < mesh->element[i].type->nodes; j++) {
      fprintf(file, " %ld", mesh->element[i].node[j]->tag);
    }
    fprintf(file, "\n");
  }
  fprintf(file, "$EndElements\n");
  
  return FEENOX_OK;
  
}


int feenox_mesh_write_data_gmsh(mesh_write_t *mesh, mesh_write_dist_t *dist) {

  // TODO: split as virtual methods
  if (dist->field_location == field_location_cells) {
    fprintf(mesh->file->pointer, "$ElementData\n");
    if (mesh->mesh->n_cells == 0) {
      feenox_call(feenox_mesh_element2cell(mesh->mesh));
    }
  } else {
    fprintf(mesh->file->pointer, "$NodeData\n");
  }

  // one string tag
  fprintf(mesh->file->pointer, "1\n");
  // the name of the view
  fprintf(mesh->file->pointer, "\"%s\"\n", dist->name);
  // the other one (optional) is the interpolation scheme
  
  // one real tag (only one)
  fprintf(mesh->file->pointer, "1\n");                          
  // time
  fprintf(mesh->file->pointer, "%g\n", feenox_special_var_value(t));

  // three integer tags
  fprintf(mesh->file->pointer, "3\n");
  // timestep
  fprintf(mesh->file->pointer, "%d\n", (feenox_special_var_value(end_time) > 0) ? (unsigned int)(feenox_special_var_value(step_transient)) : (unsigned int)(feenox_special_var_value(step_static)));
  // number of data per node
  fprintf(mesh->file->pointer, "%d\n", dist->size);

  // custom printf format
  char *format = NULL;
  if (dist->printf_format == NULL) {
    feenox_check_alloc(format = strdup(" %g"));
  } else {
    if (dist->printf_format[0] == '%') {
      feenox_check_minusone(asprintf(&format, " %s", dist->printf_format));
    } else {
      feenox_check_minusone(asprintf(&format, " %%%s", dist->printf_format));
    }  
  }
  
  if (dist->field_location == field_location_cells) {

    // number of point data
    fprintf(mesh->file->pointer, "%ld\n", mesh->mesh->n_cells);              

    // cell tag - data
    size_t i = 0;
    unsigned int g = 0;
    double value = 0;
    for (i = 0; i < mesh->mesh->n_cells; i++) {
      fprintf(mesh->file->pointer, "%ld", mesh->mesh->cell[i].element->tag);
      for (g = 0; g < dist->size; g++) {
        value = (dist->field[g]->type == function_type_pointwise_mesh_cell && dist->field[g]->mesh == mesh->mesh) ?
                  feenox_vector_get(dist->field[g]->vector_value, i) :
                  feenox_function_eval(dist->field[g], mesh->mesh->cell[i].x);
        fprintf(mesh->file->pointer, format, value);
      }
      fprintf(mesh->file->pointer, "\n");
    }
    fprintf(mesh->file->pointer, "$EndElementData\n");
    
  } else {
    
    // number of point data
    fprintf(mesh->file->pointer, "%ld\n", mesh->mesh->n_nodes);              

    // point tag - data
    double value = 0;
    for (size_t j = 0; j < mesh->mesh->n_nodes; j++) {
      fprintf(mesh->file->pointer, "%ld", mesh->mesh->node[j].tag);
      for (unsigned int g = 0; g < dist->size; g++) {
        value = (dist->field[g]->type == function_type_pointwise_mesh_node && dist->field[g]->mesh == mesh->mesh) ?
                  feenox_vector_get(dist->field[g]->vector_value, j) :
                  feenox_function_eval(dist->field[g], mesh->mesh->node[j].x);
        fprintf(mesh->file->pointer, format, value);
      }
      fprintf(mesh->file->pointer, "\n");
    }
    fprintf(mesh->file->pointer, "$EndNodeData\n");
  }  
 
  fflush(mesh->file->pointer);
  feenox_free(format);
  

  return FEENOX_OK;

}


// read the next available time step and interpolate
int feenox_mesh_update_function_gmsh(function_t *function, double t, double dt) {
 
  mesh_t *mesh = function->mesh;
  double *new_data = NULL; 
  int done = 0;

  if (mesh->file->pointer == NULL) {
    feenox_call(feenox_instruction_file_open(mesh->file));
  }

  double time = 0;
  
  char buffer[BUFFER_LINE_SIZE];
  while (done == 0 && fgets(buffer, BUFFER_LINE_SIZE-1, mesh->file->pointer) != NULL) {

    if (strncmp("\n", buffer, 1) == 0) {
      ;
    
    // ------------------------------------------------------  
    } else if (strncmp("$NodeData", buffer, 9) == 0) {
      
      // string-tags
      int n_string_tags = 0;
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
      char *string_tag = strtok(buffer, "\"");
      
      if (strcmp(string_tag, function->name_in_mesh) != 0) {
        continue;
      }
      
      // real-tags
      int n_real_tags = 0;
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
      int n_integer_tags = 0;
      if (fscanf(mesh->file->pointer, "%d", &n_integer_tags) == 0) {
        feenox_push_error_message("error reading file");
        return FEENOX_ERROR;
      }
      if (n_integer_tags != 3) {
        continue;
      }
      
      int timestep = 0;
      if (fscanf(mesh->file->pointer, "%d", &timestep) == 0) {
        feenox_push_error_message("error reading file");
        return FEENOX_ERROR;
      }
      
      int dofs = 0;
      if (fscanf(mesh->file->pointer, "%d", &dofs) == 0) {
        feenox_push_error_message("error reading file");
        return FEENOX_ERROR;
      }
      
      int nodes = 0;
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
      
      feenox_check_alloc(new_data = calloc(nodes, sizeof(double)));
      
      // the format says the node tag is now an int
      double value = 0;
      int node = 0;
      int node_index = 0;
      for (int j = 0; j < nodes; j++) {
        if (fscanf(mesh->file->pointer, "%d %lf", &node, &value) == 0) {
          feenox_push_error_message("error reading file");
          return FEENOX_ERROR;
        }
        if ((node_index = (mesh->sparse==0) ? node-1 : mesh->tag2index_from_tag_min[node]) == SIZE_MAX) {
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
    double alpha = (t-function->mesh_time)/(time-function->mesh_time);
    for (size_t j = 0; j < function->data_size; j++) {
      feenox_vector_add(function->vector_value, j, alpha * (new_data[j] - feenox_vector_get(function->vector_value, j)));
    }
    feenox_free(new_data);
  }
 
  return FEENOX_OK;
  
}


int feenox_gmsh_read_data_int(mesh_t *mesh, size_t n, int *data, int binary) {

  FILE *fp = mesh->file->pointer;

  if (binary) {
    if (fread(data, sizeof(int), n, fp) != n) {
      feenox_push_error_message("not enough data in mesh '%s'", mesh->file->name);
      return FEENOX_ERROR;
    }
  } else {  
    size_t i = 0;
    for (i = 0; i < n; i++) {
      if (fscanf(fp, "%d", &data[i]) != 1) {
        feenox_push_error_message("not enough data in mesh '%s'", mesh->file->name);
        return FEENOX_ERROR;
      }
    }
  }

  return FEENOX_OK;
}


int feenox_gmsh_read_data_size_t(mesh_t *mesh, size_t n, size_t *data, int binary) {

  FILE *fp = mesh->file->pointer;

  if (binary) {
    if (fread(data, sizeof(size_t), n, fp) != n) {
      feenox_push_error_message("not enough data in mesh '%s'", mesh->file->name);
      return FEENOX_ERROR;
    }
  } else {  
    size_t i = 0;
    for (i = 0; i < n; i++) {
      if (fscanf(fp, "%lu", &data[i]) != 1) {
        feenox_push_error_message("not enough data in mesh '%s'", mesh->file->name);
        return FEENOX_ERROR;
      }
    }
  }

  return FEENOX_OK;
}

int feenox_gmsh_read_data_double(mesh_t *mesh, size_t n, double *data, int binary) {

  FILE *fp = mesh->file->pointer;

  if (binary) {
    if (fread(data, sizeof(double), n, fp) != n) {
      feenox_push_error_message("not enough data in mesh '%s'", mesh->file->name);
      return FEENOX_ERROR;
    }
  } else {  
    size_t i = 0;
    for (i = 0; i < n; i++) {
      if (fscanf(fp, "%lf", &data[i]) != 1) {
        feenox_push_error_message("not enough data in mesh '%s'", mesh->file->name);
        return FEENOX_ERROR;
      }
    }
  }

  return FEENOX_OK;
}
