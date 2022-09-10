Merge "maze-tran-bu.msh";

Mesh.SurfaceFaces = 0;
Mesh.SurfaceEdges = 0;
General.SmallAxes = 0;

General.GraphicsHeight = 940;
General.GraphicsWidth = 1380;

View[0].Visible = 0;

View[1].ShowScale = 0;
View[1].RangeType = 2;
View[1].CustomMax = 0.0006;
View[1].SaturateValues = 1;


View[2].ShowScale = 0;
View[2].CustomMax = 0.0004;
View[2].RangeType = 2;
View[2].GlyphLocation = 1; // Glyph (arrow, number, etc.) location (1: center of gravity, 2: node)
View[2].ArrowSizeMax = 20;

For step In {0:View[0].NbTimeStep-1}
  View[1].TimeStep = step;
  View[2].TimeStep = step;
  Print Sprintf("maze-tran-bu-%03g.png", step);
  Draw;
EndFor

General.Terminal = 1;
Printf("# all frames dumped, now run");
Printf("ffmpeg -y -framerate 20 -f image2 -i maze-tran-bu-%%03d.png maze-tran-bu.mp4");
Printf("ffmpeg -y -framerate 20 -f image2 -i maze-tran-bu-%%03d.png maze-tran-bu.gif");
Exit;
