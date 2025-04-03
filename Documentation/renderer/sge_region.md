# SGE REGION MODULE

## Overview

## Table of Content
1. [Structures](#structures)
    1.1 [sge_region_settings](#sge_region_settings) <br>
    1.2 [sge_uniform_buffer_type](#sge_uniform_buffer_type) <br>
    1.3 [sge_region](#sge_region) <br>
2. [Enumerations](#enumerations)
    2.1 [SGE_REGION_TYPE](#sge_region_type);
3. [Defines](#defines)
    3.1 [SGE_REGION_FULL_DIMENSION](#sge_region_full_dimension)
4. [API Reference](#api-reference)
5. [Examples](#examples)

---

## Structures

### `sge_region_settings`
```c
typedef struct sge_region_settings {
        SGE_REGION_TYPE     type;
        SGE_BOOL            auto_scale_on_resize;
        SGE_BOOL            auto_reposition_on_resize;
        int                 width;
        int                 height;
        int                 offset_x;
        int                 offset_y;
        float               min_depth;
        float               max_depth;
        int                 z_index;
} sge_region_settings;
```

---

### `sge_uniform_buffer_type`
```c
typedef struct sge_uniform_buffer_type {
        void                *memory;
        void                *buffer;
        void                *descriptor;
        void                *descriptor_layout;
} sge_uniform_buffer_type;
```

---

### `sge_region`
```c
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
```

---

## Enumerations

### `SGE_REGION_TYPE`
```c
typedef enum SGE_REGION_TYPE {
        SGE_REGION_3D,
        SGE_REGION_2D
} SGE_REGION_TYPE;
```

---

## Defines

### `SGE_REGION_FULL_DIMENSION`
```c
#define SGE_REGION_FULL_DIMENSION -1000
```

---

## API Reference

### `sge_region *sge_region_create(sge_render *render, sge_region_settings *settings);`

#### Parameters:
- `render`: A pointer to the `sge_render` instance 
- `settings`: A pointer to an `sge_region_settings` struct 

#### Returns:
- pointer to the created `sge_region` instance.
- `NULL`: if creation fails.

---

### `SGE_RESULT sge_region_add_renderable(sge_region *region, sge_renderable *renderable);`

#### Parameters:
- `region`: A pointer to the `sge_region` instance where the renderable should be added
- `renderable`: A pointer to the `sge_renderable` object to be added

#### Returns:
- `SGE_SUCCESS` if the renderable was added
- `SGE_ERROR_FAILED_ALLOCATION` if reallocation failed

---

### `SGE_RESULT sge_region_resize_auto_resizing_regions(sge_render *render, float old_width, float old_height, float new_width, float new_height);`
Mostly called from internal functions to handle if the window resized, if `auto_scale_on_resize`, is enabled on region creation

#### Parameters:
- `render`: A pointer to the `sge_render` instance
- `old_width`: The previous width of the region
- `old_height`: The previous height of the region
- `new_width`: The new width of the region
- `new_height`: The new height of the region

#### Returns:
- `SGE_SUCCESS` 

---

### `sge_region *sge_region_get_active(sge_render *render);`
returns the region where your mouse is hovered over with the highest z index, or the first one with the same z index

#### Parameters:
- `render`: A pointer to the `sge_render` instance 

#### Returns:
- A pointer to the currently active `sge_region`
- `NULL` if no active region exists

---

### `sge_region **sge_region_get_active_list(sge_render *render, int *regions_count);`
returns a list of regions where your mouse is hovered over, sorted by z index

#### Parameters:
- `render`: A pointer to the active `sge_render` instance
- `regions_count`: A pointer to an integer that will be set to the number of active regions

#### Returns:
- A pointer to an array of active `sge_region` instances
- `NULL`: if no active regions

---

### `float sge_region_get_width(sge_region *region, sge_render *render);`

#### Parameters:
- `region`: A pointer to the `sge_region` instance whose width is being queried.
- `render`: A pointer to the `sge_render` instance managing the rendering.

#### Returns:
- The width of the region in pixels.

---

### `float sge_region_get_height(sge_region *region, sge_render *render);`

#### Parameters:
- `region`: A pointer to the `sge_region` instance whose height is being queried.
- `render`: A pointer to the `sge_render` instance managing the rendering.

#### Returns:
- The height of the region in pixels.

---

## Examples

### Region Creation

Create a Region 
```c
sge_region_settings test_region_settings = {
        .auto_scale_on_resize = SGE_TRUE, // lets say we have 200x200 window and 100x100 region if we go to 300x300 window out region scales the same so 150x150
        .auto_reposition_on_resize = SGE_FALSE, //repositions x and y based on factor
        .height = SGE_REGION_FULL_DIMENSION, //Gets set to full window height
        .width = SGE_REGION_FULL_DIMENSION,  //Gets set to full window width
        .offset_x = 0,
        .offset_y = 0,
        .min_depth = 0.0f,
        .max_depth = 1.0f,
        .z_index = 2,
        .type = SGE_REGION_3D
};

sge_region *test_region = sge_region_create(render, &test_region_settings);
if (!test_region) {
        terminate_program();
}
```

### Add Renderable to Region

Renderable created in [sge_render.md](sge_render.md#create-renderable)
```c
sge_region_add_renderable(main_region, cube);
```

### Get active Region, and use it
```c
sge_region *active_region = sge_region_get_active(render);
                
sge_region **active_regions = sge_region_get_active_list(render, &regions_count);
for (int i = 0; i < regions_count; ++i) {
        sge_region *region = active_regions[i];
        printf("Region Type: %d\n", region->type);
}
```

### Use Region to move a Camera
Camera Movement defined in [sge_camera.md](sge_camera.md)<br>
Input in [input.md](../core/input.md)

```c
if (is_key_down(KEY_S)) {
        sge_camera_move_backwards(render, active_region, movement_settings_horizontal);
}
```
