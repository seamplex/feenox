// critical size of the problem UD20-1-0-SL (number 22 report Los Alamos)
a = 2 * 10.371065;
lc = a/18;

Point(1) = {0,   0, 0, 1.4*lc};
Point(2) = {a/2, 0, 0, 0.6*lc};
Point(3) = {a,   0, 0, 1.4*lc};

Line(1) = {1, 2};
Line(2) = {2, 3};

Physical Point("left") = {1};
Physical Point("right") = {3};
Physical Line("fuel") = {1, 2};
//Uncomment the next line to have 2 order elements.
//Mesh.ElementOrder = 2;
