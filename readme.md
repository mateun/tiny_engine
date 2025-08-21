# tiny_engine

This is a single-header file 3D game engine. 

## Prerequisites: 

Windows 10 upwards.

MSVC build tools


## Building: 

Create a folder called "build".
Run "build.bat" while in the project root.

## Usage:
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

/DTE_MATH
/DTE_WINDOWING
/DTE_2D
/DTE_3D
/DTE_NETWORK
/DTE_PHYSICS
/DTE_GAMEPLAY

To use everything, just do:

/DTE_EVERYTHING
