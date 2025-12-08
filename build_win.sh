#!/bin/sh
cd "$(dirname "$0")"
cd lib
ld -r -b binary -o all_res.o all.res
cd ..
g++ main.cpp -o mctrace -Ilib/include -lopengl32 -L lib -lglew32 -lglfw3dll -lgdi32 -luser32 lib/all_res.o
g++ -g main.cpp -o mctrace_dbg -Ilib/include -lopengl32 -L lib -lglew32 -lglfw3dll -lgdi32 -luser32 lib/all_res.o
