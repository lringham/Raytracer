#pragma once
#include <vector>
#include <map>

#include "Geometry.h"
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
	std::map<unsigned, Ray> createRays();

	Camera camera;
	std::vector<Geometry> geometry;
	std::vector<Light> lights;
};
