#pragma once
#include "Tracable.h"
#include "Light.h"
#include "Camera.h"
#include "Ray.h"

#include <vector>
#include <stack>
#include <map>
#include <memory>
#include <atomic>
#include <yaml-cpp/yaml.h>

class Scene
{
public:
    Scene();
    bool init(int argc, char** argv);
    bool loadScene(int argc, char** argv);
    void trace();
    void save();

private:
    struct Block
    {
        unsigned startX_ = 0, endX_ = 0, startY_ = 0, endY_ = 0;

        Block(unsigned startX, unsigned endX, unsigned startY, unsigned endY) :
            startX_(startX), endX_(endX), startY_(startY), endY_(endY)
        {}

        Block()
        {}
    };

    bool parseNode(const YAML::Node& node, const std::string& name, Vec3& vec)
    {
        if (!node[name])
            return false;

        const YAML::Node& vec3Node = node[name];
        if(!vec3Node[0] || !vec3Node[1] || !vec3Node[2])
            return false;

        vec.x_ = vec3Node[0].as<float>();
        vec.y_ = vec3Node[1].as<float>();
        vec.z_ = vec3Node[2].as<float>();

        return true;
    }

    template<typename T>
    bool parseNode(const YAML::Node& node, const std::string& name, T& value)
    {
        if (node[name])
        {
            value = node[name].as<T>();
            return true;
        }
        return false;
    }


    bool parseArgs(int argc, char** argv, std::string& sceneFilename);
    void traceSection(Camera& camera);
    int castRay(Ray& ray);
    Vec3 calculateColor(const Ray origRay, int depth);
    bool castShadowRay(Ray& ray, float distToLight);
    Vec3 sampleBackground(const Vec3& dir) const;
    bool getBlock(unsigned& startX, unsigned& endX, unsigned& startY, unsigned& endY);
    void usage();
    void printProgress(float progress) const;

    std::vector<std::unique_ptr<Tracable>> geometry_;
    std::vector<std::unique_ptr<Light>> lights_;
    std::vector<Material> materials_;
    std::vector<Block> blocks_;
    std::atomic<unsigned> currentBlock_;

    Texture skySphere_;
    Camera camera_;
    Vec3 backgroundColor_ = Vec3(0, 0, 0);
    std::string name_ = "scene";
    std::string outputName_ = "scene.ppm";
    unsigned threadCount_ = 0, depth_ = 1, shadowSampleCount_ = 1, totalBlockCount_ = 0;
    float rayOffset_ = 0.01f, ambientIOR_ = 1.f;
    bool backgroundSet_ = false;
};

const auto defaultScene = R"(
name: defaultScene
outputName: defaultScene.ppm
recursionDepth: 1
shadowSampleCount: 1
materials:
    - name: plane
      type: metallic
      kd: [.3, .3, .3]
      ks: [1.0, 1.0, 1.0]
      Ia: 0.1
      gloss: 64
      ior: 1.0
      reflectivity: .05
      checkered: true
    - name: sphere_green
      type: blinnPhong
      kd: [.14, .68, .37]
      ks: [1, 1, 1]
      Ia: .1
      gloss: 256
      ior: 1
      reflectivity: .05
objects:
    - type: sphere
      radius: 5.0
      position: [-10, 5, 0]
      material: sphere_green
    - type: plane
      normal: [0, 1, 0]
      position: [0, 0, 0]
      material: plane
lights:
  - type: "point"
    color: [1.0, 1.0, 1.0]
    position: [200.0, 200.0, 0.0]
    radius: 25.0
    intensity: 100000.0
camera:
  position: [-10.0, 12.5, 50.0]
  fov: 1.5708
  focalLength: 50.0
  lensRadius: 1
  pxWidth: 1000
  pxHeight: 640
  sampleCount: 200
)";