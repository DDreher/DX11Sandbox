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

Explored topics:
* Setting up a bare minimum render pipeline
* Write, load and compile bare minimum shaders
* Draw a colored triangle

### [02 - Texturing](02_TexturedCube/Source/)

<p align="left">
<img src="doc/images/02_textured_cube.png?raw=true" alt="Image of Subproject: 02-TexturedCube" height="320px">
</p>

Explored topics:
* Rendering of meshes from in-memory data
* Usage of constant buffers
* Texture bindings
* Different blend states and their complications (e.g. transparency)

### [03 - Loading Models](03_Mesh/Source/)

<p align="left">
<img src="doc/images/03_model.png?raw=true" alt="Image of Subproject: 03-Mesh" height="320px">
</p>

Explored topics:
* Loading of models from files using Assimp
* Revisited quaternions to calculate mesh transforms
* Further abstractions of graphics resources (shaders, textures, buffers, materials,...)


### [04 - Shader Reflection](04_ShaderReflection/Source/)

<video src="https://user-images.githubusercontent.com/4201607/173398224-4324bcdb-3b06-40d1-afee-6ad67fac1632.mp4" controls="controls" height="320px"></video>

Explored topics:
* Setting up material parameters via shader reflection (input layouts, cbuffers, resource bindings,...)
* Exposing an API to change material parameters during runtime
* Rendering a simple UI with Imgui
## Dependencies

* [Assimp](www.assimp.org)
* [SDL2](https://www.libsdl.org/)
* [STB](https://github.com/nothings/stb)
* [Spdlog](https://github.com/gabime/spdlog)
* [Dear ImGui](https://github.com/ocornut/imgui)

## Acknowledgments

* [Chunky Knight](https://skfb.ly/6CwEJ) model by thanhtp, licensed under [Creative Commons Attribution](http://creativecommons.org/licenses/by/4.0/).

## Learning Resources

* [YouTube: ChiliTomatoNoodle](https://www.youtube.com/channel/UCsyHonfwHi4fLb2lkq0DEAA)
* [Rastertek](http://www.rastertek.com/tutdx11.html)
* [3dgep](https://www.3dgep.com/introduction-to-directx-11/)
