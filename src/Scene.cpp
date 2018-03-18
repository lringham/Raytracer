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
	
	std::map<std::string, int> materialMap;
	YAML::Node config;
	try
	{
		//TODO convert all optional args to use parseNode function
		//TODO extract default values / magic numbers
		YAML::Node config  	  = YAML::LoadFile(sceneFile);
		std::string modelsDir = parseNode<std::string>(config, "modelsDir", "");
		_outputName 	   	  = parseNode<std::string>(config, "outputName", "scene.ppm");
		_name 			   	  = parseNode<std::string>(config, "name", "scene");
		_depth 			   	  = parseNode<unsigned>(config, "recursionDepth", 1);
		_shadowSampleCount 	  = parseNode<unsigned>(config, "shadowSampleCount", 1);
		_skySphere.loadTexture(parseNode<std::string>(config, "skySphere", ""));

		const YAML::Node& materialsNode = config["materials"];
		for (YAML::const_iterator it = materialsNode.begin(); it != materialsNode.end(); ++it)
		{
		    const YAML::Node& materialNode = *it;
			std::string materialName = materialNode["name"].as<std::string>();

			materialMap[materialName] = _materials.size();
			_materials.emplace_back(
					materialName,
					materialNode["Ia"].as<float>(),
					parseNode(materialNode, "kd", Vec3(1, 1, 1)),
					parseNode(materialNode, "ks", Vec3(1, 1, 1)),
					parseNode(materialNode, "attenuation", Vec3(1, 1, 1)),
					parseNode<float>(materialNode, "gloss", 1.f),
					parseNode<float>(materialNode, "eta", 1.f),
					parseNode<float>(materialNode, "reflectivity", 0.f),
					materialNode["type"].as<std::string>(),
					parseNode<bool>(materialNode, "checkered", false));

			if(materialNode["texture"])
				_materials.back().setTexture(materialNode["texture"].as<std::string>());
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
						parseNode<float>(object, "height", std::numeric_limits<float>::max())));
			}
			else if(type == "model")
			{				
				std::string matName = parseNode<std::string>(object, "material", "");
				_geometry.emplace_back(new BVH(
					Obj(
						parseNode(object, "position", Vec3(0, 0, 0)),
						object["filename"].as<std::string>(), 
						modelsDir,
						matName != "" ? materialMap[matName] : -1,
						_materials,
						materialMap)));	
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

			if(object["material"])
				_geometry.back()->_materialID = materialMap[object["material"].as<std::string>()];
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
				parseNode(config["camera"], "direction", Vec3(0, 0, -1)),
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

	// for(auto& m : _materials)
	// {
	// 	std::cout << "Material: " << m._name << "\n";
	// 	std::cout << "\t kd: " << m._kd << "\n";
	// 	std::cout << "\t ks: " << m._ks << "\n";
	// 	std::cout << "\t Ia: " << m._Ia << "\n";
	// 	std::cout << "\t Type: " << m._type << "\n";	
	// 	std::cout << "\t Tex: " << (m.hasTexture() ? "yes\n" : "no\n");	
	// 	std::cout << "\t gloss: " << m._gloss << "\n";	
	// }

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
	std::cout << "Shadow sample count: " << _shadowSampleCount << "\n";
	//std::cout << "Tracing " << _name << "...";

	// Divide image into blocks
	unsigned width = _camera._pixels.width();
	unsigned height = _camera._pixels.height();
	unsigned blockWidth = 64;
	unsigned blockHeight = 64;
	unsigned xRes = width / blockWidth;
	unsigned yRes = height / blockHeight;
	if(width % blockWidth > 0)
		xRes++;
	if(height % blockHeight > 0)
		yRes++;

	//TODO: replace with space filling curve for better cache locality
	for (unsigned y = 0; y < yRes; ++y)
	{
		for (unsigned x = 0; x < xRes; ++x)
		{
			unsigned startX = x*blockWidth;
			unsigned startY = y*blockHeight;
			blocks.push(Block(startX, std::min(startX+blockWidth-1, width-1), 
							  startY, std::min(startY+blockHeight-1, height-1)));			
		}
	}
	_totalBlockCount = blocks.size();

	// Create threads and trace
	std::vector<std::thread> threads(_threadCount-1);
	for(unsigned i = 0; i < _threadCount-1; ++i)
		threads[i] = std::thread(&Scene::traceSection, this, std::ref(_camera));
	traceSection(_camera);
	
	// Join threads
	for(auto& thread : threads)
		thread.join();

	std::cout << "Finished\n";
}

void Scene::traceSection(Camera& _camera)
{
	// trace scene
	unsigned width = _camera._pixels.width();
	unsigned startX, endX, startY, endY;
	while(getBlock(startX, endX, startY, endY))
	{		
		for(unsigned x = startX; x <= endX; ++x)
		{
			for(unsigned y = startY; y <= endY; ++y)
			{
				std::vector<Ray> rays = _camera.createRays(x, y);

				Vec3 color(0,0,0);
				for(auto& ray : rays)
				{
					color = color + calculateColor(ray, _depth);
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
		}
	}
}

bool Scene::getBlock(unsigned& startX, unsigned& endX, unsigned& startY, unsigned& endY)
{
	if(blocks.size() > 0)
	{		
       	std::unique_lock<std::mutex> lock(_mutex);
		
		Block b = blocks.top();
		blocks.pop();

		startX = b._startX;
		endX = b._endX;
		startY = b._startY;
		endY = b._endY;

		int progressRes = 10;
		float progress = 1.f - static_cast<float>(blocks.size()) / static_cast<float>(_totalBlockCount);
		std::cout.flush();
		std::cout << "Tracing " << _name << " ";
		std::cout << "\033[1;32m[\033[0m";
		for(int i = 0; i < progressRes; ++i)
		{
			if(progress*progressRes > i)
				std::cout << "\033[1;32m#\033[0m";		
			else
				std::cout << "\033[1;32m \033[0m";		
		}
		std::cout << "\033[1;32m]\033[0m";		
		progress = static_cast<int>(progress * 100);
		if(progress < 10)
			std::cout << "  ";	
		else if(progress < 100)
			std::cout << " ";
		std::cout << "\033[1;32m" << progress << "%\033[0m" << "\r";

		return true;
	}
	return false;
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
		auto& material = _materials[ray._materialID];

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
				color = material.color(N, V, L, l->getColor(collisionPoint));

			// Shadows
			for(unsigned i = 0; i < _shadowSampleCount; ++i)
			{
				Ray shadowRay = l->getShadowRay(collisionPoint, _rayOffset);
				if(castShadowRay(shadowRay, distToLight))
					color = color + material.ambientColor();
				else
					color = color + material.color(N, V, L, l->getColor(collisionPoint));
			}
		}

		if(_shadowSampleCount > 0)
		{
			color._r /= _shadowSampleCount;
			color._g /= _shadowSampleCount;
			color._b /= _shadowSampleCount;
		}

		if(material.hasTexture())
		{
			color = color * material.sampleTexture(ray._uv);
		}
		if(material.isCheckered(collisionPoint))
			color = color * 0.25f;

		// Exit if recursive depth is met
		if(depth == 0)
			return color;

		// Calculate reflection and refraction rays
		if(material.isMetallic())
		{
			float r = material.schlick(material._eta, _ambientIOR, ray._dir.dot(N));
			Ray reflectedRay(collisionPoint, reflect(ray._dir, N), _rayOffset);
			Vec3 Ir = calculateColor(reflectedRay, depth - 1);
			color = color + r*Ir;
		}
		else if(material.isTransparent())
		{
			Vec3 Ir;
			Vec3 It;
			Vec3 k(1);
			float cosTheta;
			Ray reflectedRay(collisionPoint, reflect(ray._dir, N), _rayOffset);

			// Ray is outside
			float dDotN = ray._dir.dot(N);
			if(dDotN < 0)
			{
				Vec3 dir;			
				refract(ray._dir, N, _ambientIOR, material._eta, dir); //TIR can't happen because air is <= goem ior
				It = calculateColor(Ray(collisionPoint, dir, _rayOffset), depth - 1);
				Ir = calculateColor(Ray(collisionPoint, reflect(ray._dir, N), _rayOffset), depth - 1);
				cosTheta = -dDotN;
			}
			// Ray is inside
			else
			{
				Vec3 dir;
				k = material.attenuationColor(ray._t);	
			
				if(refract(ray._dir, -1*N, material._eta, _ambientIOR, dir)) // check for TIR
				{
					It = calculateColor(Ray(collisionPoint, dir, _rayOffset), depth - 1);
					Ir = calculateColor(Ray(collisionPoint, reflect(ray._dir, -1.f*N), _rayOffset), depth - 1);
					cosTheta = dir.dot(N);
				}
				// Reflection only
				else
					return k*calculateColor(Ray(collisionPoint, reflect(ray._dir, N), _rayOffset), depth - 1);
			}

			float r = material.schlick(material._eta, _ambientIOR, cosTheta);
			color = color + k * (r * Ir + (1.f-r) * It);
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

Vec3 Scene::sampleBackground(const Vec3& dir) const
{
	if(_skySphere.isInitialized())
	{
		Vec2 uv = calculateUV(dir);
		return _skySphere.sample(uv._u, uv._v);
	}
	else
	{
		float s = cos(((dir._y + 1.f) / 2.f) * 3.141592); 
		return (1.f-s)*normalize(Vec3(135, 206, 235)) + s*normalize(Vec3(250, 214, 165));
	}
}