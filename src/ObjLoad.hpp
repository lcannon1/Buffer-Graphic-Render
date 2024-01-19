// Load multiple components from obj file
#include <glm/glm.hpp>

// Load from file name
// Add to GLapp objects list
// If navmesh isn't nullptr, add to navmesh data
glm::vec3 ObjLoad(class GLapp &app, class NavMesh *navmesh, const char *objFileName);
