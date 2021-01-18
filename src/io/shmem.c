/*------------ -------------- -------- --- ----- ---   --       -            -
 *  wasora shared memory routines
 *
 *  Copyright (C) 2009--2014 jeremy theler
 *
 *  This file is part of wasora.
 *
 *  wasora is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  wasora is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with wasora.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <sys/mman.h>
#include <fcntl.h>

#ifndef _WASORA_H_
#include "wasora.h"
#endif

void *wasora_get_shared_pointer(char *name, size_t size) {
  void *pointer = NULL;
  int dangling_pid;
  int fd;

#if !defined(LD_STATIC)
  if ((dangling_pid = wasora_create_lock(name, 0)) != 0) {
    wasora_push_error_message("shared memory segment '%s' is being used by process %d", name, dangling_pid);
    wasora_runtime_error();
  }
  
  if (size == 0) {
    wasora_push_error_message("shared memory object '%s' has zero size", name);
    wasora_runtime_error();
  }

  umask(0);
  if ((fd = shm_open(name, O_RDWR | O_CREAT, 0666)) == -1) {
    wasora_push_error_message("'%s' opening shared memory object '%s'", strerror(errno), name);
    wasora_runtime_error();
  }

  if (ftruncate(fd, size) != 0) {
    wasora_push_error_message("'%s' truncating shared memory object '%s'", strerror(errno), name);
    wasora_runtime_error();
  }
  if ((pointer = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) {
    wasora_push_error_message("'%s' maping shared memory object '%s'", strerror(errno), name);
    wasora_runtime_error();
  }

  close(fd);
#endif
  return pointer;
}



void wasora_free_shared_pointer(void *pointer, char *name, size_t size) {
  wasora_remove_lock(name, 0);
  munmap(pointer, size);
#if !defined(LD_STATIC)
  shm_unlink(name);
#endif
}


sem_t *wasora_get_semaphore(char *name) {
  sem_t *semaphore = NULL;
  int dangling_pid;

#if !defined(LD_STATIC)
  if ((dangling_pid = wasora_create_lock(name, 1)) != 0) {
    wasora_push_error_message("sempahore '%s' is being used by process %d", name, dangling_pid);
    wasora_runtime_error();
  }


  umask(0);
  if ((semaphore = sem_open(name, O_CREAT, 0666, 0)) == SEM_FAILED) {
    perror(name);
    return NULL;
  }
#endif

  return semaphore;
}



int wasora_create_lock(char *name, int sem) {
  int dangling_pid;
  struct stat dummy;
  FILE *lock_file;
  char *lock_file_name;

  // si no hay lock_dir, entonces no generamos archivos de lock 
  if (wasora.lock_dir == NULL) {
    return WASORA_RUNTIME_OK;
  }

  if (stat(wasora.lock_dir, &dummy) == -1) {
    umask(0);
    if (mkdir(wasora.lock_dir, 0777) == -1) {
      wasora_push_error_message("creating lock directory '%s'", wasora.lock_dir);
      wasora_runtime_error();
    }
  }

  lock_file_name = malloc(BUFFER_SIZE);
  sprintf(lock_file_name, "%s/%s%s.lock", wasora.lock_dir, (sem)?("sem."):(""), (name[0]=='/')?(name+1):(name));
  umask(0022);
  if (stat(lock_file_name, &dummy) == -1) {
    if ((lock_file = fopen(lock_file_name, "w")) == NULL) {
      wasora_push_error_message("opening lock file '%s'", lock_file_name);
      wasora_runtime_error();
    }
    fprintf(lock_file, "%10d", getpid());
    fclose(lock_file);
  } else {
    if ((lock_file = fopen(lock_file_name, "r")) == NULL) {
      wasora_push_error_message("opening lock file '%s'", lock_file_name);
      wasora_runtime_error();
    }
    if (fscanf(lock_file, "%d", &dangling_pid) != 1) {
      return -1;
    }
    fclose(lock_file);
    return dangling_pid;
  }

  free(lock_file_name);

  return 0;

}

void wasora_free_semaphore(sem_t *semaphore, char *name) {
  wasora_remove_lock(name, 1);
  sem_close(semaphore);
  sem_unlink(name);
}

void wasora_remove_lock(char *name, int sem) {
  struct stat dummy;
  char *lock_file_name;

  // si no hay lock_dir, entonces no generamos archivos de lock
  if (wasora.lock_dir == 0) {
    return;
  }

  lock_file_name = malloc(BUFFER_SIZE);
  sprintf(lock_file_name, "%s/%s%s.lock", wasora.lock_dir, (sem)?("sem."):(""), (name[0]=='/')?(name+1):(name));
  if (stat(lock_file_name, &dummy) == -1) {
//    fprintf(stderr, "advertencia: el archivo de lock '%s' fue borrado externamente.\n", lock_file_name);
  } else {
    unlink(lock_file_name);
  }

  free(lock_file_name);

  return;
}
