Point(1) = {0, 0, 0, 1.0};
Point(2) = {4, 0, 0, 1.0};
Point(3) = {4, 3.5, 0, 1.0};
Point(4) = {3.5, 4, 0, 1.0};
Point(5) = {2, 4, 0, 1.0};
Point(6) = {1.5, 3.5, 0, 1.0};
Point(7) = {0, 3.5, 0, 1.0};
Line(1) = {1, 2};
Line(3) = {3, 4};
Line(4) = {4, 5};
Line(5) = {6, 7};
Line(6) = {7, 1};
Point(8) = {1.5, 4, 0, 1.0};
Circle(7) = {5, 8, 6};
Line(8) = {2, 3};

Curve Loop(1) = {1, 8, 3, 4, 7, 5, 6};
Plane Surface(1) = {1};

Extrude {{0, 1, 0}, {0, 0, 0}, 2/3*Pi} {
  Surface{1}; 
}
Extrude {{0, 1, 0}, {0, 0, 0}, 2/3*Pi} {
  Surface{40}; 
}
Coherence;
Extrude {{0, 1, 0}, {0, 0, 0}, 2/3*Pi} {
  Surface{72}; 
}
Coherence;
/*
Physical Surface("symmetry") = {72, 1, 19, 51};
Physical Surface("gap") = {59, 63, 67, 31, 35, 38, 70, 27};
Physical Surface("external") = {35, 23, 55};
*/

Physical Volume("uo2") = {1,2,3};
Physical Surface("symmetry", 103) = {51, 83, 19};
Physical Surface("external", 104) = {55, 23, 87};
Physical Surface("gap", 105) = {91, 95, 99, 102, 27, 31, 35, 38, 59, 63, 67, 70};
Mesh.MeshSizeMax = 0.5;
Mesh.ColorCarousel = 2;
