#include "Obj.h"

#include "Triangle.h"
#include "AABB.h"
#include "Utils.h"

#include <fstream>
#include <iostream>

Obj::Obj()
{}

Obj::Obj(Vec3 position, const std::string& filename, const std::string& path, int materialID, std::vector<Material>& materials, std::map<std::string, int>& materialMap) :
    _position(position), _path("")
{
    _materialID = materialID;
    load(filename, path, materials, materialMap);
}

bool Obj::load(const std::string& filename, const std::string& path, std::vector<Material>& materials, std::map<std::string, int>& materialMap)
{
    _path = path;
    OBJ_TYPE objTypeMap[3] = {V, VT, VTN};
    std::ifstream objReader(path + filename);
    int currentMaterial = _materialID;		
    if(objReader.is_open())
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
                    
                    if(objType != VN && slashCount <= 6)
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
        return false;
    }

    return true;
}

bool Obj::raycast(Ray& ray) const
{
    bool hit = false;
    float u1 = 0, v1 = 0;
    Face f1;

    for(auto& f : _faces)
    {
        Ray tempRay = ray;
        float u0, v0;
        if(raycastTri(
            _position + _positions[f._v0],
            _position + _positions[f._v1],
            _position + _positions[f._v2],
            tempRay, &u0, &v0))
        {
            hit = true;
            if(tempRay._t < ray._t)
            {
                ray = tempRay;
                u1 = u0;
                v1 = v0;
                f1 = f;
            }
        }
    }
    
    if(hit)
    {
        float w1 = (1.f-(u1+v1));
        if(_normals.size() > 0)
        {
            ray._normal = normalize(_normals[f1._n0] * w1 +
                                    _normals[f1._n1] * u1 +
                                    _normals[f1._n2] * v1);
        }
        else
        {
            ray._normal = normalize(
                    cross(
                        _positions[f1._v1] - _positions[f1._v0], 
                        _positions[f1._v2] - _positions[f1._v0])
                    );
        }

        if(hasTextureCoordinates())
        {
            ray._uv = _textureCoords[f1._t0] * w1 +
                        _textureCoords[f1._t1] * u1 +
                        _textureCoords[f1._t2] * v1;

            ray._tangent = calculateTangent(
                    _positions[f1._v0],
                    _positions[f1._v1],
                    _positions[f1._v2],
                    _textureCoords[f1._t0],
                    _textureCoords[f1._t1],
                    _textureCoords[f1._t2]);
        }
    }
    return hit;
}

void Obj::loadMTLFile(const std::string& filename, std::vector<Material>& materials, std::map<std::string, int>& materialMap)
{
    std::vector<std::string> headings = {"newmtl", "Ns", "Ka", "Kd", "Ks", "Ke", "Ni", "d", "illum", "map_Kd", "map_Bump"};
    std::ifstream matReader(_path + filename);
    
    if(matReader.is_open())
    {
        char line[256];	
        int offset = 0;					
        while (matReader.getline(line, 256))
        {		
            std::string lineStr = std::string(line + offset);
            if(line[0] == '\0' || line[0] == '\n' || line[0] == '#' || lineStr.size() == 0)
                continue;
                    
            std::string heading = "";
            for(auto& str : headings)
            {
                if(startsWith(line, str))
                {
                    offset = str.size()+1;
                    heading = str;
                }
            }

            if(heading == "newmtl")
            {
                std::string name = std::string(line +  offset);
                materialMap[name] = materials.size();
                materials.emplace_back(name);					
            }
            else if(heading == "Ns") // specular pow
                materials.back()._gloss = parsef(line + offset);
            else if(heading == "Ni") // optical_density
                materials.back()._eta = parsef(line + offset);
            else if(heading == "Kd")
            {
                char* extra = nullptr;
                materials.back()._kd._r = parsef(line + offset, &extra);
                materials.back()._kd._g = parsef(extra, &extra);
                materials.back()._kd._b = parsef(extra);
            }
            else if(heading == "Ks")
            {
                char* extra = nullptr;
                materials.back()._ks._r = parsef(line + offset, &extra);
                materials.back()._ks._g = parsef(extra, &extra);
                materials.back()._ks._b = parsef(extra);
            }
            else if(heading == "map_Kd")
                materials.back().setTexture(_path + std::string(line + offset));
            else if(heading == "map_Bump")
                materials.back().setNormalMap(_path + std::string(line + offset));
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
        std::cout << "Cannot open mtllib : " << _path + filename << std::endl;
}

void Obj::parseLine(char* line, OBJ_TYPE objType, std::vector<Material>& materials, std::map<std::string, int>& materialMap, int& materialID)
{
    if(line == nullptr)
        return;

    //Determine the line type
    int offset;
    OBJ_TYPE lineType = NONE;
    if(startsWith(line, "mtllib")) //material
    {
        offset = 7;
        lineType = MATERIAL;
    }
    else if(startsWith(line, "usemtl")) //use materials
    {
        offset = 7;
        lineType = USE_MATERIAL;
    }
    else if(line[0] == 'o') // object
    {
        offset = 2;
        lineType = OBJECT;
    }			
    else if(line[0] == 'f') //face
    {
        offset = 2;
        lineType = F;
    }
    else if(line[0] == 'v') //vertex
    {
        if(line[1] == ' ') // position
        {
            offset = 2;
            lineType = V;
        }
        else if(line[1] == 'n') // normal
        {
            offset = 3;
            lineType = VN;
        }
        else if(line[1] == 't') // texture coordinate
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
            loadMTLFile(std::string(line+offset), materials, materialMap);
            break;
        }
        case USE_MATERIAL:
        {
            materialID = materialMap[std::string(line+offset)];
            break;
        }
        case OBJECT: //Parse an object
        {
            //std::cout << std::string(line+offset) << std::endl;
            //_objects.emplace_back(std::string(line+offset));
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
                f._v0 = parsel(line + offset, &extra) - 1;
                f._v1 = parsel(extra, &extra) - 1;
                f._v2 = parsel(extra) - 1;
                break;
            case VT:
                f._v0 = parsel(line + offset, &extra) - 1;
                f._t0 = parsel(extra, &extra) - 1;

                f._v1 = parsel(extra, &extra) - 1;
                f._t1 = parsel(extra, &extra) - 1;

                f._v2 = parsel(extra, &extra) - 1;
                f._t2 = parsel(extra) - 1;
                break;
            case VN:
                f._v0 = parsel(line + offset, &extra) - 1;
                f._n0 = parsel(extra, &extra) - 1;

                f._v1 = parsel(extra, &extra) - 1;
                f._n1 = parsel(extra, &extra) - 1;

                f._v2 = parsel(extra, &extra) - 1;
                f._n2 = parsel(extra) - 1;
                break;
            case VTN:
                f._v0 = parsel(line + offset, &extra) - 1;
                f._t0 = parsel(extra, &extra) - 1;
                f._n0 = parsel(extra, &extra) - 1;

                f._v1 = parsel(extra, &extra) - 1;
                f._t1 = parsel(extra, &extra) - 1;
                f._n1 = parsel(extra, &extra) - 1;

                f._v2 = parsel(extra, &extra) - 1;
                f._t2 = parsel(extra, &extra) - 1;
                f._n2 = parsel(extra) - 1;
                break;
            case F:
            case OBJECT:
            case MATERIAL:
            case USE_MATERIAL:
            case NONE:
                break;
            }

            _faceMaterialMap[_faces.size()] = materialID; 
            _faces.push_back(f);
            break;
        }
        case V: //Parse a position
        {
            char* extra = nullptr;
            float x = parsef(line + offset, &extra);
            float y = parsef(extra, &extra);
            float z = parsef(extra);
            _positions.emplace_back(x, y, z);
            break;
        }
        case VN: //Parse a normal
        {
            char* extra = nullptr;
            float x = parsef(line + offset, &extra);
            float y = parsef(extra, &extra);
            float z = parsef(extra);
            _normals.emplace_back(x, y, z);
            break;
        }
        case VT:  //Parse a texture coordinate
        {
            char* extra = nullptr;
            float x = parsef(line + offset, &extra);
            float y = parsef(extra);
            _textureCoords.emplace_back(x, y);
            break;
        }
        case VTN:
        case NONE:
            break;
    }
}