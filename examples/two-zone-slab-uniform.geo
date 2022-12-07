Merge "ab.geo";
a_left  = Floor(a/lc)*lc;
a_right = (Floor(a/lc)+1)*lc;

Point(1) = {0, 0, 0, lc};
Point(2) = {a_left, 0, 0, lc};
Point(3) = {a_right, 0, 0, lc};
Point(4) = {b, 0, 0, lc};

Line(1) = {1, 2};
Line(2) = {2, 3};
Line(3) = {3, 4};
Physical Line("A") = {1};
Physical Line("AB") = {2};
Physical Line("B") = {3};

Physical Point("left") = {1};
Physical Point("right") = {4};
