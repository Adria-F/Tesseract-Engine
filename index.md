# 3D Tesseract-Engine

This is a 3D Engine created as the main project of the Engines subject of the
 [Bachelor’s Degree in Video Games by UPC at CITM](https://www.citm.upc.edu/ing/estudis/graus-videojocs/).
 We are using different libraries to create our own 3D engine with some basics features and then, for the final delivery, we are a creating one "high-level" system.
 In this case, it will be skeletal animation.
 
 
## Contributors:
This engine is created by:

<img src="https://drive.google.com/uc?id=1vQUdU2pbTyUeGBdSpkp46poJoaWvpupi" width="100">

[Adrià Ferrer](https://github.com/Adria-F)

<img src="https://drive.google.com/uc?id=1cIlNdQMWxLPZz0VO1siO7d_wgM4mKkGb" width="100">

[Marc Garcia](https://github.com/MaxitoSama)

## Individual work:
* GameObjects (Both)
* AABBs (Marc)
* Game Components (Marc)
* Panel Assets (Adrià)
* Panel Hierarchy (Both)
* Panel Inspector (Adrià)
* Panel FileDialog (Adrià)
* QuadTree (Marc)
* Frustum Culling (Marc)
* Textures Importing (Adrià)
* FBX importing (Both)
* meshes and scenes own format (Adrià)
* Mouse picking (Both)
* Time manager (Marc)
* Resources (classes) (Marc)
* Resource Manager (Both)
* .meta files management (Adrià)
* File system (Adrià)
* JSON_Manager (Adrià)
* Guizmos (Both)

### Adrià Ferrer
Work

### Marc garcia
Work

## Main Core Sub-systems

## Skeletal Animation system

editor gif


in-game gif

## How does it work
1 minute video

If you want to take a look to our repository click [Here](https://github.com/Adria-F/Engines).

If you want to download the latest release click [here](https://github.com/Adria-F/Engines/releases/latest).


## Libraries used

* STL to use the basic containers.
* [SDL](https://www.libsdl.org/index.php) to create the window and imputs.
* [ImGui](https://github.com/ocornut/imgui) to create the UI.
* [OpenGL](https://www.opengl.org/) to create the renderer, 3D shapes, buffers and textures.
* [rapidjson](https://github.com/Tencent/rapidjson) to read and write JSON files.
* [PCG](http://www.pcg-random.org/download.html) to create random numbers.
* [MathGeolib](https://github.com/juj/MathGeoLib) to make all the math operations.
* [Assimp](http://www.assimp.org/) to import .FBX/.fbx of 3D models.
* [DevIL](http://openil.sourceforge.net/) to import the .png and .dds to create te textures.
* [mmgr](http://www.flipcode.com/archives/Presenting_A_Memory_Manager.shtml) to manage the memory used.
* [PHYSFS](https://icculus.org/physfs/) to manage files.
* [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo) Immediate mode 3D gizmo for scene editing and other controls based on Dear Imgui

## License


MIT License

Copyright (c) 2018 Adrià Ferrer and Marc Garcia.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.