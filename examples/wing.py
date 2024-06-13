import sys
import math
import cmath
import gmsh

gmsh.initialize()

# size of the domain [-a,+a]x[-b,+b]
a = 20
b = 10

# characteristic element sizes
lc0 = 0.6      # near the boundary
lc = 0.1*lc0    # near the wing

# number of points of definition
nperf = 50
# attack angle
alpha = (float(sys.argv[1]) if len(sys.argv) > 1 else 10) * math.pi/180

# Joukowsky parameters
x0 = (float(sys.argv[2]) if len(sys.argv) > 2 else -0.11)
y0 = 0
r = math.sqrt((x0-1)**2+y0**2)

# append points
pts = []
coords = []
for i in range(nperf):
  theta = i * 2*math.pi/nperf
  dz = complex(x0 + r*math.cos(theta), y0 + r*math.sin(theta))
  z = (dz + 1/dz)*cmath.exp(complex(0, -alpha))
  # remember the coords of these three points
  if i == 0 or i == 1 or i == nperf-1:
    coords.append([z.real, z.imag])
  pts.append(gmsh.model.occ.addPoint(z.real, z.imag, 0, lc))

# create a closed loop of points
pts.append(pts[0])

# add a spline
spl = []
spl.append(gmsh.model.occ.addSpline(pts))
wing = gmsh.model.occ.addCurveLoop(spl)

# compute the location where to evaluate the Kutta condition 
# this is ugly and not pythonic at all!
vsup = [-(coords[1][0] - coords[0][0]),
        -(coords[1][1] - coords[0][1])]  
norm = math.sqrt(vsup[0]**2 + vsup[1]**2)
vsup[0] /= norm
vsup[1] /= norm

vinf = [-(coords[0][0] - coords[2][0]),
        -(coords[0][1] - coords[2][1])] 
norm = math.sqrt(vinf[0]**2 + vinf[1]**2)
vinf[0] /= norm
vinf[1] /= norm

vmed = vsup + vinf
norm = math.sqrt(vmed[0]**2 + vmed[1]**2)
vmed[0] /= norm
vmed[1] /= norm

eps = 0.04
p = [coords[0][0] + eps * vmed[0],
     coords[0][1] + eps * vmed[1]]
# this is just to show the point in case one opens the .brep
kutta = gmsh.model.occ.addPoint(p[0], p[1], 0, lc)

# f = open("p.fee", "w")
# f.write( "VECTOR p[2] DATA %g %g\n" % (p[0], p[1]))
# f.close()


# add the external boundary
p1 = gmsh.model.occ.addPoint(-a, -b, 0, lc0)
p2 = gmsh.model.occ.addPoint(+a, -b, 0, lc0)
p3 = gmsh.model.occ.addPoint(+a, +b, 0, lc0)
p4 = gmsh.model.occ.addPoint(-a, +b, 0, lc0)
bottom   = gmsh.model.occ.addLine(p1, p2)
right    = gmsh.model.occ.addLine(p2, p3)
top      = gmsh.model.occ.addLine(p3, p4)
left     = gmsh.model.occ.addLine(p4, p1)
boundary = gmsh.model.occ.addCurveLoop([bottom, right, top, left])
air      = gmsh.model.occ.addPlaneSurface([boundary, wing])

# create a circle to compute the circulation
r_circle = 3
circle = gmsh.model.occ.addCircle(0, 0, 0, r_circle)


# we could embed the kutta point into the mesh but
# then the geometrical entities 
ov, ovv = gmsh.model.occ.fragment([(2, air)], [(0, kutta), (1,circle)])
print(ov)
print(ovv)

gmsh.model.occ.synchronize()

# physical groups
# after the boolean fragment we lost the ids
# gmsh.model.addPhysicalGroup(1, [bottom], name="bottom")
# gmsh.model.addPhysicalGroup(1, [right],  name="right")
# gmsh.model.addPhysicalGroup(1, [top],    name="top")
# gmsh.model.addPhysicalGroup(1, [left],   name="left")
# gmsh.model.addPhysicalGroup(1, [wing],   name="wing")
# gmsh.model.addPhysicalGroup(2, [air],    name="air")

gmsh.model.addPhysicalGroup(0, [kutta], name="kutta")
gmsh.model.addPhysicalGroup(1, [6],     name="circle")
gmsh.model.addPhysicalGroup(1, [7],     name="bottom")
gmsh.model.addPhysicalGroup(1, [9],     name="right")
gmsh.model.addPhysicalGroup(1, [10],    name="top")
gmsh.model.addPhysicalGroup(1, [8],     name="left")
gmsh.model.addPhysicalGroup(1, [11],    name="wing")
gmsh.model.addPhysicalGroup(2, [1,2],   name="air")

# dump the brep (or xao) just in case
gmsh.write("wing.brep")
gmsh.write("wing.xao")


# mesh
gmsh.option.setNumber("Mesh.RecombineAll", 1)
gmsh.option.setNumber("Mesh.MeshSizeFromCurvature", 2*math.pi * 0.25 * r_circle/lc)
gmsh.option.setNumber("Mesh.Algorithm", 6)
gmsh.model.mesh.setAlgorithm(2, 1, 8);
gmsh.model.mesh.setAlgorithm(2, 2, 6);
gmsh.option.setNumber("Mesh.Smoothing", 10)

gmsh.model.mesh.generate(2)
gmsh.model.mesh.optimize("Netgen")
gmsh.model.mesh.optimize("Laplace2D")
gmsh.model.mesh.setOrder(2)
gmsh.model.mesh.optimize("HighOrderFastCurving")
gmsh.model.mesh.optimize("HighOrder")
gmsh.write("wing.msh")

# gmsh.fltk.run()
gmsh.finalize()

