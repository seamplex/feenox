/* 
                        BENCHMARK PROBLEM
           
  Identification: 11             Source Situation ID.11
  Date Submitted: June 1976      By: R. R. Lee (CE)
                                     D. A. Menely (Ontario Hydro)
                                     B. Micheelsen (Riso-Denmark)
                                     D. R. Vondy (ORNL)
                                     M. R. Wagner (KWU)
                                     W. Werner (GRS-Munich)
 
  Date Accepted:  June 1977      By: H. L. Dodds, Jr. (U. of Tenn.)
                                     M. V. Gregory (SRL)
 
  Descriptive Title: Multi-dimensional (x-y-z) LWR model
  
  Suggested Functions: Designed to provide a sever test for
                       the capabilities of coarse mesh
                       methods and flux synthesis approximations
 
  Configuration:       Three-dimensional configuration
                       including space dimensions and region
                       numbers: 2 Figures
*/

SetFactory("OpenCASCADE");


// vertical reflector starting from z = 0 and extruded up to z = 380
Point(1) = {0, 0, 0};
Point(2) = {170, 0, 0};
Point(3) = {170, 70, 0};
Point(4) = {150, 70, 0};
Point(5) = {150, 110, 0};
Point(6) = {130, 110, 0};
Point(7) = {130, 130, 0};
Point(8) = {110, 130, 0};
Point(9) = {110, 150, 0};
Point(10) = {70, 150, 0};
Point(11) = {70, 170, 0};
Point(12) = {0, 170, 0};

Line(1) = {1, 2};
Line(2) = {2, 3};
Line(3) = {3, 4};
Line(4) = {4, 5};
Line(5) = {5, 6};
Line(6) = {6, 7};
Line(7) = {7, 8};
Line(8) = {8, 9};
Line(9) = {9, 10};
Line(10) = {10, 11};
Line(11) = {11, 12};
Line(12) = {12, 1};
Curve Loop(1) = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
Plane Surface(1) = {1};

Extrude {0, 0, 380} {
  Surface{1};
}

// fuel 1
Point(25) = {130, 0, 20};
Point(26) = {130, 30, 20};
Point(27) = {110, 30, 20};
Point(28) = {110, 70, 20};
Point(29) = {90, 70, 20};
Point(30) = {90, 90, 20};
Point(31) = {70, 90, 20};
Point(32) = {70, 110, 20};
Point(33) = {30, 110, 20};
Point(34) = {30, 130, 20};
Point(35) = {0, 130, 20};
Point(36) = {0, 150, 20};
Point(37) = {50, 150, 20};
Point(38) = {50, 130, 20};
Point(39) = {90, 130, 20};
Point(40) = {90, 110, 20};
Point(41) = {110, 110, 20};
Point(42) = {110, 90, 20};
Point(43) = {130, 90, 20};
Point(44) = {130, 50, 20};
Point(45) = {150, 50, 20};
Point(46) = {150, 0, 20};

Line(37) = {25, 46};
Line(38) = {46, 45};
Line(39) = {45, 44};
Line(40) = {44, 43};
Line(41) = {43, 42};
Line(42) = {42, 41};
Line(43) = {41, 40};
Line(44) = {40, 39};
Line(45) = {39, 38};
Line(46) = {38, 37};
Line(47) = {37, 36};
Line(48) = {36, 35};
Line(49) = {35, 34};
Line(50) = {34, 33};
Line(51) = {33, 32};
Line(52) = {32, 31};
Line(53) = {31, 30};
Line(54) = {30, 29};
Line(55) = {29, 28};
Line(56) = {28, 27};
Line(57) = {27, 26};
Line(58) = {26, 25};
Curve Loop(15) = {38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 37};
Plane Surface(15) = {15};
Extrude {0, 0, 340} {
  Surface{15};
}

// fuel 2
Point(69) = {0, 0, 20};
Line(103) = {25, 69};
Line(104) = {69, 35};
Curve Loop(39) = {103, 104, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58};
Plane Surface(39) = {39};
Extrude {0, 0, 340} {
  Surface{39}; 
}

// control rods
Box(4) = {0, 0, 20, 10, 10, 380-20};
Box(5) = {70, 0, 20, 20, 10, 380-20};
Box(6) = {0, 70, 20, 10, 20, 380-20};
Box(7) = {70, 70, 20, 20, 20, 380-20};
Box(8) = {30, 30, 280, 20, 20, 380-280};
Coherence;

Physical Volume("fuel1", 1) = {2};
Physical Volume("fuel2", 2) = {5};
Physical Volume("fuel2rod", 3) = {7, 6, 13, 14, 12};
Physical Volume("reflector", 4) = {3};
Physical Volume("reflrod", 5) = {9, 11, 4, 10, 8};

Physical Surface("mirror", 6) = {13, 98, 66, 89, 25, 70, 97, 86, 82, 84, 1, 95, 87, 31, 73, 51, 81, 54, 8, 99};
Physical Surface("vacuum", 7) = {4, 91, 85, 88, 93, 52, 3, 15, 14, 16, 17, 18, 19, 20, 21, 22, 2};


General.SmallAxes = 0;

// meshing options
Mesh.Algorithm = 8;
Mesh.RecombineAll = 0;
Mesh.Optimize = 1;
Mesh.OptimizeNetgen = 1;
Mesh.ElementOrder = 2;

Mesh.MeshSizeMin = 10;
Mesh.MeshSizeMax = 10;


Merge "iaea-colors.geo";

