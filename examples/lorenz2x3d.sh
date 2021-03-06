#!/bin/bash
n=$(cat lorenz.dat | wc -l)

cat << EOF
<html>
<head>
 <script type="text/javascript" src="x3dom.js"> </script>
 <link rel="stylesheet" type="text/css" href="x3dom.css"/>
</head>
<body onload="document.getElementById('canvas').runtime.fitAll()">
<x3d width="100%" height="100%" id="canvas">
<Scene>
<Transform>
<Shape>
<LineSet vertexCount="${n}">
<Coordinate point="
EOF

awk '{print $2,$3,$4}' lorenz.dat 

cat << EOF
"/>
</LineSet>
</Shape>
</Scene> 
</x3d>
</body>
</html>
EOF
