import gmsh
import sys

dt = 0.01

gmsh.initialize(sys.argv)
gmsh.merge("temp-cylinder.msh")

gmsh.option.setNumber("General.TrackballQuaternion0", -0.2152548072727626)
gmsh.option.setNumber("General.TrackballQuaternion1", -0.2112165920022461)
gmsh.option.setNumber("General.TrackballQuaternion2", -0.01270861835676978)
gmsh.option.setNumber("General.TrackballQuaternion3",  0.953357965419278)

gmsh.option.setNumber("General.GraphicsWidth", 1920);
gmsh.option.setNumber("General.GraphicsHeight", 1080);


gmsh.option.setNumber("Mesh.SurfaceEdges", 0)
gmsh.option.setNumber("Mesh.SurfaceFaces", 0)
gmsh.option.setNumber("Mesh.VolumeEdges", 0)
gmsh.option.setNumber("Mesh.VolumeFaces", 0)


n_steps = int(gmsh.option.getNumber("View[0].NbTimeStep"))
times = []
temps = []

for i in range(n_steps):
  kind, tags, temp, t, _ = gmsh.view.getModelData(0, i)
  temps.append(temp)
  times.append(t)

end_time = t
temp_inst = [0] * len(temp)
view_inst = gmsh.view.add("temp_temp") # as in temperature and temporal

t = 0
step = 0
i = 1
while t < end_time:
  if t > times[i]:
    while times[i] < t:  
      i += 1
  alpha = (t-times[i-1])/(times[i]-times[i-1])  
  print(t,i,alpha)
  
  for j in range(len(temps[i])):
    temp_inst[j] = [temps[i-1][j][0] + alpha * (temps[i][j][0] - temps[i-1][j][0])]

  gmsh.view.addModelData(view_inst, step, "", kind, tags, temp_inst, t)
  
  step += 1
  t += dt

gmsh.view.remove(0)
gmsh.fltk.initialize()

for i in range(step):
  print(i)  
  gmsh.option.setNumber("View[0].TimeStep", i)
  gmsh.fltk.update()
  gmsh.write("temp-cylinder-smooth-%03d.png" % i)


gmsh.finalize()
print("all frames dumped, now run")
print("ffmpeg -framerate 20 -f image2 -i temp-cylinder-smooth-%03d.png temp-cylinder-smooth.mp4")
print("to get a video")


