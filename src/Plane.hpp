// data and functions to draw a plane
#pragma once

#include "Object.hpp"

// plane object
class Plane : public Object {
public:
    // create plane from -size/2 to size/2
    Plane(glm::vec3 size, std::string texturePPM);
};
