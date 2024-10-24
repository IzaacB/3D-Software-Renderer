#ifndef RENDERER_H
#define RENDERER_H

#include "type.h"
#include "model.h"
#include "type.h"

extern void clear_screen();
extern void clear_depth();
extern void update_screen();
extern void quit();
extern void render_scene();
extern void draw_sprite(vector2D position, char *path, f32 scale);
extern void render_sprite3D(sprite3D sprite);

#endif