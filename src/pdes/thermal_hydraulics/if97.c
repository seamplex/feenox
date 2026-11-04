/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for thermal-hydraulics: water properties
 *  IAPWS-IF97 region 1 (compressed liquid), region 4 saturation pressure
 *  IAPWS 2008 viscosity (without critical enhancement), and
 *  IAPWS 2011 thermal conductivity (industrial critical enhancement)
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
 *   [5] IAPWS R15-11, Release on the IAPWS Formulation 2011 for the Thermal
 *       Conductivity of Ordinary Water Substance, IAPWS (2011).
 *       https://iapws.org/technical-guidance/release/ThCond
 *
 *  All coefficients were checked against the tables in [1], [3], and [5].
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
static void if97_r1_gamma(double p, double T, double *gamma_pi, double *gamma_tau, double *gamma_tautau, double *gamma_pitau, double *gamma_pipi) {
  double pi = p/IF97_PSTAR1;
  double tau = IF97_TSTAR1/T;
  double a = 7.1 - pi;
  double b = tau - 1.222;
  
  double g_pi = 0, g_tau = 0, g_tautau = 0, g_pitau = 0, g_pipi = 0;
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
    if (Ii > 1) {
      g_pipi += n * Ii * (Ii-1) * gsl_pow_int(a, Ii-2) * bJ;
    }
  }
  
  if (gamma_pi != NULL)     *gamma_pi = g_pi;
  if (gamma_tau != NULL)    *gamma_tau = g_tau;
  if (gamma_tautau != NULL) *gamma_tautau = g_tautau;
  if (gamma_pitau != NULL)  *gamma_pitau = g_pitau;
  if (gamma_pipi != NULL)   *gamma_pipi = g_pipi;
  
  return;
}

double thermal_hydraulics_if97_rho(double p, double T) {
  double g_pi = 0;
  if97_r1_gamma(p, T, &g_pi, NULL, NULL, NULL, NULL);
  double pi = p/IF97_PSTAR1;
  return p / (IF97_R * T * pi * g_pi);
}

double thermal_hydraulics_if97_h(double p, double T) {
  double g_tau = 0;
  if97_r1_gamma(p, T, NULL, &g_tau, NULL, NULL, NULL);
  double tau = IF97_TSTAR1/T;
  return IF97_R * T * tau * g_tau;
}

double thermal_hydraulics_if97_cp(double p, double T) {
  double g_tautau = 0;
  if97_r1_gamma(p, T, NULL, NULL, &g_tautau, NULL, NULL);
  double tau = IF97_TSTAR1/T;
  return -IF97_R * tau*tau * g_tautau;
}

// derivative of the density with respect to temperature at constant pressure
static double if97_drhodT(double p, double T) {
  double g_pi = 0, g_pitau = 0;
  if97_r1_gamma(p, T, &g_pi, NULL, NULL, &g_pitau, NULL);
  double pi = p/IF97_PSTAR1;
  double tau = IF97_TSTAR1/T;
  // v = R T pi g_pi / p => dv/dT = R pi / p (g_pi - tau g_pitau)
  double v = IF97_R * T * pi * g_pi / p;
  double dvdT = IF97_R * pi / p * (g_pi - tau*g_pitau);
  return -dvdT/(v*v);
}

double thermal_hydraulics_if97_beta(double p, double T) {
  return -if97_drhodT(p, T)/thermal_hydraulics_if97_rho(p, T);
}

// IAPWS-2011 [5], eqs. (15)-(26), including the industrial critical enhancement.
double thermal_hydraulics_if97_k(double p, double T) {
  // [5] tables 1 and 2.
  static const double L0[5] = {2.443221e-3, 1.323095e-2, 6.770357e-3, -3.454586e-3, 4.096266e-4};
  static const double L1[5][6] = {
    {1.60397357, -0.646013523, 0.111443906, 0.102997357, -0.0504123634, 0.00609859258},
    {2.33771842, -2.78843778, 1.53616167, -0.463045512, 0.0832827019, -0.00719201245},
    {2.19650529, -4.54580785, 3.55777244, -1.40944978, 0.275418278, -0.0205938816},
    {-1.21051378, 1.60812989, -0.621178141, 0.0716373224, 0, 0},
    {-2.7203370, 4.57586331, -3.18369245, 1.1168348, -0.19268305, 0.012913842}
  };
  // [5] table 6, indexed by density interval j, then power i.
  static const double A[5][6] = {
    {6.53786807199516, -5.61149954923348, 3.39624167361325, -2.27492629730878, 10.2631854662709, 1.97815050331519},
    {6.52717759281799, -6.30816983387575, 8.08379285492595, -9.82240510197603, 12.1358413791395, -5.54349664571295},
    {5.35500529896124, -3.96415689925446, 8.91990208918795, -12.0338729505790, 9.19494865194302, -2.16866274479712},
    {1.55225959906681, 0.464621290821181, 8.93237374861479, -11.0321960061126, 6.16780999933360, -0.965458722086812},
    {1.11999926419994, 0.595748562571649, 9.88952565078920, -10.3255051147040, 4.66861294457414, -0.503243546373828}
  };
  double g_pi, g_tautau, g_pitau, g_pipi;
  if97_r1_gamma(p, T, &g_pi, NULL, &g_tautau, &g_pitau, &g_pipi);
  double tau = IF97_TSTAR1/T;
  double rho = IF97_PSTAR1/(IF97_R*T*g_pi);
  double cp = -IF97_R*tau*tau*g_tautau;
  double cv = cp + IF97_R*gsl_pow_2(g_pi-tau*g_pitau)/g_pipi;
  double drhodp = -g_pipi*rho/(g_pi*IF97_PSTAR1);
  double Tr = T/647.096;
  double rhor = rho/322.0;
  double sum0 = 0, sum1 = 0;
  for (int i = 0; i < 5; i++) {
    sum0 += L0[i]/gsl_pow_int(Tr, i);
    for (int j = 0; j < 6; j++) {
      sum1 += L1[i][j]*gsl_pow_int(1/Tr-1, i)*gsl_pow_int(rhor-1, j);
    }
  }
  double lambda = sqrt(Tr)/sum0 * exp(rhor*sum1);
  int j = (rhor <= 0.310559006) ? 0 : (rhor <= 0.776397516) ? 1 :
          (rhor <= 1.242236025) ? 2 : (rhor <= 1.863354037) ? 3 : 4;
  double sumA = 0;
  for (int i = 0; i < 6; i++) {
    sumA += A[j][i]*gsl_pow_int(rhor, i);
  }
  double zeta = drhodp*22.064e6/322.0;
  double delta_chi = GSL_MAX(0, rhor*(zeta-1.5/(Tr*sumA)));
  double y = 0.13/0.40 * pow(delta_chi/0.06, 0.630/1.239);
  if (y >= 1.2e-7) {
    double cvcp = cv/cp;
    double Z = 2/(M_PI*y) * ((1-cvcp)*atan(y) + cvcp*y +
                            expm1(-1/(1/y+y*y/(3*rhor*rhor))));
    double mur = thermal_hydraulics_iapws_mu(rho, T)/1e-6;
    lambda += 177.8514*rhor*(cp/461.51805)*Tr/mur*Z;
  }
  return 1e-3*lambda;
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
