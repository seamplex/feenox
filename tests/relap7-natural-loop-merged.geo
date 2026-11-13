// RELAP-7 Example 10 with the input-file coordinates: co-current secondary
// whose inlet is 3.7 m away from the end of pipe6, joined with JUNCTION MERGE.
// MergeOrder selects which end is the representative of the merged junction.
If (!Exists(MergeOrder))
  MergeOrder = 0;
EndIf
Point(1) = {0,1,0};
Point(2) = {0,0,0};
Point(3) = {0,0,0.8};
Point(4) = {0,0,5.98};
Point(5) = {0,1,5.98};
Point(6) = {0,1,2.27};
Point(7) = {0,0,6};
Point(8) = {0,1.5,2.27};
Point(11) = {0,1.2044,2.27};
If (MergeOrder)
  Point(9) = {0,1.2,2.27};      // end of pipe6 first
  Point(10) = {0,1.2044,5.98};  // start of secondary
  Line(8) = {8,9};
  Line(9) = {10,11};
Else
  Point(9) = {0,1.2044,5.98};   // start of secondary first
  Point(10) = {0,1.2,2.27};     // end of pipe6
  Line(8) = {8,10};
  Line(9) = {9,11};
EndIf
Line(1) = {1,2};
Line(2) = {2,3};
Line(3) = {3,4};
Line(4) = {4,5};
Line(5) = {5,6};
Line(6) = {6,1};
Line(7) = {4,7};
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
Physical Point("junction7") = {9,10};
Physical Point("pressure_control") = {7};
Physical Point("inlet") = {8};
Physical Point("outlet") = {11};
