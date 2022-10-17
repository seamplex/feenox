Merge "maze-solved.msh";

Mesh.SurfaceFaces = 0;
Mesh.SurfaceEdges = 0;
General.SmallAxes = 0;

General.GraphicsHeight = 940;
General.GraphicsWidth = 1380;

View[0].ShowScale = 0;
View[0].RangeType = 2;
View[0].CustomMax = 0.008;
View[0].SaturateValues = 1;


View[1].ShowScale = 0;
View[1].CustomMax = 1e-2;
View[1].RangeType = 2;
View[1].GlyphLocation = 1; // Glyph (arrow, number, etc.) location (1: center of gravity, 2: node)
View[1].ArrowSizeMax = 20;

Print "maze-solved.png";
Exit;
