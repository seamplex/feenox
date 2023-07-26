SetFactory("OpenCASCADE");
Point(1) = {0, 0, 0, 1.0};
Point(2) = {0, 1, 0, 1.0};
Point(3) = {0, 2, 0, 1.0};
Point(4) = {1, 1, 0, 1.0};
Point(5) = {2, 2, 0, 1.0};
Line(1) = {1, 2};
Line(2) = {2, 3};
Line(3) = {3, 5};
Line(4) = {5, 4};
Line(5) = {4, 1};
Line(6) = {2, 4};
Curve Loop(1) = {1, 6, 5};
Plane Surface(1) = {1};
Curve Loop(2) = {2, 3, 4, -6};
Plane Surface(2) = {2};


n = 3;
Transfinite Curve {1, 2, 3, 4, 5, 6} = 1+1.5*n;
Transfinite Curve {1, 5} = 1+n;
Transfinite Surface {1};
Transfinite Surface {2};
Mesh.TransfiniteTri = 1;

Mesh.Algorithm = 6;
Mesh.RecombineAll = 1;
Mesh.ElementOrder = 2;
Mesh.SecondOrderIncomplete = 0;

Physical Surface("src") = {1};
Physical Surface("bulk") = {2};
Physical Curve("mirror") = {1, 2, 4, 5};
Physical Curve("vacuum") = {3};

