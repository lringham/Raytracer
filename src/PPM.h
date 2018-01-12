/*
char data[48] = {
0,0,0,   0,0,0,  0,0,0,  15,0,15,
0,0,0,   0,15,7, 0,0,0,  0,0,0,
0,0,0,   0,0,0,  0,15,7, 0,0,0,
15,0,15, 0,0,0,  0,0,0,  0,0,0 };

savePPM("test.ppm", 4, 4, data);
*/

#include <fstream>
#include <string>
bool savePPM(std::string filename, unsigned width, unsigned height, char* data, bool normalize = false)
{
	std::ofstream file(filename, std::ios::out | std::ios::binary);
	if (file.is_open())
	{
		file << "P6\n";
		file << "# Generated with Lee Ringham's Raytracer\n";
		file << width << " " << height << "\n";

		if (normalize)
		{
			char max = 0;
			for (unsigned long i = 0; i < width*height * 3; ++i)
				if (max < data[i])
					max = data[i];
			file << std::to_string(max) << "\n";
		}
		else
			file << "255\n";

		for (unsigned long i = 0; i < width*height*3; ++i)
			file << data[i];

		file.flush();
		file.close();
		return true;
	}
	return false;
}
