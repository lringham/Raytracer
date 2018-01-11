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

	// Color raycast(Ray ray, int depth);
	// Color refract(Ray ray, int depth);
	// Color reflect(Ray ray, int depth);
	// Color shadow(Ray ray);
	// Color blinnPhong(Ray ray, const Tracable& geom);

	std::map<unsigned, Ray> createRays();
	std::vector<Tracable> geometry;
	std::vector<Light> lights;
	Camera camera;
	//Color backgroundColor;
};
