/*------------ -------------- -------- --- ----- ---   --       -            -
 *  wasora realtime routines
 *
 *  Copyright (C) 2009--2013 jeremy theler
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

#include <unistd.h>
#include <sys/time.h>

#ifndef _WASORA_H_
#include "wasora.h"
#endif


/* inicializa la estructura t0 */
void wasora_init_realtime(void) {
  gettimeofday(&wasora.t0, NULL);
  return;
}

/* espera un pasito */
void wasora_wait_realtime(void) {

  useconds_t delay;
  useconds_t t_menos_t0;

  if (wasora_var(wasora.special_vars.realtime_scale) == 0) {
    return;
  }

  gettimeofday(&wasora.t, NULL);

 t_menos_t0 = 1e6*(wasora.t.tv_sec - wasora.t0.tv_sec) + (wasora.t.tv_usec - wasora.t0.tv_usec);
 delay = 1e6*wasora_var(wasora_special_var(time))/wasora_var(wasora.special_vars.realtime_scale) - t_menos_t0;

 if (((int)delay) > 0) {
    usleep(delay);
  }
  
  return;
  
}
