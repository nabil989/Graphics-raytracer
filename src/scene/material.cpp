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

const bool proceduralTextures = false;

/*
	texture options
	0 : checkerboard
	1 : wood
	2 : coral/wavy
*/
const int textureSelection = 2;

//change to generate different textures
const int noiseOffset = 10;


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
	
	bool check = false;
	for ( const auto& pLight : scene->getAllLights() )
	{
		check = true;
		glm::dvec3 d = pLight -> getDirection(Q);
		
		double distAtten = pLight->distanceAttenuation(Q);

		glm::dvec3 ln = kd(i) * glm::abs(glm::dot(d, N)) * distAtten;

		glm::dvec3 reflectionRay = glm::reflect(d, N) * -1.0;
		glm::dvec3 specular = ks(i) * pow(max(glm::dot(r.getDirection() * -1.0, reflectionRay), 0.0), shininess(i)) * distAtten;
		glm::dvec3 final = (specular + ln) * pLight -> shadowAttenuation(r, Q);

		l += final * pLight -> getColor() + ke(i);
	}
	if(!check){
		l += ke(i);
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

	double x = coord[0] * getWidth();
	double y = coord[1] * getHeight();
	// bilinear interpolation 
	int x1 = glm::floor(x);
	int x2 = (x1 + 1) %  getWidth();
	int y1 = glm::floor(y);
	int y2 = (y1 + 1) % getHeight();
	if(coord[0] == 1){
		x2 = x1;
	}

	if(coord[1] == 1){
		y2 = y1;
	}

	double s = x - x1;
	double t = y - y1;

	glm::dvec3 c1 = (1 - s) * getPixelAt(x1, y1) + s * getPixelAt(x2, y1);
	
	glm::dvec3 c2 = (1 - s) * getPixelAt(x1, y2) + s * getPixelAt(x2, y2);

	return (1 - t) * c1 + t * c2;
}

//generate a seeded value betwene 0 and 1 || very bad code
double gradient(int x, int y){
	double seed = M_PI;
	double seed2 = M_LOG2E;
	//idk this makes something kinda random hopefully
	double num = (x *x * seed + y * y *seed2 + seed)*10000 + 1.0/noiseOffset;
	//cout << "gradient" << " " << x << " " << y << " "<< num - glm::floor(num) << "\n";
	return num - glm::floor(num);
}

double interpolate(double lo, double hi, double t){
	// double ft = d * M_PI;
	// double f = (1- glm::cos(ft) * 0.5);
	// return a * (1-f) + b * f;
	//cout << "int " << lo << " " << hi << " " << t << " " << lo * (1-t) + hi * t << "\n";   
	return lo + t * (hi - lo);
}

double fade(double t){
	//perlin noise optimized ease function
	return ((6 * t - 15) * t + 10) * t * t * t;
}

double noise(double x, double y){
	//calculate gradient but needs to be larger steps?
	x/=30;
	y/=30;
	int x0 = floor(x);
	int y0 = floor(y);
	x0 %= 1000;
	y0 %= 1000;
	int x1 = x0 + 1;
	int y1 = y0 + 1;

	//cout << "grid cell" << x0 << " " << y0 << " " << x1 << " " << y1 << "\n";

	double dx = 1- (x - x0);
	double dy = y - y0;
	//get the four corners
	double br = gradient(x0, y0);
	double tr = gradient(x0, y1);
	double bl = gradient(x1, y0);
	double tl = gradient(x1, y1);

	//interpolation
	double u = fade(dx);
	double v = fade(dy);
	double ix = interpolate(bl, tl, v);
	double ix1 = interpolate(br, tr, v);
	double ans = interpolate(ix, ix1, u);
	
	return ans;
}



glm::dvec3 TextureMap::getPixelAt(int x, int y) const
{
	// YOUR CODE HERE
	//
	// In order to add texture mapping support to the
	// raytracer, you need to implement this function.
	glm::dvec3 col(0,0,0);

	if(proceduralTextures){
		int scale = 50;
		col = {1,1,1};
		if(textureSelection == 0){
			double multx = (glm::sin(x * scale) + 1) * 0.5;
			double multy = (glm::sin(y * scale) + 1) * 0.5;
			double mult = 1;
			if(multx < 0.5 && multy < 0.5 || multx > 0.5 && multy > 0.5){
				mult = 0;
			}
			col *= mult;
		}
		if(textureSelection == 1){
			col = {255.0, 177.0, 128.0};
			col/= 255.0;
			double val = noise(x * 0.7,y *0.7) * 9;
			col *= val - (int) val;
			//(sin(x*y + (noise(x*2, y*2)*20) + noise(x*3, x*3) * 10)+1) * 0.5;
		}
		if(textureSelection == 2){
			col = {252.0, 173.0, 173.0};
			col/= 255.0;
			col *= (sin((x + noise(x,y) * 50) * 2 * M_PI/10.0) + 1) / 2.0;
		}
	}
	else{
		int start = ( x + y * width ) * 3;
		
		for(int i = 0; i < 3; i++){
			col[i] = data[start + i] / 255.0;
		}
	}
	
	return col;
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
