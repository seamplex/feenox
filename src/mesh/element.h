extern int feenox_mesh_alloc_gauss(gauss_t *gauss, element_type_t *element_type, int V);
extern int feenox_mesh_init_shape_at_gauss(gauss_t *gauss, element_type_t *element_type);
extern void feenox_mesh_add_node_parent(node_relative_t **parents, int index);
extern void feenox_mesh_compute_coords_from_parent(element_type_t *element_type, int j);


extern int feenox_mesh_line2_init(void);
extern int feenox_mesh_line3_init(void);

extern int feenox_mesh_triang3_init(void);
extern int feenox_mesh_triang6_init(void);
  
extern int feenox_mesh_quad4_init(void);
extern int feenox_mesh_quad8_init(void);
extern int feenox_mesh_quad9_init(void);
  
extern int feenox_mesh_tet4_init(void);
extern int feenox_mesh_tet10_init(void);
  
extern int feenox_mesh_hexa8_init(void);
extern int feenox_mesh_hexa20_init(void);
extern int feenox_mesh_hexa27_init(void);
    
extern int feenox_mesh_prism6_init(void);
extern int feenox_mesh_prism15_init(void);

extern int feenox_mesh_one_node_point_init(void);

extern int feenox_mesh_gauss_init_line2(element_type_t *element_type, gauss_t *gauss);
extern int feenox_mesh_gauss_init_line1(element_type_t *element_type, gauss_t *gauss);
extern int feenox_mesh_gauss_init_line3(element_type_t *element_type, gauss_t *gauss);

extern int feenox_mesh_gauss_init_triang1(element_type_t *element_type, gauss_t *gauss);
extern int feenox_mesh_gauss_init_triang3(element_type_t *element_type, gauss_t *gauss);

extern int feenox_mesh_gauss_init_quad1(element_type_t *element_type, gauss_t *gauss);
extern int feenox_mesh_gauss_init_quad4(element_type_t *element_type, gauss_t *gauss);
extern int feenox_mesh_gauss_init_quad9(element_type_t *element_type, gauss_t *gauss);

extern int feenox_mesh_gauss_init_tet1(element_type_t *element_type, gauss_t *gauss);
extern int feenox_mesh_gauss_init_tet4(element_type_t *element_type, gauss_t *gauss);

extern int feenox_mesh_gauss_init_hexa1(element_type_t *element_type, gauss_t *gauss);
extern int feenox_mesh_gauss_init_hexa8(element_type_t *element_type, gauss_t *gauss);
extern int feenox_mesh_gauss_init_hexa27(element_type_t *element_type, gauss_t *gauss);

extern int feenox_mesh_prism_gauss6_init(element_type_t *element_type);

// TODO: either tet or tetrahedron in both
extern int feenox_mesh_point_in_line(element_t *this, const double *x);
extern int feenox_mesh_point_in_triangle(element_t *this, const double *x);
extern int feenox_mesh_point_in_quadrangle(element_t *this, const double *x);
extern int feenox_mesh_point_in_tetrahedron(element_t *this, const double *x);
extern int feenox_mesh_point_in_hexahedron(element_t *this, const double *x);
extern int feenox_mesh_point_in_prism(element_t *element, const double *x);


extern double feenox_mesh_point_vol(element_t *this);
extern double feenox_mesh_line_vol(element_t *this);
extern double feenox_mesh_triang_vol(element_t *this);
extern double feenox_mesh_quad_vol(element_t *this);
extern double feenox_mesh_tet_vol(element_t *this);
extern double feenox_mesh_hex_vol(element_t *this);
extern double feenox_mesh_prism_vol(element_t *this);








extern double feenox_mesh_line2_h(int k, double *vec_r);
extern double feenox_mesh_line2_dhdr(int k, int m, double *arg);

extern double feenox_mesh_line3_h(int k, double *vec_r);
extern double feenox_mesh_line3_dhdr(int k, int m, double *vec_r);

extern double feenox_mesh_triang3_h(int j, double *vec_r);
extern double feenox_mesh_triang3_dhdr(int j, int m, double *vec_r);

extern double feenox_mesh_triang6_h(int j, double *vec_r);
extern double feenox_mesh_triang6_dhdr(int j, int m, double *vec_r);

extern double feenox_mesh_quad4_h(int j, double *vec_r);
extern double feenox_mesh_quad4_dhdr(int j, int m, double *vec_r);

extern double feenox_mesh_quad8_h(int j, double *vec_r);
extern double feenox_mesh_quad8_dhdr(int j, int m, double *vec_r);

extern double feenox_mesh_quad9_h(int j, double *vec_r);
extern double feenox_mesh_quad9_dhdr(int j, int m, double *vec_r);

extern double feenox_mesh_tet4_h(int j, double *vec_r);
extern double feenox_mesh_tet4_dhdr(int j, int m, double *vec_r);

extern double feenox_mesh_tet10_h(int j, double *vec_r);
extern double feenox_mesh_tet10_dhdr(int j, int m, double *vec_r);

extern double feenox_mesh_hexa8_h(int j, double *vec_r);
extern double feenox_mesh_hexa8_dhdr(int j, int m, double *vec_r);

extern double feenox_mesh_hexa20_h(int j, double *vec_r);
extern double feenox_mesh_hexa20_dhdr(int j, int m, double *vec_r);

extern double feenox_mesh_hexa27_h(int j, double *vec_r);
extern double feenox_mesh_hexa27_dhdr(int j, int m, double *vec_r);

extern double feenox_mesh_prism6_h(int j, double *vec_r);
extern double feenox_mesh_prism6_dhdr(int j, int m, double *vec_r);

extern double feenox_mesh_prism15_h(int j, double *vec_r);
extern double feenox_mesh_prism15_dhdr(int j, int m, double *vec_r);

extern double feenox_mesh_one_node_point_h(int i, double *vec_r);
extern double feenox_mesh_one_node_point_dhdr(int i, int j, double *vec_r);