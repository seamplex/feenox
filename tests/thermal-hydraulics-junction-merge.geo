// Two pipes whose ends do not touch, joined by a MERGE junction.
If (!Exists(Gap))
  Gap = 0.5;
EndIf
Point(1) = {0, 0, 0};
Point(2) = {1, 0, 0};
Point(3) = {1+Gap, 0, 0};
Point(4) = {2+Gap, 0, 0};
Line(1) = {1, 2};
Line(2) = {3, 4};
Transfinite Curve{1,2} = 21;
Physical Point("inlet") = {1};
Physical Point("outlet") = {4};
Physical Point("connection") = {2, 3};
Physical Curve("pipe1") = {1};
Physical Curve("pipe2") = {2};
