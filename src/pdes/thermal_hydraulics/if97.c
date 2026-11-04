/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for thermal-hydraulics: water properties
 *  IAPWS-IF97 region 1 (compressed liquid), region 4 saturation pressure
 *  and IAPWS 2008 viscosity (without critical enhancement)
 *
 *  The equations and numerical coefficients are taken from the following
 *  documents of the International Association for the Properties of Water
 *  and Steam (IAPWS), which allow publication in whole or in part provided
 *  that attribution is given to IAPWS:
 *
 *   [1] IAPWS R7-97(2012), Revised Release on the IAPWS Industrial Formulation
 *       1997 for the Thermodynamic Properties of Water and Steam, IAPWS (2007).
 *       https://www.iapws.org/technical-guidance/release/IF97-Rev
 *   [2] W. Wagner et al., The IAPWS Industrial Formulation 1997 for the
 *       Thermodynamic Properties of Water and Steam,
 *       J. Eng. Gas Turbines & Power 122, 150-182 (2000).
 *   [3] IAPWS R12-08, Release on the IAPWS Formulation 2008 for the Viscosity
 *       of Ordinary Water Substance, IAPWS (2008).
 *       https://www.iapws.org/technical-guidance/release/viscosity
 *   [4] M. L. Huber et al., New International Formulation for the Viscosity
 *       of H2O, J. Phys. Chem. Ref. Data 38, 101 (2009).
 *
 *  All coefficients were checked one by one against the tables in [1] and [3].
 *
 *  Copyright (C) 2026 Jeremy Theler
 *
 *  This file is part of FeenoX <https://www.seamplex.com/feenox>.
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
#include "thermal_hydraulics.h"

#define IF97_R      461.526      // J/(kg K), [1] eq. (1)
#define IF97_PSTAR1 16.53e6      // Pa, [1] eq. (7)
#define IF97_TSTAR1 1386.0       // K, [1] eq. (7)

// region 1 coefficients and exponents, [1] table 2

static const int if97_r1_I[34] = {
   0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  2,  2,  2,
   2,  2,  3,  3,  3,  4,  4,  4,  5,  8,  8, 21, 23, 29, 30, 31, 32 };

static const int if97_r1_J[34] = {
  -2, -1,  0,  1,  2,  3,  4,  5, -9, -7, -1,  0,  1,  3, -3,  0,  1,
   3, 17, -4,  0,  6, -5, -2, 10, -8,-11, -6,-29,-31,-38,-39,-40,-41 };

static const double if97_r1_n[34] = {
   0.14632971213167,     -0.84548187169114,     -0.37563603672040e1,
   0.33855169168385e1,   -0.95791963387872,      0.15772038513228,
  -0.16616417199501e-1,   0.81214629983568e-3,   0.28319080123804e-3,
  -0.60706301565874e-3,  -0.18990068218419e-1,  -0.32529748770505e-1,
  -0.21841717175414e-1,  -0.52838357969930e-4,  -0.47184321073267e-3,
  -0.30001780793026e-3,   0.47661393906987e-4,  -0.44141845330846e-5,
  -0.72694996297594e-15, -0.31679644845054e-4,  -0.28270797985312e-5,
  -0.85205128120103e-9,  -0.22425281908000e-5,  -0.65171222895601e-6,
  -0.14341729937924e-12, -0.40516996860117e-6,  -0.12734301741641e-8,
  -0.17424871230634e-9,  -0.68762131295531e-18,  0.14478307828521e-19,
   0.26335781662795e-22, -0.11947622640071e-22,  0.18228094581404e-23,
  -0.93537087292458e-25 };

// derivatives of the dimensionless Gibbs free energy gamma(pi,tau) of region 1
static void if97_r1_gamma(double p, double T, double *gamma_pi, double *gamma_tau, double *gamma_tautau, double *gamma_pitau) {
  double pi = p/IF97_PSTAR1;
  double tau = IF97_TSTAR1/T;
  double a = 7.1 - pi;
  double b = tau - 1.222;
  
  double g_pi = 0, g_tau = 0, g_tautau = 0, g_pitau = 0;
  for (int i = 0; i < 34; i++) {
    int Ii = if97_r1_I[i];
    int Ji = if97_r1_J[i];
    double n = if97_r1_n[i];
    double aI1 = (Ii > 0) ? gsl_pow_int(a, Ii-1) : 0;
    double aI = gsl_pow_int(a, Ii);
    double bJ = gsl_pow_int(b, Ji);
    double bJ1 = gsl_pow_int(b, Ji-1);
    double bJ2 = gsl_pow_int(b, Ji-2);
    
    g_pi     += -n * Ii * aI1 * bJ;
    g_tau    +=  n * aI * Ji * bJ1;
    g_tautau +=  n * aI * Ji * (Ji-1) * bJ2;
    g_pitau  += -n * Ii * aI1 * Ji * bJ1;
  }
  
  if (gamma_pi != NULL)     *gamma_pi = g_pi;
  if (gamma_tau != NULL)    *gamma_tau = g_tau;
  if (gamma_tautau != NULL) *gamma_tautau = g_tautau;
  if (gamma_pitau != NULL)  *gamma_pitau = g_pitau;
  
  return;
}

double thermal_hydraulics_if97_rho(double p, double T) {
  double g_pi = 0;
  if97_r1_gamma(p, T, &g_pi, NULL, NULL, NULL);
  double pi = p/IF97_PSTAR1;
  return p / (IF97_R * T * pi * g_pi);
}

double thermal_hydraulics_if97_h(double p, double T) {
  double g_tau = 0;
  if97_r1_gamma(p, T, NULL, &g_tau, NULL, NULL);
  double tau = IF97_TSTAR1/T;
  return IF97_R * T * tau * g_tau;
}

double thermal_hydraulics_if97_cp(double p, double T) {
  double g_tautau = 0;
  if97_r1_gamma(p, T, NULL, NULL, &g_tautau, NULL);
  double tau = IF97_TSTAR1/T;
  return -IF97_R * tau*tau * g_tautau;
}

// derivative of the density with respect to temperature at constant pressure
static double if97_drhodT(double p, double T) {
  double g_pi = 0, g_pitau = 0;
  if97_r1_gamma(p, T, &g_pi, NULL, NULL, &g_pitau);
  double pi = p/IF97_PSTAR1;
  double tau = IF97_TSTAR1/T;
  // v = R T pi g_pi / p => dv/dT = R pi / p (g_pi - tau g_pitau)
  double v = IF97_R * T * pi * g_pi / p;
  double dvdT = IF97_R * pi / p * (g_pi - tau*g_pitau);
  return -dvdT/(v*v);
}

// inverse function T(p,rho) for region 1 using newton-raphson
double thermal_hydraulics_if97_T_from_p_rho(double p, double rho) {
  double T = 500;
  for (int k = 0; k < 100; k++) {
    double residual = thermal_hydraulics_if97_rho(p, T) - rho;
    double dT = -residual/if97_drhodT(p, T);
    T += dT;
    // keep T within a sane range so the polynomials do not blow up
    if (T < 250) {
      T = 250;
    } else if (T > 700) {
      T = 700;
    }
    if (fabs(dT) < 1e-10*T) {
      break;
    }
  }
  return T;
}

// IAPWS 2008 viscosity of water [3] eqs. (10)-(12) with mu2 = 1
// (i.e. without the critical enhancement, as allowed for industrial use)
double thermal_hydraulics_iapws_mu(double rho, double T) {
  // [3] table 1
  static const double H0[4] = {1.67752, 2.20462, 0.6366564, -0.241605};
  // [3] table 2, H1[i][j]
  static const double H1[6][7] = {
    { 5.20094e-1,  2.22531e-1, -2.81378e-1,  1.61913e-1, -3.25372e-2,  0,           0          },
    { 8.50895e-2,  9.99115e-1, -9.06851e-1,  2.57399e-1,  0,           0,           0          },
    {-1.08374,     1.88797,    -7.72479e-1,  0,           0,           0,           0          },
    {-2.89555e-1,  1.26613,    -4.89837e-1,  0,           6.98452e-2,  0,          -4.35673e-3 },
    { 0,           0,          -2.57040e-1,  0,           0,           8.72102e-3,  0          },
    { 0,           1.20573e-1,  0,           0,           0,           0,          -5.93264e-4 } };
  
  double Tbar = T/647.096;
  double rhobar = rho/322.0;
  
  double sum0 = 0;
  for (int i = 0; i < 4; i++) {
    sum0 += H0[i]/gsl_pow_int(Tbar, i);
  }
  double mu0 = 100*sqrt(Tbar)/sum0;
  
  double sum1 = 0;
  for (int i = 0; i < 6; i++) {
    double inner = 0;
    for (int j = 0; j < 7; j++) {
      inner += H1[i][j] * gsl_pow_int(rhobar-1, j);
    }
    sum1 += gsl_pow_int(1/Tbar - 1, i) * inner;
  }
  double mu1 = exp(rhobar * sum1);
  
  return 1e-6 * mu0 * mu1;
}

// IAPWS-IF97 region 4 saturation pressure as a function of temperature, [1] eqs. (29b) & (30)
double thermal_hydraulics_if97_psat(double T) {
  // [1] table 34
  static const double n[10] = {
     0.11670521452767e4, -0.72421316703206e6, -0.17073846940092e2,
     0.12020824702470e5, -0.32325550322333e7,  0.14915108613530e2,
    -0.48232657361591e4,  0.40511340542057e6, -0.23855557567849,
     0.65017534844798e3 };
  
  double theta = T + n[8]/(T - n[9]);
  double A = theta*theta + n[0]*theta + n[1];
  double B = n[2]*theta*theta + n[3]*theta + n[4];
  double C = n[5]*theta*theta + n[6]*theta + n[7];
  return 1e6 * gsl_pow_4(2*C/(-B + sqrt(B*B - 4*A*C)));
}
