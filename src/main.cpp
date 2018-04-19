#include "Scene.h"

int main(int argc, char** argv)
{
    Scene scene;
    if(scene.init(argc, argv))
    {
        scene.trace();
        scene.save();
        return 0;
    }
    else
        return -1;
}
