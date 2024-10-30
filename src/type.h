#ifndef TYPE_H
#define TYPE_H

#include <stdlib.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <math.h>

#define CANVAS_WIDTH 320//1280//320
#define CANVAS_HEIGHT 180 //720//180
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
    vector3D normal;
    f32 d;

} plane;

typedef struct {
    vector3D position, normal, light_intensity;

} vertex;

typedef struct {
    u32 vi0, vi1, vi2, color;
    vector3D normal, origin, intensity;
    bool flat;
} face;

// ENTITY DATA TYPES

typedef struct {
    vector3D *vertices, position, rotation, scale;
    face *faces;
    u32 len_vertices, len_faces;

} model;

// LIGHT DATA TYPES

typedef struct {
    vector3D direction, intensity;

} directional_light;

// ARRAYS

typedef struct {
    vertex *values;
    u32 size, used;

} vertice_array;

typedef struct {
    face *values;
    u32 size, used;

} face_array;

typedef struct {
    directional_light *values;
    u32 size, used;

} directional_light_array;

// Define scene/window/viewport properties.
struct {
    // Window properties.
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *canvas;
    u32 surface[CANVAS_WIDTH * CANVAS_HEIGHT];
    f32 depth_buffer[CANVAS_WIDTH * CANVAS_HEIGHT];
    u8 mode;
    bool backface_culling;
    bool depth_buffering;

    // Viewport properties.
    f32 viewport_width, viewport_height, viewport_distance;
    vector3D viewport_position, viewport_rotation, ambient_light;

    // Scene buffers.
    vertice_array vertex_buffer;
    face_array geo_buffer;
    vertice_array projected_buffer;
    directional_light_array directional_light_buffer;

      
} state;

#endif