#include "renderer.h"

void clear_screen() { //Clear the pixel array that draws to the window
    memset(state.surface, 0, sizeof(state.surface));
}

void update_screen() { //Push pixel array to canvas and then present to renderer.
    SDL_UpdateTexture(state.canvas, NULL, state.surface, CANVAS_WIDTH * 4);
    SDL_RenderCopyEx(state.renderer, state.canvas, NULL, NULL, 0.0, NULL, 0);
    SDL_RenderPresent(state.renderer);
}

void clear_depth(){
    for (int i = 0; i < CANVAS_WIDTH * CANVAS_HEIGHT; i++){
        state.depth_buffer[i] = -1;
    }
}

static void add_depth(int x, int y, f32 z){
    int adj_x = (int)(CANVAS_WIDTH/2 + x);
    int adj_y = (int)(CANVAS_HEIGHT/2 - y);

    if (adj_y >= 0 && adj_y < CANVAS_HEIGHT){
        if (adj_x >= 0 && adj_x < CANVAS_WIDTH){
            state.depth_buffer[adj_y * CANVAS_WIDTH + adj_x] = z;
        }
    }
}

static f32 check_depth(int x, int y){
    int adj_x = (int)(CANVAS_WIDTH/2 + x);
    int adj_y = (int)(CANVAS_HEIGHT/2 - y);

    if (adj_y >= 0 && adj_y < CANVAS_HEIGHT){
        if (adj_x >= 0 && adj_x < CANVAS_WIDTH){
            return state.depth_buffer[adj_y * CANVAS_WIDTH + adj_x];
        }
    }
    return -1;
}

void quit() { //Destroy SDL objects.
    SDL_DestroyTexture(state.canvas);
    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.window);
}

f32* add_array(f32 *a0, f32 *a1, u32 len0, u32 len1){
    f32* na = (f32*)malloc((len0 + len1) * sizeof(f32));
    memcpy(na, a0, len0 * sizeof(f32));
    memcpy(na + len0, a1, len1 * sizeof(f32));

    return na;
}

static void draw_pixel(f32 x, f32 y, u32 color) { //Change color of pixel at surface index.
    int adj_x = (i16)(CANVAS_WIDTH/2 + x);//Anchor to center of window.
    int adj_y = (i16)(CANVAS_HEIGHT/2 - y);

    if (adj_y >= 0 && adj_y < CANVAS_HEIGHT){
        if (adj_x >= 0 && adj_x < CANVAS_WIDTH){
            state.surface[adj_y * CANVAS_WIDTH + adj_x] = color;
        }
    }
}

static f32* interpolate(f32 i0, f32 d0, f32 i1, f32 d1) {
    u32 length = (u32)ceil(fabsf(i1 - i0)) + 1;
    f32 *values = (f32 *)malloc(length * sizeof(f32));

    f32 a = 0;
    if (i1 != i0) {
        a = (f32)(d1 - d0) / (f32)(i1 - i0);
    }

    for (int i = 0; i < length; i++) {
        values[i] = d0 + a * i;
    }

    return values;
}

static void draw_line(vector3D a, vector3D b, u32 color){
    if (fabsf(b.y - a.y) < fabsf(b.x - a.x)){
        if (b.x < a.x){
            swap_vectors(&a, &b);
        }
        f32 *values = interpolate(a.x, a.y, b.x, b.y);
        for (int x = (int)a.x; x < (int)b.x; x++){
            draw_pixel(x, values[(u32)(x - a.x)], color);
        }
    } else {
        if (b.y < a.y){
            swap_vectors(&a, &b);
        }
        f32 *values = interpolate(a.y, a.x, b.y, b.x);
        for (int y = (int)a.y; y < (int)b.y; y++){
            draw_pixel(values[(u32)(y - a.y)], y, color);
        }
    }
}

static void initialize_face_normals(){
    vector3D zero = {0, 0, 0};
    for (u32 i = 0; i < state.geo_buffer.used; i++){
        state.geo_buffer.values[i].normal = zero;
    }
}

static void initialize_face_lighting(){
    for (u32 i = 0; i < state.geo_buffer.used; i++){
        state.geo_buffer.values[i].intensity = state.ambient_light;
    }
}

static void normalize_face_normals(){
    for (i32 i = 0; i < state.geo_buffer.used; i++){
        state.geo_buffer.values[i].normal = normalize(state.geo_buffer.values[i].normal);
    }
}

static void initialize_vertex_normals(){
    vector3D zero = {0, 0, 0};
    for (u32 i = 0; i < state.vertex_buffer.used; i++){
        state.vertex_buffer.values[i].normal = zero;
    }
}

static void initialize_vertex_lighting(){
    for (u32 i = 0; i < state.vertex_buffer.used; i++){
        state.vertex_buffer.values[i].light_intensity= state.ambient_light;
    }
}

static void normalize_vertex_normals(){
    for (u32 i = 0; i < state.vertex_buffer.used; i++){
        state.vertex_buffer.values[i].normal = normalize(state.vertex_buffer.values[i].normal);
    }
}

static void assign_face_normals(){
    initialize_face_normals();
    for (i32 i = 0; i < state.geo_buffer.used; i++){
        vector3D v0, v1, v2;
        v0 = state.vertex_buffer.values[state.geo_buffer.values[i].vi0].position;
        v1 = state.vertex_buffer.values[state.geo_buffer.values[i].vi1].position;
        v2 = state.vertex_buffer.values[state.geo_buffer.values[i].vi2].position;

        vector3D face_origin = {(v0.x + v1.x + v2.x)/3, (v0.y + v1.y + v2.y)/3, (v0.z + v1.z + v2.z)/3};
        state.geo_buffer.values[i].origin = face_origin;

        // Get edges of face
        vector3D v01 = subtract_vector3D(v1, v0);
        vector3D v02 = subtract_vector3D(v2, v0);

        // Get face normal
        vector3D normal = normalize(cross_product(v01, v02));
        state.geo_buffer.values[i].normal = normal;

    }
    normalize_face_normals();
}

static void assign_vertex_normals(){
    initialize_vertex_normals();
    for (i32 i = 0; i < state.geo_buffer.used; i++){
        vector3D face_normal = state.geo_buffer.values[i].normal;

        state.vertex_buffer.values[state.geo_buffer.values[i].vi0].normal = add_vector3D(state.vertex_buffer.values[state.geo_buffer.values[i].vi0].normal, face_normal);
        state.vertex_buffer.values[state.geo_buffer.values[i].vi1].normal = add_vector3D(state.vertex_buffer.values[state.geo_buffer.values[i].vi1].normal, face_normal);
        state.vertex_buffer.values[state.geo_buffer.values[i].vi2].normal = add_vector3D(state.vertex_buffer.values[state.geo_buffer.values[i].vi2].normal, face_normal);
    }
    normalize_vertex_normals();
}

static void cull_backfaces(){
    // Initialize cull buffers
    face_array culled_faces; initialize_face_array(&culled_faces, 1);

    for (i32 i = 0; i < state.geo_buffer.used; i++){
        vector3D normal = state.geo_buffer.values[i].normal;

        // Get face origin.
        vector3D face_origin = state.geo_buffer.values[i].origin;

        //Get angle.
        f32 dot = dot_product(normal, face_origin);
        f32 angle = acos(dot/(get_magnitude(normal) * get_magnitude(face_origin))) * 180/3.14;
        //printf("%f\n", angle);
        if (angle >= 90){
            insert_face(&culled_faces, state.geo_buffer.values[i]);
        }
    }
    copy_face_array(&state.geo_buffer, &culled_faces);
}

static void light_scene_faces(){
    initialize_face_lighting();
    for (u32 i = 0; i < state.geo_buffer.used; i++){
        u32 base_color = state.geo_buffer.values[i].color;
        u8 r = (base_color >> 16) & 0xFF;
        u8 g = (base_color >> 8) & 0xFF;
        u8 b = base_color & 0xFF;

        vector3D face_intensity = {r/255, g/255, b/255};
        
        for (u32 j = 0; j < state.directional_light_buffer.used; j++){
            vector3D direction, intensity;
            direction = normalize(rotate_against(state.directional_light_buffer.values[j].direction));
            intensity = state.directional_light_buffer.values[j].intensity;

            f32 dot = fmax(0, dot_product(direction, state.geo_buffer.values[i].normal));
            vector3D light_intensity = multiply_by_scalar(intensity, dot);

            state.geo_buffer.values[i].intensity.z += face_intensity.x * light_intensity.x;
            state.geo_buffer.values[i].intensity.y += face_intensity.y * light_intensity.y;
            state.geo_buffer.values[i].intensity.x += face_intensity.z * light_intensity.z;
        }

        r = fmin(state.geo_buffer.values[i].intensity.x * 255, 255);
        g = fmin(state.geo_buffer.values[i].intensity.y * 255, 255);
        b = fmin(state.geo_buffer.values[i].intensity.z * 255, 255);

        state.geo_buffer.values[i].color = (r << 16) | (g << 8) | b;
    }
}

static void light_scene_vertices(){
    initialize_vertex_lighting();
    for (u32 i = 0; i < state.geo_buffer.used; i++){
        u32 base_color = state.geo_buffer.values[i].color;
        u8 r = (base_color >> 16) & 0xFF;
        u8 g = (base_color >> 8) & 0xFF;
        u8 b = base_color & 0xFF;

        vector3D face_intensity = {r/255, g/255, b/255};
        
        for (u32 j = 0; j < state.directional_light_buffer.used; j++){
            vector3D direction, intensity;
            direction = normalize(rotate_against(state.directional_light_buffer.values[j].direction));
            intensity = state.directional_light_buffer.values[j].intensity;

            f32 dot = fmax(0, dot_product(direction, state.vertex_buffer.values[state.geo_buffer.values[i].vi0].normal));
            vector3D light_intensity = multiply_by_scalar(intensity, dot);

            state.vertex_buffer.values[state.geo_buffer.values[i].vi0].light_intensity.z += (face_intensity.x * light_intensity.x)/8;
            state.vertex_buffer.values[state.geo_buffer.values[i].vi0].light_intensity.y += (face_intensity.y * light_intensity.y)/8;
            state.vertex_buffer.values[state.geo_buffer.values[i].vi0].light_intensity.x += (face_intensity.z * light_intensity.z)/8;

            dot = fmax(0, dot_product(direction, state.vertex_buffer.values[state.geo_buffer.values[i].vi1].normal));
            light_intensity = multiply_by_scalar(intensity, dot);

            state.vertex_buffer.values[state.geo_buffer.values[i].vi1].light_intensity.z += (face_intensity.x * light_intensity.x)/8;
            state.vertex_buffer.values[state.geo_buffer.values[i].vi1].light_intensity.y += (face_intensity.y * light_intensity.y)/8;
            state.vertex_buffer.values[state.geo_buffer.values[i].vi1].light_intensity.x += (face_intensity.z * light_intensity.z)/8;

            dot = fmax(0, dot_product(direction, state.vertex_buffer.values[state.geo_buffer.values[i].vi2].normal));
            light_intensity = multiply_by_scalar(intensity, dot);

            state.vertex_buffer.values[state.geo_buffer.values[i].vi2].light_intensity.z += (face_intensity.x * light_intensity.x)/8;
            state.vertex_buffer.values[state.geo_buffer.values[i].vi2].light_intensity.y += (face_intensity.y * light_intensity.y)/8;
            state.vertex_buffer.values[state.geo_buffer.values[i].vi2].light_intensity.x += (face_intensity.z * light_intensity.z)/8;
        }
        //print_vector3D(state.vertex_buffer.values[state.geo_buffer.values[i].vi0].light_intensity);
    }
}

static u32 calculate_lighting(vector3D normal, u32 base_color){
    u8 r = (base_color >> 16) & 0xFF;
    u8 g = (base_color >> 8) & 0xFF;
    u8 b = base_color & 0xFF;

    vector3D face_intensity = {r/255, g/255, b/255};
    vector3D new_intensity = {0, 0, 0};

    for (u32 j = 0; j < state.directional_light_buffer.used; j++){
            vector3D direction, intensity;
            direction = normalize(rotate_against(state.directional_light_buffer.values[j].direction));
            intensity = state.directional_light_buffer.values[j].intensity;

            f32 dot = fmax(0, dot_product(direction, normal));
            vector3D light_intensity = multiply_by_scalar(intensity, dot);

            new_intensity.z += (face_intensity.x * light_intensity.x);
            new_intensity.y += (face_intensity.y * light_intensity.y);
            new_intensity.x += (face_intensity.z * light_intensity.z);
        }

    u32 new_color = ((u8)fmin((new_intensity.x * 255 + state.ambient_light.x * 255), 255) << 16) | ((u8)fmin((new_intensity.y * 255 + state.ambient_light.y * 255), 255) << 8) | (u8)fmin((new_intensity.z * 255 + state.ambient_light.z * 255), 255);
    return new_color;
}

static void clip_against_plane(plane clipping_plane){
    vertice_array clipped_vertices; initialize_vertice_array(&clipped_vertices, 1);
    face_array clipped_faces; initialize_face_array(&clipped_faces, 1);

    for (u32 i = 0; i < state.geo_buffer.used; i++){
        vector3D v0 = state.vertex_buffer.values[state.geo_buffer.values[i].vi0].position;
        vector3D v1 = state.vertex_buffer.values[state.geo_buffer.values[i].vi1].position;
        vector3D v2 = state.vertex_buffer.values[state.geo_buffer.values[i].vi2].position;

        vector3D n0 = state.vertex_buffer.values[state.geo_buffer.values[i].vi0].normal;
        vector3D n1 = state.vertex_buffer.values[state.geo_buffer.values[i].vi1].normal;
        vector3D n2 = state.vertex_buffer.values[state.geo_buffer.values[i].vi2].normal;

        vector3D li0 = state.vertex_buffer.values[state.geo_buffer.values[i].vi0].light_intensity;
        vector3D li1 = state.vertex_buffer.values[state.geo_buffer.values[i].vi1].light_intensity;
        vector3D li2 = state.vertex_buffer.values[state.geo_buffer.values[i].vi2].light_intensity;

        f32 d0 = dot_product(clipping_plane.normal, v0) + clipping_plane.d;
        f32 d1 = dot_product(clipping_plane.normal, v1) + clipping_plane.d;
        f32 d2 = dot_product(clipping_plane.normal, v2) + clipping_plane.d;

        bool v0_in = d0 >= 0;
        bool v1_in = d1 >= 0;
        bool v2_in = d2 >= 0;

        i32 new_vertex_indices[4];
        i8 num_clipped_vertices = 0;

        // If all vertices are inside the plane, copy them as they are
        if (v0_in && v1_in && v2_in) {
            new_vertex_indices[0] = clipped_vertices.used;
            vertex vertice0 = {v0, n0, li0};
            insert_vertice(&clipped_vertices, vertice0);

            new_vertex_indices[1] = clipped_vertices.used;
            vertex vertice1 = {v1, n1, li1};
            insert_vertice(&clipped_vertices, vertice1);

            new_vertex_indices[2] = clipped_vertices.used;
            vertex vertice2 = {v2, n2, li2};
            insert_vertice(&clipped_vertices, vertice2);

            face new_face = { new_vertex_indices[0], new_vertex_indices[1], new_vertex_indices[2], state.geo_buffer.values[i].color, state.geo_buffer.values[i].normal, state.geo_buffer.values[i].origin, state.geo_buffer.values[i].intensity};
            insert_face(&clipped_faces, new_face);
            continue;
        }

        // If all vertices are outside the plane, discard the face
        if (!v0_in && !v1_in && !v2_in) {
            continue;
        }

        // Handle vertices that are inside or crossing the plane
        if (v0_in) {
            new_vertex_indices[num_clipped_vertices++] = clipped_vertices.used;
            vertex vertice = {v0, n0, li0};  // Preserve normal and light intensity
            insert_vertice(&clipped_vertices, vertice);
        }

        // Interpolate between v0 and v1 if they are on opposite sides of the plane
        if (v0_in != v1_in) {
            float t = d0 / (d0 - d1);
            vector3D intersection = {
                v0.x + t * (v1.x - v0.x),
                v0.y + t * (v1.y - v0.y),
                v0.z + t * (v1.z - v0.z)
            };
            vector3D interpolated_normal = {
                n0.x + t * (n1.x - n0.x),
                n0.y + t * (n1.y - n0.y),
                n0.z + t * (n1.z - n0.z)
            };
            vector3D interpolated_light_intensity = {
                li0.x + t * (li1.x - li0.x),
                li0.y + t * (li1.y - li0.y),
                li0.z + t * (li1.z - li0.z)
            };

            new_vertex_indices[num_clipped_vertices++] = clipped_vertices.used;
            vertex vertice = {intersection, interpolated_normal, interpolated_light_intensity};
            insert_vertice(&clipped_vertices, vertice);
        }

        if (v1_in) {
            new_vertex_indices[num_clipped_vertices++] = clipped_vertices.used;
            vertex vertice = {v1, n1, li1};
            insert_vertice(&clipped_vertices, vertice);
        }

        // Interpolate between v1 and v2 if they are on opposite sides of the plane
        if (v1_in != v2_in) {
            float t = d1 / (d1 - d2);
            vector3D intersection = {
                v1.x + t * (v2.x - v1.x),
                v1.y + t * (v2.y - v1.y),
                v1.z + t * (v2.z - v1.z)
            };
            vector3D interpolated_normal = {
                n1.x + t * (n2.x - n1.x),
                n1.y + t * (n2.y - n1.y),
                n1.z + t * (n2.z - n1.z)
            };
            vector3D interpolated_light_intensity = {
                li1.x + t * (li2.x - li1.x),
                li1.y + t * (li2.y - li1.y),
                li1.z + t * (li2.z - li1.z)
            };

            new_vertex_indices[num_clipped_vertices++] = clipped_vertices.used;
            vertex vertice = {intersection, interpolated_normal, interpolated_light_intensity};
            insert_vertice(&clipped_vertices, vertice);
        }

        if (v2_in) {
            new_vertex_indices[num_clipped_vertices++] = clipped_vertices.used;
            vertex vertice = {v2, n2, li2};
            insert_vertice(&clipped_vertices, vertice);
        }

        // Interpolate between v2 and v0 if they are on opposite sides of the plane
        if (v2_in != v0_in) {
            float t = d2 / (d2 - d0);
            vector3D intersection = {
                v2.x + t * (v0.x - v2.x),
                v2.y + t * (v0.y - v2.y),
                v2.z + t * (v0.z - v2.z)
            };
            vector3D interpolated_normal = {
                n2.x + t * (n0.x - n2.x),
                n2.y + t * (n0.y - n2.y),
                n2.z + t * (n0.z - n2.z)
            };
            vector3D interpolated_light_intensity = {
                li2.x + t * (li0.x - li2.x),
                li2.y + t * (li0.y - li2.y),
                li2.z + t * (li0.z - li2.z)
            };

            new_vertex_indices[num_clipped_vertices++] = clipped_vertices.used;
            vertex vertice = {intersection, interpolated_normal, interpolated_light_intensity};
            insert_vertice(&clipped_vertices, vertice);
        }

        // Create new face(s) from the clipped vertices
        if (num_clipped_vertices == 3) {
            face new_face = {new_vertex_indices[0], new_vertex_indices[1], new_vertex_indices[2], state.geo_buffer.values[i].color, state.geo_buffer.values[i].normal, state.geo_buffer.values[i].origin, state.geo_buffer.values[i].intensity};
            insert_face(&clipped_faces, new_face);
        } else if (num_clipped_vertices == 4) {
            // Split quad into two triangles
            face tri1 = {new_vertex_indices[0], new_vertex_indices[1], new_vertex_indices[2], state.geo_buffer.values[i].color, state.geo_buffer.values[i].normal, state.geo_buffer.values[i].origin, state.geo_buffer.values[i].intensity};
            face tri2 = {new_vertex_indices[0], new_vertex_indices[2], new_vertex_indices[3], state.geo_buffer.values[i].color, state.geo_buffer.values[i].normal, state.geo_buffer.values[i].origin, state.geo_buffer.values[i].intensity};
            insert_face(&clipped_faces, tri1);
            insert_face(&clipped_faces, tri2);
        }
    }

    copy_face_array(&state.geo_buffer, &clipped_faces);
    clear_face_array(&clipped_faces);

    copy_vertice_array(&state.vertex_buffer, &clipped_vertices);
    clear_vertice_array(&clipped_vertices);
}

static void clip_scene(){
    plane near = {0, 0, 1, -state.viewport_distance};
    plane right = {-state.viewport_distance, 0, state.viewport_width/2, 0};
    plane left = {state.viewport_distance, 0, state.viewport_width/2, 0};
    plane top = {0, -state.viewport_distance, state.viewport_height/2, 0};
    plane bottom = {0, state.viewport_distance, state.viewport_height/2, 0};

    clip_against_plane(near);
    clip_against_plane(right);
    clip_against_plane(left);
    clip_against_plane(top);
    clip_against_plane(bottom);
}

static void project_vertices(){
    // Initialize global buffer.
    initialize_vertice_array(&state.projected_buffer, state.vertex_buffer.used);

    for (int i = 0; i < state.vertex_buffer.used; i++) {
        // Project each vertice's position, and send to buffer.
        vector3D p = project_to_canvas(state.vertex_buffer.values[i].position);
        vertex vertice = {p, state.vertex_buffer.values[i].normal, state.vertex_buffer.values[i].light_intensity};
        //print_vector3D(vertice.light_intensity);
        insert_vertice(&state.projected_buffer, vertice);
    }
}

static void draw_vertices(){
    for (u32 i = 0; i < state.projected_buffer.used; i++){
        draw_pixel(state.projected_buffer.values[i].position.x, state.projected_buffer.values[i].position.y, WHITE);
    }
}

static void draw_wireframe(){
    for (u32 i = 0; i < state.geo_buffer.used; i++){
        vector3D v0 = state.projected_buffer.values[state.geo_buffer.values[i].vi0].position;
        vector3D v1 = state.projected_buffer.values[state.geo_buffer.values[i].vi1].position;
        vector3D v2 = state.projected_buffer.values[state.geo_buffer.values[i].vi2].position;
        u32 color = state.geo_buffer.values[i].color;

        draw_line(v0, v1, WHITE);
        draw_line(v1, v2, WHITE);
        draw_line(v0, v2, WHITE);
    }
}

static void draw_filled(){
    for (u32 i = 0; i < state.geo_buffer.used; i++){
        if (state.mode == 2 || state.mode == 3){
            vertex a = state.projected_buffer.values[state.geo_buffer.values[i].vi0];
            vertex b = state.projected_buffer.values[state.geo_buffer.values[i].vi1];
            vertex c = state.projected_buffer.values[state.geo_buffer.values[i].vi2];
            u32 color = state.geo_buffer.values[i].color;

            if (b.position.y < a.position.y) {
                swap_vertices(&a, &b);
            }
            if (c.position.y < a.position.y) {
                swap_vertices(&a, &c);
            }
            if (c.position.y < b.position.y) {
                swap_vertices(&b, &c);
            }

            f32 *ab = interpolate(a.position.y, a.position.x, b.position.y, b.position.x);
            u32 len_ab = (i32)b.position.y - (i32)a.position.y + 1;
            f32 *bc = interpolate(b.position.y, b.position.x, c.position.y, c.position.x);
            u32 len_bc = (i32)c.position.y - (i32)b.position.y + 1;
            f32 *ac = interpolate(a.position.y, a.position.x, c.position.y, c.position.x);
            u32 len_ac = (i32)c.position.y - (i32)a.position.y + 1;

            f32 *zab = interpolate(a.position.y, a.position.z, b.position.y, b.position.z);
            f32 *zbc = interpolate(b.position.y, b.position.z, c.position.y, c.position.z);
            f32 *zac = interpolate(a.position.y, a.position.z, c.position.y, c.position.z);

            f32 *abc = add_array(ab, bc, len_ab - 1, len_bc);
            f32 *zabc = add_array(zab, zbc, len_ab - 1, len_bc);

            u32 m = len_ab - 1;
            f32 *x_left, *x_right;
            f32 *z_left, *z_right;

            if (abc[m] < ac[m]) {
                x_left = abc;
                x_right = ac;
                
                z_left = zabc;
                z_right = zac;

            } else {
                x_left = ac;
                x_right = abc;

                z_left = zac;
                z_right = zabc;
            }

            for (i32 y = (i32)a.position.y; y < (i32)c.position.y; y++) {
                u32 idy = y - (i32)a.position.y;

                i32 x_start = x_left[idy];
                i32 x_end = x_right[idy];

                f32 z_start = z_left[idy];
                f32 z_end = z_right[idy];
                f32 *zscan = interpolate(x_start, z_start, x_end, z_end);
                
                for (i32 x = x_start; x < x_end; x++) {
                    f32 z = zscan[(i32)(x - x_start)];

                    if (state.depth_buffering){
                        if (check_depth(x, y) == -1 || check_depth(x, y) > z){
                            add_depth(x, y, z);

                            draw_pixel(x, y, color);
                        }

                    }else{
                        draw_pixel(x, y, color);
                    }
    
                    
                }
                free(zscan);
            }

        free(abc);
        free(ab);
        free(bc);
        free(ac);

        free(zabc);
        free(zab);
        free(zbc);
        free(zac);
        }else if (state.mode == 4){
            vertex a = state.projected_buffer.values[state.geo_buffer.values[i].vi0];
            vertex b = state.projected_buffer.values[state.geo_buffer.values[i].vi1];
            vertex c = state.projected_buffer.values[state.geo_buffer.values[i].vi2];
            //print_vector3D(a.light_intensity);

            if (b.position.y < a.position.y) {
                swap_vertices(&a, &b);
            }
            if (c.position.y < a.position.y) {
                swap_vertices(&a, &c);
            }
            if (c.position.y < b.position.y) {
                swap_vertices(&b, &c);
            }

            f32 *ab = interpolate(a.position.y, a.position.x, b.position.y, b.position.x);
            u32 len_ab = (i32)b.position.y - (i32)a.position.y + 1;
            f32 *bc = interpolate(b.position.y, b.position.x, c.position.y, c.position.x);
            u32 len_bc = (i32)c.position.y - (i32)b.position.y + 1;
            f32 *ac = interpolate(a.position.y, a.position.x, c.position.y, c.position.x);
            u32 len_ac = (i32)c.position.y - (i32)a.position.y + 1;

            f32 *zab = interpolate(a.position.y, a.position.z, b.position.y, b.position.z);
            f32 *zbc = interpolate(b.position.y, b.position.z, c.position.y, c.position.z);
            f32 *zac = interpolate(a.position.y, a.position.z, c.position.y, c.position.z);

            f32 *rab = interpolate(a.position.y, a.light_intensity.x, b.position.y, b.light_intensity.x);
            f32 *rbc = interpolate(b.position.y, b.light_intensity.x, c.position.y, c.light_intensity.x);
            f32 *rac = interpolate(a.position.y, a.light_intensity.x, c.position.y, c.light_intensity.x);

            f32 *gab = interpolate(a.position.y, a.light_intensity.y, b.position.y, b.light_intensity.y);
            f32 *gbc = interpolate(b.position.y, b.light_intensity.y, c.position.y, c.light_intensity.y);
            f32 *gac = interpolate(a.position.y, a.light_intensity.y, c.position.y, c.light_intensity.y);

            f32 *bab = interpolate(a.position.y, a.light_intensity.z, b.position.y, b.light_intensity.z);
            f32 *bbc = interpolate(b.position.y, b.light_intensity.z, c.position.y, c.light_intensity.z);
            f32 *bac = interpolate(a.position.y, a.light_intensity.z, c.position.y, c.light_intensity.z);

            f32 *abc = add_array(ab, bc, len_ab - 1, len_bc);
            f32 *zabc = add_array(zab, zbc, len_ab - 1, len_bc);
            
            f32 *rabc = add_array(rab, rbc, len_ab - 1, len_bc);
            f32 *gabc = add_array(gab, gbc, len_ab - 1, len_bc);
            f32 *babc = add_array(bab, bbc, len_ab - 1, len_bc);

            u32 m = len_ab - 1;
            f32 *x_left, *x_right;
            f32 *z_left, *z_right;

            f32 *r_left, *r_right;
            f32 *g_left, *g_right;
            f32 *b_left, *b_right;

            if (abc[m] < ac[m]) {
                x_left = abc;
                x_right = ac;
                
                z_left = zabc;
                z_right = zac;

                r_left = rabc;
                r_right = rac;

                g_left = gabc;
                g_right = gac;

                b_left = babc;
                b_right = bac;

            } else {
                x_left = ac;
                x_right = abc;

                z_left = zac;
                z_right = zabc;

                r_left = rac;
                r_right = rabc;

                g_left = gac;
                g_right = gabc;

                b_left = bac;
                b_right = babc;
            }

            for (i32 y = (i32)a.position.y; y < (i32)c.position.y; y++) {
                u32 idy = y - (i32)a.position.y;

                i32 x_start = x_left[idy];
                i32 x_end = x_right[idy];

                f32 z_start = z_left[idy];
                f32 z_end = z_right[idy];

                f32 r_start = r_left[idy];
                f32 r_end = r_right[idy];

                f32 g_start = g_left[idy];
                f32 g_end = g_right[idy];

                f32 b_start = b_left[idy];
                f32 b_end = b_right[idy];

                f32 *zscan = interpolate(x_start, z_start, x_end, z_end);

                f32 *rscan = interpolate(x_start, r_start, x_end, r_end);
                f32 *gscan = interpolate(x_start, g_start, x_end, g_end);
                f32 *bscan = interpolate(x_start, b_start, x_end, b_end);
                
                for (i32 x = x_start; x < x_end; x++) {
                    f32 z = zscan[(i32)(x - x_start)];

                    if (state.depth_buffering){
                        if (check_depth(x, y) == -1 || check_depth(x, y) > z){
                            add_depth(x, y, z);

                            f32 r = fmin(rscan[(i32)(x - x_start)] * 255, 255);
                            f32 g = fmin(gscan[(i32)(x - x_start)] * 255, 255);
                            f32 b = fmin(bscan[(i32)(x - x_start)] * 255, 255);

                            //printf("%f, %f, %f\n", r, g, b);

                            u32 new_color = ((u8)r << 16) | ((u8)g << 8) | (u8)b;

                            draw_pixel(x, y, new_color);
                        }
                    }else{
                        f32 r = fmin(rscan[(i32)(x - x_start)] * 255, 255);
                        f32 g = fmin(gscan[(i32)(x - x_start)] * 255, 255);
                        f32 b = fmin(bscan[(i32)(x - x_start)] * 255, 255);

                        //printf("%f, %f, %f\n", r, g, b);

                        u32 new_color = ((u8)r << 16) | ((u8)g << 8) | (u8)b;

                        draw_pixel(x, y, new_color);
                    }
                }
                free(zscan);

                free(rscan);
                free(gscan);
                free(bscan);
            }

        free(abc);
        free(ab);
        free(bc);
        free(ac);

        free(zabc);
        free(zab);
        free(zbc);
        free(zac);

        free(rabc);
        free(rab);
        free(rbc);
        free(rac);

        free(gabc);
        free(gab);
        free(gbc);
        free(gac);

        free(babc);
        free(bab);
        free(bbc);
        free(bac);
        }else if (state.mode == 5){
            vertex a = state.projected_buffer.values[state.geo_buffer.values[i].vi0];
            vertex b = state.projected_buffer.values[state.geo_buffer.values[i].vi1];
            vertex c = state.projected_buffer.values[state.geo_buffer.values[i].vi2];
            //print_vector3D(a.light_intensity);

            if (b.position.y < a.position.y) {
                swap_vertices(&a, &b);
            }
            if (c.position.y < a.position.y) {
                swap_vertices(&a, &c);
            }
            if (c.position.y < b.position.y) {
                swap_vertices(&b, &c);
            }

            f32 *ab = interpolate(a.position.y, a.position.x, b.position.y, b.position.x);
            u32 len_ab = (i32)b.position.y - (i32)a.position.y + 1;
            f32 *bc = interpolate(b.position.y, b.position.x, c.position.y, c.position.x);
            u32 len_bc = (i32)c.position.y - (i32)b.position.y + 1;
            f32 *ac = interpolate(a.position.y, a.position.x, c.position.y, c.position.x);
            u32 len_ac = (i32)c.position.y - (i32)a.position.y + 1;

            f32 *zab = interpolate(a.position.y, a.position.z, b.position.y, b.position.z);
            f32 *zbc = interpolate(b.position.y, b.position.z, c.position.y, c.position.z);
            f32 *zac = interpolate(a.position.y, a.position.z, c.position.y, c.position.z);

            f32 *nxab = interpolate(a.position.y, a.normal.x, b.position.y, b.normal.x);
            f32 *nxbc = interpolate(b.position.y, b.normal.x, c.position.y, c.normal.x);
            f32 *nxac = interpolate(a.position.y, a.normal.x, c.position.y, c.normal.x);

            f32 *nyab = interpolate(a.position.y, a.normal.y, b.position.y, b.normal.y);
            f32 *nybc = interpolate(b.position.y, b.normal.y, c.position.y, c.normal.y);
            f32 *nyac = interpolate(a.position.y, a.normal.y, c.position.y, c.normal.y);

            f32 *nzab = interpolate(a.position.y, a.normal.z, b.position.y, b.normal.z);
            f32 *nzbc = interpolate(b.position.y, b.normal.z, c.position.y, c.normal.z);
            f32 *nzac = interpolate(a.position.y, a.normal.z, c.position.y, c.normal.z);

            f32 *abc = add_array(ab, bc, len_ab - 1, len_bc);
            f32 *zabc = add_array(zab, zbc, len_ab - 1, len_bc);
            
            f32 *nxabc = add_array(nxab, nxbc, len_ab - 1, len_bc);
            f32 *nyabc = add_array(nyab, nybc, len_ab - 1, len_bc);
            f32 *nzabc = add_array(nzab, nzbc, len_ab - 1, len_bc);

            u32 m = len_ab - 1;
            f32 *x_left, *x_right;
            f32 *z_left, *z_right;

            f32 *nx_left, *nx_right;
            f32 *ny_left, *ny_right;
            f32 *nz_left, *nz_right;

            if (abc[m] < ac[m]) {
                x_left = abc;
                x_right = ac;
                
                z_left = zabc;
                z_right = zac;

                nx_left = nxabc;
                nx_right = nxac;

                ny_left = nyabc;
                ny_right = nyac;

                nz_left = nzabc;
                nz_right = nzac;

            } else {
                x_left = ac;
                x_right = abc;

                z_left = zac;
                z_right = zabc;

                nx_left = nxac;
                nx_right = nxabc;

                ny_left = nyac;
                ny_right = nyabc;

                nz_left = nzac;
                nz_right = nzabc;
            }

            for (i32 y = (i32)a.position.y; y < (i32)c.position.y; y++) {
                u32 idy = y - (i32)a.position.y;

                i32 x_start = x_left[idy];
                i32 x_end = x_right[idy];

                f32 z_start = z_left[idy];
                f32 z_end = z_right[idy];

                f32 nx_start = nx_left[idy];
                f32 nx_end = nx_right[idy];

                f32 ny_start = ny_left[idy];
                f32 ny_end = ny_right[idy];

                f32 nz_start = nz_left[idy];
                f32 nz_end = nz_right[idy];

                f32 *zscan = interpolate(x_start, z_start, x_end, z_end);

                f32 *nxscan = interpolate(x_start, nx_start, x_end, nx_end);
                f32 *nyscan = interpolate(x_start, ny_start, x_end, ny_end);
                f32 *nzscan = interpolate(x_start, nz_start, x_end, nz_end);
                
                for (i32 x = x_start; x < x_end; x++) {
                    f32 z = zscan[(i32)(x - x_start)];

                    if (state.depth_buffering){
                        if (check_depth(x, y) == -1 || check_depth(x, y) > z){
                            add_depth(x, y, z);

                            f32 nx = fmin(nxscan[(i32)(x - x_start)] * 255, 255);
                            f32 ny = fmin(nyscan[(i32)(x - x_start)] * 255, 255);
                            f32 nz = fmin(nzscan[(i32)(x - x_start)] * 255, 255);

                            vector3D normal = {nx, ny, nz};
                            normal = normalize(normal);


                            //printf("%f, %f, %f\n", r, g, b);

                            u32 new_color = calculate_lighting(normal, state.geo_buffer.values[i].color);

                            draw_pixel(x, y, new_color);
                        }
                    }else{
                        f32 nx = fmin(nxscan[(i32)(x - x_start)] * 255, 255);
                        f32 ny = fmin(nyscan[(i32)(x - x_start)] * 255, 255);
                        f32 nz = fmin(nzscan[(i32)(x - x_start)] * 255, 255);
                        vector3D normal = {nx, ny, nz};
                        normal = normalize(normal);

                        u32 new_color = calculate_lighting(normal, state.geo_buffer.values[i].color);

                        draw_pixel(x, y, new_color);
                    }
                }
                free(zscan);

                free(nxscan);
                free(nyscan);
                free(nzscan);
            }

        free(abc);
        free(ab);
        free(bc);
        free(ac);

        free(zabc);
        free(zab);
        free(zbc);
        free(zac);

        free(nxabc);
        free(nxab);
        free(nxbc);
        free(nxac);

        free(nyabc);
        free(nyab);
        free(nybc);
        free(nyac);

        free(nzabc);
        free(nzab);
        free(nzbc);
        free(nzac);
        }
    }
}

void render_scene(){
    assign_face_normals();
    if (state.mode == 3){
        light_scene_faces();
    }
    if (state.mode == 4){
        assign_vertex_normals();
        light_scene_vertices();
    }

    if (state.mode == 5){
        assign_vertex_normals();
    }

    if (state.backface_culling){
        cull_backfaces();
    }

    clip_scene();
    project_vertices();

    if (state.mode == 0){
        draw_vertices();

    }else if(state.mode == 1){
        draw_wireframe();

    }else if(state.mode == 2 || state.mode == 3 || state.mode == 4 || state.mode == 5){
        draw_filled();
    }
    clear_vertice_array(&state.projected_buffer);
}