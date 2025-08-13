#!/bin/sh
g++ main.cpp -o mctrace -lGL -lGLEW -lglfw
g++ -g main.cpp -o mctrace_dbg -lGL -lGLEW -lglfw

