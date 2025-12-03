#!/bin/sh
g++ main.cpp -o mctrace -Ilib/include -lopengl32 -L lib -lglew32 -lglfw3dll -lgdi32 -luser32
g++ -g main.cpp -o mctrace_dbg -Ilib/include -lopengl32 -L lib -lglew32 -lglfw3dll -lgdi32 -luser32
