#include "bvh.h"
#include <iostream>
using namespace std;

bool comparex(std::shared_ptr<Geometry> o, std::shared_ptr<Geometry> o1){
    return o->getBoundingBox().getMax()[0] < o1->getBoundingBox().getMax()[0];
}

bool comparey(std::shared_ptr<Geometry> o, std::shared_ptr<Geometry> o1){
    return o->getBoundingBox().getMax()[1] < o1->getBoundingBox().getMax()[1];
}

bool comparez(std::shared_ptr<Geometry> o, std::shared_ptr<Geometry> o1){
    return o->getBoundingBox().getMax()[2] < o1->getBoundingBox().getMax()[2];
}

double getMid(std::shared_ptr<Geometry> o, int longest){
    return o->getBoundingBox().getMin()[longest] + (o->getBoundingBox().getMax()[longest] - o->getBoundingBox().getMin()[longest])/2;

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
        cout << "size 1 leaf" << "\n";
        current -> leaf = true;
        box -> merge(objects[0]->getBoundingBox());
        current -> box = box;
        std::vector<std::shared_ptr<Geometry>> obj;
        obj.push_back(objects[0]);
        current -> objects = obj;
        return current;
    }
    cout << "inner node, size = " << objects.size() << "\n"; 
    current -> leaf = false;
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
    if(longest == 0){
        sort(objects.begin(), objects.end(), comparex);
    }
    if(longest == 1){
        sort(objects.begin(), objects.end(), comparey);
    }
    if(longest == 2){
        sort(objects.begin(), objects.end(), comparez);
    }
    
    double mid = (bottomright[longest] - topleft[longest]) / 2;
    std::shared_ptr<Geometry> medianObj = objects[objects.size()/2];
    double median = getMid(medianObj, longest);
    std::vector<std::shared_ptr<Geometry>> objLeft;
    std::vector<std::shared_ptr<Geometry>> objRight;



    for(int i = 0; i < objects.size(); i++){
        double objectMid = getMid(objects[i], longest);
        //objects[i]->getBoundingBox().getMax()[longest]
        if(objectMid > median) {
            objRight.emplace_back(objects[i]);
        } else {
            objLeft.emplace_back(objects[i]);
        }
    }

    if(objRight.size() == 0){
        current -> leaf = true;
        current ->objects = objLeft;
        cout << "size " << objLeft.size() << "leaf" << "\n";
        return current;
    }
    else if(objLeft.size() == 0){
        current -> leaf = true;
        current ->objects = objRight;
        cout << "size " << objRight.size() << "leaf" << "\n";
        return current;
    }

    //split objects into 2 parts
    current -> left = createBVH(objLeft);
    current -> right = createBVH(objRight);
    return current;
}