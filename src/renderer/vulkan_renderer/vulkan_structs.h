//
// Created by Geisthardt on 31.03.2025.
//

#ifndef VULKAN_STRUCTS_H
#define VULKAN_STRUCTS_H

#include <renderer/sge_render.h>


#include "vulkan/vulkan.h"

typedef struct swapchain_infos {
        VkImage                         *sc_images;
        VkImageView                     *color_views;
        uint32_t                        sc_img_count;

        VkSurfaceFormatKHR              surface_format;
        VkSurfaceCapabilitiesKHR        surface_capabilities;
        VkFramebuffer                   framebuffer;

        VkFormat                        depth_format;
        VkImage                         *depth_images;
        VkImageView                     *depth_views;
        VkDeviceMemory                  *depth_memories;
} swapchain_infos;

typedef enum sge_render_mode {
        SGE_RENDER_EASY,
        SGE_RENDER_ADVANCED
} sge_render_mode;



typedef struct sync_objects {
        VkSemaphore                     *image_available_semaphores, *render_finished_semaphores;
        VkFence                         *in_flight_fences;
        uint32_t                        current_frame;
        uint32_t                        max_frames_in_flight;
} sync_objects;

typedef struct sge_vulkan_pipelines {
        VkPipeline              pipeline;
        VkPipelineLayout        pipeline_layout;
        sge_vertex_format       format;
} sge_vulkan_pipelines;

typedef struct sge_vulkan_context {
        VkInstance                      instance;
        VkSurfaceKHR                    surface;
        VkPhysicalDevice                physical_device;
        VkPhysicalDeviceProperties      physical_device_properties;
        VkDevice                        device;
        VkSwapchainKHR                  swapchain;

        swapchain_infos                 sc;

        sync_objects                    so;
        VkCommandPool                   command_pool;
        VkCommandBuffer                 *command_buffer;
        uint32_t                        command_buffer_count;
        uint32_t                        command_buffer_index;
        VkRenderPass                    render_pass;
        uint32_t                        graphics_queue_family_index;

        VkQueue                         graphics_queue;
        VkQueue                         present_queue;
        VkQueue                         transfer_queue;

        sge_vulkan_pipelines            *pipelines;
        uint32_t                        pipeline_count;
        VkPipeline                      pipeline;
        VkPipelineLayout                pipeline_layout;

        VkAllocationCallbacks           *sge_allocator;

        VkDescriptorPool                descriptor_pool;
        VkDescriptorSet                 descriptor_set[3];
        VkDescriptorSetLayout           descriptor_set_layout;
        VkBuffer                        uniform_buffer[3];
        VkDeviceMemory                  uniform_buffer_memory[3];
} sge_vulkan_context;

typedef struct sge_vulkan_pipeline_settings {
        VkPrimitiveTopology     topology;
        uint32_t                patch_control_points;

        VkPolygonMode           polygon_mode;
        float                   line_width;

        VkCullModeFlags         cull_mode;
        VkFrontFace             front_face;

        VkSampleCountFlagBits   msaa_sample_count;
        VkBool32                per_sample_shading_enabled;
        float                   min_sample_shading;

        sge_vertex_format *vertex_format;

        VkBool32                tesselation_enabled;
        VkBool32                geometry_enabled;

        VkBool32                is_3d;

} sge_vulkan_pipeline_settings;

#endif //VULKAN_STRUCTS_H
