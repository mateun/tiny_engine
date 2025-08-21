# tiny_engine

This is a single-header file 3D game engine. 

## Prerequisites: 

Windows 10 upwards.

MSVC build tools


## Usage:
To use the engine, just include the contents (or the whole file..) engine.h 
into your project. 

The engine is quite modular: 

- math
- windowing
- 2d
- 3d
- network
- physics
- gameplay


You can choose which parts to use, only these are compiled. 

To select modules, you must use these pre-processor definitions: 

/DTE_MATH\
/DTE_WINDOWING\
/DTE_2D\
/DTE_3D\
/DTE_NETWORK\
/DTE_PHYSICS\
/DTE_GAMEPLAY

To use everything, just do:

/DTE_EVERYTHING

## Building the sample game. 

There is a small sample game included which also shows much of the engine in use.
Its code is under sample.cpp and can be built with the build.bat file. 

For the build.bat to work, you need to run under Windows10, and have a folder "build" created
alongside the sample.cpp file. 

A file "tiny_engine_sample.exe" will be put into the "build" folder after successful compilation, 
which you can just run!
