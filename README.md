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

## TODO
- support png
- FOG
- Environment mapping
- Fresnel
- BVH or RTree?
- review usage info
- energy conserving phong

https://bheisler.github.io/post/writing-raytracer-in-rust-part-3/
