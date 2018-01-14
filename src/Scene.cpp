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

	// Add geometry
	geometry.emplace_back(new Sphere(100.f, Vec3(600, 600, -250)));
	geometry.emplace_back(new Plane(Vec3(0,1,0), Vec3(0, 0, 0)));
	geometry.emplace_back(new Triangle(Vec3(0,10,-700), Vec3(400,500,-700), Vec3(800,10,-700)));

	// Add lights
	lights.push_back(Light(Vec3(1, 1, 1), Vec3(400, 100, -400)));

	// Create Raster
	Pixels pixels(800, 800);

	// Create Camera
	camera.pixels = pixels;

	// Set background color
	backgroundColor.set(.2, .2, .4);
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
		threads[i] = std::thread(&Scene::traceSection, this, std::ref(camera), Scene::createPixelRayMap(i, threadCount));

	// Join threads
	for(auto& thread : threads)
		thread.join();

	std::cout << "Finished\n";
}

std::map<unsigned, Ray> Scene::createPixelRayMap(unsigned threadID, unsigned threadCount)
{
	std::map<unsigned, Ray> pixelRayMap;
	unsigned width = camera.pixels.width();
	unsigned height = camera.pixels.height();

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
			pixelRayMap[x+y*width] = camera.createRay(x, y);

	return pixelRayMap;
}

void Scene::traceSection(Camera& camera, std::map<unsigned, Ray> pixelRayMap)
{
	  unsigned depth = 1;
	  for(auto& r : pixelRayMap)
		{
			camera.pixels.set(r.first, calculateColor(r.second, depth));
			//TODO: account for multiple rays per pixel
		}
}

bool Scene::castRay(Ray& ray, int& geomIndex)
{
	geomIndex = -1;
	Ray origRay = ray;
	for(unsigned i = 0; i < geometry.size(); ++i)
	{
		Ray tempRay = origRay;
		if(geometry[i]->raycast(tempRay) && ray.t > tempRay.t && tempRay.t >= 0)
		{
			ray = tempRay;
			geomIndex = i;
		}
	}
	return geomIndex != -1;
}

Vec3 Scene::calculateColor(Ray ray, int depth)
{
			// TODO: handle multiple lights (clamp after summation?)
			// TODO: read geom / lights from file
			// TODO: refactor blinnPhong to method
			// TODO: refactor ray collision to method
			// TODO: add reflections
			// TODO: light attenuation

	// Recursion limit
	Vec3 color(0, 0, 0);
	if(depth == 0)
		return color;

	// Calculate shading
	int geomIndex;
	if(castRay(ray, geomIndex))
	{
		Vec3 collisionPoint = ray.intersection();
		auto& geom = geometry[geomIndex];

		// Calculate light contribution
		Vec3 N = ray.normal;
		Vec3 V = normalize(camera.location - collisionPoint);
		Vec3 L;
		Vec3 shadingColor(0, 0, 0);
		for(Light& l : lights)
		{
			if(castShadowRay(Ray(collisionPoint, normalize(l.position-collisionPoint))))
				shadingColor = shadingColor + geom->material.ambientColor();
			else
			{
				L = normalize(l.position - collisionPoint);
				shadingColor = shadingColor + geom->material.blinnPhong(N, V, L, l.color);
			}
		}

		// Calculate reflection and refraction rays
		Ray reflectedRay;
		Ray refractedRay;
		Vec3 reflectedColor = calculateColor(reflectedRay, depth - 1);
		Vec3 refractedColor = calculateColor(refractedRay, depth - 1);
		float reflCoef 		= 0;//1.f/3.f;
		float refrCoef 		= 0;//1.f/3.f;
		float shadingCoef = 1.f;
		color = reflCoef    * reflectedColor +
						refrCoef    * refractedColor +
						shadingCoef * shadingColor;
	}
	else
		color = backgroundColor;

	return color;
}

bool Scene::castShadowRay(Ray)
{
	return false;
}

void Scene::save()
{
	std::cout << "Saving Image...";
	camera.pixels.save("output.ppm");
	std::cout << "Finished\n";
}
