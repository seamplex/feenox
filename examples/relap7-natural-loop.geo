// RELAP-7 example 10: a primary loop with a core channel and the primary side
// of a heat exchanger, plus an open secondary side
// all coordinates follow the RELAP-7 input (the loop lives in the y-z plane)
Point(1)  = {0, 1,      0};     // pipe4 -> pipe1
Point(2)  = {0, 0,      0};     // pipe1 -> core
Point(3)  = {0, 0,      0.8};   // core -> pipe2
Point(4)  = {0, 0,      5.98};  // pipe2 -> pipe3 & pipe5
Point(5)  = {0, 1,      5.98};  // pipe3 -> primary
Point(6)  = {0, 1,      2.27};  // primary -> pipe4
Point(7)  = {0, 0,      6};     // end of pipe5 (pressurizer)
Point(8)  = {0, 1.5,    2.27};  // start of pipe6 (secondary inlet)
Point(9)  = {0, 1.2,    2.27};  // end of pipe6
Point(10) = {0, 1.2044, 5.98};  // start of the secondary side of the heat exchanger
Point(11) = {0, 1.2044, 2.27};  // end of the secondary side (secondary outlet)

Line(1) = {1, 2};    // pipe1
Line(2) = {2, 3};    // core
Line(3) = {3, 4};    // pipe2
Line(4) = {4, 5};    // pipe3
Line(5) = {5, 6};    // primary side of the heat exchanger
Line(6) = {6, 1};    // pipe4
Line(7) = {4, 7};    // pipe5
Line(8) = {8, 9};    // pipe6
Line(9) = {10, 11};  // secondary side of the heat exchanger

Transfinite Curve{1,2,4} = 20+1;
Transfinite Curve{3,5,6,9} = 50+1;
Transfinite Curve{7} = 10+1;
Transfinite Curve{8} = 5+1;

Physical Curve("pipe1") = {1};
Physical Curve("core") = {2};
Physical Curve("pipe2") = {3};
Physical Curve("pipe3") = {4};
Physical Curve("primary") = {5};
Physical Curve("pipe4") = {6};
Physical Curve("pipe5") = {7};
Physical Curve("pipe6") = {8};
Physical Curve("secondary") = {9};

Physical Point("junction1") = {2};
Physical Point("junction2") = {3};
Physical Point("junction3") = {4};
Physical Point("junction4") = {5};
Physical Point("junction5") = {6};
Physical Point("junction6") = {1};
// pipe6 and the secondary side do not touch, they are joined with MERGE
Physical Point("junction7") = {9, 10};

Physical Point("pressurizer") = {7};
Physical Point("inlet") = {8};
Physical Point("outlet") = {11};
