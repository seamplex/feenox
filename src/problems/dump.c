/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox function for dumping PETSc objects
 *
 *  Copyright (C) 2009--2015,2021 jeremy theler
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

int feenox_instruction_dump(void *arg) {
  dump_t *dump = (dump_t *)arg;
  
  PetscViewer viewer = NULL;

  if (dump->K) {
    feenox_call(feenox_dump_open_viewer(dump, "K", &viewer));
    petsc_call(MatView(feenox.pde.K, viewer));
    petsc_call(PetscViewerDestroy(&viewer));
  }
  if (dump->K_bc) {
    feenox_call(feenox_dump_open_viewer(dump, "K_bc", &viewer));
    petsc_call(MatView(feenox.pde.K_bc, viewer));
    petsc_call(PetscViewerDestroy(&viewer));
  }
  if (dump->b) {
    feenox_call(feenox_dump_open_viewer(dump, "b", &viewer));
    petsc_call(VecView(feenox.pde.b, viewer));
    petsc_call(PetscViewerDestroy(&viewer));
  }
  if (dump->b_bc) {
    feenox_call(feenox_dump_open_viewer(dump, "b_bc", &viewer));
    petsc_call(VecView(feenox.pde.b_bc, viewer));
    petsc_call(PetscViewerDestroy(&viewer));
  }
  if (dump->M) {
    feenox_call(feenox_dump_open_viewer(dump, "M", &viewer));
    petsc_call(MatView(feenox.pde.M, viewer));
    petsc_call(PetscViewerDestroy(&viewer));
  }
  if (dump->M_bc) {
    feenox_call(feenox_dump_open_viewer(dump, "M_bc", &viewer));
    petsc_call(MatView(feenox.pde.M_bc, viewer));
    petsc_call(PetscViewerDestroy(&viewer));
  }
  
  
  return FEENOX_OK;
}

int feenox_dump_open_viewer(dump_t *this, const char *name, PetscViewer *viewer) {

  char *path = NULL;
  
  switch (this->format) {
    case dump_format_default:
    case dump_format_binary:
      feenox_check_minusone(asprintf(&path, "%s.bin", name));
      petsc_call(PetscViewerBinaryOpen(MPI_COMM_WORLD, path, FILE_MODE_WRITE, viewer));
      feenox_free(path);
    break;
    
    case dump_format_ascii:
      feenox_check_minusone(asprintf(&path, "%s.txt", name));
      petsc_call(PetscViewerASCIIOpen(MPI_COMM_WORLD, path, viewer));
      petsc_call(PetscViewerPushFormat(*viewer, PETSC_VIEWER_DEFAULT));
      feenox_free(path);
    break;
    
    case dump_format_octave:
      feenox_check_minusone(asprintf(&path, "%s.m", name));
      petsc_call(PetscViewerASCIIOpen(MPI_COMM_WORLD, path, viewer));
      petsc_call(PetscViewerPushFormat(*viewer, PETSC_VIEWER_ASCII_MATLAB ));
      feenox_free(path);
    break;
  }
  
  return FEENOX_OK;
}