Controls: WASD to move, LSHIFT to go up, LCTRL to go down.
Key press:
    0, vertice mode.
    1, wireframe mode.
    2, base color mode.
    3, per-face shading.
    4, per-vertex shading.
    5, per-pixel shading.

This codebase is the result of years of studying books of the related techniques and math for a 3D software renderer.
All code provided by me. OBJ's imported from blender.

TO-DO: Add code comments, and texture support.

KNOWN BUGS: Mac randomly kills the application.

Compile command(with clang):    clang -o app src/main.c src/vector.c src/renderer.c src/model.c -lSDL2
                                ./app