#!/bin/bash

srcs="../src/flow/init.c ../src/pdes/init.c"
grep "///help+extra+desc" ${srcs} | cut -d" " -f2- | sed 's/@$//' | sed 's_/\\/_//_' | pandoc -t plain
