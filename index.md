# 3D Tesseract-Engine

This is a 3D Engine created as the main project of the Engines subject of the
 [Bachelor’s Degree in Video Games by UPC at CITM](https://www.citm.upc.edu/ing/estudis/graus-videojocs/).
 We are using different libraries to create our own 3D engine with some basics features and then, for the final delivery, we are a creating one "high-level" system.
 In this case, it will be skeletal animation.
 
 
## Contributors:
This engine is created by:

<p align="center"> 
<img src="https://drive.google.com/uc?id=1vQUdU2pbTyUeGBdSpkp46poJoaWvpupi" width="300">
<img src="https://drive.google.com/uc?id=1cIlNdQMWxLPZz0VO1siO7d_wgM4mKkGb" width="300">
</p>

[Adrià Ferrer](https://github.com/Adria-F)
                                          
[Marc Garcia](https://github.com/MaxitoSama)

## Members Tasks:

### Both:
* GameObjects
* Panel Hierarchy
* FBX importing
* Mouse picking
* Resource Manager
* Guizmos
* Animations importing
* Animation Component
* Animation Resource
* Game Panel

### Adrià Ferrer:
* Panel Assets
* Panel Inspector
* Panel FileDialog
* Textures Importing
* meshes and scenes own format
* .meta files management
* File system
* JSON_Manager
* Bone Components
* Skeleton animation
* Bone hierarchy and mesh linking

### Marc garcia:
* AABBs
* Game Components
* QuadTree
* Frustum Culling
* Time manager
* Resources (classes)
* Bone Resource
* Animation timeline panel
* Animation interpolations
* Skinning
* Animation Blending

## Main Core Sub-systems
* It has a hierarchy of gameObjects, each can have multiple components that define and complement them (transformation, mesh, material, animation, bone, camera).
* Can manipulate their transformation directly from the scene panel through the usage of a guizmo.
* Can freely add, remove and change parent and order of gameObjects into the hierarchy panel.
* It has a file and resource systems, to manage asset files, imort them and store them as binaries for easily usage.
* It is using a QuadTree to optimize frustum culling of static gameObjects.
* It has scene serialization of json files through rapidjson library.

## Skeletal Animation system
The importing of animations is done with Assimp, from fbx and dae files.


When creating meshes, it reads the bones that it is using and create the corresponding bone resource. It then reads all the animations of the scene and creates
the animation resources, storing the corresponding transformations over time of the bones (linked by name).


When assigning a resource to a component animation, it looks for the used bones inside the gameObjects hierarchy and save the UID of the components to later usage.


On the Update, it loops through all the bones and apply the corresponding transformation to the gameObjects, calculating interpolations if activated, as well as
animation blending when changing them.

To do the interpolation, it basically does a Lerp operation of the current frame and the next frame based on the percentage of time elapsed.
For animation blending, it temporally keeps the new resource that will be assigned and do interpolation between the frame of the current animation and the first frame
of the next animation before finally swaping the animations.

Before the render of the mesh, it calculates a morphed version of it, adapting to the linked bones and rendering it instead of the original one.


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