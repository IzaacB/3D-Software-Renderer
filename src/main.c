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

    //DEBUG

    vector3D obj_transform = {0, -15, 0};
    vector3D obj_rotation = {0, 0, 0};
    vector3D obj_scale = {100, 100, 100};

    vector3D light_dir = {0, -1, 0};
    vector3D light_intensity = {0, .1, .1};
    dir_light light = {light_dir, light_intensity};

    vector3D light_dir1 = {1, 1, 1};
    vector3D light_intensity1 = {.1, 0, 0};
    dir_light light1 = {light_dir1, light_intensity1};

    vector3D light_dir2 = {0, .5, -1};
    vector3D light_intensity2 = {0, .1, 0};
    dir_light light2 = {light_dir2, light_intensity2};

    vector3D simon_pos = {0, 0, 0};
    f32 simon_scale = 1;
    sprite3D doomguy = {"Doomguy.bmp", simon_pos, simon_scale};

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
        initialize_dir_light_array(&state.dir_light_buffer, 1);

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

        //cow_rotation.x += 1 * delta;
        //cow_rotation.y += 1 * delta;

        add_obj_to_scene("stanford-bunny.obj", obj_transform, obj_rotation, obj_scale, WHITE, 0);
        insert_dir_light(&state.dir_light_buffer, light);
        insert_dir_light(&state.dir_light_buffer, light1);
        insert_dir_light(&state.dir_light_buffer, light2);

        clear_screen();

        // RENDER CODE HERE

        render_scene();
        render_sprite3D(doomguy);

        // END RENDER
        // Update screen and clear scene buffers
        update_screen();

        clear_vertice_array(&state.vertex_buffer);
        clear_face_array(&state.geo_buffer);
        clear_depth();

        // Delta time
        Uint32 frame_time = SDL_GetTicks() - current;
        if (frame_time < (1000 / TARGET_FPS)) {
            SDL_Delay((1000 / TARGET_FPS) - frame_time);
        }
    }
    quit();
    return 0;
}