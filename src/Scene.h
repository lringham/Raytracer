#pragma once
#include "Tracable.h"
#include "Light.h"
#include "Camera.h"
#include "Ray.h"

#include <vector>
#include <stack>
#include <map>
#include <memory>
#include <mutex>
#include <yaml-cpp/yaml.h>

class Scene
{
public:
	Scene(int argc, char** argv);
	bool loadScene(int argc, char** argv);
	void trace();
	void save();

private:
	struct Block 
	{
		unsigned _startX, _endX, _startY, _endY;

		Block(unsigned startX, unsigned endX, unsigned startY, unsigned endY) :
			_startX(startX), _endX(endX), _startY(startY), _endY(endY)
		{}

		Block() 
		{}
	};

	Vec3 parseNode(const YAML::Node& node, const std::string& name, Vec3 defaultVal)
	{
		Vec3 result = defaultVal;
		bool missingComponent = false;
		if(node[name])
		{
			const YAML::Node& vec3Node = node[name];
			missingComponent = !(vec3Node[0] && vec3Node[1] && vec3Node[2]);
			result._x = vec3Node[0] ? vec3Node[0].as<float>() : defaultVal._x;
			result._y = vec3Node[1] ? vec3Node[1].as<float>() : defaultVal._y;
			result._z = vec3Node[2] ? vec3Node[2].as<float>() : defaultVal._z;
			if(missingComponent)
				std::cout << "yaml node " << name << " doesn't have 3 components" << std::endl;
		}
		return result;
	}

	template<typename T>
	T parseNode(const YAML::Node& node, const std::string& name, T defaultVal)
	{
		if(node[name])
			return node[name].as<T>();
		else
			return defaultVal;
	}

	template<typename T>
	T parseNode(const YAML::Node& node, const std::string& name)
	{
		T t;
		if(node[name])
			t = node[name].as<T>();
		else
			std::cout << "Cannot find yaml node: " << name << "\n";
		return t;
	}

	bool parseArgs(int argc, char** argv, std::string& sceneFilename);	
	void traceSection(Camera& camera);
	int castRay(Ray& ray);
	Vec3 calculateColor(const Ray origRay, int depth);
	bool castShadowRay(Ray& ray, float distToLight);
	Vec3 blinnPhong(const Ray& ray, const Tracable& geom);
	Vec3 sampleBackground(const Vec3& dir) const;
	bool getBlock(unsigned& startX, unsigned& endX, unsigned& startY, unsigned& endY);
	void usage();

	std::vector<std::unique_ptr<Tracable>> _geometry;
	std::vector<std::unique_ptr<Light>> _lights;
	std::vector<Material> _materials;
	std::stack<Block> blocks;

	Texture _skySphere;
	Camera _camera;
	Vec3 _backgroundColor;
	std::string _name;
	std::string _outputName;
	std::mutex _mutex;
	unsigned _threadCount, _depth, _shadowSampleCount, _totalBlockCount;
	float _rayOffset, _ambientIOR;
	bool _backgroundSet;
};
