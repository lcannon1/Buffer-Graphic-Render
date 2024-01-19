// base class for drawable objects
#pragma once

#include "Shader.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <string>

class Object {
public:
    // uniform buffer data about the object position
    // must be plain old data, matching layout in shaders
    // rearrange or pad as necessary for vec4 alignment
    struct ObjectShaderData {
        glm::mat4 WorldFromModel, ModelFromWorld;
        glm::vec3 Ambient; float pad0;  // ambient color & padding
        glm::vec3 Diffuse; float pad1;  // diffuse color & padding
        glm::vec4 Specular;             // specular color (rgb) and exponent (w)
    } objectShaderData;

    // arrays defining triangles for GPU
    unsigned int varrayID;              // GL vertex array object, containing:
    std::vector<glm::vec3> vert;        //   per-vertex position
    std::vector<glm::vec3> norm;        //   per-vertex normal
    std::vector<glm::vec2> uv;          //   per-vertex texture coordinate
    std::vector<unsigned int> indices;  //   3 vertex indices per triangle

    // GL texture ID(s), array for extensibility to more textures
    enum {COLOR_TEXTURE, AMBIENT_TEXTURE, SPECULAR_TEXTURE, GLOSS_TEXTURE, NUM_TEXTURES};
    unsigned int textureIDs[NUM_TEXTURES];

    // GL buffer object IDs
    enum {OBJECT_UNIFORM_BUFFER, POSITION_BUFFER, NORMAL_BUFFER, UV_BUFFER, INDEX_BUFFER, NUM_BUFFERS};
    unsigned int bufferIDs[NUM_BUFFERS];

    // GL shaders
    unsigned int shaderID;      // ID for shader program
    std::vector<ShaderInfo> shaderParts;  // vertex & fragment shader info

public:
    // base object constructor: create buffers and textures
    // channel is -1 for use all channels, 0 for red, 1 for green, or 2 for blue
    Object(std::vector<std::string> textures, std::vector<int> channels);

    // virtual destructor to delete any child class data
    virtual ~Object();

    // load an image file into a texture object
    void loadPPM(std::string imagefile, unsigned int bufferID, int channel = -1);

    // load GPU data after vert, norm, uv, and indices arrays are full
    void initGPUData();

    // load/reload shaders
    virtual void updateShaders();

    // set shader, textures, etc. for this draw
    virtual void setRenderState(class GLapp *app, double now);

    // draw this object
    virtual void draw(class GLapp *app, double now);
};
