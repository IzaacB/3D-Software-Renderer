#include <stdio.h>
#include <stdbool.h>

#include "type.h"
#include "vector.h"
#include "renderer.h"
#include "model.h"

int main(){
    //Initialization:
    SDL_Init(SDL_INIT_VIDEO);
    state.window = SDL_CreateWindow("SOFTWARE RENDERER", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_SHOWN);
    state.renderer = SDL_CreateRenderer(state.window, -1, SDL_RENDERER_PRESENTVSYNC);
    state.canvas = SDL_CreateTexture(state.renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, CANVAS_WIDTH, CANVAS_HEIGHT);

    state.viewport_width = 1.65; state.viewport_distance = .5; state.viewport_height = 1;
    state.viewport_position.x = 0; state.viewport_position.y = 0; state.viewport_position.z = 0;
    state.mode = 0;
    state.backface_culling = true;
    state.depth_buffering = true;

    //DEBUG

    f32 input_timer = 0;

    vector3D obj_transform = {0, -1, 5};
    vector3D obj_rotation = {0, 0, 0};
    vector3D obj_scale = {1, 1, 1};

    vector3D obj2_transform = {0, 0, 15};
    vector3D obj2_rotation = {0, 0, 0};
    vector3D obj2_scale = {1, 1, 1};

    directional_light light = {{1, 1, 0}, {1, 1, 1}};
    directional_light light2 = {{0, -1, 0}, {0, 1, 0}};

    vector3D ambient_light = {.1, .1, .1};
    state.ambient_light = ambient_light;

    //END DEBUG

    bool running = true;

    // Set up delta time variables
    Uint32 previous = SDL_GetTicks();
    float delta = 0.0f;

    while (running) {
        // Delta time
        Uint32 current = SDL_GetTicks();
        delta = (current - previous) / 1000.0f; // Convert milliseconds to seconds
        previous = current;

        // Setup event handler.
        SDL_Event event;
        while (SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT:
                    running = false;
                    break;
            }
        }
        // Initialize scene buffers
        initialize_vertice_array(&state.vertex_buffer, 1);
        initialize_face_array(&state.geo_buffer, 1);
        initialize_directional_light_array(&state.directional_light_buffer, 1);

        // Get keyboard input
        const u8 *keystate = SDL_GetKeyboardState(NULL);

        if (keystate[SDL_SCANCODE_W]){
            state.viewport_position.x -= 5 * sin(state.viewport_rotation.y) * delta;
            state.viewport_position.z += 5 * cos(state.viewport_rotation.y) * delta;
        }

        if (keystate[SDL_SCANCODE_S]){
            state.viewport_position.x += 5 * sin(state.viewport_rotation.y) * delta;
            state.viewport_position.z -= 5 * cos(state.viewport_rotation.y) * delta;
        }

        if (keystate[SDL_SCANCODE_D]){
            state.viewport_position.x += 5 * cos(state.viewport_rotation.y) * delta;
            state.viewport_position.z += 5 * sin(state.viewport_rotation.y) * delta;
        }

        if (keystate[SDL_SCANCODE_A]){
            state.viewport_position.x -= 5 * cos(state.viewport_rotation.y) * delta;
            state.viewport_position.z -= 5 * sin(state.viewport_rotation.y) * delta;
        }

        if (keystate[SDL_SCANCODE_LSHIFT]){
            state.viewport_position.y += 5 * delta;
        }

        if (keystate[SDL_SCANCODE_LCTRL]){
            state.viewport_position.y -= 5 * delta;
        }

        if (keystate[SDL_SCANCODE_UP]){
            state.viewport_rotation.x += 2 * delta;
        }

        if (keystate[SDL_SCANCODE_DOWN]){
            state.viewport_rotation.x -= 2 * delta;
        }

        if (keystate[SDL_SCANCODE_LEFT]){
            state.viewport_rotation.y += 2 * delta;
        }

        if (keystate[SDL_SCANCODE_RIGHT]){
            state.viewport_rotation.y -= 2 * delta;
        }

        if (keystate[SDL_SCANCODE_0]){
            state.mode = 0;
        }

        if (keystate[SDL_SCANCODE_1]){
            state.mode = 1;
        }

        if (keystate[SDL_SCANCODE_2]){
            state.mode = 2;
        }

        if (keystate[SDL_SCANCODE_3]){
            state.mode = 3;
        }
        
        if (keystate[SDL_SCANCODE_4]){
            state.mode = 4;
        }

        if (keystate[SDL_SCANCODE_5]){
            state.mode = 5;
        }

        if (keystate[SDL_SCANCODE_B] && input_timer <= 0){
            state.backface_culling = !state.backface_culling;
            input_timer = .5;
        }

        if (keystate[SDL_SCANCODE_Z] && input_timer <= 0){
            state.depth_buffering = !state.depth_buffering;
            input_timer = .5;
        }

        if (input_timer > 0){
            input_timer -= 1 * delta;
        }

        add_obj_to_scene("cow.obj", obj_transform, obj_rotation, obj_scale, WHITE);
        //add_obj_to_scene("cow.obj", obj2_transform, obj2_rotation, obj2_scale, GREEN);
        insert_directional_light(&state.directional_light_buffer, light);
        insert_directional_light(&state.directional_light_buffer, light2);

        clear_screen();
        clear_depth();

        // RENDER CODE HERE

        render_scene();

        // END RENDER
        
        // Update screen and clear scene buffers
        update_screen();

        clear_vertice_array(&state.vertex_buffer);
        clear_face_array(&state.geo_buffer);
        clear_directional_light_array(&state.directional_light_buffer);

        // Delta time
        Uint32 frame_time = SDL_GetTicks() - current;
        if (frame_time < (1000 / TARGET_FPS)) {
            SDL_Delay((1000 / TARGET_FPS) - frame_time);
        }
    }
    quit();
    return 0;
}