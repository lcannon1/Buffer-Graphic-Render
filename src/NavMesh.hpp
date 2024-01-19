// navigation intersection test data
#pragma once

#include <glm/glm.hpp>
#include <vector>

// data 
class NavMesh {
public:
	std::vector<glm::vec4> plane;	// N and -dot(N, v0)
	std::vector<glm::vec4> alpha;	// Na and -dot(Na, v1)
	std::vector<glm::vec4> beta;	// Nb and -dot(Nb, v2)

public:
	NavMesh() {}

    // add a triangle to the lists
	void addTriangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2);

    // return distance to first triangle in given normalized direction
	float trace(glm::vec3 start, glm::vec3 direction, float near, float far) const;

    // return true if there is any hit in the normalized direction between near and far
    bool anyhit(glm::vec3 start, glm::vec3 direction, float near, float far) const;
};

