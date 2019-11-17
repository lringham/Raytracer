#include <thread>
#include <iostream>
#include <functional>
#include <map>

#include "Utils.h"
#include "Scene.h"
#include "Ray.h"
#include "Sphere.h"
#include "Plane.h"
#include "Triangle.h"
#include "Obj.h"
#include "BVH.h"

Scene::Scene()
{}

bool Scene::init(int argc, char** argv)
{
    std::cout << "Loading scene...";
    if (loadScene(argc, argv))
    {
        std::cout << "finished\n";
        return true;
    }
    else
    {
        std::cout << "failed\n";
        usage();
        return false;
    }
}

bool Scene::parseArgs(int argc, char** argv, std::string& sceneFilename)
{
    std::map<std::string, std::function<void(std::string)>> argMap;
    argMap["-threads="] = [&](std::string value)
    {
        threadCount_ = std::stoi(value);
    };

    if (inbetweenExc(argc, 1, 4))
    {
        sceneFilename = argv[1];
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            size_t pos = arg.find('=');
            if (pos != std::string::npos)
            {
                std::string header = arg.substr(0, pos + 1);
                std::string value = arg.substr(pos + 1);
                if (argMap.count(header) == 1)
                    argMap.at(header)(value);
            }
        }
        return true;
    }
    else
        return false;
}

bool Scene::loadScene(int argc, char** argv)
{
    std::string sceneFilename = "";
    if (!parseArgs(argc, argv, sceneFilename))
        return false;

    std::map<std::string, int> materialMap;
    try
    {
        YAML::Node config = YAML::LoadFile(sceneFilename);
        std::string modelsDir = parseNode<std::string>(config, "modelsDir", "");
        outputName_ = parseNode<std::string>(config, "outputName", "scene.ppm");
        name_ = parseNode<std::string>(config, "name", "scene");
        depth_ = parseNode(config, "recursionDepth", 1);
        shadowSampleCount_ = parseNode(config, "shadowSampleCount", 1);

        if (config["skySphere"])
            skySphere_.loadTexture(parseNode<std::string>(config, "skySphere", ""));

        // parse materials
        const YAML::Node& materialsNode = config["materials"];
        for (YAML::const_iterator it = materialsNode.begin(); it != materialsNode.end(); ++it)
        {
            const YAML::Node& materialNode = *it;
            std::string materialName = parseNode<std::string>(materialNode, "name", "materialName");

            materialMap[materialName] = materials_.size();
            materials_.emplace_back(
                materialName,
                parseNode(materialNode, "Ia", 0.1f),
                parseNode(materialNode, "kd", Vec3(1.f, 1.f, 1.f)),
                parseNode(materialNode, "ks", Vec3(1.f, 1.f, 1.f)),
                parseNode(materialNode, "attenuation", Vec3(1.f, 1.f, 1.f)),
                parseNode(materialNode, "gloss", 1.f),
                parseNode(materialNode, "ior", 1.f),
                parseNode(materialNode, "reflectivity", 0.f),
                parseNode<std::string>(materialNode, "type", "blinnPhong"),
                parseNode(materialNode, "checkered", false));

            if (materialNode["texture"])
            {
                materials_.back().setTexture(materialNode["texture"].as<std::string>());
            }

            if (materialNode["normalMap"])
            {
                materials_.back().setNormalMap(materialNode["normalMap"].as<std::string>());
            }

            if (materialNode["specularMap"])
            {
                materials_.back().setSpecularMap(materialNode["specularMap"].as<std::string>());
            }
        }

        // parse objects
        const YAML::Node& objectsNode = config["objects"];
        for (YAML::const_iterator it = objectsNode.begin(); it != objectsNode.end(); ++it)
        {
            const YAML::Node& object = *it;
            std::string type = object["type"].as<std::string>();

            if (type == "sphere")
            {
                geometry_.emplace_back(
                    new Sphere(
                        parseNode(object, "radius", 1.f),
                        parseNode(object, "position", Vec3(0, 0, 0)),
                        parseNode(object, "invertNormals", false)));
            }
            else if (type == "triangle")
            {
                geometry_.emplace_back(
                    new Triangle(
                        parseNode(object, "x0", Vec3(-1, 0, 0)),
                        parseNode(object, "x1", Vec3(0, 1, 0)),
                        parseNode(object, "x2", Vec3(1, 0, 0))));
            }
            else if (type == "plane")
            {
                geometry_.emplace_back(
                    new Plane(
                        parseNode(object, "normal", Vec3(0, 1, 0)),
                        parseNode(object, "position", Vec3(0, 0, 0)),
                        parseNode(object, "width", std::numeric_limits<float>::max()),
                        parseNode(object, "height", std::numeric_limits<float>::max())));
            }
            else if (type == "model")
            {
                std::string matName = parseNode<std::string>(object, "material", "");
                Obj obj;
                obj.position_ = parseNode(object, "position", Vec3(0, 0, 0));
                obj.materialID_ = matName != "" ? materialMap[matName] : -1;
                if (!obj.load(object["filename"].as<std::string>(), modelsDir, materials_, materialMap))
                    return false;
                geometry_.emplace_back(new BVH(obj));
            }
            else
                std::cout << "Invalid geometry type: " << type << std::endl;

            if (object["material"])
                geometry_.back()->materialID_ = materialMap[object["material"].as<std::string>()];
        }

        // parse lights
        const YAML::Node& lights = config["lights"];
        for (YAML::const_iterator it = lights.begin(); it != lights.end(); ++it)
        {
            const YAML::Node& light = *it;
            std::string type = light["type"].as<std::string>();

            if (type == "point")
            {
                lights_.emplace_back(
                    new PointLight(
                        parseNode(light, "color", Vec3(1, 1, 1)),
                        parseNode(light, "position", Vec3(0, 0, 0)),
                        parseNode(light, "intensity", 100.f),
                        parseNode(light, "radius", 0.f)
                    ));
            }
            else if (type == "spot")
            {
                lights_.emplace_back(
                    new SpotLight(
                        parseNode(light, "color", Vec3(1, 1, 1)),
                        parseNode(light, "position", Vec3(0, 0, 0)),
                        parseNode(light, "intensity", 100.f),
                        parseNode(light, "direction", Vec3(0, -1, 0)),
                        parseNode(light, "cosThetaP", .5f),
                        parseNode(light, "cosThetaU", .7f),
                        parseNode(light, "exp", 1.f)
                    ));
            }
            else if (type == "directional")
            {
                lights_.emplace_back(
                    new DirectionalLight(
                        parseNode(light, "color", Vec3(1, 1, 1)),
                        parseNode(light, "intensity", 100.f),
                        parseNode(light, "direction", Vec3(0, -1, 0))
                    ));
            }
        }

        // create camera
        camera_.init(
            parseNode(config["camera"], "position", Vec3(0, 0, 1)),
            parseNode(config["camera"], "direction", Vec3(0, 0, -1)),
            parseNode(config["camera"], "fov", 0.f),
            parseNode(config["camera"], "focalLength", 1.f),
            Pixels(
                parseNode(config["camera"], "pxWidth", 100),
                parseNode(config["camera"], "pxHeight", 100)),
            parseNode(config["camera"], "sampleCount", 1),
            parseNode(config["camera"], "lensRadius", 0.f));

        if (config["backgroundColor"])
        {
            backgroundColor_.set(parseNode(config, "backgroundColor", Vec3(0, 0, 0)));
            backgroundSet_ = true;
        }
    }
    catch (YAML::BadFile e)
    {
        std::cout << e.what() << "\n";
        return false;
    }
    return true;
}

void Scene::usage()
{
    std::cout << "Invalid command line parameters.\nUsage: ./raytracer scene.yaml [-threads=8]" << std::endl;
}

void Scene::trace()
{
    if (threadCount_ == 0) // Automatically determine threads
    {
        threadCount_ = std::thread::hardware_concurrency();
        if (threadCount_ == 0) // Cannot be determined
            threadCount_ = 1;
    }

    // Print tracing details
    std::cout << "Thread count: " << threadCount_ << "\n";
    std::cout << "Recursion depth: " << depth_ << "\n";
    std::cout << "Pixel sample count: " << camera_.sampleCount() << "\n";
    std::cout << "Shadow sample count: " << shadowSampleCount_ << "\n";
    //std::cout << "Tracing " << name_ << "...";

    // Divide image into blocks
    unsigned width = camera_.pixels_.width();
    unsigned height = camera_.pixels_.height();
    unsigned blockWidth = 64;
    unsigned blockHeight = 64;
    unsigned xRes = width / blockWidth;
    unsigned yRes = height / blockHeight;
    if (width % blockWidth > 0)
        xRes++;
    if (height % blockHeight > 0)
        yRes++;

    //TODO: replace with space filling curve for better cache locality
    for (unsigned y = 0; y < yRes; ++y)
    {
        for (unsigned x = 0; x < xRes; ++x)
        {
            unsigned startX = x * blockWidth;
            unsigned startY = y * blockHeight;
            blocks_.emplace_back(Block(startX, std::min(startX + blockWidth - 1, width - 1),
                startY, std::min(startY + blockHeight - 1, height - 1)));
        }
    }
    totalBlockCount_ = blocks_.size();
    const unsigned finalIndex = totalBlockCount_ + threadCount_;

    // Create threads and trace
    std::vector<std::thread> threads(threadCount_);
    for (unsigned i = 0; i < threadCount_; ++i)
        threads[i] = std::thread(&Scene::traceSection, this, std::ref(camera_));

    // Print progress
    float progress = 0.f;
    while (progress < 1.f)
    {
        float prevProgress = progress;
            progress = float(currentBlock_) / float(finalIndex);
        
        if (progress != prevProgress)
            printProgress(progress);
        
        if (progress < 1.f)
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }


    // Join threads
    for (auto& thread : threads)
        thread.join();

    std::cout << "Finished\n";
}

void Scene::traceSection(Camera & camera_)
{
    // trace scene
    unsigned width = camera_.pixels_.width();
    unsigned startX, endX, startY, endY;
    while (getBlock(startX, endX, startY, endY))
    {
        for (unsigned x = startX; x <= endX; ++x)
        {
            for (unsigned y = startY; y <= endY; ++y)
            {
                std::vector<Ray> rays = camera_.createRays(x, y);

                Vec3 color(0, 0, 0);
                for (auto& ray : rays)
                {
                    color = color + calculateColor(ray, depth_);
                }

                float rayCount = static_cast<float>(rays.size());
                color.r_ /= rayCount;
                color.g_ /= rayCount;
                color.b_ /= rayCount;
                color.r_ = clamp(color.r_, 0.f, 1.f);
                color.g_ = clamp(color.g_, 0.f, 1.f);
                color.b_ = clamp(color.b_, 0.f, 1.f);

                camera_.pixels_.set(x + y * width, color);
            }
        }
    }
}

void Scene::printProgress(float progress) const
{
    const int progressRes = 10;    
    std::string progressText = "[";
    for (int i = 0; i < progressRes; ++i)
        {
        if (progress * progressRes > i)
            progressText += '=';
        else
            progressText += ' ';
        }
    progressText += ']';
    if (progress < 10)
        progressText += "  ";
    else if (progress < 100)
        progressText += ' ';
    
    std::cout << "Tracing " << name_ << " ";
    std::cout << greenText(progressText + std::to_string(int(progress * 100)) + "%\r");
    std::cout.flush();
}


bool Scene::getBlock(unsigned& startX, unsigned& endX, unsigned& startY, unsigned& endY)
{
    unsigned blockIndex = currentBlock_++;
    if (blockIndex < totalBlockCount_)
    {
        Block b = blocks_[blockIndex];

        startX = b.startX_;
        endX = b.endX_;
        startY = b.startY_;
        endY = b.endY_;

        return true;
    }
    return false;
}

int Scene::castRay(Ray & ray)
{
    int geomIndex = -1;
    Ray origRay = ray;
    for (unsigned i = 0; i < geometry_.size(); ++i)
    {
        Ray tempRay = origRay;
        if (geometry_[i]->raycast(tempRay) && ray.t_ > tempRay.t_ && tempRay.t_ >= 0)
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
    if (geomIndex != -1)
    {
        Vec3 collisionPoint = ray.intersection();
        auto& material = materials_[ray.materialID_];

        // Calculate light contribution
        Vec3 N;
        if (material.hasNormalMap())
        {
            Mat3 TBN;
            TBN.setCol(0, ray.tangent_);
            TBN.setCol(1, cross(ray.normal_, ray.tangent_));
            TBN.setCol(2, ray.normal_);
            N = TBN * (2.f * material.sampleNormalMap(ray.uv_) - 1.f);
            ray.normal_ = N;
        }
        else
            N = ray.normal_;

        Vec3 V = normalize(camera_.position_ - collisionPoint);

        // Lights
        for (auto& l : lights_)
        {
            Vec3 L = l->vectorFrom(collisionPoint);
            float distToLight = L.length();
            L.normalize();

            if (shadowSampleCount_ == 0)
                color = material.color(N, V, L, l->getColor(collisionPoint), ray.uv_);

            // Shadows
            for (unsigned i = 0; i < shadowSampleCount_; ++i)
            {
                Ray shadowRay = l->getShadowRay(collisionPoint, rayOffset_);
                if (castShadowRay(shadowRay, distToLight))
                    color = color + material.ambientColor();
                else
                    color = color + material.color(N, V, L, l->getColor(collisionPoint), ray.uv_);
            }
        }

        if (shadowSampleCount_ > 0)
        {
            color.r_ /= shadowSampleCount_;
            color.g_ /= shadowSampleCount_;
            color.b_ /= shadowSampleCount_;
        }

        if (material.hasTexture())
            color = color * material.sampleTexture(ray.uv_);
        if (material.isCheckered(collisionPoint))
            color = color * 0.25f;

        // Exit if recursive depth is met
        if (depth == 0)
            return color;

        // Calculate reflection and refraction rays
        if (material.isMetallic())
        {
            float r = material.reflectivity_;
            Ray reflectedRay(collisionPoint, reflect(ray.dir_, N), rayOffset_);
            Vec3 Ir = calculateColor(reflectedRay, depth - 1);
            color = (1.f - r) * color + r * Ir;
        }
        else if (material.isTransparent())
        {
            Vec3 Ir;
            Vec3 It;
            Vec3 k(1);
            float cosTheta;
            Ray reflectedRay(collisionPoint, reflect(ray.dir_, N), rayOffset_);

            // Ray is outside
            float dDotN = ray.dir_.dot(N);
            if (dDotN < 0)
            {
                Vec3 dir;
                refract(ray.dir_, N, ambientIOR_, material.ior_, dir); //TIR can't happen because air is <= goem ior
                It = calculateColor(Ray(collisionPoint, dir, rayOffset_), depth - 1);
                Ir = calculateColor(Ray(collisionPoint, reflect(ray.dir_, N), rayOffset_), depth - 1);
                cosTheta = -dDotN;
            }
            // Ray is inside
            else
            {
                Vec3 dir;
                k = material.attenuationColor(ray.t_);

                if (refract(ray.dir_, -1 * N, material.ior_, ambientIOR_, dir)) // check for TIR
                {
                    It = calculateColor(Ray(collisionPoint, dir, rayOffset_), depth - 1);
                    Ir = calculateColor(Ray(collisionPoint, reflect(ray.dir_, -1.f * N), rayOffset_), depth - 1);
                    cosTheta = dir.dot(N);
                }
                // Reflection only
                else
                    return k * calculateColor(Ray(collisionPoint, reflect(ray.dir_, N), rayOffset_), depth - 1);
            }

            float r = material.schlick(material.ior_, ambientIOR_, cosTheta);
            color = color + k * (r * Ir + (1.f - r) * It);
        }
    }
    else
        color = sampleBackground(ray.dir_);

    return color;
}

bool Scene::castShadowRay(Ray & ray, float distToLight)
{
    int geomIndex = castRay(ray);
    return geomIndex != -1 && ray.t_ <= distToLight;
}

void Scene::save()
{
    std::cout << "Saving Image...";
    camera_.pixels_.save(outputName_.c_str());
    std::cout << "Finished\n";
}

Vec3 Scene::sampleBackground(const Vec3 & dir) const
{
    if (skySphere_.isInitialized())
    {
        Vec2 uv = calculateUV(dir);
        return skySphere_.sample(uv.u_, uv.v_);
    }
    else if (backgroundSet_)
        return backgroundColor_;
    else
    {
        float s = cosf(((dir.y_ + 1.f) * .5f) * PI_);
        return lerp(s, normalize({ 135, 206, 235 }), normalize({ 250, 214, 165 }));
    }
}
