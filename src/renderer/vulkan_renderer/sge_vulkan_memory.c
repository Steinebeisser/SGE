//
// Created by Geisthardt on 31.03.2025.
//

#include "sge_vulkan_memory.h"

#include <core/logging.h>

#include "vulkan_structs.h"
#include "core/sge_internal_logging.h"


uint32_t find_memory_type(sge_render *render, uint32_t type_filter, VkMemoryPropertyFlags properties) {
        sge_vulkan_context *vk_context = render->api_context;
        VkPhysicalDeviceMemoryProperties memory_properties;
        vkGetPhysicalDeviceMemoryProperties(vk_context->physical_device, &memory_properties);

        for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++) {
                if ((type_filter & (1 << i)) &&
                    (memory_properties.memoryTypes[i].propertyFlags & properties) == properties) {
                        return i;
                    }
        }

        log_internal_event(LOG_LEVEL_ERROR, "Failed to find suitable memory type");
        return UINT32_MAX;
}
