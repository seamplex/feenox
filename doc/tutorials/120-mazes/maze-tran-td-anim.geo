Merge "maze-tran-td.msh";

Mesh.SurfaceFaces = 0;
Mesh.SurfaceEdges = 0;
General.SmallAxes = 0;

General.GraphicsHeight = 1024;
General.GraphicsWidth = General.MenuWidth + 1024;

View[0].Visible = 0;

View[1].ShowScale = 0;
View[1].RangeType = 2;
View[1].CustomMax = 0.01;
View[1].SaturateValues = 1;


View[2].ShowScale = 0;
View[2].CustomMax = 0.005;
View[2].RangeType = 2;
View[2].GlyphLocation = 1; // Glyph (arrow, number, etc.) location (1: center of gravity, 2: node)
View[2].ArrowSizeMax = 20;

For step In {0:View[0].NbTimeStep-1}
  View[1].TimeStep = step;
  View[2].TimeStep = step;
  Print Sprintf("maze-tran-td-%03g.png", step);
  Draw;
EndFor

General.Terminal = 1;
Printf("# all frames dumped, now run");
Printf("ffmpeg -y -framerate 10 -f image2 -i maze-tran-td-%%03d.png maze-tran-td.mp4");
Printf("ffmpeg -y -framerate 10 -f image2 -i maze-tran-td-%%03d.png maze-tran-td.gif");
Exit;
