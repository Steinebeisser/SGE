//
// Created by Geisthardt on 19.05.2025.
//

#include "sge_vulkan_texture.h"

#include <core/memory_control.h>
#include <core/sge_internal_logging.h>
#include <vulkan/vulkan_core.h>

#include "sge_vulkan_buffer.h"
#include "sge_vulkan_memory.h"
#include "vulkan_structs.h"

VkCommandBuffer begin_single_time_commands(sge_vulkan_context *ctx) {
        VkCommandBufferAllocateInfo allocInfo = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                .commandPool = ctx->command_pool,
                .commandBufferCount = 1,
            };

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(ctx->device, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            };

        vkBeginCommandBuffer(commandBuffer, &beginInfo);
        return commandBuffer;
}

void end_single_time_commands(sge_vulkan_context *ctx, VkCommandBuffer commandBuffer) {
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo = {
                .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                .commandBufferCount = 1,
                .pCommandBuffers = &commandBuffer,
            };

        vkQueueSubmit(ctx->graphics_queue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(ctx->graphics_queue);

        vkFreeCommandBuffers(ctx->device, ctx->command_pool, 1, &commandBuffer);
}


void sge_vulkan_transition_image_layout(
    sge_vulkan_context *ctx,
    VkImage image,
    VkFormat format,
    VkImageLayout oldLayout,
    VkImageLayout newLayout
) {
        VkCommandBuffer cmd = begin_single_time_commands(ctx);

        VkImageMemoryBarrier barrier = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .oldLayout = oldLayout,
                .newLayout = newLayout,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .image = image,
                .subresourceRange = {
                        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                        .baseMipLevel = 0,
                        .levelCount = 1,
                        .baseArrayLayer = 0,
                        .layerCount = 1,
                    },
                };

        VkPipelineStageFlags srcStage;
        VkPipelineStageFlags dstStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
                barrier.srcAccessMask = 0;
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

                srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

        } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
                   newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

                   } else {
                           log_internal_event(LOG_LEVEL_ERROR, "Unsupported layout transition");
                           return;
                   }

        vkCmdPipelineBarrier(cmd,
                             srcStage, dstStage,
                             0,
                             0, NULL,
                             0, NULL,
                             1, &barrier);

        end_single_time_commands(ctx, cmd);
}

void sge_vulkan_copy_buffer_to_image(
    sge_vulkan_context *ctx,
    VkBuffer buffer,
    VkImage image,
    uint32_t width,
    uint32_t height
) {
        VkCommandBuffer cmd = begin_single_time_commands(ctx);

        VkBufferImageCopy region = {
                .bufferOffset = 0,
                .bufferRowLength = 0,
                .bufferImageHeight = 0,
                .imageSubresource = {
                        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                        .mipLevel = 0,
                        .baseArrayLayer = 0,
                        .layerCount = 1,
                    },
                    .imageOffset = {0, 0, 0},
                    .imageExtent = {
                        .width = width,
                        .height = height,
                        .depth = 1,
                    },
                };

        vkCmdCopyBufferToImage(cmd,
                               buffer,
                               image,
                               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                               1,
                               &region);

        end_single_time_commands(ctx, cmd);
}




SGE_RESULT sge_vulkan_create_render_texture(sge_render *render, sge_texture *texture, sge_render_texture **out_texture) {
        sge_vulkan_context *vk_context = render->api_context;

        if (texture->sections->header.section_type != SGE_TEXTURE_BODY_INCLUDE_TYPE_EMBEDDED) {
                log_internal_event(LOG_LEVEL_WARNING, "currently only embedded textures are supported");
                return SGE_UNSUPPORTED_SYSTEM;
        }

        uint32_t width = texture->sections->parsed_data.color_embedded->width;
        uint32_t height = texture->sections->parsed_data.color_embedded->height;
        uint8_t *src = texture->sections->parsed_data.color_embedded->pixel_data;

        if (!src) {
                log_internal_event(LOG_LEVEL_ERROR, "tried creating render texture but pixel data is NULL");
                return SGE_INVALID_API_CALL;
        }

        size_t imageSize = width * height * 4;

        uint8_t *rgba_data = allocate_memory(imageSize, MEMORY_TAG_IMAGE);
        if (!rgba_data) {
                log_internal_event(LOG_LEVEL_ERROR, "failed to allocate RGBA buffer");
                return SGE_ERROR;
        }

        for (uint32_t i = 0, j = 0; i < width * height * 3; i += 3, j += 4) {
                rgba_data[j + 0] = src[i + 0]; // R
                rgba_data[j + 1] = src[i + 1]; // G
                rgba_data[j + 2] = src[i + 2]; // B
                rgba_data[j + 3] = 255;        // A
        }

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        SGE_RESULT result = sge_vulkan_create_buffer(render, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                 &stagingBuffer, &stagingBufferMemory);

        if (result != SGE_SUCCESS) {
                free_memory(rgba_data, MEMORY_TAG_UNKNOWN);
                return result;
        }

        sge_parsed_section_data *parsed_data = &texture->sections->parsed_data;

        printf("DAT SIZE: %llu\n", texture->sections->parsed_data.color_embedded->data_size);
        printf("IMAGE SIZE: %llu\n", imageSize);

        void* mapped;
        vkMapMemory(vk_context->device, stagingBufferMemory, 0, imageSize, 0, &mapped);
        memcpy(mapped, rgba_data, imageSize);
        vkUnmapMemory(vk_context->device, stagingBufferMemory);

        free_memory(rgba_data, MEMORY_TAG_UNKNOWN);

        VkImage textureImage;
        VkDeviceMemory textureImageMemory;

        VkImageCreateInfo imageInfo = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                .pNext = NULL,
                .flags = 0,
                .imageType = VK_IMAGE_TYPE_2D,
                .format = VK_FORMAT_R8G8B8A8_SRGB, //todo
                .extent = {
                        .height = parsed_data->color_embedded->height,
                        .width = parsed_data->color_embedded->width,
                        .depth = 1,
                },
                .mipLevels = 1,
                .arrayLayers = 1,
                .samples = VK_SAMPLE_COUNT_1_BIT,
                .tiling = VK_IMAGE_TILING_OPTIMAL,
                .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                .queueFamilyIndexCount = 1,
                .pQueueFamilyIndices = &vk_context->graphics_queue_family_index,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        };

        if (vkCreateImage(vk_context->device, &imageInfo, vk_context->sge_allocator, &textureImage) != VK_SUCCESS) {
                log_internal_event(LOG_LEVEL_ERROR, "failed to create image");
                return SGE_ERROR;
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(vk_context->device, textureImage, &memRequirements);

        VkMemoryAllocateInfo allocInfo = {
                .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                .allocationSize = memRequirements.size,
                .memoryTypeIndex = find_memory_type(render, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
            };

        if (vkAllocateMemory(vk_context->device, &allocInfo, vk_context->sge_allocator, &textureImageMemory) != VK_SUCCESS) {
                log_internal_event(LOG_LEVEL_ERROR, "failed to allocate image memory");
                return SGE_ERROR;
        }

        vkBindImageMemory(vk_context->device, textureImage, textureImageMemory, 0);

        sge_vulkan_transition_image_layout(vk_context, textureImage, VK_FORMAT_R8G8B8A8_SRGB,
                                   VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        sge_vulkan_copy_buffer_to_image(vk_context, stagingBuffer, textureImage, parsed_data->color_embedded->width,
                                        parsed_data->color_embedded->height);

        sge_vulkan_transition_image_layout(vk_context, textureImage, VK_FORMAT_R8G8B8A8_SRGB,
                                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                           VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);


        sge_render_texture *final_texture = allocate_memory(sizeof(sge_render_texture), MEMORY_TAG_IMAGE);
        if (!final_texture) {
                log_internal_event(LOG_LEVEL_ERROR, "failed to allocate render texture struct");
                return SGE_ERROR;
        }

        final_texture->api_texture = textureImage;
        *out_texture = final_texture;

        return SGE_SUCCESS;
}
