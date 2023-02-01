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
	glm::dvec3 d = getDirection(p);
	isect i;
	ray r2(glm::dvec3(0,0,0), glm::dvec3(0,0,0), glm::dvec3(1,1,1), ray::SHADOW);
	r2.setDirection(d);
	r2.setPosition(p + RAY_EPSILON * d);
	glm::dvec3 ans(1,1,1);
	bool done = false;
	while(!done){
		isect i;
		if(scene->intersect(r2, i)){
			const Material& m = i.getMaterial();
			if(m.Trans()) {
				glm::dvec3 ipos = r2.at(i.getT());
				ray dray(glm::dvec3(0,0,0), glm::dvec3(0,0,0), glm::dvec3(1,1,1), ray::SHADOW);
				dray.setDirection(d);
				dray.setPosition(ipos + RAY_EPSILON * d);
				isect i2;
				if(scene->intersect(dray, i2)){
					double dist = glm::distance(dray.at(i2.getT()), ipos);
					ans -= glm::dvec3(1) - glm::pow(m.kt(i) , glm::dvec3(dist));
					r2.setPosition(dray.at(i2.getT()) + RAY_EPSILON * d);
				}
				else{
					return max(glm::dvec3(0,0,0), ans);
				}
			}
			else{
				return glm::dvec3(0,0,0);
			}
		}
		else{
			return max(glm::dvec3(0,0,0), ans);
		}
	}
	return ans;
	//return glm::dvec3(1,1,1);
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
	glm::dvec3 d = getDirection(p);
	isect i;
	ray r2(glm::dvec3(0,0,0), glm::dvec3(0,0,0), glm::dvec3(1,1,1), ray::SHADOW);
	r2.setDirection(d);
	r2.setPosition(p + RAY_EPSILON * d);
	glm::dvec3 ans(1,1,1);
	//return ans;
	bool done = false;
	while(!done){
		isect i;
		if(scene->intersect(r2, i)){
			const Material& m = i.getMaterial();
			if(m.Trans()) {
				glm::dvec3 ipos = r2.at(i.getT());
				ray dray(glm::dvec3(0,0,0), glm::dvec3(0,0,0), glm::dvec3(1,1,1), ray::SHADOW);
				dray.setDirection(d);
				dray.setPosition(ipos + RAY_EPSILON * d);
				isect i2;
				if(scene->intersect(dray, i2)){
					double dist = glm::distance(dray.at(i2.getT()), ipos);
					ans -= glm::dvec3(1) - glm::pow(m.kt(i) , glm::dvec3(dist));
					r2.setPosition(dray.at(i2.getT()) + RAY_EPSILON * d);
				}
				else{
					return max(glm::dvec3(0,0,0), ans);
				}
			}
			else{
				double distance = glm::distance(p, position);
				double distanceIntersect = glm::distance(p, r2.at(i.getT()));
				if(distanceIntersect < distance){
					return glm::dvec3(0,0,0);
				}
				return max(glm::dvec3(0,0,0), ans);
			}
		}
		return max(glm::dvec3(0,0,0), ans);
	}
	return ans;
}

#define VERBOSE 0

