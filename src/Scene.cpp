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

	// Create Camera
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
		threads[i] = std::thread(&Scene::traceSection, this, std::ref(camera), Scene::createRays());

	// Join threads
	for(auto& thread : threads)
		thread.join();

	std::cout << "Finished\n";
}

std::map<unsigned, Ray> Scene::createRays()
{
	std::map<unsigned, Ray> pixelRayMap;
	return pixelRayMap;
}

void Scene::traceSection(Camera& camera, std::map<unsigned, Ray> rays)
{
	std::cout << &camera << rays.size() << "\n";
}

void Scene::save()
{
	std::cout << "Saving Image...";
	//TODO save an image
	std::cout << "Finished\n";
}
