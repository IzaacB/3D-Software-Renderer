#ifndef VECTOR_H
#define VECTOR_H

#include "type.h"

extern void print_vector3D(vector3D a);
extern vector3D add_vector3D(vector3D a, vector3D b);
extern vector3D subtract_vector3D(vector3D a, vector3D b);
extern vector3D multiply_by_scalar(vector3D a, f32 b);
extern vector3D divide_by_scalar(vector3D a, f32 b);
extern vector3D rotate_vector3D(vector3D a, vector3D rot);
extern f32 get_magnitude(vector3D a);
extern vector3D normalize(vector3D a);
extern f32 dot_product(vector3D a, vector3D b);
extern vector3D cross_product(vector3D a, vector3D b);
extern vector3D project_to_canvas(vector3D a);
extern void swap_vectors(vector3D *a, vector3D *b);
extern void transform_vertice(vector3D *a, vector3D pos, vector3D rot, vector3D scale);
extern vector3D rotate_against(vector3D a);
extern f32 get_distance_from_plane(vector3D v0, vector3D normal, f32 plane_distance);

#endif