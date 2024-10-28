Controls: WASD to move, LSHIFT to go up, LCTRL to go down.
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

TO-DO: Add code comments, and texture support.

KNOWN BUGS: I have a memory leak somewhere.

Compile command(with clang):    clang -o app src/main.c src/vector.c src/renderer.c src/model.c -lSDL2
                                ./app
