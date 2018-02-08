#include <thread>
#include <iostream>

#include "Utils.h"
#include "Scene.h"
#include "Ray.h"
#include "Sphere.h"
#include "Plane.h"
#include "Triangle.h"
#include "Obj.h"

Scene::Scene(int argc, char** argv) :
	 _name("scene"), _outputName(""), _threadCount(0)
{
	std::cout << "Loading... ";
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

Vec3 Scene::nodeToVec3(const YAML::Node& node) const
{
	return Vec3(
		node[0].as<float>(),
		node[1].as<float>(),
		node[2].as<float>());
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

		if(config["recursionDepth"])
			_depth =  config["recursionDepth"].as<unsigned>();

		if(config["shadowSampleCount"])
			_shadowSampleCount =  config["shadowSampleCount"].as<unsigned>();

		const YAML::Node& materialsNode = config["materials"];
		for (YAML::const_iterator it = materialsNode.begin(); it != materialsNode.end(); ++it)
		{
		    const YAML::Node& materialNode = *it;
				_materialMap[materialNode["name"].as<std::string>()] = Material(
						materialNode["name"].as<std::string>(),
						materialNode["Ia"].as<float>(),
						nodeToVec3(materialNode["kd"]),
						nodeToVec3(materialNode["ks"]),
						materialNode["gloss"].as<float>(),
						materialNode["eta"].as<float>(),
						materialNode["type"].as<std::string>()
					);
		}

		const YAML::Node& objectsNode = config["objects"];
		for (YAML::const_iterator it = objectsNode.begin(); it != objectsNode.end(); ++it)
		{
		    const YAML::Node& object = *it;
		    std::string type = object["type"].as<std::string>();

				if(type == "sphere")
				{
					_geometry.emplace_back(
						new Sphere(
							object["radius"].as<float>(),
							nodeToVec3(object["position"])));
				}
				else if(type == "triangle")
				{
					_geometry.emplace_back(
						new Triangle(
							nodeToVec3(object["x0"]),
							nodeToVec3(object["x1"]),
							nodeToVec3(object["x2"])));
				}
				else if(type == "plane")
				{
					_geometry.emplace_back(
						new Plane(
							nodeToVec3(object["normal"]),
							nodeToVec3(object["position"])));
				}
				else if(type == "model")
				{
					std::string modelsDir = "";
					if(config["modelsDir"])
						modelsDir = config["modelsDir"].as<std::string>();

					_geometry.emplace_back(
						new Obj(
							nodeToVec3(object["position"]),
							object["filename"].as<std::string>(),
							modelsDir));
				}
				else
					std::cout << "Invalid geometry type: " << type << std::endl;

				std::string materialName = object["material"].as<std::string>();
				if(_materialMap.count(materialName) == 1)
					_geometry.back()->_material = _materialMap[materialName];
		}

		const YAML::Node& lights = config["lights"];
		for (YAML::const_iterator it = lights.begin(); it != lights.end(); ++it)
		{
			const YAML::Node& light = *it;
			_lights.emplace_back(
					nodeToVec3(light["color"]),
					nodeToVec3(light["position"]),
					light["radius"].as<float>(),
					light["intensity"].as<float>()
				);
		}

		_camera.init(
				nodeToVec3(config["camera"]["position"]),
				config["camera"]["fov"].as<float>(),
				config["camera"]["focalLength"].as<float>(),
				Pixels(
					 config["camera"]["pxWidth"].as<unsigned>(),
					 config["camera"]["pxHeight"].as<unsigned>()),
				config["camera"]["sampleCount"].as<int>(),
				config["camera"]["lensRadius"].as<float>());

		_backgroundColor.set(nodeToVec3(config["backgroundColor"]));
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
	std::cout << "Invalid command line parameters.\nUsage: ./Raytracer scene.yaml [-threads=8] [-w=800] [-h=600] [recurDepth=6]";
}

void Scene::trace()
{
	if(_threadCount == 0) // Automatically determine threads
	{
		_threadCount = std::thread::hardware_concurrency();
		if(_threadCount == 0) // Cannot be determined
			_threadCount = 1;
	}

	// Print tracing details
	std::cout << "Thread count: " << _threadCount << "\n";
	std::cout << "Recursion depth: " << _depth << "\n";
	std::cout << "Pixel sample count: " << _camera.sampleCount() << "\n";
	std::cout << "Shadow sample count " << _shadowSampleCount << "\n";
	std::cout << "Tracing " << _name << "...";

	// Create threads and trace
	std::vector<std::thread> threads(_threadCount);
	for(unsigned i = 0; i < _threadCount; ++i)
		threads[i] = std::thread(&Scene::traceSection, this, std::ref(_camera), i);

	// Join threads
	for(auto& thread : threads)
		thread.join();

	std::cout << "Finished\n";
}

void Scene::traceSection(Camera& _camera, unsigned threadID)
{
	unsigned width = _camera._pixels.width();
	unsigned height = _camera._pixels.height();

	unsigned rem = width % _threadCount;
	unsigned xRes = width / _threadCount;
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

	std::string progressString = "";
	int depth = _depth;
	for(unsigned x = startX; x < endX; ++x)
	{
		// thread 0 prints progress
		if(threadID == 0)
		{
			progressString = std::to_string(int((100.f*x) / float(endX - startX))) + "%";
			std::cout << progressString << std::flush;
		}

		for(unsigned y = 0; y < height; ++y)
		{
			std::vector<Ray> rays = _camera.createRays(x, y);

			Vec3 color(0,0,0);
			for(auto& ray : rays)
			{
				color = color + calculateColor(ray, depth);
			}

			float rayCount = rays.size();
			color._r /= rayCount;
			color._g /= rayCount;
			color._b /= rayCount;
			color._r = clamp(color._r, 0.f, 1.f);
			color._g = clamp(color._g, 0.f, 1.f);
			color._b = clamp(color._b, 0.f, 1.f);

			_camera._pixels.set(x+y*width, color);
		}

		// erase progress indication
		if(threadID == 0)
		{
			for(unsigned i = 0; i < progressString.size(); ++i)
				std::cout << '\b';
			for(unsigned i = 0; i < progressString.size(); ++i)
				std::cout << ' ';
			for(unsigned i = 0; i < progressString.size(); ++i)
				std::cout << '\b';
			std::cout << std::flush;
		}
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

		// Lights
		for(Light& l : _lights)
		{
			Vec3 L = l._position - collisionPoint;
			float distToLight = L.length();
			L.normalize();

			// Shadows
			for(unsigned i = 0; i < _shadowSampleCount; ++i)
			{
				Ray shadowRay = l.getShadowRay(collisionPoint, _rayOffset);
				if(castShadowRay(shadowRay, distToLight))
					color = color + geom->_material.ambientColor();
				else
					color = color + geom->_material.color(N, V, L, collisionPoint, l);
			}
		}
		color._r /= _shadowSampleCount;
		color._g /= _shadowSampleCount;
		color._b /= _shadowSampleCount;

		// Exit if recursive depth is met
		if(depth == 0)
			return color;

		// Calculate reflection and refraction rays
		if(geom->_material.isMetallic())
		{
			float kr 		= 0.25f; // Reflection probability
			Ray reflectedRay(collisionPoint, reflect(normalize(collisionPoint - ray._origin), N), _rayOffset);
			Vec3 Ir = calculateColor(reflectedRay, depth - 1);
			color = color + kr*Ir;
		}
		else if(geom->_material.isTransparent())
		{
			//TODO replace with fresnel coef
			//https://blog.demofox.org/2017/01/09/raytracing-reflection-refraction-fresnel-total-internal-reflection-and-bee rs-law/
			float kt 		= .8; // Transmission probability
			float kr 		= 1.f - kt; // Reflection probability
			Ray reflectedRay(collisionPoint, reflect(normalize(collisionPoint - ray._origin), N), _rayOffset);
			Ray refractedRay(collisionPoint, refract(normalize(collisionPoint - ray._origin), N, ambientIOR, geom->_material._eta), _rayOffset);
			Vec3 Ir = calculateColor(reflectedRay, depth - 1);
			Vec3 It = calculateColor(refractedRay, depth - 1);
			color = color + kr*Ir + kt*It;
		}
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
