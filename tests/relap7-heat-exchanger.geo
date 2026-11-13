If (!Exists(N))
  N = 10;
EndIf
If (!Exists(Orientation))
  Orientation = 0;
EndIf
If (!Exists(SecondaryN))
  SecondaryN = N;
EndIf
Point(1) = {0, 0.02, 0};
Point(2) = {1, 0.02, 0};
Point(3) = {1, 0, 0};
Point(4) = {0, 0, 0};
If (Orientation)
  Line(1) = {2,1};
  Line(2) = {4,3};
Else
  Line(1) = {1,2};
  Line(2) = {3,4};
EndIf
Transfinite Curve{1} = N+1;
Transfinite Curve{2} = SecondaryN+1;
Physical Curve("primary") = {1};
Physical Curve("secondary") = {2};
Physical Point("primary_inlet") = {1};
Physical Point("primary_outlet") = {2};
Physical Point("secondary_inlet") = {3};
Physical Point("secondary_outlet") = {4};
