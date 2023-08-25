Merge "azmy-4.msh";

Mesh.SurfaceFaces = 0;
Mesh.VolumeFaces = 0;
Mesh.SurfaceEdges = 0;
Mesh.VolumeEdges = 0;

General.TrackballQuaternion0 = 0.25;
General.TrackballQuaternion1 = 0.38;
General.TrackballQuaternion2 = 0.70;
General.TrackballQuaternion3 = 0.56;

General.ScaleX = 1.3;
General.ScaleY = 1.3;
General.ScaleZ = 1.3;

For i In {0 : 12}
  For j In {0 : 12}
    View[j].Visible = 0;
  EndFor 

  If ( i == 0 )
    View[i].ColormapNumber = 2;
  Else
    View[i].ColormapNumber = 12;
  EndIf
  
  View[i].RaiseZ = 1;
  View[i].Visible = 1;
  Print Sprintf("azmy-4-%02g.png", i);
EndFor
