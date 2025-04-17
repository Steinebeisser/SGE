//
// Created by Geisthardt on 27.02.2025.
//

#include "utils/sge_string.h"
#include "sge_types.h"
#include <string.h>

int get_longest_element_in_array(char *array[]) {
        int longest_element_length = 0;
        int i = 0;
        while (array[i] != NULL) {
                const char *element = array[i];
                const int length = strlen(element);
                if (length > longest_element_length) {
                        longest_element_length = length;
                }
                i++;
        }
        return longest_element_length;
}

SGE_RESULT get_last_string_index(char *string, char element, size_t *index_var) {
        SGE_BOOL found = SGE_FALSE;
        for (size_t i = 0; i < strlen(string); ++i) {

                if (element == string[i]) {
                        *index_var = i;
                        found = SGE_TRUE;
                }
        }

        if (!found) {
                return SGE_ERROR;
        }
        return SGE_SUCCESS;
}

SGE_RESULT get_file_ending(char *filename, char *file_ending) {
        size_t last_dot_index = 0;
        if (get_last_string_index(filename, '.', &last_dot_index) != SGE_SUCCESS) {
                file_ending = NULL;
                return SGE_ERROR;
        }

        strncpy(file_ending, filename + last_dot_index + 1, strlen(filename) - last_dot_index - 1);
        file_ending[last_dot_index] = '\0';

        return SGE_SUCCESS;
}

SGE_BOOL string_includes(char *string, char *check_string) {
        if (strstr(string, check_string) != NULL) {
                return SGE_TRUE;
        }
        return SGE_FALSE;
}
