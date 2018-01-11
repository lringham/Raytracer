#include "Scene.h"

int main(int argc, char** argv)
{
	try {
		Scene scene(argc, argv);
		scene.trace();
		scene.save();
		return 0;
	}
	catch (int e) {
		return e;
	}
}
