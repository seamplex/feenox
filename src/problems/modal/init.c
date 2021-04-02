#define _GNU_SOURCE  // for asprintf
#include "feenox.h"
feenox_t feenox;

int feenox_problem_init_parser_modal(void) {
  
  feenox.pde.math_type = math_type_eigen;
  
  // TODO: custom names
  feenox_check_alloc(feenox.pde.unknown_name = calloc(feenox.pde.dofs, sizeof(char *)));
  feenox_check_alloc(feenox.pde.unknown_name[0] = strdup("u"));
  if (feenox.pde.dofs > 1) {
    feenox_check_alloc(feenox.pde.unknown_name[1] = strdup("v"));
    if (feenox.pde.dofs > 2) {
      feenox_check_alloc(feenox.pde.unknown_name[2] = strdup("w"));
    }  
  }
  
  feenox_call(feenox_problem_define_solutions());
  
///va+lambda+name lambda
///va+lambda+detail 
///va+lambda+detail Requested eigenvalue. It is equal to 1.0 until
///va+lambda+detail `FINO_STEP` is executed.  
  feenox.pde.vars.lambda = feenox_define_variable_get_ptr("lambda");
  feenox_var_value(feenox.pde.vars.lambda) = 1.0;
  
///va+M_T+name M_T
///va+M_T+desc Total mass\ $m$ computed from the mass matrix\ $M$ as
///va+M_T+desc 
///va+M_T+desc \[ M_T = \frac{1}{n_\text{DOFs}} \cdot \vec{1}^T \cdot M \cdot \vec{1} \]
///va+M_T+desc 
///va+M_T+desc where $n_\text{DOFs}$ is the number of degrees of freedoms per node.
///va+M_T+desc Note that this is only approximately equal to the actual mass, i.e. the integral of the density $\rho(x,y,z)$ over the problem domain.
  feenox.pde.vars.M_T = feenox_define_variable_get_ptr("M_T");

///ve+f+name f
///ve+f+desc _Size:_ number of requested eigen-pairs.
///ve+f+desc _Elements:_ The frequency $f_i$ of the $i$-th mode, in cycles per unit of time.
  feenox.pde.vectors.f = feenox_define_vector_get_ptr("f", feenox.pde.nev);

///ve+omega+name omega
///ve+omega+desc _Size:_ number of requested eigen-pairs.
///ve+omega+desc _Elements:_ The angular frequency $\omega_i$ of the $i$-th mode, in radians per unit of time.
  feenox.pde.vectors.omega = feenox_define_vector_get_ptr("omega", feenox.pde.nev);


///ve+m+name m
///ve+m+desc _Size:_ number of requested eigen-pairs.
///ve+m+desc _Elements:_ The generalized modal mass $M_i$ of the $i$-th mode computed as
///ve+m+desc
///ve+m+desc \[ \text{m}_i = \frac{1}{n_\text{DOFs}} \vec{\phi}_i^T \cdot M \cdot \vec{\phi}_i \]
///va+m+desc 
///va+m+desc where $n_\text{DOFs}$ is the number of degrees of freedoms per node, $M$ is the mass matrix
///va+m+desc and $\vec{\phi}_i$ is the $i$-th eigenvector normalized such that the largest element is equal to one.

  feenox.pde.vectors.m = feenox_define_vector_get_ptr("m", feenox.pde.nev);

///ve+L+name L
///ve+L+desc _Size:_ number of requested eigen-pairs.
///ve+L+desc _Elements:_ The excitation factor $L_i$ of the $i$-th mode computed as
///ve+L+desc
///ve+L+desc \[ L_i = \frac{1}{n_\text{DOFs}} \cdot \vec{\phi}_i^T \cdot M \cdot \vec{1} \]
///va+L+desc 
///va+L+desc where $n_\text{DOFs}$ is the number of degrees of freedoms per node, $M$ is the mass matrix
///va+L+desc and $\vec{\phi}_i$ is the $i$-th eigenvector normalized such that the largest element is equal to one.
  feenox.pde.vectors.L = feenox_define_vector_get_ptr("L", feenox.pde.nev);

///ve+Gamma+name Gamma
///ve+Gamma+desc _Size:_ number of requested eigen-pairs.
///ve+Gamma+desc _Elements:_ The participation factor $\Gamma_i$ of the $i$-th mode computed as
///ve+Gamma+desc
///ve+Gamma+desc \[ \Gamma_i = \frac{ \vec{\phi}_i^T \cdot M \cdot \vec{1} }{ \vec{\phi}_i^T \cdot M \cdot \vec{\phi}} \]
  feenox.pde.vectors.Gamma = feenox_define_vector_get_ptr("Gamma", feenox.pde.nev);

///ve+mu+name mu
///ve+mu+desc _Size:_ number of requested eigen-pairs.
///ve+mu+desc _Elements:_ The relatve effective modal mass $\mu_i$ of the $i$-th mode computed as
///ve+mu+desc
///ve+mu+desc \[ \mu_i = \frac{L_i^2}{M_t \cdot n_\text{DOFs} \cdot m_i} \]
///ve+mu+desc
///ve+mu+desc Note that $\sum_{i=1}^N m_i = 1$, where $N$ is total number of degrees of freedom ($n_\text{DOFs}$ times the number of nodes).
  feenox.pde.vectors.mu = feenox_define_vector_get_ptr("mu", feenox.pde.nev);

///ve+Mu+name Mu
///ve+Mu+desc _Size:_ number of requested eigen-pairs.
///ve+Mu+desc _Elements:_ The accumulated relative effective modal mass $\Mu_i$ up to the $i$-th mode computed as
///ve+Mu+desc
///ve+Mu+desc \[ \Mu_i = \sum_{j=1}^i \mu_i \]
///ve+Mu+desc
///ve+Mu+desc Note that $\Mu_N = 1$, where $N$ is total number of degrees of freedom ($n_\text{DOFs}$ times the number of nodes).
  feenox.pde.vectors.Mu = feenox_define_vector_get_ptr("Mu", feenox.pde.nev);

  feenox_check_alloc(feenox.pde.vectors.phi = calloc(feenox.pde.nev, sizeof(vector_t *)));;
  unsigned int i = 0;

  for (i = 0; i < feenox.pde.nev; i++) {
    char *modename = NULL;
    feenox_check_minusone(asprintf(&modename, "phi%d", i+1));
    feenox_check_alloc(feenox.pde.vectors.phi[i] = feenox_define_vector_get_ptr(modename, 0));
    feenox_free(modename);
  }

  
  return FEENOX_OK;
}


int feenox_problem_init_runtime_modal(void) {
  return FEENOX_OK;
}
