//
// Created by Geisthardt on 26.02.2025.
//


#include "SGE.h"
#include <signal.h>


void create_cube();
void create_healthbar();

void seg_fault_handler(int sig) {
       log_event(LOG_LEVEL_FATAL, "SEG FAULT OCCURED: %d", sig);
}

int main(void) {
        atexit(terminate_program);
        //signal(SIGSEGV, seg_fault_handler);
        sge_log_settings log_settings = { .include_internal_logs = SGE_TRUE, .write_instantly = SGE_TRUE, .is_release = SGE_FALSE };
        if (start_logger(log_settings) != 0) {
                return 1;
        }
        log_event(LOG_LEVEL_INFO, "TEST EVENT: %d", 20);

        sge_screens_data *screen_data = get_screens_data();
        sge_screen_data *secondary_screen = allocate_memory(sizeof(sge_screen_data), MEMORY_TAG_WINDOW);
        for (int i = 0; i < screen_data->num_screens; ++i) {
                sge_screen_data screen = screen_data->screens[i];
                printf("%sMonitor %s, with resolution: %d:%d\n", screen.is_primary ? "Primary   " : "Secondary ", screen.name, screen.width, screen.height);
                if (!screen.is_primary) {
                        copy_memory(secondary_screen, &screen, sizeof(sge_screen_data), 0, 0);
                }
        }

        sge_window_create_settings window_create_settings = {
                .window_mode = SGE_WINDOW_MODE_BORDERLESS_FULLSCREEN,
                .width = 700,
                .height = 500,
                .x = 200,
                .y = 200,
                .is_resizable = SGE_TRUE
        };
        sge_window *window= sge_window_create("Test Window", &window_create_settings);
        sge_render *render = sge_render_create(SGE_RENDER_API_VULKAN, window);

        enable_input_tracking();
        //vulkan_context *vk_context = allocate_memory(sizeof(vulkan_context), MEMORY_TAG_VULKAN);
        //if (initialize_vk(vk_context ,wcw, "SGE TEST", VK_MAKE_API_VERSION(0,0,1,0)) != 0) {
        //        log_event(LOG_LEVEL_FATAL, "Failed to initialize Vk");
        //}

        sge_render_settings *settings = allocate_memory(sizeof(sge_render_settings), MEMORY_TAG_RENDERER);
        settings = &(sge_render_settings){
                .vulkan = {
                        .enable_validation_layers = SGE_TRUE,
                        .use_dynamic_rendering = SGE_TRUE,
                        .use_sge_allocator = SGE_TRUE,
                        .app_info = {
                                .application_name = "Test Example",
                                .application_version = sge_make_app_version(0, 1, 1)
                        }
                }
        };
        sge_render_initialize(render, settings);

        const int target_fps = 0;

        create_cube();
        create_healthbar();
        //char *test_allocation = allocate_memory(256, MEMORY_TAG_UNKNOWN);
        //char *test_allocation_2 = allocate_memory(256, MEMORY_TAG_TEST_1);
        //char *test_allocation_3 = allocate_memory(256, MEMORY_TAG_TEST_2);
        //char *test_allocation_4 = allocate_memory(23256, MEMORY_TAG_TEST_3);
        //set_memory(test_allocation, 'A', 240, 16);
        //copy_memory(test_allocation_2, test_allocation, 240, 0, 16);
        //zero_memory(test_allocation_3, 256, 0);
        //free_memory(test_allocation_2, MEMORY_TAG_TEST_1);
        //free_memory(test_allocation_2, MEMORY_TAG_TEST_1);
        //free_memory(test_allocation_3, MEMORY_TAG_TEST_2);
        //free_memory(test_allocation, MEMORY_TAG_UNKNOWN);

        print_memory_usage_str();

        sge_region_settings region_setting_3d = {
                .auto_scale_on_resize = SGE_TRUE,
                .auto_reposition_on_resize = SGE_FALSE,
                .height = SGE_REGION_FULL_DIMENSION,
                .width = SGE_REGION_FULL_DIMENSION,
                .offset_x = 0,
                .offset_y = 0,
                .min_depth = 0.0f,
                .max_depth = 1.0f,
                .z_index = 2,
                .type = SGE_REGION_3D
        };

        sge_region *main_region = sge_region_create(render, &region_setting_3d);

        //sge_add_renderable(render, logo_renderable);

        //sge_region_add_renderable(main_region, logo_renderable);

        sge_region_settings second_setting_2d = {
                .auto_scale_on_resize = true,
                .auto_reposition_on_resize = true,
                .height = SGE_REGION_FULL_DIMENSION,
                .width = SGE_REGION_FULL_DIMENSION,
                .offset_x = 0,
                .offset_y = 0,
                .min_depth = 0.0f,
                .max_depth = 1.0f,
                .z_index = 1,
                .type = SGE_REGION_2D
        };

        sge_region *secondary_region = sge_region_create(render, &second_setting_2d);

        char* author_name = "Stein";
        char* scene_name = "test_scene";

        sge_scene *scene = sge_scene_create(scene_name, author_name, NULL);

        vec3 position_transformation = {2, -1, -1};
        sge_scene_section *sgerend_section = sge_scene_create_sgerend_section(
                SGE_SCENE_SGEREND_INCLUDE_TYPE_EXTERNAL,
                "Test Cube Section",
                "cube.sgerend",
                strlen("cube.sgerend"),
                SGE_SCENE_TRANSFORMATION_FLAG_POSITION,
                &position_transformation);

        typedef struct scale_position_trans {
                vec3 position_trans;
                vec3 scale_trans;
        }scale_position_trans;

        scale_position_trans sca_pos_trans = {
                .position_trans =  {2, -1, -1},
                .scale_trans = {2, 2, 1}
        };

        sge_scene_section *pyramid_section = sge_scene_create_sgerend_section(
                SGE_SCENE_SGEREND_INCLUDE_TYPE_EXTERNAL,
                "TEST PYRAMID",
                "test.sgerend",
                strlen("test.sgerend"),
                SGE_SCENE_TRANSFORMATION_FLAG_POSITION | SGE_SCENE_TRANSFORMATION_FLAG_SCALE,
                &sca_pos_trans);

        sge_scene_add_section(scene, sgerend_section);
        sge_scene_add_section(scene, pyramid_section);

        typedef struct pos_rot_trans {
                vec3    position_transform;
                vec3    rotation_transform;
        } pos_rot_trans;
        pos_rot_trans trans_data = {
                .position_transform = {0, 0, 0},
                .rotation_transform = {45, 45, 45}
        };
        position_transformation = (vec3){0, 0, 0};

        vec3 rotation_transformation = {45, 0, 0};

        sge_scene_section *sgerend_section_2 = sge_scene_create_sgerend_section(
                SGE_SCENE_SGEREND_INCLUDE_TYPE_EXTERNAL,
                "test 2",
                "cube.sgerend",
                strlen("cube.sgerend"),
                SGE_SCENE_TRANSFORMATION_FLAG_ROTATION | SGE_SCENE_TRANSFORMATION_FLAG_POSITION,
                &trans_data);

        sge_scene_add_section(scene, sgerend_section_2);

        //printf("Section name seconds region: %s\n", scene->sections[1].section_header->section_name);

        sge_scene_save("test_scene", scene);

        sge_scene *scene_loaded = sge_scene_load("test_scene.sgescne");
        if (!scene_loaded) {
                terminate_program();
        }

        log_event(LOG_LEVEL_DEBUG, "Addeding scene to region");
        sge_region_add_scene(render, main_region, scene_loaded);
        log_event(LOG_LEVEL_DEBUG, "finished Addeding scene to region");
        //printf("FIRST SECTION: %d, %s\n", scene_loaded->sections[0].section_header->sge_scene_section_type, (char*)scene_loaded->sections[0].parsed_data.sgerend->sgerend_source_data);

       // sge_scene_header header = {
       //         .author_name = author_name,
       //         .author_name_size = strlen(author_name),
       //         .creation_date_timestamp = get_current_ms_time(),
       //         .last_modified_date_timestamp = get_current_ms_time(),
       //         .description = NULL,
       //         .description_size = 0,
       //         .header_extension_count = 0,
       //         .header_extensions = NULL,
       //         .header_extension_size = 0,
       //         .scene_name = scene_name,
       //         .scene_name_size = strlen(scene_name),
       //         .section_count = 0
       // };

       // sge_scene_section_header sgerend_section_header = {
       //         .sge_scene_section_type = SGE_SCENE_SECTION_TYPE_SGEREND,
       //         .
       // };

       // sge_scene_sgerend_section sge_scene_sgerend_section = {
       //         .include_type = SGE_SCENE_SGEREND_INCLUDE_TYPE_EXTERNAL,
       // };
       // sge_scene_section *sgerend_section = sge_scene_create_sgerend_section(sgerend_section_header, sge_scene_sgerend_section);

       // sge_scene_section scene_sections[] = { *sgerend_section };
       // sge_scene_save("test_scene", &header, scene_sections);

        //sge_region_add_renderable(secondary_region, logo_renderable);

        //m4 test_matrix =
        //m4 transformation_matrix;
        //vec3 translation_vector = {1.5, 1.0, 1.5};
        //m4 translation_matrix;
        //sge_m4_set_identity(translation_matrix);
        //m4 x_rotation_matrix;
        //sge_m4_set_identity(x_rotation_matrix);
        //m4 y_rotation_matrix;
        //sge_m4_set_identity(y_rotation_matrix);
        //sge_m4_set_translate(translation_matrix, translation_vector);
        //sge_m4_set_rotate_x(x_rotation_matrix, 180);
        //sge_m4_set_rotate_y(y_rotation_matrix, 90);
        //m4 temp;
        //sge_m4_multiply(temp, translation_matrix, x_rotation_matrix);
        //sge_m4_multiply(transformation_matrix, temp, y_rotation_matrix);
        //
        //vec4 init_pos = {0, 1, 0, 1};
        //vec4 end_pos = sge_m4_transform_vec4(transformation_matrix, init_pos);
        //printf("end pos: %f", end_pos.x);

        sge_movement_settings movement_settings_horizontal = {
                .mode = ROTATE_PITCH_AND_YAW,
                .delta_speed = 0.5f,
        };
        sge_movement_settings left_right = {
                .mode = ROTATE_YAW_ONLY,
                .delta_speed = 0.5f,
        };
        sge_movement_settings sprint_settings_horizontal = {
                .mode = ROTATE_PITCH_AND_YAW,
                .delta_speed = 3,
        };
        sge_movement_settings movement_settings_vertical = {
                .mode = ROTATE_NONE,
                .delta_speed = 0.2f
        };




        //sge_rend_section sections[2];
//
        //strncpy(sections[0].section_header.name, "MESHI MESH", sizeof(sections[1].section_header.name) - 1);
        //sections[0].section_header.name[sizeof(sections[1].section_header.name) - 1] = '\0';
        //sections[0].section_header.type = 1;  // 1 = Mesh (for example)
        //sections[0].section_header.data_size = 9 * sizeof(float);
        //sections[0].section_header.extension_count = 0;
        //sections[0].section_header.extension_size = 0;
        //sections[0].section_header.extensions = NULL;
        //sections[0].data = allocate_memory(sections[0].section_header.data_size, MEMORY_TAG_RENDERER);
        //if (sections[0].data == NULL) {
        //    fprintf(stderr, "Failed to allocate mesh data\n");
        //    return -1;
        //}
        //float triangleVertices[9] = {
        //     0.0f, 0.0f, 0.0f,  // Vertex 1  0.0 0.0 0.0
        //     1.0f, 0.0f, 0.0f,  // Vertex 2
        //     0.0f, 1.0f, 0.0f   // Vertex 3
        //};
        //memcpy(sections[0].data, triangleVertices, sections[0].section_header.data_size);
//
        //strncpy(sections[1].section_header.name, "Materialo", sizeof(sections[1].section_header.name) - 1);
        //sections[1].section_header.name[sizeof(sections[1].section_header.name) - 1] = '\0';
        //sections[1].section_header.type = 2;
        //sections[1].section_header.data_size = 4 * sizeof(float);
        //sections[1].section_header.extension_count = 0;
        //sections[1].section_header.extension_size = 0;
        //sections[1].section_header.extensions = NULL;
        //sections[1].data = allocate_memory(sections[1].section_header.data_size, MEMORY_TAG_RENDERER);
        //if (sections[1].data == NULL) {
        //    fprintf(stderr, "Failed to allocate material data\n");
        //    free(sections[0].data);
        //    return -1;
        //}
        //float blueColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
        //memcpy(sections[1].data, blueColor, sections[1].section_header.data_size);


        sge_mesh_attribute attributes[2] = {
                {
                        .type = SGE_ATTRIBUTE_POSITION,
                        .format = SGE_FORMAT_FLOAT32,
                        .components = 3,
                        .offset = 0,
                },
                {
                        .type = SGE_ATTRIBUTE_COLOR,
                        .format = SGE_FORMAT_UINT8,
                        .components = 4,
                        .offset = 12
                }
        };

        typedef struct {
                float position[3];
                uint8_t color[4];
        } ColoredVertex;

        ColoredVertex vertices[] = {
                // Base triangle 1 (flipped base)
                { .position = {-0.5f, 0.0f, -0.5f}, .color = {255, 0, 0, 255} },   // Bottom-left
                { .position = { 0.5f, 0.0f,  0.5f}, .color = {0, 0, 255, 255} },   // Top-right
                { .position = { 0.5f, 0.0f, -0.5f}, .color = {0, 255, 0, 255} },   // Bottom-right

                // Base triangle 2
                { .position = {-0.5f, 0.0f, -0.5f}, .color = {255, 0, 0, 255} },
                { .position = {-0.5f, 0.0f,  0.5f}, .color = {255, 255, 0, 255} }, // Top-left
                { .position = { 0.5f, 0.0f,  0.5f}, .color = {0, 0, 255, 255} },

                // Side triangle 1
                { .position = {-0.5f, 0.0f, -0.5f}, .color = {255, 0, 0, 255} },
                { .position = { 0.0f, -1.0f,  0.0f}, .color = {255, 255, 255, 255} }, // Peak (negative Y)
                { .position = { 0.5f, 0.0f, -0.5f}, .color = {0, 255, 0, 255} },

                // Side triangle 2
                { .position = { 0.5f, 0.0f, -0.5f}, .color = {0, 255, 0, 255} },
                { .position = { 0.0f, -1.0f,  0.0f}, .color = {255, 255, 255, 255} },
                { .position = { 0.5f, 0.0f,  0.5f}, .color = {0, 0, 255, 255} },

                // Side triangle 3
                { .position = { 0.5f, 0.0f,  0.5f}, .color = {0, 0, 255, 255} },
                { .position = { 0.0f, -1.0f,  0.0f}, .color = {255, 255, 255, 255} },
                { .position = {-0.5f, 0.0f,  0.5f}, .color = {255, 255, 0, 255} },

                // Side triangle 4
                { .position = {-0.5f, 0.0f,  0.5f}, .color = {255, 255, 0, 255} },
                { .position = { 0.0f, -1.0f,  0.0f}, .color = {255, 255, 255, 255} },
                { .position = {-0.5f, 0.0f, -0.5f}, .color = {255, 0, 0, 255} },
        };
        sge_rend_section *triangle_test_mesh_section = sge_create_mesh_section(
                "test triangle",
                vertices,
                18,
                16,
                attributes,
                2,
                NULL,
                0
        );

        sge_rend_section sections[] = { *triangle_test_mesh_section };

        SGE_RESULT result = sge_rend_save("test", sections, 1);
        if (result != SGE_SUCCESS) {
            fprintf(stderr, "Failed to save render file\n");
        } else {
            printf("Render file saved successfully.\n");
        }




        sge_rend_file *cube_file = NULL;
        sge_rend_file *pyramid_file = NULL;
        sge_rend_file *healthbar_file = NULL;
        sge_rend_load("cube.sgerend", &cube_file);
        sge_rend_load("test.sgerend", &pyramid_file);
        sge_rend_load("healthbar.sgerend", &healthbar_file);

        //for (int i = 0; i < file->header.section_count; ++i) {
        //        sge_rend_section *section = &file->sections[i];
        //        printf("Section %d (type: %d, size: %zu bytes):\n",
        //               i, section->section_header.type, section->section_header.data_size);

        //        unsigned char *bytes = (unsigned char*)section->data;
        //        for (size_t j = 0; j < section->section_header.data_size; j++) {
        //                printf("%02X ", bytes[j]);
        //                if ((j + 1) % 16 == 0) printf("\n");
        //        }
        //        printf("\n\n");
        //}

        log_event(LOG_LEVEL_INFO, "creating test renderable");
        sge_renderable *test = create_renderable_from_rend_file(render, pyramid_file);
        log_event(LOG_LEVEL_INFO, "finished creating test renderable");
        sge_renderable *cube = create_renderable_from_rend_file(render, cube_file);
        log_event(LOG_LEVEL_INFO, "finished creating cube renderable");

        sge_renderable *healthbar = create_renderable_from_rend_file(render, healthbar_file);
        log_event(LOG_LEVEL_INFO, "finished creating test renderable");

        sge_region_add_renderable(main_region, test);
        //sge_region_add_renderable(main_region, cube);
        //sge_region_add_renderable(secondary_region, healthbar);

        printf("ADDED REGION\n");
        //printf("%p\n", file->sections[1].data);

        SGE_BOOL is_first_frame = SGE_TRUE;
        uint32_t frame_count = 0;

        while (!window_should_close()) {
                //printf("STARTED LOOP");
                //do stuff
                //timeBeginPeriod(1);
                frame_count++;
                const DWORD start_time = get_current_ms_time();

                sge_region *active_region = sge_region_get_active(render);
                //log_event(LOG_LEVEL_INFO, "checking active region");
                //if (active_region != NULL && active_region->type == SGE_REGION_2D) {
                //        log_event(LOG_LEVEL_INFO, "2d region unsetting");
                //        active_region = NULL;
                //}
                int regions_count;
                //sge_region **active_regions = sge_region_get_active_list(render, &regions_count);
                //printf("%d\n", regions_count);
                //for (int i = 0; i < regions_count; ++i) {
                //        sge_region *region = active_regions[i];
                //        printf("Region Type: %d\n", region->type);
                //}

                //for (int i = 0; i < regions_count; ++i) {
                //        printf("ACTIVE REGIONS: %i\n", i);
                //}
                //if (regions_count > 0) {
                //        printf("\n");
                //}

                //do everything in here
                if (is_key_down(KEY_ESCAPE)) {
                        //printf("PRESSED ESCAPE\n");
                }

                //if (is_key_down(KEY_A)) {
                //        //printf("HOLDING A\n");
                //        //sge_camera_move_left(render);
                //}

                if (is_key_down(KEY_A)) {
                        //printf("PRESSED A ONCE\n");
                        sge_camera_move_left(render, active_region, left_right);
                }

                if (is_key_down(KEY_W)) {
                        if (is_key_down(KEY_CTRL)) {
                                sge_camera_move_forward(render, active_region, sprint_settings_horizontal);
                        } else {
                                sge_camera_move_forward(render, active_region, movement_settings_horizontal);
                        }
                }

                if (is_key_down(KEY_S)) {
                        sge_camera_move_backwards(render, active_region, movement_settings_horizontal);
                }

                if (is_key_down(KEY_D)) {
                        sge_camera_move_right(render, active_region, left_right);
                }

                if (is_key_down(KEY_SPACE)) {
                        sge_camera_move_up(render, active_region, movement_settings_vertical);
                }

                if (is_key_down(KEY_SHIFT)) {
                        sge_camera_move_down(render, active_region, movement_settings_vertical);
                }

                if (is_key_pressed(KEY_ARROW_LEFT)) {
                        sge_camera_rotate_y(render,active_region, -15);
                }

                if (is_key_pressed(KEY_ARROW_RIGHT)) {
                        sge_camera_rotate_y(render, active_region, 15);
                }

                if (is_key_pressed(KEY_ARROW_UP)) {
                        sge_camera_rotate_x(render, active_region, 15);
                }

                if (is_key_pressed(KEY_ARROW_DOWN)) {
                        sge_camera_rotate_x(render, active_region, -15);
                }


                if (is_key_pressed(KEY_F11)) {
                        log_event(LOG_LEVEL_INFO, "updating win");
                        sge_window_create_settings win_update_settings = {
                                .window_mode = SGE_WINDOW_MODE_WINDOWED,
                                .screen = secondary_screen,
                                .x = 100,
                                .y = 200,
                                .height = 500,
                                .width = 1000,
                                .is_resizable = SGE_FALSE
                        };
                        sge_window_mode_update(window, &win_update_settings);
                }

                mouse_pos delta_mouse_pos = get_delta_mouse_position();

                sge_mouse_movement_settings mouse_movement_settings = {
                        .mouse_delta_x = delta_mouse_pos.x,
                        .mouse_delta_y = delta_mouse_pos.y,
                        .sensitivity = 1,
                        .flags = 0,
                };

                if (mouse_movement_settings.mouse_delta_x > 250 || mouse_movement_settings.mouse_delta_x < -250
                        || mouse_movement_settings.mouse_delta_y > 250 || mouse_movement_settings.mouse_delta_y < -250) {
                        mouse_movement_settings.mouse_delta_x = 0;
                        mouse_movement_settings.mouse_delta_y = 0;
                }

                if (is_mouse_down(MBUTTON_RIGHT)) {
                        hide_mouse();
                        sge_camera_rotate(render, active_region, mouse_movement_settings);
                }

                if (was_mouse_down(MBUTTON_RIGHT)) {
                        printf("MOUSE WAS DUWN\n\n\n\n\n");
                        show_mouse();
                }

                //printf("Current Mouse Pos: x: %d, y: %d\n", current_mouse_pos.x, current_mouse_pos.y);
                //draw_frame(vk_context);
                //printf("DREW 1 FRAME\n");
                //sge_update_uniform_buffer(render, main_region);
                //sge_update_uniform_buffer(render, secondary_region);
                sge_draw_frame(render);
                update_frame(target_fps, start_time, window);
                //timeEndPeriod(1);
                //printf("REACHED LOOP END, REPEATING");
                if (frame_count == 999999) {
                        log_event(LOG_LEVEL_INFO, "UPDATING WIN MODE");
                        if (!secondary_screen) {
                                terminate_program();
                        }
                        sge_window_create_settings win_update_settings = {
                                .window_mode = SGE_WINDOW_MODE_BORDERLESS_FULLSCREEN,
                                .screen = secondary_screen,
                                .x = 100,
                                .y = 200,
                                .height = 500,
                                .width = 2000,
                                .is_resizable = SGE_FALSE
                        };
                        sge_window_mode_update(window, &win_update_settings);
                }
        }

        sge_window_destroy(window);

        stop_logger();

        log_event(LOG_LEVEL_WARNING, "NO LOGGER ACTIVE");

        log_event(LOG_LEVEL_FATAL, "ERROR HIHA");

        return 0;
}

void create_cube() {
//GPT created
        sge_mesh_attribute attributes[2] = {
        {
            .type = SGE_ATTRIBUTE_POSITION,
            .format = SGE_FORMAT_FLOAT32,
            .components = 3,
            .offset = 0,
        },
        {
                .type = SGE_ATTRIBUTE_COLOR,
                .format = SGE_FORMAT_UINT8,
                .components = 4,
                .offset = 12,
        }
    };

        typedef struct {
                float position[3];
                uint8_t color[4];
        } ColoredVertex;

    // Define 36 vertices for the cube's 12 triangles (6 faces, 2 triangles each)
    ColoredVertex vertices[36] = {
        // Front face (z = -0.5)
        { .position = {-0.5f, -0.5f, -0.5f} , .color = {255, 0, 0, 255} },
        { .position = { 0.5f, -0.5f, -0.5f} , .color = {255, 0, 0, 255} },
        { .position = { 0.5f,  0.5f, -0.5f} , .color = {255, 0, 0, 255} },
        { .position = {-0.5f, -0.5f, -0.5f} , .color = {255, 0, 0, 255} },
        { .position = { 0.5f,  0.5f, -0.5f} , .color = {255, 0, 0, 255} },
        { .position = {-0.5f,  0.5f, -0.5f} , .color = {255, 0, 0, 255} },

        // Back face (z = 0.5)
        { .position = {-0.5f, -0.5f,  0.5f} , .color = {0, 0, 255, 255} },
        { .position = {-0.5f,  0.5f,  0.5f} , .color = {0, 0, 255, 255} },
        { .position = { 0.5f,  0.5f,  0.5f} , .color = {0, 0, 255, 255} },
        { .position = {-0.5f, -0.5f,  0.5f} , .color = {0, 0, 255, 255} },
        { .position = { 0.5f,  0.5f,  0.5f} , .color = {0, 0, 255, 255} },
        { .position = { 0.5f, -0.5f,  0.5f} , .color = {0, 0, 255, 255} },

        // Left face (x = -0.5)
        { .position = {-0.5f, -0.5f, -0.5f} , .color = {0, 255, 0, 255} },
        { .position = {-0.5f,  0.5f, -0.5f} , .color = {0, 255, 0, 255} },
        { .position = {-0.5f,  0.5f,  0.5f} , .color = {0, 255, 0, 255} },
        { .position = {-0.5f, -0.5f, -0.5f} , .color = {0, 255, 0, 255} },
        { .position = {-0.5f,  0.5f,  0.5f} , .color = {0, 255, 0, 255} },
        { .position = {-0.5f, -0.5f,  0.5f} , .color = {0, 255, 0, 255} },

        // Right face (x = 0.5)
        { .position = { 0.5f, -0.5f, -0.5f} , .color = {255, 0, 0, 255} },
        { .position = { 0.5f, -0.5f,  0.5f} , .color = {255, 0, 0, 255} },
        { .position = { 0.5f,  0.5f,  0.5f} , .color = {255, 0, 0, 255} },
        { .position = { 0.5f, -0.5f, -0.5f} , .color = {255, 0, 0, 255} },
        { .position = { 0.5f,  0.5f,  0.5f} , .color = {255, 0, 0, 255} },
        { .position = { 0.5f,  0.5f, -0.5f} , .color = {255, 0, 0, 255} },

        // Bottom face (y = -0.5)
        { .position = {-0.5f, -0.5f, -0.5f} , .color = {255, 0, 0, 255} },
        { .position = { 0.5f, -0.5f, -0.5f} , .color = {255, 0, 0, 255} },
        { .position = { 0.5f, -0.5f,  0.5f} , .color = {255, 0, 0, 255} },
        { .position = {-0.5f, -0.5f, -0.5f} , .color = {255, 0, 0, 255} },
        { .position = { 0.5f, -0.5f,  0.5f} , .color = {255, 0, 0, 255} },
        { .position = {-0.5f, -0.5f,  0.5f} , .color = {255, 0, 0, 255} },

        // Top face (y = 0.5)
        { .position = {-0.5f,  0.5f, -0.5f} , .color = {255, 0, 0, 255} },
        { .position = { 0.5f,  0.5f, -0.5f} , .color = {255, 0, 0, 255} },
        { .position = { 0.5f,  0.5f,  0.5f} , .color = {255, 0, 0, 255} },
        { .position = {-0.5f,  0.5f, -0.5f} , .color = {255, 0, 0, 255} },
        { .position = { 0.5f,  0.5f,  0.5f} , .color = {255, 0, 0, 255} },
        { .position = {-0.5f,  0.5f,  0.5f} , .color = {255, 0, 0, 255} },
    };

    // Create the mesh section
    sge_rend_section *cube_mesh_section = sge_create_mesh_section(
        "test cube",    // Name of the mesh
        vertices,       // Vertex data
        36,             // Number of vertices
        16,             // Size of each vertex (3 floats * 4 bytes = 12 bytes)
        attributes,     // Attribute array
        2,              // Number of attributes (only position)
        NULL,           // No indices
        0               // Index count is 0
    );

    // Prepare the sections array and save the mesh
    sge_rend_section sections[] = { *cube_mesh_section };

    SGE_RESULT result = sge_rend_save("cube", sections, 1);
    if (result != SGE_SUCCESS) {
        fprintf(stderr, "Failed to save render file\n");
    } else {
        printf("Render file saved successfully.\n");
    }
}


void create_healthbar() {
        typedef struct {
                float position[2];
                uint8_t color[4];
        } UIColoredVertex;

        sge_mesh_attribute attributes[2] = {
        {
            .type = SGE_ATTRIBUTE_POSITION,
            .format = SGE_FORMAT_FLOAT32,
            .components = 2,
            .offset = 0,
        },
        {
            .type = SGE_ATTRIBUTE_COLOR,
            .format = SGE_FORMAT_UINT8,
            .components = 4,
            .offset = 8,
        }
    };

    // Define vertices
        UIColoredVertex healthbar_vertices[6] = {
                { .position = { 0.0, 0.0}, .color = { 0, 0, 255, 255} },
                { .position = { 1.0, 1.0}, .color = { 0, 255, 0, 255} },
                { .position = { 1.0, 0.0}, .color = { 0, 255, 0, 255} },
                { .position = { 0.0, 1.0}, .color = { 0, 255, 0, 255} },
                { .position = { 1.0, 1.0}, .color = { 0, 255, 0, 255} },
                { .position = { 1.0, 01.0}, .color = { 0, 255, 0, 255} },
        };

    // Create mesh section
    sge_rend_section *health_bar_section = sge_create_mesh_section(
        "health bar",
        healthbar_vertices,
        6,
        sizeof(UIColoredVertex),
        attributes,
        2,
        NULL,
        0
    );

    // Save (optional)
    sge_rend_section sections[] = { *health_bar_section };
    SGE_RESULT result = sge_rend_save("healthbar", sections, 1);
    if (result != SGE_SUCCESS) {
        fprintf(stderr, "Failed to save render file\n");
    } else {
        printf("Render file saved successfully.\n");
    }

    // Cleanup (not in original, but good practice)
    free(health_bar_section->data);
    free(health_bar_section);
}