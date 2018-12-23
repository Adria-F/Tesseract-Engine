# 3D Tesseract-Engine

This is a 3D Engine created by [Adrià Ferrer](https://github.com/Adria-F) and [Marc Garcia](https://github.com/MaxitoSama) students of the
 [Bachelor’s Degree in Video Games by UPC at CITM](https://www.citm.upc.edu/ing/estudis/graus-videojocs/).

If you want to take a look to our repository click [Here](https://github.com/Adria-F/Engines).

If you want to check the web page click [Here](https://adria-f.github.io/Tesseract-Engine/).

If you want to download the latest release click [here](https://github.com/Adria-F/Engines/releases/latest).

##  Controls:

* Right click to rotate the camera arround itself.
* Right click + “WASD” to move the camera.
* Alt + left click to rotate the camera arround an object.
* Mouse wheel to zoom in/out.
* Mouse wheel click to move the camera across camera local Y and X axis.
* F to focus the camera around the geometry.
* SHIFT + Movement to duplicate the movement speed.
* CTRL + Movement to reducethe movement speed.
* N to recalculate QuadTree
* M to erase QuadTree
* Q to hide gizmo
* W to show translation gizmo
* E to show rotation gizmo
* R to show scale gizmo

## Extra instructions

* Drag and drop of .FBX/.fbx/.png/.dds to import into assets
* Double click assets on Assets panel to add them to the scene
* Right click on an item of asset panel or game object in hierarchy to delete
* Drag an item from assets panel into a folder to move item
* Drag an item from assets panel into the link at the top of the panel to move to the previous folder
* Drag game objects through hierarchy to change parents
* Select a game object to see its components on inspector
* In inspector you can click the '+' symbol at the end of each component tab to see more options
* In inspector you can add empty components of type "mesh" or "texture"
* Click the sphere next to the gray box to change the assigned asset to each component

## Innovation:

* We are using a frame buffer to draw all the scene on a gl texture, which we then draw into an ImGui window.
This texture, accepts windows resizing without deforming.

* We designed our own File System using PHYSFS.

* We designed a JSON_manager to easily work with JSON files using rapidjson library.

* We have a panel Assets that show everything in Assets folder (except .meta files) were you can move or remove any asset and add/remove folders.

* Can freely change the color that a mesh is using from the panel inspector.

* Can change the resource assigned to a component from a list of all the resources of that type.

* We have a panel file dialog that allows to open a directory and navigate through its folders, (you can define an exclusive extension to search or ignore).

## Libraries

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

## Work of each member

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

## License

You can access to our license [here]().

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

