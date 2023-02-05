# DirectX 11 Sandbox

A small collection of DirectX 11 projects I built while learning the API.

## Disclaimer

> **"A person who never made a mistake never tried anything new."**
\-- *Albert Einstein* (or maybe Theodore Roosevelt, Matt Bellamy or even your cat. [It's not so clear](https://quoteinvestigator.com/2014/12/16/no-mistakes/).)

The code inside this repository was only written for learning purposes.
I never really tried to make it beautiful, easy to read, performant or 100% correct.
In the end, all I wanted to do is to explore the API.
Now I share my code so other people doing the same may be inspired by my journey.

Tl;dr: I know the code's dirty ;) Don't judge me.

## How to build (Win64)

1. Install a `Windows SDK` if you haven't already (OS: Windows 8 or higher).
2. Run `GenerateProjectFiles.bat`.
3. Open the generated `DX11Sandbox.sln` with VS2022.
5. Build and run the desired project the desired configuration (Debug / DebugRender / Release)

## Subprojects

### [01 - Hello Triangle](01_HelloTriangle/Source/)

<p align="left">
<img src="doc/images/01_hello_triangle.png?raw=true" alt="Image of Subproject: 01-HelloTriangle" height="320px">
</p>

**Explored topics:**
* Setting up a bare minimum render pipeline
* Write, load and compile bare minimum shaders
* Draw a colored triangle

### [02 - Texturing](02_TexturedCube/Source/)

<p align="left">
<img src="doc/images/02_textured_cube.png?raw=true" alt="Image of Subproject: 02-TexturedCube" height="320px">
</p>

**Explored topics:**
* Rendering of meshes from in-memory data
* Usage of constant buffers
* Texture bindings
* Different blend states and their complications (e.g. transparency)

### [03 - Loading Models](03_Mesh/Source/)

<p align="left">
<img src="doc/images/03_mesh.png?raw=true" alt="Image of Subproject: 03-Mesh" height="320px">
</p>

**Explored topics:**
* Loading of meshes from files using Assimp
* Revisited quaternions to calculate mesh transforms
* Further abstractions of graphics resources (shaders, textures, buffers, materials,...)

### [04 - Shader Reflection](04_ShaderReflection/Source/)

<video src="https://user-images.githubusercontent.com/4201607/173398224-4324bcdb-3b06-40d1-afee-6ad67fac1632.mp4" controls="controls" height="320px"></video>

**Explored topics:**
* Setting up material parameters via shader reflection (input layouts, cbuffers, resource bindings,...)
* Exposing an API to change material parameters during runtime
* Rendering a simple UI with Imgui

### [05 - GLTF Models](05_Models/Source/)

<p align="left">
<img src="doc/images/05_models.png?raw=true" alt="Image of Subproject: 05-Models" height="320px">
</p>

**Explored topics:**
* Import of glTF models using Assimp (using Sponza as showcase)
* Building a scene graph
* Using resource pools with handles instead of smart pointers
* Introduction of render queues - one for opaque materials, one for transparent materials
* Introduction of render work items as foundation for draw call batching
* Building shader permutations with macros (e.g. for alpha cutoff depending on material configuration)
* Movable camera (FPS style, WASD to move, hold right mouse button to change camera direction)

### [06 - Blinn-Phong](06_Lighting/Source/)

<p align="left">
<img src="doc/images/06_phong.png?raw=true" alt="Image of Subproject: 06-Lighting" height="320px">
</p>

**Explored topics:**
* (Blinn-)Phong reflection model
* Directional lights
* Spot lights
* Point lights

### [07 - Normal Mapping](07_NormalMapping/Source/)

<p align="left">
<img src="doc/images/07_normal_mapping.png?raw=true" alt="Image of Subproject: 07-NormalMapping" height="320px">
</p>

**Explored topics:**
* Normal Mapping
* TBN Matrices
* Fixing tangents on models with mirrored UVs
* Automatically generating mip maps during run-time

## Dependencies

* [Assimp](www.assimp.org)
* [SDL2](https://www.libsdl.org/)
* [STB](https://github.com/nothings/stb)
* [Spdlog](https://github.com/gabime/spdlog)
* [Dear ImGui](https://github.com/ocornut/imgui)

## Acknowledgments

* [Chunky Knight](https://skfb.ly/6CwEJ) mesh by thanhtp, licensed under [Creative Commons Attribution](http://creativecommons.org/licenses/by/4.0/).
* [Sponza](https://www.cryengine.com/marketplace/product/crytek/sponza-sample-scene) model by Crytek, glTF taken from [KhronosGroup glTF sample models repository](https://github.com/KhronosGroup/glTF-Sample-Models).

## Learning Resources

* [Real-time Rendering 4th edition](https://www.realtimerendering.com/)
* [LearnOpenGL](https://learnopengl.com/)
* [YouTube: ChiliTomatoNoodle](https://www.youtube.com/channel/UCsyHonfwHi4fLb2lkq0DEAA)
* [Rastertek](http://www.rastertek.com/tutdx11.html)
* [3dgep](https://www.3dgep.com/introduction-to-directx-11/)