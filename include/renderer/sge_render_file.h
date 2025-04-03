//
// Created by Geisthardt on 17.03.2025.
//

#ifndef SGE_RENDER_FILE_H
#define SGE_RENDER_FILE_H


#define SGE_REND_MAGIC_NUMBER "SGEREND" //has 0 terminator automatically so 8 bytes
#define SGE_REND_HEADER_FIXED_SIZE 86
#define SGE_REND_SECTION_HEADER_FIXED_SIZE 88
#define SGE_REND_EXTENSION_FIXED_SIZE 6
#include <stdint.h>

typedef enum {
        //Version 0.1
        SGE_SECTION_MESH = 1,
        SGE_SECTION_MATERIAL = 2,
        SGE_SECTION_TEXTURE = 3,
        SGE_SECTION_SHADER_BINDING = 4,
        SGE_SECTION_INDEX_BUFFER = 5,
        SGE_SECTION_METADATA = 6,
} SGE_SELECTION_TYPE;

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

typedef enum { //uint32_t
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

typedef enum {
        SGE_MATERIAL_PARAM_FLOAT = 1,
        SGE_MATERIAL_PARAM_VEC2 = 2,
        SGE_MATERIAL_PARAM_VEC3 = 3,
        SGE_MATERIAL_PARAM_VEC4 = 4,
        SGE_MATERIAL_PARAM_TEXTURE = 5,
} SGE_MATERIAL_PARAM_TYPE;

typedef enum { //uint32_t
        SGE_POLYGON_MODE_FILL = 1,
        SGE_POLYGON_MODE_LINE = 2,
        SGE_POLYGON_MODE_POINT = 3,
        SGE_POLYGON_MAX_ENUM = 0xFFFFFFFF,
} SGE_POLYGON_MODE;

typedef enum { //uint32_t
        SGE_CULL_MODE_NONE = 0,
        SGE_CULL_MODE_FRONT = 1,
        SGE_CULL_MODE_BACK = 2,
        SGE_CULL_MODE_FRONT_AND_BACK = 3,
        SGE_CULL_MODE_MAX_ENUM = 0xFFFFFFFF,
} SGE_CULL_MODE;

typedef enum { //uint32_t
        SGE_FRONT_FACE_CLOCKWISE = 1,
        SGE_FRONT_FACE_COUNTER_CLOCKWISE = 2,
        SGE_FRONT_FACE_MAX_ENUM = 0xFFFFFFFF,
} SGE_FRONT_FACE;

typedef enum {
        SGE_MSAA_COUNT_1_BIT  = 0x00000001,
        SGE_MSAA_COUNT_2_BIT  = 0x00000002,
        SGE_MSAA_COUNT_4_BIT  = 0x00000004,
        SGE_MSAA_COUNT_8_BIT  = 0x00000008,
        SGE_MSAA_COUNT_16_BIT = 0x00000010,
        SGE_MSAA_COUNT_32_BIT = 0x00000020,
        SGE_MSAA_COUNT_64_BIT = 0x00000040,
        SGE_MSAA_MAX_ENUM     = 0x7FFFFFFF,
} SGE_MSAA_COUNT; //sint32_t

typedef struct sge_rend_extension {
        uint16_t                type;
        uint32_t                data_size;
        uint8_t*                data;
} sge_rend_extension;

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

typedef struct sge_mesh_attribute {
        uint16_t                type;
        uint16_t                format;
        uint16_t                components;
        uint16_t                offset;
} sge_mesh_attribute;

typedef struct sge_mesh_data {
        uint32_t                vertex_count;
        uint32_t                vertex_size;
        uint32_t                attribute_count;
        sge_mesh_attribute      *attributes;
        uint8_t                 *vertex_data;
} sge_mesh_data;

typedef struct sge_index_buffer_data {
        uint32_t                index_count;
        uint32_t                primitive_type;
        uint16_t                index_size;
        uint8_t                 *index_data;
} sge_index_buffer_data;

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

typedef struct sge_material_data {
        uint32_t                parameter_count;
        sge_material_parameter  *parameters;
        uint32_t                shader_binding_index;
} sge_material_data;

typedef struct sge_shader_parameter_binding {
        char                    name[32];
        uint16_t                type;
} sge_shader_parameter_binding;

typedef struct sge_shader_binding_data {
        char                            shader_identifier[64];
        uint32_t                        parameter_count;
        sge_shader_parameter_binding    *parameter_bindings;
} sge_shader_binding_data;

typedef struct sge_rend_section {
        sge_rend_section_header section_header;
        void                    *data;
} sge_rend_section;

typedef struct sge_rend_file {
        sge_rend_header         header;
        sge_rend_section        *sections;
} sge_rend_file;

#include "renderer/sge_render.h"

SGE_RESULT sge_rend_save(char *filename, sge_rend_section *sections, uint16_t section_count);
SGE_RESULT sge_rend_load(char *filename, sge_rend_file **outFile);

sge_mesh_data *sge_create_mesh_data();
sge_index_buffer_data *sge_create_index_buffer_data();
sge_material_data *sge_create_material_data();
sge_shader_binding_data *sge_create_shader_binding();

sge_mesh_data *sge_parse_mesh_data(void *raw_data, size_t data_size);
sge_index_buffer_data *sge_parse_index_buffer_data();
sge_material_data *sge_parse_material_data();
sge_shader_binding_data *sge_parse_shader_binding();


sge_rend_section *sge_create_mesh_section(
                char *name,
                void *vertex_data,
                uint32_t vertex_count,
                uint32_t vertex_size,
                sge_mesh_attribute *attributes,
                uint32_t attribute_count,
                sge_rend_extension *extensions,
                uint32_t extension_count);
sge_rend_section sge_create_index_section();
sge_rend_section sge_create_material_section();
sge_rend_section sge_create_shader_binding_section();

#endif //SGE_RENDER_FILE_H
