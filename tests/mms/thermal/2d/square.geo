SetFactory("OpenCASCADE");
Rectangle(1) = {0, 0, 0, 1, 1, 0};

Physical Surface("bulk", 1) = {1};
Physical Curve("left", 2) = {4};
Physical Curve("right", 3) = {2};
Physical Curve("bottom", 4) = {1};
Physical Curve("top", 5) = {3};

n = 1;
