If (!Exists(Orientation))
  Orientation = 0;
EndIf
Point(1) = {0,1,0};
Point(2) = {0,0,0};
Point(3) = {0,0,0.8};
Point(4) = {0,0,5.98};
Point(5) = {0,1,5.98};
Point(6) = {0,1,2.27};
Point(7) = {0,0,6};
Point(8) = {0,1.5,2.27};
Point(9) = {0,1.2,2.27};
Point(10) = {0,1.2,5.98};
If (Orientation)
  Line(1) = {2,1};
  Line(2) = {3,2};
  Line(3) = {4,3};
  Line(4) = {5,4};
  Line(5) = {6,5};
  Line(6) = {1,6};
  Line(7) = {7,4};
  Line(8) = {9,8};
  Line(9) = {10,9};
Else
  Line(1) = {1,2};
  Line(2) = {2,3};
  Line(3) = {3,4};
  Line(4) = {4,5};
  Line(5) = {5,6};
  Line(6) = {6,1};
  Line(7) = {4,7};
  Line(8) = {8,9};
  // Counterflow secondary, following the schematic rather than the inconsistent coordinates.
  Line(9) = {9,10};
EndIf
Transfinite Curve{1,2,4} = 21;
Transfinite Curve{3,5,6,9} = 51;
Transfinite Curve{7} = 11;
Transfinite Curve{8} = 6;
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
Physical Point("junction7") = {9};
Physical Point("pressure_control") = {7};
Physical Point("inlet") = {8};
Physical Point("outlet") = {10};
