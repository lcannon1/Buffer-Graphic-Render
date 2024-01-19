// functions to load shaders

#include "Shader.hpp"
#include "config.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <filesystem>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <assert.h>

#ifdef _WIN32
// don't complain if we use standard IO functions instead of windows-only
#pragma warning( disable: 4996 )
#endif

// load and compile a single shader
// id is an existing shader object
// shader type is defined by shader object type
bool loadShader(unsigned int id, const char *file)
{
    // read entire file
    std::filesystem::path path = std::filesystem::path(PROJECT_DATA_DIR) / file;
#ifdef WIN32
    struct _stat statbuf;
    int ok = _stat(path.string().c_str(), &statbuf);
#else
    struct stat statbuf;
    int ok = stat(path.string().c_str(), &statbuf);
#endif
    assert(ok != -1);

    // feed shader to OpenGL as an array of blocks of code with a parallel array of sizes
    // we're using one block with the entire code file.
    int shaderBlockSizes[] = {int(statbuf.st_size)};
    std::vector<GLchar> shader(shaderBlockSizes[0]);
    GLchar *shaderBlocks[] = {shader.data()};

    FILE *f = fopen(path.string().c_str(), "rb");
    assert(f);
    fread(shaderBlocks[0], 1, shaderBlockSizes[0], f);
    fclose(f);

    // compile as shader
    glShaderSource(id, 1, shaderBlocks, shaderBlockSizes);
    glCompileShader(id);

    // was compile successful?
    GLint success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (success) return true;

    // how big is the error message?
    GLsizei infoLen;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLen);

    // print the message
    char *infoLog = new char[infoLen];
    glGetShaderInfoLog(id, infoLen, 0, infoLog);
    fprintf(stderr, "%s", infoLog);

    // free the message buffer
    delete[] infoLog;
    return false;   // failed to compile
}


// load a set of shaders
bool loadShaders(unsigned int progID, std::vector<ShaderInfo> &components)
{
    // load shader code
    for(auto shader : components) {
        if (! loadShader(shader.id, shader.file)) return false; // bail on error
    }

    // link shader programs
    for(auto shader : components)
        glAttachShader(progID, shader.id);
    glLinkProgram(progID);

    // was link successful?
    GLint success;
    glGetProgramiv(progID, GL_LINK_STATUS, &success);
    if (success) return true;

    // how big is the error message?
    GLsizei infoLen;
    glGetProgramiv(progID, GL_INFO_LOG_LENGTH, &infoLen);

    // print the message
    char *infoLog = new char[infoLen];
    glGetProgramInfoLog(progID, infoLen, 0, infoLog);
    fprintf(stderr, "%s", infoLog);

    // free the message buffer
    delete[] infoLog;
    return false;   // failed to link
}
