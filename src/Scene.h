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

class Scene
{
public:
	Scene(int argc, char** argv);
	void trace();
	void save();

private:
	bool parseArgs(int argc, char** argv);
	void traceSection(Camera& camera, std::map<unsigned, Ray> pixelRayMap);
	int castRay(Ray& ray);
	Vec3 calculateColor(const Ray origRay, int depth);
	bool castShadowRay(Ray& ray, float distToLight, int origGeomIndex);
	Vec3 blinnPhong(const Ray& ray, const Tracable& geom);
	std::map<unsigned, Ray> createPixelRayMap(unsigned threadID, unsigned threadCount);

	std::vector<std::unique_ptr<Tracable>> _geometry;
	std::vector<Light> _lights;
	Camera _camera;
	Vec3 _backgroundColor;
};
