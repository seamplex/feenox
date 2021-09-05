Merge "temp-slab.msh";

View[0].Type = 4;
View[0].Axes = 1;
View[0].IntervalsType = 3; // to see the points

For time In {0:0.1:1e-3}
  View[0].Time = time;
  Sleep 0.1;
  Draw;
EndFor
