#pragma once
#include "Tracable.h"
#include "Triangle.h"
#include <vector>

#include <fstream>
#include <vector>
#include <iostream>
#include <string>


#include "Vec3.h"
#include "Vec2.h"


class Obj : public Tracable
{

private:
	struct Face
	{
		unsigned v0, t0, n0,
    				 v1, t1, n1,
    				 v2, t2, n2;
	};
	enum OBJ_TYPE
	{
		V = 0, VT = 3, VTN = 6, VN, F, NONE
	};

  std::vector<Vec3> positions;
  std::vector<Vec3> normals;
  std::vector<Vec2> textureCoords;
  std::vector<Face> faces;
	Vec3 _position;

public:
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
    for(auto& f : faces)
		{
			Ray tempRay = ray;
			if(raycastTri(
				_position + positions[f.v0],
				_position + positions[f.v1],
				_position + positions[f.v2],
				tempRay))
			{
				hit = true;
				if(tempRay._t < ray._t)
					ray = tempRay;
			}
		}
		return hit;
  }

	bool load(const std::string& filename, const std::string& path = "")
	{
		std::ifstream objReader(path + filename + ".obj");
		if(objReader.is_open())
		{
			OBJ_TYPE objType = NONE;

			char line[256];
			while (objReader.getline(line, 256))
			{
				if (objType != NONE)
					parseLine(line, objType, positions, normals, textureCoords, faces);
				else //determine component type
				{
					// -----------------------------------------
					// If componentType is not found,
					// and the line defines a face,
					// use the '/' count and placement
					// to determine how to parse face lines.
					// componentType designates if normals and or
					// texture coordinates are avaliable
					// -----------------------------------------
					if (line[0] == 'f')
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
					parseLine(line, objType, positions, normals, textureCoords, faces);
				}
			}
			objReader.close();
			parseFaces(objType, positions, normals, textureCoords, faces);
		}
		else
		{
			std::cout << "Can't load model: " << filename << std::endl;
			return false;
		}


		// Material loading
		//std::ifstream mtlReader(path + filename + ".mtl");
		//if (mtlReader.is_open())
		//{
		//	char line[256];
		//	while (objReader.getline(line, 256))
		//	{
		//		if (line[0] == '#')
		//			continue;
		//	}

		//	mtlReader.close();
		//	return true;
		//}
		//else
		//{
		//	std::cout << "Can't load material: " << filename << std::endl;
		//	return false;
		//}

		return true;
	}

private:
	inline float parsef(const char* data, char** extra = nullptr)
	{
		return strtof(data, extra);
	}

	inline long parsel(const char* data, char** extra = nullptr, int base = 0)
	{
		return strtol(data, extra, base);
	}

	void parseLine(char* line, OBJ_TYPE objType, std::vector<Vec3>& positions, std::vector<Vec3>& normals, std::vector<Vec2>& textureCoords, std::vector<Face>& faces)
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
				}
				faces.push_back(f);
				break;
			}
			case V: //Parse a position
			{
				char* extra = nullptr;
				float x = parsef(line + offset, &extra);
				float y = parsef(extra, &extra);
				float z = parsef(extra);
				positions.emplace_back(x, y, z);
				break;
			}
			case VN: //Parse a normal
			{
				char* extra = nullptr;
				float x = parsef(line + offset, &extra);
				float y = parsef(extra, &extra);
				float z = parsef(extra);
				normals.emplace_back(x, y, z);
				break;
			}
			case VT:  //Parse a texture coordinate
			{
				char* extra = nullptr;
				float x = parsef(line + offset, &extra);
				float y = parsef(extra);
				textureCoords.emplace_back(x, y);
				break;
			}
		}
	}

	void parseFaces(OBJ_TYPE objType, std::vector<Vec3>& parsedPositions, std::vector<Vec3>& parsedNormals, std::vector<Vec2>& parsedTextureCoords, std::vector<Face>& faces)
	{
		std::vector<Vec3> positions;
		std::vector<Vec3> normals;
		std::vector<unsigned> indices;
		std::vector<Vec2> textureCoords;

		//set all the indices of the triangles
		positions.resize(parsedPositions.size());
		switch (objType)
		{
		case VTN:
			normals.resize(parsedPositions.size());
			textureCoords.resize(parsedPositions.size());
			for (Face& face : faces)
			{
				indices.push_back(face.v0);
				indices.push_back(face.v1);
				indices.push_back(face.v2);

				positions[face.v0] = parsedPositions[face.v0];
				positions[face.v1] = parsedPositions[face.v1];
				positions[face.v2] = parsedPositions[face.v2];

				normals[face.v0] = parsedNormals[face.n0];
				normals[face.v1] = parsedNormals[face.n1];
				normals[face.v2] = parsedNormals[face.n2];

				textureCoords[face.v0] = parsedTextureCoords[face.t0];
				textureCoords[face.v1] = parsedTextureCoords[face.t1];
				textureCoords[face.v2] = parsedTextureCoords[face.t2];
			}
			break;
		case VN:
			normals.resize(parsedPositions.size());
			for (Face& face : faces)
			{
				indices.push_back(face.v0);
				indices.push_back(face.v1);
				indices.push_back(face.v2);

				positions[face.v0] = parsedPositions[face.v0];
				positions[face.v1] = parsedPositions[face.v1];
				positions[face.v2] = parsedPositions[face.v2];

				normals[face.v0] = parsedNormals[face.n0];
				normals[face.v1] = parsedNormals[face.n1];
				normals[face.v2] = parsedNormals[face.n2];
			}
			break;
		case VT:
			textureCoords.reserve(parsedPositions.size());
			for (Face& face : faces)
			{
				indices.push_back(face.v0);
				indices.push_back(face.v1);
				indices.push_back(face.v2);

				positions[face.v0] = parsedPositions[face.v0];
				positions[face.v1] = parsedPositions[face.v1];
				positions[face.v2] = parsedPositions[face.v2];

				textureCoords[face.v0] = parsedTextureCoords[face.t0];
				textureCoords[face.v1] = parsedTextureCoords[face.t1];
				textureCoords[face.v2] = parsedTextureCoords[face.t2];
			}
			break;
		case V:
			for (Face& face : faces)
			{
				indices.push_back(face.v0);
				indices.push_back(face.v1);
				indices.push_back(face.v2);

				positions[face.v0] = parsedPositions[face.v0];
				positions[face.v1] = parsedPositions[face.v1];
				positions[face.v2] = parsedPositions[face.v2];
			}
			break;
		}
	}
};
