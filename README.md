# Raytracer
A simple raytracer made for fun.

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
- BVH slow at certain angles

## TODO
- review usage info
- linear texture sampling
- FOG
- make a few interesting scenes showing off features
- clean todos and fixmes