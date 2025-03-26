//
// Created by Geisthardt on 07.03.2025.
//

#include "sge_vulkan_swapchain.h"
#include "../../core/logging.h"
#include "../../core/memory_control.h"


SGE_RESULT sge_vulkan_swapchain_create(sge_render *render) {
        sge_vulkan_context *vk_context = (sge_vulkan_context*)render->api_context;

        log_event(LOG_LEVEL_TRACE, "Creating swapchain");

        uint32_t old_width = vk_context->sc.surface_capabilities.currentExtent.width;
        uint32_t old_height = vk_context->sc.surface_capabilities.currentExtent.height;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_context->physical_device, vk_context->surface, &vk_context->sc.surface_capabilities);
        sge_region_resize_auto_resizing_regions(render, old_width, old_height, vk_context->sc.surface_capabilities.currentExtent.width, vk_context->sc.surface_capabilities.currentExtent.height);
        uint32_t format_count;
        vkGetPhysicalDeviceSurfaceFormatsKHR(vk_context->physical_device, vk_context->surface, &format_count, NULL);
        VkSurfaceFormatKHR *formats = allocate_memory(format_count * sizeof(VkSurfaceFormatKHR), MEMORY_TAG_VULKAN);
        vkGetPhysicalDeviceSurfaceFormatsKHR(vk_context->physical_device, vk_context->surface, &format_count, formats);
        vk_context->sc.surface_format = formats[0];
        free_memory(formats, MEMORY_TAG_VULKAN);

        VkSwapchainCreateInfoKHR create_info = {
                .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                .surface = vk_context->surface,
                .minImageCount = 3,
                .imageFormat = vk_context->sc.surface_format.format,
                .imageColorSpace = vk_context->sc.surface_format.colorSpace,
                .imageExtent = vk_context->sc.surface_capabilities.currentExtent,
                .imageArrayLayers = 1,
                .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                .preTransform = vk_context->sc.surface_capabilities.currentTransform,
                .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
                .presentMode = VK_PRESENT_MODE_MAILBOX_KHR,
                .clipped = VK_TRUE,
        };

        if (vkCreateSwapchainKHR(vk_context->device, &create_info, vk_context->sge_allocator, &vk_context->swapchain) != VK_SUCCESS) {
                log_event(LOG_LEVEL_FATAL, "failed to create swapchain");
                return SGE_ERROR;
        }

        vkGetSwapchainImagesKHR(vk_context->device, vk_context->swapchain, &vk_context->sc.sc_img_count, NULL);
        vk_context->sc.sc_images = allocate_memory(vk_context->sc.sc_img_count * sizeof(VkImage), MEMORY_TAG_VULKAN);
        vkGetSwapchainImagesKHR(vk_context->device, vk_context->swapchain, &vk_context->sc.sc_img_count, vk_context->sc.sc_images);

        vk_context->sc.color_views = allocate_memory(vk_context->sc.sc_img_count * sizeof(VkImageView), MEMORY_TAG_VULKAN);
        for (uint32_t i = 0; i < vk_context->sc.sc_img_count; i++) {
                VkImageViewCreateInfo view_info = {
                        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                        .image = vk_context->sc.sc_images[i],
                        .viewType = VK_IMAGE_VIEW_TYPE_2D,
                        .format = vk_context->sc.surface_format.format,
                        .subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                        .subresourceRange.levelCount = 1,
                        .subresourceRange.layerCount = 1,
                };
                if (vkCreateImageView(vk_context->device, &view_info, vk_context->sge_allocator, &vk_context->sc.color_views[i]) != VK_SUCCESS) {
                        log_event(LOG_LEVEL_FATAL, "failed creating image view");
                        return SGE_ERROR;
                }
        }

        vk_context->sc.depth_images = allocate_memory(vk_context->sc.sc_img_count * sizeof(VkImage), MEMORY_TAG_VULKAN);
        vk_context->sc.depth_views = allocate_memory(vk_context->sc.sc_img_count * sizeof(VkImageView), MEMORY_TAG_VULKAN);
        vk_context->sc.depth_memories = allocate_memory(vk_context->sc.sc_img_count * sizeof(VkDeviceMemory), MEMORY_TAG_VULKAN);

        for (uint32_t i = 0; i < vk_context->sc.sc_img_count; ++i) {
                VkImageCreateInfo depth_image_create_info = {
                        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                        .pNext = NULL,
                        .flags = 0,
                        .imageType = VK_IMAGE_TYPE_2D,
                        .format = vk_context->sc.depth_format,
                        .extent = {
                                .width = vk_context->sc.surface_capabilities.currentExtent.width,
                                .height = vk_context->sc.surface_capabilities.currentExtent.height,
                                .depth = 1,
                        },
                        .mipLevels = 1,
                        .arrayLayers = 1,
                        .samples = VK_SAMPLE_COUNT_1_BIT,
                        .tiling = VK_IMAGE_TILING_OPTIMAL,
                        .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                        .queueFamilyIndexCount = 0,
                        .pQueueFamilyIndices = 0,
                        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                };

                if (vkCreateImage(vk_context->device, &depth_image_create_info, vk_context->sge_allocator, &vk_context->sc.depth_images[i]) != VK_SUCCESS) {
                        log_event(LOG_LEVEL_ERROR, "Failed to create depth image %d", i);
                        return SGE_ERROR;
                }

                VkMemoryRequirements mem_requirements;
                vkGetImageMemoryRequirements(vk_context->device, vk_context->sc.depth_images[i], &mem_requirements);

                VkMemoryAllocateInfo alloc_info = {
                        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                        .allocationSize = mem_requirements.size,
                        .memoryTypeIndex = find_memory_type(render, mem_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
                    };

                if (vkAllocateMemory(vk_context->device, &alloc_info, vk_context->sge_allocator, &vk_context->sc.depth_memories[i]) != VK_SUCCESS) {
                        log_event(LOG_LEVEL_FATAL, "Failed to allocate depth image memory %d", i);
                        return SGE_ERROR;
                }

                vkBindImageMemory(vk_context->device, vk_context->sc.depth_images[i], vk_context->sc.depth_memories[i], 0);

                VkImageViewCreateInfo view_info = {
                        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                        .image = vk_context->sc.depth_images[i],
                        .viewType = VK_IMAGE_VIEW_TYPE_2D,
                        .format = vk_context->sc.depth_format,
                        .subresourceRange = {
                                .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
                                .baseMipLevel = 0,
                                .levelCount = 1,
                                .baseArrayLayer = 0,
                                .layerCount = 1
                            }
                };

                if (vkCreateImageView(vk_context->device, &view_info, vk_context->sge_allocator, &vk_context->sc.depth_views[i]) != VK_SUCCESS) {
                        log_event(LOG_LEVEL_FATAL, "Failed to create depth image view %d", i);
                        return SGE_ERROR;
                }

                log_event(LOG_LEVEL_INFO, "Created depth buffer %d", i);
        }
        log_event(LOG_LEVEL_TRACE, "Created swapchain");
        return SGE_SUCCESS;
}

SGE_RESULT sge_vulkan_swapchain_destroy(sge_render *render) {
        sge_vulkan_context *vk_context = (sge_vulkan_context*)render->api_context;

        vkDeviceWaitIdle(vk_context->device);

        for (uint32_t i = 0; i < vk_context->sc.sc_img_count; i++) {
                if (vk_context->sc.color_views[i] != VK_NULL_HANDLE) {
                        vkDestroyImageView(vk_context->device, vk_context->sc.color_views[i], vk_context->sge_allocator);
                }
        }
        free_memory(vk_context->sc.color_views, MEMORY_TAG_VULKAN);
        vk_context->sc.color_views = NULL;

        free_memory(vk_context->sc.sc_images, MEMORY_TAG_VULKAN);
        vk_context->sc.sc_images = NULL;

        if (vk_context->swapchain != VK_NULL_HANDLE) {
                vkDestroySwapchainKHR(vk_context->device, vk_context->swapchain, vk_context->sge_allocator);
                vk_context->swapchain = VK_NULL_HANDLE;
        }

        vk_context->sc.sc_img_count = 0;

        return SGE_SUCCESS;
}
