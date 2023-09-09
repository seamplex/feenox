// --- geometry -------------------------------------------------
SetFactory("OpenCASCADE");
a = 5;
b = 10;
Rectangle(1) = {-a, -a, 0, 2*a, 2*a};
Rectangle(2) = {a, a, 0, a, a};
Rectangle(3) = {-a, a, 0, 2*a, a};
Rectangle(4) = {a, -a, 0, a, 2*a};
Rectangle(5) = {-2*a, a, 0, a, a};
Rectangle(6) = {-2*a, -a, 0, a, 2*a};
Rectangle(7) = {-2*a, -2*a, 0, a, a};
Rectangle(8) = {-a, -2*a, 0, 2*a, a};
Rectangle(9) = {a, -2*a, 0, a, a};
Coherence;

Rotate {{0, 0, 1}, {0, 0, 0}, angle*Pi/180} {
  Surface{5}; Surface{6}; Surface{7}; Surface{3}; Surface{1}; Surface{8}; Surface{4}; Surface{2}; Surface{9}; 
}

Physical Surface("src", 1) = {1};
Physical Surface("abs", 2) = {4, 2, 3, 5, 6, 7, 8, 9};
Physical Curve("vacuum", 49) = {28, 31, 34, 32, 40, 47, 48, 43, 45, 46, 37, 27};

n = 40/Mesh.MeshSizeFactor;
Transfinite Curve {31, 30, 39, 43, 37, 35, 38, 40} = 1+2*n;
Transfinite Curve {28, 26, 36, 45, 34, 33, 41, 48, 27, 25, 29, 32, 46, 44, 42, 47} = 1+n;
Transfinite Surface "*";
Mesh.RecombineAll = 1;
Mesh.ElementOrder = 1;
Mesh.SecondOrderIncomplete = 0;

// c = Sqrt(5.84375*5.84375/2);
// Point(100) = {c-(-10)/Sqrt(2), c+(-10)/Sqrt(2), 0};
// Point(101) = {c-10/Sqrt(2), c+10/Sqrt(2), 0};
// Line(100) = {100, 101};
