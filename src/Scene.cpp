#include <thread>
#include <iostream>
#include <yaml-cpp/yaml.h>

#include "Scene.h"
#include "Ray.h"

Scene::Scene(int argc, char** argv) :
	 _name("scene"), _outputName(""), _threadCount(0)
{
	std::cout << "Loading... ";

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
}

bool Scene::parseArgs(int argc, char** argv) //argv
{
	if(argc <= 1 || argc > 3)
	{
		usage();
		return false;
	}
	else if (argc == 3)
	{
		std::string arg = argv[2];
		std::string threadCountStr = "-threads=";
		if(arg.compare(0, threadCountStr.length(), threadCountStr) == 0)
			_threadCount = std::stoi(arg.substr(arg.find(threadCountStr)+threadCountStr.size()));
		else
		{
			usage();
			return false;
		}
	}

	YAML::Node config;
	try
	{
		//
		YAML::Node config = YAML::LoadFile(argv[1]);

		if(config["outputName"])
			_outputName = config["outputName"].as<std::string>();

		if(config["name"])
			_name = config["name"].as<std::string>();

		const YAML::Node& materials = config["materials"];
		for (YAML::const_iterator it = materials.begin(); it != materials.end(); ++it)
		{
		    const YAML::Node& material = *it;
		    // std::cout << "name: " << material["name"].as<std::string>() << "\n";
		    // std::cout << "ks: " << material["ks"].as<std::string>() << "\n";
				// std::cout << "kd: " << material["kd"].as<std::string>() << "\n";
				// std::cout << "ka: " << material["ka"].as<std::string>() << "\n";
				// std::cout << "dColor: " << material["dColor"].as<std::string>() << "\n";
				// std::cout << "specularColor: " << material["specularColor"].as<std::string>() << "\n";
		}

		const YAML::Node& objects = config["objects"];
		for (YAML::const_iterator it = objects.begin(); it != objects.end(); ++it)
		{
		    const YAML::Node& object = *it;
		    std::string type = object["type"].as<std::string>();

				if(type == "sphere")
				{
					_geometry.emplace_back(
						new Sphere(
							object["radius"].as<float>(), Vec3(
								object["position"][0].as<float>(),
								object["position"][1].as<float>(),
								object["position"][2].as<float>())));
				}
				else if(type == "triangle")
				{
					_geometry.emplace_back(
						new Triangle(
							Vec3(object["x0"][0].as<float>(),
									 object["x0"][1].as<float>(),
									 object["x0"][2].as<float>()),
							Vec3(object["x1"][0].as<float>(),
									 object["x1"][1].as<float>(),
									 object["x1"][2].as<float>()),
						  Vec3(object["x2"][0].as<float>(),
									 object["x2"][1].as<float>(),
									 object["x2"][2].as<float>())));
				}
				else if(type == "plane")
				{
					_geometry.emplace_back(
						new Plane(
							Vec3(object["normal"][0].as<float>(),
									 object["normal"][1].as<float>(),
									 object["normal"][2].as<float>()),
							Vec3(object["position"][0].as<float>(),
									 object["position"][1].as<float>(),
									 object["position"][2].as<float>())));
				}

				//std::cout << "material: " << object["material"].as<std::string>() << "\n\n";
		}

		const YAML::Node& lights = config["lights"];
		for (YAML::const_iterator it = lights.begin(); it != lights.end(); ++it)
		{
			const YAML::Node& light = *it;
			_lights.emplace_back(
					Vec3(light["color"][0].as<float>(),
							 light["color"][1].as<float>(),
							 light["color"][2].as<float>()),
					Vec3(light["position"][0].as<float>(),
							 light["position"][1].as<float>(),
							 light["position"][2].as<float>()));
		}

		_camera.init(
			  Vec3(
					 config["camera"]["position"][0].as<float>(),
					 config["camera"]["position"][1].as<float>(),
					 config["camera"]["position"][2].as<float>()),
				config["camera"]["fov"].as<float>(),
				config["camera"]["focalLength"].as<float>(),
				Pixels(
					 config["camera"]["pxWidth"].as<unsigned>(),
					 config["camera"]["pxHeight"].as<unsigned>()));

		_backgroundColor.set(
			config["backgroundColor"][0].as<float>(),
			config["backgroundColor"][1].as<float>(),
			config["backgroundColor"][2].as<float>());
	}
	catch(YAML::BadFile e)
	{
		usage();
		return false;
	}

	return true;
}

void Scene::usage()
{
	std::cout << "Invalid command line parameters.\nUsage: ./Raytracer scenefile.yaml [-threads=n]";
}

void Scene::trace()
{
	if(_threadCount == 0) // Automatically determine threads
	{
		_threadCount = std::thread::hardware_concurrency();
		if(_threadCount == 0) // Cannot be determined
			_threadCount = 1;
	}

	std::cout << "Thread count: " << _threadCount << "\n";
	std::cout << "Tracing " << _name << "...";

	// Create threads and trace
	std::vector<std::thread> threads(_threadCount);
	for(unsigned i = 0; i < _threadCount; ++i)
		threads[i] = std::thread(&Scene::traceSection, this, std::ref(_camera), Scene::createPixelRayMap(i, _threadCount));

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

		for(Light& l : _lights)
		{
			L = l._position - collisionPoint;
			float distToLight = L.length();
			L.normalize();

			Ray shadowRay(collisionPoint, L, .01f/*offset to avoid self collision*/);
			if(castShadowRay(shadowRay, distToLight))
				color = color + geom->_material.ambientColor();
			else
				color = color + geom->_material.blinnPhong(N, V, L, l._color);
		}

		// Exit if recursive depth is met
		if(depth == 0)
			return color;

		// Calculate reflection and refraction rays
		Ray reflectedRay(collisionPoint, reflect(normalize(collisionPoint - ray._origin), N), .01f);
		Ray refractedRay(collisionPoint, refract(normalize(collisionPoint - ray._origin), N, 1.500f), .01f);
		Vec3 reflectedColor = calculateColor(reflectedRay, depth - 1);
		Vec3 refractedColor = calculateColor(refractedRay, depth - 1);
		float reflCoef 		= .4444f;
		float refrCoef 		= .1111f;
		float shadingCoef = .444f;
		color = reflCoef    * reflectedColor +
						refrCoef 		* refractedColor +
						shadingCoef * color;
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
	_camera._pixels.save(_outputName.c_str());
	std::cout << "Finished\n";
}
