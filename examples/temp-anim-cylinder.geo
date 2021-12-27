Merge "temp-cylinder.msh";

General.TrackballQuaternion0 = -0.2152548072727626;
General.TrackballQuaternion1 = -0.2112165920022461;
General.TrackballQuaternion2 = -0.01270861835676978;
General.TrackballQuaternion3 = 0.953357965419278;

Mesh.SurfaceFaces = 0;
Mesh.SurfaceEdges = 0;
Mesh.VolumeFaces = 0;
Mesh.VolumeEdges = 0;

View[0].TimeStep = View[0].NbTimeStep - 1;
end_time = View[0].Time;

dt = 0.01;
For time In {0:end_time:dt}
  View[0].Time = time;
  Print Sprintf("temp-cylinder-rough-%03g.png", time/dt);
  Draw;
EndFor

General.Terminal = 1;
Printf("all frames dumped, now run");
Printf("ffmpeg -y -framerate 20 -f image2 -i temp-cylinder-rough-%%03d.png temp-cylinder-rough.mp4");
Printf("to get a video");
Exit;
