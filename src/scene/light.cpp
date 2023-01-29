#include <cmath>
#include <iostream>

#include "light.h"
#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>


using namespace std;

double DirectionalLight::distanceAttenuation(const glm::dvec3& P) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	// cout << "directional light DA\n";
	return 1.0;
}


glm::dvec3 DirectionalLight::shadowAttenuation(const ray& r, const glm::dvec3& p) const
{
	// YOUR CODE HERE:
	// You should implement shadow-handling code here.
	return glm::dvec3(1.0, 1.0, 1.0);
}

glm::dvec3 DirectionalLight::getColor() const
{
	return color;
}

glm::dvec3 DirectionalLight::getDirection(const glm::dvec3& P) const
{
	return -orientation;
}


double PointLight::distanceAttenuation(const glm::dvec3& P) const
{

	// YOUR CODE HERE

	// You'll need to modify this method to attenuate the intensity 
	// of the light based on the distance between the source and the 
	// point P.  For now, we assume no attenuation and just return 1.0

	//return min( 1, 1/( a + b d + c d^2 ) )
	double d = glm::distance(P, position);
	//cout << constantTerm << " " <<  linearTerm << " " << quadraticTerm << "\n";
	double val = constantTerm + linearTerm * d + quadraticTerm * d * d;
	// cout << val << "\n";
	return std::min(1.0,1/val);
}

glm::dvec3 PointLight::getColor() const
{
	return color;
}

glm::dvec3 PointLight::getDirection(const glm::dvec3& P) const
{
	return glm::normalize(position - P);
}


glm::dvec3 PointLight::shadowAttenuation(const ray& r, const glm::dvec3& p) const
{
	// YOUR CODE HERE:
	// You should implement shadow-handling code here.

	//r = ray, p = intersection point
	glm::dvec3 d = getDirection(p);
	isect i;
	ray r2(glm::dvec3(0,0,0), glm::dvec3(0,0,0), glm::dvec3(1,1,1), ray::VISIBILITY);
	r2.setPosition(p + 0.001 * d);
	r2.setDirection(d);

	if(scene->intersect(r2, i)){
		double distance = glm::distance(p, position);
		double distanceIntersect = glm::distance(p, r2.at(i.getT()));
		if(distanceIntersect > distance){
			return glm::dvec3(1,1,1);
		}
		return glm::dvec3(0,0,0);
	}

	return glm::dvec3(1,1,1);
}

#define VERBOSE 0

