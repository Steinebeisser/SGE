//
// Created by Geisthardt on 19.05.2025.
//

#ifndef SGE_VULKAN_TEXTURE_H
#define SGE_VULKAN_TEXTURE_H

#include "renderer/sge_render.h"

SGE_RESULT sge_vulkan_create_render_texture(sge_render *render, sge_texture *texture, sge_render_texture **out_texture);

#endif //SGE_VULKAN_TEXTURE_H
