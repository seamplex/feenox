// RELAP-7 example 7: five horizontal pipes of 1 m each, 50 elements each,
// with the same start positions as in the RELAP-7 input
// pipes 1 and 2 flow into the junction, pipes 3, 4 and 5 flow out of it
// none of the pipe ends touch: they are joined by topology with MERGE
For i In {1:5}
  x0[] = {0, 0,   1.2, 1.2, 1.2};
  y0[] = {0, 0.3, 0,   0.5, 1.0};
  Point(2*i-1) = {x0[i-1],   y0[i-1], 0};
  Point(2*i)   = {x0[i-1]+1, y0[i-1], 0};
  Line(i) = {2*i-1, 2*i};
  Transfinite Curve{i} = 50+1;
  Physical Curve(Sprintf("pipe%g", i)) = {i};
EndFor

Physical Point("inlet1") = {1};
Physical Point("inlet2") = {3};
Physical Point("outlet3") = {6};
Physical Point("outlet4") = {8};
Physical Point("outlet5") = {10};

// the outlets of pipes 1 & 2 and the inlets of pipes 3, 4 & 5
Physical Point("junction") = {2, 4, 5, 7, 9};
