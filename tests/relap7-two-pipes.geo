If (!Exists(ReverseFlow))
  ReverseFlow = 0;
EndIf
If (!Exists(Orientation))
  Orientation = 0;
EndIf
Point(1) = {0, 0, 0};
Point(2) = {1, 0, 0};
Point(3) = {2, 0, 0};
If (Orientation)
  Line(1) = {2, 1};
  Line(2) = {3, 2};
Else
  Line(1) = {1, 2};
  Line(2) = {2, 3};
EndIf
Transfinite Curve{1,2} = 21;
If (ReverseFlow)
  Physical Point("inlet") = {3};
  Physical Point("outlet") = {1};
Else
  Physical Point("inlet") = {1};
  Physical Point("outlet") = {3};
EndIf
Physical Point("connection") = {2};
Physical Curve("pipe1") = {1};
Physical Curve("pipe2") = {2};
