// draw a simple sphere model

#include "Sphere.hpp"
#include "GLapp.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>
#include <math.h>

using namespace glm;  // avoid glm:: for all glm types and functions

#ifndef F_PI
#define F_PI 3.1415926f
#endif

// load the sphere data
Sphere::Sphere(int w, int h, vec3 size, std::string texturePPM) :
    Object(std::vector<std::string>{texturePPM}, std::vector<int>{-1})
{
    // build vertex, normal and texture coordinate arrays
    // * x & y are longitude and latitude grid positions
    for(unsigned int y=0;  y <= h;  ++y) {
        for(unsigned int x=0;  x <= w;  ++x) {
            // Texture coordinates scaled from x and y. Be sure to cast before division!
            float u = float(x)/float(w), v = float(y) / float(h);
            uv.push_back(vec2(u,v));

            // normal for sphere is normalized position in spherical coordinates
            float cx = cosf(2.f * F_PI * u), sx = sinf(2.f * F_PI * u);
            float cy = cosf(F_PI * v), sy = sinf(F_PI * v);
            vec3 N = vec3(cx * sy, sx * sy, cy);
            norm.push_back(N);

            // 3d vertex location scaled by sphere size
            vert.push_back(size * N);
        }
    }

    // build index array linking sets of three vertices into triangles
    // two triangles per square in the grid. Each vertex index is
    // essentially its unfolded grid array position. Be careful that
    // each triangle ends up in counter-clockwise order
    for(unsigned int y=0; y<h; ++y) {
        for(unsigned int x=0; x<w; ++x) {
            indices.push_back((w+1)* y    + x);
            indices.push_back((w+1)* y    + x+1);
            indices.push_back((w+1)*(y+1) + x+1);

            indices.push_back((w+1)* y    + x);
            indices.push_back((w+1)*(y+1) + x+1);
            indices.push_back((w+1)*(y+1) + x);
        }
    }

    initGPUData();
}

//
// this is called every time the sphere needs to be redrawn 
//
void Sphere::setRenderState(GLapp *app, double now)
{
    // inherit parent's draw settings
    Object::setRenderState(app, now);

    // update model position
    objectShaderData.WorldFromModel = translate(mat4(1), 100.f * vec3(cosf(now), sinf(now), 1));
    objectShaderData.ModelFromWorld = inverse(objectShaderData.WorldFromModel);

    glBindBufferBase(GL_UNIFORM_BUFFER, 1, bufferIDs[OBJECT_UNIFORM_BUFFER]);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ObjectShaderData), &objectShaderData);
}

