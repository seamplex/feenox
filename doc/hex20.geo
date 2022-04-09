SetFactory("OpenCASCADE");

a = 1;
n = 2;

Point(1) = {0,0,0};

line[] = Extrude{a, 0, 0} { Point{1}; Layers{n}; Recombine; };
face[] = Extrude{0, a, 0} { Line{line[1]}; Layers{n}; Recombine; };
vol[]  = Extrude{0, 0, a} { Surface{face[1]}; Layers{n}; Recombine; };

Physical Volume(1) = {vol[1]};
Mesh.ElementOrder = 2;

Mesh.SecondOrderIncomplete = 1;
