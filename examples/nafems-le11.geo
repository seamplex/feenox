// NAFEMS LE11 benchmark structured curved hexahedral mesh
SetFactory("OpenCASCADE");

Point(1) = {0, 0, 0};

Point(2) = {1.000, 0, 0};   // A
Point(3) = {1.400, 0, 0};   // B

Point(4) = {1.000*Sin(Pi/4), 0, 1.000*Sin(Pi/4)};
Point(5) = {Sqrt(1.400^2-(1.000*Sin(Pi/4))^2), 0, 1.000*Sin(Pi/4)};

Point(6) = {1.000*Sin(Pi/4), 0, 1.000*Sin(Pi/4)+.345+.345};
Point(7) = {1.000,           0, 1.000*Sin(Pi/4)+.345+.345};

Point(8) = {1.000*Sin(Pi/4), 0, 1.000*Sin(Pi/4)+.345+.345+.400};
Point(9) = {1.000,           0, 1.000*Sin(Pi/4)+.345+.345+.400};

Circle(1) = {2, 1, 4};
Circle(2) = {3, 1, 5};
Line(3) = {4, 6};
Line(4) = {6, 8};
Line(5) = {8, 9};
Line(6) = {9, 7};
Line(7) = {7, 5};
Line(8) = {3, 2};
Line(9) = {4, 5};
Line(10) = {6, 7};

Curve Loop(1) = {8, 1, 9, -2};
Plane Surface(1) = {1};

Curve Loop(2) = {7, -9, 3, 10};
Plane Surface(2) = {2};

Curve Loop(3) = {6, -10, 4, 5};
Plane Surface(3) = {3};

Transfinite Curve {8, 9, 10, 5} = 4/Mesh.MeshSizeFactor+1;
Transfinite Curve {1, 2} = 6/Mesh.MeshSizeFactor+1;
Transfinite Curve {3, 7} = 8/Mesh.MeshSizeFactor+1;
Transfinite Curve {4, 6} = 4/Mesh.MeshSizeFactor+1;
Transfinite Surface {1} = {2, 3, 5, 4};
Transfinite Surface {2} = {4, 5, 7, 6};
Transfinite Surface {3} = {6, 7, 9, 8};

Extrude {{0, 0, 1}, {0, 0, 0}, Pi/2} {
  Surface{1}; Surface{2}; Surface{3}; Layers{12/Mesh.MeshSizeFactor}; Recombine;
}

Physical Surface("xz") = {3, 2, 1};
Physical Surface("yz") = {8, 12, 16};
Physical Surface("xy") = {4};
Physical Surface("HIH'I'") = {15};

Physical Volume("bulk") = {1, 2, 3};

Mesh.SecondOrderLinear = 0;       // curved
Mesh.ElementOrder = 2;            // 2nd order 
Mesh.SecondOrderIncomplete = 0;   // 27-noded
Mesh.RecombineAll = 1;            // hexahedra

