//
// Created by Geisthardt on 03.04.2025.
//

#ifndef SGE_INTERNAL_RENDER_H
#define SGE_INTERNAL_RENDER_H

#include "renderer/sge_render.h"

SGE_RESULT sge_create_buffer(sge_render *render, void **buffer_ptr);
SGE_RESULT sge_allocate_buffer(sge_render *render, void **memory_ptr, void *buffer);
SGE_RESULT sge_create_descriptor_pool(sge_render *render, void *pool_ptr);
SGE_RESULT sge_allocate_descriptor_set(sge_render *render, void *descriptor_ptr, void *layout_ptr, void *descriptor_pool);
SGE_RESULT sge_update_descriptor_set(sge_render *render, sge_uniform_buffer_type *buffer);

#endif //SGE_INTERNAL_RENDER_H
