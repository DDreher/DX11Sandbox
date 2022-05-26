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

The very minimum to get started with a graphics API:    
A colored triangle.

### [02 - Texturing](02_TexturedCube/Source/)

<p align="left">
<img src="doc/images/02_textured_cube.png?raw=true" alt="Image of Subproject: 02-TexturedCube" height="320px">
</p>

A rotating, textured, semi-transparent cube.    
Used to explore constant buffers, different blend states.

### [03 - Loading Models](03_Mesh/Source/)

<p align="left">
<img src="doc/images/03_model.png?raw=true" alt="Image of Subproject: 03-Mesh" height="320px">
</p>

We can now load models from files! 🎉    
Also some further abstraction and refactoring of the render pipeline (Transforms, Shaders, Materials, Bindings,...)


## Dependencies

* [Assimp](www.assimp.org)
* [SDL2](https://www.libsdl.org/)
* [STB](https://github.com/nothings/stb)
* [Spdlog](https://github.com/gabime/spdlog)

## Acknowledgments

* [Chunky Knight](https://skfb.ly/6CwEJ) model by thanhtp, licensed under [Creative Commons Attribution](http://creativecommons.org/licenses/by/4.0/).

## Learning Resources

* [YouTube: ChiliTomatoNoodle](https://www.youtube.com/channel/UCsyHonfwHi4fLb2lkq0DEAA)
* [Rastertek](http://www.rastertek.com/tutdx11.html)
* [3dgep](https://www.3dgep.com/introduction-to-directx-11/)
