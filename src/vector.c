#include "vector.h"

void print_vector3D(vector3D a) { //Function to print a vector's values.
    printf("(%f, %f, %f)\n", a.x, a.y, a.z);
}

vector3D add_vector3D(vector3D a, vector3D b) { //Function to add two vectors together.
    vector3D c = {a.x + b.x, a.y + b.y, a.z + b.z};
    return c;
}

vector3D subtract_vector3D(vector3D a, vector3D b) { //Function to subtract two vectors together.
    vector3D c = {a.x - b.x, a.y - b.y, a.z - b.z};
    return c;
}

vector3D multiply_by_scalar(vector3D a, f32 b) { //Function to multiply a vector by a scalar.
    vector3D c = {a.x * b, a.y * b, a.z * b};
    return c;
}

vector3D divide_by_scalar(vector3D a, f32 b) { //Function to divide a vector by a scalar.
    vector3D c = {a.x / b, a.y / b, a.z / b};
    return c;
}

vector3D rotate_vector3D(vector3D a, vector3D rot) {
    vector3D t = a;
    // Rotate on x-axis.
    t.y = cos(rot.x) * t.y - sin(rot.x) * t.z;
    t.z = sin(rot.x) * t.y + cos(rot.x) * t.z;

    // Rotate on y-axis.
    t.x = cos(rot.y) * t.x + sin(rot.y) * t.z;
    t.z = -sin(rot.y) * t.x + cos(rot.y) * t.z;

    // Rotate on z-axis.
    t.x = cos(rot.z) * t.x - sin(rot.z) * t.y;
    t.y = sin(rot.z) * t.x + cos(rot.z) * t.y;

    return t;
}

f32 get_magnitude(vector3D a) { //Function to get magnitude of a vector.
    f32 magnitude = sqrt((a.x * a.x) + (a.y * a.y) + (a.z * a.z));
    return magnitude;
}

vector3D normalize(vector3D a) { //Function to normalize a vector.
    f32 magnitude = get_magnitude(a);
    vector3D b = divide_by_scalar(a, magnitude);
    return b;
}

f32 dot_product(vector3D a, vector3D b) { //Function for getting the dot product of two vectors.
    f32 c = (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
    return c;
}

vector3D cross_product(vector3D a, vector3D b) { //Function for getting the cross product of two vectors.
    vector3D c = {(a.y * b.z) - (a.z * b.y), (a.z * b.x) - (a.x * b.z), (a.x * b.y) - (a.y * b.x)};
    return c;
}

vector3D project_to_canvas(vector3D a) { //Function for projecting a vector on to a canvas.
    vector3D b = {CANVAS_WIDTH * ((a.x * state.viewport_distance) / a.z) / state.viewport_width,
                CANVAS_HEIGHT * ((a.y * state.viewport_distance) / a.z) / state.viewport_height,
                a.z};
    return b;
}

void swap_vectors(vector3D *a, vector3D *b) {
    vector3D t = *a;
    *a = *b;
    *b = t;
}

f32 get_distance_from_plane(vector3D v0, vector3D normal, f32 plane_distance){
    f32 distance = (v0.x * normal.x) + (v0.y * normal.y) + (v0.z * normal.z) - plane_distance;
    //printf("%f\n", distance);
    return distance;
}

void transform_vertice(vector3D *a, vector3D pos, vector3D rot, vector3D scale) {
    vector3D temp;
    
    temp.x = a->x * scale.x;
    temp.y = a->y * scale.y;
    temp.z = a->z * scale.z;
    a->x = temp.x;
    a->y = temp.y;
    a->z = temp.z;

    // Rotate on x-axis.
    temp.y = cos(rot.x) * a->y - sin(rot.x) * a->z;
    temp.z = sin(rot.x) * a->y + cos(rot.x) * a->z;
    a->y = temp.y;
    a->z = temp.z;

    // Rotate on y-axis.
    temp.x = cos(rot.y) * a->x + sin(rot.y) * a->z;
    temp.z = -sin(rot.y) * a->x + cos(rot.y) * a->z;
    a->x = temp.x;
    a->z = temp.z;

    // Rotate on z-axis.
    temp.x = cos(rot.z) * a->x - sin(rot.z) * a->y;
    temp.y = sin(rot.z) * a->x + cos(rot.z) * a->y;
    a->x = temp.x;
    a->y = temp.y;

    // Transform.
    a->x += pos.x - state.viewport_position.x;
    a->y += pos.y - state.viewport_position.y;
    a->z += pos.z - state.viewport_position.z;

    // Rotate around origin.

    // Rotate on y-axis.
    temp.x = cos(state.viewport_rotation.y) * a->x + sin(state.viewport_rotation.y) * a->z;
    temp.z = -sin(state.viewport_rotation.y) * a->x + cos(state.viewport_rotation.y) * a->z;
    a->x = temp.x;
    a->z = temp.z;

    // Rotate on x-axis.
    temp.y = cos(state.viewport_rotation.x) * a->y - sin(state.viewport_rotation.x) * a->z;
    temp.z = sin(state.viewport_rotation.x) * a->y + cos(state.viewport_rotation.x) * a->z;
    a->y = temp.y;
    a->z = temp.z;

    // Rotate on z-axis.
    temp.x = cos(state.viewport_rotation.z) * a->x - sin(state.viewport_rotation.z) * a->y;
    temp.y = sin(state.viewport_rotation.z) * a->x + cos(state.viewport_rotation.z) * a->y;
    a->x = temp.x;
    a->y = temp.y;
}

vector3D rotate_against(vector3D a){
    vector3D temp = a;
    // Rotate on y-axis.
    temp.x = cos(state.viewport_rotation.y) * a.x + sin(state.viewport_rotation.y) * a.z;
    temp.z = -sin(state.viewport_rotation.y) * a.x + cos(state.viewport_rotation.y) * a.z;
    a.x = -temp.x;
    a.z = temp.z;

    // Rotate on x-axis.
    temp.y = cos(state.viewport_rotation.x) * a.y - sin(state.viewport_rotation.x) * a.z;
    temp.z = sin(state.viewport_rotation.x) * a.y + cos(state.viewport_rotation.x) * a.z;
    a.y = -temp.y;
    a.z = temp.z;

    // Rotate on z-axis.
    temp.x = cos(state.viewport_rotation.z) * a.x - sin(state.viewport_rotation.z) * a.y;
    temp.y = sin(state.viewport_rotation.z) * a.x + cos(state.viewport_rotation.z) * a.y;
    a.x = -temp.x;
    a.y = -temp.y;

    return a;
}
