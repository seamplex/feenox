extern int mesh_line2_init(void);
extern int mesh_line3_init(void);

extern int mesh_triang3_init(void);
extern int mesh_triang6_init(void);
  
extern int mesh_quad4_init(void);
extern int mesh_quad8_init(void);
extern int mesh_quad9_init(void);
  
extern int mesh_tet4_init(void);
extern int mesh_tet10_init(void);
  
extern int mesh_hexa8_init(void);
extern int mesh_hexa20_init(void);
extern int mesh_hexa27_init(void);
    
extern int mesh_prism6_init(void);
extern int mesh_prism15_init(void);

extern int mesh_one_node_point_init(void);


extern int mesh_alloc_gauss(gauss_t *gauss, element_type_t *element_type, int V);
extern int mesh_init_shape_at_gauss(gauss_t *gauss, element_type_t *element_type);


extern double mesh_line2_h(int k, double *vec_r);
extern double mesh_line2_dhdr(int k, int m, double *arg);
extern int mesh_point_in_line(element_t *element, const double *x);
extern double mesh_line_vol(element_t *element);

extern void mesh_gauss_init_line2(element_type_t *element_type, gauss_t *gauss);
extern void mesh_gauss_init_line1(element_type_t *element_type, gauss_t *gauss);


extern double mesh_line3_h(int k, double *vec_r);
extern double mesh_line3_dhdr(int k, int m, double *vec_r);


extern double mesh_triang3_h(int j, double *vec_r);
