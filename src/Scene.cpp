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
	geometry.emplace_back(new Triangle(Vec3(0,600,-250), Vec3(100,700,-250), Vec3(200,450,-250)));

	// Add lights

	// Create Raster
	Pixels pixels(800, 800);

	// Create Camera
	camera.pixels = pixels;

	// Set background color
	backgroundColor.set(1, 1, 1);
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
			camera.pixels.set(r.first, castRay(r.second, depth));
			//TODO: account for multiple rays per pixel
		}
}

Vec3 Scene::castRay(const Ray origRay, int depth)
{
	Vec3 color(0, 0, 0);
	if(depth == 0)
		return color;

	// Detect closest geometry
	Ray ray;
	int geomIndex = -1;
	{
		for(unsigned i = 0; i < geometry.size(); ++i)
		{
			Ray tempRay = origRay;
			if(geometry[i]->raycast(tempRay) && ray.t > tempRay.t && tempRay.t >= 0)
			{
				ray = tempRay;
				geomIndex = i;
			}
		}
	}
	// Calculate shading
	if(geomIndex != -1)
	{
		color.set(0, 0, 0);

		// Calculate reflection and refraction rays
		Ray reflectedRay;
		Ray refractedRay;

		Vec3 reflectedColor = castRay(reflectedRay, depth - 1);
		Vec3 refractedColor = castRay(refractedRay, depth - 1);
		Vec3 shadingColor(0, 0, 0);

		//for each light
		//{
			//if(castShadowRay(/*shadow ray*/))
				//shadingColor += amb
			//else
				//shadingColor += blinnPhong
		//}

		shadingColor = ray.normal;

		//Coefs should add to 1
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

bool Scene::castShadowRay(Ray ray)
{

}

void Scene::save()
{
	std::cout << "Saving Image...";
	camera.pixels.save("output.ppm");
	std::cout << "Finished\n";
}
