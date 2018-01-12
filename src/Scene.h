#pragma once
#include <vector>
#include <map>

#include "Tracable.h"
#include "Light.h"
#include "Camera.h"
#include "Ray.h"

class Scene
{
public:
	Scene(int argc, char** argv);
	void trace();
	void save();

private:
	bool parseArgs(int argc, char** argv);
	void traceSection(Camera& camera, std::map<unsigned, Ray> rays);

	Vec3 raycast(Ray ray, int depth);
	Vec3 refract(Ray ray, int depth);
	Vec3 reflect(Ray ray, int depth);
	Vec3 shadow(Ray ray);
	Vec3 blinnPhong(Ray ray, const Tracable& geom);

	std::map<unsigned, Ray> createRays(unsigned threadID, unsigned threadCount);
	std::vector<Tracable> geometry;
	std::vector<Light> lights;
	Camera camera;
	Vec3 backgroundColor;
};
