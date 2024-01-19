// Load and draw OBJ file
#include "ObjLoad.hpp"

#include "Object.hpp"
#include "GLapp.hpp"
#include "NavMesh.hpp"
#include "config.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <chrono>
#include <string>
#include <regex>
#include <map>
#include <assert.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>


using namespace glm;  // avoid glm:: for all glm types and functions
using namespace std;  // avoid  on std types and functions


// obj file material
struct Material {
	vec3 Ka, Kd, Ks;
	float Ns;
	vector<string> maps;
	vector<int> channels;

	Material() : Ka(0), Kd(0.5), Ks(0), Ns(0), maps(vector<string>{"","","",""}), channels(vector<int>{-1,-1,-1,-1}) {}

    static int channel(const char c);
};

// convert character designator to channel index
int Material::channel(const char c)
{
    switch(c) {
        case 'r': return 0;
        case 'g': return 1;
        case 'b': return 2;
        default: return -1;
    }
}

// Load from file name
// Add to GLapp objects list
// If navmesh isn't nullptr, add to navmesh data
vec3 ObjLoad(GLapp &app, NavMesh *navmesh, const char *objFileName)
{
    auto startTime = chrono::high_resolution_clock::now();

    // regular expressions for parsing: static and optimize to only build once
    static const regex re_map("(?:-imfchan\\s+(r|g|b)\\s+)?(\\S+)\\s*", regex::optimize);
    static const regex re_farg("(\\d+)(?:/(\\d*)(?:/(\\d+))?)?", regex::optimize);

    // map from material name to properties
	map<string, Material> materialMap;
	Material *currentMaterial = &materialMap[""];

	// map from face v/vt/vn string to vertex ID
	map<string, int> vertexMap;

	// open obj file, relative paths from project data directory
    filesystem::path objPath(objFileName);
    if (objPath.is_relative()) objPath = filesystem::path(PROJECT_DATA_DIR) / objPath;
	ifstream objFile(objPath.string());
	assert(objFile);

	// intermediate position, texture coordinate, and normal lists
	Object *newobj = nullptr;
	vector<vec3> v;
	vector<vec2> vt;
	vector<vec3> vn;
    vec3 BoxMin = vec3(INFINITY), BoxMax = vec3(-INFINITY);

	// parse a line at a time
	string line;
	while (getline(objFile, line)) {
        const char *cline = line.c_str();
        float x, y, z;
        int pos0, pos1;
        smatch match;
        
        // material library: parse 2nd file
		if (pos1=0, sscanf(cline, " mtllib %n%*s%n", &pos0, &pos1), pos1>0) {
			filesystem::path mtlPath = objPath.parent_path() / string(cline+pos0, pos1-pos0);
			ifstream mtlFile(mtlPath.string());
			assert(mtlFile);

			Material *newMaterial = nullptr;

			while (getline(mtlFile, line)) {
                const char *cline = line.c_str();
				if (pos1=0, sscanf(cline, " newmtl %n%*s%n", &pos0, &pos1), pos1>0)
					newMaterial = &materialMap[string(cline+pos0, pos1-pos0)];
				else if (sscanf(cline, " Ka %f %f %f", &x, &y, &z) == 3)
                    newMaterial->Ka = vec3(x, y, z);
                else if (sscanf(cline, " Kd %f %f %f", &x, &y, &z) == 3)
                    newMaterial->Kd = vec3(x, y, z);
                else if (sscanf(cline, " Ks %f %f %f", &x, &y, &z) == 3)
                    newMaterial->Ks = vec3(x, y, z);
                else if (sscanf(cline, " Ns %f", &x) == 1)
                    newMaterial->Ns = x;
                else if (pos1=0, sscanf(cline, " map_Kd %n%*s%n", &pos0, &pos1), pos1>0) {
                    string line = string(cline + pos0);
                    regex_match(line, match, re_map);
                    newMaterial->maps[0] = (mtlPath.parent_path() / match[2].str()).string();
                    newMaterial->channels[0] = Material::channel(match[1].str()[0]);
                }
                else if (pos1=0, sscanf(cline, " map_Ka %n%*s%n", &pos0, &pos1), pos1>0) {
                    string line = string(cline + pos0);
                    regex_match(line, match, re_map);
                    newMaterial->maps[1] = (mtlPath.parent_path() / match[2].str()).string();
                    newMaterial->channels[1] = Material::channel(match[1].str()[0]);
                }
                else if (pos1=0, sscanf(cline, " map_Ks %n%*s%n", &pos0, &pos1), pos1>0) {
                    string line = string(cline + pos0);
                    regex_match(line, match, re_map);
                    newMaterial->maps[2] = (mtlPath.parent_path() / match[2].str()).string();
                    newMaterial->channels[2] = Material::channel(match[1].str()[0]);
                }
                else if (pos1=0, sscanf(cline, " map_Ns %n%*s%n", &pos0, &pos1), pos1>0) {
                    string line = string(cline + pos0);
                    regex_match(line, match, re_map);
                    newMaterial->maps[3] = (mtlPath.parent_path() / match[2].str()).string();
                    newMaterial->channels[3] = Material::channel(match[1].str()[0]);
                }
			}
		}

        // finalize prior object when switching materials
        else if (pos1=0, sscanf(cline, " usemtl %n%*s%n", &pos0, &pos1), pos1>0) {
			currentMaterial = &materialMap[string(cline+pos0, pos1-pos0)];

			if (newobj) newobj->initGPUData();
			newobj = nullptr;

			vertexMap.clear();
		}

        else if (sscanf(cline, " v %f %f %f", &x, &y, &z) == 3) {
            vec3 newv = vec3(x, y, z);
            BoxMin = min(BoxMin, newv);
            BoxMax = max(BoxMax, newv);
            v.push_back(newv);
        }

        else if (sscanf(cline, " vt %f %f", &x, &y) == 2)
            vt.push_back(vec2(x, y));

        else if (sscanf(cline, " vn %f %f %f", &x, &y, &z) == 3)
            vn.push_back(vec3(x, y, z));

		else if (pos1=0, sscanf(cline, " f%n", &pos1), pos1>0) {
			// set up new component object with current material
			if (!newobj) {
				newobj = new Object(currentMaterial->maps, currentMaterial->channels);
				newobj->objectShaderData.Ambient = currentMaterial->Ka;
				newobj->objectShaderData.Diffuse = currentMaterial->Kd;
				newobj->objectShaderData.Specular = vec4(currentMaterial->Ks, currentMaterial->Ns);
				app.objects.push_back(newobj);
			}

			// add to vertex and index lists
			int vID, vtID, vnID, vertexTuple[3];
            
            cline += pos1;
			for (int i=0; pos1=0, sscanf(cline, " %n%*s%n", &pos0, &pos1), pos1>0; ++i) {
                string tuple(cline + pos0, pos1-pos0);
                cline += pos1;
                
                // create new GPU vertex if we haven't seen this vertex tuple before
				if (vertexMap.find(tuple) == vertexMap.end()) {
					vertexMap[tuple] = newobj->vert.size();

                    regex_match(tuple, match, re_farg);
                    int nmatch = match.size();
					newobj->vert.push_back(v[stoi(match[1])-1]);
                    if (match.size() > 2 && match[2].length() > 0)
                        newobj->uv.push_back(vt[stoi(match[2])-1]);
                    if (match.size() > 3 && match[3].length() > 0)
                        newobj->norm.push_back(vn[stoi(match[3])-1]);
				}
                
                // advance triangle fan
                vertexTuple[1] = vertexTuple[2];
                vertexTuple[2 * (i!=0)] = vertexMap[tuple];
                
                // output next triangle in fan
                if (i > 1) {
                    newobj->indices.push_back(vertexTuple[0]);
                    newobj->indices.push_back(vertexTuple[1]);
                    newobj->indices.push_back(vertexTuple[2]);

                    if (navmesh) 
                        navmesh->addTriangle(
                            newobj->vert[vertexTuple[0]],
                            newobj->vert[vertexTuple[1]],
                            newobj->vert[vertexTuple[2]]);
                }
			}
		}
	}

	if (newobj) newobj->initGPUData();

    auto endTime = chrono::high_resolution_clock::now();
    chrono::duration<float> elapsed = endTime - startTime;
    cout << objFileName << " load in " << elapsed.count() << " seconds\n";

    return BoxMax - BoxMin;
}
