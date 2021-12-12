Merge "two-squares-results.msh";

General.ScaleX = 1.155961787065194; // X-axis scale factor
General.ScaleY = 1.155961787065194; // Y-axis scale factor
General.ScaleZ = 1.155961787065194; // Z-axis scale factor
General.TrackballQuaternion0 = 0.5489433321306485; // First trackball quaternion component (used if General.Trackball=1)
General.TrackballQuaternion1 = 0.0601802035389059; // Second trackball quaternion component (used if General.Trackball=1)
General.TrackballQuaternion2 = 0.1290769871187583; // Third trackball quaternion component (used if General.Trackball=1)
General.TrackballQuaternion3 = 0.823637476446805; // Fourth trackball quaternion component (used if General.Trackball=1)
General.TranslationX = 0.04103614376566234; // X-axis translation (in model units)
General.TranslationY = -0.1655361794192602; // Y-axis translation (in model units)

View[0].NormalRaise = 0.2; // Elevation of the view along the normal (in model coordinates)
View[0].Visible = 1; // Is the view visible?


View[1].ColormapNumber = 11; // Default colormap number (0: black, 1: vis5d, 2: jet, 3: lucie, 4: rainbow, 5: emc2000, 6: incadescent, 7: hot, 8: pink, 9: grayscale, 10: french, 11: hsv, 12: spectrum, 13: bone, 14: spring, 15: summer, 16: autumm, 17: winter, 18: cool, 19: copper, 20: magma, 21: inferno, 22: plasma, 23: viridis, 24: turbo)
View[1].Light = 0; // Enable lighting for the view
View[1].NormalRaise = 0.2; // Elevation of the view along the normal (in model coordinates)


View[0].Visible = 1; // Is the view visible?
View[1].Visible = 0; // Is the view visible?
Print "two-squares-temperature.png";

View[0].Visible = 0; // Is the view visible?
View[1].Visible = 1; // Is the view visible?
Print "two-squares-conductivity.png";
