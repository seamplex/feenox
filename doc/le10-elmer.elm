Header
  CHECK KEYWORDS Warn
  Mesh DB "." "."				# Path to the mesh
  Include Path ""
  Results Directory ""				# Path to results directory
End

Simulation					# Settings and constants for simulation
  Max Output Level = 5
  Coordinate System = Cartesian
  Coordinate Mapping(3) = 1 2 3
  Simulation Type = Steady state
  Steady State Max Iterations = 1
  Output Intervals = 1
  Timestepping Method = BDF
  BDF Order = 1
  Solver Input File = case.sif
  Post File = case.vtu
End

Constants
  Gravity(4) = 0 -1 0 9.82
  Stefan Boltzmann = 5.67e-08
  Permittivity of Vacuum = 8.8542e-12
  Boltzmann Constant = 1.3807e-23
  Unit Charge = 1.602e-19
End

Body 1						# Assigning the material and equations to the mesh
  Target Bodies(1) = 10
  Name = "Body Property 1"
  Equation = 1
  Material = 1
End

Solver 2					# Solver settings
  Equation = Linear elasticity
  Procedure = "StressSolve" "StressSolver"
  Calculate Stresses = True
  Variable = -dofs 2 Displacement
  Exec Solver = Always
  Stabilize = True
  Bubbles = False
  Lumped Mass Matrix = False
  Optimize Bandwidth = True
  Steady State Convergence Tolerance = 1.0e-5
  Nonlinear System Convergence Tolerance = 1.0e-7
  Nonlinear System Max Iterations = 20
  Nonlinear System Newton After Iterations = 3
  Nonlinear System Newton After Tolerance = 1.0e-3
  Nonlinear System Relaxation Factor = 1
  Linear System Solver = Direct
  Linear System Direct Method = Umfpack
End

Solver 1					# Saving the results from node at point D
  Equation = SaveScalars
  Save Points = 26
  Procedure = "SaveData" "SaveScalars"
  Filename = file.dat
  Exec Solver = After Simulation
End

Equation 1					# Setting active solvers
  Name = "STRESS"
  Calculate Stresses = True
  Plane Stress = True				# Turning on plane stress simulation
  Active Solvers(1) = 2
End

Equation 2
  Name = "DATA"
  Active Solvers(1) = 1
End

Material 1					# Defining the material
  Name = "STEEL"
  Poisson ratio = 0.3
  Porosity Model = Always saturated
  Youngs modulus = 2.1e11
End

Boundary Condition 1				# Applying the boundary conditions
  Target Boundaries(1) = 12
  Name = "AB"
  Displacement 1 = 0
End

Boundary Condition 2
  Target Boundaries(1) = 13
  Name = "CD"
  Displacement 2 = 0
End

Boundary Condition 3
  Target Boundaries(1) = 14
  Name = "BC"
  Normal Force = 10e6
End
