//
// Created by Geisthardt on 31.03.2025.
//

#ifndef SGE_VULKAN_MEMORY_H
#define SGE_VULKAN_MEMORY_H
#include <stdint.h>
#include <renderer/sge_render.h>
#include <vulkan/vulkan_core.h>

uint32_t find_memory_type(sge_render *render, uint32_t type_filter, VkMemoryPropertyFlags properties);

#endif //SGE_VULKAN_MEMORY_H
