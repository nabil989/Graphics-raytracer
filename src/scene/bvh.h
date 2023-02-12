#pragma once
#include "bbox.h"
#include <vector>
using std::unique_ptr;
#include "camera.h"
#include "material.h"
#include "ray.h"
#include "scene.h"

struct bvhNode {
    BoundingBox * box;
    bool leaf;
    struct bvhNode * left;
    struct bvhNode * right;
    
};

bvhNode* createBVH(std::vector<std::unique_ptr<Geometry>> objects);

// class bvhTree {
//     public:
//         bvhNode* createBVH(std::vector<std::unique_ptr<Geometry>> objects) const;
// };