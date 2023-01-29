#include "material.h"
#include "../ui/TraceUI.h"
#include "light.h"
#include "ray.h"
extern TraceUI* traceUI;

#include <glm/gtx/io.hpp>
#include <iostream>
#include "../fileio/images.h"

using namespace std;
extern bool debugMode;

Material::~Material()
{
}

// Apply the phong model to this point on the surface of the object, returning
// the color of that point.
glm::dvec3 Material::shade(Scene* scene, const ray& r, const isect& i) const
{
	// YOUR CODE HERE

	// For now, this method just returns the diffuse color of the object.
	// This gives a single matte color for every distinct surface in the
	// scene, and that's it.  Simple, but enough to get you started.
	// (It's also inconsistent with the phong model...)

	// Your mission is to fill in this method with the rest of the phong
	// shading model, including the contributions of all the light sources.
	// You will need to call both distanceAttenuation() and
	// shadowAttenuation()
	// somewhere in your code in order to compute shadows and light falloff.
	if( debugMode )
		std::cout << "Debugging Phong code..." << std::endl;

	glm::dvec3 l = ka(i) * scene->ambient();
	glm::dvec3 Q = r.at(i.getT());
	glm::dvec3 N = i.getN();
	glm::dvec3 V = r.getDirection();
	for ( const auto& pLight : scene->getAllLights() )
	{
		//l = pLight -> shadowAttenuation(r, Q);
		// //if(pLight -> shadowAttenuation(r, Q, scene))

		// light in, W_in vec.
		glm::dvec3 d = pLight -> getDirection(Q);
		isect i2;
		ray r2(glm::dvec3(0,0,0), glm::dvec3(0,0,0), glm::dvec3(1,1,1), ray::VISIBILITY);
		r2.setPosition(Q - 0.001*d);
		r2.setDirection(d);
		double distAtten = pLight->distanceAttenuation(Q);

		glm::dvec3 ln = kd(i) * abs(glm::dot(d, N)) * distAtten;
		
		// identity matrix
		glm::mat3 i3(1.0f);
		glm::mat3 matrix = glm::outerProduct(N, N);
		glm::mat3 scaledMatrix = matrix * 2.0f;
		glm::dvec3 reflectionRay = (i3 - scaledMatrix) * d;
		glm::dvec3 specular = ks(i) * pow(max(glm::dot(r.getPosition(), reflectionRay), 0.0), shininess(i)) * distAtten;
		//glm::dvec3 ls = ks(i) * max
		l += specular;
		l += ln;
		// if(scene->intersect(r2, i2)){
		// 	//need to check if intersection before light or behind light
		// 	//double d1 = glm::distance(Q, r2.at(i2.getT()));
		// 	//in shadow i guess
		// 	return glm::dvec3(0,0,0);
		// }
		// glm::mat3 i3(1.0f);
		// glm::mat3 matrix = i3  glm::outerProduct(N, N);
		// glm::dvec3 sp = ks(i) * max(glm::dot(r.getPosition(), )
		// return kd(i);
		//double atten = pLight->distanceAttenuation(Q);


		// glm::dvec3 sp = ks(i) * glm::max(glm::dot(r.getPosition(), ), glm::vec3(0,0,0));
		
		// // cout << atten << "\n";
		//l += atten * (kd(i));
		// cout << l;
	}
	return l;
}

TextureMap::TextureMap(string filename)
{
	data = readImage(filename.c_str(), width, height);
	if (data.empty()) {
		width = 0;
		height = 0;
		string error("Unable to load texture map '");
		error.append(filename);
		error.append("'.");
		throw TextureMapException(error);
	}
}

glm::dvec3 TextureMap::getMappedValue(const glm::dvec2& coord) const
{
	// YOUR CODE HERE
	//
	// In order to add texture mapping support to the
	// raytracer, you need to implement this function.
	// What this function should do is convert from
	// parametric space which is the unit square
	// [0, 1] x [0, 1] in 2-space to bitmap coordinates,
	// and use these to perform bilinear interpolation
	// of the values.

	return glm::dvec3(1, 1, 1);
}

glm::dvec3 TextureMap::getPixelAt(int x, int y) const
{
	// YOUR CODE HERE
	//
	// In order to add texture mapping support to the
	// raytracer, you need to implement this function.

	return glm::dvec3(1, 1, 1);
}

glm::dvec3 MaterialParameter::value(const isect& is) const
{
	if (0 != _textureMap)
		return _textureMap->getMappedValue(is.getUVCoordinates());
	else
		return _value;
}

double MaterialParameter::intensityValue(const isect& is) const
{
	if (0 != _textureMap) {
		glm::dvec3 value(
		        _textureMap->getMappedValue(is.getUVCoordinates()));
		return (0.299 * value[0]) + (0.587 * value[1]) +
		       (0.114 * value[2]);
	} else
		return (0.299 * _value[0]) + (0.587 * _value[1]) +
		       (0.114 * _value[2]);
}
