#ifndef TYPE_H
#define TYPE_H

#include <stdlib.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <math.h>

#define CANVAS_WIDTH 1280 //1280//320
#define CANVAS_HEIGHT 720 //720//180
#define TARGET_FPS 60

typedef float f32;
typedef double f64; 
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

#define RED 0xFF0000FF
#define GREEN 0xFF00FF00
#define BLUE 0xFFFF0000
#define CYAN 0xFFFFFF00
#define MAGENTA 0xFFFF00FF
#define YELLOW 0xFF00FFFF
#define WHITE 0xFFFFFFFF

// MESH AND POSITIONAL DATA TYPES

typedef struct {
    f32 x, y, z;
} vector3D;

typedef struct {
    f32 x, y;
} vector2D;

typedef struct {
    f32 A, B, C, D;
} plane;

typedef struct {
    vector3D position, normal, light_intensity;
} vertex;

typedef struct {
    u32 vi0, vi1, vi2, color;
    u8 flat;
    vector3D normal;
} face;

// ENTITY DATA TYPES

typedef struct {
    vector3D *vertices;
    face *faces;
    u32 len_vertices, len_faces;
    vector3D position, rotation, scale;

} model;

typedef struct {
    char *path;
    vector3D position;
    f32 scale;
    
} sprite3D;

// LIGHT DATA TYPES

typedef struct {
    vector3D direction, intensity;

} dir_light;

typedef struct {
    vector3D position;
    f32 intensity;
} omni_light;

// ENTITY ARRAYS

typedef struct {
    vertex *values;
    u32 size, used;

} vertice_array;

typedef struct {
    face *values;
    u32 size, used;

} face_array;

typedef struct {
    sprite3D *values;
    u32 size, used;

} sprite3D_array;

// LIGHT ARRAYS

typedef struct {
    dir_light *values;
    u32 size, used;

} dir_light_array;

typedef struct {
    omni_light *values;
    u32 size, used;

} omni_light_array;

// Define scene/window/viewport properties.
struct {
    // Window properties.
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *canvas;
    u32 surface[CANVAS_WIDTH * CANVAS_HEIGHT];
    f32 depth_buffer[CANVAS_WIDTH * CANVAS_HEIGHT];

    // Viewport properties.
    f32 viewport_width, viewport_height, viewport_distance;
    vector3D viewport_position, viewport_rotation;

    // Scene buffers.
    vertice_array vertex_buffer;
    face_array geo_buffer;
    sprite3D_array sprite3D_buffer;
    dir_light_array dir_light_buffer;
    omni_light_array omni_light_buffer;
    
} state;

#endif