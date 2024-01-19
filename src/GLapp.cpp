//
// Simple GL example
//


#include "GLapp.hpp"
#include "Sphere.hpp"
#include "Plane.hpp"
#include "ObjLoad.hpp"
#include "NavMesh.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>
#include <stdio.h>
#include <assert.h>

#ifndef F_PI
#define F_PI 3.1415926f
#endif

using namespace glm;  // avoid glm:: for all glm types and functions

///////
// GLFW callbacks must use extern "C"
extern "C" {
    // called for GLFW error
    void error(int error, const char *description) {
        fprintf(stderr, "GLFW error %d: %s\n", error, description);
    }

    // called whenever the window size changes
    void reshape(GLFWwindow *win, int width, int height) {
        // save window dimensions
        GLapp *app = (GLapp*)glfwGetWindowUserPointer(win);
        glfwGetFramebufferSize(win, &app->width, &app->height);

        // viewport size matches window size
        glViewport(0, 0, app->width, app->height);
    }

    // called when mouse button is pressed
    void mousePress(GLFWwindow *win, int button, int action, int mods) {
        if (button != GLFW_MOUSE_BUTTON_LEFT) return;

        // disable cursor and grab focus
        GLapp *app = (GLapp*)glfwGetWindowUserPointer(win);
        app->active = true;
        glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwGetCursorPos(win, &app->mouseX, &app->mouseY);
    }

    // called when mouse is moved
    void mouseMove(GLFWwindow *win, double x, double y) {
        GLapp *app = (GLapp*)glfwGetWindowUserPointer(win);
        if (!app->active) return;

        // rotation angle, scaled so across the window = one rotation
        app->pan += float(F_PI * float(x - app->mouseX) / app->width);
        app->tilt += float(0.5f*F_PI * float(y - app->mouseY) / app->height);

        // remember location so next update will be relative to this one
        app->mouseX = x;
        app->mouseY = y;
    }

    // called on any keypress
    void keyPress(GLFWwindow *win, int key, int scancode, int action, int mods) {
        GLapp *app = (GLapp*)glfwGetWindowUserPointer(win);

        if (action == GLFW_PRESS) {
            switch (key) {
            case 'A':                   // move left
                app->strafeRate = -app->speed;
                return;

            case 'D':                   // move right
                app->strafeRate = app->speed;
                return;

            case 'W':                   // move forward
                app->moveRate = app->speed;
                return;

            case 'S':                   // move backwards
                app->moveRate = -app->speed;
                return;

            case 'R':                   // reload shaders
                for (auto object : app->objects)
                    object->updateShaders();
                return;

            case 'I':                   // cycle through ambient intensity
                app->sceneShaderData.LightDir.a += 0.2f;
                if (app->sceneShaderData.LightDir.a > 1.f)
                    app->sceneShaderData.LightDir.a = 0.f;
                return;

            case 'L':                   // toggle lines or solid
                app->wireframe = !app->wireframe;
                glPolygonMode(GL_FRONT_AND_BACK, app->wireframe ? GL_LINE : GL_FILL);
                return;

            case '0':                   // show gBuffer 0
                app->renderMode = '0';
                return;

            case '1':                   // show gBuffer 1
                app->renderMode = '1';
                return;

            case '2':                   // show gBuffer 2
                app->renderMode = '2';
                return;

            case '-':                   // show final render result
                app->renderMode = '-';
                return;

            case GLFW_KEY_ESCAPE:                    // Escape
                if (app->active) {                   //  1st press, release mouse
                    app->active = false;
                    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                }
                else                                 //  2nd press, exit                 
                    glfwSetWindowShouldClose(win, true);
                return;
            }
        }

        if (action == GLFW_RELEASE) {
            switch (key) {
            case 'A': case 'D':         // stop strafing
                app->strafeRate = 0;
                return;
            case 'W': case 'S':         // stop moving
                app->moveRate = 0;
                return;
            }
        }
    }
}

// initialize GLFW - windows and interaction
GLapp::GLapp()
{
    // member data initialization
    active = false;                             // not tracking mouse input
    width = 843; height = 480;                  // window size
    near = 1.f; far = 20000.f;                  // clipping
    position = vec3(-10000, -1150, 500);        // player position
    pan = 1.57f; tilt = -1.4f;                  // view
    speed = 1000.f;                             // speed of keyboard motion
    moveRate = strafeRate = 0.f;                // keyboard motion
    mouseX = mouseY = 0.f;                      // mouse view controls
    wireframe = false;                          // solid drawing
    renderMode = '-';

    navmesh = new NavMesh;

    // set error callback before init
    glfwSetErrorCallback(error);
    int ok = glfwInit();
    assert(ok);

    // OpenGL version: YOU MAY NEED TO ADJUST VERSION OR OPTIONS!
    // When figuring out the settings that will work for you, make
    // sure you can see error messages on console output.
    //
    // My driver needs FORWARD_COMPAT, but others will need to comment that out.
    // Likely changes for other versions:
    //   All versions: change VERSION_MAJOR and VERSION_MINOR
    //   OpenGL 3.0 (2008): does not support features we need
    //   OpenGL 3.1 (2009):
    //     comment out GLFW_OPENGL_PROFILE line
    //     Use "140" for the "#version" line in the .vert and .frag files
    //   OpenGL 3.2 (2009): Use "150 core" for the "#version" line in the .vert and .frag files
    //   OpenGL 3.3 (2010): Use "330 core" for the "#version" line in the .vert and .frag files
    //   Any of 4.0 or later:
    //     Similar to 3.3: #version line in shaders uses <MAJOR><MINOR>0
    //     For example, 4.6 is "#version 460 core" 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    // ask for a window with dimensions 843 x 480 (HD 480p)
    win = glfwCreateWindow(width, height, "Simple OpenGL Application", 0, 0);
    assert(win);

    glfwMakeContextCurrent(win);

    // GLEW handles OpenGL shared library access
    glewExperimental = true;
    glewInit();

    // set callback functions to be called by GLFW
    glfwSetWindowUserPointer(win, this);
    glfwSetFramebufferSizeCallback(win, reshape);
    glfwSetKeyCallback(win, keyPress);
    glfwSetMouseButtonCallback(win, mousePress);
    glfwSetCursorPosCallback(win, mouseMove);

    // tell OpenGL to enable z-buffer for overlapping surfaces
    glEnable(GL_DEPTH_TEST);

    // initialize buffer for scene shader data
    glGenBuffers(1, &sceneUniformsID);
    glBindBuffer(GL_UNIFORM_BUFFER, sceneUniformsID);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(SceneShaderData), 0, GL_STREAM_DRAW);

    // initialize scene data
    sceneShaderData.LightDir = vec4(-1,-2,2,0);

    // frame buffer creation

    /*GLuint gBuffer;
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);*/

    glGenTextures(1, &gAlbedo);
    glBindTexture(GL_TEXTURE_2D, gAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGB, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gAlbedo, 0);

    glGenTextures(1, &gNorm);
    glBindTexture(GL_TEXTURE_2D, gNorm);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGB, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNorm, 0);


    glGenTextures(1, &gPos);
    glBindTexture(GL_TEXTURE_2D, gPos);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGB, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gPos, 0);


    GLuint depthrenderbuffer;
    glGenRenderbuffers(1, &depthrenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

    GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, DrawBuffers);

    // The fullscreen quad's FBO
    glGenVertexArrays(1, &quad_VertexArrayID);
    glBindVertexArray(quad_VertexArrayID);

    static const GLfloat g_quad_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,
    };

    glGenBuffers(1, &quad_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

    // Create and compile our GLSL program for screen quad
    /*GLuint quad_programID = glCreateProgram();
    GLuint vs_ID = loadShader(GL_VERTEX_SHADER, "Passthrough.vertexshader");
    GLuint fs_ID = loadShader(GL_VERTEX_SHADER, "Passthrough.fragmentshader");
    glAttachShader(quad_programID, vs_ID);
    glAttachShader(quad_programID, fs_ID);
    GLuint texID = glGetUniformLocation(quad_programID, "renderedTexture");
    GLuint timeID = glGetUniformLocation(quad_programID, "time");*/
}

///////
// Clean up any context data
GLapp::~GLapp() 
{
    for (auto obj: objects)
        delete obj;
    delete navmesh;

    glfwDestroyWindow(win);
    glfwTerminate();
}

// call before drawing each frame to update per-frame scene state
void GLapp::sceneUpdate(float dTime)
{
    vec3 forward(sin(pan), cos(pan), 0);
    vec3 right(cos(pan), -sin(pan), 0);
    vec3 nextpos = position;
    
    if (moveRate != 0.f || strafeRate != 0.f) {
        vec3 motion = moveRate * dTime * forward + strafeRate * dTime * right;
        if (! navmesh->anyhit(position, normalize(motion), 0.f, 250.f)) {
            nextpos = position + motion;
        }
    }

    float floorhit = navmesh->trace(nextpos, vec3(0,0,-1), 0.f, 750.f);
    if (floorhit > 250.f && floorhit < 750.f)
        position = vec3(nextpos.x, nextpos.y, nextpos.z - floorhit + 500.f);

    sceneShaderData.ProjFromWorld = 
        perspective(F_PI/4.f, (float)width/height, near, far)
        * rotate(mat4(1), tilt, vec3(1,0,0))
        * rotate(mat4(1), pan, vec3(0,0,1))
        * translate(mat4(1), -position);
    sceneShaderData.WorldFromProj = inverse(sceneShaderData.ProjFromWorld);

    // Check for Selected Render Mode
    switch(renderMode) {
        case '0':
            // Render Albedo G-Buffer
            glBindFramebuffer(GL_FRAMEBUFFER, gAlbedo);
            glViewport(0,0,width,height);

            // Disable Depth Test for Perform Deferred Pass
            glDisable(GL_DEPTH_TEST);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, width, height);
            glEnable(GL_DEPTH_TEST);
            break;

        case '1':
            // Render Norm G-Buffer
            glBindFramebuffer(GL_FRAMEBUFFER, gNorm);
            glViewport(0, 0, width, height);

            // Disable Depth Test for Perform Deferred Pass
            glDisable(GL_DEPTH_TEST);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, width, height);
            glEnable(GL_DEPTH_TEST);
            break;

        case '2':
            // Render Pos G-Buffer
            glBindFramebuffer(GL_FRAMEBUFFER, gPos);
            glViewport(0, 0, width, height);

            // Disable Depth Test for Perform Deferred Pass
            glDisable(GL_DEPTH_TEST);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, width, height);
            glEnable(GL_DEPTH_TEST);
            break;

        default:
            glBindBuffer(GL_UNIFORM_BUFFER, sceneUniformsID);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(SceneShaderData), &sceneShaderData);
            break;
    }
}

// render a frame
void GLapp::render()
{
    // consistent time for drawing this frame
    double currTime = glfwGetTime();
    double dTime = currTime - prevTime;

    // Check for Selected Render Mode
    switch (renderMode) {
        case '0':
            // clear old screen contents to zero
            glClearColor(0.0, 0.0, 0.0, 0.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            break;

        case '1':
            // clear old screen contents to zero
            glClearColor(0.0, 0.0, 0.0, 0.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            break;

        case '2':
            // clear old screen contents to zero
            glClearColor(0.0, 0.0, 0.0, 0.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            break;

        default:
            // clear old screen contents to a sky blue
            glClearColor(0.5, 0.7, 0.9, 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            break;
    }

    // draw all objects
    sceneUpdate(dTime);
    for (auto object : objects)
        object->draw(this, currTime);

    // show what we drew
    glfwSwapBuffers(win);
    prevTime = currTime;
}

int main(int argc, char *argv[])
{
    // initialize windows and OpenGL
    GLapp app;

    ObjLoad(app, app.navmesh, "castle/castle.obj");

    // set up initial viewport
    reshape(app.win, app.width, app.height);

    // each frame: render then check for events
    while (!glfwWindowShouldClose(app.win)) {
        app.render();
        glfwPollEvents();
    }

    return 0;
}
