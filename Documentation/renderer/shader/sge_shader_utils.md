# SGE Shader Utils Module

## Overview
The SGE Shader Module provides a way to create shader for each graphics api, not really needed by library users but still included in public header

## Table of Content

1. [Enumerations](#enumerations)
   1.1 [SGE_SHADER_LOCATIONS](#sge_shader_locations)  
   1.2 [SGE_SHADER_BINDINGS](#sge_shader_bindings)
2. [API Reference](#api-reference)
   2.1 [sge_get_location_from_attribute_type](#sge_shader_locations-sge_get_location_from_attribute_type)  
   2.2 [sge_get_vertex_shader_path_for_format](#char-sge_get_vertex_shader_path_for_format)  
   2.3 [sge_get_fragment_shader_path_for_format](#char-sge_get_fragment_shader_path_for_format)  
   2.4 [sge_create_shader](#sge_result-sge_create_shader)  
   2.5 [sge_create_shader_if_not_exist](#sge_result-sge_create_shader_if_not_exist)  
   2.6 [create_raw_shader_file](#sge_result-create_raw_shader_file)  
   2.7 [compile_shader_file](#sge_result-compile_shader_file)
3. [Examples](#examples)

## Enumerations

### `SGE_SHADER_LOCATIONS`
```c
typedef enum SGE_SHADER_LOCATIONS {
        SGE_SHADER_POSITION = 0,
        SGE_SHADER_COLOR = 1,
        SGE_SHADER_NORMAL = 2,
        SGE_SHADER_TEXCOORDS = 3,
        SGE_SHADER_TANGENT = 4,
        SGE_SHADER_BITANGENT = 5,
        SGE_SHADER_WEIGHTS = 6,
        SGE_SHADER_JOINTIDS = 7,
        SGE_SHADER_UNKNOWN = UINT32_MAX,
} SGE_SHADER_LOCATIONS;
```

### `SGE_SHADER_BINDINGS`
```c
typedef enum SGE_SHADER_BINDINGS {
        SGE_SHADER_BINDING_UBO_TRANSFORM = 0,
        // Add more when needed
} SGE_SHADER_BINDINGS;
```

## API Reference

### `SGE_SHADER_LOCATIONS sge_get_location_from_attribute_type(...)`
```c
SGE_SHADER_LOCATIONS sge_get_location_from_attribute_type(SGE_ATTRIBUTE_TYPE attribute_type);
```

#### Parameters:
- `attribute_type`: The type of the attribute

#### Returns:
- corresponding `SGE_SHADER_LOCATIONS` value

---

### `char* sge_get_vertex_shader_path_for_format(...)`
```c
char* sge_get_vertex_shader_path_for_format(sge_render *render, sge_vertex_format *format, SGE_BOOL is_3d);
```
#### Parameters:
- `render`: Pointer to `sge_render` struct
- `format`: Pointer to the `sge_vertex_format` struct, which defines the vertex format
- `is_3d`: SGE_BOOL 

#### Returns:
- string representing the path to the vertex shader file
- `NULL`: if errors occurred
---

### `char* sge_get_fragment_shader_path_for_format(...)`
```c
char* sge_get_fragment_shader_path_for_format(sge_render *render, sge_vertex_format *format, SGE_BOOL is_3d);
```
#### Parameters:
- `render`: Pointer to `sge_render` struct
- `format`: Pointer to the `sge_vertex_format` struct, which defines the vertex format
- `is_3d`: A boolean indicating if the shader is for 3D rendering

#### Returns:
- A string representing the path to the fragment shader file

---

### `SGE_RESULT sge_create_shader(...)`
This will force create shader even if already one exists
```c
SGE_RESULT sge_create_shader(char *shader_path, sge_vertex_format *format, sge_render *render);
```
#### Parameters:
- `shader_path`: Path to the shader file
- `format`: Pointer to `sge_vertex_format` struct
- `render`: Pointer to `sge_render` struct

#### Returns:
- `SGE_SUCCESS` on success
- `SGE_ERROR` on error

---

### `SGE_RESULT sge_create_shader_if_not_exist(...)`
```c
SGE_RESULT sge_create_shader_if_not_exist(char *shader_path, sge_vertex_format *format, sge_render *render);
```
#### Parameters:
- `shader_path`: Path to the shader file
- `format`: Pointer to `sge_vertex_format` struct
- `render`: Pointer to `sge_render` struct

#### Returns:
- `SGE_SUCCESS` if the shader is created successfully or already exists
- `SGE_ERROR` if shader creation fails

---

### `SGE_RESULT create_raw_shader_file(...)`
```c
SGE_RESULT create_raw_shader_file(char *filepath, char *filename, sge_vertex_format *format);
```
#### Parameters:
- `filepath`: Path where the raw shader file will be created
- `filename`: The name of the shader file to create
- `format`: Pointer to `sge_vertex_format` struct

#### Returns:
- `SGE_SUCCESS` on success
- `SGE_ERROR` if the file creation fails

---

### `SGE_RESULT compile_shader_file(...)`
```c
SGE_RESULT compile_shader_file(char *uncompiled_shader_file, sge_render *render);
```
#### Parameters:
- `uncompiled_shader_file`: Path to the shader file that needs to be compiled
- `render`: Pointer to `sge_render` struct

#### Returns:
- `SGE_SUCCESS` on successful shader compilation
- `SGE_ERROR` if compilation fails

---

## Examples

### Receiving Shader Path

```c
shader_paths[0] = sge_get_vertex_shader_path_for_format(render, format, settings->is_3d);
```

---

### Create Shader

Creating Shader if it doesnt exists, if a faulty exists, vulkan will fail to load, then try to force create a new one if this fails return
```c
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
            log_internal_event(LOG_LEVEL_ERROR, "failed to load shader %s", shader_paths[i]);
            return SGE_ERROR;
      }
}
```

---

All the Functions are not really needed by the dev but are still included bc why not