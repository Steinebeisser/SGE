//
// Created by Geisthardt on 27.02.2025.
//

#ifndef STEINSTRING_H
#define STEINSTRING_H

#include "../SGE.h"

int get_longest_element_in_array(char *array[]);
SGE_RESULT get_last_string_index(char *string, char element, size_t *index_var);
SGE_RESULT get_file_ending(char *filename, char *file_ending);
SGE_BOOL string_includes(char *string, char *check_string);
#endif //STEINSTRING_H
