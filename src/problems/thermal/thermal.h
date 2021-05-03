#ifndef THERMAL_H
#define THERMAL_H

#include "feenox.h"

#define BC_TYPE_THERMAL_UNDEFINED               0
#define BC_TYPE_THERMAL_TEMPERATURE             1
#define BC_TYPE_THERMAL_HEATFLUX                2
#define BC_TYPE_THERMAL_CONVECTION_COEFFICIENT  3
#define BC_TYPE_THERMAL_CONVECTION_TEMPERATURE  4

typedef struct thermal_t thermal_t;

struct thermal_t {
  distribution_t k;     // conductivity 
  distribution_t q;     // volumetric heat source
  distribution_t kappa; // thermal diffusivity = k/(rho cp)
  distribution_t rho;   // density
  distribution_t cp;    // heat capacity
  distribution_t rhocp; // density times heat capacity
  
  int properties_depend_space;
  int properties_depend_temperature;
  
  int bcs_depend_space;
  int bcs_depend_temperature;
};  


#endif /* THERMAL_H */

