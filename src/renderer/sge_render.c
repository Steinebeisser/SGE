//
// Created by Geisthardt on 06.03.2025.
//

#include "renderer/sge_render.h"

#include "core/memory_control.h"
#include "core/sge_internal_logging.h"
#include "vulkan_renderer/vulkan_renderer.h"
#include "../src/renderer/sge_internal_render.h"


SGE_RESULT sge_renderable_create_api_resources(sge_render *render, sge_renderable *renderable);

sge_render *sge_render_create(SGE_RENDER_API api, sge_window *window) {
        sge_render *renderer = allocate_memory(sizeof(sge_render), MEMORY_TAG_RENDERER);
        if (renderer == NULL) {
                log_internal_event(LOG_LEVEL_FATAL, "Failed to allocate memory for renderer");
                return NULL;
        }

        renderer->api = api;
        renderer->api_context = NULL;
        renderer->window = window;
        renderer->sge_interface = allocate_memory(sizeof(sge_renderer_interface), MEMORY_TAG_RENDERER);
        if (renderer->sge_interface == NULL) {
                log_internal_event(LOG_LEVEL_FATAL, "failed allocate memory for sge interface");
                return NULL;
        }

        switch (api) {
                case SGE_RENDER_API_VULKAN: {
                        renderer->sge_interface = &sge_vulkan_interface;
                        renderer->api_context = allocate_memory(sizeof(sge_vulkan_context), MEMORY_TAG_VULKAN);
                        if (renderer->api_context == NULL) {
                                log_internal_event(LOG_LEVEL_FATAL, "Failed allocate vulkan api context");
                                return NULL;
                        }
                } break;
                case SGE_RENDER_API_OPENGL: {
                        log_internal_event(LOG_LEVEL_FATAL, "OpenGL not yet implemented");
                        return NULL;
                } break;
                case SGE_RENDER_API_DIRECTX: {
                        log_internal_event(LOG_LEVEL_FATAL, "DirectX not yet implemented");
                        return NULL;
                } break;
                default: {
                        log_internal_event(LOG_LEVEL_FATAL, "unsupported api");
                        return NULL;
                }
        }
        return renderer;
}

SGE_RESULT sge_render_initialize(sge_render *render, sge_render_settings *render_settings) {
        if (!render->sge_interface->initialize(render, render_settings)) {
             return SGE_ERROR;
        }

        log_internal_event(LOG_LEVEL_INFO, "FINISHED RENDER INITIALIZATION");

        return SGE_SUCCESS;
}

SGE_RESULT sge_draw_frame(sge_render *render) {
        sge_update_uniform_buffer(render, NULL);
        SGE_RESULT draw_result = render->sge_interface->draw(render);

        if (draw_result == SGE_SUCCESS) return SGE_SUCCESS;
        if (draw_result == SGE_RESIZE) return SGE_RESIZE;

        return SGE_ERROR;
}

SGE_RESULT sge_create_buffer(sge_render *render, void **buffer_ptr) {
        if (render->sge_interface->create_buffer(render, buffer_ptr) != SGE_SUCCESS) {
                return SGE_ERROR;
        }

        return SGE_SUCCESS;
}

SGE_RESULT sge_allocate_buffer(sge_render *render, void **memory_ptr, void *buffer) {
        if (render->sge_interface->allocate_buffer_memory(render, memory_ptr, buffer) != SGE_SUCCESS) {
                return SGE_ERROR;
        }

        return SGE_SUCCESS;
}

SGE_RESULT sge_create_descriptor_pool(sge_render *render, void *pool_ptr) {
        if (render->sge_interface->create_descriptor_pool(render, pool_ptr) != SGE_SUCCESS) {
                return SGE_ERROR;
        }

        return SGE_SUCCESS;
}

SGE_RESULT sge_allocate_descriptor_set(sge_render *render, void *descriptor_ptr, void *layout_ptr, void *descriptor_pool) {
        if (render->sge_interface->allocate_descriptor_set(render, descriptor_ptr, layout_ptr, descriptor_pool) != SGE_SUCCESS) {
                return SGE_ERROR;
        }

        return SGE_SUCCESS;
}

SGE_RESULT sge_update_descriptor_set(sge_render *render, sge_uniform_buffer_type *buffer) {
        if (render->sge_interface->update_descriptor_set(render, buffer) != SGE_SUCCESS) {
                return SGE_ERROR;
        }
        return SGE_SUCCESS;
}
//todo dynamic array
//SGE_RESULT sge_add_renderable(sge_render *render, sge_renderable *renderable) {
//        render->sge_renderables = *renderable;
//        render->sge_renderables_count = 1;


//        return SGE_SUCCESS;
//}

SGE_RESULT sge_renderable_create_api_resources(sge_render *render, sge_renderable *renderable) {
        if (render->sge_interface->create_renderable_resources(render, renderable) != SGE_SUCCESS) {
                return SGE_ERROR;
        }
        return SGE_SUCCESS;
}

sge_renderable *create_renderable_from_rend_file(sge_render *render, sge_rend_file *file) {
        if (file == NULL) {
                log_internal_event(LOG_LEVEL_FATAL, "tried to create renderable from file without passing file");
                return NULL;
        }

        sge_renderable *renderable = allocate_memory(sizeof(sge_renderable), MEMORY_TAG_RENDERER);
        if (renderable == NULL) {
                log_internal_event(LOG_LEVEL_FATAL, "Failed to allocate for renderable");
                return NULL;
        }

        bool has_mesh = false;


        for (int i = 0; i < file->header.section_count; ++i) {
                sge_rend_section *section = &file->sections[i];

                switch (section->section_header.type) {
                        case SGE_SECTION_MESH: {
                                sge_mesh_data *mesh_data = sge_parse_mesh_data(section->data, section->section_header.data_size);
                                if (mesh_data == NULL) {
                                        log_internal_event(LOG_LEVEL_FATAL, "failed to parse mesh data");
                                        return NULL;
                                }

                                renderable->mesh = allocate_memory(sizeof(sge_mesh), MEMORY_TAG_RENDERER);
                                if (renderable->mesh == NULL) {
                                        log_internal_event(LOG_LEVEL_FATAL, "failed allocate renderable mesh");
                                        return NULL;
                                }

                                //NAME
                                log_internal_event(LOG_LEVEL_DEBUG, "copying name");
                                copy_memory(renderable->mesh->name, section->section_header.name, sizeof(renderable->mesh->name), 0, 0);
                                log_internal_event(LOG_LEVEL_DEBUG, "finished copying name");

                                //MESH
                                renderable->mesh->vertex_count = mesh_data->vertex_count;
                                renderable->mesh->vertex_size = mesh_data->vertex_size;
                                renderable->mesh->attribute_count = mesh_data->attribute_count;

                                renderable->mesh->attributes = allocate_memory(sizeof(sge_mesh_attribute) * renderable->mesh->attribute_count, MEMORY_TAG_RENDERER);

                                if (renderable->mesh->attributes == NULL) {
                                        log_internal_event(LOG_LEVEL_FATAL, "failed to allocate for attributes or no attributes specified, atleast position required");
                                        return NULL;
                                }

                                log_internal_event(LOG_LEVEL_DEBUG, "copying attributes");
                                copy_memory(renderable->mesh->attributes, mesh_data->attributes, sizeof(sge_mesh_attribute) * renderable->mesh->attribute_count, 0, 0);
                                log_internal_event(LOG_LEVEL_DEBUG, "finished copying attributes");

                                renderable->mesh->vertex_buffer.size = mesh_data->vertex_count * mesh_data->vertex_size;
                                renderable->mesh->vertex_buffer.data = allocate_memory(renderable->mesh->vertex_buffer.size, MEMORY_TAG_RENDERER);

                                if (renderable->mesh->vertex_buffer.data == NULL) {
                                        log_internal_event(LOG_LEVEL_FATAL, "failed to allocate vertex buffer data");
                                        return NULL;
                                }

                                log_internal_event(LOG_LEVEL_DEBUG, "copying vertex data");

                                log_internal_event(LOG_LEVEL_DEBUG,"DEST PTR: %p", renderable->mesh->vertex_buffer.data);
                                log_internal_event(LOG_LEVEL_DEBUG,"SRC PTR: %p", mesh_data->vertex_data);
                                copy_memory(renderable->mesh->vertex_buffer.data, mesh_data->vertex_data, renderable->mesh->vertex_buffer.size, 0, 0);
                                log_internal_event(LOG_LEVEL_DEBUG, "finished copying vertex data");

                                sge_vertex_attribute *attributes = allocate_memory(
                                    sizeof(sge_vertex_attribute) * renderable->mesh->attribute_count,
                                    MEMORY_TAG_RENDERER
                                );

                                if (attributes == NULL) {
                                    log_internal_event(LOG_LEVEL_FATAL, "failed to allocate vertex attributes");
                                    return NULL;
                                }

                                has_mesh = true;
                        } break;
                        case SGE_SECTION_MATERIAL: {


                        } break;
                }
        }

        if (!has_mesh) {
                log_internal_event(LOG_LEVEL_FATAL, "no mesh input");
                return NULL;
        }






        //if (mesh_section == NULL) {
        //        log_internal_event(LOG_LEVEL_FATAL, "tried to load renderable without mesh");
        //        return NULL;
        //}
//
        //renderable->mesh = allocate_memory(sizeof(sge_mesh), MEMORY_TAG_RENDERER);
        //if (renderable->mesh == NULL) {
        //        log_internal_event(LOG_LEVEL_FATAL, "failed to allocate mesh");
        //        return NULL;
        //}
//
        //copy_memory(renderable->mesh->name, mesh_section->section_header.name, sizeof(renderable->mesh->name), 0, 0);
//
        //renderable->mesh->vertex_count = mesh_section->section_header.


        log_internal_event(LOG_LEVEL_INFO, "creating renderable api resources");
        sge_renderable_create_api_resources(render, renderable);
        log_internal_event(LOG_LEVEL_INFO, "finished creating renderable api resources");

        return renderable;
}


//SGE_RESULT sge_render_initializess(sge_render *renderer, render_settings *render_settings) {
//        if (!renderer) {
//                log_internal_event(LOG_LEVEL_FATAL, "Tried to init renderer without passing sge_render struct");
//                return SGE_ERROR;
//        }
//
//        switch (renderer->api) {
//                case SGE_RENDER_API_VULKAN: {
//                        vulkan_context *vk_context = allocate_memory(sizeof(vulkan_context), MEMORY_TAG_VULKAN);
//                        if (vk_context == NULL) {
//                                log_internal_event(LOG_LEVEL_FATAL, "Failed to allocate memory for VULKAN context");
//                                return SGE_ERROR;
//                        }
//
//                        renderer->api_context = vk_context;
//
//                        if (initialize_vk(vk_context, renderer->window, "SGE Engine", VK_MAKE_API_VERSION(0, 0, 1, 0)) != 0) {
//                                free_memory(vk_context, MEMORY_TAG_VULKAN);
//                                renderer->api_context = NULL;
//                                return SGE_ERROR;
//                        }
//                } break;
//                case SGE_RENDER_API_OPENGL: {
//
//                } break;
//                case SGE_RENDER_API_DIRECTX: {
//
//                } break;
//                default: {
//                        return SGE_ERROR;
//                }
//        }
//        return SGE_SUCCESS;
//}
//
//SGE_RESULT sge_render_shutdown(sge_render *render) {
//        switch (render->api) {
//                case SGE_RENDER_API_VULKAN: {
//                        if (!shutdown_vulkan(render->api_context)) {
//                             return SGE_ERROR;
//                        }
//                } break;
//                case SGE_RENDER_API_OPENGL: {
//
//                } break;
//                case SGE_RENDER_API_DIRECTX: {
//
//                } break;
//                default: {
//                        return SGE_ERROR;
//                }
//        }
//        return SGE_SUCCESS;
//}
//
//SGE_RESULT sge_render_initialize(sge_render *renderer, render_settings *render_settings) {
//        if (!renderer || !renderer->sge_interface || !renderer->sge_interface->initialize) {
//                log_internal_event(LOG_LEVEL_FATAL, "Renderer or its interface is not initialized");
//                return SGE_ERROR;
//        }
//        return renderer->sge_interface->initialize(renderer->api_context, renderer->window, render_settings);
//}