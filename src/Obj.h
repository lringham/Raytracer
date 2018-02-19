#pragma once
#include "Tracable.h"
#include "Triangle.h"
#include "Vec3.h"
#include "Vec2.h"
#include "AABB.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class Obj : public Tracable
{
public:
	struct Face
	{
		unsigned	v0 = 0, t0 = 0, n0 = 0,
					v1 = 0, t1 = 0, n1 = 0,
					v2 = 0, t2 = 0, n2 = 0;
	};

	Obj()
	{
	}

	Obj(Vec3 position, const std::string& filename, const std::string& path = "") :
		_position(position)
	{
		load(filename, path);
	}

	~Obj()
	{
	}

	bool raycast(Ray& ray) const override
	{
	bool hit = false;
	float u1 = 0, v1 = 0;
	Face f1;

	// Ray testRay = ray;
	// if(!_boundingBox.raycast(testRay))
	// 	return false;

	for(auto& f : _faces)
	{
		Ray tempRay = ray;
		float u0, v0;
		if(raycastTri(
			_position + _positions[f.v0],
			_position + _positions[f.v1],
			_position + _positions[f.v2],
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
			ray._normal = normalize(_normals[f1.n0] * w1 +
															_normals[f1.n1] * u1 +
															_normals[f1.n2] * v1);
		}
		else
		{
			ray._normal = normalize(cross(_positions[f1.v1] - _positions[f1.v0], _positions[f1.v2] - _positions[f1.v0]));
		}

		if(_textureCoords.size() > 0)
		{
			//TODO not sure why this order is needed
			ray._uv = _textureCoords[f1.t0] * w1 +
								_textureCoords[f1.t1] * u1 +
								_textureCoords[f1.t2] * v1;
		}
	}
	return hit;
	}

	// void calcAABB()
	// {
	// 	// Calculate AABB of vertices in models space
	// 	// and then translate them into world space
	// 	_boundingBox = AABB(_positions);
	// 	_boundingBox.setPosition(_position);
	// }

	bool load(const std::string& filename, const std::string& path = "")
	{
		std::ifstream objReader(path + filename);
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
						if(objType != VN)
							objType = (OBJ_TYPE) slashCount;
				}
				parseLine(line, objType);
			}
			objReader.close();
		}
		else
		{
			std::cout << "Can't load model: " << filename << std::endl;
			return false;
		}

		// Calculate bounding box
		//calcAABB();

		// Print stats
		std::cout << "Model loaded: " << filename << std::endl;
		std::cout << "\tfaces: " << _faces.size() << std::endl;
		std::cout << "\tvertices: " << _positions.size() << std::endl;
		std::cout << "\tnormals: " << _normals.size() << std::endl;
		std::cout << "\tuvs: " << _textureCoords.size() << std::endl;

		return true;
	}

	std::vector<Vec3> _positions;
	std::vector<Vec3> _normals;
	std::vector<Vec2> _textureCoords;
	std::vector<Face> _faces;
	std::vector<unsigned> _indices;
	Vec3 _position;

private:	
	enum OBJ_TYPE
	{
		V = 0, VT = 3, VTN = 6, VN, F, NONE
	};

	inline float parsef(const char* data, char** extra = nullptr)
	{
		return strtof(data, extra);
	}

	inline long parsel(const char* data, char** extra = nullptr, int base = 0)
	{
		return strtol(data, extra, base);
	}

	void parseLine(char* line, OBJ_TYPE objType)
	{
		if(line == nullptr)
			return;

		//Determine the line type
		OBJ_TYPE lineType = NONE;
		int offset;
		switch(line[0])
		{
			case 'f': //face
				offset = 2;
				lineType = F;
				break;
			case 'v': //vertex
				switch(line[1])
				{
				case ' ': // position
					offset = 2;
					lineType = V;
					break;
				case 'n': // normal
					offset = 3;
					lineType = VN;
					break;
				case 't': // texture coordinate
					offset = 3;
					lineType = VT;
					break;
				}
				break;
		}

		//Convert Line Values
		switch (lineType)
		{
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
					f.v0 = parsel(line + offset, &extra) - 1;
					f.v1 = parsel(extra, &extra) - 1;
					f.v2 = parsel(extra) - 1;
					break;
				case VT:
					f.v0 = parsel(line + offset, &extra) - 1;
					f.t0 = parsel(extra, &extra) - 1;

					f.v1 = parsel(extra, &extra) - 1;
					f.t1 = parsel(extra, &extra) - 1;

					f.v2 = parsel(extra, &extra) - 1;
					f.t2 = parsel(extra) - 1;
					break;
				case VN:
					f.v0 = parsel(line + offset, &extra) - 1;
					f.n0 = parsel(extra, &extra) - 1;

					f.v1 = parsel(extra, &extra) - 1;
					f.n1 = parsel(extra, &extra) - 1;

					f.v2 = parsel(extra, &extra) - 1;
					f.n2 = parsel(extra) - 1;
					break;
				case VTN:
					f.v0 = parsel(line + offset, &extra) - 1;
					f.t0 = parsel(extra, &extra) - 1;
					f.n0 = parsel(extra, &extra) - 1;

					f.v1 = parsel(extra, &extra) - 1;
					f.t1 = parsel(extra, &extra) - 1;
					f.n1 = parsel(extra, &extra) - 1;

					f.v2 = parsel(extra, &extra) - 1;
					f.t2 = parsel(extra, &extra) - 1;
					f.n2 = parsel(extra) - 1;
					break;
				case F:
					break;
				case NONE:
					break;
				}
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
				break;
			case NONE:
				break;
		}
	}
};
