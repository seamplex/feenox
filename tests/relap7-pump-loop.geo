If (!Exists(Orientation))
  Orientation = 0;
EndIf
Point(1) = {0, 0, 0};
Point(2) = {1, 0, 0};
Point(3) = {1, 1, 0};
Point(4) = {0, 1, 0};
Point(5) = {1, 1.5, 0};
If (Orientation)
  Line(1) = {2, 1};
  Line(2) = {3, 2};
  Line(3) = {4, 3};
  Line(4) = {1, 4};
  Line(5) = {5, 3};
Else
  Line(1) = {1, 2};
  Line(2) = {2, 3};
  Line(3) = {3, 4};
  Line(4) = {4, 1};
  Line(5) = {3, 5};
EndIf
Transfinite Curve{1:5} = 21;
Physical Curve("pipe1") = {1};
Physical Curve("pipe2") = {2};
Physical Curve("pipe3") = {3};
Physical Curve("pipe4") = {4};
Physical Curve("pipe5") = {5};
Physical Point("pump") = {2};
Physical Point("junction2") = {3};
Physical Point("junction3") = {4};
Physical Point("junction4") = {1};
Physical Point("outlet") = {5};
