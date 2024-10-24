#ifndef MODEL_H
#define MODEL_H

#include "type.h"
#include "vector.h"

extern void initialize_vertice_array(vertice_array *array, u32 initial_size);
extern void insert_vertice(vertice_array *array, vertex vertice);
extern void clear_vertice_array(vertice_array * array);
extern void copy_vertice_array(vertice_array *dest, vertice_array *src);
extern void swap_vertices(vertex *a, vertex *b);
extern void add_model_to_scene(model model);
extern void initialize_face_array(face_array *array, u32 initial_size);
extern void insert_face(face_array *array, face tri);
extern void clear_face_array(face_array *array);
extern void copy_face_array(face_array *dest, face_array *src);
extern void add_obj_to_scene(char *path, vector3D transform, vector3D rotation, vector3D scale, u32 color);
extern void initialize_dir_light_array(dir_light_array *array, u32 initial_size);
extern void insert_dir_light(dir_light_array *array, dir_light light);
extern void clear_dir_light_array(dir_light_array *array);
extern void copy_dir_light_array(dir_light_array *dest, dir_light_array *src);
void copy_dir_light_array(dir_light_array *dest, dir_light_array *src);

#endif