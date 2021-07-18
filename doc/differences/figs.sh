# create CAD
gmsh - le10-cad.geo

# create figure with the tags
gmsh le10-tags.geo
sed -i 's/(OCC)//' le10-tags.svg 
sed -i 's/Unknown/Generic/' le10-tags.svg 

# create FEM mesh
gmsh -3 le10.geo

# create figure with the mesh
gmsh le10-mesh.geo
convert -trim le10-mesh.png le10-mesh.png

# structured
gmsh -3 le10-structured.geo
gmsh le10-mesh-structured.geo
convert -trim le10-mesh-structured.png le10-mesh-structured.png
