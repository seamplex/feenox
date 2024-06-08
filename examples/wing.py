import math
import cmath
import gmsh

gmsh.initialize()

# size of the domain [-a,+a]x[-b,+b]
a = 20
b = 10

# characteristic element sizes
lc0 = 0.5       # near the boundary
lc = 0.1*lc0    # near the wing

# number of points of definition
nperf = 50
# attack angle
alpha = 10 * math.pi/180
# Joukowsky parameters
x0 = -0.11
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

gmsh.model.occ.synchronize()
gmsh.write("wing.brep")

# physical groups
gmsh.model.addPhysicalGroup(1, [bottom], name="bottom")
gmsh.model.addPhysicalGroup(1, [right],  name="right")
gmsh.model.addPhysicalGroup(1, [top],    name="top")
gmsh.model.addPhysicalGroup(1, [left],   name="left")
gmsh.model.addPhysicalGroup(1, [wing],   name="wing")
gmsh.model.addPhysicalGroup(2, [air],    name="air")

# named points to compute the locations to evaluate the Kutta condition
gmsh.model.addPhysicalGroup(0, [1],     name="tip")
gmsh.model.addPhysicalGroup(0, [2],     name="first")
gmsh.model.addPhysicalGroup(0, [nperf], name="last")



# boundary layer
f = gmsh.model.mesh.field.add("BoundaryLayer")
gmsh.model.mesh.field.setNumbers(f, "CurvesList", spl)
gmsh.model.mesh.field.setNumber(f, "Size", 0.1*lc)
gmsh.model.mesh.field.setNumber(f, "Ratio", 2)
gmsh.model.mesh.field.setNumber(f, "Quads", 1)
gmsh.model.mesh.field.setNumber(f, "Thickness", lc)

gmsh.option.setNumber("Mesh.BoundaryLayerFanElements", 7)
gmsh.model.mesh.field.setNumbers(f, "FanPointsList", [1])
gmsh.model.mesh.field.setAsBoundaryLayer(f)

# mesh
gmsh.option.setNumber("Mesh.RecombineAll", 1)
gmsh.option.setNumber("Mesh.Algorithm", 8)
gmsh.model.mesh.generate(2)
gmsh.model.mesh.optimize("Netgen")
gmsh.model.mesh.optimize("Laplace2D")
gmsh.model.mesh.setOrder(2)
gmsh.model.mesh.optimize("HighOrderFastCurving")
gmsh.model.mesh.optimize("HighOrder")
gmsh.write("wing.msh")

gmsh.fltk.run()
gmsh.finalize()

