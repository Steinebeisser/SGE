//
// Created by Geisthardt on 10.03.2025.
//

#include "sge_vulkan_draw.h"

#include <stdio.h>

#include "sge_vulkan_resize.h"
#include "core/sge_internal_logging.h"
#include "vulkan_structs.h"

extern bool is_resize;

SGE_RESULT sge_vulkan_draw_frame(sge_render *render) {
        sge_vulkan_context *vk_context = render->api_context;
        if (is_resize) {
                vkDeviceWaitIdle(vk_context->device);
                handle_resize_out_of_date(render);
                is_resize = false;
                return SGE_RESIZE;
        }

        vkWaitForFences(vk_context->device, 1, &vk_context->so.in_flight_fences[vk_context->so.current_frame], VK_TRUE, UINT64_MAX);

        uint32_t image_index;
        VkResult acquire_result = vkAcquireNextImageKHR(vk_context->device, vk_context->swapchain, UINT64_MAX,
                                                        vk_context->so.image_available_semaphores[vk_context->so.current_frame],
                                                        VK_NULL_HANDLE, &image_index);
        if (acquire_result != VK_SUCCESS && acquire_result != VK_SUBOPTIMAL_KHR) {
            log_internal_event(LOG_LEVEL_ERROR, "Failed to acquire next image");
            return SGE_ERROR;
        }

        vkResetFences(vk_context->device, 1, &vk_context->so.in_flight_fences[vk_context->so.current_frame]);


        VkCommandBuffer command_buffer = vk_context->command_buffer[vk_context->command_buffer_index];
        //log_internal_event(LOG_LEVEL_TRACE, "beginning command buffer recording");
        vkResetCommandBuffer(command_buffer, 0);
        //log_internal_event(LOG_LEVEL_TRACE, "reset cmd buffer");
        VkCommandBufferBeginInfo cmd_buf_begin_info = {0};
        cmd_buf_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        const VkResult rec_begin_result = vkBeginCommandBuffer(command_buffer, &cmd_buf_begin_info);

        if (rec_begin_result != VK_SUCCESS) {
                log_internal_event(LOG_LEVEL_FATAL, "Failed to begin cmd buffer recording: %d", rec_begin_result);
        }

        //log_internal_event(LOG_LEVEL_INFO, "Can now record into cmd buffer");


        vk_context->command_buffer_index = (vk_context->command_buffer_index + 1) % 3;

        VkImageMemoryBarrier barrier = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                .srcQueueFamilyIndex = vk_context->graphics_queue_family_index,
                .dstQueueFamilyIndex = vk_context->graphics_queue_family_index,
                .image = vk_context->sc.sc_images[image_index],
                .subresourceRange = {
                        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                        .baseMipLevel = 0,
                        .levelCount = 1,
                        .baseArrayLayer = 0,
                        .layerCount = 1
                },
                .srcAccessMask = 0,
                .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
        };
        vkCmdPipelineBarrier(command_buffer,
                             VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                             VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                             0, 0, NULL, 0, NULL,
                             1, &barrier);

        VkImageMemoryBarrier depthBarrier = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .newLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
                .srcQueueFamilyIndex = vk_context->graphics_queue_family_index,
                .dstQueueFamilyIndex = vk_context->graphics_queue_family_index,
                .image = vk_context->sc.depth_images[image_index],
                .subresourceRange = {
                        .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
                        .baseMipLevel = 0,
                        .levelCount = 1,
                        .baseArrayLayer = 0,
                        .layerCount = 1
                    },
                    .srcAccessMask = 0,
                    .dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
                };

        vkCmdPipelineBarrier(command_buffer,
                            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
                            0, 0, NULL, 0, NULL,
                            1, &depthBarrier);

        VkRenderingInfoKHR render_info = {
                .sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
                .renderArea = {
                        .offset = {0, 0},
                        .extent = vk_context->sc.surface_capabilities.currentExtent
                },
                .layerCount = 1,
                .colorAttachmentCount = 1,
                .pColorAttachments = &(VkRenderingAttachmentInfoKHR){
                        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
                        .imageView = vk_context->sc.color_views[image_index],
                        .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                        .clearValue = {{0.0f, 0.0f, 0.0f, 0.0f}}
                },
                .pDepthAttachment = &(VkRenderingAttachmentInfoKHR) {
                        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
                        .pNext = NULL,
                        .imageView = vk_context->sc.depth_views[image_index],
                        .imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
                        .resolveMode = 0,
                        .resolveImageView = 0,
                        .resolveImageLayout = 0,
                        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                        .clearValue = { .depthStencil = { .depth = 1.0f, .stencil = 0} },
                }
        };

        //if (vk_context->so.current_frame == 2) {
        //        ((VkRenderingAttachmentInfoKHR*)render_info.pColorAttachments)->clearValue.color.float32[0] = 0.0f;
        //        ((VkRenderingAttachmentInfoKHR*)render_info.pColorAttachments)->clearValue.color.float32[1] = 0.0f;
        //        ((VkRenderingAttachmentInfoKHR*)render_info.pColorAttachments)->clearValue.color.float32[2] = 0.0f;
        //        ((VkRenderingAttachmentInfoKHR*)render_info.pColorAttachments)->clearValue.color.float32[3] = 0.0f;
        //} else if (vk_context->so.current_frame == 1) {
        //        ((VkRenderingAttachmentInfoKHR*)render_info.pColorAttachments)->clearValue.color.float32[0] = 1.0f;
        //        ((VkRenderingAttachmentInfoKHR*)render_info.pColorAttachments)->clearValue.color.float32[1] = 0.0f;
        //        ((VkRenderingAttachmentInfoKHR*)render_info.pColorAttachments)->clearValue.color.float32[2] = 0.0f;
        //        ((VkRenderingAttachmentInfoKHR*)render_info.pColorAttachments)->clearValue.color.float32[3] = 1.0f;
        //} else {
        //        ((VkRenderingAttachmentInfoKHR*)render_info.pColorAttachments)->clearValue.color.float32[0] = 0.0f;
        //        ((VkRenderingAttachmentInfoKHR*)render_info.pColorAttachments)->clearValue.color.float32[1] = 1.0f;
        //        ((VkRenderingAttachmentInfoKHR*)render_info.pColorAttachments)->clearValue.color.float32[2] = 0.0f;
        //        ((VkRenderingAttachmentInfoKHR*)render_info.pColorAttachments)->clearValue.color.float32[3] = 1.0f;
        //}

        vkCmdBeginRendering(command_buffer, &render_info);
        //vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_context->pipeline);

        for (int i = 0; i < render->region_count; ++i) {
                sge_region *region = render->regions[i];

                float height = region->viewport->height;
                float width = region->viewport->width;

                if (width == SGE_REGION_FULL_DIMENSION) {
                        width = vk_context->sc.surface_capabilities.currentExtent.width;
                }
                if (height == SGE_REGION_FULL_DIMENSION) {
                        height = vk_context->sc.surface_capabilities.currentExtent.height;
                }

                //printf("H: %f, W: %f, x: %f, y: %f, maxd: %f, mind: %f\n", height, width, region->viewport->x, region->viewport->y, region->viewport->max_depth, region->viewport->min_depth);

                VkDescriptorSet desc_set = (VkDescriptorSet)region->uniform_buffers[vk_context->so.current_frame].descriptor;
                if (region->uniform_buffers && vk_context->so.current_frame < 3) {
                        desc_set = (VkDescriptorSet)region->uniform_buffers[vk_context->so.current_frame].descriptor;
                        if (!desc_set) {
                                log_internal_event(LOG_LEVEL_ERROR, "Invalid descriptor set for region %d", i);
                                continue;
                        }
                } else {
                        log_internal_event(LOG_LEVEL_ERROR, "Missing uniform buffers for region %d", i);
                        continue;
                }

                if (!region->viewport || !region->scissor) {
                        log_internal_event(LOG_LEVEL_ERROR, "Region missing viewport or scissor");
                        continue;
                }

                VkViewport viewport = {
                        .height = height,
                        .width = width,
                        .x = region->viewport->x,
                        .y = region->viewport->y,
                        .maxDepth = region->viewport->max_depth,
                        .minDepth = region->viewport->min_depth,
                };

                VkRect2D  scissor = {
                        .extent = { width,height },
                        .offset = { region->scissor->offset_x, region->scissor->offset_y }
                };

                vkCmdSetViewport(command_buffer, 0, 1, &viewport);
                vkCmdSetScissor(command_buffer, 0, 1, &scissor);

                VkPipeline current_pipeline = VK_NULL_HANDLE;



                for (int r = 0; r < region->renderable_count; ++r) {
                        sge_renderable *renderable = region->renderables[r];

                        if (!renderable || !renderable->mesh) {
                                log_internal_event(LOG_LEVEL_ERROR, "NULL renderable or mesh at index %d", r);
                                continue;
                        }

                        log_internal_event(LOG_LEVEL_INFO, "Drawing mesh: vertices=%d, stride=%d, buffer=%p",
                                renderable->mesh->vertex_count,
                                renderable->mesh->vertex_size,
                                renderable->mesh->vertex_buffer.api_handle);

                        if (!renderable->pipeline) {
                                log_internal_event(LOG_LEVEL_WARNING, "no pipeline for renderable");
                                continue;
                        }

                        if (renderable->pipeline && renderable->pipeline_layout) {
                                if (renderable->pipeline != current_pipeline) {
                                        vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, renderable->pipeline);
                                        current_pipeline = renderable->pipeline;

                                        vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                                               renderable->pipeline_layout, 0, 1, &desc_set, 0, NULL);
                                }
                        } else {
                                if (current_pipeline != vk_context->pipeline) {
                                        log_internal_event(LOG_LEVEL_WARNING, "fallback global pipeline");
                                        vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_context->pipeline);
                                        current_pipeline = vk_context->pipeline;
                                        vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                                               vk_context->pipeline_layout, 0, 1, &desc_set, 0, NULL);
                                }
                        }

                        VkBuffer vertex_buffer[] = {renderable->mesh->vertex_buffer.api_handle};
                        VkDeviceSize offsets[] = {0};

                        vkCmdBindVertexBuffers(command_buffer, 0, 1, vertex_buffer, offsets);
                        vkCmdDraw(command_buffer, renderable->mesh->vertex_count, 1, 0, 0);


                }

        }

        //VkViewport viewport = {
        //        .x = 0.0f,
        //        .y = 0.0f,
        //        .width = (float)vk_context->sc.surface_capabilities.currentExtent.width,
        //        .height = (float)vk_context->sc.surface_capabilities.currentExtent.height,
        //        .minDepth = 0.0f,
        //        .maxDepth = 1.0f
        //};
        //VkRect2D scissor = {
        //        .offset = {0.0, 0},
        //        .extent = vk_context->sc.surface_capabilities.currentExtent
        //};
        //vkCmdSetViewport(command_buffer, 0, 1, &viewport);
        //vkCmdSetScissor(command_buffer, 0, 1, &scissor);

        //for (int i = 0; i < render->sge_renderables_count; ++i) {
        //        log_internal_event(LOG_LEVEL_INFO, "rendering renderable");
        //        sge_renderable *renderable = &render->sge_renderables;
//
        //        VkBuffer vertex_buffers[] = { renderable->mesh->vertex_buffer };
        //        VkDeviceSize offsets[] = { 0 };
//
        //        vkCmdBindVertexBuffers(command_buffer, 0, 1, vertex_buffers, offsets);
        //        vkCmdDraw(command_buffer, renderable->mesh->vertex_count, 1, 0, 0);
        //}
        vkCmdEndRendering(command_buffer);

        barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        barrier.dstAccessMask = 0;
        vkCmdPipelineBarrier(command_buffer,
                             VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                             VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                             0, 0, NULL, 0, NULL,
                             1, &barrier);

        const VkResult rec_end_result = vkEndCommandBuffer(command_buffer);
        if (rec_end_result != VK_SUCCESS) {
                log_internal_event(LOG_LEVEL_FATAL, "Failed to end cmd buffer recording: %d", rec_end_result);
        }

        //log_internal_event(LOG_LEVEL_INFO, "Finished cmd buffer recording");

        VkSubmitInfo submit_info = {
                .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                .waitSemaphoreCount = 1,
                .pWaitSemaphores = &vk_context->so.image_available_semaphores[vk_context->so.current_frame],
                .pWaitDstStageMask = (VkPipelineStageFlags[]){VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT},
                .commandBufferCount = 1,
                .pCommandBuffers = &command_buffer,
                .signalSemaphoreCount = 1,
                .pSignalSemaphores = &vk_context->so.render_finished_semaphores[vk_context->so.current_frame]
        };

        VkResult submit_result = vkQueueSubmit(vk_context->graphics_queue, 1, &submit_info,
                                               vk_context->so.in_flight_fences[vk_context->so.current_frame]);
        if (submit_result != VK_SUCCESS) {
                log_internal_event(LOG_LEVEL_ERROR, "Failed to submit queue");
                return SGE_ERROR;
        }

        VkPresentInfoKHR present_info = {
                .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                .waitSemaphoreCount = 1,
                .pWaitSemaphores = &vk_context->so.render_finished_semaphores[vk_context->so.current_frame],
                .swapchainCount = 1,
                .pSwapchains = &vk_context->swapchain,
                .pImageIndices = &image_index
        };

        VkResult present_result = vkQueuePresentKHR(vk_context->present_queue, &present_info);
        if (present_result != VK_SUCCESS) {
                log_internal_event(LOG_LEVEL_ERROR, "Failed to present queue");
                return SGE_ERROR;
        }

        vk_context->so.current_frame = (vk_context->so.current_frame + 1) % 3;

        return SGE_SUCCESS;

    //if (!sge_begin_frame(render)) {
    //    log_internal_event(LOG_LEVEL_FATAL, "Failed to begin frame");
    //    return SGE_ERROR;
    //}


    //sge_end_frame(render);
    return SGE_SUCCESS;
}

SGE_RESULT sge_vulkan_begin_frame(sge_render *render) {
    sge_vulkan_context *vk_context = render->api_context;

    return SGE_SUCCESS;
}


SGE_RESULT sge_vulkan_end_frame(sge_render *render) {
    sge_vulkan_context *vk_context = render->api_context;


    return SGE_SUCCESS;
}