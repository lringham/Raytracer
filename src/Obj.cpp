#include "Obj.h"

#include "Triangle.h"
#include "AABB.h"
#include "Utils.h"

#include <fstream>
#include <iostream>

Obj::Obj()
{}

Obj::Obj(Vec3 position, const std::string& filename, const std::string& path, int materialID, std::vector<Material>& materials, std::map<std::string, int>& materialMap) :
    position_(position), path_("")
{
    materialID_ = materialID;
    if (!load(filename, path, materials, materialMap))
    {
        std::cout << "Cannot find " << path + filename << std::endl;
        exit(1);
    }
}

bool Obj::load(const std::string& filename, const std::string& path, std::vector<Material>& materials, std::map<std::string, int>& materialMap)
{
    path_ = path;
    OBJ_TYPE objTypeMap[3] = { V, VT, VTN };
    std::ifstream objReader(path + filename);

    int currentMaterial = materialID_;
    if (objReader.is_open())
    {
        OBJ_TYPE objType = NONE;
        char line[256];
        while (objReader.getline(line, 256))
        {
            if (objType == NONE && line[0] == 'f') //determine component type
            {
                int slashCount = 0;
                char c = ' ';
                for (int i = 2; c != '\0' && objType == NONE; ++i)
                {
                    c = line[i];
                    if (c == '/')
                    {
                        if (line[i + 1] == '/') //Check for double slash denoting v//n
                            objType = VN;
                        slashCount++;
                    }
                }

                if (objType != VN && slashCount <= 6)
                {
                    objType = objTypeMap[slashCount / 3];
                }
            }
            parseLine(line, objType, materials, materialMap, currentMaterial);
        }
        objReader.close();
    }
    else
    {
        std::cout << "Cannot find " << path + filename << std::endl;
        return false;
    }

    return true;
}

bool Obj::raycast(Ray & ray) const
{
    bool hit = false;
    float u1 = 0, v1 = 0;
    Face f1;

    for (auto& f : faces_)
    {
        Ray tempRay = ray;
        float u0, v0;
        if (raycastTri(
            position_ + positions_[f.v0_],
            position_ + positions_[f.v1_],
            position_ + positions_[f.v2_],
            tempRay, &u0, &v0))
        {
            hit = true;
            if (tempRay.t_ < ray.t_)
            {
                ray = tempRay;
                u1 = u0;
                v1 = v0;
                f1 = f;
            }
        }
    }

    if (hit)
    {
        float w1 = (1.f - (u1 + v1));
        if (normals_.size() > 0)
        {
            ray.normal_ = normalize(normals_[f1.n0_] * w1 +
                normals_[f1.n1_] * u1 +
                normals_[f1.n2_] * v1);
        }
        else
        {
            ray.normal_ = normalize(
                cross(
                    positions_[f1.v1_] - positions_[f1.v0_],
                    positions_[f1.v2_] - positions_[f1.v0_])
            );
        }

        if (hasTextureCoordinates())
        {
            ray.uv_ = textureCoords_[f1.t0_] * w1 +
                textureCoords_[f1.t1_] * u1 +
                textureCoords_[f1.t2_] * v1;

            ray.tangent_ = calculateTangent(
                positions_[f1.v0_],
                positions_[f1.v1_],
                positions_[f1.v2_],
                textureCoords_[f1.t0_],
                textureCoords_[f1.t1_],
                textureCoords_[f1.t2_]);
        }
    }
    return hit;
}

void Obj::loadMTLFile(const std::string & filename, std::vector<Material> & materials, std::map<std::string, int> & materialMap)
{
    std::vector<std::string> headings = { "newmtl", "Ns", "Ka", "Kd", "Ks", "Ke", "Ni", "d", "illum", "map_Kd", "map_Bump" };
    std::ifstream matReader(path_ + filename);

    if (matReader.is_open())
    {
        char line[256];
        int offset = 0;
        while (matReader.getline(line, 256))
        {
            std::string lineStr = std::string(line + offset);
            if (line[0] == '\0' || line[0] == '\n' || line[0] == '#' || lineStr.size() == 0)
                continue;

            std::string heading = "";
            for (auto& str : headings)
            {
                if (startsWith(line, str))
                {
                    offset = str.size() + 1;
                    heading = str;
                }
            }

            if (heading == "newmtl")
            {
                std::string name = std::string(line + offset);
                materialMap[name] = materials.size();
                materials.emplace_back(name);
            }
            else if (heading == "Ns") // specular pow
                materials.back().gloss_ = parsef(line + offset);
            else if (heading == "Ni") // optical_density
                materials.back().ior_ = parsef(line + offset);
            else if (heading == "Kd")
            {
                char* extra = nullptr;
                materials.back().kd_.r_ = parsef(line + offset, &extra);
                materials.back().kd_.g_ = parsef(extra, &extra);
                materials.back().kd_.b_ = parsef(extra);
            }
            else if (heading == "Ks")
            {
                char* extra = nullptr;
                materials.back().ks_.r_ = parsef(line + offset, &extra);
                materials.back().ks_.g_ = parsef(extra, &extra);
                materials.back().ks_.b_ = parsef(extra);
            }
            else if (heading == "map_Kd")
                materials.back().setTexture(path_ + std::string(line + offset));
            else if (heading == "map_Bump")
                materials.back().setNormalMap(path_ + std::string(line + offset));
            // else if(heading == "d")
            // 	; // transparancy or (1-tr)
            // else if(heading == "Ka")
            // 	; // amb color
            // else if(heading == "illum")
            // 	; // illumination model

        }
        matReader.close();
    }
    else
        std::cout << "Cannot open mtllib: " << path_ + filename << std::endl;
}

void Obj::parseLine(char* line, OBJ_TYPE objType, std::vector<Material> & materials, std::map<std::string, int> & materialMap, int& materialID)
{
    if (line == nullptr)
        return;

    //Determine the line type
    int offset;
    OBJ_TYPE lineType = NONE;
    if (startsWith(line, "mtllib")) //material
    {
        offset = 7;
        lineType = MATERIAL;
    }
    else if (startsWith(line, "usemtl")) //use materials
    {
        offset = 7;
        lineType = USE_MATERIAL;
    }
    else if (line[0] == 'o') // object
    {
        offset = 2;
        lineType = OBJECT;
    }
    else if (line[0] == 'f') //face
    {
        offset = 2;
        lineType = F;
    }
    else if (line[0] == 'v') //vertex
    {
        if (line[1] == ' ') // position
        {
            offset = 2;
            lineType = V;
        }
        else if (line[1] == 'n') // normal
        {
            offset = 3;
            lineType = VN;
        }
        else if (line[1] == 't') // texture coordinate
        {
            offset = 3;
            lineType = VT;
        }
    }

    //Convert Line Values
    switch (lineType)
    {
    case MATERIAL: //Parse a material
    {
        loadMTLFile(std::string(line + offset), materials, materialMap);
        break;
    }
    case USE_MATERIAL:
    {
        std::string filename = std::string(line + offset);
        if (materialMap.find(filename) != materialMap.end())
            materialID = materialMap[filename];
        break;
    }
    case OBJECT: //Parse an object
    {
        //std::cout << std::string(line+offset) << std::endl;
        //objects_.emplace_back(std::string(line+offset));
        break;
    }
    case F: //Parse a face
    {
        //replace all slashes with spaces to allow for conversion
        char c = ' ';
        for (int i = offset; c != '\0'; ++i)
        {
            c = line[i];
            if (c == '/')
                line[i] = ' ';
        }

        // parse face indices
        Face f;
        char* extra = nullptr;
        switch (objType)
        {
        case V:
            f.v0_ = parsel(line + offset, &extra) - 1;
            f.v1_ = parsel(extra, &extra) - 1;
            f.v2_ = parsel(extra) - 1;
            break;
        case VT:
            f.v0_ = parsel(line + offset, &extra) - 1;
            f.t0_ = parsel(extra, &extra) - 1;

            f.v1_ = parsel(extra, &extra) - 1;
            f.t1_ = parsel(extra, &extra) - 1;

            f.v2_ = parsel(extra, &extra) - 1;
            f.t2_ = parsel(extra) - 1;
            break;
        case VN:
            f.v0_ = parsel(line + offset, &extra) - 1;
            f.n0_ = parsel(extra, &extra) - 1;

            f.v1_ = parsel(extra, &extra) - 1;
            f.n1_ = parsel(extra, &extra) - 1;

            f.v2_ = parsel(extra, &extra) - 1;
            f.n2_ = parsel(extra) - 1;
            break;
        case VTN:
            f.v0_ = parsel(line + offset, &extra) - 1;
            f.t0_ = parsel(extra, &extra) - 1;
            f.n0_ = parsel(extra, &extra) - 1;

            f.v1_ = parsel(extra, &extra) - 1;
            f.t1_ = parsel(extra, &extra) - 1;
            f.n1_ = parsel(extra, &extra) - 1;

            f.v2_ = parsel(extra, &extra) - 1;
            f.t2_ = parsel(extra, &extra) - 1;
            f.n2_ = parsel(extra) - 1;
            break;
        case F:
        case OBJECT:
        case MATERIAL:
        case USE_MATERIAL:
        case NONE:
            break;
        }

        faceMaterialMap_[faces_.size()] = materialID;
        faces_.push_back(f);
        break;
    }
    case V: //Parse a position
    {
        char* extra = nullptr;
        float x = parsef(line + offset, &extra);
        float y = parsef(extra, &extra);
        float z = parsef(extra);
        positions_.emplace_back(x, y, z);
        break;
    }
    case VN: //Parse a normal
    {
        char* extra = nullptr;
        float x = parsef(line + offset, &extra);
        float y = parsef(extra, &extra);
        float z = parsef(extra);
        normals_.emplace_back(x, y, z);
        break;
    }
    case VT:  //Parse a texture coordinate
    {
        char* extra = nullptr;
        float x = parsef(line + offset, &extra);
        float y = parsef(extra);
        textureCoords_.emplace_back(x, y);
        break;
    }
    case VTN:
    case NONE:
        break;
    }
}
