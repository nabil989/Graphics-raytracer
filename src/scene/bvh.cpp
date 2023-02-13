#include "bvh.h"

bvhNode * createBVH(std::vector<std::shared_ptr<Geometry>> objects)
{   
    bvhNode * current;
    BoundingBox* box;
    if(objects.size() == 0) {
        current -> leaf = true;
        return current;
    }
    if(objects.size() == 1){
        current->leaf = true;
        current -> box = box;
        current -> objects = objects;
        return current;
    }
    for(int i = 0; i < objects.size(); i++){
        box->merge(objects[i]->getBoundingBox());
    }
    current->box = box;
    // find longest axis
    glm::dvec3 topleft = box->getMin();
    glm::dvec3 bottomright = box->getMax();

    int longest = 0;
    int mx = -1;
    for(int i =0 ; i < 3; i++){
        double v = glm::abs(topleft[i] - bottomright[i]);
        if(v > mx){
            longest = i;
            mx = v;
        }
    }

    double mid = (bottomright[longest] - topleft[longest]) / 2;
    std::vector<std::shared_ptr<Geometry>> objLeft;
    std::vector<std::shared_ptr<Geometry>> objRight;

    for(int i = 0; i < objects.size(); i++){
        if(objects[i]->getBoundingBox().getMax()[longest] > mid) {
            objRight.emplace_back(objects[i]);
        } else {
            objLeft.emplace_back(objects[i]);
        }
    }

    //split objects into 2 parts
    current -> left = createBVH (objLeft);
    current -> right = createBVH(objRight);
    return current;
    
}