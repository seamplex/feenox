import math
import gmsh
import subprocess

def create_mesh(r, w, l1, l2, n):
  gmsh.initialize()
  gmsh.option.setNumber("General.Terminal", 0)  
  
  gmsh.model.add("fork")
  lc = 0
  
  gmsh.model.occ.addPoint(0,      0, 0, lc, 1)  
  gmsh.model.occ.addPoint(-r,     0, 0, lc, 2)  
  gmsh.model.occ.addPoint(-(r+w), 0, 0, lc, 3)  
  gmsh.model.occ.addPoint(+r,     0, 0, lc, 4)  
  gmsh.model.occ.addPoint(+(r+w), 0, 0, lc, 5)  
  
  gmsh.model.occ.addPoint(-(r+w), +l1, 0, lc, 6)
  gmsh.model.occ.addPoint(+(r+w), +l1, 0, lc, 7)

  gmsh.model.occ.addPoint(-r,     +l1, 0, lc, 8)
  gmsh.model.occ.addPoint(+r,     +l1, 0, lc, 9)

  x1 = w/2
  y1 = math.sqrt((r+w)*(r+w) - (w/2)*(w/2))
  a = x1/y1
  y2 = math.sqrt(r*r/(1+a*a))
  x2 = a*y2
  
  gmsh.model.occ.addPoint(-x1, -y1, lc, 10)
  gmsh.model.occ.addPoint(-x2, -y2, lc, 11)
  gmsh.model.occ.addPoint(+x1, -y1, lc, 12)
  gmsh.model.occ.addPoint(+x2, -y2, lc, 13)

  gmsh.model.occ.addPoint(-w/2, -l2, lc, 14)
  gmsh.model.occ.addPoint(+w/2, -l2, lc, 15)  

  gmsh.model.occ.addLine(14, 10, 1)
  gmsh.model.occ.addLine(10, 11, 2)
  gmsh.model.occ.addLine(14, 15, 3)
  gmsh.model.occ.addLine(15, 12, 4)
  gmsh.model.occ.addLine(12, 13, 5)
  gmsh.model.occ.addLine(3,  6,  6)
  gmsh.model.occ.addLine(6,  8,  7)
  gmsh.model.occ.addLine(8,  2,  8)
  gmsh.model.occ.addLine(2,  3,  9)
  gmsh.model.occ.addLine(4,  9,  10)
  gmsh.model.occ.addLine(9,  7,  11)
  gmsh.model.occ.addLine(7,  5,  12)
  gmsh.model.occ.addLine(5,  4,  13)

  gmsh.model.occ.addCircleArc(2,  1, 11, 14)
  gmsh.model.occ.addCircleArc(11, 1, 13, 15)
  gmsh.model.occ.addCircleArc(13, 1, 4 , 16)
  gmsh.model.occ.addCircleArc(3,  1, 10, 17)
  gmsh.model.occ.addCircleArc(10, 1, 12, 18)
  gmsh.model.occ.addCircleArc(12, 1, 5 , 19)

  gmsh.model.occ.addCurveLoop([3,  4,  -18, -1], 1)
  gmsh.model.occ.addCurveLoop([18, 5,  -15, -2], 2)
  gmsh.model.occ.addCurveLoop([19, 13, -16, -5], 3)
  gmsh.model.occ.addCurveLoop([14, -2, -17, -9], 4)
  gmsh.model.occ.addCurveLoop([13, 10, 11,  12], 5)
  gmsh.model.occ.addCurveLoop([8,  9,  6,   7],  6)

  gmsh.model.occ.addPlaneSurface([1], 1)
  gmsh.model.occ.addPlaneSurface([2], 2)
  gmsh.model.occ.addPlaneSurface([3], 3)
  gmsh.model.occ.addPlaneSurface([4], 4)
  gmsh.model.occ.addPlaneSurface([5], 5)
  gmsh.model.occ.addPlaneSurface([6], 6)

  gmsh.model.occ.synchronize()

  gmsh.model.mesh.setTransfiniteCurve(3,  n+1)
  gmsh.model.mesh.setTransfiniteCurve(18, n+1)
  gmsh.model.mesh.setTransfiniteCurve(15, n+1)
  gmsh.model.mesh.setTransfiniteCurve(5,  n+1)
  gmsh.model.mesh.setTransfiniteCurve(2,  n+1)
  gmsh.model.mesh.setTransfiniteCurve(9,  n+1)
  gmsh.model.mesh.setTransfiniteCurve(13, n+1)
  gmsh.model.mesh.setTransfiniteCurve(11, n+1)
  gmsh.model.mesh.setTransfiniteCurve(7,  n+1)

  gmsh.model.mesh.setTransfiniteCurve(6,  12*n+1)
  gmsh.model.mesh.setTransfiniteCurve(8,  12*n+1)
  gmsh.model.mesh.setTransfiniteCurve(10, 12*n+1)
  gmsh.model.mesh.setTransfiniteCurve(12, 12*n+1)
  
  gmsh.model.mesh.setTransfiniteCurve(14, 3*n+1)
  gmsh.model.mesh.setTransfiniteCurve(17, 3*n+1)
  gmsh.model.mesh.setTransfiniteCurve(16, 3*n+1)
  gmsh.model.mesh.setTransfiniteCurve(19, 3*n+1)
  
  gmsh.model.mesh.setTransfiniteCurve(1,  8*n+1)
  gmsh.model.mesh.setTransfiniteCurve(4,  8*n+1)
  
  gmsh.model.mesh.setTransfiniteSurface(1);
  gmsh.model.mesh.setTransfiniteSurface(2);
  gmsh.model.mesh.setTransfiniteSurface(3);
  gmsh.model.mesh.setTransfiniteSurface(4);
  gmsh.model.mesh.setTransfiniteSurface(5);
  gmsh.model.mesh.setTransfiniteSurface(6);

  gmsh.model.occ.extrude([(2,1),(2,2),(2,3),(2,4),(2,5),(2,6)], 0, 0, w, [int(n)], [], True)

  gmsh.model.occ.synchronize()
  
  gmsh.model.addPhysicalGroup(3, [1, 2, 3, 4, 5, 6], 1)  
  gmsh.model.setPhysicalName(3, 1, "bulk")

  gmsh.model.addPhysicalGroup(2, [7], 2)  
  gmsh.model.setPhysicalName(2, 2, "base")
  
  gmsh.model.occ.synchronize()
    
  gmsh.option.setNumber("Mesh.RecombineAll", 1)
  gmsh.option.setNumber("Mesh.ElementOrder", 2)
  gmsh.model.mesh.generate(3)
  nodes, coord, parametric_coord  = gmsh.model.mesh.getNodes()
  gmsh.write("fork.msh")  

  gmsh.model.remove()
  #gmsh.fltk.run()
  
  gmsh.finalize()
  return len(nodes)


def main():
  target = 440    # target frequency
  eps = 1e-2      # tolerance
  r = 4.2e-3      # geometric parameters
  w = 3e-3
  l1 = 60e-3
  l2 = 30e-3

  for n in range(1,7):   # mesh refinement level
    l1 = 60e-3    # restart l1 & error
    error = 60
    while abs(error) > eps:
      l1 = l1 - 1e-4*error
      nodes = create_mesh(r, w, l1, l2, n)
      result = subprocess.run(['feenox', 'fork.fee'], stdout=subprocess.PIPE)
      freq = float(result.stdout.decode('utf-8'))
      error = target - freq
      print("#", nodes, l1, freq)
    
    print(nodes,l1,freq)


if __name__ == "__main__":
  main()
  
