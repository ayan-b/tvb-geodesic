#!/bin/bash 
cd geodesic_library
g++ example0.cpp
diff <(./a.exe ../data/hedgehog_mesh.txt 3 14) ../data/hedgehog_mesh_3_14.txt
diff <(./a.exe ../data/flat_triangular_mesh.txt 1) ../data/flat_triangular_mesh_1.txt
