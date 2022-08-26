#include <iostream>
#include <random>
#define SEED 123456

#include <benchmark/benchmark.h>

extern "C" {
#include "../feenox.h"
feenox_t feenox;
}

#define INIT_SIGMAS \
  std::default_random_engine rng(SEED); \
  std::uniform_real_distribution<double> distribution(-100.0,+100.0); \
  double sigmax = distribution(rng); \
  double sigmay = distribution(rng); \
  double sigmaz = distribution(rng); \
  double tauxy = distribution(rng); \
  double tauyz = distribution(rng); \
  double tauzx  = distribution(rng); \
  double sigma1[1] = {0}; \
  double sigma2[1] = {0}; \
  double sigma3[1] = {0};
  
  
#define COMPUTE_PRINCIPAL_STRESS_FROM_CAUCHY \
  double I1 = sigmax + sigmay + sigmaz; \
  double I2 = sigmax*sigmay + sigmay*sigmaz + sigmaz*sigmax - gsl_pow_2(tauxy) - gsl_pow_2(tauyz) - gsl_pow_2(tauzx); \
  double I3 = sigmax*sigmay*sigmaz - sigmax*gsl_pow_2(tauyz) - sigmay*gsl_pow_2(tauzx) - sigmaz*gsl_pow_2(tauxy) + 2*tauxy*tauyz*tauzx; \
\
  double c1 = sqrt(fabs(gsl_pow_2(I1) - 3*I2)); \
  double phi = 1.0/3.0 * acos((2.0*gsl_pow_3(I1) - 9.0*I1*I2 + 27.0*I3)/(2.0*gsl_pow_3(c1))); \
  if (isnan(phi)) { \
    phi = 0; \
  } \
\
  double c2 = I1/3.0; \
  double c3 = 2.0/3.0 * c1; \
  *sigma1 = c2 + c3 * cos(phi); \
  *sigma2 = c2 + c3 * cos(phi - 2.0*M_PI/3.0); \
  *sigma3 = c2 + c3 * cos(phi - 4.0*M_PI/3.0);

  
extern "C" {

inline int feenox_principal_stress_from_cauchy_inline(double sigmax, double sigmay, double sigmaz, double tauxy, double tauyz, double tauzx, double *sigma1, double *sigma2, double *sigma3) {
  COMPUTE_PRINCIPAL_STRESS_FROM_CAUCHY;
  return FEENOX_OK;
}

int feenox_principal_stress_from_cauchy_call(double sigmax, double sigmay, double sigmaz, double tauxy, double tauyz, double tauzx, double *sigma1, double *sigma2, double *sigma3) {
  COMPUTE_PRINCIPAL_STRESS_FROM_CAUCHY;
  return FEENOX_OK;
}

} // extern C



static void BM_principal_stress_feenox(benchmark::State& state) {
  INIT_SIGMAS;
  
  for (auto _ : state) {
    feenox_principal_stress_from_cauchy(sigmax, sigmay, sigmaz, tauxy, tauyz, tauzx, sigma1, sigma2, sigma3);
  }
}

static void BM_principal_stress_call(benchmark::State& state) {
  INIT_SIGMAS;
  
  for (auto _ : state) {
    feenox_principal_stress_from_cauchy_call(sigmax, sigmay, sigmaz, tauxy, tauyz, tauzx, sigma1, sigma2, sigma3);
  }
}

static void BM_principal_stress_expanded(benchmark::State& state) {
  INIT_SIGMAS;
  
  for (auto _ : state) {
    COMPUTE_PRINCIPAL_STRESS_FROM_CAUCHY;
    benchmark::DoNotOptimize(sigma1);
    benchmark::DoNotOptimize(sigma2);
    benchmark::DoNotOptimize(sigma3);
  }
}


static void BM_principal_stress_inline(benchmark::State& state) {
  INIT_SIGMAS;
  
  for (auto _ : state) {
    feenox_principal_stress_from_cauchy_inline(sigmax, sigmay, sigmaz, tauxy, tauyz, tauzx, sigma1, sigma2, sigma3);
    benchmark::DoNotOptimize(sigma1);
    benchmark::DoNotOptimize(sigma2);
    benchmark::DoNotOptimize(sigma3);
  }
}

BENCHMARK(BM_principal_stress_feenox);
BENCHMARK(BM_principal_stress_call);
BENCHMARK(BM_principal_stress_expanded);
BENCHMARK(BM_principal_stress_inline);

BENCHMARK_MAIN();
