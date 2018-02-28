#include <thread>
#include <iostream>

#include "Utils.h"
#include "Scene.h"
#include "Ray.h"
#include "Sphere.h"
#include "Plane.h"
#include "Triangle.h"
#include "Obj.h"
#include "AABB.h"
#include "BVH.h"

Scene::Scene(int argc, char** argv) :
	 _name("scene"), _outputName(""), _threadCount(0)
{

	std::cout << "Loading scene...\n";
	if(parseArgs(argc, argv))
	{
		std::cout << "Finished loading\n";
	}
	else
	{
		std::cout << "Failed loading\n";
		usage();
		throw -1;
	}
}

bool Scene::parseArgs(int argc, char** argv) //argv
{
	std::string sceneFile = "";
	if(argc <= 1 || argc > 5)
	{
		return false;
	}
	else
	{
		sceneFile = argv[1];
		const std::string threadCountStr = "-threads=";
		for(int i = 1; i < argc; ++i)
		{
			std::string arg = argv[i];
			if(arg.compare(0, threadCountStr.length(), threadCountStr) == 0)
				_threadCount = std::stoi(arg.substr(arg.find(threadCountStr)+threadCountStr.size()));
				//FIXME account for other params
		}
	}

	YAML::Node config;
	try
	{
		//TODO convert all optional args to use parseNode function
		//TODO extract default values / magic numbers

		YAML::Node config  = YAML::LoadFile(sceneFile);
		_outputName 	   = parseNode<std::string>(config, "outputName", "scene.ppm");
		_name 			   = parseNode<std::string>(config, "name", "scene");
		_depth 			   = parseNode<unsigned>(config, "recursionDepth", 1);
		_shadowSampleCount = parseNode<unsigned>(config, "shadowSampleCount", 1);
		_skySphere.loadTexture(parseNode<std::string>(config, "skySphere", ""));

		const YAML::Node& materialsNode = config["materials"];
		for (YAML::const_iterator it = materialsNode.begin(); it != materialsNode.end(); ++it)
		{
		    const YAML::Node& materialNode = *it;
				std::string materialName = materialNode["name"].as<std::string>();
				_materialMap[materialName] = Material(
						materialName,
						materialNode["Ia"].as<float>(),
						parseNode(materialNode, "kd", Vec3(1, 1, 1)),
						parseNode(materialNode, "ks", Vec3(1, 1, 1)),
						parseNode<float>(materialNode, "gloss", 1.f),
						parseNode<float>(materialNode, "eta", 1.f),
						materialNode["type"].as<std::string>()
					);
				if(materialNode["texture"])
					_materialMap[materialName].setTexture(materialNode["texture"].as<std::string>());
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
							parseNode<float>(object, "radius", 1.f),
							parseNode(object, "position", Vec3(0, 0, 0))));
				}
				else if(type == "triangle")
				{
					_geometry.emplace_back(
						new Triangle(
							parseNode(object, "x0", Vec3(-1, 0, 0)),
							parseNode(object, "x1", Vec3(0, 1, 0)),
							parseNode(object, "x2", Vec3(1, 0, 0))));
				}
				else if(type == "plane")
				{
					_geometry.emplace_back(
						new Plane(
							parseNode(object, "normal", Vec3(0, 1, 0)),
							parseNode(object, "position", Vec3(0, 0, 0)),
							parseNode<float>(object, "width", std::numeric_limits<float>::max()),
							parseNode<float>(object, "depth", std::numeric_limits<float>::max())));
				}
				else if(type == "model")
				{
					std::string modelsDir = parseNode<std::string>(config, "modelsDir", "");
					_geometry.emplace_back(new BVH(
						Obj(
							parseNode(object, "position", Vec3(0, 0, 0)),
							object["filename"].as<std::string>(), 
							modelsDir
						)
					));
					// _geometry.emplace_back(new Obj(
					// 		parseNode(object, "position", Vec3(0, 0, 0)),
					// 		object["filename"].as<std::string>(), 
					// 		modelsDir
					// 	)
					// );
				}
				else if(type == "box")
				{
					_geometry.emplace_back(
						new AABB(
							parseNode(object, "maxCorner", Vec3(0, 0, 0)),
							parseNode(object, "minCorner", Vec3(1, 1, 1))
						));
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
			std::string type = light["type"].as<std::string>();

			if(type == "point")
			{
				_lights.emplace_back(
					new PointLight(
						parseNode(light, "color", Vec3(1, 1, 1)),
						parseNode(light, "position", Vec3(0, 0, 0)),
						parseNode<float>(light, "intensity", 100.f),
						parseNode<float>(light, "radius", 0.f)
				));
			}
			else if(type == "spot")
			{
				_lights.emplace_back(
					new SpotLight(
						parseNode(light, "color", Vec3(1, 1, 1)),
						parseNode(light, "position", Vec3(0, 0, 0)),
						parseNode<float>(light, "intensity", 100.f),
						parseNode(light, "direction", Vec3(0, -1, 0)),
						parseNode<float>(light, "cosThetaP", .5),
						parseNode<float>(light, "cosThetaU", .7),
						parseNode<float>(light, "exp", 1.f)
				));
			}
			else if(type == "directional")
			{
				_lights.emplace_back(
					new DirectionalLight(
						parseNode(light, "color", Vec3(1, 1, 1)),
						parseNode<float>(light, "intensity", 100.f),
						parseNode(light, "direction", Vec3(0, -1, 0))
				));
			}
		}

		_camera.init(
				parseNode(config["camera"], "position", Vec3(0, 0, 1)),
				parseNode<float>(config["camera"], "fov", 1.f),
				parseNode<float>(config["camera"], "focalLength", 1.f),
				Pixels(
					 parseNode<unsigned>(config["camera"], "pxWidth", 100),
					 parseNode<unsigned>(config["camera"], "pxHeight", 100)),
				parseNode<int>(config["camera"], "sampleCount", 1),
				parseNode<float>(config["camera"], "lensRadius", 0.f));

		_backgroundColor.set(parseNode(config, "backgroundColor", Vec3(0, 0, 0)));
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
	std::cout << "Invalid command line parameters.\nUsage: ./Raytracer scene.yaml [-threads=8] [-w=800] [-h=600] [recurDepth=6]" << std::endl;
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
	// Calculate tracing bounds
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

	// trace scene
	std::string progressString = "";
	int depth = _depth;
	for(unsigned x = startX; x < endX; ++x)
	{
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

		if(threadID == 0)
		{
			// erase progress indication
			for(unsigned i = 0; i < progressString.size(); ++i)
				std::cout << '\b';
			for(unsigned i = 0; i < progressString.size(); ++i)
				std::cout << ' ';
			for(unsigned i = 0; i < progressString.size(); ++i)
				std::cout << '\b';

			// thread 0 prints progress
			progressString = std::to_string(int((100.f*(x-startX)) / float(endX - startX))) + "%";
			std::cout << progressString << std::flush;
		}
	}

	// Erase progress
	if(threadID == 0)
	{
		
		for(unsigned i = 0; i < progressString.size(); ++i)
			std::cout << '\b';
		for(unsigned i = 0; i < progressString.size(); ++i)
			std::cout << ' ';
		for(unsigned i = 0; i < progressString.size(); ++i)
			std::cout << '\b';
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
		for(auto& l : _lights)
		{
			Vec3 L = l->vectorFrom(collisionPoint);
			float distToLight = L.length();
			L.normalize();

			if(_shadowSampleCount == 0)
				color = geom->_material.color(N, V, L, l->getColor(collisionPoint));

			// Shadows
			for(unsigned i = 0; i < _shadowSampleCount; ++i)
			{
				Ray shadowRay = l->getShadowRay(collisionPoint, _rayOffset);
				if(castShadowRay(shadowRay, distToLight))
					color = color + geom->_material.ambientColor();
				else
					color = color + geom->_material.color(N, V, L, l->getColor(collisionPoint));
			}
		}

		if(_shadowSampleCount > 0)
		{
			color._r /= _shadowSampleCount;
			color._g /= _shadowSampleCount;
			color._b /= _shadowSampleCount;
		}

		if(geom->_material.hasTexture())
			color =  color * geom->_material.sampleTexture(ray._uv);

		// Exit if recursive depth is met
		if(depth == 0)
			return color;

		// Calculate reflection and refraction rays
		if(geom->_material.isMetallic())
		{
			float kr = .25f; // Reflection probability
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
		color = sampleBackground(ray._dir);

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

Vec3 Scene::sampleBackground(const Vec3 dir) const
{
	if(_skySphere.isInitialized())
	{
		Vec2 uv = calculateUV(dir);
		return _skySphere.sample(uv._u, uv._v);
		std::cout << "Hello\n";
	}
	else
		return _backgroundColor;
}