/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines to perform array or hash-based lookups
 *
 *  Copyright (C) 2025 Jeremy Theler
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
 * 
 * 
 * Portions of this file were developed with the assistance of GitHub Copilot.
 *------------------- ------------  ----    --------  --     -       -         -
 */

#include "feenox.h"

int tag_index_map_init(tag_index_map_t *map, size_t min_id, size_t max_id, size_t n_nodes, double threshold) {
  printf("mamoncho\n");
  map->min_id = min_id;
  map->max_id = max_id;
  size_t range = max_id - min_id + 1;
  if (range <= threshold * n_nodes) {
    map->use_array = 1;
    feenox_check_alloc(map->array = (size_t*)malloc(range * sizeof(size_t)));
    for (size_t i = 0; i < range; ++i) {
      map->array[i] = SIZE_MAX;
    }
    map->hash = NULL;
  } else {
    map->use_array = 0;
    map->array = NULL;
    map->hash = NULL;
  }
  return FEENOX_OK;
}

int tag_index_map_insert(tag_index_map_t *map, size_t gmsh_tag, size_t vtk_idx) {
  if (map->use_array) {
    map->array[gmsh_tag - map->min_id] = vtk_idx;
  } else {
    tag_index_hash_t *entry = NULL;
    HASH_FIND(hh, map->hash, &gmsh_tag, sizeof(size_t), entry);
    if (!entry) {
      entry = (tag_index_hash_t*)malloc(sizeof(tag_index_hash_t));
      entry->gmsh_tag = gmsh_tag;
      entry->vtk_idx = vtk_idx;
      HASH_ADD(hh, map->hash, gmsh_tag, sizeof(size_t), entry);
    } else {
      entry->vtk_idx = vtk_idx; // Overwrite if needed
    }
  }
  return FEENOX_OK;
}

size_t tag_index_map_lookup(tag_index_map_t *map, size_t gmsh_tag) {
  if (map) {
    if (map->use_array) {
      return map->array[gmsh_tag - map->min_id];
    } else {
      tag_index_hash_t *entry = NULL;
      HASH_FIND_INT(map->hash, &gmsh_tag, entry);
      if (entry) {
        return entry->vtk_idx;
      } else {
        return SIZE_MAX; // Not found
      }
    }
  } else {
    return gmsh_tag-1;
  }
}

int tag_index_map_free(tag_index_map_t *map) {
  if (map->use_array) {
    feenox_free(map->array);
  } else {
    tag_index_hash_t *cur, *tmp;
    HASH_ITER(hh, map->hash, cur, tmp) {
      HASH_DEL(map->hash, cur);
      feenox_free(cur);
    }
  }
  return FEENOX_OK;
}
