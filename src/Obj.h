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
        unsigned _v0 = 0, _t0 = 0, _n0 = 0,
        _v1 = 0, _t1 = 0, _n1 = 0,
        _v2 = 0, _t2 = 0, _n2 = 0;
    };

    Obj();
    Obj(Vec3 position, const std::string& filename, const std::string& path, int materialID, std::vector<Material>& materials, std::map<std::string, int>& materialMap);
    bool load(const std::string& filename, const std::string& path, std::vector<Material>& materials, std::map<std::string, int>& materialMap);
    bool raycast(Ray& ray) const;

    inline bool hasNormals() const
    {
        return _normals.size() > 0;
    }

    inline bool hasTextureCoordinates() const
    {
        return _textureCoords.size() > 0;
    }

    Vec3 _position;
    const std::string _name;
    std::vector<Vec3> _positions;
    std::vector<Vec3> _normals;
    std::vector<Vec2> _textureCoords;
    std::vector<Face> _faces;
    std::vector<unsigned> _indices;
    std::map<int, int> _faceMaterialMap;

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

    std::string _path;
};
