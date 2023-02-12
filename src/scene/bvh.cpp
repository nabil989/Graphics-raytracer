#include "bvh.h"

bvhNode * createBVH(std::vector<std::unique_ptr<Geometry>> objects)
{
    bvhNode * current;
    BoundingBox* box;
    if(objects.size() == 1){
        current->leaf = true;
        box->merge(objects[0]->getBoundingBox());
        current -> box = box;
        return current;
    }
    for(int i = 0; i < objects.size(); i++){
        box->merge(objects[i]->getBoundingBox());
    }
    current->box = box;
    //split objects into 2 parts
    current -> left = createBVH (objects);
    current -> right = createBVH(objects);
    return current;
    
}