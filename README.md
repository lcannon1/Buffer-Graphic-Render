# Buffer-Graphic-Render
Clone repository for computer graphic rendering project with multiple render modes

# Contributions
Files created to load objects and render graphics. Implementation of graphical computations and set up and binding of graphical buffers.

# Credits
Initial Repository Creation:
Dr. Marc Olano
Associate Professor of Computer Science and Electrical Engineering
University of Maryland, Baltimore County

Source code summary:

GLapp.hpp/GLapp.cpp: Overall application data, initialization code, and well
GLFW callbacks, and main rendering loop.

Shader.hpp/Shader.cpp: Loading and compiling shaders.

Object.hpp/Object.cpp: Base class for objects, managing vertex and index
arrays, textures, and shaders.

Plane.hpp/Plane.cpp: Minimal two-triangle object with hard-coded data.

Sphere.hpp/Sphere/cpp: Parametric sphere object with per-frame position
updates.

config.h.in: Used by CMake to resolve data file paths.
