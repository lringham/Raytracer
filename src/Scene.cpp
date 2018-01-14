#include <thread>
#include <iostream>
#include "Scene.h"
#include "Ray.h"

Scene::Scene(int argc, char** argv)
{
	std::cout << "Loading...";

	// Parse
	if(parseArgs(argc, argv))
	{
		std::cout << "Finished\n";
	}
	else
	{
		std::cout << "Failed\n";
		throw -1;
	}

	// Add _geometry
	_geometry.emplace_back(new Sphere(100.f, Vec3(600, 100, -400)));
	_geometry.emplace_back(new Plane(Vec3(0,1,0), Vec3(0, 0, 0)));
	_geometry.emplace_back(new Triangle(Vec3(0,0,-700), Vec3(400,500,-700), Vec3(800,0,-700)));
	_geometry[1]->_material._diffuseColor.set(0, 1.f, 0);
	_geometry[2]->_material._diffuseColor.set(1.f, 1.f, 0);

	// Add _lights
	_lights.push_back(Light(Vec3(1, 1, 1), Vec3(400, 500, -500)));
	_lights.push_back(Light(Vec3(.8, 0.2, .6), Vec3(800, 300, -500)));

	// Create Camera
	//0.785398f
	_camera.init(Vec3(400.f, 400.f, 1000.f), 1.5708f, 3000.f, Pixels(800, 600));

	// Set background color
	_backgroundColor.set(.2, .2, .4);
}

bool Scene::parseArgs(int argc, char**) //argv
{
	if(argc <= 1)
		return false;
	return true;
}

void Scene::trace()
{
	unsigned threadCount = 0;
	if(threadCount == 0) // Automatically determine threads
	{
		threadCount = std::thread::hardware_concurrency();
		if(threadCount == 0) // Cannot be determined
			threadCount = 1;
	}

	std::cout << "Thread count: " << threadCount << "\n";
	std::cout << "Tracing...";

	// Create threads and trace
	std::vector<std::thread> threads(threadCount);
	for(unsigned i = 0; i < threadCount; ++i)
		threads[i] = std::thread(&Scene::traceSection, this, std::ref(_camera), Scene::createPixelRayMap(i, threadCount));

	// Join threads
	for(auto& thread : threads)
		thread.join();

	std::cout << "Finished\n";
}

std::map<unsigned, std::vector<Ray>> Scene::createPixelRayMap(unsigned threadID, unsigned threadCount)
{
	std::map<unsigned, std::vector<Ray>> pixelRayMap;
	unsigned width = _camera._pixels.width();
	unsigned height = _camera._pixels.height();

	unsigned rem = width % threadCount;
	unsigned xRes = width / threadCount;
	unsigned startX, endX;

	if(rem != 0)
	{
		if(threadID >= rem)
		{
			startX = rem * (xRes + 1) + (threadID-rem) * xRes;
			endX = startX + xRes;
		}
		else
		{
			startX = threadID * (xRes + 1);
			endX = startX + xRes + 1;
		}
	}
	else
	{
		startX = threadID * xRes;
		endX	 = startX + xRes;
	}

	for(unsigned x = startX; x < endX; ++x)
		for(unsigned y = 0; y < height; ++y)
			pixelRayMap[x+y*width] = _camera.createRays(x, y);

	return pixelRayMap;
}

void Scene::traceSection(Camera& _camera, std::map<unsigned, std::vector<Ray>> pixelRayMap)
{
	  unsigned depth = 8;
	  for(auto& r : pixelRayMap)
		{
			Vec3 color(0,0,0);
			for(auto& ray : r.second)
			{
				color = color + calculateColor(ray, depth);
			}

			float rayCount = static_cast<float>(r.second.size());
			color._r /= rayCount;
			color._g /= rayCount;
			color._b /= rayCount;
			color._r = color._r < 0.f ? 0.f : color._r > 1.f ? 1.f : color._r;
			color._g = color._g < 0.f ? 0.f : color._g > 1.f ? 1.f : color._g;
			color._b = color._b < 0.f ? 0.f : color._b > 1.f ? 1.f : color._b;

			_camera._pixels.set(r.first, color);
		}
}

int Scene::castRay(Ray& ray)
{
	int geomIndex = -1;
	Ray origRay = ray;
	for(unsigned i = 0; i < _geometry.size(); ++i)
	{
		Ray tempRay = origRay;
		if(_geometry[i]->raycast(tempRay) && ray._t > tempRay._t && tempRay._t >= 0)
		{
			ray = tempRay;
			geomIndex = i;
		}
	}
	return geomIndex;
}

Vec3 Scene::calculateColor(Ray ray, int depth)
{
			// TODO: read geom / lights from file
			// TODO: add refractions
			// TODO: light attenuation
			// TODO: fresnel factor
			
	Vec3 color(0, 0, 0);

	// Calculate shading
	int geomIndex = castRay(ray);
	if(geomIndex != -1)
	{
		Vec3 collisionPoint = ray.intersection();
		auto& geom = _geometry[geomIndex];

		// Calculate light contribution
		Vec3 N = ray._normal;
		Vec3 V = normalize(_camera._position - collisionPoint);
		Vec3 L;
		Vec3 shadingColor(0, 0, 0);

		for(Light& l : _lights)
		{
			L = l._position - collisionPoint;
			float distToLight = L.length();
			L.normalize();

			Ray shadowRay(collisionPoint, L, .01f/*offset to avoid self collision*/);
			if(castShadowRay(shadowRay, distToLight))
				shadingColor = shadingColor + geom->_material.ambientColor();
			else
				shadingColor = shadingColor + geom->_material.blinnPhong(N, V, L, l._color);
		}

		// Exit if recursive depth is met
		if(depth == 0)
			return shadingColor;

		// Calculate reflection and refraction rays
		Ray reflectedRay(collisionPoint, reflect(normalize(collisionPoint - ray._origin), N), .01f);
		//Ray refractedRay(collisionPoint, refract(normalize(collisionPoint - ray._origin), N), .01f);
		Vec3 reflectedColor = calculateColor(reflectedRay, depth - 1);
		//Vec3 refractedColor = calculateColor(refractedRay, depth - 1);
		float reflCoef 		= .5f;
		float refrCoef 		= 0.f;
		float shadingCoef = .5f;
		color = reflCoef    * reflectedColor +
						shadingCoef * shadingColor;
	}
	else
		color = _backgroundColor;

	return color;
}

bool Scene::castShadowRay(Ray& ray, float distToLight)
{
	int geomIndex = castRay(ray);
	return geomIndex != -1 && ray._t <= distToLight;
}

void Scene::save()
{
	std::cout << "Saving Image...";
	_camera._pixels.save("output.ppm");
	std::cout << "Finished\n";
}
