#!/bin/sh
cd "$(dirname "$0")"
cd lib
ld -r -b binary -o all_res.o all.res
cd ..
g++ main.cpp -mwindows -O -o mctrace -Ilib/include -lopengl32 -L lib -lglew32 -lglfw3dll -lgdi32 -luser32 -lshell32 -lwinmm lib/all_res.o
g++ main.cpp -mwindows -g -o mctrace_dbg -Ilib/include -lopengl32 -L lib -lglew32 -lglfw3dll -lgdi32 -luser32 -lshell32 -lwinmm lib/all_res.o
