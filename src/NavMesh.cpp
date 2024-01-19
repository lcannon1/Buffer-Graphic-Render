// navigation intersection testing

#include "NavMesh.hpp"

using namespace glm;  // avoid glm:: for all glm types and functions
using namespace std;  // avoid  on std types and functions


// add data to trace against a triangle
void NavMesh::addTriangle(vec3 v0, vec3 v1, vec3 v2)
{
    vec3 e0 = v1-v2, e1 = v2-v0, e2 = v0-v1;
    vec3 N = normalize(cross(e0, e1));
    vec3 Na = cross(N, e0), Nb = cross(N, e1);
    Na = Na / dot(Na,e2);   Nb = Nb / dot(Nb,e0);

    plane.push_back(vec4(N, -dot(N, v0)));
    alpha.push_back(vec4(Na,-dot(Na, v1)));
    beta.push_back( vec4(Nb,-dot(Nb, v2)));
}

// find the closest intersection in the given normalized direction 
float NavMesh::trace(vec3 start, vec3 direction, float near, float far) const
{
    vec4 s = vec4(start, 1), d = vec4(direction, 0);
    for(int i=0; i<plane.size(); ++i) {
        float t = -dot(plane[i], s) / dot(plane[i], d);
        if (t < near || t > far) continue;

        vec4 p = vec4(start + t * direction, 1);
        float a = dot(alpha[i], p);
        if (a < 0 || a > 1) continue;

        float b = dot(beta[i], p);
        if (b < 0 || a + b > 1) continue;

        far = t;
    }

    return far;
}


// return true if there is any hit between near and far
bool NavMesh::anyhit(vec3 start, vec3 direction, float near, float far) const
{
    vec4 s = vec4(start, 1), d = vec4(direction, 0);
    for(int i=0; i<plane.size(); ++i) {
        float t = -dot(plane[i], s) / dot(plane[i], d);
        if (t < near || t > far) continue;

        vec4 p = vec4(start + t * direction, 1);
        float a = dot(alpha[i], p);
        if (a < 0 || a > 1) continue;

        float b = dot(beta[i], p);
        if (b < 0 || a + b > 1) continue;

        return true;
    }

    return false;
}



