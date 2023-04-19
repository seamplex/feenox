SetFactory("OpenCASCADE");
Box(1) = {0, 0, 0, 1, 1, 1};
Box(2) = {1, 0, 0, 1, 1, 1};
Coherence;

Physical Surface("zero",1) = {1};
Physical Surface("ramp",2) = {7};
Physical Surface("side",3) = {4, 6, 3, 5, 9, 11, 8, 10};

Physical Volume("left",4) = {1};
Physical Volume("right",5) = {2};


