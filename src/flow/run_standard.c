/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX standard run
 *
 *  Copyright (C) 2009--2021 jeremy theler
 *
 *  This file is part of FeenoX.
 *
 *  FeenoX is free software: you can redistribute it and/or modify
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
extern feenox_t feenox;

int feenox_run_standard(void) {

  feenox_call(feenox_init_before_run());
  
  // first the static step (or steps)
  feenox_special_var_value(in_static) = 1;
  feenox_special_var_value(in_static_first) = 1;
  while (!feenox_special_var_value(done) &&
         !feenox_special_var_value(done_static) &&
         (int)(feenox_special_var_value(step_static) < rint(feenox_special_var_value(static_steps)))) {

    // TODO
//    feenox_debug();

    feenox_special_var_value(step_static) = feenox_special_var_value(step_static) + 1;
    if ((int)feenox_special_var_value(step_static) != 1 &&
        (int)feenox_special_var_value(step_static) == (int)feenox_special_var_value(static_steps)) {
      feenox_special_var_value(in_static_last) = 1;
    }

    if (feenox.dae.daes == NULL) {
      
      feenox_call(feenox_step(feenox.instructions, NULL));
      
    } else {
      
      feenox_call(feenox_step(feenox.instructions, feenox.dae.instruction));
      if (feenox.dae.system == NULL) {
        feenox_call(feenox_dae_init());
      }
      feenox_call(feenox_dae_ic());
      feenox_call(feenox_step(feenox.dae.instruction->next, NULL));
      
    }
    feenox_special_var_value(in_static_first) = 0;
  }
  feenox_special_var_value(in_static) = 0;
  feenox_special_var_value(in_static_last) = 0;
  feenox_special_var_value(step_static) = 0;

  
#ifdef HAVE_IDA
  double ida_step_dt = INFTY;
#endif
  double t_old = 0;

  // TODO: in quasistatic, should we increase the time with the initial dt?
  
  // transient/quasistatic loop  (if needed)
  feenox_special_var_value(in_transient) = 1;
  feenox_special_var_value(in_transient_first) = 1;
  while (feenox_special_var_value(done) == 0) {

//    feenox_debug();
    
    feenox_special_var_value(step_transient)++;
#ifdef HAVE_PETSC
    if (feenox.dae.dimension == 0 && feenox.pde.ts == NULL) {
#else
    if (feenox.dae.dimension == 0) {
#endif	    
      feenox_limit_time_step();
      double next_time = feenox_special_var_value(t) + feenox_special_var_value(dt);
      double next_time_path = 0;
      
      // see if we overshoot the next time path (or end_time)
      if (feenox.time_paths != NULL && feenox.time_path_current->items != NULL && next_time > feenox_expression_eval(feenox.time_path_current)) {
        // go a little bit ahead of the next time so if there's a 
        // discontinuity all the stuff gets updated
        next_time_path = feenox_expression_eval(feenox.time_path_current);
        next_time = next_time_path + 1e-8*(feenox_special_var_value(dt));
      } else if (next_time > feenox_special_var_value(end_time)) {
        next_time = feenox_special_var_value(end_time)+1e-8;
      }

      // time paths and min_dts do not get well along, but still...
      feenox_special_var_value(dt) = next_time - feenox_special_var_value(t);
      feenox_limit_time_step();
      feenox_special_var_value(t) += feenox_special_var_value(dt);
      
      // if we did pass the next time, update the pointer
      if (feenox.time_paths != NULL && feenox_special_var_value(t) > next_time_path) {
        feenox.time_path_current++;
      }

      if (feenox_special_var_value(t) >= feenox_special_var_value(end_time)) {
        feenox_special_var_value(in_transient_last) = 1;
        feenox_special_var_value(done_transient) = 1;
      }
      
      // all the instructions
      feenox_call(feenox_step(feenox.instructions, NULL));
      
    } else if (feenox.pde.solve != NULL) {
      
      feenox_limit_time_step();
      double next_time = feenox_special_var_value(t) + feenox_special_var_value(dt);
      double next_time_path = 0;
      
      // see if we overshoot the next time path (or end_time)
      if (feenox.time_paths != NULL && feenox.time_path_current->items != NULL && next_time > feenox_expression_eval(feenox.time_path_current)) {
        // go a little bit ahead of the next time so if there's a 
        // discontinuity all the stuff gets updated
        next_time_path = feenox_expression_eval(feenox.time_path_current);
        next_time = next_time_path + 1e-8*(feenox_special_var_value(dt));
      } else if (next_time > feenox_special_var_value(end_time)) {
        next_time = feenox_special_var_value(end_time)+1e-8;
      }

      // time paths and min_dts do not get well along, but still...
      feenox_special_var_value(dt) = next_time - feenox_special_var_value(t);
      feenox_limit_time_step();
      
      feenox_call(feenox_step(feenox.instructions, feenox.pde.instruction));
      
      t_old = feenox_special_var_value(t);
      feenox_call(feenox_instruction_solve_problem(NULL));
//      feenox_special_var_value(dt) = feenox_special_var_value(t) - t_old;
//      feenox_special_var_value(t) = t_old + feenox_special_var_value(dt);
      
      // if we did pass the next time, update the pointer
      if (feenox.time_paths != NULL && feenox_special_var_value(t) > next_time_path) {
        feenox.time_path_current++;
      }

      if (feenox_special_var_value(t) >= feenox_special_var_value(end_time)) {
        feenox_special_var_value(in_transient_last) = 1;
        feenox_special_var_value(done_transient) = 1;
      }
      
      feenox_call(feenox_step(feenox.pde.instruction->next, NULL));
      
      
    } else if (feenox.dae.dimension != 0) {
      
#ifdef HAVE_IDA
        
      feenox_call(feenox_step(feenox.instructions, feenox.dae.instruction));
      // integration step
      // remember what the time was so we can then compute dt
      t_old = feenox_special_var_value(t);
      double ida_step_t_old = 0;
      double ida_step_t_new = 0;
      int err = 0;

      // is there a max dt?
      if (feenox_special_var_value(max_dt) != 0) {
        ida_call(IDASetMaxStep(feenox.dae.system, feenox_special_var_value(max_dt)));
      }

      // if the actual IDA's dt is smaller than FeenoX' min_dt
      if (ida_step_dt < feenox_special_var_value(min_dt) || (feenox_special_var_value(min_dt) != 0 && feenox_special_var_value(t) == 0)) {

        if (feenox.time_path_current != NULL && feenox.time_path_current->items != NULL) {
          feenox_push_error_message("both min_dt and TIME_PATH given");
          return FEENOX_ERROR;
        }

        // set a stop time so we don't overshoot
        // TODO: is it better to call with IDA_NORMAL?
        ida_call(IDASetStopTime(feenox.dae.system, feenox_special_var_value(t)+feenox_special_var_value(min_dt)));
        do {
          ida_step_t_old = feenox_special_var_value(t);
          ida_step_t_new = feenox_special_var_value(t)+feenox_special_var_value(min_dt);
          ida_call(IDASolve(feenox.dae.system, feenox_special_var_value(dt), &ida_step_t_new, feenox.dae.x, feenox.dae.dxdt, IDA_ONE_STEP));
  //        err = IDASolve(feenox.dae.system, feenox_special_var_value((dt)), &ida_step_t_new, feenox.dae.x, feenox.dae.dxdt, IDA_NORMAL);
          feenox_special_var_value(t) = ida_step_t_new;
          ida_step_dt = ida_step_t_new - ida_step_t_old;
        } while (err != IDA_TSTOP_RETURN);

      } else {

        // check for TIME_PATH
        if (feenox.time_path_current != NULL && feenox.time_path_current->items != NULL) {
          ida_call(IDASetStopTime(feenox.dae.system, feenox_expression_eval(feenox.time_path_current)+1e-4*feenox_special_var_value(dt)));
        }

        ida_step_t_old = feenox_special_var_value(t);
        err = IDASolve(feenox.dae.system, feenox_special_var_value(end_time), &feenox_special_var_value(t), feenox.dae.x, feenox.dae.dxdt, IDA_ONE_STEP);
        ida_step_dt = feenox_special_var_value(t) - ida_step_t_old;

        if (err == IDA_SUCCESS) {
          ; // ok!
        } else if (err == IDA_TSTOP_RETURN) {
          ++feenox.time_path_current;
        } else {
          feenox_push_error_message("ida returned error code %d", err);
          return FEENOX_ERROR;
        }
      }

      feenox_special_var_value(dt) = feenox_special_var_value(t) - t_old;
      if (feenox_special_var_value(t) >= feenox_special_var_value(end_time)) {
        feenox_special_var_value(in_transient_last) = 1;
        feenox_special_var_value(done_transient) = 1;
      }
      
      feenox_call(feenox_step(feenox.dae.instruction, NULL));
#endif      
    }
    
    // done! (the following line fixes a subtle bug found by rvignolo)
    feenox_special_var_value(in_transient_first) = 0;

  }

  
  return FEENOX_OK;
}


void feenox_limit_time_step(void) {
  if (feenox_special_var_value(dt) < feenox_special_var_value(min_dt)) {
    feenox_special_var_value(dt) = feenox_special_var_value(min_dt);
  }
  if (feenox_special_var_value(max_dt) > 0 && feenox_special_var_value(dt) > feenox_special_var_value(max_dt)) {
    feenox_special_var_value(dt) = feenox_special_var_value(max_dt);
  }
  return;
}
