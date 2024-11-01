Controls: WASD to move, SPACE to go up, C to go down.
Key press:
    0, vertice mode.
    1, wireframe mode.
    2, base color mode.
    3, per-face shading.
    4, per-vertex shading.
    5, per-pixel shading.
    z, enable/disable depth-buffering.
    b, enable/disable backface culling.

This codebase is the result of years of studying books of the related techniques and math for a 3D software renderer.
All code provided by me. OBJ's imported from blender.

*NOTE* You must have the SDL2 framework installed. Latest release here: https://github.com/libsdl-org/SDL/releases/tag/release-2.30.8

TO-DO: Add code comments, and texture support.

Compile command(with clang):    clang -o app src/main.c src/vector.c src/renderer.c src/model.c -lSDL2
                                ./app
