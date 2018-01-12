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

	// Add lights

	// Create Raster
	Pixels pixels(800, 800);

	// Create Camera
	camera.pixels = pixels;

	// backgroundVec3 =
}

bool Scene::parseArgs(int argc, char**) //argv
{
	if(argc <= 1)
		return false;
	return true;
}

void Scene::trace()
{
	unsigned threadCount = 1;
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
		threads[i] = std::thread(&Scene::traceSection, this, std::ref(camera), Scene::createRays(i, threadCount));

	// Join threads
	for(auto& thread : threads)
		thread.join();

	std::cout << "Finished\n";
}

std::map<unsigned, Ray> Scene::createRays(unsigned threadID, unsigned threadCount)
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

void Scene::traceSection(Camera& camera, std::map<unsigned, Ray> rays)
{
	  unsigned depth = 1;
		Sphere sphere(100.f, Vec3(400, 600, -250));
		Plane plane(Vec3(0,1,0), Vec3(0, 0, 0));
		Triangle t(Vec3(0,600,-250), Vec3(100,700,-250), Vec3(200,450,-250));

	  for(auto& r : rays)
		{
			if(plane.raycast(r.second) && r.second.t >= 0)
	  		camera.pixels.set(r.first, Vec3(1, 0, 0));

			if(sphere.raycast(r.second) && r.second.t >= 0)
				camera.pixels.set(r.first, Vec3(1, 0, 0));

			if(t.raycast(r.second) && r.second.t >= 0)
				camera.pixels.set(r.first, Vec3(1, 0, 0));
		}
}

// Vec3 Scene::raycast(Ray ray, int depth)
// {
// 	Ray closestRay;
// 	Ray testRay;
// 	Tracable* geom = nullptr;
// 	for(unsigned i = 0; i < geometry.size(); ++i)
// 	{
// 		testRay = ray;
// 		if(geometry[i].raycast(testRay) && testRay.t < closestRay.t)
// 		{
// 			geom = &geometry[i];
// 			closestRay = testRay;
// 		}
// 	}
//
// 	Vec3 color = backgroundColor;
// 	if(geom != nullptr)
// 	{
// 		// Test if object is in shadow
// 		if(shadow(ray))
// 		{
// 			 color = geom->material.ambient;
// 		}
// 		else
// 		{
// 			color = blinnPhong(closestRay, *geom);
// 		}
//
// 		// Recurse
// 		if(depth > 0)
// 		{
// 			color += reflect(Ray ray, depth - 1);
// 			color += refract(Ray ray, depth - 1);
// 		}
// 	}
// 	return color;
// }

void Scene::save()
{
	std::cout << "Saving Image...";
	camera.pixels.save("output.ppm");
	std::cout << "Finished\n";
}
