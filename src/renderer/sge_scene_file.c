//
// Created by Geisthardt on 18.03.2025.
//

#include "renderer/sge_scene_file.h"

#include <sge_version.h>
#include <stdio.h>
#include <string.h>
#include <core/memory_control.h>
#include <core/sge_internal_logging.h>
#include <utils/sge_file.h>
#include <utils/sge_time.h>
#include <utils/hash/crc32.h>


SGE_RESULT sge_scene_save(char *filename, sge_scene_header *scene_header, sge_scene_section *sections) {
        char save_filename[512];
        zero_memory(save_filename, sizeof(save_filename), 0);
        strcat(save_filename, filename);
        strcat(save_filename, ".sgescne");

        //todo make sure file not exist, make savepath configurable
        FILE *fd = fopen(save_filename, "wb");
        if (!fd) {
                log_internal_event(LOG_LEVEL_FATAL, "failed to open file");
                return SGE_ERROR;
        }

        SGE_VERSION current_version = get_sge_version();

        uint16_t major_version = current_version.major;
        uint16_t minor_version = current_version.minor;
        uint16_t patch_version = current_version.patch;

        void *global_header = allocate_memory(SGE_SCENE_HEADER_FIXED_SIZE + scene_header->header_extension_size + scene_header->scene_name_size + scene_header->author_name_size + scene_header->description_size, MEMORY_TAG_RENDERER);
        if (global_header == NULL) {
                log_internal_event(LOG_LEVEL_FATAL, "FAiled to allocate for file save header");
                return SGE_ERROR_FAILED_ALLOCATION;
        }


        size_t file_offset = 0;

        //GLOBAL HEADER                                                                                                                               //
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        copy_memory(global_header, SGE_SCENE_MAGIC_NUMBER, sizeof(SGE_SCENE_MAGIC_NUMBER), file_offset, 0);                                             //
        file_offset += sizeof(SGE_SCENE_MAGIC_NUMBER);                                                                                                 //
        //VERSION                                                                                                                                     //
        copy_memory(global_header, &major_version, sizeof(uint16_t), file_offset, 0);                                                                 //
        file_offset += sizeof(major_version);                                                                                                         //
        copy_memory(global_header, &minor_version, sizeof(uint16_t), file_offset, 0);                                                                 //
        file_offset += sizeof(minor_version);                                                                                                         //
        copy_memory(global_header, &patch_version, sizeof(uint16_t), file_offset, 0);                                                                 //
        file_offset += sizeof(patch_version);                                                                                                         //
        //SECTION                                                                                                                                     //
        copy_memory(global_header, &scene_header->section_count, sizeof(uint32_t), file_offset, 0);                                                                 //
        file_offset += sizeof(scene_header->section_count);
        //SCENE NAME
        copy_memory(global_header, &scene_header->scene_name_size, sizeof(uint32_t), file_offset, 0);
        file_offset += sizeof(scene_header->scene_name_size);
        copy_memory(global_header, scene_header->scene_name, scene_header->scene_name_size, file_offset, 0);
        file_offset += scene_header->scene_name_size;
        //AUTHOR NAME
        copy_memory(global_header, &scene_header->author_name_size, sizeof(uint32_t), file_offset, 0);
        file_offset += sizeof(scene_header->author_name_size);
        copy_memory(global_header, scene_header->author_name, scene_header->author_name_size, file_offset, 0);
        file_offset += scene_header->author_name_size;
        //Timestamps
        copy_memory(global_header, &scene_header->creation_date_timestamp, sizeof(uint64_t), file_offset, 0);
        file_offset += sizeof(scene_header->creation_date_timestamp);
        copy_memory(global_header, &scene_header->last_modified_date_timestamp, sizeof(uint64_t), file_offset, 0);
        file_offset += sizeof(&scene_header->last_modified_date_timestamp);
        //DESCRIPTION
        copy_memory(global_header, &scene_header->description_size, sizeof(uint32_t), file_offset, 0);
        file_offset += sizeof(scene_header->description_size);
        copy_memory(global_header, scene_header->description, scene_header->description_size, file_offset, 0);
        file_offset += scene_header->description_size;
        //EXTENSIONS                                                                                                                                  //
        copy_memory(global_header, &scene_header->header_extension_count, sizeof(uint16_t), file_offset, 0);                                                               //
        file_offset += sizeof(scene_header->header_extension_count);                                                                                                       //
        copy_memory(global_header, &scene_header->header_extensions, scene_header->header_extension_size, file_offset, 0);                                                                //
        file_offset += scene_header->header_extension_size;
        //CHECKSUM                                                                                                                                    //
        uint32_t global_header_checksum = calculate_crc32(global_header, file_offset);
        copy_memory(global_header, &global_header_checksum, sizeof(uint32_t), file_offset, 0);                                          //
        file_offset += sizeof(global_header_checksum);                                                                                                //
        //WRITE TO FILE                                                                                                                               //
        fwrite(global_header, file_offset, 1, fd);
        //FREE GLOBAL HEADER BUFFER
        free_memory(global_header, MEMORY_TAG_RENDERER);
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





        //////////////////////////////////////////////////////////////////////////////////////////
        ////GLOBAL HEADER                                                                       //
        //fwrite(SGE_REND_MAGIC_NUMBER, 1, 8, fd);                                              //
        ////VERSION                                                                             //
        //fwrite(&major_version, sizeof(uint16_t), 1, fd);                                      //
        //fwrite(&minor_version, sizeof(uint16_t), 1, fd);                                      //
        //fwrite(&patch_version, sizeof(uint16_t), 1, fd);                                      //
        ////SECTION                                                                             //
        //fwrite(&section_count, sizeof(uint16_t), 1, fd);                                      //
        //fwrite(&extension_count, sizeof(uint16_t), 1, fd);                                    //
        //fwrite(&extension_data, 1 * extension_size, 1, fd);                                   //
        //fwrite(&global_header_checksum, sizeof(uint32_t), 1, fd);                             //
        //////////////////////////////////////////////////////////////////////////////////////////
        //file_offset = file_offset + SGE_REND_HEADER_FIXED_SIZE + extension_size;
        //////////////////////////////////////////////////////////////////////////////////
        //SECTIONS
        for (int i = 0; i < scene_header->section_count; ++i) {
                sge_scene_section section = sections[i];
                int section_buffer_offset = 0;
                int section_header_size = SGE_SCENE_SECTION_HEADER_FIXED_SIZE + section.section_header->section_extension_size;
                void *section_header_buffer = allocate_memory(section_header_size, MEMORY_TAG_RENDERER);
                //TYPE
                copy_memory(section_header_buffer, &section.section_header->sge_scene_section_type, sizeof(uint16_t), section_buffer_offset, 0);
                section_buffer_offset += sizeof(section.section_header->sge_scene_section_type);
                //printf("%i\n", section_buffer_offset);
                //SECTION OFFSET
                uint64_t section_offset = file_offset + section_header_size;
                copy_memory(section_header_buffer, &section_offset, sizeof(uint64_t), section_buffer_offset, 0);
                section_buffer_offset += sizeof(section_offset);
                //printf("%i\n", section_buffer_offset);
                //DATA SIZE
                copy_memory(section_header_buffer, &section.section_header->data_size, sizeof(uint64_t), section_buffer_offset, 0);
                section_buffer_offset += sizeof(section.section_header->data_size);
                //NAME
                copy_memory(section_header_buffer, &section.section_header->section_name_size, sizeof(uint32_t), section_buffer_offset, 0);
                section_buffer_offset += sizeof(section.section_header->section_name_size);
                copy_memory(section_header_buffer, section.section_header->section_name, section.section_header->section_name_size, section_buffer_offset, 0);
                section_buffer_offset += section.section_header->section_name_size;
                //TIMESTAMPS
                copy_memory(section_header_buffer, &section.section_header->creation_date_timestamp, sizeof(uint64_t), file_offset, 0);
                file_offset += sizeof(section.section_header->creation_date_timestamp);
                copy_memory(section_header_buffer, &section.section_header->last_modified_date_timestamp, sizeof(uint64_t), file_offset, 0);
                file_offset += sizeof(section.section_header->last_modified_date_timestamp);
                //EXTENSIONS
                copy_memory(section_header_buffer, &section.section_header->section_extension_count, sizeof(uint16_t), section_buffer_offset, 0);
                section_buffer_offset += sizeof(section.section_header->section_extension_count);
                //printf("%i\n", section_buffer_offset);
                copy_memory(section_header_buffer, &section.section_header->extensions, section.section_header->section_extension_size, section_buffer_offset, 0);
                section_buffer_offset += section.section_header->section_extension_size;
                // CHECKSUM
                uint32_t section_header_checksum = calculate_crc32(section_header_buffer, section_buffer_offset);
                section.section_header->crc32_checksum = section_header_checksum;
                copy_memory(section_header_buffer, &section_header_checksum, sizeof(uint32_t), section_buffer_offset, 0);
                section_buffer_offset += sizeof(section_header_checksum);
                //printf("%i\n", section_buffer_offset);
                //SIZE CHECK
                if (section_buffer_offset != SGE_SCENE_SECTION_HEADER_FIXED_SIZE + section.section_header->section_extension_size) {
                        log_internal_event(LOG_LEVEL_FATAL, "wrong section header - sgerend");
                        return SGE_ERROR;
                }
                //WRITE FILE
                fwrite(section_header_buffer, section_buffer_offset, 1, fd);
                //FREE
                free_memory(section_header_buffer, MEMORY_TAG_RENDERER);

                //DATA
                fwrite(section.data, section.section_header->data_size, 1, fd);

                //SIZE OFFSET
                file_offset += section_buffer_offset + section.section_header->data_size;

                //fwrite(&section.section_header.type, sizeof(uint16_t), 1, fd);
                //file_offset += SGE_REND_SECTION_HEADER_FIXED_SIZE + section->extension_size;
                //fwrite(&file_offset, sizeof(uint64_t), 1, fd);
                //size_t section_size
                //fwrite(&section_size, sizeof(uint64_t), 1, fd);
                //fwrite(&section->extension_count, sizeof(uint32_t), 1, fd);
                //fwrite(&section->extension_data, section->extension_size, 1, fd);
                //fwrite(&section->section_checksum, sizeof(uint32_t), 1, fd);
                //fwrite(&section->section_data, 1*section->section_data_size, 1, fd);
        }
        fclose(fd);

        return SGE_SUCCESS;
}
