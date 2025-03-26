//
// Created by Geisthardt on 20.03.2025.
//

#ifndef SGE_SHADER_UTILS_H
#define SGE_SHADER_UTILS_H

#include "../sge_render.h"

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

typedef enum SGE_SHADER_BINDINGS {
        SGE_SHADER_BINDING_UBO_TRANSFORM = 0,
        //add more when needed
} SGE_SHADER_BINDINGS;


SGE_SHADER_LOCATIONS sge_get_location_from_attribute_type(SGE_ATTRIBUTE_TYPE);

char *sge_get_vertex_shader_path_for_format(sge_render *render, sge_vertex_format *format, SGE_BOOL is_3d);
char *sge_get_fragment_shader_path_for_format(sge_render *render, sge_vertex_format *format, SGE_BOOL is_3d);

SGE_RESULT sge_create_shader(char *shader_path, sge_vertex_format *format, sge_render *render);
SGE_RESULT sge_create_shader_if_not_exist(char *shader_path, sge_vertex_format *format, sge_render *render);
SGE_RESULT create_raw_shader_file(char *filepath, char *filename, sge_vertex_format *format);
SGE_RESULT compile_shader_file(char *uncompiled_shader_file, sge_render *render);
#endif //SGE_SHADER_UTILS_H
