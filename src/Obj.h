#pragma once
#include "Tracable.h"
#include "Vec3.h"
#include "Vec2.h"
#include <string>
#include <vector>
#include <map>

class Obj : public Tracable
{
public:
    struct Face
    {
        unsigned v0_ = 0, t0_ = 0, n0_ = 0,
            v1_ = 0, t1_ = 0, n1_ = 0,
            v2_ = 0, t2_ = 0, n2_ = 0;
    };

    Obj();
    Obj(Vec3 position, const std::string& filename, const std::string& path, int materialID, std::vector<Material>& materials, std::map<std::string, int>& materialMap);
    bool load(const std::string& filename, const std::string& path, std::vector<Material>& materials, std::map<std::string, int>& materialMap);
    bool raycast(Ray& ray) const;

    inline bool hasNormals() const
    {
        return normals_.size() > 0;
    }

    inline bool hasTextureCoordinates() const
    {
        return textureCoords_.size() > 0;
    }

    Vec3 position_;
    const std::string name_;
    std::vector<Vec3> positions_;
    std::vector<Vec3> normals_;
    std::vector<Vec2> textureCoords_;
    std::vector<Face> faces_;
    std::vector<unsigned> indices_;
    std::map<int, int> faceMaterialMap_;

private:
    enum OBJ_TYPE
    {
        V, VT, VTN, VN, F, OBJECT, MATERIAL, USE_MATERIAL, NONE
    };

    enum MATERIAL
    {

    };

    inline float parsef(const char* data, char** extra = nullptr)
    {
        return strtof(data, extra);
    }

    inline long parsel(const char* data, char** extra = nullptr, int base = 0)
    {
        return strtol(data, extra, base);
    }

    void parseLine(char* line, OBJ_TYPE objType, std::vector<Material>& materials, std::map<std::string, int>& materialMap, int& materialID);
    void loadMTLFile(const std::string& filename, std::vector<Material>& materials, std::map<std::string, int>& materialMap);

    std::string path_;
};
