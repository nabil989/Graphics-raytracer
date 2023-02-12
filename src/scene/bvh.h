#pragma once
#pragma warning(disable : 4786)

#include "bbox.h"
#include <vector>
using std::unique_ptr;
#include "camera.h"
#include "material.h"
#include "ray.h"
#include "scene.h"

#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <mutex>

#include <glm/geometric.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

struct bvhNode {
    BoundingBox * box;
    bool leaf;
    struct bvhNode * left;
    struct bvhNode * right;
    std::vector<std::unique_ptr<Geometry>> objects;
};

bvhNode* createBVH(std::vector<std::unique_ptr<Geometry>> objects);

// class bvhTree {
//     public:
//         bvhNode* createBVH(std::vector<std::unique_ptr<Geometry>> objects) const;
// };