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
	geometry.emplace_back(new Sphere(100.f, Vec3(350, 100, -400)));
	geometry.emplace_back(new Plane(Vec3(0,1,0), Vec3(0, 0, 0)));
	geometry.emplace_back(new Triangle(Vec3(0,0,-700), Vec3(400,500,-700), Vec3(800,0,-700)));

	geometry[1]->material.diffuseColor.set(0, .6f, 0);
	geometry[2]->material.diffuseColor.set(.7f, .6f, 0);

	// Add lights
	lights.push_back(Light(Vec3(.6, 0.2, .8), Vec3(0, 300, -500)));
	lights.push_back(Light(Vec3(.8, 0.2, .6), Vec3(800, 300, -500)));

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
	  unsigned depth = 2;
	  for(auto& r : pixelRayMap)
		{
			Vec3 color = calculateColor(r.second, depth);
			color.x = color.x < 0.f ? 0.f : color.x > 1.f ? 1.f : color.x;
			color.y = color.y < 0.f ? 0.f : color.y > 1.f ? 1.f : color.y;
			color.z = color.z < 0.f ? 0.f : color.z > 1.f ? 1.f : color.z;
			camera.pixels.set(r.first, color);
			//TODO: account for multiple rays per pixel
		}
}

int Scene::castRay(Ray& ray)
{
	int geomIndex = -1;
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
	return geomIndex;
}

Vec3 Scene::calculateColor(Ray ray, int depth)
{
			// TODO: read geom / lights from file
			// TODO: add reflections
			// TODO: light attenuation

	// Recursion limit
	Vec3 color(0, 0, 0);
	if(depth == 0)
		return color;

	// Calculate shading
	int geomIndex = castRay(ray);
	if(geomIndex != -1)
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
			L = l.position - collisionPoint;
			float distToLight = L.length();
			L = normalize(L);
			Ray shadowRay(l.position, normalize(collisionPoint - l.position));
			if(castShadowRay(shadowRay, distToLight, geomIndex))
				shadingColor = shadingColor + geom->material.ambientColor();
			else
				shadingColor = shadingColor + geom->material.blinnPhong(N, V, L, l.color);
		}

		// Calculate reflection and refraction rays
		Ray reflectedRay(collisionPoint, normalize(reflect(-1.f*ray.dir,N)), .01f);
		Ray refractedRay;
		Vec3 reflectedColor = calculateColor(reflectedRay, depth - 1);
		Vec3 refractedColor = calculateColor(refractedRay, depth - 1);
		float reflCoef 		= 0.f;
		float refrCoef 		= 0.f;
		float shadingCoef = 1.f;
		color = reflCoef    * reflectedColor +
						refrCoef    * refractedColor +
						shadingCoef * shadingColor;


	}
	else
		color = backgroundColor;

	return color;
}

bool Scene::castShadowRay(Ray& ray, float distToLight, int origGeomIndex)
{
	int geomIndex = castRay(ray);
	return geomIndex != -1 && geomIndex != origGeomIndex && ray.t <= distToLight;
}

void Scene::save()
{
	std::cout << "Saving Image...";
	camera.pixels.save("output.ppm");
	std::cout << "Finished\n";
}
