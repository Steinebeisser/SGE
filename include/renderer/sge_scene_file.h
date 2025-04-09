//
// Created by Geisthardt on 18.03.2025.
//

#ifndef SGE_SCENE_FILE_H
#define SGE_SCENE_FILE_H


#define SGE_SCENE_MAGIC_NUMBER "SGESCNE" //has 0 terminator automatically so 8 bytes
#define SGE_SCENE_HEADER_FIXED_SIZE 48
#define SGE_SCENE_SECTION_HEADER_FIXED_SIZE 40


#define SGE_TRANSFORMATION_POSITION_SIZE  12 //3x float32 (vec3)
#define SGE_TRANSFORMATION_SCALE_SIZE     12 //3x float32 (vec3)
#define SGE_TRANSFORMATION_ROTATION_SIZE  12 //3x float32 (vec3)


#include "sge_types.h"
#include "sge_render_file.h"

typedef enum SGE_SCENE_SECTION_TYPE {
        SGE_SCENE_SECTION_TYPE_SGEREND = 1,
} SGE_SCENE_SECTION_TYPE;

typedef enum SGE_SCENE_TRANSFORMATION_FLAGS {
        SGE_SCENE_TRANSFORMATION_FLAG_POSITION  = 0x01,
        SGE_SCENE_TRANSFORMATION_FLAG_SCALE     = 0x02,
        SGE_SCENE_TRANSFORMATION_FLAG_ROTATION  = 0x04,
        SGE_SCENE_TRANSFORMATION_FLAG_MAX       = 0xFF,
} SGE_SCENE_TRANSFORMATION_FLAGS;


typedef struct sge_scene_extension {
        uint16_t                type;
        uint32_t                data_size;
        uint8_t*                data;
} sge_scene_extension;

typedef struct sge_scene_header {
        uint16_t                major_version;
        uint16_t                minor_version;
        uint16_t                patch_version;
        uint32_t                section_count;
        uint32_t                scene_name_size;
        char                    *scene_name;
        uint32_t                author_name_size;
        char                    *author_name;
        uint64_t                creation_date_timestamp;
        uint64_t                last_modified_date_timestamp;
        uint32_t                description_size;
        char                    *description;
        uint16_t                header_extension_count;
        sge_scene_extension     *header_extensions;
        size_t                  header_extension_size;
        uint32_t                crc32_checksum;
} sge_scene_header;

typedef enum SGE_SCENE_SGEREND_INCLUDE_TYPE {
        SGE_SCENE_SGEREND_INCLUDE_TYPE_EXTERNAL = 0,
        SGE_SCENE_SGEREND_INCLUDE_TYPE_EMBEDDED = 1,
} SGE_SCENE_SGEREND_INCLUDE_TYPE;

typedef struct sge_scene_sgerend_section {
        uint8_t                 include_type; //SGE_SCENE_SGEREND_INCLUDE_TYPE
        uint16_t                additional_section_count;
        union {
                uint16_t        sgerend_source_size_non_embedded; //if filepath
                uint32_t        sgerend_source_size_embedded; //if embedded
        };
        void                    *sgerend_source_data;
        uint32_t                section_data_size;      //only used if additional section count > 0
        sge_rend_section        *additional_sge_rend_sections;
        uint16_t                transformation_flags; //SGE_SCENE_TRANSFORMATION_FLAGS
        void                    *transformation_data;
} sge_scene_sgerend_section;

typedef union sge_scene_section_data {
        sge_scene_sgerend_section       *sgerend;
} sge_scene_section_data;

typedef struct sge_scene_section_header {
        uint16_t                sge_scene_section_type; //SGE_SCENE_SECTION_TYPE, but force to 2 byte
        uint64_t                section_offset;
        uint64_t                data_size;
        uint32_t                section_name_size;
        char                    *section_name;
        uint64_t                creation_date_timestamp;
        uint64_t                last_modified_date_timestamp;
        uint16_t                section_extension_count;
        size_t                  section_extension_size;
        sge_scene_extension     *extensions;
        uint32_t                crc32_checksum;
        size_t                  header_size;
} sge_scene_section_header;

typedef struct sge_scene_section {
        sge_scene_section_header        *section_header;
        void                            *data;
        sge_scene_section_data          parsed_data;
} sge_scene_section;

typedef struct sge_scene {
        sge_scene_header        header;
        sge_scene_section       *sections;
} sge_scene;

typedef struct sge_scene_settings {
        char    *filename;

} sge_scene_settings;

sge_scene *sge_scene_create(char *scene_name, char *author_name, char *description);


sge_scene_section *sge_scene_create_sgerend_section(
                        SGE_SCENE_SGEREND_INCLUDE_TYPE include_type,
                        char            *section_name,
                        void            *source_data,
                        size_t          source_data_size,
                        uint16_t        transformation_flags,
                        void            *transformation_data);


SGE_RESULT sge_scene_add_section(sge_scene *scene, sge_scene_section *section);


SGE_RESULT sge_scene_save(char *filename, sge_scene *scene);
sge_scene *sge_scene_load(char *filename);

SGE_RESULT sge_scene_parse_sgerend_section(sge_scene_section_data *parsed_output, void *data, size_t data_size);

#endif //SGE_SCENE_FILE_H
