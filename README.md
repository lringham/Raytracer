# Raytracer
A simple raytracer made for fun. Very much a work in progress.

## How to Build
```
cd Raytracer/build
cmake ..
make
```

## How to Run
```
./Raytracer ../scenes/scene.yaml [-threads=8] [-w=800] [-h=600] [-recurDepth=6]
```
## Bugs and Limitations
- Uses non PBR

## TODO
- clean todos and fixmes
- Render a scene with all types of geometry
- Add sky box
- make a few interesting scenes showing off features

## Features
- Multi-threaded
- Plane, Sphere, Triangles, OBJ/MTL files
- BlinnPhong, Relfectivity, and Transparancy 
- Point and directional lights
- Sky spheres
- Textures
- Normal Mapping
- BVH acceleration for OBJ models
- YAML scene specification
- PNG, JPG, BMP input
- PPM output
- Anti-aliasing via multiple samples
- Soft shadows