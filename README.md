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
- shadow sample count has to be greater than 0

## TODO
- BVH
- fix transparency
- Fresnel
- review usage info
- linear texture sampling
- FOG
- energy conserving phong
- openGL preview?
- make a few interesting scenes showing off features
- add to github
- networked support 
- make algorithms cache friendly 
- deal with mem leaks in BVH

https://bheisler.github.io/post/writing-raytracer-in-rust-part-3/
