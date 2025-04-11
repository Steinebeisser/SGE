//
// Created by Geisthardt on 14.03.2025.
//

#include "renderer/sge_region.h"

#include <stdio.h>

#include "core/input.h"
#include "core/sge_internal_logging.h"
#include "core/memory_control.h"

#include "renderer/sge_internal_render.h"

extern bool is_hidden;
extern mouse_pos last_visible_pos;

SGE_RESULT sge_renderable_update_api_resources(sge_render *render, sge_renderable *renderable) {
        if (render->sge_interface->update_renderable_resources(render, renderable) != SGE_SUCCESS) {
                return SGE_ERROR;
        }
        return SGE_SUCCESS;
}

sge_region *sge_region_create(sge_render *render, sge_region_settings *settings) {
        sge_region *region = allocate_memory(sizeof(sge_region), MEMORY_TAG_REGION);
        if (region == NULL) {
                log_internal_event(LOG_LEVEL_FATAL, "failed to allocate for region");
                return NULL;
        }

        region->viewport = allocate_memory(sizeof(sge_viewport), MEMORY_TAG_REGION);
        if (region->viewport == NULL) {
                log_internal_event(LOG_LEVEL_FATAL, "failed to allocate for viewport");
                return NULL;
        }

        region->scissor = allocate_memory(sizeof(sge_scissor), MEMORY_TAG_REGION);
        if (region->scissor == NULL) {
                log_internal_event(LOG_LEVEL_FATAL, "failed to allocate for scissor");
                return NULL;
        }

        region->camera = allocate_memory(sizeof(sge_camera), MEMORY_TAG_REGION);
        if (region->camera == NULL) {
                log_internal_event(LOG_LEVEL_FATAL, "failed to allocate for camera");
                return NULL;
        }


        region->viewport->width = settings->width;
        region->viewport->height = settings->height;
        region->viewport->x = settings->offset_x;
        region->viewport->y = settings->offset_y;
        region->viewport->min_depth = settings->min_depth;
        region->viewport->max_depth = settings->max_depth;

        region->scissor->extent_height = settings->height;
        region->scissor->extent_width = settings->width;
        region->scissor->offset_x = settings->offset_x;
        region->scissor->offset_y = settings->offset_y;


        region->region_index = render->region_count;
        region->auto_scale_on_resize = settings->auto_scale_on_resize;
        region->reposition_on_resize = settings->auto_reposition_on_resize;
        region->z_index = settings->z_index;

        region->type = settings->type;

        log_internal_event(LOG_LEVEL_INFO, "creating vk descriptor pool");
        SGE_RESULT descriptor_pool_result = sge_create_descriptor_pool(render, &region->descriptor_pool);
        if (descriptor_pool_result != SGE_SUCCESS) {
                log_internal_event(LOG_LEVEL_FATAL, "failed to create descriptor pool");
                return NULL;
        }

        //todo based on frames in flight
        for (int i = 0; i < 3; ++i) {
                log_internal_event(LOG_LEVEL_INFO, "creating region buffer");
                SGE_RESULT buffer_result = sge_create_buffer(render, &region->uniform_buffers[i].buffer);
                if (buffer_result != SGE_SUCCESS) {
                        log_internal_event(LOG_LEVEL_FATAL, "failed to create buffer for region");
                        return NULL;
                }

                log_internal_event(LOG_LEVEL_INFO, "allocating region buffer");
                SGE_RESULT allocation_result = sge_allocate_buffer(render, &region->uniform_buffers[i].memory, region->uniform_buffers[i].buffer);
                if (allocation_result != SGE_SUCCESS) {
                        log_internal_event(LOG_LEVEL_FATAL, "failed to allocate memory for region buffer");
                        return NULL;
                }

                log_internal_event(LOG_LEVEL_INFO, "creating descriptor set");
                SGE_RESULT descriptor_set_result = sge_allocate_descriptor_set(render, &region->uniform_buffers[i].descriptor, &region->uniform_buffers[i].descriptor_layout, region->descriptor_pool);
                if (descriptor_set_result != SGE_SUCCESS) {
                        log_internal_event(LOG_LEVEL_FATAL, "failed to set descriptor for region");
                        return NULL;
                }

                log_internal_event(LOG_LEVEL_INFO, "writing to descriptor set");
                SGE_RESULT descriptor_write_result = sge_update_descriptor_set(render, &region->uniform_buffers[i]);
                if (descriptor_write_result != SGE_SUCCESS) {
                        log_internal_event(LOG_LEVEL_FATAL, "failed to write descriptor set");
                        return NULL;
                }
        }


        render->region_count++;
        render->regions = reallocate_memory(render->regions, sizeof(sge_region *) * render->region_count, MEMORY_TAG_REGION);
        render->regions[render->region_count -1] = region;


        log_internal_event(LOG_LEVEL_INFO, "created region");

        return region;
}


SGE_RESULT sge_region_add_renderable(sge_region *region, sge_renderable *renderable) {
        region->renderable_count++;
        region->renderables = reallocate_memory(region->renderables, sizeof(sge_renderable *) * region->renderable_count, MEMORY_TAG_RENDERER);
        if (region->renderables == NULL) {
                return SGE_ERROR_FAILED_ALLOCATION;
        }
        region->renderables[region->renderable_count - 1] = renderable;

        log_internal_event(LOG_LEVEL_INFO, "added renderable to region");

        return SGE_SUCCESS;
}

SGE_RESULT sge_region_add_scene(sge_render *render, sge_region *region, sge_scene *scene) {
        char *function_name = "sge_region_add_scene";
        for (int i = 0; i < scene->header.section_count; ++i) {
                sge_scene_section section = scene->sections[i];
                if (section.section_header->sge_scene_section_type == SGE_SCENE_SECTION_TYPE_SGEREND) {
                        if (section.parsed_data->sgerend->include_type == SGE_SCENE_SGEREND_INCLUDE_TYPE_EXTERNAL) {
                                sge_rend_file *rend_file;
                                sge_rend_load(section.parsed_data->sgerend->sgerend_source_data, &rend_file);
                                if (!rend_file) {
                                        log_internal_event(LOG_LEVEL_ERROR, "Failed to create renderable from scene");
                                        continue;;
                                }
                                sge_renderable *rend = create_renderable_from_rend_file(render, rend_file);
                                if (!rend) {
                                        log_event(LOG_LEVEL_ERROR, "Failed to create renderable in \"%s\"", function_name);
                                        continue;
                                }

                                size_t total_size = 0;
                                SGE_BOOL has_pos = SGE_FALSE;
                                SGE_BOOL has_sca = SGE_FALSE;
                                SGE_BOOL has_rot = SGE_FALSE;
                                if (section.parsed_data->sgerend->transformation_flags & SGE_SCENE_TRANSFORMATION_FLAG_POSITION) {
                                        has_pos = SGE_TRUE;
                                        total_size += SGE_TRANSFORMATION_SCALE_SIZE;
                                }
                                if (section.parsed_data->sgerend->transformation_flags & SGE_SCENE_TRANSFORMATION_FLAG_SCALE) {
                                        has_sca = SGE_TRUE;
                                        total_size += SGE_TRANSFORMATION_SCALE_SIZE;
                                }
                                if (section.parsed_data->sgerend->transformation_flags & SGE_SCENE_TRANSFORMATION_FLAG_ROTATION) {
                                        has_rot = SGE_TRUE;
                                        total_size += SGE_TRANSFORMATION_SCALE_SIZE;
                                }

                                vec3 position_transform, scale_transform, rotation_transform;
                                size_t transform_offset = 0;
                                //printf("TOTAL SIZE: %llu\n", total_size);
                                if (has_pos) {
                                        //printf("HAS POS\n");
                                        copy_memory(&position_transform, section.parsed_data->sgerend->transformation_data, SGE_TRANSFORMATION_POSITION_SIZE, 0, transform_offset);
                                        transform_offset += SGE_TRANSFORMATION_POSITION_SIZE;
                                }
                                if (has_sca) {
                                        //printf("HAS SCA\n");
                                        copy_memory(&scale_transform, section.parsed_data->sgerend->transformation_data, SGE_TRANSFORMATION_SCALE_SIZE, 0, transform_offset);
                                        transform_offset += SGE_TRANSFORMATION_SCALE_SIZE;
                                }
                                if (has_rot) {
                                        //printf("HAS ROT\n");
                                        //printf("TRANSFORMATI OFFSETI: %llu\n", transform_offset);
                                        copy_memory(&rotation_transform, section.parsed_data->sgerend->transformation_data, SGE_TRANSFORMATION_ROTATION_SIZE, 0, transform_offset);
                                        transform_offset += SGE_TRANSFORMATION_ROTATION_SIZE;
                                }

                                //printf("POS TRANSFORM; x: %f, y: %f, z: %f\n", position_transform.x, position_transform.y, position_transform.z);

                                m4 rotation_matrix;
                                m4 position_matrix;
                                m4 scale_matrix;
                                m4 transformation_matrix;
                                sge_m4_set_identity(transformation_matrix);
                                sge_m4_set_identity(rotation_matrix);
                                sge_m4_set_identity(position_matrix);
                                sge_m4_set_identity(scale_matrix);
                                if (has_rot) {
                                        //sge_vec3_print(rotation_transform);
                                        sge_m4_set_rotate(rotation_matrix, rotation_transform);
                                        //printf("ROTATION\n");
                                        //sge_m4_print(rotation_matrix);
                                }
                                if (has_pos) {
                                        sge_m4_set_translate(position_matrix, position_transform);
                                }
                                if (has_sca) {
                                        sge_m4_set_scale(scale_matrix, scale_transform);
                                }

                                m4 temp_matrix;
                                sge_m4_set_identity(temp_matrix);

                                if (has_rot) {
                                        sge_m4_multiply(temp_matrix, rotation_matrix, transformation_matrix);
                                        sge_m4_copy(transformation_matrix, temp_matrix);
                                        //printf("TRANS AFTER ROT\n");
                                        //sge_m4_print(transformation_matrix);
                                }
                                if (has_sca) {
                                        //printf("SCALE MATRIX\n");
                                        //sge_m4_print(scale_matrix);
                                        sge_m4_multiply(temp_matrix, scale_matrix, transformation_matrix);
                                        sge_m4_copy(transformation_matrix, temp_matrix);
                                        //printf("TRANS AFTER SCA\n");
                                        //sge_m4_print(transformation_matrix);
                                }
                                if (has_pos) {
                                        //printf("POSITION\n");
                                        //sge_m4_print(position_matrix);
                                        sge_m4_multiply(temp_matrix, position_matrix, transformation_matrix);
                                        sge_m4_copy(transformation_matrix, temp_matrix);
                                        //printf("TRANS AFTER POS\n");
                                        //sge_m4_print(transformation_matrix);
                                }

                                //printf("Transformation Matrix\n");
                                //sge_m4_print(transformation_matrix);
                                //printf("\n");
                                uint32_t vertex_size = rend->mesh->vertex_size;
                                SGE_FORMAT_TYPE position_format = 0;
                                uint16_t position_components = 0;
                                uint16_t position_offset = 0;
                                // size of 16
                                // offset 0
                                // comp 3
                                // fomrat float 32
                                // 12 size pos attr at offset 0 nächstes dann bei 16 usw
                                // 0-3 pos 1
                                // 4-7 pos 2
                                // 8-11 pos 3
                                // 12-15 irgendwas anderes uninterressant
                                SGE_BOOL format_found = SGE_FALSE;
                                for (int j = 0; j < rend->mesh->attribute_count; ++j) {
                                        sge_mesh_attribute attribute = rend->mesh->attributes[j];
                                        if (attribute.type != SGE_ATTRIBUTE_POSITION) {
                                                continue;
                                        }
                                        position_format = attribute.format;
                                        position_components = attribute.components;
                                        position_offset = attribute.offset;
                                        format_found = SGE_TRUE;
                                        break;
                                }
                                if (!format_found) {
                                        log_event(LOG_LEVEL_ERROR, "Failed to find rend format in \"%s\"", function_name);
                                        continue;
                                }
                                uint8_t *vertex_data = (uint8_t *)rend->mesh->vertex_buffer.data;
                                for (int j = 0; j < rend->mesh->vertex_count; ++j) {
                                        uint8_t *vertex_start = vertex_data + (j * vertex_size);
                                        uint8_t *position_start = vertex_start + position_offset;

                                        vec4 position = { .x = 0, .y = 0, .z = 0, .a = 1.0f };

                                        switch (position_format) {
                                                case SGE_FORMAT_FLOAT32: {
                                                        for (uint16_t k = 0; k < position_components; ++k) {
                                                                float value = *((float*)(position_start + k * sizeof(float)));
                                                                switch (k) {
                                                                        case 0: position.x = value; break;
                                                                        case 1: position.y = value; break;
                                                                        case 2: position.z = value; break;
                                                                        case 3: position.a = value; break;
                                                                        default: log_event(LOG_LEVEL_ERROR, "unknown position component \"%s\"", function_name);
                                                                }
                                                        }
                                                }break;
                                                case SGE_FORMAT_FLOAT16:break;
                                                case SGE_FORMAT_INT8:break;
                                                case SGE_FORMAT_UINT8:break;
                                                case SGE_FORMAT_INT16:break;
                                                case SGE_FORMAT_UINT16:break;
                                                case SGE_FORMAT_INT32:break;
                                                case SGE_FORMAT_UINT32:break;
                                        }

                                        vec4 transformed = sge_m4_transform_vec4(transformation_matrix, position);
                                        //sge_vec4_print(transformed);


                                        switch (position_format) {
                                                case SGE_FORMAT_FLOAT32: {
                                                        for (uint16_t k = 0; k < position_components; ++k) {
                                                                float* position_dst = (float*)(position_start + k * sizeof(float));
                                                                switch (k) {
                                                                        case 0: *position_dst = transformed.x; break;
                                                                        case 1: *position_dst = transformed.y; break;
                                                                        case 2: *position_dst = transformed.z; break;
                                                                        case 3: *position_dst = transformed.a; break;
                                                                        default: log_event(LOG_LEVEL_ERROR, "unknown position component \"%s\"", function_name);
                                                                }
                                                        }
                                                }break;
                                                case SGE_FORMAT_FLOAT16:break;
                                                case SGE_FORMAT_INT8:break;
                                                case SGE_FORMAT_UINT8:break;
                                                case SGE_FORMAT_INT16:break;
                                                case SGE_FORMAT_UINT16:break;
                                                case SGE_FORMAT_INT32:break;
                                                case SGE_FORMAT_UINT32:break;
                                        }
                                }
                                sge_renderable_update_api_resources(render, rend);
                                sge_region_add_renderable(region, rend);
                        }
                }
        }
}


SGE_RESULT sge_region_resize_auto_resizing_regions(sge_render *render, float old_width, float old_height, float new_width, float new_height) {
        if (new_height == 0 ||new_width == 0) {
                return SGE_SUCCESS;
        }
        float resize_factor_width = new_width / old_width;
        float resize_factor_height = new_height / old_height;
        //printf("%f, %f", resize_factor_width, resize_factor_height);

        for (int i = 0; i < render->region_count; ++i) {
                sge_region *region = render->regions[i];
                if (region->auto_scale_on_resize) {
                        if (region->viewport->width == SGE_REGION_FULL_DIMENSION) {
                                region->viewport->width == new_width;
                                region->scissor->extent_width = new_width / 2;
                        } else {
                                region->viewport->width *= resize_factor_width;
                                region->scissor->extent_width *= resize_factor_width;
                        }
                        if (region->viewport->height == SGE_REGION_FULL_DIMENSION) {
                                region->viewport->height == new_height;
                                region->scissor->extent_height = new_height;
                        } else {
                                region->viewport->height *= resize_factor_height;
                                region->scissor->extent_height *= resize_factor_height;
                        }
                }

                if (region->reposition_on_resize) {

                        region->viewport->x *= resize_factor_width;
                        region->scissor->offset_x *= resize_factor_width;

                        if (region->viewport->y == 0) {
                                region->viewport->y *= resize_factor_height;
                                region->scissor->offset_y *= resize_factor_height;
                        }
                        else {
                                float old_bottom_offset = old_height - region->viewport->y;
                                float new_bottom_offset = old_bottom_offset * resize_factor_height;
                                region->viewport->y = new_height - new_bottom_offset;

                                old_bottom_offset = old_height - region->scissor->offset_y;
                                new_bottom_offset = old_bottom_offset * resize_factor_height;
                                region->scissor->offset_y = new_height - new_bottom_offset;
                        }

                }


        }

        return SGE_SUCCESS;
}

sge_region *sge_region_get_active(sge_render *render) {
        mouse_pos current_mouse_pos;
        if (is_hidden) {
                current_mouse_pos = last_visible_pos;
        } else {
                current_mouse_pos = get_mouse_position();
        }

        current_mouse_pos = get_window_mouse_pos(render, current_mouse_pos);

        sge_region *active_region = NULL;
        int max_z_index = INT_MIN;

        for (int i = 0; i < render->region_count; ++i) {
                sge_region *region = render->regions[i];

                int left = region->viewport->x;
                int top = region->viewport->y;
                int right = (region->viewport->width == -1000) ? render->window->width : region->viewport->width + left;
                int bottom = (region->viewport->height == -1000) ? render->window->height : region->viewport->height + top;

                //printf("TOP: %i, LEFT: %i, RIGHT: %i, BOTTOM: %i\nX: %i, Y: %i\n\n", top, left, right, bottom, current_mouse_pos.x, current_mouse_pos.y);

                if (current_mouse_pos.x >= left
                &&  current_mouse_pos.x <= right
                &&  current_mouse_pos.y >= top
                &&  current_mouse_pos.y <= bottom) {
                        if (region->z_index > max_z_index) {
                                active_region = region;
                                max_z_index = region->z_index;
                        }
                }
        }
        return active_region;

}

int compare_z_index(const void *a, const void *b) {
        const sge_region *region_a = *(const sge_region **)a;
        const sge_region *region_b = *(const sge_region **)b;
        if (region_a == NULL && region_b == NULL) {
                return 0;
        }
        if (region_a == NULL) {
                return 1;
        }
        if (region_b == NULL) {
                return -1;
        }
        return region_b->z_index - region_a->z_index;
}

sge_region **sge_region_get_active_list(sge_render *render, int *regions_count) {
        if (render == NULL) {
                return NULL;
        }
        mouse_pos current_mouse_pos;
        sge_region **regions = allocate_memory(sizeof(sge_region*) * render->region_count, MEMORY_TAG_REGION);
        if (regions == NULL) {
                log_internal_event(LOG_LEVEL_FATAL, "Failed to allocate for region list");
                return NULL;
        }
        if (is_hidden) {
                current_mouse_pos = last_visible_pos;
        } else {
                current_mouse_pos = get_mouse_position();
        }

        current_mouse_pos = get_window_mouse_pos(render, current_mouse_pos);

        *regions_count = 0;

        for (int i = 0; i < render->region_count; ++i) {
                sge_region *region = render->regions[i];

                int left = region->viewport->x;
                int top = region->viewport->y;
                int right = (region->viewport->width == SGE_REGION_FULL_DIMENSION) ? render->window->width : region->viewport->width + left;
                int bottom = (region->viewport->height == SGE_REGION_FULL_DIMENSION) ? render->window->height : region->viewport->height + top;

                //printf("TOP: %i, LEFT: %i, RIGHT: %i, BOTTOM: %i\nX: %i, Y: %i\n\n", top, left, right, bottom, current_mouse_pos.x, current_mouse_pos.y);

                if (current_mouse_pos.x >= left
                &&  current_mouse_pos.x <= right
                &&  current_mouse_pos.y >= top
                &&  current_mouse_pos.y <= bottom) {
                        regions[*regions_count] = region;
                        (*regions_count)++;
                }
        }


        if (*regions_count <= 0) {
                return NULL;
        }

        if (*regions_count > 1) {
                qsort(regions, *regions_count, sizeof(sge_region*), compare_z_index);
        }

        return regions;
}

float sge_region_get_height(sge_region *region, sge_render *render) {
        if (region->viewport->height == SGE_REGION_FULL_DIMENSION) {
                return render->window->height;
        }
        return region->viewport->height;
}
float sge_region_get_width(sge_region *region, sge_render *render) {
        if (region->viewport->width == SGE_REGION_FULL_DIMENSION) {
                return render->window->width;
        }
        return region->viewport->width;
}
