If (!Exists(Topology))
  Topology = 0;
EndIf
Point(1) = {0, 0, 0};
Point(2) = {1, 0, 0};
Point(3) = {2, 0, 0};
Line(1) = {1, 2};
Line(2) = {2, 3};
Transfinite Curve{1,2} = 21;
Physical Curve("pipe1") = {1,2};
Physical Point("inlet") = {1};
If (Topology == 1)
  Point(4) = {3, 0, 0};
  Line(3) = {3,4};
  Transfinite Curve{3} = 21;
  Physical Curve("pipe2") = {3};
  Physical Point("connection") = {2,3};
  Physical Point("outlet") = {4};
Else
  Physical Point("connection") = {2};
  Physical Point("outlet") = {3};
EndIf
If (Topology == 2)
  Point(4) = {1, 1, 0};
  Line(3) = {2,4};
  Transfinite Curve{3} = 21;
  Physical Curve("pipe2") = {3};
EndIf
