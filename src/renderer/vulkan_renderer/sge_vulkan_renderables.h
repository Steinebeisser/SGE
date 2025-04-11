//
// Created by Geisthardt on 19.03.2025.
//

#ifndef SGE_VULKAN_RENDERABLES_H
#define SGE_VULKAN_RENDERABLES_H

#include "renderer/sge_render.h"

SGE_RESULT sge_vulkan_create_renderable_resources(sge_render *render, sge_renderable *renderable);
SGE_RESULT sge_vulkan_update_renderable_resources(sge_render *render, sge_renderable *renderable);
SGE_BOOL compare_formats(sge_vertex_format *a, sge_vertex_format *b);
SGE_RESULT sge_vulkan_convert_sge_format_to_vulkan_format(sge_renderable *renderable);

#endif //SGE_VULKAN_RENDERABLES_H
