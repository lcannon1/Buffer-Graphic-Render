Simple OpenGL demo using GLFW, GLEW, and GLM.

Rotate with the mouse or with the WASD keys. 'I' changes the ambient
intensity, demonstrating passing data to shaders. 'L' toggles between solid
and line drawing. 'R' reloads the shaders.

In general, there is one .hpp file per class, with the same name as the class.
Implementation functions for the class are either in the corresponding .cpp
file, or for inline functions in the corresponding .inl file.

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