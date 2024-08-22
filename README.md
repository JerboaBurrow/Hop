# Hop

#### A lightweight 2D game engine, in C++ with embedded Lua

### Features

- Free, MIT open sourced..
- Use as a standalone engine or as a drop in component (e.g. with SFML)..
- Entity component system (ECS) based.
- Physics (Discrete element method):
  - [x] collision primitives (circles, rects).
  - [x] collision meshes built from primitives (rigid and soft).
  - [x] configurable friction, gravity, drag, and restitution.
  - [x] super-sampling.
  - [ ] (good) multithreading.
  - [x] Cell list collision detection.
  - [ ] Quad tree collision detection.
  - [x] Object-object and object-world collisions.
  - [ ] collision islands and other optimisations.
- Rendering (provided by jGL, Vulkan is a WIP):
  - [x] OpenGL.
  - [ ] Vulkan.
  - [x] sprites (atlasing, and animateable atlases).
  - [x] collision primitives (circles/rects).
  - [x] Freetype fonts.
  - [ ] shapes (it is in jGL).
  - [ ] lightmaps and shadows.
  - [x] msaa.
  - [ ] particle effects (it is in jGL).
  - [x] texture management.
  - [ ] (compressed) texture asset packs.
  - [ ] Engine UI (Dear ImGui).
- Window management:
  - [x] Single window creation.
  - [ ] Multi-window.
  - [x] Key and mouse events.
  - [x] Frame limiting.
  - [ ] Snapping and resizing.
  - [x] Screenspace bbs. 
- Worlds:
  - [x] Perlin (marching squares).
  - [x] Tile maps.
  - [x] object-world collisions.
  - [ ] editable.
  - [x] saveable.
  - [ ] tile textures.
- Lua console
  - [x] ECS bindings (object management).
  - [x] Compressed script asset archive support.
  - [ ] Live in game console 
- Cross platform:
  - [x] Linux.
  - [x] Windows.
  - [x] macOS.
  - [x] Android.
  - [ ] IOS (mostly held up by jGL rendering).
- Tooling
 - [x] mesh editor.
 - [x] Lua script packer.
 - [ ] World editor.
 - [ ] Texture packer.

### Documentation

Checkout the docs [here](https://jerboaburrow.github.io/Hop/), and also the jGL docs [here](https://jerboaburrow.github.io/jGL/)

### Setup

- clone, and init the submodules (you can use ```submodules.sh```)
- the ```build.sh``` can be used to build

### OSS Dependencies and Licenses

- Freetype is licensed under the The FreeType Project LICENSE
- GLEW is licensed under aModified BSD License, the Mesa 3-D License (MIT) and the Khronos License (MIT).
- GLFW is licensed under the zlib/libpng
- GLM is licensed under the MIT License (but also, no bunnies have been made unhappy)
- Lua is licensed under the MIT license
- Miniaudio is licensed under the MIT-0 license
- stduuid is licensed under the MIT license
- vorbis and ogg is licensed under a BSD license
- zlib is licensed under the zlib license

Thanks to all the OSS developers: David Turner, Robert Wilhelm, and Werner Lemberg (Freetype), Milan Ikits <milan ikits[]ieee org>, Marcelo E. Magallon <mmagallo[]debian org>, and Lev Povalahev Brian Paul, The Khronos Group Inc (GLEW), Marcus Geelnard and Camilla Löwy (GLFW), G-Truc Creation (GLM), Lua.org, PUC-Rio (Lua), David Reid (Miniaudio), Marius Bancila https://github.com/mariusbancila/stduuid#MIT-1-ov-file (stduuid), Xiph.org Foundation (vorbis, ogg), and Jean-loup Gailly and Mark Adler (zlib).
