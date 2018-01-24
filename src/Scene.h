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
	void traceSection(Camera& camera, std::map<unsigned, std::vector<Ray>> pixelRayMap);
	int castRay(Ray& ray);
	Vec3 calculateColor(const Ray origRay, int depth);
	bool castShadowRay(Ray& ray, float distToLight);
	Vec3 blinnPhong(const Ray& ray, const Tracable& geom);
	std::map<unsigned, std::vector<Ray>> createPixelRayMap(unsigned threadID, unsigned threadCount);
	void usage();
	Vec3 nodeToVec3(const YAML::Node& node) const;

	std::vector<std::unique_ptr<Tracable>> _geometry;
	std::vector<Light> _lights;
	std::vector<Material> _materials;
	Camera _camera;
	Vec3 _backgroundColor;
	std::string _name;
	std::string _outputName;
	unsigned _threadCount;
	float _rayOffset = .01f;
};
