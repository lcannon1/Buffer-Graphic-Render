// data and functions to draw a sphere
#pragma once

#include "Object.hpp"

// sphere object
class Sphere : public Object {
public:
    // create sphere given latitude and longitude sizes and color texture
    Sphere(int width, int height, glm::vec3 size, std::string texturePPM);

    // update render state, overridden to move object around
    virtual void setRenderState(GLapp *app, double now) override;
};
