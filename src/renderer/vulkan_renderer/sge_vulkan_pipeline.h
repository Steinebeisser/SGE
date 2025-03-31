//
// Created by Geisthardt on 07.03.2025.
//

#ifndef SGE_VULKAN_PIPELINE_H
#define SGE_VULKAN_PIPELINE_H

#include "renderer/sge_render.h"
#include "vulkan_structs.h"

typedef struct sge_vulkan_pipeline_settings sge_vulkan_pipeline_settings;

SGE_RESULT sge_vulkan_pipeline_create(sge_render *render);
SGE_RESULT sge_vulkan_pipeline_destroy(sge_render *render);
sge_vulkan_pipeline_settings *transform_pipeline_settings_to_vulkan_specific(sge_pipeline_settings *settings);

SGE_RESULT sge_vulkan_pipeline_create_specific_format(
                sge_render *render,
                sge_vertex_format *format,
                sge_vulkan_pipeline_settings *settings,
                VkPipeline *create_pipeline,
                VkPipelineLayout *create_pipeline_layout);

#endif //SGE_VULKAN_PIPELINE_H
