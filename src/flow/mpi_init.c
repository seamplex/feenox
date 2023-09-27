/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox abort routine
 *
 *  Copyright (C) 2009--2023 jeremy theler
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
 *  along with FeenoX.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include "feenox.h"
#include <unistd.h>

int feenox_instruction_mpi_init(void *arg) {
#ifdef HAVE_PETSC
  mpi_init_t *mpi_init = (mpi_init_t *)arg;  
  
  if (feenox.pde.petscinit_called == PETSC_FALSE) {
    MPI_Init(&feenox.argc, &feenox.argv);
    petsc_call(MPI_Comm_size(MPI_COMM_WORLD, &feenox.mpi_size));
    feenox_special_var_value(mpi_size) = (double)feenox.mpi_size;
  
    petsc_call(MPI_Comm_rank(MPI_COMM_WORLD, &feenox.mpi_rank));
    feenox_special_var_value(mpi_rank) = (double)feenox.mpi_rank;    
  }
  
  if (mpi_init->hello) {
    char host[256];
    gethostname(host, 255);
    
    if (feenox.pde.petscinit_called == PETSC_TRUE) {
      petsc_call(PetscSynchronizedPrintf(PETSC_COMM_WORLD, "[%d/%d] Hello PETSc World! I am rank %d of %d in host %s\n", feenox.mpi_rank, feenox.mpi_size, feenox.mpi_rank, feenox.mpi_size, host));
      petsc_call(PetscSynchronizedFlush(PETSC_COMM_WORLD, PETSC_STDOUT));    
    } else {
      printf("Hello MPI World! I am rank %d of %d in host %s\n", feenox.mpi_rank, feenox.mpi_size, host);
    }
  }

  return FEENOX_OK;  
  
#else
  feenox_push_error_message("FeenoX is not compiled against PETSc so MPI_INIT cannot be executed.\n");
  return FEENOX_ERROR;
#endif
  
}
