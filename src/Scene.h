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

    Vec3 parseNode(const YAML::Node& node, const std::string& name, Vec3 defaultVal)
    {
        Vec3 result = defaultVal;
        bool missingComponent = false;
        if (node[name])
        {
            const YAML::Node& vec3Node = node[name];
            missingComponent = !(vec3Node[0] && vec3Node[1] && vec3Node[2]);
            result.x_ = vec3Node[0] ? vec3Node[0].as<float>() : defaultVal.x_;
            result.y_ = vec3Node[1] ? vec3Node[1].as<float>() : defaultVal.y_;
            result.z_ = vec3Node[2] ? vec3Node[2].as<float>() : defaultVal.z_;
            if (missingComponent)
                std::cout << "yaml node " << name << " doesn't have 3 components" << std::endl;
        }
        return result;
    }

    template<typename T>
    T parseNode(const YAML::Node& node, const std::string& name, T defaultVal)
    {
        if (node[name])
            return node[name].as<T>();
        else
            return defaultVal;
    }

    template<typename T>
    T parseNode(const YAML::Node& node, const std::string& name)
    {
        T t;
        if (node[name])
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
    std::string outputName_ = "out";
    unsigned threadCount_ = 0, depth_ = 0, shadowSampleCount_ = 0, totalBlockCount_ = 0;
    float rayOffset_ = 0.01f, ambientIOR_ = 1.f;
    bool backgroundSet_ = false;
};
