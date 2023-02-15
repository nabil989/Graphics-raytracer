#include "trimesh.h"
#include <assert.h>
#include <float.h>
#include <string.h>
#include <algorithm>
#include <cmath>
#include <iostream>
#include "../ui/TraceUI.h"
#include "../scene/bvh.h"

extern TraceUI* traceUI;

using namespace std;

Trimesh::~Trimesh()
{
	for (auto m : materials)
		delete m;
	for (auto f : faces)
		delete f;
}

// must add vertices, normals, and materials IN ORDER
void Trimesh::addVertex(const glm::dvec3& v)
{
	vertices.emplace_back(v);
}

void Trimesh::addMaterial(Material* m)
{
	materials.emplace_back(m);
}

void Trimesh::addNormal(const glm::dvec3& n)
{
	normals.emplace_back(n);
}

// Returns false if the vertices a,b,c don't all exist
bool Trimesh::addFace(int a, int b, int c)
{
	int vcnt = vertices.size();

	if (a >= vcnt || b >= vcnt || c >= vcnt)
		return false;

	TrimeshFace* newFace = new TrimeshFace(
	        scene, new Material(*this->material), this, a, b, c);
	newFace->setTransform(this->transform);
	if (!newFace->degen)
		faces.push_back(newFace);
	else
		delete newFace;

	// Don't add faces to the scene's object list so we can cull by bounding
	// box
	//root = createBVHTrimesh(faces);
	return true;
}

// Check to make sure that if we have per-vertex materials or normals
// they are the right number.
const char* Trimesh::doubleCheck()
{
	if (!materials.empty() && materials.size() != vertices.size())
		return "Bad Trimesh: Wrong number of materials.";
	if (!normals.empty() && normals.size() != vertices.size())
		return "Bad Trimesh: Wrong number of normals.";

	return 0;
}

void printTree(bvhNodeTrimesh * cur, int level, int &leaf, int & leafsum, int &leafmax, int&maxlevel){
	maxlevel = max(level, maxlevel);
	for(int i = 0; i < level; i++){
		cout << "----";
	}
	if(cur -> leaf){
		cout << "level " << level << "leaf, size " << cur->faces.size() << "\n";
		leaf++;
		leafsum += cur -> faces.size();
		int temp = cur ->faces.size();
		leafmax = glm::max(leafmax,temp);
		return;
	}
	cout << "level " << level << "inner" << "\n";
	printTree(cur -> left, level + 1, leaf, leafsum, leafmax, maxlevel);
	printTree(cur -> right, level + 1, leaf, leafsum, leafmax, maxlevel);
}

void Trimesh::generateTree(){
	root = createBVHTrimesh(faces);
	int l = 0;
	int ls = 0;
	int lm = 0;
	int ml = 0;
	printTree(root, 0, l, ls, lm, ml);
	cout << l << " " << ls << " " << lm << " " << ml << "\n";
}

void recurse(ray&r, bvhNodeTrimesh * cur, isect & i, int& count){
	if(cur -> leaf){
		//cout << "in leaf, leafsize" << cur->faces.size() << "\n";
		bool have_one;
		for(auto face: cur->faces) {
			isect c;
			count++;
			if( face->intersectLocal(r, c) ) {
				if(!have_one || (c.getT() < i.getT())) {
					i = c;
					have_one = true;
				}
			}
		}
		if(!have_one){
			i.setT(1000.0);
			//cout << "leaf no intesec" << "\n";
		}
	}
	else{
		double tmin = 0.0;
		double tmax = 0.0;
		double lt = -1.0;
		double rt = -1.0;
		if(cur->left->box->intersect(r, tmin, tmax)){
			//cout << "left box intersect " << lt << "\n";
			lt = tmin;
		}
		if(cur->right->box->intersect(r, tmin, tmax)){
			//cout << "right box intersect " << rt << "\n";
			rt = tmin;
		}
		if(lt == -1.0 && rt == -1.0){
			return;
		}
		else if(lt == -1.0 || rt == -1.0){
			//cout << "one edge";
			if(lt > rt){
				recurse(r, cur->left, i, count);
			}
			else{
				recurse(r, cur->right, i, count);
			}
		}
		else{
			recurse(r,cur->left, i, count);
			recurse(r, cur->right, i, count);
		}
	}
}

bool Trimesh::intersectLocal(ray& r, isect& i) const
{
	//TODO: instead of looping through faces, traverse bvh to find the face
	isect n = isect();
	n.setT(1000.0);
	i.setT(1000.0);
	int count = 0;
	recurse(r, root, i, count);
	//cout << count << "\n";
	//cout << i.getT() << " ";
	return i.getT() < 1000.0;

	// bool have_one = false;
	// for (auto face : faces) {
	// 	count --;
	// 	isect cur;
	// 	if (face->intersectLocal(r, cur)) {
	// 		if (!have_one || (cur.getT() < i.getT())) {
	// 			i = cur;
	// 			have_one = true;
	// 		}
	// 	}
	// }
	// cout << count << "\n";
	// if (!have_one)
	// 	i.setT(1000.0);
	// return have_one;
}

glm::dvec3 TrimeshFace::centrioid(){
	glm::dvec3 a_coords = parent->vertices[ids[0]];
    glm::dvec3 b_coords = parent->vertices[ids[1]];
    glm::dvec3 c_coords = parent->vertices[ids[2]];
	return (a_coords + b_coords + c_coords)/3.0;
}

bool TrimeshFace::intersect(ray& r, isect& i) const
{
	return intersectLocal(r, i);
}

// Intersect ray r with the triangle abc.  If it hits returns true,
// and put the parameter in t and the barycentric coordinates of the
// intersection in u (alpha) and v (beta).
bool TrimeshFace::intersectLocal(ray& r, isect& i) const
{
	// YOUR CODE HERE

	// FIXME: Add ray-trimesh intersection
	glm::dvec3 a_coords = parent->vertices[ids[0]];
    glm::dvec3 b_coords = parent->vertices[ids[1]];
    glm::dvec3 c_coords = parent->vertices[ids[2]];
	glm::dvec3 edgeOne = b_coords-a_coords;
	glm::dvec3 edgeTwo = c_coords-a_coords;

	glm::dvec3 pvec = glm::cross(r.getDirection(), edgeTwo);
	double det = glm::dot(edgeOne, pvec);

	if(det < RAY_EPSILON && det > -RAY_EPSILON) {
		//cout << "no intersect" << "\n";
		return false;
	}

	double inv_det = 1.0 / det;
	glm::dvec3 tvec = r.getPosition() - a_coords;

	double u = glm::dot(tvec, pvec) * inv_det;
    if (u < 0 || u > 1){
		//cout << "no intersect" << "\n";
		return false;
	} 
    glm::dvec3 qvec = glm::cross(tvec, edgeOne);
    double v = glm::dot(r.getDirection(), qvec) * inv_det;
    if (v < 0 || u + v > 1){
		//cout << "no intersect" << "\n";
		return false;
	}
    double t = glm::dot(edgeTwo, qvec) * inv_det;
	if(t < RAY_EPSILON){
		return false;
	}
	
	i.setT(t);
	i.setObject(parent);
	glm::dvec2 uv(u, v);
	i.setUVCoordinates(uv);
	
	if(parent->vertNorms){
		glm::dvec3 centerOfMass = (1.0-u-v) * parent->normals[ids[0]] + parent->normals[ids[1]] * u + parent->normals[ids[2]] * v;
		i.setN(centerOfMass * 1.0);
	}
	else{
		i.setN(normal * 1.0);
	}
	if(parent->materials.size() > 0){
		double a = (1.0-u-v);
		Material m = a * (*parent->materials[ids[0]]);
		m+= u * (*parent->materials[ids[1]]);
		m+=  v * (*parent->materials[ids[2]]);
		i.setMaterial(m);
	}
	else{
		i.setMaterial(parent->getMaterial());
	}
	//cout << "intersect" << "\n";
	return true;
	// return false;
}

// Once all the verts and faces are loaded, per vertex normals can be
// generated by averaging the normals of the neighboring faces.
void Trimesh::generateNormals()
{
	int cnt = vertices.size();
	normals.resize(cnt);
	std::vector<int> numFaces(cnt, 0);

	for (auto face : faces) {
		glm::dvec3 faceNormal = face->getNormal();

		for (int i = 0; i < 3; ++i) {
			normals[(*face)[i]] += faceNormal;
			++numFaces[(*face)[i]];
		}
	}

	for (int i = 0; i < cnt; ++i) {
		if (numFaces[i])
			normals[i] /= numFaces[i];
	}

	vertNorms = true;

}

