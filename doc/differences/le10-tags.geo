Merge "le10.step";
Merge "le10-viewpoint.geo";

Geometry.Color.Points = {0,0,0};
Geometry.Color.Surfaces = {37,176,76};

Geometry.Points = 1;
Geometry.Curves = 1;
Geometry.Surfaces = 1; // Display geometry surfaces?

Geometry.PointLabels = 1;
Geometry.CurveLabels = 1; // Display curve labels?
Geometry.SurfaceLabels = 1; // Display surface labels?

Geometry.LabelType = 0; // Type of entity label (0: description, 1: elementary entity tag, 2: physical group tag, 3: elementary name, 4: physical name)

General.GraphicsFontEngine = "Cairo"; // Set graphics font engine (Native, StringTexture, Cairo)
General.GraphicsFont = "Helvetica"; // Font used in the graphic window
General.GraphicsFontSize = 32; // Size of the font in the graphic window, in pixels


Print "le10-tags.svg";
// Print "le10-tags.png";
// Exit;
