import gmsh
import sys

# time step, i.e. one frame every dt seconds
dt = 1

# argument like $1
if (len(sys.argv) < 2):
  n = 1
else:
  n = int(sys.argv[1])
  
# initialize Gmsh
gmsh.initialize(sys.argv)

# read the results written by FeenoX
gmsh.merge("valve-%d.msh" % n)

# set some view options
gmsh.option.setNumber("General.Trackball", 0);
gmsh.option.setNumber("General.RotationX", 290)
gmsh.option.setNumber("General.RotationY", 2)
gmsh.option.setNumber("General.RotationZ", 25)

gmsh.option.setNumber("General.ScaleX", 1.3)
gmsh.option.setNumber("General.ScaleY", 1.3)
gmsh.option.setNumber("General.ScaleZ", 1.3)

gmsh.option.setNumber("Mesh.SurfaceEdges", 0)
gmsh.option.setNumber("Mesh.SurfaceFaces", 0)
gmsh.option.setNumber("Mesh.VolumeFaces", 0)
gmsh.option.setNumber("Mesh.VolumeEdges", 0)

# read original fields
n_steps = int(gmsh.option.getNumber("View[0].NbTimeStep"))
times = []
temps = []
fluxes = []

view_tag_temp = gmsh.view.getTags()[0]
view_tag_flux = gmsh.view.getTags()[1]

for step in range(n_steps):
  print(step)
  kind_temp, tags_temp, temp, t, n_components = gmsh.view.getModelData(view_tag_temp, step)
  temps.append(temp)
  kind_flux, tags_flux, flux, t, n_components = gmsh.view.getModelData(view_tag_flux, step)
  fluxes.append(flux)
  times.append(t)

end_time = t
inst_temp = [0] * len(temp)
view_inst_temp = gmsh.view.add("Temperature transient #%d" % n)

inst_flux = [[0,0,0]] * len(flux)
view_inst_flux = gmsh.view.add("Heat flux transient #%d" % n)

# interpolate the non-constant dt data set to a fixed dt set
t = 0
i = 1
step = 0
while t < end_time:
  if t > times[i]:
    while times[i] < t:  
      i += 1
  alpha = (t-times[i-1])/(times[i]-times[i-1])  
  print(t,i,alpha)
  
  for j in range(len(temps[i])):
    inst_temp[j] = [temps[i-1][j][0] + alpha * (temps[i][j][0] - temps[i-1][j][0])]
  for j in range(len(fluxes[i])):
    inst_flux[j] = [fluxes[i-1][j][0] + alpha * (fluxes[i][j][0] - fluxes[i-1][j][0]),
                    fluxes[i-1][j][1] + alpha * (fluxes[i][j][1] - fluxes[i-1][j][1]),
                    fluxes[i-1][j][2] + alpha * (fluxes[i][j][2] - fluxes[i-1][j][2])]

  gmsh.view.addModelData(view_inst_temp, step, "", kind_temp, tags_temp, inst_temp, t)
  gmsh.view.addModelData(view_inst_flux, step, "", kind_flux, tags_flux, inst_flux, t)
  
  step += 1
  t += dt

# remove the original fields
gmsh.view.remove(view_tag_temp)
gmsh.view.remove(view_tag_flux)

# initialize the graphical interface
gmsh.fltk.initialize()

# dump each interpolated frame
for i in range(step):
  print(i)  
  gmsh.option.setNumber("View[0].TimeStep", i)
  gmsh.option.setNumber("View[1].TimeStep", i)
  gmsh.fltk.update()
  gmsh.write("valve-temp-%d-%04d.png" % (n,i))

# finalize
gmsh.finalize()

# show instructions to create a video
print("all frames dumped, now run")
print("ffmpeg -y -framerate 10 -f image2 -i valve-temp-%d-%%04d.png valve-temp-%d.mp4" % (n, n))
print("to get a video")


