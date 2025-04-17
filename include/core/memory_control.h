//
// Created by Geisthardt on 28.02.2025.
//

#ifndef MEMORY_CONTROL_H
#define MEMORY_CONTROL_H

#include <stdlib.h>

typedef enum memory_tag {
        MEMORY_TAG_UNKNOWN,
        MEMORY_TAG_LOGGER,
        MEMORY_TAG_INPUT,
        MEMORY_TAG_VULKAN,
        MEMORY_TAG_RENDERER,
        MEMORY_TAG_REGION,
        MEMORY_TAG_WINDOW,
        MEMORY_TAG_SHADER,
        MEMORY_TAG_SCENE,
        MEMORY_TAG_IMAGE,
        MEMORY_TAG_TEXTURE,
        MEMORY_TAG_CUSTOM, // New tag case
} memory_tag;

static inline const char *memory_tag_to_string(const memory_tag tag) {
        switch (tag) {
                case MEMORY_TAG_UNKNOWN: return "UNKNOWN";
                case MEMORY_TAG_LOGGER: return "LOGGER";
                case MEMORY_TAG_INPUT: return "INPUT";
                case MEMORY_TAG_VULKAN: return "VULKAN";
                case MEMORY_TAG_RENDERER: return "RENDERER";
                case MEMORY_TAG_REGION: return "REGION";
                case MEMORY_TAG_WINDOW: return "WINDOW";
                case MEMORY_TAG_SHADER: return "SHADER";
                case MEMORY_TAG_SCENE: return "SCENE";
                case MEMORY_TAG_IMAGE: return "IMAGE";
                case MEMORY_TAG_TEXTURE: return "TEXTURE";
                case MEMORY_TAG_CUSTOM: return "CUSTOM"; // New tag case
                default: return "INVALID_TAG";
        }
}

void *allocate_memory(size_t size, memory_tag tag);

void free_memory(void *ptr, memory_tag tag);

void *reallocate_memory(void *old_ptr, size_t new_size, memory_tag tag);

void *zero_memory(void *ptr, size_t size, int offset);

void *copy_memory(void *dest_ptr, void *src_ptr, size_t size, size_t offset_dest, size_t offset_src);

void *set_memory(void *ptr, int value, size_t size, int offset);

void print_memory_usage_str();

#endif //MEMORY_CONTROL_H
