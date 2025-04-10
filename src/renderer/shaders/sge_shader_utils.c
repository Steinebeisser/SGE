//
// Created by Geisthardt on 20.03.2025.
//

#include "renderer/shader/sge_shader_utils.h"

#include "core/sge_internal_logging.h"
#include "core/memory_control.h"
#include "utils/sge_file.h"
#include "utils/sge_string.h"

#define SHADER_PATH_NAME "Shaders"

//255 max filename length for unxi and mac


SGE_SHADER_LOCATIONS sge_get_location_from_attribute_type(SGE_ATTRIBUTE_TYPE type) {
        switch (type) {
                case SGE_ATTRIBUTE_POSITION: {
                        return SGE_SHADER_POSITION;
                }
                case SGE_ATTRIBUTE_COLOR: {
                        return SGE_SHADER_COLOR;
                }
                case SGE_ATTRIBUTE_NORMAL: {
                        return SGE_SHADER_NORMAL;
                }
                case SGE_ATTRIBUTE_TANGENT: {
                        return SGE_SHADER_TANGENT;
                }
                case SGE_ATTRIBUTE_WEIGHTS: {
                        return SGE_SHADER_WEIGHTS;
                }
                case SGE_ATTRIBUTE_JOINTIDS: {
                        return SGE_SHADER_JOINTIDS;
                }
                case SGE_ATTRIBUTE_BITANGENT: {
                        return SGE_SHADER_BITANGENT;
                }
                case SGE_ATTRIBUTE_TEXCOORDS: {
                        return SGE_SHADER_TEXCOORDS;
                }
        }
        return SGE_SHADER_UNKNOWN;
}


char *sge_get_vertex_shader_path_for_format(sge_render *render, sge_vertex_format *format, SGE_BOOL is_3d) {
        log_internal_event(LOG_LEVEL_INFO, "getting vertex shader path");
        char filename[255] = {0};
        char *cwd = get_current_working_directory();
        char *path = allocate_memory(1024, MEMORY_TAG_SHADER);
        if (path == NULL) {
                log_internal_event(LOG_LEVEL_FATAL, "failed to allocate for vert shader path");
                return NULL;
        }

        //add mroe

        if (format->attribute_count == 0) {
                log_internal_event(LOG_LEVEL_FATAL, "no attributes for shader");
                return path;
        }

        if (is_3d) {
                strcat(filename, "3d_");
        } else {
                strcat(filename, "2d_");
        }

        for (int i = 0; i < format->attribute_count; ++i) {
                sge_vertex_attribute attribute = format->attributes[i];
                switch (attribute.location) {
                        case SGE_SHADER_POSITION: {
                                strcat(filename, "pos");
                        }break;
                        case SGE_SHADER_COLOR: {
                                strcat(filename, "col");
                        } break;
                        default: {
                                continue;
                        }
                }

                snprintf(filename + strlen(filename), sizeof(filename), "%d", attribute.components);

                if (i != format->attribute_count -1 ) {
                       strcat(filename, "_");
                }
        }

        char file_ending[16] = "vert";

        if (render->api == SGE_RENDER_API_VULKAN) {
                strcat(file_ending, ".spv");
        } else {
                log_internal_event(LOG_LEVEL_FATAL, "unknown api");
                return NULL;
        }
        snprintf(filename + strlen(filename), sizeof(filename), ".%s", file_ending);


        sprintf(path, "%s\\%s\\%s", cwd, SHADER_PATH_NAME, filename);

        log_internal_event(LOG_LEVEL_INFO, "choose %s as vert filepath", path);

        return path;

}
char *sge_get_fragment_shader_path_for_format(sge_render *render, sge_vertex_format *format, SGE_BOOL is_3d) {
        char filename[255] = {0};
        char *cwd = get_current_working_directory();
        char *path = allocate_memory(1024, MEMORY_TAG_SHADER);
        if (path == NULL) {
                log_internal_event(LOG_LEVEL_FATAL, "failed to allocate for frag shader path");
                return NULL;
        }

        if (format->attribute_count == 0) {
                log_internal_event(LOG_LEVEL_FATAL, "no attributes for shader");
                return path;
        }

        if (is_3d) {
                strcat(filename, "3d_");
        } else {
                strcat(filename, "2d_");
        }

        for (int i = 0; i < format->attribute_count; ++i) {
                sge_vertex_attribute attribute = format->attributes[i];
                switch (attribute.location) {
                        case SGE_SHADER_POSITION: {
                                strcat(filename, "pos");
                        } break;
                        case SGE_SHADER_COLOR: {
                                strcat(filename, "col");
                        } break;
                        default: {
                                continue;
                        }
                }

                snprintf(filename + strlen(filename), sizeof(filename), "%d", attribute.components);

                if (i != format->attribute_count - 1) {
                        strcat(filename, "_");
                }
        }

        char file_ending[16] = "frag";

        if (render->api == SGE_RENDER_API_VULKAN) {
                strcat(file_ending, ".spv");
        } else {
                log_internal_event(LOG_LEVEL_FATAL, "unknown api");
                return NULL;
        }
        snprintf(filename + strlen(filename), sizeof(filename), ".%s", file_ending);

        sprintf(path, "%s\\%s\\%s", cwd, SHADER_PATH_NAME, filename);

        log_internal_event(LOG_LEVEL_INFO, "choose %s as frag filepath", path);

        return path;
}


SGE_RESULT sge_create_shader_if_not_exist(char *shader_path, sge_vertex_format *format, sge_render *render) {
        if (shader_path == NULL) {
                log_internal_event(LOG_LEVEL_ERROR, "tried creating shader but no path given");
                return SGE_ERROR;
        }

        if (sge_file_exists(shader_path)) {
                log_internal_event(LOG_LEVEL_INFO, "shader exists: %s", shader_path);
                return SGE_SUCCESS;
        }

        log_internal_event(LOG_LEVEL_INFO, "shader no existi, creating");

        if (!sge_create_shader(shader_path, format, render)) {
                return SGE_ERROR;
        }
        return SGE_SUCCESS;
}

SGE_RESULT sge_create_shader(char *shader_path, sge_vertex_format *format, sge_render *render) {
        char filename[255] = {0};
        char filepath[769] = {0};
        size_t last_slash_index = 0;

        get_last_string_index(shader_path, '\\', &last_slash_index);
        strncpy(filename, shader_path + last_slash_index+1, strlen(shader_path) - last_slash_index-1);
        log_internal_event(LOG_LEVEL_INFO, "File name: %s", filename);

        strncpy(filepath, shader_path, last_slash_index);
        log_internal_event(LOG_LEVEL_INFO, "Path where file is: %s", filepath);

        if (!create_directory_if_not_exists(filepath)) {
                log_internal_event(LOG_LEVEL_ERROR, "failed to create dir");
                return SGE_ERROR;
        }

        char ending[128] = {0};
        get_file_ending(filename, ending);
        log_internal_event(LOG_LEVEL_INFO, "File ending: %s", ending);

        char filename_no_ending[255] = {0};
        strncpy(filename_no_ending, filename, strlen(filename) - 1 - strlen(ending)); //-1 = .

        char file_shader_no_compiled[1024] = {0};
        sprintf(file_shader_no_compiled, "%s\\%s", filepath, filename_no_ending);
        log_internal_event(LOG_LEVEL_INFO, "Raw shader file: %s", file_shader_no_compiled);

        if (!sge_file_exists(file_shader_no_compiled)) {
                if (create_raw_shader_file(file_shader_no_compiled, filename_no_ending, format) != SGE_SUCCESS) {
                        return SGE_ERROR;
                }
        }

        if (compile_shader_file(file_shader_no_compiled, render) != SGE_SUCCESS) {
                log_internal_event(LOG_LEVEL_ERROR, "failed to compile shader, trying to rebuild raw shader file");
                if (create_raw_shader_file(file_shader_no_compiled, filename_no_ending, format) != SGE_SUCCESS) {
                        log_internal_event(LOG_LEVEL_ERROR, "failed to build raw shader file");
                        return SGE_ERROR;
                }
                if (compile_shader_file(file_shader_no_compiled, render) != SGE_SUCCESS) {
                        log_internal_event(LOG_LEVEL_ERROR, "failed to compile rebuild shader");
                        return SGE_ERROR;
                }
        }

        return SGE_SUCCESS;
}

SGE_RESULT create_raw_shader_file(char *filepath, char *filename, sge_vertex_format *format) {
        char ending[128] = {0};
        get_file_ending(filename, ending);
        log_internal_event(LOG_LEVEL_INFO, "File ending: %s", ending);
        SGE_BOOL is_vert = strcmp(ending, "vert") == 0 ? SGE_TRUE : SGE_FALSE;
        SGE_BOOL is_frag = strcmp(ending, "frag") == 0 ? SGE_TRUE : SGE_FALSE;

        FILE *fd = fopen(filepath, "w");
        if (fd == NULL) {
                log_internal_event(LOG_LEVEL_ERROR, "Failed to open file to write: %s", filepath);
                return SGE_ERROR;
        }

        if (is_vert) {
                fprintf(fd, "#version 450\n\n");

                for (size_t i = 0; i < format->attribute_count; ++i) {
                        sge_vertex_attribute attr = format->attributes[i];
                        char layout_string[128] = {0};

                        snprintf(layout_string, sizeof(layout_string), "layout(location = %i) in ",  attr.location);

                        switch (attr.components) {
                                case 1: {
                                        strcat(layout_string, "float");
                                } break;
                                case 2: {
                                        strcat(layout_string, "vec2");
                                } break;
                                case 3: {
                                        strcat(layout_string, "vec3");
                                } break;
                                case 4: {
                                        strcat(layout_string, "vec4");
                                } break;
                                default: {
                                        log_internal_event(LOG_LEVEL_ERROR, "unsupported component count in shader creation");
                                        return SGE_ERROR;
                                };
                        }


                        switch (attr.location) {
                                case SGE_SHADER_POSITION: {
                                        strcat(layout_string, " inPosition");
                                } break;
                                case SGE_SHADER_COLOR: {
                                        strcat(layout_string, " inColor");
                                } break;
                                default: {
                                        log_internal_event(LOG_LEVEL_ERROR, "unsupported location attribute");
                                        return SGE_ERROR;
                                };
                        }

                        strcat(layout_string, ";\n");

                        fprintf(fd, layout_string);
                }


                char ubo[128] = {0};
                snprintf(ubo, sizeof(ubo), "\nlayout(binding=%d) uniform UniformBufferObject {\n"
                            "   mat4 model;\n"
                            "   mat4 view;\n"
                            "   mat4 proj;\n"
                            "} ubo;\n\n", SGE_SHADER_BINDING_UBO_TRANSFORM);

                fprintf(fd, ubo);

                for (size_t i = 0; i < format->attribute_count; ++i) {
                        sge_vertex_attribute attr = format->attributes[i];
                        char layout_string[128] = {0};

                        if (attr.location == SGE_SHADER_POSITION) continue;

                        snprintf(layout_string, sizeof(layout_string), "layout(location = %i) out ",  attr.location);

                        switch (attr.components) {
                                case 1: {
                                        strcat(layout_string, "float");
                                } break;
                                case 2: {
                                        strcat(layout_string, "vec2");
                                } break;
                                case 3: {
                                        strcat(layout_string, "vec3");
                                } break;
                                case 4: {
                                        strcat(layout_string, "vec4");
                                } break;
                                default: {
                                        log_internal_event(LOG_LEVEL_ERROR, "unsupported component count in shader creation");
                                        return SGE_ERROR;
                                };
                        }


                        switch (attr.location) {
                                case SGE_SHADER_COLOR: {
                                        strcat(layout_string, " fragColor");
                                } break;
                                default: {
                                        log_internal_event(LOG_LEVEL_ERROR, "unsupported location attribute: %d", attr.location);
                                        return SGE_ERROR;
                                };
                        }

                        strcat(layout_string, ";\n");

                        fprintf(fd, "%s", layout_string);
                }

                char main_string[256] = {0};

                strcat(main_string, "\nvoid main() {\n");

                sge_vertex_attribute *pos_attr = NULL;
                for (size_t i = 0; i < format->attribute_count; ++i) {
                        if (format->attributes[i].location == SGE_SHADER_POSITION) {
                                pos_attr = &format->attributes[i];
                                break;
                        }
                }

                if (pos_attr) {
                        switch (pos_attr->components) {
                                case 2: {
                                        strcat(main_string, "    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 0.0, 1.0);\n");
                                } break;
                                case 3: {
                                        strcat(main_string, "    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);\n");
                                } break;
                                case 4: {
                                        strcat(main_string, "    gl_Position = ubo.proj * ubo.view * ubo.model * inPosition;\n");
                                }break;
                                default: {
                                        log_internal_event(LOG_LEVEL_ERROR, "Invalid position component count: %u", pos_attr->components);
                                        return SGE_ERROR;
                                }
                        }
                } else {
                        log_internal_event(LOG_LEVEL_ERROR, "No position attribute found in vertex format");
                        return SGE_ERROR;
                }

                for (size_t i = 0; i < format->attribute_count; ++i) {
                        sge_vertex_attribute attr = format->attributes[i];
                        if (attr.location == SGE_SHADER_POSITION) continue;

                        switch (attr.location) {
                                case SGE_SHADER_COLOR: {
                                        strcat(main_string, "    fragColor = inColor;\n");
                                } break;
                                default: {
                                        log_internal_event(LOG_LEVEL_ERROR, "unsupported attribute for shader creation");
                                        return SGE_ERROR;
                                };
                        }
                }

                strcat(main_string, "}\n");

                fprintf(fd, "%s", main_string);

        } else if (is_frag) {
                fprintf(fd, "#version 450\n\n");

                SGE_BOOL has_non_position_attribute = SGE_FALSE;
                for (size_t i = 0; i < format->attribute_count; ++i) {
                        if (format->attributes[i].location != SGE_SHADER_POSITION) {
                            has_non_position_attribute = SGE_TRUE;
                            break;
                        }
                }

                if (has_non_position_attribute) {
                        for (size_t i = 0; i < format->attribute_count; ++i) {
                                sge_vertex_attribute attr = format->attributes[i];
                                if (attr.location == SGE_SHADER_POSITION) continue;

                                char layout_string[128] = {0};
                                snprintf(layout_string, sizeof(layout_string), "layout(location = %i) in ", attr.location);

                                switch (attr.components) {
                                        case 1: strcat(layout_string, "float"); break;
                                        case 2: strcat(layout_string, "vec2"); break;
                                        case 3: strcat(layout_string, "vec3"); break;
                                        case 4: strcat(layout_string, "vec4"); break;
                                        default: {
                                                log_internal_event(LOG_LEVEL_ERROR, "unsupported component count in shader creation");
                                                return SGE_ERROR;
                                        }
                                }

                                switch (attr.location) {
                                        case SGE_SHADER_COLOR: strcat(layout_string, " fragColor"); break;
                                        default: {
                                                log_internal_event(LOG_LEVEL_ERROR, "unsupported location attribute");
                                                return SGE_ERROR;
                                        }
                                }

                                strcat(layout_string, ";\n");
                                fprintf(fd, "%s", layout_string);
                        }

                        fprintf(fd, "layout(location = 0) out vec4 outColor;\n\n");
                        fprintf(fd, "void main() {\n");
                        fprintf(fd, "    outColor = fragColor;\n");
                        fprintf(fd, "}\n");
                } else {
                        fprintf(fd, "layout(location = 0) out vec4 outColor;\n\n");
                        fprintf(fd, "void main() {\n");
                        fprintf(fd, "    outColor = vec4(1.0, 1.0, 1.0, 1.0);\n");
                        fprintf(fd, "}\n");
                }
        }else {
                log_internal_event(LOG_LEVEL_ERROR, "unsupported shader type");
                return SGE_ERROR;
        }


        fclose(fd);

        return SGE_SUCCESS;
}

SGE_RESULT compile_shader_file(char *uncompiled_shader_file_path, sge_render *render) {
        SGE_RENDER_API api = render->api;

        if (api == SGE_RENDER_API_VULKAN) {
                char spv_path[1024] = {0};
                snprintf(spv_path, sizeof(spv_path), "%s.spv", uncompiled_shader_file_path);

                char *vulkan_sdk = getenv("VULKAN_SDK");
                if (vulkan_sdk == NULL) {
                        printf("Error: VULKAN_SDK environment variable not set.\n");
                        return 1;
                }

                char command[1024] = {0};
#ifdef WIN32
                snprintf(command, sizeof(command), "%s\\Bin\\glslc.exe %s -o %s", vulkan_sdk, uncompiled_shader_file_path, spv_path);
#elif UNIX
                log_internal_event(LOG_LEVEL_FATAL, "unsupported os");
                return SGE_ERROR;
#else
                log_internal_event(LOG_LEVEL_FATAL, "unsupported os");
                return SGE_ERROR;
#endif

                int result = system(command);
                if (result != 0) {
                        log_internal_event(LOG_LEVEL_ERROR, "failed to compile shader");
                        return SGE_ERROR;
                }
        } else {
                log_internal_event(LOG_LEVEL_ERROR, "unsupported api fpr shader compiling");
                return SGE_ERROR;
        }

        log_internal_event(LOG_LEVEL_INFO, "compiled shader");
        return SGE_SUCCESS;
}

SGE_RESULT create_vertex_shader_from_path();
SGE_RESULT create_fragment_shader_from_path();