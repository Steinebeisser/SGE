//
// Created by Geisthardt on 19.03.2025.
//

#include "sge_vulkan_renderables.h"

#include "../../core/logging.h"
#include "../../core/memory_control.h"

SGE_RESULT sge_vulkan_create_renderable_resources(sge_render *render, sge_renderable *renderable) {
        if (render == NULL ||renderable == NULL || renderable->mesh == NULL) {
                log_event(LOG_LEVEL_ERROR, "tried to creating renderable without initializing render or a valid renderable");
                return SGE_ERROR;
        }


        sge_vulkan_context *vk_context = render->api_context;
        sge_mesh *mesh = renderable->mesh;

        if (!mesh->vertex_buffer.data) {
                log_event(LOG_LEVEL_ERROR, "no vertex buiffer data passed for renderable creation");
                return SGE_ERROR;
        }

        sge_vulkan_convert_sge_format_to_vulkan_format(renderable);

        VkBufferCreateInfo vertex_buffer_create_info = {
                .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                .size = mesh->vertex_buffer.size,
                .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                .sharingMode = VK_SHARING_MODE_EXCLUSIVE
        };

        VkBuffer vertex_buffer;
        VkResult vertex_buffer_result = vkCreateBuffer(vk_context->device, &vertex_buffer_create_info, vk_context->sge_allocator, &vertex_buffer);
        if (vertex_buffer_result != VK_SUCCESS) {
                log_event(LOG_LEVEL_ERROR, "Failed to create vertex buffer");
                return SGE_ERROR;
        }

        VkMemoryRequirements vertex_mem_reqs;
        vkGetBufferMemoryRequirements(vk_context->device, vertex_buffer, &vertex_mem_reqs);

        VkMemoryAllocateInfo vertex_alloc_info = {
                    .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                    .allocationSize = vertex_mem_reqs.size,
                    .memoryTypeIndex = find_memory_type(render, vertex_mem_reqs.memoryTypeBits,
                                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
        };

        VkDeviceMemory vertex_memory;
        if (vkAllocateMemory(vk_context->device, &vertex_alloc_info,
                            vk_context->sge_allocator, &vertex_memory) != VK_SUCCESS) {
            vkDestroyBuffer(vk_context->device, vertex_buffer, vk_context->sge_allocator);
            log_event(LOG_LEVEL_ERROR, "Failed to allocate vertex buffer memory");
            return SGE_ERROR;
        }

        if (vkBindBufferMemory(vk_context->device, vertex_buffer, vertex_memory, 0) != VK_SUCCESS) {
            vkFreeMemory(vk_context->device, vertex_memory, vk_context->sge_allocator);
            vkDestroyBuffer(vk_context->device, vertex_buffer, vk_context->sge_allocator);
            log_event(LOG_LEVEL_ERROR, "Failed to bind vertex buffer memory");
            return SGE_ERROR;
        }

        void *mapped_data;
        if (vkMapMemory(vk_context->device, vertex_memory, 0,
                      mesh->vertex_buffer.size, 0, &mapped_data) != VK_SUCCESS) {
            vkFreeMemory(vk_context->device, vertex_memory, vk_context->sge_allocator);
            vkDestroyBuffer(vk_context->device, vertex_buffer, vk_context->sge_allocator);
            log_event(LOG_LEVEL_ERROR, "Failed to map vertex buffer memory");
            return SGE_ERROR;
        }

        memcpy(mapped_data, mesh->vertex_buffer.data, mesh->vertex_buffer.size);

        vkUnmapMemory(vk_context->device, vertex_memory);

        mesh->vertex_buffer.api_handle = vertex_buffer;

        if (mesh->format) {
                VkPipeline renderable_pipeline = VK_NULL_HANDLE;
                VkPipelineLayout renderable_pipeline_layout = VK_NULL_HANDLE;
                //check for exisitn pipelines based on format

                for (int i = 0; i < vk_context->pipeline_count; ++i) {
                        sge_vulkan_pipelines pipeline = vk_context->pipelines[i];

                        if (compare_formats(&pipeline.format, mesh->format) == SGE_TRUE) {
                                renderable_pipeline = pipeline.pipeline;
                                renderable_pipeline_layout = pipeline.pipeline_layout;

                        }
                }


                if (renderable_pipeline == VK_NULL_HANDLE) {
                        log_event(LOG_LEVEL_INFO, "Need to create new pipeline");

                        VkPipeline pipeline;
                        VkPipelineLayout pipeline_layout;

                        sge_pipeline_settings settings = {0};

                        settings = (sge_pipeline_settings) {
                                .cull_mode = SGE_CULL_MODE_NONE,
                                .front_face = SGE_FRONT_FACE_COUNTER_CLOCKWISE,
                                .is_3d = SGE_TRUE,
                                .polygon_mode = SGE_POLYGON_MODE_FILL
                        };

                        sge_vulkan_pipeline_settings *vulkan_settings = transform_pipeline_settings_to_vulkan_specific(&settings);

                        log_event(LOG_LEVEL_INFO, "creating pipeline with settings");
                        if (sge_vulkan_pipeline_create_specific_format(
                                render,
                                mesh->format,
                                vulkan_settings,
                                &pipeline,
                                &pipeline_layout
                                ) != SGE_SUCCESS) {
                                log_event(LOG_LEVEL_ERROR, "failed to create pipeline");
                        }
                        log_event(LOG_LEVEL_INFO, "created vulkan pipeline");

                        renderable_pipeline = pipeline;
                        renderable_pipeline_layout = pipeline_layout;
                        //return SGE_ERROR;
                }


                renderable->pipeline = renderable_pipeline;
                renderable->pipeline_layout = renderable_pipeline_layout;
        } else {
                //idk
        }


        return SGE_SUCCESS;
}


SGE_BOOL compare_formats(sge_vertex_format *a, sge_vertex_format *b) {
        if (a->stride != b->stride) {
                return SGE_FALSE;
        }

        if (a->attribute_count != b->attribute_count) {
                return SGE_FALSE;
        }

        for (int i = 0; i < a->attribute_count; ++i) {
                sge_vertex_attribute *a_attribute = &a->attributes[i];

                for (int j = 0; j < b->attribute_count; ++j) {
                        sge_vertex_attribute *b_attribute = &b->attributes[i];

                        if (a_attribute->location != b_attribute->location) {
                                if (a_attribute->format != b_attribute->format ||
                                        a_attribute->components != b_attribute->components) {
                                        return SGE_FALSE;
                                }
                        }
                }
        }

        return SGE_TRUE;
}



SGE_RESULT sge_vulkan_convert_sge_format_to_vulkan_format(sge_renderable *renderable) {
        sge_vertex_format *format = allocate_memory(sizeof(sge_vertex_format), MEMORY_TAG_RENDERER);
        if (format == NULL) {
            log_event(LOG_LEVEL_FATAL, "failed to allocate vertex format");
            return SGE_ERROR;
        }

        format->stride = renderable->mesh->vertex_size;
        format->attribute_count = renderable->mesh->attribute_count;

        format->attributes = allocate_memory(sizeof(sge_vertex_attribute) * format->attribute_count, MEMORY_TAG_RENDERER);

        for (int i = 0; i < format->attribute_count; ++i) {
                sge_vertex_attribute *format_attribute = &format->attributes[i];
                SGE_MESH_ATTRIBUTE *mesh_attribute = &renderable->mesh->attributes[i];

                format_attribute->location = sge_get_location_from_attribute_type(mesh_attribute->type);
                format_attribute->offset = mesh_attribute->offset;
                format_attribute->components = mesh_attribute->components;

                //todo add rest
                switch (mesh_attribute->format) {
                        case SGE_FORMAT_FLOAT32: {
                                switch (mesh_attribute->components) {
                                        case 1: {
                                                format_attribute->format = VK_FORMAT_R32_SFLOAT;
                                        } break;
                                        case 2: {
                                                format_attribute->format = VK_FORMAT_R32G32_SFLOAT;
                                        } break;
                                        case 3: {
                                                format_attribute->format = VK_FORMAT_R32G32B32_SFLOAT;
                                        } break;
                                        case 4: {
                                                format_attribute->format = VK_FORMAT_R32G32B32A32_SFLOAT;
                                        } break;
                                        default: {
                                                format_attribute->format = VK_FORMAT_R32G32B32_SFLOAT;
                                        } break;
                                }
                        } break;
                        case SGE_FORMAT_UINT8: {
                                switch (mesh_attribute->components) {
                                        case 1: {
                                                format_attribute->format = VK_FORMAT_R8_UNORM;
                                        } break;
                                        case 2: {
                                                format_attribute->format = VK_FORMAT_R8G8_UNORM;
                                        } break;
                                        case 3: {
                                                format_attribute->format = VK_FORMAT_R8G8B8_UNORM;
                                        } break;
                                        case 4: {
                                                format_attribute->format = VK_FORMAT_R8G8B8A8_UNORM;
                                        } break;
                                        default: {
                                                format_attribute->format = VK_FORMAT_R8G8B8_UNORM;
                                        } break;
                                }
                        } break;
                        default: {
                                format_attribute->format = VK_FORMAT_R32G32B32_SFLOAT;
                        }
                }
        }

        renderable->mesh->format = format;

        return SGE_SUCCESS;
}
