/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX-to-CalculiX input file converter
 *
 *  Copyright (C) 2025 Jeremy Theler
 *
 *  This file is part of FeenoX.
 *
 *  FeenoX is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with FeenoX.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include "feenox.h"
feenox_t feenox;
#include "pdes/mechanical/mechanical.h"

#include <stdlib.h>
#include <string.h>

extern char *ccx_names[];
extern int gmsh2ccx_types[];
extern int gmsh2ccx_hex20[];


static void usage(const char *program, FILE *stream) {
  fprintf(stream, "usage: %s [FeenoX options] [-o FILE|--output FILE] input.fee [arguments]\n"
                  "Convert a supported static mechanical FeenoX input to CalculiX.\n"
                  "Use -o - to write the CalculiX deck to standard output.\n"
                  "Options:\n"
                  "  -h, --help       show this help and exit\n"
                  "  --version        show FeenoX version and exit\n"
                  "  -o, --output     write the deck to FILE (default: standard output)\n",
          program);
}


static const char *calculix_element_type(int type_id) {
  if (feenox.pde.dim == 3 && mechanical.variant == variant_full) {
    if (type_id <= ELEMENT_TYPE_UNDEFINED || type_id >= NUMBER_ELEMENT_TYPE) {
      return NULL;
    }
    int ccx_type = gmsh2ccx_types[type_id];
    return ccx_type == 0 ? NULL : ccx_names[ccx_type];
  }

  if (feenox.pde.dim != 2) {
    return NULL;
  }

  if (mechanical.variant != variant_plane_stress &&
      mechanical.variant != variant_plane_strain &&
      mechanical.variant != variant_axisymmetric) {
    return NULL;
  }

  switch (type_id) {
    case ELEMENT_TYPE_TRIANGLE3:
      return mechanical.variant == variant_axisymmetric ? "CAX3" :
             (mechanical.variant == variant_plane_strain ? "CPE3" : "CPS3");
    case ELEMENT_TYPE_TRIANGLE6:
      return mechanical.variant == variant_axisymmetric ? "CAX6" :
             (mechanical.variant == variant_plane_strain ? "CPE6" : "CPS6");
    case ELEMENT_TYPE_QUADRANGLE4:
      return mechanical.variant == variant_axisymmetric ? "CAX4" :
             (mechanical.variant == variant_plane_strain ? "CPE4" : "CPS4");
    case ELEMENT_TYPE_QUADRANGLE8:
      return mechanical.variant == variant_axisymmetric ? "CAX8" :
             (mechanical.variant == variant_plane_strain ? "CPE8" : "CPS8");
    default:
      return NULL;
  }
}


static size_t calculix_node_index(int type_id, size_t node_index) {
  if (type_id == ELEMENT_TYPE_TETRAHEDRON10) {
    if (node_index == 8) {
      return 9;
    }
    if (node_index == 9) {
      return 8;
    }
  } else if (type_id == ELEMENT_TYPE_HEXAHEDRON20) {
    return (size_t)gmsh2ccx_hex20[node_index];
  }
  return node_index;
}


static int validate_model(mesh_t *mesh, int *used_types) {
  if (feenox.mpi_size != 1) {
    fprintf(stderr, "error: fee2ccx currently supports serial execution only\n");
    return FEENOX_ERROR;
  }
  if (mesh == NULL) {
    fprintf(stderr, "error: no mechanical mesh is available\n");
    return FEENOX_ERROR;
  }
  if (feenox.pde.dim != 2 && feenox.pde.dim != 3) {
    fprintf(stderr, "error: only 2D and 3D mechanical problems are supported\n");
    return FEENOX_ERROR;
  }
  if (feenox.pde.dim == 2 &&
      mechanical.variant != variant_plane_stress &&
      mechanical.variant != variant_plane_strain &&
      mechanical.variant != variant_axisymmetric) {
    fprintf(stderr, "error: 2D conversion requires plane_stress, plane_strain, or axisymmetric formulation\n");
    return FEENOX_ERROR;
  }
  if (feenox.pde.dim == 3 && mechanical.variant != variant_full) {
    fprintf(stderr, "error: unsupported 3D mechanical formulation\n");
    return FEENOX_ERROR;
  }
  if (feenox.pde.math_type != math_type_linear ||
      mechanical.nonlinear_geom || mechanical.nonlinear_material) {
    fprintf(stderr, "error: only small-strain linear mechanical problems can be converted\n");
    return FEENOX_ERROR;
  }
  if (mechanical.material_model != material_model_elastic_isotropic) {
    fprintf(stderr, "error: only homogeneous isotropic linear elasticity is supported\n");
    return FEENOX_ERROR;
  }
  if (mechanical.E.non_uniform || mechanical.nu.non_uniform) {
    fprintf(stderr, "error: spatially varying or multi-material E and nu are not supported\n");
    return FEENOX_ERROR;
  }
  if (((mechanical.E.property != NULL || mechanical.E.function != NULL) && !mechanical.E.constant) ||
      ((mechanical.nu.property != NULL || mechanical.nu.function != NULL) && !mechanical.nu.constant)) {
    fprintf(stderr, "error: time-dependent E or nu is not supported\n");
    return FEENOX_ERROR;
  }
  if (mechanical.thermal_expansion_model != thermal_expansion_model_none) {
    fprintf(stderr, "error: thermal expansion is not supported by fee2ccx\n");
    return FEENOX_ERROR;
  }
  if (feenox_special_var_value(end_time) != 0 ||
      feenox.pde.transient_type != transient_type_undefined ||
      feenox_special_var_value(static_steps) > 1) {
    fprintf(stderr, "error: only a single static load step is supported\n");
    return FEENOX_ERROR;
  }

  int n_bulk_elements = 0;
  for (size_t i = 0; i < mesh->n_elements; i++) {
    element_t *element = &mesh->element[i];
    if (element->type->dim != feenox.pde.dim) {
      continue;
    }
    int type_id = element->type->id;
    if (type_id <= ELEMENT_TYPE_UNDEFINED || type_id >= NUMBER_ELEMENT_TYPE) {
      fprintf(stderr, "error: invalid FeenoX element type id %d\n", type_id);
      return FEENOX_ERROR;
    }
    if (calculix_element_type(type_id) == NULL) {
      fprintf(stderr, "error: CalculiX conversion does not support element type '%s' for this formulation\n",
              element->type->name);
      return FEENOX_ERROR;
    }
    used_types[type_id] = 1;
    n_bulk_elements++;
  }
  if (n_bulk_elements == 0) {
    fprintf(stderr, "error: the mesh contains no supported bulk elements for this problem dimension\n");
    return FEENOX_ERROR;
  }
  return FEENOX_OK;
}


static int write_deck(FILE *output, mesh_t *mesh, const int *used_types) {
  fprintf(output, "*NODE\n");
  for (size_t j = 0; j < mesh->n_nodes; j++) {
    fprintf(output, "%ld, %.15g, %.15g, %.15g\n",
            mesh->node[j].tag, mesh->node[j].x[0], mesh->node[j].x[1], mesh->node[j].x[2]);
  }
  fprintf(output, "\n");

  for (int type_id = 1; type_id < NUMBER_ELEMENT_TYPE; type_id++) {
    if (!used_types[type_id]) {
      continue;
    }
    fprintf(output, "*ELEMENT, TYPE=%s, ELSET=FEE_TYPE_%d\n",
            calculix_element_type(type_id), type_id);
    for (size_t i = 0; i < mesh->n_elements; i++) {
      element_t *element = &mesh->element[i];
      if (element->type->dim != feenox.pde.dim || (int)element->type->id != type_id) {
        continue;
      }

      fprintf(output, "%ld", element->tag);
      int entry = 1;
      for (unsigned int j = 0; j < element->type->nodes; j++) {
        fprintf(output, ",");
        if (entry++ == 16) {
          fprintf(output, "\n");
          entry = 0;
        } else {
          fprintf(output, " ");
        }
        size_t node_index = calculix_node_index(type_id, j);
        fprintf(output, "%ld", element->node[node_index]->tag);
      }
      fprintf(output, "\n");
    }
    fprintf(output, "\n");
  }

  fprintf(output, "*ELSET, ELSET=bulk\n");
  int entry = 0;
  for (size_t i = 0; i < mesh->n_elements; i++) {
    element_t *element = &mesh->element[i];
    if (element->type->dim == feenox.pde.dim) {
      fprintf(output, "%s%ld", entry == 0 ? "" : ", ", element->tag);
      if (++entry == 16) {
        fprintf(output, "\n");
        entry = 0;
      }
    }
  }
  if (entry != 0) {
    fprintf(output, "\n");
  }
  fprintf(output, "\n");

  double x[] = {0, 0, 0};
  double E = mechanical.E.eval(&mechanical.E, x, feenox.mesh.materials);
  double nu = mechanical.nu.eval(&mechanical.nu, x, feenox.mesh.materials);
  fprintf(output, "*MATERIAL, NAME=bulk\n");
  fprintf(output, "*ELASTIC\n");
  fprintf(output, "%.15g, %.15g\n\n", E, nu);
  fprintf(output, "*SOLID SECTION, ELSET=bulk, MATERIAL=bulk\n\n");

  if (feenox.pde.dirichlet_rows > 0) {
    fprintf(output, "*BOUNDARY\n");
    for (size_t k = 0; k < feenox.pde.dirichlet_rows; k++) {
      fprintf(output, "%ld, %d, %d, %.15g\n",
              feenox.pde.dirichlet_nodes[k],
              feenox.pde.dirichlet_dofs[k] + 1,
              feenox.pde.dirichlet_dofs[k] + 1,
              feenox.pde.dirichlet_values[k]);
    }
    fprintf(output, "\n");
  }

  fprintf(output, "*STEP\n");
  fprintf(output, "*EL FILE\nE, S\n");
  fprintf(output, "*NODE FILE\nU\n\n");
  fprintf(output, "*STATIC\n\n");

  if (feenox.pde.b != NULL) {
    PetscScalar *b = NULL;
    petsc_call(VecGetArray(feenox.pde.b, &b));
    int has_loads = 0;
    for (size_t j = 0; j < mesh->n_nodes && !has_loads; j++) {
      for (unsigned int g = 0; g < feenox.pde.dofs; g++) {
        if (b[mesh->node[j].index_dof[g]] != 0) {
          has_loads = 1;
          break;
        }
      }
    }
    if (has_loads) {
      fprintf(output, "*CLOAD\n");
      for (size_t j = 0; j < mesh->n_nodes; j++) {
        for (unsigned int g = 0; g < feenox.pde.dofs; g++) {
          PetscScalar value = b[mesh->node[j].index_dof[g]];
          if (value != 0) {
            fprintf(output, "%ld, %d, %.15g\n", mesh->node[j].tag, g + 1, (double)value);
          }
        }
      }
      fprintf(output, "\n");
    }
    petsc_call(VecRestoreArray(feenox.pde.b, &b));
  }

  fprintf(output, "*END STEP\n");
  return fflush(output) == EOF || ferror(output) ? FEENOX_ERROR : FEENOX_OK;
}


int main(int argc, char **argv) {
  const char *output_path = NULL;
  char **feenox_argv = calloc((size_t)argc + 1, sizeof(char *));
  if (feenox_argv == NULL) {
    fprintf(stderr, "error: out of memory\n");
    return EXIT_FAILURE;
  }
  int feenox_argc = 0;
  feenox_argv[feenox_argc++] = argv[0];

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
      usage(argv[0], stdout);
      free(feenox_argv);
      return EXIT_SUCCESS;
    }
    if (strcmp(argv[i], "--version") == 0) {
      feenox_show_version(version_compact);
      free(feenox_argv);
      return EXIT_SUCCESS;
    }
    if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
      if (i + 1 >= argc || argv[i + 1][0] == '\0') {
        fprintf(stderr, "error: %s requires a file name\n", argv[i]);
        free(feenox_argv);
        return EXIT_FAILURE;
      }
      output_path = argv[++i];
      continue;
    }
    if (strncmp(argv[i], "--output=", 9) == 0) {
      if (argv[i][9] == '\0') {
        fprintf(stderr, "error: --output requires a file name\n");
        free(feenox_argv);
        return EXIT_FAILURE;
      }
      output_path = argv[i] + 9;
      continue;
    }
    feenox_argv[feenox_argc++] = argv[i];
  }

  int have_input = 0;
  for (int i = 1; i < feenox_argc; i++) {
    if (feenox_argv[i][0] != '-' || strcmp(feenox_argv[i], "-") == 0) {
      have_input = 1;
      break;
    }
  }
  if (!have_input) {
    fprintf(stderr, "error: missing FeenoX input file\n");
    usage(argv[0], stderr);
    free(feenox_argv);
    return EXIT_FAILURE;
  }

  if (feenox_initialize(feenox_argc, feenox_argv) != FEENOX_OK) {
    free(feenox_argv);
    feenox_pop_errors();
    return EXIT_FAILURE;
  }
  free(feenox_argv);

  if (feenox.mpi_size > 1) {
    fprintf(stderr, "error: fee2ccx currently supports serial execution only\n");
    feenox_finalize();
    return EXIT_FAILURE;
  }

  feenox.pde.do_not_solve = 1;
  if (feenox_step(feenox.instructions, NULL) != FEENOX_OK) {
    feenox_pop_errors();
    return EXIT_FAILURE;
  }

  mesh_t *mesh = feenox.pde.mesh != NULL ? feenox.pde.mesh : feenox.mesh.mesh_main;
  int used_types[NUMBER_ELEMENT_TYPE] = {0};
  if (validate_model(mesh, used_types) != FEENOX_OK) {
    feenox_finalize();
    return EXIT_FAILURE;
  }

  FILE *output = stdout;
  if (output_path != NULL && strcmp(output_path, "-") != 0) {
    output = fopen(output_path, "w");
    if (output == NULL) {
      fprintf(stderr, "error: cannot open output file '%s'\n", output_path);
      feenox_finalize();
      return EXIT_FAILURE;
    }
  }

  int status = write_deck(output, mesh, used_types);
  if (output != stdout && fclose(output) != 0) {
    fprintf(stderr, "error: failed to close output file '%s'\n", output_path);
    status = FEENOX_ERROR;
  }
  if (status != FEENOX_OK) {
    fprintf(stderr, "error: failed while writing CalculiX input\n");
  }

  feenox_finalize();
  return status == FEENOX_OK ? EXIT_SUCCESS : EXIT_FAILURE;
}
