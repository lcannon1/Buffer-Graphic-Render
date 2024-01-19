#version 410 core
// simple object fragment shader

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

// global per-object setting, outside of a uniform block
uniform sampler2D ColorTexture;
uniform sampler2D AmbientTexture;
uniform sampler2D SpecularTexture;
uniform sampler2D GlossTexture;

// input (must match vertex shader output)
in vec2 texcoord;  // texture coordinate
in vec3 normal;    // world-space normal
in vec4 position;  // world-space position

// output to frame buffer
out vec4 fragColor;
layout (location = 0) out vec4 surfColOut;
layout (location = 1) out vec4 normOut;
layout (location = 2) out vec4 worldPosOut;

void main() {
    // lighting vectors
    vec3 N = normalize(normal);             // surface normal
    vec3 L = normalize(LightDir.xyz);       // light direction
    vec3 V = normalize(WorldFromProj[3].xyz * position.w - position.xyz * WorldFromProj[3].w);
    vec3 H = normalize(V+L);
    float N_dot_L = max(0., dot(N, L));
    float N_dot_H = max(0., dot(N, H));

    // ambient contribution
    vec3 ambCol = Ambient * LightDir.a;
    if (textureSize(AmbientTexture,0) != ivec2(1,1))
        ambCol *= texture(AmbientTexture, texcoord).rgb;

    // diffuse or texture
    vec3 diffCol = Diffuse;
    if (textureSize(ColorTexture,0) != ivec2(1,1))
        diffCol *= texture(ColorTexture, texcoord).rgb;
    diffCol *= N_dot_L;

    // gloss/roughness
    float gloss = Specular.w;
    if (textureSize(GlossTexture,0) != ivec2(1,1))
        gloss *= texture(GlossTexture, texcoord).r;

    // specular
    vec3 specCol = Specular.rgb * pow(N_dot_H, gloss) * N_dot_L;
    if (textureSize(SpecularTexture,0) != ivec2(1,1))
        specCol *= texture(SpecularTexture, texcoord).rgb;

    // final color
    fragColor = vec4(ambCol + diffCol + specCol, 1);

    surfColOut = vec4(diffCol[0], diffCol[1], diffCol[2], gloss);
    normOut = vec4(N.rgb[0], N.rgb[1], N.rgb[2], specCol[1]);
    worldPosOut = vec4(position.rgb[0], position.rgb[1], position.rgb[2], ambCol[1]);
}
