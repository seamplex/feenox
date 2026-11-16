// RELAP-7 example 8: a square loop of four 1 m pipes driven by a pump,
// plus a 0.5 m vertical stub (pipe5) with the pressure boundary condition
// the coordinates are the same as in the RELAP-7 input, each pipe has 20 elements
Point(1) = {0, 0, 0};
Point(2) = {1, 0, 0};
Point(3) = {1, 1, 0};
Point(4) = {0, 1, 0};
Point(5) = {1, 1.5, 0};

Line(1) = {1, 2};
Line(2) = {2, 3};
Line(3) = {3, 4};
Line(4) = {4, 1};
Line(5) = {3, 5};
Transfinite Curve{1:5} = 20+1;

Physical Curve("pipe1") = {1};
Physical Curve("pipe2") = {2};
Physical Curve("pipe3") = {3};
Physical Curve("pipe4") = {4};
Physical Curve("pipe5") = {5};

Physical Point("pump") = {2};       // pipe1 -> pipe2
Physical Point("junction2") = {3};  // pipe2 -> pipe3 & pipe5
Physical Point("junction3") = {4};  // pipe3 -> pipe4
Physical Point("junction4") = {1};  // pipe4 -> pipe1
Physical Point("outlet") = {5};
