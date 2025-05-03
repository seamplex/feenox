SetFactory("OpenCASCADE");
Box(1) = {0, 0, 0, 1, 1, 1};

Physical Surface("left", 1) = {1};
Physical Surface("right", 2) = {2};
Physical Surface("bottom", 3) = {3};
Physical Surface("top", 4) = {4};
Physical Surface("front", 5) = {5};
Physical Surface("back", 6) = {6};
Physical Volume("bulk", 7) = {1};

n = 1;
