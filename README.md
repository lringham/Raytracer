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
- review usage info
- clean todos and fixmes
- Add tangent calculation to all geometry
- Render a scene with all types of geometry
- finish implementing boxes
- Add sky box
- make a few interesting scenes showing off features
- list all features
- add control of metal reflectivity