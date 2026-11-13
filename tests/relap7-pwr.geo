// RELAP-7 Example 11: two-loop model PWR, with the manual's positions,
// orientations, lengths and number of elements (everything lies on x = 0).
// RELAP-7 connects components by name, not by coordinates, so pipe ends
// that do not touch are joined by MERGE point groups in the .fee.
Lc = 3.6576;   // core length

// reactor vessel
Point(1) = {0, -1.2, 0};    Point(2) = {0, -1.2, Lc};   // CH1
Point(3) = {0,  0,   0};    Point(4) = {0,  0,   Lc};   // CH2
Point(5) = {0,  1.2, 0};    Point(6) = {0,  1.2, Lc};   // CH3
Point(7) = {0,  1.5, 0};    Point(8) = {0,  1.5, Lc};   // bypass

// loop A
Point(11) = {0, 2,     4};  Point(12) = {0, 2,     0};  // DownComer-A
Point(13) = {0, 0.5,   4};  Point(14) = {0, 0.5,   8};  // pipe1-HL-A
Point(15) = {0, 4,     8};                              // pipe2-HL-A end
Point(16) = {0, 3,     4};                              // pipe1-CL-A start
Point(17) = {0, 4,     4};  Point(18) = {0, 3.2,   4};  // pipe2-CL-A
Point(19) = {0, 5.2,   4};  Point(20) = {0, 4.2,   4};  // pipe1-SC-A
Point(21) = {0, 4.2,   8};  Point(22) = {0, 5.2,   8};  // pipe2-SC-A
Point(23) = {0, 4.201, 4};  Point(24) = {0, 4.201, 8};  // HX-A:secondary_pipe
Point(25) = {0, 0.5,   8.5};                            // pipe-to-Pressurizer end

// loop B
Point(31) = {0, -2,     4};  Point(32) = {0, -2,     0};  // DownComer-B
Point(33) = {0, -0.5,   4};  Point(34) = {0, -0.5,   8};  // pipe1-HL-B
Point(35) = {0, -4,     8};                               // pipe2-HL-B end
Point(36) = {0, -3,     4};                               // pipe1-CL-B start
Point(37) = {0, -4,     4};  Point(38) = {0, -3.2,   4};  // pipe2-CL-B
Point(39) = {0, -5.2,   4};  Point(40) = {0, -4.2,   4};  // pipe1-SC-B
Point(41) = {0, -4.2,   8};  Point(42) = {0, -5.2,   8};  // pipe2-SC-B
Point(43) = {0, -4.201, 4};  Point(44) = {0, -4.201, 8};  // HX-B:secondary_pipe

// lines follow the RELAP-7 position -> position + length*orientation
Line(1) = {1, 2};     Transfinite Curve{1} = 8+1;
Line(2) = {3, 4};     Transfinite Curve{2} = 8+1;
Line(3) = {5, 6};     Transfinite Curve{3} = 8+1;
Line(4) = {7, 8};     Transfinite Curve{4} = 5+1;

Line(11) = {11, 12};  Transfinite Curve{11} = 3+1;   // DownComer-A
Line(12) = {13, 14};  Transfinite Curve{12} = 3+1;   // pipe1-HL-A
Line(13) = {14, 15};  Transfinite Curve{13} = 3+1;   // pipe2-HL-A
Line(14) = {16, 11};  Transfinite Curve{14} = 3+1;   // pipe1-CL-A
Line(15) = {17, 18};  Transfinite Curve{15} = 3+1;   // pipe2-CL-A
Line(16) = {19, 20};  Transfinite Curve{16} = 3+1;   // pipe1-SC-A
Line(17) = {21, 22};  Transfinite Curve{17} = 3+1;   // pipe2-SC-A
Line(18) = {15, 17};  Transfinite Curve{18} = 10+1;  // HX-A:primary_pipe
Line(19) = {23, 24};  Transfinite Curve{19} = 10+1;  // HX-A:secondary_pipe
Line(20) = {14, 25};  Transfinite Curve{20} = 3+1;   // pipe-to-Pressurizer

Line(31) = {31, 32};  Transfinite Curve{31} = 3+1;   // DownComer-B
Line(32) = {33, 34};  Transfinite Curve{32} = 3+1;   // pipe1-HL-B
Line(33) = {34, 35};  Transfinite Curve{33} = 3+1;   // pipe2-HL-B
Line(34) = {36, 31};  Transfinite Curve{34} = 3+1;   // pipe1-CL-B
Line(35) = {37, 38};  Transfinite Curve{35} = 3+1;   // pipe2-CL-B
Line(36) = {39, 40};  Transfinite Curve{36} = 3+1;   // pipe1-SC-B
Line(37) = {41, 42};  Transfinite Curve{37} = 3+1;   // pipe2-SC-B
Line(38) = {35, 37};  Transfinite Curve{38} = 10+1;  // HX-B:primary_pipe
Line(39) = {43, 44};  Transfinite Curve{39} = 10+1;  // HX-B:secondary_pipe

Physical Curve("CH1") = {1};
Physical Curve("CH2") = {2};
Physical Curve("CH3") = {3};
Physical Curve("bypass_pipe") = {4};

Physical Curve("DownComer_A") = {11};
Physical Curve("pipe1_HL_A") = {12};
Physical Curve("pipe2_HL_A") = {13};
Physical Curve("pipe1_CL_A") = {14};
Physical Curve("pipe2_CL_A") = {15};
Physical Curve("pipe1_SC_A") = {16};
Physical Curve("pipe2_SC_A") = {17};
Physical Curve("HX_A_primary") = {18};
Physical Curve("HX_A_secondary") = {19};
Physical Curve("pipe_to_Pressurizer") = {20};

Physical Curve("DownComer_B") = {31};
Physical Curve("pipe1_HL_B") = {32};
Physical Curve("pipe2_HL_B") = {33};
Physical Curve("pipe1_CL_B") = {34};
Physical Curve("pipe2_CL_B") = {35};
Physical Curve("pipe1_SC_B") = {36};
Physical Curve("pipe2_SC_B") = {37};
Physical Curve("HX_B_primary") = {38};
Physical Curve("HX_B_secondary") = {39};

// junctions (MERGE groups list every pipe end they connect)
Physical Point("LowerPlenum") = {12, 32, 1, 3, 5, 7};
Physical Point("UpperPlenum") = {2, 4, 6, 8, 13, 33};
Physical Point("Junction1_A") = {14};
Physical Point("Junction2_A") = {11};
Physical Point("Junction3_A") = {15};
Physical Point("Pump_A") = {18, 16};
Physical Point("Junction4_A") = {20, 23};
Physical Point("Junction5_A") = {24, 21};
Physical Point("Junction6_A") = {17};
Physical Point("Junction1_B") = {34};
Physical Point("Junction2_B") = {31};
Physical Point("Junction3_B") = {35};
Physical Point("Pump_B") = {38, 36};
Physical Point("Junction4_B") = {40, 43};
Physical Point("Junction5_B") = {44, 41};
Physical Point("Junction6_B") = {37};

// boundary conditions
Physical Point("MassFlowRateIn_SC_A") = {19};
Physical Point("PressureOutlet_SC_A") = {22};
Physical Point("MassFlowRateIn_SC_B") = {39};
Physical Point("PressureOutlet_SC_B") = {42};
Physical Point("Pressurizer") = {25};
