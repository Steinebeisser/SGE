//
// Created by Geisthardt on 26.02.2025.
//

#ifndef SGE_FILE_H
#define SGE_FILE_H
#include <stdint.h>
#include "sge_types.h"

void make_filename_save(char *filename);
SGE_BOOL create_directory_if_not_exists(char *dir_path);
char *get_current_working_directory();
SGE_BOOL sge_file_exists(char *filepath);
uint32_t *read_file_as_binary(const char *filepath, size_t *codeSize);

#endif //SGE_FILE_H
