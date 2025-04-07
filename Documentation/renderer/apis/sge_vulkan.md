# SGE Vulkan

## Overview

Used for sge_render to init vulkan specific stuff

## Table of Content

1. [Structures](#structures) <br>
   1.1 [vulkan_app_info](#vulkan_app_info) <br>
   1.2 [sge_vulkan_render_settings](#sge_vulkan_render_settings) <br>
2. [Examples](#examples) <br>
   2.1 [Initialize Settings](#initialize-settings) <br>

## Structures

### vulkan_app_info
```c
typedef struct vulkan_app_info {
    char            *application_name;
    uint32_t        application_version;
} vulkan_app_info;
```

---

### sge_vulkan_render_settings
```c
typedef struct sge_vulkan_render_settings {
    SGE_BOOL                enable_validation_layers;
    SGE_BOOL                use_dynamic_rendering;
    SGE_BOOL                use_sge_allocator;
    vulkan_app_info         app_info;
} sge_vulkan_render_settings;
```

## Examples

### Initialize Settings

```c
sge_render_settings settings = {
    .vulkan = {
        .enable_validation_layers = SGE_TRUE,
        .use_dynamic_rendering = SGE_TRUE,
        .use_sge_allocator = SGE_TRUE,
        .app_info = {
            .application_name = "Test Example",
            .application_version = sge_make_app_version(0, 1, 1)
        }
    }
};
```
