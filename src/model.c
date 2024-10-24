#include "model.h"

// Array functions
void initialize_vertice_array(vertice_array *array, u32 initial_size) { //Initialize a dynamic vertice array with some amount of values.
    array->values = (vertex *)malloc(initial_size * sizeof(vertex));
    array->size = initial_size;
    array->used = 0;
}

void insert_vertice(vertice_array *array, vertex vertice){
    if (array->used == array->size) { //Check if array is full, if so realloc.
        array->size *= 2;
        array->values = (vertex *)realloc(array->values, array->size * sizeof(vertex));
    }
    array->values[array->used++] = vertice;//Add vertice at next available index.
}

void clear_vertice_array(vertice_array *array) { //Clear the contents in a vertice array, and reset values.
    free(array->values);
    array->values = NULL;
    array->size = 0;
    array->used = 0;
}

void copy_vertice_array(vertice_array *dest, vertice_array *src){
    dest->size = src->size;
    dest->used = src->used;

    dest->values = (vertex *)malloc(dest->size * sizeof(vertex));
    for (u32 i = 0; i < src->used; i++){
        dest->values[i] = src->values[i];
    }
}

void swap_vertices(vertex *a, vertex *b) {
    vertex t = *a;
    *a = *b;
    *b = t;
}

void initialize_face_array(face_array *array, u32 initial_size) { //Initialize a dynamic vertice array with some amount of values.
    array->values = (face *)malloc(initial_size * sizeof(face));
    array->size = initial_size;
    array->used = 0;
}

void insert_face(face_array *array, face tri){
    if (array->used == array->size) { //Check if array is full, if so realloc.
        array->size *= 2;
        array->values = (face *)realloc(array->values, array->size * sizeof(face));
    }
    array->values[array->used++] = tri;//Add vertice at next available index.
}

void clear_face_array(face_array *array) { //Clear the contents in a vertice array, and reset values.
    free(array->values);
    array->values = NULL;
    array->size = 0;
    array->used = 0;
}

void copy_face_array(face_array *dest, face_array *src){
    dest->size = src->size;
    dest->used = src->used;

    dest->values = (face *)malloc(dest->size * sizeof(face));
    for (u32 i = 0; i < src->used; i++){
        dest->values[i] = src->values[i];
    }
}

void initialize_dir_light_array(dir_light_array *array, u32 initial_size) { //Initialize a dynamic vertice array with some amount of values.
    array->values = (dir_light *)malloc(initial_size * sizeof(dir_light));
    array->size = initial_size;
    array->used = 0;
}

void insert_dir_light(dir_light_array *array, dir_light light){
    if (array->used == array->size) { //Check if array is full, if so realloc.
        array->size *= 2;
        array->values = (dir_light *)realloc(array->values, array->size * sizeof(dir_light));
    }
    array->values[array->used++] = light;//Add vertice at next available index.
}

void clear_dir_light_array(dir_light_array *array) { //Clear the contents in a vertice array, and reset values.
    free(array->values);
    array->values = NULL;
    array->size = 0;
    array->used = 0;
}

void copy_dir_light_array(dir_light_array *dest, dir_light_array *src){
    dest->size = src->size;
    dest->used = src->used;

    dest->values = (dir_light *)malloc(dest->size * sizeof(dir_light));
    for (u32 i = 0; i < src->used; i++){
        dest->values[i] = src->values[i];
    }
}

void add_model_to_scene(model model) { //Add model to global buffers.
    u32 offset = state.vertex_buffer.used;
    
    for (int i = 0; i < model.len_vertices; i++){
        vector3D transformed = model.vertices[i];
        transform_vertice(&transformed, model.position, model.rotation, model.scale);
        vertex vertice = {transformed, {0, 0, 0}, {0, 0, 0}};
        insert_vertice(&state.vertex_buffer, vertice);
    }
    
    for (int i = 0; i < model.len_faces; i++){
        face n = {model.faces[i].vi0 + offset, model.faces[i].vi1 + offset, model.faces[i].vi2 + offset, model.faces[i].color};
        insert_face(&state.geo_buffer, n);
    }
}

void add_obj_to_scene(char *path, vector3D transform, vector3D rotation, vector3D scale,  u32 color){
    u32 offset = state.vertex_buffer.used;
    vector3D zero = {0, 0, 0};
    FILE *file = fopen(path, "r");
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        //printf("%s", line);
        if (line[0] == 'v' && line[1] == ' ') {
            f32 x, y, z;
            sscanf(line + 2, "%f %f %f", &x, &y, &z);
            vector3D vertice = {x, y, z}; transform_vertice(&vertice, transform, rotation, scale);
            vertex v = {vertice, {0, 0, 0}, {0, 0, 0}};
            insert_vertice(&state.vertex_buffer, v);
        }

        else if (line[0] == 'f' && line[1] == ' ') {
            u32 v0, v1, v2;
            sscanf(line + 2, "%d %d %d", &v0, &v1, &v2);
            face face = {v0 - 1 + offset, v1 - 1 + offset, v2 - 1 + offset, color};
            insert_face(&state.geo_buffer, face);
        }

    }

    fclose(file);
}

/*MODEL DATA: 
note: to keep original model data static, we will not be using dynamic arrays.*/

