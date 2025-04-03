# SGE Render Module

## Overview

The SGE Render Module is responsible for handling the rendering process. It abstracts rendering APIs as much as possible, ensuring minimal exposure to API-specific details. The only exception is during setup, where you specify the desired rendering API and its corresponding settings. This design allows for easy integration while maintaining fine-grained control over rendering configurations.

## Table of Content

1. [Structures](#structures)  
   1.1  [sge_buffer](#sge_buffer)<br>
   1.2  [sge_vertex_attribute](#sge_vertex_attribute)<br>
   1.3  [sge_vertex_format](#sge_vertex_format)<br>
   1.4  [sge_shader](#sge_shader)<br>
   1.5  [sge_pipeline_settings](#sge_pipeline_settings)<br>
   1.6  [sge_mesh](#sge_mesh)<br>
   1.7  [sge_material](#sge_material)<br>
   1.8  [sge_renderable](#sge_renderable)<br>
   1.9  [sge_pipeline](#sge_pipeline)<br>
   1.10 [sge_viewport](#sge_viewport)<br>
   1.11 [sge_scissor](#sge_scissor)<br>
   1.12 [sge_renderer_interface](#sge_renderer_interface)<br>
   1.13 [sge_view](#sge_view)<br>
   1.14 [sge_render](#sge_render)<br>
   1.15 [sge_render_settings](#sge_render_settings)

2. [Enumerations](#enumerations)  
   2.1 [SGE_RENDER_API](#sge_render_api)<br>
   2.2 [SGE_GEOMETRY_TYPE](#sge_geometry_type)

3. [API Reference](#api-reference)  
   3.1 [sge_render_create](#sge_render-sge_render_createsge_render_api-api-sge_window-window)<br>
   3.2 [sge_render_initialize](#sge_result-sge_render_initializesge_render-render-sge_render_settings-settings)<br>
   3.3 [sge_draw_frame](#sge_result-sge_draw_framesge_render-render)<br>
   3.4 [create_renderable_from_rend_file](#sge_renderable-create_renderable_from_rend_filesge_render-render-sge_rend_file-file)<br>

4. [Examples](#examples)

## Structures

### `sge_buffer`
```c
typedef struct sge_buffer {
        void            *api_handle;
        size_t          size;
        void            *data;
} sge_buffer;
```

### `sge_vertex_attribute`
```c
typedef struct sge_vertex_attribute {
        uint32_t        location;
        uint32_t        format;
        uint32_t        components;
        uint32_t        offset;
} sge_vertex_attribute;
```

### `sge_vertex_format`
```c
typedef struct sge_vertex_format {
        uint32_t                stride;
        sge_vertex_attribute    *attributes;
        size_t                  attribute_count;
} sge_vertex_format;
```

### `sge_shader`
```c
typedef struct sge_shader { //SPIR-V vulkan, HLSL directx, GLSL opengl
        SGE_RENDER_API api;
        void *api_shader;
} sge_shader;
```

### `sge_pipeline_settings`
```c
typedef struct sge_pipeline_settings {
        sge_vertex_format       *vertex_format;

        SGE_PRIMITIVE_TYPE      topology;
        uint32_t                patch_control_points;

        SGE_POLYGON_MODE        polygon_mode;
        float                   line_width;

        SGE_CULL_MODE           cull_mode;
        SGE_FRONT_FACE          front_face;
        SGE_BOOL                tesselation_enabled;
        SGE_BOOL                geometry_enabled;

        SGE_MSAA_COUNT          msaa_count;
        SGE_BOOL                per_sample_shading_enabled;
        float                   min_sample_shading;

        SGE_BOOL                is_3d;
} sge_pipeline_settings;
```

### `sge_mesh`
```c
typedef struct sge_mesh {
        sge_buffer              vertex_buffer;
        sge_buffer              index_buffer;
        uint32_t                vertex_size;
        uint32_t                attribute_count;
        uint32_t                vertex_count;
        uint32_t                index_count;
        SGE_MESH_ATTRIBUTE      *attributes;
        sge_vertex_format       *format;
        char                    name[64];
} sge_mesh;
```

### `sge_material`
```c
typedef struct sge_material {
        sge_shader      *vertex_shader;
        sge_shader      *fragment_shader;
        vec4            color;
        char name[64];
} sge_material;
```

### `sge_renderable`
```c
typedef struct sge_renderable {
        sge_mesh        *mesh;
        sge_material    *material;
        char            name[64];
        void            *pipeline;
        void            *pipeline_layout;
} sge_renderable;
```

### `sge_pipeline`
```c
typedef struct sge_pipeline {
        SGE_RENDER_API api;
        void *api_pipeline;
} sge_pipeline;
```

### `sge_viewport`
```c
typedef struct sge_viewport {
        float x;               
        float y;               
        float width;           
        float height;          
        float min_depth;       
        float max_depth;       
} sge_viewport;
```

### `sge_scissor`
```c
typedef struct sge_scissor {
        float offset_x;                
        float offset_y;                
        float extent_width;            
        float extent_height;           
} sge_scissor;
```

### `sge_renderer_interface`
maps the render api
```c
typedef struct sge_renderer_interface {
        SGE_RESULT (*initialize)(sge_render *render, sge_render_settings *settings);
        SGE_RESULT (*shutdown)(sge_render *render);
        SGE_RESULT (*begin_frame)(sge_render *render);
        SGE_RESULT (*draw)(sge_render *render);
        SGE_RESULT (*end_frame)(sge_render *render);
        SGE_RESULT (*set_pipeline)(sge_render *render, sge_pipeline *pipeline);
        SGE_RESULT (*update_uniform)(sge_render *render, sge_region *region);
        SGE_RESULT (*create_buffer)(sge_render *render, void **buffer_ptr);                             // !! uniform buffer !! todo to make setting
        SGE_RESULT (*allocate_buffer_memory)(sge_render *render, void **memory_ptr, void *buffer);      // !! uniform buffer !! todo to make setting
        SGE_RESULT (*create_descriptor_pool)(sge_render *render, void *pool_ptr);
        SGE_RESULT (*allocate_descriptor_set)(sge_render *render, void *descriptor_ptr, void *layout_ptr, void *descriptor_pool);
        SGE_RESULT (*update_descriptor_set)(sge_render *render, sge_uniform_buffer_type *buffer);
        SGE_RESULT (*create_renderable_resources)(sge_render *render, sge_renderable *renderable);
} sge_renderer_interface;
```

### `sge_view`
```c
typedef struct sge_view {
        sge_viewport                    viewport;
        sge_scissor                     scissor;
        sge_camera                      *camera;
        sge_uniform_buffer_object       ubo_transform;
        sge_renderable                  **renderables;
        size_t                          renderable_count;
} sg_view;
```

### `sge_render`
```c
typedef struct sge_render {
        SGE_RENDER_API              api;
        void                    *api_context;
        sge_window              *window;
        sge_renderer_interface  *sge_interface;
        sge_region              **regions;
        size_t                  region_count;
} sge_render;
```

### `sge_render_settings`
uses api specific setting as needed found in
`renderer/apis/sge_{api_name}.h`
```c
typedef struct sge_render_settings {
        sge_vulkan_render_settings vulkan;
        sge_opengl_render_settings opengl;
        sge_directx_render_settings directx;
} sge_render_settings;
```

### Enumerations

### `SGE_RENDER_API`
```c
typedef enum sge_render_api {
        SGE_RENDER_API_VULKAN,
        SGE_RENDER_API_DIRECTX,
        SGE_RENDER_API_OPENGL
} SGE_RENDER_API;
```

### `SGE_GEOMETRY_TYPE`
```c
typedef enum SGE_GEOMETRY_TYPE {
        SGE_GEOMETRY_2D,
        SGE_GEOMETRY_3D
} SGE_GEOMETRY_TYPE;
```

## API Reference

### `sge_render *sge_render_create(SGE_RENDER_API api, sge_window *window);`

#### Parameters:
- `api`: The rendering API to use (Vulkan, OpenGL, DirectX). Only Vulkan Supported atm
- `window`: Pointer to the window where the renderer should work.

#### Returns:
- Pointer to the created `sge_render` instance.
- `NULL` if it fails

---

### `SGE_RESULT sge_render_initialize(sge_render *render, sge_render_settings *settings);`

#### Parameters:
- `render`: Pointer to the `sge_render` instance.
- `settings`: Rendering settings to be applied.

#### Returns:
- `SGE_SUCCESS`: if it succeeds
- `SGE_ERROR`: if it fails

---

### `SGE_RESULT sge_draw_frame(sge_render *render);`

#### Parameters:
- `render`: Pointer to the `sge_render` instance.

#### Returns:
- `SGE_SUCCESS`: if the frame was successfully drawn
- `SGE_RESIZE`: if a resize happened, skipped frame, rebuild everything needed
- `SGE_ERROR`: if it fails 

---

### `sge_renderable *create_renderable_from_rend_file(sge_render *render, SGE_REND_FILE *file);`

#### Parameters: 
- `render`: Pointer to the `sge_render` instance.
- `file`: Pointer to the `SGE_REND_FILE` structure, containing a render object

#### Returns: 
- Pointer to a `sge_renderable` struct that can be used to add into a Region to render it

---

## Examples

### Create Render
Using a Window Created in [sge_window.md](../core/platform/sge_window.md#examples)
```c
sge_render *render = sge_render_create(SGE_RENDER_API_VULKAN, window);
```

### Initializing Render
###
```c
sge_render_settings *settings = allocate_memory(sizeof(sge_render_settings), MEMORY_TAG_RENDERER);
settings = &(sge_render_settings){
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
sge_render_initialize(render, settings);
```

### Create Renderable
Renderable file created in [sge_render_file.md](sge_render_file.md)
```c
sge_renderable *cube = create_renderable_from_rend_file(render, cube_file);
```

Loading renderable in [sge_region.md](sge_region.md)

```c
while (1) {
      sge_draw_frame(render);
}
```