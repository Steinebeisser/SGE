# SGE RENDER FILE MODULE

## Overview

The SGE Render file module is used to create and load `.sgerend` (as defined in [sgerend documentation.md](files/sgerend%20documentation.md)) files for storing render objects with all their data.

## Table of Content

1. [Structures](#structures)
   1.1 [sge_rend_extension](#sge_rend_extension) <br>
   1.2 [sge_rend_header](#sge_rend_header) <br>
   1.3 [sge_rend_section_header](#sge_rend_section_header) <br>
   1.4 [sge_mesh_attribute](#sge_mesh_attribute) <br>
   1.5 [sge_mesh_data](#sge_mesh_data) <br>
   1.6 [sge_index_buffer_data](#sge_index_buffer_data) <br>
   1.7 [sge_material_parameter](#sge_material_parameter) <br>
   1.8 [sge_material_data](#sge_material_data) <br>
   1.9 [sge_shader_parameter_binding](#sge_shader_parameter_binding) <br>
   1.10 [sge_shader_binding_data](#sge_shader_binding_data) <br>
   1.11 [sge_rend_section](#sge_rend_section) <br>
   1.12 [sge_rend_file](#sge_rend_file) <br>
2. [Enumerations](#enumerations)
   2.1 [SGE_SELECTION_TYPE](#SGE_SELECTION_TYPE) <br>
   2.2 [SGE_FORMAT_TYPE](#SGE_FORMAT_TYPE) <br>
   2.3 [SGE_ATTRIBUTE_TYPE](#SGE_ATTRIBUTE_TYPE) <br>
   2.4 [SGE_PRIMITIVE_TYPE](#SGE_PRIMITIVE_TYPE) <br>
   2.5 [SGE_MATERIAL_PARAM_TYPE](#SGE_MATERIAL_PARAM_TYPE) <br>
   2.6 [SGE_POLYGON_MODE](#SGE_POLYGON_MODE) <br>
   2.7 [SGE_CULL_MODE](#SGE_CULL_MODE) <br>
   2.8 [SGE_FRONT_FACE](#SGE_FRONT_FACE) <br>
   2.9 [SGE_MSAA_COUNT](#SGE_MSAA_COUNT) <br>
3. [Defines](#defines)
   3.1 [SGE_REND_MAGIC_NUMBER](#SGE_REND_MAGIC_NUMBER) <br>
   3.2 [SGE_REND_HEADER_FIXED_SIZE](#SGE_REND_HEADER_FIXED_SIZE) <br>
   3.3 [SGE_REND_SECTION_HEADER_FIXED_SIZE](#SGE_REND_SECTION_HEADER_FIXED_SIZE) <br>
   3.4 [SGE_REND_EXTENSION_FIXED_SIZE](#SGE_REND_EXTENSION_FIXED_SIZE) <br>
4. [API Reference](#api-reference)
   4.1 [SGE_RESULT sge_rend_save(...)](#sge_result-sge_rend_save) <br>
   4.2 [SGE_RESULT sge_rend_load(...)](#sge_result-sge_rend_load) <br>
   4.3 [sge_mesh_data *sge_parse_mesh_data(...)](#sge_mesh_data-sge_parse_mesh_data) <br>
   4.4 [sge_rend_section *sge_create_mesh_section(...)](#sge_rend_section-sge_create_mesh_section) <br>
5. [Examples](#examples)
   5.1 [Defining Vertex Format](#defining-vertex-format) <br>
   5.2 [Setting Mesh Attributes](#sge_mesh_attribute) <br>
   5.3 [Defining Vertices](#defining-vertices) <br>
   5.4 [Creating a mesh section](#creating-a-mesh-section) <br>
   5.5 [Bundling all sections in an array](#bundling-all-sections-in-an-array) <br>
   5.6 [Creating a file](#creating-a-file) <br>
   5.7 [Loading a file](#loading-a-file) <br>

## Structures

### sge_rend_extension
```c
typedef struct sge_rend_extension {
        uint16_t                type;
        uint32_t                data_size;
        uint8_t*                data;
} sge_rend_extension;
```  
---  

### sge_rend_header
```c
typedef struct sge_rend_header {
        uint16_t                major_version;
        uint16_t                minor_version;
        uint16_t                patch_version;
        uint16_t                section_count;
        uint16_t                extension_count;
        sge_rend_extension      *extensions;
        char                    name[64];
        uint32_t                checksum;
} sge_rend_header;
```  
---  

### sge_rend_section_header
```c
typedef struct sge_rend_section_header {
        uint16_t                type;
        uint64_t                offset;
        uint64_t                data_size;
        uint16_t                extension_count;
        size_t                  extension_size;
        sge_rend_extension      *extensions;
        char                    name[64];
        uint32_t                checksum;
} sge_rend_section_header;
```  
---  

### sge_mesh_attribute
```c
typedef struct sge_mesh_attribute {
        uint16_t                type;
        uint16_t                format;
        uint16_t                components;
        uint16_t                offset;
} sge_mesh_attribute;
```  
---  

### sge_mesh_data
```c
typedef struct sge_mesh_data {
        uint32_t                vertex_count;
        uint32_t                vertex_size;
        uint32_t                attribute_count;
        sge_mesh_attribute      *attributes;
        uint8_t                 *vertex_data;
} sge_mesh_data;
```  
---  

### sge_index_buffer_data
```c
typedef struct sge_index_buffer_data {
        uint32_t                index_count;
        uint32_t                primitive_type;
        uint16_t                index_size;
        uint8_t                 *index_data;
} sge_index_buffer_data;
```  
---  

### sge_material_parameter
```c
typedef struct sge_material_parameter {
        uint16_t                type;
        char                    name[32];
        union {
                float           float_value;
                float           vec2_value[2];
                float           vec3_value[3];
                float           vec4_value[4];
                uint32_t        texture_index;
        } value;
} sge_material_parameter;
```  
---  

### sge_material_data
```c
typedef struct sge_material_data {
        uint32_t                parameter_count;
        sge_material_parameter  *parameters;
        uint32_t                shader_binding_index;
} sge_material_data;
```  
---  

### sge_shader_parameter_binding
```c
typedef struct sge_shader_parameter_binding {
        char                    name[32];
        uint16_t                type;
} sge_shader_parameter_binding;
```  
---  

### sge_shader_binding_data
```c
typedef struct sge_shader_binding_data {
        char                            shader_identifier[64];
        uint32_t                        parameter_count;
        sge_shader_parameter_binding    *parameter_bindings;
} sge_shader_binding_data;
```  
---  

### sge_rend_section
```c
typedef struct sge_rend_section {
        sge_rend_section_header section_header;
        void                    *data;
} sge_rend_section;
```  
---  

### sge_rend_file
```c
typedef struct sge_rend_file {
        sge_rend_header         header;
        sge_rend_section        *sections;
} sge_rend_file;
```  
---  

## Enumerations

### SGE_SELECTION_TYPE
```c
typedef enum {
        SGE_SECTION_MESH = 1,
        SGE_SECTION_MATERIAL = 2,
        SGE_SECTION_TEXTURE = 3,
        SGE_SECTION_SHADER_BINDING = 4,
        SGE_SECTION_INDEX_BUFFER = 5,
        SGE_SECTION_METADATA = 6,
} SGE_SELECTION_TYPE;
```  
---  

### SGE_FORMAT_TYPE
```c
typedef enum {
        SGE_FORMAT_FLOAT32 = 1,
        SGE_FORMAT_FLOAT16 = 2,
        SGE_FORMAT_INT8 = 3,
        SGE_FORMAT_UINT8 = 4,
        SGE_FORMAT_INT16 = 5,
        SGE_FORMAT_UINT16 = 6,
        SGE_FORMAT_INT32 = 7,
        SGE_FORMAT_UINT32 = 8,
} SGE_FORMAT_TYPE;
```  
---  

### SGE_ATTRIBUTE_TYPE
```c
typedef enum {
        SGE_ATTRIBUTE_POSITION = 1,
        SGE_ATTRIBUTE_NORMAL = 2,
        SGE_ATTRIBUTE_COLOR = 3,
        SGE_ATTRIBUTE_TEXCOORDS = 4,
        SGE_ATTRIBUTE_TANGENT = 5,
        SGE_ATTRIBUTE_BITANGENT = 6,
        SGE_ATTRIBUTE_WEIGHTS = 7,
        SGE_ATTRIBUTE_JOINTIDS = 8,
} SGE_ATTRIBUTE_TYPE;
```  
---  

### SGE_PRIMITIVE_TYPE
```c
typedef enum { // uint32_t
        SGE_PRIMITIVE_POINTS = 1,
        SGE_PRIMITIVE_LINES = 2,
        SGE_PRIMITIVE_LINE_STRIP = 3,
        SGE_PRIMITIVE_TRIANGLE = 4,
        SGE_PRIMITIVE_TRIANGLE_STRIP = 5,
        SGE_PRIMITIVE_TRIANGLE_FAN = 6,
        SGE_PRIMITIVE_PATCH_LIST = 7,
        SGE_PRIMITIVE_LINES_ADJACENCY = 8,
        SGE_PRIMITIVE_LINE_STRIP_ADJACENCY = 9,
        SGE_PRIMITIVE_TRIANGLES_ADJACENCY = 10,
        SGE_PRIMITIVE_TRIANGLE_STRIP_ADJACENCY = 11,
        SGE_PRIMITIVE_MAX_ENUM = 0xFFFFFFFF,
} SGE_PRIMITIVE_TYPE;
```  
---  

### SGE_MATERIAL_PARAM_TYPE
```c
typedef enum {
        SGE_MATERIAL_PARAM_FLOAT = 1,
        SGE_MATERIAL_PARAM_VEC2 = 2,
        SGE_MATERIAL_PARAM_VEC3 = 3,
        SGE_MATERIAL_PARAM_VEC4 = 4,
        SGE_MATERIAL_PARAM_TEXTURE = 5,
} SGE_MATERIAL_PARAM_TYPE;
```  
---  

### SGE_POLYGON_MODE
```c
typedef enum { // uint32_t
        SGE_POLYGON_MODE_FILL = 1,
        SGE_POLYGON_MODE_LINE = 2,
        SGE_POLYGON_MODE_POINT = 3,
        SGE_POLYGON_MAX_ENUM = 0xFFFFFFFF,
} SGE_POLYGON_MODE;
```  
---  

### SGE_CULL_MODE
```c
typedef enum { // uint32_t
        SGE_CULL_MODE_NONE = 0,
        SGE_CULL_MODE_FRONT = 1,
        SGE_CULL_MODE_BACK = 2,
        SGE_CULL_MODE_FRONT_AND_BACK = 3,
        SGE_CULL_MODE_MAX_ENUM = 0xFFFFFFFF,
} SGE_CULL_MODE;
```  
---  

### SGE_FRONT_FACE
```c
typedef enum { // uint32_t
        SGE_FRONT_FACE_CLOCKWISE = 1,
        SGE_FRONT_FACE_COUNTER_CLOCKWISE = 2,
        SGE_FRONT_FACE_MAX_ENUM = 0xFFFFFFFF,
} SGE_FRONT_FACE;
```  
---  

### SGE_MSAA_COUNT
```c
typedef enum {
        SGE_MSAA_COUNT_1_BIT  = 0x00000001,
        SGE_MSAA_COUNT_2_BIT  = 0x00000002,
        SGE_MSAA_COUNT_4_BIT  = 0x00000004,
        SGE_MSAA_COUNT_8_BIT  = 0x00000008,
        SGE_MSAA_COUNT_16_BIT = 0x00000010,
        SGE_MSAA_COUNT_32_BIT = 0x00000020,
        SGE_MSAA_COUNT_64_BIT = 0x00000040,
        SGE_MSAA_MAX_ENUM     = 0x7FFFFFFF,
} SGE_MSAA_COUNT;
```  
---  

## Defines

### SGE_REND_MAGIC_NUMBER

```c
#define SGE_REND_MAGIC_NUMBER "SGEREND" // has 0 terminator automatically so 8 bytes
```

---

### SGE_REND_HEADER_FIXED_SIZE

```c
#define SGE_REND_HEADER_FIXED_SIZE 86
```

---

### SGE_REND_SECTION_HEADER_FIXED_SIZE

```c
#define SGE_REND_SECTION_HEADER_FIXED_SIZE 88
```

---

### SGE_REND_EXTENSION_FIXED_SIZE

```c
#define SGE_REND_EXTENSION_FIXED_SIZE 6
```

---

## API Reference

### SGE_RESULT sge_rend_save(...)

```c
SGE_RESULT sge_rend_save(char *filename, sge_rend_section *sections, uint16_t section_count);
```

#### Parameters:
- `filename`: filename the file should be named
- `sections`: Array of `sge_rend_section` structures representing the render sections
- `section_count`: Number of sections in the array

#### Results:

---  

### `SGE_RESULT sge_rend_load(...)`

```c
SGE_RESULT sge_rend_load(char *filename, sge_rend_file **outFile);
```

#### Parameters:
- `filename`: `.sgerend` file to be loaded.
- `outFile`: Pointer to a `sge_rend_file` structure to store the loaded file data.

#### Results:

---  

### `sge_mesh_data *sge_parse_mesh_data(...)`

```c
(void *raw_data, size_t data_size);
```

#### Parameters:
- `raw_data`: Pointer to raw mesh data
- `data_size`: Size of the mesh data in bytes

#### Results:

---  

### `sge_rend_section *sge_create_mesh_section(...)`

```c
sge_rend_section *sge_create_mesh_section(
    char *name, 
    void *vertex_data, 
    uint32_t vertex_count, 
    uint32_t vertex_size, 
    sge_mesh_attribute *attributes, 
    uint32_t attribute_count, 
    sge_rend_extension *extensions, 
    uint32_t extension_count);`  
```

#### Parameters:
- `name`: Name of the mesh section for identification
- `vertex_data`: Pointer to vertex data
- `vertex_count`: Number of vertices
- `vertex_size`: Size of each vertex in bytes
- `attributes`: Array of `sge_mesh_attribute` defining the vertex format.
- `attribute_count`: Number of attributes in the array
- `extensions`: Array of `sge_rend_extension` structures
- `extension_count`: Number of extensions

#### Results:
- Returns a pointer to allocated `sge_rend_section` containing the mesh data

---  

## Examples

### Defining Vertex Format
```c
typedef struct {
        float position[3];
        uint8_t color[4];
} ColoredVertex;
```

### Setting Mesh Attributes
```c
sge_mesh_attribute attributes[2] = {
        {
                .type = SGE_ATTRIBUTE_POSITION,
                .format = SGE_FORMAT_FLOAT32,
                .components = 3,
                .offset = 0,
        },
        {
                .type = SGE_ATTRIBUTE_COLOR,
                .format = SGE_FORMAT_UINT8,
                .components = 4,
                .offset = 12
        }
};
```

### Defining Vertices
```c
ColoredVertex vertices[] = {
        // Base triangle 1 (flipped base)
        { .position = {-0.5f, 0.0f, -0.5f}, .color = {255, 0, 0, 255} },   // Bottom-left
        { .position = { 0.5f, 0.0f,  0.5f}, .color = {0, 0, 255, 255} },   // Top-right
        { .position = { 0.5f, 0.0f, -0.5f}, .color = {0, 255, 0, 255} },   // Bottom-right

        // Base triangle 2
        { .position = {-0.5f, 0.0f, -0.5f}, .color = {255, 0, 0, 255} },
        { .position = {-0.5f, 0.0f,  0.5f}, .color = {255, 255, 0, 255} }, // Top-left
        { .position = { 0.5f, 0.0f,  0.5f}, .color = {0, 0, 255, 255} },

        // Side triangle 1
        { .position = {-0.5f, 0.0f, -0.5f}, .color = {255, 0, 0, 255} },
        { .position = { 0.0f, -1.0f,  0.0f}, .color = {255, 255, 255, 255} }, // Peak (negative Y)
        { .position = { 0.5f, 0.0f, -0.5f}, .color = {0, 255, 0, 255} },

        // Side triangle 2
        { .position = { 0.5f, 0.0f, -0.5f}, .color = {0, 255, 0, 255} },
        { .position = { 0.0f, -1.0f,  0.0f}, .color = {255, 255, 255, 255} },
        { .position = { 0.5f, 0.0f,  0.5f}, .color = {0, 0, 255, 255} },

        // Side triangle 3
        { .position = { 0.5f, 0.0f,  0.5f}, .color = {0, 0, 255, 255} },
        { .position = { 0.0f, -1.0f,  0.0f}, .color = {255, 255, 255, 255} },
        { .position = {-0.5f, 0.0f,  0.5f}, .color = {255, 255, 0, 255} },

        // Side triangle 4
        { .position = {-0.5f, 0.0f,  0.5f}, .color = {255, 255, 0, 255} },
        { .position = { 0.0f, -1.0f,  0.0f}, .color = {255, 255, 255, 255} },
        { .position = {-0.5f, 0.0f, -0.5f}, .color = {255, 0, 0, 255} },
};
```

### Creating a mesh section
```c
sge_rend_section *triangle_test_mesh_section = sge_create_mesh_section(
        "test triangle",   //Mesh Name
        vertices,          //the verticies defined
        18,                //Vertex Amount
        16,                //Vertex Size
        attributes,        //Attributes Array
        2,                 //Attributes Count
        NULL,              //Extensions Array
        0                  //Extensions Amount
);
```

### Bundling all sections in an array
```c
sge_rend_section sections[] = { *triangle_test_mesh_section };
int section_count = sizeof(sections)/sizeof(sections[0]);
```

### Creating a file
```c
SGE_RESULT result = sge_rend_save("test", sections, section_count);
if (result != SGE_SUCCESS) {
        terminate_program();
}
```

### Loading a file
```c
sge_rend_file *pyramid_file = NULL;
sge_rend_load("test.sgerend", &pyramid_file);
if (!pyramid_file) {
        terminate_program();
}
```
