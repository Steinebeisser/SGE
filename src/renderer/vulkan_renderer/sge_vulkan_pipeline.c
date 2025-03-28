//
// Created by Geisthardt on 07.03.2025.
//

#include "sge_vulkan_pipeline.h"

#include "sge_vulkan_shader.h"
#include "../../core/logging.h"
#include "../../core/memory_control.h"


SGE_RESULT sge_vulkan_pipeline_create(sge_render *render) {
        sge_vulkan_context *vk_context = (sge_vulkan_context*)(render->api_context);

        VkPipelineRenderingCreateInfo rendering_create_info = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
                .pNext = 0,
                .viewMask = 0,
                .colorAttachmentCount = 1,
                .pColorAttachmentFormats = &vk_context->sc.surface_format.format,
                .depthAttachmentFormat = 0,
                .stencilAttachmentFormat = 0,
        };

        VkShaderModule vertex_shader_module = sge_vulkan_shader_load_old(render, "simple_shader.vert.spv");
        VkShaderModule fragment_shader_module = sge_vulkan_shader_load_old(render, "simple_shader.frag.spv");
        if (!vertex_shader_module || !fragment_shader_module) {
                log_event(LOG_LEVEL_FATAL, "failed getting shaders");
        }

        VkPipelineShaderStageCreateInfo vertex_shader_stage_create_info = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .pNext = 0,
                .flags = 0,
                .stage = VK_SHADER_STAGE_VERTEX_BIT,
                .module = vertex_shader_module,
                .pName = "main",
                .pSpecializationInfo = NULL,
        };

        VkPipelineShaderStageCreateInfo fragment_shader_stage_create_info = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .pNext = 0,
                .flags = 0,
                .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                .module = fragment_shader_module,
                .pName = "main",
                .pSpecializationInfo = NULL,
        };

        VkPipelineShaderStageCreateInfo shader_stages[] = {
                vertex_shader_stage_create_info,
                fragment_shader_stage_create_info
        };



        VkVertexInputBindingDescription binding_description = {
                .binding = 0,
                .stride = sizeof(float) * 3 + 4,
                .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
            };

        VkVertexInputAttributeDescription attribute_descriptions[2] = {
                {
                        .binding = 0,
                        .location = 0,
                        .format = VK_FORMAT_R32G32B32_SFLOAT,
                        .offset = 0,
                },
                {
                        .binding = 0,
                        .location = 1,
                        .format = VK_FORMAT_R8G8B8A8_UNORM,
                        .offset = 12,
                }
        };

        VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
                .vertexBindingDescriptionCount = 1,
                .pVertexBindingDescriptions = &binding_description,
                .vertexAttributeDescriptionCount = 2,
                .pVertexAttributeDescriptions = attribute_descriptions,
        };

        VkPipelineInputAssemblyStateCreateInfo input_assembly_create_info = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
                .pNext = 0,
                .flags = 0,
                .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                .primitiveRestartEnable = VK_FALSE,
        };

        VkViewport viewport = {
                .x = 0.0f,
                .y = 0.0f,
                .width = vk_context->sc.surface_capabilities.maxImageExtent.width,
                .height = vk_context->sc.surface_capabilities.maxImageExtent.height,
                .minDepth = 0.0f,
                .maxDepth = 1.0f,
        };

        VkRect2D scissor = {
                .offset = { 0, 0 },
                .extent = vk_context->sc.surface_capabilities.maxImageExtent,
        };

        VkPipelineViewportStateCreateInfo viewport_state_create_info = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
                .pNext = 0,
                .flags = 0,
                .viewportCount = 1,
                .pViewports = &viewport,
                .scissorCount = 1,
                .pScissors = &scissor,
        };

        VkPipelineRasterizationStateCreateInfo rasterization_state_create_info = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
                .pNext = 0,
                .flags = 0,
                .depthClampEnable = VK_FALSE,
                .rasterizerDiscardEnable = VK_FALSE,
                .polygonMode = VK_POLYGON_MODE_FILL,
                .cullMode = VK_CULL_MODE_NONE,
                .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
                .depthBiasEnable = VK_FALSE,
                .depthBiasConstantFactor = 0,
                .depthBiasClamp = 0,
                .depthBiasSlopeFactor = 0,
                .lineWidth = 1.0f,
        };

        VkPipelineMultisampleStateCreateInfo multisample_state_create_info = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
                .pNext = 0,
                .flags = 0,
                .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
                .sampleShadingEnable = VK_FALSE,
                .minSampleShading = 0,
                .pSampleMask = NULL,
                .alphaToCoverageEnable = VK_FALSE,
                .alphaToOneEnable = VK_FALSE,
        };

        VkPipelineColorBlendAttachmentState attachment_state = {
                .blendEnable = VK_FALSE,
                .srcColorBlendFactor = 0,
                .dstColorBlendFactor = 0,
                .colorBlendOp = 0,
                .srcAlphaBlendFactor = 0,
                .dstAlphaBlendFactor = 0,
                .alphaBlendOp = 0,
                .colorWriteMask = VK_COLOR_COMPONENT_A_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_R_BIT,
        };

        VkPipelineColorBlendStateCreateInfo color_blend_state_create_info = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
                .pNext = 0,
                .flags = 0,
                .logicOpEnable = VK_FALSE,
                .logicOp = 0,
                .attachmentCount = 1,
                .pAttachments = &attachment_state,
                .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f},
        };

        VkDynamicState dynamic_states[] = {
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
                .pNext = 0,
                .flags = 0,
                .dynamicStateCount = sizeof(dynamic_states)/sizeof(dynamic_states[0]),
                .pDynamicStates = dynamic_states,
        };

        VkPipelineLayoutCreateInfo layout_create_info = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
                .pNext = 0,
                .flags = 0,
                .setLayoutCount = 1,
                .pSetLayouts = &vk_context->descriptor_set_layout,
                .pushConstantRangeCount = 0,
                .pPushConstantRanges = NULL,
        };


        VkPipelineLayout layout;

        VkResult layout_result = vkCreatePipelineLayout(vk_context->device, &layout_create_info, vk_context->sge_allocator, &layout);
        if (layout_result != VK_SUCCESS) {
                log_event(LOG_LEVEL_FATAL, "failed creating pipeline layout: %d", layout_result);
                return SGE_ERROR;
        }

        vk_context->pipeline_layout = layout;

        VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = {
                .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
                .pNext = &rendering_create_info,
                .flags = 0,
                .stageCount = sizeof(shader_stages)/sizeof(shader_stages[0]),
                .pStages = shader_stages,
                .pVertexInputState = &vertex_input_state_create_info,
                .pInputAssemblyState = &input_assembly_create_info,
                .pTessellationState = NULL,
                .pViewportState = &viewport_state_create_info,
                .pRasterizationState = &rasterization_state_create_info,
                .pMultisampleState = &multisample_state_create_info,
                .pDepthStencilState = NULL,
                .pColorBlendState = &color_blend_state_create_info,
                .pDynamicState = &dynamic_state_create_info,
                .layout = layout,
                .renderPass = NULL, //dynamic rendering extension -todo make dynamic if extension not supported
                .subpass = 0,
                .basePipelineHandle = NULL,
                .basePipelineIndex = -1,
        };

        VkPipeline graphics_pipeline;
        VkResult pipeline_result = vkCreateGraphicsPipelines(vk_context->device, VK_NULL_HANDLE, 1,
                                                                &graphics_pipeline_create_info,vk_context->sge_allocator, &graphics_pipeline);
        if (pipeline_result != VK_SUCCESS) {
                log_event(LOG_LEVEL_FATAL, "Failed to create pipeline: %d", pipeline_result);
                return SGE_ERROR;
        }

        vk_context->pipeline = graphics_pipeline;
        log_event(LOG_LEVEL_INFO, "Created pipeline");
        return SGE_SUCCESS;
}



SGE_RESULT sge_vulkan_pipeline_create_specific_format(
        sge_render *render,
        sge_vertex_format *format,
        sge_vulkan_pipeline_settings *settings,
        VkPipeline *create_pipeline,
        VkPipelineLayout *create_pipeline_layout
        ) {
        sge_vulkan_context *vk_context = render->api_context;

        VkShaderModule shaders[5] = {NULL}; //vert, tess cont, tess eval, geom, frag
        char *shader_paths[5] = {NULL};
        uint32_t shader_stage_count = 0;
        VkPipelineShaderStageCreateInfo shader_stages[5];


        log_event(LOG_LEVEL_INFO, "Getting shader paths");
        shader_paths[0] = sge_get_vertex_shader_path_for_format(render, format, settings->is_3d);
        shader_paths[4] = sge_get_fragment_shader_path_for_format(render, format, settings->is_3d);

        if (settings->tesselation_enabled) {
                //todo
                //shader_paths[1] = sge_get_tesselation_control_shader();
                //shader_paths[2] = sge_get_tesselation_evaluation_shader();
        }

        if (settings->geometry_enabled) {
                //todo
        }
        log_event(LOG_LEVEL_INFO, "^GOT SHADER PATHS");

        for (int i = 0; i < 5; ++i) {
                if (!shader_paths[i]) {
                        continue;
                }

                sge_create_shader_if_not_exist(shader_paths[i], format, render);

                shaders[i] = sge_vulkan_shader_load(render, shader_paths[i]);
                if (!shaders[i]) {
                        sge_create_shader(shader_paths[i], format, render);
                        shaders[i] = sge_vulkan_shader_load(render, shader_paths[i]);
                        if (!shaders[i]) {
                                log_event(LOG_LEVEL_ERROR, "failed to load shader %s", shader_paths[i]);
                                return SGE_ERROR;
                        }
                }


                VkShaderStageFlagBits stage;
                switch (i) {
                        case 0: {
                                stage = VK_SHADER_STAGE_VERTEX_BIT;
                        } break;
                        case 1: {
                                stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
                        } break;
                        case 2:
                        {
                                stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
                        } break;
                        case 3: {
                                stage = VK_SHADER_STAGE_GEOMETRY_BIT;
                        } break;
                        case 4: {
                                stage = VK_SHADER_STAGE_FRAGMENT_BIT;
                        } break;
                        default: {
                                log_event(LOG_LEVEL_ERROR, "unknown shader stage");
                                return SGE_ERROR;
                        }
                }

                shader_stages[shader_stage_count] = (VkPipelineShaderStageCreateInfo) {
                        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                        .flags = 0,
                        .module = shaders[i],
                        .stage = stage,
                        .pName = "main",
                        .pNext = NULL,
                        .pSpecializationInfo = NULL,
                };

                shader_stage_count++;
        }

         VkPipelineRenderingCreateInfo rendering_create_info = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
                .pNext = 0,
                .viewMask = 0,
                .colorAttachmentCount = 1,
                .pColorAttachmentFormats = &vk_context->sc.surface_format.format,
                .depthAttachmentFormat = vk_context->sc.depth_format,
                .stencilAttachmentFormat = 0,
        };

        VkVertexInputBindingDescription vertex_input_binding_description = {
                .binding = 0,
                .stride = format->stride,
                .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
        };

        VkVertexInputAttributeDescription *attributes = allocate_memory(sizeof(VkVertexInputAttributeDescription) * format->attribute_count, MEMORY_TAG_RENDERER);
        if (attributes == NULL) {
                allocation_error();
                return SGE_ERROR;
        }

        for (uint32_t i = 0; i < format->attribute_count; ++i) {
                printf("Loc: %d, form: %d\n", format->attributes[i].location, format->attributes[i].format);
                attributes[i] = (VkVertexInputAttributeDescription) {
                        .location = format->attributes[i].location,
                        .binding = 0,
                        .format = format->attributes[i].format,
                        .offset = format->attributes[i].offset,
                };
        }


        VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {
                 .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
                 .pNext = NULL,
                 .flags = 0,
                 .vertexBindingDescriptionCount = 1,
                 .pVertexBindingDescriptions = &vertex_input_binding_description,
                 .vertexAttributeDescriptionCount = format->attribute_count,
                 .pVertexAttributeDescriptions = attributes,
        };


        VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info = {
                 .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
                 .pNext = NULL,
                 .flags = 0,
                 .topology = settings->topology ? settings->topology : VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                 .primitiveRestartEnable = VK_FALSE,
        };

        VkPipelineTessellationStateCreateInfo tessellation_state_create_info = {0};
        if (settings->tesselation_enabled) {
                tessellation_state_create_info = (VkPipelineTessellationStateCreateInfo){
                        .sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
                        .pNext = NULL,
                        .flags = 0,
                        .patchControlPoints = settings->patch_control_points,
                };
        }

        VkPipelineViewportStateCreateInfo viewport_state_create_info = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
                .pNext = NULL,
                .flags = 0,
                .viewportCount = 1,
                .pViewports = NULL,
                .scissorCount = 1,
                .pScissors = NULL,
        };

        VkPipelineRasterizationStateCreateInfo rasterization_state_create_info = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
                .pNext = NULL,
                .flags = 0,
                .depthClampEnable = VK_FALSE,
                .rasterizerDiscardEnable = VK_FALSE,
                .polygonMode = settings->polygon_mode,
                .cullMode = settings->cull_mode,
                .frontFace = settings->front_face,
                .depthBiasEnable = VK_FALSE,
                .depthBiasConstantFactor = 0.0f,
                .depthBiasClamp = 0.0f,
                .depthBiasSlopeFactor = 0.0f,
                .lineWidth = settings->line_width,
        };

        VkPipelineMultisampleStateCreateInfo multisample_state_create_info = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
                .pNext = NULL,
                .flags = 0,
                .rasterizationSamples = settings->msaa_sample_count,
                .sampleShadingEnable = settings->per_sample_shading_enabled,
                .minSampleShading = settings->min_sample_shading,
                .pSampleMask = NULL,
                .alphaToCoverageEnable = VK_FALSE,
                .alphaToOneEnable = VK_FALSE,
        };

        printf("IS 3D: %d\n", settings->is_3d == VK_TRUE ? 1 : 0);
        VkPipelineDepthStencilStateCreateInfo depth_stencil_state_create_info = {
                 .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
                 .pNext = NULL,
                 .flags = 0,
                 .depthTestEnable = settings->is_3d,
                 .depthWriteEnable = settings->is_3d,
                 .depthCompareOp = VK_COMPARE_OP_LESS,
                 .depthBoundsTestEnable = VK_FALSE,
                 .stencilTestEnable = VK_FALSE,
                 .front = 0, //discarded if stencil test enable is VK_FALSE
                 .back = 0, //discarded if stencil test enable is VK_FALSE
                 .minDepthBounds = 0.0f,
                 .maxDepthBounds = 1.0f,
        };

        VkPipelineColorBlendAttachmentState color_blend_attachment_state = {
                .blendEnable = VK_FALSE,
                .srcColorBlendFactor = VK_BLEND_FACTOR_ONE, // Ignored
                .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO, // Ignored
                .colorBlendOp = VK_BLEND_OP_ADD, // Ignored
                .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE, // Ignored
                .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO, // Ignored
                .alphaBlendOp = VK_BLEND_OP_ADD, // Ignored
                .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                  VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
        };

        VkPipelineColorBlendStateCreateInfo color_blend_state_create_info = {
                 .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
                 .pNext = NULL,
                 .flags = 0,
                 .logicOpEnable = VK_FALSE,
                 .logicOp = VK_LOGIC_OP_COPY, //default if openable is false
                 .attachmentCount = 1,
                 .pAttachments = &color_blend_attachment_state,
                 .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f},
        };


        //todo check attribute types for descriptors
        uint32_t descriptor_binding_count = 0;
        VkDescriptorSetLayoutBinding descriptor_bindings[1]; //todo increase if needed

        descriptor_bindings[descriptor_binding_count] = (VkDescriptorSetLayoutBinding){
                .binding = SGE_SHADER_BINDING_UBO_TRANSFORM,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .pImmutableSamplers = NULL,
        };
        descriptor_binding_count++;

        VkDescriptorSetLayoutCreateInfo descriptor_layout_create_info = {
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
                .bindingCount = descriptor_binding_count,
                .pBindings = descriptor_bindings,
                .flags = 0,
                .pNext = NULL,
        };

        VkDescriptorSetLayout descriptor_set_layout = NULL;
        if (vkCreateDescriptorSetLayout(vk_context->device, &descriptor_layout_create_info, vk_context->sge_allocator, &descriptor_set_layout) != VK_SUCCESS) {
                log_event(LOG_LEVEL_ERROR, "failed to create descriptor set layout for format pipeline");
                return SGE_ERROR;
        }

        VkPipelineLayoutCreateInfo pipeline_layout_create_info = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
                .pNext = NULL,
                .flags = 0,
                .setLayoutCount = 1,
                .pSetLayouts = &descriptor_set_layout,
                .pPushConstantRanges = NULL,
                .pushConstantRangeCount = 0,
        };

        if (vkCreatePipelineLayout(vk_context->device, &pipeline_layout_create_info, vk_context->sge_allocator, create_pipeline_layout) != VK_SUCCESS) {
                log_event(LOG_LEVEL_ERROR, "failed to create pipeline layout for format pipeline");
                return SGE_ERROR;
        }

        VkDynamicState dynamic_states[] = {
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
                .pNext = 0,
                .flags = 0,
                .dynamicStateCount = sizeof(dynamic_states)/sizeof(dynamic_states[0]),
                .pDynamicStates = dynamic_states,
        };

        VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = {
                .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
                .pNext = &rendering_create_info,
                .flags = 0,
                .stageCount = shader_stage_count,
                .pStages = shader_stages,
                .pVertexInputState = &vertex_input_state_create_info,
                .pInputAssemblyState = &input_assembly_state_create_info,
                .pTessellationState = &tessellation_state_create_info,
                .pViewportState = &viewport_state_create_info,
                .pRasterizationState = &rasterization_state_create_info,
                .pMultisampleState = &multisample_state_create_info,
                .pDepthStencilState = &depth_stencil_state_create_info,
                .pColorBlendState = &color_blend_state_create_info,
                .pDynamicState = &dynamic_state_create_info,
                .layout = *create_pipeline_layout,
                .renderPass = NULL, //dynamic rendering extension
                .subpass = 0,
                .basePipelineHandle = NULL,
                .basePipelineIndex = -1,
        };

        if (vkCreateGraphicsPipelines(vk_context->device, VK_NULL_HANDLE, 1, &graphics_pipeline_create_info, vk_context->sge_allocator, create_pipeline) != VK_SUCCESS) {
                log_event(LOG_LEVEL_ERROR, "failed to create graphics pipeline");
                return SGE_ERROR;
        }


        return SGE_SUCCESS;
}

sge_vulkan_pipeline_settings *transform_pipeline_settings_to_vulkan_specific(sge_pipeline_settings *settings) {
        sge_vulkan_pipeline_settings *vulkan_pipeline_settings = allocate_memory(sizeof(sge_vulkan_pipeline_settings), MEMORY_TAG_RENDERER);

        log_event(LOG_LEVEL_INFO, "Transforming pipeline settings to vulkan specifics");

        if (vulkan_pipeline_settings == NULL) {
                allocation_error();
                return NULL;
        }

        switch (settings->topology) {
                case SGE_PRIMITIVE_POINTS:                      vulkan_pipeline_settings->topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;                          break;
                case SGE_PRIMITIVE_LINES:                       vulkan_pipeline_settings->topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;                           break;
                case SGE_PRIMITIVE_LINE_STRIP:                  vulkan_pipeline_settings->topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;                          break;
                case SGE_PRIMITIVE_TRIANGLE:                    vulkan_pipeline_settings->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;                       break;
                case SGE_PRIMITIVE_TRIANGLE_STRIP:              vulkan_pipeline_settings->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;                      break;
                case SGE_PRIMITIVE_TRIANGLE_FAN:                vulkan_pipeline_settings->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;                        break;
                case SGE_PRIMITIVE_PATCH_LIST:                  vulkan_pipeline_settings->topology = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;                          break;
                case SGE_PRIMITIVE_LINES_ADJACENCY:             vulkan_pipeline_settings->topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY;            break;
                case SGE_PRIMITIVE_LINE_STRIP_ADJACENCY:        vulkan_pipeline_settings->topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;           break;
                case SGE_PRIMITIVE_TRIANGLES_ADJACENCY:         vulkan_pipeline_settings->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;        break;
                case SGE_PRIMITIVE_TRIANGLE_STRIP_ADJACENCY:    vulkan_pipeline_settings->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;       break;
                default:                                        vulkan_pipeline_settings->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;                       break;
        }

        switch (settings->polygon_mode) {
                case SGE_POLYGON_MODE_FILL:                     vulkan_pipeline_settings->polygon_mode = VK_POLYGON_MODE_FILL;                                  break;
                case SGE_POLYGON_MODE_LINE:                     vulkan_pipeline_settings->polygon_mode = VK_POLYGON_MODE_LINE;                                  break;
                case SGE_POLYGON_MODE_POINT:                    vulkan_pipeline_settings->polygon_mode = VK_POLYGON_MODE_POINT;                                 break;
                default:                                        vulkan_pipeline_settings->polygon_mode = VK_POLYGON_MODE_FILL;                                  break;
        }

        switch (settings->cull_mode) {
                case SGE_CULL_MODE_NONE:                        vulkan_pipeline_settings->cull_mode = VK_CULL_MODE_NONE;                                        break;
                case SGE_CULL_MODE_FRONT:                       vulkan_pipeline_settings->cull_mode = VK_CULL_MODE_FRONT_BIT;                                   break;
                case SGE_CULL_MODE_BACK:                        vulkan_pipeline_settings->cull_mode = VK_CULL_MODE_BACK_BIT;                                    break;
                case SGE_CULL_MODE_FRONT_AND_BACK:              vulkan_pipeline_settings->cull_mode = VK_CULL_MODE_FRONT_AND_BACK;                              break;
                default:                                        vulkan_pipeline_settings->cull_mode = VK_CULL_MODE_BACK_BIT;                                    break;
        }


        switch (settings->front_face) {
                case SGE_FRONT_FACE_CLOCKWISE:                  vulkan_pipeline_settings->front_face = VK_FRONT_FACE_CLOCKWISE;                                 break;
                case SGE_FRONT_FACE_COUNTER_CLOCKWISE:          vulkan_pipeline_settings->front_face = VK_FRONT_FACE_COUNTER_CLOCKWISE;                         break;
                default:                                        vulkan_pipeline_settings->front_face = VK_FRONT_FACE_COUNTER_CLOCKWISE;                         break;
        }

        switch (settings->geometry_enabled) {
                case SGE_TRUE:                                  vulkan_pipeline_settings->geometry_enabled = VK_TRUE;                                           break;
                case SGE_FALSE:                                 vulkan_pipeline_settings->geometry_enabled = VK_FALSE;                                          break;
                default:                                        vulkan_pipeline_settings->geometry_enabled = VK_FALSE;                                          break;
        }

        switch (settings->tesselation_enabled) {
                case SGE_TRUE:                                  vulkan_pipeline_settings->tesselation_enabled = VK_TRUE;                                        break;
                case SGE_FALSE:                                 vulkan_pipeline_settings->tesselation_enabled = VK_FALSE;                                       break;
                default:                                        vulkan_pipeline_settings->tesselation_enabled = VK_FALSE;                                       break;
        }

        switch (settings->is_3d) {
                case SGE_TRUE:                                  vulkan_pipeline_settings->is_3d = VK_TRUE;                                                      break;
                case SGE_FALSE:                                 vulkan_pipeline_settings->is_3d = VK_FALSE;                                                     break;
                default:                                        vulkan_pipeline_settings->is_3d = VK_FALSE;                                                     break;
        }

        switch (settings->per_sample_shading_enabled) {
                case SGE_TRUE:                                  vulkan_pipeline_settings->per_sample_shading_enabled = VK_TRUE;                                 break;
                case SGE_FALSE:                                 vulkan_pipeline_settings->per_sample_shading_enabled = VK_FALSE;                                break;
                default:                                        vulkan_pipeline_settings->per_sample_shading_enabled = VK_FALSE;                                break;
        }

        switch (settings->msaa_count) {
                case SGE_MSAA_COUNT_1_BIT:                      vulkan_pipeline_settings->msaa_sample_count = VK_SAMPLE_COUNT_1_BIT;                            break;
                case SGE_MSAA_COUNT_2_BIT:                      vulkan_pipeline_settings->msaa_sample_count = VK_SAMPLE_COUNT_2_BIT;                            break;
                case SGE_MSAA_COUNT_4_BIT:                      vulkan_pipeline_settings->msaa_sample_count = VK_SAMPLE_COUNT_4_BIT;                            break;
                case SGE_MSAA_COUNT_8_BIT:                      vulkan_pipeline_settings->msaa_sample_count = VK_SAMPLE_COUNT_8_BIT;                            break;
                case SGE_MSAA_COUNT_16_BIT:                     vulkan_pipeline_settings->msaa_sample_count = VK_SAMPLE_COUNT_16_BIT;                           break;
                case SGE_MSAA_COUNT_32_BIT:                     vulkan_pipeline_settings->msaa_sample_count = VK_SAMPLE_COUNT_32_BIT;                           break;
                case SGE_MSAA_COUNT_64_BIT:                     vulkan_pipeline_settings->msaa_sample_count = VK_SAMPLE_COUNT_64_BIT;                           break;
                default:                                        vulkan_pipeline_settings->msaa_sample_count = VK_SAMPLE_COUNT_1_BIT;                            break;
        }

        vulkan_pipeline_settings->min_sample_shading          = settings->min_sample_shading ? settings->min_sample_shading : 1.0f;

        vulkan_pipeline_settings->patch_control_points        = settings->patch_control_points ? settings->patch_control_points : 3;

        vulkan_pipeline_settings->line_width                  = settings->line_width ? settings->line_width : 1.0f;



        log_event(LOG_LEVEL_INFO, "Finished Transforming pipeline settings to vulkan specifics");
        return vulkan_pipeline_settings;
}
