// step #1: read the cad file (length units are millimeters)
Merge "tensile-test-specimen.step";
    
// step #2: identify volumes
// there's only one volume, the id is 1 and the name can be anything
Physical Volume ("bulk") =  {1};    
  
// step #3: identify surfaces
// the name between brackets will appear in the FeenoX input file in the BC keyword
Physical Surface ("left") =  {1};   // left face, to be fixed
Physical Surface ("right") =  {7};  // right face, will hold the load
    
// step #4:
Mesh.MeshSizeMax = 3;         // set the max element size lc (in mm)
Mesh.ElementOrder = 2;        // ask for second-order tetrahedra
Mesh.OptimizeNetgen = 1;      // optimize linear elements
Mesh.HighOrderOptimize = 3;   // optimize quadratic elements
