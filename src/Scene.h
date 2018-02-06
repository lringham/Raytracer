#pragma once
#include <vector>
#include <map>
#include <memory>

#include "Tracable.h"
#include "Light.h"
#include "Camera.h"
#include "Ray.h"
#include "Sphere.h"
#include "Plane.h"
#include "Triangle.h"
#include <yaml-cpp/yaml.h>

class Scene
{
public:
	Scene(int argc, char** argv);
	void trace();
	void save();

private:
	bool parseArgs(int argc, char** argv);
	void traceSection(Camera& camera, unsigned threadID);
	int castRay(Ray& ray);
	Vec3 calculateColor(const Ray origRay, int depth);
	bool castShadowRay(Ray& ray, float distToLight);
	Vec3 blinnPhong(const Ray& ray, const Tracable& geom);

	void usage();
	Vec3 nodeToVec3(const YAML::Node& node) const;

	std::vector<std::unique_ptr<Tracable>> _geometry;
	std::vector<Light> _lights;
	std::map<std::string, Material> _materialMap;
	Camera _camera;
	Vec3 _backgroundColor;
	std::string _name;
	std::string _outputName;
	unsigned _threadCount, _depth, _shadowSampleCount;
	float _rayOffset = .01f, ambientIOR = 1.f;
};
