// same as 2dpwr-quarter but with 1/8th symmetry (instead of 1/4th)

Point(1) = {0, 0, 0};
Point(2) = {10, 0, 0};
Point(4) = {10, 10, 0};
Point(5) = {70, 0, 0};
Point(6) = {70, 10, 0};
Point(7) = {90, 10, 0};
Point(8) = {90, 0, 0};
Point(13) = {130, 0, 0};
Point(14) = {130, 30, 0};
Point(15) = {110, 30, 0};
Point(16) = {110, 70, 0};
Point(17) = {90, 70, 0};
Point(18) = {90, 90, 0};
Point(20) = {70, 70, 0};
Point(30) = {110, 110, 0};
Point(31) = {110, 90, 0};
Point(32) = {130, 90, 0};
Point(33) = {130, 50, 0};
Point(34) = {150, 50, 0};
Point(35) = {150, 0, 0};
Point(36) = {170, 0, 0};
Point(37) = {170, 70, 0};
Point(38) = {150, 70, 0};
Point(39) = {150, 110, 0};
Point(40) = {130, 110, 0};
Point(41) = {130, 130, 0};


Line(1) = {1, 2};
Line(2) = {2, 4};
Line(3) = {1, 4};
Line(4) = {5, 8};
Line(5) = {8, 7};
Line(7) = {7, 6};
Line(8) = {6, 5};
Line(9) = {2, 5};
Line(10) = {4, 20};
Line(11) = {20, 17};
Line(12) = {17, 18};
Line(13) = {20, 18};
Line(14) = {8, 13};
Line(15) = {13, 14};
Line(16) = {14, 15};
Line(17) = {15, 16};
Line(18) = {16, 17};
Line(19) = {13, 35};
Line(20) = {35, 34};
Line(23) = {34, 33};
Line(24) = {33, 32};
Line(25) = {32, 31};
Line(26) = {31, 30};
Line(27) = {18, 30};
Line(28) = {35, 36};
Line(29) = {36, 37};
Line(30) = {37, 38};
Line(31) = {38, 39};
Line(32) = {39, 40};
Line(33) = {40, 41};
Line(34) = {41, 30};


Line Loop(57) = {1, 2, -3};
Plane Surface(58) = {57};
Line Loop(59) = {7, 8, 4, 5};
Plane Surface(60) = {59};
Line Loop(61) = {11, 12, -13};
Plane Surface(62) = {61};

Line Loop(55) = {9, -8, -7, -5, 14, 15, 16, 17, 18, -11, -10, -2};
Plane Surface(56) = {55};

Line Loop(53) = {19, 20, 23, 24, 25, 26, -27, -12, -18, -17, -16, -15};
Plane Surface(54) = {53};

Line Loop(65) = {28, 29, 30, 31, 32, 33, 34, -26, -25, -24, -23, -20};
Plane Surface(66) = {65};


Physical Surface("fuel1",1) = {54};
Physical Surface("fuel2",2) = {56};
Physical Surface("fuel2rod",3) = {60, 58, 62};
Physical Surface("reflector",4) = {66};

Physical Line("external",5) = {29, 30, 31, 32, 33};
Physical Line("mirror",6) = {1, 9, 4, 14, 19, 28, 3, 10, 13, 27, 34};


// meshing options
Mesh.Algorithm = 6;
Mesh.RecombineAll = 0;  // recombine triangles to obtain quads
Mesh.Optimize = 1;
Mesh.OptimizeNetgen = 1;

Mesh.MeshSizeMin = 5;
Mesh.MeshSizeMax = 5;

Merge "iaea-colors.geo";
