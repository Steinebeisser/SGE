//
// Created by Geisthardt on 07.03.2025.
//

#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H

#include <stdint.h>




#include "renderer/sge_render.h"
#include <vulkan/vulkan_core.h>
#include "sge_vulkan_instance.h"
#include "sge_vulkan_surface.h"
#include "sge_vulkan_device.h"
#include "sge_vulkan_swapchain.h"
#include "sge_vulkan_command.h"
#include "sge_vulkan_sync.h"
#include "sge_vulkan_allocator.h"
#include <vulkan/vulkan_win32.h>
#include "sge_vulkan_buffer.h"
#include "sge_vulkan_draw.h"
#include "sge_vulkan_pipeline.h"
#include "sge_vulkan_descriptor.h"
#include "sge_vulkan_uniform.h"
#include "sge_vulkan_renderables.h"






SGE_RESULT sge_vulkan_initialize(sge_render *render, sge_render_settings *render_settings);
SGE_RESULT sge_vulkan_shutdown(sge_render *render);
//SGE_RESULT sge_vulkan_begin_frame(sge_render *render);
//SGE_RESULT sge_vulkan_draw_frame(sge_render *render, sge_mesh *mesh);
//SGE_RESULT sge_vulkan_end_frame(sge_render *render);
SGE_RESULT sge_set_pipeline(sge_render *render, sge_pipeline *pipeline);



extern sge_renderer_interface sge_vulkan_interface;

#endif //VULKAN_RENDERER_H
