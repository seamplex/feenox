/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox method for stress linearization over SCLs according to ASME
 *
 *  Copyright (C) 2017-2022 jeremy theler
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
extern feenox_t feenox;
extern mechanical_t mechanical;

struct linearize_params_t {
  double x1, y1, z1;
  double x2, y2, z2;
  double t;
  
  function_t *function;
};
double feenox_linearization_integrand_membrane(double t_prime, void *params);
double feenox_linearization_integrand_bending(double t_prime, void *params);

int feenox_instruction_linearize(void *arg) {
  
  feenox_linearize_t *linearize = (feenox_linearize_t *)arg;

  // http://www.eng-tips.com/faqs.cfm?fid=982
  struct linearize_params_t params;
  params.x1 = feenox_expression_eval(&linearize->x1);
  params.y1 = feenox_expression_eval(&linearize->y1);
  params.z1 = feenox_expression_eval(&linearize->z1);

  params.x2 = feenox_expression_eval(&linearize->x2);
  params.y2 = feenox_expression_eval(&linearize->y2);
  params.z2 = feenox_expression_eval(&linearize->z2);
  
  params.t = gsl_hypot3(params.x2-params.x1, params.y2-params.y1, params.z2-params.z1);
  
  gsl_function F;
  F.params = &params;
  F.function = &feenox_linearization_integrand_membrane;

  // TODO: choose integration intervals
  gsl_integration_workspace *w = gsl_integration_workspace_alloc(DEFAULT_INTEGRATION_INTERVALS);
  double error = 0;  

  // membrane
  double sigmax_m = 0;
  params.function = mechanical.sigmax;
  
  // int gsl_integration_qag(const gsl_function *f, double a, double b, double epsabs, double epsrel, size_t limit, int key, gsl_integration_workspace *workspace, double *result, double *abserr)
  // int gsl_integration_qags(const gsl_function *f, double a, double b, double epsabs, double epsrel, size_t limit, gsl_integration_workspace *workspace, double *result, double *abserr)
  // int gsl_integration_qagp(const gsl_function *f, double *pts, size_t npts, double epsabs, double epsrel, size_t limit, gsl_integration_workspace *workspace, double *result, double *abserr)  
  gsl_integration_qags (&F, 0, params.t, 0, DEFAULT_INTEGRATION_INTERVALS, DEFAULT_INTEGRATION_INTERVALS, w, &sigmax_m, &error);

  double sigmay_m = 0;
  params.function = mechanical.sigmay;
  gsl_integration_qags (&F, 0, params.t, 0, DEFAULT_INTEGRATION_INTERVALS, DEFAULT_INTEGRATION_INTERVALS, w, &sigmay_m, &error);

  double sigmaz_m = 0;
  params.function = mechanical.sigmaz;
  gsl_integration_qags (&F, 0, params.t, 0, DEFAULT_INTEGRATION_INTERVALS, DEFAULT_INTEGRATION_INTERVALS, w, &sigmaz_m, &error);
  
  double tauxy_m = 0;
  params.function = mechanical.tauxy;
  gsl_integration_qags (&F, 0, params.t, 0, DEFAULT_INTEGRATION_INTERVALS, DEFAULT_INTEGRATION_INTERVALS, w, &tauxy_m, &error);

  double tauyz_m = 0;
  double tauzx_m = 0;
  if (feenox.pde.dim > 2) {
    params.function = mechanical.tauyz;
    gsl_integration_qags (&F, 0, params.t, 0, DEFAULT_INTEGRATION_INTERVALS, DEFAULT_INTEGRATION_INTERVALS, w, &tauyz_m, &error);
  
    params.function = mechanical.tauzx;
    gsl_integration_qags (&F, 0, params.t, 0, DEFAULT_INTEGRATION_INTERVALS, DEFAULT_INTEGRATION_INTERVALS, w, &tauzx_m, &error);
  }

  // bending
  F.function = &feenox_linearization_integrand_bending;
 
  double sigmax_b = 0;
  params.function = mechanical.sigmax;
  gsl_integration_qags (&F, 0, params.t, 0, DEFAULT_INTEGRATION_INTERVALS, DEFAULT_INTEGRATION_INTERVALS, w, &sigmax_b, &error);

  double sigmay_b = 0;
  params.function = mechanical.sigmay;
  gsl_integration_qags (&F, 0, params.t, 0, DEFAULT_INTEGRATION_INTERVALS, DEFAULT_INTEGRATION_INTERVALS, w, &sigmay_b, &error);

  double sigmaz_b = 0;
  params.function = mechanical.sigmaz;
  gsl_integration_qags (&F, 0, params.t, 0, DEFAULT_INTEGRATION_INTERVALS, DEFAULT_INTEGRATION_INTERVALS, w, &sigmaz_b, &error);
  
  double tauxy_b = 0;
  params.function = mechanical.tauxy;
  gsl_integration_qags (&F, 0, params.t, 0, DEFAULT_INTEGRATION_INTERVALS, DEFAULT_INTEGRATION_INTERVALS, w, &tauxy_b, &error);

  double tauyz_b = 0;  
  double tauzx_b = 0;
  if (feenox.pde.dim > 2) {
    
    params.function = mechanical.tauyz;
    gsl_integration_qags (&F, 0, params.t, 0, DEFAULT_INTEGRATION_INTERVALS, DEFAULT_INTEGRATION_INTERVALS, w, &tauyz_b, &error);
  
    params.function = mechanical.tauzx;
    gsl_integration_qags (&F, 0, params.t, 0, DEFAULT_INTEGRATION_INTERVALS, DEFAULT_INTEGRATION_INTERVALS, w, &tauzx_b, &error);
  }
  
  gsl_integration_workspace_free(w);
  

  sigmax_m /= params.t;
  sigmay_m /= params.t;
  sigmaz_m /= params.t;
  tauxy_m  /= params.t;
  
  double den = gsl_pow_2(params.t)/6.0;
  sigmax_b /= den;
  sigmay_b /= den;
  sigmaz_b /= den;
  tauxy_b  /= den;

  if (feenox.pde.dim > 2) {
    tauyz_m  /= params.t;
    tauzx_m  /= params.t;
    tauyz_b  /= den;
    tauzx_b  /= den;
  }

  
  double x1[3];
  x1[0] = params.x1;
  x1[1] = params.y1;
  x1[2] = params.z1;
  double x2[3];
  x2[0] = params.x2;
  x2[1] = params.y2;
  x2[2] = params.z2;
 
  double M = 0;
  double B = 0;
  double MB = 0;
  double MBplus = 0;
  double MBminus = 0;
  double P = 0;
  double T1 = 0;
  double T2 = 0;
  double T = 0;
  function_t *total_function = NULL;
  char *total_name = NULL;

  switch (linearize->composition) {
    case linearize_composition_vonmises:
      total_function = mechanical.sigma;
      total_name = strdup("Von Mises");
      M = feenox_vonmises_from_stress_tensor(sigmax_m, sigmay_m, sigmaz_m, tauxy_m, tauyz_m, tauzx_m);
      MBplus = feenox_vonmises_from_stress_tensor(sigmax_m+sigmax_b, sigmay_m+sigmay_b, sigmaz_m+sigmaz_b, tauxy_m+tauxy_b, tauyz_m+tauyz_b, tauzx_m+tauzx_b);
      MBminus = feenox_vonmises_from_stress_tensor(sigmax_m-sigmax_b, sigmay_m-sigmay_b, sigmaz_m-sigmaz_b, tauxy_m-tauxy_b, tauyz_m-tauyz_b, tauzx_m-tauzx_b);
      
      // OJO! esto no es asi, hay que hacer un tensor que sea la diferencia
      T1 = feenox_function_eval(mechanical.sigma, x1);
      T2 = feenox_function_eval(mechanical.sigma, x2);
    break;

    case linearize_composition_tresca:
      total_function = mechanical.tresca;
      total_name = strdup("Tresca");
//      M = feenox_tresca_from_stress_tensor(sigmax_m, sigmay_m, sigmaz_m, tauxy_m, tauyz_m, tauzx_m);
//      MBplus = feenox_compute_tresca_from_stress_tensor(sigmax_m+sigmax_b, sigmay_m+sigmay_b, sigmaz_m+sigmaz_b, tauxy_m+tauxy_b, tauyz_m+tauyz_b, tauzx_m+tauzx_b);
//      MBminus = feenox_compute_tresca_from_stress_tensor(sigmax_m-sigmax_b, sigmay_m-sigmay_b, sigmaz_m-sigmaz_b, tauxy_m-tauxy_b, tauyz_m-tauyz_b, tauzx_m-tauzx_b);
      M = feenox_vonmises_from_stress_tensor(sigmax_m, sigmay_m, sigmaz_m, tauxy_m, tauyz_m, tauzx_m);
      MBplus = feenox_vonmises_from_stress_tensor(sigmax_m+sigmax_b, sigmay_m+sigmay_b, sigmaz_m+sigmaz_b, tauxy_m+tauxy_b, tauyz_m+tauyz_b, tauzx_m+tauzx_b);
      MBminus = feenox_vonmises_from_stress_tensor(sigmax_m-sigmax_b, sigmay_m-sigmay_b, sigmaz_m-sigmaz_b, tauxy_m-tauxy_b, tauyz_m-tauyz_b, tauzx_m-tauzx_b);
      
      T1 = feenox_function_eval(mechanical.tresca, x1);
      T2 = feenox_function_eval(mechanical.tresca, x2);
    break;

    case linearize_composition_principal1:
      total_function = mechanical.sigma1;
      total_name = strdup("Principal 1");
      feenox_principal_stress_from_cauchy(sigmax_m, sigmay_m, sigmaz_m, tauxy_m, tauyz_m, tauzx_m, &M, NULL, NULL);
      feenox_principal_stress_from_cauchy(sigmax_m+sigmax_b, sigmay_m+sigmay_b, sigmaz_m+sigmaz_b, tauxy_m+tauxy_b, tauyz_m+tauyz_b, tauzx_m+tauzx_b, &MBplus, NULL, NULL);
      feenox_principal_stress_from_cauchy(sigmax_m-sigmax_b, sigmay_m-sigmay_b, sigmaz_m-sigmaz_b, tauxy_m-tauxy_b, tauyz_m-tauyz_b, tauzx_m-tauzx_b, &MBminus, NULL, NULL);
      
      T1 = feenox_function_eval(mechanical.sigma1, x1);
      T2 = feenox_function_eval(mechanical.sigma1, x2);
    break;

    case linearize_composition_principal2:
      total_function = mechanical.sigma2;
      total_name = strdup("Principal 2");
      feenox_principal_stress_from_cauchy(sigmax_m, sigmay_m, sigmaz_m, tauxy_m, tauyz_m, tauzx_m, NULL, &M, NULL);
      feenox_principal_stress_from_cauchy(sigmax_m+sigmax_b, sigmay_m+sigmay_b, sigmaz_m+sigmaz_b, tauxy_m+tauxy_b, tauyz_m+tauyz_b, tauzx_m+tauzx_b, NULL, &MBplus, NULL);
      feenox_principal_stress_from_cauchy(sigmax_m-sigmax_b, sigmay_m-sigmay_b, sigmaz_m-sigmaz_b, tauxy_m-tauxy_b, tauyz_m-tauyz_b, tauzx_m-tauzx_b, NULL, &MBminus, NULL);
      
      T1 = feenox_function_eval(mechanical.sigma1, x1);
      T2 = feenox_function_eval(mechanical.sigma2, x2);
    break;
  
    case linearize_composition_principal3:
      total_function = mechanical.sigma3;
      total_name = strdup("Principal 3");
      feenox_principal_stress_from_cauchy(sigmax_m, sigmay_m, sigmaz_m, tauxy_m, tauyz_m, tauzx_m, NULL, NULL, &M);
      feenox_principal_stress_from_cauchy(sigmax_m+sigmax_b, sigmay_m+sigmay_b, sigmaz_m+sigmaz_b, tauxy_m+tauxy_b, tauyz_m+tauyz_b, tauzx_m+tauzx_b, NULL, NULL, &MBplus);
      feenox_principal_stress_from_cauchy(sigmax_m-sigmax_b, sigmay_m-sigmay_b, sigmaz_m-sigmaz_b, tauxy_m-tauxy_b, tauyz_m-tauyz_b, tauzx_m-tauzx_b, NULL, NULL, &MBminus);
      
      T1 = feenox_function_eval(mechanical.sigma3, x1);
      T2 = feenox_function_eval(mechanical.sigma3, x2);
    break;
  }

  if (linearize->composition != linearize_composition_principal3) {
    MB = (MBplus > MBminus) ? MBplus : MBminus;
    T = (T1 > T2) ? T1 : T2;
    B = MB - M;
  } else {
    MB = (MBplus < MBminus) ? MBplus : MBminus;
    T = (T1 < T2) ? T1 : T2;
    B = M - MB;
  }
  P = T - MB;
  
  feenox_var_value(linearize->M) = M;
  feenox_var_value(linearize->MB) = MB;
  feenox_var_value(linearize->P) = P;
  
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
/*
    fprintf(linearize->file->pointer, "# ## Membrane plus bending stress tensor\n");
    fprintf(linearize->file->pointer, "#\n");
    fprintf(linearize->file->pointer, "#  $\\sigma_{x}$ = %g\n", sigmax_m+sigmax_b);
    fprintf(linearize->file->pointer, "#  $\\sigma_{y}$ = %g\n", sigmay_m+sigmay_b);
    fprintf(linearize->file->pointer, "#  $\\sigma_{z}$ = %g\n", sigmaz_m+sigmaz_b);
    fprintf(linearize->file->pointer, "#   $\\tau_{xy}$ = %g\n", tauxy_m+tauxy_b);
    fprintf(linearize->file->pointer, "#   $\\tau_{yz}$ = %g\n", tauyz_m+tauyz_b);
    fprintf(linearize->file->pointer, "#   $\\tau_{zx}$ = %g\n", tauzx_m+tauzx_b);
    fprintf(linearize->file->pointer, "#\n");
*/
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
  
  return FEENOX_OK;
}

double feenox_linearization_integrand_membrane(double t_prime, void *params) {
  struct linearize_params_t *p = (struct linearize_params_t *)params;
  
  double x[3];
  x[0] = p->x1 + t_prime/p->t * (p->x2 - p->x1);
  x[1] = p->y1 + t_prime/p->t * (p->y2 - p->y1);
  x[2] = p->z1 + t_prime/p->t * (p->z2 - p->z1);

  return feenox_function_eval(p->function, x);
}

double feenox_linearization_integrand_bending(double t_prime, void *params) {
  struct linearize_params_t *p = (struct linearize_params_t *)params;
  
  double x[3];
  x[0] = p->x1 + t_prime/p->t * (p->x2 - p->x1);
  x[1] = p->y1 + t_prime/p->t * (p->y2 - p->y1);
  x[2] = p->z1 + t_prime/p->t * (p->z2 - p->z1);

  return feenox_function_eval(p->function, x) * (0.5*p->t - t_prime);
}
