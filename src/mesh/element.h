/*------------ -------------- -------- --- ----- ---   --       -            -
 *  FeenoX's mesh-related element header
 *
 *  Copyright (C) 2021--2023 jeremy theler
 *
 *  This file is part of FeenoX.
 *
 *  FeenoX is free software: you can redistribute it and/or modify
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
extern int feenox_mesh_alloc_gauss(gauss_t *gauss, element_type_t *element_type, int Q);
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


extern double feenox_mesh_point_volume(element_t *this);
extern double feenox_mesh_line_volume(element_t *this);
extern double feenox_mesh_triang_volume(element_t *this);
extern double feenox_mesh_quad_volume(element_t *this);
extern double feenox_mesh_tet_volume(element_t *this);
extern double feenox_mesh_hex_volume(element_t *this);
extern double feenox_mesh_prism_volume(element_t *this);

extern double feenox_mesh_line_area(element_t *this);
extern double feenox_mesh_triang_area(element_t *this);
extern double feenox_mesh_quad_area(element_t *this);
extern double feenox_mesh_tet_area(element_t *this);


extern double feenox_mesh_line_size(element_t *this);
extern double feenox_mesh_triang_size(element_t *this);
extern double feenox_mesh_quad_size(element_t *this);
extern double feenox_mesh_tet_size(element_t *this);
extern double feenox_mesh_hex_size(element_t *this);



extern double feenox_mesh_line2_h(int j, double *vec_xi);
extern double feenox_mesh_line2_dhdr(int j, int d, double *arg);

extern double feenox_mesh_line3_h(int j, double *vec_xi);
extern double feenox_mesh_line3_dhdr(int j, int d, double *vec_xi);

extern double feenox_mesh_triang3_h(int j, double *vec_xi);
extern char *feenox_mesh_triang3_h_latex(int j);
extern double feenox_mesh_triang3_dhdr(int j, int d, double *vec_xi);

extern double feenox_mesh_triang6_h(int j, double *vec_xi);
extern char *feenox_mesh_triang6_h_latex(int j);
extern double feenox_mesh_triang6_dhdr(int j, int d, double *vec_xi);

extern double feenox_mesh_quad4_h(int j, double *vec_xi);
extern double feenox_mesh_quad4_dhdr(int j, int d, double *vec_xi);

extern double feenox_mesh_quad8_h(int j, double *vec_xi);
extern double feenox_mesh_quad8_dhdr(int j, int d, double *vec_xi);

extern double feenox_mesh_quad9_h(int j, double *vec_xi);
extern double feenox_mesh_quad9_dhdr(int j, int d, double *vec_xi);

extern double feenox_mesh_tet4_h(int j, double *vec_xi);
extern char *feenox_mesh_tet4_h_latex(int j);
extern double feenox_mesh_tet4_dhdr(int j, int d, double *vec_xi);

extern double feenox_mesh_tet10_h(int j, double *vec_xi);
extern double feenox_mesh_tet10_dhdr(int j, int d, double *vec_xi);

extern double feenox_mesh_hexa8_h(int j, double *vec_xi);
extern double feenox_mesh_hexa8_dhdr(int j, int d, double *vec_xi);

extern double feenox_mesh_hexa20_h(int j, double *vec_xi);
extern double feenox_mesh_hexa20_dhdr(int j, int d, double *vec_xi);

extern double feenox_mesh_hexa27_h(int j, double *vec_xi);
extern double feenox_mesh_hexa27_dhdr(int j, int d, double *vec_xi);

extern double feenox_mesh_prism6_h(int j, double *vec_xi);
extern double feenox_mesh_prism6_dhdr(int j, int d, double *vec_xi);

extern double feenox_mesh_prism15_h(int j, double *vec_xi);
extern double feenox_mesh_prism15_dhdr(int j, int d, double *vec_xi);

extern double feenox_mesh_one_node_point_h(int j, double *vec_xi);
extern double feenox_mesh_one_node_point_dhdr(int j, int d, double *vec_xi);
