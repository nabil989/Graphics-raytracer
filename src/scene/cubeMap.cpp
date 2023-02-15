#include "cubeMap.h"
#include "ray.h"
#include "../ui/TraceUI.h"
#include "../scene/material.h"
#include <iostream>
#include <algorithm>

using namespace std;

extern TraceUI* traceUI;

glm::dvec3 CubeMap::getColor(ray r) const
{
	// YOUR CODE HERE
	// FIXME: Implement Cube Map here
	glm::dvec3 dir = r.getDirection();
    // std::cout << "dir " << dir;
    glm::dvec3 maxdir = glm::abs(dir);

    double maxV = glm::max(maxdir[0], glm::max(maxdir[1], maxdir[2]));

    int face = 0;

    if(maxV == glm::abs(dir[0])){
        face = dir[0] > 0 ? 0 : 1;
    }
    else if(maxV == glm::abs(dir[1])){
        face = dir[1] > 0 ? 2: 3;
    }
    else{
        face = dir[2] > 0 ? 5 : 4;
    }

    double u,v;

    if(face == 0){
		if (dir[0] == 0)
			return {0, 0, 0};
        u = dir[2] / dir [0];
        v = dir[1] / dir[0];
    }
    if(face == 1){
		if (dir[0] == 0)
			return {0, 0,0};
        u = dir[2] / dir [0];
        v = -dir[1] / dir [0];
    }
    if(face == 2){
		if (dir[1] == 0)
			return {0, 0,0};
        u = dir[0]/ dir[1];
        v = dir[2]/ dir[1];
    }
    if(face == 3){
		if (dir[1] == 0)
			return {0, 0,0};
        u = dir[0]/ dir[1];
        v = dir[2]/ dir[1];
    }
    if(face == 4){
		if (dir[2] == 0)
			return {0, 0,0};
        u = -dir[0] / dir[2];
        v = -dir[1]/ dir[2];
    }
    if(face == 5){
		if (dir[2] == 0)
			return {0, 0,0};
        u = -dir[0] / dir[2];
        v = dir[1]/ dir[2];
    }

    u = glm::max(0.0, glm::min((u/2 + 0.5), 1.0));
    v = glm::max(0.0, glm::min((v/2 + 0.5), 1.0));
    
    // std::clamp((u/2 + 0.5),0.0, 1.0);
    // v = std::clamp((v/2 + .5), 0.0, 1.0);
    // v = (v/2 + 0.5) ;
    // u = glm::abs((u/2) + .5) ;
    // v = glm::abs((v/2) + .5) ;

    return tMap[face]->getMappedValue({u, v});
}

CubeMap::CubeMap()
{
}

CubeMap::~CubeMap()
{
}

void CubeMap::setNthMap(int n, TextureMap* m)
{
	if (m != tMap[n].get())
		tMap[n].reset(m);
}