//
// Created by Geisthardt on 26.02.2025.
//

#ifndef STEINFILE_H
#define STEINFILE_H
#include <stdint.h>
#include "../SGE.h"

void make_filename_save(char *filename);
SGE_BOOL create_directory_if_not_exists(char *dir_path);
char *get_current_working_directory();
SGE_BOOL sge_file_exists(char *filepath);
uint32_t *read_file_as_binary(const char *filepath, size_t *codeSize);

#endif //STEINFILE_H
