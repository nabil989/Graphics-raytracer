#include "bvh.h"

bool compare(std::shared_ptr<Geometry> o, std::shared_ptr<Geometry> o1, int axis){
    return o->getBoundingBox().getMax()[axis] < o1->getBoundingBox().getMax()[axis];
}


bvhNode * createBVH(std::vector<std::shared_ptr<Geometry>> objects)
{   
    bvhNode * current = new bvhNode;
    BoundingBox* box = new BoundingBox();
    if(objects.size() == 0) {
        current -> leaf = true;
        return current;
    }
    if(objects.size() == 1){
        current -> leaf = true;
        box -> merge(objects[0]->getBoundingBox());
        current -> box = box;
        std::vector<std::shared_ptr<Geometry>> obj;
        obj.push_back(objects[0]);
        current -> objects = obj;
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

    sort(objects.begin(), objects.end(), compare);

    for(int i = 0; i < objects.size(); i++){
        if(objects[i]->getBoundingBox().getMax()[longest] > mid) {
            objRight.emplace_back(objects[i]);
        } else {
            objLeft.emplace_back(objects[i]);
        }
    }

    if(objRight.size() == 0){
        objRight.emplace_back(objLeft.back());
        objLeft.pop_back();

    }
    else if(objLeft.size() == 0){
        objLeft.emplace_back(objRight[0]);
        objRight.erase(objRight.begin());
    }

    //split objects into 2 parts
    current -> left = createBVH (objLeft);
    current -> right = createBVH(objRight);
    return current;
}