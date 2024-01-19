// collected state for access in callbacks
// 
#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

class GLapp {
public:
    struct GLFWwindow *win;      // graphics window from GLFW system

    // uniform buffer data about the scene
    // must be plain old data, matching layout in shaders
    // rearrange or pad as necessary for vec4 alignment
    struct SceneShaderData {
        glm::mat4 ProjFromWorld, WorldFromProj;  // viewing matrix & inverse
        glm::vec4 LightDir;         // xyz = light direction; w = ambient
    } sceneShaderData;
    unsigned int sceneUniformsID;

    // view info
    bool active;                // clicked into window
    int width, height;          // current window dimensions
    float near, far;            // near and far clipping distances
    glm::vec3 position;         // player position
    float pan, tilt;            // horizontal and vertical Euler angles
    float speed, moveRate, strafeRate; // keyboard motion rate in units/sec
    char renderMode;

    // mouse state
    double mouseX, mouseY;      // location of mouse at last event

    // drawing state
    bool wireframe;

    // time (in seconds) of last frame
    double prevTime;

    // frame buffers
    GLuint gAlbedo, gNorm, gPos;
    GLuint quad_VertexArrayID, quad_vertexbuffer;;

    // objects to draw
    std::vector<class Object*> objects;

    // ray tracing data
    class NavMesh *navmesh;

public:
    // initialize and destroy app data
    GLapp();
    ~GLapp();

    // update shader uniform state each frame
    void sceneUpdate(float dTime);

    // main rendering loop
    void render();
};
