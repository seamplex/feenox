/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox method for stress linearization over SCLs according to ASME
 *
 *  Copyright (C) 2017-2022 Jeremy Theler
 *
 *  This file is part of Feenox <https://www.seamplex.com/feenox>.
 *
 *  feenox is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Feenox is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Feenox.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include "feenox.h"
#include "mechanical.h"

struct linearize_params_t {
  double x1, y1, z1;
  double x2, y2, z2;
  double length;
  
  function_t *function;
};

double feenox_linearization_integrand_membrane(double t_prime, void *params);
double feenox_linearization_integrand_bending(double t_prime, void *params);
double feenox_linearization_integrate(gsl_function *F, function_t *function);
int feenox_linearization_compute_and_store(function_t *total_function, const double x1[], const double x2[], double M, double MBplus, double MBminus, var_t *var_M, var_t *var_MB, var_t *var_P);

  
int feenox_instruction_linearize(void *arg) {
  
  feenox_linearize_t *linearize = (feenox_linearize_t *)arg;

  // this is an implicit SOLVE_PROBLEM point
  if (feenox.pde.problem_solved == 0) {
    feenox_call(feenox_instruction_solve_problem(NULL));
  }
  
  // http://www.eng-tips.com/faqs.cfm?fid=982
  // the actual equations are in ASME VIII-div 2 sec 5
  // also see the mecway manual
  struct linearize_params_t params;
  params.x1 = feenox_expression_eval(&linearize->x1);
  params.y1 = feenox_expression_eval(&linearize->y1);
  params.z1 = feenox_expression_eval(&linearize->z1);

  params.x2 = feenox_expression_eval(&linearize->x2);
  params.y2 = feenox_expression_eval(&linearize->y2);
  params.z2 = feenox_expression_eval(&linearize->z2);
  
  params.length = gsl_hypot3(params.x2-params.x1, params.y2-params.y1, params.z2-params.z1);
  gsl_function F = {NULL, &params};
  
  // membrane stress
  F.function = &feenox_linearization_integrand_membrane;
  double sigmax_m = feenox_linearization_integrate(&F, mechanical.sigmax) / params.length;
  double sigmay_m = feenox_linearization_integrate(&F, mechanical.sigmay) / params.length;
  double sigmaz_m = feenox_linearization_integrate(&F, mechanical.sigmaz) / params.length;
  double tauxy_m = feenox_linearization_integrate(&F, mechanical.tauxy) / params.length;
  double tauyz_m = feenox_linearization_integrate(&F, mechanical.tauyz) / params.length;
  double tauzx_m = feenox_linearization_integrate(&F, mechanical.tauzx) / params.length;
  
  // bending stress
  F.function = &feenox_linearization_integrand_bending;
  double den = gsl_pow_2(params.length)/6.0;
  double sigmax_b = feenox_linearization_integrate(&F, mechanical.sigmax) / den;
  double sigmay_b = feenox_linearization_integrate(&F, mechanical.sigmay) / den;
  double sigmaz_b = feenox_linearization_integrate(&F, mechanical.sigmaz) / den;
  double tauxy_b = feenox_linearization_integrate(&F, mechanical.tauxy) / den;
  double tauyz_b = feenox_linearization_integrate(&F, mechanical.tauyz) / den;
  double tauzx_b = feenox_linearization_integrate(&F, mechanical.tauzx) / den;

  // now we have to compose these guys
  double x1[3] = {params.x1, params.y1, params.z1};
  double x2[3] = {params.x2, params.y2, params.z2};
 
  if (linearize->M != NULL || linearize->MB != NULL || linearize->P != NULL) {
    double M = feenox_vonmises_from_stress_tensor(sigmax_m, sigmay_m, sigmaz_m, tauxy_m, tauyz_m, tauzx_m);
    double MBplus  = feenox_vonmises_from_stress_tensor(sigmax_m+sigmax_b, sigmay_m+sigmay_b, sigmaz_m+sigmaz_b, tauxy_m+tauxy_b, tauyz_m+tauyz_b, tauzx_m+tauzx_b);
    double MBminus = feenox_vonmises_from_stress_tensor(sigmax_m-sigmax_b, sigmay_m-sigmay_b, sigmaz_m-sigmaz_b, tauxy_m-tauxy_b, tauyz_m-tauyz_b, tauzx_m-tauzx_b);
    feenox_call(feenox_linearization_compute_and_store(mechanical.sigma, x1, x2, M, MBplus, MBminus, linearize->M, linearize->MB, linearize->P));
  }

  if (linearize->Mt != NULL || linearize->MBt != NULL || linearize->Pt != NULL) {
    double M = feenox_tresca_from_stress_tensor(sigmax_m, sigmay_m, sigmaz_m, tauxy_m, tauyz_m, tauzx_m);
    double MBplus = feenox_tresca_from_stress_tensor(sigmax_m+sigmax_b, sigmay_m+sigmay_b, sigmaz_m+sigmaz_b, tauxy_m+tauxy_b, tauyz_m+tauyz_b, tauzx_m+tauzx_b);
    double MBminus = feenox_tresca_from_stress_tensor(sigmax_m-sigmax_b, sigmay_m-sigmay_b, sigmaz_m-sigmaz_b, tauxy_m-tauxy_b, tauyz_m-tauyz_b, tauzx_m-tauzx_b);
    feenox_call(feenox_linearization_compute_and_store(mechanical.tresca, x1, x2, M, MBplus, MBminus, linearize->Mt, linearize->MBt, linearize->Pt));
  }
  
  if (linearize->M1 != NULL || linearize->MB1 != NULL || linearize->P1 != NULL ||
      linearize->M2 != NULL || linearize->MB2 != NULL || linearize->P2 != NULL ||
      linearize->M3 != NULL || linearize->MB3 != NULL || linearize->P3 != NULL) {

    double M1, M2, M3;
    double MBplus1, MBplus2, MBplus3;
    double MBminus1, MBminus2, MBminus3;
    
    feenox_principal_stress_from_cauchy(sigmax_m, sigmay_m, sigmaz_m, tauxy_m, tauyz_m, tauzx_m, &M1, &M2, &M3);
    feenox_principal_stress_from_cauchy(sigmax_m+sigmax_b, sigmay_m+sigmay_b, sigmaz_m+sigmaz_b, tauxy_m+tauxy_b, tauyz_m+tauyz_b, tauzx_m+tauzx_b, &MBplus1,  &MBplus2,  &MBplus3);
    feenox_principal_stress_from_cauchy(sigmax_m-sigmax_b, sigmay_m-sigmay_b, sigmaz_m-sigmaz_b, tauxy_m-tauxy_b, tauyz_m-tauyz_b, tauzx_m-tauzx_b, &MBminus1, &MBminus2, &MBminus3);
    
    if (linearize->M1 != NULL || linearize->MB1 != NULL || linearize->P1 != NULL) {
      feenox_call(feenox_linearization_compute_and_store(mechanical.sigma1, x1, x2, M1, MBplus1, MBminus1, linearize->M1, linearize->MB1, linearize->P1));
    }
    if (linearize->M2 != NULL || linearize->MB2 != NULL || linearize->P2 != NULL) {
      feenox_call(feenox_linearization_compute_and_store(mechanical.sigma2, x1, x2, M2, MBplus2, MBminus2, linearize->M2, linearize->MB2, linearize->P2));
    }
    if (linearize->M3 != NULL || linearize->MB3 != NULL || linearize->P3 != NULL) {
      feenox_call(feenox_linearization_compute_and_store(mechanical.sigma3, x1, x2, M3, MBplus3, MBminus3, linearize->M3, linearize->MB3, linearize->P3));
    }
  }
  
/*  
  if (linearize->file != NULL) {
    if (linearize->file->pointer == NULL) {
      feenox_call(feenox_instruction_file_open(linearize->file));
    }

    fprintf(linearize->file->pointer, "# # Stress linearization\n");
    fprintf(linearize->file->pointer, "#\n");

    fprintf(linearize->file->pointer, "# Start point: (%g, %g, %g)\n", params.x1, params.y1, params.z1);
    fprintf(linearize->file->pointer, "#   End point: (%g, %g, %g)\n", params.x2, params.y2, params.z2);
    fprintf(linearize->file->pointer, "#       Total: %s\n", total_name);
    
    fprintf(linearize->file->pointer, "#\n");
    fprintf(linearize->file->pointer, "# ## Membrane stress tensor\n");
    fprintf(linearize->file->pointer, "#\n");
    fprintf(linearize->file->pointer, "#  $\\sigma_{x}$ = %g\n", sigmax_m);
    fprintf(linearize->file->pointer, "#  $\\sigma_{y}$ = %g\n", sigmay_m);
    fprintf(linearize->file->pointer, "#  $\\sigma_{z}$ = %g\n", sigmaz_m);
    fprintf(linearize->file->pointer, "#   $\\tau_{xy}$ = %g\n", tauxy_m);
    fprintf(linearize->file->pointer, "#   $\\tau_{yz}$ = %g\n", tauyz_m);
    fprintf(linearize->file->pointer, "#   $\\tau_{zx}$ = %g\n", tauzx_m);
    fprintf(linearize->file->pointer, "#\n");
    fprintf(linearize->file->pointer, "# ## Bending stress tensor\n");
    fprintf(linearize->file->pointer, "#\n");
    fprintf(linearize->file->pointer, "#  $\\sigma_{x}$ = %g\n", sigmax_b);
    fprintf(linearize->file->pointer, "#  $\\sigma_{y}$ = %g\n", sigmay_b);
    fprintf(linearize->file->pointer, "#  $\\sigma_{z}$ = %g\n", sigmaz_b);
    fprintf(linearize->file->pointer, "#   $\\tau_{xy}$ = %g\n", tauxy_b);
    fprintf(linearize->file->pointer, "#   $\\tau_{yz}$ = %g\n", tauyz_b);
    fprintf(linearize->file->pointer, "#   $\\tau_{zx}$ = %g\n", tauzx_b);
    fprintf(linearize->file->pointer, "#\n");
    fprintf(linearize->file->pointer, "# ## Membrane plus bending stress tensor\n");
    fprintf(linearize->file->pointer, "#\n");
    fprintf(linearize->file->pointer, "#  $\\sigma_{x}$ = %g\n", sigmax_m+sigmax_b);
    fprintf(linearize->file->pointer, "#  $\\sigma_{y}$ = %g\n", sigmay_m+sigmay_b);
    fprintf(linearize->file->pointer, "#  $\\sigma_{z}$ = %g\n", sigmaz_m+sigmaz_b);
    fprintf(linearize->file->pointer, "#   $\\tau_{xy}$ = %g\n", tauxy_m+tauxy_b);
    fprintf(linearize->file->pointer, "#   $\\tau_{yz}$ = %g\n", tauyz_m+tauyz_b);
    fprintf(linearize->file->pointer, "#   $\\tau_{zx}$ = %g\n", tauzx_m+tauzx_b);
    fprintf(linearize->file->pointer, "#\n");
    fprintf(linearize->file->pointer, "# ## Linearization results\n");
    fprintf(linearize->file->pointer, "#\n");
    fprintf(linearize->file->pointer, "#                Membrane stress $M$ = %g\n", feenox_var_value(linearize->M));
    fprintf(linearize->file->pointer, "#  Membrane plus bending stress $MB$ = %g\n", feenox_var_value(linearize->MB));
    fprintf(linearize->file->pointer, "#                    Peak stress $P$ = %g\n", feenox_var_value(linearize->P));
    fprintf(linearize->file->pointer, "#\n");
    fprintf(linearize->file->pointer, "# t_prime\tM\tMB\tT\n");

    // TODO: define 50 as a constant
    // TODO: choose 50
    double t_prime = 0;
    for (unsigned int i = 0; i <= 50; i++) {
      t_prime = i/50.0;
      x1[0] = params.x1 + t_prime * (params.x2 - params.x1);
      x1[1] = params.y1 + t_prime * (params.y2 - params.y1);
      x1[2] = params.z1 + t_prime * (params.z2 - params.z1);
      
      if (t_prime < 0.5) {
        fprintf(linearize->file->pointer, "%.2f\t%e\t%e\t%e\n", t_prime, M, M+((T1>M)?(+1):(-1))*B*(1-2*t_prime), feenox_function_eval(total_function, x1));
      } else {
        fprintf(linearize->file->pointer, "%.2f\t%e\t%e\t%e\n", t_prime, M, M+((T2>M)?(-1):(+1))*B*(1-2*t_prime), feenox_function_eval(total_function, x1));
      }
    }
  }    
   
  feenox_free(total_name);
*/
  
  return FEENOX_OK;
}

double feenox_linearization_integrand_membrane(double t, void *params) {
  struct linearize_params_t *p = (struct linearize_params_t *)params;
  
  double t_prime = t/p->length;
  double x[3];
  x[0] = p->x1 + t_prime * (p->x2 - p->x1);
  x[1] = p->y1 + t_prime * (p->y2 - p->y1);
  x[2] = p->z1 + t_prime * (p->z2 - p->z1);

  return feenox_function_eval(p->function, x);
}

double feenox_linearization_integrand_bending(double t, void *params) {
  struct linearize_params_t *p = (struct linearize_params_t *)params;
  
  double t_prime = t/p->length;
  double x[3];
  x[0] = p->x1 + t_prime * (p->x2 - p->x1);
  x[1] = p->y1 + t_prime * (p->y2 - p->y1);
  x[2] = p->z1 + t_prime * (p->z2 - p->z1);

  return feenox_function_eval(p->function, x) * (0.5*p->length - t);
}


double feenox_linearization_integrate(gsl_function *F, function_t *function) {
  struct linearize_params_t *p = (struct linearize_params_t *)F->params;
  p->function = function;
  double result = 0;
  double epsabs = 0;
  // TODO: choose the epsresl in the input file
  double epsrel = 5e-3;
  size_t limit = DEFAULT_INTEGRATION_INTERVALS;
  int key = GSL_INTEG_GAUSS31;
  double  abserr = 0;
  gsl_integration_workspace *w = gsl_integration_workspace_alloc(limit);
  if (w == NULL) {
    feenox_runtime_error();
  }
  gsl_integration_qag(F, 0, p->length, epsabs, epsrel, limit, key, w, &result, &abserr);
  gsl_integration_workspace_free(w);
  
  return result;
}


int feenox_linearization_compute_and_store(function_t *total_function, const double x1[], const double x2[], double M, double MBplus, double MBminus, var_t *var_M, var_t *var_MB, var_t *var_P) {
  // OJO! esto no es asi, hay que hacer un tensor que sea la diferencia
  double T1 = feenox_function_eval(total_function, x1);
  double T2 = feenox_function_eval(total_function, x2);

  double MB = 0;
  double T = 0;
//  double B = 0;
  if (total_function != mechanical.sigma3) {
    MB = (MBplus > MBminus) ? MBplus : MBminus;
    T = (T1 > T2) ? T1 : T2;
//    B = MB - M;
  } else {
    MB = (MBplus < MBminus) ? MBplus : MBminus;
    T = (T1 < T2) ? T1 : T2;
//    B = M - MB;
  }
  double P = T - MB;
  
  // store results
  if (var_M != NULL) {
    feenox_var_value(var_M) = M;
  }
  if (var_MB != NULL) {
    feenox_var_value(var_MB) = MB;
  }
  if (var_P != NULL) {
    feenox_var_value(var_P) = P;
  }  
  
  return FEENOX_OK;
}
