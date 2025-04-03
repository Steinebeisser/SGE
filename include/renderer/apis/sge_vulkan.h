//
// Created by Geisthardt on 31.03.2025.
//

#ifndef SGE_VULKAN_H
#define SGE_VULKAN_H
#include <stdint.h>

typedef struct vulkan_app_info {
        char            *application_name;
        uint32_t        application_version;
} vulkan_app_info;

typedef struct sge_vulkan_render_settings {
        SGE_BOOL                enable_validation_layers;
        SGE_BOOL                use_dynamic_rendering;
        SGE_BOOL                use_sge_allocator;
        vulkan_app_info         app_info;
} sge_vulkan_render_settings;

#endif //SGE_VULKAN_H
