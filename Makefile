CC = gcc
CXX = g++

window: clear main.cpp
	g++ -std=c++1y main.cpp -lX11 -lGLEW -lGLU -lGL -lIL -lILU -o window;./window 

clear: 
	rm -f window