#ifndef THERMAL_H
#define THERMAL_H

#include "feenox.h"

typedef struct thermal_t thermal_t;

struct thermal_t {
  distribution_t k;     // conductivity 
  distribution_t q;     // volumetric heat source
  distribution_t Q;     // volumetric heat source
  distribution_t kappa; // thermal diffusivity = k/(rho cp)
  distribution_t rho;   // density
  distribution_t cp;    // heat capacity
  distribution_t rhocp; // density times heat capacity
};  


#endif /* THERMAL_H */

