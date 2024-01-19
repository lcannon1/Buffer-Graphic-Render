// draw a simple plan3 model

#include "Plane.hpp"
#include "GLapp.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>

using namespace glm;  // avoid glm:: for all glm types and functions

// load the sphere data
Plane::Plane(vec3 size, std::string texturePPM) :
    Object(std::vector<std::string>{texturePPM}, std::vector<int>{-1})
{
    // build texture coordinate, normal, and vertex arrays
    uv = {vec2(0.f,0.f), vec2(1.f,0.f), vec2(0.f,1.f), vec2(1.f,1.f)};
    norm = {vec3(0.f,0.f,1.f), vec3(0.f,0.f,1.f), vec3(0.f,0.f,1.f), vec3(0.f,0.f,1.f)};
    vert = {
        vec3(-0.5f*size.x, -0.5f*size.y, 0.f),
        vec3( 0.5f*size.x, -0.5f*size.y, 0.f),
        vec3(-0.5f*size.x,  0.5f*size.y, 0.f),
        vec3( 0.5f*size.x,  0.5f*size.y, 0.f)
    };

    // build index array linking sets of three vertices into triangles
    indices = {0, 1, 3,   0, 3, 2};

    initGPUData();
}
