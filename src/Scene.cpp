#include <thread>
#include <iostream>
#include <fstream>
#include <sstream>
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
    std::cout << "Loading scene... \n";
    if (loadScene(argc, argv))
        return true;
    else
    {
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

    if (inbetweenExc(argc, 0, 4))
    {
        if(argc > 1)
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
                else
                    return false;
            }
        }        
    }
    else
        return false;

    return true;
}

bool Scene::loadScene(int argc, char** argv)
{
    std::string sceneFilename = "";
    if (!parseArgs(argc, argv, sceneFilename))
        return false;

    std::map<std::string, int> materialMap;
    try
    {
        // Try to load the specified scene, if the file cannot be opened
        // a default scene is used.
        YAML::Node config;
        {
            if (sceneFilename == "")
            {
                std::istringstream iss(defaultScene);
                config = YAML::Load(iss);
            }
            else
            {
                std::ifstream file(sceneFilename);
                if (file.is_open())
                {
                    file.close();
                    config = YAML::LoadFile(sceneFilename);
                }
                else
                {
                    std::cout << "Cannot load scene: " << sceneFilename << "\n";
                    return false;
                }
            }
        }

        std::string modelsDir = "", skySphere = "";
        parseNode(config, "modelsDir", modelsDir);
        parseNode(config, "outputName", outputName_);
        parseNode(config, "name", name_);
        parseNode(config, "recursionDepth", depth_);
        parseNode(config, "shadowSampleCount", shadowSampleCount_);

        // parse background
        if(parseNode(config, "skySphere", skySphere))
            skySphere_.loadTexture(skySphere);

        Vec3 backgroundColor(0, 0, 0);
        backgroundSet_ = parseNode(config, "backgroundColor", backgroundColor);
        backgroundColor_.set(backgroundColor);

        // parse materials
        const YAML::Node& materialsNode = config["materials"];
        for (YAML::const_iterator it = materialsNode.begin(); it != materialsNode.end(); ++it)
        {
            const YAML::Node& materialNode  = *it;
            std::string materialName        = "materialName";
            float Ia 				        = 0.1f;
            Vec3 kd 				        = Vec3(1.f, 1.f, 1.f);
            Vec3 ks 			            = Vec3(1.f, 1.f, 1.f);
            Vec3 attenuation 	            = Vec3(1.f, 1.f, 1.f);
            float gloss 		            = 1.f;
            float ior 			            = 1.f;
            float reflectivity 	            = 0.f;
            std::string type 	            = "blinnPhong";
            bool checkered 		            = false;

            if(!parseNode(materialNode, "name", materialName)) 
                throw YAML::BadFile();

            parseNode(materialNode, "Ia", Ia);
            parseNode(materialNode, "kd", kd);
            parseNode(materialNode, "ks", ks);
            parseNode(materialNode, "attenuation", attenuation);
            parseNode(materialNode, "gloss", gloss);
            parseNode(materialNode, "ior", ior);
            parseNode(materialNode, "reflectivity", reflectivity);
            parseNode(materialNode, "type", type);
            parseNode(materialNode, "checkered", checkered);

            materialMap[materialName] = materials_.size();
            materials_.emplace_back(
                materialName, 
                Ia, kd, ks, 
                attenuation, 
                gloss, 
                ior, 
                reflectivity, 
                type, 
                checkered);

            std::string texture = "", normalMap = "", specularMap = "";
            if (parseNode(materialNode, "texture", texture))
                materials_.back().setTexture(texture);
            if (parseNode(materialNode, "normalMap", normalMap))
                materials_.back().setNormalMap(normalMap); 
            if (parseNode(materialNode, "specularMap", specularMap))
                materials_.back().setNormalMap(specularMap);
        }

        // parse objects
        const YAML::Node& objectsNode = config["objects"];
        for (YAML::const_iterator it = objectsNode.begin(); it != objectsNode.end(); ++it)
        {
            const YAML::Node& object = *it;
            std::string type = "sphere";
            parseNode(object, "type", type);

            if (type == "sphere")
            {
                float radius = 1.f;
                Vec3 position(0, 0, 0);
                bool invertNormals = false;

                parseNode(object, "radius", radius);
                parseNode(object, "position", position);
                parseNode(object, "invertNormals", invertNormals);
                geometry_.emplace_back(new Sphere(radius, position, invertNormals));
            }
            else if (type == "triangle")
            {
                Vec3 x0(-1, 0, 0), x1(0, 1, 0), x2(1, 0, 0);

                parseNode(object, "x0", x0);
                parseNode(object, "x1", x1);
                parseNode(object, "x2", x2);
                geometry_.emplace_back(new Triangle(x0, x1, x2));
            }
            else if (type == "plane")
            {
                Vec3 normal(0, 1, 0), position(0, 0, 0);
                float width = std::numeric_limits<float>::max(), height = std::numeric_limits<float>::max();

                parseNode(object, "normal", normal),
                parseNode(object, "position", position),
                parseNode(object, "width", width),
                parseNode(object, "height", height);
                geometry_.emplace_back(new Plane(normal, position, width, height));
            }
            else if (type == "model")
            {
                std::string matName = "", filename = "";
                parseNode(object, "material", matName);
                parseNode(object, "filename", filename);

                Obj obj;
                parseNode(object, "position", obj.position_);
                obj.materialID_ = matName != "" ? materialMap[matName] : -1;
                if (!obj.load(filename, modelsDir, materials_, materialMap))
                    return false;
                geometry_.emplace_back(new BVH(obj));
            }
            else
                std::cout << "Invalid geometry type: " << type << std::endl;

            std::string materialName = "";
            parseNode(object, "material", materialName);
            geometry_.back()->materialID_ = materialMap[materialName];
        }

        // parse lights
        const YAML::Node& lights = config["lights"];
        for (YAML::const_iterator it = lights.begin(); it != lights.end(); ++it)
        {
            const YAML::Node& light = *it;
            std::string type = "";
            parseNode(light, "type", type);
            
            if (type == "point")
            {
                Vec3 color(1, 1, 1), position(0, 0, 0);
                float intensity = 100.f, radius = 0.f;

                parseNode(light, "color", color);
                parseNode(light, "position", position);
                parseNode(light, "intensity", intensity);
                parseNode(light, "radius", radius);

                lights_.emplace_back(new PointLight(color, position, intensity, radius));
            }
            else if (type == "spot")
            {
                Vec3 color(1, 1, 1), position(0, 0, 0), direction(0, -1, 0);
                float intensity = 100.f, cosThetaP = .5f, cosThetaU = .7f, exp = 1.f;

                parseNode(light, "color", color);
                parseNode(light, "position", position);
                parseNode(light, "intensity", intensity);
                parseNode(light, "direction", direction);
                parseNode(light, "cosThetaP", cosThetaP);
                parseNode(light, "cosThetaU", cosThetaU);
                parseNode(light, "exp", exp);

                lights_.emplace_back(new SpotLight(color, position, intensity, direction, cosThetaP, cosThetaU, exp));
            }
            else if (type == "directional")
            {
                Vec3 color(1, 1, 1), direction(0, -1, 0);
                float intensity = 100.f;

                parseNode(light, "color", color);
                parseNode(light, "intensity", intensity);
                parseNode(light, "direction", direction);

                lights_.emplace_back(new DirectionalLight(color, intensity, direction));
            }
        }

        // create camera
        Vec3 position(0, 0, 1), direction(0, 0, -1);
        float fov = 0.f, focalLength = 1.f, lensRadius = 0.f;
        unsigned pxWidth = 100, pxHeight = 100, sampleCount = 1;

        if (config["camera"])
        {
            auto cameraNode = config["camera"];
            parseNode(cameraNode, "position", position);
            parseNode(cameraNode, "direction", direction);
            parseNode(cameraNode, "fov", fov);
            parseNode(cameraNode, "focalLength", focalLength);
            parseNode(cameraNode, "pxWidth", pxWidth);
            parseNode(cameraNode, "pxHeight", pxHeight);
            parseNode(cameraNode, "sampleCount", sampleCount);
            parseNode(cameraNode, "lensRadius", lensRadius);
        }

        camera_.init(
            position, 
            direction, 
            fov, 
            focalLength, 
            Pixels(pxWidth, pxHeight), 
            sampleCount, 
            lensRadius);
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
    std::cout << "\tThread count: " << threadCount_ << "\n";
    std::cout << "\tRecursion depth: " << depth_ << "\n";
    std::cout << "\tPixel sample count: " << camera_.sampleCount() << "\n";
    std::cout << "\tShadow sample count: " << shadowSampleCount_ << "\n";
    
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

    std::cout << "\n";
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
