//
// Created by Geisthardt on 14.03.2025.
//

#ifndef SGE_REGION_H
#define SGE_REGION_H

#include "renderer/sge_camera.h"

#define SGE_REGION_FULL_DIMENSION -1000


typedef enum SGE_REGION_TYPE {
        SGE_REGION_3D,
        SGE_REGION_2D
} SGE_REGION_TYPE;

typedef struct sge_region_settings {
        SGE_REGION_TYPE type;
        SGE_BOOL        auto_scale_on_resize;
        SGE_BOOL        auto_reposition_on_resize;
        int             width;
        int             height;
        int             offset_x;
        int             offset_y;
        float           min_depth;
        float           max_depth;
        int             z_index;
} sge_region_settings;

typedef struct sge_uniform_buffer_type {
        void    *memory;
        void    *buffer;
        void    *descriptor;
        void    *descriptor_layout;
} sge_uniform_buffer_type;

typedef struct sge_region {
        SGE_REGION_TYPE                 type;
        sge_camera                      *camera;
        sge_viewport                    *viewport;
        sge_scissor                     *scissor;
        sge_renderable                  **renderables;
        size_t                          renderable_count;
        sge_uniform_buffer_object       transform_ubo;
        uint32_t                        region_index;
        SGE_BOOL                        auto_scale_on_resize;
        SGE_BOOL                        reposition_on_resize;
        sge_uniform_buffer_type         uniform_buffers[3];
        void                            *descriptor_pool;
        int                             z_index;
} sge_region;

sge_region *sge_region_create(sge_render *render, sge_region_settings *settings);
SGE_RESULT sge_region_add_renderable(sge_region *region, sge_renderable *renderable);

SGE_RESULT sge_region_resize_auto_resizing_regions(sge_render *render, float old_width, float old_height, float new_width, float new_height);

sge_region *sge_region_get_active(sge_render *render);
sge_region **sge_region_get_active_list(sge_render *render, int *regions_count);

float sge_region_get_width(sge_region *region, sge_render *render);
float sge_region_get_height(sge_region *region, sge_render *render);

#endif //SGE_REGION_H
