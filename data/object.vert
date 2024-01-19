#version 410 core
// simple object vertex shader

// per-frame data, must match in C++ and any shaders that use it
layout(std140)                          // standard layout matching C++
uniform SceneData {                     // like a class name
    mat4 ProjFromWorld, WorldFromProj;  // viewing matrices
    vec4 LightDir;                      // light direction & ambient
};

// per-object data
layout(std140)
uniform ObjectData {
    mat4 WorldFromModel, ModelFromWorld;    // object matrices
    vec3 Ambient; float pad0;               // ambient color & padding
    vec3 Diffuse; float pad1;               // diffuse color & padding
    vec4 Specular;                          // specular color and exponent
};

// per-vertex input
/*in vec2 vUV;        // vertex texture coordinate
in vec3 vPosition;  // object-space position of vertex
in vec3 vNormal;    // object-space normal at vertex*/
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vUV;

// output (must match fragment shader input)
out vec2 texcoord;  // texture coordinate
out vec3 normal;    // world-space normal
out vec4 position;  // world-space position

void main() {
    // just pass texture coordinate through
    texcoord = vUV;

    // homogeneous transform of position to world space
    position = WorldFromModel * vec4(vPosition, 1);

    // 3x3 transform of normal to world space
    normal = normalize(vNormal * mat3(ModelFromWorld));

    // further transform world-space position to projection space
    gl_Position = ProjFromWorld * position;
}
